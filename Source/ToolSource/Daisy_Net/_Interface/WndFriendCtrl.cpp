// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "party.h"
#include "WndFriendCtrl.h"
#include "WndMessenger.h"
#include "company.h"
extern	CDPClient	g_DPlay;
extern CParty g_Party;
extern	CCompanyMng		g_CompanyMng;

#include "WndManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndFriendCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndFriendCtrl::CWndFriendCtrl() 
{
	m_uParent = 0;
	m_nCurSelect = -1;

	m_bDrag = FALSE;
	m_pVBGauge = NULL;

	memset( m_uServerPlayerId, 0, sizeof( m_uServerPlayerId ) );
	memset( m_nServerCount, 0, sizeof( m_nServerCount) );
}
CWndFriendCtrl::~CWndFriendCtrl()
{
	DeleteDeviceObjects();
}

void CWndFriendCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndFriendCtrl::SetSearch( int nSelectServer, int nSex, LPCTSTR strId )
{
	m_nSelectServer = nSelectServer;
	m_nSex = nSex;
	strcpy( m_szId, strId );
}

void CWndFriendCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndFriendCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
}
HRESULT CWndFriendCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndFriendCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndFriendCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndFriendCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_nFontHeight = 20;

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );

	m_menu.CreateMenu( this );	
}

#ifdef __SMEBUG_0831
void CWndFriendCtrl::OnDraw( C2DRender* p2DRender ) 
{
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;
	memset( m_nServerCount, 0, sizeof( m_nServerCount ) );

	if( NULL == g_pPlayer )
		return;

	// Draw Max Count
	int nMax = GetDrawCount();

	CWndWorld*		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 3 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();
//	for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != g_WndMng.m_Messenger.m_aFriend.end(); i++, iter++, m_nDrawCount++ );
	int i = 0;
	m_nDrawCount = m_wndScrollBar.GetScrollPos();
	// Draw Select
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end() ; ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;
		int nMuilti = lpFriend->m_uIdofMulti -1;
		
		if( lpFriend->m_uIdofMulti == 100 || lpFriend->dwState == FRS_OFFLINE ) // logOff Last Draw
		{	
			m_uServerPlayerId[10][m_nServerCount[10]] = lpFriend->dwUserId;
			++m_nServerCount[10];
		}
		else	// Multi Server
		{
			m_uServerPlayerId[nMuilti][m_nServerCount[nMuilti]] = lpFriend->dwUserId;
			++m_nServerCount[nMuilti];
		}
	}

	// My Multi Draw
	int nMyMuilti = g_Neuz.m_uIdofMulti-1;

	for( int j = 0 ; j < m_nServerCount[nMyMuilti] && i < nMax; ++j, ++i )
	{
		if( i < m_nDrawCount )
		{
			continue;
		}
		
		LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( m_uServerPlayerId[nMyMuilti][j] );
		DWORD dwState;

		// 상태에 따라 색 변경

		DWORD dwColor = 0xff000000;
		if( i == m_nCurSelect )
			dwColor = 0xff6060ff; 
		
		int x = 0, nWidth = m_rectClient.Width();// - 1;
		
		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
		rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
		if( lpFriend->dwState == FRS_AUTOABSENT )
		{
			dwState = FRS_ABSENT;
		}
		else
		{
			if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				dwState = FRS_OFFLINE;
			else
				dwState = lpFriend->dwState;
		}

		CString string;
		if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
		{
			if( lpFriend->dwState == FRS_OFFLINEBLOCK )
			{
				string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
				dwColor = 0xff000000; 
			}
			else
			{
				string = lpFriend->szName;
			}
		}
		else
		{	
			string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
		}
		
		p2DRender->TextOut( x + 40, pt.y + 3, string, dwColor ); 				
		
		if( dwState == FRS_OFFLINE )
		{
			if( MAX_EXPERT <= lpFriend->nJob )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 66 + lpFriend->nJob - 6 ) + ( 8 * lpFriend->nSex ), &pVertices, 0xffff6464 );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffff6464 );
			}
		}
		else
		{
			if( MAX_EXPERT <= lpFriend->nJob )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + lpFriend->nJob - 6 ) + ( 8 * lpFriend->nSex ), &pVertices, 0xffffffff );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffffffff );
			}

			if( dwState != FRS_ONLINE )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
			}
			if( lpFriend->m_uIdofMulti != 100 )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
			}
		}
		pt.y += m_nFontHeight;// + 3;
	}

	// Differ Multi Draw
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == nMyMuilti )
			continue;

		for( int k = 0 ; k < m_nServerCount[j] && i < nMax ; ++k , ++i )
		{
			if( i < m_nDrawCount )
			{
				continue;
			}
			
			u_long uFriendId = m_uServerPlayerId[j][k];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			DWORD dwState;
			
			// 상태에 따라 색 변경
			DWORD dwColor = 0xff000000;
			
			if( i == m_nCurSelect )
				dwColor = 0xff6060ff; 
			
			int x = 0, nWidth = m_rectClient.Width();// - 1;
			CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
			rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 

			if( lpFriend->dwState == FRS_AUTOABSENT )
			{
				dwState = FRS_ABSENT;
			}
			else
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
					dwState = FRS_OFFLINE;
				else
					dwState = lpFriend->dwState;
			}
			CString string;
			if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				{
					string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
					dwColor = 0xff000000; 
				}
				else
				{
					string = lpFriend->szName;
				}
			}
			else
			{	
				string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
			}
			
			p2DRender->TextOut( x + 40, pt.y + 3, string, dwColor ); 				
			
			if( dwState == FRS_OFFLINE )
			{
				if( MAX_EXPERT <= lpFriend->nJob )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + lpFriend->nJob - 6 ) + ( 8 * lpFriend->nSex ), &pVertices, 0xffff6464 );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffff6464 );
				}
			}
			else
			{
				if( MAX_EXPERT <= lpFriend->nJob )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + lpFriend->nJob - 6 ) + ( 8 * lpFriend->nSex ), &pVertices, 0xffffffff );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffffffff );
				}
				if( dwState != FRS_ONLINE )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
				}
				if( lpFriend->m_uIdofMulti != 100 )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
				}
			}
			pt.y += m_nFontHeight;// + 3;
		}
	}
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}

