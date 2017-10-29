import QtQuick 2.6
import QtQuick.Layouts 1.3

Image {
    readonly property bool camModuleSettingsActive: camModuleSettingsButton.checked

    source: "qrc:/images/cam-module-pane-bg.png"

    RowLayout {
        anchors.centerIn: parent
        spacing: 16

        BasicToolButton {
            id: camModuleSettingsButton
            checkable: true

            picture: "qrc:/images/lock.png"
            hint: qsTr("Show CAM module settings")
        }

        BasicLabel {
            text: qsTr("CAM Module detected")
            font.weight: Font.Medium
        }
    }
}
