import QtQuick 2.6
import QtQuick.Controls 2.2

Button {
    id: control

    property alias caption: label.text
    property alias hint: tooltip.text

    contentItem: BasicLabel {
        id: label
        color: "#ffffff"
        horizontalAlignment: Text.AlignHCenter
    }

    background: Item {
        implicitWidth: 106
        implicitHeight: 28

        Image {
            anchors.centerIn: parent
            source: "qrc:/images/push-button-bg.png"
        }
    }

    ToolTip {
        id: tooltip
        visible: control.hovered
    }
}
