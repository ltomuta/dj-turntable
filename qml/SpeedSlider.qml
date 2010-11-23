import Qt 4.7

Item {
    id: slider

    property real speed: 1
    property real maximum: 2
    property real minimum: 1
    property alias sliderimage: sliderimage.source
    property alias sliderhandleimage: handle.source

    width: 50; height: 200

    Image {
        id: sliderimage
        anchors.fill: parent
    }

    Image {
        id: handle

        property int yMax: slider.height - handle.height

        anchors.horizontalCenter: parent.horizontalCenter
        y: handle.yMax - (speed - minimum) * handle.yMax / (maximum - minimum)
        width: parent.width * 1.4; height: parent.height * 0.2

        MouseArea {
            anchors.fill: parent

            drag.target: parent
            drag.axis: "YAxis"; drag.minimumY: 0; drag.maximumY: handle.yMax
            onPositionChanged: { speed = maximum - (maximum - minimum) * (handle.y) / handle.yMax + minimum; }
        }
    }
}
