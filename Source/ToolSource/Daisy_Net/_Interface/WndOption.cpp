// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "Resdata.h"
#include "defineSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		MAX_SLIDER		10

#ifdef __N_OPTIONUNITY_041209
#include "definetext.h"
CWndTotalOption::CWndTotalOption() 
{ 
} 
CWndTotalOption::~CWndTotalOption() 
{ 
} 

void CWndTotalOption::UpdateDataAll()
{
	//m_OptTabVideoSnd
}

void CWndTotalOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTotalOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	//	여기에 코딩하면 됩니다
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_OptTabVideoSnd.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV );
	m_OptTabEtc.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_ETC );
	m_OptTabGame.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_VIDEO);
	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_ETC);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_ETC);
	tabTabItem.pWndBase = &m_OptTabEtc;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_GAME);
	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );

	MoveParentCenter();	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTotalOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTotalOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTotalOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTotalOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTotalOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTotalOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifdef __YGUIDESYSTEM0923
	switch( nID )
	{
		case 10000:
			{
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				
				if( pWndGuide )
				{
					pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
					
					if( pWndGuide->m_dwGuideLevel == 0 )
					{
						if( g_pPlayer )
						{
							pWndGuide->m_bVisible = TRUE;
							pWndGuide->ChangeModel( g_pPlayer->GetJob() );
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
						}
					}
					else
					{
						if(g_pPlayer)
						{
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
						}
					}
				}
			}
			break;
	}
#endif //__YGUIDESYSTEM0923		
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif//__N_OPTIONUNITY_041209

CWndOption::CWndOption() 
{ 
} 
CWndOption::~CWndOption() 
{ 
} 
void CWndOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndAlpha->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndHelp->SetCheck( g_Option.m_nInstantHelp );
	
	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nSlangWord ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nChatCommand ]->SetCheck( TRUE );

	// 인터페이스 방식(구버전이 디폴트)
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO8 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nInterface ]->SetCheck( TRUE );

#ifdef __YGUIDESYSTEM0923
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO9 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO10 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO11 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ *g_Option.m_pGuide ]->SetCheck( TRUE );
#endif //__YGUIDESYSTEM0923
	

	// 비행시 롤링 효과.
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );		
	pWndRoll->SetCheck( g_Option.m_bRollEffect );
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOption::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_ETC, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	switch( nID )
	{
	case WIDC_CHECK1: // 윈도 반투명 옵션 
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
	case WIDC_CHECK2: // 초보자 도움말 옵션 
		if( pWndHelp->GetCheck() )
			g_Option.m_nInstantHelp = TRUE;
		else
			g_Option.m_nInstantHelp = FALSE;
		break;
	case WIDC_CHECK3:	// 비행시 롤링 효과.
		if( pWndRoll->GetCheck() )
			g_Option.m_bRollEffect = TRUE;
		else
			g_Option.m_bRollEffect = FALSE;
		break;
	case WIDC_RADIO1:
		g_Option.m_nSlangWord = 0;
		break;
	case WIDC_RADIO2:
		g_Option.m_nSlangWord = 1;
		break;
	case WIDC_RADIO3:
		g_Option.m_nChatCommand = 0;
		break;
	case WIDC_RADIO4:
		g_Option.m_nChatCommand = 1;
		break;
	case WIDC_RADIO5:
		g_Option.m_nChatCommand = 2;
		break;
	case WIDC_RADIO6:
		g_Option.m_nChatCommand = 3;
		break;
	case WIDC_RADIO7:	// 휠로 카메라 / 우클릭 스킬의 고전방식
		g_Option.m_nInterface = 0;
		break;
	case WIDC_RADIO8:	// 우클릭 카메라 / 좌클릭+X 스킬의 신방식.
		g_Option.m_nInterface = 1;
		break;
#ifdef __YGUIDESYSTEM0923
	case WIDC_RADIO9:
		*g_Option.m_pGuide = 0;
		break;
	case WIDC_RADIO10:
		*g_Option.m_pGuide = 1;
		break;
	case WIDC_RADIO11:
		*g_Option.m_pGuide = 2;
		break;
	case 10000:
		{
			CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			
			if( pWndGuide )
			{
				pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
				
				if( pWndGuide->m_dwGuideLevel == 0 )
				{
					if( g_pPlayer )
					{
						pWndGuide->m_bVisible = TRUE;
						pWndGuide->ChangeModel( g_pPlayer->GetJob() );
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
					}
				}
				else
				{
					if(g_pPlayer)
					{
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
				}
			}
		}
		break;
#endif //__YGUIDESYSTEM0923		
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CWndOptSound::CWndOptSound() 
{ 
} 
CWndOptSound::~CWndOptSound() 
{ 
} 
void CWndOptSound::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptSound::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptSound::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_SOUND, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptSound::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptSound::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptSound::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptSound::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptSound::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptSound::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndOptVideo::CWndOptVideo() 
{ 
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;
} 
CWndOptVideo::~CWndOptVideo() 
{ 
	g_Option.Save( "neuz.ini" );	
} 
HRESULT CWndOptVideo::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptVideo::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}

CPoint CWndOptVideo::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision+1));
}

