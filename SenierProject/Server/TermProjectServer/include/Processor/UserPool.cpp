#include "UserPool.h"
#include "../Object/User.h"


CUserPool::CUserPool():UniqueID(NUM_OF_NPC)
{


}

CUserPool::~CUserPool()
{
	Safe_Delete_VecList(UserPool);
	IndexQueue.clear();
	IDHasher::accessor Acc;
	HashMap.clear();
}

bool CUserPool::Initialize()
{  
	XMFLOAT3  InitPos = XMFLOAT3(60.f, 0.f, -60.f);
	for (int i = 0; i < MAX_USER + 1; ++i)
	{
		UserPool.emplace_back(new CUser());
		UserPool[i]->SetPos(InitPos);
		UserPool[i]->SetStage(STAGE::STAGE1);
		UserPool[i]->SetCurCharacter(CHARACTER::WARRIOR);
		UserPool[i]->SetHp(500.f);
		UserPool[i]->SetMp(500.f);
		UserPool[i]->SetAtk(10.f);
		if (i<MAX_USER)IndexQueue.push(i);
	}
	printf("Success Initialize UserPool\n");
	return true;
}

bool CUserPool::SettingUserinfo(WORD & id)
{
	WORD Index;
	if (false == IndexQueue.empty()) {
		if (!IndexQueue.try_pop(Index)) {
			return false;
		}
	}
	else return false;

	IDHasher::accessor Acc;
	if (HashMap.insert(Acc, UniqueID)) {
		Acc->second = Index;
		Acc.release();
	}
	else 
	{
		Acc.release();
		return false;
	}

	id = UniqueID;
	UserPool[Index]->SetIndex(Index);
	++UniqueID;
	return true;
}

void CUserPool::CloseUserInfo(WORD id,WORD idx)
{

	IDHasher::accessor Acc;
	while (!HashMap.erase(id));
	IndexQueue.push(idx);
}

CUser & CUserPool::GetUser(WORD id)
{
	IDHasher::const_accessor CAcc;
	if (HashMap.find(CAcc, id))return *UserPool[CAcc->second];
	return *UserPool[MAX_USER];
}
