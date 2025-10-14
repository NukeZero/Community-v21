
#include "StdAfx.h"
#include "ResManager.h"

#define			BEGIN_ID			100
#define			WNDSTYLE_HEADER		_T( "WndStyle.h" )

extern TCHAR	PROGRAM_NAME[128];

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CResManager::CResManager()
:	m_dwNewCtrlId( NULL )
#if __CURRENT_LANG == LANG_JAP
,	m_nLanguage( LANG_JAP )
#else
,	m_nLanguage( LANG_KOR ) 
#endif	
,	m_nChangeItem( -1 )
,	m_bModifiedHeader( FALSE )
,	m_bModifiedScript( FALSE )
,	m_phistorylog( NULL )
,	m_nHistoryCount( NULL )
#ifdef __USE_STR_TBL0407
,	m_strStrFile( _T("") )
,	m_nString( NULL )
#endif	
,	m_nSelectLanguage( NULL )
{
	DeleteHistory();
}


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CResManager::~CResManager()
{
	Free();
}


//---------------------------------------------------------------------------------------------
// Load ( 불러오기 )
// param	: lpszName
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CResManager::Load( LPCTSTR lpszName )
{
	FLERROR_ASSERT_LOG_RETURN( lpszName, FALSE, _T("[리소스 메니져] 파일 이름 포인터 비어있음.") );			

	CScript scriptHeader;

#ifdef __CLIENT
	if( scriptHeader.Load( WNDSTYLE_HEADER ) )
		scriptHeader.PreScan( TRUE );
	scriptHeader.Free();
	if( scriptHeader.Load( RESDATA_HEADER ) )
		scriptHeader.PreScan( TRUE );
#else
	if( scriptHeader.Load( WNDSTYLE_HEADER ) )
		scriptHeader.PreScan( FALSE );
	scriptHeader.Free();
	if( scriptHeader.Load( RESDATA_HEADER ) )
		scriptHeader.PreScan( FALSE );
#endif

	CScript scanner;
	if( FALSE == scanner.Load( lpszName, FALSE ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[리소스 메니져] 파일 읽기 실패 : [%s]"), lpszName );
		FLASSERT( FALSE );
		return FALSE;
	}

#ifdef __USE_STR_TBL0407
	CString	strFile( lpszName );
	m_strStrFile = strFile.Left( strFile.GetLength() - 3 ) + _T( "txt.txt" );
	
	if( LoadString() == FALSE )
		return FALSE;
#endif	

	DWORD	dwAppletId( NULL );
	DWORD	dwWndType( NULL );
	DWORD	dwCtrlId( NULL );

	scanner.GetToken_NoDef();

	while( scanner.tok != FINISHED )
	{
		LPWNDAPPLET pWndApplet = new WNDAPPLET;
		FLERROR_ASSERT_LOG_DO( pWndApplet, continue, _T("[리소스 메니져] 애플릿 생성 실패.") );			

		pWndApplet->pWndBase		= NULL;
		pWndApplet->strDefine		= scanner.token;
		pWndApplet->dwWndId			= CScript::GetDefineNum( scanner.token );

		scanner.GetToken();
		pWndApplet->strTexture		= scanner.token;

		scanner.GetToken();
		pWndApplet->strDefaultImg	= scanner.token;
		pWndApplet->m_bTile			= scanner.GetNumber();

		CSize size( 0, 0 );
		pWndApplet->size.cx			= scanner.GetNumber();
		pWndApplet->size.cy			= scanner.GetNumber();
		pWndApplet->dwWndStyle		= scanner.GetNumber();
		pWndApplet->d3dFormat		= (D3DFORMAT)scanner.GetNumber();


#ifdef __USE_STR_TBL0407
		GetLangApplet( scanner, pWndApplet, FALSE );
		GetLangApplet( scanner, pWndApplet, TRUE );
#else	
		pWndApplet->strTitle		= GetLangApplet( scanner, pWndApplet, FALSE );	// 타이틀 
		pWndApplet->strToolTip		= GetLangApplet( scanner, pWndApplet, TRUE );	// 핼프 키 
#endif	

		WndAppletContainer::_Pairib rst = m_mapWndApplet.insert( WndAppletContainer::value_type( pWndApplet->dwWndId, pWndApplet ) );
		if( !rst.second )
		{
			FLERROR_LOG( PROGRAM_NAME, _T("[리소스 메니져] 맵에 삽입 실패: [%u]"), pWndApplet->dwWndId );
			SAFE_DELETE( pWndApplet );
			FLASSERT( NULL );
			continue;
		}

		scanner.GetToken(); 
		dwWndType = scanner.GetNumber(); 

		while( *scanner.token != _T('}') )
		{
			LPWNDCTRL pWndCtrl = new WNDCTRL;
			FLERROR_ASSERT_LOG_DO( pWndCtrl, continue, _T("[리소스 메니져] 윈도우 컨트롤 생성 실패.") );			

			pWndCtrl->dwWndType		= dwWndType;

			scanner.GetToken_NoDef();
			pWndCtrl->strDefine		= scanner.token;

			for( int z = 0; z < pWndApplet->ptrCtrlArray.GetSize(); z++ )
			{
				if( ((LPWNDCTRL)pWndApplet->ptrCtrlArray.GetAt( z ) )->strDefine == pWndCtrl->strDefine )
				{
					FLERROR_LOG( PROGRAM_NAME, _T("[리소스 메니져] 애플릿 [%s] 에서 [%s] 컨트롤 충돌"), pWndApplet->strDefine, pWndCtrl->strDefine );
					SAFE_DELETE( pWndCtrl );
					FLASSERT( NULL );
					continue;
				}
			}

			pWndCtrl->dwWndId		= CScript::GetDefineNum( scanner.token );

			scanner.GetToken();
			pWndCtrl->strTexture	= scanner.token;

#ifdef __V050322_CACHEQUEST
			pWndCtrl->m_bTile		= scanner.GetNumber();
#endif

			pWndCtrl->rect.left		= scanner.GetNumber();
			pWndCtrl->rect.top		= scanner.GetNumber();
			pWndCtrl->rect.right	= scanner.GetNumber();
			pWndCtrl->rect.bottom	= scanner.GetNumber();
			pWndCtrl->dwWndStyle	= scanner.GetNumber();

			// visible, Group, Disabled, TabStop
			pWndCtrl->m_bVisible	= scanner.GetNumber();
			pWndCtrl->m_bGroup		= scanner.GetNumber();
			pWndCtrl->m_bDisabled	= scanner.GetNumber();
			pWndCtrl->m_bTabStop	= scanner.GetNumber();
			pWndCtrl->m_nFontColorR = scanner.GetNumber();
			pWndCtrl->m_nFontColorG = scanner.GetNumber();
			pWndCtrl->m_nFontColorB = scanner.GetNumber();

#ifdef __USE_STR_TBL0407
			GetLangCtrl( scanner, pWndCtrl, FALSE );
			GetLangCtrl( scanner, pWndCtrl, TRUE );
#else				
			pWndCtrl->strTitle = GetLangCtrl( scanner, pWndCtrl, FALSE );	// 타이틀 
			pWndCtrl->strToolTip = GetLangCtrl( scanner, pWndCtrl, TRUE );	// 핼프 키 
#endif	
			pWndApplet->ptrCtrlArray.Add( pWndCtrl );
			dwWndType = scanner.GetNumber(); 
		} 

		scanner.GetToken_NoDef();
	} 

	// 사용하지 않는 Define을 제거하고, 마지막 숫자 카운트를 얻는다.
	LPSTR			pData( NULL );
	CString			string( _T("") );
	POSITION		pos( CScript::m_globalDef.GetStartPosition() );

	CDWordArray 	idAppletArray;
	CDWordArray 	idCtrlArray;

	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "APP_" ) ) == 0 )
		{
			dwAppletId = _ttoi( pData ); 

			if( GetAt( dwAppletId ) )
			{
			}
			else
			{
			}
		}
		else
		if( string.Find( _T( "WIDC_" ) ) == 0 )
		{
			dwCtrlId = _ttoi( pData ); 
			if( dwCtrlId > m_dwNewCtrlId )
				m_dwNewCtrlId = dwCtrlId;
		}
	}	

	m_dwNewCtrlId++;
	CScript::m_localDef.Free();

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// Free ( 해제 )
// param	: lpDefine, lpDefData
// return	: LPCTSTR
//---------------------------------------------------------------------------------------------
void CResManager::Free()
{
	if(m_mapWndApplet.size() > 0)
	{
		WNDAPPLET* pApplet = NULL;
		for( WndAppletIter iter = m_mapWndApplet.begin(); iter != m_mapWndApplet.end(); ++iter )
		{
			pApplet = iter->second;
			for( int i = 0; i < pApplet->ptrCtrlArray.GetSize(); ++i )
				delete (LPWNDCTRL)pApplet->ptrCtrlArray.GetAt(i);

			SAFE_DELETE( pApplet );
		}
		m_mapWndApplet.clear();
	}
}


