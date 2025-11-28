#pragma once

#include <string>

#include "PolygonBuilder.h"

class StarPolygonBuilder : public PolygonBuilder
{
public:
    StarPolygonBuilder(int peaks = 3, QString name = "Polygone étoile");
    StarPolygonBuilder(StarPolygonBuilder const&) = default;
    StarPolygonBuilder(StarPolygonBuilder&&) = default;
    StarPolygonBuilder& operator=(StarPolygonBuilder const&) = default;
    StarPolygonBuilder& operator=(StarPolygonBuilder&&) = default;
    ~StarPolygonBuilder() = default;

    QPolygonF buildPolygon() override;

private:
    static const double smDefaultOuterRadius;
    double mOuterRadius;
    double mInnerRadius;

};

