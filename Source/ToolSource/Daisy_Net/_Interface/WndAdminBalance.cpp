#include "stdafx.h"
#include "AppDefine.h"
#include "WndAdminBalance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_ADMIN_BALANCE_GRAP - 발란스 그래프
  CtrlId : WIDC_TABCTRL1 - 
  CtrlId : WIDC_CLOSE - Close
****************************************************/

CWndAdminBalance::CWndAdminBalance() 
{ 
	SetPutRegInfo( FALSE );	
} 
CWndAdminBalance::~CWndAdminBalance() 
{ 
} 
void CWndAdminBalance::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndAdminBalance::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	m_wndAdminBalanceExp.Create( WBS_CHILDFRAME | WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_ADMIN_BALANCE_EXP );
	//m_wndPartySkill.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, pWndTabCtrl, APP_PARTY_SKILL );
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = "경험치";
	tabTabItem.pWndBase = &m_wndAdminBalanceExp;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	//pWndTabCtrl->InsertTexture( 0 ,MakePath( DIR_THEME, "WndStatusTab1.bmp" ) );
	
	//tabTabItem.pszText = "스킬";
	//tabTabItem.pWndBase = &m_wndPartySkill;
	//pWndTabCtrl->InsertItem( 1, &tabTabItem );
	//pWndTabCtrl->InsertTexture( 1 ,MakePath( DIR_THEME, "WndStatusTab2.bmp" ) );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAdminBalance::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADMIN_BALANCE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndAdminBalance::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndAdminBalance::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndAdminBalance::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndAdminBalance::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndAdminBalance::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndAdminBalance::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
/****************************************************
  WndId : APP_ADMIN_BALANCE_EXP - Applet
****************************************************/

CWndAdminBalanceExp::CWndAdminBalanceExp() 
{ 
} 
CWndAdminBalanceExp::~CWndAdminBalanceExp() 
{ 
} 
void CWndAdminBalanceExp::OnDraw( C2DRender* p2DRender ) 
{ 
#if 0
	CRect rect = GetClientRect();
	int nWidth = rect.Width();//470;
	int nHeight = rect.Height() - 3;//210;
	int x = 10,  y = 10;
	p2DRender->RenderLine( CPoint( x,  0 ), CPoint( x, 0 + nHeight ), 0xff000000, 0xff000000 );
	p2DRender->RenderLine( CPoint( x, nHeight ), CPoint( x + nWidth, nHeight ), 0xff000000, 0xff000000 );

	int nLevelMax = 15;
	int nLevelItv = nHeight / nLevelMax;
	CPoint pt = 0, ptOld = 0;

	EXPCHARACTER* pExpMax = &prj.m_aExpCharacter[ nLevelMax ];
	for( int i = 0; i < nLevelMax; i++ )
	{
		if( ( i % 10 ) == 0 )
			p2DRender->RenderLine( CPoint( x, nHeight - i * nLevelItv ), CPoint( x - 10, nHeight - i * nLevelItv ), 0xff000000, 0xff000000 );
		else
			p2DRender->RenderLine( CPoint( x, nHeight - i * nLevelItv ), CPoint( x - 5, nHeight - i * nLevelItv ), 0xff000000, 0xff000000 );
		/*
		if( ( i % 10 ) == 0 )
			p2DRender->RenderLine( CPoint( 20 + i * 3,  210 ), CPoint( 20 + i * 3, 230 ), 0xff000000, 0xff000000 );
		else
			p2DRender->RenderLine( CPoint( 20 + i * 3,  215 ), CPoint( 20 + i * 3, 225 ), 0xff000000, 0xff000000 );
			*/
		EXPCHARACTER* pExpCur = &prj.m_aExpCharacter[ i + 1 ];

		// expMax : expCur = nWidth : x
		
		int nPos = (double)( pExpCur->dwExp1 ) * nWidth / ( pExpMax->dwExp1 );

		pt = CPoint( x + nPos, nHeight - i * nLevelItv );
		if( i )
			p2DRender->RenderLine( ptOld, pt, 0xff646464 );
		p2DRender->RenderFillRect( CRect( pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2 ), 0xff0000ff );
		ptOld = pt;

	}
#endif	
} 
void CWndAdminBalanceExp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAdminBalanceExp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADMIN_BALANCE_EXP, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndAdminBalanceExp::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndAdminBalanceExp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndAdminBalanceExp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndAdminBalanceExp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndAdminBalanceExp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndAdminBalanceExp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

