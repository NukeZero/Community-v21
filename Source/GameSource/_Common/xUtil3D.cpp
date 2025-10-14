#include "stdafx.h"
#include "xUtil3D.h"
#include "..\_DirectX\d3dutil.h"


// QuaternionRotationToYPW -> 쿼터니온에서 Yaw, Pitch, Roll을 갖고 온다
void QuaternionRotationToYPW( const D3DXQUATERNION& qRot, D3DXVECTOR3& vYPW )
{
	FLOAT pitch, yaw, roll;
	FLOAT sqw = qRot.w*qRot.w;
	FLOAT sqx = qRot.x*qRot.x;
	FLOAT sqy = qRot.y*qRot.y;
	FLOAT sqz = qRot.z*qRot.z;
	
	pitch = asinf(2.0f * (qRot.w*qRot.x - qRot.y*qRot.z)); 
	yaw   = atan2f(2.0f * (qRot.x*qRot.z + qRot.w*qRot.y),(-sqx - sqy + sqz + sqw)); 
	roll  = atan2f(2.0f * (qRot.x*qRot.y + qRot.w*qRot.z),(-sqx + sqy - sqz + sqw));
	
	vYPW.x = pitch;
	vYPW.y = yaw;
	vYPW.z = roll;
}


// boundbox vMin, vMax값을 8개의 벡터 좌표로 풀어냄
void	SetBB( D3DXVECTOR3 *pBBVList, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax )
{
	pBBVList[0].x = vMin.x;
	pBBVList[0].y = vMax.y;
	pBBVList[0].z = vMin.z;

	pBBVList[1].x = vMax.x;
	pBBVList[1].y = vMax.y;
	pBBVList[1].z = vMin.z;

	pBBVList[2].x = vMax.x;
	pBBVList[2].y = vMax.y;
	pBBVList[2].z = vMax.z;

	pBBVList[3].x = vMin.x;
	pBBVList[3].y = vMax.y;
	pBBVList[3].z = vMax.z;

	pBBVList[4].x = vMin.x;
	pBBVList[4].y = vMin.y;
	pBBVList[4].z = vMin.z;

	pBBVList[5].x = vMax.x;
	pBBVList[5].y = vMin.y;
	pBBVList[5].z = vMin.z;

	pBBVList[6].x = vMax.x;
	pBBVList[6].y = vMin.y;
	pBBVList[6].z = vMax.z;

	pBBVList[7].x = vMin.x;
	pBBVList[7].y = vMin.y;
	pBBVList[7].z = vMax.z;
}


//
//
// Axis Aligned Bounding Boxes(AABB) 방식 충돌 검출
int		IsTouchAABB( const D3DXVECTOR3 &vMin1, const D3DXVECTOR3 &vMax1,  
					 const D3DXVECTOR3 &vMin2, const D3DXVECTOR3 &vMax2 )
{
	if( vMin2.x > vMax1.x )		return 0;		// v2가 x축 오른쪽으로 벗어났다.
	if( vMax2.x < vMin1.x )		return 0;		// v2가 x축 왼쪽으로 벗어났다.
	if( vMin2.z > vMax1.z )		return 0;		// v2가 z축 앞으로 벗어났다.
	if( vMax2.z < vMin1.z )		return 0;		// v2가 z축 뒤로으로 벗어났다.
	if( vMin2.y > vMax1.y )		return 0;		// v2가 y축 위로 벗어났다.
	if( vMax2.y < vMin1.y )		return 0;		// v2가 y축 아래로 벗어났다.

	return 1;
}



#define CLIP_RIGHT	0x00000001
#define CLIP_LEFT	0x00000002
#define CLIP_TOP	0x00000004
#define CLIP_BOTTOM	0x00000008
#define CLIP_FRONT	0x00000010
#define CLIP_BACK	0x00000020

// calculates the cohen-sutherland outcode for a point and a bounding box.
//
// vMin:	min vector of the bounding box
// vMax:	max vector of the bounding box
// vPnt:			the point to check
//
// returns:		the outcode
//
static int CalcOutCode( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &v )
{
	int nOutCode = 0;

	if( v.x > vMax.x ) {
		nOutCode |= CLIP_RIGHT;
	} else if( v.x < vMin.x ) {
		nOutCode |= CLIP_LEFT;
	}
	if( v.y > vMax.y ) {
		nOutCode |= CLIP_TOP;
	} else if( v.y < vMin.y ) {
		nOutCode |= CLIP_BOTTOM;
	}
	if( v.z > vMax.z ) {
		nOutCode |= CLIP_BACK;
	} else if( v.z < vMin.z ) {
		nOutCode |= CLIP_FRONT;
	}

	return nOutCode;
}

