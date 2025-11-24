#pragma once
#include "Polygon.h"

class RegularPolygon : public Polygon
{
public:
    RegularPolygon(int peaks);
    RegularPolygon(RegularPolygon const&) = default;
    RegularPolygon(RegularPolygon&&) = default;
    RegularPolygon& operator=(RegularPolygon const&) = default;
    RegularPolygon& operator=(RegularPolygon&&) = default;
    ~RegularPolygon() = default;

    QVector<QPointF> buildPoints() override;

private:
    static const double smDefaultRadius;
    double mRadius;
};

