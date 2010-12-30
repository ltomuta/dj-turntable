#include <QApplication>
#include <QDeclarativeView>
#include <QVariant>
#include <QGraphicsObject>
#include <QPointer>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSettings>

#include "TurnTable.h"
#include "DrumMachine.h"


// Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    #include <eikenv.h>
    #include <eikappui.h>
    #include <aknenv.h>
    #include <aknappui.h>
#endif

#ifndef QT_NO_OPENGL
    #include <QGLWidget>
#endif


#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    #include <QSystemDeviceInfo>
    #include "accelerometerfilter.h"

    QTM_USE_NAMESPACE
#endif


/**
 *
 * Recursive function that finds object from QObject tree.
 * Return NULL if element was not found.
 */
QObject* findQMLElement(QObject *rootElement, const QString &objectName)
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


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE( if(appUi) { appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape); } );
#endif

    QDeclarativeView view;
    view.setSource(QUrl("qrc:/qml/TurnTable.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

#ifndef QT_NO_OPENGL
    // Use QGLWidget to get the opengl support if available
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view.setViewport(glWidget);     // ownership of glWidget is taken
#endif

    // Create Qt settings object to load / store app settings
    QPointer<QSettings> settings = new QSettings("Nokia", "DJTurntable");

    // Create Qt objects to handle Turntable and Drum machine
    QPointer<TurnTable> turnTable = new TurnTable(settings);
    QPointer<DrumMachine> drumMachine = new DrumMachine(settings);
    turnTable->addAudioSource(drumMachine);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    // Create Qt accelerometer objects
    QAccelerometer sensor;
    QPointer<AccelerometerFilter> filter = new AccelerometerFilter;
    sensor.addFilter(filter);   // does not take the ownership of the filter

    // Create Qt objects for accessing profile information
    QPointer<QSystemDeviceInfo> deviceInfo = new QSystemDeviceInfo;
    turnTable->profile(deviceInfo->currentProfile());
#endif

    // Find out the interesting Qt objects of the QML elements
    QObject *turnTableQML = dynamic_cast<QObject*>(view.rootObject());
    QObject *drumMachineQML = findQMLElement(turnTableQML, "drumMachine");

    // If there are errors in QML code and the elements does not exist, they won't be found
    // in Qt side either, check existance of the elements.
    if(turnTableQML == NULL || drumMachineQML == NULL) {
        QMessageBox::warning(NULL, "Warning", "Failed to resolve QML elements in main.cpp");
        return -1;
    }

    // TurnTable connections
    QObject::connect(turnTableQML, SIGNAL(start()), turnTable, SLOT(start()));
    QObject::connect(turnTableQML, SIGNAL(stop()), turnTable, SLOT(stop()));
    QObject::connect(turnTableQML, SIGNAL(diskAimSpeed(QVariant)), turnTable, SLOT(setDiscAimSpeed(QVariant)));
    QObject::connect(turnTableQML, SIGNAL(diskSpeed(QVariant)), turnTable, SLOT(setDiscSpeed(QVariant)));
    QObject::connect(turnTableQML, SIGNAL(cutOff(QVariant)), turnTable, SLOT(setCutOff(QVariant)));
    QObject::connect(turnTableQML, SIGNAL(resonance(QVariant)), turnTable, SLOT(setResonance(QVariant)));
    QObject::connect(turnTableQML, SIGNAL(linkActivated(QVariant)), turnTable, SLOT(linkActivated(QVariant)));
    QObject::connect(turnTable, SIGNAL(audioPosition(QVariant)), turnTableQML, SLOT(audioPosition(QVariant)));

    //DrumMachine connections
    QObject::connect(drumMachineQML, SIGNAL(startBeat()), drumMachine, SLOT(startBeat()));
    QObject::connect(drumMachineQML, SIGNAL(stopBeat()), drumMachine, SLOT(stopBeat()));
    QObject::connect(drumMachineQML, SIGNAL(setBeat(QVariant)), drumMachine, SLOT(setBeat(QVariant)));
    QObject::connect(drumMachineQML, SIGNAL(drumButtonToggled(QVariant, QVariant, QVariant)), drumMachine, SLOT(drumButtonToggled(QVariant, QVariant, QVariant)));
    QObject::connect(drumMachineQML, SIGNAL(drumMachineSpeed(QVariant)), drumMachine, SLOT(setBeatSpeed(QVariant)));
    QObject::connect(drumMachine, SIGNAL(drumButtonState(QVariant, QVariant, QVariant)), drumMachineQML, SLOT(setDrumButton(QVariant, QVariant, QVariant)));
    QObject::connect(drumMachine, SIGNAL(tickChanged(QVariant)), drumMachineQML, SLOT(highlightTick(QVariant)));

    //Framework connections
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    QObject::connect(filter, SIGNAL(rotationChanged(QVariant)), turnTableQML, SLOT(inclination(QVariant)));
    QObject::connect(deviceInfo, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)), turnTable, SLOT(profile(QSystemDeviceInfo::Profile)));
#endif

    // Start with beat 0
    drumMachine->setBeat(0);

#if defined(Q_WS_MAEMO_5)|| defined(Q_OS_SYMBIAN)
    // Begins the measuring of accelerometer sensor
    sensor.start();

    view.setGeometry(QApplication::desktop()->screenGeometry());
    view.showFullScreen();
#else
    view.setGeometry(QRect(100, 100, 640, 360));
    view.show();
#endif

    return app.exec();
}
