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
        aspectRatio: jkPlayer.program.aspectRatio

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Calculate next aspect ratio
                var aspectRatio = parent.aspectRatio;
                if (aspectRatio === Vlc.Original)
                    aspectRatio = Vlc.R_4_3;
                else if (aspectRatio === Vlc.R_4_3)
                    aspectRatio = Vlc.R_16_9;
                else if (aspectRatio === Vlc.R_16_9)
                    aspectRatio = Vlc.Original;

                jkPlayer.updateProgramAspectRatio(aspectRatio);
            }
        }
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
