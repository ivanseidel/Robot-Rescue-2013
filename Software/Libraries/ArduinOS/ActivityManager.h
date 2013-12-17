/*
  ActivityManager.h - Responsable por keeping activities running
  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifndef ActivityManager_h
#define ActivityManager_h

#include "Activity.h"

#define MAX_ACTIVITY_STACK 10

class ActivityManager
{
protected:
  // Activities stack utility
	static Activity* _activities[MAX_ACTIVITY_STACK];

	// Stores the next activity to run (Buffer)
	// Used to run activities from interrupts
	static Activity* nextActivity;

	static int _index;
	static void kill();
	static bool push(Activity* activity);
	static bool pop();

public:
	// ActivityManager();
	
	// Overload method, used to run others Activityes
	static bool start(Activity* activity, bool startNow = true);

  	// Activity stack utility

  	// This method will return the current running activity
  	// (the last on the stack)
	static Activity* getCurrentActivity();

};

#endif