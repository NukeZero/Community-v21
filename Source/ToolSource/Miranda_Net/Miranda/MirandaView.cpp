//    MirandaView.cpp : implementation of the CMirandaView class
//

#include "stdafx.h"
#include "Miranda.h"
#include "MirandaDoc.h"
#include "MirandaView.h"
#include "..\_Common\sfxbase.h"
#include "MainFrm.h"
#include "..\_Common\Camera.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

C2DRender g_2DRender;

LPDIRECT3DTEXTURE9	g_pReflectMap = NULL;
LPDIRECT3DTEXTURE9	g_pReflectMap2 = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMirandaView

IMPLEMENT_DYNCREATE(CMirandaView, CView)

BEGIN_MESSAGE_MAP(CMirandaView, CView)
	//{{AFX_MSG_MAP(CMirandaView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirandaView construction/destruction

CMirandaView::CMirandaView()
{
	// TODO: add construction code here
	m_hwndRenderWindow = NULL;
	m_hwndRenderFullScreen = NULL;
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE; 	

	m_Camera.setCameraType(Camera::AIRCRAFT);
	m_bPlayMode = DEPLOY_MODE;

	CSfxBase* pSfxBase=new CSfxBase();
	g_SfxMng.AddSfxBase(pSfxBase);
	m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));

	m_bIsMBClick = m_bIsLBClick = m_bIsRBClick = false;
	m_bIsFocused = true;

	m_fFogStartValue	= 70;
	m_fFogEndValue		= 400;
	m_fFogDensity		= 0.0f;
	m_dwAmbient	= D3DCOLOR_ARGB( 255,128,128,128);

	CLight* pLight;
	pLight = new CLight;
	pLight->SetLight( "direction", D3DLIGHT_DIRECTIONAL, 0, 150, 0 );
	pLight->Attenuation0 =  0.0f;
	pLight->Range        = 40;//FLT_MAX;//127.0f;
	pLight->Diffuse.r    = 1.0f;
	pLight->Diffuse.g    = 1.0f;
	pLight->Diffuse.b    = 1.0f;
	pLight->Specular.r   = 1.0f;
	pLight->Specular.g   = 1.0f;
	pLight->Specular.b   = 1.0f;
	pLight->Ambient.r    = 0.0f;
	pLight->Ambient.g    = 0.0f;
	pLight->Ambient.b    = 0.0f;
	pLight->Direction    = D3DXVECTOR3( 0.0f, 0.5f, 0.5f);
	AddLight( pLight );


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

	m_pAutoModel = NULL;
	m_pOverloadBox = NULL;
	m_nParts = 0;
	m_bRenderBone = TRUE;
	m_pSfxUnit = NULL;

	m_fMoveUP = 0.0f;
}


CMirandaView::~CMirandaView()
{
	delete GetLight("direction");
	SAFE_DELETE( m_pAutoModel );
	SAFE_DELETE( m_pOverloadBox );
	SAFE_DELETE( m_pSfxUnit );
	ClearSfxUnits( );

	SAFE_RELEASE( g_pReflectMap );
	SAFE_RELEASE( g_pReflectMap2 );
}


BOOL CMirandaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMirandaView drawing

