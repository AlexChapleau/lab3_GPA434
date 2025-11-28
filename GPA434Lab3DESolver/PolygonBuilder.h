#pragma once

#include <QPolygonF>
#include <string>

class PolygonBuilder
{
public:
    PolygonBuilder() = delete;
    PolygonBuilder(int peaks, QString name);
    PolygonBuilder(PolygonBuilder const&) = default;
    PolygonBuilder(PolygonBuilder&&) = default;
    PolygonBuilder& operator=(PolygonBuilder const&) = default;
    PolygonBuilder& operator=(PolygonBuilder&&) = default;
    virtual ~PolygonBuilder() = default;

    virtual QPolygonF buildPolygon() = 0;
    QString name() const;
    void setPeakCount(int peaks);

protected:
    int mPeakCount;
    QString mName;
};