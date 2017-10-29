#ifndef JOKERSETTINGSSTORAGE_H
#define JOKERSETTINGSSTORAGE_H

#include "jokerprogram.h"
#include "jokerchannel.h"

#include <QObject>
#include <QVector>

QT_FORWARD_DECLARE_CLASS(QTimer)

// JokerLnbPreset

class JokerLnbPreset
{
    Q_GADGET

    Q_PROPERTY(int lowFrequency MEMBER m_lowFrequency)
    Q_PROPERTY(int highFrequency MEMBER m_highFrequency)
    Q_PROPERTY(int switchFrequency MEMBER m_switchFrequency)

public:
    explicit JokerLnbPreset(int lowFrequency = 0,
                            int highFrequency = 0,
                            int switchFrequency = 0);

    bool operator==(const JokerLnbPreset &other) const;
    bool operator!=(const JokerLnbPreset &other) const;

    int m_lowFrequency = 0;
    int m_highFrequency = 0;
    int m_switchFrequency = 0;
};

Q_DECLARE_TYPEINFO(JokerLnbPreset, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(JokerLnbPreset)

// JokerSettingsStorage

class JokerSettingsStorage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(JokerTelevision::Tone tone READ tone WRITE setTone NOTIFY toneChanged)
    Q_PROPERTY(int presetIndex READ presetIndex WRITE setPresetIndex NOTIFY presetIndexChanged)
    Q_PROPERTY(JokerLnbPreset preset READ preset WRITE setPreset NOTIFY presetChanged)

public:
    explicit JokerSettingsStorage(QObject *parent = nullptr);
    ~JokerSettingsStorage() override;

    JokerTelevision::Tone tone() const;
    void setTone(JokerTelevision::Tone tone);

    enum PresetIndex {
        UniversalPresetIndex,
        L10750PresetIndex,
        L10700PresetIndex,
        ManualPresetIndex
    };
    Q_ENUM(PresetIndex)

    int presetIndex() const;
    void setPresetIndex(int presetIndex);

    JokerLnbPreset preset() const;
    void setPreset(const JokerLnbPreset &preset);

signals:
    void toneChanged(JokerTelevision::Tone);
    void presetIndexChanged(int presetIndex);
    void presetChanged(const JokerLnbPreset &preset);

private:
    void openDatabase();
    void closeDatabase();

    void restoreChannels();
    void restoreSettings();

    QVector<JokerChannel> channelsFromConfigs() const;
    QVector<JokerChannel> channelsFromConfig(const QString &configPath) const;

    void saveProgram(const JokerProgram &program);
    QVector<JokerProgram> storedPrograms() const;

    void saveChannel(const JokerChannel &channel);

    void processPendings();

    QTimer *m_timer = nullptr;
    QVector<JokerProgram> m_pendingPrograms;
    QVector<JokerChannel> m_pendingChannels;
    QVector<JokerChannel> m_channels;
    QVector<JokerLnbPreset> m_presets;
    int m_presetIndex = 0;
    JokerTelevision::Tone m_tone = JokerTelevision::ToneOff;

    const QString m_connection;

    friend class JokerProgramModel;
    friend class JokerStandardsModel;
    friend class JokerSatellitesModel;
    friend class JokerChannelsModel;
    friend class JokerAccessProvider;
    friend class JokerAccessProviderPrivate;
    friend class JokerStandardsProxyModel;
    friend class JokerChannelsProxyModel;
};

#endif // JOKERSETTINGSSTORAGE_H
