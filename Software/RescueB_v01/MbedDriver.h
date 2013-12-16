#ifndef MbedDriver_h
#define MbedDriver_h

#include <Arduino.h>

class MbedDriver: public MoveDriver{
protected:
	// Cached data
	float speed[4];
	float ticks[4];

	USARTClass* serial;
	
	uint8_t response[20];
	uint8_t send[10];
	
	static const long TIMEOUT = 6000; // 6ms

	static const char MOTOR_1 = 0x00;
	static const char MOTOR_2 = 0x01;
	static const char MOTOR_3 = 0x02;
	static const char MOTOR_4 = 0x03;
	
	static const char ADR_CMD_INIT = 0x02;
	static const char ADR_CMD_RESET = 0x03;
	static const char ADR_CMD_STANDBY = 0x04;
	static const char ADR_CMD_PID = 0x09;
	
	static const char ADR_SET_SPEED_START = 0x10;
	static const char ADR_SET_SPEEDS = 0x15;
	
	static const char ADR_GET_SPEED_START = 0x20;
	static const char ADR_GET_SPEEDS = 0x25;
	
	static const char ADR_GET_ENCODER_START = 0x30;
	static const char ADR_GET_ENCODERS = 0x35;
	
	static const int persists = 8;

	char SpeedToByte(double speed) {
		// Filters the maximum and minimum
		if(speed > 2.0)
			speed = 2.0;
		else if(speed < -2.0)
			speed = -2.0;

		return (char)(speed*42+128);
	}

	void _readTicks(){
		send[0] = ADR_GET_ENCODERS;

		// int avail = 0;
		int actualPersistence = 0;
		while(actualPersistence++ < persists){

			// Clear in buffer
			while(serial->available()) serial->read();

			// Send command
			serial->write(send, 1);

			// Wait return OR timeout
			long start = micros();

			while(micros() - start < TIMEOUT && serial->available() < 16);

			if(serial->available() == 16)
				break;

			// Serial.print("\tbytes | ");
			// Serial.print(serial->available());
			// Serial.print("bytes | ");
			// Serial.print(micros() - start);
			// Serial.println("uS");
		}

		if(actualPersistence >= persists){
			Serial.println("$MbedDriver: FATAL ERROR. COMMUNICATION COULD NOT BE COMPLETED!");
			return;
		}else if(actualPersistence > 0){
			Serial.print("Completed transaction with");
			Serial.print(actualPersistence);
			Serial.print("trys");
		}

		for(int i = 0; i < 16; i++){
			response[i] = serial->read();
		}

		for(int i = 0; i < 4; i++){
			ticks[i] = response[i*4 + 0] | response[i*4 + 1]<<8 | response[i*4 + 2]<<16 | response[i*4 + 3]<<24;
		}
	}

public:

	MbedDriver(USARTClass* _serial){
		serial = _serial;
	};

	// Speed functions
	// Set speed on only one motor
	virtual bool setSpeed(int _m, double speed){
		send[0] = ADR_SET_SPEED_START + _m;
		send[1] = SpeedToByte(speed);
		serial->write(send, 2);
		return true;
	};

	// Set speed for all motors, given the array of speeds (all at once)
	/*virtual bool setSpeed(double *_speeds){
		Serial.println("HERE!");
		return setSpeed(_speeds[0], _speeds[1], _speeds[2], _speeds[3]);
	};*/

	virtual bool setSpeed(double _sm1, double _sm2, double _sm3, double _sm4){
		send[0] = ADR_SET_SPEEDS;
		send[1] = SpeedToByte(_sm1);
		send[2] = SpeedToByte(_sm2);
		send[3] = SpeedToByte(_sm3);
		send[4] = SpeedToByte(_sm4);

		serial->write(send, 5);
		return true;
	};

	// Only returns last cached result
	/*virtual long getSpeed(Motor _m){
	};

	// Only returns all last cached results
	virtual void getSpeeds(long* _speeds){

	};*/

	// Encoder functions
	// Read one motor ticks
	virtual long readTicks(Motor _m = M1){
		_readTicks();
		return getTicks(_m);
	};

	// Get last cached result of motor ticks read
	virtual long getTicks(Motor _m){
		return ticks[_m];
	};

	// Read all motor ticks
	/*virtual void readTicks(long* _ticks){
	};

	// Get last cached result of all motor ticks
	virtual void getTicks(long* _ticks){

	};*/

};

#endif