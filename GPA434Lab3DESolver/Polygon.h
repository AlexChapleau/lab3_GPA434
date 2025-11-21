#pragma once

#include <vector>
#include <QPolygonF>
#include <string>

class Polygon 
{
public:
    Polygon() = delete;
    Polygon(std::string name, int peaks);
    Polygon(Polygon const&) = default;
    Polygon(Polygon&&) = default;
    Polygon& operator=(Polygon const&) = default;
    Polygon& operator=(Polygon&&) = default;
    virtual ~Polygon() = default;

    double area() const;
    QPolygonF toQPolygonF() const;
    int peakCount() const;
    std::string name() const;

protected:
    virtual void buildPoints() = 0;
    std::vector<QPointF> mPoints;

private:
    std::string mName;
    int mPeakCount;
};