import Qt 4.7

Rectangle {
    property alias index: view.currentIndex

    width: 400; height: 200
    color: "#303030"
    clip: true

    radius: 8

    ListModel {
        id: model

        ListElement { name: "Predefined 1" }
        ListElement { name: "Predefined 2" }
        ListElement { name: "Predefined 3" }
        ListElement { name: "Predefined 4" }
        ListElement { name: "User defined 1" }
        ListElement { name: "User defined 2" }
        ListElement { name: "User defined 3" }
        ListElement { name: "User defined 4" }
    }

    PathView {
        id: view

        anchors.fill: parent

        model: model
        delegate: Text { font.pixelSize: 20; font.bold: true; text: name; width: view.width; color: "white" }

        pathItemCount: 8
        dragMargin: view.width / 2
        interactive: false

        path: Path {
            startX: -view.width / 4; startY: view.height / 2
            PathLine { x: view.pathItemCount * view.width + view.width / 4; y: view.height / 2}
        }
    }

    MouseArea {
        width: parent.width * 0.33; height: parent.height

        onClicked: view.decrementCurrentIndex()
    }

    MouseArea {
        width: parent.width * 0.33; height: parent.height
        anchors.right: parent.right

        onClicked: view.incrementCurrentIndex()
    }
}



/*
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

            onPressed: beatSelector.angle = -25
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
        text: {
            if(beatSelector.index < 4) {
                return "Predef. " + beatSelector.index
            }
            else {
                return "User. " + beatSelector.index
            }
        }
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

            onPressed: beatSelector.angle = 25
            onReleased: beatSelector.angle = 0
            onClicked: {
                if(beatSelector.index < 7)
                    beatSelector.index += 1
            }
        }
    }
}
*/
