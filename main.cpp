#include <QApplication>
#include <QDeclarativeView>
#include <QVariant>
#include <QGraphicsObject>
#include <QPointer>
#include <QDesktopWidget>
#include "TurnTable.h"

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

// Game Enabler namespace
using namespace GE;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE( if(appUi) { appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape); });
#endif

    QDeclarativeView view;
    view.setSource(QUrl("qrc:/TurnTable.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

#ifndef QT_NO_OPENGL
    // Use QGLWidget to get the opengl support in Windows
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view.setViewport(glWidget);     // ownership of glWidget is taken
#endif

    QPointer<CTurnTable> turnTable = new CTurnTable;

    QObject *rootObject = dynamic_cast<QObject*>(view.rootObject());

    QObject::connect(rootObject, SIGNAL(start()), turnTable, SLOT(start()));
    QObject::connect(rootObject, SIGNAL(stop()), turnTable, SLOT(stop()));
    QObject::connect(rootObject, SIGNAL(startBeat()), turnTable, SLOT(startBeat()));
    QObject::connect(rootObject, SIGNAL(stopBeat()), turnTable, SLOT(stopBeat()));
    QObject::connect(rootObject, SIGNAL(toggleBeat(QVariant)), turnTable, SLOT(toggleBeat(QVariant)));
    QObject::connect(rootObject, SIGNAL(speed(QVariant)), turnTable, SLOT(setDiscSpeed(QVariant)));
    QObject::connect(rootObject, SIGNAL(close()), &app, SLOT(quit()));

#if defined(Q_WS_MAEMO_5)|| defined(Q_OS_SYMBIAN)
    view.setGeometry(QApplication::desktop()->screenGeometry());
    view.showFullScreen();
#else
    view.setGeometry(QRect(100, 100, 800, 480));
    view.show();
#endif

    return app.exec();
}
