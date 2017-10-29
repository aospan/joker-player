#include "jokeraccessprovider.h"
#include "jokeraccessprovider_p.h"
#include "jokerplayer.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkAccessProvider)

int JokerAccessProviderPrivate::discoveringChannelIndex() const
{
    return discoveryJobIndexes.at(discoveryJobIndex);
}

JokerAccessProvider::JokerAccessProvider(QObject *parent)
    : QObject(parent)
    , d_ptr(new JokerAccessProviderPrivate(this))
{
}

JokerAccessProvider::~JokerAccessProvider()
{
    delete d_ptr;
}

void JokerAccessProvider::open()
{
    Q_D(JokerAccessProvider);

    if (d->status != ProviderStatus::DeviceClosedStatus) {
        setError(ProviderError::OperationError, tr("Device is not closed"));
        return;
    }

    d->open();
}

void JokerAccessProvider::close()
{
    Q_D(JokerAccessProvider);

    if (d->status != ProviderStatus::DeviceOpenedStatus
            && d->status != ProviderStatus::DeviceOpeningStatus
            && d->status != ProviderStatus::ProgramsDiscoveredStatus
            && d->status != ProviderStatus::ProgramsDiscoveringStatus) {
        setError(ProviderError::OperationError, tr("Provider is not open"));
        return;
    }

    d->close();
}

void JokerAccessProvider::startScan()
{
    Q_D(JokerAccessProvider);

    switch (d->status) {
    case ProviderStatus::DeviceClosedStatus:
        setError(ProviderError::NotOpenError, tr("Device is not open"));
        return;
    case ProviderStatus::DeviceClosingStatus:
        setError(ProviderError::NotOpenError, tr("Device is closing"));
        return;
    case ProviderStatus::DeviceOpeningStatus:
        setError(ProviderError::NotOpenError, tr("Device is opening"));
        return;
    case ProviderStatus::ProgramsDiscoveringStatus:
        setError(ProviderError::OperationError, tr("Device already is in discovering"));
        return;
    default:
        break;
    }

    if (!d->settingsStorage) {
        setError(ProviderError::OperationError, tr("Configuration for channels scanning is not set"));
        return;
    }

    // Choosing all selected channel indexes
    d->discoveryJobIndexes.clear();
    const auto channelsCount = d->settingsStorage->m_channels.count();
    for (auto channelIndex = 0; channelIndex < channelsCount; ++channelIndex) {
        const auto channel = d->settingsStorage->m_channels.at(channelIndex);
        if (channel.m_selected)
            d->discoveryJobIndexes.append(channelIndex);
    }

    setDiscoveryJobsCount(d->discoveryJobIndexes.count());

    if (d->discoveryJobIndexes.isEmpty()) {
        setError(ProviderError::OperationError, tr("Frequencies or channels are not selected for scanning"));
        return;
    }

    qCDebug(jkAccessProvider) << "Start discovery";

    d->discoveredPrograms.clear();
    d->startScan();
}

void JokerAccessProvider::stopScan()
{
    Q_D(JokerAccessProvider);

    switch (d->status) {
    case ProviderStatus::DeviceClosedStatus:
        setError(ProviderError::NotOpenError, tr("Device is not open"));
        return;
    case ProviderStatus::DeviceClosingStatus:
        setError(ProviderError::NotOpenError, tr("Device is closing"));
        return;
    case ProviderStatus::DeviceOpeningStatus:
        setError(ProviderError::NotOpenError, tr("Device is opening"));
        return;
    case ProviderStatus::DeviceOpenedStatus:
        setError(ProviderError::OperationError, tr("Device is not in discovering"));
        return;
    default:
        break;
    }

    qCDebug(jkAccessProvider) << "Stop discovery";

    d->stopScan();
    d->discoveryJobIndexes.clear();

    setDiscoveryJobIndex(0);
    setDiscoveryJobsCount(d->discoveryJobIndexes.count());
}

