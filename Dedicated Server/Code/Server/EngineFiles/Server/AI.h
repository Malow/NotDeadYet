//Made by Berg Wilhelm 15/12/12 created) for project Not Dead Yet at Blekinge tekniska h�gskola



#ifndef AI_H
#define AI_H
#include <bitset>
#include <list>
#include <vector>
#include "../../../../../External/NDYGFX/Include/Vector.h"
//#include <map>


struct Node
{
	int x, y;
	int fCost;
	int gCost;
	Node* parent;
	Node(){this->x = -1; this->y = -1; this->parent = NULL; fCost = 99999;};
	Node(int x, int y){this->x = x; this->y = y; this->parent = NULL; fCost = 99999;};
};

const int GRIDSIZE = 20;

class AI
{
private:
			
	std::string		zName;
	int				zNodeDistance; //The distance between nodes.

	//Node			zNodes[GRIDSIZE*GRIDSIZE];	//I made this to a list instead.
	std::list<Node> zNodeList;

	std::bitset<GRIDSIZE*GRIDSIZE>			zGrid;
	std::list<Node*>						zOpenList;
	std::list<Node*>						zClosedList;

public:
					AI();
	virtual			~AI();
					
					//The node distance needs to be the same as the distance between nodes in the grid as when they were created.
	void			InitAI(std::bitset<GRIDSIZE*GRIDSIZE> theGrid, int nodeDistance);

					//The vector reference will be filled with the path between the start postion and the target. Don't forget to empty the vector if you want to use it again, or it will be filled with an entire extra path.
					//If the goalposition is corresponding to a blocked bit in the bitset, it will return false.
	bool			Pathfinding(float startXPos, float startYPos, float goalXPos, float goalYPos, std::vector<Vector2> &inVector, int maximumPathLenght = 300);



};
#endif