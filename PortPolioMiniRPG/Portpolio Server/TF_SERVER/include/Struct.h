#pragma once

typedef struct _Vec2i
{
	int  x, y;
public:
	const _Vec2i& operator /(int oper)
	{
		x /= oper;
		y /= oper;
		return *this;
	}
	const _Vec2i& operator +=(int oper) {
		x += oper;
		y += oper;
		return *this;
	}
	const _Vec2i& operator -=(int oper) {
		x -= oper;
		y -= oper;
		return *this;
	}


	const bool operator ==(const _Vec2i& other)
	{
		return (x == other.x && y == other.y);
	}

	_Vec2i operator + (const _Vec2i& other)
	{
		return { x + other.x, y + other.y };
	}


}Vec2i, *PVec2i;

typedef struct _OBJECT_DATA
{
public:
	unsigned int           id;
	unsigned int           lv;
	unsigned int           hp;
	unsigned int           mp;
	unsigned int          atk;
	unsigned int          exp;
	bool                alive;
	Vec2i                   pos;
public:
	_OBJECT_DATA()
	{
		id = -1;
		lv = 1;
		alive = true;
		hp = 0;
		mp = 0;
		atk = 0;
		exp = 0;
	}
}OBJECT_DATA, *POBJECT_DATA;

typedef struct _NPC_EVENT
{
	chrono::high_resolution_clock::time_point   W_Time;
	EV_TYPE                                     Type;
	unsigned int                                NPC_ID;
}NPC_EVENT, *PNPC_EVENT;

typedef struct _EXOVER
{
	WSAOVERLAPPED  WSAOVER;
	IO_TYPE        IO_Type;
	EV_TYPE        EV_Type;
	WSABUF         WsaBuf;
	char           IO_Buf[MAX_BUFF_SIZE];
}EXOVER, *PEXOVER;