//---------------------------------------------------------------------------------------------
// Add Define ( 정의 추가 )
// param	: lpDefine, lpDefData
// return	: LPCTSTR
//---------------------------------------------------------------------------------------------
LPCTSTR CResManager::AddDefine( LPCTSTR lpDefine, LPCTSTR lpDefData )
{
	CScript::AddDefine( lpDefine, lpDefData );
	m_bModifiedHeader = TRUE;
	return static_cast<LPCTSTR>( lpDefData );
}


//---------------------------------------------------------------------------------------------
// Add Define ( 정의 추가 )
// param	: lpDefine, nDefData
// return	: int
//---------------------------------------------------------------------------------------------
int CResManager::AddDefine( LPCTSTR lpDefine, int nDefData )
{
	CScript::AddDefine( lpDefine, nDefData );
	m_bModifiedHeader = TRUE;
	return nDefData;
}


//---------------------------------------------------------------------------------------------
// Remove Define ( 정의 삭제 )
// param	: lpDefine
// return	: void 
//---------------------------------------------------------------------------------------------
void CResManager::RemoveDefine( LPCTSTR lpDefine )
{
	CScript::RemoveDefine( lpDefine );
	m_bModifiedHeader = TRUE;
}


//---------------------------------------------------------------------------------------------
// Remove Define ( 정의 삭제 )
// param	: lpDefine
// return	: void 
//---------------------------------------------------------------------------------------------
LPWNDCTRL WNDAPPLET::GetAt( DWORD dwWndId )
{
	//Applet의 ID값을 받아 App 안의 Control 정보를 넘겨준다.
	LPWNDCTRL pWndCtrl = NULL;
	for( int i = 0; i < ptrCtrlArray.GetSize(); i++ )
	{
		pWndCtrl = (LPWNDCTRL) ptrCtrlArray.GetAt( i );
		if( pWndCtrl->dwWndId == dwWndId )
			return pWndCtrl;
	}
	return NULL;
}


//---------------------------------------------------------------------------------------------
// Get New Applet ID ( 새로운 애플릿 아이디 얻기 )
// param	: void
// return	: DWORD 
//---------------------------------------------------------------------------------------------
DWORD CResManager::GetNewAppletId()
{
	POSITION		pos( CScript::m_globalDef.GetStartPosition() );
	CDWordArray		idAppletArray;
	DWORD			dwId( NULL );
	TCHAR*			pData( NULL );
	CString			string( _T("") );

	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "APP_" ) ) == 0 )
		{
			dwId = _ttoi( pData ); 
			idAppletArray.SetAtGrow( dwId - BEGIN_ID, 1 );
		}
	}	
	
	int i = 0;
	for( ; i < idAppletArray.GetSize(); i++ )
	{
		if( idAppletArray.GetAt( i ) == 0 )
			return i + BEGIN_ID;
	}
	
	return i + BEGIN_ID;
}


//---------------------------------------------------------------------------------------------
// Get New Control ID ( 새로운 컨트롤 아이디 얻기 )
// param	: dwAppletId
// return	: DWORD 
//---------------------------------------------------------------------------------------------
DWORD CResManager::GetNewControlId( DWORD dwAppletId )
{
	DWORD dwNewCtrlId = m_dwNewCtrlId;
	m_dwNewCtrlId++;
	return dwNewCtrlId;
}



CString CResManager::GetNewAppletDefine()
{
	//Applet을 생성할경우 Applet define도 생성됨
	CString string;
	int i = 1;
BACK:
	string.Format( _T( "APP_APPLET%d" ), i++ );

	POSITION pos 
		= CScript::m_globalDef.GetStartPosition();
	CString strTemp;
	TCHAR* pData;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, strTemp, (void*&)pData );
		if( strTemp == string )
			goto BACK;
	}	
	return string;
}

//---------------------------------------------------------------------------------
CString CResManager::GetNewControlDefine( DWORD dwAppletId, LPCTSTR lpszBaseDefine )
{
	//control을 생성할경우 Control define도 생성됨
	CString string;
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	int i = 1;
BACK:
	string.Format( _T( "%s%d" ), lpszBaseDefine, i++ );

	for( int j = 0; j < lpWndApplet->ptrCtrlArray.GetSize(); j++ )
	{
		LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( j );
		// 같은 스트링이 사용되는지 검사 
		if( lpWndCtrl->strDefine == string )
			goto BACK;
	}
	// 같은 아이디가 사용되는지 검사.
	// string 이놈이 사용하는 아이디가 이미 현재 애플렛에 만들어진 아이디와 충돌하는게 있는지 검사
	CHAR* pData;
	if( CScript::LookupDefine( string, (void*&)pData ) == TRUE )
	{
		int nId = atoi( pData );	
		for( int j = 0; j < lpWndApplet->ptrCtrlArray.GetSize(); j++ )
		{
			LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( j );
			if( lpWndCtrl->dwWndId == nId )
				goto BACK;
		}
	}
	return string;
}

//---------------------------------------------------------------------------------
void CResManager::ResetNewControlId()
{
	//Contorl을 지웠을때 ID를 Reset해줌 
	POSITION pos = CScript::m_globalDef.GetStartPosition();
	CString string;
	LPSTR pData;
	DWORD dwCtrlId;
	m_dwNewCtrlId = 0;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "WIDC_" ) ) == 0 )
		{
			dwCtrlId = _ttoi( pData ); 
			if( dwCtrlId > m_dwNewCtrlId )
				m_dwNewCtrlId = dwCtrlId;
		}
	}	
	m_dwNewCtrlId++;
}

//---------------------------------------------------------------------------------
BOOL CResManager::AddControl( DWORD dwAppletId, LPWNDCTRL pWndCtrl,BOOL bIsMode)
{
	LPWNDAPPLET pWndApplet = GetAt( dwAppletId );

	LPWNDCTRL	pWndControl = new WNDCTRL;
	
	pWndControl->dwWndId		  = pWndCtrl->dwWndId;
	pWndControl->dwWndStyle		  = pWndCtrl->dwWndStyle ;
	pWndControl->dwWndType		  = pWndCtrl->dwWndType  ;
	pWndControl->rect			  = pWndCtrl->rect       ;
	pWndControl->strDefine        = pWndCtrl->strDefine;
	pWndControl->strTitle		  = pWndCtrl->strTitle;
	pWndControl->strTexture		  = pWndCtrl->strTexture;
	pWndControl->strDescription	  = pWndCtrl->strDescription;
	pWndControl->m_bTile		  = pWndCtrl->m_bTile;

#ifndef __USE_STR_TBL0407
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		pWndControl->awszTitle[ i ][0] = 0;
		pWndControl->awszToolTip[ i ][0] = 0;
	}
#endif  //__DAISY
#endif	//__USE_STR_TBL0407
	
	AddDefine( pWndControl->strDefine, pWndControl->dwWndId );
	pWndApplet->ptrCtrlArray.Add( pWndControl );
	if( bIsMode == TRUE )
	{
		pWndControl->dwWndHistoryType = CTRL_CREATE;
		SaveHistory(pWndControl);
	}
	m_bModifiedScript = TRUE;
	
	return TRUE;
}

