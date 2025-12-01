#include "QDESensorPanel.h"
#include "QDEGeoOptimPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Qpainter>

#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "GeoOptimStrategy.h"
#include "Random.h"
#include "SensorConfigWidget.h"

QDESensorPanel::QDESensorPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer }
	, mSensorCountSpin{ new QSpinBox }
	, mSensorListLayout{ new QVBoxLayout }
{
	assemblingAndLayouting();
}

QDESensorPanel::~QDESensorPanel()
{

}

de::SolutionStrategy* QDESensorPanel::buildSolution() const
{
	return new GeoOptimStrategy(QPolygonF(), 1.0,
		1.0, QVector<QPointF>() );
}

void QDESensorPanel::assemblingAndLayouting()
{
	mSensorCountSpin->setRange(1, 50);
	mSensorCountSpin->setValue(3);

	connect(mSensorCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &QDESensorPanel::onSensorCountChanged);

	QVBoxLayout* listLayout = mSensorListLayout;

	QWidget* listWidget = new QWidget;
	listWidget->setLayout(listLayout);

	QScrollArea* scroll = new QScrollArea;
	scroll->setWidget(listWidget);
	scroll->setWidgetResizable(true);
	scroll->setFixedHeight(100);

	QFormLayout* form = new QFormLayout;
	form->addRow("Nombre de capteurs :", mSensorCountSpin);
	form->addRow("Configuration des capteurs :", scroll);

	QGroupBox* group = new QGroupBox("Capteurs");
	group->setLayout(form);

	QVBoxLayout* main = new QVBoxLayout;
	main->addWidget(group);
	main->addWidget(mVisualizationLabel, 1);

	rebuildSensorList();
	parameterChanged();

	setLayout(main);
}

void QDESensorPanel::updateVisualization(QDEAdapter const&)
{
	int w = mVisualizationLabel->width();
	int h = mVisualizationLabel->height();

	QImage img(w, h, QImage::Format_RGB32);
	QPainter p(&img);
	p.setRenderHint(QPainter::Antialiasing);

	p.fillRect(img.rect(), QColor(32, 32, 48));

	QVector<Sensor*> sensors = collectSensors();
	int n = sensors.size();
	if (n == 0) {
		mVisualizationLabel->setImage(img);
		return;
	}

	double spacing = w / (n + 1.0);
	double y = h / 2.0;

	for (int i = 0; i < n; ++i)
	{
		double x = spacing * (i + 1);
		QPointF pos(x, y);

		p.save();
		p.translate(pos);

		p.setBrush(QColor(255, 255, 0, 80));    
		p.setPen(Qt::yellow);
		p.drawPath(sensors[i]->coveragePath());

		p.setBrush(Qt::red);                    
		p.setPen(Qt::red);
		p.drawPath(sensors[i]->bodyPath());

		p.restore();
	}

	mVisualizationLabel->setImage(img);
}

void QDESensorPanel::clearSensorList()
{
	QLayoutItem* item;
	while ((item = mSensorListLayout->takeAt(0)) != nullptr)
	{
		if (QWidget* w = item->widget())
			w->deleteLater();
		delete item;
	}
}

void QDESensorPanel::rebuildSensorList()
{
	clearSensorList();

	int count = mSensorCountSpin->value();

	for (int i = 0; i < count; ++i)
	{
		SensorConfigWidget* widget = new SensorConfigWidget(new CircleSensor);
		mSensorListLayout->addWidget(widget);

		connect(widget, &SensorConfigWidget::sensorChanged,
			this, &QDESensorPanel::parameterChanged);
	}

	mSensorListLayout->addStretch();
}

QVector<Sensor*> QDESensorPanel::collectSensors() const
{
	QVector<Sensor*> list;

	for (int i = 0; i < mSensorListLayout->count(); ++i)
	{
		QWidget* w = mSensorListLayout->itemAt(i)->widget();
		if (!w) continue;

		auto cfg = qobject_cast<SensorConfigWidget*>(w);
		if (cfg)
			list.append(cfg->sensor());
	}

	return list;
}

void QDESensorPanel::onSensorCountChanged(int)
{
	rebuildSensorList();
}