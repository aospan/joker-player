#include "jokeraccessprovider.h"
#include "jokeraccessprovider_p.h"
#include "jokerplayer.h"

#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>

#include <vlc/vlc.h>

#include <QLoggingCategory>
#include <QTimer>
#include <QVector>
#include <QEvent>

#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(jkAccessProvider)

const int kChannelLockedInterval = 1500;
const int kProgramsDiscoveryInterval = 2000;
const int kStartTuneInterval = 10;

const QEvent::Type kProgramUpdateEventType = static_cast<QEvent::Type>(QEvent::User + 1);
const QEvent::Type kChannelStatusEventType = static_cast<QEvent::Type>(QEvent::User + 2);
const QEvent::Type kCamInfoEventType = static_cast<QEvent::Type>(QEvent::User + 3);
const QEvent::Type kCaidsEventType = static_cast<QEvent::Type>(QEvent::User + 4);
const QEvent::Type kMmiEventType = static_cast<QEvent::Type>(QEvent::User + 5);

// ProgramUpdateEvent

class ProgramUpdateEvent : public QEvent
{
public:
    explicit ProgramUpdateEvent(int number, const QString &description)
        : QEvent(kProgramUpdateEventType)
        , m_number(number)
        , m_description(description)
    {
    }

    int m_number = -1;
    QString m_description;
};

// ChannelStatusEvent

class ChannelStatusEvent : public QEvent
{
public:
    explicit ChannelStatusEvent(bool isLocked,
                                double snr,
                                double rfLevel,
                                int errorsCount,
                                JokerAccessProvider::SignalQuality quality)
        : QEvent(kChannelStatusEventType)
        , m_isLocked(isLocked)
        , m_snr(snr)
        , m_rfLevel(rfLevel)
        , m_errorsCount(errorsCount)
        , m_quality(quality)
    {
    }

    bool m_isLocked = false;
    double m_snr = 0.0;
    double m_rfLevel = 0.0;
    int m_errorsCount = 0;
    JokerAccessProvider::SignalQuality m_quality = JokerAccessProvider::SignalQuality::BadSignalQuality;
};

// CamInfoEvent

class CamInfoEvent : public QEvent
{
public:
    explicit CamInfoEvent(int applicationType,
                          int applicationManufacturer,
                          int manufacturerCode,
                          QString menuString,
                          QString infoString)
        : QEvent(kCamInfoEventType)
        , m_applicationType(applicationType)
        , m_applicationManufacturer(applicationManufacturer)
        , m_manufacturerCode(manufacturerCode)
        , m_menuString(menuString)
        , m_infoString(infoString)
    {
    }

    uint8_t m_applicationType = 0;
    uint16_t m_applicationManufacturer = 0;
    uint16_t m_manufacturerCode = 0;
    QString m_menuString;
    QString m_infoString;
};

// CaidsEvent

class CaidsEvent : public QEvent
{
public:
    explicit CaidsEvent(const QVector<int> &caids)
        : QEvent(kCaidsEventType)
        , m_caids(caids)
    {
    }

    QVector<int> m_caids;
};

// MmiEvent

class MmiEvent : public QEvent
{
public:
    explicit MmiEvent(const QString &menu)
        : QEvent(kMmiEventType)
        , m_menu(menu)
    {
    }

    QString m_menu;
};

// JokerMessageHandler

class JokerMessageHandler : public QObject
{
    // No Q_OBJECT macro here!
public:
    explicit JokerMessageHandler(JokerAccessProviderPrivate *d, QObject *parent)
        : QObject(parent)
        , dptr(d)
    {
    }

    void customEvent(QEvent *event)
    {
        const auto eventType = event->type();
        if (eventType == kProgramUpdateEventType) {
            if (dptr->canProcessProgramsUpdateEvents) {
                dptr->processProgramUpdateEvent(event);
            } else {
                qCDebug(jkAccessProvider) << "Ignore program update event";
            }
        } else if (eventType == kChannelStatusEventType) {
            if (dptr->canProcessChannelStatusEvents) {
                dptr->processChannelStatusEvent(event);
            } else {
                qCDebug(jkAccessProvider) << "Ignore channel status event";
            }
        } else if (eventType == kCamInfoEventType) {
            dptr->processCamInfoEvent(event);
        } else if (eventType == kCaidsEventType) {
            dptr->processCaidsEvent(event);
        } else if (eventType == kMmiEventType) {
            dptr->processMmiEvent(event);
        }
    }

private:
    JokerAccessProviderPrivate *dptr = nullptr; // not owned
};

