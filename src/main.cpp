#include <QtGui>
#include "mainwindow.h"

// Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    #include <eikenv.h>
    #include <eikappui.h>
    #include <aknenv.h>
    #include <aknappui.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Lock orientation in Symbian
    #ifdef Q_OS_SYMBIAN
        CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
        TRAP_IGNORE(
            if(appUi) {
                appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
            }
        );
    #endif

    MainWindow mainWindow;

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    mainWindow.setGeometry(QApplication::desktop()->screenGeometry());
    mainWindow.showFullScreen();
#else
    mainWindow.setGeometry(QRect(100, 100, 800, 480);
    mainWindow.show();
#endif

    return app.exec();
}
