#pragma once
#include "Obstacle.h"


class CircleObstacle : public Obstacle
{
public:
    CircleObstacle() = default;
    CircleObstacle(const QPointF& c, double baseRadius);
    CircleObstacle(CircleObstacle const&) = default;
    CircleObstacle(CircleObstacle&&) = default;
    CircleObstacle& operator=(CircleObstacle const&) = default;
    CircleObstacle& operator=(CircleObstacle&&) = default;
    ~CircleObstacle() override = default;

    void draw(QPainter& painter) const override;

    double radius() const;
    double radius2() const;
    static double maxScale();
    static double minScale();
    QPointF center() const;

    void setBaseRadius(double r);

private:
    static const double smMaxScale;
    static const double smMinScale;

    QPointF mCenter;
    double mBaseRadius;
    double mScale;
};

