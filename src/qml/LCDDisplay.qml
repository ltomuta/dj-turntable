import Qt 4.7


Rectangle {
    id: display

    property int number: -1

    width: digit1.width + digit2.width; height: digit1.height
    color: "black"

    onNumberChanged: {
        digit2.number = number % 10
        digit1.number = Math.floor(number / 10)
    }

    LCDDigit {
        id: digit1
    }

    LCDDigit {
        id: digit2

        anchors.left: digit1.right; anchors.leftMargin: 2
    }
}
