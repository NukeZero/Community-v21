#include "stdafx.h"
#include "party.h"
#include "guild.h"
#include "WndMessengerCtrl.h"
#include "WndMessenger.h"
#include "WndManager.h"
#include "Housing.h"		//sun: 13, 하우징 시스템

#include "playerdata.h"		//sun: 11, 캐릭터 정보 통합

#include "Campus.h"
#include "../Neuz/CampusHelper.h"

extern CParty		g_Party;

//sun: 11, 메신저창 개선
//////////////////////////////////////////////////////////////////////////
// Common Local Func.
//////////////////////////////////////////////////////////////////////////
bool prLevelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nLevel > player2.m_nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prLevelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nLevel < player2.m_nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nJob > player2.m_nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nJob < player2.m_nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prStatusAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;

	// offline 상태가 맨 마지막상태
	if(nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if(nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if(nPlayer1Status < nPlayer2Status)
		rtn_val = true;
	
	return rtn_val;
}

bool prStatusDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;

	// offline 상태가 맨 마지막상태
	if(nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if(nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if(nPlayer1Status > nPlayer2Status)
		rtn_val = true;
	
	return rtn_val;
	
	return rtn_val;
}

bool prChannelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if(player1.m_nChannel > player2.m_nChannel)
		rtn_val = true;
	
	return rtn_val;
}

bool prChannelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if(player1.m_nChannel < player2.m_nChannel)
		rtn_val = true;
	
	return rtn_val;
}

