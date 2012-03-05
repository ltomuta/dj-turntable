/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Image {
    id: turntable

    property bool playing: false
    property real speedSliderValue: speedslider.value

    width:  flickable.width - mixerpanel.width - 2
    height: flickable.height
    source: "../images/backgroundaluminium.png"
    fillMode: Image.Stretch

    function start() {
        playTimer.start()
    }

    function moveIn() {
        arm.moveIn()
    }

    function moveOut() {
        arm.moveOut()
    }

    function setPositionOnDisk(pos) {
        arm.setPositionOnDisk(pos)
    }

    Image {
        id: discPlate

        width: Math.min(parent.paintedWidth * 0.79,
                        parent.paintedHeight * 0.98)
        height: width

        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -0.085 * parent.paintedWidth
        anchors.verticalCenterOffset: -0.0055 * parent.paintedHeight

        source: "../images/discplate.png"
        smooth: lowPerf ? false : true
    }

    Image {
        id: discLabel
        anchors.centerIn: disc
        source: "../images/disklabel.png"
        scale: disc.width / disc.sourceSize.width
        smooth: false
        visible: lowPerf ? true : false
        z: 1
    }

    Image {
        id: disc

        // speed are Hz values of the disk
        property real targetSpeed: turntable.playing ?
                                       speedslider.value : 0.0
        property real currentSpeed: 0

        anchors {
            fill: discPlate
            margins: discPlate.width * 0.045
        }
        source: "../images/disk.png"
        smooth: lowPerf ? false : true

        onCurrentSpeedChanged: playTimer.running ?
                                   ui.diskSpeed(disc.currentSpeed) :
                                   ui.diskAimSpeed(disc.currentSpeed)

        Timer {
            id: playTimer

            // 30fps in lowPerf otherwise 60fps
            interval: lowPerf ? 32 : 16
            repeat: true

            onTriggered: {
                if (lowPerf) {
                    discLabel.rotation = (discLabel.rotation + 0.36 *
                                     disc.currentSpeed * interval) % 360
                } else {
                    disc.rotation = (disc.rotation + 0.36 *
                                     disc.currentSpeed * interval) % 360
                }

                if (Math.abs(disc.currentSpeed - disc.targetSpeed) <= 0.01) {
                    disc.currentSpeed = disc.targetSpeed
                } else {
                    disc.currentSpeed += (disc.targetSpeed -
                                          disc.currentSpeed) * 0.10
                }
            }
        }
    }

    Image {
        id: diskReflection

        anchors.fill: disc
        source: "../images/diskreflection.png"
        fillMode: Image.PreserveAspectFit

        rotation: 45
        Behavior on rotation { RotationAnimation {} }
    }

    MouseArea {
        // Don't place this as child of disk because the
        // coordination will change when disk is rotated

        property real centerx: width / 2
        property real centery: height / 2

        property int previousX: 0
        property int previousY: 0
        property variant previousTime

        anchors.fill: disc

        onPressed: {
            var xlength = Math.abs(mouse.x - centerx)
            var ylength = Math.abs(mouse.y - centery)

            if (Math.sqrt(xlength * xlength + ylength *
                         ylength) > centerx) {
                // mouse press did not hit on the disk, the disk is actually
                // rectangle shaped and the mouse was pressed one of the corners
                mouse.accepted = false
                return
            }

            playTimer.stop()
            disc.currentSpeed = 0.0

            previousX = mouse.x
            previousY = mouse.y
            previousTime = new Date().getTime()
        }

        onReleased: playTimer.start()

        onPositionChanged: {
            var now = new Date().getTime()

            if (mouse.x === previousX && mouse.y === previousY) {
                // In Harmattan sometimes we get duplicate
                // touch events, we have to filter them out
                // or we will get the angledelta = 0 in our
                // calculations below.
                return
            }

            var ax = mouse.x - centerx
            var ay = centery - mouse.y
            var bx = previousX - centerx
            var by = centery - previousY

            var angledelta = (Math.atan2(by, bx) -
                              Math.atan2(ay, ax)) * 57.2957795

            if (angledelta > 180) {
                angledelta -= 360
            } else if (angledelta < -180) {
                angledelta += 360
            }

            if (lowPerf) {
                discLabel.rotation =
                        (discLabel.rotation + angledelta) % 360
            }

            disc.rotation = (disc.rotation + angledelta) % 360

            var deltaTime = now - previousTime

            if (deltaTime > 0) {
                disc.currentSpeed = angledelta * 2.77778 / deltaTime
            }

            previousX = mouse.x
            previousY = mouse.y
            previousTime = now
        }
    }

    Item {
        anchors {
            top: speedslider.top
            bottom: speedslider.bottom
            right: speedslider.left
            rightMargin: speedslider.width * 0.12
        }

        Text {
            y: speedslider.calculateYPos(1.30)
            anchors.right: parent.right
            text: "+30"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            y: speedslider.calculateYPos(1.15)
            anchors.right: parent.right
            text: "+15"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            y: speedslider.calculateYPos(1.0)
            anchors.right: parent.right
            text: "0"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            y: speedslider.calculateYPos(0.85)
            anchors.right: parent.right
            text: "-15"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            y: speedslider.calculateYPos(0.70)
            anchors.right: parent.right
            text: "-30"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            y: speedslider.calculateYPos(0.50)
            anchors.right: parent.right
            text: "-50"; color: "#505050"; font.pixelSize: 10
        }

        Text {
            anchors {
                right: parent.right
                rightMargin: 15
                bottom: parent.bottom
                bottomMargin: 0
            }
            text: "Disk / drum speed %"
            color: "#505050"; font.pixelSize: 10
        }
    }

    SpeedSlider {
        id: speedslider

        width: parent.paintedWidth * 0.075
        height: parent.paintedHeight * 0.6

        anchors {
            right: arm.right
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 15
        }

        maximum: 1.30; minimum: 0.50; value: 1.0; defaultValue: 1.0
        mouseAreaScale: 3
    }

    Arm {
        id: arm

        width: disc.width * 0.30
        height: disc.height * 1.06
        anchors {
            left: discPlate.right
            top: discPlate.top
            leftMargin: discPlate.width * -0.05
        }

        onArmdownChanged: armdown ? ui.start() : ui.stop()
        onArmReleasedByUser: ui.seekToPosition(position)
    }
}
