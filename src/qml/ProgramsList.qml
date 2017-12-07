import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ListView {
    id: view
    clip: true
    currentIndex: -1
    maximumFlickVelocity: 100

    onCurrentIndexChanged: {
        if (currentIndex === -1)
            jkPlayer.setupProgram(); // default invalid program
        else
            jkPlayer.setupProgram(currentItem.program.program);
    }

    delegate: Item {
        width: view.width
        height: 33

        // TODO: It is a workaround to have an access to the model's
        // data outide of delegate, e.g. from the highlight item!
        property variant program: model

        Image {
            source: "qrc:/images/program-separator.png"
        }

        Image {
            visible: delegateMouseArea.containsMouse
            source: "qrc:/images/program-hovered-bg.png"

            Image {
                source: "qrc:/images/program-separator-hovered.png"
            }
        }

        Image {
            visible: (view.currentIndex === index)
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/images/program-highlight-bg.png"
            width: parent.width + 7

// TODO: Uncomment it when we will know about this property!
//            BasicLabel {
//                text: qsTr("12:35")
//                anchors.verticalCenter: parent.verticalCenter
//                x: 196
//                color: "#ffffff"
//                font.weight: Font.Normal
//            }

            Image {
                anchors.verticalCenter: parent.verticalCenter
                x: 230 // original is 245!
                source: "qrc:/images/run.png"
            }
        }

        MouseArea {
            id: delegateMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                view.currentIndex = (view.currentIndex === index) ? -1 : index;
                view.focus = true;
            }
        }

        RowLayout {
            id: programInfoLayout
            anchors.fill: parent
            anchors.leftMargin: (view.currentIndex === index) ? 10 : 5
            anchors.rightMargin: (view.currentIndex === index) ? 70 : 5
            spacing: 1

            Rectangle {
                height: programNumberTextMetric.height + 5
                width: programNumberTextMetric.width + 10
                color: programNumber.readOnly ? "transparent" : "#46545c"
                border.width: 2
                border.color: (programNumberMouseArea.containsMouse) ? "#f8f8f8" : "transparent"

                TextInput {
                    id: programNumber
                    padding: 0
                    anchors.fill: parent
                    font.family: "geometria"
                    font.pixelSize: 18
                    font.weight: (view.currentIndex === index) ? Font.Bold : Font.Normal
                    color: "#f8f8f8"
                    text: position
                    readOnly: true
                    horizontalAlignment: TextInput.AlignRight
                    verticalAlignment: TextInput.AlignVCenter

                    validator: IntValidator {
                        bottom: 1
                        top: 999
                    }

                    TextMetrics {
                        id: programNumberTextMetric
                        font: programNumber.font
                        text: "999" // template
                    }

                    MouseArea {
                        id: programNumberMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onDoubleClicked: {
                            programNumber.readOnly = false
                            programNumber.focus = true
                        }
                        onContainsMouseChanged: {
                            if (!containsMouse) {
                                if (!programNumber.readOnly)
                                    model.position = programNumber.text;

                                programNumber.readOnly = true;
                            }
                        }
                    }
                }
            }

            BasicLabel {
                font.pixelSize: 18
                font.weight: (view.currentIndex === index)
                             ? Font.Bold : Font.Normal
                color: "#f8f8f8"
                text: ". "
            }

            Rectangle {
                id: programNameArea
                clip: true
                height: programNameTextMetric.height + 5
                color: "transparent"
// TODO: It is for debugging purposes only!
// Need to remove in release, and to replace Rectangle with Item!
//                border.width: 2
//                border.color: "green"

                BasicLabel {
                    id: programName
                    text: description
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    color: "#f8f8f8"
                    font.pixelSize: 18
                    font.weight: (view.currentIndex === index)
                                 ? Font.Bold : Font.Normal
                    elide: (delegateMouseArea.containsMouse
                            || (view.currentIndex == index))
                           ? Text.ElideNone : Text.ElideRight

                    NumberAnimation on x {
                        loops: Animation.Infinite
                        from: programNameArea.width
                        to: -programNameTextMetric.width
                        duration: 5000
                        running: (delegateMouseArea.containsMouse
                                  || (view.currentIndex == index))
                        onRunningChanged: {
                            if (!running)
                                programName.x = 0
                        }
                    }
                }

                TextMetrics {
                    id: programNameTextMetric
                    font: programName.font
                    text: programName.text
                }

                Layout.fillWidth: true
            }
        }
    }
}
