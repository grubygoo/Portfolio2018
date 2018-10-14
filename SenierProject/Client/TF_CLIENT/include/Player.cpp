#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "md5Loader.h"
#include "Input.h"
#include "../include/NetWork/NetWorkManager.h"


CPlayer::CPlayer(int nMeshes) : CMoveObject(nMeshes)
{
	ISMOVE = false;
	ISIDLE = true;
	CurMeshNum = 0;
	CheckTime = 0;
	PrevDirection = 127;
	m_hp = 100;
	Speed = 10;
	m_bActive = false;
	warriorskilltime = 0;
	fairyskilltime = 0;
	sanyangskilltime = 0;
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	warriorskilltime = 0;
	CMesh* tempMesh;
	CMesh* FairyMesh;
	CMesh* GoralMesh;
	m_pModels = new Model3D[3];
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	std::vector<ID3D12Resource*> shaderResourceViewArray2;
	std::vector<std::wstring> texFileNameArray2; //이거 왜있는겨 아무것도아닌거같은데? 
	std::vector<ID3D12Resource*> shaderResourceViewArray3;
	std::vector<std::wstring> texFileNameArray3; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, tempMesh, L"Assets/Model/Movable/Player/Warrior/WarriorMesh.md5mesh", m_pModels[0], shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorIdle.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorWalk.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAttack.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAni_damaged.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAni_dead.md5anim", m_pModels[0]);

	LoadMD5Model(pd3dDevice, pd3dCommandList, FairyMesh, L"Assets/Model/Movable/Player/Fairy/FairyMesh.md5mesh", m_pModels[1], shaderResourceViewArray2, texFileNameArray2, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyIdle.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyFly.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyAttack.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDamaged.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDead.md5anim", m_pModels[1]);

	LoadMD5Model(pd3dDevice, pd3dCommandList, GoralMesh, L"Assets/Model/Movable/Player/Goral/GoralMesh_0816.md5mesh", m_pModels[2], shaderResourceViewArray3, texFileNameArray3, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_idle.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_walk2.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_attack.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_damaged.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_dead.md5anim", m_pModels[2]);

	SetMesh(0, tempMesh);
	SetMesh(1, FairyMesh);
	SetMesh(2, GoralMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Warrior/warrior.dds"), 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Fairy/FairyMap.dds"), 1);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Goral/GoralMap.dds"), 2);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	SetStage(STAGE::STAGE1);

	m_pChangeEffect = new CChangeEffect();
	m_pChangeEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pChangeEffect->isOn = false;
	
	m_pFairyEffect = new CFairyEffect();
	m_pFairyEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pFairyEffect->isOn = false;
	
	m_pTeleportEffect = new CTeleportEffect();
	m_pTeleportEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTeleportEffect->isOn = false;
	
	m_pWarriorSkillEffect = new CWarriorSkillEffect();
	m_pWarriorSkillEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorSkillEffect->isOn = false;
	
	m_pFairtSkillEffect = new CFairySkillEffect();
	m_pFairtSkillEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pFairtSkillEffect->isOn = false;

	m_pSanyangSkillEffect = new CSanyangSkillEffect();
	m_pSanyangSkillEffect->Initialize(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pSanyangSkillEffect->isOn = false;

	return true;
}

void CPlayer::ProcessInput(float fDeltaTime)
{
	if (!m_bActive) return;
	if (m_AnimState != ATTACKSTATE)
	{
		if (KEYPRESS("MF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FORWARD);
		}
		if (KEYPRESS("MR")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_RIGHT);
		}
		if (KEYPRESS("MB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BACKWARD);
		}
		if (KEYPRESS("ML")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_LEFT);
		}
		if (KEYPRESS("MRF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FR);
		}
		if (KEYPRESS("MLF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FL);
		}
		if (KEYPRESS("MRB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BR);
		}
		if (KEYPRESS("MLB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BL);
		}

		if (KEYDOWN("TELEPORT")) {
			CNetWorkManager::GET_SINGLE()->SendTelePortPacket(*this);
			XMFLOAT3 pos = GetWPosition();
			pos.y = 3;
			m_pTeleportEffect->SetWPosition(pos);
			m_pTeleportEffect->isOn = true;
		}

		if (ISMOVE)
		{
			m_AnimState = MOVESTATE;
			ISMOVE = false;

			if (PrevDirection != m_Direction) {
				PrevDirection = GetDirection();
				CheckTime = 0.f;
				CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
			}

			if (CheckTime < MoveTime) CheckTime += fDeltaTime;
			else { CheckTime = 0.f; CNetWorkManager::GET_SINGLE()->SendMovePacket(*this); }
		}
		else if (!ISIDLE)
		{
			m_AnimState = IDLESTATE;
			ISIDLE = true;
			CheckTime = 0.f;
			PrevDirection = 127;
			CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
		}
	}

	if (KEYDOWN("F1")) {
		IsChange = true;
		SetCurMeshNum(0);
		CNetWorkManager::GET_SINGLE()->SendChangeCharacterPacket(*this);
		IsSkill = false;
		//m_pChangeEffect->SetWPosition(m_xmf4x4World._41, m_xmf4x4World._42 + 5, m_xmf4x4World._43);
		//m_pChangeEffect->isOn = true;
	}
	if (KEYDOWN("F2")) {
		IsChange = true;
		SetCurMeshNum(2);
		CNetWorkManager::GET_SINGLE()->SendChangeCharacterPacket(*this);
		IsSkill = false;
		//m_pChangeEffect->SetWPosition(m_xmf4x4World._41, m_xmf4x4World._42 + 5, m_xmf4x4World._43);
		//m_pChangeEffect->isOn = true;
	}

	if (KEYDOWN("F3")) {
		IsChange = true;
		SetCurMeshNum(1);
		CNetWorkManager::GET_SINGLE()->SendChangeCharacterPacket(*this);
		IsSkill = false;
	//	m_pChangeEffect->SetWPosition(m_xmf4x4World._41, m_xmf4x4World._42 + 5, m_xmf4x4World._43);
	//	m_pChangeEffect->isOn = true;
	}

	if (KEYDOWN("F4")) {
		if (GetSpeed() == 10) SetSpeed(50);
		else SetSpeed(10);
	}

	if (KEYDOWN("Fire")) {
		CNetWorkManager::GET_SINGLE()->SendAttackPacket(*this);
		m_AnimState = ATTACKSTATE;
		//if (CurMeshNum == 1)
		//{
		//	XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 10);
		//	pos.y = 5;
		//	m_pFairyEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
		//	m_pFairyEffect->isOn = true;
		//}
		//warriorskilltime = 5;
		WarSkillOn = false;
	}

	if (KEYDOWN("X") /*|| IsSkill == true*/)
	{
		if (CurMeshNum == 0)
		{
			if (WarSkillOn)
			{
				XMFLOAT3 pos = GetWPosition();
				pos.y = 3;
				m_pWarriorSkillEffect->SetWPosition(pos);
				m_pWarriorSkillEffect->isOn = true;
				warriorskilltime = 5;
				WarSkillOn = false;
			}
		}
		else if (CurMeshNum == 1)
		{
			if (fairySkillOn)
			{
				XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 7);
				pos.y = 5;
				m_pFairtSkillEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
				m_pFairtSkillEffect->isOn = true;
				fairyskilltime = 5;
				fairySkillOn = false;
			}

		}

		else if (CurMeshNum == 2)
		{
			if (sanyangSkillOn)
			{
				XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 7);
				pos.y = 5;
				m_pSanyangSkillEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
				m_pSanyangSkillEffect->isOn = true;
				sanyangskilltime = 5;
				sanyangSkillOn = false;
			}

		}

		CNetWorkManager::GET_SINGLE()->SendSkillPacket(*this);

	}
}


void CPlayer::Update(float fDeltaTime, CCamera* pCamera)
{
	if (!m_bActive) return;
	if (m_nMeshes > 0)  UpdateMD5Model(m_ppMeshes[CurMeshNum], m_pModels[CurMeshNum], fDeltaTime, m_AnimState);
	if (m_AnimState == MOVESTATE) {
		//	printf("X:%f, Y:%f, Z:%f\n", this->GetWPosition().x, this->GetWPosition().y, this->GetWPosition().z);
		Rotate(m_Direction);
		CMoveObject::Move(Vector3::Add(XMFLOAT3(0, 0, 0), GetLook(), fDeltaTime * Speed));
	}
	else if (m_AnimState == ATTACKSTATE) {
		if ((m_pModels[CurMeshNum].animations[ATTACKSTATE].currAnimTime == 0.f)) m_AnimState = IDLESTATE;

		if (CurMeshNum == 1)
		{
			XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 10);
			pos.y = 5;
			m_pFairyEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
			m_pFairyEffect->isOn = true;
		}
	}
	m_OOBB.Center = GetWPosition();

	if (IsChange)
	{
		IsSkill = false;
		//SetCurMeshNum(CurMeshNum);
		//CNetWorkManager::GET_SINGLE()->SendChangeCharacterPacket(*this);
		m_pChangeEffect->SetWPosition(m_xmf4x4World._41, m_xmf4x4World._42 + 5, m_xmf4x4World._43);
		m_pChangeEffect->isOn = true;
	}

	if (m_pChangeEffect->isOn == true)
	{
		m_pChangeEffect->Animate(fDeltaTime, pCamera);
		IsChange = false;
	}
	//else
	//	IsChange = false;
	
	if (m_pFairyEffect->isOn == true)
	{
		m_pFairyEffect->Animate(fDeltaTime, pCamera);
	}
	
	if (WarSkillOn == false)
	{
		warriorskilltime -= fDeltaTime;
		if (warriorskilltime < 0)
		{
			warriorskilltime = 0;
			IsSkill = false;
			WarSkillOn = true;
		}
	}
	
	if (m_pTeleportEffect->isOn == true)
	{
		m_pTeleportEffect->Animate(fDeltaTime, pCamera);
		IsTell = false;
	}
	//else
	//{
	//	IsTell = false;
	//}
	if (m_pWarriorSkillEffect->isOn == true)
	{
		XMFLOAT3 pos = GetWPosition();
		pos.y = 3;
		m_pWarriorSkillEffect->SetWPosition(pos);
		m_pWarriorSkillEffect->Animate(fDeltaTime, pCamera);
	}
	
	if (m_pFairtSkillEffect->isOn == true)
	{
		m_pFairtSkillEffect->Animate(fDeltaTime, pCamera);
	}
	
	
	
	if (fairySkillOn == false)
	{
		fairyskilltime -= fDeltaTime;
		if (fairyskilltime < 0)
		{
			fairyskilltime = 0;
			IsSkill = false;
			fairySkillOn = true;
		}
	}

	if (m_pSanyangSkillEffect->isOn == true)
	{
		m_pSanyangSkillEffect->Animate(fDeltaTime, pCamera);
	}



	if (sanyangSkillOn == false)
	{
		sanyangskilltime -= fDeltaTime;
		if (sanyangskilltime < 0)
		{
			sanyangskilltime = 0;
			IsSkill = false;
			sanyangSkillOn = true;
		}
	}
	
	if (IsSkill == true)
	{
		if (CurMeshNum == 0)
		{
			if (WarSkillOn)
			{
				XMFLOAT3 pos = GetWPosition();
				pos.y = 3;
				m_pWarriorSkillEffect->SetWPosition(pos);
				m_pWarriorSkillEffect->isOn = true;
				warriorskilltime = 5;
				WarSkillOn = false;
			}
		}
		else if (CurMeshNum == 1)
		{
			if (fairySkillOn)
			{
				XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 7);
				pos.y = 5;
				m_pFairtSkillEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
				m_pFairtSkillEffect->isOn = true;
				fairyskilltime = 5;
				fairySkillOn = false;
			}
	
		}

		else if (CurMeshNum == 2)
		{
			if (sanyangSkillOn)
			{
				XMFLOAT3 pos = Vector3::ScalarProduct(GetLook(), 7);
				pos.y = 5;
				m_pSanyangSkillEffect->SetWPosition(Vector3::Add(pos, GetWPosition()));
				m_pSanyangSkillEffect->isOn = true;
				sanyangskilltime = 5;
				sanyangSkillOn = false;
			}

		}
	}
	
	if (GetIsTell() == true)
	{
		XMFLOAT3 pos = GetWPosition();
		pos.y = 3;
		m_pTeleportEffect->SetWPosition(pos);
		m_pTeleportEffect->isOn = true;
	}
	
	//if (IsChange)
	//{
	//	m_pChangeEffect->SetWPosition(m_xmf4x4World._41, m_xmf4x4World._42 + 5, m_xmf4x4World._43);
	//	m_pChangeEffect->isOn = true;
	//}

}

void CPlayer::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, CurMeshNum);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
	if (m_nMeshes > 0) m_ppMeshes[CurMeshNum]->Render(pd3dCommandList, nInstances);

	if (m_pChangeEffect->isOn == true)
		m_pChangeEffect->Render(pd3dCommandList, pCamera);
	
	if (m_pFairyEffect->isOn == true)
		m_pFairyEffect->Render(pd3dCommandList, pCamera);
	
	if (m_pTeleportEffect->isOn == true)
		m_pTeleportEffect->Render(pd3dCommandList, pCamera);
	
	if (m_pWarriorSkillEffect->isOn == true)
		m_pWarriorSkillEffect->Render(pd3dCommandList, pCamera);
	
	if (m_pFairtSkillEffect->isOn == true)
		m_pFairtSkillEffect->Render(pd3dCommandList, pCamera);

	if (m_pSanyangSkillEffect->isOn == true)
		m_pSanyangSkillEffect->Render(pd3dCommandList, pCamera);
}

