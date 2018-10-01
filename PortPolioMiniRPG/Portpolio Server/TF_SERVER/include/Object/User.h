#pragma once
#include "Object.h"
#include <mutex>
using namespace std;
class CUser :public CObject
{
	static const BYTE   NUM_OF_LOCK = 4;
public:
	CUser();
	virtual ~CUser();
	void SetSock(SOCKET sock) { tbb::queuing_rw_mutex::scoped_lock lock(userLock[0], true); socket = sock; }
	void SetConnected(bool check) { tbb::queuing_rw_mutex::scoped_lock lock(userLock[1], true); connected = check; }
	void SetName(char* name) { tbb::queuing_rw_mutex::scoped_lock lock(userLock[2], true); strncpy(name, name, sizeof(name)); }
	void SetIndex(unsigned int idx) { tbb::queuing_rw_mutex::scoped_lock lock(userLock[3], true); index = idx; }
	SOCKET GetSock() { tbb::queuing_rw_mutex::scoped_lock lock(userLock[0], false); return socket; }
	const bool GetConnected() { tbb::queuing_rw_mutex::scoped_lock lock(userLock[1], false); return connected; }
	const char* GetName() { tbb::queuing_rw_mutex::scoped_lock lock(userLock[2], false); return name; }
	const unsigned int GetIndex() { tbb::queuing_rw_mutex::scoped_lock lock(userLock[3], false); return index; }
	void  Reset();
	void  ReserveRecv();
	char* Process(int data_size, char* io_ptr);

	inline bool AddList(unsigned int id) {
		vlm.lock();
		auto a = viewlist.insert(id);
		vlm.unlock();
		return a.second;
	}

	inline bool DeleteList(unsigned int id) 
	{
		vlm.lock();
		auto a = viewlist.erase(id);
		vlm.unlock();
		return a;
	}

	inline void SwapList(const std::unordered_set<unsigned int>& list)
	{
		vlm.lock();
		viewlist = list;
		vlm.unlock();
	}

	inline void ReadList(std::unordered_set<unsigned int>& list)
	{
		vlm.lock();
		list = viewlist;
		vlm.unlock();
	}

private:
	int  packetSize;
	int  prevPacketSize;
	char packetBuf[MAX_PACKET_SIZE];
	char name[MAX_STR_SIZE];
	bool connected;
	unsigned int  index;
	SOCKET socket;
	EXOVER exover;
	std::mutex vlm;
	std::unordered_set<unsigned int > viewlist;
	tbb::queuing_rw_mutex  userLock[NUM_OF_LOCK];
};

