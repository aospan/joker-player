import QtQuick 2.6
import QtQuick.Controls 2.0

TextField {
    id: control
    property alias hint: tooltip.text

    padding: 0
    color: "#878787"
    font.pixelSize: 14
    font.family: "geometria"
    font.weight: Font.Normal

    background: Image {
        source: "qrc:/images/line-edit-bg.png"
    }

    ToolTip {
        id: tooltip
        visible: control.hovered
    }
}
