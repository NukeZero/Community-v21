// Obj.h: interface for the CObj class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_22)
#define AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_22

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <D3D9.h>
#include <basetsd.h> 
#include "Model.h"

#include "Ar.h"



//////////////////////////////////////////////////////////////////////////////////////////
// typedef
//////////////////////////////////////////////////////////////////////////////////////////

typedef map< string, DWORD > CMapStrToObjId;
typedef CMapStrToObjId :: value_type MapStrToObjIdType;


//////////////////////////////////////////////////////////////////////////////////////////
// define 
//////////////////////////////////////////////////////////////////////////////////////////
const DWORD		OBJ_FLAG_DELETE		  = 0x00000001;
const DWORD     OBJ_FLAG_VISIBLE      = 0x00000002;
const DWORD     OBJ_FLAG_VIRTUAL      = 0x00000004;
const DWORD     OBJ_FLAG_UPDATEMATRIX = 0x00000008;
const DWORD     OBJ_FLAG_POSCHANGED   = 0x00000010;

const DWORD		OBJ_FLAG_VISIBLE_COLLISION = 0x00000020;
const DWORD     OBJ_FLAG_VISIBLE_NOCOLLISION = 0x00000040;

#define RECTCOLOR_ITEM       0x80ffff00
#define RECTCOLOR_MOVER      0x80ff0000
#define RECTCOLOR_CTRL       0x8000ffff
#define RECTCOLOR_TRIGGER    0x800000ff
#define RECTCOLOR_ATTRIBUTE  0x8000ff00
#define RECTCOLOR_REVIVAL    0x80a0ff40
#define OBJID                DWORD

// 5 : 오브젝트크기 64미터까지 커버됨. 더큰오브젝트를 원할땐 숫자를 늘이시오. Jeff에게 협의바람.
#define MAX_LINKLEVEL	7		

//////////////////////////////////////////////////////////////////////////////////////////
// enum 
//////////////////////////////////////////////////////////////////////////////////////////

// Represents the result of the culling calculation on an object.
enum CULLSTATE
{
    CS_UNKNOWN,      // cull state not yet computed
    CS_INSIDE,       // object bounding box is at least partly inside the frustum
    CS_OUTSIDE,      // object bounding box is outside the frustum
    CS_INSIDE_SLOW,  // OBB is inside frustum, but it took extensive testing to determine this
    CS_OUTSIDE_SLOW, // OBB is outside frustum, but it took extensive testing to determine this
};

enum 
{
	METHOD_NONE, 
	METHOD_EXCLUDE_ITEM
};


//////////////////////////////////////////////////////////////////////////////////////////
// class
//////////////////////////////////////////////////////////////////////////////////////////

class		CCamera;
class		CLight;
class		CLandscape;
class		CWorld;
class		CAIInterface;
struct		ObjProp;

class CObj  
{ 
	friend			CLandscape;

public:
	CObj();
	virtual ~CObj();

private:
	DWORD			m_dwFlags;				// 플래그 ( m_bUpdateMatrix, m_bVisible, m_bDestroy 을 포함 )
	D3DXVECTOR3		m_vPos;					// 오브젝트 좌표 
	FLOAT			m_fAngle;				// 오브젝트 각도 
	D3DXVECTOR3		m_vScale;				// 오브젝트 사이즈 
	D3DXVECTOR3		m_vAxis;				// 오브젝트 회전 축 
	DWORD			m_dwLinkLevel;			// 오브젝트의 최대 반지름 (y축 제외)
	float			m_fAngX;				// X축 각도
#ifdef __Y_ROTXZ
	FLOAT			m_fAngZ;
#endif //__Y_ROTXZ	
	

protected:
	D3DXMATRIX		m_matTrans;				// 좌표 매트릭스 
	D3DXMATRIX		m_matScale;				// 스케일 매트릭스
	D3DXMATRIX		m_matRotation;			// 회전 매트릭스 
	D3DXMATRIX		m_matWorld;				// 모든 매트릭스가 곱해진 결과 

public:
	static int		m_nMethod;
	static CRIT_SEC	m_csMethod;
	static CObj*	m_pObjHighlight;		// 하이라이트 된 오브젝트 
	static CObj*	m_pObjActive;			// 활성화된 오브젝트 - 일반적으로 플레이어를 가리킴 
	static BOOL		m_bCollision;			// 충돌 여부 
	static BOOL		m_bAnimate;				// 오브젝를 애니메이션 할 것인지. 디버그 옵션 

	BBOX			m_OBB;					/// 충돌처리시 필요한 경계상자		//added by gmpbigsun

