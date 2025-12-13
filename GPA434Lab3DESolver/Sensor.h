#pragma once

#include <QPolygonF>
#include <QPainterPath>

#include "CircleObstacle.h"



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

    QString name() const;
    double range() const;
    virtual QVector<Parameter> parameters() const = 0;

    virtual void setParameter(int index, double value) = 0;
    void setRange(double range);

    virtual QPainterPath coveragePath() const = 0;
    virtual QPainterPath bodyPath() const = 0;
    virtual int degreesOfFreedom() const = 0;
    virtual QPainterPath buildCoverage(QPointF pos,double globalOrientation,
                                    const QVector<CircleObstacle>& obstacles) const = 0;
    virtual Sensor* clone() const = 0;

    virtual bool isCollidingObs(const CircleObstacle& obs, const QTransform& t) const = 0;
    static bool isCollidingSensor(const QPainterPath& sensorBody, const QPainterPath& otherSensorBody);

protected:
    static QPointF castRay(QPointF origin, double radAngle, double range, const QVector<CircleObstacle>& obstacles);

protected:
    double mRange;

private:
    QString mName;
};
