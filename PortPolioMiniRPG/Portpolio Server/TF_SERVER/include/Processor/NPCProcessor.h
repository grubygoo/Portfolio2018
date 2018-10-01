#pragma once
#include "CProcessor.h"

using namespace std;

class CNPCProcessor :public CProcessor
{   
	typedef void(CNPCProcessor::*functionPointer)(unsigned int);
public:
	CNPCProcessor();
	virtual ~CNPCProcessor();
	virtual void EventProcess(EV_TYPE type, unsigned int id) { (this->*Processor[type])(id); }
	virtual bool Initialize();

private:
	void IdleNPC(unsigned int id);
	void MoveNPC(unsigned int id);
	void AttackNPC(unsigned int id);
	void RespawnNPC(unsigned int id);
	void ProcessState(unsigned int id);
	unordered_map<BYTE, functionPointer>  Processor;
};

