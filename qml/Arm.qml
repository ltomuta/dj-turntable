import Qt 4.7

Image {
    id: arm

    property real angle: 0
    property bool armdown: false

    function moveIn() { armShakeAnimation.start() }
    function moveOut() { armShakeAnimation.stop(); arm.angle = 0 }

    x: 100; y: 300
    source: "arm.png"
    smooth: true
    transform: Rotation { origin.x: arm.width * 0.82; origin.y: arm.height * 0.2; angle: arm.angle }

    Behavior on angle { NumberAnimation { duration: 1000 } }

    SequentialAnimation {
        id: armShakeAnimation

        loops: Animation.Infinite
        PropertyAnimation { target: arm; property: "angle"; to: 20; duration: 2000 }
        PropertyAnimation { target: arm; property: "angle"; to: 18; duration: 2000 }
    }
}
