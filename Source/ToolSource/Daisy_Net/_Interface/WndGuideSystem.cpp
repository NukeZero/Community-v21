// GuideSystem.cpp: implementation of the CGuideSystem class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "define.h"
#include "definetext.h"
#include "defineSound.h"
#include "resdata.h"
#include "WndGuideSystem.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __YGUIDESYSTEM0923

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndGuideSystem::CWndGuideSystem()
{
	m_bIsLoad = FALSE;
	m_pModel = NULL;

	m_wndGuideText = NULL;
	m_wndGuideSelection = NULL;
}

CWndGuideSystem::~CWndGuideSystem()
{
	m_pModel->DeleteDeviceObjects();

	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_wndGuideText);
	SAFE_DELETE(m_wndGuideSelection);
}

BOOL CWndGuideSystem::Initialize( CWndBase* pWndParent, DWORD nType )
{	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE, WBS_TOPMOST, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndGuideSystem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndBase* pWndBase = (CWndBase*) pLResult;
	
	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		m_wndMenuPlace.CheckMenuItem( 0, FALSE );
		m_wndMenuPlace.CheckMenuItem( 1, FALSE );
		
		SetFocus();
		switch( nID )
		{
		case 0:
			{
				if(g_pPlayer)
					SetAni( g_pPlayer->GetJob(), ANI_BYTE );

#ifdef __YGUIDE_BUG_FIX
				if( m_wndGuideText->m_VecGuideText.size() )
					m_wndGuideText->m_bVisible = FALSE;
#endif //__YGUIDE_BUG_FIX
				
			}
			break;
		case 1:
			if( m_wndGuideText->m_VecGuideText.size() )
				m_wndGuideText->m_bVisible = !m_wndGuideText->m_bVisible;
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndGuideSystem::OnDraw( C2DRender* p2DRender )
{
	if( m_pModel == NULL )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -5.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	{
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		CRect rect = GetClientRect();

		viewport.X      = p2DRender->m_ptOrigin.x + rect.left;//2;
		viewport.Y      = p2DRender->m_ptOrigin.y + rect.top;//5;
		viewport.Width  = rect.Width();
		viewport.Height = rect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		FLOAT fov = D3DX_PI/4.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		D3DXMatrixScaling(&matScale, 2.8f, 2.8f, 2.8f);
		
		D3DXMatrixTranslation(&matTrans,0.0f,-0.8f,0.0f);

		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
		
#ifdef __YENV	
		D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
		SetLightVec( vDir );
	
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		
		::SetTransformView( matView );
		::SetTransformProj( matProj );
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
}
void CWndGuideSystem::PaintFrame( C2DRender* p2DRender )
{
}
void CWndGuideSystem::ChangeModel( int nJob )
{
	SAFE_DELETE(m_pModel);
	m_pModel = new CModelObject;
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	if( m_pModel )
	{
		switch( nJob )
		{
			default:
			case JOB_VAGRANT:
				{
					m_pModel->LoadBone( "Mvr_Guidepang.chr" );
					m_pModel->LoadElement("Mvr_Guidepang.o3d", 0 );
				}
				break;
			case JOB_KNIGHT:
			case JOB_BLADE:
			case JOB_MERCENARY:
				{
					m_pModel->LoadBone( "Mvr_McGuidepang.chr" );
					m_pModel->LoadElement("Mvr_McGuidepang.o3d", 0 );
				}
				break;
			case JOB_RINGMASTER:
			case JOB_BILLPOSTER:
			case JOB_ASSIST:
				{
					m_pModel->LoadBone( "Mvr_AsGuidepang.chr" );
					m_pModel->LoadElement("Mvr_AsGuidepang.o3d", 0 );
				}
				break;
			case JOB_PSYCHIKEEPER:
			case JOB_ELEMENTOR:
			case JOB_MAGICIAN:
				{
					m_pModel->LoadBone( "Mvr_MgGuidepang.chr" );
					m_pModel->LoadElement("Mvr_MgGuidepang.o3d", 0 );
				}
				break;
			case JOB_ACROBAT:
			case JOB_RANGER:
			case JOB_JESTER:
				{
					m_pModel->LoadBone( "Mvr_AcrGuidepang.chr" );
					m_pModel->LoadElement("Mvr_AcrGuidepang.o3d", 0 );
				}
				break;				
		}
	}
}	

void CWndGuideSystem::SetAni(int nJob, int nAniKind)
{
	if( m_pModel == NULL )
		return;
		
	CString filename;
	char* VagArry[3] = { "Mvr_Guidepang_Appear.ani", "Mvr_Guidepang_Default.ani", "Mvr_Guidepang_Disappear.ani"	};
	char* MerArry[3] = { "Mvr_McGuidepang_appear.ani", "Mvr_McGuidepang_default.ani", "Mvr_McGuidepang_Disappear.ani"	};
	char* AssArry[3] = { "Mvr_AsGuidepang_Appear.ani", "Mvr_AsGuidepang_Default.ani", "Mvr_AsGuidepang_Disappear.ani"	};
	char* MagArry[3] = { "Mvr_MgGuidepang_Appear.ani", "Mvr_MgGuidepang_Dafault.ani", "Mvr_MgGuidepang_DisAppear.ani"	};
	char* AcrArry[3] = { "Mvr_AcrGuidepang_Appear.ani", "Mvr_AcrGuidepang_Default.ani", "Mvr_AcrGuidepang_DisAppear.ani"	};
	
	switch( nJob )
	{
	default:
	case JOB_VAGRANT:
		{
			filename = VagArry[nAniKind];
		}
		break;
	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_MERCENARY:
		{
			filename = MerArry[nAniKind];
		}
		break;
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_ASSIST:
		{
			filename = AssArry[nAniKind];
		}
		break;
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_MAGICIAN:
		{
			filename = MagArry[nAniKind];
		}
		break;
	case JOB_ACROBAT:
	case JOB_RANGER:
	case JOB_JESTER:
		{
			filename = AcrArry[nAniKind];
		}
		break;
		
	}
	
	m_pModel->LoadMotion( filename );
	
	switch(nAniKind)
	{
	case ANI_INTRO:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_CONT );
			m_bAniState = ANI_INTRO;					
		}
		break;
	case ANI_IDLE:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_LOOP );
			m_bAniState = ANI_IDLE;	
		}
		break;
	case ANI_BYTE:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_CONT );
			m_bAniState = ANI_BYTE;	
		}
		break;
	}
}

