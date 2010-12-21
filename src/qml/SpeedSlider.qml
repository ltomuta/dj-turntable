import Qt 4.7

Item {
    id: slider

    property real value: 1
    property real maximum: 2
    property real minimum: 1
    property alias mouseAreaScale: ma.scale
    property real defaultValue: maximum

    width: 50; height: 200

    Image {
        id: sliderimage

        anchors.fill: parent
        source: "speed.png"

    }

    Image {
        id: handle
        source: "speedslider.png"


        property int yMax: slider.height - handle.height

        anchors.horizontalCenter: parent.horizontalCenter
        y: handle.yMax - (value - minimum) * handle.yMax / (maximum - minimum)
        width: parent.width * 1.4; height: parent.height * 0.2
    }

    MouseArea {
        id: ma

        anchors.fill: handle
        drag.target: handle
        drag.axis: "YAxis"; drag.minimumY: 0; drag.maximumY: handle.yMax
        onPositionChanged: value = maximum - (maximum - minimum) * (handle.y) / handle.yMax + minimum
        onDoubleClicked: value = defaultValue
    }
}
