#include "CSector.h"
#include "CProcessor.h"
#include "../Object/NPC.h"
#include "../Object/User.h"



CSector::CSector()
{
	sectorSet.clear();
}

CSector::~CSector()
{
}

void CSector::AddList(unsigned int id)
{
	sectorLock.lock();
	sectorSet.insert(id);
	sectorLock.unlock();
}

void CSector::DeleteList(unsigned int id)
{
	sectorLock.lock();
	sectorSet.erase(id);
	sectorLock.unlock();
}

/*이거 friend 써서 바꿀 수도 있을 것 같음*/
void CSector::TraverseList(const Vec2i& curpos, std::unordered_set<unsigned int>& set)
{
	sectorLock.lock();
	for (auto ID : sectorSet)
	{
		if (IsNPC(ID))
		{
			if (!CProcessor::npcPool[ID]->GetAlive()) continue;
			if (!InMySight(CProcessor::npcPool[ID]->GetPos(),curpos, VIEW_RADIUS)) continue;
		}
		else
		{
			if (!(*CProcessor::userPool)[ID].GetConnected()) continue;
			if (!(*CProcessor::userPool)[ID].GetAlive()) continue;
			if (!InMySight((*CProcessor::userPool)[ID].GetPos(), curpos, VIEW_RADIUS)) continue;
		}
		set.insert(ID);
	}
	sectorLock.unlock();
}