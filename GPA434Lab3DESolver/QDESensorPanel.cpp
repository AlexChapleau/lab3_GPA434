#include "QDESensorPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QPainter>

#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "SensorPlacementStrategy.h"
#include "Random.h"
#include "SensorConfigWidget.h"

const QVector<QColor> QDESensorPanel::smRankColors{ {QColor(255, 255, 0),
													 QColor(0, 200, 0),
													 QColor(255, 165, 0),
													 QColor(0, 180, 255),
													 QColor(255, 255, 255)
}};

const QVector<QDESensorPanel::PrecisionPreset> QDESensorPanel::smPrecisionPresets = {
	{
		"Rapide", 25.0, "Calcul très rapide\nPrécision grossière"
	},
	{
		"Équilibré", 15.0, "Bon compromis vitesse / précision\nrecommandée"
	},
	{
		"Précis", 10.0, "Calcul plus lent\nHaute précision"
	}
};

QDESensorPanel::QDESensorPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer }
	, mSensorCountSpin{ new QSpinBox }
	, mSensorListLayout{ new QVBoxLayout }
	, mScrollArea{ new QScrollArea }
    , mObstaclesSB{}
    , mObstaclesRadiusSB{}
	, mResetButton{ new QPushButton("Réinitialiser") }
	, mSolutionCountSpin{ new QSpinBox }
	, mPrecisionBox{ new QComboBox }
	, mCanvas(0, 0, 1500, 500)
{
	assemblingAndLayouting();
	establishConnections();
	buildSensorList();
	adjustScrollAreaHeight();
	generateObstacles(mObstaclesSB->value());
	parameterChanged();
}

