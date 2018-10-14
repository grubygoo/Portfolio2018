#include "stdafx.h"
#include "DayForestScene.h"
#include "Player.h"
#include "CMonster.h"
#include "Input.h"
#include "../include/NetWork/NetWorkManager.h"
#include "../include/CMapData.h"

DayForestScene::DayForestScene()
{   
	Processor[SC_LOG_SUCC]          = &CScene::LoginSuccessPlayer;
	Processor[SC_PUT_PLAYER]        = &CScene::PutPlayer;
	Processor[SC_REMOVE_PLAYER]     = &CScene::RemovePlayer;
	Processor[SC_MOVE_PLAYER]       = &CScene::PlayerMove;
	Processor[SC_ATTACK_PLAYER]     = &CScene::PlayerAttack;
	Processor[SC_COLLISION_PLAYER]  = &CScene::PlayerCollision;
	Processor[SC_CHAGNE_CHARACTER]  = &CScene::PlayerChangeCharacter;
	Processor[SC_DAMAGESTEP_PLAYER] = &CScene::PlayerHP;
	Processor[SC_TELEPORT_PLAYER]   = &CScene::PlayerTeleport;
	Processor[SC_SKILL_PLAYER]      = &CScene::PlayerSkill;
	Processor[SC_RESPAWN_PLAYER]    = &CScene::RespawnPlayer;
	Processor[SC_PUT_NPC]           = &DayForestScene::PutNPC;
	Processor[SC_REMOVE_NPC]        = &DayForestScene::RemoveNPC;
	Processor[SC_STATE_NPC] = &DayForestScene::StateNPC;
	for(int i=0;i<10;++i) m_pEffect[i] = NULL;
}

DayForestScene::~DayForestScene()
{
	if (m_UiShader) m_UiShader->ReleaseObjects();
	SAFE_DELETE(m_UiShader);
	SAFE_DELETE(m_pMap);
	Safe_Delete_VecList(m_vMonsters);
	Safe_Delete_VecList(m_StaticObjects);
	delete[] m_pEffect;
	
}

bool DayForestScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	CScene::Initialize(pd3dDevice, pd3dCommandList);
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Tree/Trees3x3.MD5MESH", "Tree", XMFLOAT3(0.3, 0.3, 0.3));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Stone/Stone.MD5MESH", "Stone1", XMFLOAT3(0.2, 0.2, 0.2));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Stone/Stone2.MD5MESH", "Stone2", XMFLOAT3(0.2, 0.2, 0.2));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Stone/Stone3.MD5MESH", "Stone3", XMFLOAT3(0.2, 0.2, 0.2));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Mushroom/MushroomMd5.MD5MESH", "Mushroom", XMFLOAT3(0.2, 0.2, 0.2));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Mushroom/Mushroom2.MD5MESH", "Mushroom2", XMFLOAT3(0.3, 0.3, 0.3));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Mushroom/Mushroom3.MD5MESH", "Mushroom3", XMFLOAT3(0.3, 0.3, 0.3));
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Static/Mushroom/Mushroom4.MD5MESH", "Mushroom4", XMFLOAT3(0.3, 0.3, 0.3));


	if (m_pPlayerVector.size() == 0) {
		for (int i = 0; i < MAX_USER; ++i) 
		{
			CPlayer* Player = new CPlayer(3);
			Player->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
			m_pPlayerVector.push_back(Player);

			pShadow* PlayerShadow = new pShadow(3);
			PlayerShadow->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
			m_vPlayerShadow.push_back(PlayerShadow);
		}
	}
	CreateStaticObjectFromMapFile(pd3dDevice, pd3dCommandList);

	


	m_pMap = new CPlaneMap(MAPSIZE, MAPSIZE);
	m_pMap->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildLightsAndMaterials();
	m_pCamera = GenerateCamera(THIRD_PERSON_CAMERA, m_pPlayer);
	m_pCamera->SetTimeLag(0.f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 30.0f, -30.0f));
	m_pCamera->GenerateProjectionMatrix(1.0f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < 10; ++i)
	{
		m_pEffect[i] = new CEffect();
		m_pEffect[i]->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayerVector[0]);
		m_pEffect[i]->SetWPosition(60, 5, -60);
		m_pEffect[i]->isOn = false;
	}
	

	m_UiShader = new UIShader();
	m_UiShader->CreateGraphicsRootSignature(pd3dDevice);
	m_UiShader->CreateShader(pd3dDevice, 1);
	m_UiShader->BuildObjects(pd3dDevice, pd3dCommandList);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	return true;
}