pShadow::pShadow(int nMeshes) : CPlayer(nMeshes)
{
}

pShadow::~pShadow()
{
}

bool pShadow::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CMesh* tempMesh;
	CMesh* FairyMesh;
	CMesh* GoralMesh;
	m_pModels = new Model3D[3];
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	std::vector<ID3D12Resource*> shaderResourceViewArray2;
	std::vector<std::wstring> texFileNameArray2; //이거 왜있는겨 아무것도아닌거같은데? 
	std::vector<ID3D12Resource*> shaderResourceViewArray3;
	std::vector<std::wstring> texFileNameArray3; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, tempMesh, L"Assets/Model/Movable/Player/Warrior/WarriorMesh.md5mesh", m_pModels[0], shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorIdle.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorWalk.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAttack.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAni_damaged.md5anim", m_pModels[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAni_dead.md5anim", m_pModels[0]);

	LoadMD5Model(pd3dDevice, pd3dCommandList, FairyMesh, L"Assets/Model/Movable/Player/Fairy/FairyMesh.md5mesh", m_pModels[1], shaderResourceViewArray2, texFileNameArray2, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyIdle.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyFly.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyAttack.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDamaged.md5anim", m_pModels[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDead.md5anim", m_pModels[1]);

	LoadMD5Model(pd3dDevice, pd3dCommandList, GoralMesh, L"Assets/Model/Movable/Player/Goral/GoralMesh_0816.md5mesh", m_pModels[2], shaderResourceViewArray3, texFileNameArray3, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_idle.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_walk2.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_attack.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_damaged.md5anim", m_pModels[2]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Goral/GoralAni_dead.md5anim", m_pModels[2]);

	SetMesh(0, tempMesh);
	SetMesh(1, FairyMesh);
	SetMesh(2, GoralMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	pShader->isShadow = true;
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Warrior/warrior.dds"), 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Fairy/FairyMap.dds"), 1);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Goral/GoralMap.dds"), 2);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	SetStage(STAGE::STAGE1);

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.3f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.003f, 0.0f);

	MTShadow = S * shadowOffSetY;

	return true;
}

void pShadow::Update(float fDeltaTime, XMFLOAT4X4 worldMt)
{
	if (!m_bActive) return;
	SetWMatrix(Matrix4x4::Multiply(worldMt, MTShadow));

	if (m_nMeshes > 0)  UpdateMD5Model(m_ppMeshes[CurMeshNum], m_pModels[CurMeshNum], fDeltaTime, m_AnimState);

	else if (m_AnimState == ATTACKSTATE) {
		if ((m_pModels[CurMeshNum].animations[ATTACKSTATE].currAnimTime == 0.f)) m_AnimState = IDLESTATE;
	}
}

void pShadow::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, CurMeshNum);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
	if (m_nMeshes > 0) m_ppMeshes[CurMeshNum]->Render(pd3dCommandList, nInstances);

}