void JokerAccessProvider::registerMediaStream(JokerPlayer *player)
{
    if (!player->m_instance) {
        setError(ProviderError::OperationError, tr("No player instance detected"));
        return;
    }

    Q_D(JokerAccessProvider);

    d->registerMediaStream(player);
}

void JokerAccessProvider::unregisterMediaStream(JokerPlayer *player)
{
    // TODO: Unregister stream from driver

    Q_D(JokerAccessProvider);

    d->unregisterMediaStream(player);
}

int JokerAccessProvider::activeChannelIndex() const
{
    return d_func()->activeChannelIndex;
}

void JokerAccessProvider::setupActiveChannel(int channelIndex)
{
    Q_D(JokerAccessProvider);

    switch (d->status) {
    case ProviderStatus::DeviceClosedStatus:
        setError(ProviderError::NotOpenError, tr("Device is not open"));
        return;
    case ProviderStatus::DeviceClosingStatus:
        setError(ProviderError::NotOpenError, tr("Device is closing"));
        return;
    case ProviderStatus::DeviceOpeningStatus:
        setError(ProviderError::NotOpenError, tr("Device is opening"));
        return;
    case ProviderStatus::ProgramsDiscoveringStatus:
        setError(ProviderError::OperationError, tr("Device is in programs discovering"));
        return;
    default:
        break;
    }

    setError(JokerAccessProvider::ProviderError::NoError, tr("No error"));
    setStatus(JokerAccessProvider::ProviderStatus::ChannelActivating);
    qCDebug(jkAccessProvider) << "Trying to activate channel index:" << channelIndex;
    d->activeChannelIndex = channelIndex;
    d->setupActiveChannel();
}

void JokerAccessProvider::updateCurrentScanningChannelInfo()
{
    Q_D(JokerAccessProvider);

    const auto channelIndex = d->discoveryJobIndexes.at(d->discoveryJobIndex);
    const auto channel = d->settingsStorage->m_channels.at(channelIndex);
    const auto frequencyMhz = channel.m_frequencyMhz;
    const auto standardName = JokerTelevision::getStandardName(channel.m_standard);
    emit scanningChannelChanged(standardName, frequencyMhz);
}

JokerAccessProvider::ProviderStatus JokerAccessProvider::status() const
{
    return d_func()->status;
}

void JokerAccessProvider::setStatus(ProviderStatus status)
{
    Q_D(JokerAccessProvider);

    if (d->status == status)
        return;
    d->status = status;
    qCDebug(jkAccessProvider) << "Set status:" << d->status;
    emit statusChanged(d->status);
}

JokerAccessProvider::ProviderError JokerAccessProvider::error() const
{
    return d_func()->error;
}

JokerSettingsStorage *JokerAccessProvider::settingsStorage() const
{
    return d_func()->settingsStorage;
}

void JokerAccessProvider::setSettingsStorage(JokerSettingsStorage *settingsStorage)
{
    Q_D(JokerAccessProvider);

    if (d->settingsStorage == settingsStorage)
        return;
    d->settingsStorage = settingsStorage;
    qCDebug(jkAccessProvider) << "Set settings storage:" << d->settingsStorage;
    emit settingsStorageChanged(d->settingsStorage);
}

void JokerAccessProvider::setError(ProviderError error, const QString &errorString)
{
    Q_D(JokerAccessProvider);

    d->error = error;
    d->errorString = errorString;
    qCDebug(jkAccessProvider) << "Set error:" << d->error;
    emit errorOccurred(d->error);
}

QString JokerAccessProvider::errorString() const
{
    return d_func()->errorString;
}

double JokerAccessProvider::scanningProgress() const
{
    return d_func()->scanningProgress;
}

void JokerAccessProvider::setScanningProgress(double progress)
{
    Q_D(JokerAccessProvider);

    if (qFuzzyCompare(d->scanningProgress, progress))
        return;
    d->scanningProgress = progress;
    qCDebug(jkAccessProvider) << "Set scanning progress:" << d->scanningProgress;
    emit scanningProgressChanged(d->scanningProgress);
}

