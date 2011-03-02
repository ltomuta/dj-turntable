import QtQuick 1.0

Rectangle {
    id: helpScreen

    signal backPressed()

    property real textSize: Math.min(width, height) * 0.04 <= 0
                            ? 8
                            : Math.min(width, height) * 0.02

    width: 500; height: 360
    radius: 4
    color: "#999999"
    clip: true

    Flickable {
        id: flickable

        anchors { fill: parent; margins: 20 }
        contentHeight: column.height

        Column {
            id: column

            width: parent.width

            Item {
                width: parent.width; height: projectInfo.height

                Text {
                    id: projectInfo

                    anchors { left: parent.left; right: backButton.left }
                    anchors { rightMargin: 10 }
                    color: "white"

                    width: parent.width
                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText
                    font.pointSize: helpScreen.textSize

                    text: "<b><h2>DJ Turntable v.1.2.2</b></h2>" +
                          "DJ Turntable is a Forum Nokia example that " +
                          "demonstrates integrating a Qt Quick application " +
                          "to Qt audio interface. See more information " +
                          "about the project at <a href=\"https://projects." +
                          "forum.nokia.com/turntable\">https://projects." +
                          "forum.nokia.com/turntable</a>." +
                          "<h3>Turntable</h3>" +
                          "<p>Play the looping sample with a realistic " +
                          "turntable. The sample can be scratched with " +
                          "finger, played faster, slower and backwards. " +
                          "The speed of the disk can be adjusted with the " +
                          "speed slider, the default 1x speed can be " +
                          "obtained by double clicking the speed slider " +
                          "knob. DJ Turntable offers Cutoff and Resonance " +
                          "to alternate the sample in real time. The knobs " +
                          "are rotated by moving the finger up and down on " +
                          "top of them.</p>" +
                          "<h3>Sample selector</h3>" +
                          "<p>Use the sample selector to change the sample " +
                          "that turntable is playing by selecting the " +
                          "desired sample from the directory view. " +
                          "Following uncompressed wav-formats are " +
                          "supported:</p>" +
                          "8 bit unsigned<br>" +
                          "16 bit unsigned<br>" +
                          "32 bit float" +
                          "<p>The application will open the last selected " +
                          "sample on startup. Use the default sample button " +
                          "on the right under the back button to reset back " +
                          "to the default sample.</p>" +
                          "<h3>Drum machine</h3>" +
                          "<p>Play and edit the drum beats. There are three " +
                          "predefined beats which can be played and edited " +
                          "but the edits are not stored. For the user there " +
                          "are three beats that are saved to the devices " +
                          "memory whenever they are edited. Use the beat " +
                          "selector buttons on the bottom of the view to " +
                          "switch between the beats. All the beats are 32 " +
                          "ticks long and they contain 6 different drum " +
                          "samples: hi-hat, hi-hat open, bass drum, snare, " +
                          "cymbal and cow bell.</p><p>The drum machine will " +
                          "play all drum beats at 150 bpm. Changing the " +
                          "speed of the turntable will affect to the playing " +
                          "speed of the drum machine accordingly.</p>" +
                          "<h3>Keyboard shortcuts</h3>" +
                          "<p>The following keyboard shortcuts exist:</p>" +
                          "Camera zoom up = Volume up<br>" +
                          "Camera zoom down = Volume down<br>" +
                          "Space = Start / stop the turntable<br>" +
                          "Return = Start / stop the drum machine<br>" +
                          "Key up = Go to the turntable view<br>" +
                          "Key down = Go to the drum machine view<br>" +
                          "Key left = View the 1st tick group in the drum " +
                          "machine<br>" +
                          "Key right = View the 2nd tick group in the drum " +
                          "machine<br>" +
                          "Key i = Go to the info view<br>" +
                          "Key s = Go to the sample selector view<br>" +
                          "Backspace = Return from the info or sample " +
                          "selector view to the previous view" +
                          "<h3>Samples</h3>" +
                          "<p>The turntable melody sample <i>ivory.wav</i> " +
                          "was created by nick <i>Flick3r</i> and it was " +
                          "downloaded from " +
                          "<a href=\"http://www.freesound.org\">" +
                          "freesound.org</a>. The sample follows the " +
                          "<a href=\"http://creativecommons.org/licenses/" +
                          "sampling+/1.0/\">Creative Commons Sampling Plus " +
                          "1.0</a> license.</p>"

                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Image {
                    id: backButton

                    property bool pressed: false

                    anchors { top: parent.top; right: parent.right }
                    anchors { rightMargin: -15; topMargin: -15 }
                    width: helpScreen.width * 0.10
                    height: helpScreen.height / 6
                    source: pressed ? "../images/back_on.png" :
                    "../images/back.png"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            backButton.pressed = true; backButton.scale = 0.9
                            flickable.interactive = false
                        }

                        onReleased: {
                            backButton.pressed = false; backButton.scale = 1.0
                            flickable.interactive = true
                        }

                        onClicked: helpScreen.backPressed()
                    }
                }
            }
        }
    }
}
