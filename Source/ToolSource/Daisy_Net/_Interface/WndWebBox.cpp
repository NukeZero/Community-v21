// WndWebBox.cpp: implementation of the CWndWebBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "definetext.h"
#include "resdata.h"
#include "WebBox.h"
#include "WndWebBox.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	if( g_pPlayer && g_pPlayer->IsDie() )	// 2006.01.16 raiders - 죽은 상태에서 유료상점을 이용 불가능하게 수정 
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		Destroy();
		return FALSE;
	}

#ifndef __WEB_GM_HELP1201
#if defined(__INTERNALSERVER) || defined(__TESTSERVER)
	if( g_Neuz.m_bDeveloper == FALSE )
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		g_WndMng.PutString( prj.GetText(TID_GAME_NOTCHARGEDSERVER), NULL, prj.GetTextColor(TID_GAME_NOTCHARGEDSERVER) );
		Destroy();
		return FALSE;
	}
#endif //defined(__INTERNALSERVER) || defined(__TESTSERVER)
#endif	// __WEB_GM_HELP1201

#ifdef __MAINSERVER
#ifdef __SCHOOL0701
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
		m_rectOldBackup = CWndTaskBar::m_pWndRoot->m_rectLayout;
		Destroy();
		return FALSE;
	}
#endif	// __SCHOOL0701
#endif	// __MAINSERVER
	
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX, 0, CPoint( 0, 0 ), pWndParent );
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
	
#ifdef __TAIWAN_WEBBOX
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
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX2, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndWebBox2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WTBID_CLOSE:

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
	}

	pCustom->rect.left -= FrmW;
	pCustom->rect.top  += CapH+FrmH;
	
	CWebBox::GetInstance()->Set_Rect(pCustom->rect);

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
		::SendMessage( pWebBox->m_hDlg, WM_INITDIALOG, 0, 1 );

	return CWndNeuz::RestoreDeviceObjects();
}
#endif	// __TAIWAN_WEBBOX