void DayForestScene::ProcessInput(float fDeltaTime)
{
	if (m_pPlayer) {
		if(m_pPlayer->GetActive())
		m_pPlayer->ProcessInput(fDeltaTime);
	}
}

void DayForestScene::Update(float fDeltaTime, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	
	CollisionCheck(fDeltaTime);

	for (auto& i : m_pPlayerVector) i->Update(fDeltaTime, m_pCamera);
	for (int i = 0; i < m_vPlayerShadow.size(); ++i) {
		m_vPlayerShadow[i]->SetActive(m_pPlayerVector[i]->GetActive());
		m_vPlayerShadow[i]->SetCurMeshNum(m_pPlayerVector[i]->GetCurMeshNum());
		m_vPlayerShadow[i]->SetAnimState(m_pPlayerVector[i]->GetAnimState());
		m_vPlayerShadow[i]->Update(fDeltaTime, m_pPlayerVector[i]->GetWMatrix());
	}

	for (auto& i : m_vMonsters)
	{
		if (i->GetAnimState() == DEADSTATE && i->m_dead == false)
		{
			//cout << "사망" << endl;
			for (int j = 0; j < 10; ++j)
			{
				if (m_pEffect[j]->isOn == false)
				{
					XMFLOAT3 pos = i->GetWPosition();
					pos.y = 5;
					m_pEffect[j]->SetWPosition(pos);
					m_pEffect[j]->isOn = true;
					
					break;
				}
			}
			i->m_dead = true;
		}
		i->Update(fDeltaTime);
	}
	for (auto i = 0; i < m_vMShadows.size(); ++i) {
		m_vMShadows[i]->SetActive(m_vMonsters[i]->GetActive());
		m_vMShadows[i]->SetAnimState(m_vMonsters[i]->GetAnimState());
		m_vMShadows[i]->Update(fDeltaTime, m_vMonsters[i]->GetWMatrix());
	}

	for (int i = 0; i < 10; ++i)
	{
		if (m_pEffect[i]->isOn == true)
		{
			m_pEffect[i]->Animate(fDeltaTime, m_pCamera);
		}
	}
	
	m_pCamera->Update(fDeltaTime);
	if (heat) {
		m_pCamera->Move(XMFLOAT3(2.0, 0.0, 0.0));
		heat = false;
	}
	m_pCamera->RegenerateViewMatrix();
}

void DayForestScene::EffectUpdate(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}


void DayForestScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials

	for (auto& i : m_pPlayerVector) i->Render(pd3dCommandList, m_pCamera);
	for (auto& i : m_vPlayerShadow) i->Render(pd3dCommandList, m_pCamera);

	for (auto& i : m_vMonsters)     i->Render(pd3dCommandList, m_pCamera);
	for (auto& i : m_vMShadows)		i->Render(pd3dCommandList, m_pCamera);

	for (auto& i : m_StaticShader)	i->Render(pd3dCommandList, m_pCamera);
	
	if (m_pMap) m_pMap->Render(pd3dCommandList, m_pCamera);

	for (int i = 0; i < 10; ++i)
	{
		if (m_pEffect[i]->isOn == true) m_pEffect[i]->Render(pd3dCommandList, m_pCamera);
	}
	m_UiShader->Render(pd3dCommandList, m_pCamera);
}

bool DayForestScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool DayForestScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void DayForestScene::ReleaseUploadBuffers()
{
	if (m_pPlayerVector.size() > 0)
		for (auto& i : m_pPlayerVector)i->ReleaseUploadBuffers();
	for (auto& i : m_StaticObjects) i->ReleaseUploadBuffers();
	for (auto& i : m_vMonsters) i->ReleaseUploadBuffers();
	if (m_pMap)	m_pMap->ReleaseUploadBuffers();
}

