#ifndef PoseEstimator_h
#define PoseEstimator_h

#include "Utilities.h"
#include "Gaussian.h"
#include "GaussianAverage.h"
#include "Graphics.h"
#include "sensors/IMUSensor.h"
#include "sensors/DistanceInterface.h"

#include "AngleEstimator.h"
#include "DistanceEstimator.h"
#include "Map.h"

#include "math.h"


class PoseEstimator: public Thread
{
protected:

	// Angle stuff
	Gaussian angleLeft, angleRight, angleSum;
	AngleEstimator *angleLeftEstimator, *angleRightEstimator;
	GaussianAverage angleFinal;

	// Distance stuff
	DistanceEstimator *frontEstimator, *backEstimator;
	GaussianAverage kalmanDistX, kalmanDistY, lateralAverage;



	double thetaRelative;

	double gridDistance;

public:
	Map *currentMap;

	long x, y;

	Direction direction;

	PoseEstimator(
		AngleEstimator *_angleLeftEstimator,
		AngleEstimator *_angleRightEstimator,
		DistanceEstimator *_frontEstimator,
		DistanceEstimator *_backEstimator,
		MoveController *_mc){

		angleLeftEstimator = _angleLeftEstimator;
		angleRightEstimator = _angleRightEstimator;

		angleFinal = GaussianAverage(3);

		kalmanDistX = GaussianAverage(4);
		kalmanDistY = GaussianAverage(4);

		lateralAverage = GaussianAverage(4);

		direction = North;
	}

	Gaussian estimateLateralDistance(){
		long left1 = angleLeftEstimator->dist1->readDistance() + 4;
		long left2 = angleLeftEstimator->dist2->readDistance() + 4;
		long right1 = angleRightEstimator->dist1->readDistance() + 4;
		long right2 = angleRightEstimator->dist2->readDistance() + 4;

		Gaussian val = Gaussian();

		if(left1 < 25)
			val = Gaussian(15 - left1, 3);
		if(right1 < 25)
			val = Gaussian(right1 - 15);

		lateralAverage.add(val);

		return lateralAverage.process();

		// long stering = 0;

		// Gaussian g1, g2, g3, g4;

		// if(left1 < 45 && right1 < 45){
		// 	g1 = Gaussian(15 - (left1 % 30), left1 + (left1 >= 45?10000:0));
		// 	g3 = Gaussian((right1 % 30) - 15, right1 + (right1 >= 45?10000:0));
		// }
		
		// if(left2 < 45 && right1 < 45){
		// 	g2 = Gaussian(15 - (left2 % 30), left2 + (left2 >= 45?10000:0));
		// 	g3 = Gaussian((right2 % 30) - 15, right2 + (right2 >= 45?10000:0));
		// }



		// if(right1 < 45)
		// if(right2 < 45)


		// lateralAverage.add(g1+g2+g3+g4);
		// return lateralAverage.process();

	}

	Gaussian estimateAngle(){
		// Estimate angle
		angleLeft = angleLeftEstimator->process();
		angleRight = angleRightEstimator->process();

		angleSum = angleLeft + angleRight;

		angleFinal.add(angleSum);

		long fl = angleLeftEstimator->dist1->getDistance() + 4;
		long fr = angleRightEstimator->dist1->getDistance() + 4;

		Gaussian final = angleFinal.process();

		if(fl < 30)
			final.mean += (fl - 15)/20.0;
		if(fr < 30)
			final.mean += (15 - fr)/20.0;


		// Correct distance to the wall

		return final;

		// Estimate Position from Front distance
		// Gaussian frontGauss = frontEstimator->process();, 


	}

	void map(){
		// frontDist->readDistance();
		// backDist->readDistance();

		// if(frontDist->getDistance() < )
	}

	void setX(long _x){
		x = _x;
	}

	void setY(long _y){
		y = _y;
	}

	void setDirection(Direction _dir){
		direction = _dir;
	}

	void setGridDistance(double _gd){
		gridDistance = _gd;
	}

	void setPosition(Cell *cell){
		// setX(cell->x + gridDistance/2.0);
		setX(cell->x);
		// setY(cell->y + gridDistance/2.0);
		setY(cell->y);
	}

	Cell *getCurrentCell(){
		// long _x = round((x - gridDistance/2.0) / (gridDistance * 1.0));
		// long _y = round((y - gridDistance/2.0) / (gridDistance * 1.0));

		// return currentMap->getCell(_x, _y);
		return currentMap->getCell(x, y);
	}

	Wall *getNearestWallPointingAt(Direction d, long distance){
		long inv = 1, dx = 0, dy = 0, pos, cellN;

		// int pos = (Direction == North || Direction == South ? y : x);

		if(d == North)
			dy = 1;
		else if(d == East)
			dx = 1;
		else if(d == South)
			dy = -1;
		else if(d == West)
			dx = -1;

		if(d == West || d == South)
			inv = -1;

		// Calculate the North/East cell nearest;
		pos = -gridDistance/2.0 + (inv * distance);

		cellN = round(pos / (gridDistance*1.0));

		currentMap->getRelativeCell(getCurrentCell(), d, cellN);
	}

	Direction getRelativeDirection(Direction _d1, Direction _d2){
		return (Direction)((_d2-_d1 + 4)%4);
	}

	Direction getAbsoluteDirection(Direction _d){
		return (Direction)((_d + direction) % 4);
	}

	void Front(){
		Cell *nextCell = currentMap->getRelativeCell(getCurrentCell(), direction, 1);
		x = nextCell->x;
		y = nextCell->y;
	}

	void Left(){
		direction = getAbsoluteDirection(West);
	}

	void Right(){
		direction = getAbsoluteDirection(East);
	}

	void move(Movement mov){
		// if(mov == Front){
			// setPosition(currentMap->get)
		// }
	}
};

#endif
