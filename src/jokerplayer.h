#ifndef JOKERPLAYER_H
#define JOKERPLAYER_H

#include "jokerprogram.h"

#include <QUrl>
#include <QPointer>

#include <VLCQtQml/QmlSource.h>
#include <VLCQtCore/Enums.h>

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class JokerAccessProvider;
class JokerProgramModel;

class JokerPlayer : public VlcQmlSource
{
    Q_OBJECT

    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(JokerProgram program READ program NOTIFY programChanged)

    Q_PROPERTY(JokerAccessProvider *accessProvider READ accessProvider
               WRITE setAccessProvider NOTIFY accessProviderChanged)

    Q_PROPERTY(JokerProgramModel *programModel READ programModel
               WRITE setProgramModel NOTIFY programModelChanged)

public:
    explicit JokerPlayer(QObject *parent = nullptr);
    ~JokerPlayer() override;

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    Q_INVOKABLE void setupProgram(const JokerProgram &program = JokerProgram());
    Q_INVOKABLE void updateProgramAspectRatio(int aspectRatio);

    Q_INVOKABLE void startDumpingToFile();
    Q_INVOKABLE void stopDumpingToFile();

    Vlc::State state() const;

    JokerAccessProvider *accessProvider() const;
    void setAccessProvider(JokerAccessProvider *accessProvider);

    JokerProgramModel *programModel() const;
    void setProgramModel(JokerProgramModel *programModel);

    JokerProgram program() const;

signals:
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void stateChanged(Vlc::State state);

    void accessProviderChanged(JokerAccessProvider *accessProvider);
    void programModelChanged(JokerProgramModel *programModel);

    void programChanged(const JokerProgram &program);

private:
    void processCurrentProgram();
    bool setProgram(const JokerProgram &program);

    VlcInstance *m_instance = nullptr;
    VlcMediaPlayer *m_player = nullptr;
    VlcMedia *m_media = nullptr;
    Vlc::State m_state = Vlc::State::Idle;
    int m_volume = -1;
    bool m_muted = false;
    QPointer<JokerAccessProvider> m_accessProvider; // not owned!
    QPointer<JokerProgramModel> m_programModel; // not owned!
    JokerProgram m_program;

    friend class JokerAccessProvider;
    friend class JokerAccessProviderPrivate;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, Vlc::State state);
#endif // QT_NO_DEBUG_STREAM

#endif // JOKERPLAYER_H
