import QtQuick 2.6
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

ColumnLayout {
    spacing: 10

    SatelliteLnbSettings {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    SatelliteChannelsSettings {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    JokerSatellitesModel {
        id: jkSatellitesModel
        settingsStorage: jkSettingsStorage
    }

    JokerChannelsModel {
        id: jkChannelsModel
        settingsStorage: jkSettingsStorage
    }

    JokerChannelsProxyModel {
        id: jkChannelsProxyModel
        sourceModel: jkChannelsModel
        signalSource: JokerTelevision.SatelliteSource
    }

    Component.onDestruction: jkChannelsProxyModel.channelName = "";
}
