#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabMember.h"
#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineQuest.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************
  WndId : APP_GUILD_TABMEMBER 
****************************************************/
#define		MAX_MEMBER_LIST_DRAW	14		// 한화면에 보이는 리스트 수.
#define		MEMBER_LIST_HEIGHT		18		// 리스트의 한라인 폭 y += MEMBER_LIST_HEIGHT 로 쓰라.

CWndGuildTabMember::CWndGuildTabMember() 
{ 
//	m_pListCtrl = NULL;
	m_nMax = 0;
	m_nCurrentList = 0;
	m_nSelect = 0;
	memset( m_aList, 0, sizeof(m_aList) );
	m_nMxOld = m_nMyOld = 0;
} 
CWndGuildTabMember::~CWndGuildTabMember() 
{ 
//	SAFE_DELETE( m_pListCtrl );
} 
void CWndGuildTabMember::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __S0809_GM_SCROLL
	int nPage, nRange;
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		nPage = MAX_MEMBER_LIST_DRAW;
		nRange = m_nMax;//GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	if( m_nMax - m_wndScrollBar.GetScrollPos() + 1 > m_wndScrollBar.GetScrollPage() )
		m_nCurrentList = m_wndScrollBar.GetScrollPos(); 
	if( m_nMax < m_wndScrollBar.GetScrollPos() )
		m_nCurrentList = 0;
#endif // __S0809_GM_SCROLL
//	m_pListCtrl->OnDraw( p2DRender );
	int		i, sx, sy;
	int		nData;
	MEMBERLIST *pMember;
	DWORD	dwColor;
	
	sx = 8;
	sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ ((m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW: m_nMax) * 6 * 4 ];
	TEXTUREVERTEX2* pVertices = pVertex;

	for( i = m_nCurrentList; i < m_nCurrentList + MAX_MEMBER_LIST_DRAW; i ++ )
	{

		if( i >= m_nMax )	break;
		pMember = &(m_aList[i]);
		nData = pMember->nMemberLv;
		if( i == m_nSelect )
			dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );
		else
			dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );

		int j;
		int nClass = pMember->nClass;
		++nClass;
		switch( pMember->nMemberLv )
		{
		case 0:
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5, sy - 4 ), 43 + 0 + ( 6 * 1 ), &pVertices, 0xffffffff );
			break;
		case 1:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 1 + ( 6 * 1 ), &pVertices, 0xffffffff );
				}
			}
			break;
		case 2:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 2 + ( 6 * 1 ), &pVertices, 0xffffffff );
				}
			}
			break;
		case 3:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 3 + ( 6 * 1 ), &pVertices, 0xffffffff );
				}
			}
			break;
		case 4:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 4 + ( 6 * 1 ), &pVertices, 0xffffffff );		
				}
			}
			break;
		default:
			break;
		}

		DWORD Color;

		if( pMember->bIsOnLine )
			Color = 0xffffffff;
		else
			Color = 0xffff6464;
		
		if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), ( 70 + pMember->nJob - 6 ) + ( 8 * pMember->nSex ), &pVertices, Color );
		else
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), 12 + pMember->nJob + ( 6 * pMember->nSex ), &pVertices, Color );

		p2DRender->TextOut( sx + 95, sy, pMember->nLevel, dwColor );
		p2DRender->TextOut( sx + 135, sy, pMember->szName, dwColor );
		p2DRender->TextOut( sx + 220, sy, pMember->szAlias , dwColor );

		sy += MEMBER_LIST_HEIGHT;
	}

	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndGuildTabMember::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_wndMenu.CreateMenu( this );	
	

