#ifndef BallRadar_h
#define BallRadar_h

#include <Arduino.h>

#include <Thread.h>
#include <ArduinoSensors.h>
#include <interfaces/AngleInterface.h>

#define CHUNKED_ANGLE_SCALE_FACTOR 3

class BallRadar: public Thread, public AngleInterface{
protected:
	float cachedAngle;
	int cachedIntensity;

	Stream *COM;

public:

	bool newReading;

	BallRadar(Stream *_COM){
		cachedAngle = 0;
		cachedIntensity = 0;

		COM = _COM;

		Thread::Thread();
		setInterval(25);
	}

	/*
		Reads angle, IF available
	*/
	void read(){
		while(COM->available() > 2)
			COM->read();

		// Check Serial twice for both direction and intensity
		for(int i = 0; i < 2; i++){
			// Read only if available
			if (COM->available()) {
				uint8_t data = COM->read();

				if (data & 0x01) {
					// It's a INTENSITY
					cachedIntensity = data;
				}else {
					// It's ANGLE
					uint8_t direction = data >> 1;		
					cachedAngle = direction * CHUNKED_ANGLE_SCALE_FACTOR;
					
					//fix angle from here
					cachedAngle += 180.0;
					while (cachedAngle >= 360.0)
						cachedAngle -= 360.0;
				}
				newReading = true;
			}else{
				return;
			}
		}
	}

	virtual float readAngle(){
		read();
		return getAngle();
	}

	virtual float getAngle(){
		return cachedAngle;
	}

	int readIntensity(){
		read();
		return getIntensity();
	}

	int getIntensity(){
		return cachedIntensity;
	}

	/*
		For Thread Usage
	*/
	virtual void run(){
		readAngle();
		runned();
	}
};

#endif