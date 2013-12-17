/*
  MoveDriver.h - Simple interface to create generic driver classes
  Interfaced class, should implement all methods.
  MoveController class is the one that will keep an instance of MoveDriver class,
  to send commands to the motors.
  This is the "low-level" class, responsable for communications, and interface with pins.

  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifndef MoveDriver_h
#define MoveDriver_h
	
enum Motor{
	M1	= 1,
	M2	= 2,
	M3	= 3,
	M4	= 4
};

// enum RobotModel{
// 	Mecanum,
// 	Omni,
// 	BasicTwoWheels,
// 	BasicFourWheels
// };

// Base class used to instantiate every kind of motor output controller.
class MoveDriver{
protected:
	// Cached data
	double speed[4];
	float ticks[4];

public:

	// Speed functions
	virtual bool setSpeed(int _m, double speed){};		// Set speed on only one motor
	// virtual bool setSpeed(double *_speeds){};			// Set speed for all motors, given the array of speeds (all at once)
	virtual bool setSpeed(double _sm1, double _sm2, double _sm3, double _sm4, bool brake_if_zero = true){};

	/*virtual long getSpeed(Motor _m);		// Only returns last cached result
	virtual void getSpeeds(long* _speeds);	// Only returns all last cached results*/

	// Encoder functions
	virtual long readTicks(Motor _m = M1){};		// Read one motor ticks
	virtual long getTicks(Motor _m){};		// Get last cached result of motor ticks read
	/*virtual void readTicks(long* _ticks);	// Read all motor ticks
	virtual void getTicks(long* _ticks);	// Get last cached result of all motor ticks*/

};



#endif