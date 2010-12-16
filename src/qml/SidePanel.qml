import Qt 4.7

Rectangle {
    id: sidepanel

    signal turnTableClicked()
    signal drumMachineClicked()

    property bool turnTableButtonPressed: false
    property bool drumMachineButtonPressed: false

    width: 100; height: 200
    color: "black"

    Button {
        id: turnTableButton

        width: parent.width; height: parent.height / 2
        pressedColor: "gray"
        pressedColorOpacity: turnTableButtonPressed ? 1.0 : 0.3

        Text {
            anchors.centerIn: parent
            rotation: 270
            text: "Turntable"
            color: "white"
            font.pixelSize: 20
        }

        onClicked: sidepanel.turnTableClicked()
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
            text: "Drum Machine"
            color: "white"
            font.pixelSize: 20
        }

        onClicked: sidepanel.drumMachineClicked()
    }
}
