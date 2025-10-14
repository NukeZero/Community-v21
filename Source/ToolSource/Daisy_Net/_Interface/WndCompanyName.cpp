#include "stdafx.h"
#include "WndCompanyName.h"
#include "company.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __J1216
CWndCompanyName::CWndCompanyName() 
{
}
 
CWndCompanyName::~CWndCompanyName() 
{ 
} 

void CWndCompanyName::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndCompanyName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndCompanyName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMPANY_NAME, 0, CPoint( 0, 0 ), pWndParent );
	return 0;
} 

BOOL CWndCompanyName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndCompanyName::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndCompanyName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndCompanyName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}
 
BOOL CWndCompanyName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#if 0
	if( nID == WIDC_OK )
	{
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT_COMPANY_NAME );
		CString strCompany	= pWndEdit->GetString();
		strCompany.TrimLeft();	strCompany.TrimRight();
/*
		if( strCompany.GetLength() < MAX_COMPANY_NAME )
		{
			g_DPlay.SendCreateCompany( (LPCSTR)strCompany );
			Destroy();
		}
		else
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_COMOVERNAME ), NULL, prj.GetTextColor( TID_GAME_COMOVERNAME ) );
		}
*/
		if( strCompany.IsEmpty() == FALSE )
		{
			CHAR c	= strCompany.GetAt( 0 );
			if( strCompany.GetLength() < 3 || strCompany.GetLength() >= 16 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
//				g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
				return TRUE;
			}
			else if( isdigit( c ) )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
//				g_WndMng.OpenMessageBox( _T( "명칭에 첫글자를 숫자로 사용할 수 없습니다." ) );
				return TRUE;
			}
			else
			{
				for( int i = 0; i < strCompany.GetLength(); i++ )
				{
					c	= strCompany.GetAt( i );
					if( IsDBCSLeadByte( c ) == TRUE )
					{
						CHAR c2	= strCompany.GetAt( ++i );
						DWORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
						if( IsHangul( word ) == FALSE ) 
						{
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
//							g_WndMng.OpenMessageBox( _T( "명칭에 한자나 특수 문자를 사용할 수 없습니다." ) );
							return TRUE;
						}
						else if( isalnum( c ) == FALSE || iscntrl( c ) )
						{
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
//							g_WndMng.OpenMessageBox( _T( "명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다." ) );
							return TRUE;
						}
					}
				}
			}
#ifdef __ONLINE
			g_DPlay.SendCreateCompany( (LPCSTR)strCompany );
#endif	// __ONLINE
			Destroy();
		}
		else
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
//			g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
			return TRUE;
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
#endif	// 0
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#endif	// __J1216