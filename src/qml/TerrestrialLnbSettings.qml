import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

import tv.jokersys.com 1.0

Image {
    source: "qrc:/images/terrstrial-lnb-settings-bg.png"

    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        x: 10
        spacing: 10

        BasicCheckBox {
            text: qsTr("DVB-T")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.DvbtStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.DvbtStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.DvbtStandardFlag
            }
        }

        BasicCheckBox {
            text: qsTr("DVB-T2")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.DvbsgtStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.DvbsgtStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.DvbsgtStandardFlag
            }
        }

        BasicCheckBox {
            text: qsTr("ATSC")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.AtscStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.AtscStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.AtscStandardFlag
            }
        }

        BasicCheckBox {
            text: qsTr("ISDB-T")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.IsdbtStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.IsdbtStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.IsdbtStandardFlag
            }
        }

        BasicCheckBox {
            text: qsTr("DTMB")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.DtmbStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.DtmbStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.DtmbStandardFlag
            }
        }

        BasicCheckBox {
            text: qsTr("DVB-C")
            checked: jkStandardsProxyModel.choosingFlags & JokerStandardsProxyModel.DvbcStandardFlag
            onCheckedChanged: {
                if (checked)
                    jkStandardsProxyModel.choosingFlags |= JokerStandardsProxyModel.DvbcStandardFlag
                else
                    jkStandardsProxyModel.choosingFlags &= ~JokerStandardsProxyModel.DvbcStandardFlag
            }
        }

        BasicToolButton {
            picture: "qrc:/images/help.png"
            hint: qsTr("%1\n%2\n%3\n%4\n%5\n%6")
                .arg(qsTr("DVB-S/S2 – satellite, is found everywhere in the world"))
                .arg(qsTr("DVB-T/T2 – mostly Europe"))
                .arg(qsTr("DVB-C/C2 – cable, is found everywhere in the world"))
                .arg(qsTr("ISDB-T – Brazil, Latin America, Japan"))
                .arg(qsTr("ATSC – USA, Canada, Mexico, South Korea"))
                .arg(qsTr("DTMB – China, Cuba, Hong-Kong, Pakistan"))
        }
    }
}
