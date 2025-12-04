#include "SensorConfigWidget.h"


SensorConfigWidget::SensorConfigWidget(QWidget* parent)
    : QWidget(parent)
    , mSensor(nullptr)
    , mForm{ new QFormLayout(this) }
    , mTypeSelectionBox{ new QComboBox }
    , mTypes{
          new CircleSensor,
          new SweepSensor,
          new CurtainSensor
    }
{
    setupGUI();
    establishConnections();
    buildUI();
}

SensorConfigWidget::~SensorConfigWidget()
{
    delete mSensor;
    for (Sensor* s : mTypes)
        delete s;
}

Sensor* SensorConfigWidget::sensor() const
{ 
    return mSensor;
}


void SensorConfigWidget::onTypeChanged(int index)
{
    delete mSensor;

    mSensor = mTypes[index]->clone();

    buildUI();

    emit sensorChanged();
}

void SensorConfigWidget::buildUI()
{
    clearUI();

    QVector<Sensor::Parameter> params{ mSensor->parameters() };
    QHBoxLayout* layout{ new QHBoxLayout };

    for (int i{}; i < params.size(); ++i)
    {
        const Sensor::Parameter p{ params[i] };

        QScrollBar* sb{ new QScrollBar(Qt::Horizontal) };
        sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sb->setRange(p.min, p.max);
        sb->setValue(p.value);
        sb->setProperty("paramIndex", i);

        QLabel* label{ new QLabel(QString::number(sb->value())) };
        label->setFixedWidth(30);

        layout->addWidget(new QLabel(p.name + " :"));
        layout->addWidget(sb);
        layout->addWidget(label);

        connect(sb, &QScrollBar::valueChanged,
            label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

        connect(sb, &QScrollBar::valueChanged,
            this, &SensorConfigWidget::onParamChanged);
    }
    mForm->addRow(layout);
}

void SensorConfigWidget::clearUI()
{
    while (mForm->rowCount() > 1)
    {
        QLayoutItem* labelItem{ mForm->itemAt(1, QFormLayout::LabelRole) };
        QLayoutItem* fieldItem{ mForm->itemAt(1, QFormLayout::FieldRole) };

        if (labelItem && labelItem->widget()) //protection contre les nullptr
            delete labelItem->widget();

        if (fieldItem && fieldItem->widget()) //protection contre les nullptr
            delete fieldItem->widget();

        mForm->removeRow(1);
    }
}

void SensorConfigWidget::setupGUI()
{
    for (Sensor* type : mTypes)
        mTypeSelectionBox->addItem(type->name());

    mTypeSelectionBox->setCurrentIndex(0);
    mSensor = mTypes[mTypeSelectionBox->currentIndex()]->clone();

    mForm->addRow("Type :", mTypeSelectionBox);
}

void SensorConfigWidget::establishConnections()
{
    connect(mTypeSelectionBox, &QComboBox::currentIndexChanged,
        this, &SensorConfigWidget::onTypeChanged);
}

void SensorConfigWidget::onParamChanged(int value)
{
    QScrollBar* sb{ qobject_cast<QScrollBar*>(sender()) };

    int index{ sb->property("paramIndex").toInt() };

    mSensor->setParameter(index, value);

    emit sensorChanged();
}