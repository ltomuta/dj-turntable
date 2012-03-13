DJ Turntable v1.4.1
===================

DJ Turntable demonstrates integrating a Qt Quick application to the Qt audio
interface using Qt GameEnabler's audio engine. The application is compatible
with Nokia Anna and Belle devices with Qt 4.7.4 or higher, Meego 1.2 Harmattan,
and Windows desktop computers.

This example application is hosted in Nokia Developer Projects:
- http://projects.developer.nokia.com/turntable

For more information on implementation, visit the wiki pages:
- http://projects.developer.nokia.com/turntable/wiki
- http://projects.developer.nokia.com/turntable/wiki/DesignConsiderations
- http://projects.developer.nokia.com/qtgameenabler

What's new
----------

- Added support for ogg vorbis files
- Uses the latest Qt GameEnabler audio engine

1. Usage
-------------------------------------------------------------------------------

Turntable

Play the looping sample with a realistic turntable. The sample can be
scratched with a finger, played faster, slower, and backwards. The speed of
the record can be adjusted with the speed slider, the default 1x speed can be
obtained by double-clicking the speed slider knob. DJ Turntable includes
Cutoff and Resonance control knobs to alter the sample in real time. The knobs
are rotated by moving a finger up and down on top of them.


Sample selector

Use the sample selector to change the sample that the turntable is playing by
selecting the desired sample from the directory view. DJ Turntable support
wav and ogg vorbis files. The following uncompressed WAV formats are supported:

 8-bit unsigned
16-bit unsigned
32-bit float

Only stereo ogg files are supported. Avoid using files with high bitrates and
sample rates since the files are decoded in real-time. C7/N8 and N9 devices are
capable of playing at least 120kbps/44KHz files (depending on the encoder
settings).

The application will open the last selected sample on startup. Use the default
sample button on the right under the back button to reset back to the default
sample.


Drum machine

Play and edit the drum beats. There are three predefined beats which can be
played and edited but the edits are not stored. For the user there are three
beats that are saved in the device memory whenever they are edited. Use the
beat selector buttons at the bottom of the view to switch between the beats.
All the beats are 32 ticks long and they contain 6 different drum samples:
hi-hat, hi-hat open, bass drum, snare, cymbal, and cow bell.

The drum machine will play all drum beats at 150 bpm. Changing the speed of
the turntable will affect the playing speed of the drum machine accordingly.


Keyboard shortcuts

The following keyboard shortcuts exist:
Camera zoom up = Volume up
Camera zoom down = Volume down
Space = Start / stop the turntable
Return = Start / stop the drum machine
Key up = Go to the turntable view
Key down = Go to the drum machine view
Key left = View the 1st tick group in the drum machine
Key right = View the 2nd tick group in the drum machine
Key i = Go to the info view
Key s = Go to the sample selector view
Backspace = Return from the info or sample selector view to the previous view


Samples

The turntable melody sample ivory.wav was created by nick Flick3r and it was
downloaded from http://www.freesound.org. The sample follows the
http://creativecommons.org/licenses/sampling+/1.0/ license.


2. Project structure and implementation
-------------------------------------------------------------------------------

2.1 Files
---------

src/main.cpp

- Creates all the required Qt objects and initialises the Declarative runtime.
  The integration of the QML and Qt code is done here.

src/TurnTable.h
src/TurnTable.cpp

- Represents the logic of the turntable section of the UI. The Turntable
  object contains the main audio mixer to which the drum beats and the disc
  sample are mixed.

src/DrumMachine.h
src/DrumMachine.cpp

- Represents the logic of the drum machine section of the UI. Signals of the
  drum machine QML are mapped directly to this object's slots. The storing and
  loading of the drum sequences is done here.

src/accelerometerfilter.h

- Handles the accelerometer information from the device's sensor. The signals
  about changes in accelerometer data are mapped directly to the QML slots to
  rotate the reflection on the record on the turntable.

