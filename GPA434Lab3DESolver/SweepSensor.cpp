#include "SweepSensor.h"

#include <numbers>
#include <QtMath>

SweepSensor::SweepSensor(QString name, double range, double angle)
	:Sensor(name, range)
	, mAngle{ angle }
{
}

QPainterPath SweepSensor::coveragePath() const
{
	QPainterPath path;
	double halfAngle{ qDegreesToRadians(mAngle / 2.0) };
	
	QPointF p1(mRange * std::cos(+halfAngle), mRange * std::sin(+halfAngle));
	QPointF p2(mRange * std::cos(-halfAngle), mRange * std::sin(-halfAngle));

	path.moveTo(0, 0);
	path.lineTo(p1);
	path.lineTo(p2);
	path.closeSubpath();

	return path;
}
