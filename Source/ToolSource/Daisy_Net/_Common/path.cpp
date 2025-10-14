
#include "stdafx.h"

#ifndef __SFXEDITOR
	#include "lang.h"
#endif 

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

static TCHAR	g_szResourcePath[ 256 ] = { NULL, };
extern TCHAR	PROGRAM_NAME[128];

void SetResourcePath()
{
	if( NULL == GetCurrentDirectory( _countof( g_szResourcePath ), g_szResourcePath ) )
	{
		AfxMessageBox( _T("현재 경로 얻기 실패, 프로그램을 종료합니다.") );
		PostQuitMessage( NULL );
	}
	else	
		_tcscat_s( g_szResourcePath, _countof(g_szResourcePath), "\\" );
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
	strFullPath = g_szResourcePath;
	strFullPath		+= lpDir;

	switch( dwLan )
	{
		case LANG_KOR://한국
			strFullPath += "Korean\\";
			break;
		case LANG_ENG://미국
			strFullPath += "English\\";
			break;
		case LANG_JAP://일본
			strFullPath += "Japanese\\";
			break;
		case LANG_THA://태국
			strFullPath += "Thailand\\";
			break;
		case LANG_TWN://대만
			strFullPath += "Taiwan\\";
			break;
		case LANG_GER://독일
			strFullPath += "German\\";
			break;
		case LANG_SPA://칠레
			strFullPath += "Spanish\\";
			break;
		case LANG_FRE://프랑스
			strFullPath += "French\\";
			break;
		case LANG_HK://홍콩
			strFullPath += "HongKong\\";
			break;
		case LANG_POR:// 브라질
			strFullPath += "Portuguese\\";
			break;
		case LANG_VTN://베트남
			strFullPath += "Vietnam\\";
			break;
		case LANG_RUS://러시아
			strFullPath += "Russian\\";
			break;
		case LANG_PHP://필리핀
			strFullPath += "Philippines\\";
			break;
		case LANG_IND://인도네시아
			strFullPath += "Indonesia\\";
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

#pragma warning( disable : 4996 )  // 코드가 이상하다 손대기 싫어 잠깐 끈다

#if defined( __BEAST ) || defined( __DAISY ) || defined( __SFXEDITOR )
	// 툴에서는 컬런트 폴더가 바뀔 수 있으므로 full path를 만들어준다.
	_tcscpy( lpszFullPath, (TCHAR*)g_szResourcePath );
	_tcscat( lpszFullPath, lpszDirName );
#else
	_tcscpy( lpszFullPath, lpszDirName );
#endif
	_tcscat( lpszFullPath, lpszFileName );

#pragma warning ( default : 4996 )

}
