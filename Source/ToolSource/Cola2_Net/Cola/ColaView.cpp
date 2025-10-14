// ColaView.cpp : implementation of the CColaView class
//

#include "stdafx.h"
#include "mmsystem.h"
#include "Cola.h"

#include "ColaDoc.h"
#include "ColaView.h"
#include "MainFrm.h"

#include "dlg.h"

#include "..\_common\ParticleMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

D3DXVECTOR3	g_vRotate;

extern int		g_nPlay;
static int s_nEmitCnt = 0;

//#define	__STUDY

CColaView	*g_pView = NULL;
extern CColaApp theApp;
extern CMainFrame *g_pMainFrm;

//LARGE_INTEGER	g_llFreq;
LONGLONG		g_llTime = 0;
/////////////////////////////////////////////////////////////////////////////
// CColaView

IMPLEMENT_DYNCREATE(CColaView, CView)

BEGIN_MESSAGE_MAP(CColaView, CView)
	//{{AFX_MSG_MAP(CColaView)
	ON_COMMAND(IDM_ASE_LOAD, OnAseLoad)
	ON_WM_SIZE()
	ON_COMMAND(IDM_BON_SAVE_AS, OnBonSaveAs)
	ON_COMMAND(IDM_ANI_SAVE, OnAniSave)
	ON_COMMAND(IDM_MES_SAVE_FILENAME, OnMesSaveFilename)
	ON_UPDATE_COMMAND_UI(IDM_BON_SAVE_AS, OnUpdateBonSaveAs)
	ON_UPDATE_COMMAND_UI(IDM_ANI_SAVE, OnUpdateAniSave)
	ON_UPDATE_COMMAND_UI(IDM_MES_SAVE_FILENAME, OnUpdateMesSaveFilename)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_COMMAND(IDM_MES_LOAD, OnMesLoad)
	ON_COMMAND(IDM_SELECT, OnSelect)
	ON_UPDATE_COMMAND_UI(IDM_SELECT, OnUpdateSelect)
	ON_WM_KEYDOWN()
	ON_COMMAND(IDM_SWDFORCE1, OnSwdforce1)
	ON_COMMAND(IDM_SWDFORCE2, OnSwdforce2)
	ON_UPDATE_COMMAND_UI(IDM_SWDFORCE1, OnUpdateSwdforce1)
	ON_UPDATE_COMMAND_UI(IDM_SWDFORCE2, OnUpdateSwdforce2)
	ON_COMMAND(IDM_SWDFORCE3, OnSwdforce3)
	ON_COMMAND(IDM_SWDFORCE4, OnSwdforce4)
	ON_UPDATE_COMMAND_UI(IDM_SWDFORCE3, OnUpdateSwdforce3)
	ON_UPDATE_COMMAND_UI(IDM_SWDFORCE4, OnUpdateSwdforce4)
	ON_COMMAND(IDM_MES_SAVE, OnMesSave)
	ON_COMMAND(IDM_MES_SAVEAS, OnMesSaveas)
	ON_COMMAND(IDM_SPEEDUP, OnSpeedup)
	ON_COMMAND(IDM_SPEEDDOWN, OnSpeeddown)
	ON_COMMAND(IDM_SPEEDCLR, OnSpeedclr)
	ON_COMMAND(IDM_OPTION, OnOption)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(IDM_VIEW_FNORMAL, OnUpdateViewFnormal)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_VNORMAL, OnUpdateViewVnormal)
	ON_COMMAND(IDM_VIEW_FNORMAL, OnViewFnormal)
	ON_COMMAND(IDM_VIEW_VNORMAL, OnViewVnormal)
	ON_COMMAND(IDM_NORMAL_CLEAR, OnNormalClear)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_FRAMESKIP, OnFrameskip)
	ON_UPDATE_COMMAND_UI(IDM_FRAMESKIP, OnUpdateFrameskip)
	ON_COMMAND(IDM_CONVERT, OnConvert)
	ON_COMMAND(IDM_TEXTURE, OnTexture)
	ON_UPDATE_COMMAND_UI(IDM_TEXTURE, OnUpdateTexture)
	ON_UPDATE_COMMAND_UI(IDM_MES_SAVE, OnUpdateMesSave)
	ON_UPDATE_COMMAND_UI(IDM_MES_SAVEAS, OnUpdateMesSaveas)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_LIGHTING, OnUpdateViewLighting)
	ON_COMMAND(IDM_VIEW_LIGHTING, OnViewLighting)
	ON_COMMAND(IDM_ENVMAP, OnEnvmap)
	ON_UPDATE_COMMAND_UI(IDM_ENVMAP, OnUpdateEnvmap)
	ON_COMMAND(IDM_PLAY, OnPlay)
	ON_UPDATE_COMMAND_UI(IDM_PLAY, OnUpdatePlay)
	ON_COMMAND(IDM_PREV, OnPrev)
	ON_COMMAND(IDM_NEXT, OnNext)
	ON_UPDATE_COMMAND_UI(IDM_PREV, OnUpdatePrev)
	ON_UPDATE_COMMAND_UI(IDM_NEXT, OnUpdateNext)
	ON_COMMAND(IDM_ANI_LOAD, OnAniLoad)
	ON_COMMAND(IDM_ASE_SAVEAUTO, OnAseSaveauto)
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(IDM_ASE_SAVEAUTO, OnUpdateAseSaveAuto)
	ON_COMMAND(IDM_ATTRHIT, OnAttrhit)
	ON_UPDATE_COMMAND_UI(IDM_ATTRHIT, OnUpdateAttrhit)
	ON_COMMAND(IDM_ATTRSND, OnAttrsnd)
	ON_UPDATE_COMMAND_UI(IDM_ATTRSND, OnUpdateAttrsnd)
	ON_COMMAND(IDM_SOLID, OnSolid)
	ON_UPDATE_COMMAND_UI(IDM_SOLID, OnUpdateSolid)
	ON_COMMAND(IDM_WIREFRAME, OnWireframe)
	ON_UPDATE_COMMAND_UI(IDM_WIREFRAME, OnUpdateWireframe)
	ON_COMMAND(IDM_POINT, OnPoint)
	ON_UPDATE_COMMAND_UI(IDM_POINT, OnUpdatePoint)
	ON_COMMAND(IDM_VIEWBBALL, OnViewbball)
	ON_UPDATE_COMMAND_UI(IDM_VIEWBBALL, OnUpdateViewbball)
	ON_COMMAND(IDM_VIEWBBONE, OnViewbbone)
	ON_UPDATE_COMMAND_UI(IDM_VIEWBBONE, OnUpdateViewbbone)
	ON_COMMAND(IDM_VIEWBBNOT, OnViewbbnot)
	ON_UPDATE_COMMAND_UI(IDM_VIEWBBNOT, OnUpdateViewbbnot)
	ON_COMMAND(IDM_AUTOROTATE, OnAutorotate)
	ON_UPDATE_COMMAND_UI(IDM_AUTOROTATE, OnUpdateAutoRotate)
	ON_COMMAND(IDM_EDGEDFACE, OnEdgedFace)
	ON_UPDATE_COMMAND_UI(IDM_EDGEDFACE, OnUpdateEdgedFace)
	ON_COMMAND(IDM_ROTATELIGHT, OnRotateLight)
	ON_UPDATE_COMMAND_UI(IDM_ROTATELIGHT, OnUpdateRotateLight)
	ON_COMMAND(IDM_LOD1, OnLod1)
	ON_UPDATE_COMMAND_UI(IDM_LOD1, OnUpdateLod1)
	ON_COMMAND(IDM_LOD2, OnLod2)
	ON_UPDATE_COMMAND_UI(IDM_LOD2, OnUpdateLod2)
	ON_COMMAND(IDM_LOD3, OnLod3)
	ON_UPDATE_COMMAND_UI(IDM_LOD3, OnUpdateLod3)
	ON_COMMAND(IDM_GRID, OnGrid)
	ON_UPDATE_COMMAND_UI(IDM_GRID, OnUpdateGrid)
	ON_COMMAND(IDM_RELOAD_REFLECTION, OnReloadReflection)
	ON_COMMAND(IDM_NEXT_OP, OnNextOp)
	ON_COMMAND(IDM_LIGHTNING, OnLightning)
	ON_UPDATE_COMMAND_UI(IDM_LIGHTNING, OnUpdateLightning)
	ON_COMMAND(IDM_COLLOBJECT, OnCollobject)
	ON_UPDATE_COMMAND_UI(IDM_COLLOBJECT, OnUpdateCollobject)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32829, OnUpdateCamLod)
	ON_COMMAND(ID_BUTTON32829, OnCamLod)
	ON_COMMAND(IDM_TEXTURE_EFF_ALPHA, OnViewTextureEffect )
	ON_UPDATE_COMMAND_UI(IDM_TEXTURE_EFF_ALPHA, OnUpdateViewTextureEffect)
	ON_COMMAND(IDM_TEXTURE_EFF_BASE, OnViewTextureEffectBase)
	ON_UPDATE_COMMAND_UI(IDM_TEXTURE_EFF_BASE, OnUpdateViewTextureEffectBase)

	ON_COMMAND( IDM_TOPVIEW, OnTopView )
	ON_UPDATE_COMMAND_UI(IDM_TOPVIEW, OnUpdateTopView)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CColaView construction/destruction
CColaView :: CColaView()
{
//	int abc[10] = { 0, };

//	for( int i = 0 ; i < 5; i ++ )
//		abc[i] = i;
	// TODO: add construction code here
	g_pView = this;
    m_bActive           = FALSE;
//    m_bReady            = FALSE;
    m_bWindowed         = TRUE;
    
//    m_dwAdapter         = 0L;
	m_pD3D              = NULL;
    m_pd3dDevice        = NULL;
//    m_bUseDepthBuffer   = TRUE;

	m_nCount = 0;
	m_nLightRotateCnt = 0;
	m_pFont = NULL;
//	m_pASE = NULL;
	m_pModel = NULL;
//    m_dwVertexShader    = 0L;
	m_bUseCLOD = true;
	D3DXMatrixIdentity( &m_mModel );

	QueryPerformanceFrequency( &g_llFreq );

	g_vRotate = D3DXVECTOR3( 0, 0, 0 );
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_d3dEnumeration.AppMinDepthBits = 24;
#ifdef __SHADOW
    m_d3dEnumeration.AppMinStencilBits = 4;
#endif
    m_d3dEnumeration.AppUsesMixedVP = TRUE;
	
	m_nNumParts = 0;
}

CColaView::~CColaView()
{
    Cleanup3DEnvironment();
#ifdef	__STUDY
	Message( "%d", g_llTime );
#endif
}

BOOL CColaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CColaView drawing

void CColaView::OnDraw(CDC* pDC)
{
	CColaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CColaView diagnostics

#ifdef _DEBUG
void CColaView::AssertValid() const
{
	CView::AssertValid();
}

void CColaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CColaDoc* CColaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColaDoc)));
	return (CColaDoc*)m_pDocument;
}
#endif //_DEBUG

//-----------------------------------------------------------------------------
// Name: CheckForLostFullscreen()
// Desc: If fullscreen and device was lost (probably due to alt-tab), 
//       automatically switch to windowed mode
//-----------------------------------------------------------------------------
HRESULT CColaView::CheckForLostFullscreen()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CColaView message handlers
//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
//        g_pView->OnToggleFullScreen();
        theApp.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }

    if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }

    if( msg == WM_KEYUP && wParam == VK_ESCAPE )
    {
        // User wants to leave fullscreen mode
//        g_pView->OnToggleFullScreen();
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

extern LPDIRECT3DTEXTURE9	g_pReflectMap;
extern LPDIRECT3DTEXTURE9	g_pReflectMap2;
extern LPDIRECT3DTEXTURE9	g_pReflectMap3;
#ifdef	__STUDY
void	InitTest( void );		// 나중에 지울것
#endif

void CColaView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
    // Save static reference to the render window
    m_hwndRenderWindow = GetSafeHwnd();

    // Register a class for a fullscreen window
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );

    // We create the fullscreen window (not visible) at startup, so it can
    // be the focus window.  The focus window can only be set at CreateDevice
    // time, not in a Reset, so ToggleFullscreen wouldn't work unless we have
    // already set up the fullscreen focus window.
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, CW_USEDEFAULT,
                                           CW_USEDEFAULT, 100, 100,
                                           GetTopLevelParent()->GetSafeHwnd(), 0L, NULL, 0L );

    // Note that for the MFC samples, the device window and focus window
    // are not the same.
    CD3DApplication::m_hWnd = m_hwndRenderWindow;
    CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
    CD3DApplication::Create( AfxGetInstanceHandle() );

	DragAcceptFiles( TRUE );

	// CMD line에 ASE명이 입력되었다면 그것을 읽음
	CString	strCmd = g_pApp->GetCmdFile();
	LoadModel( strCmd );
	SetFrameSkip( TRUE );
	xSRand( timeGetTime() );

