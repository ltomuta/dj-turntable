import Qt 4.7

Image {
    id: button

    signal clicked()

    property alias powerLightOpacity:  powerlight.opacity

    x: 100; y: 100
    width: 100; height: 40

    Rectangle {
        id: powerlight

        anchors { fill: parent; margins: 2 }
        color: "green"; radius: 8
        opacity: 1
        Behavior on opacity { NumberAnimation { } }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: button.clicked()
    }
}
