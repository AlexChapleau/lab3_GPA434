#include "Sensor.h"

Sensor::Sensor(QString name, double range)
	: mName{ name }
	, mRange{ range }
{
}

QString Sensor::name() const
{
	return mName;
}

double Sensor::range() const
{
	return mRange;
}

void Sensor::setRange(double range)
{
	mRange = range;
}


bool Sensor::isCollidingSensor(const QPainterPath& sensorBody, const QPainterPath& otherSensorBody)
{
    if (!sensorBody.boundingRect().intersects(otherSensorBody.boundingRect()))
        return false;

    if (!sensorBody.intersected(otherSensorBody).isEmpty())
        return true;

    return false;
}


QPointF Sensor::castRay(QPointF origin, double radAngle, double range, const QVector<CircleObstacle>& obstacles)
{
    QPointF u(qCos(radAngle), qSin(radAngle));

    double minDist{ range };

    for (const CircleObstacle& obs : obstacles)
    {
        const double dx{ origin.x() - obs.center().x()};
        const double dy{ origin.y() - obs.center().y()};

        const double b{ 2.0 * (dx * u.x() + dy * u.y()) };
        const double c{ dx * dx + dy * dy - obs.radius2() };
        const double dist{ b * b - 4.0 * c };

        if (dist < 0) continue;
        if (b > 0 && c > minDist * minDist) continue;

        const double t{ (-b - std::sqrt(dist)) * 0.5 };
        if (t > 0 && t < minDist && t <= range)
            minDist = t;
    }

    return  origin + u * minDist;
}