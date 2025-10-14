// SFXEditorView.cpp : implementation of the CSFXEditorView class
//

#include "stdafx.h"
#include "sfxbase.h"
#include "SFXEditor.h"

#include "MainFrm.h"
#include "SFXEditorDoc.h"
#include "SFXEditorView.h"
#include "childfrm.h"
#include "InfoDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorView

IMPLEMENT_DYNCREATE(CSFXEditorView, CView)

BEGIN_MESSAGE_MAP(CSFXEditorView, CView)
	//{{AFX_MSG_MAP(CSFXEditorView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_PLAY, OnEditPlay)
	ON_COMMAND(ID_EDIT_STOP, OnEditStop)
	ON_COMMAND(ID_VIEWCHRMODEL, OnViewchrmodel)
	ON_UPDATE_COMMAND_UI(ID_VIEWCHRMODEL, OnUpdateViewchrmodel)
	ON_COMMAND(ID_VIEWAXIS, OnViewaxis)
	ON_UPDATE_COMMAND_UI(ID_VIEWAXIS, OnUpdateViewaxis)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_MODEL_SETTING, OnModelSetting)
	ON_COMMAND(ID_VIEWBG, OnViewbg)
	ON_UPDATE_COMMAND_UI(ID_VIEWBG, OnUpdateViewbg)
	ON_COMMAND(ID_FLAT, OnFlat)
	ON_UPDATE_COMMAND_UI(ID_FLAT, OnUpdateFlat)
	ON_COMMAND(ID_INFO, OnInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorView construction/destruction

CSFXEditorView::CSFXEditorView()
{
	m_hwndRenderWindow = NULL;
	m_hwndRenderFullScreen = NULL;
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE; 
	m_testx=m_testy=0;
	m_fCamerax=0;
	m_fCameray=0;
	m_vCameraPos=D3DXVECTOR3(0,-1,10);

	m_bLBut=m_bMBut=m_bRBut=FALSE;

	m_nGridCount=0;
	for(int i=-10;i<11;i++) {
		if(i==0) continue;
		m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(-10.0f,.0f,float(i));
		m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(10.0f,.0f,float(i));
	}
	for(int i=-10;i<11;i++) {
		if(i==0) continue;
		m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(float(i),.0f,-10.0f);
		m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(float(i),.0f,10.0f);
	}
	m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(.0f,.0f,-10.0f);
	m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(.0f,.0f,10.0f);
	m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(-10.0f,.0f,.0f);
	m_vGrids[m_nGridCount++].vPos=D3DXVECTOR3(10.0f,.0f,.0f);


	CSfxBase* pSfxBase=new CSfxBase();
	g_SfxMng.AddSfxBase(pSfxBase);
	m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));
	m_vObjPos=D3DXVECTOR3(.0f,.0f,.0f);
	m_fObjAngle=.0f;
	m_bPlay = FALSE;

#ifdef __ACENTERMODEL
	m_bChrModel = TRUE;
	m_bModelPlay = FALSE;
#endif	
#ifdef __AXYZ
	m_bAxis     = TRUE;
#endif
	m_bBG		= TRUE;
	m_bFlat     = FALSE;

	m_Flat[0].m_v3Pos = D3DXVECTOR3( -10, 0, -10 );
	m_Flat[1].m_v3Pos = D3DXVECTOR3( -10, 0, 10 );
	m_Flat[2].m_v3Pos = D3DXVECTOR3(  10, 0, 10 );
	m_Flat[3].m_v3Pos = D3DXVECTOR3(  10, 0, -10 );

	m_bIsModified = false;
	m_nFileType   = NEW_FILE;

}

CSFXEditorView::~CSFXEditorView()
{
#ifdef __ACENTERMODEL
	SAFE_DELETE( m_pModel );
#endif

	SAFE_DELETE( m_2DRender.m_pFont );
}


void CSFXEditorView::SetModified(bool bIsMod)
{

	m_bIsModified = bIsMod;

}


void CSFXEditorView::SetFileType(int nFileType)
{

	m_nFileType = nFileType;

}


bool CSFXEditorView::IsModified() const
{

	return m_bIsModified;

}


int CSFXEditorView::GetFileType() const
{
	return m_nFileType;
}


BOOL CSFXEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


//////////////////////////////////////////////////////////////////////////////

HRESULT CSFXEditorView::AdjustWindowForChange()
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

