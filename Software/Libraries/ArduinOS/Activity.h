/*
  Activity.h - Class containing basic callbacks for ArduinOS.
  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifndef Activity_h
#define Activity_h

#include "ViewGroup.h"
#include "ThreadController.h"

class Activity: public ViewGroup
{
public:
	Activity(): ViewGroup(){
		_running = true;
		_stop = false;
		_kill = false;

		_onCreate = NULL;
		_onPause = NULL;
		_onLoop = NULL;
		_onResume = NULL;
		_onStop = NULL;

		// Initialize ThreadsController
		Threads = ThreadController();

		// public flags
		_requestTouch = true;
		_enableReturn = true;

		ActivityID = (int)this;
		ActivityName = "Activity ";
		ActivityName = ActivityName + ActivityID;
		ViewGroup::ViewGroup();
	};

	int ActivityID;
	String ActivityName;

	// Public Flags

	// This activity can return if Return Event arrives?
	int  _enableReturn;	
	virtual bool enablePause();

	// Activity Life cycle Flags (DO NOT CHANGE MANUALY)
	bool _running;
	bool _stop;
	bool _kill;

	// ThreadsController, that keeps track of the Activity Threads
	ThreadController Threads;

	// Method called to run Activity
	bool start(bool startNow = true);

	void kill();
	void stop();
	void pause();
	void resume();

	// CallBack Methods setters
	void onCreate(void (*)(void));
	void onPause(void (*)(void));
	void onLoop(void (*)(void));
	void onResume(void (*)(void));
	void onStop(void (*)(void));

	virtual void onCreate();
	virtual void onPause();
	virtual void onLoop();
	virtual void onResume();
	virtual void onStop();
	virtual void onPauseLoop(){};

protected:
	
	void (*_onCreate)(void);
	void (*_onPause)(void);
	void (*_onLoop)(void);
	void (*_onResume)(void);
	void (*_onStop)(void);

};

#endif