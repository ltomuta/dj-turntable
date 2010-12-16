import Qt 4.7

Rectangle {
    property alias index: view.currentIndex

    width: 400; height: 200
    color: "#303030"
    clip: true
    radius: 8

    ListModel {
        id: model

        // There is something wrong on the view, the indexes of the current showing
        // element has offset of a one indes, so we correct this by shifting
        // the elements in model to backwards by one element, this is lame...

        ListElement { name: "User defined 4" }
        ListElement { name: "Predefined 1" }
        ListElement { name: "Predefined 2" }
        ListElement { name: "Predefined 3" }
        ListElement { name: "Predefined 4" }
        ListElement { name: "User defined 1" }
        ListElement { name: "User defined 2" }
        ListElement { name: "User defined 3" }
    }

    PathView {
        id: view

        anchors.fill: parent
        model: model

        delegate: Item {
            width: view.width; height: view.height

            Text {
                anchors.centerIn: parent
                font.pixelSize: 20
                font.bold: true
                text: name
                color: "white"
            }
        }

        pathItemCount: 8
        dragMargin: view.width

        path: Path {
            startX: -view.width / 2; startY: view.height / 2
            PathLine { x: model.count * view.width - view.width / 2; y: view.height / 2 }
        }
    }
}