/*
	CWndListCtrl *pWndListCtrl = new CWndListCtrl;
	m_pListCtrl = pWndListCtrl;

	pWndListCtrl->SetWndStyle( WLVS_REPORT );
	pWndListCtrl->m_nFontColor = 0xff000000;
	pWndListCtrl->m_nSelectColor = 0xff0000ff;
	
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "호칭";
	lvc.cx = 48;
	lvc.fmt = LVCFMT_LEFT;
	pWndListCtrl->InsertColumn( 0, &lvc );

	lvc.iSubItem = 1;
	lvc.pszText = "직업";
	lvc.cx = 48;
	lvc.fmt = LVCFMT_LEFT;
	pWndListCtrl->InsertColumn( 1, &lvc );

	pWndListCtrl->SetWndRect( CRect(0, 200, 0, 200), FALSE );

	LV_ITEM lvi;
	CString		strItem;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	
	lvi.iSubItem = 0;
	lvi.pszText = "호칭1";
	pWndListCtrl->InsertItem(&lvi);		// 처음에만 인서트하고
	
	lvi.iSubItem = 1;
	lvi.pszText = "직업1";
	pWndListCtrl->SetItem(&lvi);		// 처음에만 인서트하고

	//----------------------
	lvi.iItem = 1;
	
	lvi.iSubItem = 0;
	lvi.pszText = "호칭2";
	pWndListCtrl->InsertItem(&lvi);		// 처음에만 인서트하고
	
	lvi.iSubItem = 1;
	lvi.pszText = "직업2";
	pWndListCtrl->SetItem(&lvi);		// 처음에만 인서트하고
*/	

//	CString strMember;
#ifdef __S0809_GM_SCROLL
	CRect rect = GetWindowRect();
	rect.top += 40;
	rect.bottom	-= 10 ;
	rect.left -= 30;
	rect.right -= 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);

	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = m_nMax;//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
#endif // __S0809_GM_SCROLL
	
	UpdateData();

#ifdef __XUZHU
/*
	for( int j = m_nMax; j < 40; j ++ )
	{
		m_aList[ j ].nMemberLv = xRandom(5);			// 호칭
		m_aList[ j ].nJob =  xRandom(5);	// 직업스트링
		if( m_aList[ j ].nJob == 2)
			m_aList[ j ].nJob+=1;

		m_aList[ j ].nSex =  xRandom(2);	// 성
		m_aList[ j ].nLevel = xRandom(100);				// 레벨
		strcpy( m_aList[ j ].szName, "낄낄낄" );		// 이름
		m_aList[ j ].bIsOnLine = xRandom(2);

		m_aList[ j ].nGiveGold     = xRandom(5000);					// 길드에 기부한 페냐
		m_aList[ j ].dwGivePxpCount = xRandom(10000);				// 길드에 기부한 PXP횟수( 스킬 경험치 )
		m_aList[ j ].nWin           = xRandom(10);							// 무엇을 이겼지?
		m_aList[ j ].nLose          = xRandom(10);						// 무엇을 졌을까나?
	}
	m_nMax = 40;
*/
#endif


	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildTabMember::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABMEMBER, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildTabMember::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
#ifdef __S0809_GM_SCROLL
	if( nID == 1000 && dwMessage == 514 )	// Scroll Bar
	{
		int adf = m_wndScrollBar.GetScrollPos();
		int bbf = m_wndScrollBar.GetScrollRange();
		return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
	}
#endif // __S0809_GM_SCROLL
	int a = 0;

	CMover* pMover	= (CMover*)CMover::GetActiveObj();
	if( !pMover )
		return FALSE;
	CGuild* pGuild	= pMover->GetGuild();
	if( !pGuild )
		return FALSE;
	if( m_nSelect < 0 || m_nSelect >= m_nMax )
		return FALSE;

	if( g_pPlayer->m_idWar )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDWARERRORMEMBER ) );
		return FALSE;
	}

	if( pWndBase == &m_wndMenu )
	{
		switch( nID )
		{
		case MGI_APPELL_UP:		// 호칭 업.
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, m_aList[m_nSelect].nMemberLv - 1 );
			break;
		case MGI_APPELL_DOWN:
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, m_aList[m_nSelect].nMemberLv + 1 );
			break;

		case MGI_CLASS_UP:		// 등급 업
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, 1 );
			break;
		case MGI_CLASS_DOWN:	
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, 0 );
			break;
