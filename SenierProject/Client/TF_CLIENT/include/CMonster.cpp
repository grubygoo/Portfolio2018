#include "stdafx.h"
#include "CMonster.h"
#include "Shader.h"
#include "md5Loader.h"

CStoneMon::CStoneMon()
{


}

CStoneMon::~CStoneMon()
{

}

bool CStoneMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_pModels = new Model3D();
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Stone/StoneMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	//CIlluminatedShader *pShader = new CIlluminatedShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Stone/Stonemon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

bool CStoneMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, INSTANCEOB obj)
{
	m_pModels = new Model3D();
	*m_pModels = obj.Model;
	CMesh* TempMesh = obj.Mesh;

	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Stone/StoneMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Stone/Stonemon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

bool CStoneMon::InitializeShadow(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_pModels = new Model3D();
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Stone/StoneMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	pShader->isShadow = true;
	//CIlluminatedShader *pShader = new CIlluminatedShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Stone/Stonemon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.3f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.002f, 0.0f);

	MTShadow = S * shadowOffSetY;

	return true;
}

void CStoneMon::Update(float fDeltaTime)
{
	if (!m_bActive) return;

	//if (m_AnimState == HEATSTATE) {
	//	fDeltaTime *= 2;
	//
	//	if (m_pModels->animations[HEATSTATE].currAnimTime +0.2f>= m_pModels->animations[DEADSTATE].totalAnimTime)
	//	{
	//		m_AnimState = IDLESTATE;
	//	}
	//}

	for (int i = 0; i < m_nMeshes; ++i) 
	{
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[DEADSTATE].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
	if (m_AnimState == HEATSTATE)
	{
		HeatTime += fDeltaTime;
		if (HeatTime+0.1f >= m_pModels->animations[HEATSTATE].totalAnimTime)
		{
			m_AnimState = IDLESTATE;
			HeatTime = 0.f;
			m_pModels->animations[HEATSTATE].currAnimTime = 0.f;
			
		}

	}

}

void CStoneMon::Update(float fDeltaTime, XMFLOAT4X4 worldMt)
{
	if (!m_bActive) return;
	SetWMatrix(Matrix4x4::Multiply(worldMt, MTShadow));

	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}

	m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}

}

CBeatleMon::CBeatleMon()
{

}

CBeatleMon::~CBeatleMon()
{

}

bool CBeatleMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CMesh* TempMesh;
	m_pModels = new Model3D();
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Beatle/BeetleMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Beatle/BeetleMon.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

bool CBeatleMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, INSTANCEOB obj)
{
	m_pModels = new Model3D();
	*m_pModels = obj.Model;
	CMesh* TempMesh = obj.Mesh;

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Beatle/BeetleMon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

bool CBeatleMon::InitializeShadow(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_pModels = new Model3D();
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Beatle/BeetleMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	pShader->isShadow = true;
	//CIlluminatedShader *pShader = new CIlluminatedShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Beatle/BeetleMon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.3f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.004f, 0.0f);

	MTShadow = S * shadowOffSetY;

	return true;
}

void CBeatleMon::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}
	//if (m_AnimState == MOVESTATE) {Rotate(m_Direction); CMoveObject::Move(Vector3::ScalarProduct(GetLook(), fDeltaTime * 7));}
	//m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
	if (m_AnimState == HEATSTATE)
	{
		HeatTime += fDeltaTime;
		if (HeatTime + 0.1f >= m_pModels->animations[HEATSTATE].totalAnimTime)
		{
			m_AnimState = IDLESTATE;
			HeatTime = 0.f;
			m_pModels->animations[HEATSTATE].currAnimTime = 0.f;

		}

	}

}

void CBeatleMon::Update(float fDeltaTime, XMFLOAT4X4 worldMt)
{
	if (!m_bActive) return;
	SetWMatrix(Matrix4x4::Multiply(worldMt, MTShadow));

	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}

	m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
}

