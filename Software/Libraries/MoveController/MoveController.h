/*
  MoveController.h - Responsable for moving motor acordingly to the Robot Structure
  Initialize this, with an instance of MoveDriver.

  This class is only implemented to work with Omni directional robots.

  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifndef MoveController_h
#define MoveController_h

#include <math.h>

#include "MoveDriver.h"

class MoveController{
public:

	double 	direction,	// Current theta (in RADIANS)
			headSpeed,	// Current linear speed
			thetaSpeed,	// Current angular speed
		 	speeds[4]; 	// Cache of calculated speeds
	
	void calculate(); 	// Calculate and save results to speeds[]

	// Class configuration
	float 	scaleSpeed, 	// Used to "scale" speed of the robot in ALL terms
								// Value should be between 0.0 and 1.0

			vectors[4];			// Keeps the angles (in RADIANS) of the vector of each wheel

	int   	invert[4];			// Should invert the motor power? -1 and 1

public:
	// Link to the driver, that deals with hardware stuff
	MoveDriver* driver;

	MoveController(MoveDriver* _driver);

	MoveController stop();
	MoveController setSpeed(double _theta);
	MoveController setSpeed(double _theta, double _speed);
	MoveController setSpeed(double _theta, double _speed, double _thetaSpeed);
	void setMotorSpeeds(double a, double b, double c, double d);
};

#endif