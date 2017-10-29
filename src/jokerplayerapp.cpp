#include "jokerplayer.h"
#include "jokeraccessprovider.h"
#include "jokerprogrammodel.h"
#include "jokersettingsstorage.h"
#include "jokerstandardsmodel.h"
#include "jokerchannelsmodel.h"
#include "jokersatellitesmodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QIcon>
#include <QLoggingCategory>

#include <VLCQtQml/Qml.h>

Q_LOGGING_CATEGORY(jkApplication, "joker.app")
Q_LOGGING_CATEGORY(jkSettingsStorage, "joker.settings.storage")
Q_LOGGING_CATEGORY(jkAccessProvider, "joker.access.provider")
Q_LOGGING_CATEGORY(jkPlayer, "joker.player")
Q_LOGGING_CATEGORY(jkProgramModel, "joker.program.model")
Q_LOGGING_CATEGORY(jkStandardsModel, "joker.signalstandard.model")
Q_LOGGING_CATEGORY(jkChannelsModel, "joker.channels.model")
Q_LOGGING_CATEGORY(jkSatellitesModel, "joker.satellites.model")

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setOrganizationName(QStringLiteral("Joker TV"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("tv.jokersys.com"));
    QGuiApplication::setApplicationName(QStringLiteral("Joker TV"));

#ifdef USE_DEVELOPMENT_BUILD
    QGuiApplication::setApplicationVersion(QStringLiteral(GIT_VERSION));
#endif

    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/images/logo-joker-square.png")));

    qCDebug(jkApplication) << "Starting:" << QGuiApplication::applicationName()
                           << "version:" << QGuiApplication::applicationVersion();

    // Register vlc-qt types.
    VlcQml::registerTypes();

    // Register own types.
    qmlRegisterType<JokerSettingsStorage>("tv.jokersys.com", 1, 0, "JokerSettingsStorage");
    qmlRegisterType<JokerAccessProvider>("tv.jokersys.com", 1, 0, "JokerAccessProvider");
    qmlRegisterType<JokerPlayer>("tv.jokersys.com", 1, 0, "JokerPlayer");
    qmlRegisterType<JokerProgramModel>("tv.jokersys.com", 1, 0, "JokerProgramModel");
    qmlRegisterType<JokerProgramProxyModel>("tv.jokersys.com", 1, 0, "JokerProgramProxyModel");
    qmlRegisterType<JokerStandardsModel>("tv.jokersys.com", 1, 0, "JokerStandardsModel");
    qmlRegisterType<JokerStandardsProxyModel>("tv.jokersys.com", 1, 0, "JokerStandardsProxyModel");
    qmlRegisterType<JokerSatellitesModel>("tv.jokersys.com", 1, 0, "JokerSatellitesModel");
    qmlRegisterType<JokerChannelsModel>("tv.jokersys.com", 1, 0, "JokerChannelsModel");
    qmlRegisterType<JokerChannelsProxyModel>("tv.jokersys.com", 1, 0, "JokerChannelsProxyModel");
    qmlRegisterType<JokerTelevision>("tv.jokersys.com", 1, 0, "JokerTelevision");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/qml/jokerplayer.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
