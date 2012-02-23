/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Rectangle {
    id: display

    property int number: -1
    property color segmentColor: "red"
    property color segmentOffColor: "#333333"
    property real segmentWidth: 2

    onNumberChanged: {
        var numbersMask = [119, 36, 93, 109, 46, 107, 123, 37, 127, 111, 91]
        for (var i = 0; i < display.children.length; i++) {
            display.children[i].lit = (numbersMask[number] & (1<<i))
        }
    }

    width: 11; height: 16
    color: "black"

    Rectangle {
        id: segment1

        property bool lit: false

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            leftMargin: display.segmentWidth + 1
            rightMargin: display.segmentWidth + 1
        }
        height: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment2

        property bool lit: false

        anchors {
            left: parent.left
            bottom: parent.verticalCenter
            top: parent.top
            topMargin: 0
            bottomMargin: 1
        }
        width: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment3

        property bool lit: false

        anchors {
            right: parent.right
            bottom: parent.verticalCenter
            top: parent.top
            topMargin: 0
            bottomMargin: 1
        }
        width: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment4

        property bool lit: false

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: display.segmentWidth + 1
            rightMargin: display.segmentWidth + 1
        }
        height: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment5

        property bool lit: false

        anchors {
            left: parent.left
            bottom: parent.bottom
            top: parent.verticalCenter
            topMargin: 0
            bottomMargin: 0
        }
        width: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment6

        property bool lit: false

        anchors {
            right: parent.right
            bottom: parent.bottom
            top: parent.verticalCenter
            topMargin: 0
            bottomMargin: 0
        }
        width: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }

    Rectangle {
        id: segment7

        property bool lit: false

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: display.segmentWidth + 1
            rightMargin: display.segmentWidth + 1
        }
        height: display.segmentWidth
        color: lit ? display.segmentColor : display.segmentOffColor
    }
}
