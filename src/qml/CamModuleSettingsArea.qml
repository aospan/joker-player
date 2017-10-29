import QtQuick 2.6
import QtQuick.Layouts 1.3

Item {

    ColumnLayout {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -160

        BasicLabel {
            text: qsTr("CAM info")
            font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter
        }

        BasicLabel {
            text: qsTr("CAM application manufacturer: 4a60")
            Layout.alignment: Qt.AlignHCenter
        }

        BasicLabel {
            text: qsTr("CAM Menu string: Pocket FTA+")
            Layout.alignment: Qt.AlignHCenter
        }

        BasicLabel {
            text: qsTr("CAM supports the following ca sistem ids: 0x4a60")
            Layout.alignment: Qt.AlignHCenter
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 40

        BasicLabel {
            text: qsTr("MMI CAM Menu")
            font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter
        }

        BasicLabel {
            text: qsTr("Pochet FTA+\n0. Quit menu\n1. Information\nEnter option:")
            Layout.alignment: Qt.AlignHCenter
        }
    }

    Image {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -60
        source: "qrc:/images/cam-module-info-delimiter.png"
    }

    RowLayout {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 135

        spacing: 20

        BasicLineEdit {
            placeholderText: qsTr("Enter value")
            background: Image {
                source: "qrc:/images/cam-module-line-edit-bg.png"
            }
            hint: qsTr("Enter CAM module value")
        }

        BasicPushButton {
            caption: qsTr("Send")
            hint: qsTr("Send CAM module value")
        }
    }
}