bool prNameAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if(strplayer1Name > strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prNameDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if(strplayer1Name < strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

//-----------------------------------------------------------------------------
__MESSENGER_PLAYER::__MESSENGER_PLAYER( void ) : 
m_nChannel( 0 ),
m_dwStatus( 0 ),
m_nLevel( 0 ), 
m_nJob( 0 ), 
m_dwPlayerId( 0 ), 
m_bBlock( FALSE ), 
m_bVisitAllowed( FALSE )
{
	ZeroMemory( m_szName, sizeof( m_szName ) );
}
//-----------------------------------------------------------------------------
void __MESSENGER_PLAYER::Initialize( void )
{
	m_nChannel = 0;
	m_dwStatus = 0;
	m_nLevel = 0;
	m_nJob = 0;
	m_dwPlayerId = 0;
	m_bBlock = FALSE;
	m_bVisitAllowed = FALSE;
	ZeroMemory( m_szName, sizeof( m_szName ) );
}
//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Messenger Friend Tab Ctrl
//////////////////////////////////////////////////////////////////////////

CWndFriendCtrlEx::CWndFriendCtrlEx() 
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndFriendCtrlEx::~CWndFriendCtrlEx()
{
}

void CWndFriendCtrlEx::Create( RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndFriendCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	m_menu.CreateMenu( this );

	UpdatePlayerList();
}

void CWndFriendCtrlEx::UpdatePlayerList()
{
	// Set Friends List
	m_vPlayerList.clear();
	__MESSENGER_PLAYER stPlayer;
//sun: 13, 하우징 시스템
	std::vector<DWORD> vecTemp;
	CHousing::GetInstance()->GetVisitAllow( vecTemp );

	for( std::map<u_long, Friend>::iterator i	= g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i )
	{
		u_long idPlayer	= i->first;
		Friend* pFriend		= &i->second;
		PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		int nJob	= pPlayerData->data.nJob;
		int nLevel	= pPlayerData->data.nLevel;
		u_long uLogin	= pPlayerData->data.uLogin;
		LPCSTR lpszPlayer	= pPlayerData->szPlayer;
		DWORD dwState	= pFriend->dwState;
		stPlayer.m_dwPlayerId	= idPlayer;
		stPlayer.m_dwStatus		= dwState;
		stPlayer.m_nJob		= nJob;
		stPlayer.m_nLevel	= nLevel;
		stPlayer.m_bBlock	= pFriend->bBlock;
//sun: 13, 하우징 시스템
		stPlayer.m_bVisitAllowed = FALSE;
		std::vector<DWORD>::iterator iterV = vecTemp.begin();
		for( ; iterV != vecTemp.end(); ++iterV)
		{
			if(idPlayer == *iterV)
				stPlayer.m_bVisitAllowed = TRUE;
		}

		if( stPlayer.m_dwStatus == FRS_OFFLINE )
			stPlayer.m_nChannel	= 100;
		else
			stPlayer.m_nChannel	= uLogin;

		FLStrcpy( stPlayer.m_szName, _countof( stPlayer.m_szName ), lpszPlayer );
		m_vPlayerList.push_back( stPlayer );
	}

	switch(m_nCurSort)
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}

void CWndFriendCtrlEx::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

void CWndFriendCtrlEx::OnDraw( C2DRender* p2DRender ) 
{
	PFONTCOLOR_WNDFRIENDCTRLEX pFontColorWndFriendCtrlEx = g_WndFontColorManager->GetWndFriendCtrlEx();
	
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;

	if( NULL == g_pPlayer )
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 4 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	CString strFormat;
	std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	for(int i=0; iter != m_vPlayerList.end(); i++, iter++)
	{
		if( i < m_nDrawCount )
			continue;

		if( i >= nMax )
			break;
		
		__MESSENGER_PLAYER stPlayer = *(iter);
		
		// Draw Channel Icon
		if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		{
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, WNDCOLOR_WHITE );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 14, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, WNDCOLOR_WHITE );
		}

		// Draw Status Icon
		DWORD dwMyState;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;
		
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 76, pt.y ), 7 + ( dwMyState - 2 ), &pVertices, WNDCOLOR_WHITE );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 72, pt.y ), 7 + ( dwMyState - 2 ), &pVertices, WNDCOLOR_WHITE );

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);
		static int LEVEL_TEXT_X = 127;

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			LEVEL_TEXT_X = 129;

		if( stPlayer.m_bVisitAllowed )
			p2DRender->TextOut( LEVEL_TEXT_X, pt.y + 3, strFormat, pFontColorWndFriendCtrlEx->m_stVisitAllow.GetFontColor() );
		else
			p2DRender->TextOut( LEVEL_TEXT_X, pt.y + 3, strFormat, m_dwColor );

		// Draw Job Icon
		int JOB_TYPE_ICON_X = 174;

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			JOB_TYPE_ICON_X = 178;

		int JOB_ICON_X = JOB_TYPE_ICON_X + 20;

		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, WNDCOLOR_WHITE );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, &pVertices, WNDCOLOR_WHITE );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, WNDCOLOR_WHITE );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, WNDCOLOR_WHITE );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, WNDCOLOR_WHITE );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_LEGEND_HERO )	//gmpbigsun(20110207) : #11442 3차직업아이콘
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, WNDCOLOR_WHITE );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 32), &pVertices, WNDCOLOR_WHITE );
		}
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, &pVertices, WNDCOLOR_WHITE );


		// Draw Name
		DWORD dwColor = m_dwColor;
		if( i == m_nCurSelect )
			dwColor = pFontColorWndFriendCtrlEx->m_stSelect.GetFontColor();
		
		if( stPlayer.m_bBlock )
			dwColor = pFontColorWndFriendCtrlEx->m_stBlock.GetFontColor();
		
		strFormat.Format("%s", stPlayer.m_szName);
	
		if( strFormat.GetLength() > 10 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount<10; )
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}

		int NAME_TEXT_X = 226;

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			NAME_TEXT_X = 236;

		int NAME_TEXT_Y = pt.y + 3;

		if( stPlayer.m_bVisitAllowed )
			p2DRender->TextOut( NAME_TEXT_X, NAME_TEXT_Y, strFormat, pFontColorWndFriendCtrlEx->m_stVisitAllow.GetFontColor() );
		else
			p2DRender->TextOut( NAME_TEXT_X, NAME_TEXT_Y, strFormat, dwColor );
		
		pt.y += m_nFontHeight;
	}

	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}

void CWndFriendCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt( 188, 3 );
	CRect rect;

	int nCount = m_vPlayerList.size();
	if(nCount > 10)
		nCount = 10;

	for( int j=0; j<nCount; j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}
}

void CWndFriendCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;

	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	g_WorldMng()->SetObjFocus(NULL);
}

void CWndFriendCtrlEx::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	u_long idPlayer;
	Friend* pFriend = NULL;		//gmpbigsun(20100715): 더블클릭했으나 선택할수 있는 영역이 아닌경우 크래쉬 수정 
	int nSelect		= GetSelect( point, idPlayer, &pFriend );
	if( nSelect != -1 && pFriend )
	{
		DWORD dwState	= pFriend->dwState;
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
		{
			m_nCurSelect = nSelect;
//sun: 11, 캐릭터 정보 통합
			CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		}
		else
		{
			CString szMessage;
			if( dwState == FRS_OFFLINE )
				szMessage = prj.GetText(TID_GAME_NOTLOGIN);                               //"??? 님은 접속되어 있지 않습니다";
			else
//sun: 11, 캐릭터 정보 통합
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );  //"??? 님은 차단되어 있어 메세지를 보낼수 없습니다";

			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor( TID_GAME_NOTLOGIN ) );		
		}
	}	
}

