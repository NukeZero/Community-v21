#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCommand.h"
#include "party.h"

#include "WndManager.h"

#include "WndChatFilter.h"

#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWndEditChat::CWndEditChat()
{
}
CWndEditChat::~CWndEditChat()
{
}
void CWndEditChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*) m_pParentWnd;
	if( pWndChat->m_pWndCommand )
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnLButtonDown( nFlags, point );
}
void CWndEditChat::OnSetFocus(CWndBase* pOldWnd)
{
#ifdef __V050408_CHATBUGFIX
	if( m_string.IsEmpty() )
	{
		CWndChat* pWndChat = (CWndChat*) m_pParentWnd;	

		if( pWndChat->m_pWndCommand == NULL )
		{
			if( pWndChat->m_nChatChannel == CHATSTY_GENERAL )
			{
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_WHISPER )
			{
				CString string;
				if(  pWndChat->m_strCharName.IsEmpty() )
					string.Format( "/w " );
				else
					string.Format( "/w %s ", pWndChat->m_strCharName );
				SetString( string );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_SHOUT )
			{
				SetString( "/s " );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_PARTY )
			{
				SetString( "/p " );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_GUILD )
			{
				SetString( "/g " );
			}
		}
	}
#else // __V050408_CHATBUGFIX
	CWndChat* pWndChat = (CWndChat*) m_pParentWnd;	
	if( pWndChat->m_pWndCommand == NULL )
	{
		if( pWndChat->m_nChatChannel == CHATSTY_GENERAL )
		{
		}
		else
		if( pWndChat->m_nChatChannel == CHATSTY_WHISPER )
		{
			CString string;
			if(  pWndChat->m_strCharName.IsEmpty() )
				string.Format( "/w " );
			else
				string.Format( "/w %s ", pWndChat->m_strCharName );
			SetString( string );
		}
		else
		if( pWndChat->m_nChatChannel == CHATSTY_SHOUT )
		{
			SetString( "/s " );
		}
		else
		if( pWndChat->m_nChatChannel == CHATSTY_PARTY )
		{
			SetString( "/p " );
		}
		else
		if( pWndChat->m_nChatChannel == CHATSTY_GUILD )
		{
			SetString( "/g " );
		}
	}
#endif // __V050408_CHATBUGFIX

	CWndEdit::OnSetFocus( pOldWnd );
}
void CWndEditChat::OnKillFocus(CWndBase* pNewWnd)
{
	CWndChat* pWndChat = (CWndChat*) m_pParentWnd;
	if( pWndChat->m_pWndCommand && this!= pNewWnd )
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnKillFocus( pNewWnd );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 채팅 윈도 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMacroChat::CWndMacroChat()
{
}
CWndMacroChat::~CWndMacroChat()
{
}
void CWndMacroChat::OnDraw( C2DRender* p2DRender )
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if( pWndEdit->m_string.IsEmpty() == FALSE )
		p2DRender->RenderTexture( CPoint( 0, 0 ), &m_texMacroChat );
}
void CWndMacroChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if( pWndEdit->m_string.IsEmpty() == FALSE )
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_CHAT;
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_pTexture = m_pTexMacro; //.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwData = 0;
		//CWndEdit* pWndEdit = (CWndEdit*)pWndChat->GetDlgItem( WIDC_EDIT );
		//strcpy( m_GlobalShortcut.m_szString, pWndEdit->m_string );
		if( pWndEdit->m_string.GetLength() >= MAX_SHORTCUT_STRING-1 )
		{
			memcpy( m_GlobalShortcut.m_szString, pWndEdit->m_string, MAX_SHORTCUT_STRING-1 );
			//SetStrNull( m_GlobalShortcut.m_szString, MAX_SHORTCUT_STRING-1 );
		}
		else
		{
			strcpy( m_GlobalShortcut.m_szString, pWndEdit->m_string );
			//m_GlobalShortcut.m_szString[3] = 0;
			//RepairString( m_GlobalShortcut.m_szString );
			
		}
	}
}

