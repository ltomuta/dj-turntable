import Qt 4.7

Image {
    id: arm

    property real angle: armdown ? positionOnDisk * 30 + armOnDiskOffset : armOnDiskOffset
    property real armOnDiskOffset: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }

    x: 100; y: 300
    source: "arm.png"
    smooth: true
    transform: Rotation { origin.x: arm.width * 0.82; origin.y: arm.height * 0.2; angle: arm.angle }

    SequentialAnimation {
        id: moveToDisk

        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 10; duration: 2000 }
        ScriptAction { script: { arm.armdown = true } }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: arm.armdown = false }
        PropertyAnimation { target: arm; property: "armOnDiskOffset"; to: 0; duration: 1000 }
    }
}
