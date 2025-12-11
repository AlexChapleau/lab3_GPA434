#include "Sensor.h"

Sensor::Sensor(QString name, double range)
	: mName{ name }
	, mRange{ range }
{
}

void Sensor::setRange(double range)
{
	mRange = range;
}

double Sensor::range() const
{
	return mRange;
}

bool Sensor::isCollidingSensor(const QPainterPath& sensorBody, const QPainterPath& otherSensorBody)
{
    if (!sensorBody.boundingRect().intersects(otherSensorBody.boundingRect()))
        return false;

    if (!sensorBody.intersected(otherSensorBody).isEmpty())
        return true;

    return false;
}

QString Sensor::name() const
{
	return mName;
}

QPointF Sensor::castRay(QPointF origin, double radAngle, double range, const QVector<CircleObstacle>& obstacles, double canvasWidth, double canvasHeight)
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