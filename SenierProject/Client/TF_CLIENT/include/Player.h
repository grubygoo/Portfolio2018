#pragma once

#include "MoveObj.h"

class CPlayer : public CMoveObject
{
	const float  MoveTime     = 0.5f;
	const float  AttackTime   = 0.5f;
	const BYTE   CharacterNum = 2; 

protected:
	//Model3D		 m_pModels[3];
	BYTE         PrevDirection;
	BYTE	     CurMeshNum;
	float        CheckTime;
	bool         ISMOVE;
	bool         ISIDLE;
	int          Speed;
	CTeleportEffect* m_pTeleportEffect;
	std::shared_mutex MeshNumLock;
	CChangeEffect* m_pChangeEffect;
	CFairyEffect* m_pFairyEffect;
	CWarriorSkillEffect* m_pWarriorSkillEffect;
	CFairySkillEffect* m_pFairtSkillEffect;
	CSanyangSkillEffect* m_pSanyangSkillEffect;

	std::shared_mutex TellLock;

public:
	bool IsSkill = false;
	bool IsTell = false;
	bool IsChange = false;
	float warriorskilltime;
	bool WarSkillOn = true;
	float fairyskilltime;
	bool fairySkillOn = true;
	float sanyangskilltime;
	bool sanyangSkillOn = true;
public:
	CPlayer(int nMeshes);
	virtual ~CPlayer();

public:
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void ProcessInput(float fDeltaTime);
	virtual void Update(float fDeltaTime, CCamera* pCamera);
	virtual void  Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);

public: 
	const BYTE GetCurMeshNum()       { std::shared_lock<shared_mutex> lock(MeshNumLock); return CurMeshNum; }
	void SetCurMeshNum(BYTE curMesh) { std::unique_lock<shared_mutex> lock(MeshNumLock); CurMeshNum = curMesh; }
	const bool GetIsTell() { std::shared_lock<shared_mutex> lock(TellLock); return IsTell; }
	void SetTel(bool tel) { std::unique_lock<shared_mutex> lock(TellLock); IsTell = tel; }
	
	void SetSpeed(int speed) { Speed = speed; }
	int  GetSpeed() { return Speed;  }

};

class pShadow : public CPlayer
{
public:
	pShadow(int nMeshes);
	virtual ~pShadow();

	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void Update(float fDeltaTime, XMFLOAT4X4 worldMt);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
};