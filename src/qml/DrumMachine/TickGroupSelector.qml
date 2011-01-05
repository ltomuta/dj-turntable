import Qt 4.7

Rectangle {
    id: toggleSwitch

    property int selectedTickGroup: 1

    width: 300; height: 40
    color: "#666666"
    radius: 4
    smooth: true

    Rectangle {
        anchors { left: parent.left; leftMargin: 1; top: parent.top; topMargin: 1; bottom: parent.bottom; bottomMargin: 1 }
        width: 8
        color: "#555555"
        radius: 2
    }

    Rectangle {
        anchors { left: parent.left; top: parent.top; right: parent.right; rightMargin: 1 }
        height: 8
        color: "#555555"
        radius: 2
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width/4 -width / 2
        text: "1-16"
        color: "white"
        font.bold: true
        font.pixelSize: 20
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width / 4 * 3 - width / 2
        text: "17-32"
        color: "white"
        font.bold: true
        font.pixelSize: 20
    }

    MouseArea {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.horizontalCenter }
        onClicked: toggleSwitch.selectedTickGroup = 2
    }

    MouseArea {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.horizontalCenter; right: parent.right;  }
        onClicked: toggleSwitch.selectedTickGroup = 1
    }

    Rectangle {
        id: knob

        anchors { top: parent.top; bottom: parent.bottom }
        anchors { topMargin: 3; bottomMargin: 3 }
        width: parent.width / 2; height: parent.height
        radius: 4
        color: "#888888"
        border.width: 2
        border.color: "#AAAAAA"

        Behavior on x { NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 } }
        x: toggleSwitch.selectedTickGroup == 1 ? toggleSwitch.width / 2 - 4 : 4

        MouseArea {
            anchors.fill: parent
            drag.target: knob
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: parent.width
            onReleased: {
                if(knob.x > (toggleSwitch.width / 4)) {
                    toggleSwitch.selectedTickGroup = 2
                    toggleSwitch.selectedTickGroup = 1
                }
                else {
                    toggleSwitch.selectedTickGroup = 1
                    toggleSwitch.selectedTickGroup = 2
                }
            }
        }
    }
}



/*
Item {
    id: selector

    property int selectedTickGroup : 1

    width: 300; height: 40

    Rectangle {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.horizontalCenter }
        radius: 8

        Behavior on color { ColorAnimation {} }
        color: selector.selectedTickGroup == 1 ? "#999999" : "#303030"

        Text {
            anchors.centerIn: parent
            font.pixelSize: 20
            font.bold: true
            text: "1-16"
            color: "white"
        }

        MouseArea { anchors.fill: parent; onClicked: selector.selectedTickGroup = 1 }
    }

    Rectangle {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.horizontalCenter; right: parent.right }
        radius: 8

        Behavior on color { ColorAnimation {} }
        color: selector.selectedTickGroup == 2 ? "#999999" : "#303030"

        Text {
            anchors.centerIn: parent
            font.pixelSize: 20
            font.bold: true
            text: "17-32"
            color: "white"
        }

        MouseArea { anchors.fill: parent; onClicked: selector.selectedTickGroup = 2 }
    }
}
*/
