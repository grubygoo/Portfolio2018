#pragma once
#include "../Error/ErrorUtil.h"
#include "../Object/User.h"
#include "../Object/NPC.h"
class CSendManager
{
public:
	static void SendPacket(CUser& cl, void* ptr)
	{
		unsigned char* Packet = reinterpret_cast<unsigned char*>(ptr);
		EXOVER* Over = new EXOVER;
		memcpy(Over->IO_Buf, ptr, Packet[0]);
		Over->IO_Type = IO_SEND;
		Over->WsaBuf.buf = Over->IO_Buf;
		Over->WsaBuf.len = Packet[0];
		ZeroMemory(&Over->WSAOVER, sizeof(WSAOVERLAPPED));
		int ret = WSASend(cl.GetSock(), &Over->WsaBuf, 1, NULL, 0, &Over->WSAOVER, NULL);
		if (0 != ret){
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				ErrorUtil::Err_Display("Error In Send Packet");
		}
	}
	static void LoginSuccPacket(CUser& to)
	{
		sc_packet_logsucc Packet;
		Packet.size = sizeof(sc_packet_logsucc);
		Packet.type = SC_LOG_SUCC;
		Packet.id    = to.GetID();
		Packet.index = to.GetIndex();
		Packet.x     = to.GetPos().x;
		Packet.y     = to.GetPos().y;
		Packet.hp    = to.GetHP();
		Packet.mp    = to.GetMP();
		Packet.lev   = to.GetLev();
		Packet.exp   = to.GetExp();
		Packet.atk   = to.GetAtk();

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
		Packet.size  = sizeof(sc_packet_put_player);
		Packet.type  = SC_PUT_PLAYER;
		Packet.id    = from.GetID();
		Packet.index = from.GetIndex();
		Packet.x     = from.GetPos().x;
		Packet.y     = from.GetPos().y;
		SendPacket(to, &Packet);
	}
	static void MovePlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_move_player Packet;

		Packet.size  = sizeof(sc_packet_move_player);
		Packet.type  = SC_MOVE_PLAYER;
		Packet.index = from.GetIndex();
		Packet.x     = from.GetPos().x;
		Packet.y     = from.GetPos().y;
		SendPacket(to, &Packet);
	}
	static void RemovePlayerPacket(CUser& to, CUser& from)
	{
		sc_packet_remove_player Packet;

		Packet.size  = sizeof(sc_packet_remove_player);
		Packet.type  = SC_REMOVE_PLAYER;
		Packet.index = from.GetIndex();
		SendPacket(to, &Packet);
	}
	
	static void PutNPCPacket(CUser& to, CNPC& from)
	{  
		//printf("Put NPC\n");
		sc_packet_put_npc Packet;
		Packet.size = sizeof(sc_packet_put_npc);
		Packet.type = SC_PUT_NPC;
		Packet.id = from.GetID();
		Packet.x = from.GetPos().x;
		Packet.y = from.GetPos().y;
		SendPacket(to, &Packet);

	}
	static void MoveNPCPacket(CUser& to, CNPC& from)
	{
		sc_packet_move_npc Packet;
		Packet.size = sizeof(sc_packet_move_npc);
		Packet.type = SC_MOVE_NPC;
		Packet.id = from.GetID();
		Packet.x = from.GetPos().x;
		Packet.y = from.GetPos().y;
		SendPacket(to, &Packet);
	}
	static void RemoveNPCPacket(CUser& to, CNPC& from)
	{
		sc_packet_remove_npc Packet;

		Packet.size = sizeof(sc_packet_remove_npc);
		Packet.type = SC_REMOVE_NPC;
		Packet.id = from.GetID();

		SendPacket(to, &Packet);
	}

	static void HeatedPlayerPacket(CUser& to)
	{
		sc_packet_heated_player Packet;

		Packet.size = sizeof(sc_packet_heated_player);
		Packet.type = SC_HEATED_PLAYER;
		Packet.idx  = to.GetIndex();
		Packet.hp   = to.GetHP();
		SendPacket(to, &Packet);
	}
	static void ExpUpPacket(CUser& to)
	{
		sc_packet_exp_up_player Packet;
		Packet.size = sizeof(sc_packet_exp_up_player);
		Packet.type = SC_EXP_UP_PLAYER;
		Packet.exp = to.GetExp();
		SendPacket(to, &Packet);
	}
	static void LevelUpPacket(CUser& to)
	{
		sc_packet_level_up_player Packet;

		Packet.size = sizeof(sc_packet_level_up_player);
		Packet.type = SC_LEVEL_UP_PLAYER;

		SendPacket(to, &Packet);
	}

};