//---------------------------------------------------------------------------------
BOOL CResManager::AddApplet( LPWNDAPPLET pWndApp )
{
	// define이 이미 정의되어 있나?

	LPWNDAPPLET pWndApplet = new WNDAPPLET;
	pWndApplet->pWndBase = pWndApp->pWndBase;
	pWndApplet->dwWndId = pWndApp->dwWndId;
	pWndApplet->size = pWndApp->size;
	pWndApplet->dwWndStyle = pWndApp->dwWndStyle;
	pWndApplet->strTitle = pWndApp->strTitle;
	pWndApplet->strDefine = pWndApp->strDefine;
	pWndApplet->strTexture= pWndApp->strTexture;
	pWndApplet->m_bTile     = pWndApp->m_bTile;
	pWndApplet->d3dFormat = 	pWndApp->d3dFormat;
	pWndApplet->nDefSizeX = 	0;
	pWndApplet->nDefSizeY = 	0;

#ifdef __USE_STR_TBL0407
	lstrcpy( pWndApplet->m_szTitleKey, pWndApp->m_szTitleKey );
	lstrcpy( pWndApplet->m_szToolTipKey, pWndApp->m_szTitleKey );
#endif	// __USE_STR_TBL0407
#ifndef __USE_STR_TBL0407
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		pWndApplet->awszTitle[ i ][0] = 0;
		pWndApplet->awszToolTip[ i ][0] = 0;
	}
#endif  //__DAISY
#endif	// __USE_STR_TBL0407

	AddDefine( pWndApplet->strDefine, pWndApplet->dwWndId );

	// Map에 추가. 정식 등록 
	m_mapWndApplet[ pWndApplet->dwWndId ] = pWndApplet;
	pair< WndAppletIter, bool > rst = m_mapWndApplet.insert( WndAppletContainer::value_type( pWndApplet->dwWndId, pWndApplet ) );
	if( !rst.second )
	{
		AfxMessageBox( "큰일입니다!! 에플릿 아이디가 중복입니다, 작업을 중단하고 원인을 찾으세요" );
	}

	m_bModifiedScript = TRUE;
	return TRUE;
}

//---------------------------------------------------------------------------------
BOOL CResManager::IsSameDefine( LPWNDAPPLET lpExceptionWndApplet, LPCTSTR lpszDefine )
{
	WNDAPPLET* lpWndApplet = NULL;
	for( WndAppletIter iter = m_mapWndApplet.begin(); iter != m_mapWndApplet.end(); ++iter )
	{
		lpWndApplet = iter->second;

		if( lpExceptionWndApplet != lpWndApplet )
		{
			for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
			{
				LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
				//if( _tcscmp( lpWndCtrl->szDefine, lpszDefine ) == 0 )
				if( lpWndCtrl->strDefine == CString(lpszDefine) )
				return TRUE;
			}
		}
	}
	return FALSE;
}

//---------------------------------------------------------------------------------
BOOL CResManager::RemoveControl( DWORD dwAppletId, LPCTSTR lpszDefine,BOOL bIsMode )
{
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );

	for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
	{
		LPWNDCTRL lpWndCtrl = new WNDCTRL;

		LPWNDCTRL lpWndCtrl_Old = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );

		*lpWndCtrl = *lpWndCtrl_Old;

		//if( _tcscmp( lpWndCtrl->szDefine, lpszDefine ) == 0 )
		if( lpWndCtrl->strDefine == lpszDefine )
		{
			// Control의 Id는 중복해서 사용될 수 있다. 즉 현재 Applet에서 
			// WIDC_CONTROL0을 사용해도 다른 Applet에서도 WIDC_CONTROL0을 사용할 수 있는 
			// 것이다. 따라서, 만약 중복된 WIDC_CONTROL0를 함부러 지워버리면 다른 Applet에서
			// Define을 찾을 수 없게 된다. 그렇기 때문에 만약 삭제 하려면
			// 모든 Applet의 Define을 찾아보고 중복되지 않았을 경우에만 제거하면 된다.
			if( IsSameDefine( lpWndApplet, lpszDefine ) == FALSE )
			{
				CString str;
				str.Format( "RemoveControl에서 %s 중복이 없어서 영구 제거함", lpszDefine );
				AfxMessageBox( str );
			}
			if( IsSameDefine( lpWndApplet, lpszDefine ) == FALSE )
			{
				RemoveDefine( lpszDefine );
				ResetNewControlId();
			}

			lpWndApplet->ptrCtrlArray.RemoveAt( i ); 
		
			m_bModifiedScript = TRUE;
			if( bIsMode == TRUE )
			{
				lpWndCtrl->dwWndHistoryType = CTRL_REMOVE;
				SaveHistory(lpWndCtrl);
			}		
			 lpWndCtrl = NULL;
			return TRUE;
		}
}
	return FALSE;
}

//---------------------------------------------------------------------------------
BOOL CResManager::RemoveApplet( DWORD dwAppletId )
{
	WNDAPPLET* lpWndApplet = NULL;
	WndAppletIter finder = m_mapWndApplet.find( dwAppletId );
	if( finder != m_mapWndApplet.end() )
	{
		lpWndApplet = finder->second;

		// 삭제할 애플렛에 포함된 콘트롤들의 define을 다른 곳에서 사용하는지
		// 확인하고 사용하지 않을 경우 삭제한다.
		for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
		{
			LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
			if( RemoveControl( dwAppletId, lpWndCtrl->strDefine ) )
				i--;
		}

		SAFE_DELETE( lpWndApplet );

		m_mapWndApplet.erase( finder++ );

		m_bModifiedScript = TRUE;
		return TRUE;

	}
	return FALSE;
}

//---------------------------------------------------------------------------------
BOOL CResManager::ChangeAppletDefine( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC, DWORD* lpdwNewWndId )
{
	if( _tcscmp( lpszOldWIDC, lpszNewWIDC ) != 0  )
	{
		DWORD dwOldAppletId = 0;
		DWORD dwNewAppletId = 0;
		LPWNDAPPLET lpWndApplet = NULL;
		LPSTR lpDefData = CScript::GetDefineStr( lpszNewWIDC );
		// 새 아이디가 이미 존재하는 것과 같다. 그것은 둘중에 하나다.
		// 1.외부에서 정의된 경우, 2.실제로 사용되고 있는 경우
		// 2.실제로 사용된 경우만 파악하면 된다.
		if( lpDefData )
		{
			dwNewAppletId = CScript::GetDefineNum( lpszNewWIDC );

			WndAppletIter finder = m_mapWndApplet.find( dwNewAppletId );
			if( finder != m_mapWndApplet.end() )
				return FALSE;

			// 이 경우는 1의 경우로 실제로 사용되고 있지는 않지만 외부에서 임의로 지정된 것이다.
			// 단지 define 이름만 바꿔주면 된다.
			dwOldAppletId = CScript::GetDefineNum( lpszOldWIDC );
			dwNewAppletId = CScript::GetDefineNum( lpszNewWIDC );
			lpWndApplet = GetAt( dwOldAppletId );
			lpWndApplet->strDefine = lpszNewWIDC;
			lpWndApplet->dwWndId = dwNewAppletId;

			WndAppletIter older = m_mapWndApplet.find( dwOldAppletId );
			if( older != m_mapWndApplet.end() )
			{
				m_mapWndApplet.erase( older );
			}

			WndAppletContainer::_Pairib rst = m_mapWndApplet.insert( WndAppletContainer::value_type( dwNewAppletId, lpWndApplet ) );
			ASSERT( rst.second );

			if( lpdwNewWndId )
				*lpdwNewWndId = dwNewAppletId;
			m_bModifiedScript = TRUE;
		}
		else
		{
			// 동일한 정의가 없을 경우에는 새 아이디를 배정받는다.
			dwOldAppletId = CScript::GetDefineNum( lpszOldWIDC );
			dwNewAppletId = GetNewAppletId();
			AddDefine( lpszNewWIDC, dwNewAppletId );
			lpWndApplet = GetAt( dwOldAppletId );
			lpWndApplet->strDefine = lpszNewWIDC;
			lpWndApplet->dwWndId = dwNewAppletId;

			WndAppletIter older = m_mapWndApplet.find( dwOldAppletId );
			if( older != m_mapWndApplet.end() )
			{
				m_mapWndApplet.erase( older++ );
			}

			WndAppletContainer::_Pairib rst = m_mapWndApplet.insert( WndAppletContainer::value_type( dwNewAppletId, lpWndApplet ) );
			ASSERT( rst.second );

			if( lpdwNewWndId )
				*lpdwNewWndId = dwNewAppletId;
			m_bModifiedScript = TRUE;
		}
	}
	else
	if( lpdwNewWndId )
		*lpdwNewWndId = CScript::GetDefineNum( lpszOldWIDC );


	return TRUE;
}
//FUCK 10
//SUCK 10

