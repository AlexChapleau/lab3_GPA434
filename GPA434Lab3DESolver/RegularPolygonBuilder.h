#pragma once
#include "PolygonBuilder.h"

class RegularPolygonBuilder : public PolygonBuilder
{
public:
    RegularPolygonBuilder(int peaks = 3, std::string name = "Polygone Régulier");
    RegularPolygonBuilder(RegularPolygonBuilder const&) = default;
    RegularPolygonBuilder(RegularPolygonBuilder&&) = default;
    RegularPolygonBuilder& operator=(RegularPolygonBuilder const&) = default;
    RegularPolygonBuilder& operator=(RegularPolygonBuilder&&) = default;
    ~RegularPolygonBuilder() = default;

    QPolygonF buildPolygon() override;

private:
    static const double smDefaultRadius;
    double mRadius;
};

