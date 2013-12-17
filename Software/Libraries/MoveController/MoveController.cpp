#include "MoveDriver.h"
#include "MoveController.h"

#include <math.h>
#include <Arduino.h>

MoveController::MoveController(MoveDriver* _driver){
	driver = _driver;

	// Initialize vars
	direction 	= 0.0;
	headSpeed	= 0.0;
	thetaSpeed 	= 0.0;

	scaleSpeed	= 1;
	
	vectors[0] = (180.0-33.0)	* M_PI/180.0;
	vectors[1] = (180.0+33.0) * M_PI/180.0;
	vectors[2] = (-33.0) * M_PI/180.0;
	vectors[3] = (33.0) * M_PI/180.0;
	

	invert = {1,1,-1,-1};

	for (int i = 0; i < 4; i++) {
		speeds[i] = 0.0;
	}
}

/*
	This function, is the main hart of the class;
	It calculates the speeds for the given motor configuration
	and then saves each motor speed to the speeds[] array.

	Note: It doesen't updates motor speed!
*/

void MoveController::calculate(){
	for(int i = 0; i < 4; i ++){
		speeds[i] = (cos(vectors[i] - direction) * headSpeed + thetaSpeed) * invert[i];
	}
}

MoveController MoveController::stop(){
	setSpeed(0,0,0);
}

MoveController MoveController::setSpeed(double _theta){
	direction = _theta * M_PI/180.0;
	calculate();

	/*Serial.print("Stats for ");
	Serial.println(_theta);

	Serial.print(speeds[1]);
	Serial.print(" - ");
	Serial.print(speeds[2]);
	Serial.print(" - ");
	Serial.print(speeds[3]);
	Serial.print(" - ");
	Serial.println(speeds[4]);*/
	
	driver->setSpeed(speeds[0], speeds[1], speeds[2], speeds[3]);

	return *this;
}


MoveController MoveController::setSpeed(double _theta, double _speed){
	headSpeed = _speed;
	return setSpeed(_theta);
}

MoveController MoveController::setSpeed(double _theta, double _speed, double _thetaSpeed){
	thetaSpeed = _thetaSpeed;
	return setSpeed(_theta, _speed);
}

void MoveController::setMotorSpeeds(double a, double b, double c, double d) {
	speeds[0] = a;
	speeds[1] = b;
	speeds[2] = c;
	speeds[3] = d;
	driver->setSpeed(speeds[0], speeds[1], speeds[2], speeds[3]);
}




