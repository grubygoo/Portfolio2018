#pragma once
#include <tbb/concurrent_priority_queue.h>
#include <tbb/concurrent_hash_map.h>
#include "../Include.h"
#include "CUserPool.h"
#include "CSector.h"

class CECompare
{
public:
	CECompare() {}
	bool operator() (const NPC_EVENT& L, const NPC_EVENT& R) const {
		return (L.W_Time > R.W_Time);
	}
};

class CProcessor
{
public:
	CProcessor();
	virtual ~CProcessor();
	bool  IsQueueEmpty() { return eventQueue.empty(); }
	bool  TryPopOfQueue(NPC_EVENT& t) { return eventQueue.try_pop(t); }


protected:
	friend class CSector;
	void  AddEvent(unsigned int id, EV_TYPE type, const high_resolution_clock::time_point& time) 
	{ eventQueue.push(NPC_EVENT{ time,type,id }); }

	void  SearchNearSector(Vec2i& pos, std::unordered_set<CSector*>& nearlist);
	virtual void EventProcess(EV_TYPE type, unsigned int id) = 0;
	virtual bool Initialize() = 0;

	static CUserPool* userPool;
	static std::vector<class CNPC*> npcPool;
	static std::vector<unsigned int> traverseVector;
	static tbb::queuing_rw_mutex traverseLock;
	static tbb::concurrent_priority_queue<NPC_EVENT, CECompare> eventQueue;
	static std::vector<std::vector<CSector*>> worldSector;

};


