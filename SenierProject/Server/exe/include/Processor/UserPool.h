#pragma once
#include <atomic>
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include "../Include.h"

class CUserPool
{
	typedef tbb::concurrent_hash_map<WORD, WORD> IDHasher;
	typedef	tbb::concurrent_queue<WORD> FreeIndex;
public:
	CUserPool();
	~CUserPool();
	bool Initialize();
	bool SettingUserinfo(WORD& id);
	void CloseUserInfo(WORD id);
	class CUser& GetUser(WORD id);
	class CUser& operator[](WORD id)
	{
		IDHasher::const_accessor CAcc;
		if (HashMap.find(CAcc, id))return *UserPool[CAcc->second];
		return *UserPool[MAX_USER];
	}
private:
	std::vector<class CUser*> UserPool;
	std::atomic<WORD> UniqueID;
	IDHasher HashMap;
	FreeIndex IndexQueue;
};

