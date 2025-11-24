#pragma once

#include <QScrollBar>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMetaType>

#include <SolutionStrategy.h>
#include "QDESolutionPanel.h"
#include "QImageViewer.h"


class QDEGeoOptimPanel : public QDESolutionPanel
{
public:
	enum class PolygonType{ Regular, Star, Random };

public:
	QDEGeoOptimPanel(QWidget* parent = nullptr);
	QDEGeoOptimPanel(QDEGeoOptimPanel const&) = default;
	QDEGeoOptimPanel(QDEGeoOptimPanel&&) = default;
	QDEGeoOptimPanel& operator=(QDEGeoOptimPanel const&) = default;
	QDEGeoOptimPanel& operator=(QDEGeoOptimPanel&&) = default;
	~QDEGeoOptimPanel() override = default;

	de::SolutionStrategy* buildSolution() const override;

public slots:
	void updateVisualization(QDEAdapter const& de) override;

private:
	QHBoxLayout* buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange, int minWidth);
	void setupGUI();
	void assemblingAndLayouting();
	void establishConnections();
	std::vector<QPointF> generateObstacles(int n, int width, int height) const;

private:

	QImageViewer* mVisualizationLabel;
	QScrollBar* mObstaclesScrollBar;
	QScrollBar* mPeaksScrollBar;
	QComboBox* mPolygonSelectionBox;
	QPushButton* mResetObstaclesButton;
	mutable std::vector<QPointF> mObstacles;
	mutable QPolygonF mShape;
};