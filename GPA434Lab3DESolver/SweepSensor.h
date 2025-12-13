#pragma once
#include "Sensor.h"


class SweepSensor : public Sensor
{
public:
    SweepSensor(QString name = "Capteur Conique", double range = 200.0, double angle = 60.0);
    SweepSensor(SweepSensor const&) = default;
    SweepSensor(SweepSensor&&) = default;
    SweepSensor& operator=(SweepSensor const&) = default;
    SweepSensor& operator=(SweepSensor&&) = default;
    ~SweepSensor() override = default;

    double angle() const;
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
    static const double smBodyLength;
    double mAngle;
};

