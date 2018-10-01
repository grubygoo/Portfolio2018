#pragma once 
enum CS_PROTOCOL
{
	CS_LOGIN = 1,
	CS_LOGOUT,
	CS_MOVE,
	CS_ATTACK,
	CS_SKILL
};

enum SC_PROTOCOL
{
	SC_LOG_SUCC = 1,
	SC_LOG_FAIL,
	SC_PUT_PLAYER,
	SC_PUT_NPC,
	SC_MOVE_PLAYER,
	SC_MOVE_NPC,
	SC_REMOVE_PLAYER,
	SC_REMOVE_NPC,
	SC_HEATED_PLAYER,
	SC_DEAD_PLAYER,
	SC_EXP_UP_PLAYER,
	SC_LEVEL_UP_PLAYER
};

#pragma pack (push, 1)
#pragma region CLIENT_PROTOCOL
struct cs_packet_login /*서버와 Connet후 DB 연결위해*/
{
	BYTE size;
	BYTE type;
	WCHAR name[MAX_STR_SIZE];
};

struct cs_packet_logout /*LogOut 했다는 Packet*/
{
	BYTE size;
	BYTE type;
};

struct cs_packet_move  /*Move Packet*/
{
	BYTE     size;
	BYTE     type;
	BYTE      dir;

};
struct cs_packet_chat
{
	BYTE size;
	BYTE type;
	WCHAR message[256];
};
struct cs_packet_attack
{
	BYTE    size;
	BYTE    type;
};
#pragma endregion CLIENT_PROTOCOL


#pragma region  SERVER_PROTOCOL
struct sc_packet_logsucc
{
	BYTE          size;
	BYTE          type;
	unsigned int    id;
	unsigned int index;
	int              x;
	int              y;
	unsigned int    hp;
	unsigned int    mp;
	unsigned int   lev;
	unsigned int   exp;
	unsigned int   atk;
};

struct sc_packet_Logfail
{
	BYTE    size;
	BYTE    type;
};

struct sc_packet_put_player
{
	BYTE          size;
	BYTE          type;
	unsigned int    id;
	unsigned int index;
	int x;
	int y;
};

struct sc_packet_put_npc
{
	BYTE       size;
	BYTE       type;
	unsigned int id;
	int x;
	int y;
};

struct sc_packet_move_player
{
	BYTE          size;
	BYTE          type;
	unsigned int index;
	int x;
	int y;
};

struct sc_packet_move_npc
{
	BYTE       size;
	BYTE       type;
	unsigned int id;
	int x;
	int y;
};

struct sc_packet_remove_player
{
	BYTE          size;
	BYTE          type;
	unsigned int index;
};

struct sc_packet_remove_npc
{
	BYTE       size;
	BYTE       type;
	unsigned int id;
};

struct sc_packet_heated_player
{
	BYTE          size;
	BYTE          type;
	unsigned int   idx;
	unsigned int    hp;
};

struct sc_packet_dead_player
{
	BYTE          size;
	BYTE          type;
	unsigned int index;
};

struct sc_packet_exp_up_player
{
	BYTE          size;
	BYTE          type;
	unsigned int   exp;
};

struct sc_packet_level_up_player
{
	BYTE          size;
	BYTE          type;
};
#pragma endregion  SERVER_PROTOCOL
#pragma pack (pop)