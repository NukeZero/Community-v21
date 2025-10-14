// PreviewBar.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "PreviewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewBar


CPreviewBar::CPreviewBar()
{

}

CPreviewBar::~CPreviewBar()
{
}

BEGIN_MESSAGE_MAP(CPreviewBar, CSizingControlBar2)
	//{{AFX_MSG_MAP(CPreviewBar)
    ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewBar message handlers
int CPreviewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CSizingControlBar2::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_viwer.Create( _T( "Fuck" ), WS_VISIBLE , CRect( 0, 0, 100, 20 ), this, 100 );


/*	
	//Create the Tab Control
	CRect rect;
	m_tabctrl.Create(WS_VISIBLE | WS_CHILD | TCS_BOTTOM, 
		rect, this, IDI_TABCTRLBAR);
	
	m_images.Create(IDB_TABIMAGES, 16, 1, RGB(255,0,255));
	m_tabctrl.SetImageList(&m_images);
	
	// set "normal" GUI-font
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_tabctrl.SetFont(font);
*/


    return 0;
}
void CPreviewBar::OnSize(UINT nType, int cx, int cy) 
{
	CSizingControlBar2::OnSize(nType, cx, cy);
	
	int bottom = ( IsHorzDocked()/*IsHorz()*/ || IsFloating()) ? cy - 14 : cy - 18;
	//m_tabctrl.MoveWindow(7, 7, cx - 14, bottom );
	//m_tabctrl.Invalidate( );
	/*
	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
       pWnd->MoveWindow(9, 9, cx-19, bottom-26);
//	   pWnd->SetWindowPos( &wndTopMost ,9, 9, cx-19, bottom-26, SWP_SHOWWINDOW   );
	   	//	((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);

    }
	//RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
    */
	//int bottom = (IsHorz() || IsFloating()) ? cy - 14 : cy - 18;
	m_viwer.MoveWindow(7, 7, cx - 14, bottom);
	//m_tabctrl.MoveWindow(7, 7, cx - 14, bottom);
	/*
	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
         pWnd->MoveWindow(9, 9, cx-19, bottom-26);
    }
*/

}

void CPreviewBar::OnPaint()
{
	CSizingControlBar2::OnPaint();
/*
	CRect rect;
	GetClientRect( &rect );

    CPaintDC dc(this);

	dc.Draw3dRect( &rect, 0xffffffff, 0xff808080 );
*/
	
	//m_tabctrl.Invalidate( FALSE );

//	CWnd *pWnd;
  //  for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    //{
      //   pWnd=m_views.GetAt(pos)->pWnd;
        // pWnd->Invalidate( TRUE );
    //}
}
void CPreviewBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CSizingControlBar2::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPreviewer

#define PREVIEW_NONE     0
#define PREVIEW_MODEL    1
#define PREVIEW_TEXTURE  2





CPreviewer::CPreviewer()
{
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	m_pObject3D = NULL;
	m_dwPreview = PREVIEW_NONE;
	m_bFrameMoving = FALSE;
}

CPreviewer::~CPreviewer()
{
	//SAFE_DELETE( m_pObject3D );
}


BEGIN_MESSAGE_MAP(CPreviewer, CStatic)
	//{{AFX_MSG_MAP(CPreviewer)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewer message handlers

BOOL CPreviewer::Create( LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	CStatic::Create( lpszText, dwStyle, rect, pParentWnd, nID );
    m_hwndRenderWindow = CStatic::GetSafeHwnd();
    CD3DApplication::m_hWnd = m_hwndRenderWindow;
    CD3DApplication::m_hWndFocus = NULL;
    CD3DApplication::Create( AfxGetInstanceHandle() );
	return TRUE;
}
void CPreviewer::SetModel( DWORD dwType, DWORD dwIndex )
{
	return;
	m_dwPreview = PREVIEW_MODEL;
	//SAFE_DELETE( m_pObject3D );
	m_pObject3D = (CModelObject*)prj.m_modelMng.LoadModel( m_pd3dDevice, dwType, dwIndex );

	//m_pObject3D = new CObject3D;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
//	if( m_pObject3D->LoadObject( _T( "mvr_fVagrant.o3d" ) ) == FAIL )
	//if( m_pObject3D->LoadObject( _T( "obj_box01.o3d" ) ) == FAIL )
	//	return;
	Render3DEnvironment();

}
void CPreviewer::SetTexture( LPCTSTR lpszFileName )
{
	return;
	m_dwPreview = PREVIEW_TEXTURE;
	m_texture.DeleteDeviceObject();//.Free();
	m_texture.LoadTexture( m_pd3dDevice, _T( MakePath( DIR_WORLDTEX, lpszFileName ) ), 0);

	CRect rect;
	GetClientRect( &rect );
	m_texture.m_size.cx = rect.Width();
	m_texture.m_size.cy = rect.Height();
	Render3DEnvironment();
}
void CPreviewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect( &rect );

