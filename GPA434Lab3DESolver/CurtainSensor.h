#pragma once
#include "Sensor.h"


class CurtainSensor : public Sensor
{
public:
    CurtainSensor(QString name = "Capteur Rideau", double range = 500.0,
                  double width = 100.0, double orientation = 0.0);
    CurtainSensor(CurtainSensor const&) = default;
    CurtainSensor(CurtainSensor&&) = default;
    CurtainSensor& operator=(CurtainSensor const&) = default;
    CurtainSensor& operator=(CurtainSensor&&) = default;
    ~CurtainSensor() override = default;

    double width() const;
    double orientation() const;
    QVector<Parameter> parameters() const override;
    void setParameter(int index, double value) override;

    QPainterPath coveragePath() const override;
    QPainterPath bodyPath() const override;
    virtual int degreesOfFreedom() const override;

    virtual QPainterPath buildCoverage(QPointF pos, double globalOrientation,
                                       const QVector<CircleObstacle>& obstacles) const override;
    Sensor* clone() const override;

    virtual bool isCollidingObs(const CircleObstacle& obs, const QTransform& t) const override;

private:
    static const double smBodyWidth;
    double mWidth;
    double mOrientation;
};

