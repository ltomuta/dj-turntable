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

protected:
    qreal m_PrevValue;

public:
    AccelerometerFilter() : m_PrevValue(0.0f) {}

    bool filter(QAccelerometerReading *reading)
    {
        qreal rx = reading->x();
        qreal ry = reading->y();
        qreal rz = reading->z();

        qreal divider = sqrt(rx * rx + ry * ry + rz * rz);

#if defined(Q_OS_SYMBIAN)
        ry = -(acos(ry / divider) * RADIANS_TO_DEGREES - 90);
        if(fabs(ry - m_PrevValue) > 3.0f) {
            emit rotationChanged(ry);
            m_PrevValue = ry;
        }
#else
        rx = acos(rx / divider) * RADIANS_TO_DEGREES - 90;
        if(fabs(rx - m_PrevValue) > 3.0f) {
            emit rotationChanged(rx);
            m_PrevValue = rx;
        }
#endif

        return false; // don't store the reading in the sensor
    }

signals:
    void rotationChanged(QVariant deg);
};

#endif // ACCELEROMETERFILTER_H