#else // __SMEBUG_0831
void CWndFriendCtrl::OnDraw( C2DRender* p2DRender ) 
{
	
	if( NULL == g_pPlayer )
		return;
	
	CPoint pt( 3, 3 );
	
	CWndMessenger* pWndMessenger = (CWndMessenger*)GetWndBase( APP_MESSENGER_ );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	// Draw Max Count
	int nMax =  g_WndMng.m_Messenger.m_aFriend.size();
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	// 여기서 텍스트를 만들때 TEXTUREVERTEX2배열이 적어서 튕기므로 Draw를 하려면 이 값도 추가 해주어야 한다.
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 3 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;

	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();

#ifdef __S0610
	m_nDrawCount = 0;
	for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != g_WndMng.m_Messenger.m_aFriend.end(); i++, iter++, m_nDrawCount++ );
#else // __S0610
	for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != g_WndMng.m_Messenger.m_aFriend.end(); i++, iter++ );
#endif // __S0610
	
	BOOL bReturn = FALSE;
	memset( m_nServerCount, 0, sizeof( m_nServerCount ) );

	// 먼저 출력해야 될것을 추려냄
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end() ; ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;
		if( lpFriend->m_uIdofMulti == 100 || lpFriend->dwState == FRS_OFFLINE ) // 로그오프인것은 나중에 출력해야 되므로 모아둠
		{	
			m_uServerPlayerId[10][m_nServerCount[10]] = lpFriend->dwUserId;
			++m_nServerCount[10];
		}
		else	// 먼저 출력해야 될것 모아둠 // 멀티서버 별로 모아둠
		{
			m_uServerPlayerId[lpFriend->m_uIdofMulti-1][m_nServerCount[lpFriend->m_uIdofMulti-1]] = lpFriend->dwUserId;
			++m_nServerCount[lpFriend->m_uIdofMulti-1];
		}
	}		

	// 나의 멀티서버 먼저 출력
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] && i < nMax; ++j, ++i )
	{
		u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
		LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
		DWORD dwState;
		
		if( m_uParent == APP_MESSENGER_SEARCH )
		{
			if( m_nSex != 1000 )
			{
				if( m_nSelectServer != 0 && m_nSelectServer != lpFriend->m_uIdofMulti )
				{
					continue;
				}
				else
				{
					if( lpFriend->dwState == FRS_OFFLINE )
						continue;
				}
				
				if( m_nSex != 3 && lpFriend->nSex != m_nSex )
				{
					continue;
				}
				
				if( strlen( m_szId ) > 0)
				{
					if( 0 == strcmp( lpFriend->szName, m_szId ) )
					{
						bReturn = TRUE;					
					}
					else
					{
						continue;
					}
				}
			}
			else
			{
				continue;
			}
		}
		// 상태에 따라 색 변경

		DWORD dwColor = 0xff000000;
		
		if( i == m_nCurSelect )
			dwColor = 0xff6060ff; 
		
		int x = 0, nWidth = m_rectClient.Width();// - 1;
		
		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
		rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
		if( lpFriend->dwState == FRS_AUTOABSENT )
		{
			dwState = FRS_ABSENT;
		}
		else
		{
			if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				dwState = FRS_OFFLINE;
			else
				dwState = lpFriend->dwState;
		}
		if( 0 == strlen(lpFriend->szName ) )
		{
			p2DRender->TextOut( x + 20, pt.y + 5, "요청중입니다", 0xff00ff00 );
		}
		else
		{
			CString string;
			if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				{
					if( m_uParent == APP_MESSENGER_SEARCH )
					{
						if( !lpFriend->nSex )
						{
							string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXMALE ) );
						}
						else
						{
							string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXFEMALE ) );
						}
					}
					else
					{
						string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
					}
					dwColor = 0xff000000; 
				}
				else
				{
					if( m_uParent == APP_MESSENGER_SEARCH )
					{
						if( !lpFriend->nSex )
						{
							string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXMALE ) );
						}
						else
						{
							string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXFEMALE ) );
						}
					}
					else
					{
						string = lpFriend->szName;
					}
				}
			}
			else
			{	
				string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
			}
			
			p2DRender->TextOut( x + 40, pt.y + 3, string, dwColor ); 				
		}
		
		if( dwState == FRS_OFFLINE )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffff6464 );
		}
		else
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffffffff );
			if( dwState != FRS_ONLINE )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
			}
			if( lpFriend->m_uIdofMulti != 100 )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
			}
		}
		pt.y += m_nFontHeight;// + 3;

		if( bReturn )
			break;
	}

	// 다른 서버 출력
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}

		for( int k = 0 ; k < m_nServerCount[j] && i < nMax ; ++k , ++i )
		{
			u_long uFriendId = m_uServerPlayerId[j][k];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			DWORD dwState;
			
			if( m_uParent == APP_MESSENGER_SEARCH )
			{
				if( m_nSex != 1000 )
				{
					if( m_nSelectServer != 0 && m_nSelectServer != lpFriend->m_uIdofMulti )
					{
						continue;
					}
					else
					{
						if( lpFriend->dwState == FRS_OFFLINE )
							continue;
					}
					
					if( m_nSex != 3 && lpFriend->nSex != m_nSex )
					{
						continue;
					}
					
					if( strlen( m_szId ) > 0)
					{
						if( 0 == strcmp( lpFriend->szName, m_szId ) )
						{
							bReturn = TRUE;					
						}
						else
						{
							continue;
						}
					}
				}
				else
				{
					continue;
				}
			}
			// 상태에 따라 색 변경

			DWORD dwColor = 0xff000000;
			
			if( i == m_nCurSelect )
				dwColor = 0xff6060ff; 
			
			int x = 0, nWidth = m_rectClient.Width();// - 1;
			
			CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
			rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
			if( lpFriend->dwState == FRS_AUTOABSENT )
			{
				dwState = FRS_ABSENT;
			}
			else
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
					dwState = FRS_OFFLINE;
				else
					dwState = lpFriend->dwState;
			}
			if( 0 == strlen(lpFriend->szName ) )
			{
				p2DRender->TextOut( x + 20, pt.y + 5, "요청중입니다", 0xff00ff00 );
			}
			else
			{
				CString string;
				if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
				{
					if( lpFriend->dwState == FRS_OFFLINEBLOCK )
					{
						if( m_uParent == APP_MESSENGER_SEARCH )
						{
							if( !lpFriend->nSex )
							{
								string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXMALE ) );
							}
							else
							{
								string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXFEMALE ) );
							}
						}
						else
						{
							string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
						}
						dwColor = 0xff000000; 
					}
					else
					{
						if( m_uParent == APP_MESSENGER_SEARCH )
						{
							if( !lpFriend->nSex )
							{
								string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXMALE ) );
							}
							else
							{
								string.Format( "%s%s", lpFriend->szName, prj.GetText( TID_GAME_TOOLTIP_SUB_SEXFEMALE ) );
							}
						}
						else
						{
							string = lpFriend->szName;
						}
					}
				}
				else
				{	
					string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
				}
				
				p2DRender->TextOut( x + 40, pt.y + 3, string, dwColor ); 				
			}
			
			if( dwState == FRS_OFFLINE )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffff6464 );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + lpFriend->nJob + ( 6 * lpFriend->nSex ), &pVertices, 0xffffffff );
				if( dwState != FRS_ONLINE )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
				}
				if( lpFriend->m_uIdofMulti != 100 )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
				}
			}
			pt.y += m_nFontHeight;// + 3;

			if( bReturn )
				break;
		}
	}

	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}