void	CWndFriendCtrlEx::GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend )
{
	*ppFriend	= NULL;
	std::vector < __MESSENGER_PLAYER >::iterator iter	= m_vPlayerList.begin();
	iter	+= SelectCount;
	__MESSENGER_PLAYER stPlayer	= *(iter);
	idPlayer	= stPlayer.m_dwPlayerId;
	*ppFriend	= g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
}

u_long CWndFriendCtrlEx::GetSelectId( int SelectCount )
{
	std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);
	
	return stPlayer.m_dwPlayerId;
}

int	CWndFriendCtrlEx::GetSelect( CPoint point, u_long &idPlayer, Friend** ppFriend )
{
	CPoint pt( 3, 3 );
	CRect rect;
	int rtn_val = -1;
	
	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rtn_val++;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			rtn_val += nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			idPlayer = stPlayer.m_dwPlayerId;
			*ppFriend = g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}

	return rtn_val;
}

BOOL CWndFriendCtrlEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	m_menu.SetVisible(FALSE);

	u_long idPlayer;

	switch( nID )
	{
	case 0:		// 메시지
		{
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
				CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		}
		break;
	case 1:		// 차단 / 차단해제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendFriendInterceptState( uidPlayer );		
		}
		break;
	case 2:		// 삭제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendRemoveFriend( uidPlayer );
		}
		break;
	case 3:		// 이동
		{
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
		}
		break;
	case 4:		// 극단 초청
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
				if( pWndWorld )
					pWndWorld->InviteParty( uidPlayer );
			}
		}
		break;
	case 6:		// 쪽지 보내기
		{
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote	= new CWndMessageNote;
				FLStrcpy( g_WndMng.m_pWndMessageNote->m_szName, _countof( g_WndMng.m_pWndMessageNote->m_szName ), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
				g_WndMng.m_pWndMessageNote->m_dwUserId	= idPlayer;
				g_WndMng.m_pWndMessageNote->Initialize();
			}
		}
		break;
//sun: 13, 하우징 시스템
		case 7 :	// 입장허가를 취소한다
			{
				u_long uidPlayer = GetSelectId( m_nCurSelect );
				if(uidPlayer > 0)
					g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, FALSE);
			}
			break;
		case 8:		// 입장을 허가한다
			{
				u_long uidPlayer = GetSelectId( m_nCurSelect );
				if(uidPlayer > 0)
					g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, TRUE);
			}
			break;

	}
	return -1; 
} 

void CWndFriendCtrlEx::OnRButtonUp( UINT nFlags, CPoint point )
{
	u_long idPlayer;
	Friend* pFriend		= NULL;
	int nSelect		= GetSelect( point, idPlayer, &pFriend );

	if( nSelect != -1 && pFriend != NULL )
	{
		DWORD dwState	= pFriend->dwState;
		m_nCurSelect	= nSelect;
		ClientToScreen( &point );
		m_menu.DeleteAllMenu();
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
			m_menu.AppendMenu( 0, 0 ,_T( prj.GetText( TID_APP_MESSAGE ) ) );
		if( pFriend->bBlock )
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText( TID_FRS_BLOCKRESTORE ) ) );
		else
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText( TID_FRS_BLOCK ) ) );
		m_menu.AppendMenu( 0, 2 ,_T( prj.GetText( TID_FRS_DELETE ) ) );

		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
		{					
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				m_menu.AppendMenu( 0, 3 ,_T( prj.GetText( TID_FRS_MOVE2 ) ) );

			if( !pFriend->bBlock )
			{
				if( g_Party.IsMember( idPlayer ) == FALSE )
					m_menu.AppendMenu( 0, 4 ,_T( prj.GetText(TID_MMI_INVITE_PARTY) ) );
			}
		}

		if( dwState == FRS_OFFLINE || pFriend->bBlock )
			m_menu.AppendMenu( 0, 6 , _T( prj.GetText( TID_GAME_TAGSEND ) ) );

