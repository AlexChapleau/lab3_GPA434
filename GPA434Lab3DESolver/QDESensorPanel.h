#pragma once

#include <QScrollBar>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QSpinbox>
#include <QScrollArea>

#include <SolutionStrategy.h>
#include "QDESolutionPanel.h"
#include "QImageViewer.h"
#include "Sensor.h"

class QDESensorPanel : public QDESolutionPanel
{
public:
	QDESensorPanel(QWidget* parent = nullptr);
	QDESensorPanel(QDESensorPanel const&) = default;
	QDESensorPanel(QDESensorPanel&&) = default;
	QDESensorPanel& operator=(QDESensorPanel const&) = default;
	QDESensorPanel& operator=(QDESensorPanel&&) = default;
	~QDESensorPanel() override;

	de::SolutionStrategy* buildSolution() const override;

public slots:
	void updateVisualization(QDEAdapter const& de) override;
	void updateShape();
	void onSensorCountChanged(int);

private:
	QHBoxLayout* buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange, int minWidth);
	void setupGUI();
	void assemblingAndLayouting();
	void establishConnections();
	QPainterPath computePreviewPath() const;
	QPainterPath computePreviewBodyPath() const;
	void clearSensorList();
	void rebuildSensorList();

private:
	QImageViewer* mVisualizationLabel;
	QScrollBar* mObstaclesScrollBar;
	QScrollBar* mPeaksScrollBar;
	QComboBox* mPolygonSelectionBox;
	QPainterPath mShape;
	QVector<Sensor*> mBuilders;
	QVector<Sensor*> mSensors;
	QSpinBox* mSensorCountSpin;
	QVBoxLayout* mSensorListLayout;
};

