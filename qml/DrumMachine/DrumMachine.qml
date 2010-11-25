import Qt 4.7
import "../"

Rectangle {
    id: drumMachine
    objectName: "drumMachine"

    signal startBeat()
    signal stopBeat()
    signal drumButtonToggled(variant tick, variant sample, variant pressed)
    signal setDemoBeat(variant index)

    // Public slots
    function maxSeqAndSamples(ticks, samples) {
        var count = drumGrid.children.length
        for(var i=0;i<count;i++) {
            drumGrid.children[i].destroy()
        }

        drumGrid.columns = ticks; drumGrid.rows = samples

        for(var row=0; row<samples; row++) {
            for(var col=0; col<ticks; col++) {
                var button = Qt.createComponent("DrumButton.qml").createObject(drumGrid)
                if((col % 4) == 0) {
                    button.notPressedColor = "#505050"
                }
                else {
                    button.notPressedColor = "#303030"
                }
                button.pressedColor = "white"
                button.tick = col
                button.sample = row
            }
        }

        maxTicks = ticks
        maxSamples = samples
    }

    function seqSize(ticks, samples) {
        drumGrid.columns = ticks

        var count = drumGrid.children.length
        var column = 0

        for(var i=0;i<count;i++) {
            column = i % maxTicks
            if(column >= ticks) {
                drumGrid.children[i].visible = false
            }
            else {
                drumGrid.children[i].visible = true
            }
        }
    }

    function clearDrumButtons() {
        var count = drumGrid.rows * drumGrid.columns
        for(var i=0; i<count; i++) {
            drumGrid.children[i].pressed = false
        }
    }

    function setDrumButton(tick, sample, pressed) {
        drumGrid.children[tick + drumGrid.columns * sample].pressed = pressed
    }

    function highlightTick(tick) {
        highligher.x = tick * 38 - 3
    }

    property int maxTicks: 0
    property int maxSamples: 0

    width: 580; height: 360
    color: "black"

    Column {
        anchors.fill: parent
        spacing: 10

        Text {
            id: titleText

            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: 30; font.bold: true
            color: "white"
            text: "Drum Machine"
        }

        Flickable {
            id: drumFlickable

            width: parent.width; height: drumGrid.height
            contentWidth: drumGrid.width

            Grid {
                // Holds dynamically created DrumButtons childern
                id: drumGrid

                spacing: 8
            }

            Rectangle {
                id: highligher

                width: 37
                height: drumGrid.height
                opacity: 0.3
                color: "red"
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

                onIndexChanged: {
                    drumMachine.clearDrumButtons()
                    drumMachine.setDemoBeat(index)
                }

                Text {
                    anchors.centerIn: parent
                    font.bold: true; font.pixelSize: 20
                    text: drumIndex.index
                    color: "white"
                }
            }

            Button {
                id: beatup; width: beatselector.arrowbuttonwidth - beatselector.spacing; height: beatselector.height; pressedColor: "gray"
                Text {
                    anchors.centerIn: parent
                    font.bold: true; font.pixelSize: 20
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
