#include "PolygonBuilder.h"


PolygonBuilder::PolygonBuilder(int peaks, std::string name)
	: mPeakCount{ std::max(peaks,3) }
	, mName{ name }
{
}

std::string PolygonBuilder::name() const{
	return mName;
}