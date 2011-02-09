import Qt 4.7

Rectangle {
    id: sidepanel

    signal turnTableClicked()
    signal drumMachineClicked()
    signal sampleSelectorClicked()

    property bool sampleSelectorButtonPressed: false
    property bool turnTableButtonPressed: false
    property bool drumMachineButtonPressed: false
    property bool turnTableLedOn: false
    property bool drumMachineLedOn: false

    color: "black"
    width: 100; height: 400

    BorderImage {
        id: settingsButton

        anchors {
            left: parent.left
            right: parent.right; rightMargin: 2
        }
        height: width
        source: sidepanel.sampleSelectorButtonPressed
                    ? "images/buttonpressed.sci"
                    : "images/buttonup.sci"

        Image {
            anchors.centerIn: parent
            width: parent.width * 0.7 ; height: width
            source: "images/iconsampleselector.png"
            smooth: true
        }

        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.sampleSelectorClicked()
        }
    }

    Image {
        id: turnTableButton

        anchors {
            left: parent.left
            right: parent.right; rightMargin: 2
            top: settingsButton.bottom; topMargin: 2
        }

        height: (parent.height - settingsButton.height) / 2 - 1


        smooth: true
        source: {
            if(turnTableButtonPressed) {
                if(turnTableLedOn) { return "images/turntable_on_play.png" }
                else {return "images/turntable_on_noplay.png" }
            }
            else {
                if(turnTableLedOn) { return "images/turntable_off_play.png" }
                else { return "images/turntable_off_noplay.png" }
            }
        }


        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.turnTableClicked()
        }
    }

    Image {
        id: drumMachineButton

        anchors {
            left: parent.left
            right: parent.right; rightMargin: 2
            top: turnTableButton.bottom; topMargin: 2
            bottom: parent.bottom
        }

        smooth: true

        source: {
            if(drumMachineButtonPressed) {
                if(drumMachineLedOn) {
                    return "images/drummachine_on_play.png"
                }
                else {
                    return "images/drummachine_on_noplay.png"
                }
            }
            else {
                if(drumMachineLedOn) {
                    return "images/drummachine_off_play.png"
                }
                else {
                    return "images/drummachine_off_noplay.png"
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.drumMachineClicked()
        }
    }
}