HRESULT CSFXEditorView::OneTimeSceneInit()
{
    return S_OK;
}

HRESULT CSFXEditorView::FrameMove()
{
	if( m_bModelPlay )
	{
		if( m_pModel )
		{
			if( m_pModel->IsEndFrame() )
			{
				m_bModelPlay = FALSE;
			}
			else
				m_pModel->FrameMove();
		}
	}

	const int nObjCount=g_SfxObjMng.m_apSfxObj.GetSize();
	for(int i=0;i<nObjCount;i++) 
	{
		CSfxModel* pSfxObj = static_cast<CSfxModel*>(g_SfxObjMng.m_apSfxObj.GetAt(i));

		for( int j=0; j<pSfxObj->m_pSfxBase->m_apParts.GetSize(); j++ )
		{
			if( pSfxObj->m_pSfxBase->Part(j)->m_nType != SFXPARTTYPE_MESH )
				continue;

			CModelObject* pMesh = g_SfxMeshMng.Mesh(pSfxObj->m_pSfxBase->Part(j)->m_strTex);		
			if( pMesh )
				pMesh->FrameMove();
		}
		

	}
    return S_OK;
}

HRESULT CSFXEditorView::Render()
{
	CMainFrame* pMain = static_cast<CMainFrame*>(AfxGetMainWnd());
	TRACE("camera %f,%f,%f\n", m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z );
	m_pd3dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, pMain->m_dwColor, 1.0f, 0);

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

	D3DXMATRIX matWorld,matView,matProj,matTemp;

	D3DXMatrixRotationY(&matView,DEGREETORADIAN(m_fCamerax));
	D3DXMatrixRotationX(&matTemp,DEGREETORADIAN(m_fCameray));
	matView*=matTemp;
	D3DXMatrixTranslation(&matTemp,m_vCameraPos.x,m_vCameraPos.y,m_vCameraPos.z);
	matView*=matTemp;

	FLOAT       fAspect = static_cast<FLOAT>(m_d3dpp.BackBufferWidth) / m_d3dpp.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.1f, 5000.0f );

	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	g_matView = matView;
	D3DXMatrixInverse(&g_matInvView,NULL,&g_matView);
	g_matInvView._41 = g_matInvView._42 = g_matInvView._43=.0f;

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene( ) ) )
	{
		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetVertexDeclaration( NULL );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO);
//draw grid
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x404040 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		m_pd3dDevice->SetTexture(0,NULL);
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		if( m_bBG )
		{
			m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, m_nGridCount/2-2, m_vGrids, sizeof(FVF_3DVERTEX) );
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xc0c040 );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 2, &(m_vGrids[m_nGridCount-4]), sizeof(FVF_3DVERTEX) );
		}

		if(m_bFlat)
		{
			m_pd3dDevice->SetFVF( CUSTOM_VERTEX2::FVF );
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0 );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, &m_Flat, sizeof(CUSTOM_VERTEX2) );
		}
		
#ifdef __AXYZ
		if( m_bAxis )
		{
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			
			m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld);

			m_pd3dDevice->SetTexture(0, NULL );
			m_pd3dDevice->SetFVF( CUSTOM_VERTEX::FVF );

			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, m_Allow, sizeof(CUSTOM_VERTEX) );
		}

/*
		// Billboard
		D3DXMATRIX InvView;
		D3DXMatrixInverse(&InvView,NULL,&g_matView);
		InvView._41 = InvView._42 = InvView._43 = 0.0f;
		
		D3DXMATRIX m_World;
		D3DXMATRIX m_Trans;
		D3DXMATRIX m_Scal;

		// X, Y, Z 텍스트 표시
		DWORD dwFlag = D3DFONT_TWOSIDED | D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y | D3DFONT_FILTERED;

		// X
		D3DXMatrixTranslation( &m_Trans, 2.0f, 0.2f, 0.0f );
		D3DXMatrixScaling( &m_Scal, 0.4f, 0.4f, 0.4f );
		m_World = m_Scal * InvView * m_Trans;
		
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_World );
		m_2DRender.m_pFont->Render3DText( "X", 0, dwFlag );
		
		// Y
		D3DXMatrixTranslation( &m_Trans, 0.0f, 2.0f, 0.0f );
		D3DXMatrixScaling( &m_Scal, 0.4f, 0.4f, 0.4f );
		m_World = m_Scal * InvView * m_Trans;
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_World );
		m_2DRender.m_pFont->Render3DText( "Y", 0, dwFlag );
		
		// Z
		D3DXMatrixTranslation( &m_Trans, 0.0f, 0.0f, 2.0f );
		D3DXMatrixScaling( &m_Scal, 0.4f, 0.4f, 0.4f );
		m_World = m_Scal * InvView * m_Trans;
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_World );
		m_2DRender.m_pFont->Render3DText( "Z", 0, dwFlag );
		*/		
		
