/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

class QDeclarativeView;
class QSettings;
class QSplashScreen;
class Turntable;
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
    QDeclarativeView *m_view; // Owned
    QSettings *m_settings; // Owned
    QPointer<Turntable> m_turntable; // Owned
    QPointer<DrumMachine> m_drumMachine; // Owned

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QAccelerometer *m_accelerometer; // Owned
    QPointer<AccelerometerFilter> m_accelerometerFilter;
    QPointer<QSystemDeviceInfo> m_deviceInfo;
#endif
};

#endif // MAINWINDOW_H
