#include "StarPolygon.h"
#include <numbers>

const double StarPolygon::smDefaultOuterRadius{ 1.0 };

StarPolygon::StarPolygon(int peaks)
	: Polygon(peaks)
	, mOuterRadius{ smDefaultOuterRadius }
	, mInnerRadius{ mOuterRadius / 2.0 }
{
}

QVector<QPointF> StarPolygon::buildPoints()
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
	return points;
}