	CWorld*			m_pWorld;				// 자신이 속한 월드의 포인터를 갖는다. 
	DWORD			m_dwType;				// 타입 
	DWORD			m_dwIndex;				// 색인  
	CModel*			m_pModel;
	CObj*			m_pPrev;				// 이전 노드 
	CObj*			m_pNext;				// 다음 노드 
	DWORD			m_dwObjAryIdx;			// CWorld의 AddObj로 인해 배열 인덱스      
	CObj**			m_ppViewPtr;

	BOOL			m_bSnaped;				// gmpbigsun : 붙엇나?

#ifdef __YAIMONSTER_EX
	DWORD			m_dwAIInterfaceState;
#endif //__YAIMONSTER_EX
	DWORD			m_dwAIInterface;
	CAIInterface*	m_pAIInterface;

	DWORD			m_dwMotionOld;			// 옛날 모션
	DWORD			m_dwMotion;				// 모션
	DWORD			m_dwMotionOption;		// 모션옵션

    D3DXVECTOR3		m_vecBoundsLocal[8];	// bounding box coordinates (in local coord space)
    D3DXVECTOR3		m_vecBoundsWorld[8];	// bounding box coordinates (in world coord space)
    D3DXPLANE		m_planeBoundsWorld[6];	// bounding box planes (in world coord space)
	int				m_nFadeoutCnt;
	WORD            m_wBlendFactor;
	float			m_dy;					// 땜빵
	short			m_nEvent;				// 땜빵코드
	
	short			m_nCnt;		
	CCamera*		m_pCamera;
	CLight*			m_pLight;
	float			m_fDistCamera;			// 카메라로부터의 거리.
	// Cull 관련 
    CULLSTATE		m_cullstate;			// whether object is in the view frustum

	#ifdef __BEAST	
		D3DXVECTOR3	m_vPosGrid;				// 비스트에서 그리드로 좌표를 이동하기 위해 임시로 보관하는 좌표 
		void RenderRect( LPDIRECT3DDEVICE9 pd3dDevice, CRect rect, DWORD dwColor );
	#endif

	#define	MAX_LINKTYPE	4
	enum	{	linkStatic	= 0, linkDynamic	= 1, linkPlayer	= 2,	linkAirShip		= 3,	};

	static BOOL		IsAnimate()						{ return m_bAnimate; }
	static void		AnimateEnable( BOOL bAnimate )	{ m_bAnimate = bAnimate; }
	static int		GetMethod()						{ return m_nMethod;	}
	static void		SetMethod( int nMethod )		{ m_nMethod	= nMethod;	}


	virtual	BOOL	SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0 ) { return FALSE; }
	virtual	BOOL	InitMotion(DWORD dwMotion) { return FALSE; }
	virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );
	virtual void	InitProp();
	virtual	LPCTSTR GetName() { return _T( "hello" ); }
	virtual void	Execute();
	virtual void	Process( FLOAT fElapsedTime );
	virtual BOOL	Read( CFileIO* pFile );
	virtual BOOL	Write( CFileIO* pFile );
	virtual void	Serialize( CAr & ar );
	virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void	RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	virtual void	MessageFromModel( DWORD dwMessage );

	BOOL			IsUpdateMatrix() { 	return (m_dwFlags & OBJ_FLAG_UPDATEMATRIX); }
	void			SetUpdateMatrix( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_UPDATEMATRIX ); }
	BOOL			IsVisible() { return (m_dwFlags & OBJ_FLAG_VISIBLE);  }
	void			SetVisible( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_VISIBLE ); }
	BOOL			IsVirtual() { return (m_dwFlags & OBJ_FLAG_VIRTUAL);  } 
	void			SetVirtual( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_VIRTUAL ); }
	BOOL			IsDelete()	{ return (m_dwFlags & OBJ_FLAG_DELETE);  }
	void			SetDelete( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_DELETE ); }
	BOOL			IsPosChanged()  { return (m_dwFlags & OBJ_FLAG_POSCHANGED); }
	void			SetPosChanged( BOOL bUpdate) { SetFlag( bUpdate, OBJ_FLAG_POSCHANGED ); }

	//gmpbigsun
	void			SetInvisibleCollision( BOOL bColl ) { SetFlag( bColl, OBJ_FLAG_VISIBLE_COLLISION ); }
	BOOL			IsInvisibleCollision( ) { return ( m_dwFlags & OBJ_FLAG_VISIBLE_COLLISION ); }
	void			SetInvisibleNoCollision( BOOL bVal ) { SetFlag( bVal, OBJ_FLAG_VISIBLE_NOCOLLISION ); }
	BOOL			IsInvisibleNoCollision( ) { return ( m_dwFlags & OBJ_FLAG_VISIBLE_NOCOLLISION ); }

	void			AddAngleX( FLOAT fAngle );
	FLOAT			GetAngleX() { return m_fAngX; }
	void			SetAngleX( FLOAT fAngX );
