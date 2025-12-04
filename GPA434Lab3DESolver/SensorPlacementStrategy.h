#pragma once
#include "SolutionStrategy.h"

#include <QVector>

#include "Sensor.h"



class SensorPlacementStrategy : public de::SolutionStrategy
{

public:
	SensorPlacementStrategy() = delete;
	SensorPlacementStrategy(QVector<Sensor*> sensors, QVector<QPointF> obstacles,
							double obstaclesRadius, double canvasWidth,double canvasHeight);
	SensorPlacementStrategy(SensorPlacementStrategy const&) = default;
	SensorPlacementStrategy(SensorPlacementStrategy&&) = default;
	SensorPlacementStrategy& operator=(SensorPlacementStrategy const&) = default;
	SensorPlacementStrategy& operator=(SensorPlacementStrategy&&) = default;
	~SensorPlacementStrategy() override = default;

	std::string toString(de::Solution const& solution) const override;

	DEFINE_OVERRIDE_CLONE_METHOD(SensorPlacementStrategy)

protected:
	double process(de::Solution const& solution) override;

private:
	struct SensorEntry { 
		Sensor* sensor;
		int dimCount;
	};

private:
	QVector<SensorEntry> buildCatalog() const;
	int dimensions() const;

private:
	static const std::string smTitle;
	static const std::string smSummary;
	static const std::string smDescription;

	QVector<Sensor*> mSensors;
	QVector<QPointF> mObstacles;
	QVector<SensorEntry> mCatalog;
	double mObstaclesRadius;
	double mCanvasWidth;
	double mCanvasHeight;
	
};

