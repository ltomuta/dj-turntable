import Qt 4.7
import "DrumMachine"
import "HelpScreen"

Rectangle {
    id: ui

    signal diskSpeed(variant speed)
    signal diskAimSpeed(variant speed)
    signal start()
    signal stop()

    signal cutOff(variant value)
    signal resonance(variant value)

    signal linkActivated(variant link)

    function audioPosition(pos) { arm.positionOnDisk = pos }
    function inclination(deg) { diskReflection.rotation = -deg * 2 + 45 }

    anchors.fill: parent
    width: 640; height: 360
    color: "black"
    focus: true

    Keys.onDownPressed: flickable.setState("DrumMachine")
    Keys.onUpPressed: flickable.setState("TurnTable")
    Keys.onSpacePressed: powerbutton.press()
    Keys.onLeftPressed: drumMachine.selectedTickGroup = 1
    Keys.onRightPressed: drumMachine.selectedTickGroup = 2
    Keys.onPressed: {
        if(event.key == 56 || event.key == Qt.Key_I) {
            flickable.setState("Help")
            event.accepted = true
        }
        else if(event.key == Qt.Key_Backspace) {
            if(flickable.state == "Help") {
                helpScreen.backPressed()
            }
        }
        else if(event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
            drumMachine.running = !drumMachine.running
            event.accepted = true
        }
    }

    Component.onCompleted: {
        flickable.setState("TurnTable")
        playTimer.start()
    }

    Flickable {
        id: flickable

        property string prevState: ""

        function setState(newState) {
            if(newState != state) {
                prevState = state
                state = newState
            }
        }

        anchors { left: sidepanel.right; right: parent.right; bottom: parent.bottom; top: parent.top }
        contentWidth: parent.width; contentHeight: parent.height * 3
        interactive: false

        HelpScreen {
            id: helpScreen

            width: flickable.width; height: flickable.height
            y: -flickable.height

            onBackPressed: flickable.setState(flickable.prevState)
            onLinkActivated: ui.linkActivated(link)
        }

        Image {
            id: turntable

            property bool playing: false

            width:  flickable.width - mixerpanel.width - 2; height: flickable.height
            source: "images/turntable.png"
            fillMode: Image.Stretch

            Image {
                id: discPlate

                width: parent.paintedWidth * 0.79; height: width
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -0.085 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.0055 * parent.paintedHeight

                source: "images/discplate.png"
                smooth: true
            }

            Image {
                id: disk

                // speed are Hz values of the disk
                property real targetSpeed: turntable.playing ? speedslider.value : 0.0
                property real currentSpeed: 0

                width: parent.paintedWidth * 0.73; height: width
                source: "images/disk.png"
                smooth: true

                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -0.085 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.0055 * parent.paintedHeight

                onCurrentSpeedChanged: playTimer.running ? ui.diskSpeed(disk.currentSpeed) : ui.diskAimSpeed(disk.currentSpeed)

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
                            disk.currentSpeed += (disk.targetSpeed - disk.currentSpeed) * 0.10
                        }
                    }
                }
            }

            Image {
                id: diskReflection

                anchors.fill: disk
                source: "images/diskreflection.png"
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

                    if(now - previousTime > 0) { disk.currentSpeed = angledelta * 2.77778 / (now - previousTime) }

                    previousX = mouse.x
                    previousY = mouse.y
                    previousTime = now
                }
            }

            Item {
                anchors { top: speedslider.top; bottom: speedslider.bottom; right: speedslider.left; rightMargin: 5 }

                Text { y: speedslider.calculateYPos(0); anchors.right: parent.right; text: "0"; color: "#505050"; font.pixelSize: 10 }
                Text { y: speedslider.calculateYPos(0.5); anchors.right: parent.right; text: "75"; color: "#505050"; font.pixelSize: 10 }
                Text { y: speedslider.calculateYPos(1.0); anchors.right: parent.right; text: "150"; color: "#505050"; font.pixelSize: 10 }
                Text { y: speedslider.calculateYPos(1.5); anchors.right: parent.right; text: "225"; color: "#505050"; font.pixelSize: 10 }

                Text {
                    text: "Disk / drum speed"
                    color: "#505050"
                    anchors { right: parent.right; rightMargin: 15; bottom: parent.bottom; bottomMargin: 0 }
                    font.pixelSize: 10
                }
            }



            SpeedSlider {
                id: speedslider

                width: parent.paintedWidth * 0.085; height: parent.paintedHeight * 0.4
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 0.4 * parent.paintedWidth
                anchors.verticalCenterOffset: 0.25 * parent.paintedHeight
                maximum: 1.5; minimum: 0.0; value: 1.0; defaultValue: 1.0
                scaleFactor: 150
                mouseAreaScale: 3
            }


            Arm {
                id: arm

                width: parent.paintedWidth * 0.20; height: parent.paintedHeight * 0.93
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 0.38 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.025 * parent.paintedHeight

                onArmdownChanged: armdown ? ui.start() : ui.stop()
            }
        }

        Rectangle {
            id: mixerpanel

            x: flickable.width - mixerpanel.width
            width: 0.203125 * ui.width; height: flickable.height
            color: "#999999"
            radius: 4

            Item {
                id: buttonPanel

                anchors { left: parent.left; right: parent.right; top: parent.top }
                height: parent.height / 6

                Image {
                    id: closeButton

                    property bool pressed: false

                    anchors { left: parent.horizontalCenter; right: parent.right }
                    anchors { top: parent.top; bottom: parent.bottom; margins: 5 }

                    source: pressed ? "images/exit_on.png" : "images/exit.png"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        onPressed: { closeButton.pressed = true; closeButton.scale = 0.9 }
                        onReleased: { closeButton.pressed = false; closeButton.scale = 1.0 }
                        onClicked: Qt.quit()
                    }
                }

                Image {
                    id: infoButton

                    property bool pressed: false

                    anchors { left: parent.left; right: parent.horizontalCenter }
                    anchors { top: parent.top; bottom: parent.bottom; margins: 5 }

                    source: pressed ? "images/info_on.png" : "images/info.png"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        onPressed: { infoButton.pressed = true; infoButton.scale = 0.9 }
                        onReleased: { infoButton.pressed = false; infoButton.scale = 1.00 }
                        onClicked: flickable.setState("Help")
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right }
                anchors { top: buttonPanel.bottom; bottom: powerButtonArea.top }

                Text {
                    text: "Resonance"
                    color: "#505050"
                    anchors { left: parent.left; leftMargin: 7; top: parent.top }
                    font.pixelSize: 10
                }

                Item {
                    anchors { left: parent.left; right: parent.right }
                    anchors { top: parent.top; bottom: parent.verticalCenter; margins: 10 }

                    KnobDial {
                        id: resonance

                        width: Math.min(parent.width, parent.height); height: width
                        anchors.centerIn: parent
                        smooth: true

                        maximumvalue: 1.0; minimumvalue: 0; value: 0
                        onValueChanged: ui.resonance(maximumvalue - value)
                    }
                }

                Text {
                    text: "Cutoff"
                    color: "#505050"
                    anchors { left: parent.left; leftMargin: 7; top: parent.verticalCenter }
                    font.pixelSize: 10
                }

                Item {
                    anchors { left: parent.left; right: parent.right }
                    anchors { top: parent.verticalCenter; bottom: parent.bottom; margins: 10 }

                    KnobDial {
                        id: cutoff

                        width: Math.min(parent.width, parent.height); height: width
                        anchors.centerIn: parent
                        smooth: true

                        maximumvalue: 1.0; minimumvalue: 0.0; value: 0
                        onValueChanged: ui.cutOff(maximumvalue - value)
                    }
                }
            }

            Item {
                id: powerButtonArea

                anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
                height: parent.height / 6

                Text {
                    text: "Power"
                    color: "#505050"

                    anchors.left: parent.left; anchors.leftMargin: 7
                    font.pixelSize: 10
                }

                ImageButton {
                    id: powerbutton

                    function press() {
                        turntable.playing = !turntable.playing
                        if(turntable.playing) { arm.moveIn()  }
                        else                  { arm.moveOut() }
                    }

                    onClicked: press()

                    width: Math.min(parent.width, parent.height) * 0.9; height: width
                    anchors.centerIn: parent

                    glowColor: pressed ? "#AA00FF00" : "#AAFF0000"
                    pressed: turntable.playing
                    smooth: true
                    buttonCenterImage: "images/powerbutton.png"
                }
            }
        }

        DrumMachine {
            id: drumMachine

            y: flickable.height
            width: flickable.width; height: flickable.height
            speed: speedslider.value
        }

        states: [
            State {
                name: "TurnTable"
                PropertyChanges { target: flickable; contentY: 0 }
                PropertyChanges { target: sidepanel; turnTableButtonPressed: true }
                PropertyChanges { target: drumMachine; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
            },
            State {
                name: "DrumMachine"
                PropertyChanges { target: flickable; contentY: ui.height }
                PropertyChanges { target: sidepanel; drumMachineButtonPressed: true }
                PropertyChanges { target: turntable; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
            },
            State {
                name: "Help"
                PropertyChanges { target: flickable; contentY: -ui.height }
                PropertyChanges { target: turntable; opacity: 0 }
                PropertyChanges { target: drumMachine; opacity: 0 }
            }
        ]

        transitions: Transition {
            PropertyAnimation { property: "contentY"; easing.type: Easing.InOutQuart }
            PropertyAnimation { property: "opacity" }
        }
    }

    SidePanel {
        id: sidepanel

        width: 0.09375 * ui.width; height: ui.height
        onTurnTableClicked: flickable.setState("TurnTable")
        onDrumMachineClicked: flickable.setState("DrumMachine")
        turnTableLedOn: turntable.playing
        drumMachineLedOn: drumMachine.ledOn
    }
}
