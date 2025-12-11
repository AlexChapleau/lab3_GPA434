#include "QDESensorPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Qpainter>
#include <QPolygonF>

#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "SensorPlacementStrategy.h"
#include "Random.h"
#include "SensorConfigWidget.h"



QDESensorPanel::QDESensorPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer }
	, mCanvas(0, 0, 1500, 500)
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
	QVector<Sensor*> sensors{ collectSensors() };
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	return new SensorPlacementStrategy(sensors, mObstacles, canvasWidth, canvasHeight);;
}

void QDESensorPanel::assemblingAndLayouting()
{
    mSensorCountSpin->setRange(1, 3);
    mSensorCountSpin->setValue(mSensorCountSpin->minimum());

	QWidget* sensorWidget{ new QWidget };
	sensorWidget->setLayout(mSensorListLayout);
    mScrollArea->setWidget(sensorWidget);
    mScrollArea->setWidgetResizable(true);

	QVBoxLayout* titlesLayout{ new QVBoxLayout };
	titlesLayout->addWidget(new QLabel("Nombre de capteurs :"));
	titlesLayout->addWidget(new QLabel("Nombre d'obstacles :"));
	titlesLayout->addWidget(new QLabel("Rayon des obstacles :"));

	QVBoxLayout* valuesLayout{ new QVBoxLayout };
	valuesLayout->addWidget(mSensorCountSpin);

	QHBoxLayout* obsNumValuesLayout{ new QHBoxLayout };
	obsNumValuesLayout->addLayout(buildScrollBarLayout(mObstaclesSB, 5, 20));
	valuesLayout->addLayout(obsNumValuesLayout);
 
	QHBoxLayout* obsRadiusLayout{ new QHBoxLayout };
	obsRadiusLayout->addLayout(buildScrollBarLayout(mObstaclesRadiusSB, 30, 70));
	valuesLayout->addLayout(obsRadiusLayout);
    
	QHBoxLayout* localParamsLayout{ new QHBoxLayout };
	localParamsLayout->addLayout(titlesLayout);
	localParamsLayout->addSpacing(20);
	localParamsLayout->addLayout(valuesLayout);

	QGroupBox* sensorsConfigBox{ new QGroupBox("Configuration des capteurs") };
	QHBoxLayout* sensorsConfigLayout{ new QHBoxLayout(sensorsConfigBox) };

	sensorsConfigLayout->addLayout(localParamsLayout,3);
	sensorsConfigLayout->addWidget(mScrollArea,6);
	sensorsConfigLayout->addWidget(mResetButton,1);

	QVBoxLayout* mainLayout{ new QVBoxLayout };
	mainLayout->addWidget(sensorsConfigBox);
	mainLayout->addWidget(mVisualizationLabel, 1);
	mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));

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

