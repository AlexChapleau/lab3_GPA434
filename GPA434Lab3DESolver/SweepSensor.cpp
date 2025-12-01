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
	{ "Range", mRange, 1.0, 1000.0 },
	{ "Angle", mAngle, 1.0, 180.0 }
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
	const double halfAngle{ qDegreesToRadians(mAngle / 2.0) };

	QPointF p1(mRange * std::cos(+halfAngle), mRange * std::sin(+halfAngle));
	QPointF p2(mRange * std::cos(-halfAngle), mRange * std::sin(-halfAngle));

	path.moveTo(0, 0); path.lineTo(p1);
	path.lineTo(p2);
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