de::SolutionStrategy* QDESensorPanel::buildSolution() const
{
	QVector<Sensor*> sensors;
	
	for (Sensor* sensor : collectSensors())
		sensors.emplace_back(sensor->clone()); //donner le ownership des sensors à la stratégie

	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };
	double cellSize{ mPrecisionBox->currentData().toDouble() };

	return new SensorPlacementStrategy(sensors, mObstacles, canvasWidth, canvasHeight, cellSize);
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
	painter.setPen(Qt::NoPen);

	for (const CircleObstacle& obs : mObstacles)
		obs.draw(painter);

	QVector<Sensor*> sensors{ collectSensors() };

	if (de.currentGeneration() > 0)
	{
		const de::Population& population{ de.actualPopulation() };
		const int maxSolutions{ mSolutionCountSpin->value() };
		int count = std::min(maxSolutions, static_cast<int>(population.size()));

		for (int p{ count - 1 }; p >= 0; --p)
		{
			const de::Solution& sol{ population[p] };
			QColor color{ smRankColors[qMin(p, smRankColors.size() - 1)] };
			qreal alpha = std::max(0.12, std::pow(0.6, p));
			color.setAlphaF(alpha);
			qreal width = (p == 0) ? 1.5 : std::max(0.4, 1.2 - 0.15 * p);

			size_t i{};

			for (Sensor* sensor : sensors)
			{
				if (!sensor) continue;

				double x{ sol[i++] };
				double y{ sol[i++] };

				QTransform t;
				t.translate(x, y);

				double angle{};
				if (dynamic_cast<SweepSensor*>(sensor))
				{
					angle = sol[i++];
					t.rotate(angle);
				}

				QPainterPath coverage{ sensor->buildCoverage(QPointF(x, y), angle, mObstacles) };

				if (p == 0)
				{
					painter.setBrush(QColor(255, 255, 0, 100));
					painter.setPen(QPen(Qt::yellow, width));
					painter.drawPath(coverage);

					painter.setBrush(Qt::red);
					painter.setPen(Qt::red);
					painter.drawPath(t.map(sensor->bodyPath()));
				}
				else
				{
					painter.setBrush(Qt::NoBrush);
					painter.setPen(QPen(color, width));
					painter.drawPath(coverage);

					painter.setBrush(color);
					painter.drawPath(t.map(sensor->bodyPath()));
				}
			}
		}
	}
	else
	{
		double spacing{ canvasWidth / (sensors.size() + 1.0) };
		double y{ canvasHeight / 2.0 };

		for (int i{}; i < sensors.size(); ++i)
		{
			painter.save();
			painter.translate(QPointF(spacing * (i + 1), y));

			painter.setBrush(QColor(255, 255, 0, 100));
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

void QDESensorPanel::updateObstacles()
{
	QScrollBar* sb{ qobject_cast<QScrollBar*>(sender()) };

	if (sb == mObstaclesSB)
		generateObstacles(mObstaclesSB->value());

	else if (sb == mObstaclesRadiusSB) {
		double baseR{ static_cast<double>(mObstaclesRadiusSB->value()) };
		for (CircleObstacle& obs : mObstacles)
			obs.setBaseRadius(baseR);
	}

	parameterChanged();
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

void QDESensorPanel::setupGUI()
{
	mSensorCountSpin->setRange(1, 3);
	mSensorCountSpin->setValue(mSensorCountSpin->minimum());

	mSolutionCountSpin->setRange(1, smRankColors.size());
	mSolutionCountSpin->setValue(mSolutionCountSpin->minimum());

	for (int i = 0; i < smPrecisionPresets.size(); ++i)
	{
		const PrecisionPreset& p = smPrecisionPresets[i];
		mPrecisionBox->addItem(p.label, p.cellSize);
		mPrecisionBox->setItemData(i, p.tooltip, Qt::ToolTipRole);
	}

	mPrecisionBox->setCurrentIndex(1);
}

void QDESensorPanel::assemblingAndLayouting()
{
	setupGUI();

	QWidget* sensorWidget{ new QWidget };
	sensorWidget->setLayout(mSensorListLayout);
	mScrollArea->setWidget(sensorWidget);
	mScrollArea->setWidgetResizable(true);

	QGroupBox* configBox{ new QGroupBox("Configuration des capteurs") };
	QHBoxLayout* configLayout{ new QHBoxLayout(configBox) };

	configLayout->addLayout(buildLeftParamsLayout(), 3);
	configLayout->addWidget(mScrollArea, 6);
	configLayout->addLayout(buildRightControlLayout(), 1);

	QVBoxLayout* mainLayout{ new QVBoxLayout };
	mainLayout->addWidget(configBox);
	mainLayout->addWidget(mVisualizationLabel, 1);
	mainLayout->setContentsMargins(0, 0, 0, 0);

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

	connect(sb, &QScrollBar::valueChanged, label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

	return layout;
}

QWidget* QDESensorPanel::buildLegendWidget()
{
	QLabel* icon{ new QLabel };
	icon->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(16, 16));

	QString tooltip("<b>Légende</b><br>");
	for (int i{}; i < smRankColors.size(); ++i)
		tooltip += QString("<font color='%1'>■</font> Solution %2<br>")
		.arg(smRankColors[i].name()).arg(i + 1);

	icon->setToolTip(tooltip);
	return icon;
}

QHBoxLayout* QDESensorPanel::buildLeftParamsLayout()
{
	QVBoxLayout* titles{ new QVBoxLayout };
	titles->addWidget(new QLabel("Nombre de capteurs :"));
	titles->addWidget(new QLabel("Nombre d'obstacles :"));
	titles->addWidget(new QLabel("Rayon des obstacles :"));

	QVBoxLayout* values{ new QVBoxLayout };
	values->addWidget(mSensorCountSpin);
	values->addLayout(buildScrollBarLayout(mObstaclesSB, 10, 30));
	values->addLayout(buildScrollBarLayout(mObstaclesRadiusSB, 30, 40));

	QHBoxLayout* layout{ new QHBoxLayout };
	layout->addLayout(titles);
	layout->addLayout(values);

	return layout;
}

QVBoxLayout* QDESensorPanel::buildRightControlLayout()
{
	QHBoxLayout* precisionLayout{ new QHBoxLayout };
	precisionLayout->addWidget(new QLabel("Précision :"));
	precisionLayout->addWidget(mPrecisionBox);

	QHBoxLayout* solutionLayout{ new QHBoxLayout };
	solutionLayout->addWidget(buildLegendWidget());
	solutionLayout->addWidget(new QLabel("Nb Solutions :"));
	solutionLayout->addWidget(mSolutionCountSpin);

	QVBoxLayout* layout{ new QVBoxLayout };
	layout->addLayout(precisionLayout);
	layout->addLayout(solutionLayout);
	layout->addWidget(mResetButton);

	return layout;
}

void QDESensorPanel::establishConnections()
{
	connect(mSensorCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &QDESensorPanel::onSensorCountChanged);

	connect(mSolutionCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &QDESensorPanel::parameterChanged);

	connect(mPrecisionBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &QDESensorPanel::parameterChanged);

	connect(mResetButton, &QPushButton::clicked,
		this, &QDESensorPanel::reset);

	connect(mObstaclesSB, &QScrollBar::valueChanged,
		this, &QDESensorPanel::updateObstacles);

	connect(mObstaclesRadiusSB, &QScrollBar::valueChanged,
		this, &QDESensorPanel::updateObstacles);
}

void QDESensorPanel::adjustScrollAreaHeight()
{
	QWidget* sensorWidget{ mSensorListLayout->itemAt(0)->widget() };
	int rowHeight{ sensorWidget->sizeHint().height() };
	int totalHeight{ rowHeight + 10 };

	mScrollArea->setFixedHeight(totalHeight);
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

void QDESensorPanel::generateObstacles(int n) 
{
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	mObstacles.clear();
	mObstacles.reserve(n);

	qreal baseR{ static_cast<qreal>(mObstaclesRadiusSB->value()) };
	qreal minDist{ 2.0 * baseR * CircleObstacle::maxScale()};

	int maxTries{ 1000 };

	for (int k{}; k < n; ++k)
	{
		QPointF p;
		bool goodPoint{ false };

		for (int attempt{}; attempt < maxTries; ++attempt)
		{
			qreal x{ Random::real(baseR, canvasWidth - baseR) };
			qreal y{ Random::real(baseR, canvasHeight - baseR) };
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