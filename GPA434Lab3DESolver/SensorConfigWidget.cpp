#include "SensorConfigWidget.h"


SensorConfigWidget::SensorConfigWidget(Sensor* sensor, QWidget* parent)
    : QWidget(parent)
    , mSensor(sensor)
    , mLayout{new QVBoxLayout}
{
    mLayout->setSpacing(6);

    mTypeCombo = new QComboBox;
    mTypeCombo->addItem("Capteur Circulaire");
    mTypeCombo->addItem("Capteur Conique");
    mTypeCombo->addItem("Capteur Rideau");

    
    if (dynamic_cast<CircleSensor*>(sensor))
        mTypeCombo->setCurrentIndex(0);
    else if (dynamic_cast<SweepSensor*>(sensor))
        mTypeCombo->setCurrentIndex(1);
    else if (dynamic_cast<CurtainSensor*>(sensor))
        mTypeCombo->setCurrentIndex(2);

    connect(mTypeCombo, &QComboBox::currentIndexChanged,
        this, &SensorConfigWidget::onTypeChanged);

    QHBoxLayout* typeRow = new QHBoxLayout;
    typeRow->addWidget(new QLabel("Type :"));
    typeRow->addWidget(mTypeCombo);

    mLayout->addLayout(typeRow);

  
    rebuildParameterUI();
}


void SensorConfigWidget::rebuildParameterUI()
{
    while (mLayout->count() > 1)
    {
        QLayoutItem* item = mLayout->takeAt(1);

        if (item->layout())
            delete item->layout();
        if (item->widget())
            delete item->widget();

        delete item;
    }

    mParamEditors.clear();
    auto params = mSensor->parameters();

    for (int i = 0; i < params.size(); ++i)
    {
        const Sensor::Parameter& p = params[i];

        QLabel* labelName = new QLabel(p.name + " :");

        QScrollBar* sb = new QScrollBar(Qt::Horizontal);
        sb->setRange(p.min, p.max);
        sb->setValue(p.value);
        sb->setProperty("paramIndex", i);

        QLabel* labelValue = new QLabel(QString::number(sb->value()));
        labelValue->setFixedWidth(30);

        QHBoxLayout* h = new QHBoxLayout;
        h->addWidget(sb);
        h->addWidget(labelValue);

        connect(sb, &QScrollBar::valueChanged,
            labelValue, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

        connect(sb, &QScrollBar::valueChanged,
            this, &SensorConfigWidget::onParamChanged);

        mLayout->addWidget(labelName);
        mLayout->addLayout(h);

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

void SensorConfigWidget::onParamChanged(int value)
{
    QScrollBar* sb = qobject_cast<QScrollBar*>(sender());
    if (!sb) return; 

    int index = sb->property("paramIndex").toInt();

    mSensor->setParameter(index, value);

    emit sensorChanged();
}