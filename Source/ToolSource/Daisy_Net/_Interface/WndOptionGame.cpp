#include "stdafx.h"
#include "AppDefine.h"
#include "WndOptionGame.h"
#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_OPTION_GAME - Game Option
****************************************************/

CWndOptionGame::CWndOptionGame() 
{ 
} 
CWndOptionGame::~CWndOptionGame() 
{ 
} 
void CWndOptionGame::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptionGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndButton[ 2 ];
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bTrade) ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bSay) ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bParty) ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHOUT_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHOUT_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bShout) ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGERJOIN_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGERJOIN_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bMessengerJoin) ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGER_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGER_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bMessenger) ]->SetCheck( TRUE );
	
#ifdef __NPP_050308
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer )
		pWndButton[ !(g_pPlayer->IsMode( FREEPK_MODE ) ) ]->SetCheck( TRUE );
	else
		pWndButton[ 1 ]->SetCheck( TRUE );

	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer )
		pWndButton[ g_pPlayer->IsMode( PVPCONFIRM_MODE ) ]->SetCheck( TRUE );
	else
		pWndButton[ 0 ]->SetCheck( TRUE );
#endif // __NPP_050308
	
#ifdef __YQUERYEQUIPSETTING
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO12 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO13 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer->IsMode( EQUIP_DENIAL_MODE ) )
		pWndButton[ 1 ]->SetCheck( TRUE );
	else
		pWndButton[ 0 ]->SetCheck( TRUE );
#endif //__YQUERYEQUIPSETTING
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptionGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_GAME, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptionGame::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptionGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptionGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptionGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptionGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptionGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_RADIO1:
		g_Option.m_bTrade = 1;
		break;
	case WIDC_RADIO2:
		g_Option.m_bTrade = 0;
		break;
	case WIDC_RADIO3:
		g_Option.m_bSay = 1;
		break;
	case WIDC_RADIO4:
		g_Option.m_bSay = 0;
		break;
	case WIDC_RADIO5:
		g_Option.m_bParty = 1;
		break;
	case WIDC_RADIO6:
		g_Option.m_bParty = 0;
		break;
	case WIDC_SHOUT_ON:
		g_Option.m_bShout = 1;
		break;
	case WIDC_SHOUT_OFF:
		g_Option.m_bShout = 0;
		break;
	case WIDC_MESSENGERJOIN_ON:
		g_Option.m_bMessengerJoin = 1;
		break;
	case WIDC_MESSENGERJOIN_OFF:
		g_Option.m_bMessengerJoin = 0;
		break;	
	case WIDC_MESSENGER_ON:
		g_Option.m_bMessenger = 1;
		break;
	case WIDC_MESSENGER_OFF:
		g_Option.m_bMessenger = 0;
		break;
#ifdef __NPP_050308
#ifdef __ONLINE
	case WIDC_AUTOPK_ON:
		g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 0 );
		break;
	case WIDC_AUTOPK_OFF:
		g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 1 );
		break;
	case WIDC_ACCEPTPVP_ON:
		g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 3 );
		break;
	case WIDC_ACCEPTPVP_OFF:
		g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 2 );
		break;
#endif //__ONLINE
#endif // __NPP_050308
#if defined(__YQUERYEQUIPSETTING) && defined(__ONLINE)
	case WIDC_RADIO12:		
		g_DPlay.SendQueryEquipSetting(TRUE);			
		break;
		
	case WIDC_RADIO13:
		g_DPlay.SendQueryEquipSetting(FALSE);			
		break;
#endif //__YQUERYEQUIPSETTING
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

