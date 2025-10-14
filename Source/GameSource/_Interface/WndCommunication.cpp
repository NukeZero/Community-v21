#include "stdafx.h"
#include "WndCommand.h"
#include "party.h"

#include "WndManager.h"

#include "WndChatFilter.h"

#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __HYPERLINK_ITEM16
#include "WndTooltip.h"
#endif //__HYPERLINK_ITEM16

#define		MAX_HISTORY_STRING		20

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
			if( pWndChat->m_nChatChannel == CHATSTY_ALL )
			{
			}
		}
	}


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

		CString strFormat;
		strFormat = pWndEdit->m_string;

		pWndEdit->m_string.GetTextFormat(strFormat);
		
		if( pWndEdit->m_string.GetLength() >= MAX_SHORTCUT_STRING-1 )
		{
			memcpy( m_GlobalShortcut.m_szString, strFormat, MAX_SHORTCUT_STRING-1 );
		}
		else
		{
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), strFormat );
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
	m_texMacroChat.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"wndButtMacroChat.bmp" ), 0xffff00ff );
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
int CWndChat::m_nChatChannel = CHATSTY_GENERAL;

#ifdef __IMPROVE_MAP_SYSTEM
CWndChat::CWndChat( void ) : 
m_bHyperlinked( FALSE ), 
m_eHyperlinkMode( HL_None ), 
m_dwHyperlinkTextStyle( ESSTY_UNDERLINE ), 
m_nHyperlinkTextStartPosition( -1 ), 
m_nHyperlinkTextLeftPosition( -1 ), 
m_nHyperlinkTextRightPosition( -1 ), 
m_strHyperlinkMarkTitle( _T( "" ) ), 
m_fHyperlinkMarkPositionX( -1.0F ), 
m_fHyperlinkMarkPositionY( -1.0F ),
m_nSelectedLineForHLITEM( 0 )
#else // __IMPROVE_MAP_SYSTEM
CWndChat::CWndChat()
#endif // __IMPROVE_MAP_SYSTEM
{
	m_pWndCommand = NULL;
//	m_timerInsMsg.Set( MIN( 1 ) );
	m_timerInsMsg.Set( MIN( 2 ) );  // 도움말 표시 간격 증가
	m_timerAlertGGrade.Set( MIN(60) );	//sun: 10, __CSC_GAME_GRADE
	m_nInsMsgCnt = 0;
	m_bChatLock = FALSE;
	m_bMoveLock = FALSE;
	m_pWndChatFilter = NULL;
	m_nHistoryIndex = 0;
	m_bChatLog = FALSE;		//sun: 8차, 'T'키 사용, 상대방 레벨 안보이기, 화면 모든 창 On/Off Neuz
	g_WndMng.m_pWndChatNotice = NULL;
	m_nMode = MODE_NORMAL;
}
CWndChat::~CWndChat()
{
	SAFE_DELETE( m_pWndCommand );
	SAFE_DELETE( m_pWndChatFilter );
	SAFE_DELETE( g_WndMng.m_pWndChatNotice );
#ifdef __HYPERLINK_ITEM16
	m_cHyperLinkItems.clear( );
#endif //__HYPERLINK_ITEM16
}
void CWndChat::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL )
		return;
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

//sun: 10, __CSC_GAME_GRADE
	if(m_timerAlertGGrade.TimeOut())
	{
		m_timerAlertGGrade.Reset();
		//PK서버 구분  12세 or 15세
		if(g_eLocal.GetState( EVE_PK ))
			g_WndMng.PutString( prj.GetText(TID_SYS_ALERTGGRADE_PK), NULL, 0xffffff00, CHATSTY_HELP );
		else
			g_WndMng.PutString( prj.GetText(TID_SYS_ALERTGGRADE_NONPK), NULL, 0xffffff00, CHATSTY_HELP );		
	}

	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 ); // party
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 ); // guild

	BOOL bCanPartyChat = g_Party.IsMember( g_pPlayer->m_idPlayer );

	//Event arena 에서는 파티챗 사용가능
	if( FALSE == bCanPartyChat )
	{
		CWorld* pWorld = g_pPlayer->GetWorld();
		bCanPartyChat = ( pWorld ? pWorld->IsEventArena() : FALSE );
	}

	pWndRadio4->EnableWindow( bCanPartyChat );
	pWndRadio5->EnableWindow( g_pPlayer->GetGuild() == NULL ? FALSE : TRUE );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR || g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )
	{
		//static TCHAR text[12] = { 0 };
		TCHAR text[12] = { 0, };	//gmpbigsun(20100809) : text가 static해서 항상 초기화되지 않는 문제가 있다.
		static wchar_t* wText = g_imeMgr.GetStringIndicator();

		if( wText )
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 

			if( pWndStatic )
			{
				WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wText, wcslen(wText), text, 12, NULL, NULL);		
				
				pWndStatic->m_dwColor = D3DCOLOR_ARGB( 255, 50, 50, 220 );

				if( PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_KOREAN )
				{
					if( g_imeMgr.GetImeState() == 0 )
					{
						FLStrcpy( text, _countof( text ), "En" );
					}
				}
				else
				if( PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_CHINESE )
				{
					static wchar_t wTextCh[3][3] = 
					{ 
						L"\x6CE8", 
						L"\x65B0", 
						L"\x5176", 
					};
					
					if( g_imeMgr.IsNewPhonetic() )
					{
						if( g_imeMgr.GetImeState() == 2 )
							FLStrcpy( text, _countof( text ), "En" );
						else
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[1], wcslen(wTextCh[1]), text, 12, NULL, NULL);		
					}
					else
					if( g_imeMgr.IsPhonetic() )
					{
						if( g_imeMgr.GetImeState() == 0 )
							FLStrcpy( text, _countof( text ), "En" );
						else	
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[0], wcslen(wTextCh[0]), text, 12, NULL, NULL);		
					}
					else
					{
						if( g_imeMgr.GetImeState() == 0 )
							FLStrcpy( text, _countof( text ), "En" );
						else	
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[2], wcslen(wTextCh[2]), text, 12, NULL, NULL);		
					}
				}
				pWndStatic->SetTitle( text );
			}
		}
	}
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
	
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_EDIT );
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_EDIT, 255, 255, 255 );
	
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

