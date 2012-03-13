/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0
import "SampleSelector"
import "DrumMachine"
import "Turntable"
import "MixerPanel"
import "HelpScreen"

Rectangle {
    id: ui
    anchors.fill: parent
    width: 640
    height: 360
    color: "black"
    focus: true

    // Used when developing with QML Viewer, property is added as
    // context property by Qt in the real application
    // property bool lowPerf: false

    signal diskSpeed(variant speed)
    signal diskAimSpeed(variant speed)
    signal start()
    signal stop()
    signal cutOff(variant value)
    signal resonance(variant value)
    signal seekToPosition(variant value)

    function audioPosition(pos) {
        turntable.setPositionOnDisk(pos)
    }

    function inclination(deg) {
        turntable.diskReflection.rotation = deg * 2 + 45
    }

    function powerOff() {
        turntable.playing = false
        turntable.moveOut()
    }

    function powerButtonPressed() {
        turntable.playing = !turntable.playing

        if (turntable.playing) {
            seekToPosition(0)
            turntable.moveIn()
        } else {
            turntable.moveOut()
        }
    }

    Keys.onDownPressed: flickable.setState("DrumMachine")
    Keys.onUpPressed: flickable.setState("Turntable")
    Keys.onSpacePressed: powerbutton.press()
    Keys.onLeftPressed: drumMachine.selectedTickGroup = 1
    Keys.onRightPressed: drumMachine.selectedTickGroup = 2
    Keys.onPressed: {
        if (event.key === 56 || event.key === Qt.Key_I) {
            flickable.setState("Help")
            event.accepted = true
        } else if (event.key === 115 || event.key === Qt.Key_S) {
            flickable.setState("SampleSelector")
            event.accepted = true
        } else if (event.key === Qt.Key_PageDown) {
            diskReflection.rotation += 11.25
            event.accepted = true
        } else if (event.key === Qt.Key_PageUp) {
            diskReflection.rotation -= 11.25
            event.accepted = true
        } else if (event.key === Qt.Key_Backspace) {
            if(flickable.state == "Help") {
                helpScreen.backPressed()
                event.accepted = true
            } else if (flickable.state == "SampleSelector") {
                sampleSelector.backPressed()
                event.accepted = true
            }
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            drumMachine.running = !drumMachine.running
            event.accepted = true
        }
    }

    Component.onCompleted: {
        flickable.setState("Turntable")
        turntable.start()
    }

    SidePanel {
        id: sidepanel

        width: 0.09375 * ui.width
        height: ui.height
        z: 1
        onTurntableClicked: flickable.setState("Turntable")
        onDrumMachineClicked: flickable.setState("DrumMachine")
        onSampleSelectorClicked: flickable.setState("SampleSelector")
        turntableLedOn: turntable.playing
        drumMachineLedOn: drumMachine.ledOn
    }

    Flickable {
        id: flickable

        property string prevState: ""

        function setState(newState) {
            if (newState !== state) {
                prevState = state
                state = newState
            }
        }

        anchors {
            left: sidepanel.right
            right: parent.right
            bottom: parent.bottom
            top: parent.top
        }
        contentWidth: parent.width * 2
        contentHeight: parent.height * 3
        interactive: false

        HelpScreen {
            id: helpScreen

            width: flickable.width
            height: flickable.height
            y: -flickable.height

            onBackPressed: flickable.setState(flickable.prevState)
        }

        Turntable {
            id: turntable

            width:  flickable.width - mixerpanel.width - 2
            height: flickable.height
        }

        MixerPanel {
            id: mixerpanel
            x: flickable.width - mixerpanel.width
            width: flickable.width * 0.23
            height: flickable.height
        }

        SampleSelector {
            id: sampleSelector

            anchors.left: mixerpanel.right
            width: flickable.width; height: flickable.height
            onBackPressed: flickable.setState(flickable.prevState)

            // sampleFolder is context property and it is set from Qt
            folder: sampleFolder
        }

        DrumMachine {
            id: drumMachine

            y: flickable.height
            width: flickable.width; height: flickable.height
            speed: turntable.speedSliderValue
            // The exitButtonVisible context property is set to false
            // on harmattan.
            exitButtonOpacity: exitButtonVisible ? 1 : 0
            onInfoPressed: flickable.setState("Help")
        }

        states: [
            State {
                name: "Turntable"
                PropertyChanges { target: flickable; contentY: 0 }
                PropertyChanges {
                    target: sidepanel; turntableButtonPressed: true
                }
                PropertyChanges { target: drumMachine; opacity: 0 }
                PropertyChanges { target: helpScreen; opacity: 0 }
                PropertyChanges { target: sampleSelector; opacity: 0 }
            },
            State {
                name: "SampleSelector"
                PropertyChanges {
                    target: flickable; contentX: sampleSelector.x; contentY: 0
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
                properties: "contentX,contentY"
                easing.type: Easing.InOutQuart
            }
            PropertyAnimation {
                property: "opacity"
            }
        }
    }
}