void CWndMacroChat::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndMacroChat::OnInitialUpdate()
{
	//m_ptexMacroe.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ICON, "icon_MacroChat.dds" ), 0xffff00ff );
	m_pTexMacro = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ICON, "icon_MacroChat.dds" ), 0xffff00ff );//, bMyLoader );
	m_texMacroChat.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "wndButtMacroChat.bmp" ), 0xffff00ff );
}
CWndTextChat::CWndTextChat()
{
}
CWndTextChat::~CWndTextChat()
{
}
BOOL CWndTextChat::IsPickupSpace( CPoint point )
{
	return FALSE;
}
#ifdef __VBUGFIX_20050201
int CWndChat::m_nChatChannel = CHATSTY_GENERAL;
#endif
CWndChat::CWndChat()
{
	m_pWndCommand = NULL;
//	m_timerInsMsg.Set( MIN( 1 ) );
	m_timerInsMsg.Set( MIN( 2 ) );  // 도움말 표시 간격 증가
	m_nInsMsgCnt = 0;
#ifndef __VBUGFIX_20050201
	m_nChatChannel = CHATSTY_GENERAL;
#endif
#ifdef __V050322_CHATLOCK
	m_bChatLock = FALSE;
#endif
#ifdef __X0804
	m_bMoveLock = FALSE;
#endif
	m_pWndChatFilter = NULL;
}
CWndChat::~CWndChat()
{
	SAFE_DELETE( m_pWndCommand );
	SAFE_DELETE( m_wndChatEdit[0].lpData );
	SAFE_DELETE( m_wndChatEdit[1].lpData );
	SAFE_DELETE( m_wndChatEdit[2].lpData );
	SAFE_DELETE( m_pWndChatFilter );
}
void CWndChat::OnDraw(C2DRender* p2DRender)
{
	//if( timer.IsTimeOut() )
	//{
	//	timer.Set( SEC( 10 ) );
	//	if( szMessage[ nMsgCnt ][0] )
	//		PutString( szMessage[ nMsgCnt ++ ] );
	//}
	if( g_Option.m_nInstantHelp )
	{
		CString string;
		if( m_strArray.GetSize() && m_timerInsMsg.TimeOut() )
		{
			m_timerInsMsg.Reset();
			//string = "[도움말] ";
			string  = prj.GetText(TID_SYS_HELP);
			string += " ";
			string += m_strArray.GetAt( m_nInsMsgCnt );
			g_WndMng.PutString( string, NULL, 0xffffff00, CHATSTY_HELP );
			m_nInsMsgCnt++;
			if( m_nInsMsgCnt == m_strArray.GetSize() )
				m_nInsMsgCnt = 0;
		}
	}
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 ); // party
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 ); // guild

	pWndRadio4->EnableWindow( g_Party.IsMember( g_pPlayer->m_idPlayer ) );
	pWndRadio5->EnableWindow( g_pPlayer->GetGuild() == NULL ? FALSE : TRUE );
}
void CWndChat::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//AddWndStyle( WBS_THICKFRAME );

	m_timerDobe.Set( SEC( 5 ) );

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_MACRO_CHAT );
	LPWNDCTRL lpWndText = GetWndCtrl( WIDC_CUSTOM1 );
	m_wndMacroChat.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_MACRO_CHAT );
	m_wndMacroChat.AddWndStyle( WBS_NODRAWFRAME );

	lpWndCtrl = GetWndCtrl( WIDC_EDIT );
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_EDIT );
	m_wndText.Create( WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		pWndText->m_bEnableClipboard = TRUE;
		pWndHead->m_bEnableClipboard = TRUE;
		pWndEdit->m_bEnableClipboard = TRUE;
	}
	pWndEdit->SetNativeMode();

	if( g_Option.m_nInstantHelp )
	{
		//CScanner scanner;
		CScript script;
		script.Load( MakePath( DIR_CLIENT, "InstantHelp.inc" ) );

		int nLang;
		nLang = script.GetNumber();
		do 
		{
			script.GetLastFull();
			if( nLang == g_Option.m_nLanguage )
				m_strArray.Add( script.Token );
			nLang = script.GetNumber();
		
		} while( script.tok != FINISHED );
	}
	//m_strArray.

/*
	CRect rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.right = 70;
	rect.left += 5;
	rect.DeflateRect( 1, 1 );
	m_wndBeginning.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, rect, this, 1000);

	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = 75;
	rect.right -= 134;
	rect.DeflateRect( 1, 1 );
	//rect.left = rect.Width() * 20 / 100;
	//m_wndEdit.AddWndStyle( WBS_NODRAWFRAME );
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, rect, this, 1001 );
*/
//	m_texEdit.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndChat2.bmp" ), 0xffff00ff, TRUE );
/*
	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = rect.right - 134;
	rect.right = r mnnnnft + 15;
	rect.DeflateRect( 1, 1 );
	m_wndShout.Create( " ", 0, rect, this, 1010 );

	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = rect.right - 117;
	rect.right = rect.left + 15;
	rect.DeflateRect( 1, 1 );
	m_wndParty.Create( " ", 0, rect, this, 1011 );
*/

