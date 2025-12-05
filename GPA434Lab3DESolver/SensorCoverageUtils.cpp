#include "SensorCoverageUtils.h"


SensorCoverageUtils::RayHit SensorCoverageUtils::castRay(QPointF origin, double angleDeg,
                                                         double maxDist, const QVector<QPointF>& obs,
                                                         double obsR, double W, double H)
{
    double angle = qDegreesToRadians(angleDeg);
    QPointF dir(qCos(angle), qSin(angle));

    const int steps = 50;                       
    double step = maxDist / steps;

    for (int i = 1; i <= steps; i++)
    {
        QPointF p = origin + dir * (step * i);

        // sortie du canvas
        if (p.x() < 0 || p.x() > W || p.y() < 0 || p.y() > H)
            return { p,true };

        // collision obstacle
        for (const QPointF& o : obs)
            if (QLineF(p, o).length() <= obsR)
                return { p,true };
    }
    return { origin + dir * maxDist,false };
}


QPainterPath SensorCoverageUtils::buildCircleCoverage(const CircleSensor* s,
                                                      QPointF pos,
                                                      const QVector<QPointF>& obs, double obsR,
                                                      double W, double H)
{
    QPainterPath area;
    const double R = s->range();
                      
    QVector<QPointF> pts;
    pts.reserve(360);

    for (int i = 0; i < 360; i ++)
    {
        RayHit h = castRay(pos, i, R, obs, obsR, W, H);
        pts.push_back(h.point);
    }

    if (pts.isEmpty())
        return area;

    area.moveTo(pts[0]);

    for (int k = 1; k < pts.size(); k++)
        area.lineTo(pts[k]);

    area.closeSubpath();
    return area;
}


QPainterPath SensorCoverageUtils::buildSweepCoverage(const SweepSensor* s,
                                                     QPointF pos, double ori,
                                                     const QVector<QPointF>& obs, double obsR,
                                                     double W, double H)
{

    QPainterPath area;

    double half = s->angle() * 0.5; 
    double R = s->range();
    int rays = 100;

    QVector<QPointF> pts;
    pts.reserve(rays);

    for (int k = 0; k <= rays; k++)
    {
        double a = ori - half + (2 * half) * (double(k) / rays);
        QPointF hit = castRay(pos, a, R, obs, obsR, W, H).point;
        pts.push_back(hit);
    }

    if (pts.isEmpty())
        return area;

    area.moveTo(pos);
    for (auto& p : pts)
        area.lineTo(p);
    area.closeSubpath();
    return area;
}


QPainterPath SensorCoverageUtils::buildCurtainCoverage(const CurtainSensor* s,
                                                       QPointF pos,
                                                       const QVector<QPointF>& obs, double obsR,
                                                       double W, double H)
{
    QVector<QPointF> pts;
    double half = s->width() / 2.0;
    double range = s->range();

    // direction selon orientation param  
    // 0 = X+, 1 = Y+, 2 = X-, 3 = Y-
    QPointF dir;
    switch ((int)s->orientation()) {
    case 0: dir = { 1,0 }; break;
    case 1: dir = { 0,1 }; break;
    case 2: dir = { -1,0 }; break;
    default:dir = { 0,-1 }; break;
    }

    double angleDeg = 90.0*s->orientation();
    const int rays = static_cast<int>(s->width());
    for (int i = 0; i < rays; i++)
    {
        double shift = -half + s->width() * i / rays;
        QPointF start = pos + QPointF(-dir.y(), dir.x()) * shift; 

        pts.push_back(castRay(start, angleDeg, range, obs, obsR, W, H).point);
    }

    QPainterPath area;
    if (pts.isEmpty())
        return area;

    area.moveTo(pts[0]);

    for (int i = 1; i < pts.size(); i++) 
        area.lineTo(pts[i]);
 
    area.lineTo(pos + QPointF(-dir.y(), dir.x()) * (half));
    area.lineTo(pos + QPointF(dir.y(), -dir.x()) * (half));
    area.closeSubpath();
    return area;
}


QPainterPath SensorCoverageUtils::buildCoverageForSensor(
    Sensor* sensor, QPointF pos, double angle,
    const QVector<QPointF>& obs, double obsR,
    double W, double H)
{
    if (auto* c = dynamic_cast<CircleSensor*>(sensor))
        return buildCircleCoverage(c, pos, obs, obsR, W, H);

    if (auto* s = dynamic_cast<SweepSensor*>(sensor))
        return buildSweepCoverage(s, pos, angle, obs, obsR, W, H);

    if (auto* t = dynamic_cast<CurtainSensor*>(sensor))
        return buildCurtainCoverage(t, pos, obs, obsR, W, H);

    return QPainterPath(); // fallback
}


QPainterPath SensorCoverageUtils::buildBodyForSensor(Sensor* s, QPointF pos, double angle)
{
    QTransform T; T.translate(pos.x(), pos.y());
    T.rotate(angle);
    return T.map(s->bodyPath());
}