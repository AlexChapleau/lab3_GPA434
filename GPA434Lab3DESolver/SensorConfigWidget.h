#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QVector>

#include "Sensor.h"
#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"


class SensorConfigWidget : public QWidget
{
    Q_OBJECT

public:
    SensorConfigWidget(Sensor* sensor, QWidget* parent = nullptr);

signals:
    void sensorChanged();    

private slots:
    void onTypeChanged(int index);
    void onParamChanged(double value);

private:
    void rebuildParameterUI();
    Sensor* createSensorOfType(int type) const;

private:
    Sensor* mSensor;                 
    QComboBox* mTypeCombo;           
    QFormLayout* mForm;              
    QVector<QDoubleSpinBox*> mParamEditors;   
};

