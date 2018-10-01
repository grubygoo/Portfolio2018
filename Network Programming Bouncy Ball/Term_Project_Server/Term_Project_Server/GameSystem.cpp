#include "stdafx.h"
#include "GameSystem.h"
#define BALLSPEED 0.06f;

GameSystem::GameSystem(SERVER_STRUCT& board) : sbuffer(board)
{
	start = 0;

	sbuffer.effect[FOG] = false;
	sbuffer.effect[STOP] = false;

	sbuffer.effect[FIRST_NO_ALPHA] = false;
	sbuffer.effect[FIRST_SLOW] = false;
	sbuffer.effect[FIRST_NO_DIE] = false;

	sbuffer.effect[SECOND_NO_ALPHA] = false;
	sbuffer.effect[SECOND_SLOW] = false;
	sbuffer.effect[SECOND_NO_DIE] = false;

	sbuffer.effect[THIRD_NO_ALPHA] = false;
	sbuffer.effect[THIRD_SLOW] = false;
	sbuffer.effect[THIRD_NO_DIE] = false;

	sbuffer.sound[FIRST_PONG] = false;
	sbuffer.sound[FIRST_GET_ITEM] = false;
	sbuffer.sound[FIRST_EXPLOSION] = false;
	sbuffer.sound[FIRST_BLANK] = false;

	sbuffer.sound[SECOND_PONG] = false;
	sbuffer.sound[SECOND_GET_ITEM] = false;
	sbuffer.sound[SECOND_EXPLOSION] = false;
	sbuffer.sound[SECOND_BLANK] = false;

	sbuffer.sound[THIRD_PONG] = false;
	sbuffer.sound[THIRD_GET_ITEM] = false;
	sbuffer.sound[THIRD_EXPLOSION] = false;
	sbuffer.sound[THIRD_BLANK] = false;

	int i = 0;

	BuildBoard(-20, -20, 0, 10, 20, false, 0, 0, i++);
	BuildBoard(-20, -70, 10, 20, -30, false, 0, 1, i++);
	BuildBoard(-20, -120, 20, 20, -80, true, 20, 0, i++);
	BuildBoard(-20, -170, 10, 20, -130, false, 0, 0, i++);
	BuildBoard(-35, -205, 0, -5, -165, false, 0, 0, i++);

	BuildBoard(-20, -235, 0, 20, -215, false, 0, 0, i++);
	BuildBoard(-20, -265, 10, 20, -245, false, 0, 1, i++);
	BuildBoard(-20, -295, 20, 20, -275, true, 20, 0, i++);
	BuildBoard(-20, -325, 10, 20, -305, false, 0, 0, i++);
	BuildBoard(-35, -355, 0, -5, -335, false, 0, 0, i++);

	BuildBoard(-20, -385, 0, 20, -365, false, 0, 0, i++);
	BuildBoard(-20, -415, 10, 20, -395, false, 0, 1, i++);
	BuildBoard(-20, -445, 20, 20, -425, true, 20, 0, i++);
	BuildBoard(-20, -475, 10, 20, -455, false, 0, 0, i++);
	BuildBoard(-35, -505, 0, -5, -485, false, 0, 0, i++);

	BuildBoard(-20, -535, 0, 20, -515, false, 0, 0, i++);
	BuildBoard(-20, -565, 10, 20, -545, false, 0, 2, i++);
	BuildBoard(-20, -595, 20, 20, -575, true, 20, 0, i++);
	BuildBoard(-20, -635, 10, 20, -595, false, 0, 0, i++);
	BuildBoard(-35, -665, 0, -5, -625, false, 0, 0, i++);

	for (int i = 0; i < nPlayer; i++)
	{
		sbuffer.A[i].state = true;
		sbuffer.A[i].x = i * 10 - 10;
		sbuffer.A[i].y = 60 - i * 10;
		sbuffer.A[i].z = 0;
		fx[i] = 0.0;

		height_buffer[i] = 0;
		ballspeed[i] = BALLSPEED;
		balltop[i] = false;
	}

	board_speed = 1.0f;
}


GameSystem::~GameSystem()
{
}

void GameSystem::MoveBoard()
{
	static float stoptime = 30.0f;

	if (sbuffer.effect[STOP])
		stoptime -= 0.1f;

	if (stoptime <= 0.0f && sbuffer.effect[STOP])
	{
		sbuffer.effect[STOP] = false;
		stoptime = 30.0f;
	}

	for (int i = 0; i < BoardMax; i++)
	{
		if (sbuffer.B[i].move && !sbuffer.effect[STOP])
		{
			if (sbuffer.B[i].m_range > sbuffer.B[i].range)
			{
				sbuffer.B[i].mark = false;
			}
			else if (sbuffer.B[i].m_range < -sbuffer.B[i].range)
			{
				sbuffer.B[i].mark = true;
			}
			if (sbuffer.B[i].mark)
			{
				sbuffer.B[i].x[0] += board_speed;
				sbuffer.B[i].x[1] += board_speed;
				sbuffer.B[i].m_range += board_speed;
			}
			else if (!sbuffer.B[i].mark)
			{
				sbuffer.B[i].x[0] -= board_speed;
				sbuffer.B[i].x[1] -= board_speed;
				sbuffer.B[i].m_range -= board_speed;
			}
		}
	}
}

