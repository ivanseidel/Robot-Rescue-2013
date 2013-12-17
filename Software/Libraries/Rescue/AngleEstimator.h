#ifndef AngleEstimator_h
#define AngleEstimator_h

#include "Utilities.h"
#include "Gaussian.h"
#include "sensors/IMUSensor.h"
#include "sensors/DistanceInterface.h"
#include "math.h"

#define MAX_VARIANCE 10000

class AngleEstimator: public Thread
{
protected:

	bool invert;
	int distanceBetween;

public:
	DistanceInterface *dist1, *dist2;

	AngleEstimator(DistanceInterface *_dist1, DistanceInterface *_dist2, int _distanceBetween, bool _invert = false){
		dist1 = _dist1;
		dist2 = _dist2;

		distanceBetween = _distanceBetween;
		invert = _invert;
	}

	Gaussian process(){
		bool invalid = false;
		long dist1Val, dist2Val;
		double 	mean = 0,
				variance = MAX_VARIANCE;

		dist1Val = dist1->readDistance();
		dist2Val = dist2->readDistance();

		// Check if values are within range
		if(dist1Val >= dist1->maxVal || dist2Val >= dist2->maxVal)
			invalid = true;

		// If invalid, return a VERY uncertainty gaussian
		if(invalid)
			return Gaussian(mean, variance);

		// Otherwise, let's calculate the "suposed" angle
		Point p1 = Point(0, dist1Val);
		Point p2 = Point(distanceBetween, dist2Val);

		Line l = Line(p1, p2);

		// Calculate the mean
		mean = l.getAngle()*(invert?-1:1);

		// Calculate the variance, based on the sum of the distance multiplied by the angle
		variance = pow(mean, 2) * 5.0 + (dist1Val + dist2Val) * 0.06;

		return Gaussian(mean, variance);

	}

};

#endif