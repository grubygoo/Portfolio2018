#pragma once
class GameSystem
{
private:
	float ballspeed[3];
	bool balltop[3];
	double fx[3];

	SERVER_STRUCT& sbuffer;
	CLIENT_STRUCT cbuffer[3];
	int height_buffer[3];
	float board_speed;
	int start;
public:

	GameSystem(SERVER_STRUCT& board);
	~GameSystem();
	void SetBalls(CLIENT_STRUCT& buf, int num);

	void MoveBoard();
	void MakeBoard();
	void BallUpdate(int num);
	int FallBall(int num);
	bool CheckPosition(int num);
	void DeleteBoard();
	bool GetItem(int num);
	void  ItemEffect(int nItem, int num);
	BOARD* GetBOARD(double x, double z);
	void BuildBoard(double x1, double z1, double y, double x2, double z2, bool move, double range, int item, int i);
};

