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
	Safe_Delete_VecList(NPCPool);
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
	for (auto& D : CMapData::GET_SINGLE()->Stage1Data)
	{  
		//printf("%d\n", D.Type);
		if (D.Type == READ_DATA::FINALBOSS) continue;
		for (int i = 0; i < 3; ++i)
		{
		   float randNum = rand() % 30 - 15;
		   XMFLOAT3 InitPos(D.x + randNum, 0, D.z + randNum);
		   NPCPool[Index]->SetPos(InitPos);
		   NPCPool[Index]->SetOOBB(InitPos);
		   NPCPool[Index]->SetMType(D.Type);
		   NPCPool[Index]->SetSpeed(D.Speed);
		   NPCPool[Index]->SetAtk(D.Atk);
		   NPCPool[Index]->SetHp(D.Hp);
		   ++Index;
		}
	}

	
	


	printf("Success Initialize NPCProcessor\n");
	return true;
}

void CNPCProcessor::IdleNPC(WORD id)
{
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) return;

	NPCPool[id]->SetDir(CVData::GET_SINGLE()->RandomDirMap[rand() % 8]);
	NPCPool[id]->SetState(IDLESTATE);
	std::unordered_set<WORD>  NPCVl;
	GetNPCViewList(NPCVl, *NPCPool[id]);
	for (auto ID : NPCVl)
		CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[id]);

	if (!NPCVl.empty())
	{
		if (NPCPool[id]->GetMType() != MONSTER::STONEMON) // 스톤몬을 제외한 나머지는 다 선공
		{
			float LowestHP = 1000.f;
			WORD  TargetID = 0;
			for (auto ID : NPCVl)
			{
				if (LowestHP > (*UserPool)[ID].GetHp())
				{
					TargetID = ID;
					LowestHP = (*UserPool)[ID].GetHp();
				}
			}
			NPCPool[id]->SetTarget(TargetID);
		}
	}
	else
	{
		NPCPool[id]->SetTarget(NONE_TARGET);
		NPCPool[id]->SetAwake(false);
	}
	AddEvent(id, NPC_MOVE, high_resolution_clock::now() + 2s);
}

void CNPCProcessor::MoveNPC(WORD id)
{
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) return;
	NPCPool[id]->SetState(MOVESTATE);
}

void CNPCProcessor::AttackNPC(WORD id)
{
	WORD Target = NPCPool[id]->GetTarget();
	if (Target == NONE_TARGET) return;
	if (chrono::high_resolution_clock::now() - NPCPool[id]->GetPrevAttackTime() < 1s) return;
	if (!NPCPool[id]->GetAlive() || !NPCPool[id]->GetAwake()) return;
	if (!(*UserPool)[Target].GetAlive() || !(*UserPool)[Target].GetConnected()) {
		AddEvent(id, NPC_IDLE, chrono::high_resolution_clock::now());
		return;
	}
	std::unordered_set<WORD>  NPCVl;
	GetNPCViewList(NPCVl, *NPCPool[id]);
	for (auto& ID : NPCVl)
		CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[id]);


	float NewHP = 0.f;
	if (!(*UserPool)[Target].GetINV())
	{
		BYTE Dir = NPCPool[id]->GetDir();

		BoundingOrientedBox AttackOOBB(Vector3::Add(NPCPool[id]->GetPos(),CVData::GET_SINGLE()->LookVectorMap[Dir], 5.f),
			XMFLOAT3(2.f, 1.f, 6.f), XMFLOAT4(0, 0, 0, 1));

		if ((*UserPool)[Target].CheckCollision(AttackOOBB))
		{
			NewHP = (*UserPool)[Target].GetHp() - NPCPool[id]->GetAtk();
			(*UserPool)[Target].SetHp(NewHP);
			NPCPool[id]->SetPrevAttackTime(chrono::high_resolution_clock::now());
		}

	}

	if ((*UserPool)[Target].GetHp() <= 0.f) //사망 했을 경우 
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
		AddEvent(id, NPC_IDLE, high_resolution_clock::now());
		AddEvent(Target, PLAYER_RESPAWN, high_resolution_clock::now() + 3s);
	}
	else // 아닐 경우 
	{
		CSendManager::PlayerDamageStepPacket((*UserPool)[Target]);
		if (Vector3::InMySight(NPCPool[id]->GetPos(), (*UserPool)[Target].GetPos(), ATTACK_RADIUS)) {
			AddEvent(id, NPC_ATTACK, high_resolution_clock::now() + 1s);
		}
		else NPCPool[id]->SetState(MOVESTATE);
	}
}

void CNPCProcessor::GetNPCViewList(std::unordered_set<WORD>& vl, CNPC& npc)
{
	tbb::queuing_rw_mutex::scoped_lock(Tlm, false);
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
		NPC->AddMoveTime(fDeltaTime);
		GetNPCViewList(OldVl, *NPC);
		if (NPC->GetTarget() != NONE_TARGET) NPC->LookAtTarget((*UserPool)[NPC->GetTarget()].GetPos());
		NPC->Move(fDeltaTime);
		NPC->MapCollisionCheck(fDeltaTime);
		/*NPC 주변 플레이어 ID를 읽어온다.*/
		GetNPCViewList(NewVl, *NPC);
		for (auto ID : NewVl){
			if (0 == OldVl.count(ID)) {
				if ((*UserPool)[ID].AddList(NPC->GetID()))
					CSendManager::PutNPCPacket((*UserPool)[ID], *NPC);
			}
			else CSendManager::StateNPCPacket((*UserPool)[ID], *NPC);
		}
		for (auto ID : OldVl)
		{
			if (0 == NewVl.count(ID))
			{
				if ((*UserPool)[ID].DeleteList(NPC->GetID()))
					CSendManager::RemoveNPCPacket((*UserPool)[ID], *NPC);

			}
		}
		if (!NewVl.empty()){
			if (NPC->GetTarget() != NONE_TARGET){
				if (Vector3::InMySight(NPC->GetPos(), (*UserPool)[NPC->GetTarget()].GetPos(), ATTACK_RADIUS)){
					NPC->SetState(ATTACKSTATE);
					AddEvent(NPC->GetID(), NPC_ATTACK, chrono::high_resolution_clock::now()+10ms);
				}
			}
			else if (NPC->GetMoveTime() > rand() % 4){
				NPC->ResetMoveTime();
				AddEvent(NPC->GetID(), NPC_IDLE, chrono::high_resolution_clock::now());
			}
		}
		else{
			NPC->SetState(IDLESTATE);
			NPC->SetAwake(false);
			NPC->SetTarget(NONE_TARGET);
		}
	}
}

