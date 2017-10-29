import QtQuick 2.6
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

Image {
    source: "qrc:/images/satellite-lnb-settings-bg.png"

    BasicLabel {
        x: 10; y: 9
        text: qsTr("LNB settings")
        font.weight: Font.Medium
    }

    GridLayout {
        anchors.centerIn: parent
        columns: 4
        rowSpacing: 20
        columnSpacing: 60

        BasicCheckBox {
            text: qsTr("Tone 22kHz")
            checked: (jkSettingsStorage.tone === JokerTelevision.ToneOn)
            onCheckedChanged: jkSettingsStorage.tone = (checked)
                              ? JokerTelevision.ToneOn
                              : JokerTelevision.ToneOff
        }

        BasicLabel {
            text: qsTr("Low frequency")
        }

        BasicLabel {
            text: qsTr("High frequency")
        }

        BasicLabel {
            text: qsTr("Switch frequency")
        }

        BasicComboBox {
            id: presetsBox
            model: [ qsTr("Universal"), qsTr("L10750"), qsTr("L10700"), qsTr("Manual") ]
            currentIndex: jkSettingsStorage.presetIndex
            onCurrentIndexChanged: jkSettingsStorage.presetIndex = currentIndex
        }

        BasicLineEdit {
            id: lowFrequencyEdit

            function currentLowFrequency() {
                var preset = jkSettingsStorage.preset;
                return preset.lowFrequency;
            }

            placeholderText: qsTr("Enter frequency")
            hint: qsTr("Low frequency")
            readOnly: (presetsBox.currentIndex !== JokerSettingsStorage.ManualPresetIndex)
            text: lowFrequencyEdit.currentLowFrequency();
            validator: IntValidator {
                bottom: 0;
                top: 100000; // 100 GHz
            }
            onEditingFinished: {
                if (!readOnly) {
                    var preset = jkSettingsStorage.preset;
                    preset.lowFrequency = text;
                    jkSettingsStorage.preset = preset;
                }
            }
            Connections {
                target: jkSettingsStorage
                onPresetIndexChanged: lowFrequencyEdit.text = lowFrequencyEdit.currentLowFrequency();
            }
        }

        BasicLineEdit {
            id: highFrequencyEdit

            function currentHighFrequency() {
                var preset = jkSettingsStorage.preset;
                return preset.highFrequency;
            }

            placeholderText: qsTr("Enter frequency")
            hint: qsTr("High frequency")
            readOnly: (presetsBox.currentIndex !== JokerSettingsStorage.ManualPresetIndex)
            text: highFrequencyEdit.currentHighFrequency();
            validator: IntValidator {
                bottom: 0;
                top: 100000; // 100 GHz
            }
            onEditingFinished: {
                if (!readOnly) {
                    var preset = jkSettingsStorage.preset;
                    preset.highFrequency = text;
                    jkSettingsStorage.preset = preset;
                }
            }
            Connections {
                target: jkSettingsStorage
                onPresetIndexChanged: highFrequencyEdit.text = highFrequencyEdit.currentHighFrequency();
            }
        }

        BasicLineEdit {
            id: switchFrequencyEdit

            function currentSwitchFrequency() {
                var preset = jkSettingsStorage.preset;
                return preset.switchFrequency;
            }

            placeholderText: qsTr("Enter frequency")
            hint: qsTr("Switch frequency")
            readOnly: (presetsBox.currentIndex !== JokerSettingsStorage.ManualPresetIndex)
            text: switchFrequencyEdit.currentSwitchFrequency();
            validator: IntValidator {
                bottom: 0;
                top: 100000; // 100 GHz
            }
            onEditingFinished: {
                if (!readOnly) {
                    var preset = jkSettingsStorage.preset;
                    preset.switchFrequency = text;
                    jkSettingsStorage.preset = preset;
                }
            }
            Connections {
                target: jkSettingsStorage
                onPresetIndexChanged: switchFrequencyEdit.text = switchFrequencyEdit.currentSwitchFrequency();
            }
        }
    }
}
