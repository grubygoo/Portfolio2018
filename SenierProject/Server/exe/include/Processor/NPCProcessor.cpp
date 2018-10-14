#include "NPCProcessor.h"
#include "SendManager.h"
#include "../Util/MapData.h"
#include "../Util/Astar.h"
#include "../Util/VectorData.h"

CNPCProcessor::CNPCProcessor()
{
	srand((unsigned)time(NULL));
	Processor[NPC_IDLE] = &CNPCProcessor::IdleNPC;
	Processor[NPC_MOVE] = &CNPCProcessor::MoveNPC;
	Processor[NPC_ATTACK] = &CNPCProcessor::AttackNPC;
}
CNPCProcessor::~CNPCProcessor()
{

}

bool CNPCProcessor::Initialize()
{
	srand(time(NULL));

	for (int i = 0; i < NUM_OF_NPC; ++i)
	{
		NPCPool.emplace_back(new CNPC());
		NPCPool[i]->SetID(i);
		NPCPool[i]->SetAlive(true);
	}

	WORD Index = 0;
	for (int y = 0; y < CMapData::GET_SINGLE()->Height; ++y)
	{
		for (int x = 0; x < CMapData::GET_SINGLE()->Width; ++x)
		{
			if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::SMON)
			{
				/*StoneMon*/
				for (int i = 0; i < 3; ++i)
				{
					int randNum = rand() % 30 - 15;
					XMFLOAT3 InitPos(x*30.f + randNum, 0, -30 * y + randNum);
					NPCPool[Index]->SetPos(InitPos);
					NPCPool[Index]->SetOOBB(InitPos);
					NPCPool[Index]->SetMType(MONSTER::STONEMON);
					NPCPool[Index]->SetHp(80.f);
					NPCPool[Index]->SetAtk(30.f);
					NPCPool[Index]->SetSpeed(5);
					Index++;
				}
			}
			else if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::BMON)
			{
				/*BeatleMon*/
				for (int i = 0; i < 3; ++i)
				{
					int randNum = rand() % 30 - 15;
					XMFLOAT3 InitPos(x*30.f + randNum, 0, -30 * y + randNum);
					NPCPool[Index]->SetPos(InitPos);
					NPCPool[Index]->SetOOBB(InitPos);
					NPCPool[Index]->SetMType(MONSTER::BEATLEMON);
					NPCPool[Index]->SetHp(100.f);
					NPCPool[Index]->SetAtk(5.f);
					NPCPool[Index]->SetSpeed(7.f);
					NPCPool[Index]->SetAlive(true);
					Index++;
				}
			}
			else if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::SMONBOSS)
			{
				/*BeatleMon*/
				XMFLOAT3 InitPos(x*30.f, 0, -30 * y);
				NPCPool[Index]->SetPos(InitPos);
				NPCPool[Index]->SetOOBB(InitPos);
				NPCPool[Index]->SetMType(MONSTER::STONEBOSS);
				NPCPool[Index]->SetHp(120.f);
				NPCPool[Index]->SetAtk(9.f);
				NPCPool[Index]->SetSpeed(10);
				NPCPool[Index]->SetAlive(true);
				Index++;
			}
			else if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::BMONBOSS)
			{
				/*BeatleMon*/
				XMFLOAT3 InitPos(x*30.f, 0, -30 * y);
				NPCPool[Index]->SetPos(InitPos);
				NPCPool[Index]->SetOOBB(InitPos);
				NPCPool[Index]->SetMType(MONSTER::BEATLEBOSS);
				NPCPool[Index]->SetHp(150.f);
				NPCPool[Index]->SetAtk(13.f);
				NPCPool[Index]->SetSpeed(10);
				NPCPool[Index]->SetAlive(true);
				Index++;
			}
		}
	}
	printf("Success Initialize NPCProcessor\n");
	return true;
}

void CNPCProcessor::IdleNPC(WORD id)
{
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) {
		NPCPool[id]->SetTarget(NONE_TARGET);
		return;
	}
	NPCPool[id]->SetDir(CVData::GET_SINGLE()->RandomDirMap[rand() % 8]);
	NPCPool[id]->SetState(IDLESTATE);

	std::unordered_set<WORD>  NPCVl;
	GetNPCViewList(NPCVl, *NPCPool[id]);

	for (auto ID : NPCVl) CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[id]);

	if (!NPCVl.empty())
	{
		if (NPCPool[id]->GetMType() != MONSTER::STONEMON) // 스톤몬을 제외한 나머지는 다 선공
		{
			float LowestHP = 1000.f;
			WORD  TargetID = 0;
			for (auto ID : NPCVl)
			{
				if (LowestHP > (*UserPool)[ID].GetHp()) {
					TargetID = ID;
					LowestHP = (*UserPool)[ID].GetHp();
				}
			}
			NPCPool[id]->SetTarget(TargetID);
		}
	}
	AddEvent(id, NPC_MOVE, high_resolution_clock::now() + 2s);
}

void CNPCProcessor::MoveNPC(WORD id)
{
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) return;
	NPCPool[id]->SetState(MOVESTATE);
	unordered_set<WORD> NPCViewList;
	GetNPCViewList(NPCViewList, *NPCPool[id]);

	for (auto ID : NPCViewList)
	{
		if ((*UserPool)[ID].AddList(id)) CSendManager::PutNPCPacket((*UserPool)[ID], *NPCPool[id]);
		else  CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[id]);
	}
}

