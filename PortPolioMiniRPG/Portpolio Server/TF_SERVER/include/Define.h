#pragma once
#define MAX_BUFF_SIZE    6000
#define MAX_PACKET_SIZE  255
#define MAX_STR_SIZE     50

#define VIEW_RADIUS      5      
#define ATTACK_RADIUS    1
#define DEAD_HP          0
#define NONE_TARGET      -1
#define MAX_USER		 8001      
#define NUM_OF_NPC       10000
#define SECTOR_SIZE        60
#define MY_SERVER_PORT   9000

#define DIR_UP     0
#define DIR_DOWN   1
#define DIR_RIGHT  2
#define DIR_LEFT   3
#define BOARD_WIDTH      1000
#define BOARD_HEIGHT     1000

//IO_TYPE
enum IO_TYPE
{
	IO_RECV,
	IO_SEND,
	IO_EVENT
}; 

enum EV_TYPE
{   
	NPC_IDLE,
	NPC_MOVE,
	NPC_TRACE,
	NPC_ATTACK,
	NPC_RESPAWN,
	PLAYER_RESPAWN
};

#define SAFE_DELETE(p) if(p)  {delete p; p =NULL;}
#define SAFE_DELETE_ARRAY(p) if(p) {delete[] p ; p = NULL;}
#define SAFE_RELEASE(p) if(p) { p->Release();  p =NULL;}

template<typename T>
void Safe_Delete_VecList(T& p)
{
	for (auto& iter : p)
		delete &iter;
		//SAFE_DELETE(iter);
	p.clear();
}

template<typename T>
void Safe_Delete_Map(T& p)
{
	for (auto& iter : p)
		SAFE_DELETE(iter->second);
	p.clear();
}

