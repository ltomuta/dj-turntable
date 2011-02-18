#include <QtDeclarative>
#include "DrumMachine.h"
#include "TurnTable.h"
#include "mainwindow.h"

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
#include <QSystemDeviceInfo>
#include "accelerometerfilter.h"

QTM_USE_NAMESPACE
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    view = new QDeclarativeView(this);

#ifndef QT_NO_OPENGL
    // Use QGLWidget to get the opengl support if available
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view->setViewport(glWidget);     // ownership of glWidget is taken
#endif

    view->setSource(QUrl("qrc:/qml/SplashScreen.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    setCentralWidget(view);

    // To delay the loading of main QML file so that the splash screen
    // would show, we use single shot timer.
    QTimer::singleShot(0, this, SLOT(initializeQMLComponent()));
}


void MainWindow::initializeQMLComponent()
{
    QDeclarativeContext *context = view->rootContext();
#ifdef Q_WS_MAEMO_5
    // Set UI to low performance mode in Maemo5, mostly this disables
    // antialiasing on some performance costly elements
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

    view->setSource(QUrl("qrc:/qml/TurnTable.qml"));

    // Create Qt settings object to load / store app settings
    settings = new QSettings("Nokia", "DJTurntable");

    // Create Qt objects to handle Turntable and Drum machine
    turnTable = new TurnTable(settings, this);
    drumMachine = new DrumMachine(settings, this);
    turnTable->addAudioSource(drumMachine);

    // Find out the interesting Qt objects of the QML elements
    QObject *turnTableQML = dynamic_cast<QObject*>(view->rootObject());
    QObject *sampleSelectorQML = findQMLElement(turnTableQML, "sampleSelector");
    QObject *drumMachineQML = findQMLElement(turnTableQML, "drumMachine");

    // If there are errors in QML code and the elements does not exist,
    // they won't be found Qt side either, check existance of the elements.
    if(turnTableQML == NULL || sampleSelectorQML == NULL || drumMachineQML == NULL) {
        QMessageBox::warning(NULL, "Warning",
                             "Failed to resolve QML elements in main.cpp");
        return;
    }

    // TurnTable connections
    connect(turnTableQML, SIGNAL(start()), turnTable, SLOT(start()));
    connect(turnTableQML, SIGNAL(stop()), turnTable, SLOT(stop()));
    connect(turnTableQML, SIGNAL(diskAimSpeed(QVariant)),
            turnTable, SLOT(setDiscAimSpeed(QVariant)));
    connect(turnTableQML, SIGNAL(diskSpeed(QVariant)),
            turnTable, SLOT(setDiscSpeed(QVariant)));
    connect(turnTableQML, SIGNAL(cutOff(QVariant)),
            turnTable, SLOT(setCutOff(QVariant)));
    connect(turnTableQML, SIGNAL(resonance(QVariant)),
            turnTable, SLOT(setResonance(QVariant)));
    connect(turnTableQML, SIGNAL(seekToPosition(QVariant)),
            turnTable, SLOT(seekToPosition(QVariant)));
    connect(turnTable, SIGNAL(audioPosition(QVariant)),
            turnTableQML, SLOT(audioPosition(QVariant)));
    connect(turnTable, SIGNAL(powerOff()), turnTableQML, SLOT(powerOff()));

    // SampleSelector connections
    connect(sampleSelectorQML, SIGNAL(sampleSelected(QVariant)),
            turnTable, SLOT(setSample(QVariant)));
    connect(sampleSelectorQML, SIGNAL(defaultSample()),
            turnTable, SLOT(openDefaultSample()));
    connect(turnTable, SIGNAL(sampleOpened(QVariant)),
            sampleSelectorQML, SLOT(setCurrentSample(QVariant)));
    connect(turnTable, SIGNAL(error(QVariant, QVariant)),
            sampleSelectorQML, SLOT(showError(QVariant, QVariant)));

    // DrumMachine connections
    connect(drumMachineQML, SIGNAL(startBeat()),
            drumMachine, SLOT(startBeat()));
    connect(drumMachineQML, SIGNAL(stopBeat()),
            drumMachine, SLOT(stopBeat()));
    connect(drumMachineQML, SIGNAL(setBeat(QVariant)),
            drumMachine, SLOT(setBeat(QVariant)));
    connect(drumMachineQML,
            SIGNAL(drumButtonToggled(QVariant, QVariant, QVariant)),
            drumMachine,
            SLOT(drumButtonToggled(QVariant, QVariant, QVariant)));
    connect(drumMachineQML, SIGNAL(drumMachineSpeed(QVariant)),
            drumMachine, SLOT(setBeatSpeed(QVariant)));
    connect(drumMachine,
            SIGNAL(drumButtonState(QVariant, QVariant, QVariant)),
            drumMachineQML,
            SLOT(setDrumButton(QVariant, QVariant, QVariant)));
    connect(drumMachine, SIGNAL(tickChanged(QVariant)),
            drumMachineQML, SLOT(highlightTick(QVariant)));

    // Framework connections
    connect((QObject*)view->engine(), SIGNAL(quit()), qApp, SLOT(quit()));

    #if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
        // Create Qt accelerometer objects
        accelerometer = new QAccelerometer(this);
        accelerometerFilter = new AccelerometerFilter;
        accelerometer->addFilter(accelerometerFilter);   // does not take the ownership of the filter

        // Create Qt objects for accessing profile information
        deviceInfo = new QSystemDeviceInfo(this);
        turnTable->profile(deviceInfo->currentProfile());

        connect(accelerometerFilter, SIGNAL(rotationChanged(QVariant)),
                turnTableQML, SLOT(inclination(QVariant)));
        connect(deviceInfo,
                SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                turnTable,
                SLOT(profile(QSystemDeviceInfo::Profile)));

        // Begin the measuring of the accelerometer sensor
        accelerometer->start();
    #endif

    turnTable->openLastSample();
    drumMachine->setBeat(0);
}


/**
 *
 * Recursive function that finds object from QObject tree.
 * Return NULL if element was not found.
 */
QObject* MainWindow::findQMLElement(QObject *rootElement, const QString &objectName)
{
    if(rootElement->objectName() == objectName) {
        return rootElement;
    }

    const QObjectList list = rootElement->children();
    for(QObjectList::const_iterator it=list.begin(); it!=list.end(); it++)
    {
        QObject *object = findQMLElement((*it), objectName);
        if(object != NULL) {
            return object;
        }
    }

    return NULL;
}
