// WndWebBox.cpp: implementation of the CWndWebBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Neuz/WebBox.h"
#include "WndWebBox.h"
#include "WndHelperWebBox.h"

#define TASKBAR_HEIGHT 48

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndWebBox::CWndWebBox()
{
	SetPutRegInfo( FALSE );
}

CWndWebBox::~CWndWebBox()
{
	CRect rect = g_Neuz.GetDeviceRect();
	rect.top = rect.bottom - TASKBAR_HEIGHT;
	CWndTaskBar::m_pWndRoot->m_rectLayout = m_rectOldBackup;
	SetWndRect( m_rectOldBackup );
	
	ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_HIDE );
	CWebBox::GetInstance()->Close();	
}
BOOL CWndWebBox::Initialize( CWndBase* pWndParent, DWORD nType )
{
	//gmpbigsun( 20110707 ) : 특정경우 게임을 최소화 시키고 익스플로러 가동
	if( g_xFlyffConfig->GetMainLanguage() == LANG_HK )
	{
	//	ShowWindow( g_Neuz.GetSafeHwnd( ), SW_SHOWMINIMIZED );
		ShellExecute(NULL, "open", CWebBox::GetInstance()->GetAddress(), NULL, NULL, SW_SHOW );
	//	ShellExecute(NULL, "open", "http://flyff.gamecyber.net/ItemMall/", NULL, NULL, SW_SHOW );
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		Destroy();
		return FALSE;
	}

	if(g_WndMng.GetWndBase( APP_TRADE )) 
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		Destroy();
		return FALSE;
	}

	// 이벤트 아레나 캐시샵 불가
	CWorld* pWorld = g_pPlayer->GetWorld();
	if( pWorld != NULL && pWorld->IsEventArena() )
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		g_WndMng.PutString( prj.GetText(TID_GAME_NOTCHARGEDSERVER), NULL, prj.GetTextColor(TID_GAME_NOTCHARGEDSERVER) );
		Destroy();
		return FALSE;
	}

#if defined(__INTERNALSERVER)
	m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
	g_WndMng.PutString( prj.GetText(TID_GAME_NOTCHARGEDSERVER), NULL, prj.GetTextColor(TID_GAME_NOTCHARGEDSERVER) );
	Destroy();
	return FALSE;
#else
	if( _tcscmp(g_xFlyffConfig->GetCertifierAddr(), "182.162.85.14") == 0)
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		g_WndMng.PutString( prj.GetText(TID_GAME_NOTCHARGEDSERVER), NULL, prj.GetTextColor(TID_GAME_NOTCHARGEDSERVER) );
		Destroy();
		return FALSE;
	}
#endif //defined(__INTERNALSERVER)


#ifdef __MAINSERVER
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		Destroy();
		return FALSE;
	}
#endif	// __MAINSERVER

	CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );
	if( pWndHelperWebBox )
		pWndHelperWebBox->Destroy();

	CWebBox::GetInstance()->SetItemShop( TRUE );
	
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndWebBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WTBID_CLOSE: // 종료

		CRect rect = g_Neuz.GetDeviceRect();
		rect.top = rect.bottom - TASKBAR_HEIGHT;
		CWndTaskBar::m_pWndRoot->m_rectLayout = m_rectOldBackup;
		SetWndRect( m_rectOldBackup );

		ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_HIDE );		
		CWebBox::GetInstance()->Close();
		Destroy( );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/*
void CWndWebBox::OnDestroy( void )
{
}
*/	
void CWndWebBox::OnDraw( C2DRender* p2DRender )
{
}
void CWndWebBox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DelWndStyle(WBS_MOVE);
	AddWndStyle(WBS_TOPMOST);
	
	m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;

	CRect rect = g_Neuz.GetDeviceRect();
	CWndTaskBar::m_pWndRoot->m_rectLayout = rect;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_CUSTOM1 );

	DWORD FrmW = 0, FrmH = 0, CapH = 0;

	if(  g_Neuz.IsWindow() )
	{
		FrmW	= GetSystemMetrics( SM_CXSIZEFRAME );
		FrmH	= GetSystemMetrics( SM_CYSIZEFRAME );
		CapH	= GetSystemMetrics( SM_CYCAPTION );
//		MenuH	= bMenuNow ? GetSystemMetrics( SM_CYMENU ) : 0;
		
//		dwWinW	= clientW + ((FrmW-1) * 2);
//		dwWinH	= clientH + ((FrmH-1) * 2) + MenuH + CapH; 
		
	}

	pCustom->rect.left -= FrmW;
	pCustom->rect.top  += CapH+FrmH;
	
	CWebBox::GetInstance()->Set_Rect(pCustom->rect);
	
