#include <Arduino.h>

#include "Activity.h"
#include "ActivityManager.h"
#include "ArduinOS.h"

// Initialize atributes
int ActivityManager::_index = 0;
Activity* ActivityManager::_activities[MAX_ACTIVITY_STACK] = {NULL};
Activity* ActivityManager::nextActivity= NULL;

// Removes the activity object from the stack
void ActivityManager::kill(){
	debug("$ ActivityManager: Kill  Activity: \n\n",getCurrentActivity()->ActivityID);
	pop();
}

// Adds one activity to the stack
bool ActivityManager::push(Activity* activity){
	int thisID = activity->ActivityID;
	
	// Checks if object is already on the stack
	for(int i=0; i < _index; i++){
		if(_activities[i]){
			debug(_activities[i]->ActivityID);
			if(_activities[i]->ActivityID == thisID){
				return false;
			}
		}
	}

	// Check if stack is full
	if(_index >= MAX_ACTIVITY_STACK)
		return false;

	// Insert on stack
	_activities[_index] = activity;
	_index++;
	return true;
}

// Pops the last element from the stack
bool ActivityManager::pop(){
	// Check if there is something to pop
	if(_index <= 0)
		return false;

	// Sets the maximum stack as index--
	_index--;
	return true;
}

Activity* ActivityManager::getCurrentActivity(){
	// Check if there is a current activity first
	if(_index <= 0)
		return NULL;

	return _activities[_index-1];
}

// Function called on request to start new activity
bool ActivityManager::start(Activity* activity, bool startNow){
	// Reset activity buffer
	nextActivity = NULL;

	// Should run now? If not, then wait completing any function to run
	if(!startNow){
		debug("$ ActivityManager: Sched. activity:\t", activity->ActivityID);
		nextActivity = activity;
		return false;
	}

	// Register Activity on STACK
	if(!push(activity)){
		debug("$ ActivityManager: Couldn't start Activity!");
		return false;
	}
	
	int activityId = activity->ActivityID;
	
	debug("$ ActivityManager: Start Activity: \t",activityId);

	// Callback on ArduinOS
	ArduinOS::onActivityStart(getCurrentActivity());

	// call onCreate on Activity
	activity->onCreate();

	while(!activity->_stop){

		// Check Kill flag
		if(activity->_kill){kill();return false;}

		// Refresh screen before activity is resumed
		// (stop Thread to prevent LCD write problems
		// ArduinOS::_threadTimer->stop();
		noInterrupts();
		ArduinOS::render(true);
		interrupts();
		// ArduinOS::_threadTimer->start();

		// Check if already runned once (means that is resuming)
		debug("$ ActivityManager: Resume Activity: ",activityId);
		activity->onResume();

		// Loop while flags allow it
		while(activity->_running && !activity->_stop){
		
			// Check Kill flag
			if(activity->_kill){kill();return false;}

			// Check activity buffer
			if(nextActivity)
				break;

			// Refresh activity only if requested
			if(ArduinOS::requestRender){
				noInterrupts();
				ArduinOS::render(false);
				interrupts();
			}

			activity->onLoop();
		}

		// Check Kill flag
		if(activity->_kill){kill();return false;}

		// Check stop flag
		// if(activity->_stop){break;};

		// Pause Activity
		debug("$ ActivityManager: Pause Activity: \t",activityId);
		activity->onPause();
		
		// Check activity buffer
		if(nextActivity)
			start(nextActivity);

		// Wait for resume
		digitalWrite(8, HIGH);
		while(!activity->_running && !activity->_stop && !activity->_kill && !nextActivity){
			activity->onPauseLoop();
		}
		digitalWrite(8, LOW);

	}

	// Check Kill flag
	if(activity->_kill){kill();return false;}

	// Check activity buffer (SHOULD HAVE THIS CHECK?)
	if(nextActivity)
		start(nextActivity);

	debug("$ ActivityManager: Stop Activity: \t",activityId);
	activity->onStop();

	// Remove activity from STACK
	pop();

	// Callback on ArduinOS
	ArduinOS::onActivityResume(getCurrentActivity());

	// Return Success
	return true;
}
