#include <Arduino.h>

// Motors incluedes
// #include "MoveController.h"
// #include "mbed_driver.h"

// General peripheral includes
// #include <DueTimer.h>
// #include <UTouch.h>
// #include <UTFT.h>

// Operational System
// #include <ArduinOS.h>

// Include Icons and fonts
// #include <fGUI.c>

/*
	GLOBAL Sensors objects
*/
// IMU setup
#ifndef DISABLE_IMU
	IMUSensor *IMU;
#endif

// SENSORS setup
#ifndef DISABLE_SENSORS
	AnalogIn 	Light1 = AnalogIn(pLIGHT_IN1),
				Light2 = AnalogIn(pLIGHT_IN2),
				*Light[2];

	DigitalOut	LightLed = DigitalOut(pLIGHT_LED),
				VictimLed = DigitalOut(pLED_VICTIM);

	DistanceInterface 	*Dist[8];

	CompoundDistanceSensor 	*DistFront,
							*DistBack;

	DistanceInterface 	*DistLeftFront,
						*DistLeftBack,
						*DistRightFront,
						*DistRightBack;

	AngleEstimator 		*angleLeftEstimator,
						*angleRightEstimator;

	PoseEstimator 		*poseEstimator;

	DistanceEstimator 	*frontEstimator,
						*backEstimator;
#endif

/*
    MoveController definitions
*/
#ifndef DISABLE_MOTION
	MoveDriver *MD;
	MoveController *MC;
#endif

/*
    MoveController definitions
*/
#ifndef DISABLE_VICTIM
	VictimChecker *VICTIM;
#endif

/*
	LED Status timer interrupt
*/
/*void irqLedStatus(){
	static bool active = false;

	 PIO_SetOutput( g_APinDescription[pLED_STATUS].pPort, g_APinDescription[pLED_STATUS].ulPin, active, 0, PIO_PULLUP ) ;
}*/

/*
	RETURN Button implementation
*/
void irqReturnBtn(){
	Serial.println("$Return: callback requested");
	static long lastCall = 0;
	// Check if there was a minimum time between clicks (debounce)
	if(millis()- lastCall < osRETURN_MIN_TIME)
		return;

	lastCall = millis();
	ArduinOS::actionHandler(ACTION_RETURN);
}

/*
	Beep function and Thread
*/
// Used to warn battery
Thread thrBeeper;
void onBeeper(){
	static bool beeping = false;
	beeping = !beeping;
	digitalWrite(pBUZER, beeping);
	if(beeping)
		thrBeeper.setInterval(TIME_BEEP_H);
	else
		thrBeeper.setInterval(TIME_BEEP_L);
}

// Notify touchs
void beep(){
	digitalWrite(pBUZER, HIGH);
	delayMicroseconds(10000);
	digitalWrite(pBUZER, LOW);
}

/*
	Function reponsable for merging both LONG and SHORT
	distance sensors.
*/
long onCalculateDistance(long dist1, long dist2){
	if(dist1 >= 30)
		return dist2 + 7;
	return dist1 + 7;
}


