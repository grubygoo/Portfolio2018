#include "Sector.h"
#include "CProcessor.h"
#include "../Object/User.h"
#include "../Object/NPC.h"

CSector::CSector()
{
	SectorSet.clear();
}

CSector::~CSector()
{
	tbb::queuing_rw_mutex::scoped_lock lock(SectorLock, true);
	SectorSet.clear();
}

void CSector::AddList(WORD id)
{
	tbb::queuing_rw_mutex::scoped_lock lock(SectorLock, true);
	SectorSet.insert(id);
}

void CSector::DeleteList(WORD id)
{
	tbb::queuing_rw_mutex::scoped_lock lock(SectorLock, true);
	SectorSet.insert(id);
}
void CSector::TraverseList(const XMFLOAT3& pos, std::unordered_set<WORD>& set)
{
	tbb::queuing_rw_mutex::scoped_lock lock(SectorLock, false);
	for (auto ID : SectorSet)
	{
		if (IsNPC(ID))
		{
			if (!CProcessor::NPCPool[ID]->GetAlive()) continue;
			if (!CProcessor::NPCPool[ID]->GetAwake()) continue;
			if (!Vector3::InMySight(CProcessor::NPCPool[ID]->GetPos(), pos, VIEW_RADIUS)) continue;
		}
		else
		{
			if (!CProcessor::UserPool->GetUser(ID).GetAlive()) continue;
			if (!CProcessor::UserPool->GetUser(ID).GetConnected()) continue;
			if (!Vector3::InMySight(CProcessor::UserPool->GetUser(ID).GetPos(), pos, VIEW_RADIUS)) continue;
		}
		set.insert(ID);
	}
}
