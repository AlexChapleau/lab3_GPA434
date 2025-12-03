#pragma once
#include "Sensor.h"

class CircleSensor : public Sensor
{
public:
    CircleSensor(QString name = "Capteur Circulaire", double range = 100.0);
    CircleSensor(CircleSensor const&) = default;
    CircleSensor(CircleSensor&&) = default;
    CircleSensor& operator=(CircleSensor const&) = default;
    CircleSensor& operator=(CircleSensor&&) = default;
    ~CircleSensor() = default;
    
    QVector<Parameter> parameters() const override;
    void setParameter(int index, double value) override;
    QPainterPath coveragePath() const override;
    QPainterPath bodyPath() const override;
    Sensor* clone() const override;

private:
    static const double smBodyRadius;
};

