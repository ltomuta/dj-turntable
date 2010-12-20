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
    property alias running: powerbutton.on
    property alias selectedTickGroup: tickGroupSelector.selectedTickGroup

    width: 600; height: 360
    Component.onCompleted: { createDrumButtons() }


    TickGroupSelector {
        id: tickGroupSelector

        anchors.top: parent.top; anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200; height: 40
    }

    Rectangle {
        color: "gray"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tickGroupSelector.bottom
        anchors.bottom: parent.bottom
    }

    Column {
        id: sampleIcons
        anchors.left: parent.left; anchors.leftMargin: 6
        anchors.top: tickGroupSelector.bottom; anchors.topMargin: 10
        anchors.bottom: controlButtons.top

        width: 30
        spacing: 0

        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "HH" } }
        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "HHo" } }
        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "Bs" } }
        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "Sn" } }
        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "Cr" } }
        Item { width: sampleIcons.width; height: drumGrid.drumButtonHeight; Text { anchors.centerIn: parent; color: "white"; text: "Cb" } }
    }

    Flickable {
        id: drumFlickable

        anchors.top: tickGroupSelector.bottom; anchors.topMargin: 10
        anchors.left: sampleIcons.right; anchors.leftMargin: 5
        anchors.right: parent.right; anchors.rightMargin: 10
        anchors.bottom: controlButtons.top; anchors.bottomMargin: 10

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

        property real arrowbuttonwidth: width / 4

        anchors.left: parent.left; anchors.leftMargin: 10
        anchors.right: parent.right; anchors.rightMargin: 10
        anchors.bottom: parent.bottom; anchors.bottomMargin: 5
        height: 40

        Text {
            id: patternText
            anchors.right: beatSelector.left; anchors.rightMargin: -10
            anchors.top: beatSelector.top; anchors.topMargin: -12
            text: "Pattern"
            color: "#505050"
            font.pixelSize: 10
        }

        BeatSelector {
            id: beatSelector

            anchors.left: parent.left; anchors.leftMargin: 30
            anchors.right: powerbutton.left; anchors.rightMargin: 30
            height: parent.height
            onIndexChanged: drumMachine.setBeat(index)
        }

        Text {
            anchors.right: powerbutton.left; anchors.rightMargin: -5
            anchors.top: patternText.top
            text: "Power"
            color: "#505050"
            font.pixelSize: 10
        }

        Item {
            id: powerbutton

            property bool on: false

            width: parent.height; height: parent.height
            anchors.right: parent.right; anchors.rightMargin: 10

            onOnChanged: on ? drumMachine.startBeat() : drumMachine.stopBeat()

            Rectangle {
                anchors.fill: parent; anchors.margins: 10
                color: powerbutton.on ? "#AA00FF00" : "#AAFF0000"
            }

            Image {
                anchors.fill: parent
                source: "../powerbutton.png"
                smooth: true
            }

            MouseArea {
                anchors.fill: parent
                onPressed: powerbutton.scale = 0.95
                onReleased: powerbutton.scale = 1.00
                onClicked: powerbutton.on = !powerbutton.on
            }
        }
    }
}
