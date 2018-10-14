#include "UserProcessor.h"
#include "NPCProcessor.h"
#include "SendManager.h"
#include "../Util/VectorData.h"
#include "../Util/MapData.h"

CUserProcessor::CUserProcessor()
{
	UserPool = new CUserPool();
	Processor[CS_MOVE] = &CUserProcessor::PlayerMove;
	Processor[CS_ATTACK] = &CUserProcessor::PlayerAttack;
	Processor[CS_COLLISION] = &CUserProcessor::PlayerCollision;
	Processor[CS_TELEPORT] = &CUserProcessor::PlayerTelePort;
	Processor[CS_CHANGE_CHARACTER] = &CUserProcessor::PlayerChanageCharacter;
	Processor[CS_SKILL] = &CUserProcessor::PlayerSkill;
	EventProcessor[PLAYER_RESPAWN] = &CUserProcessor::PlayerRespawn;
	EventProcessor[WORRIOR_INV] = &CUserProcessor::WorriorSkill;
}

CUserProcessor::~CUserProcessor()
{
	SAFE_DELETE(UserPool);
}

bool CUserProcessor::Initialize()
{
	if (!UserPool->Initialize()) {
		printf("fail to Initialize userPool\n");
		return false;
	}
	return true;
}

bool CUserProcessor::UserSetting(HANDLE hIocp, SOCKET sc)
{
	WORD NewID = 0;
	if (!UserPool->SettingUserinfo(NewID)) return false;
	(*UserPool)[NewID].SetID(NewID);
	(*UserPool)[NewID].SetSock(sc);
	printf("NewClient [%d]\n", NewID);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(sc), hIocp, NewID, 0);

	PushTraverseVec(NewID);
	
	PlayerPutIn(NewID);
	(*UserPool)[NewID].R_Recv();
	
	return true;
}

void CUserProcessor::PlayerPutIn(WORD id)
{

	/*나에게 패킷*/
	(*UserPool)[id].SetConnected(true);
	(*UserPool)[id].SetAlive(true);
	CSendManager::LoginSuccPacket((*UserPool)[id]);
	std::unordered_set<WORD> PlayerViewList;
	std::vector<WORD> PlayerList;
	CopyTraverseVec(PlayerList);

	for (auto ID : PlayerList)
	{
		if (id == ID) continue;
		if (!(*UserPool)[ID].GetConnected()) continue;
		if ((*UserPool)[ID].GetStage() != (*UserPool)[id].GetStage()) continue;
		if (!(*UserPool)[ID].GetAlive()) continue;
		if (Vector3::InMySight((*UserPool)[id].GetPos(), (*UserPool)[ID].GetPos(), VIEW_RADIUS))
		{
			PlayerViewList.insert(ID);
			CSendManager::PutPlayerPacket((*UserPool)[id], (*UserPool)[ID]);
			if ((*UserPool)[ID].AddList(id))
				CSendManager::PutPlayerPacket((*UserPool)[ID], (*UserPool)[id]);
		}
	}

	for (auto& NPC : NPCPool)
	{
		if (!NPC->GetAlive()) continue;
		if ((*UserPool)[id].GetStage() != NPC->GetStage()) continue;
		if (Vector3::InMySight((*UserPool)[id].GetPos(), NPC->GetPos(), VIEW_RADIUS))
		{
			PlayerViewList.insert(NPC->GetID());
			AWakeNPC(NPC->GetID(), id);
			CSendManager::PutNPCPacket((*UserPool)[id], *NPC);
		}
	}
	(*UserPool)[id].SwapList(PlayerViewList);
}

void CUserProcessor::Process(WORD id, int work_size, char * io_ptr)
{
	char*Packet = (*UserPool)[id].Process(work_size, io_ptr);
	if (Packet == NULL) return;
	(this->*Processor[Packet[1]])(id, Packet);
	(*UserPool)[id].R_Recv();
}

