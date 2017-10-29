#include "jokeraccessprovider.h"
#include "jokeraccessprovider_p.h"

#include <QTimer>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkAccessProvider)

const int kProgramsDiscoveryInterval = 50;
const int kProgramsUpdateInterval = 30;

JokerAccessProviderPrivate::JokerAccessProviderPrivate(JokerAccessProvider *q)
    : q_ptr(q)
{
    // Keep it empty!
}

JokerAccessProviderPrivate::~JokerAccessProviderPrivate()
{
    // Keep it empty!
}

void JokerAccessProviderPrivate::open()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpeningStatus);
    // Try to open
    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);
}

void JokerAccessProviderPrivate::close()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosingStatus);
    // Try to close
    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceClosedStatus);
}

void JokerAccessProviderPrivate::startScan()
{
    Q_Q(JokerAccessProvider);

    q->setStatus(JokerAccessProvider::ProviderStatus::ProgramsDiscoveringStatus);
    q->setScanningProgress(0.0);

    discoveryIndex = 0;

    if (!discoveryTimer) {
        discoveryTimer = new QTimer(q);
        q->connect(discoveryTimer, &QTimer::timeout, [this]() {
            Q_Q(JokerAccessProvider);

            if (discoveryTimer->interval() == kProgramsDiscoveryInterval) {
                // Add fake program
                const auto channelIndex = discoveryIndexes.at(discoveryIndex);
                const auto id = discoveryIndex + 1000;
                JokerProgram program(channelIndex, id);
                q->processProgram(program);
                q->updateCurrentScanningChannelInfo();

                ++discoveryIndex;

                // Calculate progress.
                const auto progress = double(discoveryIndex) / discoveryIndexes.count();
                q->setScanningProgress(progress);

                if (discoveryIndex >= discoveryIndexes.count()) {
                    discoveryTimer->stop();
                    q->setStatus(JokerAccessProvider::ProviderStatus::ProgramsDiscoveredStatus);
                    qCDebug(jkAccessProvider) << "Discovery finished";

                    // Start programs update simulation
                    discoveryTimer->start(kProgramsUpdateInterval);
                    discoveryIndex = 0;
                }
            } else if (discoveryTimer->interval() == kProgramsUpdateInterval) {
                if (discoveryIndex >= discoveryIndexes.count()) {
                    discoveryTimer->stop();
                } else {
                    const auto program = discoveredPrograms.at(discoveryIndex);
                    JokerProgram updated(program.m_channelIndex, program.m_id);
                    updated.m_description = JokerAccessProvider::tr("PR %1").arg(qrand() % 1000);
                    q->processProgram(updated);
                }
                ++discoveryIndex;
            } else {
                discoveryTimer->stop();
            }
        });
    }

    // Start programs discovery simulation
    discoveryTimer->start(kProgramsDiscoveryInterval);
}

void JokerAccessProviderPrivate::stopScan()
{
    // Note: Do we need to implement it?

    Q_Q(JokerAccessProvider);

    discoveryTimer->stop();

    q->setStatus(JokerAccessProvider::ProviderStatus::DeviceOpenedStatus);

    qCDebug(jkAccessProvider) << "Discovery stopped";
}

void JokerAccessProviderPrivate::registerMediaStream(JokerPlayer *player)
{
    Q_UNUSED(player);

    // Not used in stub implementation.
}

void JokerAccessProviderPrivate::unregisterMediaStream(JokerPlayer *player)
{
    Q_UNUSED(player);

    // Not used in stub implementation.
}

void JokerAccessProviderPrivate::setupActiveChannel()
{
    // Not used in stub implementation.
}
