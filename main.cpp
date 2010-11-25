#include <QApplication>
#include <QDeclarativeView>
#include <QVariant>
#include <QGraphicsObject>
#include <QPointer>
#include <QDesktopWidget>
#include <QObject>
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
    // Use QGLWidget to get the opengl support in Windows
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view.setViewport(glWidget);     // ownership of glWidget is taken
#endif

    QPointer<TurnTable> turnTable = new TurnTable;
    QPointer<CDrumMachine> drumMachine = new CDrumMachine;
    turnTable->addAudioSource(drumMachine);

    QObject *turnTableQML = dynamic_cast<QObject*>(view.rootObject());
    QObject *drumMachineQML = findQMLElement(turnTableQML, "drumMachine");

    //TurnTable connections
    QObject::connect(turnTableQML, SIGNAL(start()), turnTable, SLOT(start()));
    QObject::connect(turnTableQML, SIGNAL(stop()), turnTable, SLOT(stop()));
    QObject::connect(turnTableQML, SIGNAL(diskSpeed(QVariant)), turnTable, SLOT(setDiscSpeed(QVariant)));

    //DrumMachine connections
    QObject::connect(drumMachineQML, SIGNAL(startBeat()), drumMachine, SLOT(startBeat()));
    QObject::connect(drumMachineQML, SIGNAL(stopBeat()), drumMachine, SLOT(stopBeat()));
    QObject::connect(drumMachineQML, SIGNAL(setDemoBeat(QVariant)), drumMachine, SLOT(setDemoBeat(QVariant)));
    QObject::connect(drumMachineQML, SIGNAL(drumButtonToggled(QVariant, QVariant, QVariant)), drumMachine, SLOT(drumButtonToggled(QVariant, QVariant, QVariant)));

    QObject::connect(drumMachine, SIGNAL(maxSeqAndSamples(QVariant, QVariant)), drumMachineQML, SLOT(maxSeqAndSamples(QVariant, QVariant)));
    QObject::connect(drumMachine, SIGNAL(seqSize(QVariant, QVariant)), drumMachineQML, SLOT(seqSize(QVariant, QVariant)));
    QObject::connect(drumMachine, SIGNAL(drumButtonState(QVariant, QVariant, QVariant)), drumMachineQML, SLOT(setDrumButton(QVariant, QVariant, QVariant)));
    QObject::connect(drumMachine, SIGNAL(tickChanged(QVariant)), drumMachineQML, SLOT(highlightTick(QVariant)));

    //Framework connections
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));

    // Start with beat 0
    drumMachine->setMaxTickAndSamples(32, 6);
    drumMachine->setDemoBeat(0);

#if defined(Q_WS_MAEMO_5)|| defined(Q_OS_SYMBIAN)
    view.setGeometry(QApplication::desktop()->screenGeometry());
    view.showFullScreen();
#else
    view.setGeometry(QRect(100, 100, 800, 480));
    view.show();
#endif

    return app.exec();
}