void CUserProcessor::PlayerMove(WORD id, char * packet)
{
	cs_packet_move* Packet = (cs_packet_move*)packet;
	(*UserPool)[id].SetDir(Packet->dir);
	(*UserPool)[id].SetState(Packet->state);
	(*UserPool)[id].SetOOBB(Packet->pos);
	(*UserPool)[id].SetPos(Packet->pos);
	UpdateViewList(id);
}

void CUserProcessor::PlayerDisconnect(WORD id)
{
	std::unordered_set<WORD> CVL;
	(*UserPool)[id].ReadList(CVL);
	(*UserPool)[id].SetConnected(false);

	for (auto ID : CVL)
	{
		if (IsNPC(ID)) continue;
		if ((*UserPool)[ID].GetConnected())
		{
			if ((*UserPool)[ID].DeleteList(id)) {
				CSendManager::RemovePlayerPacket((*UserPool)[ID], (*UserPool)[id]);
			}
		}
	}
	EraseTraverseVec(id);
	unsigned int IDX = (*UserPool)[id].GetIndex();
	UserPool->CloseUserInfo(id, IDX);
	(*UserPool)[id].Reset();
}

void CUserProcessor::UpdateViewList(WORD id)
{
	std::vector<WORD> PlayerList;
	CopyTraverseVec(PlayerList);
	std::unordered_set<WORD> NewVl;
	std::unordered_set<WORD> OldVl;
	(*UserPool)[id].ReadList(OldVl);

	for (auto ID : PlayerList)
	{
		if (!(*UserPool)[ID].GetConnected()) continue;
		if ((*UserPool)[id].GetID() == (*UserPool)[ID].GetID()) continue;
		if ((*UserPool)[ID].GetStage() != (*UserPool)[id].GetStage()) continue;
		if (!(*UserPool)[ID].GetAlive()) continue;
		if (Vector3::InMySight((*UserPool)[id].GetPos(), (*UserPool)[ID].GetPos(), VIEW_RADIUS))
		{
			NewVl.insert((*UserPool)[ID].GetID());
		}
	}


	/*Add NPC*/
	for (auto& NPC : NPCPool)
	{
		if (!NPC->GetAlive()) continue;
		if ((*UserPool)[id].GetStage() != NPC->GetStage()) continue;
		if (Vector3::InMySight((*UserPool)[id].GetPos(), NPC->GetPos(), VIEW_RADIUS))
		{
			NewVl.insert(NPC->GetID());
		}
	}

	/*New ViewList Update*/
	for (auto ID : NewVl)
	{
		if (0 == OldVl.count(ID))
		{
			(*UserPool)[id].AddList(ID);
			if (IsNPC(ID)) {
				AWakeNPC(ID, id);
				CSendManager::PutNPCPacket((*UserPool)[id], *NPCPool[ID]);
			}
			else
			{
				CSendManager::PutPlayerPacket((*UserPool)[id], (*UserPool)[ID]);
				if ((*UserPool)[ID].AddList(id)) {
					CSendManager::PutPlayerPacket((*UserPool)[ID], (*UserPool)[id]);
				}
				else {
					CSendManager::StatePlayerPacket((*UserPool)[ID], (*UserPool)[id]);
				}
			}

		}
		else
		{
			if (IsNPC(ID))
			{
				CSendManager::StateNPCPacket((*UserPool)[id], *NPCPool[ID]);
				continue;
			}
			if ((*UserPool)[ID].AddList(id)) {
				CSendManager::PutPlayerPacket((*UserPool)[ID], (*UserPool)[id]);
			}
			else {
				CSendManager::StatePlayerPacket((*UserPool)[ID], (*UserPool)[id]);
			}
		}
	}

	/*Old ViewList Update*/
	for (auto ID : OldVl)
	{
		if (0 == NewVl.count(ID))
		{
			if ((*UserPool)[id].DeleteList(ID))
			{
				if (IsNPC(ID))
				{
					CSendManager::RemoveNPCPacket((*UserPool)[id], *NPCPool[ID]);
					continue;
				}
				else
				{
					if ((*UserPool)[ID].DeleteList(id))
						CSendManager::RemovePlayerPacket((*UserPool)[ID], (*UserPool)[id]);
				}
			}

		}
	}
}