void QDESensorPanel::updateVisualization(QDEAdapter const& de)
{
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	QImage img(canvasWidth, canvasHeight, QImage::Format_RGB32);
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(img.rect(), QColor(32, 32, 48));

	painter.setBrush(Qt::lightGray);
	painter.setOpacity(0.8);
	qreal r{ static_cast<qreal>(mObstaclesRadiusSB->value()) };

	for (const CircleObstacle& obs : mObstacles)
	{
		obs.draw(painter);
	}

	QVector<Sensor*> sensors{ collectSensors() };
	qsizetype n{ sensors.size() };

	QVector<QPainterPath> coveragePaths;
	if (de.currentGeneration() > 0) {
		const de::Solution bestSolution{ de.actualPopulation().statistics().bestSolution() };
		size_t i{};

		for (Sensor* s : sensors)
		{
			if (!s) //retirer un warning VisualStudio pour un nullptr possible
				continue;
			double x{ bestSolution[i++] };
			double y{ bestSolution[i++] };
			double sensorRange{ s->parameters()[0].value };
			

			QTransform t;
			t.translate(x, y);

			double angle{};
			if (dynamic_cast<SweepSensor*>(s))
			{
				angle = bestSolution[i++];
				t.rotate(angle);
			}

			//qDebug() << "sensorOri =" << angle;
			painter.translate(0,0);

			QPainterPath cov = s->buildCoverage(QPointF(x,y), angle, mObstacles, canvasWidth, canvasHeight);
			painter.setBrush(QColor(255, 255, 0, 80));
			painter.setPen(Qt::yellow);
			painter.drawPath(cov);
			coveragePaths.push_back(cov);

			painter.setBrush(Qt::red);
			painter.setPen(Qt::red);
			painter.drawPath(t.map(s->bodyPath()));

		}
	}
	else {
		double spacing{ canvasWidth / (n + 1.0) };
		double y{ canvasHeight / 2.0 };

		for (int i{}; i < n; ++i)
		{
			double x{ spacing * (i + 1) };
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
		this, &QDESensorPanel::updateObstacles);

	connect(mVisualizationLabel, &QImageViewer::resized, this, [this]() {
		static bool firstResizeDone{ false };

		if (!firstResizeDone) {
			firstResizeDone = true;

			generateObstacles(mObstaclesSB->value());
			
		}

		parameterChanged();
		});
}

void QDESensorPanel::updateObstacles()
{
	QScrollBar* sb = qobject_cast<QScrollBar*>(sender());

	if (sb == mObstaclesSB)
		generateObstacles(mObstaclesSB->value());

	else if (sb == mObstaclesRadiusSB) {
		for (CircleObstacle& obs : mObstacles) 
			obs.setRadius(static_cast<double>(mObstaclesRadiusSB->value()));
	}

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

	int count{ mSensorCountSpin->value() };

	for (int i{}; i < count; ++i)
	{
		SensorConfigWidget* widget{ new SensorConfigWidget };
		mSensorListLayout->addWidget(widget);

		connect(widget, &SensorConfigWidget::sensorChanged,
			this, &QDESensorPanel::parameterChanged);
	}

	mSensorListLayout->addStretch();
}

QVector<Sensor*> QDESensorPanel::collectSensors() const
{
	QVector<Sensor*> list;

	for (int i{}; i < mSensorListLayout->count(); ++i)
	{
		QWidget* w{ mSensorListLayout->itemAt(i)->widget() };

		auto sensorWidget{ qobject_cast<SensorConfigWidget*>(w) };

		if (sensorWidget)
			list.append(sensorWidget->sensor());
	}

	return list;
}

void QDESensorPanel::onSensorCountChanged()
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

void QDESensorPanel::generateObstacles(int n) 
{
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	mObstacles.clear();
	mObstacles.reserve(n);

	qreal r{ static_cast<qreal>(mObstaclesRadiusSB->maximum()) };
	qreal minDist{ 2.0 * r };

	int maxTries{ 1000 };

	for (int k{}; k < n; ++k)
	{
		QPointF p;
		bool goodPoint{ false };

		for (int attempt{}; attempt < maxTries; ++attempt)
		{
			qreal x{ Random::real(r, canvasWidth - r) };
			qreal y{ Random::real(r, canvasHeight - r) };
			p = QPointF(x, y);
			goodPoint = true;

			for (const CircleObstacle& obs : mObstacles)
			{
				const QPointF& center{ obs.center()};
				qreal dx{ p.x() - center.x() };
				qreal dy{ p.y() - center.y() };
				if (dx * dx + dy * dy < minDist * minDist)
				{
					goodPoint = false;
					break;
				}
			}
			if (goodPoint)
				break; 
		}
		
		mObstacles.emplace_back(p,static_cast<double>(mObstaclesRadiusSB->value()));
	}
}


void QDESensorPanel::adjustScrollAreaHeight()
{ 
	QWidget* sensorWidget{ mSensorListLayout->itemAt(0)->widget() };
	int rowHeight{ sensorWidget->sizeHint().height() };
	int totalHeight{ rowHeight + 10 };

	mScrollArea->setFixedHeight(totalHeight);
}