//	dc.FillSolidRect( &rect, 0xffffffff );

	if( m_pd3dDevice )
		Render3DEnvironment();

//	dc.Draw3dRect( &rect, 0xffffffff, 0xff808080 );	
	// Do not call CStatic::OnPaint() for painting messages
}
HRESULT CPreviewer::InitDeviceObjects()
{
	m_2DRender.InitDeviceObjects( m_pd3dDevice );
	//g_TextureMng.SetD3DDevice( m_pd3dDevice );
	//g_Object3DMng.InitDeviceObjects( m_pd3dDevice );
	return S_OK;
}
HRESULT CPreviewer::RestoreDeviceObjects()
{
#ifdef	__XVS
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
#ifdef	__X2LINK
		CreateSkinningVS( m_pd3dDevice, MakePath( _T( "" ), _T( "skining2.vsh" ) ) );
#else
		CreateSkinningVS( m_pd3dDevice, MakePath( _T( "" ), _T( "skining.vsh" ) ) );
#endif
    }
#endif
	m_2DRender.RestoreDeviceObjects( &m_d3dsdBackBuffer );
	return S_OK;
}
HRESULT CPreviewer::InvalidateDeviceObjects()
{
	m_2DRender.InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CPreviewer::DeleteDeviceObjects()
{
	m_2DRender.DeleteDeviceObjects();
	return S_OK;
}
HRESULT CPreviewer::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT Format, D3DFORMAT backBufferFormat )
{
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE
	// 스키닝 코드 
    // This sample wants mixed vertex processing rather than hardware
    // vertex processing so it can fallback to sw processing if the 
    // device supports fewer than three matrices when skinning.
    if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        return E_FAIL;

    if( pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
        return S_OK;

    return E_FAIL;
}

HRESULT CPreviewer::Render()
{
    // Clear the viewport
	HRESULT hr = m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, 0xff808080, 1.0f, 0 );

/*
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  m_dwFogColor );

    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(m_fFogStartValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(m_fFogEndValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(m_fFogDensity) );

    if( m_bUsingTableFog )
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   m_dwFogMode );
    }
    else
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  m_dwFogMode );
        m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, m_bRangeBasedFog );
    }
*/ 
    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if( m_dwPreview == PREVIEW_MODEL )
		{
			if( m_pObject3D )
			{

				///////////////////////////////////////////////////////
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, 1             );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, 1 );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );		
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTEXF_LINEAR );		

	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );

//	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( dwBlendFactorAlhpa, 0, 0, 0 ) );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

