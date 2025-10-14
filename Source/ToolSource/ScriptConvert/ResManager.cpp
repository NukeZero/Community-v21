#include "stdafx.h"
#include "lang.h"
#include "resmanager.h"

#define BEGIN_ID  100
#define WNDSTYLE_HEADER _T( "WndStyle.h" )

/*
APP_APPLET1 "콘트롤 테스트"  300 300
{
    WTYPE_BUTTONWIDC_BUTTON0 WIDC_BUTTON0 "Button" 0 0 100 20 0x260000 
    WTYPE_EDITCTRLWIDC_BUTTON1 WIDC_BUTTON1 "" 64 87 154 117 0x20000 
}
*/

LPCTSTR CResManager::AddDefine( LPCTSTR lpDefine, LPCTSTR lpDefData )
{
	//CHAR* pData;
	//if( CScript::LookupDefine( lpDefine, (void*&)pData ) == FALSE )
	//{
		CScript::AddDefine( lpDefine, lpDefData );
		m_bModifiedHeader = TRUE;
		return (LPCTSTR)lpDefData;
	//}
	//AfxMessageBox( "헉 동일 define이 있어서 추가 못했당. 보고바람." );
	//return pData;
}
int CResManager::AddDefine( LPCTSTR lpDefine, int nDefData )
{
	//CHAR* pData;
	//if( CScript::LookupDefine( lpDefine, (void*&)pData ) == FALSE )
	//{
		CScript::AddDefine( lpDefine, nDefData );
		m_bModifiedHeader = TRUE;
		return nDefData;
	//}
	//AfxMessageBox( "헉 동일 define이 있어서 추가 못했당. 보고바람." );
	//return atoi( pData );
}
void CResManager::RemoveDefine( LPCTSTR lpDefine )
{
	CScript::RemoveDefine( lpDefine );
	m_bModifiedHeader = TRUE;
}

LPWNDCTRL WNDAPPLET::GetAt( DWORD dwWndId )
{
	LPWNDCTRL pWndCtrl = NULL;
	for( int i = 0; i < ptrCtrlArray.GetSize(); i++ )
	{
		pWndCtrl = (LPWNDCTRL) ptrCtrlArray.GetAt( i );
		if( pWndCtrl->dwWndId == dwWndId )
			return pWndCtrl;
	}
	return NULL;
}