void CWndOptVideo::OnDraw( C2DRender* p2DRender ) 
{ 
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
} 
void CWndOptVideo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDetail ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDistant ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nViewArea ]->SetCheck( TRUE );
/*
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_ON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_OFF );
	pWndButton[ 1 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nShadow ]->SetCheck( TRUE );
*/
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nShadow ]->SetCheck( TRUE );
	if( g_Neuz.m_d3dCaps.MaxSimultaneousTextures <= 2 ) 
	{
		pWndButton[ 0 ]->EnableWindow( FALSE );
		pWndButton[ 1 ]->EnableWindow( FALSE );
		pWndButton[ 2 ]->EnableWindow( FALSE );
	}
		
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_VOICEON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VOICEOFF );
	pWndButton[ 1 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_bVoice ]->SetCheck( TRUE );

#ifdef __XGLARE
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_GLOWON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_GLOWOFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
	{
		pWndButton[ g_Option.m_nBloom^1 ]->SetCheck( TRUE );
	} else
	{
		pWndButton[0]->EnableWindow( FALSE );
		pWndButton[1]->EnableWindow( FALSE );
	}
#endif

#ifdef __YENV
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );

	if( g_bUsableVS && g_dwUsablePS_Level == 2 )
	{
		pWndButton[0]->EnableWindow( TRUE );
	}
	else
	{
		pWndButton[0]->EnableWindow( FALSE );
	}

	if( g_Option.m_bSpecBump )
	{
		pWndButton[0]->SetCheck( TRUE );
	}
	else
	{
		pWndButton[0]->SetCheck( FALSE );
	}
#endif //__YENV		

#ifdef __YDAMAGE_RENDER
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bDamageRender);
#endif //__YDAMAGE_RENDER
	
#ifdef __YVISIBLE_BUFF_TIME
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bVisibleBuffTimeRender);
#endif //__YDAMAGE_RENDER
	
	
#ifdef __ACROBAT_0504
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_MouseSpeed ]->SetCheck( TRUE );
#endif //__ACROBAT_0504
	
#ifdef __YFAKE_FULLSCREEN
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	pWndButton[ 0 ]->SetCheck(!g_Option.m_bStartFullScreen);
#endif //__YFAKE_FULLSCREEN	
	
#ifdef __YRENDER_FLAG	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bSFXRenderOff);
#endif //__YRENDER_FLAG	
	
/*

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_EFFECTONOFF);
	pButton->SetCheck( g_Option.m_bEffect );
	pButton = (CWndButton*)GetDlgItem(WIDC_BMGONOFF);
	pButton->SetCheck( g_Option.m_bMusic );
*/
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), 0xffff00ff, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );		
/*
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MUSIC_ON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MUSIC_OFF );
	pWndButton[ 1 ]->SetGroup( TRUE );
	pWndButton[ (int)g_Option.m_fMusicVolume ]->SetCheck( TRUE );
*/
	m_nStep[0] = g_Option.m_fEffectVolume * 10;
	m_nStep[1] = g_Option.m_fBGMVolume * 10;
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptVideo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptVideo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptVideo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptVideo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptVideo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
} 
void CWndOptVideo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	SetCapture();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );	
	CRect crect = lpWndCtrl->rect;

	crect.left += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

		if( point.x > lpWndCtrl->rect.left+GetStepPos( m_nStep[0], nWidth, MAX_SLIDER ).x )
			m_nStep[0] += 1;//nWidth / 10;
		else
			m_nStep[0] -= 1;//nWidth / 10;

		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;		
	}


	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );	

	crect = lpWndCtrl->rect;

	crect.left += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
		
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

		if( point.x > lpWndCtrl->rect.left+GetStepPos( m_nStep[1], nWidth, MAX_SLIDER ).x )
			m_nStep[1] += 1;//nWidth / 10;
		else
			m_nStep[1] -= 1;//nWidth / 10;

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);		
	}
	
} 

