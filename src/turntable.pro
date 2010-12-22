QT       += core gui declarative opengl

CONFIG   += mobility
MOBILITY += sensors multimedia systeminfo

TARGET   = turntable
TEMPLATE = app

VERSION = 1.1.0

SOURCES += main.cpp \
           TurnTable.cpp \
           DrumMachine.cpp \
           ga_src/GEAudioBuffer.cpp \
           ga_src/GEInterfaces.cpp \
           ga_src/GEAudioOut.cpp

OTHER_FILES += qml/*.qml \
               qml/DrumMachine/*.qml \
               qml/HelpScreen/*.qml

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
    # For the very ugly hack to make the master volume control possible
    INCLUDEPATH += /epoc32/include/mmf/common
    INCLUDEPATH += /epoc32/include/mmf/server
    LIBS += -lmmfdevsound

    # To handle volume up / down keys on Symbian
    LIBS += -lremconcoreapi
    LIBS += -lremconinterfacebase

    # For the icon
    ICON = icons/turntable.svg

    # To lock the application to landscape orientation
    LIBS += -lcone -leikcore -lavkon

    TARGET.EPOCHEAPSIZE = 0x100000 0x2000000
    TARGET.EPOCSTACKSIZE = 0x14000
}
