#include "SensorCoverageUtils.h"
#include <QDebug>


QPointF SensorCoverageUtils::castRay(QPointF origin, double radAngle, double range, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight)
{
    QPointF u(qCos(radAngle), qSin(radAngle));   

    double minDist{ range };

    for (const Obstacle& obs : obstacles)
    {
        const double dx{ origin.x() - obs.center.x() };
        const double dy{ origin.y() - obs.center.y() };

        const double b{ 2.0 * (dx * u.x() + dy * u.y()) };
        const double c{ dx * dx + dy * dy - obs.radius * obs.radius };
        const double dist{ b * b - 4.0 * c };

        if (dist < 0) continue;
        if (b > 0 && c > minDist * minDist) continue;

        const double t{ (-b - std::sqrt(dist)) * 0.5 };
        if (t > 0 && t < minDist && t <= range)
            minDist = t;
    }

    return  origin + u * minDist;
}

QPainterPath SensorCoverageUtils::buildCircleCoverage(const CircleSensor* sensor, QPointF sensorPos, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight)
{
    QPainterPath area;
    const double range{ sensor->range() };

    QVector<QPointF> pts;
    pts.reserve(90);

    for (int i{}; i < 90; i++)
    {
        QPointF h{ castRay(sensorPos, qDegreesToRadians(4*i), range, obstacles, canvasWidth, canvasHeight) };
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

QPainterPath SensorCoverageUtils::buildSweepCoverage(const SweepSensor* sensor, QPointF sensorPos, double sensorOri, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight)
{
    QPainterPath area;
    double half{ sensor->angle() / 2.0 };
    double range{ sensor->range() };
    const int rays{ static_cast<int>(sensor->angle()) };

    QVector<QPointF> pts;
    pts.reserve(rays);

    for (int i{}; i < rays; i++)
    {
        double step{ static_cast<double>(i) / (rays - 1) };
        double angDeg{ sensorOri - half + 2 * half * step };

        QPointF hit{ castRay(sensorPos, qDegreesToRadians(angDeg), range, obstacles, canvasWidth, canvasHeight) };
        pts.push_back(hit);
    }

    if (pts.isEmpty())
        return area;

    area.moveTo(sensorPos);

    for (const QPointF& pt : pts)
        area.lineTo(pt);

    area.closeSubpath();
    return area;
}

QPainterPath SensorCoverageUtils::buildCurtainCoverage(const CurtainSensor* sensor, QPointF sensorPos, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight)
{
    QVector<QPointF> pts;
    const double half{ sensor->width() / 2.0 };
    const double range{ sensor->range() };

    // 0 = X+, 1 = Y+, 2 = X-, 3 = Y-
    QPointF u;
    switch (static_cast<int>(sensor->orientation())) {
    case 0:
        u = { 1,0 };
        break;
    case 1:
        u = { 0,1 };
        break;
    case 2:
        u = { -1,0 };
        break;
    default:
        u = { 0,-1 };
        break;
    }

    const double angle{ qDegreesToRadians(90.0 * sensor->orientation()) };
    const int rays{ static_cast<int>(half/2.0)};

    for (int i{}; i < rays; i++)
    {
        double shift{ -half + sensor->width() * static_cast<double>(i) / rays };
        QPointF start{ sensorPos + QPointF(-u.y(), u.x()) * shift };

        pts.push_back(castRay(start, angle, range, obstacles, canvasWidth, canvasHeight));
    }

    QPointF baseLeft{ sensorPos + QPointF(-u.y(), u.x()) * -half };
    QPointF baseRight{ sensorPos + QPointF(-u.y(), u.x()) * half };

    QPainterPath area;

    area.moveTo(baseLeft);

    for (const QPointF& p : pts)
        area.lineTo(p);

    area.lineTo(baseRight);
    area.closeSubpath();

    return area;
}

QPainterPath SensorCoverageUtils::buildCoverageForSensor(Sensor* sensor, QPointF sensorPos, double sensorAngle, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight)
{
    if (CircleSensor* c = dynamic_cast<CircleSensor*>(sensor))
        return buildCircleCoverage(c, sensorPos, obstacles, canvasWidth, canvasHeight);

    if (SweepSensor* s = dynamic_cast<SweepSensor*>(sensor))
        return buildSweepCoverage(s, sensorPos, sensorAngle, obstacles, canvasWidth, canvasHeight); //fonction abstraite dans chaque classe

    if (CurtainSensor* t = dynamic_cast<CurtainSensor*>(sensor))
        return buildCurtainCoverage(t, sensorPos, obstacles, canvasWidth, canvasHeight);

    return QPainterPath();
}