Q_GLOBAL_STATIC(QVector<JokerAccessProviderPrivate *>, qProviders)

static JokerAccessProvider::SignalQuality decodeSignalQuality(int qualityCode)
{
    switch (qualityCode) {
    case SIGNAL_BAD:
        return JokerAccessProvider::SignalQuality::BadSignalQuality;
    case SIGNAL_WEAK:
        return JokerAccessProvider::SignalQuality::WeakSignalQuality;
    case SIGNAL_GOOD:
        return JokerAccessProvider::SignalQuality::GoodSignalQuality;
    default:
        return JokerAccessProvider::SignalQuality::BadSignalQuality;
    }
}

static void jokerServiceNameCallback(program_t* program) {

    if (qProviders()->isEmpty())
        return;
    const JokerAccessProviderPrivate *priv = qProviders()->first();
    if (!priv)
        return;

    const auto number = program->number;
    const QString description = QString::fromUtf8(reinterpret_cast<const char*>(program->name));

    qCDebug(jkAccessProvider) << "Program update callback called, program id:"
                              << number << ", name:" << description;

    auto event = new ProgramUpdateEvent(number, description);
    qApp->postEvent(priv->messageHandler, event);
}

static void jokerStatusCallback(void *data)
{
    if (!data)
        return;

    if (qProviders()->isEmpty())
        return;
    const JokerAccessProviderPrivate *priv = qProviders()->first();
    if (!priv)
        return;

    auto jokePtr = reinterpret_cast<joker_t *>(data);
    auto statPtr = &jokePtr->stat;

    const auto locked = (statPtr->status == JOKER_LOCK);
    const auto snr = statPtr->snr / 1000.0;
    const auto rfLevel = statPtr->rf_level / 1000.0;
    const auto errorsCount = statPtr->ucblocks;
    const auto quality = decodeSignalQuality(statPtr->signal_quality);

    qCDebug(jkAccessProvider) << "Channel status callback called, locked:"
                              << locked;

    auto event = new ChannelStatusEvent(locked, snr, rfLevel, errorsCount, quality);
    qApp->postEvent(priv->messageHandler, event);
}

// CAM module info callback
// will be called when CAM module info available
static void jokerCamInfoCallback(void *data)
{
    if (!data)
        return;

    if (qProviders()->isEmpty())
        return;

    const JokerAccessProviderPrivate *priv = qProviders()->first();
    if (!priv)
        return;

    const auto jokerPtr = reinterpret_cast<const joker_t *>(data);
    if (!jokerPtr || !jokerPtr->joker_ci_opaque)
        return;

    const auto jokerCiPtr = reinterpret_cast<const joker_ci_t *>(jokerPtr->joker_ci_opaque);

    const auto applicationType = jokerCiPtr->application_type;
    const auto applicationManufacturer = jokerCiPtr->application_manufacturer;
    const auto manufacturerCode = jokerCiPtr->manufacturer_code;
    const auto menuString = QString::fromUtf8(reinterpret_cast<const char *>(jokerCiPtr->menu_string));
    const auto infoString = QString::fromUtf8(reinterpret_cast<const char *>(jokerCiPtr->cam_infostring));

    auto event = new CamInfoEvent(applicationType, applicationManufacturer, manufacturerCode, menuString, infoString);
    qApp->postEvent(priv->messageHandler, event);
}