void GameSystem::SetBalls(CLIENT_STRUCT& buf, int num)
{
	cbuffer[num] = buf;
}
void GameSystem::MakeBoard()
{
	BOARD newBoard;

	int lastNum = start - 1;

	if (lastNum < 0)
		lastNum += BoardMax;

	newBoard.x[0] = sbuffer.B[lastNum].x[0] + rand() % 160 - 80;
	newBoard.x[1] = newBoard.x[0] + rand() % 10 + 30;
	newBoard.z[0] = sbuffer.B[lastNum].z[0] - (rand() % 10 + 50);
	newBoard.z[1] = newBoard.z[0] + rand() % 10 + 30;
	newBoard.y = sbuffer.B[lastNum].y + rand() % 20 - 10;
	newBoard.m_range = 0;
	newBoard.texture = rand() % 8;
	if (rand() % 5 == 0)
		newBoard.move = true;
	else
		newBoard.move = false;

	newBoard.range = rand() % 20 + 20;

	if (rand() % 2 == 0)
		newBoard.mark = false;
	else
		newBoard.mark = true;

	int itemRate = rand() % 10;

	if (itemRate == 0)
		newBoard.item = 1;
	else if (itemRate == 1)
		newBoard.item = 2;
	else
		newBoard.item = 0;

	sbuffer.B[start] = newBoard;
	if (start < BoardMax - 2)
		start++;
	else
		start = 0;
}

