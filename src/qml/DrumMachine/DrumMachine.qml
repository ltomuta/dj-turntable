import Qt 4.7

Item {
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
    property alias running: powerbutton.pressed
    property alias selectedTickGroup: tickGroupSelector.selectedTickGroup

    width: 600; height: 360
    Component.onCompleted: { createDrumButtons() }


    TickGroupSelector {
        id: tickGroupSelector

        anchors { top: parent.top; topMargin: 5; horizontalCenter: parent.horizontalCenter }
        width: parent.width / 2; height: parent.height / 7
    }

    Rectangle {
        color: "#999999"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tickGroupSelector.bottom; anchors.topMargin: -10
        anchors.bottom: parent.bottom
        radius: 4
    }

    Column {
        id: sampleIcons
        anchors.left: parent.left; anchors.leftMargin: 6
        anchors.top: tickGroupSelector.bottom; anchors.topMargin: 10
        anchors.bottom: controlButtons.top

        width: 30
        spacing: 3

        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_hihat.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_hihat_open.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_kick.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_snare.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_crash.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "dr_icon_cowbell.png"; smooth: true }
    }

    Flickable {
        id: drumFlickable

        anchors.top: tickGroupSelector.bottom; anchors.topMargin: 10
        anchors.left: sampleIcons.right; anchors.leftMargin: 5
        anchors.right: parent.right; anchors.rightMargin: 10
        anchors.bottom: controlButtons.top; anchors.bottomMargin: 3

        contentWidth: drumGrid.width
        interactive: false
        clip: true

        Grid {
            // Holds dynamically created DrumButtons childern
            id: drumGrid

            property real drumButtonWidth: drumFlickable.width / 16
            property real drumButtonHeight: drumFlickable.height / 6

            spacing: 0
        }

        Rectangle {
            id: highligher

            width: drumGrid.drumButtonWidth; height: drumGrid.height
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
                PropertyChanges { target: drumFlickable; contentX: drumFlickable.width + drumGrid.drumButtonWidth}
            }
        ]

        transitions: Transition {
            PropertyAnimation { property: "contentX"; easing.type: Easing.InOutQuart }
        }
    }

    Item {
        id: controlButtons

        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors { leftMargin: 10; rightMargin: 10; bottomMargin: 3 }
        height: parent.height / 7

        BeatSelector {
            id: beatSelector

            anchors.left: parent.left
            anchors.right: powerButtonArea.left; anchors.rightMargin: 30
            height: parent.height
            onIndexChanged: drumMachine.setBeat(index)
        }

        Item {
            id: powerButtonArea

            anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
            width: parent.width / 7

            Text {
                anchors.right: powerbutton.left
                text: "Power"
                color: "#505050"
                font.pixelSize: 10
            }

            ImageButton {
                id: powerbutton

                buttonCenterImage: "../powerbutton.png"
                width: beatSelector.buttonWidth; height: width
                anchors { right: parent.right; rightMargin: 10 }
                glowColor: pressed ? "#AA00FF00" : "#AAFF0000"

                onPressedChanged: pressed ? drumMachine.startBeat() : drumMachine.stopBeat()

                onClicked: pressed = !pressed
            }
        }
    }
}
