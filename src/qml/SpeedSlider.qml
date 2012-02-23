/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Item {
    id: slider
    property real value: defaultValue
    property real maximum: 1.08
    property real minimum: 0.92
    property alias mouseAreaScale: mouseArea.scale
    property real defaultValue: 1.0

    function calculateYPos(value) {
        return (value - minimum) * handle.yMax
                / (maximum - minimum) + handle.height * 0.358 + 2
    }

    width: 100
    height: 200

    Image {
        id: sliderimage

        anchors.fill: parent
        source: "images/speed.png"
    }

    Image {
        id: handle

        property int yMax: slider.height - handle.height + handle.height * 0.14

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 1
        y: (value - minimum) * handle.yMax / (maximum - minimum)
        width: sliderimage.width * 1.4
        height: parent.height * 0.3
        source: "images/speedslider.png"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: handle
        drag {
            target: handle
            axis: "YAxis"
            minimumY: 0
            maximumY: handle.yMax
        }

        onPositionChanged: value = (maximum - minimum) * (handle.y) / handle.yMax + minimum
        onDoubleClicked: moveToDefault.start()
    }

    SequentialAnimation {
        id: moveToDefault

        SmoothedAnimation {
            target: slider
            property: "value"
            to: slider.defaultValue
            velocity: 0.8
        }
    }
}
