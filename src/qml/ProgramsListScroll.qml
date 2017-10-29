import QtQuick 2.6
import QtQuick.Controls 2.2

ScrollBar {
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