//
// 코헨 서더랜드 라인클리핑 알고리즘
//
// vMin:	AABB min
// vMax:	AABB max
// v1:		라인 시작점
// v2:		라인 끝점
// pvIntercept(OUT):	실제 교차점
//
BOOL	IsTouchAABB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, 
						  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, 
						  D3DXVECTOR3 *pvIntersect )
{
	int nOutCode1, nOutCode2;

	nOutCode1 = CalcOutCode( vMin, vMax, v1 );
	if( nOutCode1 == 0 ) {
		// point inside bounding box
		*pvIntersect = v1;
		return TRUE;
	}

	nOutCode2 = CalcOutCode( vMin, vMax, v2 );
	if( nOutCode2 == 0 ) {
		// point inside bounding box
		*pvIntersect = v2;
		return TRUE;
	}

	if( (nOutCode1 & nOutCode2) > 0 ) {
		// both points on same side of box
		return FALSE;
	}

	// check intersections
	if( nOutCode1 & (CLIP_RIGHT | CLIP_LEFT) ) {
		if( nOutCode1 & CLIP_RIGHT ) {
			pvIntersect->x = vMax.x;
		} else {
			pvIntersect->x = vMin.x;
		}
		float x1 = v2.x - v1.x;
		float x2 = pvIntersect->x - v1.x;
		pvIntersect->y = v1.y + x2 * (v2.y - v1.y) / x1;
		pvIntersect->z = v1.z + x2 * (v2.z - v1.z) / x1;

		if( pvIntersect->y <= vMax.y && pvIntersect->y >= vMin.y && pvIntersect->z <= vMax.z && pvIntersect->z >= vMin.z ) {
			return TRUE;
		}
	}

	if( nOutCode1 & (CLIP_TOP | CLIP_BOTTOM) ) {
		if( nOutCode1 & CLIP_TOP ) {
			pvIntersect->y = vMax.y;
		} else {
			pvIntersect->y = vMin.y;
		}
		float y1 = v2.y - v1.y;
		float y2 = pvIntersect->y - v1.y;
		pvIntersect->x = v1.x + y2 * (v2.x - v1.x) / y1;
		pvIntersect->z = v1.z + y2 * (v2.z - v1.z) / y1;

		if( pvIntersect->x <= vMax.x && pvIntersect->x >= vMin.x && pvIntersect->z <= vMax.z && pvIntersect->z >= vMin.z ) {
			return TRUE;
		}
	}

	if( nOutCode1 & (CLIP_FRONT | CLIP_BACK) ) {
		if( nOutCode1 & CLIP_BACK ) {
			pvIntersect->z = vMax.z;
		} else {
			pvIntersect->z = vMin.z;
		}
		float z1 = v2.z - v1.z;
		float z2 = pvIntersect->z - v1.z;
		pvIntersect->x = v1.x + z2 * (v2.x - v1.x) / z1;
		pvIntersect->y = v1.y + z2 * (v2.y - v1.y) / z1;

		if( pvIntersect->x <= vMax.x && pvIntersect->x >= vMin.x && pvIntersect->y <= vMax.y && pvIntersect->y >= vMin.y ) {
			return TRUE;
		}
	}

	// nothing found
	return FALSE;
}

//
//	OBB - Line 충돌 검사
//	vMin, vMax : Local좌표
//	m : 월드매트릭스
//	v1, v2 : 월드 좌표
//
BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXMATRIX &m, 
						 const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *pvIntersect )
{

	D3DXMATRIX	mInv;
	D3DXMatrixInverse( &mInv, NULL, &m );	// AABB의 회전 매트릭스인 m을 역회전 시키기 위해 역행렬을 만든다.

	D3DXVECTOR3	rv1, rv2;
	D3DXVec3TransformCoord( &rv1, &v1, &mInv );		// 선의 양 끝점을 역회전 시킨다.
	D3DXVec3TransformCoord( &rv2, &v2, &mInv );

	BOOL bResult = IsTouchAABB_Line( vMin, vMax, rv1, rv2, pvIntersect );	// AABB - Line 교차를 테스트
	if( bResult) 
		D3DXVec3TransformCoord( pvIntersect, pvIntersect, &m );

	return bResult;
}


