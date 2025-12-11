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

int SweepSensor::degreesOfFreedom() const
{
	return 3;
}

QPainterPath SweepSensor::buildCoverage(QPointF pos, double globalOrientation, const QVector<CircleObstacle>& obstacles, double canvasWidth, double canvasHeight) const
{
	QPainterPath area;
	double half{ mAngle / 2.0 };
	double range{ mRange };
	const int rays{ static_cast<int>(mAngle) };

	QVector<QPointF> pts;
	pts.reserve(rays);

	for (int i{}; i < rays; i++)
	{
		double step{ static_cast<double>(i) / (rays - 1) };
		double angDeg{ globalOrientation - half + 2 * half * step };

		QPointF hit{ Sensor::castRay(pos, qDegreesToRadians(angDeg), range, obstacles, canvasWidth, canvasHeight) };
		pts.push_back(hit);
	}

	if (pts.isEmpty())
		return area;

	area.moveTo(pos);

	for (const QPointF& pt : pts)
		area.lineTo(pt);

	area.closeSubpath();
	return area;
}

bool SweepSensor::isCollidingObs(const CircleObstacle& obs, const QTransform& t) const
{
	QPointF c{ t.map(QPointF(0, 0)) };
	QPointF oc = obs.center();

	double dx{ c.x() - obs.center().x() };
	double dy{ c.y() - obs.center().y() };

	double dist{ obs.radius() + smBodyLength };

	return (dx * dx + dy * dy) < dist * dist;
}

Sensor* SweepSensor::clone() const
{
	return new SweepSensor(*this);
}

double SweepSensor::angle() const
{
	return mAngle;
}