//SUCK->DUCK
//---------------------------------------------------------------------------------
BOOL CResManager::ChangeControlDefine( DWORD dwAppletId, LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC )
{
	if( _tcscmp( lpszOldWIDC, lpszNewWIDC ) != 0  )
	{
		LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
		if( lpWndApplet == NULL )
			return FALSE;
		DWORD dwCtrlId = CScript::GetDefineNum( lpszOldWIDC );
		LPSTR lpDefData = CScript::GetDefineStr( lpszNewWIDC );
		DWORD dwNewId = lpDefData ? _ttoi( lpDefData ) : dwCtrlId;//0xffffffff;
		LPWNDCTRL lpWndCtrl;
		// 만약 lpDefData이 NULL이라면 새로 교체될 define이 존재하지 않는다는 의미이므로 같은게 있나 검사할 필요가 없음.
		if( lpDefData ) //dwNewId != NULL_ID )
		{
			// 내 Applet 중에 lpszNewWIDC와 같은 Define이 있으면 바꿀 수 없음
			for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
			{
				lpWndCtrl = (LPWNDCTRL) lpWndApplet->ptrCtrlArray.GetAt( i );
				// 같은게 define이 있나?
				//if( !_tcscmp( lpszNewWIDC, lpWndCtrl->szDefine ) )
				if( lpszNewWIDC == lpWndCtrl->strDefine )
					return FALSE;
				// 같은 아이디가 있나? 아이디가 같은게 있더라도 이전 define과 같은거라면,
				// 즉 내 자신이라면 같은거라고 판단하지 않는다.
				// lpszOldWIDC != lpWndCtrl->strDefine 이걸 하지 않으면 같은 아이디의 다른 define은 바꿀 수 없게 된다.
				// WIDC_TEST 889 였을 때 BUTTON2 889로 바꿀 수 없다. id가 같기 때문이다.
				// 근데 lpszOldWIDC != lpWndCtrl->strDefine 해주면 define이 같지 않기 때문에 
				// 같다고 판단하지 않고 교체를 허용하게 된다. 
				if( dwNewId == lpWndCtrl->dwWndId && lpszOldWIDC != lpWndCtrl->strDefine )
					return FALSE;
			}
		}
		// 전체에서 같은 Define이 없이 lpszOldWIDC가 유일했다면 삭제해두 된다.
		if( IsSameDefine( lpWndApplet, lpszOldWIDC ) == FALSE )
		{
			CString str;
			str.Format( "ChangeControlDefine %s 중복이 없어서 영구 제거함", lpszOldWIDC );
			AfxMessageBox( str );
			RemoveDefine( lpszOldWIDC );
			ResetNewControlId();
		}
		// 새로운 define이 없다면 추가한다.
		if( lpDefData == NULL )
		{
			AddDefine( lpszNewWIDC, dwCtrlId );
		}
		// 바꿀 콘트롤을 얻는다.
		lpWndCtrl = lpWndApplet->GetAt( dwCtrlId );
		if( lpWndCtrl == NULL )
		{
			return FALSE;
		}
		//_tcscpy( lpWndCtrl->szDefine, lpszNewWIDC );
		lpWndCtrl->strDefine = lpszNewWIDC;
		lpWndCtrl->dwWndId = dwNewId;
		m_bModifiedScript = TRUE;
	}
	return TRUE;
}


//---------------------------------------------------------------------------------
LPWNDAPPLET CResManager::GetAt( DWORD dwAppletId )
{
	LPWNDAPPLET LPWNDAPPLET = NULL;

	WndAppletIter Finder = m_mapWndApplet.find( dwAppletId );
	if( Finder != m_mapWndApplet.end() )
		LPWNDAPPLET = Finder->second;

	return LPWNDAPPLET;
}

//---------------------------------------------------------------------------------
LPWNDCTRL CResManager::GetAtControl( DWORD dwAppletId, DWORD dwCtrlId )
{
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	if( lpWndApplet )
		return lpWndApplet->GetAt ( dwCtrlId );
	return NULL;
}
/*
APP_APPLET1 "콘트롤 테스트"  300 300
{
    WTYPE_BUTTON WIDC_BUTTON0 "Button" 0 0 100 20 0x260000 

}
APP_APPLET2 "바보"  300 300
{
    WTYPE_STATIC WIDC_BUTTON1 "Static" 24 104 124 124 0x2060000 

}
*/
#ifdef __USE_STR_TBL0407
//---------------------------------------------------------------------------------
void CResManager::GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip )
{
	scanner.GetToken();	// {
	scanner.GetToken();
	if( bToolTip )
	{
		lpWndApplet->strToolTip		= scanner.token;
		lstrcpy( lpWndApplet->m_szToolTipKey, (LPCTSTR)scanner.m_strKey );
	}
	else
	{
		lpWndApplet->strTitle	= scanner.token;
		lstrcpy( lpWndApplet->m_szTitleKey, (LPCTSTR)scanner.m_strKey );
	}
	scanner.GetToken();	// }
}
//---------------------------------------------------------------------------------
void CResManager::GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip )
{
	scanner.GetToken();	// {
	scanner.GetToken();
	if( bToolTip )
	{
		lpWndCtrl->strToolTip	= scanner.token;
		lstrcpy( lpWndCtrl->m_szToolTipKey, (LPCTSTR)scanner.m_strKey );
	}
	else
	{
		lpWndCtrl->strTitle		= scanner.token;
		lstrcpy( lpWndCtrl->m_szTitleKey, (LPCTSTR)scanner.m_strKey );
	}
	scanner.GetToken();	// }
}
#else	// __USE_STR_TBL0407
//---------------------------------------------------------------------------------
CString CResManager::GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip )
{
	for( int i = 0; i < LANG_MAX; i++ )
	{
		if( bToolTip )
			lpWndApplet->awszToolTip[ i ][0] = 0;
		else
			lpWndApplet->awszTitle[ i ][0] = 0;
	}
	CString string;
	int nLang = 0;
	scanner.GetToken(); // {
	nLang = scanner.GetNumber(); // lang
	do
	{
		scanner.GetLastFull(); // string
		if( m_nLanguage == nLang )
			string = scanner.token;
		if( bToolTip )
			wcscpy( lpWndApplet->awszToolTip[ nLang ], scanner.m_wszToken );
		else
			wcscpy( lpWndApplet->awszTitle[ nLang ], scanner.m_wszToken );
		nLang = scanner.GetNumber(); // lang or '}'
	} while( *scanner.token != '}' );		
	return string;
}
//---------------------------------------------------------------------------------
CString CResManager::GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip )
{
	CString string;
	int nLang = 0;
	scanner.GetToken(); // {
	nLang = scanner.GetNumber(); // lang
	do
	{
		scanner.GetLastFull(); // string
		if( m_nLanguage == nLang )
			string = scanner.token;
		if( bToolTip )
			wcscpy( lpWndCtrl->awszToolTip[ nLang ], scanner.m_wszToken );
		else
			wcscpy( lpWndCtrl->awszTitle[ nLang ], scanner.m_wszToken );
		nLang = scanner.GetNumber(); // lang or '}'
	} while( *scanner.token != '}' );		
	return string;
}
#endif	// __USE_STR_TBL0407

