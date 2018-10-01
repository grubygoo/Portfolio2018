#include "NPCProcessor.h"
#include "SendManager.h"
#include "Astar.h"
CNPCProcessor::CNPCProcessor()
{
	srand((unsigned int)time(NULL));

	for (int i = 0; i < NUM_OF_NPC; ++i)
	{
		npcPool.emplace_back(new CNPC());
		npcPool[i]->SetID(i);
		npcPool[i]->SetPos(rand() % 1000, rand() % 1000);
		npcPool[i]->SetAtk(10);
		npcPool[i]->SetHP(100);
		npcPool[i]->SetMP(100);
		npcPool[i]->SetExp(10);
	}
	//Sector 초기화
	for (int i = 0; i < SECTOR_SIZE; ++i)
	{
		std::vector<CSector*> temp(SECTOR_SIZE);
		worldSector.emplace_back(temp);
	}
	for (int y = 0; y < SECTOR_SIZE; ++y)
	{
		for (int x = 0; x < SECTOR_SIZE; ++x) {
			worldSector[y][x] = new CSector();
		}
	}
	Astar::GET();
	Processor[NPC_IDLE] = &CNPCProcessor::IdleNPC;
	Processor[NPC_MOVE] = &CNPCProcessor::MoveNPC;
	Processor[NPC_TRACE] = &CNPCProcessor::MoveNPC;
	Processor[NPC_ATTACK] = &CNPCProcessor::AttackNPC;
	Processor[NPC_RESPAWN] = &CNPCProcessor::RespawnNPC;
}

CNPCProcessor::~CNPCProcessor()
{
	Safe_Delete_VecList(npcPool);
	for (auto & iter : worldSector) {
		Safe_Delete_VecList(iter);
	}
}

bool CNPCProcessor::Initialize()
{ 
	srand(GetTickCount());
	Vec2i  Index;
	for (int i = 0; i < NUM_OF_NPC; ++i)
	{
		Index = npcPool[i]->GetPos();
		worldSector[Index.y / SECTOR_SIZE][Index.x / SECTOR_SIZE]->AddList(npcPool[i]->GetID());
	}
	return true;
}

void CNPCProcessor::IdleNPC(unsigned int id)
{  
	if (!npcPool[id]->GetAlive()) return;
	npcPool[id]->SetEType(EV_TYPE::NPC_IDLE);
	Vec2i CurPos = npcPool[id]->GetPos();
	unordered_set<CSector*> SectorList;
	unordered_set<unsigned int> CurViewList;
	SearchNearSector(CurPos, SectorList);
	for (auto& P : SectorList) {
		P->TraverseList(CurPos, CurViewList);
	}
	bool IsInPlayer = false;
	for (auto ID : CurViewList) {
		if (!IsNPC(ID)) {
			IsInPlayer = true;
			break;
		}

	}
	if (IsInPlayer)
	{
		ProcessState(id);
	}
	else
	{
		npcPool[id]->SetAwake(false);
	}
}

