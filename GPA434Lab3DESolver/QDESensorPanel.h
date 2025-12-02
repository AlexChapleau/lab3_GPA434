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
	void onSensorCountChanged(int);

private:
	void assemblingAndLayouting();
	void clearSensorList();
	void rebuildSensorList();
	QVector<Sensor*> collectSensors() const;
	void adjustScrollAreaHeight();

private:
	QImageViewer* mVisualizationLabel;
	QSpinBox* mSensorCountSpin;
	QVBoxLayout* mSensorListLayout;
	QScrollArea* mScrollArea;
};