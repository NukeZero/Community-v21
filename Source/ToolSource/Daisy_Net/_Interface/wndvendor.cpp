#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndVendor.h"

#include "WndManager.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef __S0920_CHATTING
#include "Chatting.h"
extern	CChatting g_Chatting;
#endif // __S0920_CHATTING


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
	WndId : APP_VENDOR - 개인상점
	CtrlId : WIDC_EDIT1 - 
	CtrlId : WIDC_STATIC1 - 상점이름
	CtrlId : WIDC_EDIT2 - 
	CtrlId : WIDC_STATIC2 - 판매목록
	CtrlId : WIDC_OK - 확인
	CtrlId : WIDC_CANCEL - 취소
*/



CWndVendor::CWndVendor()
{
	SetPutRegInfo( FALSE );
	m_pVendor	= NULL;
	m_pWndRegVend	= NULL;
	m_pWndVendorBuy		= NULL;

	m_pwndVenderMessage = NULL;
		
}

CWndVendor::~CWndVendor()
{
	SAFE_DELETE( m_pWndRegVend );
	SAFE_DELETE( m_pWndVendorBuy );

	SAFE_DELETE( m_pwndVenderMessage );
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndVendor::SetVendor( CMover* pVendor )
{
	m_pVendor	= pVendor;
	m_wndctrlVendor.InitVendor( m_pVendor );
	if( !m_pVendor->IsActiveMover() )
	{
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		pWndEdit->SetString( pVendor->m_vtInfo.GetTitle() );
		pWndEdit->EnableWindow( FALSE );
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->SetVisible( FALSE );
		pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
		pButton->SetVisible( FALSE );
	}
	CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld )
		pWndWorld->m_objidTracking	= NULL_ID;

	if( g_WndMng.m_pWndDialog )
		Destroy();
}

void CWndVendor::OnDraw( C2DRender* p2DRender )
{

}

void CWndVendor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	m_wndctrlVendor.Create( WLVS_ICON | WBS_NODRAWFRAME, CRect( 18, 80, 220, 250 ), this, 100 );
	
	SetVendor( g_pPlayer );

#ifdef __SCHOOL0701
	if( g_pPlayer )
	{
		if( g_pPlayer->IsMode( COMMUNITY_MODE ) )
		{
			Destroy();
		}
	}
#endif // __SCHOOL0701

#ifdef __BUGFIX_CLONE0607
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank )
	{
		Destroy();
		return;
	}
#endif	// __BUGFIX_CLONE0607

	if( !m_pVendor->IsActiveMover() )
	{
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->SetVisible( FALSE );
		pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
		pButton->SetVisible( FALSE );
	}

	CWndInventory* pWndInventory	= (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );
	Move( point );
} 

BOOL CWndVendor::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
#ifdef __SCHOOL0701
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
#endif	// __SCHOOL0701

	return InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX, 0, 0, pWndParent );
}

BOOL CWndVendor::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndVendor::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndVendor::OnLButtonUp( UINT nFlags, CPoint point ) 
{

}

void CWndVendor::OnLButtonDown( UINT nFlags, CPoint point ) 
{

}

BOOL CWndVendor::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == 100 && nID == 100 )
	{
		LPSHORTCUT pShortcut	= (LPSHORTCUT)pLResult;
		if( pShortcut )
		{
			CWndBase* pWndFrame		= pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				CItemBase* pItemBase	= m_pVendor->GetItemId( pShortcut->m_dwId );
				if( pItemBase )
				{
#ifdef __S050504_QUESTITEM
					if( pItemBase->IsQuest() )
						return FALSE;
#endif //__S050504_QUESTITEM
					if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
						return FALSE;

#ifdef __PIERCING_SM_TIME
					if( ((CItemElem*)pItemBase)->m_bCharged == 1 )
#else // __PIERCING_SM_TIME
					if( pItemBase->GetProp()->bCharged == 1 )
#endif // __PIERCING_SM_TIME
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTTRADE ), NULL, prj.GetTextColor( TID_GAME_NOTTRADE ) );
						return FALSE;
					}

					int iIndex	= pShortcut->m_dwData - 100;
					if( 0 <= iIndex && iIndex < MAX_VENDITEM )						
					{
						if( m_pVendor->m_vtInfo.GetItem( iIndex ) != NULL )
							return FALSE;
						
						if( IsUsingItem( pItemBase ) == FALSE && m_pVendor->IsActiveMover() && !m_pVendor->m_vtInfo.IsVendorOpen() )
						{
							SAFE_DELETE( m_pWndRegVend );
							m_pWndRegVend	= new CWndRegVend;
							m_pWndRegVend->Initialize( this );
							m_pWndRegVend->SetItem( iIndex, pItemBase );
						}
					}
				}
			}	// APP_INVENTORY
		}
	}
	else
	{
		if( nID == WIDC_OK )
		{
			if( g_pPlayer->m_pActMover->IsFly() )
				return TRUE;
			if( g_pPlayer->m_vtInfo.VendorIsVendor() == FALSE )
				return TRUE;
			CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CString strVendor	= pWndEdit->GetString();
			strVendor.TrimLeft();
			strVendor.TrimRight();

			g_WndMng.WordChange( strVendor );
			// name
//			if( strVendor.IsEmpty() == FALSE )
			{
				if( strVendor.GetLength() < 6 || strVendor.GetLength() >= 38 )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
//					g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
					return TRUE;
				}
				else
				{
/*
					for( int i = 0; i < strVendor.GetLength(); i++ )
					{
						char c	= strVendor.GetAt( i );
						if( IsDBCSLeadByte( c ) == TRUE )
						{
							char c2	= strVendor.GetAt( ++i );
							DWORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
							if( IsHangul( word ) == FALSE )
							{
								g_WndMng.OpenMessageBox( _T( "명칭에 한자나 특수 문자를 사용할 수 없습니다." ) );
								return TRUE;
							}
						}
						else if( isalnum( c ) == FALSE || iscntrl( c ) )
						{
							g_WndMng.OpenMessageBox( _T( "명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다." ) );
							return TRUE;
						}
					}
*/
				}
			}
#ifdef __ONLINE
			g_DPlay.SendPVendorOpen( (const char*)strVendor );
#endif	// __ONLINE
			CWndButton* pWndButton	= (CWndButton*)GetDlgItem( WIDC_OK );
			pWndButton->SetVisible( FALSE );
		}
		else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
		{
			Destroy( FALSE );
		}
		else if( nID == WIDC_CHAT )
		{
			BOOL bChattingRoom = FALSE;
			if( g_pPlayer->m_vtInfo.IsVendorOpen() )
			{
				g_DPlay.SendOpenChattingRoom();
				bChattingRoom = TRUE;
			}
			else if( g_pPlayer->m_vtInfo.GetOther() && g_Chatting.m_bState )
			{
				bChattingRoom = TRUE;
			}

			if( bChattingRoom )
			{
				SAFE_DELETE( m_pwndVenderMessage );
				m_pwndVenderMessage = new CWndVendorMessage;
				m_pwndVenderMessage->Initialize( );
			}
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndVendor::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndVendorBuy )
	{
		SAFE_DELETE( m_pWndVendorBuy );
	}
	else if( pWndChild == m_pWndRegVend )
	{
		SAFE_DELETE( m_pWndRegVend );
	}
	else if( pWndChild == m_pwndVenderMessage )
	{
		SAFE_DELETE( m_pwndVenderMessage );
	}
}

