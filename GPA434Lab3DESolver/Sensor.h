#pragma once

#include <QPolygonF>
#include <QPainterPath>

class Sensor
{
public:
    Sensor() = delete;
    Sensor(QString name, double range);
    Sensor(Sensor const&) = default;
    Sensor(Sensor&&) = default;
    Sensor& operator=(Sensor const&) = default;
    Sensor& operator=(Sensor&&) = default;
    virtual ~Sensor() = default;

    virtual QPainterPath coveragePath() const = 0;
    QString name() const;
    void setRange(double range);

protected:
    double mRange;

private:
    QString mName;
};