//sun: 11, <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제
	CRect rectEdit = m_wndEdit.GetWndRect();
	rectEdit.left = rectEdit.left - 90;
	m_wndEdit.SetWndRect( rectEdit );

	CRect customRect = lpWndCtrl->rect;
	CWndCustom* pCustom = (CWndCustom*)GetDlgItem(WIDC_MACRO_CHAT);
	if(pCustom)
	{
		pCustom->Move(customRect.TopLeft().x - 119, customRect.TopLeft().y);
	}

	if(pWndHead)
	{
		pWndHead->EnableWindow(FALSE);
		pWndHead->SetVisible(FALSE);
	}

	if( g_Option.m_nInstantHelp )
	{
		CScript	s;
		s.Load( MakePath( DIR_CLIENT, "InstantHelp.inc" ) );
		s.GetToken();
		while( s.tok != FINISHED )
		{
			m_strArray.Add( s.Token );
			s.GetToken();
		}
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
	SetChannel( TRUE );
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	CWndButton* pWndRadio6 = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	if( m_nChatChannel == CHATSTY_GENERAL ) pWndRadio1->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_WHISPER ) pWndRadio2->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_SHOUT   ) pWndRadio3->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_PARTY   ) pWndRadio4->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_GUILD   ) pWndRadio5->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_ALL	  ) 
	{
		if( pWndRadio6 )
			pWndRadio6->SetCheck( TRUE );	
	}
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
//sun: 11, <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제

	//flyingjin : Default 폴더를 없애고 국가별 폴더로 넣어준다
	LoadTGA( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndChatEdit00_1.tga" ), &m_wndChatEdit[0] );
//	LoadTGA( MakePath( DIR_THEME, "WndChatEdit00.tga" ), &m_wndChatEdit[0] );	

	LoadTGA( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndChatEdit01.tga" ), &m_wndChatEdit[1] );
	LoadTGA( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndChatEdit02.tga" ), &m_wndChatEdit[2] );

	if( m_wndChatEdit[2].lpData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_wndChatEdit[2].lpData = NULL" ) );
		return;
	}
/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( 154, rectRoot.bottom - rectWindow.Height() );
	Move( point );
*/
	CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndLock->SetCheck( m_bChatLock );
	CWndButton* pWndMoveLock = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	pWndMoveLock->SetCheck( m_bMoveLock );
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = 0;//rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );

	m_timerInputTimeOut.Set( SEC( 300 ) );

	//sun: 시스템 메세지 윈도우화... Neuz
	if( g_WndMng.m_pWndChatLog == NULL )
	{
		g_WndMng.m_pWndChatLog = new CWndChatLog;
		
		if( g_WndMng.m_pWndChatLog )
		{
			g_WndMng.m_pWndChatLog->Initialize( NULL, APP_CHAT_LOG );
			
			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			CRect rect = GetWindowRect(TRUE);
			CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);
			
			int x = 0;
			int y = rect.top;
			
			if( (rect.right+rect2.Width()) < rectRoot.right )
				x = rect.right;
			else
				x = (rect.left - rect2.Width() );
			
			CPoint point( x, y );
			g_WndMng.m_pWndChatLog->Move( point );

			g_WndMng.m_pWndChatLog->SetVisible( FALSE );
		}
	}

	if( g_WndMng.m_pWndChatLog )
	{
		CWndButton* pWndCheck3  = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
		if( pWndCheck3 )
			pWndCheck3->SetCheck(g_WndMng.m_pWndChatLog->m_bVisible);
	}

	//19차부로 보물상자 텍스트창은 제거되고 시스템 메세지로 분류됨.
	if(_GetContentState(CT_TREASURE_CHEST) == CS_VER1 && CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_WndMng.m_pWndChatNotice == NULL)
		{
			g_WndMng.m_pWndChatNotice = new CFLWndChatNotice;
			if(g_WndMng.m_pWndChatNotice)
			{
				g_WndMng.m_pWndChatNotice->Initialize( &g_WndMng, APP_SERVER_NOTICE );
			}
		}
	}	
}

BOOL CWndChat::Process ()
{
	if( m_timerInputTimeOut.IsTimeOut() )
	{
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );

		pWndText->m_string.SetAlpha( 0 );
	}
	
	if( 0 < strlen( g_Neuz.m_szWhisperName ) )
	{
		LPCTSTR lpstr = m_wndEdit.GetString();
		BOOL bCommand = FALSE;
		if( strcmp( lpstr, "/r " ) == 0 || strcmp( lpstr, "/R " ) == 0 || ( g_xFlyffConfig->GetMainLanguage() == LANG_KOR && strcmp( lpstr, "/ㄱ " ) == 0 ) )
		{
			CString strWhisper;
			strWhisper.Format( "/whisper %s ", g_Neuz.m_szWhisperName );
			m_wndEdit.SetString( strWhisper );
			m_wndEdit.SetFocus();
		}		
	}

	//Transparent
	//if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	//{
	//	CRect rect = this->GetClientRect();

	//	if( rect.PtInRect( GetMousePoint() ) )
	//	{
	//		//if( m_nAlphaCount < 255 )
	//		//	++m_nAlphaCount;
	//		//else 
	//		//	m_nAlphaCount = 255;
	//		m_nAlphaCount = 255;

	//		this->SwitchMode( MODE_NORMAL );
	//	}
	//	else 
	//	{
	//		//if( m_nAlphaCount > 0 )
	//		//	--m_nAlphaCount;
	//		//else 
	//		//	m_nAlphaCount = 0;
	//		m_nAlphaCount = 10;

	//		this->SwitchMode( MODE_TARNSPARENT );
	//	}
	//}
	
	return CWndBase::Process();
}

void CWndChat::SwitchMode( int nMode )
{
	if( m_nMode == nMode )
		return;

	m_nMode = nMode;

	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	CWndButton* pWndRadio6 = (CWndButton*)GetDlgItem( WIDC_RADIO6 );

	CWndButton* pWndCheck  = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndCheck2  = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_HELP   );
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_CLOSE  );

	if( MODE_TARNSPARENT == m_nMode )
	{
		pWndRadio1->SetVisible( FALSE );
		pWndRadio2->SetVisible( FALSE );
		pWndRadio3->SetVisible( FALSE );
		pWndRadio4->SetVisible( FALSE );
		pWndRadio5->SetVisible( FALSE );
		pWndRadio6->SetVisible( FALSE );

		pWndCheck->SetVisible( FALSE );
		pWndCheck2->SetVisible( FALSE ); 
		pWndButton1->SetVisible( FALSE );
		pWndButton2->SetVisible( FALSE );
		pWndButton3->SetVisible( FALSE );
	}
	else if( MODE_NORMAL == m_nMode )
	{
		pWndRadio1->SetVisible( TRUE );
		pWndRadio2->SetVisible( TRUE );
		pWndRadio3->SetVisible( TRUE );
		pWndRadio4->SetVisible( TRUE );
		pWndRadio5->SetVisible( TRUE );
		pWndRadio6->SetVisible( TRUE );

		pWndCheck->SetVisible( TRUE );
		pWndCheck2->SetVisible( TRUE ); 
		pWndButton1->SetVisible( TRUE );
		pWndButton2->SetVisible( TRUE );
		pWndButton3->SetVisible( TRUE );
	}
}

void CWndChat::SetChannel( BOOL bForced )
{
	m_wndText.SetString( "" );

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
		case CHATSTY_ALL	: nChannel = 5;	break;
	}

	//gmpbigsun: #16358, 필터 버튼을 누름과 동시에 해당채털의 필터를 강제로 활성화 시킨다.
	if( bForced )
		g_Option.m_dwChatFilter[ nChannel ] |= CWndChat::m_nChatChannel;

	DWORD dwChatFilter = g_Option.m_dwChatFilterEtc | g_Option.m_dwChatFilter[ nChannel ];

	for( int i = 0; i < g_WndMng.m_aChatString.GetSize(); i++ )
	{
		if( g_WndMng.m_aChatStyle.GetAt( i ) & dwChatFilter ) 
		{
			CString string = g_WndMng.m_aChatString.GetAt( i );
			m_wndText.m_string.AddParsingString( string, g_WndMng.m_aChatColor.GetAt( i ), 0x00000000, 0, PS_NOT_MACRO );
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

extern	CDPClient	g_DPlay;

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
	{
		SAFE_DELETE( m_pWndChatFilter );
		//ChatFilter Wnd Destroy후 Focus가 World로 되어있지 않아 이동키가 먹지 않기 때문에 SetFocus해줌.
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		if( pWndWorld ) 
			pWndWorld->SetFocus();
	}
	if(g_WndMng.m_pWndChatNotice == pWndChild)
	{
		SAFE_DELETE(g_WndMng.m_pWndChatNotice);
	}
}
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
		CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
		pWndLock->SetCheck( m_bChatLock );
	}
	else
	{
		dwVersion = 1;
		ar << m_bChatLock;
	}
}

