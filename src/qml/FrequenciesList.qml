import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ListView {
    id: view
    clip: true

    delegate: RowLayout {
        spacing: 15
        height: 28
        width: view.width

        Image { // indicator
            source: "qrc:/images/checkbox-indicator-bg.png"
            Image {
                anchors.centerIn: parent
                visible: selected
                source: "qrc:/images/checkbox-indicator.png"
            }
        }

        BasicLabel {
            text: frequency + " MHz"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: selected = !selected
        }
    }

    ScrollBar.vertical: ScrollBar {
        visible: view.count > 0
        orientation: Qt.Vertical
        active: true
        padding: 0

        background: Image {
            source: "qrc:/images/playlist-scroll-bg.png"
        }

        contentItem: Image {
            source: "qrc:/images/playlist-scroll-handle.png"
        }
    }
}
