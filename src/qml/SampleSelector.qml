import Qt 4.7
import Qt.labs.folderlistmodel 1.0

Image {
    id: selector

    property string sampleFile: "qrc:/sounds/ivory.wav"

    signal backPressed()
    signal sampleSelected()

    width: 640; height: 360
    source: "images/backgroundaluminium.png"

    Image {
        id: backButton

        property bool pressed: false

        anchors { top: parent.top; right: parent.right }
        anchors { rightMargin: 5; topMargin: 5 }
        width: selector.width * 0.10
        height: selector.height / 6
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

    Item {
        id: title

        anchors {
            left: parent.left; leftMargin: 20
            right: parent.right
            top: parent.top; topMargin: 20
        }
        height: 20

        Text {
            text: "Sample: " + sampleFile
            color: "#505050"
        }
    }

    Image {
        anchors {
            top: title.bottom; topMargin: 10
            left: parent.left; leftMargin: 20
            right: backButton.left; rightMargin: 10
            bottom: parent.bottom; bottomMargin: 20
        }

        source: "images/sampleselectorlisting.png"
        clip: true

        FolderListModel {
            id: folderModel

            folder: "file:/c:/"
            nameFilters: [ "*.wav", "*.mp3" ]
            showDotAndDotDot: true
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
                margins: 20

            }

            model: folderModel
            spacing: 2
            delegate: folderDelegate
        }
    }
}
