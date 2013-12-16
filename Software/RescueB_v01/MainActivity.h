/*
	If you need to create a simple menu,
	to run many activities, than this
*/
#ifndef DISABLE_ZYGOTE

#define MENU_WIDTH	790
#define MENU_HEIGHT	50
#define MENU_SPACE	2

#include <LinkedList.h>
#include <views/Button.h>

class MainActivity: public Activity{
protected:

	class CustomButton: public Button{
	public:
		Activity* actToStart;
		
		CustomButton(){
			actToStart = 0;

			Button::Button();
		};

		void onClick(){
			ArduinOS::beep();
			if(actToStart)
				actToStart->start(false);
		};
	};

public:
	LinkedList<Activity*>* activities;

	void initialize(){
		for(int i = 0; i < activities->size(); i++){
			Activity* _act = activities->get(i);
			CustomButton* btn = new CustomButton();

			btn->name = _act->ActivityName;
			btn->setArea(
				24,
				MENU_HEIGHT*i,
				MENU_SPACE + MENU_WIDTH - 28,
				MENU_HEIGHT - MENU_SPACE);
			// Register Activity to start callback
			btn->actToStart = _act;

			// Detail View
			Button* v = new Button();
			v->_requestTouch = false;
			v->setArea(
				0,
				MENU_HEIGHT*i,
				22,
				MENU_HEIGHT - MENU_SPACE);
			v->cBg = color(240,200,31);
			v->name = String("");

			addView(btn);
			addView(v);
		}
	};

	MainActivity(){
		ActivityName = String("Main Menu");
		_enableReturn = false;

		// Initialize LinkedList
		activities = new LinkedList<Activity*>();

		Activity::Activity();
	};

	void onCreate(){
		beep();
		delay(80);
		beep();
	}

	void onLoop(){
		/*MD->setSpeed(1,0,0,0);
		delay(1000);
		MD->setSpeed(0,1,0,0);
		delay(1000);
		MD->setSpeed(0,0,1,0);
		delay(1000);
		MD->setSpeed(0,0,0,1);
		delay(1000);
		MD->setSpeed(0,0,0,0);
		delay(2000);*/

		// delay(500);
		// LightLed = 1;
		// delay(500);
		// LightLed = 0;

		// delay(1000);
		// Serial.println("alive");
	}



};

#endif