//	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "mvr_atrox.tga" ), &g_pReflectMap ) ) )
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "Part_fAcr05Upper.tga" ), &g_pReflectMap ) ) )
//	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "mvr_Abraxas.tga" ), &g_pReflectMap ) ) )
	{
		//Error( "Reflect.tga 텍스쳐 못읽음" );
	}

//	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "magician_firebomb2.dds" ), &g_pReflectMap2 ) ) )
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "water_river01.dds" ), &g_pReflectMap2 ) ) )
	{
		//Error( "Reflect.tga 텍스쳐 못읽음" );
	}

	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "magician_firebomb2.tga" ), &g_pReflectMap3 ) ) )
	{
		//Error( "Reflect.tga 텍스쳐 못읽음" );
	}

#ifdef	__STUDY
	InitTest();

	m_vRayOrig.x = 0;
	m_vRayOrig.y = 5.0f;
	m_vRayOrig.z = -5.0;

	m_vRayDir.x = 0;
	m_vRayDir.y = 1.0f;
	m_vRayDir.z = 0;
#endif
/*
	while(1)
	{
		D3DXVECTOR3	v1, v2;
		v1.x = 0;	v1.y = 0;	v1.z = 0;
		v2.x = 0;	v2.y = -10.0f;	v2.z = 10.0f;
		float	yDist = v2.y - v1.y;
		float	zDist = v2.z - v1.z;

		double r = atan2( yDist, -zDist );
		double a = D3DXToDegree( r );
	}
*/

	if( 0 )
	{
		D3DXVECTOR3	v1, v2, v3;
		v1.x = 0;	v1.y = 0;	v1.z = 0;
		v2.x = 0;	v2.y = 10.0f;	v2.z = 10.0f;
		float	yDist = v2.y - v1.y;
		float	zDist = v2.z - v1.z;
		int		i;
		static double r[100];
		static D3DXVECTOR3 v[100];
		LARGE_INTEGER llCnt1, llCnt2;

		v3 = v2 - v1;
		QueryPerformanceCounter( &llCnt1 );
		for( i = 0; i < 100; i ++ )
		{
			r[i] = atan2( (double)(yDist+(double)i), (double)zDist );
		}
//			D3DXVec3Normalize( &v[i], &v3 );

		QueryPerformanceCounter( &llCnt2 );
		int nTime = (int)(llCnt2.QuadPart - llCnt1.QuadPart);

		QueryPerformanceCounter( &llCnt1 );
		for( i = 0; i < 100; i ++ )
		{
			D3DXVec3Normalize( &v[i], &v3 );
		}
		
		QueryPerformanceCounter( &llCnt2 );
		int nTime2 = (int)(llCnt2.QuadPart - llCnt1.QuadPart);
		
		Message( "%d %d", nTime, nTime2 );
		
	}
	
	CString strTitle = g_strModel;
#ifdef __VIEWER_ONLY
	strTitle += " - AeonSoft Viewer ";
#else
	strTitle += " - AeonSoft Cola ";
#endif
	strTitle += g_strVersion;
	g_pMainFrm->SetWindowText( strTitle );
}



