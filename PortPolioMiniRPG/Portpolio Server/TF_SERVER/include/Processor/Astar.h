#pragma once
#include "../Include.h"
#include "../SingleTone.h"
#include <algorithm>
#include <functional>
#include <set>

struct NODE
{
	unsigned int G, H;
	Vec2i pos;
	NODE()
	{  
		pos.x = 0;
		pos.y = 0;
		G = H = 999;
	}
	NODE(Vec2i p){
		pos = p;
		G = H = 0;
	}
	unsigned int GetScore(){
		return G + H;
	}
};


class Astar : public CSingleTone<Astar>
{
	using HeuristicFunction = std::function<unsigned int(const Vec2i&, const Vec2i&)>;

public:
	Astar();
	~Astar();
	const Vec2i& FindPath( const Vec2i& source, const Vec2i& target);
	void SetHeuristic(HeuristicFunction heuristic);


private:
	bool DetectCollision(const Vec2i& pos);
	std::vector<Vec2i> direction;
	HeuristicFunction heuristic;
	Vec2i worldSize; 
	unsigned int numOfdir;
};

class Heuristic
{
	static Vec2i GetDelta(const Vec2i& source,const Vec2i& target)
	{
		return{ abs(source.x - target.x),  abs(source.y - target.y) };
	}

public:
	static unsigned int  manhattan(const Vec2i& source_, const Vec2i& target_)
	{
		auto delta = std::move(GetDelta(source_, target_));
		return static_cast<unsigned int>(10 * (delta.x + delta.y));
	}

	static unsigned int euclidean(const Vec2i& source_, const Vec2i& target_)
	{
		auto delta = std::move(GetDelta(source_, target_));
		return static_cast<unsigned int>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
	}
};

