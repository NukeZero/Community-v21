// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "Model.h"
#include "..\_DirectX\dxutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CModel::~CModel()
{
	SAFE_DELETE( m_pBillboard );
	DeleteDeviceObjects();
}

// fSpeed : 애니메이션 속도를 조절하고 싶을때. 1.0이 기본값 2.0은 두배 빨라짐.
void CModel::FrameMove( D3DXVECTOR3 *pvSndPos, float fSpeed )
{
	if( m_nPause )		
		return;
	if( m_nFrameMax == 0 )	return;		// 프레임이 없는건 돌필요 없다.

//	m_bEndFrame = FALSE;
	int		nCurrFrame;
	// 보간을 위한 Slerp 계산.
//	dPerSlerp = g_dUps / m_dKeySpeed;
//	dPerSlerp = 0.016666 / m_dKeySpeed;		// 60프레임에 맞춰졌으므로 ups는 1/60이 고정이다.
//	m_fPerSlerp = 0.5;						// 프레임수의 정확도땜에 0.5로 했다
//#ifdef __COLA
//	if( g_nPlay )		// 플레이 켰을때만 애니되게.
//#endif
	m_fFrameOld = m_fFrameCurrent;			// 바로 이전 프레임 번호를 받아둠.
#ifdef __X0201
	if( m_fFrameOld > (float)(m_nFrameMax - 1) )
		m_fFrameOld -= (float)m_nFrameMax;
#endif
	if( m_bSlow )
		m_fFrameCurrent += 0.1f;			// 애니메이션 진행
	else
		m_fFrameCurrent += (m_fPerSlerp * fSpeed);			// 애니메이션 진행


	nCurrFrame = (int)m_fFrameCurrent;				// 소숫점 떼내고 정수부만..
	m_fSlp = m_fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴

	if( m_nLoop & ANILOOP_LOOP )		// 루핑모드일때
	{
//		if( nCurrFrame >= m_nFrameMax )		// 마지막 프레임을 넘어가면
		if( m_fFrameCurrent > (float)(m_nFrameMax - 1) )
		{
			m_bEndFrame = TRUE;
			nCurrFrame %= m_nFrameMax;		// 처음으로 되돌림
		}
	} 
	else		// 루핑모드가 아니라면 마지막 프레임에서 멈춘다.
	{
//		if( nCurrFrame >= m_nFrameMax )
		if( m_fFrameCurrent > (float)(m_nFrameMax - 1) )
		{
			m_bEndFrame = TRUE;
			m_fSlp = 0.0f;
			nCurrFrame = m_nFrameMax - 1;
		}
	}
	m_fFrameCurrent = (float)nCurrFrame + m_fSlp;	// 편의상 시각적으로 보기 쉽도록 수치 보정
}

void CModel::AddFrame( FLOAT fAddFrame )
{
}
void CModel::SetFrame( FLOAT fFrame )
{
}
BOOL CModel::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	return TRUE;
}
BOOL CModel::IsAniable()
{
	if( m_nModelType == MODELTYPE_MESH || m_nModelType == MODELTYPE_BILLBOARD ) 
		return FALSE;
	return TRUE;
}

//
//	현재 프레임의 다음 프레임번호를 리턴
//
int	CModel::GetNextFrame( void )
{
	int		nCurrFrm = (int)m_fFrameCurrent;
	int		nNextFrm;

	nNextFrm = nCurrFrm + 1;
	if( nNextFrm >= m_nFrameMax )		
	{
		if( m_nLoop & ANILOOP_LOOP )	
			nNextFrm = 0;			// 루핑애니메이션이면 처음프레임과 연결될수 있도록 한다.
		else													
			nNextFrm = m_nFrameMax - 1;		// 아니면 마지막 프레임에서 멈춘다
	}

	return nNextFrm;
}

//
//	바운드박스 vMin,vMax를 기준으로 반지름을 돌려줌
//
float CModel::GetRadius( void )
{
	D3DXVECTOR3 vLen = m_vMax - m_vMin;
	float	fLen = D3DXVec3Length( &vLen );
	return fLen / 2;
}

