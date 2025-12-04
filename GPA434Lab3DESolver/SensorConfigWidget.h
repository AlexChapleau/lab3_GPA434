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
    SensorConfigWidget(QWidget* parent = nullptr);
    SensorConfigWidget(SensorConfigWidget const&) = default;
    SensorConfigWidget(SensorConfigWidget&&) = default;
    SensorConfigWidget& operator=(SensorConfigWidget const&) = default;
    SensorConfigWidget& operator=(SensorConfigWidget&&) = default;
    ~SensorConfigWidget();

    Sensor* sensor() const;

signals:
    void sensorChanged();

private slots:
    void onTypeChanged(int index);
    void onParamChanged(int value);

private:
    void buildUI();
    void clearUI();
    void setupGUI();
    void establishConnections();

private:
    Sensor* mSensor;
    QComboBox* mTypeSelectionBox;
    QFormLayout* mForm;
    QVector<QScrollBar*> mParamEditors;
    QVector<Sensor*> mTypes;
};