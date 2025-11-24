#pragma once
#include "Polygon.h"

class RandomPolygon : public Polygon
{
public:
    RandomPolygon(int peaks);
    RandomPolygon(RandomPolygon const&) = default;
    RandomPolygon(RandomPolygon&&) = default;
    RandomPolygon& operator=(RandomPolygon const&) = default;
    RandomPolygon& operator=(RandomPolygon&&) = default;
    ~RandomPolygon() = default;

    QVector<QPointF> buildPoints() override;

private:
    std::vector<double> randomAngleSplit();
};

