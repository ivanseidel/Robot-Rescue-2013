#ifndef DistanceEstimator_h
#define DistanceEstimator_h

#include "Utilities.h"
#include "Gaussian.h"
#include "Graphics.h"
#include "sensors/IMUSensor.h"
#include "sensors/DistanceInterface.h"
#include "GaussianAverage.h"
#include "math.h"

#define MAX_VARIANCE 10000

class DistanceEstimator: public Thread
{
protected:
	DistanceInterface *dist1;

	GaussianAverage finalDistance;

public:

	DistanceEstimator(DistanceInterface *_dist1){
		dist1 = _dist1;

		finalDistance = GaussianAverage(4);
	}

	Gaussian process(){
		bool invalid = false;
		long dist1Val;
		double 	mean = 0,
				variance = MAX_VARIANCE;

		dist1Val = dist1->readDistance();

		// Check if values are within range
		if(dist1Val >= dist1->maxVal)
			invalid = true;

		// If invalid, return a VERY uncertainty gaussian
		if(invalid)
			return Gaussian(mean, variance);

		// Calculate the mean
		mean = dist1Val;

		// Calculate the variance, based on the sum of the distance multiplied by the angle
		variance = dist1Val / 15.0;

		// Put on the averageble
		finalDistance.add(Gaussian(mean, variance));

		return finalDistance.process();

	}

};

#endif