#include "stdafx.h"
#include "WndMessage.h"

#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWndInstantMsg::CWndInstantMsg() 
{ 
} 
CWndInstantMsg::~CWndInstantMsg() 
{ 
} 
void CWndInstantMsg::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndInstantMsg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.bottom  - rectWindow.Height() );
	Move( point );
	//MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInstantMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMUNICATION_INSMSG, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndInstantMsg::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndInstantMsg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndInstantMsg::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndInstantMsg::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndMessage* pWndMessage = g_WndMng.OpenMessage( m_strPlayer );
	pWndMessage->AddMessage( m_strPlayer, pWndText->m_string );
	Destroy();
} 
void CWndInstantMsg::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndInstantMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndMessage* pWndMessage = g_WndMng.OpenMessage( m_strPlayer );
	pWndMessage->AddMessage( m_strPlayer, pWndText->m_string );
	Destroy();

	/*
	switch(nID)
	{
		case WIDC_EDIT: // 본문 
			if(message == EN_CHANGE)
			{
				CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT );
				CString string;
				string.Format( "/say %s %s", m_strPlayer, pWndText->m_string );
#ifdef __ONLINE
				g_DPlay.SendChat( string );
#endif
				pWndText->Empty();
			}
			break;
		case WIDC_SEND: // 본문 
			{
				CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT );
				CString string;
				string.Format( "/say %s %s", m_strPlayer, pWndText->m_string );
#ifdef __ONLINE
				g_DPlay.SendChat( string );
#endif
				pWndText->Empty();
//				pWndText->ResetString();
			}
			break;

	}
	*/
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndInstantMsg::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CString strMessage;
	strMessage.Format( "%s%s :\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3),lpszMessage );
//	strMessage.Format( "%s님의 말 :\n  %s\n", lpszFrom, lpszMessage );
	pWndText->AddString( strMessage );
	pWndText->ResetString();
	pWndText->m_wndScrollBar.SetMaxScrollPos();
}
//////////////////////////////////////////////////////////

CWndMessage::CWndMessage() 
{ 
} 
CWndMessage::~CWndMessage() 
{ 
} 
void CWndMessage::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMUNICATION_MESSAGE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndMessage::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMessage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessage::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMessage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_EDIT: // 본문 
			if(message == EN_CHANGE)
			{
				CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT );
				CString string;
				string.Format( "/say \"%s\" %s", m_strPlayer, pWndText->m_string );
#ifdef __ONLINE
				g_DPlay.SendChat( string );
#endif
				pWndText->Empty();
			}
			break;
		case WIDC_SEND: // 본문 
			{
				CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT );
				CString string;
				string.Format( "/say \"%s\" %s", m_strPlayer, pWndText->m_string );
#ifdef __ONLINE
				g_DPlay.SendChat( string );
#endif
				pWndText->Empty();
//				pWndText->ResetString();
			}
			break;

	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndMessage::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CString strMessage;
	strMessage.Format( "%s%s :\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3),lpszMessage );
//	strMessage.Format( "%s님의 말 :\n  %s\n", lpszFrom, lpszMessage );
	pWndText->AddString( strMessage );
	pWndText->ResetString();
	pWndText->m_wndScrollBar.SetMaxScrollPos();
}
