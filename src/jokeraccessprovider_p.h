#ifndef JOKERACCESSPROVIDER_P_H
#define JOKERACCESSPROVIDER_P_H

#include "jokeraccessprovider.h"
#include "jokersettingsstorage.h"

#include <QPointer>

#ifdef USE_STUB_JOKER
    // Include stub's headers
#else
#  include <u_drv_tune.h>
#  include <joker_ts.h>
#  include <joker_ci.h>
#endif

QT_FORWARD_DECLARE_CLASS(QTimer)

class JokerAccessProviderPrivate
{
    Q_DECLARE_PUBLIC(JokerAccessProvider)

public:
    JokerAccessProviderPrivate(JokerAccessProvider *q);
    ~JokerAccessProviderPrivate();

    void open();
    void close();
    void startScan();
    void stopScan();
    void registerMediaStream(JokerPlayer *player);
    void unregisterMediaStream(JokerPlayer *player);
    void setupActiveChannel();
    int discoveringChannelIndex() const;

    JokerAccessProvider::ProviderStatus status = JokerAccessProvider::ProviderStatus::DeviceClosedStatus;
    JokerAccessProvider::ProviderError error = JokerAccessProvider::ProviderError::NoError;
    QString errorString;
    QVector<JokerProgram> discoveredPrograms;
    double scanningProgress = 0.0;
    JokerAccessProvider::SignalQuality signalQuality = JokerAccessProvider::SignalQuality::BadSignalQuality;
    double signalCarrierNoiseRatio = 0.0;
    double signalRadioFrequencyLevel = 0.0;
    int signalErrorsCount = 0;
    bool signalLocked = false;
    bool camDetected = false;
    JokerCamInfo camInfo;
    QString caids;
    bool antennaPowered = false;
    QTimer *discoveryTimer = nullptr;
    QTimer *lockTimer = nullptr;

    QVector<int> discoveryJobIndexes;
    int discoveryJobIndex = -1;
    int activeChannelIndex = -1;
    QPointer<JokerSettingsStorage> settingsStorage; // not owned

    JokerAccessProvider *q_ptr = nullptr;

#ifdef USE_STUB_JOKER
    int discoveryIndex = 0;
    QVector<int> discoveryIndexes;
#else
    void processDiscoveryInterval();
    void startProgramsDiscovery();
    void completeProgramsDiscovery();
    void startTune();
    void completeTune();
    void startNextChannelDiscovery();
    void processProgramUpdateEvent(QEvent *event);
    void processChannelStatusEvent(QEvent *event);
    void processCamInfoEvent(QEvent *event);
    void processCaidsEvent(QEvent *event);

    big_pool_t pool;
    joker_t joker;
    tune_info_t info;
    QObject *messageHandler = nullptr;
    bool canProcessProgramsUpdateEvents = false;
    bool canProcessChannelStatusEvents = false;
#endif
};

#endif // JOKERACCESSPROVIDER_P_H
