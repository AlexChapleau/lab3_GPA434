#pragma once

#include <QPointF>
#include <QPainterPath>
#include <QVector>
#include <QtMath>

#include "Sensor.h"
#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"

namespace SensorCoverageUtils {

    struct RayHit {
        QPointF point;    // point atteint
        bool blocked;     // true si obstacle touché avant maxDist
    };

    // ================== Raycasting ==================
    RayHit castRay(QPointF origin, double angleDeg, double maxDist,
        const QVector<QPointF>& obstacles,
        double obstacleRadius,
        double canvasWidth, double canvasHeight);

    // ================== Coverage Builder ==================
    QPainterPath buildCircleCoverage(const CircleSensor* s,
        QPointF pos,
        const QVector<QPointF>& obstacles, double obsR,
        double canvasW, double canvasH);

    QPainterPath buildSweepCoverage(const SweepSensor* s,
        QPointF pos, double orientation,
        const QVector<QPointF>& obstacles, double obsR,
        double canvasW, double canvasH);

    QPainterPath buildCurtainCoverage(const CurtainSensor* s,
        QPointF pos,
        const QVector<QPointF>& obstacles, double obsR,
        double canvasW, double canvasH);

    // ================== Interface principale ==================
    QPainterPath buildCoverageForSensor(Sensor* sensor, QPointF pos, double angle,
        const QVector<QPointF>& obstacles, double obsR,
        double canvasW, double canvasH);

    // Body transform
    QPainterPath buildBodyForSensor(Sensor* sensor, QPointF pos, double angle);

}

