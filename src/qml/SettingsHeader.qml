import QtQuick 2.6
import QtQuick.Layouts 1.3

RowLayout {
    property alias satelliteSettingsActive: inputSelector.satelliteSettingsActive
    property alias camModuleSettingsActive: camModuleDetector.camModuleSettingsActive

    spacing: 0
    anchors.left: parent.left
    anchors.right: parent.right

    InputSelector {
        id: inputSelector
        opacity: !camModuleDetector.camModuleSettingsActive
    }

    Item { // dummy
        Layout.fillWidth: true
    }

    CamModuleDetector {
        id: camModuleDetector
        visible: jkAccessProvider.camDetected
    }
}
