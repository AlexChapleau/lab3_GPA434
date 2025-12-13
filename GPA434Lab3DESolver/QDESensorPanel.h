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
#include "CircleObstacle.h"
#include "SensorPlacementStrategy.h"

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
	void onSensorCountChanged();
	void reset();

private:
	struct PrecisionPreset
	{
		QString label;
		double cellSize;
		QString tooltip;
	};

private:
	// === Construction UI ===
	void setupGUI();
	void assemblingAndLayouting();
	QHBoxLayout* buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange);
	QWidget* buildLegendWidget();
	QHBoxLayout* buildLeftParamsLayout();
	QVBoxLayout* buildRightControlLayout();
	void establishConnections();
	void adjustScrollAreaHeight();

	// === Comportement UI ======
	void clearSensorList();
	void buildSensorList();

	// === Logique et données ======
	QVector<Sensor*> collectSensors() const;
	void generateObstacles(int n);

private:
	static const QVector<QColor> smRankColors;
	static const QVector<PrecisionPreset> smPrecisionPresets;

	QImageViewer* mVisualizationLabel;
	QSpinBox* mSensorCountSpin;
	QVBoxLayout* mSensorListLayout;
	QScrollArea* mScrollArea;
	QScrollBar* mObstaclesSB;
	QScrollBar* mObstaclesRadiusSB;
	QPushButton* mResetButton;
	QSpinBox* mSolutionCountSpin;
	QComboBox* mPrecisionBox;

	QRect mCanvas;
	QVector<CircleObstacle> mObstacles;

};