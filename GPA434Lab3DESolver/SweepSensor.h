#pragma once
#include "Sensor.h"


class SweepSensor : public Sensor
{
    SweepSensor(QString name = "Capteur à balayage conique", double range = 50.0, double angle = 60.0);
    SweepSensor(SweepSensor const&) = default;
    SweepSensor(SweepSensor&&) = default;
    SweepSensor& operator=(SweepSensor const&) = default;
    SweepSensor& operator=(SweepSensor&&) = default;
    ~SweepSensor() = default;

    QPainterPath coveragePath() const override;

private:
    double mAngle;
};

