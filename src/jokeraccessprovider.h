#ifndef JOKERPROVIDER_H
#define JOKERPROVIDER_H

#include "jokerprogram.h"

#include <QObject>
#include <QVector>
#include <QLoggingCategory>

class JokerPlayer;
class JokerAccessProvider;
class JokerSettingsStorage;

class JokerAccessProviderPrivate;

// JokerCamInfo

class JokerCamInfo
{
    Q_GADGET

    Q_PROPERTY(QString applicationType MEMBER m_applicationType)
    Q_PROPERTY(QString applicationManufacturer MEMBER m_applicationManufacturer)
    Q_PROPERTY(QString manufacturerCode MEMBER m_manufacturerCode)
    Q_PROPERTY(QString menuString MEMBER m_menuString)
    Q_PROPERTY(QString infoString MEMBER m_infoString)

public:
    explicit JokerCamInfo(const QString &applicationType = QString(),
                          const QString &applicationManufacturer = QString(),
                          const QString &manufacturerCode = QString(),
                          const QString &menuString = QString(),
                          const QString &infoString = QString());

    bool operator==(const JokerCamInfo &other) const;
    bool operator!=(const JokerCamInfo &other) const;

    QString m_applicationType;
    QString m_applicationManufacturer;
    QString m_manufacturerCode;
    QString m_menuString;
    QString m_infoString;
};

Q_DECLARE_TYPEINFO(JokerCamInfo, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(JokerCamInfo)

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const JokerCamInfo &camInfo);
#endif // QT_NO_DEBUG_STREAM

// JokerAccessProvider

class JokerAccessProvider : public QObject
{
    Q_OBJECT

    // Readable and notifiable properties
    Q_PROPERTY(double scanningProgress READ scanningProgress
               NOTIFY scanningProgressChanged)

    Q_PROPERTY(SignalQuality signalQuality READ signalQuality
               NOTIFY signaQualityChanged)
    Q_PROPERTY(double signalCarrierNoiseRatio READ signalCarrierNoiseRatio
               NOTIFY signalCarrierNoiseRatioChanged)
    Q_PROPERTY(double signalRadioFrequencyLevel READ signalRadioFrequencyLevel
               NOTIFY signalRadioFrequencyLevelChanged)
    Q_PROPERTY(int signalErrorsCount READ signalErrorsCount
               NOTIFY signalErrorsCountChanged)
    Q_PROPERTY(bool signalLocked READ isSignalLocked
               NOTIFY signalLockedChanged)

    Q_PROPERTY(bool camDetected READ isCamDetected
               NOTIFY camDetectedChanged)
    Q_PROPERTY(JokerCamInfo camInfo READ camInfo
               NOTIFY camInfoChanged)
    Q_PROPERTY(QString caids READ caids
               NOTIFY caidsChanged)

    Q_PROPERTY(ProviderStatus status READ status
               NOTIFY statusChanged)
    Q_PROPERTY(ProviderError error READ error
               NOTIFY errorOccurred)
    Q_PROPERTY(QString errorString READ errorString
               NOTIFY errorOccurred)

    Q_PROPERTY(int discoveryJobIndex READ discoveryJobIndex
               NOTIFY discoveryJobIndexChanged)

    Q_PROPERTY(int discoveryJobsCount READ discoveryJobsCount
               NOTIFY discoveryJobsCountChanged)

    // Readable, writable and notifiable properties
    Q_PROPERTY(bool antennaPower READ isAntennaPowered
               WRITE setAntennaPowered NOTIFY antennaPowerChanged)

    Q_PROPERTY(JokerSettingsStorage *settingsStorage READ settingsStorage
               WRITE setSettingsStorage NOTIFY settingsStorageChanged)

public:
    enum class ProviderStatus {
        DeviceClosedStatus,
        DeviceOpenedStatus,
        DeviceClosingStatus,
        DeviceOpeningStatus,
        ProgramsDiscoveringStatus,
        ProgramsDiscoveredStatus,
        ChannelActivating,
        ChannelActivated
    };
    Q_ENUM(ProviderStatus)

    enum class ProviderError {
        NoError,
        OpenError, // critical
        NotOpenError,
        DiscoveryError,
        OperationError,
        TimeoutError
    };
    Q_ENUM(ProviderError)

    enum class SignalQuality {
        BadSignalQuality,
        WeakSignalQuality,
        GoodSignalQuality
    };
    Q_ENUM(SignalQuality)

    explicit JokerAccessProvider(QObject *parent = nullptr);
    ~JokerAccessProvider() override;

    JokerSettingsStorage *settingsStorage() const;
    void setSettingsStorage(JokerSettingsStorage *settingsStorage);

    ProviderStatus status() const;

    ProviderError error() const;
    QString errorString() const;

    double scanningProgress() const;

    SignalQuality signalQuality() const;
    double signalCarrierNoiseRatio() const;
    double signalRadioFrequencyLevel() const;
    int signalErrorsCount() const;
    bool isSignalLocked() const;

    bool isCamDetected() const;
    JokerCamInfo camInfo() const;
    QString caids() const;

    bool isAntennaPowered() const;
    void setAntennaPowered(bool antennaPowered);

    int discoveryJobIndex() const;
    int discoveryJobsCount() const;

    QVector<JokerProgram> discoveredPrograms() const;

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();

signals:
    void settingsStorageChanged(JokerSettingsStorage *settingsStorage);

    void statusChanged(ProviderStatus status);
    void errorOccurred(ProviderError error);

    void programDiscovered(const JokerProgram &program);
    void programUpdated(const JokerProgram &program);

    void scanningProgressChanged(double progress);
    void scanningChannelChanged(const QString &standardName, int frequencyMhz);

    void signaQualityChanged(SignalQuality quality);
    void signalCarrierNoiseRatioChanged(double ratio);
    void signalRadioFrequencyLevelChanged(double level);
    void signalErrorsCountChanged(int count);
    void signalLockedChanged(bool locked);
    void camDetectedChanged(bool detected);
    void camInfoChanged(const JokerCamInfo &camInfo);
    void caidsChanged(const QString &caids);
    void antennaPowerChanged(bool on);

    void discoveryJobIndexChanged(int discoveryJobIndex);
    void discoveryJobsCountChanged(int discoveryJobsCount);

private:
    void setStatus(ProviderStatus status);
    void setError(ProviderError error, const QString &errorString);

    void setScanningProgress(double progress);

    void setSignalQuality(SignalQuality quality);
    void setSignalCarrierNoiseRatio(double ratio);
    void setSignalRadioFrequencyLevel(double level);
    void setSignalErrorsCount(int count);
    void setSignalLocked(bool locked);

    void setCamDetected(bool detected);
    void setCamInfo(const JokerCamInfo &camInfo);
    void setCaids(const QString &caids);

    void setDiscoveryJobIndex(int discoveryJobIndex);
    void setDiscoveryJobsCount(int discoveryJobsCount);

    void processProgram(const JokerProgram &program);

    void registerMediaStream(JokerPlayer *player);
    void unregisterMediaStream(JokerPlayer *player);

    int activeChannelIndex() const;
    void setupActiveChannel(int channelIndex);

    void updateCurrentScanningChannelInfo();

    JokerAccessProviderPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(JokerAccessProvider)

    friend class JokerPlayer;
    friend class JokerMessageHandler;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, JokerAccessProvider::ProviderStatus status);
QDebug operator<<(QDebug debug, JokerAccessProvider::ProviderError error);
#endif // QT_NO_DEBUG_STREAM

#endif // JOKERPROVIDER_H