bool CStoneBOSS::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_pModels = new Model3D();
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Stone/StoneMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1.3, 1.3, 1.3);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StonemonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Stone/StoneMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Stone/StoneMonMap_2.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

void CStoneBOSS::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}
	//if (m_AnimState == MOVESTATE) { Rotate(m_Direction); CMoveObject::Move(Vector3::ScalarProduct(GetLook(), fDeltaTime * 5)); }
	//m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
	if (m_AnimState == HEATSTATE)
	{
		HeatTime += fDeltaTime;
		if (HeatTime + 0.1f >= m_pModels->animations[HEATSTATE].totalAnimTime)
		{
			m_AnimState = IDLESTATE;
			HeatTime = 0.f;
			m_pModels->animations[HEATSTATE].currAnimTime = 0.f;

		}

	}

}

bool CBeatleBOSS::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CMesh* TempMesh;
	m_pModels = new Model3D();
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Beatle/BeetleMonMesh.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1.3, 1.3, 1.3);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonIdle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonWalk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonAttack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDamaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Beatle/BeetleMonDead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Beatle/BeetleMon_map2.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

void CBeatleBOSS::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}
	if (m_AnimState == MOVESTATE) { Rotate(m_Direction); CMoveObject::Move(Vector3::ScalarProduct(GetLook(), fDeltaTime * 7)); }
	m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
	if (m_AnimState == HEATSTATE)
	{
		HeatTime += fDeltaTime;
		if (HeatTime + 0.1f >= m_pModels->animations[HEATSTATE].totalAnimTime)
		{
			m_AnimState = IDLESTATE;
			HeatTime = 0.f;
			m_pModels->animations[HEATSTATE].currAnimTime = 0.f;

		}

	}
}

bool Mon_Shadow::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, XMFLOAT3 position)
{
	CTexturedRectMesh *pRectMesh = NULL;
	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 5.0, 0.0, 2.0, position.x, 0.001, position.z);
	SetMesh(0, pRectMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Shadow.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void Mon_Shadow::Update(float fDeltaTime, XMFLOAT4X4 WMTX, bool active)
{
	SetWMatrix(WMTX);
	SetActive(active);
}

bool Groot::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CMesh* TempMesh;
	m_pModels = new Model3D();
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Groot/GrootBossMesh_0914.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_idle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_walk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_attack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_damaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_dead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Groot/GrootTex.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

bool Groot::InitializeShadow(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_pModels = new Model3D();
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/Movable/Monster/Groot/GrootBossMesh_0914.md5mesh", *m_pModels, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_idle.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_walk.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_attack.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_damaged.md5anim", *m_pModels);
	LoadMD5Anim(L"Assets/Model/Movable/Monster/Groot/GrootBossAni_dead.md5anim", *m_pModels);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	pShader->isShadow = true;
	//CIlluminatedShader *pShader = new CIlluminatedShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Monster/Groot/GrootTex.dds"), 0);


	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.3f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.004f, 0.0f);

	MTShadow = S * shadowOffSetY;

	return true;
}

void Groot::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}
	if (m_AnimState == MOVESTATE) { Rotate(m_Direction); CMoveObject::Move(Vector3::ScalarProduct(GetLook(), fDeltaTime * 7)); }
	m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
}

void Groot::Update(float fDeltaTime, XMFLOAT4X4 worldMt)
{
	if (!m_bActive) return;
	SetWMatrix(Matrix4x4::Multiply(worldMt, MTShadow));

	for (int i = 0; i < m_nMeshes; ++i) {
		if (m_ppMeshes[i] != NULL) UpdateMD5Model(m_ppMeshes[i], *m_pModels, fDeltaTime, m_AnimState);
	}

	m_OOBB.Center = GetWPosition();

	if (m_AnimState == DEADSTATE)
	{
		m_DeadTime += fDeltaTime;
		if (m_DeadTime + 0.2f >= m_pModels->animations[m_AnimState].totalAnimTime)
		{
			SetActive(false);
			m_DeadTime = 0;
		}

	}
}
