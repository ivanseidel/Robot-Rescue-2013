// #include "views/TextView.h"
#include "Math.h"

class TestVictimActivity: public Activity
{
protected:

	TextView 	deviation, deviationTag, *tempTag[10];
	DrawableLine dLine;

public:

	float x, y, scale;

	int _rX1, _rX2, _rY1, _rY2;
	int zeroX, zeroY;

	int tempXY[10][2];

	TestVictimActivity(){
		Activity::Activity();

		ActivityName =String("Test Victim");

		zeroX = 800/2;
		zeroY = 480/2;

		_rX1 = 0;
		_rY1 = 0;
		_rX2 = 0;
		_rY2 = 0;

		deviationTag = TextView(String("Deviation"));
		deviationTag.o = Point(630, 10);
		addView(&deviationTag);
		deviation = TextView(String("-"));
		deviation.o = Point(630, 40);
		addView(&deviation);

		for(int i = 0; i < 10; i++){
			tempTag[i] = new TextView(String("-"));
			tempTag[i]->o = Point(630, 120+i*30);
			addView(tempTag[i]);
		}

		dLine = DrawableLine();
		dLine.o = Point(zeroX, zeroY);
		addView(&dLine);

		// Add Victim Thread
		Threads.add(VICTIM);

	}

	void onResume(){
		
		scale = 20;

	}

	int xAccel, yAccel,zAccel;

	void onLoop(){
		if(!VICTIM->hasNewData)
			return;

		// Atualize vector
		x = VICTIM->vX;
		y = VICTIM->vY;

		noInterrupts();
		// Update text views
		deviation.setValue(floatToString(VICTIM->deviation));

		for(int i = 0; i < 10; i++)
			tempTag[i]->setValue(floatToString(VICTIM->temp[i]));

		// Draw Vector
		dLine.line.p2.x = -scale*x;
		dLine.line.p2.y = scale*y;
		dLine.invalidate();

		// _rX1 = zeroX;
		// _rY1 = zeroY;
		// _rX2 = _rX1 + scale*x;
		// _rY2 = _rY1 - scale*y;
		
		interrupts();

		// drawBall();
		// delay(100);
		VICTIM->hasNewData = false;
	};
};