JokerAccessProvider::SignalQuality JokerAccessProvider::signalQuality() const
{
    return d_func()->signalQuality;
}

void JokerAccessProvider::setSignalQuality(SignalQuality quality)
{
    Q_D(JokerAccessProvider);

    if (d->signalQuality == quality)
        return;
    d->signalQuality = quality;
    qCDebug(jkAccessProvider) << "Set signal quality:" << d->signalQuality;
    emit signaQualityChanged(d->signalQuality);
}

double JokerAccessProvider::signalCarrierNoiseRatio() const
{
    return d_func()->signalCarrierNoiseRatio;
}

void JokerAccessProvider::setSignalCarrierNoiseRatio(double ratio)
{
    Q_D(JokerAccessProvider);

    if (qFuzzyCompare(d->signalCarrierNoiseRatio, ratio))
        return;
    d->signalCarrierNoiseRatio = ratio;
    qCDebug(jkAccessProvider) << "Set signal carrier-to-noise ratio:" << d->signalCarrierNoiseRatio;
    emit signalCarrierNoiseRatioChanged(d->signalCarrierNoiseRatio);
}

double JokerAccessProvider::signalRadioFrequencyLevel() const
{
    return d_func()->signalRadioFrequencyLevel;
}

void JokerAccessProvider::setSignalRadioFrequencyLevel(double level)
{
    Q_D(JokerAccessProvider);

    if (qFuzzyCompare(d->signalRadioFrequencyLevel, level))
        return;
    d->signalRadioFrequencyLevel = level;
    qCDebug(jkAccessProvider) << "Set signal radio frequency level:" << d->signalRadioFrequencyLevel;
    emit signalRadioFrequencyLevelChanged(d->signalRadioFrequencyLevel);
}

int JokerAccessProvider::signalErrorsCount() const
{
    return d_func()->signalErrorsCount;
}

void JokerAccessProvider::setSignalErrorsCount(int count)
{
    Q_D(JokerAccessProvider);

    if (d->signalErrorsCount == count)
        return;
    d->signalErrorsCount = count;
    qCDebug(jkAccessProvider) << "Set signal errors count:" << d->signalErrorsCount;
    emit signalErrorsCountChanged(d->signalErrorsCount);
}

bool JokerAccessProvider::isSignalLocked() const
{
    return d_func()->signalLocked;
}

void JokerAccessProvider::setSignalLocked(bool locked)
{
    Q_D(JokerAccessProvider);

    if (d->signalLocked == locked)
        return;
    d->signalLocked = locked;
    qCDebug(jkAccessProvider) << "Set signal locked:" << d->signalLocked;
    emit signalLockedChanged(d->signalLocked);
}

bool JokerAccessProvider::isCamDetected() const
{
    return d_func()->camDetected;
}

void JokerAccessProvider::setCamDetected(bool detected)
{
    Q_D(JokerAccessProvider);

    if (d->camDetected == detected)
        return;
    d->camDetected = detected;
    qCDebug(jkAccessProvider) << "Set CAM detected:" << d->camDetected;
    emit camDetectedChanged(d->camDetected);
}

bool JokerAccessProvider::isAntennaPowered() const
{
    return d_func()->antennaPowered;
}

void JokerAccessProvider::setAntennaPowered(bool antennaPowered)
{
    Q_D(JokerAccessProvider);

    if (d->antennaPowered == antennaPowered)
        return;
    d->antennaPowered = antennaPowered;
    qCDebug(jkAccessProvider) << "Set antenna power:" << d->antennaPowered;
    emit antennaPowerChanged(d->antennaPowered);
}

int JokerAccessProvider::discoveryJobIndex() const
{
    return d_func()->discoveryJobIndex;
}