#ifdef __DAISY
//---------------------------------------------------------------------------------
int SortDefine( const VOID* arg1, const VOID* arg2 )
{
	LPDEFINEDATA* lpDefineData1 = (DEFINEDATA**)arg1;
	LPDEFINEDATA* lpDefineData2 = (DEFINEDATA**)arg2;
	if( (*lpDefineData1)->dwData > (*lpDefineData2)->dwData )
		return 1;
	if( (*lpDefineData1)->dwData < (*lpDefineData2)->dwData )
		return -1;

	if( (*lpDefineData1)->dwData == (*lpDefineData2)->dwData )
	{
		//gmpbigsun(20101109): CString:: > 의 return type is bool ==> -1 or 1
		return ( (*lpDefineData1)->strDefine > (*lpDefineData2)->strDefine ) ? 1 : -1;
//		return (*lpDefineData1)->strDefine > (*lpDefineData2)->strDefine ;
	}

    return 0;
}
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------
// Save Header ( 헤더 저장하기 )
// param	: lpszFileName
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CResManager::SaveHeader( LPCTSTR lpszFileName )
{
	FLERROR_ASSERT_LOG_RETURN( lpszFileName, FALSE, _T("[리소스 메니져] 파일이름 포인터가 유효하지 않음 - [ %s ]."), lpszFileName );			

	if( IsModifiedHeader() == FALSE )
		return TRUE;
		
	CFileIO file;

	if( FALSE == file.Open( lpszFileName, _T( "wt" ) ))
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, _T("[리소스 메니져] 파일 열기 실패 - [ %s ]."), lpszFileName );			

	CString			string( _T("") );
	CStringArray	stringArray;

	file.PutString( _T( "#ifndef __RESOURCE_H\n" ) ); 
	file.PutString( _T( "#define __RESORUCE_H\n" ) );
	file.PutString( _T( "\n" ) );
	file.PutString( _T( "// Applet Id\n" ) );

	LPDEFINEDATA*	lpDefineDataArray = new LPDEFINEDATA[ CScript::m_globalDef.GetCount() ];
	FLERROR_ASSERT_LOG_RETURN( lpDefineDataArray, FALSE, _T("[리소스 메니져] Define Array 생성 실패.") );			
	
	LPDEFINEDATA	lpDefineData( NULL );
	int				nDefineDataNum( NULL );

	CString			strTemp( _T("") );
	CString			strDefSrc( _T("") );
	CString			strDefDst( _T( "APP_" ) );
	
	POSITION pos = CScript::m_globalDef.GetStartPosition();

	int nLength = strDefDst.GetLength();
	while(pos)
	{ 
		TCHAR* pData( NULL );
		CScript::m_globalDef.GetNextAssoc( pos, strDefSrc, (void*&)pData );

		int i( NULL );
		for( ; i < nLength; i++ )
		{
			if( strDefSrc[i] != strDefDst[ i ] )
				break;
		}

		if( i == nLength )
		{
			lpDefineData = new DEFINEDATA;
			FLERROR_ASSERT_LOG_DO( lpDefineData, continue, _T("[리소스 메니져] Define Data 생성 실패.") );			
			
			lpDefineData->dwData = _ttoi( pData ); 
			lpDefineData->strDefine = strDefSrc;
		
			lpDefineDataArray[ nDefineDataNum++ ] = lpDefineData;
		}
	}

	qsort( lpDefineDataArray, nDefineDataNum, sizeof( DEFINEDATA* ), SortDefine );

	for( int i = 0; i < nDefineDataNum; i++ )
	{
		file.PutString( _T( "#define " ) );
		lpDefineData = lpDefineDataArray[ i ];
		file.PutString( lpDefineData->strDefine );

		int nSpace( 64 - lpDefineData->strDefine.GetLength() );

		for( int j = 0; j < nSpace; j++ )
			file.PutString( _T( " " ) );

		strTemp.Format( _T( "%d" ), lpDefineData->dwData );
		file.PutString( strTemp );
		file.PutString( _T( "\n" ) );
		SAFE_DELETE( lpDefineData );
	}

	file.PutString( _T( "\n" ) );
	file.PutString( _T( "// Control Id\n" ) );

	nDefineDataNum = 0;

	strDefDst = _T( "WIDC_" );

	pos = CScript::m_globalDef.GetStartPosition();
	nLength = strDefDst.GetLength();

	while(pos)
	{ 
		TCHAR* pData( NULL );
		CScript::m_globalDef.GetNextAssoc( pos, strDefSrc, (void*&)pData );
		int i = 0;
		for( ; i < nLength; i++)
			if( strDefSrc[i] != strDefDst[ i ] )
				break;
		if( i == nLength )
		{
			lpDefineData = new DEFINEDATA;
			FLERROR_ASSERT_LOG_DO( lpDefineData, continue, _T("[리소스 메니져] Define Data 생성 실패2.") );			
			lpDefineData->strDefine = strDefSrc;
			lpDefineData->dwData = _ttoi( pData );
			lpDefineDataArray[ nDefineDataNum++ ] = lpDefineData;
		}
	}

	qsort( lpDefineDataArray, nDefineDataNum, sizeof( DEFINEDATA* ), SortDefine );

	for( int i = 0; i < nDefineDataNum; i++ )
	{
		file.PutString( _T( "#define " ) );
		lpDefineData = lpDefineDataArray[ i ];
		file.PutString( lpDefineData->strDefine );
		int nSpace = 32 - lpDefineData->strDefine.GetLength();

		for( int j = 0; j < nSpace; j++ )
			file.PutString( _T( " " ) );

		strTemp.Format( _T( "%d" ), lpDefineData->dwData );
		file.PutString( strTemp );
		file.PutString( _T( "\n" ) );
		SAFE_DELETE( lpDefineData );
	}

	nDefineDataNum = 0;

	SAFE_DELETE( lpDefineDataArray );

	file.PutString( _T( "\n" ) );
	file.PutString( _T( "#endif\n" ) );

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// Save ( 저장하기 )
// param	: lpszFileName
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CResManager::Save( LPCTSTR lpszFileName )
{
	if( SaveHeader( MakePath( "", RESDATA_HEADER ) ) == FALSE )
		return FALSE;

	if( IsModifiedScript() == FALSE )
	{
		AfxMessageBox( _T("[리소스 매니져] 수정 된 부분이 없어서 저장하지 않습니다. :)"), MB_OK );
		return TRUE;
	}

	TCHAR	szBuffer[256] = { NULL, };
	CFileIO file;

	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
	{
		_sntprintf_s( szBuffer, _countof(szBuffer), _TRUNCATE, _T("[리소스 매니져] 파일 열기 실패 - [ %s ]"), lpszFileName ); 
		AfxMessageBox( szBuffer, MB_OK );
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, _T("[리소스 매니져] 파일 열기 실패 - [ %s ]"), lpszFileName );
	}

	CString		str(_T(""));
	wchar_t		wszBuff[ 1024 ] = { NULL, };
	
	// BOM
	file.PutC( (char)0xff );
	file.PutC( (char)0xfe );

	str = "// window resource script\r\n";
	file.PutWideString( str );
		
	DWORD			dwAppletId( NULL );
	LPWNDAPPLET		lpWndApplet( NULL );

	for( WndAppletIter iter = m_mapWndApplet.begin(); iter != m_mapWndApplet.end(); ++iter )
	{
		dwAppletId = iter->first;
		lpWndApplet = iter->second;

		FLERROR_ASSERT_LOG_RETURN( dwAppletId, FALSE, _T("[리소스 매니져] 윈도우 애플릿 아이디가 0 입니다. 무시함 - ID : [ %u ]"), dwAppletId );
		FLERROR_ASSERT_LOG_RETURN( lpWndApplet, FALSE, _T("[리소스 매니져] 윈도우 애플릿 포인터가 유효하지 않습니다. 무시함  - ID : [ %u ]"), dwAppletId );

		// AppID
		str.Format( _T( "%s" ), CScript::GetFindIdStr( _T( "APP_" ), dwAppletId ) );
		if( str.IsEmpty() )
		{
			str.Format( _T( "%d" ), dwAppletId );
			file.PutWideString( str );
		}
		else
		{
			file.PutWideString( str );
		}

		str.Format( _T( " \"%s\"" ), lpWndApplet->strTexture );
		file.PutWideString( str );		

		str.Format( _T( " \"%s\"" ), lpWndApplet->strDefaultImg );
		file.PutWideString( str );		

		str.Format( _T( " %d" ), lpWndApplet->m_bTile );
		file.PutWideString( str );		
		
		str.Format( _T( " %d %d" ), lpWndApplet->size.cx, lpWndApplet->size.cy );
		file.PutWideString( str );		
		
		str.Format( _T( " 0x%x" ), lpWndApplet->dwWndStyle );
		file.PutWideString( str );		

		str.Format( _T( " %d\r\n" ), lpWndApplet->d3dFormat );
		file.PutWideString( str );		

#ifdef __USE_STR_TBL0407
		// Title string
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		str = "{\r\n// Title String\r\n";
		file.PutWideString( str );

		if( *lpWndApplet->m_szTitleKey == '\0' )	// new
		{
			CString strKey(_T("")), strtemp(_T(""));
			if( m_nString > 0 )
			{
				map<string, string>::iterator i	= CScript::m_mapString.begin();
				strtemp		= i->first.data();
				strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
			}
			else
			{
				strKey.Format( _T( "IDS_%s_INC_%06d" ), m_strStrFile.Left( m_strStrFile.GetLength() - 8 ), m_nString++ );
				strKey.MakeUpper();
			}
			
			lstrcpy( lpWndApplet->m_szTitleKey, (LPCTSTR)strKey );
			BOOL bResult = CScript::m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, (LPCTSTR)lpWndApplet->strTitle ) ).second;
		
			// flyingjin : Log
			if(!strncmp(lpWndApplet->strTitle,"IDS_",3))
			{
				FILEOUT( "Daisy_Log.txt","==================================================");
				FILEOUT( "Daisy_Log.txt","CResManager::Save_1 [TITLE : %s]",lpWndApplet->strTitle );
				FILEOUT( "Daisy_Log.txt","CResManager::Save_1 [DEFINE : %s]",lpWndApplet->strDefine );
				lpWndApplet->strTitle.Format("");
			}
		}
		else
		{
			CString strKey	= lpWndApplet->m_szTitleKey;
			map<string, string>::iterator i	= CScript::m_mapString.find( (LPCTSTR)strKey );
			if( i != CScript::m_mapString.end() )
			{
				CString strTitleOld		= i->second.data();
				if( lpWndApplet->strTitle != strTitleOld )
				{
					CScript::m_mapString.erase( i );
					BOOL bResult	= CScript::m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)lpWndApplet->strTitle ) ).second;
					//flyingjin : Log
					if(!strncmp(lpWndApplet->strTitle,"IDS_",3))
					{
						FILEOUT( "Daisy_Log.txt","==================================================");
						FILEOUT( "Daisy_Log.txt","CResManager::Save_2 [TITLE : %s]",lpWndApplet->strTitle );
						FILEOUT( "Daisy_Log.txt","CResManager::Save_2 [DEFINE : %s]",lpWndApplet->strDefine );
						lpWndApplet->strTitle.Format("");
					}
				}
			}
		}

		file.PutWideString( (LPCTSTR)lpWndApplet->m_szTitleKey );
		file.PutWideString( "\r\n}\r\n" );
		// ToolTip
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		str = "{\r\n// Help Key\r\n";
		file.PutWideString( str );
		if( *lpWndApplet->m_szToolTipKey == '\0' )	// new
		{
			CString strKey, strtemp;
			if( m_nString > 0 )
			{
				map<string, string>::iterator i	= CScript::m_mapString.begin();
				strtemp		= i->first.data();
				strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
			}
			else
			{
				strKey.Format( _T( "IDS_%s_INC_%06d" ), m_strStrFile.Left( m_strStrFile.GetLength() - 8 ), m_nString++ );
				strKey.MakeUpper();
			}
			lstrcpy( lpWndApplet->m_szToolTipKey, (LPCTSTR)strKey );
			BOOL bResult	= CScript::m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, (LPCTSTR)lpWndApplet->strToolTip ) ).second;
			//flyingjin :  Log
			if(!strncmp(lpWndApplet->strToolTip,"IDS_",3))
			{
				FILEOUT( "Daisy_Log.txt","==================================================");
				FILEOUT( "Daisy_Log.txt","CResManager::Save_3 [ToolTip: %s]",lpWndApplet->strToolTip );
				FILEOUT( "Daisy_Log.txt","CResManager::Save_3 [DEFINE : %s]",lpWndApplet->strDefine );
				lpWndApplet->strToolTip.Format("");
			}
		}
		else
		{
			CString strKey	= lpWndApplet->m_szToolTipKey;
			map<string, string>::iterator i	= CScript::m_mapString.find( (LPCTSTR)strKey );
			if( i != CScript::m_mapString.end() )
			{
				CString strToolTipOld		= i->second.data();
				if( lpWndApplet->strToolTip != strToolTipOld )
				{
					CScript::m_mapString.erase( i );
					BOOL bResult	= CScript::m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)lpWndApplet->strToolTip ) ).second;
					//flyingjin : Log
					if(!strncmp(lpWndApplet->strToolTip,"IDS_",3))
					{
						FILEOUT( "Daisy_Log.txt","==================================================");
						FILEOUT( "Daisy_Log.txt","CResManager::Save_4 [ToolTip: %s]",lpWndApplet->strToolTip );
						FILEOUT( "Daisy_Log.txt","CResManager::Save_4 [DEFINE : %s]",lpWndApplet->strDefine );
						lpWndApplet->strToolTip.Format("");
					}
				}
			}
		}

		file.PutWideString( lpWndApplet->m_szToolTipKey );
		str.Format( _T( "\r\n}\r\n" ) );
		file.PutWideString( str );