void CWndGuideSystem::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bIsLoad = FALSE;

	this->DelWndStyle(WBS_CAPTION);
	this->AddWndStyle(WBS_TOPMOST);

	SAFE_DELETE(m_pModel);

	if( g_pPlayer )
	{
		ChangeModel( g_pPlayer->GetJob() );
		SetAni( g_pPlayer->GetJob(), ANI_INTRO );
	}
	else
		Error( "CWndGuideSystem::OnInitialUpdate() -> g_pPlayer == NULL " );

	m_wndTitleBar.SetVisible( FALSE );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );

	m_listGuide.clear();
	m_listGuideMsg.clear();
	m_listGuideChart.clear();

	LoadGuide( MakePath( DIR_CLIENT, "Guide.inc" ) );
	//*
	//텍스트 창 출력
	SAFE_DELETE(m_wndGuideText);
	m_wndGuideText = new CWndGuideTextMgr;
	m_wndGuideText->Initialize();
	/**/

	m_bIsGuideChart[0] = FALSE;
	m_bIsGuideChart[1] = FALSE;
	
	m_dwTime = timeGetTime();

	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_GUIDE_HIDE) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_GUIDE_OPEN) );
	m_wndMenuPlace.CheckMenuItem( 0, FALSE );
	m_wndMenuPlace.CheckMenuItem( 1, m_wndGuideText->m_bVisible );
	
	m_dwGuideLevel = 1;
}

void CWndGuideSystem::PushBack(GUIDE_STRUCT guide)
{
	m_listGuide.push_back( guide );

	GUIDE_STRUCT* pguide = &(m_listGuide.back());
	
	m_listGuideChart.push_back(pguide);
}

