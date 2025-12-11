#include "CircleSensor.h"


const double CircleSensor::smBodyRadius{ 4.0 };

CircleSensor::CircleSensor(QString name, double range)
    :Sensor(name,range)
{
}

QVector<Sensor::Parameter> CircleSensor::parameters() const {
    return {
        { "Portée", mRange, 100.0, 200.0 }
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


int CircleSensor::degreesOfFreedom() const
{
    return 2;
}

QPainterPath CircleSensor::buildCoverage(QPointF pos, double globalOrientation, const QVector<CircleObstacle>& obstacles, double canvasWidth, double canvasHeight) const
{
    QPainterPath area;
    const double range{ mRange };

    QVector<QPointF> pts;
    pts.reserve(90);

    for (int i{}; i < 90; i++)
    {
        QPointF h{ castRay(pos, qDegreesToRadians(4 * i), range, obstacles, canvasWidth, canvasHeight) };
        pts.push_back(h);
    }

    if (pts.isEmpty())
        return area;

    area.moveTo(pts[0]);

    for (const QPointF& pt : pts)
        area.lineTo(pt);

    area.closeSubpath();
    return area;
}

bool CircleSensor::isCollidingObs(const CircleObstacle& obs, const QTransform& t) const
{
    QPointF c{ t.map(QPointF(0, 0)) };

    double dx{ c.x() - obs.center().x() };
    double dy{ c.y() - obs.center().y() };

    double dist{ obs.radius() + smBodyRadius };

    return (dx * dx + dy * dy) < dist*dist;
}

Sensor* CircleSensor::clone() const
{
    return new CircleSensor(*this);
}
