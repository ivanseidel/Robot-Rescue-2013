#define NEAR_CHANCE 1.0
#define GO_FRONT 1
#define TURN_LEFT 0
#define TURN_RIGHT 2

void zigBee_changed(bool val){
	digitalWrite(14, val);
}

class Program2Activity: public Activity
{
public:

	Map *map;

	long lastVictim;
	MapView *mapView;
	SeekBar *victimNumber;


	LinkedList<int> moves;
	LinkedList<Cell*> *cells;
	BFS bfs;


	int st;
	int firstLightCalibration;

	int victims;

	bool firstRun;

	CheckBox *zigBee;
	CheckBox *check;

	Program2Activity(){
		Activity::Activity();
		ActivityName = "Program 2";

		map = new Map(12, 12);

		st = 0;
		firstRun = true;
		victims = 0;
		// map->getCell(0,0)->hasVictim = true;
		// map->getCell(1,1)->hasVictim = true;
		// map->getCell(0,0)->walls[North]->setProb(1.0);
		// map->getCell(3,3)->walls[West]->setProb(1.0);
		// map->getCell(0,3)->walls[East]->setProb(1.0);
		// map->getCell(3,0)->walls[South]->setProb(1.0);

		//  __ __ __ 
		// |__|__|__|
		// |__|__|__|
		// |__|__|__|
		// |__|__|__|
		// |__|
		

		map->getCell(0,0)->walls[South]->exist(true);
		map->getCell(0,0)->walls[East]->exist(true);

		map->getCell(0,0)->walls[West]->exist(true);
		map->getCell(0,1)->walls[West]->exist(true);
		map->getCell(0,2)->walls[West]->exist(true);
		map->getCell(0,3)->walls[West]->exist(true);
		map->getCell(0,4)->walls[West]->exist(true);

		map->getCell(2,1)->walls[East]->exist(true);
		map->getCell(2,2)->walls[East]->exist(true);
		map->getCell(2,3)->walls[East]->exist(true);
		map->getCell(2,4)->walls[East]->exist(true);

		map->getCell(1,1)->walls[South]->exist(true);
		map->getCell(2,1)->walls[South]->exist(true);

		map->getCell(0,4)->walls[North]->exist(true);
		map->getCell(1,4)->walls[North]->exist(true);
		map->getCell(2,4)->walls[North]->exist(true);

		poseEstimator->setX(0);
		poseEstimator->setY(0);
		poseEstimator->direction = North;
		poseEstimator->currentMap = map;

		mapView = new MapView(poseEstimator);

		mapView->o = Point(20,20);
		mapView->xOffset = -1;
		mapView->xSpan = 5;
		mapView->yOffset = -1;

		addView(mapView);

		victimNumber = new SeekBar();
		victimNumber->setMax(20);
		victimNumber->setMin(0);
		victimNumber->o = Point(400, 20);
		victimNumber->w = 300;
		victimNumber->setValue(9);
		addView(victimNumber);

		check = new CheckBox("Enable?");
		check->o = Point(400,60);
		check->w = 100;
		check->h = 100;
		addView(check);

		zigBee = new CheckBox("Enable?");
		zigBee->o = Point(400,200);
		zigBee->w = 100;
		zigBee->h = 100;
		zigBee->onChange(zigBee_changed);
		addView(zigBee);


		Threads.add(IMU);
		Threads.add(VICTIM);

		lastVictim = 0;

		// mean = new AngleView();
		// mean->o = Point(250, 200);
		// mean->w = 100;

		// meanavg = new AngleView();
		// meanavg->o = Point(550, 200);
		// meanavg->w = 100;

		// addView(mean);
		// addView(meanavg);

		// angleEstimator1 = new AngleEstimator(DistLeftFront, DistLeftBack, 14, false);
		// angleEstimator2 = new AngleEstimator(DistRightFront, DistRightBack, 14, true);

		// average = GaussianAverage(10);

		moves = LinkedList<int>();


	}

