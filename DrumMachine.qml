import Qt 4.7

Rectangle {
    width: 800
    height: 480

    color: "black"

    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: "Drum Machine"

        font.pixelSize: 40
        font.bold: true
        color: "white"
    }

    Row {
        id: beatselector

        property real arrowbuttonwidth: width / 4

        anchors.top: text.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 20

        height: parent.height / 3

        Button {
            id: beatdown; width: beatselector.arrowbuttonwidth - beatselector.spacing; height: beatselector.height; pressedColor: "gray"
            Text {
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
                text: "<"
                color: "white"
            }
            onClicked: {
                if(drumIndex.index > 0)
                    drumIndex.index -= 1
            }
        }

        Button {
            id: drumIndex; width: beatselector.arrowbuttonwidth * 2; height: beatselector.height; pressedColor: "lightgray"

            property int index: 0

            onIndexChanged: ui.toggleBeat(index)

            Text {
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
                text: drumIndex.index
                color: "white"
            }

        }

        Button {
            id: beatup; width: beatselector.arrowbuttonwidth - beatselector.spacing; height: beatselector.height; pressedColor: "gray"
            Text {
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
                text: ">"
                color: "white"
            }
            onClicked: {
                if(drumIndex.index < 3)
                    drumIndex.index += 1
            }
        }
    }

    Row {
        id: buttons

        spacing: 20

        anchors.top: beatselector.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20

        Button {
            id: startBeat
            width: buttons.width / 2 - buttons.spacing; height: buttons.height; pressedColor: "gray"; pressedColorOpacity: 0.3

            Text {
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
                text: "Start"
                color: "white"
            }

            onClicked: {
                ui.startBeat()
                startBeat.pressedColorOpacity = 1.0
                stopBeat.pressedColorOpacity = 0.3
            }
        }
        Button {
            id: stopBeat

            width: buttons.width / 2; height: buttons.height; pressedColor: "gray"; pressedColorOpacity: 1.0

            Text {
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
                text: "Stop"
                color: "white"
            }

            onClicked: {
                ui.stopBeat()
                startBeat.pressedColorOpacity = 0.3
                stopBeat.pressedColorOpacity = 1.0
            }
        }
    }
}
