//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_pRootArgumentInfos2 = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures){
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
	}
	if (m_pRootArgumentInfos){
		delete[] m_pRootArgumentInfos;
	}
	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
	//m_pRootArgumentInfos[nIndex].m_d3dCbvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetRootArgumentCbv(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGpuDescriptorHandle)
{
	m_pRootArgumentInfos2[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos2[nIndex].m_d3dSrvGpuDescriptorHandle = d3dCbvGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		//for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, int c)
{
	//if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	//{
	//	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	//}
	//else
	{
		//for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(5, m_pRootArgumentInfos2[c].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_nReflection = 0;
	m_pTexture = NULL;
	m_pShader = NULL;
}

CMaterial::~CMaterial()
{   
	if (m_pTexture) delete m_pTexture;
	if (m_pShader) delete m_pShader;
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) 
		m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}





CGameObject::CGameObject(int nMeshes)
{
	m_pd3dcbGameObject = NULL;
	m_pcbMappedGameObject = NULL;
	m_bActive = true;
	m_ppMeshes = NULL;
	m_pMaterial = NULL;
	m_xmf4x4World = Matrix4x4::Identity();
	SetWPosition(10.f, 0.f, 0.f);
	m_nMeshes = nMeshes;
	if (m_nMeshes > 0) {
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)   m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) delete m_ppMeshes[i];
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_pMaterial) delete m_pMaterial;
}

bool CGameObject::IsInCamera(CCamera * pCamera)
{
	//bool bIsVisible = false;
	//BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox(); 
	////모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다.
	//xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World)); 
	//
	//if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox); 
	//
	//return(bIsVisible); 
	return true;
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		CMaterial *pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

bool CGameObject::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	return true;
}

ID3D12Resource* CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	//UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255)
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();

     //m_MD5Model.animations.clear();
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera,UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)){
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

void CGameObject::RenderShadow(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	if (shadowUpdate) {
		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// xz평면
		XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.5f, -1.0f, 0.0f));
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowOffSeteY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
		m_xmf4x4Shadow = m_xmf4x4World;
		m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4Shadow, S*shadowOffSeteY);
		//XMStoreFloat4x4(&m_xmf4x4World, );
		shadowUpdate = false;
	}
	UpdateShaderVariables(pd3dCommandList);

	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
		}
		//if (m_pMaterial->m_pTexture)
		//	m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	//pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

void CGameObject::Update(float fDeltaTime)
{


}

void CGameObject::Update(float fDeltaTime, XMFLOAT4X4 worldmtx)
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
	}
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

bool CGameObject::CheckCollision(const BoundingOrientedBox & other)
{
	return m_OOBB.Intersects(other);
}

void BillboardObject::SetLookAt(XMFLOAT3& xmf3Target)
{
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void BillboardObject::Update(float fTimeElapsed, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition);
}


CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength,
	int nBlockWidth, int nBlockLength, XMFLOAT3& xmf3Scale, XMFLOAT4& xmf4Color) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	m_xmf3Scale = xmf3Scale;
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CTexture *pTerrainTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Land01.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Detail_Texture_7.dds", 1);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	CTerrainShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 6, true);
	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);
	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTerrainShader);
}

CPlaneMap::CPlaneMap(int nWidth, int nLength ) : CGameObject(nWidth*nLength)
{
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다. 
	m_nWidth = nWidth;
	m_nLength = nLength;


}