/*
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.AddWndStyle( WBS_NOFRAME );
	m_wndText.AddWndStyle( WBS_NODRAWFRAME );
	m_wndText.Create( 0, rect, this, 1003 ); 
	m_wndText.SetFont( g_Neuz.m_Theme.m_pFontChat );
	m_wndText.m_string.Reset( m_pFont, &rect);
*/
/*
	rect = GetClientRect();
	rect.left = rect.right - 100;
	//rect.bottom -= 80;
	rect.DeflateRect( 1, 1 );
	m_wndReceiver.AddWndStyle( WBS_VSCROLL );
	m_wndReceiver.AddWndStyle( WBS_NOFRAME );
	m_wndReceiver.Create( EBS_AUTOVSCROLL, rect, this, 1002 );
	m_wndReceiver.AddString( "Vampyre\n" );
	m_wndReceiver.AddString( "Xuzhu\n" );
	m_wndReceiver.AddString( "Zodiacus\n" );
	m_wndReceiver.AddString( "Jeff\n" );
	m_wndReceiver.m_string.Reset( m_pFont, &rect);
*/

	//CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	SetChannel();
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	if( m_nChatChannel == CHATSTY_GENERAL ) pWndRadio1->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_WHISPER ) pWndRadio2->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_SHOUT   ) pWndRadio3->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_PARTY   ) pWndRadio4->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_GUILD   ) pWndRadio5->SetCheck( TRUE );
	//pWndText->m_string.Init(
	//pWndText->m_string = g_WndMng.m_ChatString;
	pWndText->m_nLineSpace = 2;
	//pWndText->m_string.Reset( m_pFont, &pWndText->GetClientRect() );
	
	//pWndText->
	/*
	CRect rect = GetClientRect();
	rect.bottom -= 20;
	rect.right -= 0;//100;
	rect.DeflateRect( 1, 1);
	m_wndText.SetWndRect( rect );
*/
	//LPBYTE m_wndChatEdit[3];
	//CSize sizeWndChatEdit[3];
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit00.tga" ), &m_wndChatEdit[0] );
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit01.tga" ), &m_wndChatEdit[1] );
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit02.tga" ), &m_wndChatEdit[2] );
/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( 154, rectRoot.bottom - rectWindow.Height() );
	Move( point );
*/
#ifdef __V050322_CHATLOCK
	CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndLock->SetCheck( m_bChatLock );
#endif
#ifdef __X0804
	CWndButton* pWndMoveLock = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	pWndMoveLock->SetCheck( m_bMoveLock );
//	DWORD dwStyle = pWndMoveLock->GetWndStyle();
//	pWndMoveLock->SetWndStyle( dwStyle | WBS_CHECK );
#endif
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );
}
void CWndChat::SetChannel()
{
	m_wndText.SetString( "" );
#ifdef __VCHATFILTER_20050316
	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
#ifdef __V050523_BUGFIX_CHATFILTER
	DWORD dwChatFilter = g_Option.m_dwChatFilterEtc | g_Option.m_dwChatFilter[ nChannel ];
#else
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
#endif
#endif
	for( int i = 0; i < g_WndMng.m_aChatString.GetSize(); i++ )
	{
#ifdef __VCHATFILTER_20050316
		//CString string2 = g_WndMng.m_aChatString.GetAt( i );
		//DWORD d = g_WndMng.m_aChatStyle.GetAt( i );
		if( g_WndMng.m_aChatStyle.GetAt( i ) & dwChatFilter ) 
#else
		if( g_WndMng.m_aChatStyle.GetAt( i ) == m_nChatChannel || m_nChatChannel == CHATSTY_GENERAL ) //on.m_dwChatFilter )
#endif
		{
			CString string = g_WndMng.m_aChatString.GetAt( i );
#ifdef __VBUGFIX_20050105
			m_wndText.m_string.AddParsingString( string, g_WndMng.m_aChatColor.GetAt( i ) );
#else
			m_wndText.m_string.AddString( string, g_WndMng.m_aChatColor.GetAt( i ) );
#endif
			m_wndText.m_string.AddString( "\n" );
		}
	}
	m_wndText.ResetString();
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
}
void CWndChat::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChat::OnKillFocus(CWndBase* pNewWnd)
{
	//if( m_pWndCommand && pNewWnd != m_pWndCommand )
	//	m_pWndCommand->Destroy();
}
BOOL CWndChat::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height() );
	SetTitle( GETTEXT( TID_APP_COMMUNICATION_CHAT ) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY| WBS_THICKFRAME, CPoint( 792, 130 ), pWndParent );
	//return CWndNeuz::Create( 0|WBS_MOVE|/*WBS_DOCKABLE|*/WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME|WBS_MAXIMIZEBOX, rect, &g_WndMng, dwWndId);
}

/*
	"applet" TY_CLIENT 0
	"to"     TY_SERVER 0 
	"to"     TY_SERVER 0
	"to"     TY_SERVER 0
*/

#ifdef __ONLINE
extern	CDPClient	g_DPlay;
#endif
#ifdef __GM_COMMAND_ITEM
//extern int ParsingCommand( LPCTSTR lpszString, CMover* pMover, BOOL bItem = FALSE );
#else // __GM_COMMAND_ITEM
extern int ParsingCommand( LPCTSTR lpszString, CMover* pMover );
#endif // __GM_COMMAND_ITEM

void CWndChat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pWndCommand == pWndChild )
	{
		SAFE_DELETE( m_pWndCommand );
	}
	if( m_pWndChatFilter == pWndChild )
		SAFE_DELETE( m_pWndChatFilter );
}
#ifdef __V050322_CHATLOCK
void CWndChat::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		{
			ar >> m_bChatLock;
		}
#ifdef __V050322_CHATLOCK
		CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
		pWndLock->SetCheck( m_bChatLock );