//sun: 13, 하우징 시스템
		std::vector<__MESSENGER_PLAYER>::iterator iter = m_vPlayerList.begin();
		int		nCount		= 0;
		BOOL	bIsAllowed	= FALSE;
		for(; iter != m_vPlayerList.end(); ++iter)
		{
			if(idPlayer == iter->m_dwPlayerId)
			{
				if(iter->m_bVisitAllowed)	
					bIsAllowed = TRUE;
			}
		}
		if(bIsAllowed)
			m_menu.AppendMenu( 0, 7 , _T( prj.GetText( TID_GAME_PROHIBIT_VISIT ) ) );
		else
			m_menu.AppendMenu( 0, 8 , _T( prj.GetText( TID_GAME_ALLOW_VISIT ) ) );

		m_menu.Move( point );
		m_menu.SetVisible( TRUE );
		m_menu.SetFocus();
	}
}

void CWndFriendCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange	= g_WndMng.m_RTMessenger.size();
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndFriendCtrlEx::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndFriendCtrlEx::GetDrawCount( void )
{
	int nMax	= g_WndMng.m_RTMessenger.size();
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}

void CWndFriendCtrlEx::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange	= g_WndMng.m_RTMessenger.size();
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndFriendCtrlEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndFriendCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndFriendCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndFriendCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndFriendCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndFriendCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

//////////////////////////////////////////////////////////////////////////
// Messenger Guild Tab Ctrl
//////////////////////////////////////////////////////////////////////////

CWndGuildCtrlEx::CWndGuildCtrlEx() 
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndGuildCtrlEx::~CWndGuildCtrlEx()
{
}

void CWndGuildCtrlEx::Create( RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndGuildCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	UpdatePlayerList();
}

void CWndGuildCtrlEx::UpdatePlayerList()
{
	// Set GuildMemeber List
	m_vPlayerList.clear();
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		std::map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.begin();

		for( ; iter != pGuild->m_mapPMember.end() ; ++iter )
		{
			__MESSENGER_PLAYER stPlayer;
			CGuildMember* pGuildMember = (CGuildMember*)iter->second;
//sun: 11, 캐릭터 정보 통합
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
			stPlayer.m_nJob	= pPlayerData->data.nJob;
			stPlayer.m_nLevel = pPlayerData->data.nLevel;
			stPlayer.m_dwPlayerId = pGuildMember->m_idPlayer;
			stPlayer.m_nChannel = pPlayerData->data.uLogin;
			if( pPlayerData->data.uLogin > 0 )
				stPlayer.m_dwStatus = FRS_ONLINE;
			else
				stPlayer.m_dwStatus = FRS_OFFLINE;

			FLStrcpy( stPlayer.m_szName, _countof( stPlayer.m_szName ), pPlayerData->szPlayer );						//sun: 11, 캐릭터 정보 통합

			m_vPlayerList.push_back(stPlayer);
		}
	}

	switch(m_nCurSort)
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}

void CWndGuildCtrlEx::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

void CWndGuildCtrlEx::OnDraw( C2DRender* p2DRender ) 
{
	FLERROR_ASSERT_LOG_RETURN_VOID( g_pPlayer, _T("[윈도우 길드 컨트롤(메신져)] 플레이어의 포인터가 유효하지 않습니다") );

	PFONTCOLOR_WNDGUILDCTRLEX	pFontColorWndGuildCtrlEx( g_WndFontColorManager->GetWndGuildCtrlEx() );
	CPoint						pt( 3, 3 );

	m_nDrawCount	= 0;
	int nMax		= GetDrawCount();
	m_nDrawCount	= m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndWorld, _T("[윈도우 길드 컨트롤(메신져)] 월드 윈도우의 포인터가 유효하지 않습니다") );

	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 4 * nMax ];
	FLERROR_ASSERT_LOG_RETURN_VOID( pVertex, _T("[윈도우 길드 컨트롤(메신져)] 정점의 포인터가 유효하지 않습니다") );

	TEXTUREVERTEX2* pVertices( pVertex );

	CString strFormat( _T("") );

	std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	for(int i=0; iter != m_vPlayerList.end(); i++, iter++)
	{
		if( i < m_nDrawCount )
			continue;
		if( i >= nMax )
			break;
				
		__MESSENGER_PLAYER stPlayer = *(iter);
		
		// Draw Channel Icon
		//if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		//	pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 12, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, WNDCOLOR_WHITE );

		if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		{
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, WNDCOLOR_WHITE );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 14, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, WNDCOLOR_WHITE );
		}


		// Draw Status Icon
		DWORD dwMyState;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;
		
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 76, pt.y ), 7 + ( dwMyState - 2 ), &pVertices, WNDCOLOR_WHITE );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 72, pt.y ), 7 + ( dwMyState - 2 ), &pVertices, WNDCOLOR_WHITE );
	
		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 127, pt.y + 3, strFormat, m_dwColor );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 129, pt.y + 3, strFormat, m_dwColor );

		// Draw Job Icon
		static int JOB_TYPE_ICON_X = 174;

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			JOB_TYPE_ICON_X = 178;

		int JOB_ICON_X = JOB_TYPE_ICON_X + 20;

		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_LEGEND_HERO ) //gmpbigsun(20110207) : #11442 3차직업아이콘
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff );
		}
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, &pVertices, 0xffffffff );


		// Draw Name
		DWORD dwColor = m_dwColor;
		if( i == m_nCurSelect )
			dwColor = pFontColorWndGuildCtrlEx->m_stSelect.GetFontColor();
		
		strFormat.Format("%s", stPlayer.m_szName);
	
		if( strFormat.GetLength() > 10 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount<10; )
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 226, pt.y + 3, strFormat, dwColor );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 236, pt.y + 3, strFormat, dwColor );
		
		pt.y += m_nFontHeight;
	}

	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}

void CWndGuildCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt( 188, 3 );
	CRect rect;

	int nCount = m_vPlayerList.size();
	if(nCount > 10)
		nCount = 10;

	for( int j=0; j<nCount; j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}
}

void CWndGuildCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;

	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	g_WorldMng()->SetObjFocus(NULL);
}

void CWndGuildCtrlEx::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	u_long idPlayer;
	CGuildMember* pGuildMember = NULL;
	int nSelect	= GetSelect( point, idPlayer, &pGuildMember );
	if( nSelect != -1 && pGuildMember )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );

		if( pPlayerData && pPlayerData->data.uLogin > 0 ) // state online
		{
			m_nCurSelect = nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		}
		else
		{
			CString szMessage;
//			if( dwState == FRS_OFFLINE )
				szMessage = prj.GetText( TID_GAME_NOTLOGIN );                               //"??? 님은 접속되어 있지 않습니다";

			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );		
		}
	}	
}

u_long CWndGuildCtrlEx::GetSelectId( int SelectCount )
{
	std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);
	
	return stPlayer.m_dwPlayerId;
}

int	CWndGuildCtrlEx::GetSelect( CPoint point, u_long & idPlayer, CGuildMember** lppGuildMember )
{
	CPoint pt( 3, 3 );
	CRect rect;
	int rtn_val = -1;
	
	CGuild* pGuild = g_pPlayer->GetGuild();

	if(pGuild)
	{
		for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
		{
			rtn_val++;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				std::vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
				int nPos = m_wndScrollBar.GetScrollPos();
				iter += j + nPos;
				rtn_val += nPos;
				__MESSENGER_PLAYER stPlayer = *(iter);
				idPlayer = stPlayer.m_dwPlayerId;
				CGuildMember * lpGuildMember = pGuild->GetMember( idPlayer );
				if( lpGuildMember )
					*lppGuildMember = lpGuildMember;

				j = m_vPlayerList.size();
			}
			pt.y += m_nFontHeight;
		}
	}

	return rtn_val;
}

void CWndGuildCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange	= g_WndMng.m_RTMessenger.size();
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndGuildCtrlEx::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndGuildCtrlEx::GetDrawCount( void )
{
	CGuild * pGuild = g_pPlayer->GetGuild();
	int nMax = 0;
	if( pGuild )
		nMax = pGuild->GetSize();

	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}

void CWndGuildCtrlEx::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		nRange = pGuild->GetSize();
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );
	}
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndGuildCtrlEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndGuildCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndGuildCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndGuildCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndGuildCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndGuildCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

