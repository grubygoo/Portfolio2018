#include "UserProcessor.h"
#include "SendManager.h"
#include "../Object/User.h"
#include "../DB/DB.h"

CUserProcessor::CUserProcessor()
{
	userPool = new CUserPool();
	processor[CS_LOGIN] = &CUserProcessor::PlayerLogin;
	processor[CS_MOVE] = &CUserProcessor::PlayerMove;
	processor[CS_ATTACK] = &CUserProcessor::PlayerAttack;
	processor[CS_LOGOUT] = &CUserProcessor::PlayerLogOut;
	eventProcessor[PLAYER_RESPAWN] = &CUserProcessor::PlayerRespawn;
}

CUserProcessor::~CUserProcessor()
{
	SAFE_DELETE(userPool);
}

bool CUserProcessor::Initialize()
{
	if (!userPool->Initialize()) {
		printf("fail to Initialize userPool\n");
		return false;
	}
	return true;
}

bool CUserProcessor::UserSetting(HANDLE hIocp, SOCKET sock)
{
	unsigned int NewID = 0;
	if (!userPool->SettingUserinfo(NewID))  return false;
	//printf("New Client Connected ID[%d]\n", NewID);

	(*userPool)[NewID].SetID(NewID);
	(*userPool)[NewID].SetSock(sock);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), hIocp, NewID, 0);

	PlayerPutIn(NewID);
	(*userPool)[NewID].ReserveRecv();
	return true;
}

void CUserProcessor::PacketProcess(unsigned int id, int work_size, char * io_ptr)
{
	char*Packet = (*userPool)[id].Process(work_size, io_ptr);
	if (Packet == NULL) return;
	(this->*processor[Packet[1]])(id, Packet);
	(*userPool)[id].ReserveRecv();
}

void CUserProcessor::PlayerPutIn(unsigned int id)
{
	(*userPool)[id].SetConnected(true);
	(*userPool)[id].SetAlive(true);
	CSendManager::LoginSuccPacket((*userPool)[id]);
	unordered_set<CSector*> NearSectorList;
	unordered_set<unsigned int> PlayerViewList;
	Vec2i CurPos = (*userPool)[id].GetPos();

	//현재 자신 위치의 섹터에 집어넣는다.
	worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE]->AddList(id);

	//자신섹터 뿐 아니라 주변에 걸치는 부분 섹터리스트를 가져온다.
	SearchNearSector(CurPos, NearSectorList);

	//주변 섹터리스트를 차례차례 Traverse 한다.
	for (auto& p : NearSectorList) {
		p->TraverseList(CurPos, PlayerViewList);
	}


	for (auto p : PlayerViewList)
	{
		if (id == p) continue;
		if (IsNPC(p))
		{
			AWakeNPC(p);
			CSendManager::PutNPCPacket((*userPool)[id], *npcPool[p]);
			continue;
		}
		CSendManager::PutPlayerPacket((*userPool)[id], (*userPool)[p]);

		if ((*userPool)[p].AddList(id))
			CSendManager::PutPlayerPacket((*userPool)[p], (*userPool)[id]);
	}
	//플레이어 뷰리스트를 Swap
	PlayerViewList.erase(id);
	(*userPool)[id].SwapList(PlayerViewList);
}

void CUserProcessor::PlayerLogin(unsigned int id, char* packet)
{
	cs_packet_login* Packet = (cs_packet_login*)packet;
	char Str[30] = { 0, };
	int StrSize = WideCharToMultiByte(CP_ACP, 0, Packet->name, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, Packet->name, -1, Str, StrSize, 0, 0);
	/*이미 로그인이 되어있는지 확인한다.*/
	{
		//뭐가 더 성능이 좋은지는 실험 해봐야겠네//
		tbb::queuing_rw_mutex::scoped_lock lock(traverseLock, false);
		for (auto ID : traverseVector) {
			if (ID == id) continue;
			if (!strcmp((*userPool)[ID].GetName(), Str)) {
				CSendManager::LoginFailPacket((*userPool)[id]);
				return;
			}
		}
	}

	bool Success = CDB::GET()->GetUserData((*userPool)[id], Str);

	/*if DB Success*/
	if (Success) {
		//CSendManager::LoginSuccPacket((*userPool)[id]);
		PlayerPutIn(id);
		return;
	}
	CSendManager::LoginFailPacket((*userPool)[id]);
}