void CMirandaView::OnDraw(CDC* pDC)
{
	CMirandaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMirandaView printing

BOOL CMirandaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMirandaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMirandaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMirandaView diagnostics

#ifdef _DEBUG
void CMirandaView::AssertValid() const
{
	CView::AssertValid();
}

void CMirandaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMirandaDoc* CMirandaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMirandaDoc)));
	return (CMirandaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMirandaView message handlers


HRESULT CMirandaView::AdjustWindowForChange()
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


BOOL CMirandaView::CreateDevice() 
{
	m_hwndRenderWindow = CView::GetSafeHwnd();

	// Note that for the MFC samples, the device window and focus window
	// are not the same.
	CD3DApplication::m_hWnd = m_hwndRenderWindow;
	CD3DApplication::m_hWndFocus = NULL;//m_hwndRenderFullScreen;
	CD3DApplication::Create( AfxGetInstanceHandle() );

	
	DXUtil_Timer( TIMER_RESET );
	
	return TRUE;
}


void CMirandaView::AddLight( CLight* pLight )
{
	pLight->m_dwIndex = m_dwLightIndex++;
	m_mapLight.SetAt( pLight->m_strKey, pLight );
}


CLight* CMirandaView::GetLight( LPCTSTR lpszKey )
{
	CLight* pLight = NULL;
	m_mapLight.Lookup( lpszKey, (void*&)pLight );
	return pLight;
}


void CMirandaView::SetLight( BOOL bLight )
{

	CMirandaDoc*	pDoc = GetDocument();

#ifndef  __WORLDSERVER 
	DWORD dwAmbient = D3DCOLOR_ARGB( 0,0,0,0);
	CLight* pLight;
	// 플레이어 위치로 변경해야 함 
	D3DXVECTOR3 vPos = D3DXVECTOR3( 0, 0, 0 );


	//D3DXVECTOR3 vPos = ( CObj::GetActiveObj() != NULL ? CObj::GetActiveObj()->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );

	pLight = GetLight( "direction" );

	
	if( pLight )
	{
		LIGHTCOLOR light[24] = // diffuse, ambient
		{
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 0 12
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 1
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 2
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 3
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 4
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 5

			0.5f,  0.5f,  0.6f,    0.4f, 0.4f, 0.4f, // 6
			0.7f,  0.7f,  0.7f,    0.5f, 0.5f, 0.5f, // 7
			0.8f,  0.8f,  0.8f,    0.5f, 0.5f, 0.5f, // 8
			0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 9
			1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 10
			1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 11
			1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 12
			1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 13 1
			1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 14 2
			1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 15 3
			0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 16 5
			0.9f,  0.6f,  0.2f,    0.5f, 0.5f, 0.4f, // 17 6
			0.6f,  0.6f,  0.4f,    0.4f, 0.4f, 0.4f, // 18 7
			0.5f,  0.5f,  0.4f,    0.4f, 0.4f, 0.4f, // 19 8
			0.45f,  0.45f,  0.4f,    0.35f, 0.35f, 0.35f, // 20 8
			0.43f,  0.43f,  0.5f,    0.33f, 0.33f, 0.3f, // 21 9
			0.41f,  0.41f,  0.5f,    0.31f, 0.31f, 0.3f, // 22 10
			0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f  // 23 11
		};
		int nHour = 8, nMin = 0;

		// 클라이언트는 시간을 g_GameTimer에서 가져온다. 
		nHour = g_GameTimer.m_nHour;
		nMin  = g_GameTimer.m_nMin ;

		//nHour--;

		if( nHour < 0 ) nHour = 0;
		if( nHour > 23 ) nHour = 23;

		//if( m_bFixedHour )
		//	nHour = m_nFixedHour, nMin = 0;
		LIGHTCOLOR lightColorPrv = light[ ( nHour - 1 == -1 ) ? 23 : nHour - 1 ];
		LIGHTCOLOR lightColor = light[ nHour ];

		//m_lightColor = lightColorPrv;
		lightColorPrv.r1 += ( lightColor.r1 - lightColorPrv.r1) * nMin / 60;
		lightColorPrv.g1 += ( lightColor.g1 - lightColorPrv.g1) * nMin / 60;
		lightColorPrv.b1 += ( lightColor.b1 - lightColorPrv.b1) * nMin / 60;
		lightColorPrv.r2 += ( lightColor.r2 - lightColorPrv.r2) * nMin / 60;
		lightColorPrv.g2 += ( lightColor.g2 - lightColorPrv.g2) * nMin / 60;
		lightColorPrv.b2 += ( lightColor.b2 - lightColorPrv.b2) * nMin / 60;
		// 60(minMax) : 15(curMin) = 0.5(colorDistant) : x(curCol)

		// 음영 변화 
		pLight->Diffuse.r  = lightColorPrv.r1;
		pLight->Diffuse.g  = lightColorPrv.g1;
		pLight->Diffuse.b  = lightColorPrv.b1;
		// 변화 없음 
		pLight->Specular.r = 1.0f;
		pLight->Specular.g = 1.0f;
		pLight->Specular.b = 1.0f;
		// 전체 변화 
		pLight->Ambient.r  = lightColorPrv.r2;
		pLight->Ambient.g  = lightColorPrv.g2;
		pLight->Ambient.b  = lightColorPrv.b2;
/*
		if( m_nBloom )		// 뽀샤시 옵션이 켜져있을땐 조명을 좀 낮춰줘야 한다. 안그러면 너무 밝아.
		{
			pLight->Diffuse.r *= 0.6f;
			pLight->Diffuse.g *= 0.6f;
			pLight->Diffuse.b *= 0.6f;
			pLight->Ambient.r *= 0.7f;
			pLight->Ambient.g *= 0.7f;
			pLight->Ambient.b *= 0.7f;
		}*/
		
		memcpy( &m_lightFogSky, pLight, sizeof( m_lightFogSky ) );


		pLight->Diffuse.r  *= 1.0f;
		pLight->Diffuse.g  *= 1.0f;
		pLight->Diffuse.b  *= 1.0f;
		// 변화 없음 
		pLight->Specular.r = 2.0f;
		pLight->Specular.g = 2.0f;
		pLight->Specular.b = 2.0f;
		// 주변 
 		pLight->Ambient.r  *= 1.0f;
		pLight->Ambient.g  *= 1.0f;
		pLight->Ambient.b  *= 1.0f;


		memcpy( &m_light, pLight, sizeof( m_light ) );
		
		D3DXVECTOR3 vecSun=D3DXVECTOR3( 0.0f, 0.0f,1.0f);
		D3DXMATRIX  matTemp;
		D3DXMatrixRotationX( &matTemp,(pDoc->m_SkyBox.m_fSunAngle +180)*3.1415926f/180.0f);
		D3DXVec3TransformCoord(&vecSun,&vecSun,&matTemp);
		pLight->SetDir( vecSun.x, vecSun.y, vecSun.z ); 

		vecSun = D3DXVECTOR3( 0.0f, 0.5f,0.5f);
		D3DXVec3Normalize(&(vecSun),&(vecSun));
		pLight->SetDir( -vecSun.x, -vecSun.y, -vecSun.z ); 
		pLight->Appear( m_pd3dDevice, TRUE );

		DWORD dwR, dwG, dwB;
		dwR = (DWORD)( pLight->Ambient.r * 255 );
		dwG = (DWORD)( pLight->Ambient.g * 255 );
		dwB = (DWORD)( pLight->Ambient.b * 255 );
		dwAmbient = D3DCOLOR_ARGB( 255, dwR, dwG, dwB );			
	}
	

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, bLight );//m_bViewLight );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0,0,0,0) ); //m_dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );
//	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );
	::SetLight( bLight );

	// 기본 패터리얼 정의 
	m_pd3dDevice->SetMaterial( &m_baseMaterial );
#endif // not WORLDSERVER
}



