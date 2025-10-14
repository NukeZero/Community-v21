#include "stdafx.h"
#include <direct.h>

#include "xUtil.h"

LARGE_INTEGER	g_llFreq;

////////////////////////////////////////////////////////////////
//
//                            ETC
//
///////////////////////////////////////////////////////////////

DWORD next;

DWORD	xRand( void )
{
//	static int nRandomSeed[ 10 ] = { 1,5,3,7,8,3,4,5,7,3 };
//	static int i = 0;
	next = next * 1103515245 + 12345;// + nRandomSeed[ i++ ];
//	next = next * 1103 + 12345;// + nRandomSeed[ i++ ];
	//	if( i >= 10 ) i = 0;
//	return  (WORD)next;
//	return  (DWORD)((next >> 16) & 0x7FFF);	
	return next;	// 억단위 확률이 필요해서바꿈.
}

DWORD xRandom( DWORD num )
{
	return  xRand() % num;
}

DWORD xRandom( DWORD min, DWORD max )
{
	if( max > min )
		return min + xRandom( max - min );
	else
		return min;
}


float xRandomF( float num )
{
	return ((float)rand() / (float)RAND_MAX) * (float)num;
}

void	xSRand( DWORD seed )
{
   next = seed;
}

//static char s_szBuff[ 0x7fff ];
//static char s_szStr[ 0x7fff ];

#ifndef __COLA
//CRIT_SEC	s_mcs;
#endif

LPCTSTR Error( LPCTSTR str, ... )
{
#ifndef __COLA
//	s_mcs.Enter();
#endif

	static char            szBuff[8192] = { NULL, };
    va_list         vl;

    va_start(vl, str);
    vsprintf_s(szBuff, _countof(szBuff), str, vl);
#if defined(__MASTER) || defined(__ERRORTXT)		// 마스터 버전이거나 ERRORTXT가 붙은건 에러를 파일로 쓰자.
    static char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf( szStr, "%d/%2d/%2d   %02d:%02d:%02d   %s\n", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, 
		szBuff );		// 날짜까지 붙여서....
	
	DEBUGOUT2( szStr );	// 마스터 모드일땐 창으로 뜨면 안되고 파일로 써야 한다.	
#else
	HWND hWnd = NULL;
#ifdef __CLIENT
#ifndef _DEBUG
	hWnd = g_Neuz.GetSafeHwnd();
#endif // not debug
#endif
    MessageBox( hWnd, szBuff, "오류", MB_OK);
#endif
    va_end(vl);
	
#ifndef __COLA
//	s_mcs.Leave();
#endif

#if defined(__MASTER) || defined(__ERRORTXT)		// 마스터 버전이거나 ERRORTXT가 붙은건 에러를 파일로 쓰자.
	return szStr;
#else
	return szBuff;
#endif
}


//--------------------------------------------------------------
//	Path
//
//------------------------------------------------------------

static	char _szTempBuff[256] = { NULL, };
char	g_szWorkDir[256] = { NULL, };		// 워킹폴더
char	g_szExeDir[256] = { NULL, };		// 실행파일 경로

void	InitLIB( void )
{
	_getcwd( g_szWorkDir, 256 );		// 워킹 폴더를 읽음

	GetModuleFileName( NULL, g_szExeDir, 256 );	// 실행파일 경로
	char* spzRealDirEnd = strrchr( g_szExeDir, '\\' );
	*spzRealDirEnd = '\0';
}
#ifdef	__XTOOL
/*LPCTSTR		MakePath( LPCTSTR szPath, LPCTSTR szFileName )
{
	strcpy( _szTempBuff, g_szWorkDir );
	strcat( _szTempBuff, "\\" );
	strcat( _szTempBuff, szPath );
	strcat( _szTempBuff, szFileName );

	return _szTempBuff;
}*/
#endif
// 실행파일 경로명에 패스와 파일명을 붙여서 리턴
LPCTSTR	MakeExePath( LPCTSTR szPath, LPCTSTR szFileName )
{
	ZeroMemory(_szTempBuff, _countof(_szTempBuff) );
	_tcscpy_s( _szTempBuff, _countof(_szTempBuff), g_szExeDir);
	_tcscat_s( _szTempBuff, _countof(_szTempBuff), "\\" );
	_tcscat_s( _szTempBuff, _countof(_szTempBuff), szPath );
	_tcscat_s( _szTempBuff, _countof(_szTempBuff), szFileName );

	return _szTempBuff;
}


