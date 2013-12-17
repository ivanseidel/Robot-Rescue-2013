#include <Arduino.h>

#include "Activity.h"
#include "ActivityManager.h"
#include "ViewGroup.h"

// Default value to enableReturn
bool Activity::enablePause(){
	return false;
}

// onCreate callback checks
void Activity::onCreate(){
	// No callback set?
	if(!_onCreate) return;
	_onCreate();
}
void Activity::onCreate(void (*func)(void)){
	_onCreate = func;
}

// onPause callback checks
void Activity::onPause(){
	// No callback set?
	if(!_onPause) return;
	_onPause();
}
void Activity::onPause(void (*func)(void)){
	_onPause = func;
}

// onLoop callback checks
void Activity::onLoop(){
	// No callback set?
	if(!_onLoop) return;
	_onLoop();
}
void Activity::onLoop(void (*func)(void)){
	_onLoop = func;
}

// onResume callback checks
void Activity::onResume(){
	// No callback set?
	if(!_onResume) return;
	_onResume();
}
void Activity::onResume(void (*func)(void)){
	_onResume = func;
}

// onStop callback checks
void Activity::onStop(){
	// No callback set?
	if(!_onStop) return;
	_onStop();
}
void Activity::onStop(void (*func)(void)){
	_onStop = func;
}

// Responsable for registering activity on ActivityManager
bool Activity::start(bool startNow){
	return ActivityManager::start(this, startNow);
}

// Useful methods for Activities
void Activity::kill(){
	_kill = true;
	_running = false;
	_stop = true;
}

void Activity::stop(){
	_kill = false;
	_running = true;
	_stop = true;
}

void Activity::pause(){
	_kill = false;
	_running = false;
	_stop = false;
}

void Activity::resume(){
	_kill = false;
	_running = true;
	_stop = false;
}