void JokerAccessProvider::setDiscoveryJobIndex(int discoveryJobIndex)
{
    Q_D(JokerAccessProvider);

    if (d->discoveryJobIndex == discoveryJobIndex)
        return;
    d->discoveryJobIndex = discoveryJobIndex;
    emit discoveryJobIndexChanged(d->discoveryJobIndex);
}

int JokerAccessProvider::discoveryJobsCount() const
{
    return d_func()->discoveryJobIndexes.count();
}

void JokerAccessProvider::setDiscoveryJobsCount(int discoveryJobsCount)
{
    emit discoveryJobsCountChanged(discoveryJobsCount);
}

QVector<JokerProgram> JokerAccessProvider::discoveredPrograms() const
{
    return d_func()->discoveredPrograms;
}

void JokerAccessProvider::processProgram(const JokerProgram &program)
{
    Q_D(JokerAccessProvider);

    auto programIt = std::find(d->discoveredPrograms.begin(), d->discoveredPrograms.end(), program);
    if (programIt == d->discoveredPrograms.end()) {
        qCDebug(jkAccessProvider) << "Discovered new program:" << program;
        d->discoveredPrograms.append(program);
        emit programDiscovered(program);
    } else {
        const auto currentDescription = programIt->m_description;
        const auto newDescription = program.m_description;
        if (currentDescription != newDescription) {
            qCDebug(jkAccessProvider) << "Updated program:" << program;
            programIt->m_description = newDescription;
            emit programUpdated(*programIt);
        } else {
            qCDebug(jkAccessProvider) << "Nothing to do with program:" << program;
        }
    }
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, JokerAccessProvider::ProviderStatus status)
{
    switch (status) {
    case JokerAccessProvider::ProviderStatus::DeviceClosedStatus:
        debug << "ProviderStatus::DeviceClosedStatus";
        break;
    case JokerAccessProvider::ProviderStatus::DeviceOpenedStatus:
        debug << "ProviderStatus::DeviceOpenedStatus";
        break;
    case JokerAccessProvider::ProviderStatus::DeviceClosingStatus:
        debug << "ProviderStatus::DeviceClosingStatus";
        break;
    case JokerAccessProvider::ProviderStatus::DeviceOpeningStatus:
        debug << "ProviderStatus::DeviceOpeningStatus";
        break;
    case JokerAccessProvider::ProviderStatus::ProgramsDiscoveringStatus:
        debug << "ProviderStatus::ProgramsDiscoveringStatus";
        break;
    case JokerAccessProvider::ProviderStatus::ProgramsDiscoveredStatus:
        debug << "ProviderStatus::ProgramsDiscoveredStatus";
        break;
    case JokerAccessProvider::ProviderStatus::ChannelActivating:
        debug << "ProviderStatus::ChannelActivating";
        break;
    case JokerAccessProvider::ProviderStatus::ChannelActivated:
        debug << "ProviderStatus::ChannelActivated";
        break;
    default:
        debug << "ProviderStatus::UnknownStatus";
    }
    return debug;
}

QDebug operator<<(QDebug debug, JokerAccessProvider::ProviderError error)
{
    switch (error) {
    case JokerAccessProvider::ProviderError::NoError:
        debug << "ProviderError::NoError";
        break;
    case JokerAccessProvider::ProviderError::OpenError:
        debug << "ProviderError::OpenError";
        break;
    case JokerAccessProvider::ProviderError::NotOpenError:
        debug << "ProviderError::NotOpenError";
        break;
    case JokerAccessProvider::ProviderError::DiscoveryError:
        debug << "ProviderError::DiscoveryError";
        break;
    case JokerAccessProvider::ProviderError::OperationError:
        debug << "ProviderError::OperationError";
        break;
    case JokerAccessProvider::ProviderError::TimeoutError:
        debug << "ProviderError::TimeoutError";
        break;
    default:
        debug << "ProviderError::UnknownError";
    }
    return debug;
}
#endif // QT_NO_DEBUG_STREAM
