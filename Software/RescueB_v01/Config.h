#include <Arduino.h>
#include <DueTimer.h>

/*
	ROBOT GENERAL CONFIGURATION
*/

// Timers usage definition, to avoid conflicts
#define TIMER_THREAD1	Timer3		// Timer that will trigger threads on ArduinOS
#define TIMER_TCH		Timer4		// Used for LCD hit tests while moving
#define TIMER_LED		Timer5		// Blinking led, to show that "I'm alive!"


// User Interface (LED's, Buzzer...)
#define pLED_STATUS		9
#define pLED_VICTIM		8
#define pBTN_PAUSE		DAC0
#define pBTN_SW1		23
#define pBTN_SW2		22

// Buzzer configuration
#define pBUZER			12
#define TIME_BEEP		20
#define TIME_BEEP_H		1 			
#define TIME_BEEP_L		200			

// PC (Just for debugging)
#define PC_USART		Serial
#define PC				Serial 		// To simplify

// Bluetooth
#define BLUETOOTH_USART	Serial3
#define BLUETOOTH		Serial3

// Motor Driver Controller
#define DRIVER_USART	Serial1
#define pDRIVER_ENABLE	10

// CO-PROCESSOR (ATMEGA328) with Temperature sensors
#define VICTIM_USART	Serial2
#define pTMP_INT		7
#define pTMP_FRONT		6
#define pTMP_BACK		5
#define pTMP_LEFT		4
#define pTMP_RIGHT		3
#define pTMP_CLR		2

// LCD
// DB0-DB7:  33-40
// DB8-DB15: 51-44
#define pLCD_RS			41
#define pLCD_WR			42
#define pLCD_CS			43
#define pLCD_RESET		53
#define pLCD_LED		DAC1

#define LCDWidth		799
#define	LCDHeight		479

// TOUCH
#define pTCH_CLK		27
#define pTCH_CS			28
#define pTCH_DIN		29
#define pTCH_OUT		30
#define pTCH_IRQ		31

// IMU
#define IMU_WIRE		Wire1
#define pIMU_SDA		SDA1
#define pIMU_SCL		SCL1
#define pIMU_IRQ		32

// SD Card
// Uses SPI pins, no need for definition
#define pSD_CS			52

// Battery voltage
#define pVIN			A11
#define VOLTAGE_USB		110			// In raw units of analogRead()
#define VOLTAGE_LOW		6.4
#define VOLTAGE_HIGH	8.2

// Light Sensors
#define pLIGHT_IN1		A0
#define pLIGHT_IN2		A5
#define pLIGHT_LED		11

int pLIGHT_IN[] = {
	pLIGHT_IN1,
	pLIGHT_IN2
};

// IR Distance Sensor
#define IR_Short		0
#define IR_Medium		1
#define IR_Long			2

#define pIR_IN1			A1
#define pIR_IN2			A2
#define pIR_IN3			A3
#define pIR_IN4			A4
#define pIR_IN5			A10
#define pIR_IN6			A9
#define pIR_IN7			A7
#define pIR_IN8			A6

int pIR_IN[] = {
	pIR_IN1,
	pIR_IN2,
	pIR_IN3,
	pIR_IN4,
	pIR_IN5,
	pIR_IN6,
	pIR_IN7,
	pIR_IN8,
};

int IR_TYPE[] = {
	IR_Medium,
	IR_Short,
	IR_Long,
	IR_Medium,
	IR_Medium,
	IR_Long,
	IR_Short,
	IR_Medium
};

/*
	LCD Schemes
*/
#define cRectFill		200,200,200
#define cTextNeutral	30,30,30
#define cGUITopBarFill	255,255,255

#define fTextBig		OCR_A_Extended_M
#define fTextSmall		SmallFont
#define fNumBig			SevenSegNumFont

/*
	ArduinOS Configuration
*/
#define osRETURN_MIN_TIME	100		// Minimum time between clicks on return Btn
									// to debounce sensor. (in Milliseconds)
									
#define UPDATE_GUI_INTERVAL	2000	// Time to update GUI (constantly)
									// Also compute the remaining battery

#define fGUI_USB			9