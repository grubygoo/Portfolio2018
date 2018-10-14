#include "Core.h"
#include "../include/Util/Timer.h"
#include "Processor/UserProcessor.h"
#include "Processor/NPCProcessor.h"
#include "Util\ErrorUtil.h"
#include "Util/VectorData.h"
#include "Util/MapData.h"

void CCore::InitNetwork()
{
	wcout.imbue(locale("Korean"));
	/*Window Startup*/
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);
	/*Create IOCP HANDLE*/
	HIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	/*Create Socket*/
	ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	/*Bind N Listen*/
	SOCKADDR_IN BindAddr;
	ZeroMemory(&BindAddr, sizeof(SOCKADDR_IN));
	BindAddr.sin_family = AF_INET;
	BindAddr.sin_port = htons(MY_SERVER_PORT);
	BindAddr.sin_addr.s_addr = INADDR_ANY;
	::bind(ListenSock, reinterpret_cast<sockaddr*>(&BindAddr), sizeof(SOCKADDR));
	::listen(ListenSock, SOMAXCONN);
}

CCore::CCore():USession(nullptr), NSession(nullptr),Timer(nullptr)
{
	InitNetwork();
	USession = new CUserProcessor();
	NSession = new CNPCProcessor();
	Timer = new CGameTimer();
}

CCore::~CCore()
{
	SAFE_DELETE(USession);
	SAFE_DELETE(NSession);
	SAFE_DELETE(Timer);

	closesocket(ListenSock);
	CloseHandle(HIOCP);
	WSACleanup();
	
	CVData::DESTROY_SINGLE();
	CMapData::GET_SINGLE()->Close();
	CMapData::DESTROY_SINGLE();
}

void CCore::Initialize()
{   
	if (!CVData::GET_SINGLE()->Initialize()) printf("error VectorData\n");
	if (!CMapData::GET_SINGLE()->Initialize()) printf("error MapData\n");
	if (!USession->Initialize()) std::cout << "Processor Initialize Failed..." << std::endl;
	if (!NSession->Initialize()) std::cout << "NPC Manager Initialize Failed..." << std::endl;
}

void CCore::Run()
{
	std::vector<thread> WorkerThread;
	std::thread AcceptThread;
	std::thread EventThread;
	std::thread SimulationThread;

	std::cout << "WORKER THREAD  VECTOR SIZE:: ";
	std::cin >> WorkerSize;

	AcceptThread = thread([this] { this->CoreAcceptThread(); });
	for (int i = 0; i < WorkerSize; ++i) WorkerThread.push_back(std::thread([this] { this->CoreWorkerThread();  }));
	EventThread = thread([this] { this->CoreEventThread(); });
	SimulationThread = thread([this] { this->CoreSimulationThread(); });

	for (auto& th : WorkerThread) th.join();
	AcceptThread.join();
	EventThread.join();
	SimulationThread.join();
}
void CCore::CoreAcceptThread()
{
	printf("Start Accept Thread\n");
	while (true)
	{
		SOCKADDR_IN CAddr;
		ZeroMemory(&CAddr, sizeof(SOCKADDR_IN));
		int AddrSize = sizeof(SOCKADDR_IN);
		auto CSock = WSAAccept(ListenSock, reinterpret_cast<sockaddr*>(&CAddr), &AddrSize, NULL, NULL);
		if (INVALID_SOCKET == CSock) { ErrorUtil::Err_Quit("SocketCreateError!\n"); continue; }
		if (!USession->UserSetting(HIOCP, CSock))
		{
			printf("Client Pool Overflow\n");
			closesocket(CSock);
			continue;
		}
	}

}

void CCore::CoreWorkerThread()
{
	unsigned long                  IO_Size;
	unsigned long long             IO_Key;
	WORD                           Key;
	bool                           Ret;
	WSAOVERLAPPED*                 Over;

	printf("Start Worker Thread\n");
	while (true)
	{
		Ret = GetQueuedCompletionStatus(HIOCP, &IO_Size, (&IO_Key), &Over, INFINITE);
		Key = static_cast<WORD>(IO_Key);

		if (false == Ret || IO_Size == 0)
		{
			ErrorUtil::Err_Display("ERROR IN GQCS");
			USession->PlayerDisconnect(Key);
			continue;
		}

		/*Send / Recv Ã³¸®*/
		EXOVER* Exover = reinterpret_cast<EXOVER*>(Over);
		switch (Exover->IO_Type)
		{
		case IO_TYPE::IO_RECV:{
			USession->Process(Key, static_cast<int>(IO_Size), Exover->IO_Buf);
			break;
		}
		case IO_TYPE::IO_SEND:{
			delete Exover;
			break;
		}
		case IO_TYPE::IO_EVENT: {    
			if (IsNPC(Key)) NSession->Process(Exover->EV_Type, Key); 
			else USession->EventProcess(Exover->EV_Type, Key); 

			delete Exover;
			break;
		}
		}
	}
}

void CCore::CoreEventThread()
{
	printf("Start Event Thread\n");
	NPC_EVENT E;
	while (true)
	{
		Sleep(10);
		while (false == NSession->IsQueueEmpty())
		{
			if (NSession->TryPopOfQueue(E))
			{
				while (E.W_Time > chrono::high_resolution_clock::now());
				EXOVER* Ex = new EXOVER;
				Ex->EV_Type = E.Type;
				Ex->IO_Type = IO_TYPE::IO_EVENT;
				PostQueuedCompletionStatus(HIOCP, 1, E.NPC_ID, &Ex->WSAOVER);
			}
		}
	}
}

void CCore::CoreSimulationThread()
{   
	printf("Start Simulation Thread\n");
	if (!Timer) return;
	Timer->Start();
	float fDeltaTime = 0.f;
	while (true)
	{
		Timer->Tick(0.f);
		fDeltaTime = Timer->GetTimeElapsed();
		USession->Simulation(fDeltaTime);
		NSession->Simulation(fDeltaTime);
	}
}

