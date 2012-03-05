/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0
import "../Common"

Rectangle {
    id: mixerpanel
    color: "#999999"
    radius: 4

    Item {
        id: buttonPanel

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: parent.height / 6

        Image {
            id: closeButton

            property bool pressed: false

            anchors {
                left: parent.horizontalCenter
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 5
            }

            source: pressed ? "../images/exit_on.png" : "../images/exit.png"
            smooth: true
            asynchronous: true
            // The exitButtonVisible is context property which is set to false
            // in harmattan target.
            opacity: exitButtonVisible ? 1 : 0

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

            anchors {
                left: exitButtonVisible ? parent.left : parent.horizontalCenter
                right: exitButtonVisible ? parent.horizontalCenter : parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 5
            }

            source: pressed ? "../images/info_on.png" : "../images/info.png"
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
        anchors {
            left: parent.left
            right: parent.right
            top: buttonPanel.bottom
            bottom: powerButtonArea.top
        }

        Text {
            text: "Resonance"
            color: "#505050"
            anchors {
                left: parent.left
                leftMargin: 7
                top: parent.top
            }
            font.pixelSize: 10
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.verticalCenter
                margins: 10
            }

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
            anchors {
                left: parent.left
                leftMargin: 7
                top: parent.verticalCenter
            }
            font.pixelSize: 10
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10
                top: parent.verticalCenter
            }

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

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 6

        Text {
            text: "Power"
            color: "#505050"

            anchors.left: parent.left; anchors.leftMargin: 7
            font.pixelSize: 10
        }

        ImageButton {
            id: powerbutton

            onClicked: ui.powerButtonPressed()

            width: Math.min(parent.width, parent.height) * 0.9
            height: width
            anchors.centerIn: parent

            glowColor: pressed ? "#CC00FF00" : "#CCFF0000"
            pressed: turntable.playing
            smooth: true
            buttonCenterImage: "../images/powerbutton.png"
        }
    }
}
