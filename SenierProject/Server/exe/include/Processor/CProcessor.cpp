#include "CProcessor.h"
#include "SendManager.h"
#include "../Object/NPC.h"

CUserPool* CProcessor::UserPool;
std::vector<CNPC*> CProcessor::NPCPool;
tbb::concurrent_priority_queue<NPC_EVENT, CTCompare> CProcessor::EventQueue;
std::vector<WORD> CProcessor::TraverseVec;
tbb::queuing_rw_mutex CProcessor::Tlm;

CProcessor::CProcessor()
{   
	NPCPool.clear();
	EventQueue.clear();
	TraverseVec.clear();
}
CProcessor::~CProcessor()
{
}