#endif // __AXYZ

#ifdef __ACENTERMODEL
		if( m_bChrModel )
		{
			static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
			m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
			SetTransformView( matView );
			SetTransformProj( matProj );
			SetDiffuse( 1.0, 1.0, 1.0 );
			SetAmbient( 1.0, 1.0, 1.0 );
				
			//m_pModel->FrameMove();
				
			if(m_pModel)
			{
				D3DXMATRIX mWorld;
				D3DXMATRIX mTrans;
				D3DXMATRIX mScal;
				D3DXMATRIX mRot;

				D3DXMatrixIdentity( &mTrans );
				D3DXMatrixIdentity( &mScal );
				D3DXMatrixIdentity( &mRot );

				CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
				D3DXMatrixRotationYawPitchRoll( &mRot, DEGREETORADIAN(pMain->m_vRot.y),
														DEGREETORADIAN(pMain->m_vRot.x), 
														DEGREETORADIAN(pMain->m_vRot.z) );

				mWorld = mScal * mRot * mTrans;

				m_pModel->Render(m_pd3dDevice, &mWorld);
			}

			/*
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_pModel->Render(m_pd3dDevice, &matWorld);
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			m_pModel->Render(m_pd3dDevice, &matWorld);
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			*/
		}
			
#endif
#if 1
		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetVertexDeclaration( NULL );		
		
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		
		
		if( g_SfxObjMng.m_apSfxObj.GetSize() > 0 ) 
		{
			g_SfxObjMng.Process();
			g_SfxObjMng.Render();
		}
		else 
		{
			//KillTimer( 1 );
			m_bPlay = FALSE;
			CFramePanel* pFramePanel=&(((CChildFrame*)GetParent())->m_FramePanel);
			if( m_bPlay == FALSE )
				m_SfxObj.m_nCurFrame = pFramePanel->m_nCurFrame;
//			m_SfxObj.Render();
		//	m_SfxObj.SetFrame( m_SfxObj.m_nCurFrame );
			m_SfxObj.Render2( m_pd3dDevice, NULL );//m_SfxObj.m_vPos, m_SfxObj.m_nCurFrame, m_SfxObj.m_vRotate.z );
			//m_SfxObj.m_pSfxBase->Render( m_SfxObj.m_vPos, m_SfxObj.m_nCurFrame, m_SfxObj.m_vRotate.z );
		
			if( pFramePanel->m_SelectedKeys.GetSize() == 1 ) 
			{
				m_SfxObj.m_pSfxBase->RenderOutLine( m_SfxObj.m_vPos, m_SfxObj.m_nCurFrame, m_SfxObj.m_vRotate.z, pFramePanel->m_nCurPart );
				m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff0000 );
				FVF_3DVERTEX vTemp[ 8 ];
				D3DXVECTOR3 vTemp1 = ( (SfxKeyFrame*)pFramePanel->m_SelectedKeys[ 0 ] )->vPos;

				D3DXMATRIX matTemp,matTemp1;
	/*
				D3DXMatrixRotationZ(&matTemp,DEGREETORADIAN(pFramePanel->m_pKey->vPosRotate.z));
				D3DXMatrixRotationX(&matTemp1,DEGREETORADIAN(pFramePanel->m_pKey->vPosRotate.x)); matTemp*=matTemp1;
				D3DXMatrixRotationY(&matTemp1,DEGREETORADIAN(pFramePanel->m_pKey->vPosRotate.y)); matTemp*=matTemp1;
	*/
				D3DXVECTOR3 vTemp2=DEGREETORADIAN(((SfxKeyFrame*)pFramePanel->m_SelectedKeys[0])->vPosRotate);
				D3DXMatrixRotationYawPitchRoll(&matTemp,vTemp2.y,vTemp2.x,vTemp2.z);
				D3DXVec3TransformCoord(&vTemp1,&(((SfxKeyFrame*)pFramePanel->m_SelectedKeys[0])->vPos),&matTemp);

				vTemp[0].vPos=vTemp1;
				vTemp[1].vPos=D3DXVECTOR3(vTemp1.x,vTemp1.y,0);
				vTemp[2].vPos=D3DXVECTOR3(vTemp1.x,0,0);
				vTemp[3].vPos=D3DXVECTOR3(vTemp1.x,0,vTemp1.z);
				vTemp[4].vPos=D3DXVECTOR3(0,0,vTemp1.z);
				vTemp[5].vPos=D3DXVECTOR3(0,vTemp1.y,vTemp1.z);
				vTemp[6].vPos=vTemp[0].vPos;
				vTemp[7].vPos=vTemp[3].vPos;

				m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld);
				m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 7,vTemp,sizeof(FVF_3DVERTEX));
			}
		}

