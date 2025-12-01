#include "CircleSensor.h"


const double CircleSensor::smBodyRadius{ 4.0 };

CircleSensor::CircleSensor(QString name, double range)
    :Sensor(name,range)
{
}

QVector<Sensor::Parameter> CircleSensor::parameters() const {
    return {
        { "Range", mRange, 20.0, 200.0 }
    };
}

void CircleSensor::setParameter(int index, double value)
{
    if (index == 0)
        mRange = value;
}

QPainterPath CircleSensor::coveragePath() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), mRange, mRange);
    return path;
}

QPainterPath CircleSensor::bodyPath() const
{
    QPainterPath body;
    body.addEllipse(QPointF(0, 0), smBodyRadius, smBodyRadius);
    return body;
}
