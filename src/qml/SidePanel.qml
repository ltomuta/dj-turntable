import Qt 4.7

Rectangle {
    id: sidepanel

    signal turnTableClicked()
    signal drumMachineClicked()

    property bool turnTableButtonPressed: false
    property bool drumMachineButtonPressed: false
    property bool turnTableLedOn: false
    property bool drumMachineLedOn: false

    width: 100; height: 400
    color: "black"

    Image {
        id: turnTableButton

        width: parent.width - 2; height: parent.height / 2 - 1
        smooth: true
        source: {
            if(turnTableButtonPressed) {
                if(turnTableLedOn) { return "turntable_on_play.png" }
                else {return "turntable_on_noplay.png" }
            }
            else {
                if(turnTableLedOn) { return "turntable_off_play.png" }
                else { return "turntable_off_noplay.png" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.turnTableClicked()
        }

    }

    Image {
        id: drumMachineButton

        width: parent.width - 2; height: parent.height / 2 - 1
        y: parent.height / 2 + 1
        smooth: true
        source: {
            if(drumMachineButtonPressed) {
                if(drumMachineLedOn) { return "drummachine_on_play.png" }
                else {return "drummachine_on_noplay.png" }
            }
            else {
                if(drumMachineLedOn) { return "drummachine_off_play.png" }
                else { return "drummachine_off_noplay.png" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.drumMachineClicked()
        }
    }
}
