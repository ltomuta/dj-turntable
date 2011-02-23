import QtQuick 1.0


Rectangle {
    id: display

    property int number: -1

    width: digit2.x + digit2.width + 2 ; height: digit1.y + digit1.height + 2
    color: "black"

    onNumberChanged: {
        digit2.number = number % 10
        digit1.number = Math.floor(number / 10)
    }

    LCDDigit {
        id: digit1
        x: 2; y: 2
    }

    LCDDigit {
        id: digit2

        anchors.left: digit1.right; anchors.leftMargin: 2
        anchors.top: digit1.top
    }
}