src/sounds/*

- Contains the audio samples used by DJ Turntable.

src/qml/DrumMachine/*

- QML files containing the UI controls of the drum machine.

src/qml/InfoScreen/*

- QML files containing the UI controls of the info screen.

src/qml/SampleSelector/*

- QML files containing the UI controls of the sample selector screen.

src/qml/TurnTable.qml

- The main QML file that represents the whole UI of the application.
  All other QML files and elements are children of this file.

src/qtgameenabler/*

- Qt GameEnablers' source codes.

bin/

- The compiled binaries.


2.2 Used classes and elements
-----------------------------

The most important Qt classes and QML elements used in the application are
listed here.

Qt / C++ classes
~~~~~~~~~~~~~~~~
 QDeclarativeView - used to interpret QML files
 QGLWidget        - used to draw QDeclarativeView with Qt OpenGL
 QPointer         - relaxes the use of pointers
 QSettings        - used to store and retrieve user defined beats to / from
                    permanent storage
 QMessageBox      - used to report serious errors
 QVariant         - used as parameters in Qt Signals and Slots between Qt and
                    QML
 QVector          - used to store beat sequences in memory
 std::vector      - used to convert predefined beats stored as byte arrays to
                    QVectors

Qt Mobility classes
~~~~~~~~~~~~~~~~~~~
 QAccelerometer       - gets the inclination of the mobile device
 QAccelerometerFilter - used to filter values of QAccelerometer to save CPU
                        cycles
 QSystemDeviceInfo    - used to detect the profile of the device, for
                        instance, when the mobile is on silent the volume is
                        set to 0
 QAudioOutput         - used to access audio device (part of QtMultimediaKit
                        in Symbian target)

Qt classes used in GE audio engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 QAudioOutput   - used to access audio device (part of Qt multimedia module in
                  desktop and Maemo targets)
 QThread        - used to implement audio functionality in worker thread
 QIODevice      - used to handle audio buffers
 QMutex         - used to synchronise critical sections between worker thread
                  and GUI thread
 QFile          - used to read WAV files

Symbian specific classes
~~~~~~~~~~~~~~~~~~~~~~~~
 MRemConCoreApiTargetObserver,
 CRemConInterfaceSelector,
 CRemConCoreApiTarget          - used to handle volume up/down HW keys on
                                 Symbian devices
 CMMFDevSound                  - used to set the master volume in Symbian
                                 devies, involves a nasty hack

QML elements
~~~~~~~~~~~~
 Flickable         - used to implement view change between the info screen,
                     the turntable, and the drum machine, also used to switch
                     between 1-16 and 17-32 ticks in drum machine
 Repeater          - used to create 32 x 6 drum buttons in drum machine
 Rotation          - used to rotate the 'arm' on top of the record


3. Compatibility
-------------------------------------------------------------------------------

 - Symbian devices with Qt 4.7.4 or higher.
 - MeeGo 1.2 Harmattan.
 - Windows 7

Tested to work on;
 - Nokia N8-00
 - Nokia C7-00
 - Nokia N9
 - Nokia 701
 - Windows 7

Developed with:

 - Qt SDK 1.2


4.1 Required Capabilities
-------------------------

None; The application can be self signed on Symbian.


4.2 Known Issues
----------------

 - Some OGG files may not work properly. This depends on the used encoder and
   used encoding settings. For example, the ogg file can contain only one vorbis
   stream. The decodind is done using (rather old) stb vorbis, see
   http://nothings.org/stb_vorbis/

5. Building, installing, and running the application
-------------------------------------------------------------------------------

5.1 Preparations
----------------

Check that you have the latest Qt SDK installed in the development environment
and the latest Qt version on the device.

5.2 Using the Qt SDK
--------------------

You can install and run the application on the device by using the Qt SDK.
Open the project in the SDK, set up the correct target (depending on the device
platform), and click the Run button. For more details about this approach,
visit the Qt Getting Started section at Nokia Developer
(http://www.developer.nokia.com/Develop/Qt/Getting_started/).

5.3 Symbian device
------------------

Make sure your device is connected to your computer. Locate the .sis
installation file and open it with Ovi Suite. Accept all requests from Ovi
Suite and the device. Note that you can also install the application by copying
the installation file onto your device and opening it with the Symbian File
Manager application.

After the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.

5.4 Nokia N9
------------

Copy the application Debian package onto the device. Locate the file with the
device and run it; this will install the application. Note that you can also
use the terminal application and install the application by typing the command
'dpkg -i <package name>.deb' on the command line. To install the application
using the terminal application, make sure you have the right privileges 
to do so (e.g. root access).

Once the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.


6. License
-------------------------------------------------------------------------------

See the license text file delivered with this project. The license file is also
available online at

http://projects.developer.nokia.com/turntable/browser/Licence.txt


7. Version history
-------------------------------------------------------------------------------
v1.4.1 Added surround icon. Fixed the disk reflection.
v1.4.0 Added support for ogg vorbis files and use the latest Qt GameEnabler.
v1.3.1 Some optimizations for low performance Symbian devices.
v1.3 Added support for MeeGo 1.2 Harmattan.
v1.2.2 Compatibility with Qt 4.7.2.
v1.2.1 Added error handling to opening of external WAV files.
v1.2 Added possibility to play external WAV files.
v1.1 Updated version for Forum Nokia.
v1.0 Initial version published in Forum Nokia Projects only.

