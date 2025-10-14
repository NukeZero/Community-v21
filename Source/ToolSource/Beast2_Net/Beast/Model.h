#ifndef __MODEL_H
#define __MODEL_H

#ifndef __XTOOL
#include "ModelMng.h"
#endif // not xtool
#include "bone.h"

#ifndef __MODELTYPE_NONE
#define MODELTYPE_NONE               0
#define MODELTYPE_MESH               1
#define	MODELTYPE_ANIMATED_MESH		 2
#define MODELTYPE_BILLBOARD          3 
#define MODELTYPE_SFX                4 // ANIMATED_BILLBOARD 4 
#define MODELTYPE_ASE				 5		// ASE모델
#endif
/*
#define MODELTYPE_NONE               0
#define MODELTYPE_STATIC_MESH		 1		// 움직임이 없고 단일 오브젝트로 이뤄진 정적인 일반 메쉬
#define	MODELTYPE_KEYFRAME_MESH	     2		// 오브젝트가 두개이상이고 계층별로 애니메이션 정보가 있다.
#define	MODELTYPE_ANIMATED_MESH	     3		// 애니메이션때 모든 버텍스들의 좌표를 다 가지고 있는 메쉬
#define	MODELTYPE_BIPED_MESH		 4		// 캐릭터 스튜디오의 Biped(본) + Physique(스킨)구조의 모델. 일반적인 인간형이 이에 해당.
#define MODELTYPE_BILLBOARD          5      // 단일 빌보드 오브젝트 
#define MODELTYPE_ANIMATED_BILLBOARD 6      // 애니메이션 가능한 빌보드 
#define MODELTYPE_ASE				 7		// ASE모델
*/
// BoundBox의 Vertex순서 
//     - z
//   3 | 2
// - --+-- + x
//   0 | 1
//
//   7 | 6
// - --+-- + x
//   4 | 5
struct BOUND_BOX
{
	D3DXVECTOR3		m_vPos[8];
};

enum
{
	MODELMSG_BEGINFRAME = 0,
	MODELMSG_ENDFRAME,
	MODELMSG_ACTFRAME
};

// Model Render Effect 
#define REFF_NONE    0
#define REFF_ALPHA   1
#define REFF_SFX     2
//#define	REFF_REFLECT 3

// file version
#define		VER_BONE			4

#define		VER_MESH			20
#define		VER_MOTION			10

// 애니메이션 루핑에 관한...
#define		ANILOOP_1PLAY			(0x00000001)			// 한번 플레이후 끝.
#define		ANILOOP_CONT			(0x00000002)			// 한번 플레이후 마지막 동작으로 지속
#define		ANILOOP_LOOP			(0x00000004)			// 반복
#define		ANILOOP_RETURN			(0x00000008)			// 왕복 - 사용되지 않음.
#define		ANILOOP_BACK			(0x00000010)			// 뒤에서 부터. - 사용되지 않음

#define		COL_WHITE		D3DCOLOR_ARGB(255, 255, 255, 255)
#define		COL_RED			D3DCOLOR_ARGB(255, 255,   0,   0)
#define		COL_GREEN		D3DCOLOR_ARGB(255,   0, 255,   0)
#define		COL_BLUE		D3DCOLOR_ARGB(255,   0,   0, 255)
#define		COL_GRAY		D3DCOLOR_ARGB(255, 128, 128, 128)
#define		COL_DARKGRAY	D3DCOLOR_ARGB(255,  96,  96,  96)
#define		COL_LIGHTGRAY	D3DCOLOR_ARGB(255, 192, 192, 192)

// 뼈대를 VS에 전송하는 방식에 대한 타입(bone send)
#define		BS_NONE			0
#define		BS_MODEL		1		// 모델단위 렌더링전에 뼈대를 전송하는 방식 
#define		BS_OBJECT		2		// GEOMOBJECT단위 렌더링전에 뼈대를 전송하는 방식
#define		BS_MTRLBLK		3		// 매터리얼 블럭단위 렌더링전에 뼈대를 전송하는 방식 - 느림.

class CObj;
class CMesh;

#if defined(__CLIENT) && defined(__YSPARK)
typedef struct _SPARKINFO
{
	D3DXVECTOR3		m_v3Color;
	D3DXVECTOR3		m_v3SparkDir;
	BOOL			m_bUsed;
	FLOAT			m_fLerp;
	int				m_nCnt;

	_SPARKINFO() { m_nCnt = 0; m_fLerp = 1.0f; }
} _SPARKINFO, *_PSPARKINFO;
#endif //defined(__CLIENT) && defined(__YSPARK)

