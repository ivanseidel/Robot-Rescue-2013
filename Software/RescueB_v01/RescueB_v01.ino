// ------------- ECONOMIZE MEMORY -------------
// Disable Definitions (Make code lighter)

// #define DISABLE_ZYGOTE	1

// #define DISABLE_VICTIM	1
// #define DISABLE_SENSORS	1
// #define DISABLE_IMU		1
// #define DISABLE_MOTION	1
// #define DISABLE_LCD		1 	// Also disables Touch
								// Disabling LCD will cause WEIRD things
//#define DISABLE_CATABRIGA	1
// #define DISABLE_PROGRAM1	1
// --------------------------------------------


// Motors incluedes
#ifndef DISABLE_MOTION
	#include "MoveController.h"
	#include "MbedDriver.h"
#endif

// General peripheral includes
#include <DueTimer.h>

#include <ArdUI.h>
#include <View.h>

// LCD stuff
#ifndef DISABLE_LCD
	#include <UTouch.h>
	#include <UTFT.h>
#endif

// #include "Util.h"

// IMU includes
#ifndef DISABLE_IMU
	#include "Wire.h"
	#include "I2Cdev.h"
	#include "dmpMPU9150.h"
	#include "sensors/IMUSensor.h"
#endif

#include <Utilities.h>
#include "BFS.h"

// VICTIM includes
#ifndef DISABLE_VICTIM
	#include "VictimChecker.h"
#endif

#ifndef DISABLE_ZYGOTE
	#include <LinkedList.h>
#endif

// Analog Sensors
#include <sensors/AnalogIn.h>
#include <sensors/DigitalOut.h>
#include <sensors/SharpShort.h>
#include <sensors/SharpMedium.h>
#include <sensors/SharpLong.h>
#include <sensors/CompoundDistanceSensor.h>

// Operational System
#include <Thread.h>
#include <ArduinOS.h> 
#include <views/Button.h>
#include <views/CheckBox.h>
#include <views/ProgressBar.h>
#include <views/SeekBar.h>
#include <views/TextView.h>
#include <views/DrawableLine.h>
#include <views/AngleView.h>
#include <views/LinearLayout.h>
#include <views/GraphicView.h>
#include <Gaussian.h>

// Rescue Classes (Map, Cell, Wall...)
#include "Rescue.h"
#include "MapView.h"
#include "AngleEstimator.h"
#include "DistanceEstimator.h"
#include "PoseEstimator.h"
#include "GaussianAverage.h"
#include "MazeWalker.h"

// Basic robot definitions
#include "Config.h"
#include "Robot.h"

// Activity includes
#include "MainActivity.h"
#include "TestIMUActivity.h"
#include "TestVictimActivity.h"
#include "TestSensorsActivity.h"
#include "TestMotorsActivity.h"
#include "Program1Activity.h"
#include "Program2Activity.h"
#ifndef DISABLE_CATABRIGA
	#include "Catabriga.h"
#endif

#ifndef DISABLE_ZYGOTE
	MainActivity zygote = MainActivity();
#endif

Button v = Button();
Button back = Button();
SeekBar pb, pbup;

char c;
void serial(){

	if(PC.available() > 0){
		c = PC.read();

		if(!c)
		return;

        // Serial.print("$ thread: got: ");
        // Serial.println(c);
        Activity* act = ActivityManager::getCurrentActivity();
        if(c == 'p'){
            // Serial.print("$ Thread: pausing Activity: ");
            // Serial.println(act->ActivityID);
            act->pause();
            }else if(c == 'r'){
            // Serial.print("$ Thread: resuming Activity: ");
            // Serial.println(act->ActivityID);
            act->resume();
            }else if(c == 's'){
            // Serial.print("$ Thread: resuming Activity: ");
            // Serial.println(act->ActivityID);
            act->stop();
            }else if(c == 'k'){
            // Serial.print("$ Thread: resuming Activity: ");
            // Serial.println(act->ActivityID);
            act->kill();
            }else if(c == 'g'){
            	PC.print("$ Thread: Current Activity: ");
            	PC.println(act->ActivityID);
            // act->kill();
        }
    }
}

void setup(){  
	initRobot();

	Timer.getAvailable().attachInterrupt(serial).start(50);

	#ifndef DISABLE_ZYGOTE
		
		#ifndef DISABLE_PROGRAM1
			Program1Activity *program1 = new Program1Activity();
			zygote.activities->add(program1);
		#endif

		#ifndef DISABLE_CATABRIGA
			CatabrigaActivity *catabriga = new CatabrigaActivity();
			zygote.activities->add(catabriga);
		#endif

		Program2Activity *program2 = new Program2Activity();
		zygote.activities->add(program2);

		#ifndef DISABLE_IMU
			zygote.activities->add(new TestIMUActivity());
		#endif
		
		TestVictimActivity *testVictim = new TestVictimActivity();
		zygote.activities->add(testVictim);

		#ifndef DISABLE_SENSORS
			TestSensorsActivity *testSensors = new TestSensorsActivity();
			zygote.activities->add(testSensors);
		#endif

		#ifndef DISABLE_MOTION
			TestMotorsActivity *testMotors = new TestMotorsActivity();
			zygote.activities->add(testMotors);
		#endif


		zygote.o.x = 5;
		zygote.o.y = 5;
		zygote.initialize();
	#endif

	/*back.setArea(600,350,200,80);
	back.name = String("Return");
	back.onClick(goBack);
	act1.addView(&back);
	act2.addView(&back);
	act3.addView(&back);

	CheckBox *check = new CheckBox("Enable?");
	check->onChange(checkbox_changed);
	check->o = Point(30,10);
	act3.addView(check);

	CheckBox *check2 = new CheckBox("");
	check2->onChange(checkbox_changed);
	check2->o = Point(30,100);
	check2->w = 100;
	check2->h = 100;
	act3.addView(check2);

	// Progress bar testing
	Thread* updatPbBar = new Thread();
	updatPbBar->onRun(onUpdateProgress);
	updatPbBar->setInterval(0);
	act3.Threads.add(updatPbBar);

	pb = SeekBar();
	pb.o.x = 200;
	pb.o.y = 10;
	pb.w = 500;
	pb.h = 30;
	pb.setMax(100);
	act3.addView(&pb);

	pbup = SeekBar();
	pbup.o.x = 200;
	pbup.o.y = 250;
	pbup.w = 30;
	pbup.h = 200;
	pbup.type = ProgressBar::LINEAR_UP;
	// pbup.setMax(1.0);
	act3.addView(&pbup);*/



}

void loop(){
	PC.println("====== START OF PROGRAM ======");
    // Start Activity
    #ifndef DISABLE_ZYGOTE
    	zygote.start();
    #else
    	// Enter code here to run "custom" zygote
	#endif
    // End of Program
    PC.println("====== END OF PROGRAM ======");
}
