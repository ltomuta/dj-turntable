import Qt 4.7

Item {
    id: knob

    property real minimumvalue: 0
    property real maximumvalue: 1.5

    property real minimumrotation: 0
    property real maximumrotation: -315

    property real preferredvalue: 1.0
    property real value: preferredvalue

    width: 300; height: 300

    Image {
        id: knobImage

        anchors.fill: parent
        source: "knobdial.png"
        rotation: knob.value / knob.maximumvalue * knob.maximumrotation
    }

    Text {
        anchors.centerIn: parent
        color: "white"
        text: knob.value.toFixed(2)
        font.bold: true
        font.pixelSize: 10
    }

    MouseArea {
        property int previousY: 0

        anchors.fill: knobImage

        //onDoubleClicked: knob.value = knob.preferredvalue
        onPressed: previousY = mouse.y
        onPositionChanged: {
            var delta = (previousY - mouse.y) * 0.008

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