//	SetVisible(FALSE);

	MoveParentCenter();
}

BOOL CWndWebBox::Process()
{
	return TRUE;
}

BOOL CWndWebBox::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndWebBox::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndWebBox::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndWebBox::OnLButtonDown( UINT nFlags, CPoint point )
{
}
HRESULT CWndWebBox::RestoreDeviceObjects()
{
	CWebBox* pWebBox	= CWebBox::GetInstance();
	
	if( pWebBox && pWebBox->m_hDlg )
		::SendMessage( pWebBox->m_hDlg, WM_INITDIALOG, 0, 0 );

	return CWndNeuz::RestoreDeviceObjects();
}

/*--------------------------------------------------------------------------------*/
CWndWebBox2::CWndWebBox2()
{
	SetPutRegInfo( FALSE );
}

CWndWebBox2::~CWndWebBox2()
{
	CRect rect = g_Neuz.GetDeviceRect();
	rect.top = rect.bottom - TASKBAR_HEIGHT;
	CWndTaskBar::m_pWndRoot->m_rectLayout = m_rectOldBackup;
	SetWndRect( m_rectOldBackup );
	
	ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_HIDE );
	CWebBox::GetInstance()->Close();	
}
BOOL CWndWebBox2::Initialize( CWndBase* pWndParent, DWORD nType )
{	
	CMover* pPlayer	= CMover::GetActiveMover();
/*
	if( pPlayer == NULL || pPlayer->GetLevel() < 16 )
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		g_WndMng.PutString( prj.GetText(TID_TIP_QOODO_LEVEL), NULL, prj.GetTextColor(TID_TIP_QOODO_LEVEL) );
		Destroy();
		return FALSE;
	}
*/
	CWndWebBox* pWndWebBox = (CWndWebBox*)g_WndMng.GetApplet( APP_WEBBOX );
	if(pWndWebBox)
		pWndWebBox->Destroy();

	CWebBox::GetInstance()->SetItemShop( FALSE );
	
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX2, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndWebBox2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WTBID_CLOSE: // 종료

		CRect rect = g_Neuz.GetDeviceRect();
		rect.top = rect.bottom - TASKBAR_HEIGHT;
		CWndTaskBar::m_pWndRoot->m_rectLayout = m_rectOldBackup;
		SetWndRect( m_rectOldBackup );

		ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_HIDE );		
		CWebBox::GetInstance()->Close();
		Destroy( );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/*
void CWndWebBox2::OnDestroy( void )
{
}
*/	
void CWndWebBox2::OnDraw( C2DRender* p2DRender )
{
}
void CWndWebBox2::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	DelWndStyle(WBS_MOVE);
	AddWndStyle(WBS_TOPMOST);
	
	m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;

	CRect rect = g_Neuz.GetDeviceRect();
	CWndTaskBar::m_pWndRoot->m_rectLayout = rect;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_CUSTOM1 );

	DWORD FrmW = 0, FrmH = 0, CapH = 0;

	if(  g_Neuz.IsWindow() )
	{
		FrmW	= GetSystemMetrics( SM_CXSIZEFRAME );
		FrmH	= GetSystemMetrics( SM_CYSIZEFRAME );
		CapH	= GetSystemMetrics( SM_CYCAPTION );
//		MenuH	= bMenuNow ? GetSystemMetrics( SM_CYMENU ) : 0;
		
//		dwWinW	= clientW + ((FrmW-1) * 2);
//		dwWinH	= clientH + ((FrmH-1) * 2) + MenuH + CapH; 
		
	}

	pCustom->rect.left -= FrmW;
	pCustom->rect.top  += CapH+FrmH;
	
	CWebBox::GetInstance()->Set_Rect(pCustom->rect);
	
//	SetVisible(FALSE);

	MoveParentCenter();
}
BOOL CWndWebBox2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndWebBox2::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndWebBox2::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndWebBox2::OnLButtonDown( UINT nFlags, CPoint point )
{
}
HRESULT CWndWebBox2::RestoreDeviceObjects()
{
	CWebBox* pWebBox	= CWebBox::GetInstance();
	
	if( pWebBox && pWebBox->m_hDlg )
		::SendMessage( pWebBox->m_hDlg, WM_INITDIALOG, 0, 0 );

	return CWndNeuz::RestoreDeviceObjects();
}