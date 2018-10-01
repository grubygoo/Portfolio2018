#pragma once
#include <atomic>
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include "../Include.h"

class CUserPool
{
	typedef tbb::concurrent_hash_map<unsigned int, unsigned int> IDHasher;
    typedef	tbb::concurrent_queue<unsigned int> FreeIndex;

public:
	CUserPool();
	~CUserPool();
	bool Initialize();
	bool SettingUserinfo(unsigned int& id);
	void CloseUserInfo(unsigned int id,unsigned int idx);
	inline class CUser& operator[](unsigned int id)
	{
		IDHasher::const_accessor CAcc;
		if (hashMap.find(CAcc, id)) 
			return *userPool[CAcc->second];
		return *userPool[MAX_USER];
	}


private:
	std::vector<CUser*> userPool;
	std::atomic<unsigned int> specialID;
	IDHasher hashMap;
	FreeIndex indexQueue;
};

