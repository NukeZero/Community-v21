#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildBank.h"

#include "WndManager.h"

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
  WndId : APP_GUILD_BANK - 길드창고
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_TABCTRL1 - 
  CtrlId : WIDC_GUILDBANK_NUM - 0
  CtrlId : WIDC_BUTTON1 - 길드지원
  CtrlId : WIDC_BUTTON2 - 망토제작
****************************************************/

CWndGuildBank::CWndGuildBank() 
{ 
	m_pwndGuildMerit = NULL;
} 
CWndGuildBank::~CWndGuildBank() 
{ 
	SAFE_DELETE( m_pwndGuildMerit );
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
} 
void CWndGuildBank::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_pPlayer && g_pPlayer->GetGuild() )
	{
		CString			string;
		string.Format( _T( "%u" ),g_pPlayer->GetGuild()->m_nGoldGuild );
		CWndStatic*	pCost = (CWndStatic*) GetDlgItem( WIDC_GUILDBANK_NUM );

		pCost->SetTitle( string );	
	}
} 
void CWndGuildBank::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

#ifdef __BUGFIX_CLONE0607
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.GetWndBase(APP_VENDOREX)  )
	{
		Destroy();
		return;
	}
#endif	// __BUGFIX_CLONE0607

	
#ifdef	__YSTATIC_MONEY1028
	CWndStatic* pCost = (CWndStatic*) GetDlgItem( WIDC_GUILDBANK_NUM );
	pCost->AddWndStyle(WSS_MONEY);
#endif //__YSTATIC_MONEY1028

	CWndTabCtrl* pTabCtrl = reinterpret_cast<CWndTabCtrl*>(GetDlgItem(WIDC_TABCTRL1));
	
	m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl.InitItem( &g_pPlayer->GetGuild()->m_GuildBank, APP_GUILD_BANK );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_ITEM);//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem( 0, &tabTabItem );
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_CUSTOM1 );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );

	CGuild *pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		if(	pGuild->IsMaster( g_pPlayer->m_idPlayer ) == FALSE )	// 길마가 아니면 
		{
			CWndButton* pWndButt = (CWndButton*)GetDlgItem(WIDC_BUTTON2);	// 망토제작 버튼을 disable 시킨다.
			pWndButt->EnableWindow( FALSE );
		}
	}


#ifdef __GLDISABLE
	CWndButton* pWndButt = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndButt->EnableWindow( FALSE );
#endif	

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_BANK, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildBank::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildBank::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildBank::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildBank::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildBank::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	LPSHORTCUT	lpShortcut	= reinterpret_cast<LPSHORTCUT>(pLResult);
	if( lpShortcut == NULL )
		return FALSE;
	if( lpShortcut->m_pFromWnd == NULL )
		return FALSE;
	CWndBase*	pWndFrame	= lpShortcut->m_pFromWnd->GetFrameWnd();

	if( pWndFrame->GetWndId() == APP_INVENTORY )
	{
		switch (message)
		{
		case	WIN_ITEMDROP:
			{
				if( nID == 11 ) // item
				{
					BOOL bForbid = TRUE;
					SAFE_DELETE( g_WndMng.m_pWndTradeGold );
					if( lpShortcut->m_dwData != 0 )
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						
						UINT SelectCount = pWndItemCtrl->GetSelectedCount();
						if( SelectCount != 1)
						{
							//g_WndMng.PutString( "장착 되어 있는것은 넣을수 없습니다", NULL, 0xffffff00 );
							g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor( TID_GAME_EQUIPPUT ) );
							
						}
						else
						{
							for( int i = 0; i < SelectCount; i++ )
							{
								int nItem = pWndItemCtrl->GetSelectedItem( i );
								pWndItemCtrl->GetItem( nItem );
							}
							CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
							
							if( itemElem->IsCharged() )
							{
								g_WndMng.PutString( prj.GetText( TID_GAME_NOTTRADE ), NULL, prj.GetTextColor( TID_GAME_NOTTRADE ) );
								return FALSE;
							}

							if( itemElem->m_nItemNum > 1 )
							{ 
								g_WndMng.m_pWndTradeGold = new CWndTradeGold;
								memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
								g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
								g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GUILD_BANK;
								g_WndMng.m_pWndTradeGold->m_pWndBase = this;
								
								g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
								g_WndMng.m_pWndTradeGold->MoveParentCenter();
								CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
								CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
								CString strMain = prj.GetText(TID_GAME_MOVECOUNT);//"몇개를 이동하시겠습니까?";
								CString strCount = prj.GetText(TID_GAME_NUMCOUNT);// " 갯수 : ";
								pStatic->m_strTitle = strMain;
								pStaticCount->m_strTitle = strCount;
							}
							else
							{
#ifdef __ONLINE
								g_DPlay.SendPutItemGuildBank( lpShortcut->m_dwId, 1, 1 );
#endif
							}
							
						}
					}
				}
			}
			break;

		case	WIN_DBLCLK:
			{
			}
			break;
			
		case	WNM_CLICKED:
			{
				switch( nID )
				{
				case WTBID_CLOSE: // 종료
					{
#ifdef __ONLINE
						g_DPlay.SendCloseGuildBankWnd();
#endif
					}
					break;
				}
			}
			break;
		}
	} else
	{
		switch( nID )
		{
		case WTBID_CLOSE:
			{
#ifdef __ONLINE
				g_DPlay.SendCloseGuildBankWnd();
#endif
			}
			break;

		case WIDC_BUTTON1:		// 길드 지원(공헌)
			SAFE_DELETE(m_pwndGuildMerit);
			// TID_GAME_GUILDNOTHINGNAME 
			{
				CGuild *pGuild = g_pPlayer->GetGuild();
				if( pGuild )
				{
					if( pGuild->m_szGuild[0] == '\0' )
					{
						// "길드명칭이 없어 길드에 지원을 할 수 없습니다. 길드마스터는 길드명칭을 정해주세요."
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDNOTHINGNAME ), MB_OK, this );
					}
					else
					{
						m_pwndGuildMerit = new CWndGuildMerit;
						m_pwndGuildMerit->Initialize( this );
					}
				}
			}
			break;
		case WIDC_BUTTON2:		// 망토제작.
#ifdef __ONLINE
			{
				CGuild *pGuild = g_pPlayer->GetGuild();
				if( pGuild )
				{
					if( pGuild->m_nLevel < 4 )		// 4레벨 이하는 망토못만듬.
					{
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDNOTLEVEL ), MB_OK, this );
						break;
					}
					g_DPlay.SendCreateGuildCloak();
				}
			}
#endif
			break;
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


