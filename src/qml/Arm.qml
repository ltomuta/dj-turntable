import Qt 4.7

Image {
    id: arm

    property real angle: armdown ? positionOnDisk * 22 + armOnDiskOffset : armOnDiskOffset
    property real armOnDiskOffset: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0
    property bool liftedByUser: false

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }

    x: 100; y: 300
    source: "arm.png"
    smooth: true
    transform: Rotation { origin.x: arm.width * 0.82; origin.y: arm.height * 0.1; angle: arm.angle }

    MouseArea {
        width: 50; height: 50
        anchors.bottom: parent.bottom
        onPressed: arm.liftedByUser = true
        onReleased: arm.liftedByUser = false
    }

    SequentialAnimation {
        id: moveToDisk

        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 12; duration: 2000 }
        ScriptAction { script: { arm.armdown = true } }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: arm.armdown = false }
        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 0; duration: 1000 }
    }
}
