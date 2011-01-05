import Qt 4.7

Image {
    id: drumMachine
    objectName: "drumMachine"   // used to identify this element in Qt side

    signal startBeat()
    signal stopBeat()
    signal drumButtonToggled(variant tick, variant sample, variant pressed)
    signal setBeat(variant index)
    signal drumMachineSpeed(variant speed)

    signal infoPressed()

    function setDrumButton(tick, sample, pressed) {
        drumGrid.children[tick + 32 * sample].pressed = pressed
    }

    function highlightTick(tick) {
        if(tick % 8) { ledOn = false }
        else { ledOn = true }

        if(tick < 16) { tickGroupSelector.selectedTickGroup = 1 }
        else { tickGroupSelector.selectedTickGroup = 2 }

        highligher.x = drumGrid.children[tick].x
    }

    property bool ledOn: false
    property alias running: powerbutton.pressed
    property alias selectedTickGroup: tickGroupSelector.selectedTickGroup
    property real speed

    onSpeedChanged: drumMachineSpeed(speed)

    width: 600; height: 360
    source: "../images/turntable.png"

    Text {
        anchors { right: tickGroupSelector.left; rightMargin: 15; top: tickGroupSelector.top; topMargin: 10 }
        text: "Ticks"
        color: "#505050"
        font.pixelSize: 10
    }

    TickGroupSelector {
        id: tickGroupSelector

        anchors { top: parent.top; left: drumFlickable.left; right: buttonPanel.left; topMargin: 5 }
        anchors { bottom: buttonPanel.bottom; bottomMargin: 5 }

    }

    Item {
        id: buttonPanel

        anchors { right: parent.right; top: parent.top }
        width:  parent.width * 0.23; height: parent.height / 6

        Image {
            id: closeButton

            property bool pressed: false

            anchors { left: parent.horizontalCenter; right: parent.right }
            anchors { top: parent.top; bottom: parent.bottom; margins: 5 }

            source: pressed ? "../images/exit_on.png" : "../images/exit.png"
            smooth: true

            MouseArea {
                anchors.fill: parent
                onPressed: { closeButton.pressed = true; closeButton.scale = 0.9 }
                onReleased: { closeButton.pressed = false; closeButton.scale = 1.0 }
                onClicked: Qt.quit()
            }
        }

        Image {
            id: infoButton

            property bool pressed: false

            anchors { left: parent.left; right: parent.horizontalCenter }
            anchors { top: parent.top; bottom: parent.bottom; margins: 5 }

            source: pressed ? "../images/info_on.png" : "../images/info.png"
            smooth: true

            MouseArea {
                anchors.fill: parent
                onPressed: { infoButton.pressed = true; infoButton.scale = 0.9 }
                onReleased: { infoButton.pressed = false; infoButton.scale = 1.00 }
                onClicked: drumMachine.infoPressed()
            }
        }
    }


    Column {
        id: sampleIcons
        anchors { left: parent.left; leftMargin: 6; top: tickGroupSelector.bottom; topMargin: 2; bottom: controlButtons.top }
        width: height / 6; spacing: 3

        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_hihat.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_hihat_open.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_kick.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_snare.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_crash.png"; smooth: true }
        Image { width: sampleIcons.width; height: drumGrid.drumButtonHeight; source: "../images/dr_icon_cowbell.png"; smooth: true }
    }

    Flickable {
        id: drumFlickable

        anchors { top: sampleIcons.top; bottom: sampleIcons.bottom }
        anchors { left: sampleIcons.right; leftMargin: 5; right: parent.right }

        contentWidth: drumGrid.width
        interactive: false
        clip: true

        Grid {
            id: drumGrid

            property real drumButtonWidth: drumFlickable.width / 16
            property real drumButtonHeight: drumFlickable.height / 6

            columns: 32; rows: 6

            Repeater {
                model: drumGrid.columns * drumGrid.rows

                DrumButton {
                    width: drumGrid.drumButtonWidth; height: drumGrid.drumButtonHeight
                    unselectedSource: "../images/drumbutton.png"
                    selectedSource: "../images/drumbuttonselected.png"
                    tick: index % 32
                    sample: Math.floor(index / 32)
                    onButtonToggled: drumMachine.drumButtonToggled(tick, sample, pressed)
                }
            }
        }

        Rectangle {
            // Hides the sometimes visible column of drumbuttons when in state "Ticks1".
            // When moving to state "Ticks2" this rectangle is hidded.
            id: hackAround
            color: "#999999"

            x: drumGrid.children[16].x
            width: drumGrid.drumButtonWidth; height: drumFlickable.height
        }

        Rectangle {
            id: highligher

            width: drumGrid.drumButtonWidth; height: drumGrid.height
            opacity: 0.3; color: "red"
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
                PropertyChanges { target: hackAround; visible: false }
                PropertyChanges { target: drumFlickable; contentX: drumGrid.children[16].x }
            }
        ]

        transitions: Transition {
            from: "Ticks1"
            to: "Ticks2"
            reversible: true
            SequentialAnimation {
                PropertyAction { target: hackAround; property: "visible" }
                PropertyAnimation { property: "contentX"; easing.type: Easing.InOutQuart }
            }
        }
    }

    Item {
        id: controlButtons

        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors { leftMargin: 10; rightMargin: 10; bottomMargin: 3 }
        height: parent.height / 7

        BeatSelector {
            id: beatSelector

            anchors { left: parent.left; right: powerButtonArea.left; rightMargin: 30 }
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

                anchors { right: parent.right; rightMargin: 10 }
                width: beatSelector.buttonWidth; height: width
                buttonCenterImage: "../images/powerbutton.png"
                glowColor: pressed ? "#AA00FF00" : "#AAFF0000"

                onPressedChanged: pressed ? drumMachine.startBeat() : drumMachine.stopBeat()
                onClicked: pressed = !pressed
            }
        }
    }
}