bool CPlaneMap::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	CPlaneMapGridMesh *pPlaneMapGridMesh = NULL;
	CTexturedRectMesh *pRectMesh = NULL;

	int index = 0;
	for (int z = 0, zStart = 0; z < m_nLength; z++) {
		for (int x = 0, xStart = 0; x < m_nWidth; x++) {
			xStart = (x) * 40 - 100;
			zStart = (z) * -40 + 100;
			pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 40.0, 0.0, 40.0, xStart, 0.0, zStart);
			SetMesh(index, pRectMesh);
			index++;
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	CTexture *pTerrainTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Ground.dds", 0);
	
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	
	CTexturedShader* pTexturedShader = new CTexturedShader();
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

CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Front_0.dds", 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Back_0.dds", 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Left_0.dds", 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Right_0.dds", 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Top_0.dds", 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Bottom_0.dds", 5);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 6);
	pSkyBoxShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkyBoxTexture, 10, false);


	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	SetMaterial(pSkyBoxMaterial);
	SetCbvGPUDescriptorHandle(pSkyBoxShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{

}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetWPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
	if (m_pMaterial){
		if (m_pMaterial->m_pShader){
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes){
		for (int i = 0; i < m_nMeshes; i++){
			if (m_pMaterial) if (m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

bool CEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 0.0f, 10.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(16, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_01.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_02.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_03.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_04.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_05.dds", 4);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_06.dds", 5);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_07.dds", 6);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_08.dds", 7);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_09.dds", 8);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_10.dds", 9);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_11.dds", 10);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_12.dds", 11);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_13.dds", 12);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_14.dds", 13);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_15.dds", 14);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/Effect_16.dds", 15);


	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 16);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 20;
	if (etime > 16)
	{
		etime = 0;
		isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{
	
	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CChangeEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 15.0f, 0.0f, 15.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(5, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/change1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/change2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/change3.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/change4.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/change5.dds", 4);
	


	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 5);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CChangeEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 10;
	if (etime > 5)
	{
		etime = 0;
		isOn = false;
		
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CChangeEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CChangeEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CChangeEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CFairyEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 0.0f, 10.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(16, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect3.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect4.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect5.dds", 4);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect6.dds", 5);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect7.dds", 6);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect8.dds", 7);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect9.dds", 8);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect10.dds", 9);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect11.dds", 10);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect12.dds", 11);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect13.dds", 12);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect14.dds", 13);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect15.dds", 14);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/FairyEffect16.dds", 15);



	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 16);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CFairyEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 20;
	if (etime > 16)
	{
		etime = 0;
		isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CFairyEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CFairyEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CFairyEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CTeleportEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/teleport1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/teleport2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/teleport3.dds", 2);




	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CTeleportEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 10;
	//skilltime += fTimeElapsed * 10;
	//if (skilltime > 10)
	//{
	//   isOn = false;
	//   etime = 0;
	//   skilltime = 0;
	//}
	if (etime > 3)
	{
		etime = 0;
		isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CTeleportEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CTeleportEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CTeleportEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CWarriorSkillEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(5, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/warskill1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/warskill2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/warskill3.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/warskill4.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/warskill5.dds", 4);



	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 5);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CWarriorSkillEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 10;
	skilltime += fTimeElapsed * 10;
	if (skilltime > 15)
	{
		isOn = false;
		etime = 0;
		skilltime = 0;
	}
	if (etime > 5)
	{
		etime = 0;
		//isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CWarriorSkillEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CWarriorSkillEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CWarriorSkillEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CFairySkillEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 0.0f, 10.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(5, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/faiskill1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/faiskill2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/faiskill3.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/faiskill4.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/faiskill5.dds", 4);



	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 5);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CFairySkillEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 10;
	skilltime += fTimeElapsed * 10;


	if (etime > 5)
	{
		skilltime = 0;
		etime = 0;
		isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CFairySkillEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CFairySkillEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CFairySkillEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

bool CSanyangSkillEffect::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void *pContext)
{
	//CPlayer *pTerrain = (CPlayer *)pContext;
	//SetWPosition(pTerrain->GetWPosition());

	CTexturedRectMesh *pRectMesh = NULL;


	pRectMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 0.0f, 10.0f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(4, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/sanskill1.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/sanskill2.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/sanskill3.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Effect/sanskill4.dds", 3);




	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CChangeShader* pTexturedShader = new CChangeShader();
	pTexturedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	//pTexturedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTexturedShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 4);
	pTexturedShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTexturedShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 5, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTexturedShader->GetGPUCbvDescriptorStartHandle());
	SetShader(pTexturedShader);

	return true;
}

void CSanyangSkillEffect::Animate(float fTimeElapsed, CCamera * pCamera)
{
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//XMFLOAT3 Right = pCamera->GetRightVector();
	//SetLookAt(xmf3CameraPosition, Right);
	etime += fTimeElapsed * 10;
	skilltime += fTimeElapsed * 10;


	if (etime > 4)
	{
		skilltime = 0;
		etime = 0;
		isOn = false;
		//ReleaseShaderVariables();
		//ReleaseUploadBuffers();
		//delete this;
		//return;
	}



	XMFLOAT4X4 m = pCamera->GetViewMatrix();
	SetLookAt2(m);
}

void CSanyangSkillEffect::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 right)
{

	XMFLOAT3 xmf3Right = right;
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x, m_xmf4x4World._12 = xmf3Right.y, m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x, m_xmf4x4World._22 = xmf3Up.y, m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x, m_xmf4x4World._32 = xmf3Look.y, m_xmf4x4World._33 = xmf3Look.z;
}

void CSanyangSkillEffect::SetLookAt2(XMFLOAT4X4 & tartget)
{
	XMFLOAT4X4 m = tartget;


	XMFLOAT4X4 m2;
	m2 = Matrix4x4::Identity();
	m2._11 = m._11;
	m2._13 = m._13;
	m2._31 = m._31;
	m2._33 = m._33;
	m2 = Matrix4x4::Inverse(m2);
	m2._41 = GetWPosition().x;
	m2._42 = GetWPosition().y;
	m2._43 = GetWPosition().z;

	m_xmf4x4World._11 = m2._11;
	m_xmf4x4World._12 = m2._12;
	m_xmf4x4World._13 = m2._13;
	m_xmf4x4World._21 = m2._21;
	m_xmf4x4World._22 = m2._22;
	m_xmf4x4World._23 = m2._23;
	m_xmf4x4World._31 = m2._31;
	m_xmf4x4World._32 = m2._32;
	m_xmf4x4World._33 = m2._33;

	m_xmf4x4World._41 = m2._41;
	m_xmf4x4World._42 = m2._42;
	m_xmf4x4World._43 = m2._43;

}

void CSanyangSkillEffect::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, etime);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}