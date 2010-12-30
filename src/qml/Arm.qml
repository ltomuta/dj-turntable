import Qt 4.7

Item {
    id: arm

    property real angle: armdown ? positionOnDisk * 20 + armOnDiskOffset : armOnDiskOffset
    property real armOnDiskOffset: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }

    Image {
        id: pedal
        width: parent.width; height: width
        smooth: true
        source: "images/pedal.png"
    }

    Item {
        anchors { top: pedal.verticalCenter; bottom: parent.bottom; left: parent.left; right: parent.right }

        transform: Rotation { origin.x: width / 2; origin.y: 0; angle: arm.angle }

        Image {
            anchors { fill: armImage; leftMargin: parent.width * 0.15; topMargin: parent.height * 0.02 }
            smooth: true
            source: "images/armshadow.png"
        }

        Image {
            id: armImage
            anchors { fill: parent; leftMargin: parent.width * 0.3 }

            source: "images/arm.png"
            smooth: true
        }
    }

    Image {
        anchors.fill: pedal
        anchors.leftMargin: pedal.width * 0.15
        anchors.topMargin: anchors.leftMargin

        smooth: true
        source: "images/armcasingshadow.png"
    }

    Image {
        anchors.fill: pedal; anchors.margins: pedal.width * 0.15
        smooth: true
        source: "images/armcasing.png"
    }


    SequentialAnimation {
        id: moveToDisk

        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 23; duration: 2000 }
        ScriptAction { script: { arm.armdown = true } }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: arm.armdown = false }
        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 0; duration: 1000 }
    }
}
