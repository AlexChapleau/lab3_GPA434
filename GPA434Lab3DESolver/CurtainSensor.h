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
    ~CurtainSensor() = default;

    QVector<Parameter> parameters() const override;
    void setParameter(int index, double value) override;
    QPainterPath coveragePath() const override;
    QPainterPath bodyPath() const override;
    Sensor* clone() const override;

private:
    static const double mBodyWith;
    double mWidth;
    double mOrientation;
};

