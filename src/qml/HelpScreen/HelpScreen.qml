import Qt 4.7

Rectangle {
    width: 500; height: 360

    radius: 15
    color: "gray"

    Text {
        id: freesoundlicense

        color: "white"
        anchors.fill: parent; anchors.margins: 20
        wrapMode: Text.WordWrap
        text: "Samples\nThe main turntable sample was created by nick Flick3r and it was downloaded from freesound.org, the sample follows the Creative Commons Sampling Plus 1.0 license, see http://creativecommons.org/licenses/sampling+/1.0/ for more information."
    }
}
