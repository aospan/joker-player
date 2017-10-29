import QtQuick 2.6
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

ColumnLayout {
    spacing: 10

    TerrestrialLnbSettings {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    TerrestrialAntennaSettings {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    TerrestrialChannelsSettings {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    JokerStandardsModel {
        id: jkStandardsModel
        settingsStorage: jkSettingsStorage
    }

    JokerStandardsProxyModel {
        id: jkStandardsProxyModel
        sourceModel: jkStandardsModel
    }

    JokerChannelsModel {
        id: jkChannelsModel
        settingsStorage: jkSettingsStorage
    }

    JokerChannelsProxyModel {
        id: jkChannelsProxyModel
        sourceModel: jkChannelsModel
        signalSource: JokerTelevision.TerrestrialSource
    }

    Component.onDestruction: jkStandardsProxyModel.choosingFlags =
                             JokerStandardsProxyModel.UnspecifiedStandardFlag
}