class CModel 
{
protected:
	LPDIRECT3DDEVICE9 m_pd3dDevice;        // The D3D rendering device
	int   m_nModelType;
//	FLOAT m_nBBWidth, m_nBBHeight, m_nBBDepth;
	BOUND_BOX		m_BB;
	DWORD m_dwRenderEffect;
	DWORD   m_dwBlendFactor;
	
public:
	//gmpbigsun
	BOOL m_bRenderCollsion;			//충돌전용 오브젝트일경우 켜진다.
	DWORD   m_dwColor;
	CModel*      m_pBillboard;
//	CMesh*       m_pMesh;
	D3DXVECTOR3	m_vMin, m_vMax;
#ifndef __COLA
	LPMODELELEM m_pModelElem;
#endif

	BOOL IsNullBoundBox() { if( m_vMin == m_vMax || m_vMin.x == 65535.0f ) return TRUE; return FALSE; }
	
	CModel* m_pModel;
	int		m_nPause;		// Pause대기 시간.  0이면 Pause상태 아님 1이상이면 포즈상태고 숫자는 대기시간
	int		m_bSlow;		// 슬로우 모드.
	BOOL	m_bEndFrame;		// 마지막 프레임까지 왔는가?
	float	m_fFrameCurrent;
	float	m_fFrameOld;	// 바로 이전의 m_fFrameCurrent
	int		m_nFrameMax;
//	float	m_fKeySpeed;		// sec per key - 한 애니메이션 키가 넘어가는데 기다리는 시간
	float	m_fPerSlerp;		// 애니메이션 프레임 증가량
	float	m_fSlp;				// 프레임사이의 보간량
	int		m_nLoop;			// 애니메이션 루핑 방법
	float	m_fScrlU;
	float	m_fScrlV;
	float	m_Tu, m_Tv;			// 현재 U/V값.
	CObj* m_pObj;
	BOOL	m_bSkin;			// 스키닝 오브젝트냐.

#if defined(__YSPARK) && defined(__CLIENT)
	_SPARKINFO	m_SparkInfo;
#endif //defined(__YSPARK) && defined(__CLIENT)

#ifdef __COLA
	BOOL	m_bNormalClear;
#endif
		int		m_nNoTexture;		// 텍스쳐를 나타내지 않음.
	int		m_nNoEffect;		// 내부에서 알파효과에 관한 어떠한 렌더스테이트도 변화시키지 않는다.
	
	CModel() { Init(); }
	CModel(int nModelType) { Init(); m_nModelType = nModelType; }
	virtual ~CModel();

	void Init( void )
	{
		m_nModelType = MODELTYPE_MESH; 
		m_pObj = NULL; 
		m_pModel = NULL;
		m_dwRenderEffect = REFF_NONE;
//		m_pMesh = NULL;
		m_pBillboard = NULL;
		InitAnimate();
		m_vMin.x = m_vMin.y = m_vMin.z = 0;
		m_vMax.x = m_vMax.y = m_vMax.z = 0;
		m_fScrlU = m_fScrlV = 0.0f;
		m_Tu = m_Tv = 0.0f;
		m_dwBlendFactor = 255;
		m_bRenderCollsion = FALSE;
		m_dwColor = 0;
		m_pd3dDevice = NULL;
		m_bSkin = FALSE;
#if defined(__CLIENT) && defined(__YSPARK)
		memset( &m_SparkInfo, 0, sizeof(_SPARKINFO) );
#endif //defined(__CLIENT) && defined(__YSPARK)

#ifdef __COLA
		m_bNormalClear = 0;
#else
		m_pModelElem = NULL;
#endif
		m_nNoTexture = 0;
		m_nNoEffect = 0;
		m_bSlow = FALSE;
	}
	void InitAnimate( void )
	{
		m_fFrameCurrent = 0.0f;
		m_fFrameOld = -1.0f;
		m_nFrameMax = 0;
		m_nPause = 0;
//		m_fKeySpeed = (float)1 / 30;
		m_bEndFrame = FALSE;
		m_nLoop = ANILOOP_LOOP;
		m_fSlp = 0.0f;
		m_fPerSlerp = 0.5f;						// 프레임수의 정확도땜에 0.5로 했다
		m_bSlow = FALSE;
	}
	void    SetBlendFactor( DWORD dwBlendFactor ) { m_dwBlendFactor = dwBlendFactor; }
	DWORD   GetBlendFactor( ) { return m_dwBlendFactor; }
	BOOL    IsAniable();
	BOOL	IsEndFrame() { return m_bEndFrame; }
	int     GetModelType() { return m_nModelType; }
	int		GetType() { return m_nModelType; }
	void	SetModelType(int nModelType) { m_nModelType = nModelType; }
//	void	SetDevice( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; }
	void	SetLoop( int nLoop ) { m_nLoop = nLoop; }
	void	SetSpeed( float fPerSlerp ) { m_fPerSlerp = fPerSlerp; }	// 일반 0.5
	const BOUND_BOX	*GetBBVector( void ) { if( m_pModel ) return &m_pModel->m_BB; return &m_BB; } // 바운딩박스의 8귀퉁이 좌표를 리턴
	float	GetRadius( void );

