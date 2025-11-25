#pragma once

#include <QPolygonF>
#include <string>

class PolygonBuilder
{
public:
    PolygonBuilder() = delete;
    PolygonBuilder(std::string name, int peaks);
    PolygonBuilder(PolygonBuilder const&) = default;
    PolygonBuilder(PolygonBuilder&&) = default;
    PolygonBuilder& operator=(PolygonBuilder const&) = default;
    PolygonBuilder& operator=(PolygonBuilder&&) = default;
    virtual ~PolygonBuilder() = default;

    virtual QPolygonF buildPolygon() = 0;

protected:
    int mPeakCount;
    std::string mName;
};