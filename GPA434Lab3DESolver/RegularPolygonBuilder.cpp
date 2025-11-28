#include "RegularPolygonBuilder.h"

#include <numbers>

const double RegularPolygonBuilder::smDefaultRadius{ 1.0 };

RegularPolygonBuilder::RegularPolygonBuilder(int peaks, QString name)
    : PolygonBuilder(peaks, name)
    , mRadius{ smDefaultRadius }
{
}

QPolygonF RegularPolygonBuilder::buildPolygon()
{
    QVector<QPointF> points;
    points.reserve(mPeakCount);

    const double pi{ std::numbers::pi };
    double angleSplit{ 2.0 * pi / mPeakCount };

    for (int i = 0; i < mPeakCount; ++i)
    {
        double currentAngle{ i * angleSplit - 0.5 * pi };

        double x{ mRadius * std::cos(currentAngle) };
        double y{ mRadius * std::sin(currentAngle) };

        points.emplace_back(x, y);
    }
    return QPolygonF(points);
}
