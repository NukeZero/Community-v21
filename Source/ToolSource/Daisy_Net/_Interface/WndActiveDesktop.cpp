// WndArcane.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "WndActiveDesktop.h"
#include "WndManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWndDesktopMatrix::CWndDesktopMatrix()
{
}

CWndDesktopMatrix::~CWndDesktopMatrix()
{

}
void CWndDesktopMatrix::OnDraw( C2DRender* p2DRender )
{
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_pTheme->m_d3dcBackground, 1.0f, 0 ) ;
	CRect rect = p2DRender->m_clipRect;
	for( int i = 0; i < 32; i++ )
	{
		CTexture* pTexture0 = m_texturePack.GetAt( 1 );
		CTexture* pTexture1 = m_texturePack.GetAt( 0 );
		CTexture* pTexture2 = m_texturePack.GetAt( 2 );

		if( m_type[ i ] )
		{
			pTexture0->Render( p2DRender, CPoint( i * 30, m_vertical[ i ] - 256 ) );
			pTexture1->Render( p2DRender, CPoint( i * 30, m_vertical[ i ] ) );
		}
		else
		{
			pTexture2->Render( p2DRender, CPoint( i * 30, m_vertical[ i ] ) );
		}
	}
	CSize size;//m_pTheme->m_pFontGameTitle->GetTextExtent( _T( "CLOCKWORKS XP" ) );
	//m_pTheme->m_pFontGameTitle->DrawText( rect.Width() / 2 - size.cx / 2, 0, 0xffffffff, "CLOCKWORKS XP" );

	//size = m_pTheme->m_pFontText->GetTextExtent( _T( "Active Desktop" ) );
	//m_pTheme->m_pFontText->DrawText( rect.Width() / 2 - size.cx / 2, 40, 0xffffffff, "Active Desktop v0.1" );


//	size = m_pTheme->m_pFontText->GetTextExtent( _T( "Copyright (C) 2002~2003 Allright Reserved AEONSOFT Inc." ) );
//	m_pTheme->m_pFontText->DrawText( rect.Width() / 2 - size.cx / 2, rect.bottom - 20, 0xffffffff, "Copyright (C) 2002~2003 Allright Reserved AEONSOFT Inc." );
}
BOOL CWndDesktopMatrix::Process()
{
	for( int i = 0; i < 32; i++ )
	{
		m_vertical[ i ] += m_speed[ i ];
		if( m_vertical[ i ] > 1000 ) 
		{
			m_vertical[ i ] = -( ( rand() % 1000 ) + 256 );
			m_type[ i ] = rand() % 3;
			if( m_type[ i ] == 0 )
				m_speed[ i ]  = 5 + ( rand() % 5 );
			else
				m_speed[ i ]= 10 + ( rand() % 20 );

		}
	}
	return TRUE;
}
void CWndDesktopMatrix::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	CSize size = m_pTheme->m_pFontText->GetTextExtent( _T( "버튼을 누르세요~" ) );
	size.cx += 20;
	size.cy += 10;
	CRect rect = GetClientRect();
	CPoint point( rect.Width() / 2 - size.cx / 2, 70 );
	//m_wndButton.Create( _T( "버튼을 누르세요~" ), 0, CRect( point.x, point.y, point.x + size.cx, point.y + size.cy ), this, 100 ); 
}
BOOL CWndDesktopMatrix::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect(0,0,280,190);
	SetTitle( _T( "Matrix" ) );
	return CWndBase::Create( WBS_NOFRAME | WBS_MANAGER, m_pWndRoot->GetWindowRect(), &g_WndMng, dwWndId);
}
BOOL CWndDesktopMatrix::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case 100:
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0055) ) );
//			g_WndMng.OpenMessageBox( _T( "Active Desktop 버튼 콘트롤 테스트" ) );
			break;
		case 101: 
			break;
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndDesktopMatrix::OnCommand(UINT nID,DWORD dwMessage)
{
	return CWndBase::OnCommand(nID,dwMessage);
}
void CWndDesktopMatrix::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType,cx,cy);
}
void CWndDesktopMatrix::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndBase::OnLButtonUp(nFlags,point);
}
void CWndDesktopMatrix::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndBase::OnLButtonDown(nFlags,point);
}
BOOL CWndDesktopMatrix::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
HRESULT CWndDesktopMatrix::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_texturePack.LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "matrix.txt" ) ) );

	for( int i = 0; i < 32; i++ )
	{
		m_vertical[ i ] = -( rand() % 1000 );
		m_type[ i ] = rand() % 10;
		if( m_type[ i ] == 0 )
			m_speed[ i ]  = 2 + ( rand() % 3 );
		else
			m_speed[ i ]= 10 + ( rand() % 10 );
	}
	return S_OK;
}
HRESULT CWndDesktopMatrix::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CWndDesktopMatrix::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CWndDesktopMatrix::DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice)
{
	return S_OK;
}
//////////////////////////////////////////////////////