void CWndOptVideo::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonClick )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );	
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

		if( point.x > lpWndCtrl->rect.left+GetStepPos( m_nStep[0], nWidth, MAX_SLIDER ).x )
			m_nStep[0] += 1;//nWidth / 10;
		if( point.x < lpWndCtrl->rect.left+GetStepPos( m_nStep[0], nWidth, MAX_SLIDER ).x )
			m_nStep[0] -= 1;//nWidth / 10;

		if( m_nStep[0] < 0 )
			m_nStep[0] = 0;

		if( m_nStep[0] > MAX_SLIDER )
			m_nStep[0] = MAX_SLIDER;

		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;
		
	}

	if( m_bLButtonClick2 )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );	
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( m_nStep[1], nWidth, MAX_SLIDER ).x )
			m_nStep[1] += 1;//nWidth / 10;
		if( point.x < lpWndCtrl->rect.left+GetStepPos( m_nStep[1], nWidth, MAX_SLIDER ).x )
			m_nStep[1] -= 1;//nWidth / 10;

		if( m_nStep[1] < 0 )
			m_nStep[1] = 0;
		
		if( m_nStep[1] > MAX_SLIDER )
			m_nStep[1] = MAX_SLIDER;

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}

}

void CWndOptVideo::OnMouseWndSurface( CPoint point )
{
}
BOOL CWndOptVideo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndButton = (CWndButton*)*pLResult;
	switch( nID )
	{
	case WIDC_OBJECT_LOD_HIGH:
		g_Option.m_nObjectDetail = 0;
		break;
	case WIDC_OBJECT_LOD_MID:
		g_Option.m_nObjectDetail = 1;
		break;
	case WIDC_OBJECT_LOD_LOW:
		g_Option.m_nObjectDetail = 2;
		break;

	case WIDC_OBJECT_DISTANT_HIGH:
		g_Option.m_nObjectDistant = 0;
		break;
	case WIDC_OBJECT_DISTANT_MID:
		g_Option.m_nObjectDistant = 1;
		break;
	case WIDC_OBJECT_DISTANT_LOW:
		g_Option.m_nObjectDistant = 2;
		break;

	case WIDC_TERRAIN_LOD_HIGH:
		g_Option.m_nViewArea = 0;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_MID:
		g_Option.m_nViewArea = 1;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_LOW:
		g_Option.m_nViewArea = 2;
		SetLODDetail( g_Option.m_nViewArea );
		break;

	case WIDC_SHADOW_HIGH:
		g_Option.m_nShadow = 0;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_MID:
		g_Option.m_nShadow = 1;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_LOW:
		g_Option.m_nShadow = 2;
		break;
#ifdef __XGLARE		
	case WIDC_GLOWON:
		if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
		{
			g_Option.m_nBloom = 1; 
			g_Glare.Create( D3DDEVICE, D3DFMT_R5G6B5, g_Option.m_nResWidth, g_Option.m_nResHeight - 48 );
		}
		break;
	case WIDC_GLOWOFF:
		g_Option.m_nBloom = 0;
		break;
#endif
	case WIDC_VOICEON:
		g_Option.m_bVoice = 1;
		break;
	case WIDC_VOICEOFF:
		g_Option.m_bVoice = 0;
		break;
#ifdef __YENV
	case WIDC_SEPCBUMPCHECK:
		// 설정값을 바로 적용하는것이 아니라 종료할때 적용한다
		// 옵션중 다시 시작해야 적용되는것들을 위한 변수..( 범프맵... )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );
		if( pWndButton->GetCheck() != g_Option.m_bSpecBump )
		{
			g_Option.m_bExitChange = TRUE;
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		else
			g_Option.m_bExitChange = FALSE;
		break;
#endif //__YENV
#ifdef __YDAMAGE_RENDER
	case WIDC_CHECK2:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
		g_Option.m_bDamageRender = pWndButton->GetCheck();
		break;
#endif //__YDAMAGE_RENDER	
		
#ifdef __YFAKE_FULLSCREEN
	case WIDC_CHECK4:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
		g_Option.m_bStartFullScreen = !(pWndButton->GetCheck());
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		break;
#endif //__YFAKE_FULLSCREEN

#ifdef __YRENDER_FLAG
	case WIDC_CHECK5:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
		g_Option.m_bSFXRenderOff = pWndButton->GetCheck();
		break;
#endif //__YRENDER_FLAG
		

#ifdef __YVISIBLE_BUFF_TIME
	case WIDC_VISIBLEKEEPTIME:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
		g_Option.m_bVisibleBuffTimeRender = pWndButton->GetCheck();
		break;
#endif //__YDAMAGE_RENDER

#ifdef __ACROBAT_0504
	case WIDC_MOUSE_HIGH:
		g_Option.m_MouseSpeed = 0;
		break;
	case WIDC_MOUSE_MID:
		g_Option.m_MouseSpeed = 1;
		break;
	case WIDC_MOUSE_LOW:
		g_Option.m_MouseSpeed = 2;
		break;
#endif //__ACROBAT_0504		
		
		
	/*
	case WIDC_EFFECTONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_EFFECTONOFF);
			g_Option.m_bEffect = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bEffect );
		}
		break;
	case WIDC_BMGONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BMGONOFF);
			g_Option.m_bMusic = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bMusic );

			// 음악 끄고 켜기
			SetOn( g_Option.m_bMusic );
		}
		break;
	*/
		
/*		
	case WIDC_SOUND_ON:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 1;
		break;
	case WIDC_SOUND_OFF:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 0;
		break;

	case WIDC_MUSIC_ON:
		g_Option.m_fMusicVolume = 1.0f;
		SetVolume( 1.0f );
		break;
	case WIDC_MUSIC_OFF:
		g_Option.m_fMusicVolume = 0.0f;
		SetVolume( 0.0f );
		break;
*/
	case WIDC_OK:
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
CWndOptSlang::CWndOptSlang() 
{ 
} 
CWndOptSlang::~CWndOptSlang() 
{ 
} 
void CWndOptSlang::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptSlang::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptSlang::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_SLANG, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptSlang::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptSlang::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptSlang::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptSlang::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptSlang::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptSlang::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
CWndOptPassword::CWndOptPassword() 
{ 
} 
CWndOptPassword::~CWndOptPassword() 
{ 
} 
void CWndOptPassword::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptPassword::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptPassword::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_PASSWORD, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptPassword::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptPassword::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptPassword::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptPassword::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptPassword::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptPassword::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
CWndOptWindow::CWndOptWindow() 
{ 
} 
CWndOptWindow::~CWndOptWindow() 
{ 
} 
void CWndOptWindow::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptWindow::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_CHECK );
	pWndEffect->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nWindowEffect ]->SetCheck( TRUE );
/*
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
*/

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptWindow::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_WINDOW, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptWindow::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptWindow::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptWindow::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptWindow::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptWindow::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptWindow::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK );
	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	switch( nID )
	{
	case WIDC_CHECK:
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
	case WIDC_RADIO1:
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndOptMyInfo::CWndOptMyInfo() 
{ 
} 
CWndOptMyInfo::~CWndOptMyInfo() 
{ 
} 
void CWndOptMyInfo::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptMyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_MYINFO, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptMyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptMyInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptMyInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptMyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptMyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptVideo, _T( "비디오 옵션" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptSound, _T( "사운드 옵션" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndOptMusic, _T( "배경음악 옵션" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndOptMouse, _T( "마우스 감도 조절" ) )


