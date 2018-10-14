#pragma once
#include "../Include.h"
#include "SingleTone.h"
#include <fstream>

struct MonsterData
{
	BYTE  Type;
	float x = 0;
	float z = 0;
	float Speed = 0.f;
	float Atk = 0.f;
	float Hp = 0.f;

	MonsterData(BYTE t, float _x, float _z, float s, float a, float h)
	{
		Type = t;
		x = _x;
		z = _z;
		Speed = s;
		Atk = a;
		Hp = h;
	}
};

class CMapData :public CSingleTone<CMapData>
{
public:
	std::vector<MonsterData> Stage1Data;
	std::unordered_map<BYTE, std::vector<BoundingOrientedBox>> MapBoundingBoxes;
	int Width;
	int Height;
public:
	bool  Initialize()
	{
		ifstream  ReadStage1("include/MapData/0816.txt");
		if (ReadStage1.is_open())
		{
			ReadStage1 >> Width;
			ReadStage1 >> Height;
			int tempType = 0;
			for (int z = 0; z < 20; ++z)
			{
				for (int x = 0; x < 20; ++x)
				{
					ReadStage1 >> tempType;

					if (tempType <= 1) continue;
					
					float Speed = 0.f;
					float Atk = 0.f;
					float Hp = 0.f;
					//printf("TempType: %d\n", tempType);
					
					switch (tempType)
					{
					case READ_DATA::SMON: {Speed = 5.f; Atk = 5.f; Hp = 100.f; break; }
					case READ_DATA::BMON: {Speed = 6.f; Atk = 10.f; Hp = 150.f; break; }
					case READ_DATA::SMONBOSS: {Speed = 7.f; Atk = 15.f; Hp = 180.f; break; }
					case READ_DATA::BMONBOSS: {Speed = 8.f; Atk = 20.f; Hp = 220.f; break; }
					case READ_DATA::FINALBOSS: {
						printf("생성이되냐\n");
						Speed = 9.f; Atk = 50.f; Hp = 300.f; break; 
					}
					default:break;
					}
					Stage1Data.emplace_back(tempType, x*30.f, -30 * z, Speed, Atk, Hp);
				}
			}
			ReadStage1.close();
			printf("Stage1 MapRead Success!\n");
		}
		XMFLOAT3 Points[16][8] = {

		{ XMFLOAT3(0, -20, 0), XMFLOAT3(570, -20, 0), XMFLOAT3(570, -20, -45), XMFLOAT3(0, -20, -45),
		XMFLOAT3(0, 100, 0), XMFLOAT3(570, 100, 0), XMFLOAT3(570, 100, -45), XMFLOAT3(0, 100, -45) },

		{ XMFLOAT3(0, -20, -525), XMFLOAT3(570, -20, -525), XMFLOAT3(570, -20, -570), XMFLOAT3(0, -20, -570),
		XMFLOAT3(0, 100, -525), XMFLOAT3(570, 100, -525), XMFLOAT3(570, 100, -570), XMFLOAT3(0, 100, -570) },

		{ XMFLOAT3(0, -20, -45), XMFLOAT3(45, -20, -45), XMFLOAT3(45, -20, -525), XMFLOAT3(0, -20, -525),
		XMFLOAT3(0, 100, -45), XMFLOAT3(45, 100, -45), XMFLOAT3(45, 100, -525), XMFLOAT3(0, 100, -525) },

		{ XMFLOAT3(525, -20, -45), XMFLOAT3(570, -20, -45), XMFLOAT3(570, -20, -525), XMFLOAT3(525, -20, -525),
		XMFLOAT3(525, 100, -45), XMFLOAT3(570, 100, -45), XMFLOAT3(570, 100, -525), XMFLOAT3(525, 100, -525) },

		{ XMFLOAT3(75, -20, -45), XMFLOAT3(105, -20, -45), XMFLOAT3(105, -20, -495), XMFLOAT3(75, -20, -495),
		XMFLOAT3(75,100, -45), XMFLOAT3(105,100, -45), XMFLOAT3(105,100, -495), XMFLOAT3(75,100, -495) },

		{ XMFLOAT3(105, -20, -465), XMFLOAT3(435, -20, -465), XMFLOAT3(435, -20, -495), XMFLOAT3(105, -20, -495),
		XMFLOAT3(105,100, -465), XMFLOAT3(435,100, -465), XMFLOAT3(435,100, -495), XMFLOAT3(105,100, -495) },

		{ XMFLOAT3(405, -20, -465), XMFLOAT3(435, -20, -465), XMFLOAT3(435, -20, -135), XMFLOAT3(405, -20, -135),
		XMFLOAT3(405,100, -465), XMFLOAT3(435,100, -465), XMFLOAT3(435,100, -135), XMFLOAT3(405,100, -135) },

		{ XMFLOAT3(405, -20, -135), XMFLOAT3(405, -20, -165), XMFLOAT3(195, -20, -165), XMFLOAT3(195, -20, -135),
		XMFLOAT3(405,100, -135), XMFLOAT3(405,100, -165), XMFLOAT3(195,100, -165), XMFLOAT3(195,100, -135) },

		{ XMFLOAT3(195, -20, -165), XMFLOAT3(225, -20, -165), XMFLOAT3(225, -20, -375), XMFLOAT3(195, -20, -375),
		XMFLOAT3(195,100, -165), XMFLOAT3(225,100, -165), XMFLOAT3(225,100, -375), XMFLOAT3(195,100, -375) },

		{ XMFLOAT3(225, -20, -375), XMFLOAT3(225, -20, -315), XMFLOAT3(315, -20, -315), XMFLOAT3(315, -20, -375),
		XMFLOAT3(225,100, -375), XMFLOAT3(225,100, -315), XMFLOAT3(315,100, -315), XMFLOAT3(315,100, -375) },

		{ XMFLOAT3(495, -20, -525), XMFLOAT3(495, -20, -75), XMFLOAT3(465, -20, -75), XMFLOAT3(465, -20, -525),
		XMFLOAT3(495,100, -525), XMFLOAT3(495,100, -75), XMFLOAT3(465,100, -75), XMFLOAT3(465,100, -525) },

		{ XMFLOAT3(465, -20, -75), XMFLOAT3(465, -20, -105), XMFLOAT3(135, -20, -105), XMFLOAT3(135, -20, -75),
		XMFLOAT3(465,100, -75), XMFLOAT3(465,100, -105), XMFLOAT3(135,100, -105), XMFLOAT3(135,100, -75) },

		{ XMFLOAT3(135, -20, -105), XMFLOAT3(135, -20, -375), XMFLOAT3(165, -20, -375), XMFLOAT3(165, -20, -105),
		XMFLOAT3(135,100, -105), XMFLOAT3(135,100, -375), XMFLOAT3(165,100, -375), XMFLOAT3(165,100, -105) },

		{ XMFLOAT3(165, -20, -435), XMFLOAT3(165, -20, -405), XMFLOAT3(375, -20, -405), XMFLOAT3(375, -20, -435),
		XMFLOAT3(165,100, -435), XMFLOAT3(165,100, -405), XMFLOAT3(375,100, -405), XMFLOAT3(375,100, -435) },

		{ XMFLOAT3(345, -20, -405), XMFLOAT3(345, -20, -195), XMFLOAT3(375, -20, -195), XMFLOAT3(375, -20, -405),
		XMFLOAT3(345,100, -405), XMFLOAT3(345,100, -195), XMFLOAT3(375,100, -195), XMFLOAT3(375,100, -405) },

		{ XMFLOAT3(345, -20, -255), XMFLOAT3(345, -20, -195), XMFLOAT3(255, -20, -195), XMFLOAT3(255, -20, -255),
		XMFLOAT3(345,100, -255), XMFLOAT3(345,100, -195), XMFLOAT3(255,100, -195), XMFLOAT3(255,100, -255) }
		};
		std::vector<BoundingOrientedBox> Stage1BoundingBox;
		for (int i = 0; i < 16; ++i)
		{
			BoundingOrientedBox *tempCollision = new BoundingOrientedBox();
			BoundingOrientedBox::CreateFromPoints(*tempCollision, 8, Points[i], sizeof(float) * 3);
			Stage1BoundingBox.push_back(*tempCollision);
		}
		MapBoundingBoxes[STAGE::STAGE1] = Stage1BoundingBox;
		return true;
	}
	void Close()
	{
		Stage1Data.clear();
		for (auto& M : MapBoundingBoxes) {
			Safe_Delete_VecList(M.second);
		}

	}
};

