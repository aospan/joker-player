import QtQuick 2.6
import QtQuick.Layouts 1.3

Item {
    id: root

    ColumnLayout {
        anchors.centerIn: parent

        ColumnLayout {
            BasicLabel {
                text: qsTr("CAM info")
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }

            BasicLabel {
                text: qsTr("CAM application manufacturer: %1")
                        .arg(jkAccessProvider.camInfo.applicationManufacturer);
                Layout.alignment: Qt.AlignHCenter
            }

            BasicLabel {
                text: qsTr("CAM Menu string: %1")
                        .arg(jkAccessProvider.camInfo.menuString)
                Layout.alignment: Qt.AlignHCenter
            }

            BasicLabel {
                text: {
                    var caids = jkAccessProvider.caids;
                    var formattedCaids = "";
                    for (var index = 0; index < caids.length; ++index) {
                        if (index === (caids.length - 1)) {
                            formattedCaids += caids[index];
                        } else {
                            var mod = index % 10;
                            if (mod === 0)
                                formattedCaids += "\n";
                            formattedCaids += caids[index] + ", ";
                        }
                    }

                    return qsTr("CAM supports the following ca sistem ids:%1")
                        .arg(formattedCaids)
                }
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Image {
                id: delimiter
                source: "qrc:/images/cam-module-info-delimiter.png"
                Layout.alignment: Qt.AlignHCenter
            }

            BasicLabel {
                text: qsTr("MMI CAM Menu")
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }

            BasicLabel {
                text: jkAccessProvider.mmiCamMenu
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            spacing: 20

            BasicLineEdit {
                id: command
                hoverEnabled: false
                placeholderText: qsTr("Enter value")
                background: Image {
                    source: "qrc:/images/cam-module-line-edit-bg.png"
                }
                Keys.onReturnPressed: sendMmiCommand();

                function sendMmiCommand() {
                    jkAccessProvider.sendMmiCommand(command.text);
                    command.text = "";
                }
            }

            BasicPushButton {
                hoverEnabled: false
                caption: qsTr("Send")
                onClicked: command.sendMmiCommand();
            }

            Layout.alignment: Qt.AlignHCenter
        }
    }

    Component.onCompleted: jkAccessProvider.startMmiSession()
}
