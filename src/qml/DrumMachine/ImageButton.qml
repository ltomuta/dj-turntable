/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Image {
    id: button

    signal clicked(variant button, int index)

    property int index: -1
    property bool pressed: false
    property alias buttonCenterImage: buttonCenter.source
    property alias glowColor: glowColor.color

    width: 50; height: 50
    smooth: true
    source: "../images/buttonedge.png"

    Rectangle {
        id: glowColor
        anchors.centerIn: parent
        width: 0.5 * parent.width
        height: 0.5 * parent.height
        z: -1
        color: button.pressed ? "#CCFF0000" : "#CC202020"
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    Image {
        id: buttonCenter

        anchors.fill: parent
        scale: button.pressed ? 0.85 : 0.98
        Behavior on scale { PropertyAnimation { duration: 100 } }

        source: "../images/button1.png"
        smooth: true
    }

    MouseArea {
        anchors.fill: parent
        onPressed: button.clicked(button, button.index)
    }
}
