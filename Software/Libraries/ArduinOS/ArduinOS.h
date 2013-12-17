/*
	ArduinOS.h - Main class responsable for configuration and starting main process

	This Library requires others libraries to run:
		* ArdUI 			- https://github.com/ivanseidel/ArdUI
		* ArduinoThread 	- https://github.com/ivanseidel/ArduinoThread

	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/

#ifndef ArduinOS_h
#define ArduinOS_h

#ifndef DEBUG_FUNCS
// #define DEBUG
#define DEBUG_FUNCS 1
#ifdef DEBUG
    inline void debug(const char *s) { Serial.println(s); }
    inline void debug(const int s) { Serial.println(s); }
    inline void debug(const char *s, const char *t) { Serial.print(s);Serial.println(t); }
    inline void debug(const char *s, const int t) { Serial.print(s);Serial.println(t); }
#else
    inline void debug(const char *s) {  }
    inline void debug(const int i) {  }
    inline void debug(const char *s, const char *t) { }
    inline void debug(const char *s, const int t) { }
#endif
#endif

#include "ArdUI.h"
#include "Graphics.h"

#include "Activity.h"
#include "ActivityManager.h"
#include "ThreadController.h"

class ArduinOS
{
protected:
	/*
		Configure ambient with Activity flags

		This method is called before any activity starts.
		It's responsable for reading the configuration flags
		from the Activity and setting up the current enviroment.
	*/
	static void prepareForActivity(Activity* act);

public:
	// ActivityManager callbacks

	/*
		onActivityStart is called 
	*/
	static void onActivityStart(Activity* act);
	static void onActivityResume(Activity* act);

	/*
		Rendering stuff
	*/
	
	/*
		_rel is the start of the screen, so that UI
		stays correct at the top, and the use of "relative rendering" works

		Imagine that this point, is the same as the start of the REAL content.
		If you have a top status bar of 32pixels height, then the _rel will be
		Pont(0, 32);
	*/
	static Point _rel;
	
	/*
		General User Interface (System Interface)

		Use this View to render the GUI, such as:
			* Battery voltage
			* Bluetooth status
			* Activity name
			* ThreadUsage
			* etc...
	*/
	static View* GUI;
	
	/*
		Stage (Background and other stuff) that will be BEHIND all views

		Instead of erasing all the screen (making it blink), we use this
		STAGE View, to erase only what is renderable.

		Imagine that you have a top status bar of 50px. Then your STAGE should
		render a box that will fill the entire screen, EXCEPT for the top
		status bar. This prevents the status bar from blinking.

		Also, feel free to use whatever color you want...
	*/
	static View* STAGE;
	
	/*
		This is a very important flag used by ArduinOS, to know that a
		View suffered changes and should be re-rendered.

		ActivityManager keeps tracks of it, and whenever this is set to true,
		it will call a render() on the root view, making the entire scene
		to be updated.

		Note: This is not runned on Threads, but on the ActivityManager itself.
	*/
	static bool requestRender;

	/*
		This will render Views in the Tree, starting from:
			1. GUI
			2. STAGE
			3. Root view (the Activity itself, since it extends ViewGroup class)

		If forceRender is set to true, then this will override the
		_invalidate of the view, forcing it to render.

		Note: This shouldn't be called from you. ArduinOS will deal with all
		rendering stuff for you automaticaly. However, if you want to manualy
		request a synced render, call this method.

		Note2: GUI and STAGE will ONLY be rendered by this method, IF forceRender = true;
	*/
	static void render(bool forceRender = false);

	/*
		Interruptions (By touch, button...)

		This will forward the action the the current running Activity
	*/
	// Handles external interrupts, such as:
	// Return button, Confirm, left, right...
	static void actionHandler(ActionType type); // Redirects to actionHandler(ActionEvent)
	static void actionHandler(ActionEvent evt);

	/*
		ThreadController configurations
	*/
protected:
	/*
		_mainThreadController is the ThreadController responsable
		for running all others Threads of ArduinOS.

		It will be updated every time an activity starts, and
		the activity Threads will be registered to this controller.

		Also, it will keeps include the SystemThreads
	*/
	static ThreadController* _mainThreadController;

	// Static Method required to be the callback of _mainThreadController
	static void runThread();

public:
	/*
		Keeps the System time sensitive Threads, such as:
			GUI Updates

		(will be running on All activities)
	*/
	static ThreadController* SystemThreads;

	/*
		This is the interval between calls to run the main
		thread controller, in miliseconds.
	*/
	static int _threadInterval;

	/*
		Timer responsable for running the threads.
		This timer will run runThread() method, that
		will run _mainThreadController threads.
	*/
	static DueTimer* _threadTimer;		// Timer responsable for running
										// Activity Threads and ArduinOS Threads

	/*
		Keeps track of the current CPU usage of 
		_mainThreadController.

		It's based on the time took to run all Threads,
		and it's period.

		| --------------- _threadInterval ---------------- |
		| ----  Thread time ----- | -------- Idle -------- |

		To calculate the cpuThreadUsage we do:
			threadUsage = (threadTime / _threadInterval) * 100

		It's value goes from 0 to 100.
			0 meaning that there is almost nothing running.
			100 meaning that 100% of the threadInterval is beign used.
	*/
	static int cpuThreadUsage;
};

#endif


