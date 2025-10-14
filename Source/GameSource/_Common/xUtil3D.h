#ifndef	__XUTIL3D_H__
#define	__XUTIL3D_H__

void QuaternionRotationToYPW( const D3DXQUATERNION& qRot, D3DXVECTOR3& vYPW );

void	SetBB( D3DXVECTOR3 *pBBVList, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );

// AABB간의 충돌검출
int		IsTouchAABB( const D3DXVECTOR3 &vMin1, const D3DXVECTOR3 &vMax1,  
					 const D3DXVECTOR3 &vMin2, const D3DXVECTOR3 &vMax2 );

BOOL	IsTouchAABB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, 
						  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, 
						  D3DXVECTOR3 *pvIntersect );

BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXMATRIX &m, 
						 const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *pvIntersect );

BOOL	IsTouchRayTri( const D3DXVECTOR3 *v0, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2,
					   const D3DXVECTOR3 *vOrig, const D3DXVECTOR3 *vDir, 
					   FLOAT* pfDist );

void	CalcSlideVec( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vN );

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


//선분 클래스 (양 끝점은 Origin-Extent*Direction, Origin+Extent*Direction ) 
class Segment3
{
public:
	Segment3( const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 );

    D3DXVECTOR3 Origin, Direction;
    float Extent;
};

// oriented bound box
class BBOX
{
public:
	D3DXVECTOR3		Center;
	float			Extent[3];
	D3DXVECTOR3		Axis[3];

	void UpdateMartix( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax,
		               const D3DXMATRIX& matScale, const D3DXMATRIX& matRotation, const D3DXMATRIX& matWorld );
};						  

bool IntrSegment3Box3_Test( const Segment3& rkSegment, const BBOX& rkBox );


#endif
