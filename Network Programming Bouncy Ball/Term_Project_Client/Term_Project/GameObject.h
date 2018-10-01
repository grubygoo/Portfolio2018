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

	


	//---- �޴� ���� �Լ�.  
	void Draw_Menu();
	void SetStartState(int i) { startstate = i; }
	int GetStartState() { return startstate; }

	// recv�� ��������ü�� �Ѱܹ޴´�. 
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

	// Player A B C �� ���翩�� ������ ���Ϲ޴´�. 
	bool GetBallState(int num) { return Sbuffer.A[num].state; }

	//�Ȱ� ���� ���� ���� 
	bool GetFogSwitch() { return Sbuffer.effect[FOG]; }

	//Ball  Ű �Է� 
	void  PlayerMove();

	/**�Ȱ��׸���*/
	void Draw_Fog();

	/**���� �׸���*/
	void Draw_Board();

	/**������ �ڽ��׸���*/
	void  Draw_ItemBox(int effect);
	
	/**�� �׸���*/
	void Draw_Ball();
	
	/**������ ���� �׸���*/
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