void CUserProcessor::PlayerAttack(WORD id, char * packet)
{
	cs_packet_attack* Packet = (cs_packet_attack*)packet;
	(*UserPool)[id].SetState(ATTACKSTATE);

	/*맞은 NPC 리스트와*/
	/*플레이어 Viewlist*/
	std::vector<WORD> HeatedNPCList;
	std::unordered_set<WORD> CVL;
	(*UserPool)[id].ReadList(CVL);
	
	/*공격 충돌체*/
	BoundingOrientedBox AttackOOBB(Vector3::Add((*UserPool)[id].GetPos(),
		CVData::GET_SINGLE()->LookVectorMap[(*UserPool)[id].GetDir()], 5.f),
		XMFLOAT3(2.f, 1.f, 5.f), XMFLOAT4(0, 0, 0, 1));


	for (auto ID : CVL)
	{
		if (!IsNPC(ID)){
			CSendManager::PlayerAttackPacket((*UserPool)[ID], (*UserPool)[id]);
			continue;
		}

		if (NPCPool[ID]->CheckCollision(AttackOOBB))
		{
			float CurNPCHP = NPCPool[ID]->GetHp() - (*UserPool)[id].GetAtk();
			NPCPool[ID]->SetHp(CurNPCHP);
			if (CurNPCHP <= 0.f)
			{
				NPCPool[ID]->SetAlive(false);
				NPCPool[ID]->SetState(DEADSTATE);  
			}
			else
			{
				NPCPool[ID]->SetState(HEATSTATE);
			}
			HeatedNPCList.push_back(ID);
		}
	}

	std::vector<WORD> UserList;
	CopyTraverseVec(UserList);
	for (auto NPC : HeatedNPCList)
	{
		for (auto ID : UserList)
		{
			if (Vector3::InMySight((*UserPool)[ID].GetPos(), NPCPool[NPC]->GetPos(), VIEW_RADIUS))
				CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[NPC]);
		}

		if (NPCPool[NPC]->GetState() == HEATSTATE)
		{
			if (NPCPool[NPC]->GetTarget() == NONE_TARGET) NPCPool[NPC]->SetTarget(id);
			NPCPool[NPC]->SetState(MOVESTATE);
		}
	}
}

void CUserProcessor::PlayerCollision(WORD id, char * packet)
{
	cs_packet_collision* Packet = (cs_packet_collision*)packet;
	(*UserPool)[id].SetPos(Packet->pos);
	unordered_set<WORD> PlayerViewList;
	(*UserPool)[id].ReadList(PlayerViewList);

	for (auto& ID : PlayerViewList) {
		if (IsNPC(ID)) continue;
		CSendManager::CollisionPlayerPacket((*UserPool)[ID], (*UserPool)[id]);
	}
}

void CUserProcessor::PlayerChanageCharacter(WORD id, char * packet)
{
	cs_packet_change_character* Packet = (cs_packet_change_character*)packet;
	(*UserPool)[id].SetCurCharacter(Packet->num);

	unordered_set<WORD> CVL;
	(*UserPool)[id].ReadList(CVL);

	for (auto& ID : CVL)
	{
		if (IsNPC(ID)) continue;
		CSendManager::ChangeCharacterPlayerPacket((*UserPool)[ID], (*UserPool)[id]);
	}
}

