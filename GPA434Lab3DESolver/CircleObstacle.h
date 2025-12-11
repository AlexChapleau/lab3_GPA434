#pragma once
#include "Obstacle.h"


class CircleObstacle : public Obstacle
{
public:
    CircleObstacle() = default;
    CircleObstacle(const QPointF& c, double r);
    CircleObstacle(CircleObstacle const&) = default;
    CircleObstacle(CircleObstacle&&) = default;
    CircleObstacle& operator=(CircleObstacle const&) = default;
    CircleObstacle& operator=(CircleObstacle&&) = default;
    ~CircleObstacle() = default;

    void draw(QPainter& painter) const override;
    void setRadius(double r);
    double radius() const;
    double radius2() const;
    QPointF center() const;

private:
    QPointF mCenter;
    double mRadius;
};