#endif
	}
	else
	{
		dwVersion = 1;
		ar << m_bChatLock;
	}
}
#endif
BOOL CWndChat::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifdef __VCHATFILTER_20050316
	switch(nID)
	{
		case WIDC_CLOSE:
			nID = WTBID_CLOSE;
			break;
		case WIDC_HELP:
			nID = WTBID_HELP;
			break;
		case WIDC_BUTTON1:
			{
				if( m_pWndChatFilter == NULL )
				{
					m_pWndChatFilter = new CWndChatFilter;
					m_pWndChatFilter->Initialize( this );
				}
			}
			break;
#ifdef __V050322_CHATLOCK
		case WIDC_CHECK1: // 채팅 입력창 잠구기 
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
				m_bChatLock = pWndCheck->GetCheck();
			}
			break;
#endif
#ifdef __X0804
		case WIDC_MOVELOCK: // 이동 잠그기
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
				m_bMoveLock = pWndCheck->GetCheck();
				DWORD dwStyle = GetWndStyle();
				if( m_bMoveLock )
					dwStyle &= (~WBS_MOVE);
				else
					dwStyle |= WBS_MOVE;
				SetWndStyle( dwStyle );
			}
			break;
#endif
		case WIDC_RADIO1:
			m_nChatChannel = CHATSTY_GENERAL;
			m_wndEdit.SetString( "" );
			m_wndEdit.SetFocus();
			/*
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_WHISPER;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_SHOUT;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_PARTY;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GUILD;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			*/
			SetChannel();
			break;
		case WIDC_RADIO2:
			m_nChatChannel = CHATSTY_WHISPER;
			m_wndEdit.SetString( "/w " );
			m_wndEdit.SetFocus();
			/*
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_WHISPER;
			*/
			SetChannel();
			break;
		case WIDC_RADIO3:
			m_nChatChannel = CHATSTY_SHOUT;
			m_wndEdit.SetString( "/s " );
			m_wndEdit.SetFocus();
			/*
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_SHOUT;
			*/
			SetChannel();
			break;
		case WIDC_RADIO4:
			m_nChatChannel = CHATSTY_PARTY;
			m_wndEdit.SetString( "/p " );
			m_wndEdit.SetFocus();
			/*
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_PARTY;
			*/
			SetChannel();
			break;
		case WIDC_RADIO5:
			m_nChatChannel = CHATSTY_GUILD;
			m_wndEdit.SetString( "/g " );
			m_wndEdit.SetFocus();
			/*
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter[m_nChatChannel] &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter[m_nChatChannel] |= CHATSTY_GUILD;
			*/
			SetChannel();
			break;
		case APP_COMMAND:
			{
				if( message == WNM_DBLCLK )
				{
					CString string;
					m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), string );
					string += " ";
					m_wndEdit.SetString( "/" );
					m_wndEdit.AddString( string );
					m_wndEdit.ResetString();
					m_pWndCommand->Destroy();
				}
				SetFocus();
			}
		case WIDC_HEAD: // 말머리
			if( message == EN_RETURN )
			{
				g_WndMng.SetFocus();
			}
			break;
		case WIDC_EDIT: // 본문
