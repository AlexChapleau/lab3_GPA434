#include "QDEGeoOptimPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Qpainter>

#include "PeakFunctionMinMaxSolution.h"
#include "RegularPolygonBuilder.h"
#include "StarPolygonBuilder.h"
#include "RandomPolygonBuilder.h"
#include "GeoOptimStrategy.h"
#include "Random.h"

QDEGeoOptimPanel::QDEGeoOptimPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer }
	, mObstaclesScrollBar{}
	, mPeaksScrollBar{}
	, mPolygonSelectionBox{ new QComboBox }
	, mResetObstaclesButton{ new QPushButton("Regénérer") }
	, mObstacles{}
	, mShape{}
{
	setupGUI();
	assemblingAndLayouting();
	establishConnections();
	
}

de::SolutionStrategy* QDEGeoOptimPanel::buildSolution() const
{
	return new GeoOptimStrategy(mShape, mVisualizationLabel->width(),
							    mVisualizationLabel->height(), mObstacles);
}

void QDEGeoOptimPanel::updateVisualization(QDEAdapter const& de)
{
	int w = mVisualizationLabel->width();
	int h = mVisualizationLabel->height();

	QImage img(w, h, QImage::Format_RGB32);
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing, true);

	painter.fillRect(img.rect(), QColor(20, 22, 35)); 

	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(255, 255, 255)); 

	for (const QPointF& p : mObstacles)
		painter.drawRect(p.x(), p.y(), 3, 3);

	painter.setPen(QColor(150, 150, 180, 80)); 
	painter.setBrush(Qt::NoBrush);

	
	painter.setPen(QPen(QColor(200, 170, 40), 2));
	painter.setBrush(QColor(200, 170, 40, 180)); 

	QPolygonF poly = mShape;
	QTransform T;

	if (de.currentGeneration() > 0) {
		const de::Solution bestSolution{ de.actualPopulation().statistics().bestSolution() };

		T.translate(bestSolution[0], bestSolution[1]);
		T.rotate(bestSolution[2]);
		T.scale(bestSolution[3], bestSolution[3]);
		painter.drawPolygon(T.map(poly));
	}
	else
		painter.drawPolygon(poly);
	painter.end();

	mVisualizationLabel->setImage(img);
}

void QDEGeoOptimPanel::updateObstacles()
{
	int nbObstacles{ mObstaclesScrollBar->value() };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	mObstacles = generateObstacles(nbObstacles, canvasWidth, canvasHeight);
	parameterChanged();
}

void QDEGeoOptimPanel::updateShape()
{
	int nbPeaks{ mPeaksScrollBar->value() };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };
	PolygonType type{ mPolygonSelectionBox->currentData().value<PolygonType>() };

	PolygonBuilder* shape{ nullptr };
	switch (type) {
	case PolygonType::Regular:
		shape = new RegularPolygonBuilder("Polygone régulier", nbPeaks);
		break;
	case PolygonType::Star:
		shape = new StarPolygonBuilder("Polygone étoile", nbPeaks);
		break;
	case PolygonType::Random:
		shape = new RandomPolygonBuilder("Polygone aléatoire convexe", nbPeaks);
		break;
	}

	mShape = shape->buildPolygon();
	delete shape;

	QRectF bounds = mShape.boundingRect();
	double scaleX = (canvasWidth * 0.4) / bounds.width();
	double scaleY = (canvasHeight * 0.4) / bounds.height();
	double scaleFactor = std::min(scaleX, scaleY);

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);
	t.scale(scaleFactor, scaleFactor);
	mShape = t.map(mShape);
	parameterChanged();
}

QHBoxLayout* QDEGeoOptimPanel::buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange,int minWidth)
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

	connect(sb, &QScrollBar::valueChanged, label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

	return layout;
}

void QDEGeoOptimPanel::setupGUI()
{
	mPolygonSelectionBox->addItem("Polygone régulier", QVariant::fromValue(PolygonType::Regular));
	mPolygonSelectionBox->addItem("Polygone étoile", QVariant::fromValue(PolygonType::Star));
	mPolygonSelectionBox->addItem("Polygone aléatoire convexe", QVariant::fromValue(PolygonType::Random));
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

	setLayout(layout);
}

void QDEGeoOptimPanel::establishConnections()
{
	connect(mPolygonSelectionBox, &QComboBox::currentIndexChanged,
			this, &QDEGeoOptimPanel::updateShape);

	connect(mResetObstaclesButton, &QPushButton::clicked,
			this, &QDEGeoOptimPanel::updateObstacles);

	connect(mVisualizationLabel, &QImageViewer::resized,
			this, &QDESolutionPanel::parameterChanged);

	connect(mObstaclesScrollBar, &QScrollBar::valueChanged,
			this, &QDEGeoOptimPanel::updateObstacles);

	connect(mPeaksScrollBar, &QScrollBar::valueChanged,
			this, &QDEGeoOptimPanel::updateShape);
}

std::vector<QPointF> QDEGeoOptimPanel::generateObstacles(int n, int width, int height) const
{
	std::vector<QPointF> obs;
	obs.reserve(n);

	for (int i = 0; i < n; ++i)
	{
		int x = Random::integer(0.0, width);
		int y = Random::integer(0.0, height);
		obs.emplace_back(x, y);
	}

	return obs;
}