import QtQuick 1.0

Item {
    id: arm

    property real angle: 0
    property bool armdown: false
    property real positionOnDisk: 0   // Values from 0.0 - 1.0

    property int minAngleOnDisk: 23
    property int maxAngleOnDisk: 43

    signal armReleasedByUser(real position)

    function moveIn() { moveToStop.stop(); moveToDisk.start() }
    function moveOut() { moveToDisk.stop(); moveToStop.start() }
    function setPositionOnDisk(position) {
        if (armdown) {
            positionOnDisk = position
            angle = (maxAngleOnDisk - minAngleOnDisk) * position
                    + minAngleOnDisk
        }
    }

    function updateArmDown(userMoving) {
        if (angle > minAngleOnDisk && userMoving == false) {
            arm.armReleasedByUser((angle - minAngleOnDisk)
                                  / (maxAngleOnDisk - minAngleOnDisk))
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

        transform: [
            Rotation {
                origin.x: width / 2; origin.y: 0
                angle: arm.angle
            },
            Rotation {
                axis { x: 1; y: 0; z: 0 }
                origin.x: width / 2; origin.y: 0
                angle: arm.armdown ? 0 : 2
                Behavior on angle { SmoothedAnimation { velocity: 40 } }
            }
        ]

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
                if (angle > arm.maxAngleOnDisk)
                    angle = arm.maxAngleOnDisk
                else if (angle < 0)
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

        ScriptAction { script: dragMouse.enabled = false }
        ScriptAction { script: arm.armdown = false }
        SmoothedAnimation {
            target: arm; property: "angle"; to: arm.minAngleOnDisk; velocity: 23
        }
        ScriptAction { script: { arm.armdown = true } }
        ScriptAction { script: dragMouse.enabled = true }
    }

    SequentialAnimation {
        id: moveToStop

        ScriptAction { script: dragMouse.enabled = false }
        ScriptAction { script: arm.armdown = false }
        SmoothedAnimation {
            target: arm; property: "angle"; to: -0.1; velocity: 23
        }
        ScriptAction { script: dragMouse.enabled = true }
    }
}
