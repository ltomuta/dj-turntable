import Qt 4.7

Rectangle {
    id: helpScreen

    signal linkActivated(variant link)

    width: 500; height: 360
    radius: 15
    color: "gray"
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        anchors.margins: 20
        contentHeight: column.height

        Column {
            id: column

            x: 10
            width: parent.width - 20

            Text {
                id: projectInfo

                color: "white"
                width: parent.width
                wrapMode: Text.WordWrap
                textFormat: Text.RichText

                text: "<b>Dj Turntable</b><br>" +
                      "Dj Turntable is demonstration of integrating Qt Quick application to Qt audio interface. " +
                      "See more information about the project at " +
                      "<a href=\"https://projects.forum.nokia.com/turntable\">https://projects.forum.nokia.com/turntable</a>.<br>"
            }

            Text {
                id: features

                color: "white"
                width: parent.width
                wrapMode: Text.WordWrap
                textFormat: Text.RichText

                text: "<b>Turntable</b><br>" +
                      "Play the looping sample with a realistic turntable. " +
                      "The sample can be scratched by the finger, played faster / slower and backwards. " +
                      "The speed of the disk can be adjusted with the speed slider, the default speed 1x can be " +
                      "obtained by double clicking the speed slider knob. Dj Turntable offers Cutoff and Resonance " +
                      "to alternate the sample in real time. The knobs are rotated by moving the finger up and down " +
                      "on top of the knobs.<br><br>" +
                      "<b>Drum machine</b><br>" +
                      "Play and edit the drum beats. There are four predefined beats to play and edit but which " +
                      "are not saved. For the user there are four beats that are saved to the devices memory " +
                      "whenever they are edited. Use the beat selector on the bottom right corner of the view to " +
                      "iterate through the beats. All the beats are 32 ticks long and they accommondate 6 different " +
                      "drum samples: hi-hat, hi-hat open, bass drum, snare, cymbal and cow bell.<br>"
            }

            Text {
                id: keyboardHelp

                color: "white"
                width: parent.width
                wrapMode: Text.WordWrap
                textFormat: Text.RichText

                text: "<b>Keyboard shortcuts</b><br>" +
                      "There are few keyboard shortcuts implemented into the application:<br>" +
                      "Camera zoom up = Volume up<br>" +
                      "Camera zoom down = Volume down<br>" +
                      "Space = Start / stop turntable<br>" +
                      "Key up = Go to turntable view<br>" +
                      "Key down = Go to drum machine view<br>" +
                      "Key left = View the 1st tick group in drum machine<br>" +
                      "Key right = View the 2nd tick group in drum machine<br>" +
                      "Key i = Go to info view<br>"
            }

            Text {
                id: freesoundlicense

                color: "white"
                width: parent.width
                wrapMode: Text.WordWrap
                textFormat: Text.RichText

                text: "<b>Samples</b><br>" +
                      "The turntable melody sample <i>ivory.wav</i> was created by nick <i>Flick3r</i> " +
                      "and was downloaded from <a href=\"http://www.freesound.org\">freesound.org</a>. The sample follows the " +
                      "<a href=\"http://creativecommons.org/licenses/sampling+/1.0/\">Creative Commons Sampling Plus 1.0 license</a>.<br>"

                onLinkActivated: helpScreen.linkActivated(link)
            }
        }
    }
}