#else	// __USE_STR_TBL0407
		// Title string
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		MultiByteToWideChar( CP_ACP, 0, lpWndApplet->strTitle, -1, wszBuff, 1024 );
		wcscpy( lpWndApplet->awszTitle[ m_nLanguage ], wszBuff );
		str = "{\r\n// Title String\r\n";
		file.PutWideString( str );		

		for( int i = 0; i < LANG_MAX; i++ )
		{
			file.PutWideString( g_szLanguage[ i ] );
			file.PutWideString( " " );
			file.Write( lpWndApplet->awszTitle[ i ], wcslen( lpWndApplet->awszTitle[ i ] ) * 2 );
			file.PutWideString( "\r\n" );	
		}
		file.PutWideString( "}\r\n" );
	
		// ToolTip
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		MultiByteToWideChar( CP_ACP, 0, lpWndApplet->strToolTip, -1, wszBuff, 1024 );
		wcscpy( lpWndApplet->awszToolTip[ m_nLanguage ], wszBuff );
		str = "{\r\n// Help Key\r\n";
		file.PutWideString( str );		
		for( i = 0; i < LANG_MAX; i++ )
		{
			str.Format( _T( "%s " ), g_szLanguage[ i ] );
			file.PutWideString( str );		
			file.Write( lpWndApplet->awszToolTip[ i ], wcslen( lpWndApplet->awszToolTip[ i ]) * 2 );
			str.Format( _T( "\r\n" ) );
			file.PutWideString( str );		
		}

		str = "}\r\n";
		file.PutWideString( str );		