#endif
//		m_2DRender.RenderRect( CRect(m_testx-5,m_testy-5,m_testx+5,m_testy+5), 0x80ffffff );

		// End the scene.
		m_pd3dDevice->EndScene();
	}

    return S_OK;
}

HRESULT CSFXEditorView::InitDeviceObjects()
{
	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);
	m_2DRender.InitDeviceObjects( m_pd3dDevice );
//	m_2DRender.m_pFont->InitDeviceObjects( m_pd3dDevice );
	//m_2DRender.m_pFont = new CD3DFont( _T("Arial"), 9, D3DFONT_BOLD );
	m_2DRender.SetFont( m_2DRender.m_pFont );
//	g_SfxTex.AddTex("default.tga");

	CTexPanel* pTexPanel=&(((CChildFrame*)GetParent())->m_WorkPanel.m_TexPanel);
	pTexPanel->LoadTexture("default.tga");
	
#ifdef __AXYZ
    m_Allow[0].m_v3Pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_Allow[1].m_v3Pos = D3DXVECTOR3( 0.0f, 1.5f, 0.0f );
    m_Allow[2].m_v3Pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_Allow[3].m_v3Pos = D3DXVECTOR3( 1.5f, 0.0f, 0.0f );
    m_Allow[4].m_v3Pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_Allow[5].m_v3Pos = D3DXVECTOR3( 0.0f, 0.0f, 1.5f );
    
    m_Allow[0].m_dwColor = D3DCOLOR_XRGB( 0, 255, 0 );
    m_Allow[1].m_dwColor = D3DCOLOR_XRGB( 0, 255, 0 );
    m_Allow[2].m_dwColor = D3DCOLOR_XRGB( 255, 0, 0 );
    m_Allow[3].m_dwColor = D3DCOLOR_XRGB( 255, 0, 0 );
    m_Allow[4].m_dwColor = D3DCOLOR_XRGB( 0, 0, 255 );
    m_Allow[5].m_dwColor = D3DCOLOR_XRGB( 0, 0, 255 );
#endif // __AXYZ

#ifdef __ACENTERMODEL
	m_pModel = new CModelObject;
	m_pModel->InitDeviceObjects(m_pd3dDevice) ;
	m_pModel->LoadBone( "mvr_female.chr" );
	m_pModel->LoadElement("Part_femaleHair01.o3d", 0 );
	m_pModel->LoadElement("Part_femaleHand.o3d",   1 );
	m_pModel->LoadElement("Part_femaleHead01.o3d", 2 );
	m_pModel->LoadElement("Part_femaleLower.o3d",  3 );
	m_pModel->LoadElement("Part_femaleUpper.o3d",  4 );
	m_pModel->LoadElement("Part_femaleFoot.o3d",   5 );
	m_pModel->LoadMotion( "Mvr_female_GenStand.ani" );
#endif
	
	DragAcceptFiles( TRUE );

    return S_OK;
}

HRESULT CSFXEditorView::RestoreDeviceObjects()
{
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) || ( m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
		//CreateSkinningVS( m_pd3dDevice, _T("skining.vsh") );
		CreateSkinningVS( m_pd3dDevice, _T("skining2.vsh") );
    }

	m_2DRender.RestoreDeviceObjects(&m_d3dsdBackBuffer);
	g_SfxMng.RestoreDeviceObjects();
    RenderScene( FALSE );
    Invalidate();

    return S_OK;
}

HRESULT CSFXEditorView::InvalidateDeviceObjects()
{
	m_2DRender.InvalidateDeviceObjects();
	g_SfxMng.InvalidateDeviceObjects();
    return S_OK;
}

