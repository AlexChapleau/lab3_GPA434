#pragma once

#include <string>
#include "QDESolutionPanel.h"
#include <SolutionStrategy.h>


class GeoOptimStrategy : public de::SolutionStrategy
{
public:
	GeoOptimStrategy() = delete;
	GeoOptimStrategy(QPolygonF polygon, double canvasWidth, double canvasHeight, QVector<QPointF> obstacles);
	GeoOptimStrategy(GeoOptimStrategy const&) = default;
	GeoOptimStrategy(GeoOptimStrategy&&) = default;
	GeoOptimStrategy& operator=(GeoOptimStrategy const&) = default;
	GeoOptimStrategy& operator=(GeoOptimStrategy&&) = default;
	~GeoOptimStrategy() override = default;

	std::string toString(de::Solution const& solution) const override;

	DEFINE_OVERRIDE_CLONE_METHOD(GeoOptimStrategy)	     

protected:
	double process(de::Solution const& solution) override;

private:
	bool isInsideCanvas(QPolygonF const& poly) const;
	bool isValidSolution(de::Solution const& solution) const;
	bool isColliding(QPolygonF const& poly) const;

private:
	static const std::string smTitle;
	static const std::string smSummary;
	static const std::string smDescription;
	
	QPolygonF mPolygon;
	double mCanvasWidth;
	double mCanvasHeight;
	const QVector<QPointF> mObstacles;
};