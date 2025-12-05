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

double Sensor::range() const
{
	return mRange;
}

QString Sensor::name() const
{
	return mName;
}