//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Adjusts the window properties for windowed or fullscreen mode
//-----------------------------------------------------------------------------
HRESULT CColaView::AdjustWindowForChange()
{
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
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CColaView::OneTimeSceneInit()
{
    m_pFont = new CD3DFont( _T("Arial"), 9, D3DFONT_BOLD );
	HRESULT hr = g_SoundMng.Initialize( GetSafeHwnd(), DSSCL_PRIORITY, 2, 22050, 16 );
	
//    g_pFloorTexture = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CColaView::FrameMove()
{
//	D3DXMatrixTranslation( &m_mWorld, 0, 0, 0 );
//	m_mWorld = *m_ArcBall.GetRotationMatrix() * *m_ArcBall.GetTranslationMatrix(); 
	D3DXMATRIX m1, m2;
	D3DXMatrixRotationX( &m1, g_vRotate.x );
	D3DXMatrixRotationY( &m2, g_vRotate.y );
	m_mWorld = m2 * m1;
	m_mWorld *= *m_ArcBall.GetTranslationMatrix();
//	D3DXMatrixRotationYawPitchRoll( &m_mWorld, g_vRotate.y, g_vRotate.x, 0 );
	//	D3DXMatrixMultiply( &m_mWorld, &m_mWorld, m_ArcBall.GetRotationMatrix() );
//	D3DXMatrixMultiply( &m_mWorld, &m_mWorld, m_ArcBall.GetTranslationMatrix() );

#ifdef	__STUDY
//	D3DXMatrixRotationX( &m_mModel, D3DXToRadian(45) );
#endif
    // Lighting vector (normalized) and material colors. (Use red light
    // to show difference from non-vertex shader case.)
/*
	D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.0f, 765.01f );
    D3DXVECTOR4 vLight( 1.0f, -1.0f, 1.0f, 0.0f );
    D3DXVec4Normalize( &vLight, &vLight );
//	FLOAT       fDiffuse[] = { 1.00f, 1.00f, 0.00f, 0.00f };
//	FLOAT       fAmbient[] = { 0.25f, 0.25f, 0.25f, 0.25f };
	FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
	FLOAT       fAmbient[] = { 1.00f, 1.00f, 1.00f, 1.00f };

	D3DXMATRIX	mViewProj, mViewProjTranspose;
	mViewProj = m_mView * m_mProj;
	D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
	
    m_pd3dDevice->SetVertexShaderConstant( 66, &mViewProjTranspose, 4 );
    m_pd3dDevice->SetVertexShaderConstant( 70, &vLight,   1 );
    m_pd3dDevice->SetVertexShaderConstant( 71, &fDiffuse, 1 );
    m_pd3dDevice->SetVertexShaderConstant( 72, &fAmbient, 1 );
    m_pd3dDevice->SetVertexShaderConstant( 73, &vConst, 1 );
*/

	if( m_pModel )
	{
//		if( g_nPlay )
		static float fOldFrame;
		fOldFrame = m_pModel->m_fFrameCurrent;
		m_pModel->FrameMove();
//		MOTION_ATTR *pAttr = m_pModel->IsAttrSound();
//		if( pAttr )
//		{
//			PLAYSND( pAttr->m_nSndID );
//		}
		if( m_pModel->m_fFrameCurrent != fOldFrame )
		{
//			((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.SetFrameNum( m_pModel->m_fFrameCurrent );
			((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);
		}

		if(m_bUseCLOD)
		{
			// 모델과 카메라의 거리에 따라 LOD모델 세팅
			D3DXVECTOR3		vMPos,vCamPos, vLength;
			float			fLength;


			vMPos.x = m_mModel._41;
			vMPos.y = m_mModel._42;
			vMPos.z = m_mModel._43;
			
			vCamPos.x = m_mView._41;
			vCamPos.y = m_mView._42;
			vCamPos.z = m_mView._43;

			vLength = vCamPos - vMPos;
			fLength = D3DXVec3Length(&vLength);
			int nLevel = int(fLength / 10.0f);
			if(nLevel > 2) nLevel = 2;
			m_pModel->SetGroup(nLevel);
			if(m_nLastLodLv != nLevel)
			{
				CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
				pMainFrm->m_dlgInfo.UpdateInfo();
			}
			m_nLastLodLv = nLevel;
		}
	}
//	if( m_pASE )
//		m_pASE->FrameMove();
//	CBipedMesh	*pBip = (CBipedMesh *)m_pModel;
//	if( pBip )
//	{
//		pBip->FrameMove();
//	} 

	m_nCount ++;

    return S_OK;
}

void	CColaView :: RenderGrid( void )
{
	FVF_3DVERTEX	aList[4];

	aList[0].vPos = D3DXVECTOR3( -100.0f, 0.0f, 0.0f );
	aList[1].vPos = D3DXVECTOR3( 100.0f, 0.0f, 0.0f );
	aList[2].vPos = D3DXVECTOR3( 0.0f, 0.0f, 100.0f );
	aList[3].vPos = D3DXVECTOR3( 0.0f, 0.0f, -100.0f );

	aList[0].dwColor = 0xffffffff;
	aList[1].dwColor = 0xffffffff;
	aList[2].dwColor = 0xffffffff;
	aList[3].dwColor = 0xffffffff;

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 2, aList, sizeof(FVF_3DVERTEX) );


}


void	CColaView::SetLight( void )
{
//	D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;

	m_vLightPos = D3DXVECTOR3( 5.0f * sin((float)m_nLightRotateCnt/100.0f), 5.0f, 5.0f * cos((float)m_nLightRotateCnt/100.0f) );
	m_vLightDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f) - m_vLightPos;
	float l = D3DXVec3Length( &m_vLightDir );
	D3DXVec3Normalize( &m_vLightDir, &m_vLightDir );

	if( g_nAutoRotateLight )
		m_nLightRotateCnt ++;
	
#ifdef	__SPECULAR
	light.Diffuse.r  = 0.5f;
	light.Diffuse.g  = 0.5f;
	light.Diffuse.b  = 0.5f;
	light.Specular.a = light.Specular.r = light.Specular.g = light.Specular.b = 1.0f;
#else
	light.Diffuse.r  = 1.0f;
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 1.0f;
#endif
//    vecDir = D3DXVECTOR3( cosf(timeGetTime()/350.0f),
//							1.0f,
//							sinf(timeGetTime()/350.0f) );
//	vecDir = D3DXVECTOR3(-1.0F, -1.0F, 1.0f );		// 

//    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &m_vLightDir );
//	light.Direction = D3DXVECTOR3( m_vLightDir.x, m_vLightDir.y, m_vLightDir.z );
	light.Direction = m_vLightDir;
    light.Range       = 1000.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
//    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
	int nAmbient = (g_Option.m_nR << 16) | (g_Option.m_nG << 8) | g_Option.m_nB;
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, nAmbient );
#ifdef	__SPECULAR
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
//	m_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL ); 
#endif
	 
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
#ifdef	__STUDY
//----------------------------------------------------------------------------
struct LINE
{
	D3DXVECTOR3	vStart;
	D3DXVECTOR3 vEnd;
};

struct OBB
{
	D3DXVECTOR3	vCenter;
	D3DXVECTOR3	vAxis[3];
	float		fExtent[3];
};

bool TestIntersection (const LINE& rkLine, const OBB& rkBox)
{
    float fAWdU[3], fAWxDdU[3], fRhs;

    D3DXVECTOR3 kDiff = rkLine.vStart - rkBox.vCenter;
    D3DXVECTOR3 kWxD;
	
	D3DXVec3Cross( &kWxD, &rkLine.vEnd, &kDiff );

	fAWdU[1] = fabs(D3DXVec3Dot( &rkLine.vEnd, &rkBox.vAxis[1] ));
    fAWdU[2] = fabs(D3DXVec3Dot( &rkLine.vEnd, &rkBox.vAxis[2] ));
    fAWxDdU[0] = fabs(D3DXVec3Dot( &kWxD, &rkBox.vAxis[0]));
    fRhs = rkBox.fExtent[1]*fAWdU[2] + rkBox.fExtent[2]*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWdU[0] = fabs(D3DXVec3Dot( &rkLine.vEnd, &rkBox.vAxis[0] ));
    fAWxDdU[1] = fabs(D3DXVec3Dot( &kWxD, &rkBox.vAxis[1]));
    fRhs = rkBox.fExtent[0]*fAWdU[2] + rkBox.fExtent[2]*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = fabs(D3DXVec3Dot( &kWxD, &rkBox.vAxis[2]));
    fRhs = rkBox.fExtent[0]*fAWdU[1] + rkBox.fExtent[1]*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}

LINE	g_Line;
OBB		g_OBB;
float	rx=0, ry=0, rz=0;			// x, y, z 회전치
D3DXVECTOR3	g_vBasic;
D3DXMATRIX	mOBB;

D3DXVECTOR3	g_vMin, g_vMax;		// OBB의 좌상 우하

void	InitTest2();
void	InitTest( void )
{
	g_Line.vStart = D3DXVECTOR3( 0, 0, 0 );
	g_Line.vEnd   = D3DXVECTOR3( 1, 2, 0 );

	// 회전 매트릭스 설정
	D3DXMatrixIdentity( &mOBB );
	ry = D3DXToRadian( 32 );
	D3DXMatrixRotationYawPitchRoll( &mOBB, ry, rx, rz );	// OBB회전 매트릭스

	// OBB설정
	g_vMin = D3DXVECTOR3( -1, -1, -1 );
	g_vMax = D3DXVECTOR3( 1, 1, 1 );

	g_OBB.vCenter = D3DXVECTOR3( 0, 0, 0 );
	g_OBB.fExtent[0] = 1.0f;
	g_OBB.fExtent[1] = 1.0f;
	g_OBB.fExtent[2] = 1.0f;

	D3DXMATRIX	mTemp = mOBB;
	// 회전매트릭스를 3개의 축으로 변환
	D3DXVECTOR3	vBasic0 = D3DXVECTOR3( 1, 0, 0 );	// y,p,r이 원점일때의 좌표를 넣음.
	D3DXVECTOR3	vBasic1 = D3DXVECTOR3( 0, 1, 0 );	// y,p,r이 원점일때의 좌표를 넣음.
	D3DXVECTOR3	vBasic2 = D3DXVECTOR3( 0, 0, 1 );	// y,p,r이 원점일때의 좌표를 넣음.

	D3DXMatrixRotationYawPitchRoll( &mTemp, ry, rx, rz );	// 
	D3DXVec3TransformCoord( &g_OBB.vAxis[0], &vBasic0, &mTemp );
	D3DXVec3TransformCoord( &g_OBB.vAxis[1], &vBasic1, &mTemp );
	D3DXVec3TransformCoord( &g_OBB.vAxis[2], &vBasic2, &mTemp );

	InitTest2();
}

D3DXVECTOR3	g_vTri[3];

// 선 - 삼각형 충돌 테스트 
void	InitTest2( void )
{
	// 선 정의
	g_Line.vStart = D3DXVECTOR3( 3.4, 2.4, 3.7 );
	g_Line.vEnd   = D3DXVECTOR3( 0.7, 2.7, 1.0 );

	// 삼각형 정의
	g_vTri[0] = D3DXVECTOR3( 2.9, 3.1, 0.3 );
	g_vTri[1] = D3DXVECTOR3( 0.4, 3.6, 2.9 );
	g_vTri[2] = D3DXVECTOR3( 0.4, 0.6, 0.9 );

	D3DXMATRIX	m1, m2, mInv;

	D3DXMatrixIdentity( &m1 );
	D3DXMatrixRotationX( &m1, D3DXToRadian( 32 ) );
	m1._41 = 10;
	m1._42 = 20;
	m1._43 = 30;
	D3DXMatrixIdentity( &m2 );
	D3DXMatrixRotationX( &m2, D3DXToRadian( 45 ) );
	m2._41 = 5;
	m2._42 = 10;
	m2._43 = 15;

	D3DXVECTOR3 v = D3DXVECTOR3( 1, 2, 3 );
	D3DXVECTOR3 vFinal, v1, v2;

	// vFial = v * M1 * 0.3 + v * M2 * 0.7
	D3DXVec3TransformCoord( &v1, &v, &m1 );
	v1 *= 0.41;					
	D3DXVec3TransformCoord( &v2, &v, &m2 );
	v2 *= 0.6;

	vFinal = v1 + v2;

	//------------------------- v = vFinal / 0.3 * Inv(M1)
	D3DXVECTOR3		vTemp;
/*
	m1 *= 0.3;
	m2 *= 0;
	m1 += m2;
	D3DXMatrixInverse( &mInv, NULL, &m1 );
	D3DXVec3TransformCoord( &vTemp, &vFinal, &mInv );
*/
	
	m1 *= 0.41;
	m2 *= 0.6;
	m1 += m2;
	D3DXMatrixInverse( &mInv, NULL, &m1 );
	D3DXVec3TransformCoord( &vTemp, &vFinal, &mInv );
/*

	v1 = vFinal / 0.3;
	D3DXVec3TransformCoord( &v1, &v1, &mInv );
	v2 = vFinal / 0.7;
	D3DXMatrixInverse( &mInv, NULL, &m2 );
	D3DXVec3TransformCoord( &v2, &v2, &mInv );
	vTemp = v1 + v2;
*/

	

}

// 삼각형과 라인의 충돌/교차점 검사
//
// vTri : vTri[0], [1], [2]   각 버텍스
// vN : Tri의 노말
// D : 평면방정식 D
// v1, v2 : 라인의 시작점 끝점
// vIntersect(OUT) : 교차점
// 
/*
BOOL	IsTouchTriLine( const D3DXVECTOR3 *vTri, const D3DXVECTOR3 &vN, float D, 
					    const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *vIntersect )
{
	// 평면(plane)과 선의 교차점 계산
	float	i0 = D3DXVec3Dot( &vN, &v1 );
	float	i1 = D3DXVec3Dot( &vN, &v2 );

	float	t = -(i1 + D) / (i0 - i1);
	*vIntersect = (v1 * t) + (v2 * (1.0f - t));

	// 선과 평면의 교차점 계산(D3DX버전) - 이함수는 t값을 알수 없으므로 충돌여부를 알지 못한다.  
//	D3DXPLANE	plane = D3DXPLANE( vN.x, vN.y, vN.z, D );
//	D3DXPlaneIntersectLine( &vIntersect, &plane, &v1, &v2 );

	float	ax, ay, bx, by, cx, cy, tx, ty;
	float	ux, uy, utx, uty;
	float fUxV;
	
	// x축 평면(z, y평면)에 투영하여 삼각형 점포함 테스트
	tx = vIntersect->z;	ty = vIntersect->y;
	ax = vTri[0].z;	ay = vTri[0].y;
	bx = vTri[1].z;	by = vTri[1].y;  0
	cx = vTri[2].z;	cy = vTri[2].y;

	// AB x AT
	ux = bx - ax;	uy = by - ay;	// AB
	utx = tx - ax;	uty = ty - ay;	// AT
	
	fUxV = xMathCCW( ux, uy, utx, uty );
	if( fUxV < 0 )	


	// y축 평면에 투영하여 삼각형 점포함 테스트

	// z축 평면에 투영하여 삼각형 점포함 테스트


}
*/
BOOL IntersectTriangle2( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
						const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, 
						D3DXVECTOR3* pIntersect, FLOAT* pfDist )
{
    // Find vectors for two edges sharing vert0
	FLOAT t,u,v;
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
		// 반대 방향. cull ok
		return FALSE;
		// 뒤집는다. 
        //tvec = v0 - orig;
        //det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < -0.05f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &dir, &qvec );
    if( v < -0.05f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

	*pIntersect = orig + t * dir;
    return TRUE;
}


void	RenderTest2( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX *mWorld )
{
//    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	g_Grp3D.Render3DLine( g_Line.vStart, g_Line.vEnd, 0xff0000ff );		// 라인 그리기,


	D3DXVECTOR3	vN, v1, v2;
	// calc normal
	v1 = g_vTri[0] - g_vTri[1];
	v2 = g_vTri[0] - g_vTri[2];
	D3DXVec3Cross( &vN, &v1, &v2 );	// calc normal
	// draw normal
	v1 = (g_vTri[0] + g_vTri[1] + g_vTri[2]) / 3;
	v2 = v1 + vN;
	g_Grp3D.Render3DLine( v1, v2, 0xffff0000 );		// draw normal
/*
	// 평면 방정식 D 계산
	float	D = -D3DXVec3Dot( &vN, &g_vTri[0] );	// 평면방정식 D
*/
	D3DXVECTOR3	vLineDir = g_Line.vEnd - g_Line.vStart;
	D3DXVECTOR3	vIntersect;
	FLOAT	fDist, u, v;
	LARGE_INTEGER	llCnt, llCnt2;
	int		i;
	BOOL ret ;

	QueryPerformanceCounter( &llCnt );
	for( i = 0; i < 100; i ++ )
	{
//		ret = IntersectTriangle2( g_vTri[0], g_vTri[1], g_vTri[2], g_Line.vStart, vLineDir, &vIntersect, &fDist );
//		FLOAT	fDot = D3DXVec3Dot( &vN, &vLineDir );
//		if( fDot > 0 )	continue;
		ret = D3DXIntersectTri( &g_vTri[0], &g_vTri[1], &g_vTri[2], &g_Line.vStart, &vLineDir, &u, &v, &fDist );
		vIntersect = g_Line.vStart + fDist * vLineDir;
	}
	QueryPerformanceCounter( &llCnt2 );
	g_llTime = llCnt2.QuadPart - llCnt.QuadPart;



	D3DXVECTOR3	vMin = vIntersect - D3DXVECTOR3( 0.1f, 0.1f, 0.1f );
	D3DXVECTOR3 vMax = vIntersect + D3DXVECTOR3( 0.1f, 0.1f, 0.1f );
	g_Grp3D.RenderAABB( vMin, vMax, 0xffffff00 );					// 교차점 그리기
	
	g_Grp3D.Render3DTri( g_vTri, (ret==TRUE)?0xffff0000:0xffffffff );		// 삼각형 그리기

}

void	RenderOBB( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX *mWorld, DWORD dwColor )
{
	D3DXMATRIX	m;

	m = mOBB * *mWorld;		// OBB의 회전행렬과 월드행렬을 곱함.
	pd3dDevice->SetTransform( D3DTS_WORLD,	  &m );		// Set World Transform 
	g_Grp3D.RenderAABB( g_vMin, g_vMax, dwColor );
	
}

void	RenderTest( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX *mWorld )
{
	// 충돌테스트용 라인 draw
	g_Grp3D.Render3DLine( g_Line.vStart, g_Line.vStart + g_Line.vEnd );
/*
	g_Grp3D.Render3DLine( g_OBB.vCenter, g_OBB.vAxis[0] / 4, 0xffff0000 );
	g_Grp3D.Render3DLine( g_OBB.vCenter, g_OBB.vAxis[1] / 4, 0xff00ff00 );
	g_Grp3D.Render3DLine( g_OBB.vCenter, g_OBB.vAxis[2] / 4, 0xff0000ff );

	D3DXVECTOR3 vCross;
    D3DXVECTOR3 kDiff = g_Line.vStart - g_OBB.vCenter;
	D3DXVec3Cross( &vCross, &g_Line.vEnd, &kDiff );
	g_Grp3D.Render3DLine( g_OBB.vCenter, vCross, 0xffffff00 );
*/
	D3DXVECTOR3 vEnd = g_Line.vStart + g_Line.vEnd;
	D3DXVECTOR3 vIntersect;
	BOOL ret = IsTouchOBB_Line( g_vMin, g_vMax, mOBB, g_Line.vStart, vEnd, &vIntersect );
//	bool r = TestIntersection( g_Line, g_OBB );
	
	//( pd3dDevice, mWorld, (ret==TRUE)?0xffff0000:0xffffffff );
/*
	pd3dDevice->SetTransform( D3DTS_WORLD,	  &m );		// Set World Transform 
	g_Grp3D.RenderAABB( g_OBB.vCenter + D3DXVECTOR3( -g_OBB.fExtent[0], -g_OBB.fExtent[1], -g_OBB.fExtent[2] ), 
						g_OBB.vCenter + D3DXVECTOR3( g_OBB.fExtent[0], g_OBB.fExtent[1], g_OBB.fExtent[2] ), 
						(r==true)?0xffff0000:0xffffffff );
*/
}

#endif	// study


CPartsBeam	g_Beam;

//static int nCnt = 0;



TCHAR	g_szStr[256];

int		testx, testy;

HRESULT CColaView::Render()
{
	D3DXMATRIX	m1;
	D3DXMATRIX	mViewProj;
	float	bias = 0.0f;
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;


/*	g_mReflect = D3DXMATRIX(	0.50f, 0.00f, 0.00f, 0.00f,
								0.00f,-0.50f, 0.00f, 0.00f,
								0.00f, 0.00f, 0.50f, 0.00f,
								0.50f, 0.50f, 0.00f, 1.00f );*/
	

	// 렌더타겟에 shadow zbuffer를 만든다.
#ifdef __SHADOW2
	
	//	D3DXMATRIX mWorldToLight; // [1] 월드             -> 광원 좌표
	//	D3DXMATRIX mShadowProj;   // [3] 광원 좌표        -> 텍스쳐 좌표
	

	if( m_pModel && g_nLighting )
	{
		HRESULT hr;
		hr = g_pRenderToSurface->BeginScene( g_pShadowSurface, NULL );
		hr = pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET , D3DCOLOR_ARGB(0,255,255,255), 0, 0 );
		
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		
		// TEXTUREFACTOR 를 색상으로 사용해서 단색 렌더링을 할것임.
		//		pd3dDevice->SetTexture(0,NULL);
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255,192,192,192) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		//		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
		//		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
		// 변환 행렬 설정.
		// [그림자 객체 좌표] -> [월드좌표] -> [광원좌표] -> [텍스쳐 좌표]
		//		D3DXVECTOR3 vLight( m_vLightDir.x, m_vLightDir.y, m_vLightDir.z );
		D3DXVECTOR3 vPos( m_mModel._41, m_mModel._42 + 1.0f, m_mModel._43 );
		//		SetLightVec( vLight );		// 라이트 벡터 외부에서세팅
		D3DXVECTOR3 vLightPos( m_vLightPos.x, m_vLightPos.y, m_vLightPos.z );
		D3DXMatrixLookAtLH( &g_mViewLight, &vLightPos, &vPos, &D3DXVECTOR3(0.0f,1.0f,0.0f));
		D3DXMatrixPerspectiveFovLH( &g_mShadowProj, D3DX_PI/4, 1.0f, 0.5f, 5000.0f );
		pd3dDevice->SetTransform( D3DTS_VIEW, &g_mViewLight );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_mShadowProj );
		
		// 렌더링.
		//		m_pModel->m_nNoTexture = 1;		// 텍스쳐를 사용하지 못하게 함.
		m_pModel->m_nNoEffect = 1;
		if(!m_bUseCLOD)
		{
			m_pModel->SetGroup( g_nLOD );
		}
		
		m_pModel->Render( m_pd3dDevice, &m_mModel );
		m_pModel->m_nNoEffect = 0;
		m_pModel->m_nNoTexture = 0;
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_Grp2D.DrawRect( 0, 0, 2048, 2048, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		
		g_pRenderToSurface->EndScene( D3DX_FILTER_POINT );
	}
#endif // SHADOW2
	

	g_nMaxTri = 0;
    HRESULT hClear = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
//                       D3DCOLOR_ARGB(255, 128, 0, 0),
					   g_Option.m_bgColor, 
					   1.0f, 0L );

	if( FAILED( hClear ) )
		DisplayErrorMsg( hClear, MSGERR_APPMUSTEXIT );
    // Begin the scene
    if( FAILED( m_pd3dDevice->BeginScene() ) )
        return E_FAIL;
	