#ifndef DISABLE_LCD
	/*
		LCD and Touch definitions
	*/
	UTFT LCD(ITDB50,pLCD_RS,pLCD_WR,pLCD_CS,pLCD_RESET);
	UTouch TCH = UTouch(pTCH_CLK,pTCH_CS,pTCH_DIN,pTCH_OUT,pTCH_IRQ);

	// extern uint8_t SmallFont[];			// Font
	// extern uint8_t BigFont[];		// Font
	extern uint8_t OCR_A_Extended_M[];	// Font
	// extern uint8_t SevenSegNumFont[];	// Font
	extern uint8_t fGUI[];				// Font


	/*
		System GUI (General User Interface)
	*/
	Thread thrUpdateGUI;
	View GUI = View();
	void onGUIupdate(){
		static float voltage, analog;
		static int batteryStatus, failsCount = 0;
		LCD.setBackColor(cGUITopBarFill);
		
		// Print BT status
		LCD.setFont(fGUI);
		LCD.setColor(180,180,180);
		LCD.printNumI(6, 1,1);
		

		// Calculates battery voltage and current status
		analog = analogRead(pVIN);
		voltage = max((analog-175)/86.25 + 6, 0);

		if(analog <= VOLTAGE_USB){
			batteryStatus = fGUI_USB;	// On USB
		}else{
			batteryStatus = 
				max(0,min(5,map(voltage*100, VOLTAGE_LOW*100, VOLTAGE_HIGH*100,
					0,5)));
		}

		if(batteryStatus == 0){
			if(++failsCount >= 3)
				thrBeeper.enabled = true;	// Enable Beeper thread
			LCD.setColor(240,20,30);	// Very low
		}else if(batteryStatus == fGUI_USB){
			LCD.setColor(70,70,70);		// USB Plugged
		}else if(batteryStatus >=  4){
			LCD.setColor(20,200,20);	// Full charge
		}else{
			LCD.setColor(245,210,95);	// Normal
		}

		if(batteryStatus != 0){
			thrBeeper.enabled = false;	// Disable Beeper thread
			digitalWrite(pBUZER, LOW); 	// Stop beeping
			failsCount = 0;
		}

		// Render Battery status
		LCD.printNumI(batteryStatus, 731,1);
		LCD.setFont(fTextBig);
		LCD.setColor(150,150,150);
		if(batteryStatus != fGUI_USB)
			LCD.printNumF(voltage,1, 665,4,'.',4);
		else
			LCD.print("USB",680,4);

		LCD.print(String(ArduinOS::cpuThreadUsage) + "%  ", 78,4);
	}
	
	void onGUIrender(){
		static Activity* act;
		act = ActivityManager::getCurrentActivity();

		LCD.setColor(cGUITopBarFill);
		LCD.fillRect(0,0,LCDWidth, 31);
		LCD.setBackColor(cGUITopBarFill);

		// Render Activity Name
		if(act){
			LCD.setColor(VGA_BLACK);
			LCD.setFont(fTextBig);
			LCD.print(act->ActivityName, CENTER, 4);
		}

		onGUIupdate();
		// PC.println("$: GUI: Rendered.");
	}

	View STAGE = View();
	void onSTAGErender(){
		LCD.setColor(VGA_BLACK);
		LCD.fillRect(ArduinOS::_rel.x,ArduinOS::_rel.y,LCDWidth, LCDHeight);
	}

	void onInvalidateView(){
		ArduinOS::requestRender = true;
	}

#endif	// DISABLE_LCD

ProgressBar preLoader = ProgressBar();

