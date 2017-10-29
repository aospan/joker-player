import QtQuick 2.6
import QtQuick.Layouts 1.3

Image {
    signal updated(real position, real size)

    property alias settingsActivated: header.settingsActivated

    function setupPosition(position) {
        if (position === playlist.visibleArea.yPosition)
            return;
        playlist.contentY = playlist.contentHeight * position;
    }

    source: "qrc:/images/playlist-bg.png"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ProgramsListHeader {
            id: header

            Layout.fillWidth: true
        }

        ProgramsList {
            id: playlist
            focus: true
            model: jkProgramProxyModel

            QtObject {
                property real position : playlist.visibleArea.yPosition
                property real ratio : playlist.visibleArea.heightRatio
                onPositionChanged: { updated(position, ratio)}
                onRatioChanged: updated(position, ratio)
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
