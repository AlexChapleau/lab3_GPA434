#pragma once
#include "PolygonBuilder.h"

class RandomPolygonBuilder : public PolygonBuilder
{
public:
    RandomPolygonBuilder(std::string name = "Polygone aléatoire convexe", int peaks = 3);
    RandomPolygonBuilder(RandomPolygonBuilder const&) = default;
    RandomPolygonBuilder(RandomPolygonBuilder&&) = default;
    RandomPolygonBuilder& operator=(RandomPolygonBuilder const&) = default;
    RandomPolygonBuilder& operator=(RandomPolygonBuilder&&) = default;
    ~RandomPolygonBuilder() = default;

    QPolygonF buildPolygon() override;

private:
    QVector<double> randomAngleSplit();
};