#ifdef _DEBUG
//			if( message == EN_UP )		// 일본버전에서 컴팔에러 나서 잠시 막았습니다. xuzhu
//			{
//				if( m_strChatBackup.IsEmpty() )
//					return FALSE;
//
//				m_wndEdit.m_string = m_strChatBackup;
//				return TRUE;
//			}
#endif
			
			if( message == EN_RETURN )
			{
				BOOL bFinished = TRUE;
				if( m_pWndCommand )
				{
					CString str1, str2;
					str1 = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
					if( m_pWndCommand->GetCurSel() != -1 )
					{
						m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), str2 );
						if( str1 != str2 )
						{
							str2 += " ";
							m_wndEdit.SetString( "/" );
							m_wndEdit.AddString( str2 );
							m_wndEdit.ResetString();
							bFinished = FALSE;
						}
					}
					m_pWndCommand->Destroy();
				}
				if( bFinished == TRUE )
				{
					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
					CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
					CString string; 
					if( pWndHead->m_string.IsEmpty() == FALSE && 
						( pWndHead->m_string[ 0 ] == '/' || 
						( m_wndEdit.m_string.IsEmpty() == FALSE && pWndHead->m_string[ 0 ] != '/' && m_wndEdit.m_string[ 0 ] != '/' ) ) 
					)
						string = pWndHead->m_string + " ";
#if !defined(_MULTI_LANGUAGE_)
					string += m_wndEdit.m_string;
#else //_MULTI_LANGUAGE_
					WORD wCodePage = g_codePage;
					const char* begin = m_wndEdit.m_string;
					const char* end = begin + m_wndEdit.m_string.GetLength();
					const char* iter = begin;
					while(*iter && iter < end)
					{
						if(*((BYTE*)iter) >= 0x80) 
						{
							if(wCodePage != m_wndEdit.m_string.m_awCodePage[iter-begin])
							{
								wCodePage = m_wndEdit.m_string.m_awCodePage[iter-begin];
								
								char temp[16];
								
								if(wCodePage == g_codePage)
								{
									wsprintf(temp, "#nl");
								}
								else
								{
									wsprintf(temp, "#l%04d", wCodePage);
								}
								string += temp;
							}
						}
						const char* next = CharNextEx(iter, wCodePage);
						char temp[16];
						memcpy(temp, iter, next-iter);
						temp[next-iter] = 0;
						string += temp;
						iter = next;
					}
#endif //_MULTI_LANGUAGE_
#ifdef _DEBUG
					m_strChatBackup = string;
#endif 
					int a = string.GetLength();
					g_WndMng.ParsingChat( string );
					CScanner scanner;
					scanner.SetProg( m_wndEdit.m_string.LockBuffer() );
					scanner.GetToken(); // 
					if( scanner.Token == "/" )
					{
						scanner.GetToken();
						if( scanner.Token == "w" || scanner.Token == "whisper" )
						{
							scanner.GetToken();
							m_strCharName = scanner.Token;
						}
					}
					m_wndEdit.m_string.UnlockBuffer();
					m_wndEdit.Empty();
#ifdef __V050322_CHATLOCK
					if( m_bChatLock == FALSE ) 
#endif
					{
						CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
						if( pWndWorld ) pWndWorld->SetFocus();
						else g_WndMng.SetFocus();
					}
#ifdef __V050527_BUGFIX_CHATLOCK
					else
						m_wndEdit.SetFocus();
#endif
				}
			}
			else
			if( message == EN_CHANGE )
			{
				if( m_wndEdit.m_string.IsEmpty() == FALSE )
				{
					if( m_wndEdit.m_string[ 0 ] == '/' && m_wndEdit.GetOffset() == 1 && m_pWndCommand == NULL )
					{
						m_pWndCommand = new CWndCommand;
						m_pWndCommand->m_pWndEdit = this;
						m_pWndCommand->Initialize( this );
						CRect rect = m_wndEdit.GetScreenRect();
						CPoint point = rect.TopLeft();
						point.y -= m_pWndCommand->GetWndRect().Height();
						m_pWndCommand->Move( point );
					}
					else
					if( m_pWndCommand )
					{
						CString string; 
						string = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
						// 중간에 스페이스가 껴 있으면 닫아버린다.
						if( string.Find( ' ' ) == -1 )
							m_pWndCommand->SelectString( 0, string );
						else
							m_pWndCommand->Destroy();
						
					#if __CURRENT_LANG == LANG_KOR
						if( m_wndEdit.m_string == "/ㅎ " )
						{
							m_wndEdit.SetString( "/g " );
						}
						else
						if( m_wndEdit.m_string == "/ㅔ " )
						{
							m_wndEdit.SetString( "/p " );
						}
						else
						if( m_wndEdit.m_string == "/ㄴ " )
						{
							m_wndEdit.SetString( "/s " );
						}						
					#endif //__CURRENT_LANG == LANG_JAP
					}
				}
				else
				if( m_pWndCommand )
					m_pWndCommand->Destroy();
			}
			break;
		case 10000: // 최소화 
			break;
		case 10001: // 최대화 
			break;
		case 10002: // 종료

			Destroy();
			break;
	}
	
	if( message == EN_CHANGE )
	{


	}
#else // __VCHATFILTER_20050316
	switch(nID)
	{
		case WIDC_CLOSE:
			nID = WTBID_CLOSE;
			break;
		case WIDC_HELP:
			nID = WTBID_HELP;
			break;
		case WIDC_BUTTON1:
			{
				if( m_pWndChatFilter == NULL )
				{
					m_pWndChatFilter = new CWndChatFilter;
					m_pWndChatFilter->Initialize( this );
				}
			}
			break;
		case WIDC_RADIO1:
			m_nChatChannel = CHATSTY_GENERAL;
			m_wndEdit.SetString( "" );
			m_wndEdit.SetFocus();
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
			g_Option.m_dwChatFilter |= CHATSTY_WHISPER;
			g_Option.m_dwChatFilter |= CHATSTY_SHOUT;
			g_Option.m_dwChatFilter |= CHATSTY_PARTY;
			g_Option.m_dwChatFilter |= CHATSTY_GUILD;
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
			SetChannel();
			break;
		case WIDC_RADIO2:
			m_nChatChannel = CHATSTY_WHISPER;
			m_wndEdit.SetString( "/w " );
			m_wndEdit.SetFocus();
#ifdef __VBUGFIX_20050201
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
#else
			g_Option.m_dwChatFilter &= ~CHATSTY_GENERAL;
#endif
			g_Option.m_dwChatFilter &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter |= CHATSTY_WHISPER;
			SetChannel();
			break;
		case WIDC_RADIO3:
			m_nChatChannel = CHATSTY_SHOUT;
			m_wndEdit.SetString( "/s " );
			m_wndEdit.SetFocus();
#ifdef __VBUGFIX_20050201
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
#else
			g_Option.m_dwChatFilter &= ~CHATSTY_GENERAL;
#endif
			g_Option.m_dwChatFilter &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter |= CHATSTY_SHOUT;
			SetChannel();
			break;
		case WIDC_RADIO4:
			m_nChatChannel = CHATSTY_PARTY;
			m_wndEdit.SetString( "/p " );
			m_wndEdit.SetFocus();
#ifdef __VBUGFIX_20050201
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
#else
			g_Option.m_dwChatFilter &= ~CHATSTY_GENERAL;
#endif
			g_Option.m_dwChatFilter &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter |= CHATSTY_PARTY;
			SetChannel();
			break;
		case WIDC_RADIO5:
			m_nChatChannel = CHATSTY_GUILD;
			m_wndEdit.SetString( "/g " );
			m_wndEdit.SetFocus();
#ifdef __VBUGFIX_20050201
			g_Option.m_dwChatFilter |= CHATSTY_GENERAL;
#else
			g_Option.m_dwChatFilter &= ~CHATSTY_GENERAL;
#endif
			g_Option.m_dwChatFilter &= ~CHATSTY_WHISPER;
			g_Option.m_dwChatFilter &= ~CHATSTY_SHOUT;
			g_Option.m_dwChatFilter &= ~CHATSTY_PARTY;
			g_Option.m_dwChatFilter &= ~CHATSTY_GUILD;
			g_Option.m_dwChatFilter |= CHATSTY_GUILD;
			SetChannel();
			break;
		case APP_COMMAND:
			{
				if( message == WNM_DBLCLK )
				{
					CString string;
					m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), string );
					string += " ";
					m_wndEdit.SetString( "/" );
					m_wndEdit.AddString( string );
					m_wndEdit.ResetString();
					m_pWndCommand->Destroy();
				}
				SetFocus();
			}
		case WIDC_HEAD: // 말머리
			if( message == EN_RETURN )
			{
				g_WndMng.SetFocus();
			}
			break;
		case WIDC_EDIT: // 본문
