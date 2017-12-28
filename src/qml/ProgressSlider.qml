import QtQuick 2.6
import QtQuick.Controls 2.0

Slider {
    id: control
    padding: 0

    from: 0.0
    to: 1.0
    value: player.position

    background: Image {
        source: "qrc:/images/progress-groove-bg.png"
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2.0 - height / 2.0
        height: implicitHeight

        Image {
            source: "qrc:/images/progress-groove.png"
            width: control.visualPosition * parent.width
        }
    }

    handle: Item {
        width: 7
        height: 16
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2.0 - height / 2.0

        Image {
            anchors.centerIn: parent
            source: "qrc:/images/progress-handle.png"
        }
    }
}
