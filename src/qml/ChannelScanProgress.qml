import QtQuick 2.6
import QtQuick.Controls 2.0

import tv.jokersys.com 1.0

ProgressBar {
    id: control
    value: jkAccessProvider.scanningProgress
    opacity: jkAccessProvider.status === JokerAccessProvider.ProgramsDiscoveringStatus

    background: Image {
        source: "qrc:/images/channels-scan-progress-bg.png"
    }

    contentItem: Item {
        Image {
            width: control.visualPosition * parent.width
            source: "qrc:/images/channels-scan-progress-fg.png"
        }

        BasicLabel {
            anchors.centerIn: parent
            text: {
                var percent = (control.value * 100.0) / control.to;
                var from = (jkAccessProvider.discoveryJobIndex < 0)
                        ? 0 : jkAccessProvider.discoveryJobIndex;
                var to = jkAccessProvider.discoveryJobsCount;
                return qsTr("%1% (%2 from %3 done)")
                    .arg(percent.toFixed(0)).arg(from).arg(to)

            }
            font.pixelSize: 12
            color: "#424242"
        }
    }

    ToolTip {
        visible: control.hovered
        text: qsTr("Channels scanning progress")
    }
}