#ifdef _DEBUG
//			if( message == EN_UP )		// 일본버전에서 컴팔에러 나서 잠시 막았습니다. xuzhu
//			{
//				if( m_strChatBackup.IsEmpty() )
//					return FALSE;
//
//				m_wndEdit.m_string = m_strChatBackup;
//				return TRUE;
//			}
#endif
			
			if( message == EN_RETURN )
			{
				BOOL bFinished = TRUE;
				if( m_pWndCommand )
				{
					CString str1, str2;
					str1 = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
					if( m_pWndCommand->GetCurSel() != -1 )
					{
						m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), str2 );
						if( str1 != str2 )
						{
							str2 += " ";
							m_wndEdit.SetString( "/" );
							m_wndEdit.AddString( str2 );
							m_wndEdit.ResetString();
							bFinished = FALSE;
						}
					}
					m_pWndCommand->Destroy();
				}
				if( bFinished == TRUE )
				{
					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
					CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
					CString string; 
					if( pWndHead->m_string.IsEmpty() == FALSE && 
						( pWndHead->m_string[ 0 ] == '/' || 
						( m_wndEdit.m_string.IsEmpty() == FALSE && pWndHead->m_string[ 0 ] != '/' && m_wndEdit.m_string[ 0 ] != '/' ) ) 
					)
						string = pWndHead->m_string + " ";
#if !defined(_MULTI_LANGUAGE_)
					string += m_wndEdit.m_string;
#else //_MULTI_LANGUAGE_
					WORD wCodePage = g_codePage;
					const char* begin = m_wndEdit.m_string;
					const char* end = begin + m_wndEdit.m_string.GetLength();
					const char* iter = begin;
					while(*iter && iter < end)
					{
						if(*((BYTE*)iter) >= 0x80) 
						{
							if(wCodePage != m_wndEdit.m_string.m_awCodePage[iter-begin])
							{
								wCodePage = m_wndEdit.m_string.m_awCodePage[iter-begin];
								
								char temp[16];
								
								if(wCodePage == g_codePage)
								{
									wsprintf(temp, "#nl");
								}
								else
								{
									wsprintf(temp, "#l%04d", wCodePage);
								}
								string += temp;
							}
						}
						const char* next = CharNextEx(iter, wCodePage);
						char temp[16];
						memcpy(temp, iter, next-iter);
						temp[next-iter] = 0;
						string += temp;
						iter = next;
					}
#endif //_MULTI_LANGUAGE_
#ifdef _DEBUG
					m_strChatBackup = string;
#endif 
					int a = string.GetLength();
					g_WndMng.ParsingChat( string );
					CScanner scanner;
					scanner.SetProg( m_wndEdit.m_string.LockBuffer() );
					scanner.GetToken(); // 
					if( scanner.Token == "/" )
					{
						scanner.GetToken();
						if( scanner.Token == "w" || scanner.Token == "whisper" )
						{
							scanner.GetToken();
							m_strCharName = scanner.Token;
						}
					}
					m_wndEdit.m_string.UnlockBuffer();
					m_wndEdit.Empty();
					CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
					if( pWndWorld ) pWndWorld->SetFocus();
					else g_WndMng.SetFocus();
				}
			}
			else
			if( message == EN_CHANGE )
			{
				if( m_wndEdit.m_string.IsEmpty() == FALSE )
				{
					if( m_wndEdit.m_string[ 0 ] == '/' && m_wndEdit.GetOffset() == 1 && m_pWndCommand == NULL )
					{
						m_pWndCommand = new CWndCommand;
						m_pWndCommand->m_pWndEdit = this;
						m_pWndCommand->Initialize( this );
						CRect rect = m_wndEdit.GetScreenRect();
						CPoint point = rect.TopLeft();
						point.y -= m_pWndCommand->GetWndRect().Height();
						m_pWndCommand->Move( point );
					}
					else
					if( m_pWndCommand )
					{
						CString string; 
						string = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
						// 중간에 스페이스가 껴 있으면 닫아버린다.
						if( string.Find( ' ' ) == -1 )
							m_pWndCommand->SelectString( 0, string );
						else
							m_pWndCommand->Destroy();
					}
				}
				else
				if( m_pWndCommand )
					m_pWndCommand->Destroy();
			}
			break;
		case 10000: // 최소화 
			break;
		case 10001: // 최대화 
			break;
		case 10002: // 종료

			Destroy();
			break;
	}
	
	if( message == EN_CHANGE )
	{


	}
