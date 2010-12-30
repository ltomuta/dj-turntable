import Qt 4.7

Item {
    id: slider
    property real value: 1
    property real maximum: 2
    property real minimum: 1
    property alias mouseAreaScale: mouseArea.scale
    property real defaultValue: maximum
    property real scaleFactor: 10

    function calculateYPos(value) {
        return handle.yMax - (value - minimum) * handle.yMax / (maximum - minimum) + handle.height * 0.358
    }

    width: 100; height: 200

    Image {
        id: sliderimage

        anchors.fill: parent
        source: "images/speed.png"
    }

    Image {
        id: handle

        property int yMax: slider.height - handle.height + handle.height * 0.14

        anchors { horizontalCenter: parent.horizontalCenter; horizontalCenterOffset: 1 }
        y: handle.yMax - (value - minimum) * handle.yMax / (maximum - minimum)
        width: sliderimage.width * 1.4; height: parent.height * 0.3
        source: "images/speedslider.png"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: handle
        drag { target: handle; axis: "YAxis"; minimumY: 0; maximumY: handle.yMax }
        onPositionChanged: value = maximum - (maximum - minimum) * (handle.y) / handle.yMax + minimum
        onDoubleClicked: value = defaultValue
    }
}
