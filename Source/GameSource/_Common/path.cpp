// Neuz.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "lang.h"

#include "langman.h"

/*
static TCHAR g_szResourcePath[ MAX_PATH ];

void SetResourcePath()
{
	DWORD dwRet = ::GetCurrentDirectory( MAX_PATH, g_szResourcePath );
	if( dwRet == 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "¿¡·¯ %08x" ), GetLastError() );
	}
	_tcscat( g_szResourcePath, "\\" );
}
LPCTSTR GetResourcePath()
{
	return  g_szResourcePath;
}

CString MakePath( LPCTSTR lpszFileName )
{
	CString strFullPath;
	strFullPath += lpszFileName;
	return strFullPath;
}
*/
void MakePath( CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	strFullPath = lpszDirName;
	strFullPath += lpszDirName;
	strFullPath += lpszFileName;
}

CString MakePath( LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile )
{
	CString	strFullPath( lpDir );

	strFullPath		+= CLangMan::GetInstance()->GetLangData( dwLan )->szPath;

	strFullPath		+= lpFile;
	return strFullPath;
}


CString MakePath( LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	CString strFullPath( lpszDirName );
	strFullPath += lpszFileName;
	return strFullPath;
}

void MakePath( TCHAR* lpszFullPath, size_t cchFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	FLStrcpy( lpszFullPath, cchFullPath, lpszDirName );
	FLStrcat( lpszFullPath, cchFullPath, lpszFileName );
}
