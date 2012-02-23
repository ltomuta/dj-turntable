/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0

Item {
    id: button

    signal buttonToggled()

    property int tick: -1
    property int sample: -1
    property bool pressed: false

    property string selectedSource
    property string unselectedSource

    width: 34; height: 40

    Image {
        id: image

        anchors {
            fill: parent
            margins: 3
        }
        source: button.pressed ? selectedSource : unselectedSource
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.pressed = !button.pressed
            if (tick !== -1 || sample !== -1) {
                button.buttonToggled()
            }
        }
    }
}