BOOL CWndChat::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	m_timerInputTimeOut.Reset();

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );	
	if( pWndText )
		pWndText->m_string.SetAlpha( 255 );	
	
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
		case WIDC_CHECK1: // 채팅 입력창 잠구기 
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
				m_bChatLock = pWndCheck->GetCheck();
			}
			break;
		//sun: 시스템 메세지 윈도우화... Neuz
		case WIDC_CHECK3: // 시스탐창
			{
				m_bChatLog = !m_bChatLog;		//sun: 8차, 'T'키 사용, 상대방 레벨 안보이기, 화면 모든 창 On/Off Neuz

				if( g_WndMng.m_pWndChatLog )
				{
					if( m_bChatLog )
					{
						CRect rectRoot = m_pWndRoot->GetLayoutRect();
						CRect rect = GetWindowRect(TRUE);
						CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);
						
						int x = 0;
						int y = rect.top;
						
						if( (rect.right+rect2.Width()) < rectRoot.right )
							x = rect.right;
						else
							x = (rect.left - rect2.Width() );
						
						CPoint point( x, y );
						g_WndMng.m_pWndChatLog->Move( point );
						g_WndMng.m_pWndChatLog->SetVisible( FALSE );
					}
					
					g_WndMng.m_pWndChatLog->SetVisible( m_bChatLog );
				}
			}
			break;

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
		case WIDC_RADIO1:
			m_nChatChannel = CHATSTY_GENERAL;
			m_wndEdit.SetString( "" );
			m_wndEdit.SetFocus();
			SetChannel( TRUE );
			break;
		case WIDC_RADIO2:
			m_nChatChannel = CHATSTY_WHISPER;
			m_wndEdit.SetString( "/w " );
			m_wndEdit.SetFocus();
			SetChannel( TRUE );
			break;
		case WIDC_RADIO3:
			m_nChatChannel = CHATSTY_SHOUT;
			m_wndEdit.SetString( "/s " );
			m_wndEdit.SetFocus() ;
			SetChannel( TRUE );
			break;
		case WIDC_RADIO4:
			m_nChatChannel = CHATSTY_PARTY;
			m_wndEdit.SetString( "/p " );
			m_wndEdit.SetFocus();
			SetChannel( TRUE );
			break;
		case WIDC_RADIO5:
			m_nChatChannel = CHATSTY_GUILD;
			m_wndEdit.SetString( "/g " );
			m_wndEdit.SetFocus();
			SetChannel( TRUE );
			break;
		case WIDC_RADIO6:
			m_nChatChannel = CHATSTY_ALL;
			m_wndEdit.SetString( "" );
			m_wndEdit.SetFocus();
			SetChannel( TRUE );
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
			if( message == EN_UP )		
			{
				if( m_strHistory.size() == 0 )
					return FALSE;

				if( m_nHistoryIndex >= (int)( m_strHistory.size() ) )
				{
					m_strHistory.front();
					m_nHistoryIndex = 0;
				}

				m_nHistoryIndex++;
				int n = m_strHistory.size()-m_nHistoryIndex;
				m_wndEdit.m_string = m_strHistory[n];

				return TRUE;
			}		
			//flyingjin : 채팅창 히스토리 뒷부분 검색 추가 
			if( message == EN_DOWN )
			{
				if( m_strHistory.size() == 0 )
					return FALSE;

				if( m_nHistoryIndex >= (int)( m_strHistory.size() ) )
				{
					m_strHistory.back();
					m_nHistoryIndex = m_strHistory.size();
				}
				m_nHistoryIndex--;
				if(m_nHistoryIndex<0)
				{
					m_nHistoryIndex = m_strHistory.size();
					m_nHistoryIndex--;
				}
				int nCnt;
				if(m_nHistoryIndex == 0)
				{
					nCnt = 0;
				}
				else
 				{
					nCnt = m_strHistory.size()-m_nHistoryIndex;
				}
				int nlen = strlen(m_strHistory[nCnt]);
				CString str;
				if( nlen > 0 )
				{
					str = m_strHistory[nCnt];
					m_wndEdit.m_string = str;
				}
				
				return TRUE;
			}
			if( message == EN_RETURN )
			{
				BOOL bHistory = TRUE;
#ifdef __HYPERLINK_ITEM16
				//gmpbigsun_WKG : 채팅창에서 엔터입력을 받았다.
				//gmpbigsun(100719) : reset temp HLITEM_DATA
				if( 0 != m_kTempHLData_Send._masterID )
				{
					//링크아이템이 입력한 채팅 문자열 안에 있다.
					//링크아이템 관련 문자열( < ... > ) 를 찾아서 원본과 교체해줘야 한다.
					CEditString& strOrg = m_wndEdit.m_string;
					int start = strOrg.Find( "<", 0 );
					int end = 0;
					if( -1 != start )
					{
						end = strOrg.Find( ">", start );
					}
					
					CString token1, token2;
					AfxExtractSubString( token1, strOrg, 0, '<' );
					AfxExtractSubString( token2, strOrg, 1, '>' );

					m_wndEdit.m_string = token1 + m_kTempHLData_Send._szOrg + token2;

					bHistory = FALSE;
				}
				m_kTempHLData_Send.reset();
#endif //__HYPERLINK_ITEM16

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

					WORD wCodePage = g_xFlyffConfig->GetCodePage();
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
								
								if(wCodePage == g_xFlyffConfig->GetCodePage())
								{
									FLSPrintf(temp, _countof(temp), "#nl");
								}
								else
								{
									FLSPrintf(temp, _countof(temp), "#l%04d", wCodePage);
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
					if( m_strHistory.size() >= MAX_HISTORY_STRING )
					{
						m_nHistoryIndex = 0;
						m_strHistory.erase( m_strHistory.begin() );
					}

					if( bHistory )
						m_strHistory.push_back(string);

					int a = string.GetLength();
					if( !m_wndEdit.m_string.IsEmpty() )
						m_wndEdit.m_string.GetTextFormat(string);
					
					g_WndMng.ParsingChat( string );		//gmpbigsun: ParsingChat함수안에서 SendChat
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
					if( m_bChatLock == FALSE ) 
					{
						if( IsWndStyle( WBS_MODAL ) == TRUE )
						{
							DelWndStyle( WBS_MODAL );
							if( g_Neuz.pOldModalWindow )
							{
								g_Neuz.pOldModalWindow->SetFocus();
							}
						}
						else
						{
							CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
							if( pWndWorld )
							{
								pWndWorld->SetFocus();
							}
							else
							{
								g_WndMng.SetFocus();
							}
						}
					}
					else
						m_wndEdit.SetFocus();
				}
			}
			else if( message == EN_CHANGE )
			{
#ifdef __HYPERLINK_ITEM16
				//gmpbigsun(100729) : 링크아이템이 있는데 "<", ">" 가 없다면 데이터소멸(유저가 지웠다거나 한 경우)
				if( 0 != m_kTempHLData_Send._masterID )
				{
					int lb = m_wndEdit.m_string.Find( "<", 0 );
					int rb = m_wndEdit.m_string.Find( ">", 0 );
					if( -1 == lb || -1 == rb )
						m_kTempHLData_Send.reset();
				}
#endif //__HYPERLINK_ITEM16

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
						
						if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
						{
							if( m_wndEdit.m_string == "/ㅎ " )
							{
								m_wndEdit.SetString( "/g " );
							}
							else if( m_wndEdit.m_string == "/ㅔ " )
							{
								m_wndEdit.SetString( "/p " );
							}
							else if( m_wndEdit.m_string == "/ㄴ " )
							{
								m_wndEdit.SetString( "/s " );
							}
//sun: 11, <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제
							else if( m_wndEdit.m_string == "/ㅈ " )
							{
								m_wndEdit.SetString( "/w " );
							}
						}
					}
				}
				else if( m_pWndCommand )
				{
					m_pWndCommand->Destroy();
				}
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
	
	//아래 특정 버튼 누를 경우 Focus가 World로 되어있지 않아 이동키가 먹지 않기 때문에 SetFocus해줌.
	if(nID == WIDC_CHECK1 || nID == WIDC_CHECK3 || nID == WIDC_MOVELOCK)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		if( pWndWorld ) 
			pWndWorld->SetFocus();
	}
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



