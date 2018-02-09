import QtQuick 2.6

BasicLabel {
    id: control
    horizontalAlignment: Text.AlignHCenter
    color: "#d9d9d9"

    Connections {
        target: jkPlayer
        onProgramChanged: {
            control.text = (program.id === -1)
                          ? "" : program.description
        }
    }
}
