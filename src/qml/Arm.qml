import Qt 4.7

Item {
    id: arm

    property real angle: armdown ? positionOnDisk * 20 + armOnDiskOffset : armOnDiskOffset
    property real armOnDiskOffset: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0
    property bool liftedByUser: false

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }

    transform: Rotation { origin.x: arm.width * 0.25; origin.y: -5; angle: arm.angle }

    Image {
        source: "armshadow.png"
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: -5
    }

    Image {
        source: "arm.png"
        smooth: true
        anchors.fill: parent
    }

    MouseArea {
        width: 50; height: 50
        anchors.bottom: parent.bottom
        onPressed: arm.liftedByUser = true
        onReleased: arm.liftedByUser = false
    }

    SequentialAnimation {
        id: moveToDisk

        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 26; duration: 2000 }
        ScriptAction { script: { arm.armdown = true } }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: arm.armdown = false }
        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 0; duration: 1000 }
    }
}