// CAM module supported CAIDs callback
// will be called when CAM module supported CAIDs available
void jokerCaidsCallback(void *data)
{
    if (!data)
        return;

    if (qProviders()->isEmpty())
        return;

    const JokerAccessProviderPrivate *priv = qProviders()->first();
    if (!priv)
        return;

    const auto jokerPtr = reinterpret_cast<joker_t *>(data);
    if (!jokerPtr || !jokerPtr->joker_ci_opaque)
        return;

    const auto jokerCiPtr = reinterpret_cast<const joker_ci_t *>(jokerPtr->joker_ci_opaque);

    QVector<int> caids;
    for (int i = 0; i < CAID_MAX_COUNT; ++i) {
        const auto caid = jokerCiPtr->ca_ids[i];
        if (!caid)
            break;
        caids.append(caid);
    }

    auto event = new CaidsEvent(caids);
    qApp->postEvent(priv->messageHandler, event);
}

static void jokerMmiCallback(void *data, unsigned char *buf, int len)
{
    if (qProviders()->isEmpty())
        return;

    const JokerAccessProviderPrivate *priv = qProviders()->first();
    if (!priv)
        return;

    const auto jokerPtr = reinterpret_cast<joker_t *>(data);
    if (!jokerPtr || !jokerPtr->joker_en50221_opaque)
        return;

    const auto menu = QString::fromUtf8(reinterpret_cast<const char *>(buf), len);

    auto event = new MmiEvent(menu);
    qApp->postEvent(priv->messageHandler, event);
}

JokerAccessProviderPrivate::JokerAccessProviderPrivate(JokerAccessProvider *q)
    : q_ptr(q)
{
    qProviders()->append(this);
}

JokerAccessProviderPrivate::~JokerAccessProviderPrivate()
{
    qProviders()->removeAll(this);
}

void JokerAccessProviderPrivate::open()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpeningStatus);

    if (!messageHandler) {
        messageHandler = new JokerMessageHandler(this, q);
        qApp->installEventFilter(messageHandler);
    }

    if (!discoveryTimer) {
        discoveryTimer = new QTimer(q);
        lockTimer = new QTimer(q);
        discoveryTimer->setSingleShot(true);
        q->connect(discoveryTimer, &QTimer::timeout, [this]() {
            processDiscoveryInterval(); });
    }

    ::memset(&joker, 0, sizeof(joker));
    joker.status_callback = jokerStatusCallback;
    joker.ci_info_callback = jokerCamInfoCallback;
    joker.ci_caid_callback = jokerCaidsCallback;
    joker.libusb_verbose = 0;

    {
        const auto result = ::joker_open(&joker);
        ::memset(&pool, 0, sizeof(pool));
        if (result != 0) {
            q->setError(JokerAccessProvider::ProviderError::OpenError,
                        JokerAccessProvider::tr("Unable to open device"));
            q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosedStatus);
            return;
        }
    }

    {
        const auto result = ::joker_ci(&joker);
        if (result != 0) {
            q->setError(JokerAccessProvider::ProviderError::OpenError,
                        JokerAccessProvider::tr("Unable to initialize the CAM module"));
            q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosedStatus);
            return;
        }
    }

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);
}

void JokerAccessProviderPrivate::close()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosingStatus);

    if (messageHandler) {
        qApp->removeEventFilter(messageHandler);
        delete messageHandler;
        messageHandler = nullptr;
    }

    if (discoveryTimer) {
        discoveryTimer->stop();
        delete discoveryTimer;
        discoveryTimer = nullptr;
    }

    // Try to close
    ::stop_ts(&joker, &pool);
    ::joker_close(&joker);

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosedStatus);
}

void JokerAccessProviderPrivate::startScan()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::ProgramsDiscoveringStatus);
    q->setSignalLocked(false);
    q->setScanningProgress(0.0);

    discoveredPrograms.clear();
    discoveryJobIndex = -1;

    // Invalidate active channel index
    activeChannelIndex = -1;

    startNextChannelDiscovery();
}

void JokerAccessProviderPrivate::stopScan()
{
    if (discoveryTimer)
        discoveryTimer->stop();

    ::stop_ts(&joker, &pool);
    canProcessProgramsUpdateEvents = false;

    completeTune();

    discoveryJobIndexes.clear();

    Q_Q(JokerAccessProvider);
    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);
    qCDebug(jkAccessProvider) << "Discovery stopped";
}

