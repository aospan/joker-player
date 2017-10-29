import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RadioButton {
    id: control
    property alias pictogram: pic.source
    property alias hint: toolTip.text

    indicator: Image {
        source: "qrc:/images/radiobutton-indicator-bg.png"
        x: control.leftPadding
        y: parent.height / 2 - height / 2

        Image {
            anchors.centerIn: parent
            visible: control.checked
            source: "qrc:/images/radiobutton-indicator.png"
        }
    }

    contentItem: RowLayout {
        spacing: 9
        BasicLabel {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            leftPadding: control.indicator.width + control.spacing
            text: control.text
        }

        Image {
            id: pic
        }
    }

    ToolTip {
        id: toolTip
        text: control.text
        visible: control.hovered
    }
}
