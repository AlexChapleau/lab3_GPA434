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
	, mObstaclesScrollBar{}
	, mPeaksScrollBar{}
	, mPolygonSelectionBox{ new QComboBox }
	, mShape{}
	, mBuilders{
		  new CircleSensor,
		  new SweepSensor,
		  new CurtainSensor
	}
	, mSensorCountSpin{ new QSpinBox }
	, mSensorListLayout{ new QVBoxLayout }
	, mSensors{}
{
	setupGUI();
	assemblingAndLayouting();
	establishConnections();
}

QDESensorPanel::~QDESensorPanel()
{

}

de::SolutionStrategy* QDESensorPanel::buildSolution() const
{
	return new GeoOptimStrategy(QPolygonF(), 1.0,
		1.0, QVector<QPointF>() );
}

void QDESensorPanel::updateVisualization(QDEAdapter const& de)
{
	int w{ mVisualizationLabel->width() };
	int h{ mVisualizationLabel->height() };

	QImage img(w, h, QImage::Format_RGB32);
	QPainter painter(&img);

	painter.setRenderHint(QPainter::Antialiasing, true);

	painter.fillRect(img.rect(), QColor(32, 32, 48));



	QPen polyPen(Qt::yellow);
	polyPen.setWidth(1);

	QColor polyFillColor(Qt::yellow);
	polyFillColor.setAlpha(95);

	painter.setPen(polyPen);
	painter.setBrush(polyFillColor);
	painter.setOpacity(1.0);

	painter.drawPath(computePreviewPath());
	
	QPen bodyPen(Qt::red);
	bodyPen.setWidth(1);

	QColor bodyFillColor(Qt::red);

	painter.setPen(bodyPen);
	painter.setBrush(bodyFillColor);
	painter.setOpacity(1.0);
	painter.drawPath(computePreviewBodyPath());
	painter.end();

	mVisualizationLabel->setImage(img);
}


void QDESensorPanel::updateShape()
{
	int index{ mPolygonSelectionBox->currentIndex() };

	mBuilders[index]->setRange(mPeaksScrollBar->value());
	mShape = mBuilders[index]->coveragePath();

	parameterChanged();
}

QHBoxLayout* QDESensorPanel::buildScrollBarLayout(QScrollBar*& sb, int minRange,
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

void QDESensorPanel::setupGUI()
{
	for (Sensor* builder : mBuilders)
		mPolygonSelectionBox->addItem(builder->name());
}

void QDESensorPanel::assemblingAndLayouting()
{
	mSensorCountSpin->setRange(1, 50);
	mSensorCountSpin->setValue(3);

	connect(mSensorCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &QDESensorPanel::onSensorCountChanged);


	mSensorListLayout = new QVBoxLayout;
	mSensorListLayout->setSpacing(6);

	QWidget* sensorListWidget = new QWidget;
	sensorListWidget->setLayout(mSensorListLayout);

	QScrollArea* sensorScrollArea = new QScrollArea;
	sensorScrollArea->setWidget(sensorListWidget);
	sensorScrollArea->setWidgetResizable(true);
	sensorScrollArea->setMinimumHeight(220);   
	sensorScrollArea->setFrameShape(QFrame::NoFrame);

	// Regrouper capteurs
	QFormLayout* sensorForm = new QFormLayout;
	sensorForm->addRow("Nombre de capteurs :", mSensorCountSpin);
	sensorForm->addRow("Configuration des capteurs :", sensorScrollArea);

	QGroupBox* sensorGroup = new QGroupBox("Capteurs");
	sensorGroup->setLayout(sensorForm);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(sensorGroup);

	mainLayout->addWidget(mVisualizationLabel, 1);

	rebuildSensorList();
	setLayout(mainLayout);
}

void QDESensorPanel::establishConnections()
{
	connect(mPolygonSelectionBox, &QComboBox::currentIndexChanged,
		this, &QDESensorPanel::updateShape);

	connect(mPeaksScrollBar, &QScrollBar::valueChanged,
		this, &QDESensorPanel::updateShape);

}

QPainterPath QDESensorPanel::computePreviewPath() const
{
	QPainterPath poly{ mShape };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	QRectF bounds{ poly.boundingRect() };
	double scaleX{ (canvasWidth * 0.3) / bounds.width() };
	double scaleY{ (canvasHeight * 0.3) / bounds.height() };

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);

	return t.map(poly);
}

QPainterPath QDESensorPanel::computePreviewBodyPath() const
{
	int index{ mPolygonSelectionBox->currentIndex() };

	QPainterPath poly{ mBuilders[index]->bodyPath() };
	int canvasWidth{ mVisualizationLabel->width() };
	int canvasHeight{ mVisualizationLabel->height() };

	QRectF bounds{ poly.boundingRect() };
	double scaleX{ (canvasWidth * 0.3) / bounds.width() };
	double scaleY{ (canvasHeight * 0.3) / bounds.height() };

	QTransform t;
	t.translate(canvasWidth / 2, canvasHeight / 2);

	return t.map(poly);
}

void QDESensorPanel::clearSensorList()
{
	QLayoutItem* item;
	while ((item = mSensorListLayout->takeAt(0)) != nullptr) {
		if (QWidget* w = item->widget())
			w->deleteLater();
		delete item;
	}

	// supprimer les capteurs existants
	for (Sensor* s : mSensors)
		delete s;
	mSensors.clear();
}

void QDESensorPanel::rebuildSensorList()
{
	clearSensorList();

	int count = mSensorCountSpin->value();
	mSensors.reserve(count);

	for (int i = 0; i < count; ++i)
	{
		// Sensor par défaut = CircleSensor
		Sensor* s = new CircleSensor;
		mSensors.push_back(s);

		// Ajouter le widget de configuration
		SensorConfigWidget* widget = new SensorConfigWidget(s);
		mSensorListLayout->addWidget(widget);

		// Repaint sur changement
		connect(widget, &SensorConfigWidget::sensorChanged,
			this, &QDESensorPanel::parameterChanged);
	}

	mSensorListLayout->addStretch();
}

void QDESensorPanel::onSensorCountChanged(int)
{
	rebuildSensorList();
}