#pragma once
#include "PolygonBuilder.h"

class RandomPolygonBuilder : public PolygonBuilder
{
public:
    RandomPolygonBuilder(int peaks = 3, std::string name = "Polygone aléatoire convexe");
    RandomPolygonBuilder(RandomPolygonBuilder const&) = default;
    RandomPolygonBuilder(RandomPolygonBuilder&&) = default;
    RandomPolygonBuilder& operator=(RandomPolygonBuilder const&) = default;
    RandomPolygonBuilder& operator=(RandomPolygonBuilder&&) = default;
    ~RandomPolygonBuilder() = default;

    QPolygonF buildPolygon() override;

private:
    QVector<double> randomAngleSplit();
};

