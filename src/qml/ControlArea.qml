import QtQuick 2.6
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

ColumnLayout {
    signal fullScreenRequested();

    spacing: 0

    Image {
        Layout.fillWidth: true
        source: "qrc:/images/cover-bg.png"
        z: -1

        LockControl {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 46
        }

        ExpandControl {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            onClicked: fullScreenRequested();
        }

        BasicCheckBox {
            id: saveButton
            text: "Save to file"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 100

            onClicked: {
                if (checked)
                    jkPlayer.startDumpingToFile();
                else
                    jkPlayer.stopDumpingToFile();
            }
        }

        MuteControl {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 86
        }

        ProgramInfo {
            id: programInfo
            anchors.centerIn: parent
        }

        BasicLabel {
            anchors.horizontalCenter: programInfo.horizontalCenter
            anchors.top: programInfo.bottom
            horizontalAlignment: Text.AlignHCenter

            opacity: {
                if (jkAccessProvider.status === JokerAccessProvider.ChannelActivating)
                    return true;
                if (jkPlayer.state === 1) // TODO: Fix me, it is opening state constant!
                    return true;
                if (jkAccessProvider.error === JokerAccessProvider.TimeoutError)
                    return true;
                return false;
            }

            text: {
                if (jkAccessProvider.status === JokerAccessProvider.ChannelActivating)
                    return qsTr("Channel synchronisation...");
                if (jkPlayer.state === 1)
                    return qsTr("Opening stream...");
                if (jkAccessProvider.error === JokerAccessProvider.TimeoutError)
                    return qsTr("Channel synchronisation timeout");
                return "";
            }

            color: {
                if (jkAccessProvider.error === JokerAccessProvider.TimeoutError)
                    return "red";
                return "#f8f8f8";
            }
        }
    }

    Component.onCompleted: jkPlayer.volume = 100
}
