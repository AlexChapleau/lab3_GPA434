#pragma once

#include <QPointF>
#include <QPainterPath>

struct Obstacle {
	QPointF center;
	double radius;
	double radius2;
	QPainterPath shape;

	Obstacle(QPointF c, double r)
		: center(c), radius(r), radius2(r* r)
	{
		shape.addEllipse(c, r, r);

	}

	void setRadius(double r) 
	{
		radius = r;
		radius2 = r * r;
		shape.clear();
		shape.addEllipse(center, r, r);
	}
};