//-----------------------------------------------------------------------------
CWndCampus::CWndCampus( void ) : 
m_bSortbyChannel( TRUE ), 
m_bSortbyStatus( FALSE ), 
m_bSortbyLevel( TRUE ), 
m_bSortbyJob( TRUE ), 
m_bSortbyName( TRUE ), 
m_nCurSort( SORT_BY_STATUS ), 
m_bCurSelectedMaster( FALSE ), 
m_nCurSelectedDisciple( -1 ), 
m_nFontHeight( 20 ), 
m_MasterPlayer()
{
	m_vDisciplePlayer.clear();
}
//-----------------------------------------------------------------------------
CWndCampus::~CWndCampus( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_TAB_CAMPUS, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndCampus::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndListBox* pWndListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX_MASTER );
	FLASSERT( pWndListBox );
	pWndListBox->EnableWindow( FALSE );
	pWndListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX_DISCIPLE );
	FLASSERT( pWndListBox );
	pWndListBox->EnableWindow( FALSE );

	m_Menu.CreateMenu( this );

	UpdatePlayerList();
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	m_Menu.SetVisible( FALSE );
	switch( nID )
	{
	case 0:
		{
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
			if( pCampus == NULL )
				return FALSE;
			if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
			{
				__MESSENGER_PLAYER* pstDisciplePlayer = GetSelectedDiscipleID( m_nCurSelectedDisciple );
				if( !pstDisciplePlayer )
					return FALSE;
				if( pstDisciplePlayer->m_dwPlayerId != -1 )
				{
					if( g_WndMng.m_pWndCampusSeveranceConfirm )
						SAFE_DELETE( g_WndMng.m_pWndCampusSeveranceConfirm );
					g_WndMng.m_pWndCampusSeveranceConfirm = new CWndCampusSeveranceConfirm( static_cast< u_long >( pstDisciplePlayer->m_dwPlayerId ), 
																							pstDisciplePlayer->m_szName );
					g_WndMng.m_pWndCampusSeveranceConfirm->Initialize( NULL );
				}
			}
			else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
			{
				if( m_MasterPlayer.m_dwPlayerId != -1 )
				{
					if( g_WndMng.m_pWndCampusSeveranceConfirm )
						SAFE_DELETE( g_WndMng.m_pWndCampusSeveranceConfirm );
					g_WndMng.m_pWndCampusSeveranceConfirm = new CWndCampusSeveranceConfirm( static_cast< u_long >( m_MasterPlayer.m_dwPlayerId ), 
																							m_MasterPlayer.m_szName );
					g_WndMng.m_pWndCampusSeveranceConfirm->Initialize( NULL );
				}
			}
			break;
		}
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndCampus::PaintFrame( C2DRender* p2DRender )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		CRect rect = GetWindowRect();
		DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
		DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
		DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

		p2DRender->RenderFillRect ( rect, dwColor1 );
		p2DRender->RenderRoundRect( rect, dwColor2 );
		rect.DeflateRect( 1 , 1 );
		p2DRender->RenderRect( rect, dwColor2 );
		rect.DeflateRect( 1 , 1 );
		p2DRender->RenderRect( rect, dwColor3 );
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		CWndBase::PaintFrame( p2DRender );
}
//-----------------------------------------------------------------------------