#endif	// __USE_STR_TBL0407

		// 본문
		str = "{\r\n";
		file.PutWideString( str );		

		for( int j = 0; j < lpWndApplet->ptrCtrlArray.GetSize(); j++ )
		{
			LPWNDCTRL pWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( j );
			str = "    ";
			file.PutWideString( str );
			
			switch( pWndCtrl->dwWndType )
			{
			case WTYPE_STATIC:
				str = "WTYPE_STATIC";
				break;
			case WTYPE_TEXT:
				str = "WTYPE_TEXT";
				break;
			case WTYPE_BUTTON:
				str = "WTYPE_BUTTON";
				break;
			case WTYPE_EDITCTRL: 
				str = "WTYPE_EDITCTRL";
				break;
			case WTYPE_LISTBOX: 
				str = "WTYPE_LISTBOX";
				break;
			case WTYPE_LISTCTRL:
				str = "WTYPE_LISTCTRL";
				break;
			case WTYPE_COMBOBOX:
				str = "WTYPE_COMBOBOX";
				break;
			case WTYPE_CUSTOM:
				str = "WTYPE_CUSTOM";
				break;
			case WTYPE_TABCTRL:
				str = "WTYPE_TABCTRL";
				break;
			case WTYPE_TREECTRL:
				str = "WTYPE_TREECTRL";
				break;
			}
			file.PutWideString( str );		
			str = " ";
			file.PutWideString( str );		
			
			// WNDID
			_mbstowcsz( wszBuff, pWndCtrl->strDefine, pWndCtrl->strDefine.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			// Texture string
			str.Format( _T( " \"%s\" " ), pWndCtrl->strTexture );
			file.PutWideString( str );
			
#ifdef __V050322_CACHEQUEST
			// Texture tile
			str.Format( _T( "%d " ), pWndCtrl->m_bTile );
			file.PutWideString( str );
#endif

			// size
			str.Format( _T( "%d %d %d %d " ), pWndCtrl->rect.left, pWndCtrl->rect.top, pWndCtrl->rect.right, pWndCtrl->rect.bottom );
			file.PutWideString( str );

			// style
			str.Format( _T( "0x%x " ),pWndCtrl->dwWndStyle );
			file.PutWideString( str );
			
			// visible, Group, Disabled, TabStop
			str.Format( _T( "%d %d %d %d %d %d %d\r\n" ), 
						pWndCtrl->m_bVisible, 
						pWndCtrl->m_bGroup, 
						pWndCtrl->m_bDisabled, 
						pWndCtrl->m_bTabStop,
						pWndCtrl->m_nFontColorR,
						pWndCtrl->m_nFontColorG,
						pWndCtrl->m_nFontColorB);
			file.PutWideString( str );
			
#ifdef __USE_STR_TBL0407
			// Title string
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			str = "    {\r\n    // Title String\r\n";
			file.PutWideString( str );
			if( *pWndCtrl->m_szTitleKey == '\0' )	// new
			{
				CString strKey, strtemp;
				if( m_nString > 0 )
				{
					map<string, string>::iterator i	= CScript::m_mapString.begin();
					strtemp		= i->first.data();
					strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
				}
				else
				{
					strKey.Format( _T( "IDS_%s_INC_%06d" ), m_strStrFile.Left( m_strStrFile.GetLength() - 8 ), m_nString++ );
					strKey.MakeUpper();
				}
				lstrcpy( pWndCtrl->m_szTitleKey, (LPCTSTR)strKey );
				BOOL bResult	= CScript::m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, (LPCTSTR)pWndCtrl->strTitle ) ).second;
				//flyingjin : Log
				if(!strncmp(pWndCtrl->strTitle,"IDS_",3))
				{
					FILEOUT( "Daisy_Log.txt","==================================================");
					FILEOUT( "Daisy_Log.txt","CResManager::Save_5 [TITLE: %s]",pWndCtrl->strTitle);
					FILEOUT( "Daisy_Log.txt","CResManager::Save_5 [DEFINE : %s]",pWndCtrl->strDefine );
					pWndCtrl->strTitle.Format("");
				}
			}
			else
			{
				CString strKey	= pWndCtrl->m_szTitleKey;
				map<string, string>::iterator i	= CScript::m_mapString.find( (LPCTSTR)strKey );
				if( i != CScript::m_mapString.end() )
				{
					CString strTitleOld		= i->second.data();
					if( pWndCtrl->strTitle != strTitleOld )
					{
						CScript::m_mapString.erase( i );
						BOOL bResult	= CScript::m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)pWndCtrl->strTitle ) ).second;
						//flyingjin : Log
						if(!strncmp(pWndCtrl->strTitle,"IDS_",3))
						{
							FILEOUT( "Daisy_Log.txt","==================================================");
							FILEOUT( "Daisy_Log.txt","CResManager::Save_6 [TITLE: %s]",pWndCtrl->strTitle);
							FILEOUT( "Daisy_Log.txt","CResManager::Save_6 [DEFINE : %s]",pWndCtrl->strDefine );
							pWndCtrl->strTitle.Format("");
						}
					}
				}
			}

			file.PutWideString( (LPCTSTR)pWndCtrl->m_szTitleKey );
			str.Format( _T( "\r\n    }\r\n" ) );
			file.PutWideString( str );
			// ToolTip
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			str = "    {\r\n    // ToolTip\r\n";
			file.PutWideString( str );
			if( *pWndCtrl->m_szToolTipKey == '\0' )	// new
			{
				CString strKey, strtemp;
				if( m_nString > 0 )
				{
					map<string, string>::iterator i	= CScript::m_mapString.begin();
					strtemp		= i->first.data();
					strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
				}
				else
				{
					strKey.Format( _T( "IDS_%s_INC_%06d" ), m_strStrFile.Left( m_strStrFile.GetLength() - 8 ), m_nString++ );
					strKey.MakeUpper();
				}
				lstrcpy( pWndCtrl->m_szToolTipKey, (LPCTSTR)strKey );
				BOOL bResult	= CScript::m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, (LPCTSTR)pWndCtrl->strToolTip ) ).second;
				//flyingjin : Log
				if(!strncmp(pWndCtrl->strToolTip,"IDS_",3))
				{
					FILEOUT( "Daisy_Log.txt","==================================================");
					FILEOUT( "Daisy_Log.txt","CResManager::Save_7 [TOOLTIP: %s]",pWndCtrl->strToolTip);
					FILEOUT( "Daisy_Log.txt","CResManager::Save_7 [DEFINE : %s]",pWndCtrl->strDefine );
					pWndCtrl->strToolTip.Format("");
				}
			}
			else
			{
				CString strKey	= pWndCtrl->m_szToolTipKey;
				map<string, string>::iterator i	= CScript::m_mapString.find( (LPCTSTR)strKey );
				if( i != CScript::m_mapString.end() )
				{
					CString strToolTipOld		= i->second.data();
					if( pWndCtrl->strToolTip != strToolTipOld )
					{
						CScript::m_mapString.erase( i );
						BOOL bResult	= CScript::m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)pWndCtrl->strToolTip ) ).second;
						//flyingjin : Log
						if(!strncmp(pWndCtrl->strToolTip,"IDS_",3))
						{
							FILEOUT( "Daisy_Log.txt","==================================================");
							FILEOUT( "Daisy_Log.txt","CResManager::Save_8 [TOOLTIP: %s]",pWndCtrl->strToolTip);
							FILEOUT( "Daisy_Log.txt","CResManager::Save_8 [DEFINE : %s]",pWndCtrl->strDefine );
							pWndCtrl->strToolTip.Format("");
						}
					}
				}
			}
			file.PutWideString( pWndCtrl->m_szToolTipKey );
			str.Format( _T( "\r\n    }\r\n" ) );
			file.PutWideString( str );
#else	// __USE_STR_TBL0407
			// Title string
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			MultiByteToWideChar( CP_ACP, 0, pWndCtrl->strTitle, -1, wszBuff, 1024 );
			wcscpy( pWndCtrl->awszTitle[ m_nLanguage ], wszBuff );
			str = "    {\r\n    // Title String\r\n";
			file.PutWideString( str );
			for(  i = 0; i < LANG_MAX; i++ )
			{
				str.Format( _T( "    %s " ), g_szLanguage[ i ] );//
				file.PutWideString( str );
				file.Write( pWndCtrl->awszTitle[ i ], wcslen( pWndCtrl->awszTitle[ i ]) * 2 );
				str.Format( _T( "\r\n" ), g_szLanguage[ i ] );//
				file.PutWideString( str );
			}
			str = "    }\r\n";
			file.PutWideString( str );
			
			// ToolTip
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			MultiByteToWideChar( CP_ACP, 0, pWndCtrl->strToolTip, -1, wszBuff, 1024 );
			wcscpy( pWndCtrl->awszToolTip[ m_nLanguage ], wszBuff );
			str = "    {\r\n    // ToolTip\r\n";
			file.PutWideString( str );
			for( i = 0; i < LANG_MAX; i++ )
			{
				str.Format( _T( "    %s " ), g_szLanguage[ i ] );//
				file.PutWideString( str );
				file.Write( pWndCtrl->awszToolTip[ i ], wcslen( pWndCtrl->awszToolTip[ i ]) * 2 );
				str.Format( _T( "\r\n" ), g_szLanguage[ i ] );//
				file.PutWideString( str );
			}
			//fontColor_R
			str.Format( _T( "%d " ), pWndCtrl->m_nFontColorR );
			file.PutWideString( str );
			str.Format( _T( "%d " ), pWndCtrl->m_nFontColorG );
			file.PutWideString( str );
			str.Format( _T( "%d " ), pWndCtrl->m_nFontColorB );
			file.PutWideString( str );

			str = "    }\r\n";
			file.PutWideString( str );

#endif	// __USE_STR_TBL0407
		}
		str = "\r\n}\r\n";
		file.PutWideString( str );
	}

#ifdef __USE_STR_TBL0407
	WriteString();
#endif	// __USE_STR_TBL0407

	return TRUE;
}
#endif


