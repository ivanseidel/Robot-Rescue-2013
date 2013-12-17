#ifndef VictimChecker_h
#define VictimChecker_h

#include "Arduino.h"
#include "Thread.h"

#define USART_SPEED             9600

#define CMD_READ_ALL            'r'
#define CMD_SETUP_ALL_SENSORS   's'
#define CMD_LIST_CONNECTIONS    'l'
#define CMD_SET_DEFAULT_VALUES  'd'
#define CMD_QUIT                '0'
#define CMD_CHECK_ALIVE         'a'
#define CMD_PERFORM_READ        'p'

// Raw Commands
#define CMD_SET_MIN_DEVIATION   'm' // Should be followed by a single byte
                                    // containing the DEVIATION*50
                                    // unsigned uint8_t type

#define CMD_START_BYTE          0xFF
#define CMD_SIZE                14  // Size of packet sent from ATMEGA328

#define DEFAULT_MIN_DEVIATION   1.0
#define DEVIATION_PRESCALE      20
#define VECTOR_PRESCALE         2
#define VECTOR_ZERO             64
#define TEMPERATURE_PRESCALE    2

class VictimChecker: public Thread
{
protected:

	char _buff[CMD_SIZE];

public:

	USARTClass *USART;

	float 	deviation,
			vX,
			vY,
			temp[10];

	bool hasNewData;

	VictimChecker(USARTClass* _serial){
		Thread::Thread();

		// Initialize data
		deviation = 0.0;
		vX = 0.0;
		vY = 0.0;
		for(int i = 0; i < 10; i++)
			temp[i] = 0;

		hasNewData = false;

		USART = _serial;
		USART->begin(USART_SPEED);
	}

	void run(){
		static bool newCommand = false;

		// Check for start byte
		while(USART->available() >= CMD_SIZE && !newCommand){
			if(USART->read() == CMD_START_BYTE){
				newCommand = true;
			}
		}


		if(!newCommand || USART->available() < CMD_SIZE-1)
			return;

		USART->readBytes(_buff, CMD_SIZE-1);

		// Check if Bytes are Valid
		for(int i = 0; i < CMD_SIZE - 1; i++){
			if(_buff[i] == CMD_START_BYTE){
				newCommand = false;
				return;
			}
		}

		// Read Deviation
		deviation = _buff[0]/(DEVIATION_PRESCALE*1.0);
		vX = ((uint8_t)_buff[1]-VECTOR_ZERO)/(VECTOR_PRESCALE*1.0);
		vY = ((uint8_t)_buff[2]-VECTOR_ZERO)/(VECTOR_PRESCALE*1.0);

		for(int i = 0; i < 10; i++)
			temp[i] = _buff[3+i]/(TEMPERATURE_PRESCALE*1.0);

		/*Serial.print(deviation);
		Serial.print("\t");
		Serial.print(vX);
		Serial.print("\t");
		Serial.print(vY);
		Serial.print("\t");
		Serial.println(temp[0]);*/

		// USART->flush();

		// Serial.print("\t\t");
		// Serial.println((int)(deviation*100));

		runned();
		newCommand = false;
		hasNewData = true;
	};

	bool isConnected(){
		// Serial.println("Writing...");
		USART->print(CMD_CHECK_ALIVE);
		// Serial.println("Waiting...");
		delayMicroseconds(100000);
		// Serial.println("Checking...");
		if(USART->available() > 0){
			// Serial.println("Verifying...");
			return USART->read() == 0xFF;
		}
		return false;
	}

};

#endif