	DWORD GetRenderEffect() { if( m_pModel ) return m_pModel->m_dwRenderEffect; return m_dwRenderEffect; }
	double GetCurrentFrame() { return m_fFrameCurrent; }
	int	GetMaxFrame() { return m_nFrameMax; }
	int	GetNextFrame();

	virtual int LoadModel( LPCTSTR szFileName ) { return 1; }
	virtual int SaveModel( LPCTSTR szFileName ) { return 1; }
	virtual void	ClearNormal( void ) {}		// 모든 노말을 초기화시킨다.

	virtual void AddFrame( FLOAT fAddFrame );
	virtual void SetFrame( FLOAT fFrame );

	// Animation - 일단여기 넣었는데 깝깝하다.....
	virtual BOOL	IsLoadAttr( void ) { return FALSE; }
	virtual MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return NULL; }
	virtual DWORD	GetAttr( int nNumFrm ) { return 0; }
	virtual DWORD	IsAttrHit( float fNumFrm ) { return 0; }
	virtual DWORD	IsAttrHit( void ) { return 0; }
	virtual MOTION_ATTR *IsAttrSound( float fNumFrm ) { return NULL; }
	virtual MOTION_ATTR *IsAttrSound( void ) { return 0; }
	virtual DWORD	IsAttrQuake( float fNumFrm ) { return 0; }
	virtual DWORD	IsAttrQuake( void ) { return 0; }
	virtual void	SetAttr( float fNumFrm, DWORD dwAttr ) {}
	virtual void	ResetAttr( int nNumFrm, DWORD dwAttr ) {}
#ifdef __COLA
	virtual int		GetMaxObject( void ) { return 0;}			// GMOBJECT 수
	virtual int		GetMaxVertex( void ) { return 0;}			// 총 버텍스 개수
	virtual int		GetMaxFace( void ) { return 0;}				// 총 면수
	virtual int		GetMaxMtrlBlk( void ) { return 0;}			// 총 매터리얼 블럭 수
	virtual int		GetMaxBone( void ) { return 0; }			// 본 개수.
	virtual char	*GetBonesName( char *pBuff ) { return NULL; }				// 본이 있을때 본 이름들을 pBuff에 담아준다.
#endif
	virtual	void	SetGroup( int nNum ) {}		// Set LOD Group
	virtual int		IsHaveCollObj( void ) { return 0; }
	
	// Rendering
	virtual BOOL Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
	virtual void RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL ) {}
	virtual BOOL RenderBB( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld ) { return S_OK; }
	virtual void FrameMove( D3DXVECTOR3 *pvSndPos = NULL, float fSpeed = 1.0f );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; return S_OK; }
	virtual HRESULT RestoreDeviceObjects() { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects() { return S_OK; }
	virtual HRESULT DeleteDeviceObjects() { return S_OK; }

	virtual BOOL	Intersect( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) { return FALSE; }
	BOOL IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist );

	virtual D3DXVECTOR3 *IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) { return NULL; }
	virtual D3DXVECTOR3 *IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir ) { return NULL; }

	virtual	BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, BOOL bNeedCollObject = TRUE ) { return FALSE;}

	FLOAT GetMaxWidth() { return ( m_vMax.x - m_vMin.x > m_vMax.z - m_vMin.z ) ? m_vMax.x - m_vMin.x : m_vMax.z - m_vMin.z; }
	FLOAT GetMaxHeight() { return m_vMax.y - m_vMin.y; }
	
/*
	// Process
	virtual HRESULT FrameMove(FLOAT m_fElapsedTime)  { return S_OK; }

	// Initializing
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice,D3DCAPS8* pd3dCaps,D3DSURFACE_DESC* pd3dsdBackBuffer)  { return S_OK; }
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice) { return S_OK; }

	// Creation/destruction
	virtual BOOL Load(LPDIRECT3DDEVICE9 pd3dDevice,TCHAR* lpszFileName) { return TRUE; }
	*/
};

#endif



