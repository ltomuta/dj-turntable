#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

class QDeclarativeView;
class QSettings;
class QSplashScreen;
class TurnTable;
class DrumMachine;

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include <QAccelerometer>
#include <QSystemDeviceInfo>

class AccelerometerFilter;

QTM_USE_NAMESPACE
#endif


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

public slots:
    void initializeQMLComponent();

protected:
    QDeclarativeView *view;
    QSettings *settings;
    QPointer<TurnTable> turnTable;
    QPointer<DrumMachine> drumMachine;

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QAccelerometer *accelerometer;
    QPointer<AccelerometerFilter> accelerometerFilter;
    QPointer<QSystemDeviceInfo> deviceInfo;
#endif

    static QObject* findQMLElement(QObject *rootElement, const QString &objectName);
};

#endif // MAINWINDOW_H
