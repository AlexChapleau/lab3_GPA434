#include "CircleObstacle.h"

#include "Random.h"

const double CircleObstacle::smMaxScale{ 1.4 };
const double CircleObstacle::smMinScale{ 0.8 };

CircleObstacle::CircleObstacle(const QPointF& c, double baseRadius)
	: mCenter{ c }
	, mBaseRadius{ baseRadius }
	, mScale{ Random::real(smMinScale, smMaxScale) }
{
}

void CircleObstacle::draw(QPainter& painter) const
{
	painter.drawEllipse(mCenter, radius(), radius());
}

double CircleObstacle::radius() const
{
	return mBaseRadius * mScale;
}

double CircleObstacle::radius2() const
{
	double r{ radius() };
	return r * r;
}

double CircleObstacle::maxScale()
{
	return smMaxScale;
}

double CircleObstacle::minScale()
{
	return smMinScale;
}

QPointF CircleObstacle::center() const 
{
	return mCenter;
}

void CircleObstacle::setBaseRadius(double r)
{
	mBaseRadius = r;
}