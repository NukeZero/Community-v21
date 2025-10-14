#include "stdafx.h"
#include "WndCompany.h"
#include "company.h"
#include "dpclient.h"

extern	CCompanyMng		g_CompanyMng;
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_COMPANY - 컴퍼니(길드)
  CtrlId : WIDC_STATIC1 - 컴퍼니명칭 :
  CtrlId : WIDC_STATIC2 - 컴퍼니레벨 :
  CtrlId : WIDC_STATIC3 - 컴퍼니킹핀 :
  CtrlId : WIDC_STATIC4 - 현재/최대인원 :
  CtrlId : WIDC_STATIC5 - 컴퍼니경험치 :
  CtrlId : WIDC_STATIC6 - 컴퍼니지원금 :
  CtrlId : WIDC_NAME - 
  CtrlId : WIDC_LEVEL - 000
  CtrlId : WIDC_KINGPIN - 
  CtrlId : WIDC_CURRENTMEMBER - 000
  CtrlId : WIDC_STATIC7 - /
  CtrlId : WIDC_MAXMEMBER - 000
  CtrlId : WIDC_EXP - 0.00
  CtrlId : WIDC_STATIC8 - 
  CtrlId : WIDC_PENYA - 0.00
  CtrlId : WIDC_STATIC9 - 
  CtrlId : WIDC_STATIC10 - 컴퍼니종류 :
  CtrlId : WIDC_MEMBERLIST - 
  CtrlId : WIDC_LEAVE - 탈퇴
  CtrlId : WIDC_STATIC11 - 일반
****************************************************/

#ifdef __J1216
CWndCompany::CWndCompany()
{ 
} 

CWndCompany::~CWndCompany() 
{ 

} 

void CWndCompany::OnDraw( C2DRender* p2DRender ) 
{ 

} 

void CWndCompany::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
#if 0

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
	if( pWndListBox )
	{
		pWndListBox->m_nFontColor	= 0xff000000;
		pWndListBox->m_nSelectColor	= 0xff0000ff;
	}

	UpdateData();
#endif// 0
} 

BOOL CWndCompany::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMPANY, 0, CPoint( 0, 0 ), pWndParent );
	return 0;
} 

BOOL CWndCompany::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndCompany::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndCompany::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndCompany::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndCompany::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#if 0
	if( nID == WIDC_LEAVE )
	{
		CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
		if( pWndListBox )
		{
			int nIndex	= pWndListBox->GetCurSel();
			if( nIndex >= 0 && nIndex < pWndListBox->GetCount() )
			{
				PCOMPANYMEMBER pData	= (PCOMPANYMEMBER)pWndListBox->GetItemDataPtr( nIndex );
				g_DPlay.SendRemoveCompanyMember( g_pPlayer->m_idPlayer, pData->idPlayer );
			}
		}
	}
#endif	// 0
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCompany::UpdateData( void )
{
#if 0
	Clear();

	if( !g_pPlayer )	return;

	CCompany* pCompany	= g_CompanyMng.GetCompany( g_pPlayer->m_idCompany );
	if( pCompany )
	{
		LPCSTR lpszPlayer	= NULL;

		CWndStatic* pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_NAME );
		if( pWndStatic )
			pWndStatic->SetTitle( CString( pCompany->m_szCompany ) );
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_KINGPIN );
		if( pWndStatic )
		{
			lpszPlayer	= prj.GetPlayerString( pCompany->m_idLeader );
			if( lpszPlayer )
			{
				pWndStatic->SetTitle( CString( lpszPlayer ) );
			}
			else
			{
				g_DPlay.SendQueryPlayerString( pCompany->m_idLeader, QPS_COMPANY_LEADER );
			}
		}
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_MAXMEMBER );	// max
		if( pWndStatic )
		{
			CString strMax;
			strMax.Format( "%d", MAX_COMPANY_MEMBER );
			pWndStatic->SetTitle( strMax );
		}
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_CURRENTMEMBER );	// current
		if( pWndStatic )
		{
			CString strSize;
			strSize.Format( "%d", pCompany->GetSize() );
			pWndStatic->SetTitle( strSize );
		}			

		CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
		if( pWndListBox )
		{
			CCompanyMember* pMember;
			for( map<u_long, CCompanyMember*>::iterator i = pCompany->m_mapMemberPtr.begin();
				i != pCompany->m_mapMemberPtr.end(); ++i )
			{
				pMember	= i->second;
				lpszPlayer	= prj.GetPlayerString( pMember->m_idPlayer );
				if( NULL == lpszPlayer )
				{
					int nIndex	= pWndListBox->AddString( "" );
					PCOMPANYMEMBER pData	= new COMPANYMEMBER;
					pData->idPlayer	= pMember->m_idPlayer;
					pWndListBox->SetItemDataPtr( nIndex, (void*)pData );
					g_DPlay.SendQueryPlayerString( pMember->m_idPlayer, QPS_COMPANY_MEMBER );
				}
				else
				{
					int nIndex	= pWndListBox->AddString( lpszPlayer );
					PCOMPANYMEMBER pData	= new COMPANYMEMBER;
					pData->idPlayer	= pMember->m_idPlayer;
					pWndListBox->SetItemDataPtr( nIndex, (void*)pData );
				}
			}
		}
	}
#endif	// 0
}

void CWndCompany::Clear()
{
#if 0
	CWndStatic* pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_NAME );
	if( pWndStatic )
		pWndStatic->SetTitle( CString( "" ) );
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_KINGPIN );
	if( pWndStatic )
		pWndStatic->SetTitle( CString( "" ) );
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_MAXMEMBER );	// max
	if( pWndStatic )
		pWndStatic->SetTitle( CString( "0" ) );
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_CURRENTMEMBER );	// current
	if( pWndStatic )
		pWndStatic->SetTitle( "0" );
	CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
	if( pWndListBox )
	{
		int nSize	= pWndListBox->GetCount();
		for( int i = 0; i < nSize; i++ )
			delete (PCOMPANYMEMBER)pWndListBox->GetItemDataPtr( i );
		pWndListBox->ResetContent();
	}
#endif	// 0
}

void CWndCompany::AddCompanyMember( u_long idMember )
{
#if 0
	CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
	if( pWndListBox )
	{

		LPCSTR lpszPlayer	= prj.GetPlayerString( idMember );
		if( NULL == lpszPlayer )
		{
			int nIndex	= pWndListBox->AddString( "" );
			PCOMPANYMEMBER pData	= new COMPANYMEMBER;
			pData->idPlayer	= idMember;
			pWndListBox->SetItemDataPtr( nIndex, (void*)pData );
			g_DPlay.SendQueryPlayerString( idMember, QPS_COMPANY_MEMBER );
		}
		else
		{
			int nIndex	= pWndListBox->AddString( lpszPlayer );
			PCOMPANYMEMBER pData	= new COMPANYMEMBER;
			pData->idPlayer	= idMember;
			pWndListBox->SetItemDataPtr( nIndex, (void*)pData );
		}
	}
#endif	// 0
}

void CWndCompany::RemoveCompanyMember( u_long idMember )
{
#if 0
	CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_MEMBERLIST );
	if( pWndListBox )
	{
		int nSize	= pWndListBox->GetCount();
		for( int i = 0; i < nSize; i++ )
		{
			PCOMPANYMEMBER pData	= (PCOMPANYMEMBER)pWndListBox->GetItemDataPtr( i );
			if( pData->idPlayer == idMember )
			{
				delete pData;
				pWndListBox->DeleteString( i );
				break;
			}
		}
	}
#endif	// 0
}
#endif	// __J1216
