#ifndef Search_h
#define Search_h

#include "LinkedList.h"
#include "Rescue.h"
#include "Map.h"

class Search
{
public:
	virtual LinkedList<Cell*> *search(Map* map, Cell* start, Cell* target){};
};

#endif