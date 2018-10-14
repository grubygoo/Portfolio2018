#pragma once
#include <tbb/concurrent_priority_queue.h>
#include "../Include.h"
#include "UserPool.h"
class CTCompare
{
public:
	CTCompare() {}
	bool operator() (const NPC_EVENT& L, const NPC_EVENT& R) const {
		return (L.W_Time > R.W_Time);
	}
};

class CProcessor
{
public:
	CProcessor();
	virtual ~CProcessor();
	bool  IsQueueEmpty() { return EventQueue.empty(); }
	bool  TryPopOfQueue(NPC_EVENT& t) { return EventQueue.try_pop(t); }
	virtual void Simulation(float fDeltaTime) = 0; 
	
	inline void PushTraverseVec(WORD id) {
		tbb::queuing_rw_mutex::scoped_lock(Tlm, true);
		TraverseVec.push_back(id);
	}
	inline void EraseTraverseVec(WORD id)
	{
		tbb::queuing_rw_mutex::scoped_lock(Tlm, true);
		auto It = std::find(TraverseVec.begin(), TraverseVec.end(), id);
		if (It != TraverseVec.end()) TraverseVec.erase(It);
	}

	inline void CopyTraverseVec(std::vector<WORD>& vec)
	{
		tbb::queuing_rw_mutex::scoped_lock(Tlm, false);
		vec = TraverseVec;
	
	}

protected: 
	inline void  AddEvent(WORD id, EV_TYPE type, const high_resolution_clock::time_point& time){EventQueue.push(NPC_EVENT{ time,type,id });}
	static CUserPool* UserPool;
	static std::vector<class CNPC*> NPCPool;
	static std::vector<WORD> TraverseVec;
	static tbb::queuing_rw_mutex Tlm;
	static tbb::concurrent_priority_queue<NPC_EVENT,CTCompare> EventQueue;
};

