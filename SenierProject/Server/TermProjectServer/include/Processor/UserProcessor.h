#pragma once
#include "CProcessor.h"
class CUserProcessor :public CProcessor
{
	typedef void(CUserProcessor::*functionPointer)(WORD, char* packet);
	typedef void(CUserProcessor::*EventfuntionPointer)(WORD);

public:
	CUserProcessor();
	virtual ~CUserProcessor();
	virtual void  Simulation(float fDeltaTime);
    bool Initialize();
	void Process(WORD id, int work_size, char* io_ptr);
	void EventProcess(EV_TYPE type, WORD id) { (this->*EventProcessor[type])(id);}
	bool UserSetting(HANDLE hIocp, SOCKET sc);
	void PlayerDisconnect(WORD id);


private:
	void  AWakeNPC(WORD id, WORD target);
	void  PlayerPutIn(WORD id);
	void  PlayerMove(WORD  id, char* packet);
	void  PlayerAttack(WORD id, char* packet);
	void  PlayerCollision(WORD id, char* packet);
	void  PlayerChanageCharacter(WORD id, char* packet);
	void  PlayerTelePort(WORD id, char* packet);
	void  PlayerSkill(WORD id, char* packet);
	void  PlayerRespawn(WORD id);
	void  WorriorSkill(WORD id);
	void  UpdateViewList(WORD id);
	std::unordered_map<char, functionPointer>  Processor;
	std::unordered_map<char, EventfuntionPointer>  EventProcessor;
};