// Perform a FULL initialization and configuration
// of all connected hardware.
void initRobot(){
	
	// Initialize All Serial's
	PC.begin(115200);
	BLUETOOTH.begin(9600);
	DRIVER_USART.begin(57600);
	while(!PC || !BLUETOOTH || !DRIVER_USART);

	PC.println("$: Serial initialized");

	// Initialize Pin Ports as I/O
	const int output[] = {
		pLED_STATUS,
		pLED_VICTIM,
		pBUZER,
		pDRIVER_ENABLE,
		pTMP_CLR,
		pLCD_LED,
		pSD_CS,
		pLIGHT_LED,
		14
	};

	const int input[] = {
		pBTN_PAUSE,
		pBTN_SW1,
		pBTN_SW2,
		pTMP_INT,
		pTMP_FRONT,
		pTMP_BACK,
		pTMP_LEFT,
		pTMP_RIGHT,
		pTMP_CLR,
		pIMU_IRQ,
		pVIN,
		pLIGHT_IN1,
		pLIGHT_IN2,
		pIR_IN1,
		pIR_IN2,
		pIR_IN3,
		pIR_IN4,
		pIR_IN5,
		pIR_IN6,
		pIR_IN7,
		pIR_IN8,
	};

	
	for(int i = 0; i < sizeof(output)/sizeof(int); i++){
		pinMode(output[i], OUTPUT);
		digitalWrite(output[i], LOW);
	}

	PC.println("$: Output pins initialized");

	for(int i = 0; i < sizeof(input)/sizeof(int); i++){
		pinMode(input[i], INPUT);
	}

	PC.println("$: Input pins initialized");


	// Initialize Status LED Timer
	// TIMER_LED.attachInterrupt(irqLedStatus).setFrequency(10).start();
	// PC.println("$: Status LED Initialized");

	/*
		ArduinOS setup
	*/
	#ifndef DISABLE_LCD
		// Initialize TFT LCD
		LCD.InitLCD();
	  	LCD.setFont(OCR_A_Extended_M);
	  	LCD.clrScr();
		PC.println("$: LCD Initialized");

	  	// Initialize TOUCH Screen
	  	TCH.InitTouch(LANDSCAPE);
	    TCH.setPrecision(PREC_MEDIUM);
		PC.println("$: TOUCH Initialized");
	#endif
	
	// Preloader
	preLoader.setMax(12);
	preLoader.o = Point(150,230);
	preLoader.w = 500;
	preLoader.setValue(1, true);

	// Thread initialization
	ArduinOS::_threadTimer = &TIMER_THREAD1;
	ArduinOS::SystemThreads = new ThreadController();
	ArduinOS::SystemThreads->ThreadName = "SystemThread";
	ArduinOS::_threadInterval = 10000;
	preLoader.setValue(2, true);

	#ifndef DISABLE_LCD
	    // Configure ArduinOS to read Touch interrupts from LCD
	    ArdUI::touchInterrupt = pTCH_IRQ;
	    ArdUI::touchTimer =  &TIMER_TCH;
	    ArdUI::touchObject = &TCH;
	    ArdUI::touchMode = ArdUI::INTERRUPT_TIMER;
	    ArdUI::touchTimerPeriod  = 50000;
	    ArdUI::LCD = &LCD;

	    ArdUI::onInvalidateView = onInvalidateView;

	    // Configure GUI
	    GUI.onRender(onGUIrender);
	    ArduinOS::GUI = &GUI;
	    STAGE.onRender(onSTAGErender);
	    ArduinOS::STAGE = &STAGE;

	    // Create and register GUI updater on SystemThreads
	    thrUpdateGUI = Thread(onGUIupdate, UPDATE_GUI_INTERVAL);
	    thrUpdateGUI.ThreadName = "Update GUI";
	    ArduinOS::SystemThreads->add(&thrUpdateGUI);
		PC.println("$: GUI Updater Thread registered: "+thrUpdateGUI.ThreadName);
	#endif

    // Configure Return button of ArduinOS
    attachInterrupt(pBTN_SW2, irqReturnBtn, RISING);
	preLoader.setValue(3, true);


	// snipet beep() register on ArduinOS
	ArduinOS::beep = beep;
	preLoader.setValue(4, true);
	
	// Beeping initialization
	thrBeeper = Thread(onBeeper, TIME_BEEP_H);	// Instantiate beeper object
    thrBeeper.ThreadName = "Beeper";			// Default ThreadName
	thrBeeper.enabled = false; 				// Defalut is disabled
    ArduinOS::SystemThreads->add(&thrBeeper);	// Register thread on SystemThreads
    PC.println("$: Beeper Thread registered: "+thrBeeper.ThreadName);
	preLoader.setValue(5, true);

    // IMU Initialization
    #ifndef DISABLE_IMU
    	IMU = new IMUSensor(Wire1);
		Serial.println(IMU->initialize()? "$: IMU INITIALIZATION SUCCESFULL!": "$: !!!!!!!!!!FAILED TO INITIALIZE IMU!!!!!!!!!!!");
		IMU->readYPR = true;
		IMU->setInterval(40);
		// ArduinOS::SystemThreads->add(IMU);	// We must register LOCALY the Thread
		preLoader.setValue(6, true);
	#endif

	// IMU Initialization
    #ifndef DISABLE_VICTIM
		VICTIM = new VictimChecker(&VICTIM_USART);
		VICTIM->setInterval(50);
		Serial.println(VICTIM->isConnected()? "$: VICTIM INITIALIZATION SUCCESFULL!": "$: !!!!!!!!!!FAILED TO INITIALIZE VICTIM!!!!!!!!!!!");
		// ArduinOS::SystemThreads->add(VICTIM);
		preLoader.setValue(7, true);
	#endif

	// SENSORS setup
	analogReadResolution(10); // Set 10 bits resolution

	#ifndef DISABLE_SENSORS
		// Initialize LightSensors
		// Light1 = new AnalogIn(pLIGHT_IN1);
		// Light2 = new AnalogIna(pLIGHT_IN2);
		
		Light[0] = &Light1;
		Light[1] = &Light2;

		Light[0]->setInterval(30); // 100ms to check
		Light[1]->setInterval(30); // 100ms to check

		LightLed = 0;

		// Initialize Distance Sensors
		for(int i = 0; i < 8; i++){
			// Dist[i] = new AnalogIn(pIR_IN[i]);

			if(IR_TYPE[i] == IR_Short)
				Dist[i] = new SharpShort(pIR_IN[i]);
			else if(IR_TYPE[i] == IR_Medium)
				Dist[i] = new SharpMedium(pIR_IN[i]);
				// Dist[i] = new AnalogIn(pIR_IN[i]);
			else if(IR_TYPE[i] == IR_Long)
				Dist[i] = new SharpLong(pIR_IN[i]);
		}

		DistFront = new CompoundDistanceSensor(Dist[6], Dist[5]);
		DistFront->onCalculate = onCalculateDistance;
		DistBack = new CompoundDistanceSensor(Dist[1], Dist[2]);
		DistBack->onCalculate = onCalculateDistance;

		DistLeftBack = Dist[3];
		DistLeftFront = Dist[4];
		DistRightBack = Dist[0];
		DistRightFront = Dist[7];

		// Initialize estimators
		angleLeftEstimator  = new AngleEstimator(DistLeftFront, DistLeftBack, 14, false);
		angleRightEstimator = new AngleEstimator(DistRightFront, DistRightBack, 14, true);

		frontEstimator = new DistanceEstimator(DistFront);
		backEstimator = new DistanceEstimator(DistBack);

		poseEstimator = new PoseEstimator(	angleLeftEstimator, angleRightEstimator,
											frontEstimator, backEstimator,
											MC);

		preLoader.setValue(8, true);

	#endif

	/*
	    MoveController Initialization
	*/
	#ifndef DISABLE_MOTION
		MD = new MbedDriver(&DRIVER_USART);
		MC = new MoveController(MD);
		MC->setSpeed(0);
	#endif


    // Finished
	PC.println("$: ArduiOS Initialized");
	preLoader.setValue(9, true);

}

