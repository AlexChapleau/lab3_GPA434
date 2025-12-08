#include "CurtainSensor.h"

const double CurtainSensor::mBodyWith{ 2.0 };

CurtainSensor::CurtainSensor(QString name, double range, double width, double orientation)
	: Sensor(name,range)
	, mWidth{ width }
    , mOrientation{ orientation }
{
}

QVector<Sensor::Parameter> CurtainSensor::parameters() const
{
    return {
    { "Portée", mRange, 500.0, 1000.0 },
    { "Largeur", mWidth, 100.0, 400.0 },
    { "Direction", mOrientation, 0.0, 3.0} // 0 = X+, 1 = Y+, 2 = X-, 3 = Y-
    };
}

void CurtainSensor::setParameter(int index, double value)
{
    if (index == 0) mRange = value;
    if (index == 1) mWidth = value;
    if (index == 2) mOrientation = value;
}

QPainterPath CurtainSensor::coveragePath() const
{
    QPainterPath path;
    const double halfHeight{ mWidth / 2.0 };

    path.moveTo(0, -halfHeight);
    path.lineTo(mRange, -halfHeight);
    path.lineTo(mRange, +halfHeight);
    path.lineTo(0, +halfHeight);
    path.closeSubpath();

    QTransform t;
    t.rotate(90.0 * static_cast<int>(mOrientation));

    return t.map(path);
}

QPainterPath CurtainSensor::bodyPath() const
{
    QPainterPath body;

    body.addRect(-mBodyWith / 2, -mWidth / 2, mBodyWith, mWidth);

    QTransform t;
    t.rotate(90.0 * static_cast<int>(mOrientation));

    return t.map(body);
}

Sensor* CurtainSensor::clone() const
{
    return new CurtainSensor(*this);
}

double CurtainSensor::width() const
{
    return mWidth;
}

double CurtainSensor::orientation() const
{
    return mOrientation;
}