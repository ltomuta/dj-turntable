/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef ACCELEROMETERFILTER_H
#define ACCELEROMETERFILTER_H

#include <QAccelerometerFilter>
#include <QVariant>

QTM_USE_NAMESPACE

class AccelerometerFilter : public QObject, public QAccelerometerFilter
{
    Q_OBJECT

public:
    explicit AccelerometerFilter();

protected: // from QAccelerometerFilter
    bool filter(QAccelerometerReading *reading);

signals:
    void rotationChanged(QVariant deg);

private:
    qreal m_prevValue;
};

#endif // ACCELEROMETERFILTER_H
