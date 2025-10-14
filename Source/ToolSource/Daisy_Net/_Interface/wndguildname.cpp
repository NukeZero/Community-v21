#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildName.h"
#include "WndManager.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_GUILDNAME - 길드명설정창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC1 - 길드명칭을 입력해주세요.
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndGuildName::CWndGuildName() 
{ 
	LoadInvalidName();
#ifdef __J0524
	m_nId	= 0xff;
#endif	// __J0524
} 
CWndGuildName::~CWndGuildName() 
{ 
} 
BOOL CWndGuildName::LoadInvalidName( void )
{
	CScanner s;
	TCHAR szName[64];
	
	CString strFilter;
	strFilter = GetLangFileName( g_Option.m_nLanguage, FILE_INVALID );
	
	if( s.Load( strFilter ) ) {
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

BOOL CWndGuildName::IsInvalidName( LPCSTR sName )
{
	TCHAR pszName[ 64 ];
	strcpy( pszName, sName );
	strlwr( pszName );
	string str	= pszName;
	for( set<string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		if( str.find( *i, 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}

void CWndGuildName::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		CWndEdit *pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );
		pWndEdit->SetString( pGuild->m_szGuild );		// 디폴트 이름을 에디트 박스에 입력함.
	}
	else
	{
		Destroy();
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDNAME, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildName::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildName::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:		// OK
		{
			CWndEdit *pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );
			LPCTSTR szName = pWndEdit->GetString();

			// 이곳에다 szName을 서버로 보내는 코드를 넣으시오.
			CString strGuild = szName;
			strGuild.TrimLeft();
			strGuild.TrimRight();
			CHAR c = strGuild.GetAt( 0 );

			int nLength = strGuild.GetLength();
			if( nLength < 3 || nLength > 32 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
				//				g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
				return TRUE;
			}
			else
			if( isdigit( c ) && !IsDBCSLeadByte( strGuild.GetAt( 0 ) ) )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
				//				g_WndMng.OpenMessageBox( _T( "명칭에 첫글자를 숫자로 사용할 수 없습니다." ) );
				return TRUE;
			}
			else
			for( int i = 0; i < strGuild.GetLength(); i++ )
			{
				c = strGuild.GetAt( i );
				// 숫자나 알파벳이 아닐 경우는 의심하자.
				if( IsDBCSLeadByte( c ) == TRUE ) 
				{
					CHAR c2 = strGuild.GetAt( ++i );
					DWORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( m_resMng.m_nLanguage == LANG_KOR )
					{
						if( IsHangul( word ) == FALSE ) 
						{
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
							return TRUE;
						}
					}
				}
				else
				if( m_resMng.m_nLanguage != LANG_THA )						
				{
					if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
						return TRUE;
					}
				}
			}

			if( IsInvalidName( szName ) )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
				return TRUE;
			}

//			strGuild.TrimLeft();
//			strGuild.TrimRight();
#ifdef __J0524
			if( m_nId == 0xff )
				g_DPlay.SendGuildSetName( (LPCSTR)strGuild );
			else
				g_DPlay.SendQuerySetGuildName( (LPCSTR)strGuild, m_nId );
#else	// __J0524
			g_DPlay.SendGuildSetName( (LPCSTR)strGuild );
#endif	// __J0524
//			g_DPlay.SendGuildSetName( szName );
		}
		Destroy();
		break;
	case WIDC_CANCEL:
		Destroy();
		break;
	}
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_GUILD_NICKNAME - 길드별칭지정
  CtrlId : WIDC_OK - OK
  CtrlId : WIDC_CANCEL - Cancel
  CtrlId : WIDC_STATIC1 - 지정해 줄 별칭을 입력해주세요.
  CtrlId : WIDC_EDIT1 - 
****************************************************/

CWndGuildNickName::CWndGuildNickName() 
{ 
	m_idPlayer = 0;
	LoadInvalidName();
} 
CWndGuildNickName::~CWndGuildNickName() 
{ 
} 
void CWndGuildNickName::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildNickName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildNickName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_NICKNAME, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildNickName::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildNickName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildNickName::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildNickName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildNickName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildNickName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( nID == WIDC_OK )
	{
		CWndEdit *pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CString strNickName = pEdit->GetString();
		
		strNickName.TrimLeft();
		strNickName.TrimRight();
		LPCTSTR lpszString = strNickName;
		if( strNickName.IsEmpty() == FALSE )
		{
			CHAR c = strNickName[ 0 ];
			int nLength = strNickName.GetLength();

			if( nLength < 3 || nLength > 10  )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011_01) ) );
//				g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
				return TRUE;
			}
			else
			if( IsDBCSLeadByte( c ) == FALSE && isdigit( c ) ) 
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
//				g_WndMng.OpenMessageBox( _T( "명칭에 첫글자를 숫자로 사용할 수 없습니다." ) );
				return TRUE;
			}
			else
			{
				for( int i = 0; i < strNickName.GetLength(); i++ )
				{
					c = strNickName[ i ];
					// 숫자나 알파벳이 아닐 경우는 의심하자.
					if( IsDBCSLeadByte( c ) == TRUE ) 
					{
						CHAR c2 = strNickName[ ++i ];
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
					{
						if( g_Option.m_nLanguage != LANG_THA )
						{							
							if( isalnum( c ) == FALSE || iscntrl( c ) )
							{
								// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
								return TRUE;
							}
						}
					}
				}				
				/*
				for( int i = 0; i < strNickName.GetLength(); i++ )
				{
					c = strNickName[ i ];
					//BOOL a = isalpha( c );
					//BOOL b = isdigit( c );
					// 숫자나 알파벳이 아닐 경우는 의심하자.
					if( IsDBCSLeadByte( c ) == TRUE ) 
					{
						CHAR c2 = strNickName[ ++i ];
						DWORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
						//if( g_Option.m_nLanguage == LANG_KOR )
						{
#ifdef _MULTI_LANGUAGE_
							if( IsHangul(word) == FALSE ) 
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
	//							g_WndMng.OpenMessageBox( _T( "명칭에 한자나 특수 문자를 사용할 수 없습니다." ) );
								return TRUE;
							}
#endif
						}
					}
					else
					if( g_Option.m_nLanguage != LANG_THA )
					{
						if( isalnum( c ) == FALSE || iscntrl( c ) )
						{
						// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
	//							g_WndMng.OpenMessageBox( _T( "명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다." ) );
							return TRUE;
						}
					}
				}
				*/
			}
			
			if( TRUE == IsInvalidName( strNickName ) )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
				//				g_WndMng.OpenMessageBox( _T( "사용할수 없는 이름입니다" ) );
				return TRUE;
			}
#ifdef __ONLINE
			CGuild* pGuild = g_pPlayer->GetGuild();
			if( pGuild )
			{
				CGuildMember* pGuildMember = pGuild->GetMember( g_pPlayer->m_idPlayer );
				if( pGuildMember )
				{
					if( strcmp( pGuildMember->m_szAlias, strNickName ) )
					{
						g_DPlay.SendGuildNickName( m_idPlayer, strNickName );
						Destroy();	
					}
				}
			}
#endif
		}
		else
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
		//		g_WndMng.OpenMessageBox( _T( "순회극단 명칭을 정해야 합니다." ) );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndGuildNickName::LoadInvalidName( void )
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
	
	if( s.Load( strFilter ) ) {
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

BOOL CWndGuildNickName::IsInvalidName( LPCSTR sName )
{
	TCHAR pszName[ 64 ];
	strcpy( pszName, sName );
	strlwr( pszName );
	string str	= pszName;
	for( set<string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		if( str.find( *i, 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}

