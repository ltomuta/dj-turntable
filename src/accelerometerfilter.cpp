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
    const qreal newReadingWeight = 0.1f;

    qreal newValue = -(acos(rx / divider) * radians_to_degrees - 90);
#else
    // And these devices the accelerometer is placed
    // as landscape orientation.
    const qreal newReadingWeight = 0.2f;

    qreal newValue = acos(ry / divider) * radians_to_degrees - 90;
#endif

    // Low pass filtering
    qreal value =
            newValue * newReadingWeight + m_prevValue * (1 - newReadingWeight);

    if (fabs(value - m_prevValue) > 0.1f)
        emit rotationChanged(value);

    m_prevValue = value;

    return false;
}
