QT       += core gui
QT       += declarative opengl

CONFIG   += mobility
MOBILITY += sensors multimedia

TARGET   = turntable
TEMPLATE = app


SOURCES += main.cpp \
           TurnTable.cpp \
           DrumMachine.cpp \
           ga_src/GEAudioBuffer.cpp \
           ga_src/GEInterfaces.cpp \
           ga_src/GEAudioOut.cpp

OTHER_FILES += qml/*.qml \
               qml/DrumMachine/*.qml

RESOURCES +=   resources.qrc

HEADERS +=     TurnTable.h \
               DrumMachine.h \
               accelerometerfilter.h \
               ga_src/GEAudioOut.h \
               ga_src/GEInterfaces.h \
               ga_src/GEAudioBuffer.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}


symbian {
    # To lock the application to landscape orientation
    LIBS += -lcone -leikcore -lavkon

    # For QtMobility
    TARGET.CAPABILITY = NetworkServices \
                        Location \
                        ReadUserData \
                        WriteUserData \
                        LocalServices \
                        UserEnvironment

    TARGET.EPOCHEAPSIZE = 0x100000 0x2000000
    TARGET.EPOCSTACKSIZE = 0x14000
}
