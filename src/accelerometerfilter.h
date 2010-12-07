#ifndef ACCELEROMETERFILTER_H
#define ACCELEROMETERFILTER_H

#include <QAccelerometerFilter>
#include <QVariant>
#include <math.h>


QTM_USE_NAMESPACE

#define RADIANS_TO_DEGREES 57.2957795

class AccelerometerFilter : public QObject, public QAccelerometerFilter
{
    Q_OBJECT

public:
    AccelerometerFilter() {}

    bool filter(QAccelerometerReading *reading)
    {
        static int counter = 0;
        counter++;
        if(counter < 10) {
            return false;
        }
        else {
            counter = 0;
        }

        qreal rx = reading->x();
        qreal ry = reading->y();
        qreal rz = reading->z();

        qreal divider = sqrt(rx * rx + ry * ry + rz * rz);

#if defined(Q_OS_SYMBIAN)
        emit rotationChanged(-(acos(ry / divider) * RADIANS_TO_DEGREES - 90));
#else
        emit rotationChanged(acos(rx / divider) * RADIANS_TO_DEGREES - 90);
#endif

        return false; // don't store the reading in the sensor
    }

signals:
    void rotationChanged(QVariant deg);
};

#endif // ACCELEROMETERFILTER_H