void CUserProcessor::PlayerLogOut(unsigned int id, char * packet)
{
	printf("log out\n");
	PlayerDisconnect(id);
}

void CUserProcessor::PlayerMove(unsigned int id, char * packet)
{
	cs_packet_move* Packet = (cs_packet_move*)packet;

	/*현재 섹터 확인*/
	Vec2i  CurPos = (*userPool)[id].GetPos();
	CSector* CurSector = worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE];

	/*Move*/
	switch (Packet->dir)
	{
	case DIR_UP:    if (0 < CurPos.y) (*userPool)[id].SetPos(CurPos.x, --CurPos.y); break;
	case DIR_DOWN:  if (BOARD_HEIGHT - 1 > CurPos.y) (*userPool)[id].SetPos(CurPos.x, ++CurPos.y);  break;
	case DIR_LEFT:  if (0 < CurPos.x) (*userPool)[id].SetPos(--CurPos.x, CurPos.y); break;
	case DIR_RIGHT: if (BOARD_WIDTH - 1 > CurPos.x) (*userPool)[id].SetPos(++CurPos.x, CurPos.y);  break;
	}

	CurPos = (*userPool)[id].GetPos();
	if (CurSector != worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE])
	{
		worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE]->AddList(id);
		CurSector->DeleteList(id);
	}
	/*MovePacket to OwnPlayer*/
	CSendManager::MovePlayerPacket((*userPool)[id], (*userPool)[id]);
	/*Update Viewlist*/
	UpdateViewList(id);
}

void CUserProcessor::PlayerAttack(unsigned int id, char * packet)
{
	cs_packet_attack* Packet = (cs_packet_attack*)packet;

	//printf("ID : %d 공격\n", id);
	unsigned int NewHp;
	unsigned int NewExp;

	unordered_set<unsigned int> AttackList;
	/*뷰리스트 가져온다*/
	(*userPool)[id].ReadList(AttackList);
	Vec2i CurPos = (*userPool)[id].GetPos();

	for (auto ID : AttackList)
	{
		if (!IsNPC(ID)) continue;
		if (!npcPool[ID]->GetAlive()) continue;
		if (!npcPool[ID]->GetAwake()) continue;

		if (InMySight(CurPos, npcPool[ID]->GetPos(), ATTACK_RADIUS)) // 공격 범위에 들어왔다.
		{
			NewHp = npcPool[ID]->GetHP() - (*userPool)[id].GetAtk();
			npcPool[ID]->SetHP(NewHp);
			if (NewHp <= DEAD_HP)
			{
				NewExp = (*userPool)[id].GetExp() + npcPool[ID]->GetExp();
				if (NewExp >= (*userPool)[id].GetLev() * 100)
				{
					/*LevelUpPacket*/
					unsigned int NewLev = (*userPool)[id].GetLev() + 1;
					NewExp -= (*userPool)[id].GetLev() * 100;
					(*userPool)[id].SetExp(NewExp);
					(*userPool)[id].SetLev(NewLev);
					CSendManager::LevelUpPacket((*userPool)[id]);
				}
				else /*ExpPacket*/
				{
					(*userPool)[id].SetExp(NewExp);
					CSendManager::ExpUpPacket((*userPool)[id]);
				}

				/*NPC 사후 처리*/
				std::unordered_set<CSector*> SectorList;
				std::unordered_set<unsigned int> NPCViewList;
				Vec2i CurPos = npcPool[ID]->GetPos();
				CSector* CurSector = worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE];
				CurSector->DeleteList(ID);

				npcPool[ID]->SetAlive(false);
				npcPool[ID]->SetAwake(false);

				SearchNearSector(CurPos, SectorList);

				for (auto P : SectorList) {
					P->TraverseList(CurPos, NPCViewList);
				}

				for (auto T : NPCViewList) {
					if (IsNPC(T)) continue;
					(*userPool)[T].DeleteList(ID);
					//printf("ID: %d 죽음\n", ID);
					CSendManager::RemoveNPCPacket((*userPool)[T], *npcPool[ID]);
				}
				/*Respawn Event*/
				CurSector->DeleteList(ID);
				//AddEvent(ID, NPC_RESPAWN, high_resolution_clock::now() + 3s);
			}
			else
			{
				if (npcPool[ID]->GetTarget() == NONE_TARGET)
					npcPool[ID]->SetTarget(id);
				//printf("목표 설정\n");
			}
		}
	}
}

