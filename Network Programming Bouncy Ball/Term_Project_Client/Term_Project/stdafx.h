#pragma once 

#include <WinSock2.h>
#include "targetver.h"
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <math.h>


#include <gl/glut.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>
#include <vector>
#include <list>
#include <chrono>
#include <windows.h>
#include "SOUND\fmod.hpp"

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"

#define BOARDMAX 20 
#define PARTICLENUM 60
#define SOUNDNUM 4
#define PLAYERNUM 3 
#pragma comment(lib, "User32.lib")
#pragma comment (lib,"ws2_32")
#pragma comment (lib, "SOUND\\fmodex_vc.lib")

#define Pi 3.141592653589793238462643383279



using namespace FMOD;

typedef struct BOARD {
	double x[2], y, z[2];
	double range;
	double m_range;
	bool mark;
	bool move;
	bool clicked;
	int item;
	int texture;
};

typedef struct OBJECT {
	double x, y, z;
	bool state;
};



typedef struct CLIENT_STRUCT
{
	OBJECT position; // 공의 위치 값
	bool change; // 공이 죽었을 시 화면전환
};


typedef struct SERVER_STRUCT
{
	OBJECT A[3]; // 새로운 위치 값
	bool effect[11]; // 5가지 효과에 대한 상태
	bool sound[12]; // 4가지 사운드의 On/ Off
	BOARD Board[20];
	int pIndex;
	int pcount;
};

enum PLAYER
{
	A = 0,
	B = 1,
	C = 2
};
enum EFFECT
{
	EFFECT_FIRST = 0,
	FOG = 0,
	STOP = 1,
	FIRST_NO_ALPHA,
	FIRST_SLOW,
	FIRST_NO_DIE,
	SECOND_NO_ALPHA,
	SECOND_SLOW,
	SECOND_NO_DIE,
	THIRD_NO_ALPHA,
	THIRD_SLOW,
	THIRD_NO_DIE,
	EFFECT_LAST = 10
};
enum SOUND
{
	SOUND_FIRST = 0,
	FIRST_PONG = 0,
	FIRST_GET_ITEM = 1,
	FIRST_EXPLOSION = 2,
	FIRST_BLANK = 3,
	SECOND_PONG = 4,
	SECOND_GET_ITEM = 5,
	SECOND_EXPLOSION = 6,
	SECOND_BLANK = 7,
	THIRD_PONG = 8,
	THIRD_GET_ITEM = 9,
	THIRD_EXPLOSION = 10,
	THIRD_BLANK = 11,
	SOUND_LAST = 11
};

#include "GameObject.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
