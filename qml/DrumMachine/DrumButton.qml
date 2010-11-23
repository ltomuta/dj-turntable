import Qt 4.7

Rectangle {
    id: button

    property int index: -1
    property bool pressed: false

    width: 30; height: 30
    color: pressed ? "white" : "gray"
    //radius: 3

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.pressed = !button.pressed
            drumMachine.drumButtonToggled(button.index, button.pressed)
        }
    }
}
