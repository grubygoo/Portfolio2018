#include "CProcessor.h"
#include "SendManager.h"
#include "../Object/NPC.h"
CUserPool*  CProcessor::userPool = nullptr;
std::vector<class CNPC*> CProcessor::npcPool;

std::vector<unsigned int> CProcessor::traverseVector;
tbb::queuing_rw_mutex CProcessor::traverseLock;

tbb::concurrent_priority_queue<NPC_EVENT, CECompare> CProcessor::eventQueue;
std::vector<std::vector<CSector*>> CProcessor::worldSector; //2차원 벡터를 이용해 접근한다.

CProcessor::CProcessor()
{
	npcPool.clear();
	eventQueue.clear();
	traverseVector.clear();
}
CProcessor::~CProcessor()
{
}

void CProcessor::SearchNearSector(Vec2i & pos, std::unordered_set<CSector*>& nearlist)
{
	nearlist.insert(worldSector[pos.y / SECTOR_SIZE][pos.x / SECTOR_SIZE]);
	//주변 4점을 검사하도록한다. 
	Vec2i NearPos;
	for (int i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0: NearPos.x = pos.x - VIEW_RADIUS; NearPos.y = pos.y + VIEW_RADIUS; break;
		case 1: NearPos.x = pos.x + VIEW_RADIUS; NearPos.y = pos.y + VIEW_RADIUS; break;
		case 2: NearPos.x = pos.x - VIEW_RADIUS; NearPos.y = pos.y - VIEW_RADIUS; break;
		case 3: NearPos.x = pos.x + VIEW_RADIUS; NearPos.y = pos.y - VIEW_RADIUS; break;
		}
		if (NearPos.x < 0) NearPos.x = 0;
		if (NearPos.y < 0) NearPos.y = 0;
		if (NearPos.x > BOARD_WIDTH - 1)  NearPos.x = BOARD_WIDTH;
		if (NearPos.y > BOARD_HEIGHT - 1) NearPos.y = BOARD_HEIGHT;

		nearlist.insert(worldSector[NearPos.y / SECTOR_SIZE][NearPos.x / SECTOR_SIZE]);
	}
}

