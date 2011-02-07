import Qt 4.7

Item {
    id: arm

    property real angle: 0
    property real armOnDiskOffset: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0

    signal armReleasedByUser(real position)

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }
    function setPositionOnDisk(position) {
        if(armdown) {
            positionOnDisk = position
            angle = (43 - 23) * position + 23
        }
    }

    function updateArmDown(userMoving) {
        if(angle > 23 && userMoving == false) {
            arm.armReleasedByUser((angle - 23) / (43 - 23))
            armdown = true
        }
        else {
            armdown = false
        }
    }

    width: 40; height: 360

    Image {
        id: pedal

        property real centerX: width / 2
        property real centerY: height / 2

        width: parent.width; height: width
        smooth: true
        source: "images/pedal.png"
    }

    Item {
        anchors {
            top: pedal.verticalCenter; bottom: parent.bottom
            left: parent.left; right: parent.right
        }

        transform: Rotation {
            origin.x: width / 2; origin.y: 0
            angle: arm.angle
        }

        Image {
            anchors {
                fill: armImage; leftMargin: parent.width * 0.15
                topMargin: parent.height * 0.02
            }
            source: "images/armshadow.png"
            smooth: true
        }

        Image {
            id: armImage

            anchors { fill: parent; leftMargin: parent.width * 0.3 }
            source: "images/arm.png"
            smooth: true
        }

        MouseArea {
            id: dragMouse

            anchors {
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: parent.width * 0.15
                bottom: parent.bottom
            }

            width: parent.width * 0.7; height: parent.height * 0.20

            onPressed: updateArmDown(true)
            onReleased: updateArmDown(false)
            onCanceled: updateArmDown(false)

            onPositionChanged: {
                var object = mapToItem(pedal, mouse.x, mouse.y)
                var xdistance = -(object.x - pedal.centerX)
                var ydistance = object.y - pedal.centerY
                var angle = Math.atan(xdistance / ydistance) * 57.2957795
                if(angle > 43)
                    angle = 43
                else if(angle < 0)
                    angle = 0

                arm.angle = angle
            }
        }
    }

    Image {
        anchors {
            fill: pedal
            leftMargin: pedal.width * 0.15
            topMargin: anchors.leftMargin
        }

        smooth: true
        source: "images/armcasingshadow.png"
    }

    Image {
        anchors { fill: pedal; margins: pedal.width * 0.15 }
        smooth: true
        source: "images/armcasing.png"
    }

    SequentialAnimation {
        id: moveToDisk

        SmoothedAnimation {
            target: arm; property: "angle"; to: 23; velocity: 23
        }
        ScriptAction { script: { arm.armdown = true } }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: arm.armdown = false }
        SmoothedAnimation {
            target: arm; property: "angle"; to: 0; velocity: 23
        }
    }
}
