#pragma once
#include "CProcessor.h"
class CUserProcessor :public CProcessor
{
	typedef void(CUserProcessor::*functionPointer)(unsigned int , char* packet);
	typedef void(CUserProcessor::*EventfuntionPointer)(unsigned int);
public:
	CUserProcessor();
	virtual ~CUserProcessor();
    virtual bool Initialize();
	virtual void EventProcess(EV_TYPE type, unsigned int id) { (this->*eventProcessor[type])(id); }
	void PacketProcess(unsigned int id, int work_size, char* io_ptr);
	bool  UserSetting(HANDLE hIocp, SOCKET sock);
	void  PlayerDisconnect(unsigned int id);

private:
	void  PlayerLogin(unsigned int id, char* packet);
	void  PlayerLogOut(unsigned int id, char* packet);
	void  PlayerPutIn(unsigned int id);
	void  PlayerMove(unsigned int  id, char* packet);
	void  PlayerAttack(unsigned int id, char* packet);
	void  PlayerRespawn(unsigned int id);
	bool  AWakeNPC(unsigned int id);
	void  UpdateViewList(unsigned int id);

	std::unordered_map<char, functionPointer>  processor;
	std::unordered_map<char, EventfuntionPointer>  eventProcessor;
};

