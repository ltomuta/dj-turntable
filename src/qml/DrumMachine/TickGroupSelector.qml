import Qt 4.7

BorderImage {
    id: toggleSwitch

    property int selectedTickGroup: 1

    width: 300; height: 40
    source: "../images/buttonpressed.sci"

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width / 4 - width / 2
        text: "1-16"
        color: toggleSwitch.selectedTickGroup == 1 ? "white" : "gray"
        font.bold: true
        font.pixelSize: parent.height / 3
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width / 4 * 3 - width / 2
        text: "17-32"
        color: toggleSwitch.selectedTickGroup == 2 ? "white" : "gray"
        font.bold: true
        font.pixelSize: parent.height / 3
    }

    MouseArea {
        anchors { top: parent.top; bottom: parent.bottom }
        anchors { left: parent.left; right: parent.horizontalCenter }
        onClicked: toggleSwitch.selectedTickGroup = 1
    }

    MouseArea {
        anchors { top: parent.top; bottom: parent.bottom }
        anchors { left: parent.horizontalCenter; right: parent.right }
        onClicked: toggleSwitch.selectedTickGroup = 2
    }

    Image {
        id: knob

        anchors { top: parent.top; bottom: parent.bottom }
        anchors { topMargin: 3; bottomMargin: 3 }
        width: parent.width / 2; height: parent.height
        source: "../images/tickgroupselectorslider.png"

        x: toggleSwitch.selectedTickGroup == 2 ? toggleSwitch.width / 2 - 4 : 4

        Behavior on x {
            NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
        }

        MouseArea {
            anchors.fill: parent
            drag.target: knob
            drag.axis: Drag.XAxis
            drag.minimumX: 1
            drag.maximumX: parent.width - 1
            onReleased: {
                if (knob.x > (toggleSwitch.width / 4)) {
                    toggleSwitch.selectedTickGroup = 1
                    toggleSwitch.selectedTickGroup = 2
                }
                else {
                    toggleSwitch.selectedTickGroup = 2
                    toggleSwitch.selectedTickGroup = 1
                }
            }
        }
    }
}
