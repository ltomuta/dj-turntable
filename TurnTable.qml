import Qt 4.7

Rectangle {
    id: ui

    signal speed(variant speed)
    signal start()
    signal stop()
    signal startBeat()
    signal stopBeat()

    anchors.fill: parent
    width: 640; height: 360
    color: "black"

    Component.onCompleted: playTimer.start()

    SidePanel {
        id: sidepanel

        width: 100; height: ui.height
        onTurnTableClicked: flickable.state = ""
        onDrumMachineClicked: flickable.state = "DrumMachine"
    }

    Flickable {
        id: flickable

        anchors.left: sidepanel.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top

        contentWidth: parent.width
        contentHeight: parent.height * 2
        interactive: false

        Image {
            id: turntable

            property bool playing: false

            width: ui.width; height: ui.height
            source: "turntable.png"
            fillMode: Image.PreserveAspectFit

            Image {
                id: disk

                // speed are Hz values of the disk
                property real targetSpeed: turntable.playing ? speedslider.speed : 0.0
                property real currentSpeed: 0

                width: parent.paintedWidth * 0.80; height: width
                source: "disk.png"

                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -0.095 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.0055 * parent.paintedHeight

                onCurrentSpeedChanged: ui.speed(disk.currentSpeed)

                Timer {
                    id: playTimer

                    interval: 16  // 60 fps
                    repeat: true
                    onTriggered: {
                        disk.rotation = (disk.rotation + 0.36 * disk.currentSpeed * interval) % 360
                        if(Math.abs(disk.currentSpeed - disk.targetSpeed) <= 0.01) {
                            disk.currentSpeed = disk.targetSpeed
                        }
                        else {
                            disk.currentSpeed += (disk.targetSpeed - disk.currentSpeed) * 0.01
                        }
                    }
                }
            }

            MouseArea {
                // Don't place this as child of disk because the
                // coordination will change when disk is rotated

                property real centerx: width / 2
                property real centery: height / 2

                property int previousX: 0
                property int previousY: 0
                property variant previousTime

                anchors.fill: disk

                onPressed: {
                    var xlength = Math.abs(mouse.x - centerx)
                    var ylength = Math.abs(mouse.y - centery)

                    if(Math.sqrt(xlength * xlength + ylength * ylength) > centerx) {
                        // mouse press did not hit on the disk, the disk is actually
                        // rectangle shaped and the mouse was pressed one of the corners
                        mouse.accepted = false
                        return
                    }

                    playTimer.stop()
                    disk.currentSpeed = 0.0

                    previousX = mouse.x
                    previousY = mouse.y
                    previousTime = new Date().getTime()
                }

                onReleased: playTimer.start()

                onPositionChanged: {
                    var now = new Date().getTime()

                    var ax = mouse.x - centerx
                    var ay = centery - mouse.y
                    var bx = previousX - centerx
                    var by = centery - previousY

                    var angledelta = (Math.atan2(by, bx) - Math.atan2(ay, ax)) * 57.2957795
                    if(angledelta > 180)       { angledelta -= 360 }
                    else if(angledelta < -180) { angledelta += 360 }

                    disk.rotation = (disk.rotation + angledelta) % 360
                    disk.currentSpeed = angledelta * 2.77778 / (now - previousTime)

                    previousX = mouse.x
                    previousY = mouse.y
                    previousTime = now
                }
            }

            Button {
                id: powerbutton

                width: parent.paintedWidth * 0.12; height: parent.paintedHeight * 0.07
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 0.2 * parent.paintedWidth
                anchors.verticalCenterOffset: 0.41 * parent.paintedHeight

                source: "powerbutton.png"
                powerLightOpacity: turntable.playing ? 0.8 : 0

                onClicked: {
                    turntable.playing = !turntable.playing
                    if(turntable.playing) { arm.moveIn()  }
                    else                  { arm.moveOut() }
                }
            }

            Arm {
                id: arm

                width: parent.paintedWidth * 0.1518; height: parent.paintedHeight * 0.8927
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 0.275 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.03 * parent.paintedHeight

                onArmdownChanged: armdown ? ui.start() : ui.stop()
                onAngleChanged: angle < 14 ? armdown = false : armdown = true
            }

            SpeedSlider {
                id: speedslider

                width: parent.paintedWidth * 0.085; height: parent.paintedHeight * 0.4
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 0.4 * parent.paintedWidth
                anchors.verticalCenterOffset: 0.25 * parent.paintedHeight
                maximum: 1.5; minimum: 0.0; speed: 1.0
                sliderimage: "speedslider.png"
                sliderhandleimage: "speedknob.png"
            }
        }

        Rectangle {
            id: drummachine

            y: flickable.height
            width: flickable.width; height: flickable.height
            color: "black"

            Text {
                anchors.centerIn: parent
                text: "Drum Machine"

                font.pixelSize: 40
                font.bold: true
                color: "white"
            }
        }

        states: State {
            name: "DrumMachine"
            PropertyChanges { target: flickable; contentY: ui.height }
        }

        transitions: Transition {
            from: ""
            to: "DrumMachine"
            reversible: true
            PropertyAnimation { properties: "contentY" }
        }

    }
}
