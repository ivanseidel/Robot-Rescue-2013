#ifndef DISABLE_IMU

#include "views/TextView.h"

class TestIMUActivity: public Activity
{
protected:

	/*TextView 	tY, vY,
				tP, vP,
				tR, vR;

	Alignment align;

	ProgressBar *bars[4];

	String barsTitle[4];

	ViewGroup *axlesGroup;
	TextView *axles[3];

	Button *reset;*/

	AngleView *yprViews[3];

	GraphicView *graphic;

public:

	static int xAccel, yAccel,zAccel;

	static float int1[3];
	static float int2[3];

	/*static void resetIntegral(){
		TestIMUActivity::int1[0] = 0.0;
		TestIMUActivity::int1[1] = 0.0;
		TestIMUActivity::int1[2] = 0.0;
	}*/

	TestIMUActivity(){
		Activity::Activity();

		ActivityName =String("Test IMU");

		for(int i = 0; i < 3; i++)
		{
			yprViews[i] = new AngleView();
			yprViews[i]->o = Point(100 + i*250, 120);
			yprViews[i]->w = 80;
			addView(yprViews[i]);
		}

		//tY = TextView(String("SCORE"));
		/*tP = TextView(String("Pitch"));
		tR = TextView(String("Roll"));*/
		//tY.o = Point(70,10);
		/*tP.o = Point(360,10);
		tR.o = Point(630,10);*/

		//vY = TextView(String("-"));
		/*vP = TextView(String("-"));
		vR = TextView(String("-"));*/
		//vY.o = Point(70,40);
		/*vP.o = Point(360,40);
		vR.o = Point(630,40);*/

		/*addView(&tY);
		addView(&tP);
		addView(&tR);
		addView(&vY);
		addView(&vP);
		addView(&vR);

		barsTitle[0] = "W";
		barsTitle[1] = "X";
		barsTitle[2] = "Y";
		barsTitle[3] = "Z";

		for(int i = 0 ; i < 4; i++){
			TextView *tv = new TextView(barsTitle[i]);
			tv->o = Point(20, 20 + i*70);
			addView(tv);

			ProgressBar *pb = new ProgressBar();
			pb->w = 350;
			pb->setMax(1000);
			pb->setMin(-1000);
			pb->o = Point(20, tv->o.y + 30);
			addView(pb);
			bars[i] = pb;
		}


		axlesGroup = new ViewGroup();
		axlesGroup->o = Point(450,40);

		for(int i = 0; i < 3; i++){
			axles[i] = new TextView(String("-"));
			axles[i]->o = Point(0,i*30);
			axlesGroup->addView(axles[i]);
		}
		addView(axlesGroup);

		reset = new Button();
		reset->setArea(600,350,150,40);
		reset->name = String("Reset");
		reset->onClick(resetIntegral);
		addView(reset);*/

		/*graphic = new GraphicView(10,-314, 314);
		graphic->o = Point(10, 280);
		graphic->h = 150;
		graphic->w = 780;

		addView(graphic);*/

		Threads.add(IMU);

	}

	/*float _xSpeed;
	float _ySpeed;

	float kXSpeed;
	float kYSpeed;

	float _x, _y;
	int _renX, _renY;

	int diameter;

	void drawBall(){
		// Erase last circle position
		LCD.setColor(cBLACK);
		LCD.fillCircle(_renX, _renY, diameter);

		_renX = _x;
		_renY = _y;
		LCD.setColor(cWHITE);
		LCD.fillCircle(_renX, _renY, diameter);
	}

	int xHit, yHit, hitDiameter;
	void generateTarget(){
		ArduinOS::beep();

		LCD.setColor(cBLACK);
		LCD.fillCircle(xHit, yHit, hitDiameter);

		randomSeed(micros());
		do{
			xHit = random(10, 780);
			yHit = random(40, 450);
		}while(abs((xHit - _renX)) < hitDiameter*4 && abs((yHit - _renY)) < hitDiameter*4);

		LCD.setColor(cYELLOW);
		LCD.fillCircle(xHit, yHit, hitDiameter);
	}

	long startGame;
	long score;
	bool GAMEOVER;
	int filter;*/
	void onResume(){
		/*_xSpeed = 0;
		_ySpeed = 0;
		kXSpeed = 0.01;
		kYSpeed = 0.01;
		_x = 800/2.0;
		_y = 480/2.0;
		_renX = _x;
		_renY = _y;

		xHit = 0;
		yHit = 0;

		diameter = 10;
		hitDiameter = 20;

		score = 0;
		GAMEOVER = false;
		filter = 0;

		startGame = millis();
		generateTarget();*/

		IMU->start();
		IMU->readAccelReal = true;
	}


	

	void onLoop(){
		if(!IMU->hasNewData)
			return;

		yprViews[0]->setAngle(-IMU->ypr[0]);
		yprViews[1]->setAngle(IMU->ypr[1]);
		yprViews[2]->setAngle(IMU->ypr[2]);

		// graphic->addValue(IMU->ypr[0]*100);

		/*bars[0]->setValue(IMU->quaternion.w*1000, true);
		bars[1]->setValue(IMU->quaternion.x*1000, true);
		bars[2]->setValue(IMU->quaternion.y*1000, true);
		bars[3]->setValue(IMU->quaternion.z*1000, true);

		/*TestIMUActivity::int1[0] += IMU->accelReal.x;
		TestIMUActivity::int1[1] += IMU->accelReal.y;
		TestIMUActivity::int1[2] += IMU->accelReal.z;

		axles[0]->setValue(floatToString(TestIMUActivity::int1[0],3),true);
		axles[1]->setValue(floatToString(TestIMUActivity::int1[1],3),true);
		axles[2]->setValue(floatToString(TestIMUActivity::int1[2],3),true);*/

		/*if(score < 6){
			xAccel = -(int)(IMU->ypr[2]*180/M_PI);
			yAccel = -(int)(IMU->ypr[1]*180/M_PI);
			zAccel = -(int)(IMU->ypr[0]*180/M_PI);
		
			// estimate acceleration
			_xSpeed = _xSpeed + xAccel*kXSpeed;
			_ySpeed = _ySpeed + yAccel*kYSpeed;

			// Atualize position
			_x += _xSpeed;
			_y += _ySpeed;

			if(_x > 790.0){
				_x = 790.0;
				_xSpeed = -_xSpeed;
			}
			if(_x < 10.0){
				_x = 10.0;
				_xSpeed = _xSpeed;
			}

			if(_y > 470.0){
				_y = 470.0;
				_ySpeed = -_ySpeed;
			}
			if(_y < 50.0){
				_y = 50.0;
				_ySpeed = -_ySpeed;
			}

			_xSpeed = min(10.0, max(-10.0, _xSpeed));
			_ySpeed = min(10.0, max(-10.0, _ySpeed));

			drawBall();

			// Check for ball hits
			if(abs((xHit - _renX) < hitDiameter) && abs((yHit - _renY) < hitDiameter)){
				filter++;
				if(filter > 10){
					filter = 0;
					score++;
					generateTarget();
				}
			}else{
				filter = 0;
			}


			vY.setValue(String(score));

		}else if(!GAMEOVER){
			ArduinOS::STAGE->render(true);
			ArdUI::LCD->setColor(cYELLOW);
			ArdUI::LCD->print("SCORE:", 280, 170);
			ArdUI::LCD->print(String(millis()-startGame), 280, 200);
			GAMEOVER = true;
		}

		// Check if game ended


		// IMU->hasNewData = false;

		delay(20);*/

	};

	void onStop(){
		IMU->stop();
		IMU->setDefaults();
	}
};

#endif