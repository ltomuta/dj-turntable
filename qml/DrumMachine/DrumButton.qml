import Qt 4.7

Rectangle {
    id: button

    property int tick: -1
    property int sample: -1
    property bool pressed: false

    property color pressedColor
    property color notPressedColor

    width: 30; height: 30
    color: pressed ? pressedColor : notPressedColor

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.pressed = !button.pressed
            drumMachine.drumButtonToggled(button.tick, button.sample, button.pressed)
        }
    }
}
