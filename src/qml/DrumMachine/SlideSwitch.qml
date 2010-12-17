import Qt 4.7

Rectangle {
    id: toggleSwitch

    property bool on: false

    width: 40; height: 30
    color: "#303030"
    radius: 8
    smooth: true

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width/4 -width / 2
        text: "On"
        color: "white"
        font.bold: true
        font.pixelSize: 20
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width / 4 * 3 - width / 2
        text: "Off"
        color: "white"
        font.bold: true
        font.pixelSize: 20
    }

    Rectangle {
        id: knob

        width: parent.width / 2; height: parent.height
        radius: width / 2
        color: "black"
        border.width: 2
        border.color: "gray"

        Behavior on x { NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 } }
        x: toggleSwitch.on ? toggleSwitch.width / 2 : 0

        MouseArea {
            anchors.fill: parent
            drag.target: knob
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: parent.width
            onReleased: {
                if(knob.x > (toggleSwitch.width / 4)) {
                    toggleSwitch.on = false
                    toggleSwitch.on = true
                }
                else {
                    toggleSwitch.on = true
                    toggleSwitch.on = false
                }
            }
        }
    }
}
