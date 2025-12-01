#include "Sensor.h"

Sensor::Sensor(QString name, double range)
	: mName{ name }
	, mRange{ range }
{
}

void Sensor::setRange(double range)
{
	mRange = range;
}

QString Sensor::name() const
{
	return mName;
}