import Qt 4.7

Rectangle {
    id: selector

    property int selectedTickGroup : 1

    width: 300; height: 40

    color: "black"

    Rectangle {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.horizontalCenter }
        radius: 8

        Behavior on color { ColorAnimation {} }
        color: selector.selectedTickGroup == 1 ? "gray" : "#303030"

        Text {
            anchors.centerIn: parent
            font.pixelSize: 20
            font.bold: true
            text: "1"
            color: "white"
        }

        MouseArea { anchors.fill: parent; onClicked: selector.selectedTickGroup = 1 }
    }

    Rectangle {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.horizontalCenter; right: parent.right }
        radius: 8

        Behavior on color { ColorAnimation {} }
        color: selector.selectedTickGroup == 2 ? "gray" : "#303030"

        Text {
            anchors.centerIn: parent
            font.pixelSize: 20
            font.bold: true
            text: "2"
            color: "white"
        }

        MouseArea { anchors.fill: parent; onClicked: selector.selectedTickGroup = 2 }
    }
}