void CNPCProcessor::AttackNPC(WORD id) //체크 요망 
{
	WORD Target = NPCPool[id]->GetTarget();
	if ((high_resolution_clock::now() - NPCPool[id]->GetAttackTime()) < 1s) {
		return;
	}
	if (Target == NONE_TARGET) return;
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) return;
	if (!(*UserPool)[Target].GetAlive()) return;
	if (!(*UserPool)[Target].GetConnected()) return;

	NPCPool[id]->SetAttackTime(high_resolution_clock::now());
	NPCPool[id]->SetState(ATTACKSTATE);
	NPCPool[id]->LookAtTarget((*UserPool)[id].GetPos());



	std::unordered_set<WORD>  NPCVl;
	GetNPCViewList(NPCVl, *NPCPool[id]);
	for (auto& ID : NPCVl) {
		NPCPool[id]->LookAtTarget((*UserPool)[ID].GetPos());
		CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[id]);
	}

	float NewHP = (*UserPool)[Target].GetHp();
	if (!(*UserPool)[Target].GetINV()) {
		 NewHP = (*UserPool)[Target].GetHp() - NPCPool[id]->GetAtk();
		(*UserPool)[Target].SetHp(NewHP);
	}

	if (NewHP <= 0.f) //사망 했을 경우 
	{
		(*UserPool)[Target].SetAlive(false);

		std::unordered_set<WORD> UserVl;
		(*UserPool)[Target].ReadList(UserVl);

		for (auto ID : UserVl)
		{
			if (IsNPC(ID)) continue;
			if ((*UserPool)[ID].DeleteList(Target))
				CSendManager::RemovePlayerPacket((*UserPool)[ID], (*UserPool)[Target]);
		}
		CSendManager::RemovePlayerPacket((*UserPool)[Target], (*UserPool)[Target]); // 나 자신에게도 죽었다는 것을 보여줘야함.
		NPCPool[id]->SetTarget(NONE_TARGET);
		AddEvent(id, NPC_IDLE, high_resolution_clock::now() + 10ms);
		AddEvent(Target, PLAYER_RESPAWN, high_resolution_clock::now() + 3s);
		NPCPool[id]->SetAttackTime(high_resolution_clock::time_point());
	}
	else // 아닐 경우 
	{
		CSendManager::PlayerDamageStepPacket((*UserPool)[Target]);
		if (Vector3::InMySight(NPCPool[id]->GetPos(), (*UserPool)[Target].GetPos(), ATTACK_RADIUS)) {
			AddEvent(id, NPC_ATTACK, high_resolution_clock::now() + 1s);
		}
		else
		{
			AddEvent(id, NPC_MOVE, high_resolution_clock::now() + 10ms);
		}
	}

}

void CNPCProcessor::GetNPCViewList(std::unordered_set<WORD>& vl, CNPC& npc)
{

	tbb::queuing_rw_mutex::scoped_lock(Tlm);
	for (auto ID : TraverseVec)
	{
		if (!(*UserPool)[ID].GetConnected()) continue;
		if (!(*UserPool)[ID].GetAlive()) continue;
		if ((*UserPool)[ID].GetStage() != npc.GetStage()) continue;
		if (Vector3::InMySight(npc.GetPos(), (*UserPool)[ID].GetPos(), VIEW_RADIUS)) {
			vl.insert(ID);
		}
	}
}

void CNPCProcessor::Simulation(float fDeltaTime)
{
	for (auto& NPC : NPCPool)
	{
		if (!NPC->GetAlive()) continue;
		if (!NPC->GetAwake()) continue;
		if (NPC->GetState() != MOVESTATE) continue;
		std::unordered_set<WORD> OldVl;
		std::unordered_set<WORD> NewVl;
		bool IsColl = false;
		WORD Dir = NPC->GetDir();
		NPC->AddMoveTime(fDeltaTime);

		GetNPCViewList(OldVl, *NPC);
		if (NPC->GetTarget() != NONE_TARGET) NPC->LookAtTarget((*UserPool)[NPC->GetTarget()].GetPos());

		NPC->Move(fDeltaTime);
		NPC->MapCollisionCheck(fDeltaTime, IsColl);
		GetNPCViewList(NewVl, *NPC);
		for (auto ID : NewVl)
		{
			if (0 == OldVl.count(ID)) {
				if ((*UserPool)[ID].AddList(NPC->GetID()))
					CSendManager::PutNPCPacket((*UserPool)[ID], *NPC);
			}
			else {
				if (NPC->GetDir() != Dir || IsColl)
					CSendManager::StateNPCPacket((*UserPool)[ID], *NPC);
			}
		}

		for (auto ID : OldVl)
		{
			if (0 == NewVl.count(ID))
			{
				if ((*UserPool)[ID].DeleteList(NPC->GetID()))
				{
					CSendManager::RemoveNPCPacket((*UserPool)[ID], *NPC);
				}
			}
		}

		if (!NewVl.empty())
		{
			if (NPC->GetTarget() != NONE_TARGET) {

				if (Vector3::InMySight(NPC->GetPos(), (*UserPool)[NPC->GetTarget()].GetPos(), ATTACK_RADIUS)) {
					AddEvent(NPC->GetID(), NPC_ATTACK, chrono::high_resolution_clock::now() + 10ms);
				}
			}
			else if (NPC->GetMoveTime() > rand() % 4) {
				AddEvent(NPC->GetID(), NPC_IDLE, chrono::high_resolution_clock::now() + 10ms);
				NPC->ResetMoveTime();
			}
		}
		else
		{
			NPC->SetState(IDLESTATE);
			NPC->SetAwake(false);
			NPC->SetTarget(NONE_TARGET);
		}
	}
}

