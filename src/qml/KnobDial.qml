import Qt 4.7

Image {
    id: knob

    property real minimumvalue: 0
    property real maximumvalue: 99


    property real minimumrotation: 35
    property real maximumrotation: -285
    //property real minimumrotation: 0
    //property real maximumrotation: -315

    property real value: 0

    width: 300; height: 300
    source: "images/knobdial.png"
    smooth: true


    Image {
        id: knobImage

        anchors.fill: parent
        source: "images/knobdialpointer.png"
        rotation: knob.minimumrotation + knob.value / knob.maximumvalue *
                  -knob.maximumrotation
        smooth: true
    }

    /*
    Image {
        source: "images/arrowup.png"
        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height / -6
    }

    Image {
        source: "images/arrowdown.png"
        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height / 6
    }
    */

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

            if(knob.value + delta > knob.maximumvalue) {
                knob.value = knob.maximumvalue
            }
            else if(knob.value + delta < knob.minimumvalue) {
                knob.value = knob.minimumvalue
            }
            else {
                knob.value += delta
                previousY = mouse.y
            }
        }
    }
}
