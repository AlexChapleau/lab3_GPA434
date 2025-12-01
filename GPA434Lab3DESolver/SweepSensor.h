#pragma once
#include "Sensor.h"


class SweepSensor : public Sensor
{
public:
    SweepSensor(QString name = "Capteur conique", double range = 50.0, double angle = 60.0);
    SweepSensor(SweepSensor const&) = default;
    SweepSensor(SweepSensor&&) = default;
    SweepSensor& operator=(SweepSensor const&) = default;
    SweepSensor& operator=(SweepSensor&&) = default;
    ~SweepSensor() = default;

    QVector<Parameter> parameters() const override;
    void setParameter(int index, double value) override;
    QPainterPath coveragePath() const override;
    QPainterPath bodyPath() const override;

private:
    static const double smBodyLength;
    double mAngle;
};

