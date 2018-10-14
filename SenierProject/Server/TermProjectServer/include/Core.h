#pragma once
#include "../include/Include.h"
#include <thread>
class CCore 
{
public:
	CCore();
	~CCore();
	void Initialize();
	void Run();
private:
	HANDLE                HIOCP;
	SOCKET                ListenSock;
	SOCKADDR_IN           BindAddr;
	unsigned int          WorkerSize;
	class CNPCProcessor*  NSession;
	class CUserProcessor* USession;
	class CGameTimer*     Timer;

	void      InitNetwork();
	void      CoreWorkerThread();
	void      CoreEventThread();
	void      CoreAcceptThread();
	void      CoreSimulationThread();
};