class MazeWalker
{
protected:
	static double getCompass(){
		return toDegs(IMU->ypr[0]);
	}
	
	static double normalize(double angle){
		while (angle > 180.0)angle -= 360.0;
	    while (angle < -180.0)angle += 360.0;
	    return angle;
	}
	
	static double getRelativeAngle(double angle){
		double compassAngle = getCompass(), delta;
		delta = angle - compassAngle;
		return normalize(delta);
	}

	static const double KcmToDegree = 20.333;
	// static const double KdegreeToCm = 17.7;
	static const double KdegreeToCm = 15.2;
	static long rotationToCm(double rotations){
		return (rotations * KdegreeToCm);
	}

public:
	static void move(Movement m){
		// if(m == Front)
		// 	Front();
		// else if(m == Left)
		// 	Left();
		// else if(m == Right)
		// 	Right();
	}



	static void Front(){
		bool interrupt = false;
		long start = millis();

		double speed = 1.0;

		double targetAngle = normalize(getCompass());

		while(millis() - start < 8000 && !interrupt){

			long distFront = DistFront->readDistance();
			long distBack = DistBack->readDistance();
			long variance = 0, variN = 0;

			// Estimate actual postition
			long pos = rotationToCm((millis() - start)/1000.0 * speed);
			
			// Serial.print(distFront);
			// Serial.print("\t");
			// Serial.print(distBack);
			// Serial.print("\t");

			if(pos > 20){
				if(distFront < 15){
					// variance += distFront % 30;
					// variN ++;
					// interrupt = true;
					break;
				}
				if(abs(distFront - 45) <= 1)
					break;

				if(distBack < 15){
					// variance += distBack % 30;
					// variN ++;
					// interrupt = 10;
					break;
				}
				if(abs(distBack - 45) <= 1)
					break;
				// variance /= variN;
				// Serial.print(15 - variance);
			}

			Gaussian angle = poseEstimator->estimateAngle();// + Gaussian(0, 40.0);
			Gaussian lateralShift = poseEstimator->estimateLateralDistance();
			lateralShift.mean = -lateralShift.mean/100.0;
			lateralShift.variance = lateralShift.variance * lateralShift.variance;

			Gaussian final = /*Gaussian(getRelativeAngle(targetAngle), 10) +*/lateralShift + Gaussian(0,10.0);
			// Serial.print("\t");
			// Serial.print(angle.mean);

			// Serial.print("\n");

			// if(abs(15 - distBack % 30) < 3)
			// 	interrupt = true;
			// else if(abs(15 - distFront % 30) < 3)
			// 	interrupt = true;
			// else if(pos >= 30)
			// 	interrupt = true;


			// if(variN > 0 && abs(15 - variance) < 3){
			// 	interrupt = true;
			// }else 
			if(pos >= 28 && distFront >= 17){
				// interrupt = true;
				break;
			}

			MC->setSpeed(0,speed, -final.mean);
			delay(50);

		}

		MC->stop();
		beep();
	}

	static void Left(){
		rotate(90);
		beep();
	}

	static void Right(){
		rotate(-90);
		beep();
	}

	static void Back(){
		rotate(180);
		beep();
	}

	static void magneticAlign(){
		long start = millis();
		while(millis() - start < 1500){
			Gaussian angle = poseEstimator->estimateAngle();
			angle = angle + Gaussian(0, 80.0);

			// if(abs(angle.mean) < 2.0)
				// break;

			MC->setSpeed(0,0,angle.mean);

			delay(50);
		}
		MC->setSpeed(0,0,0);
	}

	static void rotate(double angle){

		double targetAngle = normalize(getCompass() - angle), delta;
		
		long start = millis();
		while(millis() - start < 5000){
			delta = getRelativeAngle(targetAngle);

			MC->setSpeed(0,0,-delta/30.0);

			if(millis() - start > 500 && abs(delta < 0.0? -delta:delta) < 1.0)
				break;

			Serial.println(delta);
			delay(50);

		}

		MC->stop();

	}
};