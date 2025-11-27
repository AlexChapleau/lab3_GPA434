#include "StarPolygonBuilder.h"
#include <numbers>

const double StarPolygonBuilder::smDefaultOuterRadius{ 1.0 };

StarPolygonBuilder::StarPolygonBuilder(int peaks, std::string name)
	: PolygonBuilder(peaks,name)
	, mOuterRadius{ smDefaultOuterRadius }
	, mInnerRadius{ mOuterRadius / 2.0 }
{
}

QPolygonF StarPolygonBuilder::buildPolygon()
{
	QVector<QPointF> points;
	int totalPoints{ mPeakCount * 2 };
	points.reserve(totalPoints);
	const double pi{ std::numbers::pi };
	double angleSplit{ 2.0 * pi / totalPoints };

	for (int i{}; i < totalPoints; i++) {
		double currentAngle{ i * angleSplit - 0.5 * pi };

		double r{ (i % 2 == 0 ? mOuterRadius : mInnerRadius) };

		double x{ r * std::cos(currentAngle) };
		double y{ r * std::sin(currentAngle) };

		points.emplace_back(x, y);
	}
	return QPolygonF(points);
}