void CNPCProcessor::MoveNPC(unsigned int id)
{
	/*이전 섹터와 현재 섹터를 구한다.*/
	if (!npcPool[id]->GetAlive()) return;
	npcPool[id]->SetEType(EV_TYPE::NPC_MOVE);

	unsigned int Target = npcPool[id]->GetTarget();
	Vec2i CurPos = npcPool[id]->GetPos();
	CSector* CurSector = worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE];
	unordered_set<CSector*> SectorList;
	unordered_set<unsigned int> OldViewList;
	unordered_set<unsigned int> NewViewList;
	bool IsInPlayer = false;

	SearchNearSector(CurPos, SectorList);
	for (auto P : SectorList){
		P->TraverseList(CurPos, OldViewList);
	}

	/*Move 함수로 대체할 예정*/


	if (Target == NONE_TARGET)
	{
		switch (rand() % 4)
		{
		case 0: if (CurPos.x > 0) npcPool[id]->SetPos(--CurPos.x, CurPos.y); break;
		case 1: if (CurPos.x < BOARD_WIDTH - 1) npcPool[id]->SetPos(++CurPos.x, CurPos.y); break;
		case 2: if (CurPos.y > 0) npcPool[id]->SetPos(CurPos.x, --CurPos.y); break;
		case 3: if (CurPos.y < BOARD_HEIGHT - 1) npcPool[id]->SetPos(CurPos.x, ++CurPos.y); break;
		default: break;
		}
	}
	else
	{ 
		Vec2i TargetPos = (*userPool)[Target].GetPos();
		CurPos = Astar::GET()->FindPath(CurPos,TargetPos);
		npcPool[id]->SetPos(CurPos.x, CurPos.y);
	}


	/*움직인 위치에서 자신의 섹터가 바뀌었다면 섹터를 변경*/
	CurPos = npcPool[id]->GetPos();
	if (CurSector != worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE])
	{
		worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE]->AddList(id);
		CurSector->DeleteList(id);
	}

	SectorList.clear();
	SearchNearSector(CurPos, SectorList);

	/*자신의 섹터 및 주변 섹터를 한꺼번에 검색*/
	for (auto P : SectorList) {
		P->TraverseList(CurPos, NewViewList);
	}

	/*Move Update*/
	for (auto ID : NewViewList)
	{
		if (IsNPC(ID)) continue;
		if ((*userPool)[ID].AddList(id)) CSendManager::PutNPCPacket((*userPool)[ID], *npcPool[id]);
		else CSendManager::MoveNPCPacket((*userPool)[ID], *npcPool[id]);
		IsInPlayer = true;
	}

	for (auto ID : OldViewList)
	{
		if (IsNPC(ID)) continue;
		if (0 == NewViewList.count(ID)) {
			if ((*userPool)[ID].DeleteList(id))
				CSendManager::RemoveNPCPacket((*userPool)[ID], *npcPool[id]);
		}
	}

	if (IsInPlayer) ProcessState(id);
	else 
	{
		npcPool[id]->SetAwake(false);
		npcPool[id]->SetTarget(NONE_TARGET);
	}
}

void CNPCProcessor::AttackNPC(unsigned int id)
{
	if (!npcPool[id]->GetAlive()) return;
	npcPool[id]->SetEType(EV_TYPE::NPC_ATTACK);
	unsigned int Target = npcPool[id]->GetTarget();
	  
	if (InMySight(npcPool[id]->GetPos(), (*userPool)[Target].GetPos(), ATTACK_RADIUS))
	{
		unsigned int NewHP = (*userPool)[Target].GetHP() - npcPool[id]->GetAtk();
	
		if (NewHP <= DEAD_HP) // Damage Packet
		{    
			(*userPool)[Target].SetHP(0);
			CSendManager::HeatedPlayerPacket((*userPool)[Target]);
			std::unordered_set<unsigned int> PlayerViewList;
			(*userPool)[Target].ReadList(PlayerViewList);
			PlayerViewList.insert(Target);

			for (auto ID : PlayerViewList){
				if (IsNPC(ID)) continue;
				CSendManager::RemovePlayerPacket((*userPool)[ID], (*userPool)[Target]);
			}

			Vec2i CurPos = (*userPool)[Target].GetPos();
			CSector*CurSector = worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE];
			CurSector->DeleteList(Target);
			(*userPool)[Target].SetAlive(false);	
			AddEvent(Target, PLAYER_RESPAWN, high_resolution_clock::now() + 5s);
			npcPool[id]->SetTarget(NONE_TARGET);
		}
		else
		{
			(*userPool)[Target].SetHP(NewHP);
			//printf("New HP :%d\n", NewHP);
			CSendManager::HeatedPlayerPacket((*userPool)[Target]);
		}
	}
	ProcessState(id);
}

void CNPCProcessor::RespawnNPC(unsigned int id)
{   
	npcPool[id]->SetEType(EV_TYPE::NPC_RESPAWN);
}

void CNPCProcessor::ProcessState(unsigned int id)
{
	/*행동을 실행하고 나서의 시점*/
	/*주변에 Player가 있는 경우에 들어오기*/
	
	unsigned int Target = npcPool[id]->GetTarget();
	if (Target != NONE_TARGET)
	{
		if (InMySight(npcPool[id]->GetPos(), (*userPool)[Target].GetPos(), ATTACK_RADIUS))
			AddEvent(id, NPC_ATTACK, high_resolution_clock::now() + 1s);
		else AddEvent(id, NPC_TRACE, high_resolution_clock::now() + 1s);
	}
	else
	{  
		
		/*일정확률로 IDLE 상태로 돌입*/
		if (rand() % 100 < 70) AddEvent(id, NPC_MOVE, high_resolution_clock::now() + 1s);
		else AddEvent(id, NPC_IDLE, high_resolution_clock::now() + 1s);
	}
}


