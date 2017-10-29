import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

ListView {
    id: view
    clip: true

    function modulationFriendlyName(modulation) {
        switch (modulation) {
        case JokerTelevision.QpskModulation:
            return qsTr("QPSK");
        case JokerTelevision.Qam16Modulation:
            return qsTr("QAM16");
        case JokerTelevision.Qam32Modulation:
            return qsTr("QAM32");
        case JokerTelevision.Qam64Modulation:
            return qsTr("QAM64");
        case JokerTelevision.Qam128Modulation:
            return qsTr("QAM128");
        case JokerTelevision.Qam256Modulation:
            return qsTr("QAM256");
        case JokerTelevision.QamAutoModulation:
            return qsTr("QAM AUTO");
        case JokerTelevision.Vsb8Modulation:
            return qsTr("VSB8");
        case JokerTelevision.Vsb16Modulation:
            return qsTr("VSB16");
        case JokerTelevision.Psk8Modulation:
            return qsTr("PSK8");
        case JokerTelevision.Apsk16Modulation:
            return qsTr("APSK16");
        case JokerTelevision.Apsk32Modulation:
            return qsTr("APSK32");
        case JokerTelevision.DqpskModulation:
            return qsTr("DQPSK");
        case JokerTelevision.Qam4NrModulation:
            return qsTr("QAM4NR");
        default:
            return qsTr("Unknown");
        }
    }

    function voltageFriendlyName(voltage) {
        switch (voltage) {
        case JokerTelevision.Voltage13:
            return qsTr("13V");
        case JokerTelevision.Voltage18:
            return qsTr("18V");
        default:
            return qsTr("Off");
        }
    }

    delegate: RowLayout {
        spacing: 15
        height: 28
        width: view.width

        Image { // indicator
            source: "qrc:/images/checkbox-indicator-bg.png"
            Image {
                anchors.centerIn: parent
                visible: selected
                source: "qrc:/images/checkbox-indicator.png"
            }
        }

        BasicLabel {
            text: {
                var modulationName = view.modulationFriendlyName(modulation);
                var voltageName = view.voltageFriendlyName(voltage);
                return qsTr("%1 MHz, %2, %3")
                    .arg(frequency).arg(modulationName).arg(voltageName)
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: selected = !selected
        }
    }

    ScrollBar.vertical: ScrollBar {
        visible: view.count > 0
        orientation: Qt.Vertical
        active: true
        padding: 0

        background: Image {
            source: "qrc:/images/playlist-scroll-bg.png"
        }

        contentItem: Image {
            source: "qrc:/images/playlist-scroll-handle.png"
        }
    }
}
