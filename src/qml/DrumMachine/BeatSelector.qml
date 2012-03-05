/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0
import "../Common"

Item {
    id: selector

    property int index
    property variant pressedButton: -1
    property alias buttonWidth: predefined.buttonWidth

    function buttonPressed(button, i) {
        if (pressedButton !== -1) {
            pressedButton.pressed = false
        }
        pressedButton = button
        index = i
        pressedButton.pressed = true
    }

    Component.onCompleted: selector.buttonPressed(first, 0)

    width: 450; height: 50

    Text {
        id: predefinedText

        text: "Predefined"
        color: "#505050"
        font.pixelSize: 10
    }

    Row {
        id: predefined

        property real buttonWidth: Math.min(width / 3 - spacing, height)

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: predefinedText.right
            right: parent.horizontalCenter
            leftMargin: 10
            rightMargin: 10
        }

        spacing: 5

        ImageButton {
            id: first
            width: predefined.buttonWidth; height: width
            index: 0; buttonCenterImage: "../images/button1.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 1; buttonCenterImage: "../images/button2.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 2; buttonCenterImage: "../images/button3.png"
            onClicked: selector.buttonPressed(button, index)
        }
    }

    Text {
        id: userDefinedText

        anchors.left: parent.horizontalCenter
        text: "User defined"
        color: "#505050"
        font.pixelSize: 10
    }

    Row {
        id: userDefined

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: userDefinedText.right
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }
        spacing: 5

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 3; buttonCenterImage: "../images/button1.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 4; buttonCenterImage: "../images/button2.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 5; buttonCenterImage: "../images/button3.png"
            onClicked: selector.buttonPressed(button, index)
        }
    }
}