#endif // __SMEBUG_0831

void CWndFriendCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;

	CPoint pt( 3, 3 );
	CRect rect;

#ifdef __S0610
	int count = m_nDrawCount;	
#else
	pt.y -= (m_nFontHeight) * m_wndScrollBar.GetScrollPos();
	int count = 0;	
#endif // __S0610

	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			m_nCurSelect = count;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = count;
				return;
			}
			pt.y += m_nFontHeight;
		}
	}
	g_WorldMng()->SetObjFocus(NULL);
}
void CWndFriendCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndFriendCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
#ifdef __ONLINE
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
	if( nSelect != -1 )
	{
		if( lpFriend->dwState != FRS_OFFLINE && lpFriend->dwState != FRS_BLOCK && lpFriend->dwState != FRS_OFFLINEBLOCK )
		{
			m_nCurSelect = nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );
		}
		else
		{
			CString szMessage;
			if( lpFriend->dwState == FRS_OFFLINE )
				szMessage = prj.GetText(TID_GAME_NOTLOGIN);                               //szMessage += "님은 접속되어 있지 않습니다";
			else
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), lpFriend->szName );  //szMessage += "님은 차단되어 있어 메세지를 보낼수 없습니다";
			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );
			
		}
	}	
#else
	CWndMessage* pWndMessage = g_WndMng.OpenMessage( "test" );
