void TestMotorsActiviy_select_m1(long i){
	Serial.print("M1: ");
	Serial.println(i);

	double speed = i/100.0;
	MD->setSpeed(speed, speed, speed, speed);
}

class TestMotorsActivity: public Activity
{
protected:
	SeekBar m1Val, m2Val, m3Val, m4Val;
	LinearLayout group;

	DrawableLine vector;
	TextView angle, speed;
public:

	TestMotorsActivity(){
		ActivityName = String("Test Motors");

		group = LinearLayout();
		group.margin = 40;
		group.o = Point(20, 20);

		angle = TextView();
		group.addView(&angle);

		speed = TextView();
		group.addView(&speed);

		/*m1Val = SeekBar();
		m1Val.w = 250;
		m1Val.setMin(-200);
		m1Val.setMax(200);
		m1Val.onChange(TestMotorsActiviy_select_m1);
		m2Val.setValue(0);
		group.addView(&m1Val);

		m2Val = SeekBar();
		m2Val.w = 250;
		m2Val.setMin(-200);
		m2Val.setMax(200);
		m2Val.onChange(TestMotorsActiviy_select_m1);
		m2Val.setValue(0);
		group.addView(&m2Val);

		addView(&group);*/
		_requestTouch = true;

		w = 800;
		h = 480;
		vector = DrawableLine();
		vector.line = Line(400,220,400,220);
		addView(&vector);

		addView(&group);

		Threads.add(IMU);
	}

	void onCreate(){
		IMU->start();
	}

	void onStop(){
		MC->setSpeed(0,0,0);
		
		IMU->stop();
		IMU->setDefaults();
	}

	void onLoop(){
		/*if(vector.line.getLength() < 20){
			vector.line.p2 = vector.line.p1;
			MC->setSpeed(0, 0);
		}else{
			Serial.println(IMU->ypr[0]);
			MC->setSpeed(vector.line.getAngle()*180.0/M_PI, (vector.line.getLength()-20)/200.0, IMU->ypr[0]*4);
			// MC->setSpeed(0, 0, IMU->ypr[0]*4);
		}*/
		// Serial.print("Enc:\t");
		// Serial.print(MD->readTicks((Motor)0));
		MD->readTicks();
		for(int i = 0; i < 4; i++){
			Serial.print("Motor");
			Serial.print(i);
			Serial.print("\t");
			Serial.println(MD->getTicks((Motor)i));
		}
		delay(100);
	}

	virtual void onTouch(ActionEvent evt){
		switch(evt.type){
				
			case ACTION_DOWN:
			case ACTION_MOVE:
	        	vector.line.p2 = Point(evt.x - x1(), evt.y - y1());
	        	vector.invalidate();

	        	// process();

	        	angle.setValue((long)(vector.line.getAngle()*180/PI));
	        	speed.setValue((long)vector.line.getLength());

	        	break;

	        case ACTION_HOVER_EXIT:
			case ACTION_UP:
				break;

	    }
	};
};