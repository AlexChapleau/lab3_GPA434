#include "QDEGeoOptimPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <Qpainter>

#include "PeakFunctionMinMaxSolution.h"
#include "RegularPolygonBuilder.h"
#include "StarPolygonBuilder.h"
#include "RandomPolygonBuilder.h"
#include "GeoOptimStrategy.h"
#include "Random.h"

QDEGeoOptimPanel::QDEGeoOptimPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer}
	, mCanvas(0, 0, 1500, 500)
	, mObstaclesScrollBar{}
	, mPeaksScrollBar{}
	, mPolygonSelectionBox{ new QComboBox }
	, mResetObstaclesButton{ new QPushButton("Regénérer") }
	, mObstacles{}
	, mShape{ }
	, mBuilders{
		  new RegularPolygonBuilder,
		  new StarPolygonBuilder,
		  new RandomPolygonBuilder,
	}
{
	assemblingAndLayouting();
	setupGUI();
	establishConnections();
	parameterChanged();
}

QDEGeoOptimPanel::~QDEGeoOptimPanel()
{
	for (PolygonBuilder* builder : mBuilders)
		delete builder;
}

de::SolutionStrategy* QDEGeoOptimPanel::buildSolution() const
{
	return new GeoOptimStrategy(mShape, mCanvas.width(), mCanvas.height(), mObstacles);
}

void QDEGeoOptimPanel::updateVisualization(QDEAdapter const& de)
{
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	QImage img(canvasWidth, canvasHeight, QImage::Format_RGB32);
	QPainter painter(&img);
	
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	painter.fillRect(img.rect(), QColor(32, 32, 48)); 

	QPen obstaclePen(Qt::white);
	obstaclePen.setWidth(3);
	obstaclePen.setCapStyle(Qt::RoundCap);
	painter.setPen(obstaclePen);

	painter.drawPoints(mObstacles.data(), mObstacles.size());
	
	QPen historyPen(QColor(255,255,255,30));
	historyPen.setWidth(1);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(historyPen);

	QPen polyPen(Qt::yellow);
	polyPen.setWidth(1);

	QColor polyFillColor(Qt::yellow);
	polyFillColor.setAlpha(95);

	painter.setPen(polyPen);
	painter.setBrush(polyFillColor);
	painter.setOpacity(1.0);
	
	if (de.currentGeneration() > 0) {

		const de::Population& population = de.actualPopulation();

		painter.setBrush(Qt::NoBrush);
		painter.setPen(historyPen);

		for (int i{ static_cast<int>(population.size()) - 1 }; i >= 0; --i) {
			if (i == 0) {
				painter.setPen(polyPen);
				painter.setBrush(polyFillColor);
			}

			const de::Solution& sol = population[i];
			QTransform t;
			t.translate(sol[0], sol[1]);
			t.rotate(sol[2]);
			t.scale(sol[3], sol[3]);

			QPolygonF transformed{ t.map(mShape) };
			painter.drawPolygon(transformed);
		}
	}
	else {
		painter.drawPolygon(computePreviewPolygon());
	}
	painter.end();

	mVisualizationLabel->setImage(img);
}

void QDEGeoOptimPanel::updateObstacles()
{
	mObstacles = generateObstacles(mObstaclesScrollBar->value());
	parameterChanged();
}

void QDEGeoOptimPanel::updateShape()
{
	int index{ mPolygonSelectionBox->currentIndex() };

	mBuilders[index]->setPeakCount(mPeaksScrollBar->value());
	mShape = mBuilders[index]->buildPolygon();

	parameterChanged();
}

QHBoxLayout* QDEGeoOptimPanel::buildScrollBarLayout(QScrollBar*& sb, int minRange,
													int maxRange, int minWidth)
{
	sb = new QScrollBar(Qt::Horizontal);
	sb->setRange(minRange, maxRange);
	sb->setMinimumWidth(minWidth);
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

void QDEGeoOptimPanel::setupGUI()
{
	for (PolygonBuilder* builder : mBuilders)
		mPolygonSelectionBox->addItem(builder->name());

	mShape = mBuilders[0]->buildPolygon();
	updateObstacles();
}

void QDEGeoOptimPanel::assemblingAndLayouting()
{
	QHBoxLayout* firstRow{ new QHBoxLayout };
	firstRow->addLayout(buildScrollBarLayout(mObstaclesScrollBar, 10, 500, 150));
	firstRow->addWidget(mResetObstaclesButton);

	QHBoxLayout* secondRow{ new QHBoxLayout };
	secondRow->addWidget(mPolygonSelectionBox);
	secondRow->addLayout(buildScrollBarLayout(mPeaksScrollBar, 3, 20, 100));
	secondRow->addWidget(new QLabel("Sommets"));

	QFormLayout* parameterLayout{ new QFormLayout };
	parameterLayout->addRow("Nombre d'obstacles", firstRow);
	parameterLayout->addRow("Forme géométrique", secondRow);

	QGroupBox* parameterGroupBox{ new QGroupBox("Définition des paramètres") };
	parameterGroupBox->setLayout(parameterLayout);

	QVBoxLayout* layout{ new QVBoxLayout };
	layout->addWidget(parameterGroupBox);
	layout->addWidget(mVisualizationLabel);
	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);
}

void QDEGeoOptimPanel::establishConnections()
{
	connect(mPolygonSelectionBox, &QComboBox::currentIndexChanged,
			this, &QDEGeoOptimPanel::updateShape);

	connect(mResetObstaclesButton, &QPushButton::clicked,
			this, &QDEGeoOptimPanel::updateObstacles);

	connect(mObstaclesScrollBar, &QScrollBar::valueChanged,
			this, &QDEGeoOptimPanel::updateObstacles);

	connect(mPeaksScrollBar, &QScrollBar::valueChanged,
			this, &QDEGeoOptimPanel::updateShape);
}

QVector<QPointF> QDEGeoOptimPanel::generateObstacles(int n) const
{
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	QVector<QPointF> obs;
	obs.reserve(n);

	for (int i{}; i < n; ++i)
	{
		int x{ Random::integer(0.0, canvasWidth) };
		int y{ Random::integer(0.0, canvasHeight) };
		obs.emplace_back(x, y);
	}

	return obs;
}

QPolygonF QDEGeoOptimPanel::computePreviewPolygon() const
{
	QPolygonF poly{ mShape };
	int canvasWidth{ mCanvas.width() };
	int canvasHeight{ mCanvas.height() };

	QRectF bounds{ poly.boundingRect() };
	double scaleX{ (canvasWidth * 0.3) / bounds.width() };
	double scaleY{ (canvasHeight * 0.3) / bounds.height() };
	double scaleFactor{ std::min(scaleX, scaleY) };

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);
	t.scale(scaleFactor, scaleFactor);

	return t.map(poly);
}