HRESULT CMirandaView::Render()
{

	CMirandaDoc*	pDoc = GetDocument();
	CMainFrame*		pMain = (CMainFrame*)AfxGetMainWnd();
	D3DXMATRIX		matWorld,matView,matProj,matTemp;
	FLOAT		    fAspect = ((FLOAT)m_d3dpp.BackBufferWidth) / m_d3dpp.BackBufferHeight;


	//TRACE("camera %f,%f,%f\n", m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z );
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, pMain->m_dwColor, 1.0f, 0);

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 0.1f, 5000.0f );


	m_Camera.getViewMatrix(&matView);	

	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
	g_matView = matView;
	D3DXMatrixInverse(&g_matInvView,NULL,&g_matView);
	g_matInvView._41 = g_matInvView._42 = g_matInvView._43=.0f;

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene( ) ) )
	{

		// 기본 랜더 세팅 
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
		
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	//	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		D3DXVECTOR3		vecLook, vecPos(0.0f, 0.0f, 0.0f);
		m_Camera.getLook(&vecLook);
		//m_Camera.getPosition(&vecPos);
		pDoc->m_SkyBox.Render(vecPos, vecLook, m_pd3dDevice);

		D3DXMatrixIdentity(&matWorld);
		m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld);
		m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
		
		SetLight( TRUE );	
		
		//SetFogEnable( m_pd3dDevice, TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,      0x33333333 );

		//SetLight(TRUE);
		// Terrain Render
		SetStateShadowMap( m_pd3dDevice, 2, matView );
		pDoc->m_Terrain->Render(&matWorld);
		ResetStateShadowMap( m_pd3dDevice, 2 );
		
		SetLight(TRUE);
		
		static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
		m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
		SetTransformView( matView );
		SetTransformProj( m_matProj );
		SetDiffuse( 0.5f, 0.5f, 0.5f );
		SetAmbient( m_light.Ambient.r+0.2f, m_light.Ambient.g+0.2f, m_light.Ambient.b+0.2f );	
		
		// Obj Rendering
		pDoc->m_ObjMgr.Render();
		
		if( m_pAutoModel )
		{
			if( g_bWireframe )
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				m_pd3dDevice->SetTexture( 0, NULL );
			}

			if( m_fMoveUP != 0.0f )
				matWorld._42 += m_fMoveUP;

			m_pAutoModel->Render( m_pd3dDevice, &matWorld );
		
			// gmpbigsun : 본은 Solid, no lighting 으로 ... 그리고 젤 마지막에 그려주자 
			if( g_bWireframe )
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			
// 			if( m_bZoff )
// 			{
// 				m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
// 				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
// 			}

			if( m_bRenderBone )
			{
				m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				m_pAutoModel->RenderBone( m_pd3dDevice, &matWorld, m_pOverloadBox );
			}
		//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		}

		// Sfx Rendering 
		pDoc->m_SfxMgr.Render();

		if( !g_bMultiMode )
		{
			if( m_pSfxUnit )
				m_pSfxUnit->Render( m_pd3dDevice );
		}
		else
		{
			for( BsSfxUnitIter iter = m_cSfxUnits.begin(); iter != m_cSfxUnits.end(); ++iter )
			{
				BsSfxUnit* pUnit = *iter;
				pUnit->Render( m_pd3dDevice );
			}
		}

		SetLight( FALSE );
		RenderText( );

		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetVertexDeclaration( NULL );		
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		// End the scene.
		m_pd3dDevice->EndScene();
	}



    return S_OK;
}

int CMirandaView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}

void CMirandaView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	RenderScene(FALSE);
	// TODO: Add your message handler code here
	
	// Do not call CView::OnPaint() for painting messages
}

void CMirandaView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	HRESULT hr;
	
	if( m_bActive && m_bWindowed )
	{
		RECT rcClientOld;
		rcClientOld = m_rcWindowClient;

		// Update window properties
		CView::GetWindowRect(&m_rcWindowBounds );
		CView::GetClientRect(&m_rcWindowClient );

		if( rcClientOld.right - rcClientOld.left !=
				m_rcWindowClient.right - m_rcWindowClient.left ||
				rcClientOld.bottom - rcClientOld.top !=
				m_rcWindowClient.bottom - m_rcWindowClient.top)
		{

			m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

			// Resize the 3D environment
			if( FAILED( hr = Reset3DEnvironment() ) ) //Resize3DEnvironment() ) )
			{
				//DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
				return;
			}
			//Pause( FALSE );
			//m_bReady = TRUE;
		}
	}	
	Invalidate(FALSE);
	
}


HRESULT CMirandaView::InitDeviceObjects()
{

	CMirandaDoc* pDoc = GetDocument();

	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);
	
	pDoc->m_ObjMgr.Init(m_pd3dDevice);
 	pDoc->m_SfxMgr.Init(m_pd3dDevice);
	pDoc->m_SfxMgr.GetObjMgr(&pDoc->m_ObjMgr);

	// 지형 셋업 
	if(pDoc->Setup(m_pd3dDevice)) Error("지형 텍스쳐 로딩 실패");

	pDoc->m_SkyBox.InitDeviceObjects(m_pd3dDevice);
	
	DragAcceptFiles( TRUE );

	//setup 2dRender
#if 0 
	m_pFont = new CD3DFont( _T("gulim"), 9 );
	m_pFont->InitDeviceObjects( m_pd3dDevice );
			
	g_2DRender.InitDeviceObjects( m_pd3dDevice );
	g_2DRender.m_pFont = m_pFont;
#endif

	m_pFontApi = new CD3DFontAPI( "Arial", 10 );
    m_pFontApi->m_nOutLine = 2;
	m_pFontApi->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
	m_pFontApi->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
	m_pFontApi->m_dwFlags = D3DFONT_FILTERED;
	m_pFontApi->InitDeviceObjects( m_pd3dDevice );	 
    return S_OK;

}


HRESULT CMirandaView::InvalidateDeviceObjects()
{
	CMirandaDoc* pDoc = GetDocument();

	g_SfxMng.InvalidateDeviceObjects();
	pDoc->m_SkyBox.InvalidateDeviceObjects();

	g_2DRender.InvalidateDeviceObjects( );

	m_pFontApi->InvalidateDeviceObjects( );
    return S_OK;

}

extern LPDIRECT3DTEXTURE9 g_pReflectMap;
extern LPDIRECT3DTEXTURE9 g_pReflectMap2;

