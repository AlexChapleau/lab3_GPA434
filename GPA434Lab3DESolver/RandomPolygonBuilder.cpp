#include "RandomPolygonBuilder.h"

#include <vector>
#include <numbers>

#include "Random.h"

RandomPolygonBuilder::RandomPolygonBuilder(int peaks, QString name)
	: PolygonBuilder(peaks,name)
{
}

QPolygonF RandomPolygonBuilder::buildPolygon()
{
	QVector<QPointF> points;
	points.reserve(mPeakCount);
	QVector<double> angles{ randomAngleSplit() };

	for (double angle : angles) {
		double currentAngle{ angle - 0.5 * std::numbers::pi };
		double r{ Random::real(0.5, 1.0) };

		double x{ r * std::cos(currentAngle) };
		double y{ r * std::sin(currentAngle) };

		points.emplace_back(x, y);
	}
	return QPolygonF(points);
}

QVector<double> RandomPolygonBuilder::randomAngleSplit()
{
	QVector<double> splits(mPeakCount);
	double sum{ 0.0 };

	for (double& split : splits) {
		split = Random::real(0.2, 1.5);
		sum += split;
	}

	for (double& split : splits)
		split = (split / sum) * 2.0 * std::numbers::pi;

	QVector<double> angles(mPeakCount);
	double acc{ 0.0 };
	for (int i = 0; i < mPeakCount; ++i) {
		acc += splits[i];
		angles[i] = acc;
	}

	return angles;
}