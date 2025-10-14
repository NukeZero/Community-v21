#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndDuelConfirm.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_DUEL_CONFIRM - party confirm
  CtrlId : WIDC_YES - yes
  CtrlId : WIDC_NO - No
  CtrlId : WIDC_STATIC1 - Static
****************************************************/

CWndDuelConfirm::CWndDuelConfirm() 
{ 
	memset( m_szSrc, 0, sizeof(m_szSrc) );
	m_idSrc = NULL_ID;
	m_bDuelParty = FALSE;
} 
CWndDuelConfirm::~CWndDuelConfirm() 
{ 
} 
void CWndDuelConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndDuelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	//	CWndStatic* pWndStatic;
	//	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CString strTemp;

	/*
	if( m_bDuelParty == FALSE )
		strTemp.Format( _T("%s 님이 듀얼신청을 하였습니다. 허락하겠습니까?"), m_szSrc ); // 메시지 바꾸려면 이걸 바꾸시오
	else
		strTemp.Format( _T("%s 님이 극단듀얼신청을 하였습니다. 허락하겠습니까?"), m_szSrc ); // 메시지 바꾸려면 이걸 바꾸시오
	*/
	if( m_bDuelParty == FALSE )
		strTemp.Format( _T( prj.GetText(TID_DIAG_0066) ), m_szSrc ); // 메시지 바꾸려면 이걸 바꾸시오
	else
		strTemp.Format( _T( prj.GetText(TID_DIAG_0067) ), m_szSrc ); // 메시지 바꾸려면 이걸 바꾸시오
	
	pWndText->SetString( strTemp );

	// 윈도를 중앙으로 옮기는 부분.
	/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	*/
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDuelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DUEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndDuelConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndDuelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDuelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDuelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDuelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndDuelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bResult = FALSE;
	if( m_bDuelParty == FALSE )
	{
		if( nID == WIDC_YES ) 
		{
			// 여기다가 승락하는 처리 추가하시오
			// 서버로 승락 했다는메시지를 보냄
			// 서버에서 받으면 두 캐릭터에게 모두 듀얼이 시작됐다는걸 보냄
			// 각 클라에서 받으면 시작.
			CMover *pSrc = prj.GetMover( m_idSrc );	// 듀얼 신청자.
			if( IsValidObj( pSrc ) )
			{
#ifndef __NPP_050308
#ifdef __BUGFIX_DUEL050407
				if( g_pPlayer->IsFly() || pSrc->IsFly() )
					g_WndMng.PutString( prj.GetText( TID_PK_NOFLIGHT_DURING ), NULL, prj.GetTextColor( TID_PK_NOFLIGHT_DURING ) );
				else
#endif	// __BUGFIX_DUEL050407
#endif	// __NPP_050308
					g_DPlay.SendDuelYes( pSrc, g_pPlayer );		// 듀얼 신청 허락함.
			}
			bResult = TRUE;
		}
		else if( nID == WIDC_NO || nID == WTBID_CLOSE ) 
		{
			// 여기다가 거부하는 처리 추가하시오
			CMover *pSrc = prj.GetMover( m_idSrc );	// 듀얼 신청자.
			if( IsValidObj( pSrc ) )
			{
				g_DPlay.SendDuelNo( pSrc );		// pSrc에게 거부했다는걸 알림.
			}
			bResult = TRUE;
		}
	} else
	// 파티 듀얼.
	{
		if( nID == WIDC_YES ) 
		{
			// 여기다가 승락하는 처리 추가하시오
			// 서버로 승락 했다는메시지를 보냄
			// 서버에서 받으면 두 캐릭터에게 모두 듀얼이 시작됐다는걸 보냄
			// 각 클라에서 받으면 시작.
			CMover *pSrc = prj.GetMover( m_idSrc );	// 듀얼 신청자.
			if( IsValidObj( pSrc ) )
			{
				g_DPlay.SendDuelPartyYes( pSrc, g_pPlayer );		// 듀얼 신청 허락함.
			}
			bResult = TRUE;
		}
		else if( nID == WIDC_NO || nID == WTBID_CLOSE ) 
		{
			// 여기다가 거부하는 처리 추가하시오
			CMover *pSrc = prj.GetMover( m_idSrc );	// 듀얼 신청자.
			if( IsValidObj( pSrc ) )
			{
				g_DPlay.SendDuelPartyNo( pSrc );		// pSrc에게 거부했다는걸 알림.
			}
			bResult = TRUE;
		}
	}
	// 반드시!!! TRUE로 해서 메모리에서 날리자!!
	if( bResult )
		Destroy( /*TRUE*/ );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