#ifdef __CHG_MASTER1025
		case MGI_CHG_MASTER:
			g_DPlay.SendChgMaster( pMover->m_idPlayer, 
				m_aList[m_nSelect].idPlayer );
			break;
#endif	// __CHG_MASTER1025
		case MGI_NICKNAME:		// 별칭 부여
			{
				CGuild* pGuild = g_pPlayer->GetGuild();
				if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
				{
					if( 10 > pGuild->m_nLevel )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_GUILDNOTLEVEL) ) );	
					}
					else
					{
						SAFE_DELETE( g_WndMng.m_pWndGuildNickName );
						g_WndMng.m_pWndGuildNickName = new CWndGuildNickName;
						g_WndMng.m_pWndGuildNickName->m_idPlayer = m_aList[m_nSelect].idPlayer;
						g_WndMng.m_pWndGuildNickName->Initialize( &g_WndMng, APP_GUILD_NICKNAME );
					}
				}
				else
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDONLYMASTER ), MB_OK, this );
				}
				
			}
			break;

		case MGI_GUILD_LEAVE:	// 탈퇴
			{
				if(  pGuild->GetQuest( QUEST_WARMON_LV1 ) != NULL && pGuild->GetQuest( QUEST_WARMON_LV1 )->nState == QS_BEGIN )	// 클락워크 퀘스트면 길드 해체 안됨
				{
					QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( QUEST_WARMON_LV1 );
					if( pQuestProp )
					{
						g_WndMng.OpenMessageBox( prj.GetText( TID_GUILD_QUEST_LEAVEERROR ), MB_OK, this );
					}
					return FALSE;
				}
				else
				{
					g_DPlay.SendRemoveGuildMember( pMover->m_idPlayer, m_aList[m_nSelect].idPlayer );
				}
			}
			break;
		} // switch nID
			
	}
		
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabMember::OnSize( UINT nType, int cx, int cy ) \
{ 
#ifdef __S0809_GM_SCROLL
	CRect rect = GetWindowRect();
	rect.top = rect.top + 40;
	rect.bottom	= rect.bottom - 10 ;
	rect.left = rect.right - 40;
	rect.right = rect.right - 25;
	m_wndScrollBar.SetWndRect( rect );
	
	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = m_nMax;//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
#endif // __S0809_GM_SCROLL
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildTabMember::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabMember::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	int mx = point.x - 8;
	int my = point.y - 32;
	m_nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;
	if( m_nSelect < 0 )
		m_nSelect = 0;
	if( m_nSelect >= m_nMax )
		m_nSelect = m_nMax - 1;
} 

void CWndGuildTabMember::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndGuildTabMember::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_wndMenu.DeleteAllMenu();
	m_wndMenu.AppendMenu( 0, MGI_APPELL_UP,		prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) );
	m_wndMenu.AppendMenu( 0, MGI_APPELL_DOWN,	prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) );
	m_wndMenu.AppendMenu( 0, MGI_CLASS_UP,		prj.GetText( TID_GAME_GUILD_CLASSUP ) );
	m_wndMenu.AppendMenu( 0, MGI_CLASS_DOWN,	prj.GetText( TID_GAME_GUILD_CLASSDOWN ) );
	m_wndMenu.AppendMenu( 0, MGI_NICKNAME,		prj.GetText( TID_GAME_GUILD_NICKNAME ) );
	m_wndMenu.AppendMenu( 0, MGI_GUILD_LEAVE,	prj.GetText( TID_GAME_GUILD_LEAVE ) );
#ifdef __CHG_MASTER1025
	CMover* pMover	= CMover::GetActiveMover();
	if( pMover )
	{
		CGuild* pGuild	= pMover->GetGuild();
		if( pGuild && pGuild->IsMaster( pMover->m_idPlayer ) )
			m_wndMenu.AppendMenu( 0, MGI_CHG_MASTER,	prj.GetText( TID_GAME_CHG_MASTER ) );
	}
