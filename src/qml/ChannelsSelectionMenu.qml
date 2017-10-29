import QtQuick 2.6
import QtQuick.Controls 2.2

BasicToolButton {
    id: control
    hint: qsTr("Menu")
    onClicked: menu.open();

    BasicLabel {
        anchors.fill: parent
        text: "\u25bc"
    }

    Menu {
        id: menu
        y: control.height
        implicitWidth: 90

        MenuItem {
            implicitHeight: 20
            text: "Select All"
            onClicked: jkChannelsProxyModel.selectAllChannels();
        }
        MenuItem {
            implicitHeight: 20
            text: "Unselect All"
            onClicked: jkChannelsProxyModel.unselectAllChannels();
        }
    }
}
