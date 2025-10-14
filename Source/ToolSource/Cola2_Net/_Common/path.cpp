// Neuz.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
//#include "..\_UnhandledException\ExceptionHandler.h"

#ifndef __SFXEDITOR
#include "lang.h"
#endif //__SFXEDITOR

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR g_szResourcePath[ 128 ];

void SetResourcePath()
{
	DWORD dwRet = ::GetCurrentDirectory( 128, g_szResourcePath );
	if( dwRet == 0 )
	{
		LPCTSTR szErr = Error( "SetResourcePath : 에러 %08x", GetLastError() );
		ADDERRORMSG( szErr );
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
#if defined( __BEAST ) || defined( __DAISY ) || defined( __SFXEDITOR )
	// 툴에서는 컬런트 폴더가 바뀔 수 있으므로 full path를 만들어준다.
	strFullPath = g_szResourcePath;
#endif
	strFullPath += lpszFileName;
	return strFullPath;
}
void MakePath( CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
#if defined( __BEAST ) || defined( __DAISY ) || defined( __SFXEDITOR )
	// 툴에서는 컬런트 폴더가 바뀔 수 있으므로 full path를 만들어준다.
	strFullPath = g_szResourcePath;
	strFullPath += lpszDirName;
#else
	strFullPath = lpszDirName;
#endif
	strFullPath += lpszDirName;
	strFullPath += lpszFileName;
}

CString MakePath( LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile )
{
	CString	strFullPath;
	strFullPath		+= lpDir;

	switch( dwLan )
	{
		case LANG_KOR:
			strFullPath += "Korean\\";
			break;
		case LANG_ENG:
			strFullPath += "English\\";
			break;
		case LANG_JAP:
			strFullPath += "Japanese\\";
			break;
		case LANG_CHI:
			strFullPath += "Chinese\\";
			break;
		case LANG_THA:
			strFullPath += "Thailand\\";
			break;
		case LANG_TWN:
			strFullPath += "Taiwan\\";
			break;
		default:
			AfxMessageBox( "unknown language setting" );
			break;
	}

	strFullPath		+= lpFile;
	return strFullPath;
}


CString MakePath( LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	CString strFullPath;
#if defined( __BEAST ) || defined( __DAISY ) || defined( __SFXEDITOR ) 
	// 툴에서는 컬런트 폴더가 바뀔 수 있으므로 full path를 만들어준다.
	strFullPath = g_szResourcePath;
#endif
	strFullPath += lpszDirName;
	strFullPath += lpszFileName;
	return strFullPath;
}

void MakePath( TCHAR* lpszFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
#if defined( __BEAST ) || defined( __DAISY ) || defined( __SFXEDITOR )
	// 툴에서는 컬런트 폴더가 바뀔 수 있으므로 full path를 만들어준다.
	_tcscpy( lpszFullPath, (TCHAR*)g_szResourcePath );
	_tcscat( lpszFullPath, lpszDirName );
#else
	_tcscpy( lpszFullPath, lpszDirName );
#endif
	_tcscat( lpszFullPath, lpszFileName );
}