bool CUserProcessor::AWakeNPC(unsigned int id)
{
	if (npcPool[id]->GetAwake())return false;
	if (!npcPool[id]->GetAlive()) return false;
	npcPool[id]->SetAwake(true);
	AddEvent(id, NPC_MOVE, chrono::high_resolution_clock::now() + 1s);
	return true;
}

void CUserProcessor::UpdateViewList(unsigned int id)
{
	///*NewViewList*/
	std::unordered_set<CSector*> SectorList;
	std::unordered_set<unsigned int> NewViewList;
	std::unordered_set<unsigned int> OldViewList;
	/*Old ViewList를 읽어온다*/
	(*userPool)[id].ReadList(OldViewList);
	/*자신 주변 섹터 검색*/
	SearchNearSector((*userPool)[id].GetPos(), SectorList);
	for (auto Sptr : SectorList){
		Sptr->TraverseList((*userPool)[id].GetPos(), NewViewList);
	}

	for (auto ID : NewViewList)
	{
		if (id == ID) continue;
		if (0 == OldViewList.count(ID)) //새로 들어온 ID
		{
			(*userPool)[id].AddList(ID);

			if (IsNPC(ID))
			{
				AWakeNPC(ID);
				CSendManager::PutNPCPacket((*userPool)[id], *npcPool[ID]);
			}
			else
			{
				CSendManager::PutPlayerPacket((*userPool)[id], (*userPool)[ID]);

				if ((*userPool)[ID].AddList(id)) 
				{
					CSendManager::PutPlayerPacket((*userPool)[ID], (*userPool)[id]);
				}
				else
				{
					CSendManager::MovePlayerPacket((*userPool)[ID], (*userPool)[id]);
				}
			}

		}
		else //이미 있는 ID 
		{
			if (IsNPC(ID)) 
			{ // NPC 일 경우
				CSendManager::MoveNPCPacket((*userPool)[id], *npcPool[ID]);
				continue;
			}
			if ((*userPool)[ID].AddList(id)) 
			{ //다른사람은 없다면?
				CSendManager::PutPlayerPacket((*userPool)[ID], (*userPool)[id]);
			}
			else
			{
				CSendManager::MovePlayerPacket((*userPool)[ID], (*userPool)[id]);
			}
		}
	}

	//Erase
	for (auto ID : OldViewList)
	{
		if (0 == NewViewList.count(ID)) // 내 시야 밖을 나간 ID
		{
			if ((*userPool)[id].DeleteList(ID))
			{
				if (IsNPC(ID)){
					CSendManager::RemoveNPCPacket((*userPool)[id], *npcPool[ID]);
					continue;
				}
				CSendManager::RemovePlayerPacket((*userPool)[id], (*userPool)[ID]);
				if ((*userPool)[ID].DeleteList(id)) 
				{
					CSendManager::RemovePlayerPacket((*userPool)[ID], (*userPool)[id]);
				}
			}
		}
	}

}


void CUserProcessor::PlayerDisconnect(unsigned int id)
{

	//	CDB::GET()->SaveUserData((*userPool)[id]);
	std::unordered_set<unsigned int> PlayerViewList;
	(*userPool)[id].ReadList(PlayerViewList);
	Vec2i CurPos = (*userPool)[id].GetPos();
	//(*userPool)[id].SetConnected(false);
	for (auto ID : PlayerViewList)
	{
		if (IsNPC(ID)) continue;

		if ((*userPool)[ID].GetConnected()) {
			if ((*userPool)[ID].DeleteList(id)) {
				CSendManager::RemovePlayerPacket((*userPool)[ID], (*userPool)[id]);
			}
		}

	}
	unsigned int IDX = (*userPool)[id].GetIndex();
	userPool->CloseUserInfo(id, IDX);
	(*userPool)[id].Reset();
	worldSector[CurPos.y / SECTOR_SIZE][CurPos.x / SECTOR_SIZE]->DeleteList(id);

}


void CUserProcessor::PlayerRespawn(unsigned int id)
{
	std::unordered_set<unsigned int> ClearList;
	(*userPool)[id].SwapList(ClearList);
	(*userPool)[id].SetPos(rand() % 1000, rand() % 1000);
	(*userPool)[id].SetHP(100);
	(*userPool)[id].SetAlive(true);
	PlayerPutIn(id);
}