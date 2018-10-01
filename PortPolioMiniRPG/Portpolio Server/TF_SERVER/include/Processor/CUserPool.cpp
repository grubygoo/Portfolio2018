#include "CUserPool.h"
#include "../Object/User.h"

CUserPool::CUserPool() :specialID(NUM_OF_NPC)
{
}

CUserPool::~CUserPool()
{
	Safe_Delete_VecList(userPool);
	indexQueue.clear();
	IDHasher::accessor Acc;
	hashMap.clear();
}
bool CUserPool::Initialize()
{
	srand(time(NULL));
	for (int i = 0; i < MAX_USER + 1; ++i) {
		userPool.push_back(new CUser());
		if (i < MAX_USER)
			indexQueue.push(i);
		else
		{
			userPool[i]->SetID(-1);
			userPool[i]->SetIndex(-1);
		}
	}
	for (int i = 0; i < MAX_USER + 1; ++i)
	{
		//�ð��� ���� �����Լ��� ���� ������.
		userPool[i]->SetPos(rand() % 1000, rand() % 1000);
		userPool[i]->SetHP(100);
		userPool[i]->SetMP(100);
		userPool[i]->SetAtk(10);
	}
	printf("Success Initialize userPool\n");
	return true;
}

bool CUserPool::SettingUserinfo(unsigned int& id)
{
	unsigned int Index;
	if (false == indexQueue.empty()) {
		if (!indexQueue.try_pop(Index)) {
			return false;
		}
	}
	else return false;

	IDHasher::accessor Acc;
	hashMap.insert(Acc, specialID);
		Acc->second = Index;
		Acc.release();

	id = specialID;
	userPool[Index]->SetIndex(Index);
	++specialID;
	return true;
}

void CUserPool::CloseUserInfo(unsigned int id, unsigned int idx)
{
	IDHasher::accessor Acc;
	while (!hashMap.erase(id));
	indexQueue.push(idx);
}
