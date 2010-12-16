import Qt 4.7

Rectangle {
    id: drumMachine
    objectName: "drumMachine"   // used to identify this element in Qt side

    signal startBeat()
    signal stopBeat()
    signal drumButtonToggled(variant tick, variant sample, variant pressed)
    signal setBeat(variant index)

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
                if((col % 4) != 0) {
                    button.opacity = 0.8
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
        highligher.x = tick * 34 - 3
    }

    property int maxTicks: 0
    property int maxSamples: 0

    width: 600; height: 360

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
                // Holds dynamically created DrumButtons childern
                id: drumGrid

                spacing: 0
            }

            Rectangle {
                id: highligher

                width: 37; height: drumGrid.height
                opacity: 0.3
                color: "red"
            }
        }

        Row {
            id: controlButtons

            property real arrowbuttonwidth: width / 4

            spacing: 20
            anchors.left: parent.left; anchors.leftMargin: 10
            anchors.right: parent.right; anchors.rightMargin: 10
            height: (parent.height - y) - parent.spacing

            SlideSwitch {
                width: parent.width / 2; height: parent.height
                onOnChanged: on ? drumMachine.startBeat() : drumMachine.stopBeat()
            }

            BeatSelector {
                width:  parent.width / 2 - 20; height: parent.height
                onIndexChanged: drumMachine.setBeat(index)
            }
        }
    }
}
