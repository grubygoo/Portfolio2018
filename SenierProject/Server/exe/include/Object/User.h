#pragma once
#include "Object.h"
class CUser :public CObject
{
	static const BYTE   UserLockNum = 4;

public:
	CUser();
	virtual ~CUser();
	void SetSock(SOCKET sock) { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[0], true); mSock = sock; }
	void SetConnected(bool check) { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[1], true); Connected = check; }
	void SetCurCharacter(const BYTE cur) { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[2], true); CurCharacter = cur; }
	void SetINV(const bool cur) { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[3], true);  IsInvincible = cur; }
	void SetIndex(WORD idx) { Index = idx; }

	const SOCKET GetSock() { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[0], false); return mSock; }
	const bool  GetConnected() { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[1], false); return Connected; }
	const BYTE  GetCurCharacter() { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[2], false); return CurCharacter; }
	const BYTE  GetINV() { tbb::queuing_rw_mutex::scoped_lock lock(UserLock[3], false); return IsInvincible; }
	const WORD GetIndex() { return Index; }

	void  R_Recv();
	char* Process(int data_size, char* io_ptr);
	void Reset();

	inline bool AddList(WORD id)
	{
		tbb::mutex::scoped_lock(vlm);
		if (ViewList.count(id) != 0) return false;
		ViewList.insert(id);
		return true;
	}
	inline bool DeleteList(WORD id)
	{
		tbb::mutex::scoped_lock(vlm);
		if (ViewList.count(id) == 0) return false;
		ViewList.erase(id);
		return true;
	}
	inline void SwapList(const std::unordered_set<WORD>& list)
	{
		tbb::mutex::scoped_lock(vlm);
		ViewList = list;
	}
	inline void ReadList(std::unordered_set<WORD>& list)
	{
		tbb::mutex::scoped_lock(vlm);
		list = ViewList;
	}

private:
	std::unordered_set<WORD>  ViewList;
	tbb::mutex                Vlm;
	int                PacketSize;
	int                PrevPacketSize;
	char               Packet[MAX_PACKET_SIZE];
	bool               IsInvincible;
	SOCKET             mSock;
	EXOVER             Exover;
	bool               Connected;
	BYTE               CurCharacter;
	WORD               Index;
	tbb::queuing_rw_mutex  UserLock[UserLockNum];
};

