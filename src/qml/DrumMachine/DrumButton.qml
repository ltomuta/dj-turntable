import Qt 4.7

Item {
    id: button

    property int tick: -1
    property int sample: -1
    property bool pressed: false

    property string selectedSource
    property string unselectedSource

    width: 34; height: 40

    Image {
        id: image

        anchors.fill: parent
        anchors.margins: 3
        source: button.pressed ? selectedSource : unselectedSource
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.pressed = !button.pressed
            if(tick != -1 || sample != -1) {
                drumMachine.drumButtonToggled(button.tick, button.sample, button.pressed)
            }
        }
    }
}
