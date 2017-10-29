import QtQuick 2.6
import QtQuick.Controls 2.2

ToolButton {
    id: control

    property alias picture: pic.source
    property alias hint: tooltip.text

    background: Item {
        implicitWidth: pic.implicitWidth
        implicitHeight: pic.implicitHeight

        Image {
            id: pic
            anchors.centerIn: parent
        }
    }

    ToolTip {
        id: tooltip
        visible: control.hovered
    }
}