#ifdef __SHADOW2
//	if( 0 )
	RenderShadowMapInfo( pd3dDevice );
#endif
//	int *p = NULL;
//	*p = 1;

	//sunTODO:

	// Set the transform matrices
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 1.0f, -4.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );

	//__BS_TOPVIEW
	if( g_bTopView )
	{	
		vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -6.0f );
		vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
		vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		g_vRotate.x = - ( D3DX_PI /2 );
	}

	D3DXMatrixLookAtLH( &m_mView, &vEyePt, &vLookatPt, &vUpVec );
	D3DXMATRIX mTest = m_mView;
	m_mView = m_mWorld * m_mView;
	FLOAT       fAspect = ((FLOAT)m_nWidth) / m_nHeight;
	D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 0.01f, 5000.0f );
//	D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 0.1f, 5000.0f );
//	D3DXMatrixOrthoLH( &m_mProj, (FLOAT)m_nWidth/200, (FLOAT)m_nHeight/200, 0.1f, 5000.0f );

	m_ArcBall.SetWindow( m_nWidth, m_nHeight, 1.0f );
	//m_ArcBall.Init( );

	m_pd3dDevice->SetTransform( D3DTS_WORLD,	  &m_mModel );		// Set World Transform 
	m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_mView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	// Grid판 렌더.
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// 그리드 그릴땐 라이팅 꺼야 한다.
    m_pd3dDevice->SetMaterial( &mtrl );
	m_pd3dDevice->SetTexture( 0, NULL );
	D3DXMatrixIdentity( &m1 );
	m_pd3dDevice->SetTransform( D3DTS_WORLD,	  &m1 );		// Set World Transform 
	if( g_nGrid )
	{
		g_Grp3D.RenderGrid( COL_DARKGRAY );
		g_Grp3D.Render3DLine( m_vRayOrig, m_vRayOrig + m_vRayDir, COL_RED );
	}
	
	if( g_nLighting )
		SetLight();	//
	else
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
	

#ifdef	__STUDY
//	RenderTest( m_pd3dDevice, &m_mWorld );
	RenderTest2( m_pd3dDevice, &m_mWorld );

#endif

//	D3DXMatrixTranslation( &m_mModel, 1.0f, 1.0f, 1.0f );
	if( g_nAutoRotate )
	{
		D3DXMatrixIdentity( &m1 );
		D3DXMatrixRotationY( &m1, 0.01f );
		m_mModel = m1 * m_mModel;
	}
	
//	if( m_pASE )
//	{
//		m_pASE->Render( &m_mWorld );
//		_stprintf( g_szStr, _T("Face:%d"), g_nMaxTri );
//		m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), g_szStr );
//	}
	// m_pModel렌더링
	if( m_pModel )
	{

		// 필터링
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	
		m1 =  m_mModel;// * m_mWorld;
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, g_nFillMode );	// 솔리드/와이어/포인트 렌더링 모드

//		if( 0 )
		{
			D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.01f, 100.0f );
//			D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;
			
			//		static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
			float Time = (float)GetTickCount() / 1000.0f;
//			D3DXVECTOR4 vLightPos(150.0f * sin(Time), 90.0f, 100.0f * cos(Time), 1.0f);
//			D3DXVECTOR4 vLight = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f) - vLightPos;
//			D3DXVec4Normalize(&vLight, &vLight);
			D3DXVECTOR4 vEyePos( 0.0f, 1.0f, -5.0f, 0.0f );
//			D3DXVec4Normalize( &vLight, &vLight );
			static FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
			static FLOAT       fAmbient[] = { 0.00f, 0.00f, 0.00f, 0.00f };
			static FLOAT       fSpecular[] ={ 1.00f, 1.00f, 1.00f, 128.00f };
			static FLOAT       fLightCol[] ={ 1.00f, 1.00f, 1.00f, 1.00f };
//			mViewProj = m_mModel;
//			mViewProj *= (m_mView * m_mProj);
//			mViewProj = m_mModel * m_mView * m_mProj;
//			D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
//			D3DXMatrixTranspose( &mWorldTranspose, mWorld );
			
//			D3DXVECTOR3 vLightDir, vLightPos;
//			D3DXMatrixInverse( &mInvWorld, NULL, &m_mModel );
//			D3DXVec3TransformCoord( &vLightDir, &m_vLightDir, &mInvWorld );
//			D3DXVec3TransformCoord( &vLightPos, &m_vLightPos, &mInvWorld );
//			D3DXVec3Normalize( &vLightDir, &vLightDir );

//			D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );
			
//			m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
			//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
			m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );
			m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );
			m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );
//			m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&vLightPos,   1 );
//			m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLightDir,   1 );
			m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&fDiffuse, 1 );
			m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&fAmbient, 1 );
			m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
		}

		// 기본 텍스쳐 세팅
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		
		//------ 모델 렌더링 --------------------------
//		pd3dDevice->SetVertexShader( m_dwVertexShader );
//		bias = -0.0001f;
//		m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*) (&bias)));
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( g_Option.m_nR / 255.0f, g_Option.m_nG / 255.0f, g_Option.m_nB / 255.0f );	// 엠비언트 외부에서 세팅.
		D3DVECTOR vLight = { m_vLightDir.x, m_vLightDir.y, m_vLightDir.z };		
		SetLightVec( vLight );		// 라이트 벡터 외부에서세팅
		::SetTransformView( m_mView );
		::SetTransformProj( m_mProj );
		

//		((CModelObject*)m_pModel)->SetEffect( 0, XE_REFLECT );
//		{
//			D3DXMATRIX m;
//			D3DXMatrixTranslation( &m, -2.0f, -1.0, 0 );
//			m_pModel->Render( m_pd3dDevice, &m );
//		}
//#ifdef __SHADOW2
//		ResetStateShadowMap( pd3dDevice );
//#endif

		if(!m_bUseCLOD)
		{
			m_pModel->SetGroup( g_nLOD );
		}
		m_pModel->Render( m_pd3dDevice, &m1 );

		if( g_nLightning )
		{
			D3DXVECTOR3 vEye, vF;
			D3DXMATRIX	mInv;
			D3DXMatrixInverse( &mInv, NULL, &m_mWorld );
			D3DXVec3TransformCoord( &vEye, &vEyePt, &mInv );	// 카메라 위치.
			vF = vLookatPt - vEye;
			D3DXVec3Normalize( &vF, &vF );		// 앞쪽 벡터
			g_Beam.Render( m_pd3dDevice, &m_mModel, vEye, vF, g_vForce1, g_vForce2, 0 );
			g_Beam.Render( m_pd3dDevice, &m_mModel, vEye, vF, g_vForce3, g_vForce4, 0 );
		}


#ifdef __SHADOW
//		D3DXMATRIX m2;
//		D3DXMatrixTranslation( &m2, -2.0f, -1.0f, 0.0f );
//		m_pModel->Render( m_pd3dDevice, &m2 );
#endif

		// 그림자 텍스쳐 행렬 생성.
		if( m_d3dCaps.MaxSimultaneousTextures > 2 )
		{
#ifdef __SHADOW2
			SetStateShadowMap( pd3dDevice, 2, m_mView );
#endif // SHADOW2		
			
#if defined(__SHADOW) || defined(__SHADOW2)		
			{
				D3DXMATRIX m;
				D3DXMatrixIdentity( &m );
				m_pPlane->Render( m_pd3dDevice, &m );
			}
#endif
#ifdef __SHADOW2
			ResetStateShadowMap( pd3dDevice, 2 );
#endif
		} else
		{	
#if defined(__SHADOW) || defined(__SHADOW2)		
			// 텍스쳐 스테이지가 모자라는 경우는...
			pd3dDevice->SetTexture( 1, NULL );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			pd3dDevice->SetTexture( 2, NULL );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			D3DXMATRIX m;
			D3DXMatrixIdentity( &m );
			m_pPlane->Render( m_pd3dDevice, &m );		// 일단 정상으로 렌더링을 한번 하고.

//			bias = -0.00001f;
//			m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*) (&bias)));
/*			
			m_pPlane->m_nNoEffect = 1;
			m_pPlane->m_nNoTexture = 1;
#ifdef __SHADOW2
			SetStateShadowMap( pd3dDevice, 0, m_mView );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
#endif // SHADOW2		
			m_pPlane->Render( m_pd3dDevice, &m );		// 위에다 한겹 덧칠한다.
#ifdef __SHADOW2
			ResetStateShadowMap( pd3dDevice, 0 );
#endif
			m_pPlane->m_nNoEffect = 0;
			m_pPlane->m_nNoTexture = 0;
*/
#endif
		}
			
		
		if( g_nEdgedFace )
		{
//			m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 1000000);
			bias = -0.0001f;
			m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*) (&bias)));
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_pModel->Render( m_pd3dDevice, &m1 );
			m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
//			m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		}
		
		// 그림자 렌더
#ifdef __SHADOW
		if( m_pModel->GetType() != MODELTYPE_ASE )
		{
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->SetStateShadow( m_pd3dDevice );
			mViewProj = m_mView * m_mProj;
			SetLightPos( m_vLightPos );		// 라이트 벡터 외부에서세팅
			SetTransformViewProj( mViewProj );
			pModel->RenderShadow( m_pd3dDevice, &m1/*, m_vLightPos, m_vLightDir*/ );
			RenderShadowMask( m_pd3dDevice );	// Shadow mask render
			pModel->ResetStateShadow( m_pd3dDevice );
		}
#endif
		
		// 선택된 버텍스 그림 - Model->Render직후에 해야한다 SetTransform을 그대로 쓰기 위해서다.
		if( g_SelObj.GetType() == ST_VERTEX )		
		{
			D3DXVECTOR3	v = g_SelObj.m_vList[0];		// 오브젝트단위 좌표

			m1 =  m_mModel;// * m_mWorld;
			pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );
			float	fHalf = 0.01f;
			g_Grp3D.RenderAABB( v + D3DXVECTOR3(-fHalf, -fHalf, -fHalf), 
								v + D3DXVECTOR3( fHalf,  fHalf,  fHalf), D3DCOLOR_ARGB(255,255,0,0) );
			g_Grp3D.RenderAABB( v + D3DXVECTOR3( -0.25, -0.25, -0.25), 
								v + D3DXVECTOR3( +0.25,  0.25,  0.25), D3DCOLOR_ARGB(255,255,255,255) );

//			_stprintf( g_szStr, _T("Select Vertex num : %d"), g_SelObj.GetIdx() );
//			m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), g_szStr );
		}
		if( g_SelObj.GetType() == ST_TRI )
		{
			// draw select face;
			m1 =  m_mModel;// * m_mWorld;
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			g_Grp3D.Render3DTri( g_SelObj.m_vList, 0xffff0000 );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}

		// 검광포인트가 지정되어 있으면 그것을 표시함
//		if( m_pModel && m_pModel->GetType() != MODELTYPE_ASE )
		{
//			CModelObject *pModel = (CModelObject *)m_pModel;
//			CObject3D	*pObject = pModel->GetObject3D();
			m1 =  m_mModel;// * m_mWorld;
			pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );
			D3DVIEWPORT9 viewPort;
			m_pd3dDevice->GetViewport( &viewPort );