void JokerAccessProviderPrivate::startMmiSession()
{
    const auto result = joker_en50221_mmi_enter(&joker, &jokerMmiCallback);
    if (result != 0) {
        qCWarning(jkAccessProvider) << "Unable to startup the MMI session";

        Q_Q(JokerAccessProvider);
        q->setError(JokerAccessProvider::ProviderError::OperationError,
                    JokerAccessProvider::tr("Unable to startup the MMI session"));
    };
}

void JokerAccessProviderPrivate::sendMmiCommand(const QString &command)
{
    const QByteArray buffer = command.toUtf8();
    const auto result = joker_en50221_mmi_call(&joker, reinterpret_cast<const unsigned char *>(buffer.constData()),
                                               buffer.size());
    if (result != 0) {
        qCWarning(jkAccessProvider) << "Unable to send the MMI command";

        Q_Q(JokerAccessProvider);
        q->setError(JokerAccessProvider::ProviderError::OperationError,
                    JokerAccessProvider::tr("Unable to send the MMI command"));
    };
}

void JokerAccessProviderPrivate::processDiscoveryInterval()
{
    Q_Q(JokerAccessProvider);

    const auto discoveryInterval = discoveryTimer->interval();

    if (discoveryInterval == kStartTuneInterval) {
        startTune();
    } else if (discoveryInterval == kChannelLockedInterval) {
        // This means that timed out occurred when waiting for channel 'locked'
        completeTune();

        // Detecting current programs discovery or channel activating status
        const auto isChannelActivatingStatus =
                (status == JokerAccessProvider::ProviderStatus::ChannelActivating);

        const auto channelIndex = (isChannelActivatingStatus)
                ? activeChannelIndex : discoveringChannelIndex();

        if (isChannelActivatingStatus) {
            qCWarning(jkAccessProvider) << "Activation timeout for channel" << channelIndex;

            // Invalidate active channel index
            activeChannelIndex = -1;

            Q_Q(JokerAccessProvider);
            q->setError(JokerAccessProvider::ProviderError::TimeoutError,
                        JokerAccessProvider::tr("Channel activation timeout"));
            q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);
        } else {
            qCWarning(jkAccessProvider) << "Discovery timeout for channel" << channelIndex;
            startNextChannelDiscovery();
        }
    } else if (discoveryInterval == kProgramsDiscoveryInterval) {
        completeProgramsDiscovery();
    }
}

void JokerAccessProviderPrivate::startProgramsDiscovery()
{
    ::memset(&pool, 0, sizeof pool);
    const auto result = ::start_ts(&joker, &pool);
    if (result != 0) {
        const auto channelIndex = discoveringChannelIndex();
        qCWarning(jkAccessProvider) << "Unable to startup stream to get programs for channel" << channelIndex;
        startNextChannelDiscovery();
    } else {
        Q_Q(JokerAccessProvider);

        canProcessProgramsUpdateEvents = true;

        // Register program update callback

        pool.service_name_callback = jokerServiceNameCallback;

        const auto channelIndex = discoveringChannelIndex();
        program_t *program = nullptr;
        program_t *tmp = nullptr;
        list_head *programs = ::get_programs(&pool);
        list_for_each_entry_safe(program, tmp, programs, list) {
            const JokerProgram jokerProgram(channelIndex, program->number);
            q->processProgram(jokerProgram);
        }

        discoveryTimer->start(kProgramsDiscoveryInterval);
    }
}

void JokerAccessProviderPrivate::completeProgramsDiscovery()
{
    canProcessProgramsUpdateEvents = false;
    ::stop_ts(&joker, &pool);

    startNextChannelDiscovery();
}

