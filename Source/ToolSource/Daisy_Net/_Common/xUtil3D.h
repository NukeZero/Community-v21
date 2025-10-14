#ifndef	__XUTIL3D_H__
#define	__XUTIL3D_H__

void	SetBB( D3DXVECTOR3 *pBBVList, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );

// AABB간의 충돌검출
int		IsTouchAABB( const D3DXVECTOR3 &vMin1, const D3DXVECTOR3 &vMax1,  
					 const D3DXVECTOR3 &vMin2, const D3DXVECTOR3 &vMax2 );

// Ray와 AABB간의 충돌검출
BOOL	IsTouchRayAABB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, 
					    const D3DXVECTOR3 &vBoxOrig, const D3DXVECTOR3 &vBoxSize );
BOOL	IsTouchRayAABB2( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
					     const D3DXVECTOR3& vBoxOrig, const D3DXVECTOR3& vBoxSize );
BOOL	IsTouchRayAABB3( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
					     const D3DXVECTOR3& vBoxOrig, const D3DXVECTOR3& vBoxSize );

BOOL	IsTouchRaySphere( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
						  const D3DXVECTOR3& vPos, float fR );
BOOL	IsTouchRaySphere2( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
						  const D3DXVECTOR3& vPos, float fR );

void	CalcRay( D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir, 
				 int nScrW, int nScrH,
				 int nMouX, int nMouY, 
				 const D3DXMATRIX &mProj, const D3DXMATRIX &mView, const D3DXMATRIX *pmWorld = NULL );

BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXMATRIX &m, 
						 const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *pvIntersect );
BOOL	IsTouchAABB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, 
						  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, 
						  D3DXVECTOR3 *pvIntersect );

BOOL	IsTouchRayTri( const D3DXVECTOR3 *v0, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2,
					   const D3DXVECTOR3 *vOrig, const D3DXVECTOR3 *vDir, 
					   FLOAT* pfDist );

void	CalcSlideVec( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vN );
void	CalcFaceNormal( D3DXVECTOR3 *pNormal, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3 );
void	CalcFaceUnitNormal( D3DXVECTOR3 *pNormal, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3 );

inline void AngleToVector( D3DXVECTOR3 *vDelta, float fAngXZ, float fAngH, float fSpeed )
{
	float	fDist;

	fAngXZ = D3DXToRadian(fAngXZ);
	fAngH  = D3DXToRadian(fAngH);
	fDist = cosf(-fAngH) * fSpeed;
	vDelta->x = sinf(fAngXZ) * fDist;
	vDelta->z = -cosf(fAngXZ) * fDist;
	vDelta->y = -sinf(-fAngH) * fSpeed;
}

inline void AngleToVectorXZ( D3DXVECTOR3 *vDelta, float fAngXZ, float fSpeed )
{
	fAngXZ = D3DXToRadian(fAngXZ);
	vDelta->x = sinf(fAngXZ) * fSpeed;
	vDelta->z = -cosf(fAngXZ) * fSpeed;
	vDelta->y = 0;
}

// XZ평면상에서의 원점을 기준으로 한 벡터의 각도
inline FLOAT xGetDegreeXZ( float x, float z )
{
	return D3DXToDegree( (FLOAT)atan2( x, -z ) );
}

// XZ평면의 벡터길이와 y좌표간의 각도.
inline FLOAT xGetDegreeH( float fLenXZ, float y )
{
	return D3DXToDegree( (FLOAT)atan2( fLenXZ, y ) );
}

// XZ평면의 각도와 높이쪽 각도를 한꺼번에 구함.
inline void xGetDegree( float *pfAngXZ, float *pfAngH, const D3DXVECTOR3 &vDist )
{
	D3DXVECTOR3 vDistXZ = vDist;
	vDistXZ.y = 0;
	FLOAT fAngXZ = D3DXToDegree( (FLOAT)atan2( vDist.x, -vDist.z ) );		// 우선 XZ평면의 각도를 먼저 구함
	FLOAT fLenXZ = D3DXVec3Length( &vDistXZ );						// y좌표를 무시한 XZ평면에서의 길이를 구함.
	FLOAT fAngH  = D3DXToDegree( (FLOAT)atan2( fLenXZ, vDist.y ) );		// XZ평면의 길이와 y높이간의 각도를 구함.
	fAngH -= 90.0f;
	
	*pfAngXZ = fAngXZ;
	*pfAngH  = fAngH;
}

//-----------------------------------------------------------------------------
// Name: class CArcBall
// Desc:
//-----------------------------------------------------------------------------
class CArcBall
{
    INT            m_iWidth;   // ArcBall's window width
    INT            m_iHeight;  // ArcBall's window height
    FLOAT          m_fRadius;  // ArcBall's radius in screen coords
    FLOAT          m_fRadiusTranslation; // ArcBall's radius for translating the target
	
    D3DXQUATERNION m_qDown;               // Quaternion before button down
    D3DXQUATERNION m_qNow;                // Composite quaternion for current drag
    D3DXMATRIX     m_matRotation;         // Matrix for arcball's orientation
    D3DXMATRIX     m_matRotationDelta;    // Matrix for arcball's orientation
    D3DXMATRIX     m_matTranslation;      // Matrix for arcball's position
    D3DXMATRIX     m_matTranslationDelta; // Matrix for arcball's position
    BOOL           m_bDrag;               // Whether user is dragging arcball
    BOOL           m_bRightHanded;        // Whether to use RH coordinate system
	
    int         iCurMouseX;      // Saved mouse position
    int         iCurMouseY;
    D3DXVECTOR3 s_vDown;         // Button down vector
	
    D3DXVECTOR3 ScreenToVector( int sx, int sy );
	
public:
    LRESULT     HandleMouseMessages( HWND, UINT, WPARAM, LPARAM );
	
    D3DXMATRIX* GetRotationMatrix()         { return &m_matRotation; }
    D3DXMATRIX* GetRotationDeltaMatrix()    { return &m_matRotationDelta; }
    D3DXMATRIX* GetTranslationMatrix()      { return &m_matTranslation; }
    D3DXMATRIX* GetTranslationDeltaMatrix() { return &m_matTranslationDelta; }
    BOOL        IsBeingDragged()            { return m_bDrag; }
	
    VOID        SetRadius( FLOAT fRadius );
    VOID        SetWindow( INT w, INT h, FLOAT r=0.9 );
    VOID        SetRightHanded( BOOL bRightHanded ) { m_bRightHanded = bRightHanded; }
	void		Zoom( int nFactor );
	void		MouseMovePanZoom(UINT nFlags, CPoint point);
	void		MouseMoveRot(UINT nFlags, CPoint point);
	
	void OnRButtonDown(UINT nFlags, CPoint point) ;
	void OnMButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;
	
	
    CArcBall();
	void Init( void );
};

//VOID D3DUtil_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r, FLOAT g, FLOAT b, FLOAT a );
						  
#endif
