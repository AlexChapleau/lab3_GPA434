#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QVector>
#include <QScrollBar>
#include <QLabel>

#include "Sensor.h"
#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"


class SensorConfigWidget : public QWidget
{
    Q_OBJECT

public:
    SensorConfigWidget(Sensor* sensor, QWidget* parent = nullptr);

    Sensor* sensor() const { return mSensor; }

signals:
    void sensorChanged();    

private slots:
    void onTypeChanged(int index);
    void onParamChanged(int value);

private:
    void rebuildParameterUI();
    Sensor* createSensorOfType(int type) const;

private:
    Sensor* mSensor;                 
    QComboBox* mTypeCombo;           
    QFormLayout* mForm;              
    QVector<QScrollBar*> mParamEditors;   
};

