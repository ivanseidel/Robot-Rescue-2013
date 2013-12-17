#ifndef Map_h
#define Map_h

class Cell;
// #include "Cell.h"

#define X_SIZE 25
#define Y_SIZE 25

class Map
{
protected:
	Cell *grid[X_SIZE][Y_SIZE];

public:

	long xOffset, yOffset, xSize, ySize;

	Map(int _xOffset = 0, int _yOffset = 0){
		xSize = X_SIZE;
		ySize = Y_SIZE;
		initialize(_xOffset, _yOffset);
	}

	void initialize(int _xOffset, int _yOffset){
		xOffset = _xOffset;
		yOffset = _yOffset;
		
		for(int x = 0; x < xSize; x++){
			for(int y = 0; y < ySize; y++){
				Cell *c = new Cell(this, x - xOffset, y - yOffset);
				
				c->walls[North] = new Wall();
				c->walls[East]  = new Wall();
				
				if(y == 0){
					c->walls[South] = new Wall();
					
				}else{
					c->walls[South] = grid[x][y-1]->walls[North];
				}
				
				if(x == 0){
					c->walls[West] = new Wall();
				}else{
					c->walls[West] = grid[x-1][y]->walls[East];
				}
							
				grid[x][y] = c;
			}	
		}
	}

	Cell *getCell(long x, long y){
		x += xOffset;
		y += yOffset;
		
		if (x >= xSize || x < 0)
			return 0;
		if (y >= ySize || y < 0)
			return 0;

		return grid[x][y];
	}

	Cell *getRelativeCell(Cell* cell, Direction dir, int number = 1)
	{
		int dx = 0;
		int dy = 0;

		if (dir == North)
			dy = 1 * number;
		
		else if (dir == East)
			dx = 1 * number;

		else if (dir == West)
			dx = -1 * number;

		else if (dir == South)
			dy = -1 * number;

		return getCell(cell->x + dx, cell->y + dy);
	}


};

#endif