#endif // __VCHATFILTER_20050316
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndChat::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndChat::Parsing( CString string )
{
	/*
	MotionProp* pProp;
	for( int i = 0; i < prj.m_aMotionProp.GetSize(); i++ )
	{
		pProp = prj.GetMotionProp( i );
		if( pProp && pProp->dwRequireLv )
		{
			if( string.Find( pProp->szLink ) != -1 )
			{
			//	g_pPlayer->SendActMsg( OBJMSG_MOTION, pProp->dwID );
			}
		}
	}
	*/
}
void CWndChat::PutString( LPCTSTR lpszString, DWORD dwColor )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	/*
	pWndText->m_string.AddString( lpszString, dwColor );
	pWndText->m_string.AddString( "\n", dwColor );// += '\n';
	pWndText->m_string.Reset( m_pFont, &pWndText->GetClientRect() );
	
	pWndText->UpdateScrollBar();
	*/
#ifdef __VBUGFIX_20050201
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;
#endif
	pWndText->AddString( lpszString, dwColor );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	//pWndText->ResetString();// m_pFont, &pWndText->GetClientRect() );
#ifdef __VBUGFIX_20050201
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
#else	
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif
}
void CWndChat::OnDestroy()
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	g_WndMng.m_ChatString = pWndText->m_string;
}
/*
void CWndChat::PutDialog( LPCTSTR lpszString, CObj* pObj )
{
#ifdef __ONLINE
	TCHAR* lpszChat		= _tcschr( lpszString, _T( ':' ) );
	lpszChat	+= sizeof(TCHAR) * 2;	// Skip ": "
	g_DialogMsg.AddMessage( pObj, lpszChat, 0xffffffff );
#else
	g_DialogMsg.AddMessage( pObj, lpszString, 0xffffffff );
#endif
}
*/   
void CWndChat::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
#ifdef __V050322_CHATLOCK
	AdjustMinRect( &rectWnd, 16 * 26, 16 * 3 );
#else 
	AdjustMinRect( &rectWnd, 16 * 23, 16 * 3 );
#endif
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}
void CWndChat::OnSize(UINT nType, int cx, int cy)
{
//#ifdef __NEWINTERFACE
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

	CRect rectEdit;
	CRect rect = GetClientRect();
	//rect.left = 60;
	rect.top = 0;
	rect.left = 0;
	rect.right -= 0;
	rect.bottom -= 39;
	pWndText->SetWndRect( rect );
	pWndText->m_wndScrollBar.SetMaxScrollPos();
	
	rect = GetClientRect();
	rectEdit = pWndHead->GetWndRect();
	rectEdit.top = rect.bottom - 18;
	rectEdit.bottom = rect.bottom;
	rect.DeflateRect( 1, 1 );
	pWndHead->SetWndRect( rectEdit );

	rect = GetClientRect();
	rectEdit = m_wndEdit.GetWndRect();
	rectEdit.top = rect.bottom - 18;
	rectEdit.bottom = rect.bottom;
	rectEdit.right = rect.right;
	rect.DeflateRect( 1, 1 );
	m_wndEdit.SetWndRect( rectEdit );

	rect = GetClientRect();
	rectEdit = m_wndMacroChat.GetWndRect();
	rectEdit.top = rect.bottom - 19;
	rectEdit.bottom = rect.bottom;
	m_wndMacroChat.SetWndRect( rectEdit );
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
#ifdef __V050322_CHATLOCK
	CWndButton* pWndCheck  = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
#endif
#ifdef __X0804
	CWndButton* pWndCheck2  = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
#endif
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_HELP   );
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_CLOSE  );

	rect = GetClientRect();
	rectEdit = pWndRadio1->GetWndRect();
	rectEdit.top = rect.bottom - 38;
	rectEdit.left = 0;
	pWndRadio1->Move( rectEdit.TopLeft() ); 
	rectEdit.left = 50;
	pWndRadio2->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 100;
	pWndRadio3->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 150;
	pWndRadio4->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 200;
	pWndRadio5->Move( rectEdit.TopLeft()  ); 

