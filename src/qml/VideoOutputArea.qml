import QtQuick 2.6
import QtQuick.Layouts 1.3

ColumnLayout {
    spacing: 0

    VideoOutput {
        id: videoOutput
        Layout.fillWidth: true
        Layout.fillHeight: true

        Shortcut {
            sequence: "F"
            onActivated: videoOutput.fullScreen = !videoOutput.fullScreen
        }
        Shortcut {
            sequence: "Esc"
            onActivated: videoOutput.fullScreen = false
        }
    }

    ControlArea {
        Layout.fillWidth: true
        onFullScreenRequested: videoOutput.fullScreen = true
    }
}
