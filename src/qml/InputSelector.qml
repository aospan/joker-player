import QtQuick 2.6
import QtQuick.Layouts 1.3

Image {
    readonly property bool satelliteSettingsActive: satelliteSettingsButton.checked

    source: "qrc:/images/source-signal-pane-bg.png"

    RowLayout {
        anchors.centerIn: parent
        spacing: 35

        BasicLabel {
            text: qsTr("input")
            font.weight: Font.Medium
        }

        BasicRadioButton {
            id: satelliteSettingsButton
            pictogram: "qrc:/images/satellite-on.png"
            text: qsTr("Satellite")
        }

        BasicRadioButton {
            id: terrstrialSettingsButton
            checked: true
            pictogram: "qrc:/images/antenna-on.png"
            text: qsTr("Terrestrial")
        }
    }
}
