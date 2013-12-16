// Basic robot definitions
#include "Config.h"

void setup(){
	Serial.begin(115200);
	while(!Serial);
	Serial.println("Initialized");

	pinMode(pLED_VICTIM, OUTPUT);

	/*
		IMU Initialization
	*/
	IMU_WIRE.begin();
}

void loop(){

}