#include "Polygon.h"

Polygon::Polygon(std::string name, int peaks)
	: mName{ name }
	, mPeakCount{ peaks }
	, mPoints{}
{
}

//utilise la formule shoelace
//https://www.101computing.net/the-shoelace-algorithm/
double Polygon::area() const
{
	int n = mPoints.size();
	double sum = 0.0;
	const QPointF* p1{};
	const QPointF* p2{};

	for (int i = 0; i < n; ++i)
	{
		p1 = &mPoints[i];
		p2 = &mPoints[(i + 1) % n];
		sum += (p1->x() * p2->y()) - (p2->x() * p1->y());
	}

	return std::abs(sum) * 0.5;
}

QPolygonF Polygon::toQPolygonF() const
{
	QPolygonF poly;
	for (const QPointF& p : mPoints)
		poly << p;
	return poly;
}

int Polygon::peakCount() const
{
	return 0;
}
