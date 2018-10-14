#pragma once
#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
#define FIRST_PERSON_CAMERA			0x01
#define SPACESHIP_CAMERA			0x02
#define THIRD_PERSON_CAMERA			0x03

class CGameObject;
struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT3						m_xmf3Position;
};

class CCamera 
{
protected:
	CGameObject*                    m_pTarget;   //Target 
	DWORD							m_nMode;     // 카메라 모드 
	float           				m_fTimeLag;
	float							movingTime = 0;
	XMFLOAT3						m_xmf3Position;
	XMFLOAT3						m_xmf3Right;
	XMFLOAT3						m_xmf3Up;
	XMFLOAT3						m_xmf3Look;

	XMFLOAT3						m_xmf3LookAtWorld;
	XMFLOAT3						m_xmf3Offset;
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;

	D3D12_VIEWPORT					m_d3dViewport;
	D3D12_RECT						m_d3dScissorRect;
	ID3D12Resource*                 m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO*              m_pcbMappedCamera = NULL;
	BoundingFrustum                 m_xmFrustum;

public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();

public:
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 60.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Update(float fTimeElapsed);
	virtual void SetLookAt( XMFLOAT3& xmf3LookAt) { }
	void shakeCamera(float);

public: 
	void           GenerateViewMatrix();
	void           GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void           GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	CGameObject*   GetTarget()           { return(m_pTarget); }
	XMFLOAT3&      GetPosition()         { return(m_xmf3Position); }
	XMFLOAT3&      GetLookAtPosition()   { return(m_xmf3LookAtWorld); }
	XMFLOAT3&      GetRightVector()      { return(m_xmf3Right); }
	XMFLOAT3&      GetUpVector()         { return(m_xmf3Up); }
	XMFLOAT3&      GetLookVector()       { return(m_xmf3Look); }
	XMFLOAT3&      GetOffset()           { return(m_xmf3Offset); }
	XMFLOAT4X4     GetViewMatrix()       { return(m_xmf4x4View); }
	XMFLOAT4X4     GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport()         { return(m_d3dViewport); }
	D3D12_RECT     GetScissorRect()      { return(m_d3dScissorRect); }
	DWORD          GetMode()             { return(m_nMode); }
	float          GetTimeLag()          { return(m_fTimeLag); }

public:
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetTarget(CGameObject *pTarget) { m_pTarget = pTarget; }
	void SetMode(DWORD nMode) { m_nMode = nMode; }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z; }
	void RegenerateViewMatrix();
	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	void GenerateFrustum();
	bool IsInFrustum(BoundingOrientedBox& oobb);
};


class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera( CCamera *pCamera);
	virtual ~CFirstPersonCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera( CCamera* pCamera);
	virtual ~CThirdPersonCamera() { }
	virtual void Update(float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};

