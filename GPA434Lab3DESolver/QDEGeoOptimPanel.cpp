#include "QDEGeoOptimPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>

#include "PeakFunctionMinMaxSolution.h"

QDEGeoOptimPanel::QDEGeoOptimPanel(QWidget* parent)
	: mVisualizationLabel{ new QImageViewer }
	, mObstaclesScrollBar{}
	, mPeaksScrollBar{}
	, mPolygonSelectionBox{}
	, mResetObstaclesButton{}
{
	QGroupBox* parameterGroupBox{ new QGroupBox("Paramètres") };
	QFormLayout* parameterLayout{ new QFormLayout };
	parameterLayout->addRow("Nombre d'obstacles", buildScrollBarLayout(mObstaclesScrollBar));
	parameterLayout->addRow("Sommets", buildScrollBarLayout(mPeaksScrollBar));
	parameterGroupBox->setLayout(parameterLayout);

	QVBoxLayout* layout{ new QVBoxLayout };
	layout->addWidget(parameterGroupBox);
}

de::SolutionStrategy* QDEGeoOptimPanel::buildSolution() const
{
	return new de::PeakFunctionMinMaxSolution;
}

QHBoxLayout* QDEGeoOptimPanel::buildScrollBarLayout(QScrollBar*& sb)
{
	// constants definition
	const int minSize{ 10 };
	const int maxSize{ 200 };
	const int defSize{ 100 };
	const int minWidth{ 150 };

	// create the scroll bar and the label
	sb = new QScrollBar;
	sb->setOrientation(Qt::Horizontal);
	sb->setRange(minSize, maxSize);
	sb->setValue(defSize);
	sb->setMinimumWidth(minWidth);
	sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QLabel* label{ new QLabel(QString::number(defSize)) };
	label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

	// create the layout
	QHBoxLayout* layout{ new QHBoxLayout };
	layout->addWidget(sb);
	layout->addWidget(label);


	return layout;
}

void QDEGeoOptimPanel::setupGUI()
{

}

void QDEGeoOptimPanel::assemblingAndLayouting()
{

}

void QDEGeoOptimPanel::establishConnections()
{

}