#ifdef __Y_ROTXZ
	FLOAT			GetAngleZ() { return m_fAngZ; }
	void			SetAngleZ( FLOAT fAngZ );
#endif //__Y_ROTXZ	
	void			UpdateLocalMatrix();
	void			UpdateMatrix( BOOL bUpdateBoundBox = TRUE );
	BOOL			IsTouchObj( D3DXVECTOR3* pvPos  );
	BOOL			IsTouchObj( CObj* pObj );
	BOOL			IsRangeObj( CObj *pSrc, float fRange );
	BOOL			IsRangeObj( const D3DXVECTOR3 &vDest, float fRange );		// 좌표로 하는 버전. this가 Src측이 된다.
	BOOL			IsRangeObj_Plane( CObj *pObj, float fRange );
	void			NullNode() { m_pPrev = m_pNext = NULL; }
	CObj*			GetPrevNode() { return m_pPrev; }
	CObj*			GetNextNode() { return m_pNext; }
	void			InsPrevNode(CObj* pBkgr);
	void			InsNextNode(CObj* pBkgr);
	void			DelNode();
	DWORD			GetLinkLevel() { return m_dwLinkLevel; }
	void			ResetScale();
	BOOL			UpdateBoundBox();
	void			SetOnLand();

	DWORD			GetMotion() { return m_dwMotion; }
	void			SetAIInterface( DWORD dwAIInterface, DWORD dwState = 2 );
	ObjProp*		GetProp();
	DWORD			GetType() { return m_dwType; }
	BOOL			IsType( DWORD dwObjType ) { return m_dwType == dwObjType; }
	DWORD			GetIndex() { return m_dwIndex; }
	BOOL			SetTypeIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex, BOOL bInitProp = FALSE );
virtual	BOOL		SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE ) { return SetTypeIndex( pd3dDevice, m_dwType, dwIndex, bInitProp ); }
	void			ProcessAI( long lIdx = 0 );
	void			ProcessAirShip();		// 땜빵코드.
	DWORD			GetLinkType();
	void			Delete();
	void			SetPos( const D3DXVECTOR3& vPos ); 
	void			SetAngle( FLOAT fAngle );
	void			AddAngle( FLOAT fAngle );
	BOOL			TurnAngle( FLOAT fDestAngle, FLOAT fAddAngle );
	BOOL			HowDirAngle( FLOAT fSrcAngle, FLOAT fDestAngle );
	void			SetScale( D3DXVECTOR3 vScale ); 
	FLOAT			GetAngle()	{ return m_fAngle; }
	D3DXVECTOR3		GetScale()	{ return m_vScale; }
	D3DXVECTOR3		GetPos()	{ return m_vPos; }
	CWorld*			GetWorld()	{ return m_pWorld; }
	void			SetWorld( CWorld* pWorld );
	CModel*			GetModel()	{ return m_pModel; }
	BOOL			Pick( D3DXVECTOR3* pPickRayOrg, D3DXVECTOR3* pPickRayDir, D3DXVECTOR3* pvIntersect, 
		                  FLOAT* pfDist, BOOL bBoundBox = FALSE, BOOL bColl = FALSE );
	BOOL			IsDynamicObj()			{ return m_dwType != OT_OBJ && m_dwType != OT_SFX; }
	D3DXMATRIX		GetMatrixTrans()		{ return m_matTrans; } // 좌표 매트릭스 
	D3DXMATRIX		GetMatrixScale()		{ return m_matScale; } // 스케일 매트릭스
	D3DXMATRIX		GetMatrixRotation()		{ return m_matRotation; } // 회전 매트릭스 
	D3DXMATRIX		GetMatrixWorld()		{ return m_matWorld; } // 모든 매트릭스가 곱해진 결과 
	D3DXMATRIX*		GetMatrixWorldPtr()		{ return &m_matWorld; } // 포인터 리턴 버전

	FLOAT			GetRadius();			// 모델의 X,Y,Z평균의 반지름을 구함
	FLOAT			GetRadiusXZ();			// 모델의 X,Z 평면의 반지름을 구함.		

	D3DXVECTOR3		GetLinkPos()							{	return m_vPos;	}
	CPoint			GetClientPoint( LPDIRECT3DDEVICE9 pd3dDevice );
	CPoint			GetClientPoint( LPDIRECT3DDEVICE9 pd3dDevice, int nBoundBoxIndex );
	BOOL			IsShowSmallSize( LPDIRECT3DDEVICE9 pd3dDevice, int nSmallSize );

	static void		SetActiveObj( CObj* pObj )		{ m_pObjActive = pObj; }
	static CObj*	GetActiveObj()					{ return m_pObjActive; }
	BOOL			IsActiveObj()					{ return m_pObjActive == this; }

	BOOL			IsCull() { return m_cullstate == CS_OUTSIDE; } 
	void			SetCullState( CULLSTATE cullStats ) { m_cullstate = cullStats; }

	void		Lock()			{ }
	void		Unlock()		{ }



