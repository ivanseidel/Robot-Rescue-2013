
class Program1Activity: public Activity
{
public:

	// Map *map;

	// MapView *mapView;

	AngleEstimator *angleEstimator1, *angleEstimator2;

	GaussianAverage average;

	AngleView *mean, *meanavg;

	Program1Activity(){
		ActivityName = "Program 1";

		// map = new Map(12, 12);

		// map->getCell(0,0)->hasVictim = true;
		// map->getCell(1,1)->hasVictim = true;
		// // map->getCell(0,0)->walls[North]->setProb(1.0);
		// // map->getCell(3,3)->walls[West]->setProb(1.0);
		// // map->getCell(0,3)->walls[East]->setProb(1.0);
		// // map->getCell(3,0)->walls[South]->setProb(1.0);

		// map->getCell(0,0)->walls[South]->exist(true);
		// map->getCell(0,0)->walls[West]->exist(true);
		// map->getCell(0,0)->walls[East]->exist(true);
		// map->getCell(0,1)->walls[North]->exist(true);

		// mapView = new MapView(map);

		// mapView->o = Point(20,20);

		// addView(mapView);

		mean = new AngleView();
		mean->o = Point(250, 200);
		mean->w = 100;

		meanavg = new AngleView();
		meanavg->o = Point(550, 200);
		meanavg->w = 100;

		addView(mean);
		addView(meanavg);

		angleEstimator1 = new AngleEstimator(DistLeftFront, DistLeftBack, 14, false);
		angleEstimator2 = new AngleEstimator(DistRightFront, DistRightBack, 14, true);

		average = GaussianAverage(10);


	}

	Gaussian result1, result2, sum;

	void onLoop(){
		// Serial.println(".");
		result1 = angleEstimator1->process();
		result2 = angleEstimator2->process() + Gaussian(0, 100.0);

		sum = result1 + result2;

		average.add(sum);

		/*Serial.print("\n");
		Serial.print("Mean:\t");
		Serial.print(result1.mean * 180/3.1415);
		Serial.print("\tVariance:\t");
		Serial.println(result1.variance);
		Serial.print("Mean:\t");
		Serial.print(result2.mean * 180/3.1415);
		Serial.print("\tVariance:\t");
		Serial.println(result2.variance);
		Serial.print("Mean:\t");
		Serial.print(sum.mean * 180/3.1415);
		Serial.print("\tVariance:\t");
		Serial.println(sum.variance);*/

		mean->setAngle(-sum.mean - HALF_PI);
		// Serial.println("\t.");
		meanavg->setAngle(-average.process().mean - HALF_PI);

		// MC->setSpeed(0, -1.4, sum.mean*5);
		/*long start = millis();
		for(int i = 0; i < 10; i++){
			mapView->render(false);
		}
		Serial.println(millis() - start);*/
		// Serial.println("\t\t.");

		delay(50);
		// Serial.println("\t\t\t.");
	}

};