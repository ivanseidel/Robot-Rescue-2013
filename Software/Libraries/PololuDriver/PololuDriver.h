#ifndef POLOLU_DRIVER
#define POLOLU_DRIVER

/*
Class that implements MotorDriver to drive 4 DC motors using Pololu Qik boards on Serial1 and Serial2
*/

#include "MoveDriver.h"

#include <Arduino.h>

//Pololu Qik definitions
#define QIK_MOTOR_M0_BRAKE_AMOUNT		 0x86
#define QIK_MOTOR_M1_BRAKE_AMOUNT		 0x87

#define QIK_MOTOR_M0_FORWARD             0x88
#define QIK_MOTOR_M0_FORWARD_8_BIT       0x89
#define QIK_MOTOR_M0_REVERSE             0x8A
#define QIK_MOTOR_M0_REVERSE_8_BIT       0x8B
#define QIK_MOTOR_M1_FORWARD             0x8C
#define QIK_MOTOR_M1_FORWARD_8_BIT       0x8D
#define QIK_MOTOR_M1_REVERSE             0x8E
#define QIK_MOTOR_M1_REVERSE_8_BIT       0x8F

/*
Serial1 and Serial2 must have been initialized previously!
*/

class PololuDriver : public MoveDriver {
public:
	PololuDriver(); //initialize boards
	
	void getCmd(byte cmd[2], int _qm, int speed); //find out the serial command to be written to Qik, _qm = 0 or 1
	void getBrakeCmd(byte cmd[2], int _qm, int amount);
	
	//Assumes speeds from -255 to 255
	
	bool setBrake(int _m, int amount);
	bool setSpeed(int _m, double speed); //set motor _m to speed
	bool setSpeed(double _sm1, double _sm2, double _sm3, double _sm4, bool brake_if_zero = true); //set all 4 speeds
	bool setConfig(int param, int val);
};

#endif