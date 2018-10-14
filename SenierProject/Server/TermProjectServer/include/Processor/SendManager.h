#pragma once
#include "../Util/ErrorUtil.h"
#include "../Object/User.h"
#include "../Object/NPC.h"
class CSendManager
{
public:
	static void SendPacket( CUser& cl, void* ptr)
	{
		unsigned char* Packet = reinterpret_cast<unsigned char*>(ptr);
		EXOVER* Over = new EXOVER;
		memcpy(Over->IO_Buf, ptr, Packet[0]);
		Over->IO_Type = IO_SEND;
		Over->WsaBuf.buf = Over->IO_Buf;
		Over->WsaBuf.len = Packet[0];
		ZeroMemory(&Over->WSAOVER, sizeof(WSAOVERLAPPED));
		int ret = WSASend(cl.GetSock(), &Over->WsaBuf, 1, NULL, 0, &Over->WSAOVER, NULL);

		if (0 != ret)
		{
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no) ErrorUtil::Err_Display("Error In Send Packet");
		}
	}
	static void LoginSuccPacket(CUser& to)
	{
		sc_packet_logsucc Packet;
		Packet.size      = sizeof(sc_packet_logsucc);
		Packet.type      = SC_LOG_SUCC;
		Packet.id        = to.GetIndex();
		Packet.pos       = to.GetPos();
		Packet.dir       = to.GetDir();
		Packet.state     = to.GetState();
		Packet.character = to.GetCurCharacter();
		Packet.stage     = to.GetState();
		Packet.hp        = to.GetHp();
		Packet.mp        = to.GetMp();
		Packet.atk       = to.GetAtk();
		SendPacket(to, &Packet);
	}
	static void LoginFailPacket(CUser& to)
	{
		sc_packet_Logfail Packet;
		Packet.size = sizeof(sc_packet_Logfail);
		Packet.type = SC_LOG_FAIL;
		SendPacket(to, &Packet);
	}
	static void PutPlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_put_player Packet;
		Packet.size      = sizeof(sc_packet_put_player);
		Packet.type      = SC_PUT_PLAYER;
		Packet.id        = from.GetIndex();
		Packet.pos       = from.GetPos();
		Packet.dir       = from.GetDir();
		Packet.state     = from.GetState();
		Packet.character = from.GetCurCharacter();
		SendPacket(to, &Packet);
	}
	static void StatePlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_move_player Packet;

		Packet.size = sizeof(sc_packet_move_player);
		Packet.type = SC_MOVE_PLAYER;
		Packet.id = from.GetIndex();
		Packet.pos = from.GetPos();
		Packet.dir = from.GetDir();
		Packet.state = from.GetState();
		SendPacket(to, &Packet);
	}
	static void RemovePlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_remove_player Packet;

		Packet.size = sizeof(sc_packet_remove_player);
		Packet.type = SC_REMOVE_PLAYER;
		Packet.id = from.GetIndex();
		SendPacket(to, &Packet);
	}
	static void PlayerDamageStepPacket(CUser& to)
	{
		sc_packet_damagestep_player Packet;

		Packet.size = sizeof(sc_packet_damagestep_player);
		Packet.type = SC_DAMAGESTEP_PLAYER;
		Packet.id = to.GetIndex();
		Packet.hp = to.GetHp();

		SendPacket(to, &Packet);
	}
	static void PlayerAttackPacket(CUser& to, CUser& from)
	{
		sc_packet_attack_player Packet;
		Packet.size = sizeof(sc_packet_attack_player);
		Packet.type = SC_ATTACK_PLAYER;
		Packet.id = from.GetIndex();
		SendPacket(to, &Packet);
	}
	static void OtherPlayerSkillPacket(CUser& to, CUser& from)
	{
		sc_packet_other_skill Packet;
		Packet.size = sizeof(sc_packet_other_skill);
		Packet.type = SC_SKILL_PLAYER;
		Packet.id = from.GetIndex();
		SendPacket(to, &Packet);
	}
	static void OtherPlayerTelePortPacket(CUser& to, CUser& from)
	{
		sc_packet_other_teleport Packet;
		Packet.size = sizeof(sc_packet_other_teleport);
		Packet.type = SC_TELEPORT_PLAYER;
		Packet.id = from.GetIndex();
		SendPacket(to, &Packet);
	}
	static void CollisionPlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_collision_player Packet;
		Packet.size = sizeof(sc_packet_collision_player);
		Packet.type = SC_COLLISION_PLAYER;
		Packet.id = from.GetIndex();
		Packet.pos = from.GetPos();

		SendPacket(to, &Packet);
	}
	static void ChangeCharacterPlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_change_character Packet;
		Packet.size = sizeof(sc_packet_change_character);
		Packet.type = SC_CHAGNE_CHARACTER;
		Packet.id = from.GetIndex();
		Packet.num = from.GetCurCharacter();

		SendPacket(to, &Packet);
	}

	static void RespawnPlayerPacket(CUser& to)
	{
		sc_packet_respawn_player Packet;
		Packet.size = sizeof(sc_packet_respawn_player);
		Packet.type = SC_RESPAWN_PLAYER;
		Packet.id = to.GetIndex();
		Packet.hp = to.GetHp();
		Packet.pos = to.GetPos();
		Packet.dir = to.GetDir();
		Packet.state = to.GetState();
		SendPacket(to, &Packet);
	}

	static void PutNPCPacket(CUser& to,  CNPC& from)
	{
		sc_packet_put_npc Packet;
		Packet.size    = sizeof(sc_packet_put_npc);
		Packet.type    = SC_PUT_NPC;
		Packet.id      = from.GetID();
		Packet.mtype   = from.GetMType();
		Packet.pos     = from.GetPos();
		Packet.dir     = from.GetDir();
		Packet.state   = from.GetState();
		//printf("TYPE:: %d\n", from.GetMType());
		SendPacket(to, &Packet);
		
	}
	static void StateNPCPacket(CUser& to, CNPC& from)
	{
		sc_packet_state_npc Packet;
		Packet.size = sizeof(sc_packet_state_npc);
		Packet.type = SC_STATE_NPC;
		Packet.id = from.GetID();
		Packet.dir = from.GetDir();
		Packet.pos = from.GetPos();
		Packet.state = from.GetState();
		SendPacket(to, &Packet);
	}


	static void RemoveNPCPacket(CUser& to, CNPC& from)
	{  
		sc_packet_remove_npc Packet;

		Packet.size = sizeof(sc_packet_remove_npc);
		Packet.type = SC_REMOVE_NPC;
		Packet.id   = from.GetID();

		SendPacket(to, &Packet);
	}
	
};
