#include "SensorConfigWidget.h"


SensorConfigWidget::SensorConfigWidget(Sensor* sensor, QWidget* parent)
    : QWidget(parent)
    , mSensor(sensor)
    , mForm{ new QFormLayout(this) }
    , mTypeSelectionBox{ new QComboBox }
{

    mTypeSelectionBox->addItem("Capteur Circulaire");
    mTypeSelectionBox->addItem("Capteur Conique");
    mTypeSelectionBox->addItem("Capteur Rideau");


    if (dynamic_cast<CircleSensor*>(sensor))
        mTypeSelectionBox->setCurrentIndex(0);
    else if (dynamic_cast<SweepSensor*>(sensor))
        mTypeSelectionBox->setCurrentIndex(1);
    else if (dynamic_cast<CurtainSensor*>(sensor))
        mTypeSelectionBox->setCurrentIndex(2);

    connect(mTypeSelectionBox, &QComboBox::currentIndexChanged,
        this, &SensorConfigWidget::onTypeChanged);

    mForm->addRow("Type :", mTypeSelectionBox);


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
    QHBoxLayout* layout{ new QHBoxLayout };

    for (int i = 0; i < params.size(); ++i)
    {
        const Sensor::Parameter& p = params[i];

        QScrollBar* sb = new QScrollBar(Qt::Horizontal);
        sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sb->setRange(p.min, p.max);
        sb->setValue(p.value);
        sb->setProperty("paramIndex", i);

        QLabel* label{ new QLabel(QString::number(sb->value())) };
        label->setFixedWidth(20);

        layout->addWidget(new QLabel(p.name + " :"));
        layout->addWidget(sb);
        layout->addWidget(label);
        if(params.size()>1 && (i+1) != params.size())
            layout->addSpacing(50);

        connect(sb, &QScrollBar::valueChanged,
            label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

        connect(sb, &QScrollBar::valueChanged,
            this, &SensorConfigWidget::onParamChanged);

        
        mParamEditors.push_back(sb);
    }
    mForm->addRow(layout);
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