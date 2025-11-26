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
	, mShapeHistory{}
{
	setupGUI();
	assemblingAndLayouting();
	establishConnections();
	updateObstacles();
	updateShape();
	
}

de::SolutionStrategy* QDEGeoOptimPanel::buildSolution() const
{

	return new GeoOptimStrategy(mShape, mVisualizationLabel->width(),
							    mVisualizationLabel->height(), mObstacles);
}

void QDEGeoOptimPanel::updateVisualization(QDEAdapter const& de)
{
	int w{ mVisualizationLabel->width() };
	int h{ mVisualizationLabel->height() };

	QImage img(w, h, QImage::Format_RGB32);
	QPainter painter(&img);
	

	painter.setRenderHint(QPainter::Antialiasing, true);
	
	painter.fillRect(img.rect(), QColor(32, 32, 48)); 

	QPen obstaclePen(Qt::white);
	obstaclePen.setWidth(3);
	obstaclePen.setCapStyle(Qt::RoundCap);
	painter.setPen(obstaclePen);

	painter.drawPoints(mObstacles.data(), mObstacles.size());
	
	QPen historyPen(QColor(255,255,255,10));
	historyPen.setWidth(1);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(historyPen);

	for (const QPolygonF& pastShape : mShapeHistory)
	{
		painter.drawPolygon(pastShape);
	}


	QPen polyPen(Qt::yellow); 
	polyPen.setWidth(1);

	QColor polyFillColor(Qt::yellow);
	polyFillColor.setAlpha(80);

	painter.setPen(polyPen);
	painter.setBrush(polyFillColor);
	painter.setOpacity(1.0);

	if (de.currentGeneration() > 0) {
		const de::Solution bestSolution{ de.actualPopulation().statistics().bestSolution() };

		QTransform t;
		t.translate(bestSolution[0], bestSolution[1]);
		t.rotate(bestSolution[2]);
		t.scale(bestSolution[3], bestSolution[3]);

		QPolygonF transformed{ t.map(mShape) };   
		painter.drawPolygon(transformed);        
		mShapeHistory.append(transformed);
	}
	else {
		painter.drawPolygon(computePreviewPolygon());
	}
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

QVector<QPointF> QDEGeoOptimPanel::generateObstacles(int n, int width, int height) const
{
	QVector<QPointF> obs;
	obs.reserve(n);

	for (int i = 0; i < n; ++i)
	{
		int x{ Random::integer(0.0, width) };
		int y{ Random::integer(0.0, height) };
		obs.emplace_back(x, y);
	}

	return obs;
}

QPolygonF QDEGeoOptimPanel::computePreviewPolygon() const
{
	QPolygonF poly{ mShape };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	QRectF bounds{ poly.boundingRect() };
	double scaleX{ (canvasWidth * 0.3) / bounds.width() };
	double scaleY{ (canvasHeight * 0.3) / bounds.height() };
	double scaleFactor{ std::min(scaleX, scaleY) };

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);
	t.scale(scaleFactor, scaleFactor);

	return t.map(poly);
}