//			viewPort.X = viewPort.Y = 0;
//			viewPort.Width = m_nWidth;		viewPort.Height = m_nHeight;
//			viewPort.MinZ = 0.0f;			viewPort.MaxZ = 1.0f;
			if( g_vForce1.x )
			{
				D3DXVECTOR3	vs;
//				D3DXVec3TransformCoord( &v, &g_vForce1, &pObject->GetGMOBJECT()->m_mLocalTM );	// 버텍스에 LocalTM적용시켜 월드로 변환
				D3DXVec3Project( &vs, &g_vForce1, &viewPort, &m_mProj, &m_mView, &m_mModel );	

				int x, y;
				x = (int)vs.x;	y = (int)vs.y;
				g_Grp2D.DrawPoint( x,   y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x-1, y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x+1, y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x,   y-1, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x,   y+1, 0, COL_BLUE );
				m_pFont->DrawText( (float)(x + 8), (float)y, COL_WHITE, "1" );
			}
			if( g_vForce2.x )
			{
				D3DXVECTOR3	vs;
//				D3DXVec3TransformCoord( &v, &pModel->m_vForce2, &pObject->GetGMOBJECT()->m_mLocalTM );	// 버텍스에 LocalTM적용시켜 월드로 변환
				D3DXVec3Project( &vs, &g_vForce2, &viewPort, &m_mProj, &m_mView, &m_mModel );

				int x, y;
				x = (int)vs.x;	y = (int)vs.y;
				g_Grp2D.DrawPoint( x,   y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x-1, y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x+1, y, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x,   y-1, 0, COL_BLUE );
				g_Grp2D.DrawPoint( x,   y+1, 0, COL_BLUE );
				m_pFont->DrawText( (float)(x + 8), (float)y, COL_WHITE, "2" );
			}
			if( g_vForce3.x )
			{
				D3DXVECTOR3	vs;
				D3DXVec3Project( &vs, &g_vForce3, &viewPort, &m_mProj, &m_mView, &m_mModel );	
				
				int x, y;
				x = (int)vs.x;	y = (int)vs.y;
				g_Grp2D.DrawPoint( x,   y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x-1, y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x+1, y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x,   y-1, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x,   y+1, 0, COL_GREEN );
				m_pFont->DrawText( (float)(x + 8), (float)y, COL_WHITE, "3" );
			}
			if( g_vForce4.x )
			{
				D3DXVECTOR3	vs;
				D3DXVec3Project( &vs, &g_vForce4, &viewPort, &m_mProj, &m_mView, &m_mModel );
				
				int x, y;
				x = (int)vs.x;	y = (int)vs.y;
				g_Grp2D.DrawPoint( x,   y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x-1, y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x+1, y, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x,   y-1, 0, COL_GREEN );
				g_Grp2D.DrawPoint( x,   y+1, 0, COL_GREEN );
				m_pFont->DrawText( (float)(x + 8), (float)y, COL_WHITE, "4" );
			}
		}


		// 모델 바운딩 박스 그림.
		if( g_nBoundBox != VIEWBB_ALL )
		{
			m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// 바운딩박스 그릴땐 라이팅 꺼야 한다.
			m_pd3dDevice->SetMaterial( &mtrl );
			m_pd3dDevice->SetTexture( 0, NULL );
			m1 =  m_mModel;// * m_mWorld;
			m_pModel->RenderBB( m_pd3dDevice, &m1 );
		}

		_stprintf( g_szStr, _T("Face:%d %f"), g_nMaxTri, m_pModel->m_fFrameCurrent );
		m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), g_szStr );
		_stprintf( g_szStr, _T("%f"), m_pModel->m_fPerSlerp );
		m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), g_szStr );
		if( g_SelObj.GetType() == ST_VERTEX )		
		{
			D3DXVECTOR3	v = g_SelObj.m_vList[0];		// 오브젝트단위 좌표
			
			m1 =  m_mModel;// * m_mWorld;
			pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );
			_stprintf( g_szStr, _T("%f %f %f"), v.x, v.y, v.z );
			m_pFont->DrawText( 2, 70, D3DCOLOR_ARGB(255,255,255,0), g_szStr );
		}
	} // m_pModel
	
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	

    m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
    m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
	static const char szTextureOp[MAX_TEXTURE_OP][32] =
	{
		"Modulate",
		"Modulate2x",
		"Modulate4x",
		"Add",
		"AddSigned",
		"AddSigned2x",
		"Subtract",
		"AddSmooth",
		"MultiplyAdd",
		"Lerp"
	};
    m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,0), szTextureOp[ g_nSelectTextureOp ] );
	
//	TCHAR* pstr;
//	pstr = _T("Unsupported color op");

    // End the scene.
    m_pd3dDevice->EndScene();

//	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);
//	m_wndTrack.Invalidate(0);
    return S_OK;
}
//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CColaView::InitDeviceObjects()
{
    // Initialize the font's internal textures
	g_Grp2D.SetDevice( m_pd3dDevice );
	g_Grp3D.SetDevice( m_pd3dDevice );
    m_pFont->InitDeviceObjects( m_pd3dDevice );
//	g_TextureMng.SetD3DDevice( m_pd3dDevice );

//	if( m_pASE )	m_pASE->InitDeviceObjects( m_pd3dDevice );
	if( m_pModel )	m_pModel->InitDeviceObjects( m_pd3dDevice );
//	CBipedMesh *pBip;
//	m_pModel = new CBipedMesh;
//	pBip = (CBipedMesh *)m_pModel;
//	pBip->InitDeviceObjects( m_pd3dDevice );
//	pBip->OpenBone( "mvr_buff.bon" );			// load bone
//	pBip->OpenMesh( "mvr_buff.o3d" );			// load skin
//	pBip->OpenMotion( "mvr_buff_attack.ani" );	// load motion
//    m_ArcBall.SetRightHanded( TRUE );
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CColaView::RestoreDeviceObjects()
{
	HRESULT hr;

    m_pFont->RestoreDeviceObjects();
//	if( m_pASE )	m_pASE->RestoreDeviceObjects();
	if( m_pModel )	m_pModel->RestoreDeviceObjects();

#ifdef	__XVS
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) || ( m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
#ifdef	__X2LINK
//		hr = CreateSkinningVS( m_pd3dDevice, _T("../NEUROSPACE/resource/skining2.vsh") );
		hr = CreateSkinningVS( m_pd3dDevice, _T("skining2.vsh") );
		if( FAILED(hr) )
			Error( "RestoreDeviceObjects : CreateSkinningVS 에러" );
#else
		CreateSkinningVS( m_pd3dDevice, _T("skining.vsh") );
#endif
#ifdef __SHADOW
		hr = CreateShadowVS( m_pd3dDevice, _T("Shadow.vsh") );
		if( FAILED(hr) )
			Error( "RestoreDeviceObjects : CreateShadowVS 에러" );
#endif
    } else
	{
		Error( "하드웨어가 버텍스쉐이더 1.1을 지원하지 않아 쉐이더를 읽지 못했다." );
	}
#endif

    // Create and set up the shine materials w/ textures
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set the transform matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 1.0f, -4.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	FLOAT       fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
//	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.01f, 50.0f );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.49f, 512.0f );
	
    m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0.5f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set any appropiate state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

#ifdef __SHADOW
	CreateShadowMask( m_pd3dDevice, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );
#endif
	
#if defined(__SHADOW) || defined(__SHADOW2)		
	m_pPlane = new CModelObject;
	m_pPlane->InitDeviceObjects( m_pd3dDevice );
	int ret = m_pPlane->LoadModel( "판때기.o3d" );
#endif

#ifdef __SHADOW2
	CreateShadowMap( m_pd3dDevice, m_d3dpp.BackBufferFormat );
#endif

	g_ModelGlobal.RestoreDeviceObjects( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CColaView::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();
//	if( m_pASE )	m_pASE->InvalidateDeviceObjects();
	if( m_pModel )	m_pModel->InvalidateDeviceObjects();

	DeleteVertexShader( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CColaView::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();
//	if( m_pASE )	m_pASE->DeleteDeviceObjects();
	if( m_pModel )	m_pModel->DeleteDeviceObjects();
//	SAFE_RELEASE( m_pVBWalls );
	SAFE_DELETE( m_pModel );
    SAFE_DELETE( m_pFont );
	SAFE_RELEASE( g_pReflectMap	);
	SAFE_RELEASE( g_pReflectMap2 );
	SAFE_RELEASE( g_pReflectMap3 );

	g_Object3DMng.Destroy();
	g_TextureMng.DeleteDeviceObjects();

	DeleteShadowMask();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CColaView::FinalCleanup()
{
    SAFE_DELETE( m_pFont );
//	SAFE_DELETE( m_pASE );
	SAFE_DELETE( m_pModel );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
//HRESULT CColaView::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format )
HRESULT CColaView::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

    // Check that the device supports at least one of the two techniques
    // used in this sample: either a vertex shader, or at least two blend
    // matrices and a directional light.

	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		g_bUsableVS = FALSE;
		if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
			return E_FAIL;
	}
			

/*
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1) )
            return S_OK;
*/

    // Check that the device can blend vertices with at least two matrices
    // (Software can always do up to 4 blend matrices)
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
		if( pCaps->MaxVertexBlendMatrices < 2 )
			return E_FAIL;
		//    if( pCaps->MaxVertexBlendMatrices < 2 )
//        return E_FAIL;

    // If this is a TnL device, make sure it supports directional lights
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
            return E_FAIL;
    }
    return S_OK;
}


void CColaView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_nWidth = cx;
	m_nHeight = cy;
	if( m_pd3dDevice == NULL )		return;
	D3DXMATRIX	matProj;
//	FLOAT       fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
	FLOAT       fAspect = ((FLOAT)cx) / cy;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.49f, 512.0f );
//	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 5000.0f );

    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_d3dpp.BackBufferWidth  = cx;
	m_d3dpp.BackBufferHeight = cy;

//	Resize3DEnvironment();
	
}


void CColaView::OnAseLoad() 
{
	// TODO: Add your command handler code here

	// 이미 로딩된게 있다면 삭제하고 다시 읽음
	CFileDialog		dlgFileOpen( true, "ase", NULL, OFN_FILEMUSTEXIST,
								"ASE Files(*.ase)|*.ase|AllFiles(*.*)|*.*||", NULL );

//	dlgFileOpen.m_ofn.lpstrInitialDir = (LPCSTR)(FULLDIR_ASE);

	if ( dlgFileOpen.DoModal() == IDOK )
	{
//		LoadASE( dlgFileOpen.GetFileName() );
		LoadModel( dlgFileOpen.GetFileName() );
	}	
		
}

void CColaView::LoadASE( LPCTSTR szFileName )
{
//	if( m_pASE )	SAFE_DELETE( m_pASE );	// 이미 로딩되어 있으면 날림
	if( m_pModel )		SAFE_DELETE( m_pModel );

	g_vForce1.x = g_vForce1.y = g_vForce1.z = 0;
	g_vForce2.x = g_vForce2.y = g_vForce2.z = 0;
	g_vForce3.x = g_vForce3.y = g_vForce3.z = 0;
	g_vForce4.x = g_vForce4.y = g_vForce4.z = 0;

	m_pModel = new CAse;
	m_pModel->InitDeviceObjects( m_pd3dDevice );
	m_pModel->LoadModel( szFileName );
	m_pModel->RestoreDeviceObjects();
	g_strModel = szFileName;	// ASE파일명 받아둠

	m_ArcBall.SetRadius( m_pModel->GetRadius() );		// 읽은 모델의 반지름을 구해서 ArcBall에 셋
//	m_ArcBall.SetRadius( 2 );		// 읽은 모델의 반지름을 구해서 ArcBall에 셋
}

//
//	ASE/O3D파일명을 넘기면 확장자를 판단해서 로딩함
//
void CColaView::LoadModel( LPCTSTR szFileName )
{
	CString	strFileName = szFileName;
	if( strFileName.IsEmpty() == FALSE )
	{
		CString strName = GetFileName( szFileName );
		if( strName.Find( " " ) != -1 )
			Message( "경고 : %s 파일명 내에 공백이 있다.", strFileName );
		

		CString	strExt = strFileName.Right(3);
		strExt.MakeLower();
		if( strExt == "ase" )
			LoadASE( strFileName );
		if( strExt == "o3d" )
			LoadO3D( strFileName );
		if( strExt == "ani" )
		{
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->LoadMotion( strFileName );
		}
		if( strExt == "chr" )
		{
			AfxMessageBox( "외장본을 사용하는 o3d는 chr파일을 로딩할필요없이 곧바로 o3d파일을 드래그하면 자동으로 본과 애니를 로드합니다" );

			if( m_pModel )	SAFE_DELETE( m_pModel );
			m_pModel = new CModelObject;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->LoadBone( strFileName );
		}
//		CString	strTitle = GetFileTitle(strFileName);
		CString strTitle = strFileName;
#ifdef __VIEWER_ONLY
		strTitle += " - AeonSoft Viewer ";
#else
		strTitle += " - AeonSoft Cola ";
#endif
		strTitle += g_strVersion;
		g_pMainFrm->SetWindowText( strTitle );

		if( m_pModel )
		{
			CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
			pMainFrm->m_dlgTrack.SetModel( m_pModel );
			pMainFrm->m_dlgInfo.SetModel( m_pModel );
			pMainFrm->m_dlgInfo.UpdateInfo();
			
			static char buff[256*64];
			memset( buff, 0, sizeof(buff) );
			if( m_pModel->GetBonesName( buff ) )		// m_pModel에 본이 있을때 본들의 이름을 버퍼에 담아준다.
			{
				pMainFrm->m_dlgInfo.UpdateInfo2( buff );
			}
		}
	}
}

