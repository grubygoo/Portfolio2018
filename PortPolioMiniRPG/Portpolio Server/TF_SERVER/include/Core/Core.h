#pragma once
#include <thread>
#include "../Include.h"
using namespace std;
class CCore
{
public:
	CCore();
	~CCore();
	void Initialize();
	void Run();

private:
	void InitNetwork();
	void CoreWorkerThread();
	void CoreEventThread();
	void CoreAcceptThread();
	HANDLE        HIOCP;
	SOCKET        ListenSock;
	SOCKADDR_IN   BindAddr;
	int           WorkerSize;
	class CNPCProcessor*  NPCSession;
	class CUserProcessor* UserSession;
	class CGameTimer*     GameTimer;
};