BOOL CWndGuideSystem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuideSystem::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuideSystem::OnLButtonUp( UINT nFlags, CPoint point )
{
//	m_wndGuideText->UpDate();
//	m_wndGuideText->m_bVisible = FALSE;
}
void CWndGuideSystem::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_wndMenuPlace.SetVisible( FALSE );
	m_wndGuideText->m_bVisible = FALSE;
	
	if( IsPickupSpace(point) )
	{
		m_bPickup = TRUE;
	}
}
void CWndGuideSystem::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	m_wndGuideText->UpDate();
}
void CWndGuideSystem::OnRButtonDown( UINT nFlags, CPoint point )
{
	CRect rect = GetWindowRect( TRUE );
	CRect menurect = m_wndMenuPlace.GetWindowRect();

	if( rect.left - m_wndMenuPlace.GetWindowRect().Width() < m_wndMenuPlace.GetWindowRect().Width() )
		m_wndMenuPlace.Move( CPoint( rect.left + m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );
	else
		m_wndMenuPlace.Move( CPoint( rect.left - m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );

	m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
	m_wndMenuPlace.SetFocus();	
}


void CWndGuideSystem::SendGuideMessage( int nMsg )
{
	if( m_bAniState != ANI_IDLE )
		return;

	if( m_dwGuideLevel != 2 )
		m_listGuideMsg.push_back( nMsg );
}

BOOL CWndGuideSystem::Process()
{
//	return 1;
	if( !(timeGetTime() > m_dwTime+SEC(2.5)) )
		return TRUE;

	if( m_pModel == NULL )
		return FALSE;
	
	m_pModel->FrameMove();
	
	if( m_pModel->IsEndFrame() )
	{
		if( m_bAniState == ANI_INTRO )
		{
			if( g_pPlayer )
				SetAni( g_pPlayer->GetJob(), ANI_IDLE );
		}
		else
		if( m_bAniState == ANI_BYTE )
		{
			m_bVisible     = FALSE;
			m_bAniState    = ANI_IDLE;
		}
	}
	
	int nMsg;

	if( m_bIsGuideChart[0] )
	{	
		GUIDE_STRUCT* guidestruct;

		if( m_listGuideMsg.size() )
		{
			nMsg = m_listGuideMsg.front();

			if( m_listGuideChart.size() )
			{
				guidestruct = m_listGuideChart.front();

				if( guidestruct->m_nEventMsg == nMsg )
				{
					m_listGuideChart.pop_front();
					m_listGuideMsg.pop_front();

					// 새로운 가이드 추가
					if( m_listGuideChart.size() )
					{
						guidestruct = m_listGuideChart.front();
						m_wndGuideText->AddGuideText( *guidestruct );
					}
					else
					{
						m_wndGuideText->m_bVisible = FALSE;
						m_bIsGuideChart[0] = FALSE;
						m_bIsGuideChart[1] = TRUE;
					}
				}
				else
					m_listGuideMsg.pop_front();
			}
		}
	}
	else
	if( m_bIsGuideChart[1] )
	{
		if( m_listGuideMsg.size() )
		{
			nMsg = m_listGuideMsg.front();

			for( list<GUIDE_STRUCT>::iterator i = m_listGuide.begin(); i != m_listGuide.end(); ++i )
			{
				GUIDE_STRUCT guidestruct;

				guidestruct = (*i);

				if( (*i).m_nEventMsg == nMsg && (*i).m_bBeginner == FALSE  )
				{
					if( g_pPlayer && g_pPlayer->GetLevel() == (*i).m_nShowLevel )
					{
						(*i).m_bFlag = !(*i).m_bFlag;

						m_listGuideMsg.pop_front();
						m_wndGuideText->AddGuideText( (*i) );
					}
				}
			}

			if( m_listGuideMsg.size() )
				m_listGuideMsg.pop_front();
		}
		
	}

	return TRUE;
}

BOOL CWndGuideSystem::LoadGuide( LPCTSTR lpszFileName )
{
	CScript script;
	CScript scriptdefine1, scriptdefine2, scriptdefine3;

//	if( scriptdefine1.Load( "define.h" ) )
//		scriptdefine1.PreScan( FALSE ); 

	if( scriptdefine2.Load( "resdata.h" ) )
		scriptdefine2.PreScan( TRUE ); 
	
//	if( scriptdefine3.Load( "lang.h" ) )
//		scriptdefine3.PreScan( FALSE ); 

	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	GUIDE_STRUCT guidestruct;
	
	m_listGuide.clear();
	m_listGuideChart.clear();
	
	script.tok = 0;
	
	while( script.tok != FINISHED )
	{
		guidestruct.m_nEventMsg = script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			guidestruct.m_bBeginner = script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			guidestruct.m_nShowLevel = script.GetNumber();
			script.GetToken(); // FLAG
			guidestruct.m_bFlag      = script.GetNumber();
			script.GetToken(); // KEY
			guidestruct.m_nkey = script.GetNumber();
			
			DWORD dwNation = script.GetNumber();

			guidestruct.m_str = "";

			while( *script.token  != '}' )
			{
				script.GetLastFull();//Token(); // 스트링 
				if( dwNation == prj.m_nLanguage )
				{
					guidestruct.m_str = script.Token;
				}

				dwNation = script.GetNumber(); // 국가 코드 
			}

			PushBack(guidestruct);	
		}
		else
			script.GetToken();
	}
	
	m_bIsLoad = TRUE;
	CScript::m_localDef.Free();
	
	return TRUE;
}

void CWndGuideSystem::GuideStart(BOOL ischart)
{
	if( ischart )
	{
		m_bIsGuideChart[0] = TRUE;
		m_bIsGuideChart[1] = FALSE;
		GUIDE_STRUCT* guidestruct = m_listGuideChart.front();
		m_wndGuideText->AddGuideText( *guidestruct );
	}
	else
	{
		m_bIsGuideChart[1] = TRUE;
		m_bIsGuideChart[0] = FALSE;
	}
}

void CWndGuideSystem::CreateGuideSelection()
{
	SAFE_DELETE(m_wndGuideSelection);
	m_wndGuideSelection = new CWndGuideSelection;
	m_wndGuideSelection->Initialize();
}





////////////////////////////////////////////////////////////////////////////////////
CWndGuideTextMgr::CWndGuideTextMgr() 
{ 
	SetPutRegInfo( FALSE );
} 
CWndGuideTextMgr::~CWndGuideTextMgr() 
{ 
} 
void CWndGuideTextMgr::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	
	if( !pWndGuide )
		return;
} 
void CWndGuideTextMgr::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	DelWndStyle(WBS_MOVE);
	AddWndStyle(WBS_TOPMOST);
	m_wndTitleBar.SetVisible( FALSE );

	m_bVisible = FALSE;

	CWndText* pWndText;
	CWndButton* pWndButton;
    pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	m_Rect[0] = pWndText->GetWndRect();
	pWndButton = (CWndButton*)GetDlgItem( WIDC_BACK );
	m_Rect[1] = pWndButton->GetWndRect();
	pWndButton  = (CWndButton*)GetDlgItem( WIDC_NEXT );	
	m_Rect[2] = pWndButton->GetWndRect();
	m_Rect[3] = GetWndRect();

	m_nCurrentVector = 0;
	m_VecGuideText.clear();	

	m_pTextureBG = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GuideBG.tga" ), 0, TRUE );
} 