// Bone 저장
void CColaView::OnBonSaveAs() 
{
	// TODO: Add your command handler code here
	CFileDialog		dlgFileOpen( false, "bon", NULL, OFN_FILEMUSTEXIST,
								"Bone Files(*.bon)|*.bon|AllFiles(*.*)|*.*||", NULL );

	// ASE모델만 본 세이브가 되게 한다.
	if( m_pModel->GetType() != MODELTYPE_ASE )	return;

	if ( dlgFileOpen.DoModal() == IDOK )
	{
		CAse *pASE = (CAse *)m_pModel;
		CGeoMObject	*pBiped = pASE->GetBiped();
		pASE->SaveCHR( dlgFileOpen.GetFileName() );
		Message( "바이페드 %s을 저장함. 관절수 %d개", pBiped->m_strName, pASE->m_nMaxBoneNode );
	}
	
}

//
// Load Ani
//
void CColaView::OnAniLoad() 
{
	// TODO: Add your command handler code here
	// 이미 로딩된게 있다면 삭제하고 다시 읽음
	CFileDialog		dlgFileOpen( true, "ani", NULL, OFN_FILEMUSTEXIST,
								"ANI Files(*.ani)|*.ani|AllFiles(*.*)|*.*||", NULL );

//	dlgFileOpen.m_ofn.lpstrInitialDir = (LPCSTR)(FULLDIR_ASE);

	if ( dlgFileOpen.DoModal() == IDOK )
	{
//		CModelObject *pModel = (CModelObject *)m_pModel;
//		pModel->LoadMotion( dlgFileOpen.GetFileName() );
		LoadModel( dlgFileOpen.GetFileName() );
	}	
	
}

//
//	Save Ani
//
void CColaView::OnAniSave() 
{
	// TODO: Add your command handler code here
	char	szName[256];

	// ASE모델만 본 세이브가 되게 한다.
	if( m_pModel->GetType() != MODELTYPE_ASE )	return;
	CAse *pASE = (CAse *)m_pModel;

	CGeoMObject	*pBiped = pASE->GetBiped();

	strcpy( szName, GetFilePath(g_strModel) );
	strcat( szName, GetFileTitle(g_strModel) );		// test.ase -> test
	strcat( szName, ".ani" );						// test -> test.ani
	pASE->SaveANI( szName );
}



void CColaView::OnUpdateBonSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	if( m_pModel && m_pModel->GetType() == MODELTYPE_ASE )
//		pCmdUI->Enable( true );
//	else
		pCmdUI->Enable( false );
}

void CColaView::OnUpdateAniSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	if( m_pModel && m_pModel->GetType() == MODELTYPE_ASE )
//		pCmdUI->Enable( true );
//	else
		pCmdUI->Enable( false );
	
}


//------------------------------------------------------------
// 마우스 버튼 이벤트 핸들러
//
//-----------------------------------------------------------
void CColaView::OnLButtonDown(UINT nFlags, CPoint point) 
{
/*
	{
		D3DXVECTOR3 v1, v2;
		//		v1.x = 5.0f - xRandomF( 10 );		v1.y = 5.0f - xRandomF( 10 );		v1.z = xRandomF( 10 );
		//		v2.x = 5.0f - xRandomF( 10 );		v2.y = 5.0f - xRandomF( 10 );		v2.z = xRandomF( 10 );
		v1.x = 1.0f - xRandomF( 2 );		v1.y = 1.0f - xRandomF( 2 );		v1.z = xRandomF( 2 );
		v2.x = 1.0f - xRandomF( 2 );		v2.y = 1.0f - xRandomF( 2 );		v2.z = xRandomF( 2 );
		v1 /= 2.0f;
		g_Beam.Create( v1 );
	}
*/

	// TODO: Add your message handler code here and/or call default
//	switch( g_EditMode )
//	{
//	case EM_SELECT:
	s_nEmitCnt = 6;


		if( m_pModel )
		{
			D3DXVECTOR3	vRayOrig, vRayDir;
			CModel *pModel = m_pModel;

			// mouse pos -> ray
			CalcRay( &vRayOrig, &vRayDir, m_nWidth, m_nHeight, point.x, point.y, m_mProj, m_mView, &m_mModel );
			m_vRayOrig = vRayOrig;		m_vRayDir = vRayDir;

			D3DXVECTOR3 vSelect;
			if( pModel->IntersectRayVertex( &vSelect, vRayOrig, vRayDir ) )
			{
				g_SelObj.SetType( ST_VERTEX );
				g_SelObj.SetSelect( &vSelect );		// 셀렉트한버텍스의 오브젝트좌표를 저장
			} else
				g_SelObj.Clear();

 // 페이스 셀렉트
			D3DXVECTOR3 vIntersect;
			FLOAT	fDist;
//			D3DXVECTOR3 *pTri = ((CModelObject *)m_pModel)->IntersectRayTri( vRayOrig, vRayDir, m_mModel, &vIntersect, &fDist );
			D3DXVECTOR3 *pTri = m_pModel->IntersectRayTri( vRayOrig, vRayDir, m_mModel, &vIntersect, &fDist );
			if( pTri )
			{
				g_SelObj.SetType( ST_TRI );
				g_SelObj.m_vList[0] = pTri[0];	
				g_SelObj.m_vList[1] = pTri[1];	
				g_SelObj.m_vList[2] = pTri[2];
			}
		

/*
	if( m_pModel && m_pModel->GetType() != MODELTYPE_ASE )		// 임시로 이렇게 해놨다. 나중에 다 추상화 시키자
	{
		D3DXVECTOR3	vRayOrig, vRayDir;
		CModelObject *pModel = (CModelObject *)m_pModel;

		// mouse pos -> ray
		CalcRay( &vRayOrig, &vRayDir, m_nWidth, m_nHeight, point.x, point.y, m_mProj, m_mView, &m_mWorld );

		D3DXVECTOR3 vIntersect;
		FLOAT	fDist;
		D3DXVECTOR3 *pTri = pModel->IntersectRayTri( vRayOrig, vRayDir, m_mWorld, &vIntersect, &fDist );
		if( pTri )
		{
			g_SelObj.SetType( ST_TRI );
			g_SelObj.m_vList[0] = pTri[0];	
			g_SelObj.m_vList[1] = pTri[1];	
			g_SelObj.m_vList[2] = pTri[2];
		}
	}
*/
/*
			D3DXVECTOR3 vIntersect;
			FLOAT	fDist;
//			D3DXMATRIX	mWorld;
//			D3DXMatrixIdentity( &mWorld );		// 툴에선 오브젝트가 항상 중점이다.
			CUSTOMVERTEX *pVB;
			pVB = m_pModel->FindTouchFaceRay( vRayOrig, vRayDir, m_mModel, &vIntersect, &fDist );
			if( pVB )
			{
				g_SelObj.m_vTri[0] = pVB[0].position;
				g_SelObj.m_vTri[1] = pVB[1].position;
				g_SelObj.m_vTri[2] = pVB[2].position;
			} else
			{
				g_SelObj.Clear();
			}
*/
		}
//		break;
//	case EM_ROTATE:
//		m_ArcBall.OnLButtonDown( nFlags, point );
//		break;
//	}

	
	CView::OnLButtonDown(nFlags, point);
}

void CColaView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	switch( g_EditMode )
	{
	case EM_SELECT:
		break;
//	case EM_ROTATE:
//		m_ArcBall.OnLButtonUp( nFlags, point );
		break;
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void CColaView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	switch( g_EditMode )
	{
	case EM_SELECT:
		m_ArcBall.MouseMovePanZoom( nFlags, point );
		break;
	case EM_ROTATE:
		m_ArcBall.MouseMovePanZoom( nFlags, point );
//		m_ArcBall.MouseMoveRot( nFlags, point );
		if( nFlags & MK_RBUTTON )
		{
			g_vRotate.y += ((testx - point.x) / 200.0f);
			g_vRotate.x += ((testy - point.y) / 200.0f);
		}
			
		
		
		
		
		break;
	}
	
	testx = point.x;
	testy = point.y;


	CView::OnMouseMove(nFlags, point);
	SetCursor();
}

void CColaView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	switch( g_EditMode )
//	{
//	case EM_SELECT:
//		break;
//	case EM_ROTATE:
		m_ArcBall.OnLButtonDown( nFlags, point );
//		break;
//	}
	
	CView::OnRButtonDown(nFlags, point);
}

void CColaView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	switch( g_EditMode )
	{
	case EM_SELECT:
		break;
	case EM_ROTATE:
		break;
	}
	m_ArcBall.OnRButtonDown( nFlags, point );
	
	CView::OnMButtonDown(nFlags, point);
}

BOOL CColaView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	//sunTODO:

	// TODO: Add your message handler code here and/or call default
	int		nZoom = 64;

    if( nFlags & MK_SHIFT )
		nZoom /= 8;

	if( zDelta > 0 )
		m_ArcBall.Zoom( -nZoom );
	else
		m_ArcBall.Zoom( nZoom );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

//
//	메쉬 로드
//
void CColaView::OnMesLoad() 
{
	// TODO: Add your command handler code here
	// 이미 로딩된게 있다면 삭제하고 다시 읽음
	CFileDialog		dlg( true, "o3d", NULL, OFN_FILEMUSTEXIST,
					 	"O3D Files(*.o3d)|*.o3d|AllFiles(*.*)|*.*||", NULL );

//	dlg.m_ofn.lpstrInitialDir = (LPCSTR)(FULLDIR_MODEL);

	if ( dlg.DoModal() == IDOK )
	{
//		LoadO3D( dlg.GetFileName() );
		LoadModel( dlg.GetFileName() );
	}
	
}

//
// 본이 필요한 오브젝트를 읽는경우 - mvr_buff, part_upper
// 본이 필요없는 오브젝트를 읽는경우 - obj_house, obj_풍선
//

int GetPartsNum( const TCHAR* lval )		// 파츠번호를 내마음대로 설정해서 넘기고 그 번호를 기준으로 기존 파츠 삭제하거나 한다.
{
	int nParts = -1;
	if( strstrlwr( lval, "upper." ) || strstrlwr( lval, "suit." ) )
	{
		nParts = 0;
	}
	else if ( strstrlwr( lval, "cap." ) || strstrlwr( lval, "hat." ) )
	{
		nParts = 1;
	}
	else if( strstrlwr( lval, "hand." ) || strstrlwr( lval, "gloves." ) )
	{
		nParts = 2;
	}
	else if( strstrlwr( lval, "foot." ) || strstrlwr( lval, "shoes." ) )
	{
		nParts = 3;
	}
	else if( strstrlwr( lval, "head0" ) )	//얼굴 
	{
		nParts = 4; \
	}
	else if( strstrlwr( lval, "hair0" ) ) //머리
	{
		nParts = 5;  
	}
	else 
	{
		nParts = 10;
	}


	return nParts;
}

