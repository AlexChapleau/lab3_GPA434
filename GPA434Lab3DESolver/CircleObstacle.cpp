#include "CircleObstacle.h"

CircleObstacle::CircleObstacle(const QPointF& c, double r)
	: mCenter{ c }
	, mRadius{ r }
{
}

void CircleObstacle::draw(QPainter& painter) const
{
	painter.drawEllipse(mCenter, mRadius, mRadius);
}

void CircleObstacle::setRadius(double r)
{
	mRadius = r;
}

double CircleObstacle::radius() const
{
	return mRadius;
}

double CircleObstacle::radius2() const
{
	return mRadius * mRadius;
}

QPointF CircleObstacle::center() const 
{
	return mCenter;
}