/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include "mainwindow.h"

#include <QDebug>
#include <QtDeclarative>

#include "drummachine.h"
#include "turntable.h"

#ifndef QT_NO_OPENGL
    #include <QGLWidget>
#endif

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
// If OpenGL is not used, we're building for Symbian^1. If that is the case,
// for performance reasons let's also drop the accelerometer feature.
#ifndef QT_NO_OPENGL
    #include <QSystemDeviceInfo>
    #include "accelerometerfilter.h"

    QTM_USE_NAMESPACE
#endif
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_view = new QDeclarativeView(this);
    m_view->setAttribute(Qt::WA_NoSystemBackground);

#ifndef QT_NO_OPENGL
    // Use QGLWidget to get the opengl support if available
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    m_view->setViewport(glWidget);     // ownership of glWidget is taken
#endif

    m_view->setSource(QUrl("qrc:/qml/SplashScreen.qml"));
    m_view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    setCentralWidget(m_view);

    // To delay the loading of main QML file so that the splash screen
    // would show, we use single shot timer.
    QTimer::singleShot(0, this, SLOT(initializeQMLComponent()));
}


void MainWindow::initializeQMLComponent()
{
    QDeclarativeContext *context = m_view->rootContext();

#if defined(Q_WS_MAEMO_5) || defined(QT_NO_OPENGL)
    // Set UI to low performance mode for Maemo5 and Symbian^1. This mainly
    // disables antialiasing on some performance costly elements.
    context->setContextProperty("lowPerf", true);
#else
    context->setContextProperty("lowPerf", false);
#endif

#ifdef Q_OS_SYMBIAN
    context->setContextProperty("sampleFolder", "file:");
#else
    context->setContextProperty("sampleFolder", QString("file:/") +
                                QDir::currentPath());
#endif

#ifdef Q_WS_MAEMO_6
    // Hide the exit button in Harmattan
    context->setContextProperty("exitButtonVisible", false);
#else
    context->setContextProperty("exitButtonVisible", true);
#endif

    m_view->setSource(QUrl("qrc:/qml/Main.qml"));

    // Create Qt settings object to load / store app settings
    m_settings = new QSettings("Nokia", "DJTurntable");

    // Create Qt objects to handle Turntable and Drum machine
    m_turntable = new Turntable(m_settings, this);
    m_drumMachine = new DrumMachine(m_settings, this);
    m_turntable->addAudioSource(m_drumMachine);

    // Find out the interesting Qt objects of the QML elements
    QObject *turntableQML = dynamic_cast<QObject*>(m_view->rootObject());
    QObject *sampleSelectorQML =
            m_view->rootObject()->findChild<QObject*>("sampleSelector");
    QObject *drumMachineQML =
            m_view->rootObject()->findChild<QObject*>("drumMachine");

    // If there are errors in QML code and the elements does not exist,
    // they won't be found Qt side either, check existance of the elements.
    if (!turntableQML || !sampleSelectorQML || !drumMachineQML) {
        QMessageBox::warning(NULL, "Warning",
                             "Failed to resolve QML elements in main.cpp");
        return;
    }

    // Turntable connections
    connect(turntableQML, SIGNAL(start()),
            m_turntable, SLOT(start()));
    connect(turntableQML, SIGNAL(stop()),
            m_turntable, SLOT(stop()));
    connect(turntableQML, SIGNAL(diskAimSpeed(QVariant)),
            m_turntable, SLOT(setDiscAimSpeed(QVariant)));
    connect(turntableQML, SIGNAL(diskSpeed(QVariant)),
            m_turntable, SLOT(setDiscSpeed(QVariant)));
    connect(turntableQML, SIGNAL(cutOff(QVariant)),
            m_turntable, SLOT(setCutOff(QVariant)));
    connect(turntableQML, SIGNAL(resonance(QVariant)),
            m_turntable, SLOT(setResonance(QVariant)));
    connect(turntableQML, SIGNAL(seekToPosition(QVariant)),
            m_turntable, SLOT(seekToPosition(QVariant)));
    connect(m_turntable, SIGNAL(audioPosition(QVariant)),
            turntableQML, SLOT(audioPosition(QVariant)));
    connect(m_turntable, SIGNAL(powerOff()), turntableQML, SLOT(powerOff()));

    // SampleSelector connections
    connect(sampleSelectorQML, SIGNAL(sampleSelected(QVariant)),
            m_turntable, SLOT(setSample(QVariant)));
    connect(sampleSelectorQML, SIGNAL(defaultSample()),
            m_turntable, SLOT(openDefaultSample()));
    connect(m_turntable, SIGNAL(sampleOpened(QVariant)),
            sampleSelectorQML, SLOT(setCurrentSample(QVariant)));
    connect(m_turntable, SIGNAL(error(QVariant, QVariant)),
            sampleSelectorQML, SLOT(showError(QVariant, QVariant)));

    // DrumMachine connections
    connect(drumMachineQML, SIGNAL(startBeat()),
            m_drumMachine, SLOT(startBeat()));
    connect(drumMachineQML, SIGNAL(stopBeat()),
            m_drumMachine, SLOT(stopBeat()));
    connect(drumMachineQML, SIGNAL(setBeat(QVariant)),
            m_drumMachine, SLOT(setBeat(QVariant)));
    connect(drumMachineQML,
            SIGNAL(drumButtonToggled(QVariant, QVariant, QVariant)),
            m_drumMachine,
            SLOT(drumButtonToggled(QVariant, QVariant, QVariant)));
    connect(drumMachineQML, SIGNAL(drumMachineSpeed(QVariant)),
            m_drumMachine, SLOT(setBeatSpeed(QVariant)));
    connect(m_drumMachine,
            SIGNAL(drumButtonState(QVariant, QVariant, QVariant)),
            drumMachineQML,
            SLOT(setDrumButton(QVariant, QVariant, QVariant)));
    connect(m_drumMachine, SIGNAL(tickChanged(QVariant)),
            drumMachineQML, SLOT(highlightTick(QVariant)));

    // Framework connections
    connect((QObject*)m_view->engine(), SIGNAL(quit()), qApp, SLOT(quit()));

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#ifndef QT_NO_OPENGL
    // Create Qt accelerometer objects
    m_accelerometer = new QAccelerometer(this);
    m_accelerometerFilter = new AccelerometerFilter;
    // Does not take the ownership of the filter
    m_accelerometer->addFilter(m_accelerometerFilter);

    m_accelerometer->setDataRate(50);

    // Create Qt objects for accessing profile information
    m_deviceInfo = new QSystemDeviceInfo(this);
    m_turntable->profile(m_deviceInfo->currentProfile());

    connect(m_accelerometerFilter, SIGNAL(rotationChanged(QVariant)),
            turntableQML, SLOT(inclination(QVariant)));
    connect(m_deviceInfo,
            SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            m_turntable,
            SLOT(profile(QSystemDeviceInfo::Profile)));

    // Begin the measuring of the accelerometer sensor
    m_accelerometer->start();
#endif
#endif

    m_turntable->openLastSample();
    m_drumMachine->setBeat(0);
}