void JokerAccessProviderPrivate::startTune()
{
    // Detecting current programs discovery or channel activating status
    const auto isChannelActivatingStatus =
            (status == JokerAccessProvider::ProviderStatus::ChannelActivating);

    const auto channelIndex = (isChannelActivatingStatus)
            ? activeChannelIndex : discoveringChannelIndex();

    qCDebug(jkAccessProvider) << "Start tune for channel index:" << channelIndex;

    ::stop_ts(&joker, &pool);

    const auto channel = settingsStorage->m_channels.at(channelIndex);

    ::memset(&info, 0, sizeof(info));
    info.delivery_system = static_cast<joker_fe_delivery_system>(channel.m_standard);
    info.bandwidth_hz = channel.m_bandwidth;
    info.frequency = quint64(channel.m_frequencyMhz) * 1000000;
    info.symbol_rate = channel.m_symbolRate;
    info.modulation = static_cast<joker_fe_modulation>(channel.m_modulation);
    // TODO: Where a voltage should be taken from?
    info.voltage = static_cast<joker_fe_sec_voltage>(channel.m_voltage);
    info.tone = static_cast<joker_fe_sec_tone_mode>(settingsStorage->m_tone);

    // Setup LNB settings
    const auto preset = settingsStorage->preset();
    info.lnb.lowfreq = preset.m_lowFrequency;
    info.lnb.highfreq = preset.m_highFrequency;
    info.lnb.switchfreq = preset.m_switchFrequency;

    const auto result = ::tune(&joker, &info);
    if (result != 0) {
        if (isChannelActivatingStatus) {
            qCWarning(jkAccessProvider) << "Unable to startup activation tune for channel index:" << channelIndex;
            Q_Q(JokerAccessProvider);
            q->setError(JokerAccessProvider::ProviderError::OperationError,
                        JokerAccessProvider::tr("Channel activation failed"));
            q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);
        } else {
            qCWarning(jkAccessProvider) << "Unable to startup discovery tune for channel index:" << channelIndex;
            startNextChannelDiscovery();
        }
    } else {
        canProcessChannelStatusEvents = true;
        qCDebug(jkAccessProvider) << "Start locked status waiting for channel index:" << channelIndex;
        discoveryTimer->start(kChannelLockedInterval);
    }
}

void JokerAccessProviderPrivate::completeTune()
{
    qCDebug(jkAccessProvider) << "Complete tune";
    canProcessChannelStatusEvents = false;
    // Disable status event triggering
    joker.stat.refresh_enable = 0;
}

void JokerAccessProviderPrivate::startNextChannelDiscovery()
{
    if ((discoveryJobIndex + 1) < discoveryJobIndexes.count()) {
        Q_Q(JokerAccessProvider);

        q->setDiscoveryJobIndex(discoveryJobIndex + 1);

        const auto progress = double(discoveryJobIndex) / discoveryJobIndexes.count();
        q->setScanningProgress(progress);

        const auto channelIndex = discoveringChannelIndex();
        qCDebug(jkAccessProvider) << "Start discovery for next channel index:" << channelIndex;

        q->updateCurrentScanningChannelInfo();

        discoveryTimer->start(kStartTuneInterval);
    } else {
        discoveryTimer->stop();
        Q_Q(JokerAccessProvider);
        q->setStatus(JokerAccessProvider::ProviderStatus::ProgramsDiscoveredStatus);
        qCDebug(jkAccessProvider) << "Discovery finished";
        activeChannelIndex = -1;
    }
}

void JokerAccessProviderPrivate::processProgramUpdateEvent(QEvent *event)
{
    const auto updateEvent = static_cast<ProgramUpdateEvent *>(event);
    const auto channelIndex = discoveringChannelIndex();
    JokerProgram program(channelIndex, updateEvent->m_number);
    program.m_description = updateEvent->m_description;

    Q_Q(JokerAccessProvider);
    q->processProgram(program);
}

void JokerAccessProviderPrivate::processCamInfoEvent(QEvent *event)
{
    const auto camInfoEvent = static_cast<CamInfoEvent *>(event);

    Q_Q(JokerAccessProvider);

    const auto hexAplicationType = QString(QLatin1String("0x%1"))
            .arg(camInfoEvent->m_applicationType, 2, 16, QLatin1Char('0'));

    const auto hexApplicationManufacturer = QString(QLatin1String("0x%1"))
            .arg(camInfoEvent->m_applicationManufacturer, 4, 16, QLatin1Char('0'));

    const auto hexManufacturerCode = QString(QLatin1String("0x%1"))
            .arg(camInfoEvent->m_manufacturerCode, 4, 16, QLatin1Char('0'));

    const JokerCamInfo camInfo(hexAplicationType,
                               hexApplicationManufacturer,
                               hexManufacturerCode,
                               camInfoEvent->m_menuString,
                               camInfoEvent->m_infoString);
    q->setCamInfo(camInfo);
    q->setCamDetected(true);
}

