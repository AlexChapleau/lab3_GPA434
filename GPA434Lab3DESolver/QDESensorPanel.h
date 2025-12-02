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
	~QDESensorPanel() override = default;

	de::SolutionStrategy* buildSolution() const override;

public slots:
	void updateVisualization(QDEAdapter const& de) override;
	void updateObstacles();
	void onSensorCountChanged(int);
	void reset();

private:
	void assemblingAndLayouting();
	QHBoxLayout* buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange);
	void clearSensorList();
	void buildSensorList();
	void establishConnections();
	QVector<Sensor*> collectSensors() const;
	QVector<QPointF> generateObstacles(int n) const;
	void adjustScrollAreaHeight();

private:
	QImageViewer* mVisualizationLabel;
	QSpinBox* mSensorCountSpin;
	QVBoxLayout* mSensorListLayout;
	QScrollArea* mScrollArea;
	QScrollBar* mObstaclesSB;
	QScrollBar* mObstaclesRadiusSB;
	QVector<QPointF> mObstacles;
	QPushButton* mResetButton;
};