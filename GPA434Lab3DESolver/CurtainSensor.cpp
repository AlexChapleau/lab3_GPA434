#include "CurtainSensor.h"

const double CurtainSensor::smBodyWidth{ 2.0 };

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

    body.addRect(-smBodyWidth / 2, -mWidth / 2, smBodyWidth, mWidth);

    QTransform t;
    t.rotate(90.0 * static_cast<int>(mOrientation));

    return t.map(body);
}

int CurtainSensor::degreesOfFreedom() const
{
    return 2;
}

QPainterPath CurtainSensor::buildCoverage(QPointF pos, double globalOrientation, const QVector<CircleObstacle>& obstacles, double canvasWidth, double canvasHeight) const
{
    QVector<QPointF> pts;
    const double half{ mWidth / 2.0 };
    const double range{ mRange };

    QPointF u;
    switch (static_cast<int>(mOrientation)) {
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

    const double angle{ qDegreesToRadians(90.0 * mOrientation) };
    const int rays{ static_cast<int>(half / 2.0) };

    for (int i{}; i < rays; i++)
    {
        double shift{ -half + mWidth * static_cast<double>(i) / rays };
        QPointF start{ pos + QPointF(-u.y(), u.x()) * shift };

        pts.push_back(castRay(start, angle, range, obstacles, canvasWidth, canvasHeight));
    }

    QPointF baseLeft{ pos + QPointF(-u.y(), u.x()) * -half };
    QPointF baseRight{ pos + QPointF(-u.y(), u.x()) * half };

    QPainterPath area;

    area.moveTo(baseLeft);

    for (const QPointF& p : pts)
        area.lineTo(p);

    area.lineTo(baseRight);
    area.closeSubpath();

    return area;
}

bool CurtainSensor::isCollidingObs(const CircleObstacle& obs, const QTransform& t) const
{
    QPointF c{ t.map(QPointF(0, 0)) };

    QRectF rect;

    if (static_cast<int>(mOrientation) % 2 == 0)
        rect = QRectF(c.x() - smBodyWidth * 0.5,c.y() - mWidth * 0.5,smBodyWidth,mWidth);
    else
        rect = QRectF(c.x() - mWidth * 0.5,c.y() - smBodyWidth * 0.5,mWidth,smBodyWidth);

    double obsX{ obs.center().x() };
    double obsY{ obs.center().y() };

    double closestX{ std::clamp(obsX, rect.left(), rect.right()) };
    double closestY{ std::clamp(obsY, rect.top(), rect.bottom()) };

    double dx{ obsX - closestX };
    double dy{ obsY - closestY };

    return (dx * dx + dy * dy) < obs.radius2();
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