/*

				m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

				m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
*/
				m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );

				CRect rectViewport;;//( 0, 0, viewport.Width, viewport.Height );
				GetClientRect( &rectViewport );

				D3DXMATRIX matProj;
				D3DXMatrixIdentity( &matProj );
				m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

				// 카메라 
				D3DXMATRIX  matView;
				D3DXVECTOR3 vecLookAt( 0.0f, -1.0f, 0.0f );
				D3DXVECTOR3 vecPos(  0.0f, 2.0f, 0.0f );

				D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,0.0f,1.0f) );
				D3DXMatrixIdentity( &matView );
				m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

				D3DXMATRIX matScale, matTrans, matWorld;
				D3DXMatrixIdentity( &matWorld );
				D3DXMatrixIdentity( &matScale );
				D3DXMatrixIdentity( &matTrans );
				D3DXMatrixScaling( &matScale, 0.13f, 0.13f, 0.13f );
				D3DXMatrixTranslation( &matTrans, 0.0f, -0.5f, 0.5f) ;
				D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
				D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
				m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

				/*
				// 프로젝션 
				D3DXMATRIX matProj;
				FLOAT fAspect = ((FLOAT)rectViewport.Width()) / (FLOAT)rectViewport.Height();
				D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
				m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

				// 카메라 
				D3DXMATRIX matView;
				D3DXVECTOR3 vecLookAt( 0.0f, -2.0f, 3.0f );
				D3DXVECTOR3 vecPos(  0.0f, -2.0f, -5.5f );
				D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
				m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

				// 월드 
				D3DXMATRIXA16 matWorld;
				D3DXMATRIXA16 matScale;
				D3DXMATRIXA16 matRot;
				D3DXMATRIXA16 matTrans;

				// 초기화 
				D3DXMatrixIdentity( &matScale );
				D3DXMatrixIdentity( &matRot );
				D3DXMatrixIdentity( &matTrans );
				D3DXMatrixIdentity( &matWorld );
				
				//D3DXMatrixScaling( &matScale, 3.6f, 3.6f, 3.6f );
				//D3DXMatrixTranslation( &matTrans, 0.0f, -5.0f, 5.0f) ;

				//D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
				//D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
				//D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
				m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
*/
				// 랜더링 
				//m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

			//pObject3D->FrameMove();
				//m_pObject3D->Render( m_pd3dDevice, 0, 0, &matWorld );
				m_pObject3D->Render( m_pd3dDevice, &matWorld );

				//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				//m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

				///////////////////////////////////////////////////////
			}
		}
		else
		if( m_dwPreview == PREVIEW_TEXTURE )
		{
			if(	m_texture.m_pTexture )
				m_2DRender.RenderTexture( CPoint( 0, 0), &m_texture );
		}
		/*
        // Reset the world matrix
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity( &matWorld );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Draw the terrain
        m_pd3dDevice->SetVertexShader( D3DFVF_FOGVERTEX );
        m_pd3dDevice->SetStreamSource( 0, m_pTerrainVB, sizeof(FOGVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                   0, m_dwNumTerrainVertices-2 );

        // Draw the columns
        for( DWORD i=0; i<20; i++ )
        {
            FLOAT tx = (i%10)*20.0f - 100.0f;
            FLOAT ty =  0.0f;
            FLOAT tz = (i<=10) ? 40.0f : -40.0f;

            D3DXMatrixTranslation( &matWorld, tx, ty, tz );
            m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

            m_pd3dDevice->SetVertexShader( D3DFVF_FOGVERTEX );
            m_pd3dDevice->SetStreamSource( 0, m_pColumnVB, sizeof(FOGVERTEX) );
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                         0, m_dwNumColumnVertices-2 );
        }
*/
        // End the scene.
        m_pd3dDevice->EndScene();
    }
	return S_OK;
}

HRESULT CPreviewer::AdjustWindowForChange()
{
	/*
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }
	*/
    return S_OK;
}
HRESULT CPreviewer::CheckForLostFullscreen()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();

    return S_OK;
}
void CPreviewer::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	if( cx > 1024 || cy > 1024 )
		
	//if(m_bBeastViewReady == FALSE)
		return;

	HRESULT hr;
	if( m_bActive && m_bWindowed)
	{
		RECT rcClientOld;
		rcClientOld = m_rcWindowClient;

		// Update window properties
		GetWindowRect(&m_rcWindowBounds );
		GetClientRect(&m_rcWindowClient );

		if( rcClientOld.right - rcClientOld.left !=
				m_rcWindowClient.right - m_rcWindowClient.left ||
				rcClientOld.bottom - rcClientOld.top !=
				m_rcWindowClient.bottom - m_rcWindowClient.top)
		{
			// A new window size will require a new backbuffer
			// size, so the 3D structures must be changed accordingly.
			//m_bReady = FALSE;

			m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

			// Resize the 3D environment
			if( FAILED( hr = Reset3DEnvironment() ) )
			{
				//DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
				return;
			}
		//	m_bReady = TRUE;
		}
	}	


	
}

