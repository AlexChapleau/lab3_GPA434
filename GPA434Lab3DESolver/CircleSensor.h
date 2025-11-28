#pragma once
#include "Sensor.h"

class CircleSensor : public Sensor
{
public:
    CircleSensor(QString name = "Capteur Circulaire", double range = 50.0);
    CircleSensor(CircleSensor const&) = default;
    CircleSensor(CircleSensor&&) = default;
    CircleSensor& operator=(CircleSensor const&) = default;
    CircleSensor& operator=(CircleSensor&&) = default;
    ~CircleSensor() = default;

    QPainterPath coveragePath() const override;

};