void CMirandaView::OnInitialUpdate() 
{

	CMirandaDoc* pDoc = GetDocument();
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	// dx장치 생성 
	CreateDevice();

	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "mvr_atrox-ef01.tga" ), &g_pReflectMap ) ) )
	{
	//	assert( 0 );
	}

	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "water_river01.dds" ), &g_pReflectMap2 ) ) )
//if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( "", "Rartesia_Grass02.dds" ), &g_pReflectMap2 ) ) )
	{
	//	assert( 0 );
	}
	
	// 업데이트 타이머 세팅 
	SetTimer(MAIN_TIMER, 10, NULL);
	
}

void CMirandaView::OnTimer(UINT nIDEvent) 
{
	
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
		case MAIN_TIMER:
			Update();
			RenderScene(FALSE);
			Invalidate(FALSE);
			break;
	}

	CView::OnTimer(nIDEvent);
}


HRESULT CMirandaView::RestoreDeviceObjects()
{

	CMirandaDoc* pDoc = GetDocument();

    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) || ( m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
		CreateSkinningVS( m_pd3dDevice, _T("skining2.vsh") );
    }
	
	// Create and set up the shine materials w/ textures
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );
	
	g_SfxMng.RestoreDeviceObjects();
	pDoc->m_SkyBox.RestoreDeviceObjects(m_pd3dDevice);
	g_ModelGlobal.RestoreDeviceObjects(m_pd3dDevice);
	g_2DRender.RestoreDeviceObjects( &m_d3dsdBackBuffer );
	m_pFontApi->RestoreDeviceObjects();

    RenderScene( FALSE );
    Invalidate(FALSE);

    return S_OK;
}

BOOL CMirandaView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	
	//return CView::OnEraseBkgnd(pDC);
}

void CMirandaView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CMirandaDoc* pDoc = GetDocument();
	switch(nChar)
	{
		case VK_SPACE :
			// 스위칭 디플로이모드 / 시뮬모드 
 			m_bPlayMode = !m_bPlayMode;
			break;
		case VK_SUBTRACT :
			if(pDoc->m_Selected.nType == TYPE_OBJECT) 
				 pDoc->m_ObjMgr.AddFrame(pDoc->m_Selected.nID, -1);
			else pDoc->m_SfxMgr.AddFrame(pDoc->m_Selected.nID, -1);
			break;

		case VK_ADD :
			if(pDoc->m_Selected.nType == TYPE_OBJECT) 
				 pDoc->m_ObjMgr.AddFrame(pDoc->m_Selected.nID, 1);
			else pDoc->m_SfxMgr.AddFrame(pDoc->m_Selected.nID, 1);
			break;

		case VK_DELETE :
			if(pDoc->m_Selected.nType == TYPE_OBJECT)
			{
				pDoc->m_ObjMgr.RemoveObj(pDoc->m_Selected.nID);
			}
			else if(pDoc->m_Selected.nType == TYPE_SFX)
			{
				pDoc->m_SfxMgr.DeleteSfx(pDoc->m_Selected.nID);
			}
			break;

		case 'C' :
			// 컨트롤 패널 보이기/숨기기
			((CMainFrame *)AfxGetMainWnd())->SwitchVisibility(IDD_CTRL_PANEL);
			break;

		case 'F' :
			// 프레임 패널 보이기/숨기기
			if(((CMainFrame *)AfxGetMainWnd())->m_FramePanel.IsVisible())
			{
				((CMainFrame *)AfxGetMainWnd())->SwitchVisibility(IDD_CTRL_PANEL);
				((CMainFrame *)AfxGetMainWnd())->m_FramePanel.ShowWindow(SW_HIDE);
				((CMainFrame *)AfxGetMainWnd())->SwitchVisibility(IDD_CTRL_PANEL);
			}
			else
			{
				((CMainFrame *)AfxGetMainWnd())->SwitchVisibility(IDD_CTRL_PANEL);
				((CMainFrame *)AfxGetMainWnd())->m_FramePanel.ShowWindow(SW_SHOW);
				((CMainFrame *)AfxGetMainWnd())->SwitchVisibility(IDD_CTRL_PANEL);
			}
			break;

		case 'M' : 
			// 에니메이션 다이얼로그
			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->SwitchVisibility( IDD_DIALOG_ANIMATION );
			}
			break;

		case 'L' :
			// sfx 저장목록
			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				BOOL bShow = pFrame->m_kFinal.IsWindowVisible( );
				pFrame->m_kFinal.ShowWindow( !bShow );
				pFrame->SetFocus();
			}
			break;

		case 'B' :
			// 본 리스트 
			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->SwitchVisibility( IDD_DIALOG_BONELIST );
			}
			break;

		case 'R' : //reset rendering effect
			{
				SAFE_DELETE( m_pSfxUnit );

				ClearSfxUnits( );
			}
			break;

	}


	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CMirandaView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	// 역시 메시지로 구현해놓으니 좋지않다.. 나중에 DX로 바꾸자..

	int b = 0;
	switch( nChar )
	{
	case VK_HOME:
		if( m_pAutoModel )
		{
			m_fMoveUP += 1.0f;
		}

		break;

	case VK_END:
		if( m_pAutoModel )
		{
			m_fMoveUP -= 1.0f;
		}
	}


	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


