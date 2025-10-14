#include "stdafx.h"
#include "WndCompanyInvite.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __J1216
CWndCompanyInvite::CWndCompanyInvite( CCompany* pCompany )
{ 
	m_idCompany	= pCompany->m_idCompany;
	strcpy( m_szCompany, pCompany->m_szCompany );
	m_idCmdr	= pCompany->m_idLeader;
	LPCSTR lpszPlayer	= prj.GetPlayerString( m_idCmdr );
	*m_szCmdr	= '\0';
	if( lpszPlayer )
		strcpy( m_szCmdr, lpszPlayer );
} 

CWndCompanyInvite::~CWndCompanyInvite() 
{

}
 
void CWndCompanyInvite::OnDraw( C2DRender* p2DRender ) 
{ 
}

void CWndCompanyInvite::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
#if 0
	CWndStatic* pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CString str;
	str.Format( _T( "%s 컴퍼니로부터 초청을 받았습니다. 참여하시겠습니까?" ), m_szCompany );
	pWndStatic->SetTitle( str );
#endif	// 0
} 

BOOL CWndCompanyInvite::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMPANY_INVITE, 0, CPoint( 0, 0 ), pWndParent );
	return 0;
} 

BOOL CWndCompanyInvite::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndCompanyInvite::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndCompanyInvite::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

}

void CWndCompanyInvite::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	
}

BOOL CWndCompanyInvite::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
#if 0
	if( nID == WIDC_YES )
	{
		g_DPlay.SendAddCompanyMember( m_idCmdr, g_pPlayer->m_idPlayer );
	}
	else
	{
		g_DPlay.SendIgnoreCompanyInvite( m_idCmdr );
	}
	Destroy( FALSE );
#endif	// 0
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#endif	// __J1216