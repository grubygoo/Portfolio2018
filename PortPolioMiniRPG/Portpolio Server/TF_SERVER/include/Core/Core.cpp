#include "Core.h"
#include "../Processor/UserProcessor.h"
#include "../Processor/NPCProcessor.h"
#include "../DB/DB.h"
#include "../Error/ErrorUtil.h"

CCore::CCore()
{
	InitNetwork();
	UserSession = new CUserProcessor();
	NPCSession = new CNPCProcessor();
}

void CCore::InitNetwork()
{
	wcout.imbue(locale("Korean"));
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);
	HIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN BindAddr;
	ZeroMemory(&BindAddr, sizeof(SOCKADDR_IN));

	BindAddr.sin_family = AF_INET;
	BindAddr.sin_port = htons(MY_SERVER_PORT);
	BindAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(ListenSock, reinterpret_cast<sockaddr*>(&BindAddr), sizeof(SOCKADDR));
	::listen(ListenSock, SOMAXCONN);
}

CCore::~CCore()
{
	closesocket(ListenSock);
	CloseHandle(HIOCP);
	WSACleanup();
	SAFE_DELETE(UserSession);
	SAFE_DELETE(NPCSession);
} 

void CCore::Initialize()
{
	if (!UserSession->Initialize()) return;
	if (!NPCSession->Initialize())  return;
	//if (!CDB::GET()->ConnectDB()) return;
}

void CCore::Run()
{
	vector<thread> WorkerThread;
	thread AcceptThread;
	thread EventThread;
	cout << "WORKER THREAD  POOL SIZE:: ";
	cin >> WorkerSize;

	AcceptThread = thread([this] { this->CoreAcceptThread();});
	for (int i = 0; i < WorkerSize; ++i) WorkerThread.push_back(std::thread([this] { this->CoreWorkerThread();  }));
	EventThread = thread([this] { this->CoreEventThread();});

	AcceptThread.join();
	EventThread.join();
	for (auto& th : WorkerThread) th.join();
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
		if (INVALID_SOCKET == CSock) { ErrorUtil::Err_Quit("Accept Thread!\n"); continue; }

		if (!UserSession->UserSetting(HIOCP,CSock))
		{   
			printf("Client Pool Overflow\n");
			closesocket(CSock);
			continue; 
		}
	}
}

void CCore::CoreWorkerThread()
{
	unsigned long       IO_Size;
	unsigned long long  IO_Key;
	unsigned int         Key;
	bool                 Ret;
	WSAOVERLAPPED*      Over;

	printf("Start Worker Thread\n");
	while (true)
	{
		//printf("1\n");
		Ret = GetQueuedCompletionStatus(HIOCP, &IO_Size, (&IO_Key), &Over, INFINITE);
		Key = static_cast<unsigned int>(IO_Key);
		if (false == Ret || IO_Size == 0)
		{
			ErrorUtil::Err_Display("ERROR IN GQCS\n");
			UserSession->PlayerDisconnect(Key);
			continue;
		}
		EXOVER* Exover = reinterpret_cast<EXOVER*>(Over);
		switch (Exover->IO_Type)
		{
		case IO_TYPE::IO_RECV:
		{
			UserSession->PacketProcess(Key, static_cast<int>(IO_Size), Exover->IO_Buf);
			break;
		}
		case IO_TYPE::IO_SEND:
		{
			delete Exover;
			break;
		}
		case IO_TYPE::IO_EVENT:
		{
			if (IsNPC(Key)) {
		     //	printf("???\n");
				NPCSession->EventProcess(Exover->EV_Type, Key); }
			else UserSession->EventProcess(Exover->EV_Type, Key);
			delete Exover;
			break;
		}
		default: break;
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
		while (false == NPCSession->IsQueueEmpty())
		{
			
			if (NPCSession->TryPopOfQueue(E))
			{   
				//printf("%d\n", E.NPC_ID);
				while (E.W_Time > chrono::high_resolution_clock::now());
				EXOVER* Ex = new EXOVER;
				Ex->EV_Type = E.Type;
				Ex->IO_Type = IO_TYPE::IO_EVENT;
				PostQueuedCompletionStatus(HIOCP, 
					1,E.NPC_ID, &Ex->WSAOVER);
			}
		}
	}
}


