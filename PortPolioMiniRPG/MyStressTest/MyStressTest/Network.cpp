#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>
#include <unordered_map>
#include "Network.h"
#include "protocol.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace chrono;
const static int MAX_TEST = MAX_USER;
HANDLE g_hiocp;
bool a = true;
const static int INVALID_ID = -1;
enum OPTYPE { OP_SEND, OP_RECV };
high_resolution_clock::time_point last_connect_time;
struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT {
	unsigned int id;
	unsigned int idx;
	int x;
	int y;
	unsigned int  mp;
	unsigned int lev;
	unsigned int exp;
	unsigned int atk;
	unsigned int  hp;
	high_resolution_clock::time_point last_move_time;
	bool connect;
	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
};
struct NPC
{
	unsigned int id;
	unsigned int index;
	int x;
	int y;
	bool awake;
};

array<CLIENT, MAX_USER> g_clients;
array<NPC, NUM_OF_NPC> g_npcs;
atomic_int num_connections;

vector <thread *> worker_threads;
thread test_thread;
void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1])
	{
	case SC_LOG_SUCC:
	{
		sc_packet_logsucc* Packet = (sc_packet_logsucc*)packet;
		unsigned int idx = Packet->index;
		g_clients[idx].hp = Packet->hp;
		g_clients[idx].exp = Packet->exp;
		g_clients[idx].mp = Packet->mp;
		g_clients[idx].lev = Packet->lev;
		g_clients[idx].idx = Packet->index;
		g_clients[idx].id = Packet->id;
		g_clients[idx].x = Packet->x;
		g_clients[idx].y = Packet->y;
		g_clients[idx].connect = true;
		break;
	}
	case SC_PUT_PLAYER: {
		//sc_packet_put_player * PutPacket = reinterpret_cast<sc_packet_put_player *>(packet);
		//printf("ID  : %d\n", PutPacket->id);
		break;
	}
	case SC_HEATED_PLAYER:
	{  
		sc_packet_heated_player* Packet = reinterpret_cast<sc_packet_heated_player*>(packet);
		g_clients[Packet->idx].hp = Packet->hp;
		break;
	}
	case SC_MOVE_PLAYER:
	{
		sc_packet_move_player* Packet = reinterpret_cast<sc_packet_move_player *>(packet);
		if (Packet->index == g_clients[ci].idx)
		{
			g_clients[ci].x = Packet->x;
			g_clients[ci].y = Packet->y;
		}
		break;
	}

	case SC_REMOVE_PLAYER:
	{
		break;
	}
	case SC_PUT_NPC:
	{
		sc_packet_put_npc * Packet = reinterpret_cast<sc_packet_put_npc*>(packet);
		g_npcs[Packet->id].x = Packet->x;
		g_npcs[Packet->id].y = Packet->y;
		g_npcs[Packet->id].awake = true;
		//printf("ID : %d\n", PutNPCPacket->id);
		break;
	}
	case SC_MOVE_NPC:
	{
		sc_packet_move_npc* Packet = reinterpret_cast<sc_packet_move_npc*>(packet);
		g_npcs[Packet->id].x = Packet->x;
		g_npcs[Packet->id].y = Packet->y;
		break;
	}
	case SC_REMOVE_NPC: break;
	case SC_EXP_UP_PLAYER:
	{
		break;
	}
	case SC_LEVEL_UP_PLAYER:
	{
		break;
	}
	default: std::cout << "Unknown Packet Type from Server : " << ci << std::endl;
		while (true);
	}
}

void DisconnectClient(int ci)
{
	closesocket(g_clients[ci].client_socket);
	g_clients[ci].connect = false;
	g_clients[ci].connect = false;
	cout << "Client [" << ci << "] Disconnected!\n";
}

void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	//while (true);
}