BOOL CWndGuideTextMgr::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE_TEXT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuideTextMgr::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuideTextMgr::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuideTextMgr::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuideTextMgr::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuideTextMgr::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	GUIDE_STRUCT guide;
#ifdef __ONLINE
	switch( nID )
	{
		case WIDC_BACK:
			{
				if( m_VecGuideText.size() )
				{
					m_nCurrentVector--;

					if( m_nCurrentVector < 0 )
						m_nCurrentVector = 0;

					guide = m_VecGuideText[m_nCurrentVector];
					_SetGuideText(guide);
				}
			}
			break;
		case WIDC_NEXT:
			{
				if( m_VecGuideText.size() )
				{
					m_nCurrentVector++;

					if( m_nCurrentVector > m_VecGuideText.size()-1 )
						m_nCurrentVector = m_VecGuideText.size()-1;

					guide = m_VecGuideText[m_nCurrentVector];
					_SetGuideText(guide);
					
					CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
					
					if( pWndWorld ) 
					{
						pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_INTRO);
						pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_END);
					}
				}
			}
			break;
		default:
			{
				m_bVisible = FALSE;
				CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
				
				if( pWndWorld ) 
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_INTRO);
				
			}
			break;
	}
#endif
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuideTextMgr::PaintFrame( C2DRender* p2DRender )
{
}

BOOL CWndGuideTextMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	
	if( !pWndGuide )
		return FALSE;

	CRect rect = GetLayoutRect();

	rect.bottom = rect.bottom+5;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pTextureBG->Render( p2DRender, CPoint(8,0), CPoint( rect.right, rect.bottom ), 200 );
	//p2DRender->RenderFillRect( CRect( 8, -8, rect.right, rect.bottom ), D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.8f ) );
	//p2DRender->RenderFillRect( CRect( rect.right, 8, rect.right+3, rect.bottom+3 ), D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	//p2DRender->RenderFillRect( CRect( 8+3, rect.bottom, rect.right+3, rect.bottom+3 ), D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	
	return TRUE;
}

void CWndGuideTextMgr::AddGuideText(GUIDE_STRUCT guide)
{
	m_VecGuideText.push_back( guide );

	m_nCurrentVector = m_VecGuideText.size()-1;

	PLAYSND( SND_INF_MESSENGERRING );
	
	_SetGuideText( guide );
}

