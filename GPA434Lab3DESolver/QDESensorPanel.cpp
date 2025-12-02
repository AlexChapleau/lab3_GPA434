#include "QDESensorPanel.h"
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
	, mScrollArea{ new QScrollArea }
    , mObstaclesSB{}
    , mObstaclesRadiusSB{}
	, mResetButton{ new QPushButton("Réinitialiser") }
{
	assemblingAndLayouting();
	establishConnections();
	buildSensorList();
	adjustScrollAreaHeight();
	parameterChanged();
}

de::SolutionStrategy* QDESensorPanel::buildSolution() const
{
	return new GeoOptimStrategy(QPolygonF(), 1.0,
		1.0, QVector<QPointF>());
}

void QDESensorPanel::assemblingAndLayouting()
{
    mSensorCountSpin->setRange(2, 6);
    mSensorCountSpin->setValue(mSensorCountSpin->minimum());

    QWidget* sensorWidget = new QWidget;
	sensorWidget->setLayout(mSensorListLayout);
    mScrollArea->setWidget(sensorWidget);
    mScrollArea->setWidgetResizable(true);

    QVBoxLayout* titlesLayout = new QVBoxLayout; 
	titlesLayout->addWidget(new QLabel("Nombre de capteurs :"));
	titlesLayout->addWidget(new QLabel("Nombre d'obstacles :"));
	titlesLayout->addWidget(new QLabel("Rayon des obstacles :"));

	QVBoxLayout* valuesLayout = new QVBoxLayout; 
	valuesLayout->addWidget(mSensorCountSpin);

    QHBoxLayout* obsNumValuesLayout = new QHBoxLayout;
	obsNumValuesLayout->addLayout(buildScrollBarLayout(mObstaclesSB, 5, 30));
	valuesLayout->addLayout(obsNumValuesLayout);
 
    QHBoxLayout* obsRadiusLayout = new QHBoxLayout;
	obsRadiusLayout->addLayout(buildScrollBarLayout(mObstaclesRadiusSB, 10, 30));
	valuesLayout->addLayout(obsRadiusLayout);
    
	QHBoxLayout* localParamsLayout = new QHBoxLayout;
	localParamsLayout->addLayout(titlesLayout);
	localParamsLayout->addSpacing(20);
	localParamsLayout->addLayout(valuesLayout);

	QGroupBox* sensorsConfigBox = new QGroupBox("Configuration des capteurs");
	QHBoxLayout* sensorsConfigLayout = new QHBoxLayout(sensorsConfigBox);

	sensorsConfigLayout->addLayout(localParamsLayout);
	sensorsConfigLayout->addSpacing(10);
	sensorsConfigLayout->addWidget(mScrollArea);
	sensorsConfigLayout->addWidget(mResetButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(sensorsConfigBox);
	mainLayout->addWidget(mVisualizationLabel, 1);

    setLayout(mainLayout);

}

QHBoxLayout* QDESensorPanel::buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange)
{
	sb = new QScrollBar(Qt::Horizontal);
	sb->setRange(minRange, maxRange);
	sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QLabel* label{ new QLabel(QString::number(sb->value())) };
	label->setFixedWidth(20);

	QHBoxLayout* layout{ new QHBoxLayout };
	layout->addWidget(sb);
	layout->addWidget(label);

	connect(sb, &QScrollBar::valueChanged,
		label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

	return layout;
}

void QDESensorPanel::updateVisualization(QDEAdapter const&)
{
	int w = mVisualizationLabel->width();
	int h = mVisualizationLabel->height();

	QImage img(w, h, QImage::Format_RGB32);
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(img.rect(), QColor(32, 32, 48));

	painter.setBrush(Qt::lightGray);
	painter.setOpacity(0.8);
	qreal r = static_cast<qreal>(mObstaclesRadiusSB->value());
	for (const QPointF& pos : mObstacles)
	{
		painter.drawEllipse(pos, r, r);
	}


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

		painter.save();
		painter.translate(pos);

		painter.setBrush(QColor(255, 255, 0, 80));
		painter.setPen(Qt::yellow);
		painter.drawPath(sensors[i]->coveragePath());

		painter.setBrush(Qt::red);
		painter.setPen(Qt::red);
		painter.drawPath(sensors[i]->bodyPath());

		painter.restore();
	}

	mVisualizationLabel->setImage(img);
}

void QDESensorPanel::establishConnections()
{
	connect(mSensorCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &QDESensorPanel::onSensorCountChanged);

	connect(mResetButton, &QPushButton::clicked,
		this, &QDESensorPanel::reset);

	connect(mObstaclesSB, &QScrollBar::valueChanged,
		this, &QDESensorPanel::updateObstacles);

	connect(mObstaclesRadiusSB, &QScrollBar::valueChanged,
		this, &QDESensorPanel::parameterChanged);

	connect(mVisualizationLabel, &QImageViewer::resized, this, [this]() {
		static bool firstResizeDone{ false };

		if (!firstResizeDone) {
			firstResizeDone = true;

			updateObstacles();
			
		}

		parameterChanged();
		});
}

void QDESensorPanel::updateObstacles()
{
	mObstacles = generateObstacles(mObstaclesSB->value());
	parameterChanged();
}

void QDESensorPanel::clearSensorList()
{
	QLayoutItem* item;
	while ((item = mSensorListLayout->takeAt(0)) != nullptr)
	{
		if (QWidget* w = item->widget())
			delete w;
		delete item;
	}
}

void QDESensorPanel::buildSensorList()
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

		auto sensorWidget = qobject_cast<SensorConfigWidget*>(w);

		if (sensorWidget)
			list.append(sensorWidget->sensor());
	}

	return list;
}

void QDESensorPanel::onSensorCountChanged(int)
{
	buildSensorList();
	parameterChanged();
}

void QDESensorPanel::reset() 
{
	mObstaclesSB->setValue(mObstaclesSB->minimum());
	mObstaclesRadiusSB->setValue(mObstaclesRadiusSB->minimum());

	buildSensorList();
	updateObstacles();
}

QVector<QPointF> QDESensorPanel::generateObstacles(int n) const
{
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	QVector<QPointF> obs;
	obs.reserve(n);

	qreal r = static_cast<qreal>(mObstaclesRadiusSB->maximum());
	qreal minDist = 2.0 * r;               

	int maxTries = 1000; 

	for (int k = 0; k < n; ++k)
	{
		QPointF p;
		bool goodPoint = false;

		for (int attempt = 0; attempt < maxTries; ++attempt)
		{
			qreal x = Random::real(r, canvasWidth - r);
			qreal y = Random::real(r, canvasHeight - r);
			p = QPointF(x, y);

			goodPoint = true;

			for (const QPointF& q : obs)
			{
				qreal dx = p.x() - q.x();
				qreal dy = p.y() - q.y();
				if (dx * dx + dy * dy < minDist * minDist)
				{
					goodPoint = false;
					break;
				}
			}

			if (goodPoint)
				break; 
		}

		obs.push_back(p);
	}

	return obs;
}


void QDESensorPanel::adjustScrollAreaHeight()
{ 
	QWidget* sensorWidget = mSensorListLayout->itemAt(0)->widget();
	int rowHeight = sensorWidget->sizeHint().height();

	int totalHeight = rowHeight + 10;

	mScrollArea->setFixedHeight(totalHeight);
}