BOOL	IsTouchRayTri( const D3DXVECTOR3 *v0, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2,
					   const D3DXVECTOR3 *vOrig, const D3DXVECTOR3 *vDir, 
					   FLOAT* pfDist )
{
    // Find vectors for two edges sharing vert0
	FLOAT t,u,v;
    D3DXVECTOR3 edge1 = *v1 - *v0;
    D3DXVECTOR3 edge2 = *v2 - *v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, vDir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
        tvec = *vOrig - *v0;
    else
		return FALSE;	// 면이 반대다.

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( vDir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
	t = D3DXVec3Dot( &edge2, &qvec );
	FLOAT fInvDet = 1.0f / det;
	t *= fInvDet;
	*pfDist = t;
//	u *= fInvDet;
//	v *= fInvDet;

//	*pvIntersect = vOrig + t * vDir;
    return TRUE;
}

//
//	vN을 법선벡터로 갖는 면에 vDir벡터가 부딪혔을 때 Slide벡터를 계산해서 pOut으로 돌려준다. 
//	vN은 단위벡터.
//

void	CalcSlideVec( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vN )
{
	D3DXVECTOR3 vInvN;
	vInvN.x = -vN.x;	vInvN.y = -vN.y;	vInvN.z = -vN.z;	// 노말을 뒤집음.
	FLOAT	b = D3DXVec3Dot( &vDir, &vInvN );		// (V dot -N)	직각삼각형의 밑변의 길이.
	*pOut = vDir - (vInvN * b);		// slide vector
}

//////////////////////////////////////////////////////////////////////

Segment3::Segment3( const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 )
{
	Origin = (v1 + v2) * 0.5f;
	Direction = v2-v1;
	Extent = D3DXVec3Length( &Direction ) * 0.5f;
	D3DXVec3Normalize( &Direction, &Direction );
} 

// vMin, vMax - AABB 로컬좌표 
void BBOX::UpdateMartix( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, 
						 const D3DXMATRIX& matScale, const D3DXMATRIX& matRotation, const D3DXMATRIX& matWorld )
{
	//center
	Center = ( vMin + vMax ) * 0.5f;
	D3DXVec3TransformCoord( &Center, &Center, &matWorld );

	//extent
	D3DXVECTOR3	vExtent = ( vMax - vMin ) * 0.5f;

	Extent[0] = vExtent.x * matScale._11;
	Extent[1] = vExtent.y * matScale._22;
	Extent[2] = vExtent.z * matScale._33;
	
	//axis
	Axis[0].x = matRotation._11;
	Axis[0].y = matRotation._12;
	Axis[0].z = matRotation._13;

	Axis[1].x = matRotation._21;
	Axis[1].y = matRotation._22;
	Axis[1].z = matRotation._23;

	Axis[2].x = matRotation._31;
	Axis[2].y = matRotation._32;
	Axis[2].z = matRotation._33;
}

bool IntrSegment3Box3_Test( const Segment3& rkSegment, const BBOX& rkBox )
{
    float afAWdU[3], afADdU[3], afAWxDdU[3], fRhs;

    D3DXVECTOR3 kDiff = rkSegment.Origin - rkBox.Center;

	for( int i=0; i<3; ++i )
	{
		afAWdU[i] = fabs( D3DXVec3Dot(&rkSegment.Direction, &rkBox.Axis[i]) );
		afADdU[i] = fabs( D3DXVec3Dot(&kDiff, &rkBox.Axis[i]) );

		fRhs = fabs(rkBox.Extent[i]) + rkSegment.Extent*afAWdU[i];
		
		if (afADdU[i] > fRhs)
		{
			return false;
		}
	}

	D3DXVECTOR3 kWxD;
    D3DXVec3Cross( &kWxD, &rkSegment.Direction, &kDiff );
	
    afAWxDdU[0] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[0]) );
  
	fRhs = fabs(rkBox.Extent[1])*afAWdU[2] + fabs(rkBox.Extent[2])*afAWdU[1];

    if (afAWxDdU[0] > fRhs)
    {
        return false;
    }

    afAWxDdU[1] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[1]) );
   
	fRhs = fabs(rkBox.Extent[0])*afAWdU[2] + fabs(rkBox.Extent[2])*afAWdU[0];

    if (afAWxDdU[1] > fRhs)
    {
        return false;
    }

    afAWxDdU[2] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[2]) );
   
	fRhs = fabs(rkBox.Extent[0])*afAWdU[1] + fabs(rkBox.Extent[1])*afAWdU[0];

    if (afAWxDdU[2] > fRhs)
    {
        return false;
    }

    return true;
}