void CUserProcessor::PlayerSkill(WORD id, char * packet)
{
	cs_packet_skill* Packet = (cs_packet_skill*)packet;
	(*UserPool)[id].SetPos(Packet->pos);
	(*UserPool)[id].SetDir(Packet->dir);

	unordered_set<WORD> PVl;
	(*UserPool)[id].ReadList(PVl);

	switch ((*UserPool)[id].GetCurCharacter())
	{
	case CHARACTER::WARRIOR:
	{
		(*UserPool)[id].SetINV(true);
		AddEvent(id, WORRIOR_INV, chrono::high_resolution_clock::now() + 3s);
		break;
	}
	case CHARACTER::PAIRY:
	{
		BoundingOrientedBox AttackOOBB(Vector3::Add((*UserPool)[id].GetPos(), CVData::GET_SINGLE()->LookVectorMap[(*UserPool)[id].GetDir()], 5.f),
			XMFLOAT3(2.f, 1.f, 5.f), XMFLOAT4(0, 0, 0, 1));
		unordered_set<WORD> DeadList;
		for (auto ID : PVl)
		{
			if (IsNPC(ID))
			{
				if (NPCPool[ID]->CheckCollision(AttackOOBB))
				{
					NPCPool[ID]->SetState(DEADSTATE);
					NPCPool[ID]->SetAlive(false);
					NPCPool[ID]->SetAwake(false);
					DeadList.insert(ID);
				}

			}
		}

		for (auto NPC : DeadList)
		{
			for (auto ID : PVl)
			{
				if (IsNPC(ID)) continue;
				CSendManager::StateNPCPacket((*UserPool)[ID], *NPCPool[NPC]);
			}
		}
		break;
	}
	case CHARACTER::GOAT:
	{

		break;
	}
	}


	for (auto ID : PVl)
	{
		if (IsNPC(ID)) continue;
		CSendManager::OtherPlayerSkillPacket((*UserPool)[ID], (*UserPool)[id]);
	}
}

void CUserProcessor::PlayerTelePort(WORD id, char * packet)
{
	cs_packet_teleport* Packet = (cs_packet_teleport*)packet;
	XMFLOAT3 NewPos = Vector3::Add((*UserPool)[id].GetPos(), CVData::GET_SINGLE()->LookVectorMap[(*UserPool)[id].GetDir()], 15.f);

	BoundingOrientedBox OOBB(NewPos, XMFLOAT3(2.f, 2.f, 10.f), XMFLOAT4(0, 0, 0, 1));
	for (int i = 0; i < CMapData::GET_SINGLE()->MapBoundingBoxes[(*UserPool)[id].GetStage()].size(); ++i) {
		auto& Collider = CMapData::GET_SINGLE()->MapBoundingBoxes[(*UserPool)[id].GetStage()][i];
		ContainmentType Type = Collider.Contains(OOBB);
		if (Type == INTERSECTS || Type == CONTAINS) return;
	}
	(*UserPool)[id].SetPos(NewPos);
	(*UserPool)[id].SetOOBB(NewPos);
	unordered_set<WORD> vl;
	(*UserPool)[id].ReadList(vl);
	for (auto& ID : vl) {
		if (IsNPC(ID)) continue;
		CSendManager::OtherPlayerTelePortPacket((*UserPool)[ID], (*UserPool)[id]);
	}
	UpdateViewList(id);
	CSendManager::StatePlayerPacket((*UserPool)[id], (*UserPool)[id]);

	
}

void CUserProcessor::AWakeNPC(WORD id, WORD target)
{
	if (!NPCPool[id]->GetAwake())
	{
		NPCPool[id]->SetAwake(true);
		switch (NPCPool[id]->GetMType())
		{
		case MONSTER::STONEMON:
		{
			AddEvent(id, NPC_IDLE, chrono::high_resolution_clock::now() + 1s);
			break;
		}
		default:
		{
			NPCPool[id]->SetTarget(target);
			AddEvent(id, NPC_MOVE, chrono::high_resolution_clock::now() + 1s);
			break;
		}
		}
	}
}

void CUserProcessor::Simulation(float fDeltaTime)
{

}

void CUserProcessor::PlayerRespawn(WORD id)
{
	(*UserPool)[id].SetHp(500.f);
	(*UserPool)[id].SetMp(500.f);
	(*UserPool)[id].SetState(IDLESTATE);
	(*UserPool)[id].SetPos(XMFLOAT3(60.f, 0.f, -60.f));
	(*UserPool)[id].SetAlive(true);
	CSendManager::RespawnPlayerPacket((*UserPool)[id]);
	CUserProcessor::UpdateViewList(id);
}

void CUserProcessor::WorriorSkill(WORD id)
{
	(*UserPool)[id].SetINV(false);
}