HRESULT CSFXEditorView::DeleteDeviceObjects()
{
	m_2DRender.DeleteDeviceObjects();
	g_SfxMng.DeleteDeviceObjects();
    return S_OK;
}

HRESULT CSFXEditorView::FinalCleanup()
{
    return S_OK;
}

HRESULT CSFXEditorView::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
                                 D3DFORMAT Format )
{
    return S_OK;
}

HRESULT CSFXEditorView::CheckForLostFullscreen()
{
    HRESULT hr;
	
    if( m_bWindowed )
        return S_OK;
	
    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();
	
    return S_OK;
}

BOOL CSFXEditorView::CreateDevice() 
{
	m_hwndRenderWindow = CView::GetSafeHwnd();

	// Note that for the MFC samples, the device window and focus window
	// are not the same.
	CD3DApplication::m_hWnd = m_hwndRenderWindow;
	CD3DApplication::m_hWndFocus = NULL;//m_hwndRenderFullScreen;
	CD3DApplication::Create( AfxGetInstanceHandle() );

	// Update UI, and device's fog parameters
	//OnVertexFog();
	//UpdateUIForDeviceCapabilites();
	//SetFogParameters();

	//if( m_bFrameMoving )
	{
		DXUtil_Timer( TIMER_RESET );
		SetTimer( 1, 10, NULL );
	}
	// 자동 세이브 타이머 설정 5분 
	//SetTimer(2,1000*60*5,NULL);
//	SetTimer( 2, 1000 * 60 * 1, NULL );
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// CSFXEditorView drawing

void CSFXEditorView::OnDraw(CDC* pDC)
{
	CSFXEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
/*
	if(IsReady())
	{
		CheckForLostFullscreen();
		RenderScene( FALSE );
	}
*/
//	pDC->TextOut(0,0,pDoc->m_strTest);
}

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorView diagnostics

#ifdef _DEBUG
void CSFXEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSFXEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSFXEditorDoc* CSFXEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSFXEditorDoc)));
	return (CSFXEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorView message handlers

int CSFXEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	((CChildFrame*)GetParent())->m_pView=this;
	((CChildFrame*)GetParent())->m_pDoc=GetDocument();

	return 0;
}

void CSFXEditorView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

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
			// A new window size will require a new backbuffer
			// size, so the 3D structures must be changed accordingly.
		//	m_bReady = FALSE;
			//Pause( TRUE );

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

void CSFXEditorView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	CreateDevice() 	;
	
}

void CSFXEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_testx=point.x;
	m_testy=point.y;

	if(m_bRBut) 
	{
		m_fCamerax+=((float)m_ptRClicked.x-(float)point.x)/5.0f;
		m_fCameray+=((float)m_ptRClicked.y-(float)point.y)/5.0f;
		m_ptRClicked=point;
		RenderScene( FALSE );
	}
	if(m_bMBut) 
	{
		D3DXVECTOR3 vTemp((float)point.x-(float)m_ptMClicked.x,(float)m_ptMClicked.y-(float)point.y,0);
		m_vCameraPos+=vTemp/100.0f;
		m_ptMClicked=point;
		TRACE("camera2 %f,%f,%f\n", m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z );
		RenderScene( FALSE );
	}
	if(m_bLBut) 
	{
		CFramePanel* pFramePanel=&(((CChildFrame*)GetParent())->m_FramePanel);
		if(pFramePanel->m_SelectedKeys.GetSize()) 
		{
			D3DXVECTOR3 vTemp;
			D3DXMATRIX matTemp;
//			D3DXMatrixInverse(&g_matInvView,NULL,&g_matView);
//			if(nFlags&MK_SHIFT) vTemp=D3DXVECTOR3((FLOAT)point.x-(FLOAT)m_ptLClicked.x,0,(FLOAT)m_ptLClicked.y-(FLOAT)point.y)*0.01;
//			else vTemp=D3DXVECTOR3((FLOAT)point.x-(FLOAT)m_ptLClicked.x,(FLOAT)m_ptLClicked.y-(FLOAT)point.y,0)*0.01;
			if(nFlags&MK_SHIFT) vTemp=D3DXVECTOR3((FLOAT)point.x-(FLOAT)m_ptLClicked.x,(FLOAT)m_ptLClicked.y-(FLOAT)point.y,0.0f)*0.01f;
			else vTemp=D3DXVECTOR3((FLOAT)point.x-(FLOAT)m_ptLClicked.x,0,(FLOAT)m_ptLClicked.y-(FLOAT)point.y)*0.01f;
//			g_matInvView._41=g_matInvView._42=g_matInvView._43=0;
			D3DXVec3TransformCoord(&vTemp,&vTemp,&g_matInvView);
//			if(!(nFlags&MK_SHIFT)) vTemp.y=0;
			if(nFlags&MK_SHIFT) vTemp.x=vTemp.z=0;
			else vTemp.y=0;
			((SfxKeyFrame*)pFramePanel->m_SelectedKeys[0])->vPos+=vTemp;
			pFramePanel->UpdatePanel();
			m_ptLClicked=point;
			RenderScene( FALSE );
		}
	}
	//Invalidate( FALSE );
	CView::OnMouseMove(nFlags, point);
}

void CSFXEditorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//Invalidate(FALSE);
	//if(IsReady())
	{
		CheckForLostFullscreen();
		RenderScene( m_bPlay );
	}
	//((CChildFrame*)(GetParent()))->m_FramePanel.Invalidate(FALSE);
	//m_SfxObj.m_nCurFrame++;
	//if( m_SfxObj.SetFrame( m_SfxObj.m_nCurFrame ) )
	//	m_SfxObj.m_nCurFrame = 0;
	CView::OnTimer(nIDEvent);
}

void CSFXEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLBut=TRUE;
	m_ptLClicked=point;
	SetCapture();
	Invalidate( FALSE );
	
	CView::OnLButtonDown(nFlags, point);
}

void CSFXEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bRBut=TRUE;
	m_ptRClicked=point;
	SetCapture();
	Invalidate( FALSE );
	
	CView::OnRButtonDown(nFlags, point);
}
void CSFXEditorView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bMBut=TRUE;
	m_ptMClicked=point;
	SetCapture();
	Invalidate( FALSE );
	
	CView::OnMButtonDown(nFlags, point);
}

BOOL CSFXEditorView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	
	return CView::OnEraseBkgnd(pDC);
}

void CSFXEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLBut=FALSE;
	ReleaseCapture();
	
	CView::OnLButtonUp(nFlags, point);
}

void CSFXEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bRBut=FALSE;
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}
void CSFXEditorView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bMBut=FALSE;
	ReleaseCapture();
	
	CView::OnMButtonUp(nFlags, point);
}

BOOL CSFXEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	m_vCameraPos.z+=(float)zDelta/400.0f;
	BOOL bResult = CView::OnMouseWheel(nFlags, zDelta, pt);
	RenderScene( FALSE );
	return bResult;
}

void CSFXEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CFramePanel* pFramePanel=&(((CChildFrame*)GetParent())->m_FramePanel);
	
	// 키제거 
	if( g_SfxObjMng.m_apSfxObj.GetSize() == 0 )
	{
		if( nChar == VK_DELETE ) 
		{
			CSfxPart* pPart = m_SfxObj.m_pSfxBase->Part( pFramePanel->m_nCurPart );
			if( pPart ) pPart->DeleteKeyFrame( pFramePanel->m_nCurFrame );
			pFramePanel->UpdatePanel();
		}
		// 레이어 삭제 (모든 키를 제거한 후에만 가능함)
		if( nChar == VK_END && m_SfxObj.m_pSfxBase->m_apParts.GetSize() ) 
		{
			m_SfxObj.RemovePart( pFramePanel->m_nCurPart );
			/*
			CSfxPart* pPart = m_SfxObj.m_pSfxBase->Part( pFramePanel->m_nCurPart );
			pPart->DeleteAllKeyFrame();
			m_SfxObj.m_pSfxBase->DeletePart( pFramePanel->m_nCurPart );
			m_SfxObj.m
			*/
			if( pFramePanel->m_nCurPart >= m_SfxObj.m_pSfxBase->m_apParts.GetSize() )
			{
				pFramePanel->m_nCurPart = m_SfxObj.m_pSfxBase->m_apParts.GetSize() - 1;
				if( pFramePanel->m_nCurPart < 0 ) pFramePanel->m_nCurPart = 0;
			}
			pFramePanel->m_pSfxPart = m_SfxObj.m_pSfxBase->Part( pFramePanel->m_nCurPart );
			pFramePanel->m_SelectedKeys.RemoveAll();
			pFramePanel->UpdatePanel();

		}
	}
	// 플레이 
	if( nChar == 191 )  // '/'
	{
		m_bModelPlay = TRUE;
		m_pModel->m_bEndFrame = FALSE;
		m_pModel->m_fFrameCurrent = 0;

		g_SfxObjMng.AddObj( m_SfxObj.m_pSfxBase, D3DXVECTOR3( 1, 0, 0) );
		if( m_bPlay == FALSE )
		{
			//DXUtil_Timer( TIMER_RESET );
			//SetTimer( 1, 10, NULL );
		}
		
		int nObjCount=g_SfxObjMng.m_apSfxObj.GetSize();

		CSfxModel* pSfxObj = NULL;

		for(int i=0;i<nObjCount;i++) 
		{
			pSfxObj=((CSfxModel*)(g_SfxObjMng.m_apSfxObj.GetAt(i)));

			for( int j=0; j<pSfxObj->m_pSfxBase->m_apParts.GetSize(); j++ )
			{
				if( pSfxObj->m_pSfxBase->Part(j)->m_nType != SFXPARTTYPE_MESH )
					continue;

				CModelObject* pMesh = NULL;
				
				pMesh = g_SfxMeshMng.Mesh( pSfxObj->m_pSfxBase->Part(j)->m_strTex );

				if( pMesh )
				{
					pMesh->m_bEndFrame = FALSE;
					pMesh->m_fFrameCurrent = 0;
				}
			}
		}

		//OnEditPlay(); 
	}
	if(nChar=='J') 
	{
		m_vObjPos.x-=0.1f;
	}
	if(nChar=='L') {
		m_vObjPos.x+=0.1f;
	}
	if(nChar=='I') 
	{
		m_vObjPos.z+=0.1f;
	}
	if(nChar=='K') 
	{
		m_vObjPos.z-=0.1f;
	}
	if(nChar=='U') 
	{
		m_fObjAngle-=10.0f;
	}
	if(nChar=='O') 
	{
		m_fObjAngle+=10.0f;
	}
	if(nChar=='1') 
	{
		pFramePanel->m_nGapLevel=0;
		pFramePanel->UpdatePanel();
	}
	if(nChar=='2') 
	{
		pFramePanel->m_nGapLevel=1;
		pFramePanel->UpdatePanel();
	}
	if(nChar=='3') 
	{
		pFramePanel->m_nGapLevel=2;
		pFramePanel->UpdatePanel();
	}
	int nObjCount=g_SfxObjMng.m_apSfxObj.GetSize();
	CSfxModel* pSfxObj;
	for(int i=0;i<nObjCount;i++) 
	{
		pSfxObj=((CSfxModel*)(g_SfxObjMng.m_apSfxObj.GetAt(i)));
		pSfxObj->m_vPos=m_vObjPos;
		pSfxObj->m_vRotate.y=m_fObjAngle;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSFXEditorView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
//	if(IsReady())
	{
		CheckForLostFullscreen();
		RenderScene( m_bPlay );
	}
	
	// Do not call CView::OnPaint() for painting messages
}

void CSFXEditorView::OnEditPlay() 
{
	//g_SfxObjMng.AddObj( m_SfxObj.m_pSfxBase, D3DXVECTOR3( 1, 0, 0) );
	if( m_bPlay == FALSE )
	{
		m_bPlay = TRUE;
		//DXUtil_Timer( TIMER_RESET );
		//SetTimer( 1, 10, NULL );
	}
	else
	{
		//KillTimer( 1 );
		m_bPlay = FALSE;
	}
}

void CSFXEditorView::OnEditStop() 
{
	m_bPlay = FALSE;
}
/*
void CSFXEditorView::OnChrmodel() 
{
	// TODO: Add your command handler code here
	
}
*/
void CSFXEditorView::OnViewchrmodel() 
{
#ifdef __ACENTERMODEL
	m_bChrModel = !m_bChrModel;
#endif
}

void CSFXEditorView::OnUpdateViewchrmodel(CCmdUI* pCmdUI) 
{
#ifdef __ACENTERMODEL
	(m_bChrModel) ? pCmdUI->SetCheck(1): pCmdUI->SetCheck(0);
#endif
}

void CSFXEditorView::OnViewaxis() 
{
#ifdef __AXYZ
	m_bAxis = !m_bAxis;
#endif
}

void CSFXEditorView::OnUpdateViewaxis(CCmdUI* pCmdUI) 
{
#ifdef __AXYZ
	(m_bAxis) ? pCmdUI->SetCheck(1): pCmdUI->SetCheck(0);
#endif
}

