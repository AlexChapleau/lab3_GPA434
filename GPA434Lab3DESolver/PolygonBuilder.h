#pragma once

#include <QPolygonF>
#include <string>

class PolygonBuilder
{
public:
    PolygonBuilder() = delete;
    PolygonBuilder(int peaks, std::string name);
    PolygonBuilder(PolygonBuilder const&) = default;
    PolygonBuilder(PolygonBuilder&&) = default;
    PolygonBuilder& operator=(PolygonBuilder const&) = default;
    PolygonBuilder& operator=(PolygonBuilder&&) = default;
    virtual ~PolygonBuilder() = default;

    virtual QPolygonF buildPolygon() = 0;
    std::string name() const;

protected:
    int mPeakCount;
    std::string mName;
};