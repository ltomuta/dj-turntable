/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Image {
    id: knob

    property real minimumvalue: 0
    property real maximumvalue: 99

    property real minimumrotation: 35
    property real maximumrotation: 290

    property real value: 0

    width: 300
    height: 300
    source: "../images/knobdial.png"
    smooth: true

    Image {
        id: knobShadow

        width: parent.width
        height: parent.height
        x: width * 0.04
        y: height * 0.04
        source: "../images/armcasingshadow.png"
        smooth: true
    }

    Image {
        id: knobImage

        anchors.fill: parent
        source: "../images/knobdialpointer.png"
        rotation: knob.minimumrotation + knob.value / knob.maximumvalue *
                  knob.maximumrotation
        smooth: true
    }

    Image {
        id: knobHat
        anchors.fill: parent
        source: "../images/knobhat.png"
        smooth: true
    }

    LCDDisplay {
        anchors.centerIn: parent
        number: knob.value
    }

    MouseArea {
        property int previousY: 0

        anchors.fill: knobImage

        onPressed: previousY = mouse.y
        onPositionChanged: {
            var delta = (previousY - mouse.y) * 0.20

            if (knob.value + delta > knob.maximumvalue) {
                knob.value = knob.maximumvalue
            } else if (knob.value + delta < knob.minimumvalue) {
                knob.value = knob.minimumvalue
            } else {
                knob.value += delta
                previousY = mouse.y
            }
        }
    }
}
