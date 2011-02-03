import Qt 4.7
import Qt.labs.folderlistmodel 1.0

Image {
    id: selector

    property string sampleFile: defaultSampleFile
    property string defaultSampleFile: ":/sounds/ivory.wav"
    property alias folder: folderModel.folder

    signal backPressed()
    signal sampleSelected()

    width: 640; height: 360
    source: "images/backgroundaluminium.png"

    Image {
        id: backButton

        property bool pressed: false

        anchors { top: parent.top; right: parent.right }
        anchors { rightMargin: 5; topMargin: 5 }
        width: parent.width * 0.10
        height: width * 0.83607
        source: pressed ? "images/back_on.png" :
                          "images/back.png"
        smooth: true

        MouseArea {
            anchors.fill: parent
            onPressed: {
                backButton.pressed = true; backButton.scale = 0.9
            }

            onReleased: {
                backButton.pressed = false; backButton.scale = 1.0
            }

            onClicked: selector.backPressed()
        }
    }

    Image {
        id: defaultSampleButton

        property bool pressed: false

        anchors { top: backButton.bottom; right: parent.right }
        anchors { rightMargin: 5; topMargin: 10 }
        width: backButton.width
        height: backButton.height
        source: pressed ? "images/defaultsample_on.png" :
                          "images/defaultsample.png"
        smooth: true

        MouseArea {
            anchors.fill: parent
            onPressed: {
                defaultSampleButton.pressed = true; defaultSampleButton.scale = 0.9
            }

            onReleased: {
                defaultSampleButton.pressed = false; defaultSampleButton.scale = 1.0
            }

            onClicked: {
                selector.sampleFile = selector.defaultSampleFile
                selector.sampleSelected()
            }
        }
    }

    Item {
        id: title

        anchors {
            left: parent.left; leftMargin: 20
            right: backButton.left; rightMargin: 20
            top: parent.top; topMargin: 20
        }
        height: 20


        Image {
            id: folderUp

            property bool pressed: false

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left; leftMargin: 15
            }

            width: height; height: 30
            source: "images/iconfolderup.png"
            smooth: true

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    folderUp.pressed = true; folderUp.scale = 0.9
                }

                onReleased: {
                    folderUp.pressed = false; folderUp.scale = 1.0
                }

                onClicked: folderModel.folder = folderModel.parentFolder
            }
        }


        Text {
            anchors {
                left: folderUp.right; leftMargin: 10
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            text: sampleFile
            color: "#505050"
            elide: Text.ElideLeft
        }
    }

    Image {
        anchors {
            top: title.bottom; topMargin: 10
            left: parent.left; leftMargin: 20
            right: backButton.left; rightMargin: 20
            bottom: parent.bottom; bottomMargin: 20
        }

        source: "images/sampleselectorlisting.png"
        clip: true

        FolderListModel {
            id: folderModel

            folder: "file:/c:/"
            nameFilters: [ "*.wav" ]
        }

        Component {
            id: folderDelegate

            Item {
                width: view.width
                height: 30

                Behavior on scale { PropertyAnimation { duration: 50 } }

                Image {
                    id: icon
                    height: parent.height
                    width: height

                    source: folderModel.isFolder(index) ? "images/iconfolder.png"
                                                        : "images/iconsample.png"
                    smooth: true
                }

                Text {
                    anchors {
                        left: icon.right; leftMargin: 5
                        verticalCenter: parent.verticalCenter
                    }
                    text: fileName
                    color: "white"//"#505050"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if(fileName == "") {
                            return;
                        }

                        if(folderModel.isFolder(index)) {
                            folderModel.folder = filePath
                        }
                        else {
                            sampleFile = filePath
                            selector.sampleSelected()
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
            spacing: 2
            delegate: folderDelegate
        }
    }
}
