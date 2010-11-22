import Qt 4.7

Rectangle {
    id: sidepanel

    signal turnTableClicked()
    signal drumMachineClicked()

    width: 100; height: 200
    color: "black"

    Button {
        id: turnTableButton

        width: parent.width; height: parent.height / 2
        pressedColor: "gray"
        pressedColorOpacity: 1.0

        Text {
            anchors.centerIn: parent
            rotation: 90
            text: "Turntable"
            color: "white"
            font.pixelSize: 20
        }

        onClicked: {
            pressedColorOpacity = 1.0
            drumMachineButton.pressedColorOpacity = 0.3
            sidepanel.turnTableClicked()
        }
    }

    Button {
        id: drumMachineButton

        width: parent.width; height: parent.height / 2
        y: parent.height / 2
        pressedColor: "gray"
        pressedColorOpacity: 0.3

        Text {
            anchors.centerIn: parent
            rotation: 90
            text: "Drum Machine"
            color: "white"
            font.pixelSize: 20
        }

        onClicked: {
            pressedColorOpacity = 1.0
            turnTableButton.pressedColorOpacity = 0.3
            sidepanel.drumMachineClicked()
        }
    }
}
