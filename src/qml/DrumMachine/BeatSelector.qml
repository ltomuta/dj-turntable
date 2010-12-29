import Qt 4.7

Item {
    id: selector

    property int index
    property variant pressedButton: -1
    property alias buttonWidth: predefined.buttonWidth

    function buttonPressed(button, i) {
        if(pressedButton != -1) { pressedButton.pressed = false }
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

        anchors { left: predefinedText.right; right: parent.horizontalCenter }
        anchors { top: parent.top; bottom: parent.bottom }
        anchors { leftMargin: 10; rightMargin: 10 }

        spacing: 5

        ImageButton {
            id: first;
            width: predefined.buttonWidth; height: width
            index: 0; buttonCenterImage: "button1.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 1; buttonCenterImage: "button2.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 2; buttonCenterImage: "button3.png"
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

        anchors { left: userDefinedText.right; right: parent.right }
        anchors { top: parent.top; bottom: parent.bottom }
        anchors { leftMargin: 10; rightMargin: 10 }
        spacing: 5

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 3; buttonCenterImage: "button1.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 4; buttonCenterImage: "button2.png"
            onClicked: selector.buttonPressed(button, index)
        }

        ImageButton {
            width: predefined.buttonWidth; height: width
            index: 5; buttonCenterImage: "button3.png"
            onClicked: selector.buttonPressed(button, index)
        }
    }
}
