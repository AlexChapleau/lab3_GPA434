#pragma once

#include <QPointF>
#include <QPainter>


class Obstacle
{
public:
	Obstacle() = default;
	Obstacle(Obstacle const&) = default;
	Obstacle(Obstacle&&) = default;
	Obstacle& operator=(Obstacle const&) = default;
	Obstacle& operator=(Obstacle&&) = default;
	virtual ~Obstacle() = default;

	virtual void draw(QPainter& painter) const = 0;

};