#ifdef __USE_STR_TBL0407
//---------------------------------------------------------------------------------
BOOL CResManager::LoadString( void )
{
	CScript::m_mapString.clear();

	CScript	s;
	if( s.Load( (LPCTSTR)m_strStrFile, FALSE ) == FALSE )
	{
		Error( "LoadString: file not found: %s", m_strStrFile );
		return FALSE;
	}

	s.GetToken_NoDef();
	while( s.tok != FINISHED )
	{
		CString str	= s.Token;
		if( str.Find( "IDS", 0 ) != 0 )
		{
			Error( "LoadString: %s", m_strStrFile );
			s.GetToken();
			continue;
		}
		s.GetLastFull();
		bool bResult	= CScript::m_mapString.insert( map<string, string>::value_type( (LPCTSTR)str, (LPCTSTR)s.Token ) ).second;
		TRACE( _T( "%s, %s" ), str, s.Token );

		if( !bResult )
		{
			Error( "string error: %s", str );
		}
		else
		{
			CString strKey	= str.Right( 6 );
			int nString		= atoi( (LPCSTR)strKey ) + 1;
			if( nString > m_nString )
				m_nString	= nString;
#ifdef _DEBUG
			char lpOutputString[1024] = { NULL, };
			if( s.Token.GetLength() < 1024 )
			{
				_tprintf_s( lpOutputString, _countof(lpOutputString), _T("string %s\t%s\n"), str, s.Token );
				OutputDebugString( lpOutputString );
			}
#endif	// _DEBUG
		}
		s.GetToken_NoDef();
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------------
// String 정보 저장하기
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CResManager::WriteString( void )
{
	CFileIO	FileIO;
	
	CString	strSaveFile( _T("") );
	strSaveFile.Format( _T("%s\\%s"), g_szWorkDirectory, m_strStrFile );
	
	if( FileIO.Open( strSaveFile, _T( "wb" ) ) == FALSE )
	{
		CString strErrorMsg( _T("") );
		strErrorMsg.Format( _T("[리소스 메니져] 문자열 저장 파일 열기를 실패 하였습니다 : 파일이름 - [%s]"), strSaveFile );
		
		AfxMessageBox( strErrorMsg, MB_OK );
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, strErrorMsg );
	}

	FileIO.PutC( (char)0xff );
	FileIO.PutC( (char)0xfe );
	int j = 0;
	for( map<string, string>::iterator i = CScript::m_mapString.begin(); i != CScript::m_mapString.end(); ++i )
	{
		TRACE( _T( "%s\t%s\n" ), i->first.data(), i->second.data() );
		FileIO.PutWideString( i->first.data() );
		FileIO.PutWideString( _T( "\t" ) );
		FileIO.PutWideString( i->second.data() );
		FileIO.PutWideString( _T( "\r\n" ) );
	}
	return TRUE;
}
#endif	// __USE_STR_TBL0407

//---------------------------------------------------------------------------------
void CResManager::DeleteHistory()
{
	m_phistorylog = NULL;

	if(!m_vecUndoData.empty())
	{
		while(m_vecUndoData.size())
		{
			(*m_vecUndoData.rbegin()) = NULL;
			m_vecUndoData.pop_back();
		}
	}
	
	if(!m_vecRedoData.empty())
	{
		while(m_vecRedoData.size())
		{
			(*m_vecRedoData.rbegin()) = NULL;
			m_vecRedoData.pop_back();
		}
	}
	m_nHistoryCount = 0;
}

//---------------------------------------------------------------------------------
void CResManager::SaveHistory(LPWNDCTRL lpWndDlgData)
{
	if(lpWndDlgData != NULL)
	{
		m_nHistoryCount++;
		lpWndDlgData->dwHistorypCnt = m_nHistoryCount;
		m_vecUndoData.push_back(lpWndDlgData);
		m_phistorylog = m_vecUndoData[m_vecUndoData.size()-1];
	}
}
//---------------------------------------------------------------------------------
void CResManager::UndoHistory(LPWNDAPPLET lpWndApplet,CWndBase* pWndBase, CWndDialog* pWndDialog, HWND hWnd)
{

	if(m_vecUndoData.empty()) return;
	LPWNDCTRL pHistoryData = m_vecUndoData[m_vecUndoData.size()-1];

	m_phistorylog = pHistoryData;
	switch(pHistoryData->dwWndHistoryType)
	{
	case CTRL_CREATE:
		{
			if(pWndBase->m_wndArray.GetUpperBound() == 0)
				return;
			pWndBase->Destroy();
			RemoveControl(pWndDialog->GetWndId(),pHistoryData->strDefine);	
		}
		break;
	case CTRL_REMOVE:
		{
			// 콘트롤 생성 
			CWndBase* pWndBase = pWndDialog->CreateControl( hWnd, pHistoryData );
			// 최종 추가 
			pHistoryData->dwWndStyle = pWndBase->GetStyle();	
			AddControl( pWndDialog->GetWndId(), pHistoryData);
		}
		break;
	case CTRL_CHANGE:
		{
			if(pHistoryData->dwWndType== WTYPE_BUTTON)
			{
				pWndBase->SetTexture( pWndBase->m_pApp->m_pd3dDevice,
					MakePath(DIR_THEME,CWndBase::m_resMng.GetSelectLang(),pHistoryData->strTexture),TRUE);
			}
			else
			{
				((CWndStatic*)pWndBase)->m_strTexture = pHistoryData->strTexture;
			}

			pWndBase->m_bTile = pHistoryData->m_bTile ? true : false;
			if(pHistoryData->strTitle.GetLength() > 0)
				pWndBase->SetTitle(pHistoryData->strTitle);
			pWndBase->SetWndRect(pHistoryData->rect);
			pWndBase->SetWndId(pHistoryData->dwWndId);
		}
		break;
	}
	m_vecRedoData.push_back(pHistoryData);
	pHistoryData = NULL;
	m_vecUndoData.pop_back();

}

//---------------------------------------------------------------------------------
void	CResManager::RedoHistory(LPWNDAPPLET lpWndApplet,CWndBase* pWndBase, CWndDialog* pWndDialog, HWND hWnd)
{
	if(m_vecRedoData.size() > 0)
	{
		LPWNDCTRL pRedoHistoryData = m_vecRedoData[m_vecRedoData.size()-1];

		switch(pRedoHistoryData->dwWndHistoryType)
		{
		case CTRL_CREATE:
			{	
				if(lpWndApplet != NULL) 
				{
					pRedoHistoryData->strDefine = GetNewControlDefine( lpWndApplet->dwWndId, pRedoHistoryData->strDefine );
					// 콘트롤 생성 
					CWndBase* pWndBase = pWndDialog->CreateControl( hWnd, pRedoHistoryData );
					// 최종 추가 8
					pRedoHistoryData->dwWndStyle = pWndBase->GetStyle();	
					AddControl( lpWndApplet->dwWndId, pRedoHistoryData);
				}				
			}
			break;
		case CTRL_REMOVE:
			{
				if(pWndBase->m_wndArray.GetUpperBound() == 0)
					break;
				pWndBase->Destroy();
				RemoveControl(lpWndApplet->dwWndId,pRedoHistoryData->strDefine);	
			}
			break;
		case CTRL_CHANGE:
			{
				CWndBase* pWndBaseOLD = pWndDialog->GetChildWnd(pRedoHistoryData->dwWndId);
				if(pWndBaseOLD)
				{
					if(pRedoHistoryData->dwWndType== WTYPE_BUTTON)
					{
						pWndBaseOLD->SetTexture( pWndBaseOLD->m_pApp->m_pd3dDevice,
							MakePath(DIR_THEME,CWndBase::m_resMng.GetSelectLang(),pRedoHistoryData->strTexture),TRUE);
					}
					else
					{
						((CWndStatic*)pWndBaseOLD)->m_strTexture = pRedoHistoryData->strTexture;
					}

					pWndBaseOLD->m_bTile = pRedoHistoryData->m_bTile ? true : false;
					if(pRedoHistoryData->strTitle.GetLength() > 0)
						pWndBaseOLD->SetTitle(pRedoHistoryData->strTitle);
					pWndBaseOLD->SetWndRect(pRedoHistoryData->rect);
					pWndBaseOLD->SetWndId(pRedoHistoryData->dwWndId);
				}
			}
			break;
		}

		//undo를 위해 
		m_vecUndoData.push_back(pRedoHistoryData);
		m_vecRedoData.pop_back();
	}	
}