import QtQuick 2.6
import QtQuick.Controls 2.2

import tv.jokersys.com 1.0

Item {
    property bool settingsActivated;

    height: 60

    Item {
        visible: (jkProgramProxyModel.permissions === JokerProgramProxyModel.AllPermissions)
        Image {
            x: 29; y: 14;
            source: "qrc:/images/all.png"
        }

        Image {
            x: 53; y: 17;
            source: "qrc:/images/arrow-down.png"
        }
    }

    BasicToolButton {
        id: satelliteButton
        x: 153; y: 10;
        checkable: true
        picture: checked ? "qrc:/images/satellite-on.png" : "qrc:/images/satellite-off.png"
        hint: qsTr("Show only satellite channels")
        onCheckedChanged: {
            if (checked)
                jkProgramProxyModel.permissions |= JokerProgramProxyModel.SatellitePermission;
            else
                jkProgramProxyModel.permissions &= ~JokerProgramProxyModel.SatellitePermission;
        }
    }

    BasicToolButton {
        id: terrstrialButton
        x: 193; y: 12;
        checkable: true
        picture: checked ? "qrc:/images/antenna-on.png" : "qrc:/images/antenna-off.png"
        hint: qsTr("Show only terrstrial channels")
        onCheckedChanged: {
            if (checked)
                jkProgramProxyModel.permissions |= JokerProgramProxyModel.TerrestrialPermission;
            else
                jkProgramProxyModel.permissions &= ~JokerProgramProxyModel.TerrestrialPermission;
        }
    }

    BasicToolButton {
        x: 230; y: 10;
        picture: "qrc:/images/settings.png"
        hint: qsTr("Show settings")
        onClicked: settingsActivated = !settingsActivated;
    }

    Component.onCompleted: {
        satelliteButton.checked = (jkProgramProxyModel.permissions & JokerProgramProxyModel.SatellitePermission);
        terrstrialButton.checked = (jkProgramProxyModel.permissions & JokerProgramProxyModel.TerrestrialPermission);
    }
}