CResManager::CResManager()
{
	m_dwNewCtrlId = 0;
	m_bModifiedHeader = 0;
	m_bModifiedScript = 0;

#if __CURRENT_LANG == LANG_JAP
	m_nLanguage = LANG_JAP;
#else
	m_nLanguage = LANG_KOR; // default
#endif	

	/*
	CScript scriptHeader;
	if( scriptHeader.Load( _T( "WndStyle.h" ) ) )
		scriptHeader.PreScan( TRUE );
	scriptHeader.Free();
	if( scriptHeader.Load( _T( "WndID_.h" ) ) )
		scriptHeader.PreScan( TRUE );
	CWndBase::m_resMng.Load( _T( "Resource2.inc" ) );
	*/
}
CResManager::~CResManager()
{
	Free();
}
void CResManager::Free()
{
	DWORD dwAppletId;
	LPWNDAPPLET LPWNDAPPLET;
	POSITION pos 
		= m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)LPWNDAPPLET );
		for( int i = 0; i < LPWNDAPPLET->ptrCtrlArray.GetSize(); i++ )
			delete (LPWNDCTRL)LPWNDAPPLET->ptrCtrlArray.GetAt( i );
		SAFE_DELETE( LPWNDAPPLET );
	}
}
DWORD CResManager::GetNewAppletId()
{
	/*
	DWORD dwAppletId;
	LPWNDAPPLET LPWNDAPPLET;
	POSITION pos 
		= m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)LPWNDAPPLET );
		for( int i = 0; i < LPWNDAPPLET->ptrCtrlArray.GetSize(); i++ )
			delete LPWNDAPPLET->ptrCtrlArray.GetAt( i );
		SAFE_DELETE( LPWNDAPPLET );
	}
*/
	POSITION pos 
		= CScript::m_globalDef.GetStartPosition();
	CDWordArray idAppletArray;
	CString string;
	TCHAR* pData;
	DWORD dwId;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "APP_" ) ) == 0 )
		{
			dwId = _ttoi( pData ); 
			// dwId의 값을 id로 갖고 있는 Applet을 찾는다.
			//if( GetAt( dwId ) )
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
DWORD CResManager::GetNewControlId( DWORD dwAppletId )
{
	DWORD dwNewCtrlId = m_dwNewCtrlId;
	m_dwNewCtrlId++;
	return dwNewCtrlId;
	/*
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	CDWordArray idControlArray;
	for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
	{
		LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
		idControlArray.SetAtGrow( lpWndCtrl->dwWndId - BEGIN_ID, 1 );
	}
	for( i = 0; i < idControlArray.GetSize(); i++ )
	{
		if( idControlArray.GetAt( i ) == 0 )
			return i + BEGIN_ID;
	}
	return i + BEGIN_ID;
	*/
}

CString CResManager::GetNewAppletDefine()
{
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
CString CResManager::GetNewControlDefine( DWORD dwAppletId, LPCTSTR lpszBaseDefine )
{
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
void CResManager::ResetNewControlId()
{
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
BOOL CResManager::AddControl( DWORD dwAppletId, LPWNDCTRL pWndCtrl )
{
	LPWNDAPPLET pWndApplet = GetAt( dwAppletId );
	LPWNDCTRL pWndControl = new WNDCTRL;
	pWndControl->dwWndId    = pWndCtrl->dwWndId;
	pWndControl->dwWndStyle = pWndCtrl->dwWndStyle ;
	pWndControl->dwWndType  = pWndCtrl->dwWndType  ;
	pWndControl->rect       = pWndCtrl->rect       ;
	pWndControl->strDefine  = pWndCtrl->strDefine;
	pWndControl->strTitle   = pWndCtrl->strTitle;
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		pWndControl->awszTitle[ i ][0] = 0;
		pWndControl->awszToolTip[ i ][0] = 0;
	}
#endif
	//_tcscpy( pWndControl->szDefine  , pWndCtrl->szDefine   );
	//_tcscpy( pWndControl->szTitle   , pWndCtrl->szTitle    );

	// Define을 추가 
	/*
	TCHAR* szDefData = new TCHAR[ 32 ];
	_itot( pWndControl->dwWndId, szDefData, 10 );
	//AddDefine( pWndControl->szDefine, szDefData );
	AddDefine( pWndControl->strDefine, szDefData );
	*/
	AddDefine( pWndControl->strDefine, pWndControl->dwWndId );

	pWndApplet->ptrCtrlArray.Add( pWndControl );
	m_bModifiedScript = TRUE;

	return TRUE;
}

BOOL CResManager::AddApplet( LPWNDAPPLET pWndApp )
{
	// define이 이미 정의되어 있나?
//	LPSTR lpDefData = CScript::GetDefineStr( lpszDefine );
//	if( lpDefData ) return FALSE;

	LPWNDAPPLET pWndApplet = new WNDAPPLET;
	//ZeroMemory( pWndApplet, sizeof( WNDAPPLET ) );
	pWndApplet->pWndBase = pWndApp->pWndBase;
	pWndApplet->dwWndId = pWndApp->dwWndId;
	pWndApplet->size = pWndApp->size;
	pWndApplet->dwWndStyle = pWndApp->dwWndStyle;
	pWndApplet->strTitle = pWndApp->strTitle;
	pWndApplet->strDefine = pWndApp->strDefine;
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		pWndApplet->awszTitle[ i ][0] = 0;
		pWndApplet->awszToolTip[ i ][0] = 0;
	}
#endif
	// 새 아이디 할당 
	//DWORD dwNewId = GetNewAppletId();
	/*
	if( m_wndAppletIdStack.IsEmpty() == FALSE )
		dwNewId = m_wndAppletIdStack.Pop();
	else
		dwNewId = m_dwAppletId++;
		*/
	//pWndApplet->dwWndId = dwNewId;//GetNewAppletId();

	// Define을 추가 
	/*
	TCHAR* szDefData = new TCHAR[ 32 ];
	_itot( pWndApplet->dwWndId, szDefData, 10 );
	AddDefine( pWndApplet->strDefine, szDefData );
	*/
	AddDefine( pWndApplet->strDefine, pWndApplet->dwWndId );

	// Map에 추가. 정식 등록 
	m_mapWndApplet.SetAt( (void*)pWndApplet->dwWndId, pWndApplet );

	m_bModifiedScript = TRUE;
	return TRUE;
}
/*
BOOL CResManager::RemoveApplet( LPCTSTR lpszDefine )
{
	LPWNDAPPLET lpWndApplet;
	if( m_mapWndApplet.Lookup( lpszDefine, (void*&) lpWndApplet ) )
	{
		// Applet 제거 
		SAFE_DELETE( lpWndApplet );
		m_mapWndApplet.RemoveKey( lpszDefine );
		// 삭제된 Id를 스텍에 넣어서 보관
		//m_wndAppletIdStack.Push( dwAppletId );
		// Define에서 제거 
		CScript::RemoveDefine( lpszDefine );
		return TRUE;
	}
	return FALSE;
}
*/
BOOL CResManager::IsSameDefine( LPWNDAPPLET lpExceptionWndApplet, LPCTSTR lpszDefine )
{
	LPWNDAPPLET lpWndApplet;
	DWORD dwAppletId;
	POSITION pos 
		= m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)lpWndApplet );
		if( lpExceptionWndApplet != lpWndApplet )
		{
			for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
			{
				LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
				//if( _tcscmp( lpWndCtrl->szDefine, lpszDefine ) == 0 )
				if( lpWndCtrl->strDefine == lpszDefine )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
BOOL CResManager::RemoveControl( DWORD dwAppletId, LPCTSTR lpszDefine )
{
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
	{
		LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
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
			SAFE_DELETE( lpWndCtrl );
		
			m_bModifiedScript = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CResManager::RemoveApplet( DWORD dwAppletId )
{
	LPWNDAPPLET lpWndApplet;
	if( m_mapWndApplet.Lookup( (void*) dwAppletId, (void*&) lpWndApplet ) )
	{
		// 삭제할 애플렛에 포함된 콘트롤들의 define을 다른 곳에서 사용하는지
		// 확인하고 사용하지 않을 경우 삭제한다.
		for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
		{
			LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );
			//if( RemoveControl( dwAppletId, lpWndCtrl->szDefine ) )
			if( RemoveControl( dwAppletId, lpWndCtrl->strDefine ) )
				i--;
		}
		// Applet 제거 
		SAFE_DELETE( lpWndApplet );
		m_mapWndApplet.RemoveKey( (void*)dwAppletId );
		// 삭제된 Id를 스텍에 넣어서 보관
		//m_wndAppletIdStack.Push( dwAppletId );
		// Define에서 제거 
		//CString string =
		//	CScript::GetFindIdStr( _T( "APP_" ), dwAppletId );
		//CScript::RemoveDefine( string );

		m_bModifiedScript = TRUE;
		return TRUE;
	}
	return FALSE;
}
BOOL CResManager::ChangeAppletDefine( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC, DWORD* lpdwNewWndId )
{
	if( _tcscmp( lpszOldWIDC, lpszNewWIDC ) != 0  )
	{
		DWORD dwOldAppletId;
		DWORD dwNewAppletId;
		LPWNDAPPLET lpWndApplet;
		LPSTR lpDefData = CScript::GetDefineStr( lpszNewWIDC );
		// 새 아이디가 이미 존재하는 것과 같다. 그것은 둘중에 하나다.
		// 1.외부에서 정의된 경우, 2.실제로 사용되고 있는 경우
		// 2.실제로 사용된 경우만 파악하면 된다.
		if( lpDefData )
		{
			DWORD dwAppletId;
			dwNewAppletId = CScript::GetDefineNum( lpszNewWIDC );
			POSITION pos 
				= m_mapWndApplet.GetStartPosition();
			while(pos)
			{ 
				m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)lpWndApplet );
				if( dwNewAppletId == dwAppletId )
					return FALSE;
			}
			// 이 경우는 1의 경우로 실제로 사용되고 있지는 않지만 외부에서 임의로 지정된 것이다.
			// 단지 define 이름만 바꿔주면 된다.
			dwOldAppletId = CScript::GetDefineNum( lpszOldWIDC );
			dwNewAppletId = CScript::GetDefineNum( lpszNewWIDC );
			lpWndApplet = GetAt( dwOldAppletId );
			lpWndApplet->strDefine = lpszNewWIDC;
			lpWndApplet->dwWndId = dwNewAppletId;
			m_mapWndApplet.RemoveKey( (void*)dwOldAppletId );
			m_mapWndApplet.SetAt( (void*)dwNewAppletId, lpWndApplet );
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
			m_mapWndApplet.RemoveKey( (void*)dwOldAppletId );
			m_mapWndApplet.SetAt( (void*)dwNewAppletId, lpWndApplet );
			if( lpdwNewWndId )
				*lpdwNewWndId = dwNewAppletId;
			m_bModifiedScript = TRUE;
		}
		/*
		DWORD dwAppletId = CScript::GetDefineNum( lpszOldWIDC );
		CScript::RemoveDefine( lpszOldWIDC );
		CScript::AddDefine( lpszNewWIDC, strApplet );
		LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
		lpWndApplet->strDefine = lpszNewWIDC;
		*/
	}
	else
	if( lpdwNewWndId )
		*lpdwNewWndId = CScript::GetDefineNum( lpszOldWIDC );


	return TRUE;
}
//FUCK 10
//SUCK 10

//SUCK->DUCK
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
				if( lpWndCtrl->strDefine == lpszNewWIDC )
					return FALSE;
				// 같은 아이디가 있나?
				if( dwNewId == lpWndCtrl->dwWndId )
					return FALSE;
			}
		}
		//DWORD dwCtrlId = CScript::GetDefineNum( lpszOldWIDC );
		//LPSTR lpDefData = CScript::GetDefineStr( lpszNewWIDC );
		// 전체에서 같은 Define이 없이 lpszOldWIDC가 유일했다면 삭제해두 된다.
		if( IsSameDefine( lpWndApplet, lpszOldWIDC ) == FALSE )
		{
			CString str;
			str.Format( "ChangeControlDefine %s 중복이 없어서 영구 제거함", lpszOldWIDC );
			AfxMessageBox( str );
		}
		if( IsSameDefine( lpWndApplet, lpszOldWIDC ) == FALSE )
		{
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

/*
BOOL CResManager::ChangeAppletDefine( LPCTSTR lpszOldDefine,LPCTSTR lpszNewDefine )
{
	CScript::GetDefineStr( lpszOldDefine )
	LPWNDAPPLET lpWndApplet;
	if( m_mapWndApplet.Lookup( (void*) dwAppletId, (void*&) lpWndApplet ) )
	{
		// Applet 제거 
		SAFE_DELETE( lpWndApplet );
		m_mapWndApplet.RemoveKey( (void*) dwAppletId );
		// 삭제된 Id를 스텍에 넣어서 보관
		//m_wndAppletIdStack.Push( dwAppletId );
		// Define에서 제거 
		CString string =
			CScript::GetFindIdStr( _T( "APP_" ), dwAppletId );
		CScript::RemoveDefine( string );
		return TRUE;
	}
	return FALSE;
}
*/
/*
void CResManager::RenameAppletDefine( DWORD dwAppletId )
{

}
void CResManager::RemoveApplet( DWORD dwAppletId )
{

}
*/
//DWORD CResManager::AddApplet( LPCTSTR lpszDefine, LPWNDAPPLET pWndApplet )
//{
	//if( m_wndAppletIdStack.IsEmpty() == FALSE )
	//	return m_wndAppletIdStack.Pop();
	//return m_dwWndId++;
/*
	DWORD dwWndId;
	LPWNDAPPLET LPWNDAPPLET;
	POSITION pos 
		= m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwWndId, (void*&)LPWNDAPPLET );
		for( int i = 0; i < LPWNDAPPLET->ptrCtrlArray.GetSize(); i++ )
			delete LPWNDAPPLET->ptrCtrlArray.GetAt( i );
		LPWNDAPPLET->dwWndId
		
	}
	*/
//}
LPWNDAPPLET CResManager::GetAt( DWORD dwAppletId )
{
	LPWNDAPPLET LPWNDAPPLET = NULL;
	m_mapWndApplet.Lookup( (void*)dwAppletId, (void*&) LPWNDAPPLET );
	return LPWNDAPPLET;
}
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
CString CResManager::GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip )
{
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		if( bToolTip )
			lpWndApplet->awszToolTip[ i ][0] = 0;
		else
			lpWndApplet->awszTitle[ i ][0] = 0;
	}