HRESULT CMirandaView::Update()
{
	CMirandaDoc* pDoc = GetDocument();

	if(m_bPlayMode)
	{
		// 시뮬모드 
		pDoc->m_SfxMgr.Update();
		pDoc->m_ObjMgr.Update();
	}

	if( m_pAutoModel )
		m_pAutoModel->FrameMove();

	if( !g_bMultiMode )
	{
		if( m_pSfxUnit )
			m_pSfxUnit->Update( m_pAutoModel );
	}
	else
	{
		for( BsSfxUnitIter iter = m_cSfxUnits.begin(); iter != m_cSfxUnits.end(); ++iter )
		{
			BsSfxUnit* pUnit = *iter;
			pUnit->Update( m_pAutoModel );
		}
	}
	
	if(m_bIsFocused)
	{
		//gmpbigsun
		if( ::GetAsyncKeyState( 0x57) & 0x8000f )		// 'W'
			m_Camera.fly( 0.1f );

		if( ::GetAsyncKeyState( 0x53 ) & 0x8000f )		// 'S'
			m_Camera.fly( -0.1f );

		if( ::GetAsyncKeyState( 0x41 ) & 0x8000f )		// 'A'
			m_Camera.strafe( -0.2f );
		
		if( ::GetAsyncKeyState( 0x44 ) & 0x8000f )		// 'D'
			m_Camera.strafe( 0.2f );

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			m_Camera.walk(0.2f);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			m_Camera.walk(-0.2f);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			m_Camera.strafe(-0.2f);

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			m_Camera.strafe(0.2f);
		
		if( ::GetAsyncKeyState(VK_NUMPAD8) & 0x8000f )
			m_Camera.pitch(-0.04f);
	
		if( ::GetAsyncKeyState(VK_NUMPAD2) & 0x8000f )
			m_Camera.pitch(0.04f);
	
		if( ::GetAsyncKeyState(VK_NUMPAD4) & 0x8000f )
			m_Camera.yaw(-0.04f);
	
		if( ::GetAsyncKeyState(VK_NUMPAD6) & 0x8000f )
			m_Camera.yaw(0.04f);
	}

	m_Camera.Update();

	return S_OK;
			
}


void CMirandaView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bIsLBClick = true;
	m_ptLast	 = point;	
	SetCapture();
	CView::OnLButtonDown(nFlags, point);
}


void CMirandaView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
	// TODO: Add your message handler code here and/or call default
	m_bIsRBClick = true;
	m_ptLast	 = point;
	CMirandaDoc*	pDoc = GetDocument();

	if((nFlags & MK_CONTROL) && pDoc->m_Selected.nType == TYPE_OBJECT) 
	{
		m_Camera.switchMode(MODE_MODEL_ROTATE);
		m_Camera.SetTarget(pDoc->m_ObjMgr.GetPos(pDoc->m_Selected.nID));
	}
	SetCapture();
	CView::OnRButtonDown(nFlags, point);
}


void CMirandaView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	
	// TODO: Add your message handler code here and/or call default
	m_bIsRBClick = false;
	
	m_Camera.switchMode(MODE_WORLD_ROTATE);
		
	ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
}


void CMirandaView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bIsMBClick = true;
	SetCapture();
	
	CView::OnMButtonDown(nFlags, point);
}

void CMirandaView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bIsMBClick = false;
	ReleaseCapture();

	CView::OnMButtonUp(nFlags, point);
}

BOOL CMirandaView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	CMirandaDoc*	pDoc = GetDocument();

	if((nFlags & MK_CONTROL) && pDoc->m_Selected.nType == TYPE_OBJECT) 
	{
		m_Camera.AddDistance(zDelta * 0.002f);
	}
	else
	{
		// 시프트키를 누르면 세밀하게 조절한다
		if(nFlags & MK_SHIFT)
			zDelta >>= 2;
		
		m_Camera.walk(zDelta * 0.01f);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMirandaView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CMirandaDoc*	pDoc = GetDocument();

	if((nFlags & MK_CONTROL) && pDoc->m_Selected.nType == TYPE_OBJECT) 
	{
		if(m_bIsRBClick)
		{
			m_Camera.m_fCameraYawAngle		+= ((float)m_ptLast.x-(float)point.x) * 0.2f;
			m_Camera.m_fCameraPitchAngle    += ((float)m_ptLast.y-(float)point.y) * 0.2f;
		}
		
	}
	else
	{
		if(m_bIsRBClick)
		{	
			CPoint ptAdd = point - m_ptLast;
			CRect rect;
			CView::GetClientRect(&rect);
			FLOAT m_fRadius = 0.1f;
			FLOAT fX = (ptAdd.x / (m_fRadius*rect.Width()));
			FLOAT fY =  (ptAdd.y / (m_fRadius*rect.Height()));
			if(nFlags & MK_SHIFT)
			{
				m_Camera.yaw(fX* 0.1f);
				m_Camera.pitch(fY* 0.1f);
			}
			else
			{
				m_Camera.yaw(fX* 0.4f);
				m_Camera.pitch(fY* 0.4f);
			}
			
		}
		if(m_bIsMBClick)
		{	
			CPoint	ptAdd = point - m_ptLast;
			float	fX = (float)( ptAdd.x );
			float	fY = (float)( ptAdd.y );

			if(nFlags & MK_SHIFT)
			{
				m_Camera.strafe(fX * 0.02f);
				m_Camera.walk(-fY * 0.02f);
			}
			else
			{
				m_Camera.strafe(fX * 0.2f);
				m_Camera.walk(-fY * 0.2f);
			}
		}
	}
	m_ptLast = point;

	CView::OnMouseMove(nFlags, point);
}


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

