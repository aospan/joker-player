import QtQuick 2.6
import QtQuick.Layouts 1.3

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.bottomMargin: 9
        anchors.rightMargin: 10
        anchors.leftMargin: 25
        spacing: 10

        SettingsHeader {
            id: header
            Layout.fillWidth: true
        }

        Loader { // settigs loader
            source: {
                if (header.camModuleSettingsActive) {
                    source: "qrc:/qml/CamModuleSettingsArea.qml";
                } else {
                    if (header.satelliteSettingsActive)
                        return "qrc:/qml/SatelliteSettingsArea.qml";
                    return "qrc:/qml/TerrestrialSettingsArea.qml";
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        SignalStatus {
            opacity: !header.camModuleSettingsActive
            Layout.fillWidth: true
        }
    }
}