private:		
	void			CalcLinkLevel();
	void			SetFlag( BOOL bUpdate, DWORD dwValue );
};

inline void CObj::SetFlag( BOOL bUpdate, DWORD dwValue )
{
	if( bUpdate )
		m_dwFlags |= dwValue;
	else
		m_dwFlags &= (~dwValue);
}


#ifdef __SERVER
inline void	CObj::LockMethod( int nMethod, unsigned int uiLine/*=0*/, const char* rostrFileName/*=""*/ )	// {	m_csMethod.Enter( theLineFile );		m_nMethod	= nMethod;	}
{
		m_csMethod.Enter();
		m_nMethod	= nMethod;
}

inline void CObj::UnlockMethod( unsigned int uiLine/*=0*/, const char* rostrFileName/*=""*/ )	// {	m_csMethod.Leave( uiLine, rostrFileName );	}
{
		m_csMethod.Leave();
}
#endif // __SERVER



inline D3DXVECTOR3 VelocityToVec( FLOAT fAngle, FLOAT fVelocity )
{
	FLOAT fTheta = D3DXToRadian( fAngle );
	FLOAT fx = sinf( fTheta ) * fVelocity;
	FLOAT fz = -cosf( fTheta ) * fVelocity;
	return D3DXVECTOR3( fx, 0.0f, fz );
}

inline FLOAT GetDegree( D3DXVECTOR3 vDestPos, D3DXVECTOR3 vSrcPos )
{
	vSrcPos.y = vDestPos.y = 0.0f;
	D3DXVECTOR3 vDir1 = VelocityToVec( 0.0f, 1.0f );
	D3DXVECTOR3 vDir2 = vDestPos - vSrcPos;
	D3DXVec3Normalize( &(vDir1), &(vDir1) );
	D3DXVec3Normalize( &(vDir2), &(vDir2) );
	FLOAT fDot = D3DXVec3Dot( &vDir1, &vDir2 );
	FLOAT fRadian = (FLOAT)( acos( fDot ) );
	FLOAT fDegree = D3DXToDegree( fRadian );
	if( vDir2.x < 0.0f )
		fDegree = 360.0f - fDegree;
	else
		fDegree = fDegree;
	return fDegree;
}

inline FLOAT GetDegreeX( const D3DXVECTOR3 &vDestPos, const D3DXVECTOR3 &vSrcPos )
{
	float	yDist = vDestPos.y - vSrcPos.y;
	D3DXVECTOR3 vXZ = vDestPos - vSrcPos;
	vXZ.y = 0.0f;									// xz평면상에서의 벡터만 필요하다.
	float	fLenXZ  = D3DXVec3Length( &vXZ );		// XZ평면상에서의 길이.
	float	zDist = fLenXZ;

	float r = (float)atan2( yDist, -zDist );
	float a = D3DXToDegree( r );
	a = -a;

	if( a > 90.0f )
		a	= 180.0f - a;
	else if( a < -90.0f )
		a	= -180.0f - a;
	return a;
}

inline BOOL IsValidObj( CObj* pObj ) 
{ 
	return pObj && pObj->IsDelete() == FALSE; 
}

inline BOOL IsInvalidObj( CObj* pObj ) 
{ 
	return pObj == NULL || pObj->IsDelete(); 
}

//
// 오브젝트 타입(OT_OBJ, OT_MOVER.. )을 오브젝트 필터로 변환한다.(OF_OBJ, OF_MOVER... )
// 오브젝트 타입으니 enum형식의 열거값이고, 오브젝트 필터는 비트 연산을 위한 값이다.
// 필드에서 오브젝트를 추출하기 위해서는 열거값인 오브젝트 타입을 사용할 수 없어

// 대안으로 오브젝트 필터 정의를 한 것이다. ( OF_OBJ | OBJ_CTRL | OF_MOVER )
//
inline DWORD ObjTypeToObjFilter( DWORD dwType )
{  
	static DWORD m_dwFilter[] = { OF_OBJ, OF_ANI, OF_CTRL, OF_SFX, OF_ITEM, OF_MOVER, OF_REGION, OF_SHIP };
	return m_dwFilter[ dwType ] ;
}


#endif // !defined(AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_)
