import Qt 4.7
import Qt.labs.folderlistmodel 1.0
import "SampleSelector"
import "DrumMachine"
import "HelpScreen"

Rectangle {
    id: ui

    // Used when developing with QML Viewer, property is added as
    // context property by Qt in the real application
    property bool lowPerf: false

    signal diskSpeed(variant speed)
    signal diskAimSpeed(variant speed)
    signal start()
    signal stop()
    signal cutOff(variant value)
    signal resonance(variant value)
    signal seekToPosition(variant value)
    signal linkActivated(variant link)

    function audioPosition(pos) { arm.setPositionOnDisk(pos) }
    function inclination(deg) { diskReflection.rotation = deg * 8 + 45 }

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
        else if(event.key == Qt.Key_PageDown) {
            diskReflection.rotation += 11.25
        }
        else if(event.key == Qt.Key_PageUp) {
            diskReflection.rotation -= 11.25
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

    SidePanel {
        id: sidepanel

        width: 0.09375 * ui.width; height: ui.height
        z: 1
        onTurnTableClicked: flickable.setState("TurnTable")
        onDrumMachineClicked: flickable.setState("DrumMachine")
        onSampleSelectorClicked: flickable.setState("SampleSelector")
        turnTableLedOn: turntable.playing
        drumMachineLedOn: drumMachine.ledOn
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

        anchors { left: sidepanel.right; right: parent.right }
        anchors { bottom: parent.bottom; top: parent.top }
        contentWidth: parent.width * 2; contentHeight: parent.height * 3
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

            width:  flickable.width - mixerpanel.width - 2
            height: flickable.height
            source: "images/backgroundaluminium.png"
            fillMode: Image.Stretch

            Image {
                id: discPlate

                width: Math.min(parent.paintedWidth * 0.79,
                                parent.paintedHeight * 0.98)
                height: width

                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -0.085 * parent.paintedWidth
                anchors.verticalCenterOffset: -0.0055 * parent.paintedHeight

                source: "images/discplate.png"
                smooth: lowPerf ? false : true
            }

            Image {
                id: disc

                // speed are Hz values of the disk
                property real targetSpeed: turntable.playing ?
                                               speedslider.value : 0.0
                property real currentSpeed: 0

                anchors { fill: discPlate; margins: discPlate.width * 0.045 }
                source: "images/disk.png"
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
                        disc.rotation = (disc.rotation + 0.36 *
                                         disc.currentSpeed * interval) % 360

                        if(Math.abs(disc.currentSpeed - disc.targetSpeed) <= 0.01) {
                            disc.currentSpeed = disc.targetSpeed
                        }
                        else {
                            disc.currentSpeed += (disc.targetSpeed -
                                                  disc.currentSpeed) * 0.10
                        }
                    }
                }
            }

            Image {
                id: diskReflection

                anchors.fill: disc
                source: "images/diskreflection.png"
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

                    if(Math.sqrt(xlength * xlength + ylength *
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

                    var ax = mouse.x - centerx
                    var ay = centery - mouse.y
                    var bx = previousX - centerx
                    var by = centery - previousY

                    var angledelta = (Math.atan2(by, bx) -
                                      Math.atan2(ay, ax)) * 57.2957795

                    if(angledelta > 180)       { angledelta -= 360 }
                    else if(angledelta < -180) { angledelta += 360 }

                    disc.rotation = (disc.rotation + angledelta) % 360

                    if(now - previousTime > 0) {
                        disc.currentSpeed = angledelta * 2.77778 /
                                            (now - previousTime)
                    }

                    previousX = mouse.x
                    previousY = mouse.y
                    previousTime = now
                }
            }

            Item {
                anchors { top: speedslider.top; bottom: speedslider.bottom }
                anchors { right: speedslider.left; rightMargin: 5 }

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
                    anchors { right: parent.right; rightMargin: 15 }
                    anchors { bottom: parent.bottom; bottomMargin: 0 }
                    text: "Disk / drum speed"
                    color: "#505050"; font.pixelSize: 10
                }
            }

            SpeedSlider {
                id: speedslider

                width: parent.paintedWidth * 0.085
                height: parent.paintedHeight * 0.6

                anchors {
                    right: arm.right; rightMargin: 10
                    bottom: parent.bottom; bottomMargin: 15
                }

                maximum: 1.30; minimum: 0.50; value: 1.0; defaultValue: 1.0
                mouseAreaScale: 3
            }

            Arm {
                id: arm

                width: disc.width * 0.30; height: disc.height * 1.06
                anchors { left: discPlate.right; top: discPlate.top }
                anchors { leftMargin: discPlate.width * -0.05 }

                onArmdownChanged: armdown ? ui.start() : ui.stop()
                onArmReleasedByUser: ui.seekToPosition(position)
            }
        }

        Rectangle {
            id: mixerpanel

            x: flickable.width - mixerpanel.width
            width: flickable.width * 0.23
            height: flickable.height
            color: "#999999"
            radius: 4

            Item {
                id: buttonPanel

                anchors { left: parent.left; right: parent.right }
                anchors { top: parent.top }
                height: parent.height / 6

                Image {
                    id: closeButton

                    property bool pressed: false

                    anchors { left: parent.horizontalCenter }
                    anchors { right: parent.right; top: parent.top }
                    anchors { bottom: parent.bottom; margins: 5 }

                    source: pressed ? "images/exit_on.png" : "images/exit.png"
                    smooth: true
                    asynchronous: true

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            closeButton.pressed = true
                            closeButton.scale = 0.9
                        }

                        onReleased: {
                            closeButton.pressed = false
                            closeButton.scale = 1.0
                        }

                        onClicked: Qt.quit()
                    }
                }

                Image {
                    id: infoButton

                    property bool pressed: false

                    anchors { left: parent.left }
                    anchors { right: parent.horizontalCenter }
                    anchors { top: parent.top; bottom: parent.bottom }
                    anchors { margins: 5 }

                    source: pressed ? "images/info_on.png" : "images/info.png"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            infoButton.pressed = true
                            infoButton.scale = 0.9
                        }

                        onReleased: {
                            infoButton.pressed = false
                            infoButton.scale = 1.00
                        }

                        onClicked: flickable.setState("Help")
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right }
                anchors { top: buttonPanel.bottom }
                anchors { bottom: powerButtonArea.top }

                Text {
                    text: "Resonance"
                    color: "#505050"
                    anchors { left: parent.left; leftMargin: 7 }
                    anchors { top: parent.top }
                    font.pixelSize: 10
                }

                Item {
                    anchors { left: parent.left; right: parent.right }
                    anchors { top: parent.top; bottom: parent.verticalCenter }
                    anchors { margins: 10 }

                    KnobDial {
                        id: resonance

                        width: Math.min(parent.width, parent.height)
                        height: width

                        anchors.centerIn: parent
                        smooth: true

                        maximumvalue: 99; minimumvalue: 0; value: 0
                        onValueChanged: ui.resonance(maximumvalue / 100 -
                                                     value / 100)
                    }
                }

                Text {
                    text: "Cutoff"
                    color: "#505050"
                    anchors { left: parent.left; leftMargin: 7 }
                    anchors { top: parent.verticalCenter }
                    font.pixelSize: 10
                }

                Item {
                    anchors { left: parent.left; right: parent.right }
                    anchors { bottom: parent.bottom; margins: 10 }
                    anchors { top: parent.verticalCenter }

                    KnobDial {
                        id: cutoff

                        width: Math.min(parent.width, parent.height)
                        height: width

                        anchors.centerIn: parent
                        smooth: true

                        maximumvalue: 99; minimumvalue: 0.0; value: 0
                        onValueChanged: ui.cutOff(maximumvalue / 100 -
                                                  value / 100)
                    }
                }
            }

            Item {
                id: powerButtonArea

                anchors { left: parent.left; right: parent.right }
                anchors { bottom: parent.bottom }
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
                        if(turntable.playing) {
                            ui.seekToPosition(0)
                            arm.moveIn()
                        }
                        else {
                            arm.moveOut()
                        }
                    }

                    onClicked: press()

                    width: Math.min(parent.width, parent.height) * 0.9
                    height: width
                    anchors.centerIn: parent

                    glowColor: pressed ? "#CC00FF00" : "#CCFF0000"
                    pressed: turntable.playing
                    smooth: true
                    buttonCenterImage: "images/powerbutton.png"
                }
            }
        }

        SampleSelector {
            id: sampleSelector

            x: ui.width
            width: flickable.width; height: flickable.height
            onBackPressed: flickable.setState(flickable.prevState)

            // sampleFolder is context property set in Qt
            folder: sampleFolder
        }

        DrumMachine {
            id: drumMachine

            y: flickable.height
            width: flickable.width; height: flickable.height
            speed: speedslider.value
            onInfoPressed: flickable.setState("Help")
        }

        states: [
            State {
                name: "TurnTable"
                PropertyChanges { target: flickable; contentY: 0 }
                PropertyChanges {
                    target: sidepanel; turnTableButtonPressed: true
                }
                PropertyChanges { target: drumMachine; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
                PropertyChanges { target: sampleSelector; opacity: 0 }
            },
            State {
                name: "SampleSelector"
                PropertyChanges {
                    target: flickable; contentX: ui.width; contentY: 0
                }
                PropertyChanges {
                    target: sidepanel; sampleSelectorButtonPressed: true
                }
                PropertyChanges { target: turntable; opacity: 0 }
                PropertyChanges { target: mixerpanel; opacity: 0 }
                PropertyChanges { target: drumMachine; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
            },
            State {
                name: "DrumMachine"
                PropertyChanges { target: flickable; contentY: ui.height }
                PropertyChanges {
                    target: sidepanel; drumMachineButtonPressed: true
                }
                PropertyChanges { target: turntable; opacity: 0 }
                PropertyChanges { target: mixerpanel; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
                PropertyChanges { target: sampleSelector; opacity: 0 }
            },
            State {
                name: "Help"
                PropertyChanges { target: flickable; contentY: -ui.height }
                PropertyChanges { target: turntable; opacity: 0 }
                PropertyChanges { target: mixerpanel; opacity: 0 }
                PropertyChanges { target: drumMachine; opacity: 0 }
                PropertyChanges { target: sampleSelector; opacity: 0 }
            }
        ]

        transitions: Transition {
            PropertyAnimation {
                properties: "contentX,contentY"; easing.type: Easing.InOutQuart
            }
            PropertyAnimation { property: "opacity" }
        }
    }
}