void SendPacket(int cl, void *packet)
{
	int psize = reinterpret_cast<unsigned char *>(packet)[0];
	int ptype = reinterpret_cast<unsigned char *>(packet)[1];
	OverlappedEx *over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
	//std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}

void LoginOut(int c1)
{
	if (g_clients[c1].connect == false)
		return;
	cs_packet_logout p;
	p.size = sizeof(cs_packet_logout);
	p.type = CS_LOGOUT;

	SendPacket(c1, &p);
}

void Adjust_Number_Of_Client()
{
	//if (num_connections >= MAX_USER) return;
	//if (high_resolution_clock::now() < last_connect_time + 50ms) return;
	//last_connect_time = high_resolution_clock::now();
	g_clients[num_connections].client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int Result = WSAConnect(g_clients[num_connections].client_socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	
	g_clients[num_connections].curr_packet_size = 0;
	g_clients[num_connections].prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].recv_over, sizeof(g_clients[num_connections].recv_over));
	g_clients[num_connections].recv_over.event_type = OP_RECV;
	g_clients[num_connections].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR *>(g_clients[num_connections].recv_over.IOCP_buf);
	g_clients[num_connections].recv_over.wsabuf.len = sizeof(g_clients[num_connections].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].client_socket), g_hiocp, num_connections, 0);
	WSARecv(g_clients[num_connections].client_socket, &g_clients[num_connections].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recv_over.over, NULL);
	g_clients[num_connections].connect = true;
	
	++num_connections;
}

void Test_Thread()
{
	srand(time(NULL));
	while (a) 
	{
		Sleep(10);
		Adjust_Number_Of_Client();
		for (int i = 0; i < num_connections; ++i)
		{
			if (false == g_clients[i].connect) continue;
			for (int i = 0; i < num_connections; ++i)
			{
				if (false == g_clients[i].connect) continue;
				if (g_clients[i].last_move_time + 1s > high_resolution_clock::now()) continue;
				g_clients[i].last_move_time = high_resolution_clock::now();

				switch (rand() % 2)
				{
				case 0: 
				{
					cs_packet_move Movepacket;
					Movepacket.type = CS_MOVE;
					switch (rand() % 4) {
					case 0: Movepacket.dir = DIR_UP; break;
					case 1: Movepacket.dir = DIR_DOWN; break;
					case 2: Movepacket.dir = DIR_LEFT; break;
					case 3: Movepacket.dir = DIR_RIGHT; break;
					}
					Movepacket.size = sizeof(cs_packet_move);
					SendPacket(i, &Movepacket);
					break;
				}
				case 1:
				{
					cs_packet_attack AttackPacket;
					AttackPacket.type = CS_ATTACK;
					AttackPacket.size = sizeof(cs_packet_attack);
					SendPacket(i, &AttackPacket);
					break;
				}
				}
				
			}
		}
		
	}
	
}

void Worker_Thread()
{
	while (a) {
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci,
			reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);
		// std::cout << "GQCS :";
		if (FALSE == ret) 
		{
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(ci);
			else error_display("GQCS : ", WSAGetLastError());
		}
		if (0 == io_size) 
		{
			DisconnectClient(ci);
			continue;
		}
		if (OP_RECV == over->event_type) {
			
			unsigned char *buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].curr_packet_size;
			unsigned pr_size = g_clients[ci].prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<unsigned int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
		}
		else if (OP_SEND == over->event_type) {
			if (io_size != over->wsabuf.len) {
				std::cout << "Send Incomplete Error!\n";
				closesocket(g_clients[ci].client_socket);
				g_clients[ci].connect = false;
			}
			delete over;
		}
		else {
			std::cout << "Unknown GQCS event!\n";
			while (true);
		}
	}
}

void ShutdownNetwork()
{
	for (int i = 0; i < num_connections; ++i)
		LoginOut(i);
	a = false;
	test_thread.join();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
}

void InitializeNetwork()
{
	for (int i = 0; i < MAX_USER; ++i) {
		g_clients[i].connect = false;
		g_clients[i].id = INVALID_ID;
	}
	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


	//IOCP 핸들 생성 
	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);


	//워커 스레드 
	for (int i = 0; i < 4; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });

	//Test 스레드 돌리는데 이곳에서는 Connect를 test 개수만큼 진행한다. 
	test_thread = thread{ Test_Thread };

}

void getpos(int id, int* x, int * y,unsigned int* numofconnection)
{
	*x = g_clients[id].x;
	*y = g_clients[id].y;
	*numofconnection = num_connections;
}

void getnpcPos(int id, int *x, int* y, unsigned int* numofnpc)
{
	*x = g_npcs[id].x;
	*y = g_npcs[id].y;
	*numofnpc = NUM_OF_NPC;
}