void CWndChat::PutString( LPCTSTR lpszString )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	DWORD dwPStyle( 0x00000001 ); 

#ifdef __HYPERLINK_ITEM16
	dwPStyle |= PS_REALADDING_CHATWND;
#endif //__HYPERLINK_ITEM16

	pWndText->AddString( lpszString, m_dwColor, dwPStyle );
	pWndText->AddString( "\n", m_dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
#ifdef __HYPERLINK_ITEM16
		UpdateLineForHLItems( 0, 500 );
#endif //__HYPERLINK_ITEM16

	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}


void CWndChat::PutString( LPCTSTR lpszString, DWORD dwColor )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	if( dwColor != 0xffffff00 )
	{
		m_timerInputTimeOut.Reset();
		
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );	
		pWndText->m_string.SetAlpha( 255 );	
	}

	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	DWORD dwPStyle( 0x00000001 ); 

#ifdef __HYPERLINK_ITEM16
	dwPStyle |= PS_REALADDING_CHATWND;
#endif //__HYPERLINK_ITEM16

	pWndText->AddString( lpszString, dwColor, dwPStyle );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
#ifdef __HYPERLINK_ITEM16
		UpdateLineForHLItems( 0, 500 );
#endif //__HYPERLINK_ITEM16

	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}


void CWndChat::PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	if( dwColor != 0xffffff00 )
	{
		m_timerInputTimeOut.Reset();
		
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );	
		pWndText->m_string.SetAlpha( 255 );	
	}

	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

#ifdef __HYPERLINK_ITEM16
	dwPStyle |= PS_REALADDING_CHATWND;
#endif //__HYPERLINK_ITEM16

	pWndText->AddString( lpszString, dwColor, dwPStyle );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
#ifdef __HYPERLINK_ITEM16
		UpdateLineForHLItems( 0, 500 );
#endif //__HYPERLINK_ITEM16

	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}
