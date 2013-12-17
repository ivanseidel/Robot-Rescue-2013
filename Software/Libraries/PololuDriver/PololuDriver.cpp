//Implementation of class PololuDriver

#include "PololuDriver.h"
#include <Arduino.h> //in order to use Serial1 and Serial2

PololuDriver::PololuDriver() {
	//Let the boards autodetect baud rate
	Serial1.write(0xAA);
	delay(5);
	Serial2.write(0xAA);
	delay(5);
}

void PololuDriver::getCmd(byte cmd[2], int _qm, int speed) {
	//get commands in useful arrays
	byte forward[] = { QIK_MOTOR_M0_FORWARD, 
					  QIK_MOTOR_M1_FORWARD };
	byte reverse[] = { QIK_MOTOR_M0_REVERSE, 
					  QIK_MOTOR_M1_REVERSE };
	byte forward8[] = { QIK_MOTOR_M0_FORWARD_8_BIT, 
					   QIK_MOTOR_M1_FORWARD_8_BIT };
	byte reverse8[] = { QIK_MOTOR_M0_REVERSE_8_BIT, 
					   QIK_MOTOR_M1_REVERSE_8_BIT };
	
	bool isreverse = 0;
	
	if (speed < 0) {
		speed = -speed;
		isreverse = 1;
	}
	
	if (speed > 255)
		speed = 255;
	
	//if big, convert to 8 bit command
	if (speed > 127) {
		cmd[0] = isreverse ? reverse8[_qm] : forward8[_qm];
		cmd[1] = speed - 128;
	}
	else {
		cmd[0] = isreverse ? reverse[_qm] : forward[_qm];
		cmd[1] = speed;
	}
}

void PololuDriver::getBrakeCmd(byte cmd[2], int _qm, int amount) {
	byte brake[] = { QIK_MOTOR_M0_BRAKE_AMOUNT, 
					 QIK_MOTOR_M1_BRAKE_AMOUNT };
	
	cmd[0] = brake[_qm];
	cmd[1] = amount;
}

bool PololuDriver::setBrake(int _m, int amount) {
	//Bytes to be sent
	byte cmd[2];
	
	//Find out the correct command
	if (_m == M1 || _m == M3)
		getBrakeCmd(cmd, 0, amount);
	else if (_m == M2 || _m == M4)
		getBrakeCmd(cmd, 1, amount);
	else
		cmd[0] = cmd[1] = 0;
	
	//Send it to the correct board
	if (_m == M1 || _m == M2)
		Serial1.write(cmd, 2);
	else
		Serial2.write(cmd, 2);
	
	delay(3);
}

bool PololuDriver::setSpeed(int _m, double speed) {
	int intSpeed = (int) speed;
	
	//Bytes to be sent
	byte cmd[2];
	
	//Find out the correct command
	if (_m == M1 || _m == M3)
		getCmd(cmd, 0, intSpeed);
	else if (_m == M2 || _m == M4)
		getCmd(cmd, 1, intSpeed);
	else
		cmd[0] = cmd[1] = 0;
	
	//Send it to the correct board
	if (_m == M1 || _m == M2)
		Serial1.write(cmd, 2);
	else
		Serial2.write(cmd, 2);
	
	delay(3);
}

bool PololuDriver::setSpeed(double _sm1, double _sm2, double _sm3, double _sm4, bool brake_if_zero) {
	if (_sm1 == 0 && brake_if_zero)
		setBrake(M1, 127);
	else
		setBrake(M1, 0);
	if (_sm2 == 0 && brake_if_zero)
		setBrake(M2, 127);
	else
		setBrake(M2, 0);
	if (_sm3 == 0 && brake_if_zero)
		setBrake(M3, 127);
	else
		setBrake(M3, 0);
	if (_sm4 == 0 && brake_if_zero)
		setBrake(M4, 127);
	else
		setBrake(M4, 0);
	
	setSpeed(M1, _sm1);
	setSpeed(M2, _sm2);
	setSpeed(M3, _sm3);
	setSpeed(M4, _sm4);
}

bool PololuDriver::setConfig(int param, int val) {
	bool ok1 = false, ok2 = false;
	long firstMillis;
	
	Serial1.write(0x84);
	Serial1.write(param);
	Serial1.write(val);
	Serial1.write(0x55);
	Serial1.write(0x2A);
	
	firstMillis = millis();
	while (millis() - firstMillis < 10 && !Serial1.available());
	
	if (Serial1.available() && Serial1.read() == 0)
		ok1 = true;
	
	Serial2.write(0x84);
	Serial2.write(param);
	Serial2.write(val);
	Serial2.write(0x55);
	Serial2.write(0x2A);
	
	firstMillis = millis();
	while (millis() - firstMillis < 10 && !Serial2.available());
	
	if (Serial2.available() && Serial2.read() == 0)
		ok2 = true;
	
	return ok1 && ok2;
}