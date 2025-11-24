#pragma once

#include <QPolygonF>
#include <string>

class Polygon 
{
public:
    Polygon(int peaks = 3);
    Polygon(Polygon const&) = default;
    Polygon(Polygon&&) = default;
    Polygon& operator=(Polygon const&) = default;
    Polygon& operator=(Polygon&&) = default;
    virtual ~Polygon() = default;

    virtual QVector<QPointF> buildPoints() = 0;

protected:
    int mPeakCount;
};