#ifdef __IMPROVE_MAP_SYSTEM
BOOL CWndChat::CalculateItemHyperlink( int nValidTextLine, const CPoint& pointMouse )
{
	int nLineStartPosition = 0;
	CString strLine = m_wndText.m_string.GetLine( nValidTextLine, nLineStartPosition );
	CString strLineText = strLine;
	char szSection[ MAX_PATH ] = { 0, };
	int nLeftPointX = 0;
	int nRightPointX = 0;
	int nPosition = 0;
	int nCharacterCounter = 0;
	CSize sizeSection( 0, 0 );

	while( nPosition < strLine.GetLength() - 1 )
	{
		nCharacterCounter = strLineText.Find( _T( "<" ), 0 );
		if( nCharacterCounter == -1 )
		{
			return FALSE;
		}

		nPosition += nCharacterCounter;
		int nLeftStylePosition = nPosition;

		CString strParsingText = strLineText;
		int nParsingStartCharacterCounter = nCharacterCounter;
		BOOL bParsingTerminated = FALSE;
		int nParsingEndCharacterCounter = -1;
		static const int nFindingPosition = 1;
		while( true )
		{
			strParsingText = strParsingText.Mid( nParsingStartCharacterCounter );
			nParsingEndCharacterCounter = strParsingText.Find( _T( ">" ), nFindingPosition );
			nParsingStartCharacterCounter = strParsingText.Find( _T( "<" ), nFindingPosition );
			if( nParsingStartCharacterCounter == -1 )
			{
				if( nParsingEndCharacterCounter == -1 )
				{
					bParsingTerminated = TRUE;
				}
				break;
			}
			else if( nParsingStartCharacterCounter > nParsingEndCharacterCounter == TRUE )
			{
				break;
			}

			nCharacterCounter += nParsingStartCharacterCounter;
			nPosition += nParsingStartCharacterCounter;
			nLeftStylePosition = nPosition;
		}

		if( bParsingTerminated == TRUE )
		{
			return FALSE;
		}

		ZeroMemory( szSection, sizeof( szSection ) );
		if( nCharacterCounter > 0 )
		{
			FLStrncpy( szSection, _countof( szSection ), strLineText, static_cast< size_t >( nCharacterCounter ) );
		}

		CString strChangedLeftSection = szSection;

		sizeSection = m_wndText.m_pFont->GetTextExtent( szSection );
		nLeftPointX = nRightPointX + sizeSection.cx;
		strLineText = strLineText.Mid( nCharacterCounter );

		nCharacterCounter = strLineText.Find( _T( ">" ), 0 );
		if( nCharacterCounter == -1 )
		{
			return FALSE;
		}

		++nCharacterCounter;
		nPosition += nCharacterCounter;
		int nRightStylePosition = nCharacterCounter;

		ZeroMemory( szSection, sizeof( szSection ) );
		if( nCharacterCounter > 0 )
		{
			FLStrncpy( szSection, _countof( szSection ), strLineText, static_cast< size_t >( nCharacterCounter ) );
		}

		sizeSection = m_wndText.m_pFont->GetTextExtent( szSection );
		nRightPointX = nLeftPointX + sizeSection.cx;
		strLineText = strLineText.Mid( nCharacterCounter );

		if( pointMouse.x > nLeftPointX && pointMouse.x < nRightPointX )
		{
			m_wndText.m_string.ClearStyle( 
				m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition, 
				m_nHyperlinkTextRightPosition, 
				m_dwHyperlinkTextStyle );
			m_nHyperlinkTextStartPosition = nLineStartPosition;
			m_nHyperlinkTextLeftPosition = nLeftStylePosition;
			m_nHyperlinkTextRightPosition = nRightStylePosition;
			m_wndText.m_string.SetStyle( 
				m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition, 
				m_nHyperlinkTextRightPosition, 
				m_dwHyperlinkTextStyle );
			int nCalculatedLeftPosition = m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition;
			int nCalculatedRightPosition = nCalculatedLeftPosition + m_nHyperlinkTextRightPosition;
			BOOL bItemHyperlink = TRUE;
			for( int nIndex = nCalculatedLeftPosition; nIndex < nCalculatedRightPosition; ++nIndex )
			{
				DWORD dwStyle = m_wndText.m_string.m_abyStyle.GetAt( nIndex );
				if( !( dwStyle & ESSTY_HYPERLINK_ITEM ) )
				{
					bItemHyperlink = FALSE;
					break;
				}
			}

			if( bItemHyperlink == FALSE )
			{
				return FALSE;
			}

			m_eHyperlinkMode = CWndChat::HL_Item;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWndChat::CalculateUserMarkHyperlink( int nValidTextLine, const CPoint& pointMouse )
{
	int nLineStartPosition = 0;
	CString strLine = m_wndText.m_string.GetLine( nValidTextLine, nLineStartPosition );
	CString strLineText = strLine;
	char szSection[ MAX_PATH ] = { 0, };
	int nLeftPointX = 0;
	int nRightPointX = 0;
	int nPosition = 0;
	int nCharacterCounter = 0;
	CSize sizeSection( 0, 0 );

	while( nPosition < strLine.GetLength() - 1 )
	{
		nCharacterCounter = strLineText.Find( _T( "[" ), 0 );
		if( nCharacterCounter == -1 )
		{
			return FALSE;
		}

		nPosition += nCharacterCounter;
		int nLeftStylePosition = nPosition;

		CString strParsingText = strLineText;
		int nParsingStartCharacterCounter = nCharacterCounter;
		BOOL bParsingTerminated = FALSE;
		int nParsing1stCharacterCounter = -1;
		int nParsing2ndCharacterCounter = -1;
		int nParsing3rdCharacterCounter = -1;
		CString strParsingTitle = _T( "" );
		FLOAT fParsingPositionX = -1.0F;
		FLOAT fParsingPositionY = -1.0F;
		static const int nFindingPosition = 1;
		while( true )
		{
			strParsingText = strParsingText.Mid( nParsingStartCharacterCounter );
			nParsing1stCharacterCounter = strParsingText.Find( _T( ":" ), nFindingPosition );
			nParsing2ndCharacterCounter = strParsingText.Find( _T( "," ), nFindingPosition );
			nParsing3rdCharacterCounter = strParsingText.Find( _T( "]" ), nFindingPosition );
			nParsingStartCharacterCounter = strParsingText.Find( _T( "[" ), nFindingPosition );
			if( nParsingStartCharacterCounter == -1 )
			{
				if( IsUserMarkHyperlinkParsingValid( 
					strParsingText, 
					nParsing1stCharacterCounter, 
					nParsing2ndCharacterCounter, 
					nParsing3rdCharacterCounter, 
					strParsingTitle, 
					fParsingPositionX, 
					fParsingPositionY ) == FALSE )
				{
					bParsingTerminated = TRUE;
				}
				break;
			}
			else if( 
				nParsingStartCharacterCounter > nParsing1stCharacterCounter && 
				nParsingStartCharacterCounter > nParsing2ndCharacterCounter && 
				nParsingStartCharacterCounter > nParsing3rdCharacterCounter && 
				IsUserMarkHyperlinkParsingValid( 
				strParsingText, 
				nParsing1stCharacterCounter, 
				nParsing2ndCharacterCounter, 
				nParsing3rdCharacterCounter, 
				strParsingTitle, 
				fParsingPositionX, 
				fParsingPositionY ) == TRUE )
			{
				break;
			}

			nCharacterCounter += nParsingStartCharacterCounter;
			nPosition += nParsingStartCharacterCounter;
			nLeftStylePosition = nPosition;
		}

		if( bParsingTerminated == TRUE )
		{
			return FALSE;
		}

		ZeroMemory( szSection, sizeof( szSection ) );
		if( nCharacterCounter > 0 )
		{
			FLStrncpy( szSection, _countof( szSection ), strLineText, static_cast< size_t >( nCharacterCounter ) );
		}

		CString strChangedLeftSection = szSection;

		sizeSection = m_wndText.m_pFont->GetTextExtent( szSection );
		nLeftPointX = nRightPointX + sizeSection.cx;
		strLineText = strLineText.Mid( nCharacterCounter );

		nCharacterCounter = strLineText.Find( _T( "]" ), 0 );
		if( nCharacterCounter == -1 )
		{
			return FALSE;
		}

		++nCharacterCounter;
		nPosition += nCharacterCounter;
		int nRightStylePosition = nCharacterCounter;

		ZeroMemory( szSection, sizeof( szSection ) );
		if( nCharacterCounter > 0 )
		{
			FLStrncpy( szSection, _countof( szSection ), strLineText, static_cast< size_t >( nCharacterCounter ) );
		}

		sizeSection = m_wndText.m_pFont->GetTextExtent( szSection );
		nRightPointX = nLeftPointX + sizeSection.cx;
		strLineText = strLineText.Mid( nCharacterCounter );

		if( pointMouse.x > nLeftPointX && pointMouse.x < nRightPointX )
		{
			m_wndText.m_string.ClearStyle( 
				m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition, 
				m_nHyperlinkTextRightPosition, 
				m_dwHyperlinkTextStyle );
			m_nHyperlinkTextStartPosition = nLineStartPosition;
			m_nHyperlinkTextLeftPosition = nLeftStylePosition;
			m_nHyperlinkTextRightPosition = nRightStylePosition;
			m_wndText.m_string.SetStyle( 
				m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition, 
				m_nHyperlinkTextRightPosition, 
				m_dwHyperlinkTextStyle );
			m_strHyperlinkMarkTitle = strParsingTitle;
			m_fHyperlinkMarkPositionX = fParsingPositionX;
			m_fHyperlinkMarkPositionY = fParsingPositionY;
			m_eHyperlinkMode = CWndChat::HL_UserMark;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWndChat::IsUserMarkHyperlinkParsingValid( 
	const CString& strParsing, 
	int nParsing1stCharacterCounter, 
	int nParsing2ndCharacterCounter, 
	int nParsing3rdCharacterCounter, 
	CString& strParsingTitle, 
	FLOAT& fParsingPositionX, 
	FLOAT& fParsingPositionY )
{
	int nAdded1stCharacterCounter = nParsing1stCharacterCounter + 1;
	int nAdded2ndCharacterCounter = nParsing2ndCharacterCounter + 1;

	// 어느 한 문자라도 없으면 안 되고, 각 문자들은 적어도 1칸씩은 띄워져 있어야 함
	if( ( nParsing1stCharacterCounter == -1 || nParsing2ndCharacterCounter == -1 || nParsing3rdCharacterCounter == -1 ) || 
		!( nAdded1stCharacterCounter < nParsing2ndCharacterCounter && nAdded1stCharacterCounter < nParsing3rdCharacterCounter && nAdded2ndCharacterCounter < nParsing3rdCharacterCounter ) )
	{
		return FALSE;
	}

	// '['와 ':' 사이에 한 글자라도 없다면(타이틀이 없는 것이므로) 무효함
	if( nParsing1stCharacterCounter == 1 )
	{
		return FALSE;
	}

	// '['와 ':' 사이에 글자가 있더라도 모두 공백(' ')으로 되어 있다면 무효함
	int nSubtracted2ndCharacterCounter = nParsing1stCharacterCounter - 1;
	CString strSpaceAllChecking = strParsing.Mid( 1, nSubtracted2ndCharacterCounter );
	const char* szSpaceAllChecking = strSpaceAllChecking;
	BOOL bSpaceAllChecking = FALSE;
	int nIndex = 0;
	while( nIndex < nSubtracted2ndCharacterCounter )
	{
		if( szSpaceAllChecking[ nIndex++ ] != ' ' )
		{
			bSpaceAllChecking = TRUE;
			break;
		}
	}
	if( bSpaceAllChecking == FALSE )
	{
		return FALSE;
	}
	strParsingTitle = strSpaceAllChecking;

	// ':'와 ',' 사이의 문자가 숫자로 되어 있지 않다면 무효함
	CString strFirstLocation = strParsing.Mid( nAdded1stCharacterCounter, nParsing2ndCharacterCounter - nAdded1stCharacterCounter );
	BOOL bIntegerFirstLocation = IsInteger( strFirstLocation );
	if( bIntegerFirstLocation == FALSE )
	{
		return FALSE;
	}
	fParsingPositionX = static_cast< FLOAT >( atoi( strFirstLocation ) );

	// ','와 ']' 사이의 문자가 숫자로 되어 있지 않다면 무효함
	CString strSecondLocation = strParsing.Mid( nAdded2ndCharacterCounter, nParsing3rdCharacterCounter - nAdded2ndCharacterCounter );
	BOOL bIntegerSecondLocation = IsInteger( strSecondLocation );
	int nIntegerSecondLocation = atoi( strSecondLocation );
	if( bIntegerSecondLocation == FALSE )
	{
		return FALSE;
	}
	fParsingPositionY = static_cast< FLOAT >( atoi( strSecondLocation ) );

	return TRUE;
}
#endif // __IMPROVE_MAP_SYSTEM
void CWndChat::OnDestroy()
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	g_WndMng.m_ChatString = pWndText->m_string;
}
void CWndChat::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	//sun: 시스템 메세지 윈도우화... Neuz
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		AdjustMinRect( &rectWnd, 16 * 29, 16 * 3 );
	else if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		AdjustMinRect( &rectWnd, 16 * 19, 16 * 3 );


	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	if(g_WndMng.m_pWndChatNotice)
	{
		CRect rect = g_WndMng.m_pWndChatNotice->GetClientRect(TRUE);
		int nHeight = rect.Height()/2;
		int x_ = m_rectClient.left - 4;
		int y_ = m_rectClient.top - (nHeight + 7);

		CPoint point_( x_,y_);
		g_WndMng.m_pWndChatNotice->Move( point_ );	//_JIN_TREASURE_CHEST
	}

	MakeVertexBuffer();
}
void CWndChat::OnSize(UINT nType, int cx, int cy)
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	CWndButton* pWndRadio6 = (CWndButton*)GetDlgItem( WIDC_RADIO6 );

	CWndButton* pWndCheck  = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndCheck2  = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_HELP   );
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_CLOSE  );

	//sun: 시스템 메세지 윈도우화... Neuz
	CWndButton* pWndCheck3  = (CWndButton*)GetDlgItem( WIDC_CHECK3 );

	CRect rectEdit;
	CRect rect = GetClientRect();

	//!!!!
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
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
		rectEdit.right = rect.right - 20;
		rect.DeflateRect( 1, 1 );
		m_wndEdit.SetWndRect( rectEdit );

		rect = GetClientRect();
		rectEdit = m_wndMacroChat.GetWndRect();
		rectEdit.top = rect.bottom - 19;
		rectEdit.bottom = rect.bottom;
		m_wndMacroChat.SetWndRect( rectEdit );

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
		//sun: 시스템 메세지 윈도우화... Neuz
		rectEdit.left = rect.right - 200;
		pWndCheck->Move( rectEdit.TopLeft()  ); 
		rectEdit.left = rect.right - 150;
		pWndButton1->Move( rectEdit.TopLeft()  ); 

		rectEdit.left = rect.right - 100;
		pWndCheck3->Move( rectEdit.TopLeft()  ); 	

		rectEdit.left = rect.right - 49;

		CRect rectEdit2 = rectEdit;

		rectEdit2.top  += 1;
		pWndCheck2->Move( rectEdit2.TopLeft()  ); 
		rectEdit.left = rect.right - 32;
		rectEdit.top += 1;
		pWndButton2->Move( rectEdit.TopLeft()  ); 
		rectEdit.left = rect.right - 15;
		pWndButton3->Move( rectEdit.TopLeft()  ); 

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 

		if( pWndStatic )
		{
			rectEdit.top  += 20;
			rectEdit.left -= 5;
			pWndStatic->Move( rectEdit.TopLeft()  ); 
		}
	}
	else if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		rect.top += 10;
		rect.left += 6;
		rect.right -= 0;
		rect.bottom -= 42;
		pWndText->SetWndRect( rect );
		pWndText->m_wndScrollBar.SetMaxScrollPos();
		
		rect = GetClientRect();
		rectEdit = pWndHead->GetWndRect();
		rectEdit.top = rect.bottom - 22;
		rectEdit.bottom = rect.bottom;
		rect.DeflateRect( 1, 1 );
		pWndHead->SetWndRect( rectEdit );

		rect = GetClientRect();
		rectEdit = m_wndEdit.GetWndRect();
		rectEdit.top = rect.bottom - 22;
		rectEdit.bottom = rect.bottom;
		rectEdit.right = rect.right - 20;
		rect.DeflateRect( 1, 1 );
		m_wndEdit.SetWndRect( rectEdit );

		rect = GetClientRect();

		rectEdit = m_wndMacroChat.GetWndRect();
		rectEdit.top = rect.bottom - 20;
		rectEdit.bottom = rect.bottom;
		rectEdit.left = rect.left + 7;
		rectEdit.right = rectEdit.right + 7;
		m_wndMacroChat.SetWndRect( rectEdit );

		const int nGap = 30;
		rect = GetClientRect();
		rectEdit = pWndRadio1->GetWndRect();
		rectEdit.top = rect.bottom - 44;
		rectEdit.left = 5;
		pWndRadio6->Move( rectEdit.TopLeft() );
		rectEdit.left += nGap;
		pWndRadio1->Move( rectEdit.TopLeft() ); 
		rectEdit.left += nGap;
		pWndRadio2->Move( rectEdit.TopLeft()  ); 
		rectEdit.left += nGap;
		pWndRadio3->Move( rectEdit.TopLeft()  ); 
		rectEdit.left += nGap;
		pWndRadio4->Move( rectEdit.TopLeft()  ); 
		rectEdit.left += nGap;
		pWndRadio5->Move( rectEdit.TopLeft()  ); 
	
		rectEdit.left = rect.right - 46;
		pWndButton1->Move( rectEdit.TopLeft()  ); 

		rectEdit.left = rect.right - 68;
		pWndCheck3->Move( rectEdit.TopLeft()  ); 	

		//close
		rectEdit.left = rect.right - 24;
		pWndButton3->Move( rectEdit.TopLeft()  ); 

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 

		if( pWndStatic )
		{
			rectEdit.top  += 20;
			rectEdit.left -= 5;
			pWndStatic->Move( rectEdit.TopLeft()  ); 
		}

		rectEdit.top = 1000;

		pWndCheck->Move( rectEdit.TopLeft()  ); 
		pWndCheck2->Move( rectEdit.TopLeft()  ); 
		pWndButton2->Move( rectEdit.TopLeft()  ); 
		
	}

	//flyingjin : 채팅창의 명령어 박스가 따로 그려지는 현상을 방지하기 위해,,, 
	if(m_pWndCommand)
	{
		m_pWndCommand->SetVisible(FALSE);
	}
	
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
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonUp( nFlags, p );
	}
}

void CWndChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pWndCommand )
		m_pWndCommand->Destroy();

#ifdef __HYPERLINK_ITEM16
	if( 0 != m_nSelectedLineForHLITEM )
	{
		//링크 아이템 문자열 범위안에서 클릭댐
		//서버로 masterID, itemOID를 찾아서 요청한다.
		if( m_nSelectedLineForHLITEM < 1000 )		//1000라인 이상은 지원하지 않는다.
		{
			DWORD lKey = m_nSelectedLineForHLITEM;
			DWORD rKey = 0;
			DWORD key = 0;
			key = ( lKey << 16 ) | rKey;

			HyperLinkItemIter iter = m_cHyperLinkItems.find( key );
			if( iter != m_cHyperLinkItems.end() )
			{
				HLITEM_DATA& kData = iter->second;

				//한번도 요청하지 않아서 아이템이 생성되지 않은 경우만 요청한다.
				if( NULL == kData.pItem )
				{
					//서버로 key, masterID, ItemOID 로 아이템 데이터 요청
					T_PACKET_ITEM_LINK kPacketData = { 0 };
					//mem_set( &kPacketData, 0, sizeof( kPacketData ) );
					kPacketData.dwStringId = iter->first;
					kPacketData.dwPlayerId = kData._masterID;
					kPacketData.dwObjId = kData._itemID;
					g_DPlay.SendReqItemLinkInfo( &kPacketData );
				}
				else
				{
					_ShowTooltip_LinkedItem( kData.pItem );
				}

			}
		}

		m_nSelectedLineForHLITEM = 0;
	}
