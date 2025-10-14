#pragma once


#define		MODE_WORLD_ROTATE 1
#define		MODE_MODEL_ROTATE 2

class Camera
{
public:
	enum CameraType { LANDCRAFT, AIRCRAFT };

private:
	int			m_nMode;
	D3DXVECTOR3 m_vTargetPos;
	CameraType  _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;

public:
	
	Camera();
	Camera(CameraType cameraType);
	~Camera();

	void strafe(float units); // left/right
	void fly(float units);    // up/down
	void walk(float units);   // forward/backward
	
	void pitch(float angle); // rotate on right vector
	void yaw(float angle);   // rotate on up vector
	void roll(float angle);  // rotate on look vector

	void Update();
	void getViewMatrix(D3DXMATRIX* V); 
	void setCameraType(CameraType cameraType); 
	void switchMode(int	nMode);
	void SetTarget(D3DXVECTOR3 vPos);
	void getPosition(D3DXVECTOR3* pos); 
	void setPosition(D3DXVECTOR3* pos);

	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);

	float AddDistance(float fAmount);

	// 추적모드 관련
	D3DXVECTOR3    m_vPos        ; // 카메라가 있는 위치 

	D3DXMATRIX     m_matView     ; // View 행렬 저장 
	D3DXMATRIX     m_matInvView  ; // View 역행렬 저장 

	D3DXVECTOR3    m_vLookAt     ; // 카메라가 보는 방향

	D3DXVECTOR3    m_vPosDest    ; // 카메라 이동 목표
	D3DXVECTOR3    m_vLookAtDest ; // 카메라 보는 위치 이동 목표 

	D3DXVECTOR3    m_vOffset     ; // 캐릭터 위치에서 카메라 상대 위치
	D3DXVECTOR3    m_vOffsetDest ; // 캐릭터 위치에서 카메라 상대 위치 이동 목표

	float		   m_fCameraPitchAngle, m_fCameraYawAngle;
	float		   m_fDistance;

	DWORD          m_dwCamStyle  ;
	FLOAT m_fRotx,m_fRoty; // 마우스 조작에 의한 x,y축 회전값의 목표점
	FLOAT m_fCurRotx,m_fCurRoty; // 현재 마우스 조작에 의한 x,y축 회전값. 서서히 목표값에 가까와진다.
	
	int		m_nQuakeSec;
	float	m_fQuakeSize;		// 진동크기. 진동폭을 넣는다. 1.0f는 1미터폭으로 흔들린다.
	BOOL m_bLock;
	FLOAT m_fLength1;
	FLOAT m_fLength2;
	BOOL  m_bOld;
	BOOL  m_bStart;
	
	FLOAT m_fBackupZoom;

	CObj* pObjTarget;
	FLOAT m_fZoom;

	BOOL IsLock() { return m_bLock; }
	void Lock();
	void Unlock();

	int  GetAnglePie( FLOAT fAngle );
	
	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual	void Transform( LPDIRECT3DDEVICE9 pd3dDevice);
	void SetPos( D3DXVECTOR3& vPos ) { m_vPos = vPos; }
	D3DXVECTOR3 GetPos() { return m_vPos; }

};
