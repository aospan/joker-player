#include "jokerplayer.h"
#include "jokeraccessprovider.h"

#include <VLCQtCore/Audio.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/TrackModel.h>
#include <VLCQtCore/Video.h>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkPlayer)

JokerPlayer::JokerPlayer(QObject *parent)
    : VlcQmlSource(parent)
{
    qCDebug(jkPlayer) << "Create" << this;
    // set VLC plugins path as executable path + "/plugins"
    // example final path: /Users/aospan/src/joker-player/build/src/joker-player.app/Contents/MacOS/plugins 
    VlcCommon::setPluginPath(QCoreApplication::applicationDirPath() + QString::fromLatin1("/../"));

    m_instance = new VlcInstance(VlcCommon::args(), this);
    m_player = new VlcMediaPlayer(m_instance);

    // TODO: Only for debugginng, need to remove on release!
    m_instance->setLogLevel(Vlc::DebugLevel);

    connect(m_player, &VlcMediaPlayer::stateChanged, [this]() {
        const auto state = m_player->state();
        if (m_state == state)
            return;
        m_state = state;
        qCDebug(jkPlayer) << "State" << m_state;
        emit stateChanged(m_state);
    });

    setPlayer(m_player);

    // Get initial states of volume and mute
    m_volume = m_player->audio()->volume();
    m_muted = m_player->audio()->getMute();
}

JokerPlayer::~JokerPlayer()
{
    qCDebug(jkPlayer) << "Destroy" << this;
    m_player->stop();
    removePlayer();

    delete m_media;
}

int JokerPlayer::volume() const
{
    return m_volume;
}

void JokerPlayer::setVolume(int volume)
{
    if (m_volume == volume)
        return;
    m_volume = volume;
    qCDebug(jkPlayer) << "Set volume:" << m_volume;
    m_player->audio()->setVolume(m_volume);
    emit volumeChanged(m_volume);
}

bool JokerPlayer::isMuted() const
{
    return m_muted;
}

void JokerPlayer::setMuted(bool muted)
{
    if (m_muted == muted)
        return;
    m_muted = muted;
    qCDebug(jkPlayer) << "Set muted:" << m_muted;
    m_player->audio()->setMute(m_muted);
    emit mutedChanged(m_muted);
}

void JokerPlayer::setupProgram(const JokerProgram &program)
{
    if (!m_accessProvider) {
        qCCritical(jkPlayer) << "No access provider set";
        return;
    }

    if (m_program == program)
        return;

    m_program = program;
    qCDebug(jkPlayer) << "Set program:" << m_program.m_description
                      << "id:" << m_program.m_id;

    emit programChanged(m_program);

    if (m_program.m_id < 0 || m_program.m_channelIndex < 0) {
        m_player->stop();
    } else {
        const auto currentActiveChannelIndex = m_accessProvider->activeChannelIndex();
        if (currentActiveChannelIndex == m_program.m_channelIndex) {
            const auto providerStatus = m_accessProvider->status();
            if (providerStatus == JokerAccessProvider::ProviderStatus::ChannelActivated) {
                // Process (switch) current program immediatelly because current
                // active channel matches with the program's channel.
                processCurrentProgram();
            } else {
                // Do nothing here because the program will be started
                // asynchronously when the state becomes as ChannelActivated.
            }
        } else {
            // Setup asynchronously active channel for current program
            m_accessProvider->setupActiveChannel(m_program.m_channelIndex);
        }
    }
}

Vlc::State JokerPlayer::state() const
{
    return m_state;
}

JokerAccessProvider *JokerPlayer::accessProvider() const
{
    return m_accessProvider;
}

void JokerPlayer::setAccessProvider(JokerAccessProvider *accessProvider)
{
    if (m_accessProvider) {
        m_player->stop();
        disconnect(m_accessProvider, &JokerAccessProvider::statusChanged,
                   this, &JokerPlayer::processCurrentProgram);
        m_accessProvider->unregisterMediaStream(this);
    }

    m_accessProvider = accessProvider;

    qCDebug(jkPlayer) << "Set access provider:" << m_accessProvider;

    if (m_accessProvider) {
        m_accessProvider->registerMediaStream(this);
        connect(m_accessProvider, &JokerAccessProvider::statusChanged,
                this, &JokerPlayer::processCurrentProgram);
    }

    emit accessProviderChanged(m_accessProvider);
}

void JokerPlayer::processCurrentProgram()
{
    if (!m_media) {
        qCCritical(jkPlayer) << "Unable to process program due to mo media object set";
        return;
    }

    const auto providerStatus = m_accessProvider->status();
    switch (providerStatus) {
    case JokerAccessProvider::ProviderStatus::ChannelActivated: {
        qCCritical(jkPlayer) << "Start program:" << m_program;
        m_player->stop();
        delete m_media;
        m_accessProvider->unregisterMediaStream(this);
        m_accessProvider->registerMediaStream(this);
        m_media->setProgram(m_program.m_id);
        m_player->play();
    }
        break;
    default: {
        if (m_player->state() != Vlc::State::Stopped) {
            qCCritical(jkPlayer) << "Stop previous program";
            m_player->stop();
        }
    }
        break;
    }
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, Vlc::State state)
{
    switch (state) {
    case Vlc::State::Idle:
        debug << "State::Idle";
        break;
    case Vlc::State::Opening:
        debug << "State::Opening";
        break;
    case Vlc::State::Buffering:
        debug << "State::Buffering";
        break;
    case Vlc::State::Playing:
        debug << "State::Playing";
        break;
    case Vlc::State::Paused:
        debug << "State::Paused";
        break;
    case Vlc::State::Stopped:
        debug << "State::Stopped";
        break;
    case Vlc::State::Ended:
        debug << "State::Ended";
        break;
    case Vlc::State::Error:
        debug << "State::Error";
        break;
    default:
        debug << "State::Unknown";
        break;
    }
    return debug;
}
#endif // QT_NO_DEBUG_STREAM
