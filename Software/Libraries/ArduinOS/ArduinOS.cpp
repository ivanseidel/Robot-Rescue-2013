#include <Arduino.h>

#include "Activity.h"
#include "ActivityManager.h"
#include "ThreadController.h"
#include "View.h"
#include "ArduinOS.h"

// ThreadControllers initialization
ThreadController* ArduinOS::_mainThreadController = new ThreadController();
ThreadController* ArduinOS::SystemThreads = new ThreadController();
DueTimer* ArduinOS::_threadTimer = NULL;
int ArduinOS::_threadInterval = 10000; //in Us

int ArduinOS::cpuThreadUsage = 0; //in %*100

// Screen Views configuration
bool ArduinOS::requestRender = false;
Point ArduinOS::_rel = Point(0, 32);
View* ArduinOS::GUI = NULL; // General User Interface (System interface)
View* ArduinOS::STAGE = NULL; // Stage (Background and other stuff)


void ArduinOS::prepareForActivity(Activity* act){
	// debug("$ ArduinOS: Preparing activity: \t", act->ActivityID);
	/*
		Touch handeling preparation
	*/
	// Disable touch for "safety reasons"
	ArdUI::disableTouch();

	/*
		Configure Threads
	*/
	if(_threadTimer){
		// Stop to avoid conflicts while changing
		_threadTimer->stop();

		// Configures callback
		_threadTimer->attachInterrupt(runThread);

		// Clean mainThreadController
		_mainThreadController->clear();

		// Adds SystemThreads to mainThreadController
		_mainThreadController->add(SystemThreads);

		// If act is not NULL, then add's it's Threads
		if(act){
			_mainThreadController->add(&act->Threads);
		}

		_threadTimer->start(_threadInterval);
	}

	// Change current view on ArdUI
	ArdUI::rootView = act;
	
	// Enable if necessary
	if(act->_requestTouch){
		ArdUI::enableTouch();
	}
}

void ArduinOS::runThread(){
	static long startTime, count = 0, integral = 0;
	
	// Saves start time, to calculate CPU usage
	startTime = micros();

	_mainThreadController->run();

	// Integrates over 50 iterations before calculating
	// (Improves performance and accurracy)
	integral += micros() - startTime;
	count++;
	if(count >= 50){
		ArduinOS::cpuThreadUsage = (integral*100.0/count)/_threadInterval;
		count = 0;
		integral = 0;
	}
}


void ArduinOS::onActivityStart(Activity* act){
	// debug("$ ArduinOS: Start Activity: \t\t", act->ActivityID);
	// Override flags of activity
	act->resume();
	prepareForActivity(act);
}

void ArduinOS::onActivityResume(Activity* act){
	// debug("$ ArduinOS: Resume activity: \t\t", act->ActivityID);
	prepareForActivity(act);
}

// Handles screen rendering on current view
void ArduinOS::render(bool forceRender){
	if(forceRender){
		if(GUI)
			GUI->render(forceRender);
		if(STAGE)
			STAGE->render(forceRender);
	}

	Activity* act = ActivityManager::getCurrentActivity();
	// Is there a activity running, or is it NULL?
	if(act){
		act->_rel = ArduinOS::_rel;
		act->render(forceRender);
	}

	requestRender = false;
}

// Just redirects to actionHandler(ActionEvent)
void ArduinOS::actionHandler(ActionType type){
	static ActionEvent evt;
	evt.type = type;
	actionHandler(evt);
}

void ArduinOS::actionHandler(ActionEvent evt){
	static Activity* act;
	act = ActivityManager::getCurrentActivity();
	
	// Is there an activity running?
	if(!act){
		debug("$ ArduinOS: No activity running to receive event.");
		return;
	}

	// Is the action, a return button?
	if(evt.type == ACTION_RETURN){
		// Can the Activity return with this event?
		if(act->_enableReturn){
			// Stop the activity
			act->stop();
			return;
		}else{
			debug("$ ArduinOS: _enableReturn disabled by Activity.");
		}
	}

	// Otherwise, just send the event to the current activity
	act->onTouch(evt);
}