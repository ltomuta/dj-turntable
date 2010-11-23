import Qt 4.7
import "../"

Rectangle {
    id: drumMachine

    signal startBeat()
    signal stopBeat()
    signal drumButtonToggled(int index, bool pressed)
    signal toggleBeat(int index)

    function destroyDrumButtons() {
        var count = drumGrid.children.length
        for(var i=0;i<count;i++)
            drumGrid.children[i].destroy()
    }

    function createDrumButtons(columns, rows) {
        destroyDrumButtons()
        drumGrid.columns = columns; drumGrid.rows = rows
        for(var i=0; i<drumGrid.rows * drumGrid.columns; i++) {
            var button = Qt.createComponent("DrumButton.qml").createObject(drumGrid)
            button.index = i
        }
    }

    function setDrumButton(index, pressed) {
        drumGrid.children[index].pressed = pressed
    }

    width: 760; height: 480
    color: "black"

    Column {
        anchors.fill: parent
        spacing: 10

        Text {
            id: titleText

            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: 40; font.bold: true
            color: "white"
            text: "Drum Machine"
        }

        Flickable {
            id: drumFlickable

            width: parent.width; height: drumGrid.height
            contentWidth: drumGrid.width

            Grid {
                id: drumGrid
                spacing: 8
            }
        }

        Row {
            id: beatselector

            property real arrowbuttonwidth: width / 4

            spacing: 20
            width: parent.width
            height: (parent.height - y) / 2 - parent.spacing

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

                onIndexChanged: drumMachine.toggleBeat(index)

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

            width: parent.width
            height: beatselector.height
            spacing: 20

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
                    drumMachine.startBeat()
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
                    drumMachine.stopBeat()
                    startBeat.pressedColorOpacity = 0.3
                    stopBeat.pressedColorOpacity = 1.0
                }
            }
        }
    }
}
