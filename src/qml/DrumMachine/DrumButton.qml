import Qt 4.7

Item {
    id: button

    property int tick: -1
    property int sample: -1
    property bool pressed: false

    property color pressedColor
    property color notPressedColor

    width: 34; height: 40

    Image {
        anchors.fill: parent
        anchors.margins: 3
        source: button.pressed ? "drumbuttonselected.png" : "drumbutton.png"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.pressed = !button.pressed
            drumMachine.drumButtonToggled(button.tick, button.sample, button.pressed)
        }
    }
}
