import Qt 4.7

Item {
    id: selector

    property int index
    property variant pressedButton: -1

    function buttonPressed(button, i) {
        if(pressedButton != -1) { pressedButton.pressed = false }
        pressedButton = button
        index = i
        pressedButton.pressed = true
    }

    Component.onCompleted: selector.buttonPressed(first, 0)

    width: 450
    height: 50

    Row {
        id: predefined

        spacing: 5

        Text { text: "Predefined"; color: "#505050"; font.pixelSize: 10 }
        ImageButton { id: first; buttonCenterImage: "button1.png"; index: 0; onClicked: selector.buttonPressed(button, index) }
        ImageButton { buttonCenterImage: "button2.png"; index: 1; onClicked: selector.buttonPressed(button, index) }
        ImageButton { buttonCenterImage: "button3.png"; index: 2; onClicked: selector.buttonPressed(button, index) }
    }

    Row {
        id: userDefined

        spacing: 5
        anchors.left: predefined.right; anchors.leftMargin: 10

        Text { text: "User defined"; color: "#505050"; font.pixelSize: 10 }
        ImageButton { buttonCenterImage: "button1.png"; index: 3; onClicked: selector.buttonPressed(button, index) }
        ImageButton { buttonCenterImage: "button2.png"; index: 4; onClicked: selector.buttonPressed(button, index) }
        ImageButton { buttonCenterImage: "button3.png"; index: 5; onClicked: selector.buttonPressed(button, index) }
    }
}
