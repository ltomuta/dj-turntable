import Qt 4.7

Rectangle {
    id: sidepanel

    signal turnTableClicked()
    signal drumMachineClicked()

    width: 100
    height: 200

    Rectangle {
        id: turnTableButton

        width: parent.width; height: parent.height / 2
        radius: 4
        color: "gray"

        Text {
            anchors.centerIn: parent
            rotation: 90
            text: "Turntable"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: sidepanel.turnTableClicked()
        }
    }

    Rectangle {
        id: drumMachineButton

        width: parent.width; height: parent.height / 2
        y: parent.height / 2
        radius: 4
        color: "gray"

        Text {
            anchors.centerIn: parent
            rotation: 90
            text: "Drum machine"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: sidepanel.drumMachineClicked()
        }
    }
}