#endif	// __CHG_MASTER1025

	m_wndMenu.Move( CPoint( m_rectCurrentWindow.left, m_rectCurrentWindow.top ) + point );
	m_wndMenu.SetVisible( TRUE );//!m_wndMenuMover.IsVisible() );
	m_wndMenu.SetFocus();
	
} 

BOOL CWndGuildTabMember::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabMember::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.

		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
		{
			m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
	
	}

	m_nMxOld = point.x;
	m_nMyOld = point.y;
}
void CWndGuildTabMember::OnMouseWndSurface( CPoint point )
{
	int mx = point.x - 8;
	int my = point.y - 32;
	int nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;

	if( nSelect < 0 )
		nSelect = 0;
	if( nSelect >= m_nMax )
		nSelect = m_nMax - 1;
	
	int nMax = (m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW : m_nMax;

	int offset_y = 30;

	for( int i=0; i<nMax; i++ )
	{
		CRect rect = CRect( 4,offset_y,m_rectLayout.right - m_rectLayout.left, (offset_y+15) );
		offset_y+=MEMBER_LIST_HEIGHT;

		if( PtInRect( &rect, point ) )
		{
			CRect rect2 = CRect( point.x, point.y, point.x+200, point.y+20 );
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect2 );

			CString str;

			str.Format( "%s\n%s %u\n%s %d\n%s %d", m_aList[ nSelect ].szName,
											   prj.GetText(TID_GAME_TOOLTIP_EXPMERIT ), m_aList[ nSelect ].dwGivePxpCount,
				                               prj.GetText(TID_GAME_TOOLTIP_PENYAMERIT ), m_aList[ nSelect ].nGiveGold,
											   prj.GetText(TID_GAME_TOOLTIP_GIVEUPNUM ), m_aList[ nSelect ].nLose );

			g_toolTip.PutToolTip( m_nIdWnd, str, rect2, point2 );
			break;
		}
	}
}	

BOOL CWndGuildTabMember::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	
//    if( nFlags & MK_SHIFT )
//		nZoom /= 8;
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;

	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
	{
		m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}

#ifdef __S0809_GM_SCROLL
	m_wndScrollBar.SetScrollPos( m_nCurrentList );
#endif // __S0809_GM_SCROLL
	return TRUE;
}


void CWndGuildTabMember::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		m_nMax = 0;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;

			m_aList[ m_nMax ].idPlayer       = pMember->m_idPlayer;
			m_aList[ m_nMax ].nMemberLv      = pMember->m_nMemberLv;			// 호칭
			m_aList[ m_nMax ].nJob           = pMember->m_nJob;	// 직업
			m_aList[ m_nMax ].nSex           = pMember->m_dwSex;	// 성
			m_aList[ m_nMax ].bIsOnLine      = pMember->m_nLogin;

			m_aList[ m_nMax ].nGiveGold     = pMember->m_nGiveGold;					// 길드에 기부한 페냐
			m_aList[ m_nMax ].dwGivePxpCount = pMember->m_dwGivePxpCount;				// 길드에 기부한 PXP횟수( 스킬 경험치 )
			m_aList[ m_nMax ].nWin           = pMember->m_nWin;							// 무엇을 이겼지?
			m_aList[ m_nMax ].nLose          = pMember->m_nLose;						// 무엇을 졌을까나?
		
			m_aList[ m_nMax ].nLevel = pMember->m_nLevel;			// 레벨
			LPCSTR pszPlayer	= prj.GetPlayerString( pMember->m_idPlayer );
			if( pszPlayer )
				lstrcpy( m_aList[m_nMax].szName, pszPlayer );
			else
				g_DPlay.SendQueryPlayerString( pMember->m_idPlayer, QPS_GUILD_MEMBER );

			strcpy( m_aList[ m_nMax ].szAlias, pMember->m_szAlias );
			m_aList[ m_nMax ].nClass		= pMember->m_nClass;

			m_nMax ++;
		}
	}
	else
	{
		m_nMax = 0;
		m_nCurrentList = 0;
		m_nSelect = 0;
		memset( m_aList, 0, sizeof(m_aList) );
		m_nMxOld = m_nMyOld = 0;
	}		
}

