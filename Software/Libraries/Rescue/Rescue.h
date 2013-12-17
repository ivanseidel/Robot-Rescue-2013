
/*
	1. Sense
		- Read IR sensors
		- Filter data
		- Get realative wall angle
		- Get relative speed and angle from IMU
	2. Localize
		- Based on current LAST localization:
			- Find nearest lowest error
	3. Map
		- Based on the Sense from sensors (1):
			- Update probability map
	4. Move

	Classes:
		PoseEstimator
		Gaussian
		Link
		Node
		Map

*/

#ifndef Rescue_h
#define Rescue_h

enum Direction{
	North = 0,
	West  = 3,
	South = 2,
	East  = 1
};

enum Movement{
	Stop = 0,
	Front  = 1,
	Left = 2,
	Right  = 3
};

#include "Wall.h"
#include "Cell.h"
#include "Map.h"

#endif