void CSFXEditorView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char	szFileName[256];
	
	DragQueryFile( hDropInfo ,0 ,  szFileName ,  256 );

	g_SfxObjMng.RemoveAll();
	CString filename=szFileName;
	filename.MakeLower();
	CString szTemp = filename;
	szTemp = szTemp.Right(szTemp.GetLength() - szTemp.ReverseFind('\\') - 1);
	// 확장자 얻음
	CString	ExtName(szTemp);
	ExtName = ExtName.Right(ExtName.GetLength() - ExtName.Find('.') - 1);
	
	if(ExtName == "sfx")
	{
		g_SfxMng.GetSfxBase(0)->m_strName = filename;

		g_SfxMng.GetSfxBase(0)->DropFileLoad();
	//	g_SfxMng.GetSfxBase(0)->m_strName = filename.Left(filename.Find( '.', 0 ));
	//	g_SfxMng.GetSfxBase(0)->m_strName = g_SfxMng.GetSfxBase(0)->m_strName.Right( g_SfxMng.GetSfxBase(0)->m_strName.GetLength() - g_SfxMng.GetSfxBase(0)->m_strName.ReverseFind('\\')-1 );
		m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));
		CFramePanel* pFrame=&((   (CChildFrame*)(GetParent()) )->m_FramePanel);
		pFrame->m_SelectedKeys.RemoveAll();
		pFrame->m_nCurFrame=pFrame->m_nLCFrame=pFrame->m_nLeftFrame=0;
		pFrame->m_nCurPart=pFrame->m_nLCPart=pFrame->m_nTopObj=0;
		pFrame->m_pClickedKey=NULL;
		
	#ifdef __AALLSELECT
		pFrame->m_bLayerDragDrop = FALSE;
		pFrame->m_nSfxPartClipBoard = -1;
	#endif
		
		pFrame->UpdatePanel();

		CString strTitle;
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		strTitle.Format("%s", g_SfxMng.GetSfxBase(0)->m_strName );
		pMain->SetWindowText(strTitle);
		
		SetModified(false);
		SetFileType(LOAD_FILE);
	}
	
	CView::OnDropFiles(hDropInfo);
}

void CSFXEditorView::OnModelSetting() 
{
	// TODO: Add your command handler code here
	CModelSettingDlg dlg;

	dlg.m_pView = this;
	dlg.DoModal();
	
}

void CSFXEditorView::OnViewbg() 
{
	// TODO: Add your command handler code here
	m_bBG = !m_bBG;
}

void CSFXEditorView::OnUpdateViewbg(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(m_bBG) ? pCmdUI->SetCheck(1): pCmdUI->SetCheck(0);
	
}

void CSFXEditorView::OnFlat() 
{
	// TODO: Add your command handler code here
	m_bFlat = !m_bFlat;
}

void CSFXEditorView::OnUpdateFlat(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(m_bFlat) ? pCmdUI->SetCheck(1): pCmdUI->SetCheck(0);
	
}

void CSFXEditorView::OnInfo() 
{
	// TODO: Add your command handler code here
	CInfoDlg dlg;
	
	dlg.DoModal();
	
}


void  CSFXEditorView::OnDestroy()
{

	if(IsModified())
	{
		int nRet = this->MessageBox("저장되지 않은 파일이 있습니다. \n 작업하던 파일을 저장하시겠습니까?", "알림",MB_OKCANCEL);
   

		if(nRet == IDOK)
		{
			if(GetFileType() == NEW_FILE)
			{
				CFileDialog dlgFile(FALSE);
	
				dlgFile.m_ofn.lpstrInitialDir=DIR_SFX;
				dlgFile.m_ofn.lpstrTitle=_T("세이브");
				dlgFile.m_ofn.Flags|=OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN ;
				dlgFile.m_ofn.lpstrDefExt = "sfx";
				dlgFile.m_ofn.lpstrFilter = "Sfx Files (*.sfx)\0*.sfx\0All Files\0*.*\0";

				if(dlgFile.DoModal()==IDOK) 
				{
					CString str = dlgFile.GetPathName();
					CString filename=dlgFile.GetPathName();
					filename.MakeLower();
					g_SfxMng.GetSfxBase(0)->m_strName=filename;
					g_SfxMng.GetSfxBase(0)->Save();
				}	
			}
			else
			{
				g_SfxMng.GetSfxBase(0)->Save();
			}
		}
	}
}
