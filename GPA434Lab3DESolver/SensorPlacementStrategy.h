#pragma once
#include "SolutionStrategy.h"

#include <QVector>

#include "Sensor.h"
#include "CircleObstacle.h"
#include "QDESolutionPanel.h"


class SensorPlacementStrategy : public de::SolutionStrategy
{

public:
	SensorPlacementStrategy() = delete;
	SensorPlacementStrategy(QVector<Sensor*> sensors, QVector<CircleObstacle> obstacles,
							double canvasWidth, double canvasHeight);
	SensorPlacementStrategy(SensorPlacementStrategy const&) = default;
	SensorPlacementStrategy(SensorPlacementStrategy&&) = default;
	SensorPlacementStrategy& operator=(SensorPlacementStrategy const&) = default;
	SensorPlacementStrategy& operator=(SensorPlacementStrategy&&) = default;
	~SensorPlacementStrategy() override = default;

	std::string toString(de::Solution const& solution) const override;

	DEFINE_OVERRIDE_CLONE_METHOD(SensorPlacementStrategy)

//public:
//	QPainterPath debugGridMask(QVector<QPainterPath> const& coveragePaths);

protected:
	double process(de::Solution const& solution) override;

private:
	struct Grid {
		QVector<QRectF> cellShape;
		QVector<bool> covered;

		void reset() {
			std::fill(covered.begin(), covered.end(), false);
		}
	};

private:
	int dimensions() const;
	void configParams();
	bool isInsideCanvas(QVector<QPainterPath> const& bodyPaths) const;
	void buildGrid();
	qsizetype computeCoverageArea(QVector<QPainterPath> const& coveragePaths);

private:
	static const std::string smTitle;
	static const std::string smSummary;
	static const std::string smDescription;
	static const double smCellSize;

	Grid mGrid;
	QVector<Sensor*> mSensors;
	QVector<CircleObstacle> mObstacles;
	double mCanvasWidth;
	double mCanvasHeight;
	
};