#endif
	CString string;
	int nLang = 0;
	scanner.GetToken(); // {
	nLang = scanner.GetNumber(); // lang
	do
	{
#ifdef __VMULTILANG
		scanner.GetLastFull(); // string
#else
		scanner.GetToken(); // string
#endif		
		if( m_nLanguage == nLang )
			string = scanner.token;
#ifdef __DAISY
		if( bToolTip )
			wcscpy( lpWndApplet->awszToolTip[ nLang ], scanner.m_wszToken );
		else
			wcscpy( lpWndApplet->awszTitle[ nLang ], scanner.m_wszToken );
#endif
		nLang = scanner.GetNumber(); // lang or '}'
	} while( *scanner.token != '}' );		
	//if( string.IsEmpty() )
	//	string = "Empty";
	return string;
}
CString CResManager::GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip )
{
#ifdef __DAISY
	for( int i = 0; i < LANG_MAX; i++ )
	{
		if( bToolTip )
			lpWndCtrl->awszToolTip[ i ][0] = 0;
		else
			lpWndCtrl->awszTitle[ i ][0] = 0;
	}
#endif
	CString string;
	int nLang = 0;
	scanner.GetToken(); // {
	nLang = scanner.GetNumber(); // lang
	do
	{
#ifdef __VMULTILANG
		scanner.GetLastFull(); // string
#else
		//scanner.GetToken(); // string
#endif
		if( m_nLanguage == nLang )
			string = scanner.token;
#ifdef __DAISY
		if( bToolTip )
			wcscpy( lpWndCtrl->awszToolTip[ nLang ], scanner.m_wszToken );
		else
			wcscpy( lpWndCtrl->awszTitle[ nLang ], scanner.m_wszToken );
#endif
		nLang = scanner.GetNumber(); // lang or '}'
	} while( *scanner.token != '}' );		
	//if( string.IsEmpty() )
	//	string = "Empty";
	return string;
}