BOOL CModel::IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist )
{
	// Collect all intersections
	D3DXVECTOR3 v1, v2, v3;
//	CModel* pModel;
	D3DXVECTOR4 vOut;
	D3DXVECTOR3 vPos;

	//  3|2  
	//  -+-
	//	0|1
	//  
	//  7|6
	//  -+-
	//	4|5

	static int anIntexTable[ 12 * 3 ] =
	{
		0, 1, 2,  0, 2, 3,
		0, 4, 5,  0, 5, 1,
		4, 5, 6,  4, 6, 7,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 2, 6,  1, 6, 5
	};
	const BOUND_BOX* pBB = GetBBVector();

	for( int i = 0; i < 12 * 3; i += 3 )
	{
		v1 = pBB->m_vPos[ anIntexTable[ i + 0 ] ];
		v2 = pBB->m_vPos[ anIntexTable[ i + 1 ] ];
		v3 = pBB->m_vPos[ anIntexTable[ i + 2 ] ];					

//		D3DXVec3Transform( &vOut, &v1, &mWorld ); v1.x = vOut.x; v1.y = vOut.y; v1.z = vOut.z;
//		D3DXVec3Transform( &vOut, &v2, &mWorld ); v2.x = vOut.x; v2.y = vOut.y; v2.z = vOut.z;
//		D3DXVec3Transform( &vOut, &v3, &mWorld ); v3.x = vOut.x; v3.y = vOut.y; v3.z = vOut.z;
		D3DXVec3TransformCoord( &v1, &v1, &mWorld ); 
		D3DXVec3TransformCoord( &v2, &v2, &mWorld ); 
		D3DXVec3TransformCoord( &v3, &v3, &mWorld ); 
		

		if( IntersectTriangle( v1, v2, v3, vRayOrig, vRayDir, pvIntersect, pfDist ) ) 
			return TRUE;


	}
	return FALSE;
}
/*
BOOL CModel::GetHeight( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist )
{
	// Collect all intersections
	D3DXVECTOR3 v1, v2, v3;
	D3DXVECTOR4 vOut;
	D3DXVECTOR3 vPos;

	//  3|2  
	//  -+-
	//	0|1
	//  
	//  7|6
	//  -+-
	//	4|5

	static int anIntexTable[ 12 * 3 ] =
	{
		0, 1, 2, 3,  0, 2, 3,
		0, 4, 5,  0, 5, 1,
		4, 5, 6,  4, 6, 7,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 2, 6,  1, 6, 5
	};
	const BOUND_BOX* pBB = GetBBVector();

	for( int i = 0; i < 12 * 3; i += 3 )
	{
		v1 = pBB->m_vPos[ anIntexTable[ i + 0 ] ];
		v2 = pBB->m_vPos[ anIntexTable[ i + 1 ] ];
		v3 = pBB->m_vPos[ anIntexTable[ i + 2 ] ];					

		D3DXVec3Transform( &vOut, &v1, &mWorld ); v1.x = vOut.x; v1.y = vOut.y; v1.z = vOut.z;
		D3DXVec3Transform( &vOut, &v2, &mWorld ); v2.x = vOut.x; v2.y = vOut.y; v2.z = vOut.z;
		D3DXVec3Transform( &vOut, &v3, &mWorld ); v3.x = vOut.x; v3.y = vOut.y; v3.z = vOut.z;

		if( IntersectTriangle( v1, v2, v3, vRayOrig, vRayDir, pvIntersect, pfDist ) ) 
			return TRUE;
	}
	return FALSE;














	int px,pz;
	FLOAT dx,dz;
	FLOAT dy1,dy2,dy3,dy4;
	px=(int)x;
	pz=(int)z;
	dx=x-px;
	dz=z-pz;

	FLOAT y1=m_pHeightMap[px+pz*(MAP_SIZE+1)];
	FLOAT y2=m_pHeightMap[px+1+pz*(MAP_SIZE+1)];
	FLOAT y3=m_pHeightMap[px+(pz+1)*(MAP_SIZE+1)];
	FLOAT y4=m_pHeightMap[px+1+(pz+1)*(MAP_SIZE+1)];
	dy1=y1*(1-dx)*(1-dz);
	dy2=y2*dx*(1-dz);
	dy3=y3*(1-dx)*dz;
	dy4=y4*dx*dz;
	return dy1+dy2+dy3+dy4;
}
*/