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
{
	assemblingAndLayouting();
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

    connect(mSensorCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &QDESensorPanel::onSensorCountChanged);

    // ============================================================
    // 1) SCROLL AREA DES CAPTEURS (GroupBox 2 - LEFT)
    // ============================================================
    QWidget* listWidget = new QWidget;
    listWidget->setLayout(mSensorListLayout);

    mScrollArea->setWidget(listWidget);
    mScrollArea->setWidgetResizable(true);

    QGroupBox* sensorsBox = new QGroupBox("Configuration des capteurs");

    QVBoxLayout* sensorsBoxLayout = new QVBoxLayout(sensorsBox);
    //sensorsBoxLayout->setContentsMargins(6, 6, 6, 6);
    sensorsBoxLayout->addWidget(mScrollArea);


    // ============================================================
    // 2) PARAMETRES GENERAUX (GroupBox 1 - RIGHT)
    // ============================================================
    QGroupBox* globalsBox = new QGroupBox("Paramètres généraux");

    // --- Obstacles widgets ---
    QLabel* obsTitle = new QLabel("Obstacles aléatoires");
    obsTitle->setStyleSheet("font-weight: bold;");

    QLabel* lblObsCount = new QLabel("Nombre :");
    QLabel* lblObsRadius = new QLabel("Rayon :");

    QScrollBar* mObstacleCountSB = new QScrollBar(Qt::Horizontal);
    mObstacleCountSB->setRange(0, 50);
    QLabel* mObstacleCountLabel = new QLabel("10");

    QScrollBar* mObstacleRadiusSB = new QScrollBar(Qt::Horizontal);
    mObstacleRadiusSB->setRange(1, 100);
    QLabel* mObstacleRadiusLabel = new QLabel("20");

    QPushButton* resetObsBtn = new QPushButton("Régénérer");


    // ---- Layout interne (grid compact) ----
    QGridLayout* g = new QGridLayout;
    g->setContentsMargins(4, 2, 4, 2);
    g->setHorizontalSpacing(6);
    g->setVerticalSpacing(2);

    // Row 0 : Nombre capteurs
    QHBoxLayout* hCount = new QHBoxLayout;

    hCount->addWidget(new QLabel("Nombre de capteurs :"));
    hCount->addWidget(mSensorCountSpin);
    g->addLayout(hCount, 1, 0);


    // Row 1 : Obstacles Title
    g->addWidget(obsTitle, 2, 0, Qt::AlignCenter);

    // Row 2 : Nombre + Rayon + Bouton
    QHBoxLayout* obsLine = new QHBoxLayout;

    // Nombre
    {
        QHBoxLayout* hNum = new QHBoxLayout;
        hNum->addWidget(lblObsCount);
        hNum->addWidget(mObstacleCountSB);
        hNum->addWidget(mObstacleCountLabel);
        obsLine->addLayout(hNum);
    }

    // Rayon
    {
        QHBoxLayout* hRad = new QHBoxLayout;
        hRad->addWidget(lblObsRadius);
        hRad->addWidget(mObstacleRadiusSB);
        hRad->addWidget(mObstacleRadiusLabel);
        obsLine->addLayout(hRad);
    }

    // Bouton
    //obsLine->addWidget(resetObsBtn);

    g->addLayout(obsLine, 1, 1);


    globalsBox->setLayout(g);


    // ============================================================
    // 3) HORIZONTAL SPLIT : sensors left / globals right
    // ============================================================
    QHBoxLayout* topRow = new QHBoxLayout;

    topRow->addWidget(sensorsBox);
    topRow->setStretchFactor(sensorsBox, 3);   // <= prend plus d'espace

    // globalsBox
    topRow->addWidget(globalsBox);
    topRow->setStretchFactor(globalsBox, 3);   // <= un peu moins large

    // spacer droite
    topRow->addStretch(2);


    // ============================================================
    // 4) Final main layout
    // ============================================================
    QVBoxLayout* main = new QVBoxLayout;
    main->addLayout(topRow);
    main->addWidget(mVisualizationLabel, 1);

    setLayout(main);


    // ============================================================
    // 5) Init
    // ============================================================
    rebuildSensorList();
    adjustScrollAreaHeight();
    parameterChanged();

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
	adjustScrollAreaHeight();
}


void QDESensorPanel::adjustScrollAreaHeight()
{
	if (!mSensorListLayout)
		return;

	int maxSensorsVisible = 1;   
	int rowHeight = 0;

	for (int i = 0; i < mSensorListLayout->count(); ++i)
	{
		QWidget* w = mSensorListLayout->itemAt(i)->widget();
		if (w)
		{
			rowHeight = w->sizeHint().height();
			break;
		}
	}

	if (rowHeight == 0)
		return;

	int spacing = mSensorListLayout->spacing();
	int totalHeight = maxSensorsVisible * rowHeight + (maxSensorsVisible - 1) * spacing + 10;

	mScrollArea->setFixedHeight(totalHeight);
}