#endif //__HYPERLINK_ITEM16

#ifdef __IMPROVE_MAP_SYSTEM
	if( m_bHyperlinked == TRUE )
	{
		switch( m_eHyperlinkMode )
		{
		case HL_UserMark:
			{
				CWndMapEx* pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
				if( pWndMapEx == NULL )
				{
					return;
				}
				else
				{
					SAFE_DELETE( pWndMapEx );
					pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
				}

				pWndMapEx->SetHyperlinkMarkPosition( 
					m_strHyperlinkMarkTitle, 
					m_fHyperlinkMarkPositionX, 
					m_fHyperlinkMarkPositionY );

				break;
			}
		}

		return;
	}
#endif // __IMPROVE_MAP_SYSTEM

	if( CS_VER1 == _GetContentState( CT_NEWUI_19) )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
		}
	}
}

void CWndChat::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
	}

	int nLineCount = m_wndText.m_string.GetLineCount();
	int nPos = m_wndText.IsWndStyle( WBS_VSCROLL ) ? m_wndText.m_wndScrollBar.GetScrollPos() : 0;
	DWORD dwMaxHeight = m_wndText.GetFontHeight();
	for( int i = nPos ; i < nLineCount ; ++i )
	{
		if( (int)( ((i-nPos) * dwMaxHeight) ) <= point.y && point.y < (int)( (dwMaxHeight * (i-nPos + 1)) ) )
		{
			CString str = m_wndText.m_string.GetLine( i );
			char szFind[MAX_NAME] = {0,};
			
			BOOL bChat = FALSE;
			
			// 일반 채팅 검색
			int nFindName = str.Find( " :" );
			if( 0 < nFindName && nFindName < MAX_NAME )
			{
				int nFindSpace = str.Find( ' ' );
				if(  nFindSpace == -1 || nFindName <= nFindSpace )
				{
					FLStrncpy( szFind, _countof( szFind ), str, nFindName );
					if( strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;
				}
			}
			
			// 귓속말 검색
			if( bChat == FALSE )
			{
				int nFindName1 = str.Find( "<- [" );
				int nFindName2 = str.Find( "]" );
				if( 0 == nFindName1 )
				{
					str.Replace( "<- [", "" );
				}					
				else
				{
					nFindName1 = str.Find( "-> [" );
					str.Replace( "-> [", "" );
				}
				
				if( 0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 4 )
				{
					nFindName2 = str.Find( "]" );
					FLStrncpy( szFind, _countof( szFind ), str, nFindName2 );
					int nFindSpace = strspn( " ", szFind );
					if(  nFindSpace < 1 && strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;
				}
			}
			
			// 외치기 검색( 알림은 빠짐 )
			if( bChat == FALSE )
			{
				int nFindName1 = str.Find( "[" );
				int nFindName2 = str.Find( "]" );
				if( 0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 1 )
				{
					str.Replace( "[", "" );
					nFindName2 = str.Find( "]" );
					FLStrncpy( szFind, _countof( szFind ), str, nFindName2 );
					int nFindSpace = strspn( " ", szFind );
					if(  nFindSpace < 1 && strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;
					
					CString strSystem = prj.GetText(TID_ADMIN_ANNOUNCE);
					strSystem.Replace( "[", "" );
					int nFindSystem = strSystem.Find("]");
					if( 0 < nFindSystem )
					{
						char szFindSystem[MAX_NAME] = {0,};
						FLStrncpy( szFindSystem, _countof( szFindSystem ), strSystem, nFindSystem );
						if( strcmp( szFind, szFindSystem ) == 0 )
							bChat = FALSE;
					}
				}
			}
			
			if( bChat )
			{
				CString strWhisper;
				strWhisper.Format( "/whisper %s ", szFind );
				m_wndEdit.SetString( strWhisper );
				m_wndEdit.SetFocus();
			}
			break;
		}
	}
}	
void CWndChat::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonUp( nFlags, p );
	}
}

void CWndChat::OnMouseMove(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnMouseMove( nFlags, p );
	}
}

#ifdef __IMPROVE_MAP_SYSTEM
void CWndChat::OnMouseWndSurface( CPoint point )
{
	int nLineCount = m_wndText.m_string.GetLineCount();
	int nPos = m_wndText.IsWndStyle( WBS_VSCROLL ) ? m_wndText.m_wndScrollBar.GetScrollPos() : 0;
	DWORD dwMaxHeight = m_wndText.GetFontHeight();
	BOOL bHyperlinked = FALSE;
	for( int i = nPos; i < nLineCount; ++i )
	{
		if( point.y >= static_cast< int >( ( i - nPos ) * dwMaxHeight ) && point.y < static_cast< int >( ( i - nPos + 1 ) * dwMaxHeight ) )
		{
			bHyperlinked = CalculateItemHyperlink( i, point );
			if( bHyperlinked == FALSE )
			{
				bHyperlinked = CalculateUserMarkHyperlink( i, point );
				m_nSelectedLineForHLITEM = 0;
			}
			else 
				m_nSelectedLineForHLITEM = i;
		}
	}

	if( bHyperlinked == TRUE )
	{
		m_bHyperlinked = TRUE;
		SetMouseCursor( CUR_SELECT );
	}
	else
	{
		if( m_nHyperlinkTextLeftPosition != -1 && m_nHyperlinkTextRightPosition != -1 )
		{
			m_wndText.m_string.ClearStyle( 
				m_nHyperlinkTextStartPosition + m_nHyperlinkTextLeftPosition, 
				m_nHyperlinkTextRightPosition, 
				m_dwHyperlinkTextStyle );
			m_nHyperlinkTextStartPosition = -1;
			m_nHyperlinkTextLeftPosition = -1;
			m_nHyperlinkTextRightPosition = -1;
		}
		m_bHyperlinked = FALSE;
		m_eHyperlinkMode = CWndChat::HL_None;
		SetMouseCursor( CUR_BASE );
	}
}

BOOL CWndChat::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	if( IsWndStyle( WBS_THICKFRAME ) && ( IsFullMax() == FALSE || m_nWinSize != WSIZE_MAX ) )
	{
		if( m_bLButtonDown == FALSE )
		{
			CPoint point = GetMousePoint();
			ClientToScreen( &point );
			CRect rectWindow = GetWindowRect( TRUE );
			point -= rectWindow.TopLeft();
			int nResizeDir = GetResizeDir( point );
			if( nResizeDir )
			{
				switch( nResizeDir )
				{
				case 1:
				case 2:
					{
						SetMouseCursor( CUR_RESIZE_VERT );
						break;
					}
				case 3:
				case 4:
					{
						SetMouseCursor( CUR_RESIZE_HORI );
						break;
					}
				case 5:
				case 8:
					{
						SetMouseCursor( CUR_RESIZE_HV1 );
						break;
					}
				case 6:
				case 7:
					{
						SetMouseCursor( CUR_RESIZE_HV2 );
						break;
					}
				}
			}
		}
	}
	else
	{
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}
#endif // __IMPROVE_MAP_SYSTEM