void DayForestScene::BuildLightsAndMaterials()
{

	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);


	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));


	/*조명 머테리얼*/
	m_pMaterials->m_pReflections[0] = { XMFLOAT4(0.3f, 0.3f, 0.05f, 1.0f), XMFLOAT4(0.3f, 0.3f, 0.05f, 1.0f), XMFLOAT4(0.3f, 0.3f, 0.05f, 1.0f), XMFLOAT4(0.3f, 0.3f, 0.05f, 1.0f) };

	/*그림자 머테리얼*/
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

	m_nMaterials = 2;
}

void DayForestScene::CreateStaticObjectFromMapFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	int index = 0;

	// Tree
	INSTANCEOB tempOB = FindStaticObProtoType("Tree");
	CInstancingShader* tempShader = new CInstancingShader();
	tempShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempShader->Initialize(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempShader);

	// Tree Shadow
	CInstancingShader* tempShadow = new CInstancingShader();
	tempShadow->CreateShadowShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempShadow->InitializeShadow(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempShadow);

	// Stone
	tempOB = FindStaticObProtoType("Stone1");
	CInstancingShader* tempStone = new CInstancingShader();
	tempStone->CreateTexturedShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempStone->InitializeStone(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempStone);

	// Stone2
	tempOB = FindStaticObProtoType("Stone2");
	CInstancingShader* tempStone1 = new CInstancingShader();
	tempStone1->CreateTexturedShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempStone1->InitializeStone2(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempStone1);

	// Stone3
	tempOB = FindStaticObProtoType("Stone3");
	CInstancingShader* tempStone2 = new CInstancingShader();
	tempStone2->CreateTexturedShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempStone2->InitializeStone3(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempStone2);

	// Mushrooms
	tempOB = FindStaticObProtoType("Mushroom3");
	CInstancingShader* tempMushroom = new CInstancingShader();
	tempMushroom->CreateTexturedShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	tempMushroom->InitializeMush(pd3dDevice, pd3dCommandList, tempOB.Mesh);
	m_StaticShader.push_back(tempMushroom);

	for (int y = 0; y < MAPSIZE; ++y) {
		for (int x = 0; x < MAPSIZE; ++x) {
			switch (CMapData::GET_SINGLE()->Stage1[y][x])
			{
			case READ_DATA::SMON: {
				for (int i = 0; i < 3; ++i) {
					CStoneMon* i_Monster = new CStoneMon();
					i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
					i_Monster->SetWPosition(x * 30.0f, 0.0f, y* -30.0f);
					i_Monster->SetActive(false);
					m_vMonsters.push_back(i_Monster);
					
					CStoneMon* s_Monster = new CStoneMon();
					s_Monster->InitializeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
					s_Monster->SetActive(false);
					m_vMShadows.push_back(s_Monster);
					
				}
				break;
			}
	
			case READ_DATA::BMON: {
				for (int i = 0; i < 3; ++i) {
					CBeatleMon* i_Monster = new CBeatleMon();
					i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
					i_Monster->SetWPosition(x * 30.0f, 0.0f, y* -30.0f);
					i_Monster->SetActive(false);
					m_vMonsters.push_back(i_Monster);
					
					
					CBeatleMon* s_Monster = new CBeatleMon();
					s_Monster->InitializeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
					s_Monster->SetActive(false);
					m_vMShadows.push_back(s_Monster);
				}
				break;
			}
	
			case READ_DATA::SMONBOSS: {
				for (int i = 0; i < 3; ++i)
				{

				CStoneBOSS* i_Monster = new CStoneBOSS();
				i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				i_Monster->SetWPosition(x * 30.0f, 0.0f, y* -30.0f);
				i_Monster->SetActive(false);
				m_vMonsters.push_back(i_Monster);
				
				CStoneMon* s_Monster = new CStoneMon();
				s_Monster->InitializeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				s_Monster->SetActive(false);
				m_vMShadows.push_back(s_Monster);
				}
				break;
			}
	
			case READ_DATA::BMONBOSS: {
				for (int i = 0; i < 3; ++i)
				{

				CBeatleBOSS* i_Monster = new CBeatleBOSS();
				i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				i_Monster->SetWPosition(x * 30.0f, 0.0f, y* -30.0f);
				i_Monster->SetActive(false);
				m_vMonsters.push_back(i_Monster);
				
				CBeatleMon* s_Monster = new CBeatleMon();
				s_Monster->InitializeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				s_Monster->SetActive(false);
				m_vMShadows.push_back(s_Monster);
				}
				break;
			}
			case READ_DATA::GROOT: {
				for (int i = 0; i < 3; ++i)
				{
				Groot* groot = new Groot();
				groot->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				//groot->SetWPosition(60.0f, 0.0f, -60.0f);
				//groot->SetActive(true);
				groot->SetActive(false);
				m_vMonsters.push_back(groot);

				Groot* s_groot = new Groot();
				s_groot->InitializeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				s_groot->SetActive(false);
				m_vMShadows.push_back(s_groot);
				}
				break;
			}
			default:
				break;
			}
		}
	}

	printf("VectorSize::%d\n", m_vMonsters.size());
}

