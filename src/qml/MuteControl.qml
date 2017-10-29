import QtQuick 2.6

BasicToolButton {
    checkable: true
    picture: checked ? "qrc:/images/mute-off.png" : "qrc:/images/mute-on.png"
    hint: qsTr("Mute control")
    onCheckedChanged: jkPlayer.muted = checked
}
