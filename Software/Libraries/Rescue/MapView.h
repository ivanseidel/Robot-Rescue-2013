#ifndef MapView_h
#define MapView_h

#include "View.h"
#include "Map.h"
#include "PoseEstimator.h"

#define pCellVictim color(40,220,40)
#define pCellBlack color(30,30,30)
#define pCellVisited color(40,40,220)
#define pCellClear color(200,200,200)

class MapView: public View
{
protected:
	// Map *map;

	PoseEstimator *poseEstimator;

	long DistSCALE;
	long WallDist;

public:

	long 	xSpan,   // Number of cells to show in width
			ySpan,   // Number of cells to show in Height
			xOffset, // Offset (Start point) in the x axis
			yOffset; // Offset (Start point) in the y axis

	// MapView(Map *_map = 0){
	MapView(PoseEstimator *_poseEstimator = 0){
		poseEstimator = _poseEstimator;

		DistSCALE = 2;
		WallDist = 30;

		xSpan = 10;
		ySpan = 7;
		xOffset = 0;
		yOffset = 0;
	}

	Color getWallColor(Wall *wall){
		if(wall->exist())
			return color(255,0,0);
		else
			return color(200, 200, 200);
	}

	void renderCell(Cell *cell, long xs, long ys, long xSlot, long ySlot, bool renderRobot, bool forceRender = false){
		if(!cell)
			return;

		long x = cell->x;
		long y = cell->y;

		long spacer = WallDist * DistSCALE;

		long xS = ((xSlot) 			* spacer + xs);
		long xE = (xS + spacer);

		long yE = ((ySpan - ySlot) 	* spacer + ys);
		long yS = (yE - spacer);

		// long xS = ((xSpan - xSlot)     * WallDist*DistSCALE + xs);
		// long xE = ((xSpan - xSlot - 1) * WallDist*DistSCALE + xs);
		// long yS = ((ySpan - ySlot)     * WallDist*DistSCALE + ys);
		// long yE = ((ySpan - ySlot - 1) * WallDist*DistSCALE + ys);
		
		if(cell->hasVictim){
			if(forceRender || cell->lastColor != pCellVictim){
				ArdUI::LCD->setColor(pCellVictim);
				ArdUI::LCD->fillRect(xS, yS, xE, yE);
				cell->lastColor = pCellVictim;
			}
		}else if(cell->isBlack){
			if(forceRender || cell->lastColor != pCellBlack){
				ArdUI::LCD->setColor(pCellBlack);
				ArdUI::LCD->fillRect(xS, yS, xE, yE);
				cell->lastColor = pCellBlack;
			}
		}else if(cell->isVisited){
			if(forceRender || cell->lastColor != pCellVisited){
				ArdUI::LCD->setColor(pCellVisited);
				ArdUI::LCD->fillRect(xS, yS, xE, yE);
				cell->lastColor = pCellVisited;
			}
		}else{
			if(forceRender || cell->lastColor != pCellClear){
				ArdUI::LCD->setColor(pCellClear);
				ArdUI::LCD->fillRect(xS, yS, xE, yE);
				cell->lastColor = pCellClear;
			}
		}

		// North Wall
		// if(ySlot == 0){
			// ArdUI::LCD->setColor(255, 255, 0);
			ArdUI::LCD->setColor(getWallColor(cell->walls[North]));
			ArdUI::LCD->drawLine(xS, yS, xE, yS);
		// }

		// South Wall
		// ArdUI::LCD->setColor(255, 0, 0);
		ArdUI::LCD->setColor(getWallColor(cell->walls[South]));
		ArdUI::LCD->drawLine(xS, yE, xE, yE);

		// West Wall
		// if(xSlot == 0){
			// ArdUI::LCD->setColor(0, 0, 255);
			ArdUI::LCD->setColor(getWallColor(cell->walls[West]));
			ArdUI::LCD->drawLine(xS, yS, xS, yE);
		// }

		// East Wall
		// ArdUI::LCD->setColor(0, 255, 255);
		ArdUI::LCD->setColor(getWallColor(cell->walls[East]));
		ArdUI::LCD->drawLine(xE, yS, xE, yE);


		if(renderRobot){
			Point p1 = Point(xE - (xE-xS)/2, yS - (yS-yE)/2);
			Point p2 = p1;
			
			if(poseEstimator->direction == North)
				p2.y -= 20;
			else if(poseEstimator->direction == East)
				p2.x += 20;
			else if(poseEstimator->direction == South)
				p2.y += 20;
			else if(poseEstimator->direction == West)
				p2.x -= 20;

			ArdUI::LCD->setColor(pCellBlack);
			ArdUI::LCD->drawLine(
				p1.x,
				p1.y,
				p2.x,
				p2.y);
		}

	}

	void render(bool forceRender = false){

		if(!poseEstimator->currentMap)
			return;

		long _x1 = x1();
		long _x2 = x2();
		long _y1 = y1();
		long _y2 = y2();

		long xCell = 0, yCell = 0;
		Cell *robotCell = poseEstimator->getCurrentCell();

		for(int x = xOffset; x < xOffset + xSpan; x++){
			yCell = 0;
			for(int y = yOffset; y < yOffset + ySpan; y++){
				Cell *cell_tmp = poseEstimator->currentMap->getCell(x, y);
				if(robotCell->x == cell_tmp->x && robotCell->y == cell_tmp->y)
					renderCell(cell_tmp, _x1, _y1, xCell, yCell, true, forceRender);
				else
					renderCell(cell_tmp, _x1, _y1, xCell, yCell, false, forceRender);

				
				// ArdUI::LCD->setColor(pText);
				// 
				yCell++;
			}

			xCell++;
		}

	}

};


#endif