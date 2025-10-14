// Ctrl.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct REGIONBOXVERTEX
{ 
	FLOAT x, y, z; 
	FLOAT nx, ny, nz;
	DWORD   color;
};
#define D3DFVF_REGIONBOXVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE) 

class CModelRegion : public CModel
{
public:
	LPDIRECT3DVERTEXBUFFER9 m_pRegionBoxVertexBuffer;
	int                     m_nRegionBoxVertexNum   ;
	D3DXVECTOR3             m_vPosBegin;
	D3DXVECTOR3             m_vPosEnd;
	/*
	FLOAT                   m_fLeft                 ;
	FLOAT                   m_fRight                ; 
	FLOAT                   m_fBottom                  ;
	FLOAT                   m_fTop               ;
	FLOAT                   m_fBase                 ;
	FLOAT                   m_fHeight               ;
*/
	CModelRegion();
	~CModelRegion();

	virtual BOOL Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld = NULL );
	virtual void FrameMove();
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects() ;
	virtual BOOL Intersect( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist );
};

CModelRegion::CModelRegion()
{
	m_nModelType = 10;//MODELTYPE_REGION; 
	m_pRegionBoxVertexBuffer = NULL;
	m_nRegionBoxVertexNum    = 0;
	m_vPosBegin = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vPosEnd   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	/*
	m_fLeft                  =  0.0f;
	m_fRight                 = 100.0f;
	m_fBottom                   =  0.0f;
	m_fTop                = 100.0f;
	m_fBase                  =  0.0f;
	m_fHeight                =  50.0f;
	*/

}
CModelRegion::~CModelRegion()
{
}
BOOL CModelRegion::Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld )
{
	m_pd3dDevice->SetFVF( D3DFVF_REGIONBOXVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pRegionBoxVertexBuffer, 0,sizeof( REGIONBOXVERTEX ) );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nRegionBoxVertexNum / 3 );
	return S_OK; 
}
void CModelRegion::FrameMove()
{
}
HRESULT CModelRegion::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) 
{ 
	m_pd3dDevice = pd3dDevice; 
	return S_OK; 
}
HRESULT CModelRegion::RestoreDeviceObjects() 
{
	// 바운드 박스 버텍스 버퍼 만들기 

	m_nRegionBoxVertexNum = 6 * 6; // 한면당 버텍스 수 * 면수 
	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( 
		m_nRegionBoxVertexNum * sizeof( REGIONBOXVERTEX ),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_REGIONBOXVERTEX,
		D3DPOOL_SYSTEMMEM, &m_pRegionBoxVertexBuffer, NULL );

	REGIONBOXVERTEX* pVertices;

	hr = m_pRegionBoxVertexBuffer->Lock( 0, m_nRegionBoxVertexNum * sizeof(REGIONBOXVERTEX), (void**)&pVertices, 0 );
	if( FAILED(hr) ) return hr;

	DWORD dwRed   = 0xffff0000;
	DWORD dwGreen = 0xff00ff00;
	DWORD dwBlue  = 0xff0000ff;

	D3DXVECTOR3 vPosB = m_vPosBegin - m_vPosBegin;
	D3DXVECTOR3 vPosE = m_vPosEnd - m_vPosBegin;

	REGIONBOXVERTEX regionBoxVertex[ 36 ]  =
	{
		vPosB.x,  vPosB.y, vPosB.z, 0.0f, -1.0f, 0.0f, 0,
		vPosB.x,  vPosB.y, vPosE.z, 0.0f, -1.0f, 0.0f, 0,
		vPosE.x,  vPosB.y, vPosB.z, 0.0f, -1.0f, 0.0f, 0,
		vPosE.x,  vPosB.y, vPosB.z, 0.0f, -1.0f, 0.0f, 0,
		vPosB.x,  vPosB.y, vPosE.z, 0.0f, -1.0f, 0.0f, 0,
		vPosE.x,  vPosB.y, vPosE.z, 0.0f, -1.0f, 0.0f, 0,

		vPosB.x,  vPosE.y, vPosB.z, 0.0f, 1.0f, 0.0f, dwBlue,
		vPosB.x,  vPosE.y, vPosE.z, 0.0f, 1.0f, 0.0f, dwBlue,
		vPosE.x,  vPosE.y, vPosB.z, 0.0f, 1.0f, 0.0f, dwBlue,
		vPosE.x,  vPosE.y, vPosB.z, 0.0f, 1.0f, 0.0f, dwBlue,
		vPosB.x,  vPosE.y, vPosE.z, 0.0f, 1.0f, 0.0f, dwBlue,
		vPosE.x,  vPosE.y, vPosE.z, 0.0f, 1.0f, 0.0f, dwBlue,

		vPosB.x,  vPosB.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,
		vPosB.x,  vPosE.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,
		vPosE.x,  vPosE.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,
		vPosB.x,  vPosB.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,
		vPosE.x,  vPosE.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,
		vPosE.x,  vPosB.y, vPosB.z, 0.0f, 0.0f,-1.0f, dwRed,

		vPosB.x,  vPosB.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,
		vPosB.x,  vPosE.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,
		vPosE.x,  vPosE.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,
		vPosB.x,  vPosB.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,
		vPosE.x,  vPosE.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,
		vPosE.x,  vPosB.y, vPosE.z, 0.0f, 0.0f, 1.0f, dwRed,

		vPosB.x,  vPosB.y, vPosB.z, -1.0f, 0.0f, 0.0f, dwGreen,
		vPosB.x,  vPosE.y, vPosB.z, -1.0f, 0.0f, 0.0f, dwGreen,
		vPosB.x,  vPosE.y, vPosE.z, -1.0f, 0.0f, 0.0f, dwGreen,
		vPosB.x,  vPosB.y, vPosB.z, -1.0f, 0.0f, 0.0f, dwGreen,
		vPosB.x,  vPosB.y, vPosE.z, -1.0f, 0.0f, 0.0f, dwGreen,
		vPosB.x,  vPosE.y, vPosE.z, -1.0f, 0.0f, 0.0f, dwGreen,

		vPosE.x,  vPosB.y, vPosB.z, 1.0f, 0.0f, 0.0f, dwGreen,
		vPosE.x,  vPosE.y, vPosB.z, 1.0f, 0.0f, 0.0f, dwGreen,
		vPosE.x,  vPosE.y, vPosE.z, 1.0f, 0.0f, 0.0f, dwGreen,
		vPosE.x,  vPosB.y, vPosB.z, 1.0f, 0.0f, 0.0f, dwGreen,
		vPosE.x,  vPosB.y, vPosE.z, 1.0f, 0.0f, 0.0f, dwGreen,
		vPosE.x,  vPosE.y, vPosE.z, 1.0f, 0.0f, 0.0f, dwGreen,

	};
	for( int i = 0; i < 36; i++ )
	{
		pVertices->x = regionBoxVertex[ i ].x;
		pVertices->y = regionBoxVertex[ i ].y;
		pVertices->z = regionBoxVertex[ i ].z;
		pVertices->nx = regionBoxVertex[ i ].nx;
		pVertices->ny = regionBoxVertex[ i ].ny;
		pVertices->nz = regionBoxVertex[ i ].nz;
	
		pVertices->color = regionBoxVertex[ i ].color;
		pVertices++;
	}
//     - z
//   3 | 2
// - --+-- + x
//   0 | 1
//
//   7 | 6
// - --+-- + x
//   4 | 5
	m_BB.m_vPos[ 0 ] = D3DXVECTOR3( vPosB.x, vPosE.y, vPosB.z );
	m_BB.m_vPos[ 1 ] = D3DXVECTOR3( vPosE.x, vPosE.y, vPosB.z );
	m_BB.m_vPos[ 2 ] = D3DXVECTOR3( vPosE.x, vPosE.y, vPosE.z );
	m_BB.m_vPos[ 3 ] = D3DXVECTOR3( vPosB.x, vPosE.y, vPosE.z );
	m_BB.m_vPos[ 4 ] = D3DXVECTOR3( vPosB.x, vPosB.y, vPosB.z );
	m_BB.m_vPos[ 5 ] = D3DXVECTOR3( vPosE.x, vPosB.y, vPosB.z );
	m_BB.m_vPos[ 6 ] = D3DXVECTOR3( vPosE.x, vPosB.y, vPosE.z );
	m_BB.m_vPos[ 7 ] = D3DXVECTOR3( vPosB.x, vPosB.y, vPosE.z );
	m_pRegionBoxVertexBuffer->Unlock();
	
	return S_OK; 
}
HRESULT CModelRegion::InvalidateDeviceObjects() 
{
	SAFE_RELEASE( m_pRegionBoxVertexBuffer );
	return S_OK; 
}
HRESULT CModelRegion::DeleteDeviceObjects() 
{
	SAFE_RELEASE( m_pRegionBoxVertexBuffer );
	return S_OK; 
}
BOOL CModelRegion::Intersect( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist ) 
{
	return S_OK; 
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegion::CRegion()
{
	m_dwType = 0; // event, attribute
	m_dwType = OT_REGION;
	m_pModel = NULL;
	m_bEditable = FALSE;
	/*
	m_fLeft                  =  0.0f;
	m_fRight                 = 100.0f;
	m_fBottom                   =  0.0f;
	m_fTop                = 100.0f;
	m_fBase                  =  0.0f;
	m_fHeight                =  50.0f;
*/


	SetPos( D3DXVECTOR3( 0.0f, 0.0f,0.0f ) );
//	m_vPosEnd = D3DXVECTOR3( 10.0f, 10.0f,10.0f );
	ZeroMemory( &m_region, sizeof( m_region ) );
	m_region.m_rect.SetRect( -MPU * 2, -MPU * 2, MPU * 2, MPU * 2 );
	
#ifdef __N_TRIGGER_AREASET_050203
	m_region.m_uItemId		= 0xffffffff;
	m_region.m_uiItemCount	= 0xffffffff;
	m_region.m_uiMinLevel	= 0xffffffff;
	m_region.m_uiMaxLevel	= 0xffffffff;
	m_region.m_iQuest		= 0xffffffff;
	m_region.m_iQuestFlag	= 0xffffffff;
	m_region.m_iJob			= 0xffffffff;
	m_region.m_iGender		= 0xffffffff;
	m_region.m_bCheckParty	= FALSE;
	m_region.m_bCheckGuild	= FALSE;
	m_region.m_bChaoKey		= FALSE;
#endif // __N_TRIGGER_AREASET_050203
}

CRegion::~CRegion()
{
	//for( int i = 0; i < m_region.m_cDescSize; i++ )
	//	SAFE_DELETE( m_region.m_apszDesc[ i ] );
}
/*
BOOL CRegion::PickBoundBoxVertex( LPDIRECT3DDEVICE9 pd3dDevice, CPoint point )
{
#ifndef __WORLDSERVER
	if( m_bEditable )
	{
		for( int i = 0; i < 8; i++ )
		{
			CPoint pt = GetClientPoint( pd3dDevice, i );
			CRect rect( pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3 );
			if( rect.PtInRect( point ) )
				return TRUE;
		}
	}
#endif
	return FALSE;
}
HRESULT CRegion::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_pModel == NULL )
	{
		m_pModel = new CModelRegion;
		((CModelRegion*)m_pModel)->m_vPosBegin = GetPos();
		((CModelRegion*)m_pModel)->m_vPosEnd = m_vPosEnd;
	}
	m_pModel->InitDeviceObjects( pd3dDevice );
	return S_OK;
}
HRESULT CRegion::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pModel->RestoreDeviceObjects();
	return S_OK;
}
HRESULT CRegion::DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{ 
	return S_OK;
}
HRESULT CRegion::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
*/
void CRegion::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __BEAST
	CObj::Render( pd3dDevice );
	/*
	UpdateLocalMatrix();
//	if( IsUpdateMatrix() )
		UpdateMatrix();
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	//pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 0 )->m_pTexture );
 	m_pModel->Render( pd3dDevice );
*/
	{
		D3DXVECTOR3 vPos = GetPos();
		CRect rect = m_region.m_rect;
		CPoint point = CPoint( (int)( vPos.x ), (int)( vPos.z ) );
		point.x = point.x / MPU * MPU;
		point.y = point.y / MPU * MPU;
		rect += point;

		// RenderGrids는 원점의 트랜스폼에서 시작하므로 트랜스폼을 리셋해야한다.
		D3DXMATRIX mWorld;
		D3DXMatrixIdentity( &mWorld ); 
		pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );

		if( GetIndex() == RI_TRIGGER )
			GetWorld()->RenderGrids( rect, 0, RECTCOLOR_TRIGGER );
		else
		if( GetIndex() == RI_ATTRIBUTE )
			GetWorld()->RenderGrids( rect, 0, RECTCOLOR_ATTRIBUTE );
		else
		if( GetIndex() == RI_REVIVAL )
		{
			if( m_region.m_bTargetKey )
				GetWorld()->RenderGrids( rect, 0, RECTCOLOR_REVIVAL );
		}

	//	RenderName( pd3dDevice );
	}

	/*
	if( m_bEditable )
	{
		for( int i = 0; i < 8; i++ )
		{
			CPoint pt = GetClientPoint( pd3dDevice, i );
			//g_Neuz.m_2DRender.RenderFillRect( CRect( pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3 ), 0xffffffff );
		}
	}
	*/
#endif
}
void CRegion::Process(FLOAT fElapsedTime)
{
}

BOOL CRegion::Read( CFileIO* pFile )
{
	return TRUE;//CObj::Read( pFile );
}
BOOL CRegion::Write( CFileIO* pFile )
{
	return TRUE;//CObj::Read( pFile );
}