void CWndVendor::OnDestroy( void )
{
#ifdef __ONLINE
	g_DPlay.SendPVendorClose( m_pVendor->GetId() );
#endif	// __ONLINE
	SAFE_DELETE( m_pWndVendorBuy );
	SAFE_DELETE( m_pWndRegVend );

	SAFE_DELETE( m_pwndVenderMessage );
	
}


CWndVendorMessage::CWndVendorMessage() 
{ 
} 
CWndVendorMessage::~CWndVendorMessage() 
{ 
//	g_Chatting.m_nSizeofMember = 0;
} 
void CWndVendorMessage::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 ); 
	
	pWndList->ResetContent();

	for( int i = 0 ; i < g_Chatting.GetChattingMember(); ++i )
	{
		pWndList->AddString( g_Chatting.m_szName[i] );
	}
} 
void CWndVendorMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );
	pWndEdit->SetNativeMode();

	pWndEdit->SetFocus();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
void CWndVendorMessage::InitSize( void )
{
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndVendorMessage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX_CHAT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndVendorMessage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndVendorMessage::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 10, 16 * 10 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndVendorMessage::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndVendorMessage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndVendorMessage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndVendorMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	switch(nID)
	{
		case WIDC_EDIT1: // 본문 
			if( message == EN_RETURN)
			{
				if( pWndText->m_string.IsEmpty() == FALSE )
				{
					char szChat[1024];
					if( strlen( pWndText->m_string ) < 1020 )
					{
						strcpy( szChat, pWndText->m_string );
#ifdef __ONLINE
						g_DPlay.SendChatting( szChat );
#endif
						pWndText->Empty();
					}
				}
			}
			break;
		case WIDC_BUTTON1: // 본문 
			{
				if( pWndText->m_string.IsEmpty() == FALSE )
				{
					char szChat[1024];
					if( strlen( pWndText->m_string ) < 1020 )
					{
						strcpy( szChat, pWndText->m_string );
#ifdef __ONLINE
						g_DPlay.SendChatting( szChat );
#endif
						pWndText->Empty();
					}
				}
			}
			break;
		case WTBID_CLOSE:
			{
				if( g_pPlayer->m_vtInfo.IsVendorOpen() )
					g_DPlay.SendCloseChattingRoom();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndVendorMessage::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strMessage;

	if( !strcmp( lpszFrom, g_pPlayer->GetName() ) )
		strMessage.Format( "#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	else
		strMessage.Format( "#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	//  	strMessage.Format( "#cff0000ff%s님의 말 :#nc\n  %s\n", lpszFrom, lpszMessage );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();
}

void CWndVendorMessage::AddChattingMemver(LPCTSTR lpszName)
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CString str;
	str.Format( prj.GetText(TID_GAME_VENCHATIN), lpszName );

	CString strMessage;
	strMessage.Format( "#cffff0000%s\n", str );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();	
}

void CWndVendorMessage::RemoveChattingMemver(LPCTSTR lpszName)
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CString str;
	str.Format( prj.GetText(TID_GAME_VENCHATOUT), lpszName );

	CString strMessage;
	strMessage.Format( "#cffff0000%s\n", str );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();	
}

