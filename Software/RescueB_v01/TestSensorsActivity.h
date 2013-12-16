class TestSensorsActivity: public Activity, Thread
{
protected:

	ProgressBar light1,
				light2,
				dist[8];

	LinearLayout group1, group2;

	// Thread updateViews;
public:

	TestSensorsActivity(){
		Activity::Activity();

		group1 = LinearLayout();
		group1.o = Point(20,50);
		group1.margin = 40;

		group2 = LinearLayout();
		group2.o = Point(400,50);
		group2.margin = 40;

		// o = Point(20,20);

		ActivityName = String("Test Sensors");

		light1 = ProgressBar();
		light1.setTitle(String("Light 1"));
		light1.w = 250;
		light1.setMax(1024);
		group1.addView(&light1);

		light2 = ProgressBar();
		light2.setTitle(String("Light 2"));
		light2.w = 250;
		light2.setMax(1024);
		group1.addView(&light2);

		// Register Light threads
		Threads.add(Light[0]);
		Threads.add(Light[1]);

		// Add Distance sensors views
		for(int i = 0; i < 4; i++){
			String title = String("Dist ");
			title = title + i;
			dist[i] = *(new ProgressBar());
			dist[i].setTitle(title);
			dist[i].setMin(0);
			dist[i].setMax(1023);
			dist[i].w = 250;
			// dist[i].o = Point(0,30);

			group1.addView(&dist[i]);
		}

		for(int i = 4; i < 8; i++){
			String title = String("Dist ");
			title = title + i;
			dist[i] = *(new ProgressBar());
			dist[i].setTitle(title);
			dist[i].setMin(0);
			dist[i].setMax(1023);
			dist[i].w = 250;
			// dist[i].o = Point(0,30);

			group2.addView(&dist[i]);
		}

		addView(&group1);
		addView(&group2);

		// Configure this thread
		setInterval(50);
		Threads.add(this);
	}

	void onResume(){};
	void onLoop(){};

	void run(){
		light1.setValue(Light[0]->getValue());
		light2.setValue(Light[1]->getValue());

		for(int i = 0; i < 8; i++){
			dist[i].setValue(Dist[i]->readDistance());
		}

		runned();
	}

};