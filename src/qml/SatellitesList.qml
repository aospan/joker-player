import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import tv.jokersys.com 1.0

RowLayout {
    property alias model: view.model
    property string satelliteName: (view.currentItem) ? view.currentItem.config.satelliteName : "";

    spacing: 0

    Rectangle {
        width: 330
        color: "#EBEBEB"

        ListView {
            id: view
            clip: true
            anchors.fill: parent
            highlightMoveDuration: 0
            interactive: false
            currentIndex: -1

            delegate: BasicLabel {
                x: 10
                height: 20
                color: ListView.isCurrentItem ? "#282828" : "#878787"
                font.weight: ListView.isCurrentItem ? Font.Medium : Font.Normal
                font.pixelSize: 12

                // TODO: It is a workaround to have an access to the model's
                // data outide of delegate, e.g. from the highlight item!
                property variant config: model

                text: satelliteName

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        view.currentIndex = index;
                        view.focus = true;
                    }
                }
            }
        }

        Layout.fillHeight: true
    }

    Slider {
        id: slider
        orientation: Qt.Vertical
        padding: 0
        from: 1.0
        to: 0.0
        onValueChanged: view.contentY = view.contentHeight * value;

        background: Image {
            source: "qrc:/images/channels-settings-scroll-bg.png"
            width: implicitWidth
        }

        handle: Item {
            width: 12
            height: 12
            y: slider.topPadding + slider.visualPosition * (slider.availableHeight - height)

            Image {
                anchors.centerIn: parent
                source: "qrc:/images/channels-settings-scroll-handle.png"
            }
        }

        Layout.fillHeight: true
    }
}