void CColaView::LoadO3D( LPCTSTR szFileName )
{
	if( strstrlwr(szFileName, "part_" ) )		
	{
		// gmpbigsun: 로드한넘이 파츠고 
		// 현재 로드된넘이 파츠면 기존모델 유지 
		// 현재 로드된넘이 파츠가 아니면 삭제 
		 m_nNumParts = GetPartsNum( szFileName );
		 if( m_nNumParts < 0 )
		 {
			 assert( 0 );
			 m_nNumParts = 0;
		 }

		if( !strstrlwr( g_strModel, "part_" ) ) // 현재 모델이 파츠가 아님 
		{
			// 현재 모델이 파츠가 아님. 지워야 함 
			if( m_pModel )
			{
				m_pModel->InitAnimate( );		// 애니 초기화를 안하고 지우면 CDlgTrack UI에서 문제가 생긴다 
				SAFE_DELETE( m_pModel );
			}
		}
	}
	else
	{	
		//로드된 넘이 파츠가 아니므로 기존모델 무조건 삭제 
		if( m_pModel )
		{
			m_pModel->InitAnimate( );
			SAFE_DELETE( m_pModel );
		}

		m_nNumParts = 0;
	}
	
	if( m_pModel )
	{
/*
		if( && m_pModel->본 == 0 && m_pModel->모델 == 1 )
		{
			// 일반형오브젝트가 로딩되었었다.
			SAFE_DELETE( m_pModel );		// 일단 삭제하고
			m_pModel = new CModelObject;	// 일단 new.
		} else
		if( m_pModel->본 == 1 && 모델 == 1 )	// 본 + 모델이 로딩되어 있다는 뜻.
		{
			// 모델만 추가 하거나 교체.
			CModel *pTemp = new CModelObject;
			pTemp->InitDeviceObjects( m_pd3dDevice );
			if( pTemp->LoadModel( szFileName ) == SUCCESS )		// 일단 다른 포인터로 함 읽어보고 성공했다면 본이 필요없는 오브젝트였다.
			{
				SAFE_DELETE( m_pModel );		
				SAFE_DELETE( pTemp );
				m_pModel = new CModelObject;	// 재 생성시키고 다시 읽으면 된다.
			} else
			{
				SAFE_DELETE( pTemp );		// 임시로 생성시켰던거 삭제시키고 아래쪽에서 읽으면 된다.
			}
		} else
		if( m_pModel->본만 있음 )
		{
			// 모델 추가
		} else
		{
			// 아무것도 로딩되어 있지 앟음.
			// 여기까진 오지않는다. m_pModel에 뭔가가 할당되어 있다는것은 본이든 모델이든 로딩되어 있다는 뜻.
		}
*/
	} else
	{
		// 아무것도 로딩한적 없음.
		m_pModel = new CModelObject;		// 일단 new.   
		// 일반형 오브젝트를 읽었다면 그냥 읽어짐
		// 스킨형 오브젝트를 읽었다면 에러메시지후 new했던거 삭제.
	}

//	if( m_pModel )	SAFE_DELETE( m_pModel );
		
//	m_pModel = new CModelObject;
	CModelObject *pModel = (CModelObject *)m_pModel;

	g_vForce1.x = g_vForce1.y = g_vForce1.z = 0;
	g_vForce2.x = g_vForce2.y = g_vForce2.z = 0;	
	g_vForce3.x = g_vForce3.y = g_vForce3.z = 0;
	g_vForce4.x = g_vForce4.y = g_vForce4.z = 0;

	pModel->InitDeviceObjects( m_pd3dDevice );
	g_strModelPath = GetFilePath( szFileName );

// 	if( m_nNumParts > 0 && MessageBox( "추가로 로딩하겠습니까?", "알림", MB_YESNO ) == IDYES )			//sun 100108 : 추가로딩부 자동화함 
// 	{
// 		if( m_nNumParts >= MAX_ELEMENT )
// 		{
// 			Error( "더 이상 추가할 수 없습니다." );
// 			m_nNumParts = 0;
// 		}
// 	} else
// 	{
// 		m_nNumParts = 0;
// 	}

	if( pModel->LoadElement( szFileName, m_nNumParts ) == FAIL )
	{
		SAFE_DELETE( m_pModel );	// 만약 본을 안읽고 스킨.o3d를 읽었다면 삭제시키고 리턴.
		return;
	}
//	m_nNumParts ++;
	g_strModel = szFileName;	// 파일명 받아둠

	if( pModel )
	{
//		m_ArcBall.SetRadius( pModel->GetRadius() );		// 읽은 모델의 반지름을 구해서 ArcBall에 셋
	}
}

//
//	메쉬 세이브
//  ASE 바이페드일땐 스킨저장
//      일반일땐 노말메쉬 저장
//  O3D 일땐 SaveMesh
//
void CColaView::OnMesSave() 
{
	// TODO: Add your command handler code here
/*
	if( m_pModel == NULL )			return;
	CModelObject *pModel = (CModelObject *)m_pModel;

	pModel->m_pMesh->m_dKPS = pModel->m_fPerSlerp;		// 조정된 모델의 속도값을 넣고 저장함

	if( g_strModel.IsEmpty() )
		OnMesSaveas();								// 읽은게 없었다면 다른이름으로 저장
	else
		pModel->m_pMesh->SaveMesh( g_strModel );	// 이미 읽었었다면 그이름으로 저장
*/
/*
	if( m_pASE )
	{
		CString strName = GetFileTitle( g_strASE );
		strName += ".o3d";
		if( m_pASE->IsBiped() )
			m_pASE->SaveSkin( strName );		// 바이페드일땐 스킨으로 저장
		else
			m_pASE->SaveNormMesh( strName );	// 일반일땐 일반저장.
	} else
	if( m_pModel )
	{
		CModelObject *pModel = (CModelObject *)m_pModel;

		pModel->m_pMesh->m_dKPS = pModel->m_fPerSlerp;		// 조정된 모델의 속도값을 넣고 저장함
		if( g_strModel.IsEmpty() )
			OnMesSaveas();								// 읽은게 없었다면 다른이름으로 저장
		else
			pModel->m_pMesh->SaveMesh( g_strModel );	// 이미 읽었었다면 그이름으로 저장
	}
*/
	if( m_pModel )
	{
		CString strName;
		strName = GetFilePath(g_strModel);
		strName += GetFileTitle( g_strModel );
		strName += ".o3d";
		if( g_strModel.IsEmpty() )
		{
			OnMesSaveas();
		}
		else
		{
			m_pModel->SaveModel( strName );
		}
	}
}

//
//
//
void CColaView::OnMesSaveas() 
{
	// TODO: Add your command handler code here
	CFileDialog		dlg( false, "o3d", NULL, OFN_FILEMUSTEXIST,
					 	"O3D Files(*.o3d)|*.o3d|AllFiles(*.*)|*.*||", NULL );

	if( m_pModel == NULL )	return;

	if ( dlg.DoModal() == IDOK )
	{
		CString	strName = dlg.GetFileName();
		m_pModel->SaveModel( strName );
/*
		if( m_pASE )
		{
			if( m_pASE->IsBiped() )
				m_pASE->SaveSkin( strName );
			else
				m_pASE->SaveNormMesh( strName );
		} else
		if( m_pModel )
		{
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->m_pMesh->SaveMesh( strName );
		}
*/
	}
	
}

//
//	ASE이름 + mes + n 으로 저장
//
void CColaView::OnMesSaveFilename() 
{
	// TODO: Add your command handler code here
	char	szName[256];
	if( m_pModel == NULL )	return;

	strcpy( szName, GetFileTitle(g_strModel) );		// test.ase -> test
	strcat( szName, ".o3d" );						// test -> test.o3d
	
	// mes저장.  바이페드가 있을때와 없을때 따로 처리
	m_pModel->SaveModel( szName );
/*	if( m_pASE->m_bBiped )
		m_pASE->SaveBipedMeshSeq( szName );
	else
		m_pASE->SaveNormMesh( szName );*/

}



void CColaView::OnArcrotate() 
{
	// TODO: Add your command handler code here
//	g_EditMode = EM_ROTATE;	
//	SetCursor( CURSOR_CROSS );
}

void CColaView::OnSelect() 
{
	// TODO: Add your command handler code here
//	g_EditMode = EM_SELECT;	
	SetCursor( CURSOR_NORMAL );
}

void CColaView::OnUpdateArcrotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( false );
//	if ( g_EditMode == EM_ROTATE )
//		pCmdUI->SetCheck( true );
//	else					
//		pCmdUI->SetCheck( false );
}

void CColaView::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( true );
//	if ( g_EditMode == EM_SELECT )
//		pCmdUI->SetCheck( true );
//	else					
//		pCmdUI->SetCheck( false );
	
}

void CColaView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_F5 )
	{
		D3DXMatrixIdentity( &m_mWorld );
		D3DXMatrixIdentity( &m_mModel );
		m_ArcBall.Init();
		g_vRotate.x = g_vRotate.y = g_vRotate.z = 0;
	}
	switch( nChar )
	{
		case VK_RETURN: m_bUseCLOD = !m_bUseCLOD; break;
	}

	//__BS_TOPVIEW
	if( g_bTopView && nChar == 'R' )
	{
		g_vRotate.y += ( D3DX_PI / 2 );
	}

#ifdef	__STUDY
/*	switch( nChar )
	{
	case VK_LEFT:	g_Line.vStart.x -= 0.1f;	break;
	case VK_RIGHT:	g_Line.vStart.x += 0.1f;	break;
	case VK_UP:		g_Line.vStart.y += 0.1f;	break;
	case VK_DOWN:	g_Line.vStart.y -= 0.1f;	break;
	case 'Q':		g_Line.vStart.z += 0.1f;	break;
	case 'A':		g_Line.vStart.z -= 0.1f;	break;
	}
*/
	switch( nChar )
	{
	case VK_LEFT:	g_Line.vEnd.x -= 0.01f;	break;
	case VK_RIGHT:	g_Line.vEnd.x += 0.01f;	break;
	case VK_UP:		g_Line.vEnd.y += 0.01f;	break;
	case VK_DOWN:	g_Line.vEnd.y -= 0.01f;	break;
	case 'Q':		g_Line.vEnd.z += 0.01f;	break;
	case 'A':		g_Line.vEnd.z -= 0.01f;	break;
	
	}

	
/*
	switch( nChar )
	{
	case VK_LEFT:	m_vRayOrig.x -= 0.01f;	break;
	case VK_RIGHT:	m_vRayOrig.x += 0.01f;	break;
	case VK_UP:		m_vRayOrig.y += 0.01f;	break;
	case VK_DOWN:	m_vRayOrig.y -= 0.01f;	break;
	case 'Q':		m_vRayOrig.z += 0.01f;	break;
	case 'A':		m_vRayOrig.z -= 0.01f;	break;
	}
*/
#endif
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//
// 검광 지점 지정
//
void CColaView::OnSwdforce1() 
{
	// TODO: Add your command handler code here
	g_vForce1 = D3DXVECTOR3(0, 0, 0);
	if( g_SelObj.GetType() == ST_VERTEX )
		g_vForce1 = *((D3DXVECTOR3 *)g_SelObj.GetSelect());
	else
		Message( "선택된 버텍스가 없슴" );
}

void CColaView::OnSwdforce2() 
{
	// TODO: Add your command handler code here
	g_vForce2 = D3DXVECTOR3(0, 0, 0);
	if( g_SelObj.GetType() == ST_VERTEX )
		g_vForce2 = *((D3DXVECTOR3 *)g_SelObj.GetSelect());
	else
		Message( "선택된 버텍스가 없슴" );
}

void CColaView::OnUpdateSwdforce1(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( true );
}

void CColaView::OnUpdateSwdforce2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true );
}

//
// 추가 검광 지점 지정
//
void CColaView::OnSwdforce3() 
{
	// TODO: Add your command handler code here
	g_vForce3 = D3DXVECTOR3(0, 0, 0);
	if( g_SelObj.GetType() == ST_VERTEX )
		g_vForce3 = *((D3DXVECTOR3 *)g_SelObj.GetSelect());
	else
		Message( "선택된 버텍스가 없슴" );
}

void CColaView::OnSwdforce4() 
{
	// TODO: Add your command handler code here
	g_vForce4 = D3DXVECTOR3(0, 0, 0);
	if( g_SelObj.GetType() == ST_VERTEX )
		g_vForce4 = *((D3DXVECTOR3 *)g_SelObj.GetSelect());
	else
		Message( "선택된 버텍스가 없슴" );
}

void CColaView::OnUpdateSwdforce3(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( true );
}

void CColaView::OnUpdateSwdforce4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true );
}

void CColaView::OnSpeedup() 
{
	// TODO: Add your command handler code here
	if( m_pModel )
	{
		m_pModel->m_fPerSlerp += 0.005f;
	}
		
}

void CColaView::OnSpeeddown() 
{
	// TODO: Add your command handler code here
	if( m_pModel )
	{
		m_pModel->m_fPerSlerp -= 0.005f;
	}
	
}

void CColaView::OnSpeedclr() 
{
	// TODO: Add your command handler code here
	if( m_pModel )
	{
		m_pModel->m_fPerSlerp = 0.5f;
	}
	
}

void CColaView::OnOption() 
{
	// TODO: Add your command handler code here
	CDlgOption	dlg;

	dlg.m_fScale = g_Option.m_fScale;
	dlg.m_nR = g_Option.m_nR;
	dlg.m_nG = g_Option.m_nG;
	dlg.m_nB = g_Option.m_nB;
	dlg.m_strTexture = g_strPathTexture;
	dlg.m_strResource = g_Option.m_strResource;

	if( dlg.DoModal() == IDOK )
	{
		g_Option.m_fScale = dlg.m_fScale;
		g_Option.m_nR = dlg.m_nR;
		g_Option.m_nG = dlg.m_nG;
		g_Option.m_nB = dlg.m_nB;
		g_Option.m_strResource = dlg.m_strResource;
		g_strPathTexture = dlg.m_strTexture;

		extern int SaveINI( LPCTSTR szFileName );
		SaveINI( "Cola.ini" );
	}
}

void CColaView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char	szFileName[256];

	DragQueryFile( hDropInfo ,0 ,  szFileName ,  256 );

	LoadModel( szFileName );

	CView::OnDropFiles(hDropInfo);
}

void CColaView::OnUpdateViewFnormal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_nShowFNormal )
		pCmdUI->SetCheck( true );
	else
		pCmdUI->SetCheck( false );
	
}

void CColaView::OnUpdateViewVnormal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_nShowVNormal )
		pCmdUI->SetCheck( true );
	else
		pCmdUI->SetCheck( false );
	
}

void CColaView::OnViewFnormal() 
{
	// TODO: Add your command handler code here
	m_nShowFNormal = !m_nShowFNormal;
}