void DayForestScene::CreateStaticObProtoType(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const std::wstring filePath, const std::string strTag, XMFLOAT3 scale)
{
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1;
	INSTANCEOB  TempOBJ;
	//LoadIlluminatedMD5Model(pd3dDevice, pd3dCommandList, TempOBJ.Mesh, filePath, TempOBJ.Model, shaderResourceViewArray1, texFileNameArray1, 0.3, 0.3, 0.3);
	LoadIlluminatedMD5Model(pd3dDevice, pd3dCommandList, TempOBJ.Mesh, filePath, TempOBJ.Model, shaderResourceViewArray1, texFileNameArray1, scale.x, scale.y, scale.z);
	m_mProtoType.insert(std::make_pair(strTag, TempOBJ));
}

void DayForestScene::CreateMovableObProtoType(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const std::wstring filePath, const std::string strTag, READ_DATA type)
{
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1;
	INSTANCEOB  TempOBJ;
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempOBJ.Mesh, filePath, TempOBJ.Model, shaderResourceViewArray1, texFileNameArray1, 0.3, 0.3, 0.3);
	switch (type) {
	case READ_DATA::SMON :
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonIdle.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonWalk.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonAttack.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDamaged.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDead.md5anim", TempOBJ.Model);
		break;
	case READ_DATA::BMON :
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonIdle.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonWalk.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonAttack.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDamaged.md5anim", TempOBJ.Model);
		LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDead.md5anim", TempOBJ.Model);
		break;
	}
	m_mProtoType.insert(std::make_pair(strTag, TempOBJ));
}

INSTANCEOB DayForestScene::FindStaticObProtoType(const std::string & strKey)
{
	auto Iter = m_mProtoType.find(strKey);
	/*if (Iter == m_mProtoType.end()) return NULL;*/
	return Iter->second;
}