void CWndCampus::OnDraw( C2DRender* p2DRender )
{
	if( NULL == g_pPlayer )
		return;

	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;

	PFONTCOLOR_WNDCAMPUS pFontColorWndCampus = g_WndFontColorManager->GetWndCampus();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CPoint pt( 3, 3 );

	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = DISCIPLE_RENDERING_POSITION;
		int nDiscipleMax = GetDiscipleDrawCount();
		std::vector < __MESSENGER_PLAYER >::iterator iter = m_vDisciplePlayer.begin();
		for( int i = 0; iter < m_vDisciplePlayer.end(); ++i, ++iter )
		{
			CString strFormat = _T( "" );
			if( i >= nDiscipleMax )
			{
				iter = m_vDisciplePlayer.end();
				continue;
			}

			__MESSENGER_PLAYER stPlayer = *iter;

			if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
			{
				if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
					pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, WNDCOLOR_WHITE );
				if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
					pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 14, pt.y ), 34 + stPlayer.m_nChannel - 1, WNDCOLOR_WHITE );
			}

			// Draw Status Icon
			DWORD dwMyState = 0;
			if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
				dwMyState = FRS_ABSENT;
			else if( stPlayer.m_dwStatus == FRS_ONLINE )
				dwMyState = 2;
			else if( stPlayer.m_dwStatus == FRS_OFFLINE )
				dwMyState = 8;
			else
				dwMyState = stPlayer.m_dwStatus;

			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 76, pt.y ), 7 + ( dwMyState - 2 ), WNDCOLOR_WHITE );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 72, pt.y ), 7 + ( dwMyState - 2 ), WNDCOLOR_WHITE );

			// Draw Level
			strFormat.Format("%d", stPlayer.m_nLevel);

			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				p2DRender->TextOut( 127, pt.y + 3, strFormat, m_dwColor );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				p2DRender->TextOut( 129, pt.y + 3, strFormat, m_dwColor );

			// Draw Job Icon
			static int JOB_TYPE_ICON_X = 174;

			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				JOB_TYPE_ICON_X = 168;

			int JOB_ICON_X = JOB_TYPE_ICON_X + 20;

			if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), 0xffffffff );
			else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
			{
				int nMasterIndex = 27;
				if( stPlayer.m_nLevel < 70 )
					nMasterIndex = 27;
				else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
					nMasterIndex = 28;
				else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
					nMasterIndex = 29;
				else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
					nMasterIndex = 30;
				else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
					nMasterIndex = 31;
				else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
					nMasterIndex = 32;
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, 0xffffffff );
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), 0xffffffff );
			}
			else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
			{
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, 0xffffffff );
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), 0xffffffff );
			}
			else
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, 0xffffffff );

			// Draw Name
			DWORD dwColor = ( i == m_nCurSelectedDisciple ) ? pFontColorWndCampus->m_stSelect.GetFontColor() : m_dwColor;
			strFormat.Format( "%s", stPlayer.m_szName );
			if( strFormat.GetLength() > 10 )
			{
				int	nReduceCount = 0;

				for( nReduceCount = 0; nReduceCount < 10; )
				{
					if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
						nReduceCount += 2;
					else
						++nReduceCount;
				}

				strFormat = strFormat.Left( nReduceCount );
				strFormat += "...";
			}

			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				p2DRender->TextOut( 226, pt.y + 3, strFormat, dwColor );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				p2DRender->TextOut( 236, pt.y + 3, strFormat, dwColor );

			pt.y += m_nFontHeight;
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = MASTER_RENDERING_POSITION;
		CString strFormat = _T( "" );
		__MESSENGER_PLAYER stPlayer = m_MasterPlayer;

		// Draw Channel Icon
		if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		{
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, WNDCOLOR_WHITE );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 14, pt.y ), 34 + stPlayer.m_nChannel - 1, WNDCOLOR_WHITE );
		}

		// Draw Status Icon
		DWORD dwMyState = 0;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 76, pt.y ), 7 + ( dwMyState - 2 ), WNDCOLOR_WHITE );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 72, pt.y ), 7 + ( dwMyState - 2 ), WNDCOLOR_WHITE );

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 127, pt.y + 3, strFormat, m_dwColor );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 129, pt.y + 3, strFormat, m_dwColor );

		// Draw Job Icon
		static int JOB_TYPE_ICON_X = 174;

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			JOB_TYPE_ICON_X = 168;

		int JOB_ICON_X = JOB_TYPE_ICON_X + 20;

		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), WNDCOLOR_WHITE );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, WNDCOLOR_WHITE );
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), WNDCOLOR_WHITE );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, WNDCOLOR_WHITE );
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), WNDCOLOR_WHITE );
		}
		else
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, WNDCOLOR_WHITE );

		// Draw Name
		DWORD dwColor = ( m_bCurSelectedMaster == TRUE ) ? pFontColorWndCampus->m_stSelect.GetFontColor() : m_dwColor;
		strFormat.Format( "%s", stPlayer.m_szName );
		if( strFormat.GetLength() > 10 )
		{
			int	nReduceCount = 0;

			for( nReduceCount = 0; nReduceCount < 10; )
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}
			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 226, pt.y + 3, strFormat, dwColor );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			p2DRender->TextOut( 236, pt.y + 3, strFormat, dwColor );
	}
}
//-----------------------------------------------------------------------------
void CWndCampus::OnLButtonUp( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
		GetSelectedDiscipleID( point );
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
		GetSelectedMasterID( point );
		
}
//-----------------------------------------------------------------------------
void CWndCampus::OnRButtonUp( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		if( GetSelectedDiscipleID( point ) == -1 )
			return;
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		if( GetSelectedMasterID( point ) == -1 )
			return;
	}
	ClientToScreen( &point );
	m_Menu.DeleteAllMenu();
	m_Menu.AppendMenu( 0, 0, prj.GetText( TID_GAME_MENU_CAMPUS_SEVERANCE ) );
	m_Menu.Move( point );
	m_Menu.SetVisible( TRUE );
	m_Menu.SetFocus();
}
//-----------------------------------------------------------------------------
void CWndCampus::OnMouseMove( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;

	CPoint pt( 188, 3 );
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = DISCIPLE_RENDERING_POSITION;
		CRect rect( 0, 0, 0, 0 );
		for( int i = 0; i < (int)( m_vDisciplePlayer.size() ); ++i )
		{
			rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) == TRUE )
			{
				std::vector< __MESSENGER_PLAYER >::iterator DiscipleIterator = m_vDisciplePlayer.begin();
				DiscipleIterator += i;
				__MESSENGER_PLAYER stPlayer	= *DiscipleIterator;
				ClientToScreen( &point );
				ClientToScreen( &rect );
				g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
				i = m_vDisciplePlayer.size();
			}
			pt.y += m_nFontHeight;
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = MASTER_RENDERING_POSITION;
		CRect rect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) == TRUE )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ m_MasterPlayer.m_nJob ].szName, rect, point, 3 );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndCampus::UpdatePlayerList( void )
{
	m_MasterPlayer.Initialize();
	m_vDisciplePlayer.clear();
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) )
	{
		typedef std::vector< u_long > DiscipleVector;
		DiscipleVector vecPupil = pCampus->GetPupilPlayerId();
		for( DiscipleVector::iterator itor = vecPupil.begin(); itor != vecPupil.end(); ++itor )
		{
			u_long idDisciplePlayer = *itor;
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( idDisciplePlayer );
			if( pPlayerData == NULL )
				continue;
			__MESSENGER_PLAYER stDisciplePlayer;
			stDisciplePlayer.m_dwPlayerId = idDisciplePlayer;
			stDisciplePlayer.m_dwStatus = ( pPlayerData->data.uLogin > 0 ) ? FRS_ONLINE : FRS_OFFLINE;
			stDisciplePlayer.m_nChannel	= pPlayerData->data.uLogin;
			stDisciplePlayer.m_nJob = pPlayerData->data.nJob;
			stDisciplePlayer.m_nLevel = pPlayerData->data.nLevel;
			FLStrcpy( stDisciplePlayer.m_szName, _countof( stDisciplePlayer.m_szName ), pPlayerData->szPlayer );
			m_vDisciplePlayer.push_back( stDisciplePlayer );
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) )
	{
		u_long idMasterPlayer = pCampus->GetMaster();
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( idMasterPlayer );
		if( pPlayerData == NULL )
			return;
		m_MasterPlayer.m_dwPlayerId = idMasterPlayer;
		m_MasterPlayer.m_dwStatus = ( pPlayerData->data.uLogin > 0 ) ? FRS_ONLINE : FRS_OFFLINE;
		m_MasterPlayer.m_nChannel = pPlayerData->data.uLogin;
		m_MasterPlayer.m_nJob = pPlayerData->data.nJob;
		m_MasterPlayer.m_nLevel = pPlayerData->data.nLevel;
		FLStrcpy( m_MasterPlayer.m_szName, _countof( m_MasterPlayer.m_szName ), pPlayerData->szPlayer );
	}
	switch( m_nCurSort )
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}
//-----------------------------------------------------------------------------
int CWndCampus::GetDiscipleDrawCount( void ) const
{
	return m_vDisciplePlayer.size();
}
//-----------------------------------------------------------------------------
__MESSENGER_PLAYER* CWndCampus::GetSelectedDiscipleID( int nSelectedNumber )
{
//	vector < __MESSENGER_PLAYER >::iterator iter = m_vDisciplePlayer.begin();
//	iter += nSelectedNumber;
	//	BEGINTEST100113	??????????
//	return	( ( __MESSENGER_PLAYER* )( &iter ) );
	if( nSelectedNumber >= (int)( m_vDisciplePlayer.size() ) || nSelectedNumber < 0 )	//gmpbigsun: with => || nSelectedNumber < 0 )
		return NULL;
	return	&m_vDisciplePlayer[nSelectedNumber];
	//	ENDTEST100113	??????????
}
//-----------------------------------------------------------------------------
u_long CWndCampus::GetSelectedMasterID( CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return -1;
	if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		CPoint pt( 3, MASTER_RENDERING_POSITION );
		CRect rect( pt.x, pt.y, pt.x + m_rectWindow.Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) == TRUE )
		{
			m_bCurSelectedMaster = TRUE;
			return static_cast< u_long >( m_MasterPlayer.m_dwPlayerId );
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
u_long CWndCampus::GetSelectedDiscipleID( CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return -1;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		CPoint pt( 3, DISCIPLE_RENDERING_POSITION );
		CRect rect( 0, 0, 0, 0 );
		for( int i = 0; i < (int)( m_vDisciplePlayer.size() ); ++i )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) == TRUE )
			{
				m_nCurSelectedDisciple = i;
				return static_cast< u_long >( ( ( __MESSENGER_PLAYER )( m_vDisciplePlayer[i] ) ).m_dwPlayerId );
			}
			pt.y += m_nFontHeight;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyChannel( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyStatus( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyLevel( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyJob( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyName( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}
//-----------------------------------------------------------------------------