void CWndGuideTextMgr::_SetGuideText(GUIDE_STRUCT guide)
{
	CWndText* pWndText;
	CWndButton* pWndButton;

	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetWndRect( m_Rect[0] );
	pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
	pWndButton->SetWndRect( m_Rect[1] );
	pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
	pWndButton->SetWndRect( m_Rect[2] );
	SetWndRect(	m_Rect[3] );
	
	
	m_bVisible = TRUE;

	m_strHelpKey = guide.m_str;
	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( (guide.m_nkey == CWndGuideSystem::KEY) ? prj.GetHelp( m_strHelpKey ) : guide.m_str );

	CRect rect = pWndText->GetWndRect();

	pWndText->m_string.Align( m_pFont );
	
	int nLine = pWndText->m_string.GetLineCount();
	
	if( nLine < 10 )
	{
		if( nLine < 8 )
			nLine = 8;

		CRect clientrect = GetWndRect();
		
		int ngap = rect.Height() - (nLine * 22);
		clientrect.top += ngap;
		SetWndRect(clientrect);

		rect.bottom -= ngap;
		pWndText->SetWndRect( rect );

		CRect ptRect;
		pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
		ptRect = pWndButton->GetWndRect();
		ptRect.OffsetRect( 0, -ngap );
		pWndButton->SetWndRect(ptRect);
		pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
		ptRect = pWndButton->GetWndRect();
		ptRect.OffsetRect( 0, -ngap );
		pWndButton->SetWndRect(ptRect);
		
		pWndText->m_wndScrollBar.m_bVisible = FALSE;
	}
	else
	{
		/*
		pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->SetWndRect( m_Rect[0] );
		pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
		pWndButton->SetWndRect( m_Rect[1] );
		pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
		pWndButton->SetWndRect( m_Rect[2] );
		SetWndRect(	m_Rect[3] );
		*/

		pWndText->m_wndScrollBar.m_bVisible = TRUE;
		pWndText->SetWndRect( rect );
	}

	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	CRect rectGuide = pWndGuide->GetWindowRect( TRUE );
	CPoint ptGuide = rectGuide.TopLeft();
	CPoint ptMove;

	pWndGuide->m_bVisible = TRUE;
	
	CRect windowrect = GetWindowRect( TRUE );
	
	if( ptGuide.x > windowrect.Width() )
		ptMove = ptGuide - CPoint( windowrect.Width(), 0 );
	else
		ptMove = ptGuide + CPoint( rectGuide.Width(), 0 );

	ptMove.y = rectGuide.bottom;
	ptMove.y -= windowrect.Height();
	
	if( ptMove.y < 0 )
		ptMove.y = rectGuide.top;
	
	Move( ptMove );
}

void CWndGuideTextMgr::UpDate()
{
	if( m_VecGuideText.size() )
	{
		m_bVisible = TRUE;
		
		GUIDE_STRUCT guide;
		guide = m_VecGuideText[m_nCurrentVector];
		_SetGuideText(guide);
	}
	else
		m_bVisible = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGuideSelection::CWndGuideSelection()
{
}

CWndGuideSelection::~CWndGuideSelection()
{
}

BOOL CWndGuideSelection::Initialize( CWndBase* pWndParent, DWORD nType )
{	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE_SELECTION, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndGuideSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case WIDC_RADIO1:
			{
				*g_Option.m_pGuide = 0;
			}
			break;
		case WIDC_RADIO2:
			{
				*g_Option.m_pGuide = 1;
			}
			break;
		case WIDC_RADIO3:
			{
				*g_Option.m_pGuide = 2;
			}
			break;
		case WIDC_BUTTON1:
			{
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );

				if( pWndGuide && pWndGuide->m_bAniState == CWndGuideSystem::ANI_IDLE && pWndGuide->m_bIsLoad )
				{
					pWndGuide->m_dwGuideLevel = *g_Option.m_pGuide;

					if( pWndGuide->m_dwGuideLevel == 0 )
						pWndGuide->GuideStart(TRUE);
					else
					{
						pWndGuide->GuideStart(FALSE);
						if(g_pPlayer)
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
					
					Destroy();
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndGuideSelection::OnDraw( C2DRender* p2DRender )
{

}
void CWndGuideSelection::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	m_wndTitleBar.SetVisible( FALSE );

	CWndButton* pWndButton[3];
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 0 ]->SetCheck( TRUE );

	*g_Option.m_pGuide = 0;
	
	MoveParentCenter();
}
BOOL CWndGuideSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuideSelection::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuideSelection::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndGuideSelection::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndGuideSelection::OnRButtonDown( UINT nFlags, CPoint point )
{
}

////////////////////////////////////////////////////////////////////////////////////


#endif //__YGUIDESYSTEM0923

