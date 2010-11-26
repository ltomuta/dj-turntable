import Qt 4.7

Rectangle {
    id: beatSelector

    property int index: 0
    property real angle: 0

    Behavior on angle { NumberAnimation { duration:  50 } }

    width: 300; height: 200
    radius: 8
    color: "#303030"
    smooth: true

    transform: Rotation {
        id: rotation
        origin.x: width / 2; origin.y: height / 2
        axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
        angle: beatSelector.angle
    }

    Item {
        id: beatdown
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 100

        Text {
            anchors.centerIn: parent
            font.bold: true
            font.pixelSize: 20
            text: "<"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent

            onPressed: beatSelector.angle = -15
            onReleased: beatSelector.angle = 0
            onClicked: {
                if(beatSelector.index > 0)
                    beatSelector.index -= 1
            }
        }
    }

    Text {
        anchors.centerIn: parent
        font.bold: true; font.pixelSize: 20
        text: beatSelector.index
        color: "white"
    }

    Item {
        id: beatup
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        width: 100

        Text {
            anchors.centerIn: parent
            font.bold: true; font.pixelSize: 20
            text: ">"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent

            onPressed: beatSelector.angle = 15
            onReleased: beatSelector.angle = 0
            onClicked: {
                if(beatSelector.index < 3)
                    beatSelector.index += 1
            }
        }
    }
}
