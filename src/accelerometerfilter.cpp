/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include <math.h>
#include "accelerometerfilter.h"

QTM_USE_NAMESPACE

/*!
  \class AccelerometerFilter
  \brief Processes raw accelerometer readings into single rotation value which
  is used to rotate the reflection on the turntable disc.
*/
AccelerometerFilter::AccelerometerFilter()
    : m_prevValue(0.0f)
{
}

/*!
  Called when accelerometer \a reading changes.
  Returns false to prevent the reading from propagating.
*/
bool AccelerometerFilter::filter(QAccelerometerReading *reading)
{
    const qreal radians_to_degrees = 57.2957795;
    qreal rx = reading->x();
    qreal ry = reading->y();
    qreal rz = reading->z();

    qreal divider = sqrt(rx * rx + ry * ry + rz * rz);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_6)
    // These devices has accelerometer sensors placed as portrait
    // orientation.
    rx = -(acos(rx / divider) * radians_to_degrees - 90);

    if (fabs(rx - m_prevValue) > 0.1f) {
        emit rotationChanged(rx);
        m_prevValue = rx;
    }
#else
    // And these devices the accelerometer is placed
    // as landscape orientation.
    ry = acos(ry / divider) * radians_to_degrees - 90;

    if (fabs(ry - m_prevValue) > 3.0f) {
        emit rotationChanged(ry);
        m_prevValue = ry;
    }
#endif

    return false;
}
