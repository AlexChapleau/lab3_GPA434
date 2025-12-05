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

    struct Parameter {
        QString name;
        double  value;
        double  min;
        double  max;
    };

    virtual QVector<Parameter> parameters() const = 0;
    virtual void setParameter(int index, double value) = 0;
    virtual QPainterPath coveragePath() const = 0;
    virtual QPainterPath bodyPath() const = 0;
    virtual Sensor* clone() const = 0;

    QString name() const;
    void setRange(double range);
    double range() const;

protected:
    double mRange;

private:
    QString mName;
};

