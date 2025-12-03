#include "CurtainSensor.h"

const double CurtainSensor::mBodyWith{ 2.0 };

CurtainSensor::CurtainSensor(QString name, double range, double height)
	: Sensor(name,range)
	, mHeight{ height }
{
}

QVector<Sensor::Parameter> CurtainSensor::parameters() const
{
    return {
    { "Portée", mRange, 500.0, 2000.0 },
    { "Hauteur", mHeight, 100.0, 400.0 }
    };
}

void CurtainSensor::setParameter(int index, double value)
{
    if (index == 0) mRange = value;
    if (index == 1) mHeight = value;
}

QPainterPath CurtainSensor::coveragePath() const
{
    QPainterPath path;
    const double halfHeight{ mHeight / 2.0 };

    path.moveTo(0, -halfHeight);
    path.lineTo(mRange, -halfHeight);
    path.lineTo(mRange, +halfHeight);
    path.lineTo(0, +halfHeight);
    path.closeSubpath();

    return path;
}

QPainterPath CurtainSensor::bodyPath() const
{
    QPainterPath body;

    body.addRect(-mBodyWith / 2, -mHeight / 2, mBodyWith, mHeight);
    return body;
}

Sensor* CurtainSensor::clone() const
{
    return new CurtainSensor(*this);
}
