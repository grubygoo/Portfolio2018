#pragma once
#include "CProcessor.h"
#include "../Include.h"

class CNPCProcessor :public CProcessor
{
	typedef void(CNPCProcessor::*functionPointer)(WORD);
public:
	CNPCProcessor();
	virtual ~CNPCProcessor();
	bool Initialize();
	void Process(EV_TYPE type, WORD id) { (this->*Processor[type])(id); }
	virtual void Simulation(float fDeltaTime);

private:
	void IdleNPC(WORD id);
	void MoveNPC(WORD id);
	void AttackNPC(WORD id);
	void GetNPCViewList(std::unordered_set<WORD>& vl,CNPC& npc);
	std::unordered_map<char, functionPointer>  Processor;
};