#endif
}

#ifdef __SMEBUG_0831

void CWndFriendCtrl::GetSelectFriend( int SelectCount, LPFRIEND* lppFriend )
{
	int count = 0;
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		}
		
		if( SelectCount == count )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
			{
				*lppFriend = lpFriend;
				return;
			}
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( count < m_nDrawCount )
			{
				continue;
			}
			
			if( SelectCount == count )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
				{
					*lppFriend = lpFriend;
					return;
				}
			}
		}
	}
	
	*lppFriend = NULL;
}

u_long CWndFriendCtrl::GetSelectId( int SelectCount )
{
	int count = 0;
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		} 
		if( SelectCount == count )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				return lpFriend->dwUserId;
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( j == g_Neuz.m_uIdofMulti -1 )
			{
				continue;
			}
			if( SelectCount == count )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					return lpFriend->dwUserId;
			}
		}
	}
	return -1;
}

int CWndFriendCtrl::GetSelect( CPoint point, LPFRIEND* lppFriend )
{
	CPoint pt( 3, 3 );
	CRect rect;
	int count = 0;	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		} 
		
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				*lppFriend = lpFriend;
			return count;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( count < m_nDrawCount )
			{
				continue;
			} 
			
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					*lppFriend = lpFriend;
				return count;
			}
			pt.y += m_nFontHeight;
		}
	}
	
	if( lppFriend )
		*lppFriend = NULL;
	return -1;
}
#else // __SMEBUG_0831
void CWndFriendCtrl::GetSelectFriend( int SelectCount, LPFRIEND* lppFriend )
{
#ifdef __S0610
	int count = m_nDrawCount;
#else // __S0610
	int count  = 0;
#endif // __S0610
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( SelectCount == count )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
			{
				*lppFriend = lpFriend;
				return;
			}
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			u_long uFriendId = m_uServerPlayerId[j][k];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( SelectCount == count )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
				{
					*lppFriend = lpFriend;
					return;
				}
			}
		}
	}
	
	*lppFriend = NULL;
}


u_long CWndFriendCtrl::GetSelectId( int SelectCount )
{
#ifdef __S0610
	int count = m_nDrawCount;
#else // __S0610
	int count  = 0;
#endif // __S0610
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( SelectCount == count )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				return lpFriend->dwUserId;
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( SelectCount == count )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					return lpFriend->dwUserId;
			}
		}
	}
	return -1;
}
int CWndFriendCtrl::GetSelect( CPoint point, LPFRIEND* lppFriend )
{
	CPoint pt( 3, 3 );
	CRect rect;
#ifdef __S0610
	int count = m_nDrawCount;	
#else
	pt.y -= (m_nFontHeight) * m_wndScrollBar.GetScrollPos();
	int count = 0;	
#endif // __S0610
	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				*lppFriend = lpFriend;
			return count;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					*lppFriend = lpFriend;
				return count;
			}
			pt.y += m_nFontHeight;
		}
	}
	
	if( lppFriend )
		*lppFriend = NULL;
	return -1;
}

#endif // __SMEBUG_0831

