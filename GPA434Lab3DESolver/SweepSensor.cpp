#include "SweepSensor.h"

#include <numbers>
#include <QtMath>

const double SweepSensor::smBodyLength{ 6.0 };

SweepSensor::SweepSensor(QString name, double range, double angle)
	:Sensor(name, range)
	, mAngle{ angle }
{
}

QVector<Sensor::Parameter> SweepSensor::parameters() const
{
	return {
	{ "Portée", mRange, 200.0, 400.0 },
	{ "Angle", mAngle, 30.0, 120.0 }
	};
}

void SweepSensor::setParameter(int index, double value)
{
	if (index == 0) mRange = value;
	if (index == 1) mAngle = value;
}

QPainterPath SweepSensor::coveragePath() const
{
	QPainterPath path;
	const double half = mAngle * 0.5; 

	QRectF circleRect(-mRange, -mRange, 2 * mRange, 2 * mRange);

	path.moveTo(0, 0);

	path.arcTo(circleRect, -half, mAngle);
	path.closeSubpath();

	return path;
}

QPainterPath SweepSensor::bodyPath() const
{
	QPainterPath body;

	const double halfLength{ smBodyLength / 2 };

	body.moveTo(smBodyLength, 0);
	body.lineTo(-halfLength, -halfLength);
	body.lineTo(-halfLength, +halfLength);
	body.closeSubpath();

	return body;
}

Sensor* SweepSensor::clone() const
{
	return new SweepSensor(*this);
}

double SweepSensor::angle() const
{
	return mAngle;
}