CWndDesktopClockworks::CWndDesktopClockworks()
{
}

CWndDesktopClockworks::~CWndDesktopClockworks()
{

}
void CWndDesktopClockworks::OnDraw( C2DRender* p2DRender )
{
	CRect rect = p2DRender->m_clipRect;
//	texture.SetAutoFree( FALSE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	//////////////////////////////////////////////////////////////////////////

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// Frame Window와 관련된 Viewport 세팅 
	CRect rectRoot = m_pWndRoot->GetWindowRect();
	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = rectRoot.Width();
	viewport.Height = rectRoot.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	// Frame Window와 관려된 Projection
	D3DXMATRIX matProj;
	D3DXMATRIX matView;
	D3DXMATRIX matWorld;
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 vPos    = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vLookAt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXMatrixLookAtLH( &matView, &vPos, &vLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if( m_mesh.GetRenderEffect() == REFF_ALPHA )		// 알파로 출력해야 할것들인가?
	{
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	} 
	else
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	}
	CLight pLight;
	pLight.SetLight( "direction", D3DLIGHT_DIRECTIONAL, 0, 20, 0 );
	pLight.Attenuation0 =  0.0f;
	pLight.Range        = 10;//FLT_MAX;//127.0f;
	pLight.Diffuse.r    = 1.0f;
	pLight.Diffuse.g    = 1.0f;
	pLight.Diffuse.b    = 1.0f;
	pLight.Specular.r   = 1.0f;
	pLight.Specular.g   = 1.0f;
	pLight.Specular.b   = 1.0f;
	pLight.Ambient.r    = 0.0f;
	pLight.Ambient.g    = 0.0f;
	pLight.Ambient.b    = 0.0f;
	pLight.Direction    = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
	pLight.Appear( pd3dDevice );
	pd3dDevice->SetLight( pLight.m_dwIndex, &pLight );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );

	D3DMATERIAL9       m_baseMaterial;
	ZeroMemory( &m_baseMaterial, sizeof(m_baseMaterial) );
	m_baseMaterial.Diffuse.r = 1.0f;
	m_baseMaterial.Diffuse.g = 1.0f;
	m_baseMaterial.Diffuse.b = 1.0f;
	m_baseMaterial.Diffuse.a = 1.0f;
	m_baseMaterial.Ambient.r  = 1.0f;
	m_baseMaterial.Ambient.g  = 1.0f;
	m_baseMaterial.Ambient.b  = 1.0f;
	m_baseMaterial.Ambient.a  = 1.0f;
	m_baseMaterial.Specular.r  = 1.0f;
	m_baseMaterial.Specular.g  = 1.0f;
	m_baseMaterial.Specular.b  = 1.0f;
	m_baseMaterial.Specular.a  = 1.0f;
	m_baseMaterial.Power = 50.0f;  
	pd3dDevice->SetMaterial( &m_baseMaterial );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );


	D3DXMATRIX matScale;
	D3DXMATRIX matRotation;
	D3DXMATRIX matTrans;
	//////////////////
	D3DXMatrixScaling( &matScale, 1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation( &matTrans, 0.0f, -15.0f, 50.0f );
	D3DXMatrixIdentity( &matWorld );
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_mesh.Render( pd3dDevice, &matWorld ); 
	////////////////////
	D3DXMatrixScaling( &matScale, 0.5f, 0.5f, 0.5f );
	D3DXMatrixTranslation( &matTrans, 10.0f, -15.0f, 50.0f );
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_mesh.Render( pd3dDevice, &matWorld ); 
	////////////////////
	D3DXMatrixScaling( &matScale, 0.5f, 0.5f, 0.5f );
	D3DXMatrixTranslation( &matTrans, -17.0f,  0.0f, 50.0f );
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_mesh.Render( pd3dDevice, &matWorld ); 

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );

	//////////////////////////////////////////////////////////////////

	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	CSize size = m_pTheme->m_pFontGameTitle->GetTextExtent( _T( "CLOCKWORKS Online" ) );
	m_pTheme->m_pFontGameTitle->DrawText( rect.Width() / 2 - size.cx / 2, 0, 0xffffffff, "CLOCKWORKS Online" );

	size = m_pTheme->m_pFontText->GetTextExtent( _T( "Active Desktop" ) );
	m_pTheme->m_pFontText->DrawText( rect.Width() / 2 - size.cx / 2, 40, 0xffffffff, "Active Desktop v0.1" );


	size = m_pTheme->m_pFontText->GetTextExtent( _T( "Copyright (C) 2002~2003 Allright Reserved AEONSOFT Inc." ) );
	m_pTheme->m_pFontText->DrawText( rect.Width() / 2 - size.cx / 2, rect.bottom - 20, 0xffffffff, "Copyright (C) 2002~2003 Allright Reserved AEONSOFT Inc." );
}
BOOL CWndDesktopClockworks::Process()
{
	m_mesh.FrameMove();
	return TRUE;
}
void CWndDesktopClockworks::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	CSize size = m_pTheme->m_pFontText->GetTextExtent( _T( "버튼을 누르세요~" ) );
	size.cx += 20;
	size.cy += 10;
	CRect rect = GetClientRect();
	CPoint point( rect.Width() / 2 - size.cx / 2, 70 );
	m_wndButton.Create( _T( "버튼을 누르세요~" ), 0, CRect( point.x, point.y, point.x + size.cx, point.y + size.cy ), this, 100 ); 
}
BOOL CWndDesktopClockworks::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect(0,0,280,190);
	SetTitle( _T( "디버그 정보" ) );
	return CWndBase::Create( WBS_NOFRAME | WBS_MANAGER, m_pWndRoot->GetWindowRect(), &g_WndMng, dwWndId);
}
BOOL CWndDesktopClockworks::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case 100:
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0055) ) );
//			g_WndMng.OpenMessageBox( _T( "Active Desktop 버튼 콘트롤 테스트" ) );
			break;
		case 101: 
			break;
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndDesktopClockworks::OnCommand(UINT nID,DWORD dwMessage)
{
	return CWndBase::OnCommand(nID,dwMessage);
}
void CWndDesktopClockworks::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType,cx,cy);
}
void CWndDesktopClockworks::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndBase::OnLButtonUp(nFlags,point);
}
void CWndDesktopClockworks::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndBase::OnLButtonDown(nFlags,point);
}
BOOL CWndDesktopClockworks::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
HRESULT CWndDesktopClockworks::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_mesh.InitDeviceObjects( pd3dDevice );
	m_mesh.LoadModel( _T("obj_rideferriswheel.mes") );
	return S_OK;
}
HRESULT CWndDesktopClockworks::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_mesh.RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndDesktopClockworks::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_mesh.InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndDesktopClockworks::DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_mesh.DeleteDeviceObjects();
	return S_OK;
}