BOOL CWndFriendCtrl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	switch( nID )
	{
	case 0:		// 메시지

		{
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );				
			}
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
	case 3: // 이동
		{
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", lpFriend->szName );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();			
			}
		}
		break;
	case 4:	// 극단 초청
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				if( g_Party.m_nKindTroup == 0 )	// 단막극단일때는 포인트가 필요없이 초대할수 있다
				{
					if( g_Party.m_aMember[0].m_uPlayerId == 0 || g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
					{
						int nMaxPartyMember = MAX_PTMEMBER_SIZE;
						CString szMessageBuf;
					
						if( g_Party.GetLevel() <= 5 )
						{
							nMaxPartyMember	= 3;
							szMessageBuf = prj.GetText(TID_GAME_FULLPARTY1);
						}
						else
						{
							nMaxPartyMember	= 4;
							szMessageBuf = prj.GetText(TID_GAME_FULLPARTY2);
						}
	
						if( nMaxPartyMember >= g_Party.m_nSizeofMember + 1 )
						{
							g_DPlay.SendPartyMemberRequest( g_pPlayer, uidPlayer, FALSE );
						}
						else
						{
							g_WndMng.PutString( szMessageBuf, NULL, prj.GetTextColor(TID_GAME_FULLPARTY1) );
						}
					}
					else
					{
						// 단장이 아니거나 단원이 꽉차있습니다.
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0007) ) ); // 단장이 아니거나 꽉차서 참여시킬수가 없습니다
					}
				}
				else // 순회극단일대 스킬을 이용하여 초대

				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0024) ) ); // 순회극단일때는 스카웃스킬을 이용해야만 추가시킬수가 있습니다
				}				
			}
		}
		break;
	case 5: // 컴퍼니 초청
		{

		}
		break;
	case 6: // 쪽지 보내기
		{
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote = new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, lpFriend->szName );
				g_WndMng.m_pWndMessageNote->m_dwUserId = lpFriend->dwUserId;
				g_WndMng.m_pWndMessageNote->Initialize();		
			}
		}
		break;
	}
	return -1; 
} 

void CWndFriendCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
}

void CWndFriendCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
	if( nSelect != -1 )
	{
		m_nCurSelect = nSelect;
		ClientToScreen( &point );
		m_menu.DeleteAllMenu();
		if( lpFriend->dwState != FRS_OFFLINE && lpFriend->dwState != FRS_BLOCK && lpFriend->dwState != FRS_OFFLINEBLOCK )
			m_menu.AppendMenu( 0, 0 ,_T( prj.GetText(TID_APP_MESSAGE) ) );
		if( lpFriend->dwState == FRS_BLOCK || lpFriend->dwState == FRS_OFFLINEBLOCK )
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText(TID_FRS_BLOCKRESTORE) ) );
		else
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText(TID_FRS_BLOCK) ) );
		m_menu.AppendMenu( 0, 2 ,_T( prj.GetText(TID_FRS_DELETE) ) );
		if( lpFriend->dwState != FRS_OFFLINE && lpFriend->dwState != FRS_OFFLINEBLOCK )
		{					
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				m_menu.AppendMenu( 0, 3 ,_T( prj.GetText(TID_FRS_MOVE2) ) );	
			if( lpFriend->dwState != FRS_BLOCK )
			{
				if( g_Party.IsMember( lpFriend->dwUserId ) == FALSE )
					m_menu.AppendMenu( 0, 4 ,_T( prj.GetText(TID_MMI_INVITE_PARTY) ) );
			}
		}
		if( lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_BLOCK || lpFriend->dwState == FRS_OFFLINEBLOCK )
			m_menu.AppendMenu( 0, 6 , _T( prj.GetText(TID_GAME_TAGSEND) ) );
		m_menu.Move( point );
		m_menu.SetVisible( TRUE );
		m_menu.SetFocus();
	}
}
void CWndFriendCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndFriendCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndFriendCtrl::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = g_WndMng.m_Messenger.m_aFriend.size();//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndFriendCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = g_WndMng.m_Messenger.m_aFriend.size();//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndFriendCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndFriendCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = 20;//GetFontHeight( &g_2DRender );
		nPage = GetClientRect().Height() / ( m_nFontHeight );
		nRange = g_WndMng.m_Messenger.m_aFriend.size();
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}
BOOL CWndFriendCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndFriendCtrl::SetItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndFriendCtrl::InsertItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndFriendCtrl::GetItemCount() const
{
	return 1;//m_aItems.GetSize();
}

void CWndFriendCtrl::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndFriendCtrl::GetDrawCount( void )
{
	int nMax =  g_WndMng.m_Messenger.m_aFriend.size();
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}