#pragma once

#include <string>

#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>
#include <SolutionStrategy.h>

class GeoOptimStrategy : public de::SolutionStrategy
{
public:
	GeoOptimStrategy(double width = 1.0, double height = 1.0);
	GeoOptimStrategy(GeoOptimStrategy const&) = default;
	GeoOptimStrategy(GeoOptimStrategy&&) = default;
	GeoOptimStrategy& operator=(GeoOptimStrategy const&) = default;
	GeoOptimStrategy& operator=(GeoOptimStrategy&&) = default;
	~GeoOptimStrategy() override = default;

	std::string toString(de::Solution const& ag) const override;

	DEFINE_OVERRIDE_CLONE_METHOD(GeoOptimStrategy)	     

protected:
	double process(de::Solution const& ag) override;

private:
	static const std::string smTitle;
	static const std::string smSummary;
	static const std::string smDescription;
	
};