////////////////////////////////////////////////////////////
// 파일타이틀과 확장자를 리턴  ex) test.wri
LPCTSTR		GetFileName( LPCTSTR szSrc )
{
	int		len, i;

	// 파일명만 추출.

	memset( _szTempBuff, 0, 256 );
	len = strlen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
		if( szSrc[i] == '\\' )
		{
			_tcscpy_s( _szTempBuff, _countof(_szTempBuff), szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			_tcscpy_s( _szTempBuff, _countof(_szTempBuff), szSrc );
	}

	return _szTempBuff;
}

// 파일타이틀만 임시버퍼에 저장	ex) test
LPCTSTR		GetFileTitle( LPCTSTR szSrc )
{
	int		len, i;
	LPCTSTR	szName;
	char	buff[256] = { NULL, };

	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	_tcscpy_s( buff, _countof(buff), szName );
	szSrc = buff;		// xuzhu.txt

	// 파일명만 추출.
	len = strlen( szSrc );		// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )
	{
		if( szSrc[i] == '.' )	break;
		_szTempBuff[i] = szSrc[i];
	}
	_szTempBuff[i] = '\0';

	return _szTempBuff;
}

// 확장자만 리턴.
LPCTSTR		GetFileExt( LPCTSTR szSrc )
{
	int		len, i;
	
	// 파일명만 추출.
	
	memset( _szTempBuff, 0, sizeof(_szTempBuff) );
	len = strlen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 .이 나오는곳을 찾는다.
		if( szSrc[i] == '.' )
		{
			_tcscpy_s( _szTempBuff, _countof(_szTempBuff), szSrc + i + 1 );	// .가 나온 다음 칸부터 확장자로 씀
			break;
		}
//		if( i == 0 )		// 끝까지 검색했는데도 .가 나오지 않았으면 ""가 리턴.
//			strcpy( _szTempBuff, szSrc );
	}
	
	return _szTempBuff;
}

// 풀네임에서 패스 부분만 리턴함
LPCTSTR		GetFilePath( LPCTSTR szSrc )
{
	int len = strlen( szSrc );		// 풀네임 길이구함

	// 마지막 \찾음
	int i = len-1;
	for( ; i >= 0; i -- )
	{
		if( szSrc[i] == '\\' )
			break;
	}
	_tcsncpy_s( _szTempBuff, _countof( _szTempBuff ), szSrc, i+1 );		// 패스 부분만 카피
	_szTempBuff[i+1] = '\0';
	return _szTempBuff;

}



//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
OSTYPE g_osVersion = WINDOWS_UNKNOWN;
//CPU 정보를 나타낸다.
LPCTSTR GetCPUInfo( void ) 
{
#ifdef __CLIENT	
	CRegKey rg;
	DWORD dwCount;

	memset( _szTempBuff, 0, sizeof(_szTempBuff) );
	
	if( rg.Open( HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0" ) == ERROR_SUCCESS ) 
	{
		if( rg.QueryValue( _szTempBuff, "ProcessorNameString", &dwCount ) != ERROR_SUCCESS )
			if( rg.QueryValue( _szTempBuff, "Identifier", &dwCount ) != ERROR_SUCCESS )
				strcpy( _szTempBuff, "unknown" );
	} else
	{
		strcpy( _szTempBuff, "unknown" );
	}
		
	rg.Close();

	OSVERSIONINFO versionInformation;
	versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	BOOL b = GetVersionEx( &versionInformation );
	if( b )
	{
		strcat( _szTempBuff, " " );
		switch( versionInformation.dwMajorVersion ) 
		{
		case 3:
			strcat( _szTempBuff, "Windows NT 3.51 " );
			g_osVersion = WINDOWS_NT351;
			break;
		case 4:
			switch( versionInformation.dwMinorVersion )
			{
			case 0:		
				if( versionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT )
				{
					strcat( _szTempBuff, "Windows NT 4.0 " );
					g_osVersion = WINDOWS_NT;
				}
				else
				{
					strcat( _szTempBuff, "Windows 95 " );	
					g_osVersion = WINDOWS_95;
				}
				break;
			case 10:	strcat( _szTempBuff, "Windows 98 " );	g_osVersion = WINDOWS_98;	break;
			case 90:	strcat( _szTempBuff, "Windows Me " );	g_osVersion = WINDOWS_ME;	break;
			}
			break;
		case 5:
			switch( versionInformation.dwMinorVersion )
			{
			case 0:		strcat( _szTempBuff, "Windows 2000 " );	g_osVersion = WINDOWS_2000;	break;
			case 1:		strcat( _szTempBuff, "Windows XP " );	g_osVersion = WINDOWS_XP;	break;
			case 2:		strcat( _szTempBuff, "Windows Server 2003 family " );	g_osVersion = WINDOWS_SERVER_2003;	break;
				
			}
			break;
		}
/*		
		switch( versionInformation.dwPlatformId )
		{
		case VER_PLATFORM_WIN32s:
			strcat( _szTempBuff, "Windows 3.1 " );
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if( versionInformation.dwMinorVersion == 0 )
				strcat( _szTempBuff, "Windows 95 " );
			else
				strcat( _szTempBuff, "Windows 98 " );
			break;
		case VER_PLATFORM_WIN32_NT:
			strcat( _szTempBuff, "Windows NT " );
			break;
		}
		char szVersion[32];
		sprintf( szVersion, "v%d ", versionInformation.dwMajorVersion );
		strcat( _szTempBuff, szVersion );
*/
		if( versionInformation.szCSDVersion[0] )
			strcat( _szTempBuff, versionInformation.szCSDVersion );

	}
#endif // client	
	return _szTempBuff;
	
}

//////////////////////////////////////////////////////////////////////////
CString GetNumberFormatEx( LPCTSTR szNumber )
{
	ASSERT( szNumber );

	CString str;

	int nLength = lstrlen( szNumber );	
	for( int i=nLength-1; i>=0; --i)
	{
		str.Insert(0, szNumber[i]);
		if( ((nLength-i) % 3) == 0 && i != 0 )
			str.Insert(0, ',' );
	}
	return str;
}

void MakeEven( long& x )
{
	x ++;
	x &= (~0x01);
}



#ifdef __PROF
CProf	g_Prof;
#endif

int		g_nMaxTri = 0;