/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.0

Rectangle {
    id: page

    function forceClose() {
        page.opacity = 0;
    }

    function show(txt) {
        dialogText.text = txt
        page.opacity = 0.95
        timer.start()
    }

    width: dialogText.width + 40
    height: dialogText.height + 40
    color: "#373737"
    border.width: 1
    opacity: 0

    Behavior on opacity {
        NumberAnimation {
            duration: 400
        }
    }

    Text {
        id: dialogText

        anchors.centerIn: parent
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: forceClose()
    }

    Timer {
        id: timer

        interval: 5000
        onTriggered: page.forceClose()
    }
}

