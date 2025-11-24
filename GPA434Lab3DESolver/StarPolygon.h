#pragma once

#include <string>
#include "Polygon.h"

class StarPolygon : public Polygon
{
public:
    StarPolygon(int peaks = 3);
    StarPolygon(StarPolygon const&) = default;
    StarPolygon(StarPolygon&&) = default;
    StarPolygon& operator=(StarPolygon const&) = default;
    StarPolygon& operator=(StarPolygon&&) = default;
    ~StarPolygon() = default;

    QVector<QPointF> buildPoints() override;

private:
    static const double smDefaultOuterRadius;
    double mOuterRadius;
    double mInnerRadius;

};

