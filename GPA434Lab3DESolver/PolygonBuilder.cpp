#include "PolygonBuilder.h"


PolygonBuilder::PolygonBuilder(int peaks, QString name)
	: mPeakCount{ std::max(peaks,3) }
	, mName{ name }
{
}

QString PolygonBuilder::name() const{
	return mName;
}

void PolygonBuilder::setPeakCount(int peaks)
{
	mPeakCount = std::max(peaks, 3);
}