void CColaView::OnViewVnormal() 
{
	// TODO: Add your command handler code here
	m_nShowVNormal = !m_nShowVNormal;
}

void CColaView::OnNormalClear() 
{
	// TODO: Add your command handler code here
	if( AfxMessageBox( "정말로?", MB_YESNO ) == IDYES )
	{
		m_pModel->ClearNormal();
		m_pModel->m_bNormalClear = 1;
	}
}



void CColaView::OnFrameskip() 
{
	// TODO: Add your command handler code here
	BOOL bSkip = GetFrameSkip();
	SetFrameSkip( !bSkip );
		
}

void CColaView::OnUpdateFrameskip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( GetFrameSkip() )
		pCmdUI->SetCheck( TRUE );
	else
		pCmdUI->SetCheck( FALSE );
}

void CColaView::OnConvert() 
{
	// TODO: Add your command handler code here
	CDlgConvert		dlg;
	
	dlg.DoModal();
}

void CColaView::OnTexture() 
{
	// TODO: Add your command handler code here
	CDlgTexture		dlg;

	if( m_pModel )
	{
		dlg.m_fScrlU = m_pModel->m_fScrlU;
		dlg.m_fScrlV = m_pModel->m_fScrlV;
	}
	
	if( dlg.DoModal() == IDOK )
	{
		if( m_pModel )
		{
			m_pModel->m_fScrlU = dlg.m_fScrlU;
			m_pModel->m_fScrlV = dlg.m_fScrlV;
		}
		
	}
}

void CColaView::OnUpdateTexture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
	
}

void CColaView::OnUpdateMesSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( false );
//	if( m_pModel )
//		pCmdUI->Enable( true );
//	else
//		pCmdUI->Enable( false );
		
}

void CColaView::OnUpdateMesSaveas(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	BOOL	bBiped = FALSE;
//	if( m_pModel && m_pModel->GetType() == MODELTYPE_ASE )
//	{
//		if( ((CAse*)m_pModel)->IsBiped() )
//			bBiped = TRUE;
//	}
//	if( m_pModel && bBiped )		// ase biped파일이면 save as는 없다.
//		pCmdUI->Enable( false );
//	else
//		pCmdUI->Enable( true );
		pCmdUI->Enable( false );
}

void CColaView::OnUpdateMesSaveFilename(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	if( m_pModel && m_pModel->GetType() == MODELTYPE_ASE )
//		pCmdUI->Enable( true );
//	else
		pCmdUI->Enable( false );

}

void CColaView::OnUpdateViewLighting(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nLighting )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
	
}

void CColaView::OnViewLighting() 
{
	// TODO: Add your command handler code here
	g_nLighting ^= 1;
}

void CColaView::OnEnvmap() 
{
	// TODO: Add your command handler code here
	g_nEnvMap ^= 1;
}

void CColaView::OnUpdateEnvmap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nEnvMap )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}

void CColaView::OnPlay() 
{
	// TODO: Add your command handler code here
	g_nPlay ^= 1;
	if( m_pModel )
		m_pModel->m_nPause = g_nPlay ^ 1;
}

void CColaView::OnUpdatePlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel == NULL )
	{
		pCmdUI->Enable( false );
		g_nPlay = 0;
		return;
	} else
	{
		if( m_pModel->m_nPause )	g_nPlay = 0;	// pause면 플레이는 멈춤(0)
		else						g_nPlay = 1;
		pCmdUI->Enable( true );
	}

	if( g_nPlay )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}

// 이전 프레임으로
void CColaView::OnPrev() 
{
	// TODO: Add your command handler code here
	if( m_pModel == NULL )	return;
	
	int nCurr = (int)m_pModel->m_fFrameCurrent;
	
	-- nCurr;		// 이전 프레임으로 돌림.
	if( nCurr < 0 )		
		nCurr = m_pModel->m_nFrameMax - 1;

	m_pModel->m_fFrameCurrent = (float)nCurr;
//	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.SetFrameNum( m_pModel->m_fFrameCurrent );
	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);
	
}

// 다음 프레임으로.
void CColaView::OnNext() 
{
	// TODO: Add your command handler code here
	if( m_pModel == NULL )	return;
	
	int nCurr = (int)m_pModel->m_fFrameCurrent;
	
	++ nCurr;		// 이전 프레임으로 돌림.
	if( nCurr >= m_pModel->m_nFrameMax )		
		nCurr = 0;

	m_pModel->m_fFrameCurrent = (float)nCurr;
//	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.SetFrameNum( m_pModel->m_fFrameCurrent );
	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);
	
}

void CColaView::OnUpdatePrev(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

void CColaView::OnUpdateNext(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}


void CColaView::OnAseSaveauto() 
{
	// TODO: Add your command handler code here
	if( m_pModel )
	{
		if( m_pModel->GetType() != MODELTYPE_ASE )	return;		// ase가 아니면 리턴

		CAse *pASE = (CAse *)m_pModel;
		pASE->SaveAuto( g_strModel );
	}
		
}

void CColaView::OnUpdateAseSaveAuto(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#ifdef __VIEWER_ONLY
	pCmdUI->Enable( false );		// 항상 enable
#else
	if( m_pModel && m_pModel->GetModelType() == MODELTYPE_ASE )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
#endif
	
}

void CColaView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_ArcBall.OnLButtonUp( nFlags, point );
	
	CView::OnRButtonUp(nFlags, point);
}

//
// 타격점 클릭
//
void CColaView::OnAttrhit() 
{
	// TODO: Add your command handler code here
	if( m_pModel == NULL )	return;
	if( m_pModel->IsLoadAttr() == FALSE )	return;
	
	float fCurFrm = m_pModel->m_fFrameCurrent;
	if( m_pModel->IsAttrHit(fCurFrm) )
		m_pModel->ResetAttr( (int)fCurFrm, MA_HIT );
	else	
		m_pModel->SetAttr( fCurFrm, MA_HIT );

	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);

}


void CColaView::OnUpdateAttrhit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel == NULL )	return;
	if( m_pModel->IsLoadAttr() == FALSE )	return;

	if( m_pModel->IsAttrHit( m_pModel->m_fFrameCurrent ) )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnAttrsnd() 
{
	// TODO: Add your command handler code here
	if( m_pModel == NULL )	return;
	if( m_pModel->IsLoadAttr() == FALSE )	return;
	
	float fCurFrm = m_pModel->m_fFrameCurrent;
	if( m_pModel->IsAttrSound(fCurFrm) )
		m_pModel->ResetAttr( (int)fCurFrm, MA_SOUND );
	else	
		m_pModel->SetAttr( fCurFrm, MA_SOUND );

	((CMainFrame*)AfxGetMainWnd())->m_dlgTrack.Invalidate(0);
	
}

void CColaView::OnUpdateAttrsnd(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_pModel == NULL )	return;
	if( m_pModel->IsLoadAttr() == FALSE )	return;

	if( m_pModel->IsAttrSound( m_pModel->m_fFrameCurrent ) )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnSolid() 
{
	// TODO: Add your command handler code here
	g_nFillMode = D3DFILL_SOLID;
}

void CColaView::OnUpdateSolid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nFillMode == D3DFILL_SOLID )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnWireframe() 
{
	// TODO: Add your command handler code here
	g_nFillMode = D3DFILL_WIREFRAME;
}

void CColaView::OnUpdateWireframe(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nFillMode == D3DFILL_WIREFRAME )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnPoint() 
{
	// TODO: Add your command handler code here
	g_nFillMode = D3DFILL_POINT;
}

void CColaView::OnUpdatePoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nFillMode == D3DFILL_POINT )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
		
}

void CColaView::OnViewbball() 
{
	// TODO: Add your command handler code here
	g_nBoundBox = 0;
}

void CColaView::OnViewTextureEffect()
{
	g_bTextureEffect = !g_bTextureEffect;
	
	//텍스춰 효과를 보겠다면 텍스쳐 선택 Dialog띄움
	if( g_bTextureEffect )
	{
		CFileDialog		dlgFileOpen( true, "tga", NULL, OFN_FILEMUSTEXIST,
								"TGA Files(*.tga)|*.tga|AllFiles(*.*)|*.*||", NULL );

		if ( dlgFileOpen.DoModal() == IDOK )
		{
			SAFE_RELEASE( g_pReflectMap );
			if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", dlgFileOpen.GetFileName() ), &g_pReflectMap ) ) )
				return;
		}	
	}
}

void CColaView::OnUpdateViewTextureEffect( CCmdUI* pCmdUI )
{
	if( g_bTextureEffect )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}


void CColaView::OnViewTextureEffectBase()
{
	g_bTextureEffectBase = !g_bTextureEffectBase;
	
	//텍스춰 효과를 보겠다면 텍스쳐 선택 Dialog띄움
	if( g_bTextureEffectBase )
	{
		CFileDialog		dlgFileOpen( true, "dds", NULL, OFN_FILEMUSTEXIST,
								"DDS Files(*.dds)|*.dds|AllFiles(*.*)|*.*||", NULL );

		if ( dlgFileOpen.DoModal() == IDOK )
		{
			SAFE_RELEASE( g_pReflectMap2 );
			if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", dlgFileOpen.GetFileName() ), &g_pReflectMap2 ) ) )
				return;
		}	
	}
}

void CColaView::OnUpdateViewTextureEffectBase( CCmdUI* pCmdUI )
{
	if( g_bTextureEffectBase )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CColaView::OnTopView( )
{
	g_bTopView = !g_bTopView;

	g_vRotate.x = 0.0f;
	m_ArcBall.Init();
	
}

void CColaView::OnUpdateTopView( CCmdUI* pCmdUI )
{
	if( g_bTopView )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CColaView::OnUpdateViewbball(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nBoundBox == 0 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CColaView::OnViewbbone() 
{
	// TODO: Add your command handler code here
	g_nBoundBox = 1;
	
}

void CColaView::OnUpdateViewbbone(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nBoundBox == 1 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}

void CColaView::OnViewbbnot() 
{
	// TODO: Add your command handler code here
	g_nBoundBox = 2;
	
}

void CColaView::OnUpdateViewbbnot(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nBoundBox == 2 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnAutorotate() 
{
	// TODO: Add your command handler code here
	g_nAutoRotate = !g_nAutoRotate;
	D3DXMatrixIdentity( &m_mModel );
}

void CColaView::OnUpdateAutoRotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nAutoRotate )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnEdgedFace() 
{
	// TODO: Add your command handler code here
	g_nEdgedFace ^= 1;
}

void CColaView::OnUpdateEdgedFace(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nEdgedFace )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnRotateLight() 
{
	// TODO: Add your command handler code here
	g_nAutoRotateLight ^= 1;
}

void CColaView::OnUpdateRotateLight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	if( g_nAutoRotateLight )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	if( g_nLighting )
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
	
}

void CColaView::OnLod1() 
{
	// TODO: Add your command handler code here
	g_nLOD = 0;	
	if( m_pModel )	m_pModel->SetGroup( g_nLOD );

	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->m_dlgInfo.UpdateInfo();
}

void CColaView::OnUpdateLod1(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nLOD == 0 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CColaView::OnLod2() 
{
	// TODO: Add your command handler code here
	g_nLOD = 1;
	if( m_pModel )	m_pModel->SetGroup( g_nLOD );
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->m_dlgInfo.UpdateInfo();
}

void CColaView::OnUpdateLod2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nLOD == 1 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnLod3() 
{
	// TODO: Add your command handler code here
	g_nLOD = 2;	
	if( m_pModel )	m_pModel->SetGroup( g_nLOD );
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->m_dlgInfo.UpdateInfo();
}

void CColaView::OnUpdateLod3(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nLOD == 2 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnGrid() 
{
	// TODO: Add your command handler code here
	g_nGrid ^= 1;
}

void CColaView::OnUpdateGrid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nGrid )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


void CColaView::OnReloadReflection() 
{
	// TODO: Add your command handler code here
	SAFE_RELEASE( g_pReflectMap	);
	
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakeExePath( "", "reflect.tga" ), &g_pReflectMap ) ) )
	{
		Error( "Reflect.tga 텍스쳐 못읽음" );
	}
	
}

void CColaView::OnNextOp() 
{
	// TODO: Add your command handler code here
	g_nSelectTextureOp ++;
	g_nSelectTextureOp %= MAX_TEXTURE_OP;


}

void CColaView::OnLightning() 
{
	// TODO: Add your command handler code here
	g_nLightning ^= 1;	
}

void CColaView::OnUpdateLightning(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nLightning )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}


void CColaView::OnCollobject() 
{
	// TODO: Add your command handler code here
	g_nCollObject ^= 1;	
}

void CColaView::OnUpdateCollobject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( g_nCollObject )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CColaView::OnUpdateCamLod(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	if(m_bUseCLOD) pCmdUI->SetCheck(1);
	else		   pCmdUI->SetCheck(0);
}

void CColaView::OnCamLod() 
{
	// TODO: Add your command handler code here
	m_bUseCLOD = !m_bUseCLOD;
}