	GaussianAverage temperature;

	void onResume(){
		IMU->start();
		temperature = GaussianAverage(4);
		check->setValue(false, true);

		poseEstimator->setX(0);
		poseEstimator->setY(0);
		poseEstimator->direction = North;

		firstLightCalibration = Light2.read();

	}

	void onStop(){
		IMU->stop();
		MC->setSpeed(0,0,0);
	}

	void mapping(){

		Cell *currCell = poseEstimator->getCurrentCell();
		if(DistRightBack->readDistance() < 17)
			currCell->getRelativeWall(poseEstimator->direction, East)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, East)->exist(false);//updateWall(1.0-NEAR_CHANCE);
			
		if(DistRightFront->readDistance() < 17)
			currCell->getRelativeWall(poseEstimator->direction, East)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, East)->exist(false);//updateWall(1.0-NEAR_CHANCE);

		if(DistLeftFront->readDistance() < 17)
			currCell->getRelativeWall(poseEstimator->direction, West)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, West)->exist(false);//updateWall(1.0-NEAR_CHANCE);

		if(DistLeftBack->readDistance() < 17)
			currCell->getRelativeWall(poseEstimator->direction, West)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, West)->exist(false);//updateWall(1.0-NEAR_CHANCE);

		if(DistFront->readDistance() < 25)
			currCell->getRelativeWall(poseEstimator->direction, North)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, North)->exist(false);//updateWall(1.0-NEAR_CHANCE);

		if(DistBack->readDistance() < 25)
			currCell->getRelativeWall(poseEstimator->direction, South)->exist(true);//updateWall(NEAR_CHANCE);
		else
			currCell->getRelativeWall(poseEstimator->direction, South)->exist(false);//updateWall(1.0-NEAR_CHANCE);

			// poseEstimator->currentMap->getRelativeCell(currCell, poseEstimator->getRelativeDirection(East), 1)->
			// poseEstimator->getRelativeDirection(West);
	}

	Cell *nextCell(){
		

	}

	void makeMovementList(){
		Cell *lastCell = cells->get(0);
		moves.clear();

		Direction lastDirection = poseEstimator->direction;


		for(int i = 1; i < cells->size(); i++){
			Cell *thisCell = cells->get(i);
			Direction thisDirection;

			int dx = thisCell->x - lastCell->x;
			Serial.print("XY: ");
			Serial.print(thisCell->x);
			Serial.print(" ");
			Serial.print(thisCell->y);
			Serial.print("\t");
			Serial.print(lastCell->x);
			Serial.print(" ");
			Serial.println(lastCell->y);
			int dy = thisCell->y - lastCell->y;
			if(dx < 0)
				thisDirection = West;
			else if(dx > 0)
				thisDirection = East;
			else if(dy < 0)
				thisDirection = South;
			else if(dy > 0)
				thisDirection = North;

			
			Direction relative = poseEstimator->getRelativeDirection(lastDirection, thisDirection);

			Serial.print(lastDirection);
			Serial.print(" ");
			Serial.print(thisDirection);
			Serial.print(" ");
			Serial.println(relative);

			if(relative == North){
				moves.add(GO_FRONT);
			}else if(relative == East){
				moves.add(TURN_RIGHT);
				moves.add(GO_FRONT);
			}else if(relative == West){
				moves.add(TURN_LEFT);
				moves.add(GO_FRONT);
			}else{
				moves.add(999);
			}

			lastDirection = thisDirection;
			lastCell = thisCell;
		}

		for(int i = 0; i < moves.size(); i++){
			Serial.print("Move: ");
			Serial.println(moves.get(i));
		}
	}

	void alignBack(){
		while(DistBack->readDistance() >= 16 && DistBack->getDistance() <= 18)
			MC->setSpeed(0,0.5,0);
		MC->setSpeed(0,0,0);
	}

	void onLoop(){

		if(!check->isChecked())
			return;
		// 	if(!wasChecked)
		// 		poseEstimator->direction = poseEstimator->getAbsoluteDirection(East);

		// 	wasChecked = true;
		// }else{
		// 	wasChecked = false;
		// }

		if(firstRun){

			MazeWalker::Front();
			poseEstimator->Front();

			firstRun = false;
		}

		if(abs(firstLightCalibration - Light2.read()) > 60){
			poseEstimator->getCurrentCell()->isBlack = true;
			mapView->render(true);
			delay(5000);
			stop();
			return;
		}

		mapping();
		mapView->render(true);
		

		Cell *currCell = poseEstimator->getCurrentCell();
		currCell->isVisited = true;


		if(!currCell->hasVictim){
			for(int i = 0; i < 5; i++){
				temperature.add(Gaussian(VICTIM->deviation, 10.0));
				delay(50);
			}

			if(temperature.process().mean > victimNumber->getValue()/10.0){
				currCell->hasVictim = true;

				VictimLed = 1;
				delay(300);
				VictimLed = 0;
				delay(200);
				VictimLed = 1;
				delay(300);
				VictimLed = 0;
				delay(200);
				VictimLed = 1;
				delay(300);
				VictimLed = 0;
				delay(200);

				if(++victims >= 3)
					zigBee->setValue(true,true);
			}
		}

		bool leftWall = currCell->getRelativeWall(poseEstimator->direction, West)->exist();
		bool northWall = currCell->getRelativeWall(poseEstimator->direction, North)->exist();
		bool eastWall = currCell->getRelativeWall(poseEstimator->direction, East)->exist();

		Serial.print(leftWall);
		Serial.print("\t");
		Serial.print(northWall);
		Serial.print("\t");
		Serial.println(eastWall);

		if(!leftWall){
			MazeWalker::Left();
			poseEstimator->Left();
			mapView->render(true);

			MazeWalker::magneticAlign();
			MazeWalker::Front();
			poseEstimator->Front();
		}else if(!northWall){
			MazeWalker::Front();
			poseEstimator->Front();
			MazeWalker::magneticAlign();
		}else if(!eastWall){
			MazeWalker::Right();
			poseEstimator->Right();
			MazeWalker::magneticAlign();
			alignBack();
		}else{
			MazeWalker::Back();
			poseEstimator->Right();
			poseEstimator->Right();
			alignBack();
		}



		// Serial.println("currentCell");
		// currCell->isVisited = true;
		
		// Serial.println("bfs");

		// Cell *targetCell = map->getCell(2,4);
		// if(st == 0 && bfs.compare(map->getCell(2,4), currCell))
		// 	st = 1;

		// if(st == 1)
		// 	targetCell =  map->getCell(rand()%5, rand()%4 + 1);
		

		// cells = bfs.search(map, currCell, targetCell);
		// Serial.println("move list:");
		// makeMovementList();

		// if(moves.size() > 0){
		// 	int nextMoveInt = moves.get(0);

		// 	if(nextMoveInt == 1){
		// 		MazeWalker::Front();
		// 		poseEstimator->Front();
		// 	}else if(nextMoveInt == 0){
		// 		MazeWalker::Left();
		// 		poseEstimator->Left();
		// 	}else if(nextMoveInt == 2){
		// 		MazeWalker::Right();
		// 		poseEstimator->Right();
		// 	}
		// }
		currCell->isVisited = true;
		mapView->render(true);

		// // nextMove();

		delay(500);

		// Gaussian g = poseEstimator->estimateLateralDistance();
		// Serial.print(g.mean);
		// Serial.print("\t");
		// Serial.print(g.variance);
		// Serial.print("\n");
		// delay(200);

		// MazeWalker::rotate(90);
		// MazeWalker::Left();
		
		// double angleSpeed = 0.0;
		// double headSpeed = 1.5;

		// // headSpeed = (distFront->readDistance())
		// Serial.println(DistFront->readDistance());
		// angleSpeed = 10 - DistLeftFront->readDistance();

		// if(toDegs(IMU->ypr[1] < 0? -IMU->ypr[1]: IMU->ypr[1]) > 15){
		// 	MC->setSpeed(0,headSpeed/2,-angleSpeed/3.0);
		// }else if((DistFront->getDistance() < 16  &&
		// 	DistLeftFront->getDistance() < 8 &&
		// 	DistRightFront->readDistance() < 8) || Light2.read() < 150){

		// 	angleSpeed = 3.0;
		// 	headSpeed = -0.5;
		// 	MC->setSpeed(0,headSpeed,-angleSpeed);
		// 	delay(200);

		// }else if(DistFront->getDistance() < 15){
			
		// 	angleSpeed = 3.0;
		// 	headSpeed = -0.5;
		// 	MC->setSpeed(0,headSpeed,-angleSpeed);
		// 	delay(80);
		// }else{

		// 	// headSpeed /= max(30, DistLeftFront->getDistance())/20;
		// 	if(DistLeftFront->getDistance() > 15)
		// 		headSpeed /= 10;
		// 	MC->setSpeed(0,headSpeed,-angleSpeed/1.0);
		// }

		// temperature.add(Gaussian(VICTIM->deviation, 10.0));

		// if(temperature.process().mean > 1.8 && millis() - lastVictim > 3000){
		// 	MC->stop();

		// 	VictimLed = 1;
		// 	delay(300);
		// 	VictimLed = 0;
		// 	delay(300);
		// 	VictimLed = 1;
		// 	delay(300);
		// 	VictimLed = 0;
		// 	delay(300);
		// 	VictimLed = 1;
		// 	delay(300);
		// 	VictimLed = 0;
		// 	delay(300);

		// 	lastVictim = millis();
		// }


		// delay(50);

		// MazeWalker::Front();
		// poseEstimator->Front();
		// mapView->render(true);

		// MazeWalker::Front();
		// poseEstimator->Front();
		// mapView->render(true);

		// MazeWalker::Left();
		// poseEstimator->Left();
		// mapView->render(true);

		// MazeWalker::Front();
		// poseEstimator->Front();
		// mapView->render(true);

		// MazeWalker::Back();
		// poseEstimator->Left();
		// poseEstimator->Left();
		// mapView->render(true);

		// MazeWalker::Front();
		// poseEstimator->Front();
		// mapView->render(true);

		// MazeWalker::Right();
		// poseEstimator->Right();
		// mapView->render(true);
		// stop();
		
		// MazeWalker::Right();
		// MazeWalker::Back();
		// delay(1000);
		// delay(3000);
		// Serial.println(".");
		// result1 = angleEstimator1->process();
		// result2 = angleEstimator2->process();

		// sum = result1 + result2;

		// average.add(sum);

		// Serial.print("\n");
		// Serial.print("Mean:\t");
		// Serial.print(result1.mean * 180/3.1415);
		// Serial.print("\tVariance:\t");
		// Serial.println(result1.variance);
		// Serial.print("Mean:\t");
		// Serial.print(result2.mean * 180/3.1415);
		// Serial.print("\tVariance:\t");
		// Serial.println(result2.variance);
		// Serial.print("Mean:\t");
		// Serial.print(sum.mean * 180/3.1415);
		// Serial.print("\tVariance:\t");
		// Serial.println(sum.variance);

		// mean->setAngle(-sum.mean - HALF_PI);
		// Serial.println("\t.");
		// meanavg->setAngle(-average.process().mean - HALF_PI);

		// // MC->setSpeed(0, -1.4, sum.mean*5);
		// /*long start = millis();
		// for(int i = 0; i < 10; i++){
		// 	mapView->render(false);
		// }
		// Serial.println(millis() - start);*/
		// Serial.println("\t\t.");

		// delay(50);
		// Serial.println("\t\t\t.");
	}

};