void JokerAccessProviderPrivate::processCaidsEvent(QEvent *event)
{
    const auto caidsEvent = static_cast<CaidsEvent *>(event);

    Q_Q(JokerAccessProvider);

    // TODO: It is better to use the std algorithms here!
    const auto caids = caidsEvent->m_caids;
    QStringList hexCaids;
    for (const auto caid : qAsConst(caids)) {
        const auto hexCaid = QString(QLatin1String("0x%1")).arg(caid, 4, 16, QLatin1Char('0'));
        hexCaids << hexCaid;
    }

    q->setCaids(hexCaids);
}

void JokerAccessProviderPrivate::processMmiEvent(QEvent *event)
{
    const auto mmiEvent = static_cast<MmiEvent *>(event);

    Q_Q(JokerAccessProvider);

    const auto menu = mmiEvent->m_menu;
    q->setMmiCamMenu(menu);
}

void JokerAccessProviderPrivate::processChannelStatusEvent(QEvent *event)
{
    const auto statusEvent = static_cast<ChannelStatusEvent *>(event);

    Q_Q(JokerAccessProvider);
    q->setSignalLocked(statusEvent->m_isLocked);
    q->setSignalCarrierNoiseRatio(statusEvent->m_snr);
    q->setSignalRadioFrequencyLevel(statusEvent->m_rfLevel);
    q->setSignalErrorsCount(statusEvent->m_errorsCount);
    q->setSignalQuality(statusEvent->m_quality);

    if (statusEvent->m_isLocked) {
        completeTune();

        // Detecting current programs discovery or channel activating status
        const auto isChannelActivatingStatus =
                (status == JokerAccessProvider::ProviderStatus::ChannelActivating);

        if (isChannelActivatingStatus) {
            discoveryTimer->stop();
            ::memset(&pool, 0 , sizeof(pool));
            ::start_ts(&joker, &pool);
            q->setStatus(JokerAccessProvider::ProviderStatus::ChannelActivated);
        } else {
            startProgramsDiscovery();
        }
    }
}

static int mediaOpenCallback(void *opaque, void **datap, uint64_t *sizep)
{
    *sizep = UINT64_MAX;
    *datap = opaque;
    return 0;
}

static ssize_t mediaReadCallback(void *opaque, unsigned char *buf, size_t len)
{
    auto priv = reinterpret_cast<JokerAccessProviderPrivate *>(opaque);
    if (qProviders->contains(priv)) {

        auto bytes= ::read_ts_data(&priv->pool, buf, len);
        return bytes;
    }
    return 0;
}

static int mediaSeekCallback(void *opaque, uint64_t offset)
{
    Q_UNUSED(opaque);
    Q_UNUSED(offset);
    return 0;
}

static void mediaCloseCallback(void *opaque)
{
    Q_UNUSED(opaque);
}

void JokerAccessProviderPrivate::registerMediaStream(JokerPlayer *player)
{
    Q_Q(JokerAccessProvider);

    if (!player || !player->m_instance || !player->m_instance->core()) {
        q->setError(JokerAccessProvider::ProviderError::OperationError,
                    JokerAccessProvider::tr("Unable to register media stream due "
                                            "to no player instance"));
        return;
    }

    auto media = ::libvlc_media_new_callbacks(
                player->m_instance->core(),
                mediaOpenCallback,
                mediaReadCallback,
                mediaSeekCallback,
                mediaCloseCallback,
                this
                );

    if (!media) {
        q->setError(JokerAccessProvider::ProviderError::OperationError,
                    JokerAccessProvider::tr("Unable to register media stream due "
                                            "to media core creation fails"));
        return;
    }

    player->m_media = new VlcMedia(media);
    player->m_player->openOnly(player->m_media);
}

void JokerAccessProviderPrivate::unregisterMediaStream(JokerPlayer *player)
{
    // TODO: Unregister stream from driver
}

void JokerAccessProviderPrivate::setupActiveChannel()
{
    Q_Q(JokerAccessProvider);

    discoveryTimer->start(kStartTuneInterval);
}
