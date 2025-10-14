
#include "StdAfx.h"
#include "FLScriptFileManager.h"


FLScriptFileManager::FLScriptFileManager()
{
	m_szResourcePath[ 0 ]	= '\0';
	m_szScriptPath[ 0 ]		= '\0';

	SetScriptPath();
}

FLScriptFileManager::~FLScriptFileManager()
{

}

FLScriptFileManager* FLScriptFileManager::GetInstance()
{
	static FLScriptFileManager sScriptFileManager;

	return & sScriptFileManager;
}

void	FLScriptFileManager::SetScriptPath( /*const TCHAR* pszResourcePath*/ )
{
	::GetCurrentDirectory( _countof( m_szResourcePath ), m_szResourcePath );

	TCHAR szScriptDirName[ _MAX_PATH ]	= { 0, };

#ifdef __SERVER
	FLStrcpy( szScriptDirName, _countof( szScriptDirName ), "" );
#else // !__SERVER
	FLStrcpy( szScriptDirName, _countof( szScriptDirName ), "" );
#endif // __SERVER

	FLSPrintf( m_szScriptPath, _countof( m_szScriptPath ), "%s\\%s", m_szResourcePath, szScriptDirName );

	CreateDir( m_szResourcePath, szScriptDirName );
}

bool	FLScriptFileManager::LoadMoveFileList( const TCHAR* pszFileName ) const
{
#ifdef __MAINSERVER
	CScript kScript;

	if( kScript.Load( pszFileName ) == FALSE )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ NOT EXIST SCRIPT FILE. FILE_NAME:(%s) ]" ), pszFileName );
		return true;
	}

	kScript.GetToken();		// move file name

	while( kScript.tok != FINISHED )
	{
		TCHAR szFileName[ _MAX_PATH ]	= { 0, };
		FLStrcpy( szFileName, _countof( szFileName ), kScript.Token );

		const bool bret = Move( szFileName );
		if( bret == false )
		{
			FLASSERT( 0 );
			return false;
		}

		kScript.GetToken();
	}

	::DeleteFile( pszFileName );
#else	// !__MAINSERVER
	UNREFERENCED_PARAMETER( pszFileName );
#endif // __MAINSERVER

	return true;
}

// DWORD	FLScriptFileManager::GetOrder( const TCHAR* pszKey ) const
// {
// 	TCHAR szKey[ 32 ]	= { 0, };
// 	FLStrcpy( szKey, _countof( szKey ), pszKey );
// 	FLStrUppercase( szKey, _countof( szKey ) );
// 
// 	if( _tcscmp( szKey, _T( "MOVE" ) ) == 0 )
// 	{
// 		return E_MOVE;
// 	}
// 	else if( _tcscmp( szKey, _T( "DELETE" ) ) == 0 )
// 	{
// 		return E_DELETE;
// 	}
// 	else
// 	{
// 		return E_NONE;
// 	}
// }

bool	FLScriptFileManager::CreateDir( const TCHAR* pszPath, const TCHAR* pszDirName ) const
{
	if( pszPath == NULL || pszDirName == NULL )
	{
		return false;
	}

	TCHAR szCreateDirPath[ _MAX_PATH ]	= { 0, };
	FLStrcpy( szCreateDirPath, _countof( szCreateDirPath ), pszPath );

	// 생성 할 디렉토리의 하위 폴더 유무 확인.
	CString strDirName	= pszDirName;

	const bool bExistSubDir	= ( strDirName.Find( "\\") < 0 && strDirName.Find( "/") < 0 ) ? false : true;
	if( bExistSubDir == true )		// 하위 폴더가 존재 한다.
	{
		TCHAR szDirName[ _MAX_PATH ]	= { 0, };
		FLStrcpy( szDirName, _countof( szDirName ), pszDirName );

		TCHAR szSeps[]		= _T( "\\/" );
		TCHAR* pszToken		= NULL;
		TCHAR* pszNextToken	= NULL;

		pszToken	= _tcstok_s( szDirName, szSeps, &pszNextToken );

		while( pszToken != NULL )
		{
			FLSPrintf( szCreateDirPath, _countof( szCreateDirPath ), "%s\\%s", szCreateDirPath, pszToken );
			if( ::CreateDirectory( szCreateDirPath, NULL ) == FALSE && ::GetLastError() != ERROR_ALREADY_EXISTS )
			{
				return false;
			}

			pszToken	= _tcstok_s( NULL, szSeps, &pszNextToken );
		}
	}
	else							// 그냥 단일 디렉토리..
	{
		FLSPrintf( szCreateDirPath, _countof( szCreateDirPath ), "%s\\%s", szCreateDirPath, pszDirName );
		if( ::CreateDirectory( szCreateDirPath, NULL ) == FALSE && ::GetLastError() != ERROR_ALREADY_EXISTS )
		{
			return false;
		}
	}

	return true;
}

bool	FLScriptFileManager::Move( const TCHAR* pszFileName/*, const TCHAR* pszDestPath*/ ) const
{
	TCHAR szDirName[ _MAX_PATH ]	= { 0, };

	_splitpath_s( pszFileName, NULL, 0, szDirName, _countof( szDirName ), NULL, 0, NULL, 0 );

	const bool bCreate	= CreateDir( m_szScriptPath, szDirName );
	if( bCreate == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CREATE DIRECTORY. FILE_NAME:(%s) ]" ), pszFileName );
		return false;
	}

	TCHAR szFullPath[ _MAX_PATH ]	= { 0, };
	FLSPrintf( szFullPath, _countof( szFullPath ), "%s\\%s", m_szScriptPath, pszFileName );

	if( ::MoveFile( pszFileName, szFullPath ) == FALSE )
	{
		const DWORD dwError	= ::GetLastError();
		if( dwError == ERROR_FILE_NOT_FOUND )
		{
			FLINFO_LOG( PROGRAM_NAME, _T( "[ DON'T MOVE FILE. FILE NOT FOUND. FILE_NAME:(%s) ]" ), pszFileName );
		}
		else if( dwError == ERROR_ALREADY_EXISTS )
		{
			FLINFO_LOG( PROGRAM_NAME, _T( "[ DON'T MOVE FILE. FILE ALREADY EXISTS. FILE_NAME:(%s) ]" ), pszFileName );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DON'T MOVE FILE. CHECK MOVE SCRIPT. FILE_NAME:(%s), ERROR_CODE:(%u) ]" ), pszFileName, dwError );
			return false;
		}
	}

	::DeleteFile( pszFileName );

	return true;
}

bool	FLScriptFileManager::Delete( const TCHAR* pszFileName ) const
{
	TCHAR szSrcFullPath[ _MAX_PATH ]	= { 0, };

	FLSPrintf( szSrcFullPath, _countof( szSrcFullPath ), "%s\\%s", m_szResourcePath, pszFileName );

	return ::DeleteFile( szSrcFullPath ) == TRUE ? true : false;
}

void	FLScriptFileManager::GetScriptFileFullPath( TCHAR* pszFullPath, size_t size, const TCHAR* pszFileName ) const
{
	if( pszFullPath == NULL || pszFullPath == NULL )
	{
		FLASSERT( 0 );
		return;
	}

	FLSPrintf( pszFullPath, size, "%s\\%s", m_szScriptPath, pszFileName );
}
