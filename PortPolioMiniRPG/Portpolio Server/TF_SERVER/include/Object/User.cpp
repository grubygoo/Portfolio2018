#include "User.h"
#include "../Error/ErrorUtil.h"

CUser::CUser():packetSize(0),prevPacketSize(0),connected(false)
{
	ZeroMemory(&exover.WSAOVER, sizeof(WSAOVERLAPPED)); 
	exover.WsaBuf.buf = exover.IO_Buf;
	exover.WsaBuf.len = sizeof(exover.IO_Buf);
	exover.IO_Type = IO_RECV;

}

CUser::~CUser()
{
}

void CUser::Reset()
{
	packetSize = 0;
	prevPacketSize = 0;
	ZeroMemory(&exover.WSAOVER, sizeof(WSAOVERLAPPED));
	connected = false;
	ZeroMemory(&name, sizeof(WSAOVERLAPPED));
	closesocket(socket);
	socket = NULL;
	SetConnected(false);
	SetAlive(false);
	//tbb::queuing_rw_mutex::scoped_lock lock(userLock[2], true);
	//name[0] = '\0';
}

void CUser::ReserveRecv()
{
	unsigned long flag = 0;
	//WSAOVER √ ±‚»≠ 
	ZeroMemory(&exover.WSAOVER, sizeof(WSAOVERLAPPED));
	if (0 != WSARecv(socket, &exover.WsaBuf, 1, NULL, &flag, &exover.WSAOVER, NULL)) {
		if (WSA_IO_PENDING != WSAGetLastError())
			ErrorUtil::Err_Display("Start RECV");
	}
}

char* CUser::Process(int data_size, char* io_ptr)
{

	int Work_Size = data_size;
	char* IO_Ptr = io_ptr;

	while (0 < Work_Size) {
		int p_size;
		if (0 != packetSize)
			p_size = packetSize;
		else {
			p_size = IO_Ptr[0];
			packetSize = p_size;
		}
		int need_size = p_size - prevPacketSize;
		if (need_size <= Work_Size) {
			memcpy(packetBuf
				+ prevPacketSize, IO_Ptr, need_size);
			prevPacketSize = 0;
			packetSize = 0;
			Work_Size -= need_size;
			IO_Ptr += need_size;
			return packetBuf;

		}
		else {
			memcpy(packetBuf + prevPacketSize, IO_Ptr, Work_Size);
			prevPacketSize += Work_Size;
			Work_Size = -Work_Size;
			IO_Ptr += Work_Size;
		}
	}
	return NULL;
}