void DayForestScene::CollisionCheck(float fDeltaTime)
{
	CMapData* ptr = CMapData::GET_SINGLE();
	XMFLOAT3 xmfShift;
	BYTE DIR;
	/*Player <-> Monster*/
	if (!m_pPlayer) return;
	for (auto& M : m_vMonsters)
	{
		if (m_pPlayer->GetAnimState() != MOVESTATE) continue;
		if (!M->GetActive()) continue;
		if (M->CheckCollision(m_pPlayer->GetOOBB()))
		{
			xmfShift = Vector3::ScalarProduct(m_pPlayer->GetRight(), 3.f, false);
			if (Vector3::CrossProduct(Vector3::Normalize(Vector3::Subtract(M->GetWPosition(), m_pPlayer->GetWPosition())), m_pPlayer->GetLook()).y > 0)
				xmfShift = Vector3::Add(m_pPlayer->GetLook(), xmfShift);
			else xmfShift = Vector3::Subtract(m_pPlayer->GetLook(), xmfShift);
			xmfShift = Vector3::Normalize(xmfShift);
			xmfShift = Vector3::Add(m_pPlayer->GetWPosition(), xmfShift, fDeltaTime * 15);
			m_pPlayer->SetWPosition(xmfShift);
			m_pPlayer->SetOOBB(xmfShift);
			CNetWorkManager::GET_SINGLE()->SendAccurCollisionPacket(*m_pPlayer);
			break;
		}
	}
	/*Player <-> Forest*/
	for (int i = 0; i < ptr->MapBoundingBoxes[m_pPlayer->GetStage()].size(); ++i)
	{
		auto& Collider = ptr->MapBoundingBoxes[m_pPlayer->GetStage()][i];
		if (m_pPlayer->CheckCollision(*Collider))
		{
			xmfShift = m_pPlayer->GetWPosition();
			if (i == 0)      DIR = DIR_BACKWARD;
			else if (i == 1) DIR = DIR_FORWARD;
			else if (i == 2) DIR = DIR_RIGHT;
			else if (i == 3) DIR = DIR_LEFT;
			else if (i == 4)
			{
				if (xmfShift.z > -490)
				{
					if (xmfShift.x < Collider->Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 5)
			{
				if (xmfShift.x < 435)
				{
					if (xmfShift.z < Collider->Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 6)
			{
				if (xmfShift.z < -135)
				{
					if (xmfShift.x < Collider->Center.x)DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else  DIR = DIR_FORWARD;
			}
			else if (i == 7)
			{
				if (xmfShift.x > 195)
				{
					if (xmfShift.z < Collider->Center.z)DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}
			else if (i == 8)
			{
				if (xmfShift.z > -375)
				{
					if (xmfShift.x < Collider->Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 9)
			{
				if (xmfShift.x < 315)
				{
					if (xmfShift.z < Collider->Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 10)
			{
				if (xmfShift.z < -75)
				{
					if (xmfShift.x < Collider->Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_FORWARD;
			}
			else if (i == 11)
			{
				if (xmfShift.x > 135)
				{
					if (xmfShift.z < Collider->Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}
			else if (i == 12)
			{
				if (xmfShift.z > -375)
				{
					if (xmfShift.x < Collider->Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 13)
			{
				if (xmfShift.x < 375)
				{
					if (xmfShift.z < Collider->Center.z) DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 14)
			{
				if (xmfShift.z < -195)
				{
					if (xmfShift.x < Collider->Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_FORWARD;
			}
			else if (i == 15)
			{
				if (xmfShift.x > 255)
				{
					if (xmfShift.z < Collider->Center.z)DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}
			CollisionMove(DIR, fDeltaTime);
		}
	}
	
}

void DayForestScene::CollisionMove(BYTE dir, float fDeltaTime)
{
	XMFLOAT3 S;
	S = Vector3::Subtract(m_pPlayer->GetLook(), Vector3::ScalarProduct(DIRMAP[dir], Vector3::DotProduct(m_pPlayer->GetLook(), DIRMAP[dir])));
	S = Vector3::Add(m_pPlayer->GetWPosition(), Vector3::Normalize(S), fDeltaTime * 15);
	S = Vector3::Add(m_pPlayer->GetWPosition(), DIRMAP[dir], fDeltaTime * 10);
	m_pPlayer->SetWPosition(S);
	m_pPlayer->SetOOBB(S);
	CNetWorkManager::GET_SINGLE()->SendAccurCollisionPacket(*m_pPlayer);
}


void DayForestScene::PutNPC(char * packet)
{
	sc_packet_put_npc*  Packet = (sc_packet_put_npc*)packet;
	WORD ID = Packet->id;
	m_vMonsters[ID]->Rotate(Packet->dir);
	m_vMonsters[ID]->SetDirection(Packet->dir);
	m_vMonsters[ID]->SetWPosition(Packet->pos);
	m_vMonsters[ID]->SetAnimState(Packet->state);
	m_vMonsters[ID]->SetActive(true);
}

void DayForestScene::StateNPC(char * packet)
{
	sc_packet_state_npc* Packet = (sc_packet_state_npc*)packet;
	WORD ID = Packet->id;
	
	if (m_vMonsters[ID]->GetAnimState() != HEATSTATE)
	{
		if (m_vMonsters[ID]->GetAnimState() != Packet->state) {
			m_vMonsters[ID]->GetCurModel()->animations[m_vMonsters[ID]->GetAnimState()].currAnimTime = 0.f;
		}
	   m_vMonsters[ID]->SetAnimState(Packet->state); 

	}

	m_vMonsters[ID]->Rotate(Packet->dir); 
	m_vMonsters[ID]->SetWPosition(Packet->pos);
	m_vMonsters[ID]->SetOOBB(Packet->pos);
	
}

void DayForestScene::RemoveNPC(char * packet)
{
    sc_packet_remove_npc* Packet = (sc_packet_remove_npc*)packet;
    m_vMonsters[Packet->id]->SetActive(false);
}

