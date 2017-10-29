import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

Image {
    source: "qrc:/images/terrstrial-antenna-settings-bg.png"

    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        x: 10
        spacing: 10

        BasicCheckBox {
            text: qsTr("Enable 5V antenna power")
            checked: jkAccessProvider.antennaPower
            onCheckedChanged: jkAccessProvider.antennaPower = checked
        }

        BasicToolButton {
            picture: "qrc:/images/help.png"
            hint: qsTr("%1\n%2\n%3\n%4")
                .arg(qsTr("Inject 5V to power active terrestrial antenna."))
                .arg(qsTr("WARNING: Do not enable 5V if you have passive antenna!"))
                .arg(qsTr("This can cause shortage and device overheating and damage!"))
                .arg(qsTr("WARNING: Do not enable if unsure!"))
        }
    }
}
