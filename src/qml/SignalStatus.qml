import QtQuick 2.6
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

Image {
    source: "qrc:/images/signal-status-pane-bg.png"

    RowLayout {
        anchors.centerIn: parent
        spacing: 52
        opacity: jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveringStatus

        Image {
            source: (jkAccessProvider.signalLocked)
                    ? "qrc:/images/signal-status-green.png"
                    : "qrc:/images/signal-status-red.png"
        }

        BasicLabel {
            id: currentCcanningChannelInfo
            color: "#d2d2d2"
            font.weight: Font.Normal
        }

        Connections {
            target: jkAccessProvider
            onScanningChannelChanged: {
                currentCcanningChannelInfo.text = qsTr("%1: %2 MHz")
                    .arg(standardName).arg(frequencyMhz);
            }
        }

        BasicLabel {
            function decodeSignalQualityName(quality) {
                switch (quality) {
                case JokerAccessProvider.BadSignalQuality:
                    return qsTr("Bad");
                case JokerAccessProvider.WeakSignalQuality:
                    return qsTr("Weak");
                case JokerAccessProvider.GoodSignalQuality:
                    return qsTr("Good");
                default:
                    return qsTr("Unknow");
                }
            }

            text: qsTr("Guality: %1").arg(decodeSignalQualityName(jkAccessProvider.signalQuality))
            color: "#d2d2d2"
            font.weight: Font.Normal
        }

        BasicLabel {
            text: qsTr("RF Level: %1dBm C/N: %2 dB Errors: %3")
                    .arg(jkAccessProvider.signalRadioFrequencyLevel)
                    .arg(jkAccessProvider.signalCarrierNoiseRatio)
                    .arg(jkAccessProvider.signalErrorsCount)
            color: "#d2d2d2"
            font.weight: Font.Normal
        }
    }
}