BOOL CResManager::Load( LPCTSTR lpszName )
{
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
	else
	{
		AfxMessageBox( "작업에 필요한 WndStyle.h파일이 없습니다." );
		return FALSE;
	}

	scriptHeader.Free();

	if( scriptHeader.Load( RESDATA_HEADER ) )
		scriptHeader.PreScan( FALSE );
	else
	{
		AfxMessageBox( "작업에 필요한 ResData.h파일이 없습니다." );
		return FALSE;
	}
#endif
	////////////////////////////////////////////////////////////////////
	CScript scanner;
	if( scanner.Load( lpszName, FALSE ) == FALSE )
		return FALSE;
	DWORD dwAppletId;
	DWORD dwCtrlId;
	DWORD dwWndType;
	//script.CScanner::GetToken();//= script.GetNumber();
	scanner.GetToken_NoDef();

	while( scanner.tok != FINISHED )
	{
		LPWNDAPPLET pWndApplet = new WNDAPPLET;
		pWndApplet->pWndBase = NULL;
		pWndApplet->strDefine = scanner.token;
		pWndApplet->dwWndId = CScript::GetDefineNum( scanner.token );
		/*
		TRACE( "%s\n", script.token );
		if( script.Token == "APP_DELETE_CHAR" ) //pWndApplet->dwWndId == 145 )
		{
			int a;
			a++;
		}
		*/
		//script.GetToken();
		//pWndApplet->strTitle = script.Token;
#ifdef __SCRIPT_TOOL
		scanner.GetLastFull();
		pWndApplet->strTexture = scanner.token;	
/*
		int n = pWndApplet->strTexture.ReverseFind( '{' );
		if( n != -1 )
		{
			CString sss = pWndApplet->strTexture;
			pWndApplet->strTexture = sss.Left( n - 1 );
		}
*/
#else // __SCRIPT_TOOL
		scanner.GetToken();
		pWndApplet->strTexture = scanner.token;//m_szToken;//Char;
		pWndApplet->bTile = scanner.GetNumber();
		CSize size;
		pWndApplet->size.cx = scanner.GetNumber();
		pWndApplet->size.cy = scanner.GetNumber();
		pWndApplet->dwWndStyle = scanner.GetNumber();
#endif //__SCRIPT_TOOL
		// 타이틀 
		pWndApplet->strTitle = GetLangApplet( scanner, pWndApplet, FALSE );
		// 핼프 키 
		pWndApplet->strToolTip = GetLangApplet( scanner, pWndApplet, TRUE );
		
		// HelpKey
//		script.GetToken();
//		strcpy( pWndApplet->szHelpKey, script.Token );
		
		//pWndApplet->dwWndStyle = pWndApplet->dwWndStyle & ~WBS_THICKFRAME;
		m_mapWndApplet.SetAt( (void*)pWndApplet->dwWndId, pWndApplet );
		scanner.GetToken(); // skip {
		dwWndType = scanner.GetNumber(); 
		while( *scanner.token != '}' )
		{
			LPWNDCTRL pWndCtrl = new WNDCTRL;
			pWndCtrl->dwWndType = dwWndType;
			scanner.GetToken_NoDef();
			//script.CScanner::GetToken();
			//_tcscpy( pWndCtrl->szDefine, script.token );
			pWndCtrl->strDefine = scanner.token;///Char;
			pWndCtrl->dwWndId = CScript::GetDefineNum( scanner.token );
			//script.GetToken();
			//_tcscpy( pWndCtrl->szTitle, script.Token );
			//pWndCtrl->strTitle = script.Token;
#ifdef __SCRIPT_TOOL
			scanner.GetLastFull();
			pWndCtrl->strTexture = scanner.token;	
/*
			CString sss = pWndCtrl->strTexture;
			pWndCtrl->strTexture = sss.Mid( 1, sss.GetLength() - 2 );
*/

#else // __SCRIPT_TOOL			
			scanner.GetToken();
			pWndCtrl->strTexture = scanner.token;//token;
			pWndCtrl->rect.left = scanner.GetNumber();
			pWndCtrl->rect.top = scanner.GetNumber();
			pWndCtrl->rect.right = scanner.GetNumber();
			pWndCtrl->rect.bottom = scanner.GetNumber();
			pWndCtrl->dwWndStyle = scanner.GetNumber();
			// visible, Group, Disabled, TabStop
			pWndCtrl->m_bVisible = scanner.GetNumber();
			pWndCtrl->m_bGroup = scanner.GetNumber();
			pWndCtrl->m_bDisabled = scanner.GetNumber();
			pWndCtrl->m_bTabStop = scanner.GetNumber();
#endif //__SCRIPT_TOOL			

			//script.GetToken();
			//pWndCtrl->strToolTip = script.Token;

			// 타이틀 
			pWndCtrl->strTitle = GetLangCtrl( scanner, pWndCtrl, FALSE );
			// 핼프 키 
			pWndCtrl->strToolTip = GetLangCtrl( scanner, pWndCtrl, TRUE );
			
			pWndApplet->ptrCtrlArray.Add( pWndCtrl );
			dwWndType = scanner.GetNumber(); 
		} 
		scanner.GetToken_NoDef();
		//script.CScanner::GetToken();
	} 
	// 사용하지 않는 Define을 제거하고, 마지막 숫자 카운트를 얻는다.
	POSITION pos = CScript::m_globalDef.GetStartPosition();
	CString string;
	LPSTR pData;
	CDWordArray idAppletArray;
	CDWordArray idCtrlArray;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "APP_" ) ) == 0 )
		{
			dwAppletId = _ttoi( pData ); 
			// dwId의 값을 id로 갖고 있는 Applet을 찾는다.
			if( GetAt( dwAppletId ) )
			{
				//idAppletArray.SetAtGrow( dwId, 1 );
				//if( dwId > m_dwAppletId )
				//	m_dwAppletId = dwId;
			}
			else
			{
				// 못찾았으면 현재 define도 불필요하다. 제거.
				//CScript::RemoveDefine( string );
			}
		}
		else
		if( string.Find( _T( "WIDC_" ) ) == 0 )
		{
			dwCtrlId = _ttoi( pData ); 
			if( dwCtrlId > m_dwNewCtrlId )
				m_dwNewCtrlId = dwCtrlId;

			/*
			if( GetAt( dwId ) )
			{
				idCtrlArray.SetAtGrow( dwId, 1 );
				if( dwId > m_dwCtrlId )
					m_dwCtrlId = dwId;
			}
			else
			{
				// 못찾았으면 현재 define도 불필요하다. 제거.
				CScript::RemoveDefine( string );
			}
			*/
		}
	}	
	m_dwNewCtrlId++;

	/*
	m_dwAppletId++;
	m_dwCtrlId++;
	// ID와 ID사이에 비어있는 것을 찾아서 스텍에 넣어둔다.
	// 차후 새 Id 발급시 스텍에 있는 것을 먼저 소비한다.
	for( int i = 0; i < idAppletArray.GetSize(); i++ )
	{
		if( idAppletArray.GetAt( i ) == 0 )
			m_wndAppletIdStack.Push( i );
	}
	for( i = 0; i < idCtrlArray.GetSize(); i++ )
	{
		if( idCtrlArray.GetAt( i ) == 0 )
			m_wndCtrlIdStack.Push( i );
	}
	*/
	CScript::m_localDef.Free();
	return TRUE;
}
#ifdef __DAISY
int SortDefine( const VOID* arg1, const VOID* arg2 )
{
	LPDEFINEDATA* lpDefineData1 = (DEFINEDATA**)arg1;
	LPDEFINEDATA* lpDefineData2 = (DEFINEDATA**)arg2;
	if( (*lpDefineData1)->dwData > (*lpDefineData2)->dwData )
		return 1;
	if( (*lpDefineData1)->dwData < (*lpDefineData2)->dwData )
		return -1;
    return 0;
}
BOOL CResManager::SaveHeader( LPCTSTR lpszFileName )
{
	if( IsModifiedHeader() == FALSE )
		return TRUE;
		
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wt" ) ) == FALSE )
		return FALSE;
	CString string;
	CStringArray stringArray;
	file.PutString( _T( "#ifndef __RESOURCE_H\n" ) ); 
	file.PutString( _T( "#define __RESORUCE_H\n" ) );
	file.PutString( _T( "\n" ) );
	file.PutString( _T( "// Applet Id\n" ) );

	LPDEFINEDATA* lpDefineDataArray = new LPDEFINEDATA[ CScript::m_globalDef.GetCount() ];
	LPDEFINEDATA lpDefineData;
	int nDefineDataNum = 0;
	CString strTemp;
	CString strDefSrc;
	CString strDefDst = _T( "APP_" );
	
	POSITION pos = CScript::m_globalDef.GetStartPosition();
	int nLength = strDefDst.GetLength();
	while(pos)
	{ 
		TCHAR* pData;
		CScript::m_globalDef.GetNextAssoc( pos, strDefSrc, (void*&)pData );
		int i = 0;
		for( ; i < nLength; i++)
			if( strDefSrc[i] != strDefDst[ i ] )
				break;
		if( i == nLength )
		{
			lpDefineData = new DEFINEDATA;
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
	file.PutString( _T( "\n" ) );
	file.PutString( _T( "// Control Id\n" ) );
	nDefineDataNum = 0;
	strDefDst = _T( "WIDC_" );
	pos = CScript::m_globalDef.GetStartPosition();
	nLength = strDefDst.GetLength();
	while(pos)
	{ 
		TCHAR* pData;
		CScript::m_globalDef.GetNextAssoc( pos, strDefSrc, (void*&)pData );
		int i = 0;
		for( ; i < nLength; i++)
			if( strDefSrc[i] != strDefDst[ i ] )
				break;
		if( i == nLength )
		{
			lpDefineData = new DEFINEDATA;
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
	/*
	CScript::GetFindIdToArray( _T( "APP_" ), &stringArray );
	for( int i = 0; i < stringArray.GetSize(); i++ )
	{
		file.PutString( _T( "#define " ) );
		string = stringArray.GetAt( i );
		file.PutString( string );
		file.PutString( _T( " " ) );
		file.PutString( CScript::GetDefineStr( string ) );
		file.PutString( _T( "\n" ) );
	}
	file.PutString( _T( "\n" ) );
	file.PutString( _T( "// Control Id\n" ) );
	stringArray.RemoveAll();
	CScript::GetFindIdToArray( _T( "WIDC_" ), &stringArray );
	for( i = 0; i < stringArray.GetSize(); i++ )
	{
		file.PutString( _T( "#define " ) );
		string = stringArray.GetAt( i );
		file.PutString( string );
		file.PutString( _T( " " ) );
		file.PutString( CScript::GetDefineStr( string ) );
		file.PutString( _T( "\n" ) );
	}
	*/
	SAFE_DELETE( lpDefineDataArray );
	file.PutString( _T( "\n" ) );
	file.PutString( _T( "#endif\n" ) );
	return TRUE;
}
BOOL CResManager::Save( LPCTSTR lpszFileName )
{
	if( SaveHeader( MakePath( "", RESDATA_HEADER ) ) == FALSE )
		return FALSE;

	if( IsModifiedScript() == FALSE )
		return TRUE;

	CFileIO file;
	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
	//if( file.Open( "test.txt", _T( "wb" ) ) == FALSE )
		return FALSE; //fopen

	CString string;
	wchar_t wszBuff[ 1024 ];
	
	file.PutC( 0xff );
	file.PutC( 0xfe );

	string = "// window resource script\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	
/*
	FILE* fp;
	fp = _wfopen( L"test.txt", L"wt" );

	//stl
	wchar_t a[100] = L"abc";
	//file.PutWString( a );
	fwprintf( fp, a );

	fclose( fp );
*/

	
	DWORD dwAppletId;
	LPWNDAPPLET lpWndApplet;
	POSITION pos 
		= m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)lpWndApplet );
		//file.PutString( _T( "applet " ) );
		// AppID
		string.Format( _T( "%s" ), CScript::GetFindIdStr( _T( "APP_" ), dwAppletId ) );
		if( string.IsEmpty() )
		{
			string.Format( _T( "%d" ), dwAppletId );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			//file.PutString( string );
		}
		else
		{
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );

			//file.PutString( string );
		}
		// Texture string
#ifdef __SCRIPT_TOOL
		string.Format( _T( " %s\r\n" ), lpWndApplet->strTexture );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
#else //__SCRIPT_TOOL		
		string.Format( _T( " \"%s\"" ), lpWndApplet->strTexture );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
#endif //__SCRIPT_TOOL
		
#ifndef __SCRIPT_TOOL
		//file.PutString( string );
		string.Format( _T( " %d" ), lpWndApplet->bTile );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		
		//file.PutString( string );
		// Size
		string.Format( _T( " %d %d" ), lpWndApplet->size.cx, lpWndApplet->size.cy );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		
		// Style
		string.Format( _T( " 0x%x\r\n" ), lpWndApplet->dwWndStyle );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
#endif //__SCRIPT_TOOL
		
		// Title string
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		MultiByteToWideChar( CP_ACP, 0, lpWndApplet->strTitle, -1, wszBuff, 1024 );
		wcscpy( lpWndApplet->awszTitle[ m_nLanguage ], wszBuff );
		string = "{\r\n// Title String\r\n";
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		for( int i = 0; i < LANG_MAX; i++ )
		{
			file.PutWideString( g_szLanguage[ i ] );
			file.PutWideString( " " );
			file.Write( lpWndApplet->awszTitle[ i ], wcslen( lpWndApplet->awszTitle[ i ] ) * 2 );
			file.PutWideString( "\r\n" );
			/*
			string.Format( _T( "%s " ), g_szLanguage[ i ] );//
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.Write( lpWndApplet->awszTitle[ i ], wcslen( lpWndApplet->awszTitle[ i ]) * 2 );
			string.Format( _T( "\r\n" ) );//, g_szLanguage[ i ] );//
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			*/
			
		}
		file.PutWideString( "}\r\n" );
		//string = "}\r\n";
		//_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		//file.Write( wszBuff, wcslen( wszBuff) * 2 );
		
		// ToolTip
		// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
		MultiByteToWideChar( CP_ACP, 0, lpWndApplet->strToolTip, -1, wszBuff, 1024 );
		wcscpy( lpWndApplet->awszToolTip[ m_nLanguage ], wszBuff );
		string = "{\r\n// Help Key\r\n";
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		for( int i = 0; i < LANG_MAX; i++ )
		{
			string.Format( _T( "%s " ), g_szLanguage[ i ] );//
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.Write( lpWndApplet->awszToolTip[ i ], wcslen( lpWndApplet->awszToolTip[ i ]) * 2 );
			string.Format( _T( "\r\n" ) );//, g_szLanguage[ i ] );//
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
		}
		string = "}\r\n";
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );

		// 본문
		string = "{\r\n";
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		for( int j = 0; j < lpWndApplet->ptrCtrlArray.GetSize(); j++ )
		{
			LPWNDCTRL pWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( j );
			string = "    ";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			switch( pWndCtrl->dwWndType )
			{
			case WTYPE_STATIC:
				string = "WTYPE_STATIC";
				break;
			case WTYPE_TEXT:
				string = "WTYPE_TEXT";
				break;
			case WTYPE_BUTTON:
				string = "WTYPE_BUTTON";
				break;
				
			//case CTRL_CHECK:   
				//file.PutString( _T( "CTRL_CHECK " ) );
				//file.PutString( CScript::GetFindIdStr( _T( "WIDC_" ), pWndChild->GetWndId() ) );
				//break;
			//case CTRL_RADIO:   
			//	file.PutString( _T( "CTRL_RADIO " ) );
			//	file.PutString( CScript::GetFindIdStr( _T( "WIDC_" ), pWndChild->GetWndId() ) );
			//	break;
				
			case WTYPE_EDITCTRL: 
				string = "WTYPE_EDITCTRL";
				break;
			case WTYPE_LISTBOX: 
				string = "WTYPE_LISTBOX";
				break;
			case WTYPE_LISTCTRL:
				string = "WTYPE_LISTCTRL";
				break;
			case WTYPE_COMBOBOX:
				string = "WTYPE_COMBOBOX";
				break;
			case WTYPE_CUSTOM:
				string = "WTYPE_CUSTOM";
				break;
			case WTYPE_TABCTRL:
				string = "WTYPE_TABCTRL";
				break;
			case WTYPE_TREECTRL:
				string = "WTYPE_TREECTRL";
				break;
			}
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			string = " ";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			// WNDID
			_mbstowcsz( wszBuff, pWndCtrl->strDefine, pWndCtrl->strDefine.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
#ifdef __SCRIPT_TOOL
			string.Format( _T( " %s\r\n" ), pWndCtrl->strTexture );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
#else //__SCRIPT_TOOL					
			// Texture string
			string.Format( _T( " \"%s\" " ), pWndCtrl->strTexture );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
#endif //__SCRIPT_TOOL
			
#ifndef __SCRIPT_TOOL
			// size
			string.Format( _T( "%d %d %d %d " ), pWndCtrl->rect.left, pWndCtrl->rect.top, pWndCtrl->rect.right, pWndCtrl->rect.bottom );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );

			// style
			string.Format( _T( "0x%x " ),pWndCtrl->dwWndStyle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			// visible, Group, Disabled, TabStop
			string.Format( _T( "%d %d %d %d\r\n" ), pWndCtrl->m_bVisible, pWndCtrl->m_bGroup, pWndCtrl->m_bDisabled, pWndCtrl->m_bTabStop );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
#endif //__SCRIPT_TOOL			

			// Title string
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			MultiByteToWideChar( CP_ACP, 0, pWndCtrl->strTitle, -1, wszBuff, 1024 );
			wcscpy( pWndCtrl->awszTitle[ m_nLanguage ], wszBuff );
			string = "    {\r\n    // Title String\r\n";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			for( int i = 0; i < LANG_MAX; i++ )
			{
				string.Format( _T( "    %s " ), g_szLanguage[ i ] );//
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.Write( pWndCtrl->awszTitle[ i ], wcslen( pWndCtrl->awszTitle[ i ]) * 2 );
				string.Format( _T( "\r\n" ), g_szLanguage[ i ] );//
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
			}
			string = "    }\r\n";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			// ToolTip
			// 툴에서 입력된 데이타는 멀티바이트. 저장할 때는 와이드로 바꿔야한다.
			MultiByteToWideChar( CP_ACP, 0, pWndCtrl->strToolTip, -1, wszBuff, 1024 );
			wcscpy( pWndCtrl->awszToolTip[ m_nLanguage ], wszBuff );
			string = "    {\r\n    // ToolTip\r\n";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			for( int i = 0; i < LANG_MAX; i++ )
			{
				string.Format( _T( "    %s " ), g_szLanguage[ i ] );//
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.Write( pWndCtrl->awszToolTip[ i ], wcslen( pWndCtrl->awszToolTip[ i ]) * 2 );
				string.Format( _T( "\r\n" ), g_szLanguage[ i ] );//
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
			}
			string = "    }\r\n";
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
		}
		string = "\r\n}\r\n";
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
	}
	return TRUE;
}
#endif
/*
void CResManager::Save( LPCTSTR lpszFileName )
{
	SaveHeader( _T( "WndId_.h" ) );

	CFileIO file;
	if( file.Open( lpszFileName, _T( "wt" ) ), FALSE )
		return;
	CString string;
	file.PutString( _T( "// window resource script\n\n" ) );
	//CStringArray stringArray;
	//CScript::GetFindIdToArray( _T( "APP_" ), &stringArray );
	//for( int i = i; i < stringArray.GetSize(); i++ )

	CString stringTemp;
	for( int i = 0; i < CWndBase::m_wndOrder.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*)CWndBase::m_wndOrder.GetAt( i );
		if( pWndBase->IsWndStyle( WBS_MANAGER ) )
			continue;
		CRect rectWindow = pWndBase->GetWindowRect();

		//file.PutString( _T( "applet " ) );
		// AppID
		string.Format( _T( "%s" ), CScript::GetFindIdStr( _T( "APP_" ), pWndBase->GetWndId() ) );
		if( string.IsEmpty() )
		{
			string.Format( _T( "%d" ), pWndBase->GetWndId() );
			file.PutString( string );
		}
		else
			file.PutString( string );
		// Title string
		string.Format( _T( " \"%s\" " ), pWndBase->GetTitle() );
		file.PutString( string );
		// Size
		string.Format( _T( " %d %d" ), rectWindow.Width(), rectWindow.Height() );
		file.PutString( string );
		// Style
		string.Format( _T( " 0x%x" ), pWndBase->m_dwStyle );
		file.PutString( string );

		file.PutString( _T( "\n{\n" ) );
		for( int j = 0; j < pWndBase->m_wndArray.GetSize(); j++ )
		{
			CWndBase* pWndChild = (CWndBase*)pWndBase->m_wndArray.GetAt( j );
			if( pWndChild->IsWndStyle( WBS_DOCKING ) )
				continue;
			file.PutString( _T( "    " ) );
			switch( pWndChild->GetWndType() )
			{
			case WTYPE_STATIC:
				file.PutString( _T( "WTYPE_STATIC" ) );
				break;
			case WTYPE_BUTTON:
				file.PutString( _T( "WTYPE_BUTTON" ) );
				break;
				
			//case CTRL_CHECK:   
				//file.PutString( _T( "CTRL_CHECK " ) );
				//file.PutString( CScript::GetFindIdStr( _T( "WIDC_" ), pWndChild->GetWndId() ) );
				//break;
			//case CTRL_RADIO:   
			//	file.PutString( _T( "CTRL_RADIO " ) );
			//	file.PutString( CScript::GetFindIdStr( _T( "WIDC_" ), pWndChild->GetWndId() ) );
			//	break;
				
			case WTYPE_EDITCTRL: 
				file.PutString( _T( "WTYPE_EDITCTRL" ) );
				break;
			case WTYPE_LISTBOX: 
				file.PutString( _T( "WTYPE_LISTBOX" ) );
				break;
			case WTYPE_LISTCTRL:
				file.PutString( _T( "WTYPE_LISTCTRL" ) );
				break;
			case WTYPE_COMBOBOX:
				file.PutString( _T( "WTYPE_COMBOBOX" ) );
				break;
			}
			file.PutString( _T( " ") );
			// WNDID
			string = CScript::GetFindIdStr( _T( "WIDC_" ), pWndChild->GetWndId() );
			if( string.IsEmpty() )
			{
				string.Format( _T( "%d" ), pWndChild->GetWndId() );
				file.PutString( string );
			}
			else
				file.PutString( string );
			// Title string
			string.Format( _T( " \"%s\" " ), pWndChild->GetTitle() );
			file.PutString( string );
			// size
			rectWindow = pWndChild->GetWindowRect( TRUE );
			string.Format( _T( "%d %d %d %d " ), rectWindow.left, rectWindow.top, rectWindow.right, rectWindow.bottom );
			file.PutString( string );
			// style
			string.Format( _T( "0x%x " ),pWndChild->GetStyle() );
			file.PutString( string );

			// enter
			file.PutString( _T( "\n" ) );
		}
		file.PutString( _T( "\n}\n" ) );
	}
	
	//FILE *fp = fopen( lpszFileName, _T( "wt" ) );
	//if(fp == NULL) return FALSE;
	//_ftprintf(fp, _T( "// neuz browser ini file\n\n" ) );
	//_ftprintf(fp, _T( "// option\n" ) ,0);
	//_ftprintf(fp, _T( "sound %d\n" ) ,g_dwSoundEffect );
	//_ftprintf(fp, _T( "music %d\n" ) ,g_dwMusicVolume );

}
*/
