#include "CircleSensor.h"

CircleSensor::CircleSensor(QString name, double range)
    :Sensor(name,range)
{
}

QPainterPath CircleSensor::coveragePath() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), mRange, mRange);
    return path;
}