import QtQuick 2.6

import VLCQt 1.1

Rectangle {
    id: videoOutput
    color: "black"

    property bool fullScreen;
    onFullScreenChanged: videoOutput.state = (fullScreen) ? "expanded" : ""

    VlcVideoOutput {
        anchors.fill: parent
        source: jkPlayer
    }

    states: State {
        name: "expanded"
        ParentChange {
            target: videoOutput;
            parent: dummyVideoOutput;
            x: dummyVideoOutput.x;
            y: dummyVideoOutput.y;
            width: dummyVideoOutput.width
            height: dummyVideoOutput.height
        }
    }
}
