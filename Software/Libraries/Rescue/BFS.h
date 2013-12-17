#ifndef BSF_h
#define BSF_h
#include "Search.h"
#include "LinkedList.h"
#include "Map.h"

class BFS: public Search
{
protected:

	LinkedList<Cell*> *list;
public:
	bool compare(Cell* c1, Cell* c2){
		return (c1->x == c2->x && c1->y == c2->y);
	}

	BFS(){
		list = new LinkedList<Cell*>();
	}

	LinkedList<Cell*> *search(Map* map, Cell* start, Cell* target){
		list->clear();

		if (start == target)
			return list;

		bool visited[22][22] = { { 0 } };

		Cell *currCell;
		list->add(target);
		visited[target->x + 10][target->y + 10] = true;
		while (list->size() > 0)
		{
			currCell = list->get(0);

			Serial.print(currCell->x);
			Serial.print("\t");
			Serial.println(currCell->y);
			if (compare(currCell, start)) {
				Serial.println("start == currCell");
				break;
			}
			else {
				Serial.println("not yet");
			}

			if (!currCell->getWall(North)->exist()){
				Cell *nextCell = map->getRelativeCell(currCell, North);
				Serial.print(nextCell->x); Serial.print("  "); Serial.print(nextCell->y); Serial.print(" : ");
				Serial.print(nextCell->reachable()); Serial.print("  "); Serial.println(visited[nextCell->x + 10][nextCell->y + 10]);
				if(nextCell->reachable() && !visited[nextCell->x + 10][nextCell->y + 10]){
					list->add(nextCell);
					nextCell->parent = currCell;
					visited[nextCell->x + 10][nextCell->y + 10] = true;
				}
			}
			else {
				Serial.println("north wall exists");
			}

			if (!currCell->getWall(West)->exist()){
				Cell *nextCell = map->getRelativeCell(currCell, West);
				Serial.print(nextCell->x); Serial.print("  "); Serial.print(nextCell->y); Serial.print(" : ");
				Serial.print(nextCell->reachable()); Serial.print("  "); Serial.println(visited[nextCell->x + 10][nextCell->y + 10]);
				if(nextCell->reachable() && !visited[nextCell->x + 10][nextCell->y + 10]){
					list->add(nextCell);
					nextCell->parent = currCell;
					visited[nextCell->x + 10][nextCell->y + 10] = true;
				}
			}
			else {
				Serial.println("west wall exists");
			}

			if (!currCell->getWall(South)->exist()){
				Cell *nextCell = map->getRelativeCell(currCell, South);
				Serial.print(nextCell->x); Serial.print("  "); Serial.print(nextCell->y); Serial.print(" : ");
				Serial.print(nextCell->reachable()); Serial.print("  "); Serial.println(visited[nextCell->x + 10][nextCell->y + 10]);
				if(nextCell->reachable() && !visited[nextCell->x + 10][nextCell->y + 10]){
					list->add(nextCell);
					nextCell->parent = currCell;
					visited[nextCell->x + 10][nextCell->y + 10] = true;
				}
			}
			else {
				Serial.println("south wall exists");
			}

			if (!currCell->getWall(East)->exist()){
				Cell *nextCell = map->getRelativeCell(currCell, East);
				Serial.print(nextCell->x); Serial.print("  "); Serial.print(nextCell->y); Serial.print(" : ");
				Serial.print(nextCell->reachable()); Serial.print("  "); Serial.println(visited[nextCell->x + 10][nextCell->y + 10]);
				if(nextCell->reachable() && !visited[nextCell->x + 10][nextCell->y + 10]){
					list->add(nextCell);
					nextCell->parent = currCell;
					visited[nextCell->x + 10][nextCell->y + 10] = true;
				}
			}
			else {
				Serial.println("east wall exists");
			}

			list->remove(0);
		}

		Serial.println("PATH:");

		list->clear();
		currCell = start;
		do {
			list->add(currCell);
			Serial.print(currCell->x); Serial.print("\t"); Serial.println(currCell->y);
			currCell = currCell->parent;
		} while (currCell != target);
		list->add(currCell);
		Serial.print(currCell->x); Serial.print("\t"); Serial.println(currCell->y);

		return list;
	}
};


#endif