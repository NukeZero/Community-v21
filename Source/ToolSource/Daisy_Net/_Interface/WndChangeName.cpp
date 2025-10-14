#include "stdafx.h"
#ifdef __J0519
#include "resData.h"
#include "WndChangeName.h"
#include "dpclient.h"
#include "definetext.h"
extern		CDPClient		g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
  WndId : APP_CHANGENAME
  CtrlId : WIDC_STATIC1
  CtrlId : WIDC_OK 
  CtrlId : WIDC_CANCEL 
  CtrlId : WIDC_EDIT_CHANGENAME 
*/

CWndChangeName::CWndChangeName() 
{ 
	
} 

CWndChangeName::~CWndChangeName() 
{ 

} 

void CWndChangeName::OnDraw( C2DRender* p2DRender ) 
{ 

} 

void CWndChangeName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	LoadInvalidName();

	m_dwData	= 0;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndChangeName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGENAME, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndChangeName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndChangeName::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndChangeName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangeName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangeName::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );
}

BOOL CWndChangeName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndEdit* pEdit		= (CWndEdit*)GetDlgItem( WIDC_EDIT_CHANGENAME );
		CString string	= pEdit->GetString();
		string.TrimLeft();
		string.TrimRight();
		LPCTSTR lpszString		= string;

		if( string.IsEmpty() )
		{
			pEdit->SetFocus();
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0031) ) );
			return TRUE;
		}
		CHAR c = string[ 0 ];
		if( string.GetLength() < 3 || string.GetLength() > 16 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
			return TRUE;
		}
		else
#ifdef _MULTI_LANGUAGE_
		if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) )
#else
		if( ( c >= '0' && c <= '9' ) && !IsDBCSLeadByte( string[ 0 ] ) )
#endif
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
			return TRUE;
		}
		else
		{
#ifdef __TEXT_READ
			const char* begin	= string;
			const char* end		= begin + string.GetLength();
			const char* iter	= begin;
			
			while(*iter && iter < end) 
			{ 
				const char* next = CharNext(iter);

				char bytes[16];
				memcpy( bytes, iter, next-iter );
				bytes[next-iter]	= 0;

				switch ( g_Option.m_nLanguage )
				{
					case	LANG_THA:
						{
#ifdef _MULTI_LANGUAGE_
							if ( !IsNative( iter ) &&	(!isalnum( *iter ) || iscntrl( *iter )))
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
								return TRUE;
							}
#endif
						}
						break;
					default:
						{
							if( IsMultiByte( iter ) ) 
							{
								wchar_t ch = MAKEWORD( bytes[1], bytes[0] );
								
								if( ch >= 0xA259 && ch <= 0xA261 || ch == 0xA2CD || 
									ch >= 0xA440 && ch <= 0xC67E || ch >= 0xC940 && ch <= 0xF9D5 )
								{
								}
								else
								{
									g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
									return TRUE;
								}
								
							}
							else if( isalnum( bytes[0] ) == FALSE || iscntrl( bytes[0] ) )
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
								return TRUE;
							}
						}
						break;
				}
				iter = next;
			}
#else
			for( int j = 0; j < string.GetLength(); j++ )
			{
				c = string[ j ];
				if( g_Option.m_nLanguage == LANG_THA )
				{
#ifdef _MULTI_LANGUAGE_
					if( IsNative( &lpszString[ j ] ) == FALSE && ( isalnum( c ) == FALSE || iscntrl( c ) ) )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
						return TRUE;
					}
#endif	// _MULTI_LANGUAGE_
				}
				else
				{
					if( IsDBCSLeadByte( c ) == TRUE ) 
					{
						CHAR c2 = string[ ++j ];
						DWORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
				
						if( g_Option.m_nLanguage == LANG_KOR )
						{
							if( IsHangul( word ) == FALSE ) 
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
								return TRUE;
							}
						}
					}
					else
					if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
						return TRUE;
					}
				}
			}
#endif
		}
		if( TRUE == IsInvalidName( string ) )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
			return TRUE;
		}
		g_DPlay.SendQuerySetPlayerName( m_dwData, (LPSTR)(LPCSTR)string );
		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndChangeName::LoadInvalidName( void )
{
	CScanner s;
	TCHAR szName[64];
	
	CString strFilter;
	switch( g_Option.m_nLanguage )
	{
	case LANG_KOR:
		strFilter = "InvalidName_KOR.inc";
		break;
	case LANG_ENG:
		strFilter = "InvalidName_ENG.inc";
		break;
	case LANG_JAP:
		strFilter = "InvalidName_JAP.inc";
		break;
	case LANG_CHI:
		strFilter = "InvalidName_CHI.inc";
		break;
	case LANG_THA:
		strFilter = "InvalidName_THA.inc";
		break;
	case LANG_TWN:
		strFilter = "InvalidName_TWN.inc";
		break;
	default:
		Error( "InvalidName_XXX.inc File Error = %d", g_Option.m_nLanguage );
		break;
	}
	
	if( s.Load( strFilter ) ) 
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			strcpy( szName, s.Token );
			strlwr( szName );
			m_sInvalidNames.insert( (LPCSTR)szName );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CWndChangeName::IsInvalidName( LPCSTR lpszPlayer )
{
	TCHAR pszPlayer[ 64 ];
	strcpy( pszPlayer, lpszPlayer );
	strlwr( pszPlayer );
	string str	= pszPlayer;
	for( set<string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		string strstr = *i;
		
		if( str.find( *i, 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}
#endif	// __J0519