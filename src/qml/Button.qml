import Qt 4.7

Image {
    id: button

    signal clicked()

    property alias pressedColor: pressed.color
    property alias pressedColorOpacity: pressed.opacity
    property bool pressed: false

    width: 100; height: 40
    Behavior on scale { NumberAnimation { duration: 50 } }

    Rectangle {
        id: pressed

        anchors { fill: parent }
        color: "transparent"
        radius: 3
        opacity: 1
        Behavior on opacity { NumberAnimation { } }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: { button.scale = 0.9; button.pressed = true }
        onReleased: { button.scale = 1.0; button.pressed = false }
        onClicked: button.clicked()
    }
}
