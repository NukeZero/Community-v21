#include "stdafx.h"
#include "WndCompanyDestroy.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __J1216
CWndCompanyDestroy::CWndCompanyDestroy() 
{ 
} 

CWndCompanyDestroy::~CWndCompanyDestroy() 
{ 
} 

void CWndCompanyDestroy::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndCompanyDestroy::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndCompanyDestroy::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMPANY_DESTROY, 0, CPoint( 0, 0 ), pWndParent );
	return 0;
} 

BOOL CWndCompanyDestroy::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndCompanyDestroy::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndCompanyDestroy::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndCompanyDestroy::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

BOOL CWndCompanyDestroy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
#if 0
	if( nID == WIDC_OK )
	{
		g_DPlay.SendDestroyCompany( g_pPlayer->m_idPlayer );
		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
#endif	// 0
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#endif	// __J1216