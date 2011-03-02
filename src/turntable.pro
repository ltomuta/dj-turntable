QT       += core gui declarative opengl

TEMPLATE = app


VERSION = 1.2.2

SOURCES += main.cpp \
           TurnTable.cpp \
           DrumMachine.cpp \
           mainwindow.cpp \
           ga_src/GEAudioBuffer.cpp \
           ga_src/GEInterfaces.cpp \
           ga_src/GEAudioOut.cpp \


OTHER_FILES += qml/*.qml \
               qml/SampleSelector/*.qml \
               qml/DrumMachine/*.qml \
               qml/HelpScreen/*.qml

RESOURCES +=   turntable.qrc

HEADERS   +=   mainwindow.h \
               TurnTable.h \
               DrumMachine.h \
               ga_src/GEAudioOut.h \
               ga_src/GEInterfaces.h \
               ga_src/GEAudioBuffer.h

win32:!maemo5 {
    TARGET = DjTurntable
    QT += multimedia
    RC_FILE = turntable.rc
}


unix:!symbian {
    maemo5 {
        TARGET    = turntable
        QT        += multimedia
        CONFIG    += mobility
        MOBILITY  += sensors systeminfo

        HEADERS   += accelerometerfilter.h

        BINDIR    = /opt/usr/bin
        DATADIR   = /usr/share
        DEFINES  += DATADIR=\\\"$$DATADIR\\\" \
                    PKGDATADIR=\\\"$$PKGDATADIR\\\"
        INSTALLS += target \
                    desktop \
                    iconxpm \
                    icon26 \
                    icon40 \
                    icon64

        target.path = $$BINDIR
        desktop.path = $$DATADIR/applications/hildon
        desktop.files += $${TARGET}.desktop

        iconxpm.path = $$DATADIR/pixmap
        iconxpm.files += icons/xpm/turntable.xpm

        icon26.path = $$DATADIR/icons/hicolor/26x26/apps
        icon26.files += icons/26x26/turntable.png

        icon40.path = $$DATADIR/icons/hicolor/40x40/apps
        icon40.files += icons/40x40/turntable.png

        icon64.path = $$DATADIR/icons/hicolor/64x64/apps
        icon64.files += icons/64x64/turntable.png
    } else {
        target.path = /usr/local/bin
    }
}


symbian {
    # in Symbian1 we don't have OpenGL available
    contains(SYMBIAN_VERSION, 9.4) {
        QT -= opengl
        DEFINES += QT_NO_OPENGL
    }

    QT += multimedia

    TARGET = DjTurntable
    CONFIG   += mobility
    MOBILITY += sensors systeminfo

    HEADERS  += accelerometerfilter.h

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