void CMirandaView::Load_O3D( const TCHAR* szFullName )
{
	static const int MAX_TYPE = 10;

	// 파일이름으로 파츠인지 여부를 판단하고
	int nKind = 0;
	if( strstrlwr( szFullName, "mvr_" ) )
	{
		nKind = 1;
	}
	else if( strstrlwr( szFullName, "part_" ) )
	{
		nKind = 2;
	}else
	{
		nKind = MAX_TYPE;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if( !pFrame )
		return;

	if( 2 != nKind )
	{
		pFrame->m_kAniPanel.ResetAllWnd( );
		pFrame->m_kBoneList.ResetAllWnd( );
	}

	//파츠일 경우는 파츠를 대변하는 이름을 저장해야 하지만 일단 주인공 캐릭터는 제외하고
	//npc 및 몬스터만을 대상으로 한다. 
	//만일 파츠형태의 오브젝트까지 지원할 생각이라면 이름규칙을 정해야 한다 gmpbigsun( 100126 )
	m_strLoadedO3DName = GetFileName( szFullName );
	((CMainFrame*)AfxGetMainWnd())->SetWindowText( m_strLoadedO3DName );

	// 파츠여부에 따라 기존모델을 삭제하거나 보존한다.
	switch( nKind )
	{
	case 1:  //mover
		m_nParts = 0;
		SAFE_DELETE( m_pAutoModel );
		break;

	case 2:
		m_nParts = GetPartsNum( szFullName );
		if( !strstrlwr( m_strLoadedName, "part_" ) )			// 현재 로드된게 parts가 아니면 걍 삭제  
		{
			if( m_pAutoModel )
			{	m_pAutoModel->InitAnimate( );
				SAFE_DELETE( m_pAutoModel );
			}
		}
		break;

	default:
		m_nParts = 0;
		SAFE_DELETE( m_pAutoModel );
		break;
	}

	BOOL bLoadingOK = FALSE;
	if( nKind > 0 )			
		bLoadingOK = LoadModel_Auto( szFullName );
			
	if( bLoadingOK )
		m_strLoadedName = szFullName;		//로드된 파일 보존 
}

void CMirandaView::Load_SFX( const TCHAR* szFullName )
{
	//TODO : 
	// 1. 현재 모델이 로드되어 있으면 어디에 붙일것인지 물어본다 
	// 2.    -                 있지 않으면 걍 ( 0, 0, 0 )에 출력한다.
	// 3. 저장모드를 지원 ( 어떤 이펙트를, 어떤본에, 반복속성으로 )

	SAFE_DELETE( m_pSfxUnit );
	
	m_pSfxUnit = new BsSfxUnit;
	m_pSfxUnit->Init( szFullName );
}

void CMirandaView::Load_SFX_Multi( const TCHAR* szFullName, const TCHAR* szBone )
{
	for( BsSfxUnitIter iter = m_cSfxUnits.begin(); iter != m_cSfxUnits.end(); ++iter )
	{
		BsSfxUnit* pUnit = *iter;
		if( 0 == strcmp( pUnit->_szFollowBoneName, szBone ) )
		{
			return;
		}
	}

	BsSfxUnit* pUnit = new BsSfxUnit;
	pUnit->Init( szFullName );
	strcpy( pUnit->_szFollowBoneName, m_strFollowedBone.GetBuffer(0) );

	m_cSfxUnits.push_back( pUnit );
}

void CMirandaView::ClearSfxUnits( )
{
	for( BsSfxUnitIter iter = m_cSfxUnits.begin(); iter != m_cSfxUnits.end(); ++iter )
		SAFE_DELETE( *iter );

	m_cSfxUnits.clear();
}

BOOL CMirandaView::LoadModel_Auto( const TCHAR* szFullName )
{
	// 1. 본을 자동으로 읽는다
	// 2. m_pAutoModel를 생성한다.
	// 3. 해당 Ani파일을 찾어서 리스트에 등록 시킨다.

	// m_pAutoModel이 NULL이면 생성한다

	if( !m_pAutoModel )
		m_pAutoModel = new CModelObject;

	m_strBoneName = "";
	m_strFollowedBone = "";

	m_pAutoModel->InitDeviceObjects( m_pd3dDevice );

	// load element
	if( m_pAutoModel->LoadElement( szFullName, m_nParts ) == FAIL )		//이함수 안에 자동으로 본을 읽어주는 함수가 들어갔다. check the func!!
	{
		SAFE_DELETE( m_pAutoModel );	// 만약 본을 안읽고 스킨.o3d를 읽었다면 삭제시키고 리턴.
		return FALSE;
	}

	//모델이 로드되고 본이 28개를 넘는다면 경고메시지 
	CMainFrame*		pMain = (CMainFrame*)AfxGetMainWnd();
	if( !pMain )
		return FALSE;

	if( m_pAutoModel->m_pBone )
	{	
		// list up all animation
		CString szFileName = GetFileName( szFullName );
		pMain->m_kAniPanel.ListUpAllAnimation( szFileName );

		// 본 표시용 박스 
		if( !m_pOverloadBox )
		{
			//gmpbigsun : 일단 본위치에 박스있는거 좀 쭐여서 랜더함 
			D3DXMATRIX kScaleMat;
			m_pOverloadBox = new CModelObject;
			m_pOverloadBox->InitDeviceObjects( m_pd3dDevice );
			m_pOverloadBox->LoadElement( "Item_GenMatMoonstone.o3d", 0 );  
//			m_pOverloadBox->LoadElement( "Item_Bobohealth.o3d", 0 );  
		//	m_pOverloadBox->LoadElement( "Item_Fujitea.o3d", 0 );  

			D3DXMatrixIdentity( &kScaleMat );
 			D3DXMatrixScaling( &kScaleMat, 0.5f, 0.5f, 0.5f );

			CObject3D* pObj = m_pOverloadBox->GetObject3D( );
			GMOBJECT* pGMObj = pObj->GetGMOBJECT();
		
			//float x, y, z ;
			//x = matOld._41;
			//y = matOld._42;
			//z = matOld._43;
			D3DXMatrixMultiply( &pGMObj->m_mLocalTM, &pGMObj->m_mLocalTM, &kScaleMat );
			//matOld._41 = x;
			//matOld._42 = y;
			//matOld._43 = z;
		
			//m_pOverloadBox->m_vMin *= 0.04f;
			//m_pOverloadBox->m_vMax *= 0.04f;

			g_Grp3D.SetDevice( m_pd3dDevice );
		}
	}

	return TRUE;
}

void CMirandaView::LoadBone_Auto( const TCHAR* szFileName )
{
	// bone
	m_pAutoModel->m_pBone = NULL;
	SAFE_DELETE_ARRAY( m_pAutoModel->m_mUpdateBone );

	CString strBone;

	if( strstrlwr( szFileName, "part_" ) )
	{
		if( strstrlwr( szFileName, "part_m" ) ) //남성 
			strBone = "mvr_male";
		else strBone = "mvr_female";
	}else
	{
		strBone = GetFileTitle( szFileName );
	}
	
	strBone += ".chr";

//	CString fullPath = GetResourcePath();
//	fullPath += "Model\\";
//	fullPath += strBone;

	if( FAIL == m_pAutoModel->LoadBone( strBone ) )
	{
		Error( "%s : bone not loaded.", szFileName );		
		return;
	}

	// 성공적으로 본을 로드했다면 , 본 리스트를 만들어 준다.
	CMainFrame*	pMain = (CMainFrame*)AfxGetMainWnd();
	if( pMain )
	{
		pMain->m_kBoneList.Make_BoneList( m_pAutoModel );
	}
}


void CMirandaView::RenderText( )
{
#if 0
	g_2DRender.SetViewportOrg( rect.TopLeft() );
	g_2DRender.SetViewport( rect );
	g_2DRender.m_clipRect = rect;
	g_2DRender.TextOut( 500, 500, "dlaksdjflaksjdflkjaslfkjslfjk", 0xffffff00 );		//왜 안댐??? 20100501
	g_2DRender.TextOut( 100, 100, 2.0f, 2.0f, "lakdfjlaskdfjlaskjdflkj", 0xffffff00 );

	rect.left += 10;
	rect.top += 10;
	g_2DRender.RenderRect( rect, 0xffffff00 );
	g_2DRender.RenderFillRect( rect, 0x770000ff );
#endif

	float x = 10.0f;
	float y = 16.0f;
	
	m_pFontApi->DrawText( 10.0f, y, 0xffffff00, " ' C' toggle control panel ");
	m_pFontApi->DrawText( 10.0f, y+1*y, 0xffffff00, " ' F' toggle frame panel ");
	m_pFontApi->DrawText( 10.0f, y+2*y, 0xffffff00, " ' M' toggle animation panel ");
	m_pFontApi->DrawText( 10.0f, y+3*y, 0xffffff00, " ' L' toggle sfx list panel ");
	m_pFontApi->DrawText( 10.0f, y+4*y, 0xffffff00, " ' B' toggle bone list panel ");
	m_pFontApi->DrawText( 10.0f, y+5*y, 0xffffff00, " ' R' remove all sfx ");

	if( m_pAutoModel )
	{
		CString strInfo;
		CRect rect;
		GetClientRect( &rect );

		m_pFontApi->DrawText( rect.right - 200.f, y, 0xff22ff22, ">> Model information" );

		strInfo.Format( ">> radius : %f m ", m_pAutoModel->GetRadius() );
		m_pFontApi->DrawText( rect.right - 200.f, y+1*y, 0xff22ff22, strInfo.GetBuffer(0) );

		CBones* pBones = m_pAutoModel->m_pBone;
		
		if( pBones )
		{
			strInfo.Format( ">> Number of Bones : %d ", pBones->m_nMaxBone );

			if( pBones->m_nMaxBone > 28 )
			{
				strInfo += " !!";
				m_pFontApi->DrawText( rect.right - 200.f, y+2*y, 0xffff2222, strInfo.GetBuffer(0) );
			}
			else 
				m_pFontApi->DrawText( rect.right - 200.f, y+2*y, 0xff22ff22, strInfo.GetBuffer(0) );
		}
	}

}

void CMirandaView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	int				nFiles;
    char			szPathName[MAX_PATH];
   	CMirandaDoc*	pDoc = GetDocument();
    
    // 드롭된 파일의 갯수
    nFiles = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, szPathName, MAX_PATH );

	// gmpbigsun : Auto loading 
	::DragQueryFile(hDropInfo, 0, szPathName, MAX_PATH);
	
	if( strstrlwr( szPathName, ".o3d" ) )
	{
		Load_O3D( szPathName );
		CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
		assert( pFrame );

		pFrame->m_kFinal.SetWindowText( szPathName );

		//read lua
		CString strPath = GetResourcePath( );
		CString strFile = GetFileTitle( szPathName );
		strFile += ".lua";
		strPath += "sfx/scripts/";
		strPath += strFile;
		BOOL bReadOK = pFrame->m_kFinal.ReadData( strPath.GetBuffer(0) );
		if( bReadOK )
		{
			if( !pFrame->m_kFinal.IsWindowVisible( ) )
			{
				CRect rectMain, rectSub;
				pFrame->GetWindowRect( &rectMain );
				pFrame->m_kFinal.GetWindowRect( &rectSub );
				pFrame->m_kFinal.MoveWindow( rectMain.left, rectMain.bottom - 100, rectSub.Width(), rectSub.Height() );
				pFrame->m_kFinal.ShowWindow( SW_SHOWNORMAL );
			}
 
	//		pFrame->SetFocus( );
		}
	}
	else if( strstrlwr( szPathName, ".sfx" ) )
		Load_SFX( szPathName ); 
		

	::DragFinish(hDropInfo);

	CView::OnDropFiles(hDropInfo);
    