#ifdef __X0804
 #ifdef __V050322_CHATLOCK
	rectEdit.left = rect.right - 150;
	pWndCheck->Move( rectEdit.TopLeft()  ); 
 #endif
	rectEdit.left = rect.right - 100;
	pWndButton1->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = rect.right - 49;

	CRect rectEdit2 = rectEdit;
	rectEdit2.top  += 1;
	pWndCheck2->Move( rectEdit2.TopLeft()  ); 
#else // x0804
 #ifdef __V050322_CHATLOCK
	rectEdit.left = rect.right - 130;
	pWndCheck->Move( rectEdit.TopLeft()  ); 
 #endif
	rectEdit.left = rect.right - 80;
	pWndButton1->Move( rectEdit.TopLeft()  ); 
#endif // not x0804
	rectEdit.left = rect.right - 32;
	rectEdit.top += 1;
	pWndButton2->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = rect.right - 15;
	pWndButton3->Move( rectEdit.TopLeft()  ); 
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndChat::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
#ifdef __X0804
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonUp( nFlags, p );
	}
#endif
	
}
void CWndChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pWndCommand )
		m_pWndCommand->Destroy();

#ifdef __X0804
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
	}
#endif
}

void CWndChat::OnRButtonDown(UINT nFlags, CPoint point)
{
#ifdef __X0804
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
	}
#endif
}	
void CWndChat::OnRButtonUp(UINT nFlags, CPoint point)
{
#ifdef __X0804
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonUp( nFlags, p );
	}
#endif
}	

void CWndChat::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __X0804
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnMouseMove( nFlags, p );
	}
#endif
}



BOOL CWndChat::OnEraseBkgnd(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();
	rect.right = 60;
	rect.bottom -= 30;
	//p2DRender->RenderFillRect( rect, 0xff606060 );
	//oint pt = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	//CRect rect = GetClientRect();
//	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );

	//rect.right -= 70;
	//rect.OffsetRect(pt);
//	rect.bottom -= 23;
//if(m_nWinSize == 2)
//		p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 70, 70, 70 ) );
//	else
//		p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100,   0,   0,   0 ) );

	//m_texEdit.Render( p2DRender, CPoint( 3, rect.bottom - 20 ) );

//	rect = GetClientRect();
//	rect.left = rect.right - 70;
/*
	rect.DeflateRect(-1,-1,-1,0);
	dwColor1 = D3DCOLOR_ARGB( 255, 130, 130, 230 );
	dwColor2 = D3DCOLOR_ARGB( 255, 100, 100, 200 );
	dwColor3 = D3DCOLOR_ARGB( 255, 180, 180, 220 );

	GradationRect( p2DRender, rect, dwColor1 ,dwColor2, dwColor3 );
	p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3);
*/
//	DWORD dwColor1t = D3DCOLOR_ARGB( 255, 250, 250, 255 );
//	DWORD dwColor1b = D3DCOLOR_ARGB( 255, 200, 200, 210 );
//	DWORD dwColor2b = D3DCOLOR_ARGB( 255, 143, 173, 245 );

	//GradationRect( p2DRender, rect, dwColor1t, dwColor1b, dwColor2b ); 
//	rect = GetClientRect();
//	rect.left = rect.right - 100;
	//rect.DeflateRect( 1, 1 );
	//p2DRender->RenderRoundRect( rect, dwColor2b );
	//p2DRender->RenderLine( rect.TopLeft(), CPoint( rect.left, rect.bottom ), dwColor2b );

	return TRUE;
}
//void CWndNeuz::PaintTexture( LPBYTE pDest, LPBYTE pSrc, CPoint pt,
// CSize sizeTexture, CSize sizeSurface )
#ifdef __V050322_CACHEQUEST
void CWndChat::AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
#else
void CWndChat::AdditionalSkinTexture( LPWORD pDest, CSize size )
#endif
{
#ifdef __V050322_CACHEQUEST
	CWndNeuz::AdditionalSkinTexture( pDest, size, d3dFormat );
#endif
	CRect rect = GetWindowRect();
	CPoint point =  CPoint( 0, rect.Height() - m_wndChatEdit[0].size.cy );
	PaintTexture( pDest, &m_wndChatEdit[0], point, size );
	int nWidth = ( rect.Width() - m_wndChatEdit[0].size.cx ) / m_wndChatEdit[1].size.cx;
	point.x += m_wndChatEdit[0].size.cx;
	for( int i = 0; i < nWidth; i++ )
	{
		if( i == nWidth - 2 )
			PaintTexture( pDest, &m_wndChatEdit[2], point, size );
		else
		if( i < nWidth - 2 )
			PaintTexture( pDest, &m_wndChatEdit[1], point, size );
		point.x += m_wndChatEdit[1].size.cx;
	}
}





















