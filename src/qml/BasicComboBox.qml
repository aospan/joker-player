import QtQuick 2.6
import QtQuick.Controls 2.2

ComboBox {
    id: control
    padding: 0

//    delegate: ItemDelegate {
//        width: control.width
//        contentItem: BasicLabel {
//            text: modelData
//            color: "black"
//            verticalAlignment: Text.AlignVCenter
//        }
//        highlighted: control.highlightedIndex === index
//    }

    indicator: Image {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 8
        source: "qrc:/images/arrow-down-small.png"
    }

    contentItem: BasicLabel {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 7
        text: control.displayText
        color: "#ffffff"
    }

    background: Item {
        implicitWidth: 106
        implicitHeight: 21

        Image {
            anchors.centerIn: parent
            source: "qrc:/images/combo-box-bg.png"
        }
    }
}
