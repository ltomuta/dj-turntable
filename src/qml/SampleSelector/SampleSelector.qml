/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0

Image {
    id: selector
    objectName: "sampleSelector"

    property string sampleFile
    property alias folder: folderModel.folder

    // Used as Qt signals
    signal sampleSelected(variant sample)
    signal defaultSample()

    // QML signal
    signal backPressed()

    // Called by Qt
    function setCurrentSample(filePath) {
        sampleFile = filePath
    }

    // Called by Qt
    function showError(file, error) {
        errorDialog.show("Failed to load sample:\n" + file + "\n\n" + error)
    }

    // QML function
    function setFolder(folder) {
        folderAnimation.folderToChange = folder
        folderAnimation.start()
    }

    width: 640
    height: 360
    source: "../images/backgroundaluminium.png"

    Image {
        id: backButton

        property bool pressed: false

        anchors {
            top: parent.top
            right: parent.right
            rightMargin: 5
            topMargin: 5
        }
        width: parent.width * 0.10
        height: width * 0.83607
        source: pressed ? "../images/back_on.png"
                        : "../images/back.png"
        smooth: true

        MouseArea {
            anchors.fill: parent
            onPressed: {
                backButton.pressed = true
                backButton.scale = 0.9
            }

            onReleased: {
                backButton.pressed = false
                backButton.scale = 1.0
            }

            onClicked: selector.backPressed()
        }
    }

    Image {
        id: defaultSampleButton

        property bool pressed: false

        anchors {
            top: backButton.bottom
            right: parent.right
            rightMargin: 5
            topMargin: 10
        }
        width: backButton.width
        height: backButton.height
        source: pressed ? "../images/defaultsample_on.png"
                        : "../images/defaultsample.png"
        smooth: true

        MouseArea {
            anchors.fill: parent
            onPressed: {
                defaultSampleButton.pressed = true
                defaultSampleButton.scale = 0.9
            }

            onReleased: {
                defaultSampleButton.pressed = false
                defaultSampleButton.scale = 1.0
            }

            onClicked: selector.defaultSample()
        }
    }


    Image {
        id: folderUp

        property bool pressed: false

        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            leftMargin: 35
        }

        width: height; height: 40
        source: "../images/iconfolderup.png"
        smooth: true
    }


    Text {
        id: currentFolderTitle

        anchors {
            verticalCenter: folderUp.verticalCenter
            left: folderUp.right
            leftMargin: 10
            right: backButton.left
            rightMargin: 20
        }

        text: folderModel.folder
        color: "#505050"
        elide: Text.ElideLeft
    }

    MouseArea {
        anchors {
            top: parent.top
            bottom: folderHole.top
            left: folderUp.left
            right: folderHole.right
            rightMargin: 20
        }

        onPressed: {
            folderUp.pressed = true
            folderUp.scale = 0.9
        }

        onReleased: {
            folderUp.pressed = false
            folderUp.scale = 1.0
        }

        onClicked: selector.setFolder(folderModel.parentFolder)
    }


    BorderImage {
        id: folderHole

        anchors {
            top: folderUp.bottom; topMargin: 10
            left: parent.left; leftMargin: 20
            right: backButton.left; rightMargin: 20
            bottom: nowPlayingText.top; bottomMargin: 10
        }

        source: "../images/buttonpressed.sci"
        clip: true

        FolderListModel {
            id: folderModel

            folder: "file:/c:/"
            nameFilters: [ "*.wav", "*.ogg" ]
        }

        Component {
            id: folderDelegate

            Item {
                width: view.width
                height: 40

                Behavior on scale { PropertyAnimation { duration: 50 } }

                Image {
                    id: icon
                    width: height
                    height: parent.height
                    source: folderModel.isFolder(index) ? "../images/iconfolder.png"
                                                        : "../images/iconsample.png"
                    smooth: true
                }

                Text {
                    anchors {
                        left: icon.right
                        leftMargin: 5
                        verticalCenter: parent.verticalCenter
                    }
                    text: fileName
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (fileName === "") {
                            return
                        }

                        if (folderModel.isFolder(index)) {
                            selector.setFolder(filePath)
                        } else {
                            selector.sampleSelected(filePath)
                        }
                    }
                }
            }
        }

        ListView {
            id: view

            anchors {
                fill: parent
                margins: 15
            }

            model: folderModel
            spacing: 10
            delegate: folderDelegate

            SequentialAnimation {
                id: folderAnimation

                property string folderToChange

                PropertyAnimation {
                    target: view
                    property: "opacity"
                    to: 0
                    duration: 100
                }

                PropertyAction {
                    target: folderModel
                    property: "folder"
                    value: folderAnimation.folderToChange
                }

                PropertyAnimation {
                    target: view
                    property: "opacity"
                    to: 1.0
                    duration: 100
                }
            }
        }
    }

    Text {
        id: nowPlayingText

        anchors {
            left: folderHole.left
            leftMargin: 5
            bottom: parent.bottom
            bottomMargin: 10
        }

        text: "Now playing: "
        color: "#505050"
    }

    Text {
        color: "white"
        text: selector.sampleFile
        elide: Text.ElideLeft
        anchors {
            left: nowPlayingText.right
            leftMargin: 5
            top: nowPlayingText.top
            right: backButton.left
        }
    }

    Dialog {
        id: errorDialog

        anchors.centerIn: parent
        radius: 10
    }
}
