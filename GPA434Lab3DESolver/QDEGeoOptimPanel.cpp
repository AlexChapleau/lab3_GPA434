#include "QDEGeoOptimPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Qpainter>

#include "PeakFunctionMinMaxSolution.h"
#include "RegularPolygon.h"
#include "StarPolygon.h"
#include "RandomPolygon.h"
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
	int nbObstacles{ mObstaclesScrollBar->value() };
	int nbPeaks{ mPeaksScrollBar->value() };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	PolygonType type{ mPolygonSelectionBox->currentData().value<PolygonType>() };

	mObstacles = generateObstacles(nbObstacles,canvasWidth,canvasHeight);

	Polygon* shape{ nullptr };
	switch (type) {
	case PolygonType::Regular:
		shape = new RegularPolygon(nbPeaks);
		break;
	case PolygonType::Star:
		shape = new StarPolygon(nbPeaks);
		break;
	case PolygonType::Random:
		shape = new RandomPolygon(nbPeaks);
		break;
	}
	QVector<QPointF> shapePoints{ shape->buildPoints()};
	delete shape;

	mShape = QPolygonF(shapePoints);

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);
	t.scale(50.0,50.0);
	mShape = t.map(mShape);

	return new GeoOptimStrategy(mShape, canvasWidth, canvasHeight, mObstacles);
}

void QDEGeoOptimPanel::updateVisualization(QDEAdapter const& de)
{
	int w = mVisualizationLabel->width();
	int h = mVisualizationLabel->height();

	// 1) Créer image
	QImage img(w, h, QImage::Format_RGB32);
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// 2) Fond
	painter.fillRect(img.rect(), QColor(20, 22, 35)); // bleu foncé

	// 3) Dessiner obstacles
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(255, 255, 255)); // blanc

	for (const QPointF& p : mObstacles)
		painter.drawRect(p.x(), p.y(), 3, 3);

	// 4) Dessiner les polygones de la population DE
	painter.setPen(QColor(150, 150, 180, 80)); // gris clair translucide
	painter.setBrush(Qt::NoBrush);

	//for (size_t i{}; i < de.actualPopulation().size(); ++i)
	//{
	//	QPolygonF poly = mShape;
	//	QTransform T;
	//	T.translate(de.actualPopulation()[i][0], de.actualPopulation()[i][1]);
	//	T.rotate(de.actualPopulation()[i][2]);
	//	T.scale(de.actualPopulation()[i][3], de.actualPopulation()[i][3]);
	//	painter.drawPolygon(poly);
	//}

	painter.setPen(QPen(QColor(200, 170, 40), 2));
	painter.setBrush(QColor(200, 170, 40, 180)); // jaune translucide

	QPolygonF poly = mShape;
	QTransform T;
	T.translate(de.actualPopulation()[0][0], de.actualPopulation()[0][1]);
	T.rotate(de.actualPopulation()[0][2]);
	T.scale(de.actualPopulation()[0][3], de.actualPopulation()[0][3]);
	painter.drawPolygon(poly);

	painter.end();

	// 6) Afficher l'image
	mVisualizationLabel->setImage(img);
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
	connect(sb, &QScrollBar::valueChanged, this, &QDESolutionPanel::parameterChanged);

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