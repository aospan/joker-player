import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

Image {
    source: "qrc:/images/satellite-transponder-settings-bg.png"

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            BasicLabel {
                text: qsTr("Satellites")
                font.weight: Font.Medium
            }

            SatellitesList {
                id: satellitesList
                model: jkSatellitesModel
                onSatelliteNameChanged: jkChannelsProxyModel.channelName = satelliteName

                Layout.fillHeight: true
            }

            Layout.margins: 10
        }

        ColumnLayout {
            ColumnLayout {
                RowLayout {
                    BasicLabel {
                        text: qsTr("Transponders")
                        font.weight: Font.Medium
                    }

                    ChannelsSelectionMenu {
                        opacity: (transpondersList.count > 0)
                    }
                }

                TranspondersList {
                    id: transpondersList
                    width: 200
                    model: jkChannelsProxyModel

                    Layout.fillHeight: true
                }
            }

            RowLayout {
                ChannelScanProgress {
                    id: scanProgress

                    Layout.fillWidth: true
                }

                BasicCheckBox {
                    id: truncateButton
                    text: "clean channel list"
                }

                BasicPushButton {
                    id: scanButton
                    caption: {
                        if (jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveringStatus) {
                            return qsTr("Stop Scan");
                        } else if (jkAccessProvider.status === JokerAccessProvider.DeviceOpenedStatus
                                   || jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveredStatus
                                   || jkAccessProvider.status === JokerAccessProvider.ChannelActivated) {
                            return qsTr("Start Scan");
                        } else {
                            return qsTr("Unknown");
                        }
                    }
                    hint: qsTr("Start channel scanning")
                    onClicked: {
                        if (jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveringStatus) {
                            jkAccessProvider.stopScan();
                        } else if (jkAccessProvider.status === JokerAccessProvider.DeviceOpenedStatus
                                   || jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveredStatus
                                   || jkAccessProvider.status === JokerAccessProvider.ChannelActivated) {
                            jkAccessProvider.startScan(truncateButton.checked);
                        }
                    }
                }
            }

            Layout.margins: 10
        }
    }

    Component.onDestruction: {
        if (jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveringStatus)
            jkAccessProvider.stopScan();
    }
}
