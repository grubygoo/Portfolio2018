#include "Astar.h"
using namespace std::placeholders;

Astar::Astar() : numOfdir(8)
{
	SetHeuristic(&Heuristic::manhattan);
	worldSize.x = BOARD_WIDTH;
	worldSize.y = BOARD_HEIGHT;
	direction = { { 0, 1 },{ 1, 0 },{ 0, -1 },{ -1, 0 },
				{ -1, -1 },{ 1, 1 },{ -1, 1 },{ 1, -1 } };
}

Astar::~Astar()
{
}

const Vec2i& Astar::FindPath(const Vec2i& source, const Vec2i&  target)
{
	NODE *current = &NODE(source);
	NODE NextNode;

	/*인접 8개에대해서 검사*/
	for (unsigned int i = 0; i < numOfdir; ++i) 
	{
		Vec2i newCoordinates(current->pos + direction[i]);
		if (DetectCollision(newCoordinates)){
			continue;
		}
		unsigned int totalCost = current->G + ((i < 4) ? 10 : 14);
		NODE successor =  NODE(newCoordinates);
		successor.G = totalCost;
		successor.H = heuristic(successor.pos, target);  
		if (i == 0) {
			NextNode = successor;
			continue;
		}
		if (successor.GetScore() < NextNode.GetScore()) {
			NextNode = successor;
		}
	}
	return NextNode.pos;
}

void Astar::SetHeuristic(HeuristicFunction heuristic_)
{
	heuristic = std::bind(heuristic_, _1, _2);
}

bool Astar::DetectCollision(const Vec2i& pos)
{
	if (pos.x < 0 || pos.x >= worldSize.x ||
		pos.y < 0 || pos.y >= worldSize.y)
	{
		return true;
	}
	return false;
}

