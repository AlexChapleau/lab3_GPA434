#pragma once

#include <QPointF>
#include <QPainterPath>

struct Obstacle {
	QPointF center;
	double radius;
	double radius2;
	QPainterPath shape;
	QRectF bbox;

	Obstacle(QPointF c, double r)
		: center(c), radius(r), radius2(r* r)
	{
		shape.addEllipse(c, r, r);
		bbox = QRectF(c.x() - r, c.y() - r, 2 * r, 2 * r);
	}

	void setRadius(double r) 
	{
		radius = r;
		radius2 = r * r;
		shape.clear();
		shape.addEllipse(center, r, r);
		bbox = QRectF(center.x() - r, center.y() - r, 2 * r, 2 * r);
	}
};