//     for(int i = nFiles-1; i >= 0; i--)
//     {
//         // 파일의 경로 얻어옴
//         ::DragQueryFile(hDropInfo, i, szPathName, MAX_PATH);
// 		// 파일 이름만 얻음
// 		CString FileName((LPCSTR)szPathName);
// 		FileName = FileName.Right(FileName.GetLength() - FileName.ReverseFind('\\') - 1);
// 		// 확장자 얻음
// 		CString	ExtName(FileName);
// 		ExtName = ExtName.Right(ExtName.GetLength() - ExtName.Find('.') - 1);
// 		
// 		if(ExtName == "o3d" && pDoc->m_Selected.nType == TYPE_OBJECT)	// 오브젝트 
// 		{
// 			pDoc->m_ObjMgr.AddPart(pDoc->m_Selected.nID, FileName);	
// 		}
// 		else if(ExtName == "chr" && pDoc->m_Selected.nType == TYPE_OBJECT)	// 본
// 		{
// 			pDoc->m_ObjMgr.LoadBone(pDoc->m_Selected.nID, FileName);	
// 		}
// 		else if(ExtName == "ani" && pDoc->m_Selected.nType == TYPE_OBJECT)	// 애니메이션
// 		{
// 			pDoc->m_ObjMgr.LoadAnim(pDoc->m_Selected.nID, FileName);	
// 		}
// 		else if(ExtName == "sfx" && pDoc->m_Selected.nType == TYPE_SFX)	// 이펙트
// 		{
// 			pDoc->m_SfxMgr.AddSfx(pDoc->m_Selected.nID, (LPCSTR)szPathName);
// 		}
// 		else
// 		{
// 			AfxMessageBox( "지원안함" );
// 		}
//     }
//     
//     ::DragFinish(hDropInfo);
// 
// 	CView::OnDropFiles(hDropInfo);
}

