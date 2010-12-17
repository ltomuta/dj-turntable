import Qt 4.7

Rectangle {
    id: drumMachine
    objectName: "drumMachine"   // used to identify this element in Qt side

    signal startBeat()
    signal stopBeat()
    signal drumButtonToggled(variant tick, variant sample, variant pressed)
    signal setBeat(variant index)

    // Public slots
    function createDrumButtons() {
        drumGrid.columns = 33; drumGrid.rows = 6

        for(var row=0; row<6; row++) {
            for(var col=0; col<33; col++) {
                var button = Qt.createComponent("DrumButton.qml").createObject(drumGrid)

                if(col != 16) {
                    // The column 16 is blank column, that column was made to leave
                    // a empty column to the breaking point of two tick groups
                    if(col < 15) {
                        button.tick = col
                    }
                    else {
                        button.tick = col - 1
                    }
                    button.sample = row
                    button.width = drumGrid.drumButtonWidth
                    button.height = drumGrid.drumButtonHeight

                    button.selectedSource = "drumbuttonselected.png"
                    button.unselectedSource = "drumbutton.png"
                }
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
        var offset = sample + Math.floor(tick / 16)
        drumGrid.children[tick + 32 * sample + offset].pressed = pressed
    }

    function highlightTick(tick) {
        if(tick % 8) {
            ledOn = false
        }
        else {
            ledOn = true
        }

        if(tick < 16) {
            tickGroupSelector.selectedTickGroup = 1
        }
        else {
            tickGroupSelector.selectedTickGroup = 2
        }

        highligher.x = drumGrid.width / 33 * (tick + Math.floor(tick / 16))
    }

    property bool ledOn: false
    property alias selectedTickGroup: tickGroupSelector.selectedTickGroup

    width: 600; height: 360
    color: "black"
    Component.onCompleted: { createDrumButtons() }

    TickGroupSelector {
        id: tickGroupSelector

        anchors.top: parent.top; anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: 400; height: 40
    }

    Flickable {
        id: drumFlickable

        anchors.top: tickGroupSelector.bottom; anchors.topMargin: 10
        anchors.left: parent.left; anchors.leftMargin: 10
        anchors.right: parent.right; anchors.rightMargin: 10
        anchors.bottom: controlButtons.top; anchors.bottomMargin: 10

        contentWidth: drumGrid.width
        interactive: false

        Grid {
            // Holds dynamically created DrumButtons childern
            id: drumGrid

            property real drumButtonWidth: drumFlickable.width / 16
            property real drumButtonHeight: drumFlickable.height / 6

            spacing: 0
        }

        Rectangle {
            id: highligher

            width: 37; height: drumGrid.height
            opacity: 0.3
            color: "red"
        }

        states: [
            State {
                name: "Ticks1"
                when: tickGroupSelector.selectedTickGroup == 1
                PropertyChanges { target: drumFlickable; contentX: 0 }

            },
            State {
                name: "Ticks2"
                when: tickGroupSelector.selectedTickGroup == 2
                PropertyChanges { target: drumFlickable; contentX: drumGrid.drumButtonWidth * 17 }

            }
        ]

        transitions: Transition {
            PropertyAnimation { property: "contentX"; easing.type: Easing.InOutQuart }
        }
    }

    Row {
        id: controlButtons

        property real arrowbuttonwidth: width / 4

        spacing: 20
        anchors.left: parent.left; anchors.leftMargin: 10
        anchors.right: parent.right; anchors.rightMargin: 10
        anchors.bottom: parent.bottom; anchors.bottomMargin: 10
        height: 40

        SlideSwitch {
            width: parent.width / 2; height: parent.height
            onOnChanged: {
                if(on) {
                    startBeat()
                }
                else {
                    stopBeat()
                    ledOn = false
                }
            }
        }

        BeatSelector {
            width:  parent.width / 2 - 20; height: parent.height
            onIndexChanged: drumMachine.setBeat(index)
        }
    }
}
