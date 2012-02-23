/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Rectangle {
    id: sidepanel

    signal turntableClicked()
    signal drumMachineClicked()
    signal sampleSelectorClicked()

    property bool sampleSelectorButtonPressed: false
    property bool turntableButtonPressed: false
    property bool drumMachineButtonPressed: false
    property bool turntableLedOn: false
    property bool drumMachineLedOn: false

    color: "black"
    width: 100
    height: 400

    BorderImage {
        id: settingsButton

        anchors {
            left: parent.left
            right: parent.right
            rightMargin: 2
        }
        height: width
        source: sidepanel.sampleSelectorButtonPressed
                ? "images/buttonpressed.sci"
                : "images/buttonup.sci"

        Image {
            anchors.centerIn: parent
            width: parent.width * 0.5
            height: width
            source: "images/iconsampleselector.png"
            smooth: true
        }

        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.sampleSelectorClicked()
        }
    }

    BorderImage {
        id: turntableButton

        anchors {
            left: parent.left
            right: parent.right
            rightMargin: 2
            top: settingsButton.bottom
            topMargin: 2
        }

        height: (parent.height - settingsButton.height) / 2 - 1
        smooth: true
        source: sidepanel.turntableButtonPressed
                ? "images/buttonpressed.sci"
                : "images/buttonup.sci"

        Image {
            anchors {
                top: parent.top
                topMargin: parent.height * 0.1
                horizontalCenter: parent.horizontalCenter
            }
            width: parent.width * 0.6
            height: width
            smooth: true

            source: sidepanel.turntableLedOn
                    ? "images/led_on.png"
                    : "images/led_off.png"
        }

        Image {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: parent.height * 0.05
            }
            width: parent.width * 0.6
            height: width
            smooth: true
            source: "images/icon_turntable.png"
        }


        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.turntableClicked()
        }
    }

    BorderImage {
        id: drumMachineButton

        anchors {
            left: parent.left
            right: parent.right
            rightMargin: 2
            top: turntableButton.bottom
            topMargin: 2
            bottom: parent.bottom
        }

        smooth: true

        source: sidepanel.drumMachineButtonPressed
                ? "images/buttonpressed.sci"
                : "images/buttonup.sci"

        Image {
            anchors {
                top: parent.top
                topMargin: parent.height * 0.1
                horizontalCenter: parent.horizontalCenter
            }
            width: parent.width * 0.6
            height: width
            smooth: true

            source: sidepanel.drumMachineLedOn
                    ? "images/led_on.png"
                    : "images/led_off.png"
        }

        Image {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: parent.height * 0.05
            }
            width: parent.width * 0.6
            height: width
            smooth: true
            source: "images/icon_drummachine.png"
        }


        MouseArea {
            anchors.fill: parent
            onPressed: sidepanel.drumMachineClicked()
        }
    }
}
