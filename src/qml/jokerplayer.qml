import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import tv.jokersys.com 1.0

ApplicationWindow {
    id: mainWindow
    visible: true;
    minimumWidth: 1020
    minimumHeight: 700
    property int prevState : 0
//    maximumWidth: 1020
//    maximumHeight: 700

    title: Qt.application.name + " " + Qt.application.version

    FontLoader { source: "qrc:/fonts/geometria.otf"; }
    FontLoader { source: "qrc:/fonts/geometria-light.otf"; }
    FontLoader { source: "qrc:/fonts/geometria-bold.otf"; }
    FontLoader { source: "qrc:/fonts/geometria-medium.otf"; }

    JokerTelevision {
        id: jkTelevision
    }

    JokerSettingsStorage {
        id: jkSettingsStorage
    }

    JokerAccessProvider {
        id: jkAccessProvider
        settingsStorage: jkSettingsStorage
        onErrorOccurred: {
            console.log("Access provider error: " + error)
        }
    }

    JokerPlayer {
        id: jkPlayer
        accessProvider: jkAccessProvider
        programModel: jkProgramModel
    }

    JokerProgramModel {
        id: jkProgramModel
        accessProvider: jkAccessProvider
        settingsStorage: jkSettingsStorage
    }

    JokerProgramProxyModel {
        id: jkProgramProxyModel
        sourceModel: jkProgramModel
    }

    Component.onCompleted: {
        // Startup joker provider
        jkAccessProvider.open();
    }

    Component.onDestruction: {
        jkAccessProvider.close();
    }

    background: Background {
        anchors.fill: parent
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ProgramsListArea {
            id: playlistArea

            Layout.fillHeight: true
        }

        ProgramsListScroll {
            id: playlistScroll
            onPositionChanged: playlistArea.setupPosition(position)

            Layout.fillHeight: true
        }

        SwipeView {
            z: -1
            currentIndex: playlistArea.settingsActivated ? 1 : 0

            VideoOutputArea {
            }

            SettingsArea {
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Item {
        id: dummyVideoOutput
        anchors.fill: parent
        onChildrenChanged: {
            if (children.length > 0) {
                prevState = mainWindow.visibility
                mainWindow.visibility = Window.FullScreen
            }
            else
                mainWindow.visibility = prevState
        }
    }

    MessageDialog {
        title: Qt.application.name
        text: jkAccessProvider.errorString
        visible: {
            if (jkAccessProvider.error === JokerAccessProvider.TimeoutError)
                return false;
            return (jkAccessProvider.error !== JokerAccessProvider.NoError)
        }
        icon: (jkAccessProvider.error === JokerAccessProvider.OpenError) ? StandardIcon.Critical : StandardIcon.Warning
        onAccepted: {
            if (jkAccessProvider.error === JokerAccessProvider.OpenError)
                Qt.quit()
        }
    }

    Connections {
        target: playlistArea
        onUpdated: {
            playlistScroll.position = position;
            playlistScroll.size = size;
        }
    }
}