void GameSystem::BallUpdate(int num)
{
	sbuffer.A[num] = cbuffer[num].position;
	static double ballspeed_timer = 5.0;
	static double ballpower_timer = 5.0;
	static double balldisappear_timer =5.0;
	static double fog_timer = 5.0;

	BOARD *NOW = GetBOARD(sbuffer.A[num].x, sbuffer.A[num].z);

	if (FallBall(num) == 0 && sbuffer.A[num].state)
	{
	BACK:
		fx[num] += ballspeed[num];
		sbuffer.A[num].y = height_buffer[num] - 60.0f*fx[num] *fx[num] + 60.0f;

		if (sbuffer.A[num].y >= height_buffer[num] + 59.0)
		{
			balltop[num] = true;
		}
		else
		{
			balltop[num] = false;
		}

		if (sbuffer.A[num].y <= height_buffer[num] + 0.06)
		{
			sbuffer.sound[4 * num] = true;
		}
		else
		{
			sbuffer.sound[4 * num] = false;
		}
		if (!(sbuffer.effect[3 * num + 4])) {
			if ((FallBall(num) == 2 && sbuffer.A[num].state) || sbuffer.A[num].y <= height_buffer[num] - 200) // 떨어졌는지
			{
				sbuffer.A[num].state = false;
			}
		}
		if (GetItem(num) && sbuffer.A[num].state)// 이벤트 박스 충돌
		{
			sbuffer.sound[4 * num + 1] = true;
			if (NOW->item == 1) {
				ItemEffect(rand() % 4 + 1, num);
				NOW->item = 0;
			}
			else if (NOW->item == 2)
			{
				ItemEffect(rand() % 3 + 4, num);
				NOW->item = 0;
			}
		}
		else
		{
			sbuffer.sound[4 * num + 1] = false;
		}
		if (FallBall(num) == 1) // 공이 튕길때
		{
			sbuffer.sound[4 * num] = true;
		}
		else
		{
			sbuffer.sound[4 * num] = false;
		}
	}
	else if (sbuffer.A[num].state &&  FallBall(num) == 1)
	{
		fx[num] = -1.0;
		if (NOW != NULL) {
			height_buffer[num] = NOW->y;
		}
		goto BACK;
	}
	else
	{
		sbuffer.A[num].y -= 2.0;
	}

	if (sbuffer.effect[3 * num + 3] || ballspeed[num] == 0.02f * 0.5f)
	{
		ballspeed_timer -= 0.1;
		if (ballspeed_timer <= 0.0)
		{
			ballspeed[num] = BALLSPEED;
			ballspeed_timer = 5.0;
			sbuffer.effect[3 * num + 3] = false;
		}
	}

	if (sbuffer.effect[3 * num + 4])
	{
		ballpower_timer -= 0.1;
		if (ballpower_timer <= 0.0)
		{
			ballpower_timer = 5.0;
			sbuffer.effect[3 * num + 4] = false;
		}
	}

	if (sbuffer.effect[3 * num + 2])
	{
		balldisappear_timer -= 0.1;
		if (balldisappear_timer <= 0.0)
		{
			ballpower_timer = 5.0;
			sbuffer.effect[3 * num + 2] = false;
		}
	}

	if (sbuffer.effect[FOG])
	{
		fog_timer -= 0.1;
		if (fog_timer <= 0.0)
		{
			fog_timer = 5.0;
			sbuffer.effect[FOG] = false;
		}
	}
	if (CheckPosition(num))
	{
		MakeBoard();
	}


	if (!(sbuffer.effect[FOG]) && fog_timer < 5.0)
	{
		fog_timer = 5.0;
	}
	if (!(sbuffer.effect[3 * num + 2]) && balldisappear_timer < 5.0)
	{
		balldisappear_timer = 5.0;
	}
	if (!(sbuffer.effect[3 * num + 4]) && ballpower_timer < 5.0)
	{
		ballpower_timer = 5.0;
	}
	if (!(sbuffer.effect[3 * num + 3]) && ballspeed_timer < 5.0)
	{
		ballspeed_timer = 5.0;
	}
}
int GameSystem::FallBall(int num)
{

	BOARD *A = GetBOARD(sbuffer.A[num].x, sbuffer.A[num].z);

	if (A == NULL)
	{
		if (height_buffer[num] <  (sbuffer.A[num].y))//공이 공중에 있음
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else {
		if (A->y <  sbuffer.A[num].y)//공이 공중에 있음
		{
			return 0;
		}
		else
		{
			if ((A->x[0] <= sbuffer.A[num].x && A->x[1] >= sbuffer.A[num].x) && (A->z[0] - 5 <= sbuffer.A[num].z && A->z[1] + 5 >= sbuffer.A[num].z))
			{//공이 판에 맞음
				if (A->y + 8.0 >= sbuffer.A[num].y) {
					return 1;
				}
			}
		}
		return 2;//공이 바닥으로 떨어짐
	}
}
bool GameSystem::CheckPosition(int num)
{
	int n = start - 3;

	if (n < 0)
		n += BoardMax;

	if (sbuffer.A[num].z < sbuffer.B[n].z[0])
		return true;
	else
		return false;
}
void GameSystem::DeleteBoard()
{

}

bool  GameSystem::GetItem(int num)
{
	for (int i = 0; i < BoardMax; i++)
	{
		if (sbuffer.B[i].item == 1)
		{
			if (((sbuffer.B[i].x[0] + sbuffer.B[i].x[1]) / 2 - 5 <= sbuffer.A[num].x && (sbuffer.B[i].x[0] + sbuffer.B[i].x[1]) / 2 + 5 >= sbuffer.A[num].x)
				&& ((sbuffer.B[i].z[0] + sbuffer.B[i].z[1]) / 2 - 5 <= sbuffer.A[num].z && (sbuffer.B[i].z[0] + sbuffer.B[i].z[1]) / 2 + 5 >= sbuffer.A[num].z)
				&& (sbuffer.A[num].y >= 4 + sbuffer.B[i].y &&  sbuffer.A[num].y <= +sbuffer.B[i].y + 14))
			{
				return true;
			}
		}
		else if (sbuffer.B[i].item == 2)
		{
			if (((sbuffer.B[i].x[0] + sbuffer.B[i].x[1]) / 2 - 8.5 <= sbuffer.A[num].x && (sbuffer.B[i].x[0] + sbuffer.B[i].x[1]) / 2 + 8.5 >= sbuffer.A[num].x)
				&& ((sbuffer.B[i].z[0] + sbuffer.B[i].z[1]) / 2 - 8.5 <= sbuffer.A[num].z && (sbuffer.B[i].z[0] + sbuffer.B[i].z[1]) / 2 + 8.5 >= sbuffer.A[num].z)
				&& (sbuffer.A[num].y >= 4 + sbuffer.B[i].y &&  sbuffer.A[num].y <= +sbuffer.B[i].y + 21))
			{
				return true;
			}
		}
	}
	return false;
}

BOARD* GameSystem::GetBOARD(double x, double z)
{
	for (int i = 0; i < BoardMax; i++)
	{
		if ((sbuffer.B[i].x[0] <= x && sbuffer.B[i].x[1] >= x) && (sbuffer.B[i].z[0] <= z && sbuffer.B[i].z[1] >= z))
		{
			return &sbuffer.B[i];
		}
	}
	return NULL;
}
void  GameSystem::ItemEffect(int nItem, int num)
{
	if (nItem == 2)// 발판 잠시 정지
	{
		sbuffer.effect[STOP] = true;
	}
	else if (nItem == 3)//공 무적
	{
		sbuffer.effect[3 * num + 4] = true;
	}
	else if (nItem == 4)//공의 속도를 줄인다.
	{
		ballspeed[num] = 0.02f * 0.5f;
		sbuffer.effect[3 * num + 3] = true;
	}
	else if (nItem == 5)// 공 블라인드
	{
		sbuffer.effect[3 * num + 2] = true;
	}
	else if (nItem == 6)//안개 효과
	{
		sbuffer.effect[FOG] = true;
	}
}
void GameSystem::BuildBoard(double x1, double z1, double y, double x2, double z2, bool move, double range, int item, int i)
{
	BOARD A;

	A.x[0] = x1, A.x[1] = x2;
	A.y = y;
	A.z[0] = z1, A.z[1] = z2;
	A.clicked = false;
	A.move = move;
	A.m_range = 0;
	A.range = range;
	A.item = item;
	A.texture = rand() % 8;
	if (rand() % 2 == 0)
		A.mark = false;
	else
		A.mark = true;

	sbuffer.B[i] = A;
}