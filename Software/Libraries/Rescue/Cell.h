#ifndef Cell_h
#define Cell_h

// #include "Rescue.h"

#include "Wall.h"
class Map;

class Cell
{
protected:

public:

	int x, y;

	bool isVisited;
	bool hasVictim;
	bool isBlack;
	bool isRamp;

	Cell *parent;

	// Used for rendering only
	uint16_t lastColor;

	Map  *map;
	Wall *walls[4];

	Cell(Map *_map, long _x, long _y){
		x = _x;
		y = _y;

		isVisited = false;
		hasVictim = false;
		isBlack = false;
		isRamp = false;

		lastColor = 0;

		map = _map;
	}

	bool reachable(){
		return (!isBlack && !(walls[0]->exist() && walls[1]->exist() && walls[2]->exist() && walls[3]->exist()));
	}

	Wall *getWall(Direction dir){
		return walls[dir];
	}

	Wall *getRelativeWall(Direction d1, Direction d2){
		return walls[((d1 + d2) % 4)];
	}

};

#endif