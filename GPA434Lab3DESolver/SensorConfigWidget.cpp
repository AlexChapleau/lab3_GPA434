#include "SensorConfigWidget.h"


SensorConfigWidget::SensorConfigWidget(Sensor* sensor, QWidget* parent)
    : QWidget(parent)
    , mSensor(sensor)
{
    mForm = new QFormLayout(this);
    mForm->setSpacing(6);

    mTypeCombo = new QComboBox;
    mTypeCombo->addItem("Capteur Circulaire");
    mTypeCombo->addItem("Capteur Conique");
    mTypeCombo->addItem("Capteur Rideau");

    
    if (dynamic_cast<CircleSensor*>(sensor))     mTypeCombo->setCurrentIndex(0);
    if (dynamic_cast<SweepSensor*>(sensor))      mTypeCombo->setCurrentIndex(1);
    if (dynamic_cast<CurtainSensor*>(sensor))    mTypeCombo->setCurrentIndex(2);

    connect(mTypeCombo, &QComboBox::currentIndexChanged,
        this, &SensorConfigWidget::onTypeChanged);

    mForm->addRow("Type :", mTypeCombo);

  
    rebuildParameterUI();
}


void SensorConfigWidget::rebuildParameterUI()
{
    while (mForm->rowCount() > 1)
    {
        auto labelItem = mForm->itemAt(1, QFormLayout::LabelRole);
        auto fieldItem = mForm->itemAt(1, QFormLayout::FieldRole);

        if (labelItem && labelItem->widget())
            labelItem->widget()->deleteLater();
        if (fieldItem && fieldItem->widget())
            fieldItem->widget()->deleteLater();

        mForm->removeRow(1);
    }

    mParamEditors.clear();
    auto params = mSensor->parameters();

    for (int i = 0; i < params.size(); ++i)
    {
        const Sensor::Parameter& p = params[i];

        auto sb = new QDoubleSpinBox;
        sb->setRange(p.min, p.max);
        sb->setValue(p.value);
        sb->setDecimals(2);
        sb->setProperty("paramIndex", i);

        connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SensorConfigWidget::onParamChanged);

        mForm->addRow(p.name + " :", sb);
        mParamEditors.push_back(sb);
    }
}

void SensorConfigWidget::onTypeChanged(int index)
{
    delete mSensor;

    mSensor = createSensorOfType(index);

    rebuildParameterUI();

    emit sensorChanged();
}

Sensor* SensorConfigWidget::createSensorOfType(int type) const
{
    switch (type)
    {
    case 0: return new CircleSensor;
    case 1: return new SweepSensor;
    case 2: return new CurtainSensor;
    }
    return new CircleSensor; 
}

void SensorConfigWidget::onParamChanged(double value)
{
    QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(sender());
    int index = sb->property("paramIndex").toInt();

    mSensor->setParameter(index, value);

    emit sensorChanged();
}