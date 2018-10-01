#pragma once
#include "Texture.h"
#include "stdafx.h"
class Etc;
class Ball;
class Board;
class Particle;
class GameSound;

class GameObject
{
private:
	int startstate = 1;
	int board_speed = 1;
	SERVER_STRUCT Sbuffer;
	int pIndex;

public:
	Texture* t;
	Particle* p;
	GameSound* s;

	GameObject();
	~GameObject();

	


	//---- 메뉴 관련 함수.  
	void Draw_Menu();
	void SetStartState(int i) { startstate = i; }
	int GetStartState() { return startstate; }

	// recv한 서버구조체를 넘겨받는다. 
	void SetSBuffer(SERVER_STRUCT&  SBUFFER);
	
	//
	SERVER_STRUCT GetSBuffer() { return Sbuffer; }

	CLIENT_STRUCT GetCBuffer(int num, bool c)
	{
		CLIENT_STRUCT Temp; 
		Temp.position = Sbuffer.A[num];
		Temp.change = c; 
		return Temp;
	}

	// Player A B C 의 생사여부 변수를 리턴받는다. 
	bool GetBallState(int num) { return Sbuffer.A[num].state; }

	//안개 적용 여부 리턴 
	bool GetFogSwitch() { return Sbuffer.effect[FOG]; }

	//Ball  키 입력 
	void  PlayerMove();

	/**안개그리기*/
	void Draw_Fog();

	/**보드 그리기*/
	void Draw_Board();

	/**아이템 박스그리기*/
	void  Draw_ItemBox(int effect);
	
	/**공 그리기*/
	void Draw_Ball();
	
	/**아이템 정보 그리기*/
	void Draw_Inform(int num);

	void SoundPlay();
  
};

class Particle
{
private:
	OBJECT p[PARTICLENUM];
public:
	Particle();
	~Particle();

	void Init();
	void Draw();
	void Update();
};

//_____________________________________________sound 
class GameSound
{
private:
	System* pSystem;
	Sound *pSound[6];
	Channel *pChannel[6];
public:
	GameSound();
	~GameSound();
	void Play_GameSound(int i);
};