BOOL CWndChat::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
//void CWndNeuz::PaintTexture( LPBYTE pDest, LPBYTE pSrc, CPoint pt,
// CSize sizeTexture, CSize sizeSurface )

void CWndChat::AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, size, d3dFormat );

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

#ifdef __HYPERLINK_ITEM16
int CWndChat::GetLineCount( )
{
	//현재 문자열로 채워진 마지막 라인을 리턴
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	if( !pWndText )
		FLERROR_LOG( PROGRAM_NAME, _T( "pWndText is NULL" ) );
		
	return pWndText->m_string.GetLineCount();
}

BOOL CWndChat::UpdateLineForHLItems( const int nBegin, const int nCount )
{
	//채팅창의 라인이 삭제됐다. 하이퍼링크 아이템 데이터도 재구축 되어야 한다.

	for( HyperLinkItemIter iter = m_cHyperLinkItems.begin(); iter != m_cHyperLinkItems.end(); /*none*/ )
	{
		int key = (int)iter->first;
		key = key >> 16;
		if( key >= nBegin && key <= ( nBegin + nCount ) )
		{
			//지워진 라인이다, 데이터 삭제
			m_cHyperLinkItems.erase( iter++ );
		}
		else if( key > ( nBegin + nCount ) )
		{
			//라인변경, 키가 바껴야 한다.

			DWORD newKey = ( ( nCount - nBegin ) << 16 ) | 0 ;
			HLITEM_DATA kData = iter->second;

			m_cHyperLinkItems.erase( iter++ );

			m_cHyperLinkItems.insert( HyperLinkItemContainer::value_type( newKey, kData ) );
		}
		else 
			++iter;
				
	}

	return TRUE;
}
#endif //__HYPERLINK_ITEM16

//sun: 시스템 메세지 윈도우화... Neuz
CWndChatLog::CWndChatLog()
{
}
CWndChatLog::~CWndChatLog()
{
}
/*
void CWndChatLog::OnDraw(C2DRender* p2DRender)
{
}
*/
void CWndChatLog::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndText = GetWndCtrl( WIDC_CUSTOM1 );
	m_wndText.Create( WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT );
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );

	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		pWndText->m_bEnableClipboard = TRUE;
	}

	pWndText->m_nLineSpace = 2;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = 0;//rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );
	
	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_GUIDE_HIDE) );
}

BOOL CWndChatLog::Process ()
{	
	return TRUE;
}
void CWndChatLog::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChatLog::OnKillFocus(CWndBase* pNewWnd)
{
}
BOOL CWndChatLog::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height() );
	SetTitle( GETTEXT( TID_APP_COMMUNICATION_CHAT ) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_THICKFRAME, CPoint( 792, 130 ), pWndParent );
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}

void CWndChatLog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnDestroyChildWnd( CWndBase* pWndChild )
{
}
BOOL CWndChatLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndBase* pWndBase = (CWndBase*) pLResult;
	
	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		SetFocus();
		switch( nID )
		{
		case 0:
			{
				CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );

				if( pWndChat )
				{
					CWndButton* pWndCheck = (CWndButton*)pWndChat->GetDlgItem( WIDC_CHECK3 );

					if( pWndCheck )
						pWndCheck->SetCheck(FALSE);
				}

				SetVisible(FALSE);
			}
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndChatLog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndChatLog::PutString( LPCTSTR lpszString )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	pWndText->AddString( lpszString, m_dwColor, 0x00000001 );
	pWndText->AddString( "\n", m_dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}

void CWndChatLog::PutString( LPCTSTR lpszString, DWORD dwColor )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	pWndText->AddString( lpszString, dwColor, 0x00000001 );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}


void CWndChatLog::PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	pWndText->AddString( lpszString, dwColor, dwPStyle );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}
void CWndChatLog::OnDestroy()
{
}
void CWndChatLog::OnSize(UINT nType, int cx, int cy)
{
	CRect rectEdit;
	CRect rect = GetClientRect();
	//rect.left = 60;
	rect.top = 10;
	rect.left = 6;
	rect.right -= 0;
	rect.bottom -= 0;

	m_wndText.SetWndRect( rect );
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
	
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndChatLog::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
	}
}

void CWndChatLog::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetWindowRect( TRUE );
	CRect menurect = m_wndMenuPlace.GetWindowRect();

	rect.top  += point.y;
	rect.left += point.x;
	
	m_wndMenuPlace.Move( CPoint( rect.left, rect.top ) );
	
	m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
	m_wndMenuPlace.SetFocus();	
}	
void CWndChatLog::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnMouseMove(UINT nFlags, CPoint point)
{
}
/*
BOOL CWndChatLog::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
*/

void CWndChatLog::AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, size, d3dFormat );
}

void CWndChatLog::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 17, 16 * 3 );
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}

CFLWndChatNotice::CFLWndChatNotice()
{

}
CFLWndChatNotice::~CFLWndChatNotice()
{

}
BOOL CFLWndChatNotice::IsPickupSpace(CPoint point)
{
	return FALSE;
}
BOOL CFLWndChatNotice::Initialize( CWndBase* pWndParent, DWORD nType)
{
 	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SERVER_NOTICE, 0, CPoint( 0, 0), pWndParent );
}

void CFLWndChatNotice::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_vecTempNotice.clear();
	m_vecNotice.clear();

	m_pNotice = (CWndStatic*)GetDlgItem( WIDC_ST_NOTICE );
	if( m_pNotice )
	{
		m_pNotice->AddWndStyle( WSS_TOLEFT_AXISX );
		m_pNotice->SetTileColor(0xffffffff);
		m_pNotice->SetTextSpeed(2);
	}
	m_nIndex = 0;
}

void CFLWndChatNotice::SetNoticeText(CString strNotice) 
{
	m_vecTempNotice.push_back(strNotice);
}

BOOL CFLWndChatNotice::Process()
{
	//========================================================================================================
	//gmpbigsun(20120117 ) : #21485  
	//v19이후로 없어질 class 
	//CWndChat의 visible상태와 같게 행동한다.

	CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );	
	if( pWndChat )
		SetVisible( pWndChat->IsVisible() );
	//========================================================================================================

	CString str;
	if(m_pNotice->GetOffsetMax() == m_pNotice->GetOffset())
	{
		//내용이 다 지나갔을때
		if(m_vecNotice.size() > 0)
		{
			m_vecNotice.erase(m_vecNotice.begin());
			m_vecTempNotice.erase(m_vecTempNotice.begin());
			m_pNotice->SetOffset(400);
		}
	}
	else if(m_vecNotice.size() == 0)
	{
		//가지고 있던 알림을 다 지웠거나 받은 알림이 하나도 없을때
		str.Format("");
		if(m_vecTempNotice.size() > 0)
		{
			m_vecNotice = m_vecTempNotice;
			m_pNotice->SetOffset(400);
		}	
	}
	else
	{
		//현재 보여주고있는 알림 
		if(m_vecNotice.size() > 0)
		{
			if(m_vecNotice.size() > 0)
			{
				str.Format("%s",m_vecNotice[0]);
			}
		}				
	}	

	m_pNotice->SetTitle(str);	

	return TRUE;
}

void CFLWndChatNotice::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 17, 16 * 3 );
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}