void CMirandaView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CMirandaDoc* pDoc = GetDocument();

	m_bIsLBClick = false;

	if(!pDoc->m_Selected.nID) return;
	// TODO: Add your message handler code here and/or call default
	D3DXVECTOR3		v, vPickRayDir, vPickRayOrig, vPos;
	D3DVIEWPORT9	vp;
	D3DXMATRIX		matProj, matView, m, matWorld;
	D3DXPLANE		Plane(0.0f, 1.0f, 0.0f, 0.0f);
	
	
	ReleaseCapture();
	// 피킹 구현 
	m_pd3dDevice->GetViewport(&vp);
	m_Camera.getViewMatrix(&matView);
	D3DXMatrixInverse( &m, NULL, &matView );
	D3DXMatrixIdentity(&matWorld);
	
	v.x = ((  (((point.x-vp.X)*2.0f/vp.Width ) - 1.0f)) - m_matProj._31 ) / m_matProj._11;
	v.y = ((- (((point.y-vp.Y)*2.0f/vp.Height) - 1.0f)) - m_matProj._32 ) / m_matProj._22;
	v.z =  1.0f;

	// 피킹 방향 
    vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	// 카메라 위치 
    vPickRayOrig.x = m._41;
    vPickRayOrig.y = m._42;
    vPickRayOrig.z = m._43;

	// 위를 향하는 경우는 충돌이 생길수가 없으므로 중단한다 
	if(vPickRayDir.y > 0.0f) return;
	// 이제 지형하고 부딪히는 점을 구해야겠지..
	float fTy = (-vPickRayOrig.y) / vPickRayDir.y;
	vPos = vPickRayDir * fTy + vPickRayOrig;

	// 이제 배치한다
	if(pDoc->m_Selected.nType == TYPE_OBJECT && !m_bPlayMode)
	{
		pDoc->m_ObjMgr.DeployObj(pDoc->m_Selected.nID, vPos);
	}
	else if(pDoc->m_Selected.nType == TYPE_SFX)
	{
		pDoc->m_SfxMgr.DeploySfx(pDoc->m_Selected.nID, vPos);
		if(m_bPlayMode)
		pDoc->m_SfxMgr.SetFrame(pDoc->m_Selected.nID, 0);
	}

	CView::OnLButtonUp(nFlags, point);
}




void CMirandaView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	m_bIsFocused = true;
}

void CMirandaView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	m_bIsFocused = false;
}



void CMirandaView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CMirandaView::OnFileSave() 
{
	// TODO: Add your command handler code here
	CMirandaDoc*	pDoc		= GetDocument();
	LPCTSTR			lpszFilter	= "scn file(*.scn)|*.scn|scn file|*.*||";
 
	/// 파일 공용컨트롤 대화상자 인스턴스 생성.. 첫번째 인자가 TRUE 이면 읽기 FALSE 이면 쓰기.
	CFileDialog dlg(FALSE,".scn",NULL,OFN_HIDEREADONLY | 
					OFN_OVERWRITEPROMPT,lpszFilter);
	
	dlg.m_ofn.lpstrInitialDir=".\\";
	if(dlg.DoModal()==IDOK) 
	{
		pDoc->OnSaveDocument(dlg.GetFileName());
	}
}

void CMirandaView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CMirandaDoc*	pDoc		= GetDocument();
	LPCTSTR			lpszFilter	= "scn file(*.scn)|*.scn|scn file|*.*||";

	/// 파일 공용컨트롤 대화상자 인스턴스 생성.. 첫번째 인자가 TRUE 이면 읽기 FALSE 이면 쓰기.
	CFileDialog dlg(TRUE,".scn",NULL,OFN_HIDEREADONLY | 
					OFN_OVERWRITEPROMPT,lpszFilter);

	dlg.m_ofn.lpstrInitialDir=".\\";
	if(dlg.DoModal()==IDOK) 
	{
		pDoc->OnOpenDocument(dlg.GetFileName());
	}
}

void CMirandaView::Load_MTE_Texture1( const TCHAR* szModelName )
{
	SAFE_RELEASE( g_pReflectMap );

	LPCTSTR title = GetFileTitle( szModelName );
	CString tgaName = CString( title ) + CString( "-ef01.tga" );

	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( DIR_SFXTEX, tgaName.GetBuffer(0) ), &g_pReflectMap ) ) )
		AfxMessageBox( "failed loading effect map" );
}

void CMirandaView::Load_MTE_Texture2( const TCHAR* szModelName )	// Multitexture effect 제 2번 texture 
{
	SAFE_RELEASE( g_pReflectMap2 );

	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, MakePath( DIR_SFXTEX, szModelName ), &g_pReflectMap2 ) ) )
		AfxMessageBox( "failed loading effect map2" );
}