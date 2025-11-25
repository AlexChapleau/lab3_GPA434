#include "PolygonBuilder.h"


PolygonBuilder::PolygonBuilder(std::string name, int peaks)
	: mPeakCount{ std::max(peaks,3) }
	, mName{ name }
{
}

