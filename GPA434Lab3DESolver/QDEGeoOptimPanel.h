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
	void updateObstacles();
	void updateShape();

private:
	QHBoxLayout* buildScrollBarLayout(QScrollBar*& sb, int minRange, int maxRange, int minWidth);
	void setupGUI();
	void assemblingAndLayouting();
	void establishConnections();
	QVector<QPointF> generateObstacles(int n, int width, int height) const;
	QPolygonF computePreviewPolygon() const;

private:

	QImageViewer* mVisualizationLabel;
	QScrollBar* mObstaclesScrollBar;
	QScrollBar* mPeaksScrollBar;
	QComboBox* mPolygonSelectionBox;
	QPushButton* mResetObstaclesButton;
	QVector<QPointF> mObstacles;
	QPolygonF mShape;
	QVector<QPolygonF> mShapeHistory;
};