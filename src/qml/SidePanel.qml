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

    Button {
        id: turnTableButton

        width: parent.width; height: parent.height / 2
        pressedColor: "gray"
        pressedColorOpacity: turnTableButtonPressed ? 1.0 : 0.3

        Text {
            anchors.centerIn: parent
            rotation: 270
            text: "    Turntable"
            color: "white"
            font.pixelSize: 20
        }

        Image {
            width: 44; height: 44
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom; anchors.bottomMargin: 0

            source: sidepanel.turnTableLedOn ? "ledon.png" : "ledoff.png"
        }

        onPressed: sidepanel.turnTableClicked()
    }

    Button {
        id: drumMachineButton

        width: parent.width; height: parent.height / 2
        y: parent.height / 2
        pressedColor: "gray"
        pressedColorOpacity: drumMachineButtonPressed ? 1.0 : 0.3

        Text {
            anchors.centerIn: parent
            rotation: 270
            text: "    Drum Machine"
            color: "white"
            font.pixelSize: 20
        }

        Image {
            width: 44; height: 44
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom; anchors.bottomMargin: 0
            source: sidepanel.drumMachineLedOn ? "ledon.png" : "ledoff.png"
        }

        onPressed: sidepanel.drumMachineClicked()
    }
}
