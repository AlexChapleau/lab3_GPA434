#pragma once

#include <QPointF>
#include <QPainterPath>
#include <QVector>
#include <QtMath>

#include "Obstacle.h"
#include "Sensor.h"
#include "CircleSensor.h"
#include "SweepSensor.h"
#include "CurtainSensor.h"

namespace SensorCoverageUtils {


    QPointF castRay(QPointF origin, double radAngle, double range, const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight);

    QPainterPath buildCircleCoverage(const CircleSensor* sensor, QPointF sensorPos, const QVector<Obstacle>& obstacles,
                                     double canvasWidth, double canvasHeight);

    QPainterPath buildSweepCoverage(const SweepSensor* sensor, QPointF sensorPos, double sensorOri,
                                    const QVector<Obstacle>& obstacles, double canvasWidth, double canvasHeight);

    QPainterPath buildCurtainCoverage(const CurtainSensor* sensor,
                                      QPointF sensorPos,
                                      const QVector<Obstacle>& obstacles,
                                      double canvasWidth, double canvasHeight);

    QPainterPath buildCoverageForSensor(Sensor* sensor, QPointF sensorPos, double sensorAngle,
                                        const QVector<Obstacle>& obstacles,
                                        double canvasWidth, double canvasHeight);

}

