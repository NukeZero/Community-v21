#include "stdafx.h"
#include <direct.h>
#include "xUtil.h"
////////////////////////////////////////////////////////////////////////////////////
// global variable
////////////////////////////////////////////////////////////////////////////////////

#ifdef __PROF
CProf			g_Prof;
#endif

int				g_nMaxTri = 0;
static BOOL		g_bShowMsgBox	= TRUE;
LARGE_INTEGER	g_llFreq;
DWORD			g_next;
char			g_szWorkDir[256];		// 워킹폴더
char			g_szExeDir[256];		// 실행파일 경로
OSTYPE			g_osVersion = WINDOWS_UNKNOWN;


////////////////////////////////////////////////////////////////////////////////////
// global functions
////////////////////////////////////////////////////////////////////////////////////

/*
DWORD	xRand( void )
{
	g_next = g_next * 1103515245 + 12345;// + nRandomSeed[ i++ ];
	return g_next;	// 억단위 확률이 필요해서바꿈.
}

DWORD xRandom( DWORD num )
{
	return  xRand() % num;
}
*/

DWORD xRandom( DWORD min, DWORD max /* max : max -1 */ )
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
   g_next = seed;
}


// 파일타이틀과 확장자를 리턴  ex) test.wri
void	GetFileName( LPCTSTR szSrc, LPTSTR szFileName, size_t cchFileName )
{
	int	len, i;
	// 파일명만 추출.
	i	= len	= strlen( szSrc );		// 풀네임 길이 읽음
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
		if( szSrc[i] == '\\' )
		{
			FLStrcpy( szFileName, cchFileName, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			FLStrcpy( szFileName, cchFileName, szSrc );
	}
}

// 파일타이틀만 임시버퍼에 저장	ex) test
void	GetFileTitle( LPCTSTR szSrc, LPTSTR szFileTitle, size_t cchFileTitle )
{
	int		len, i;
	GetFileName( szSrc, szFileTitle, cchFileTitle );		// 패스 분리

	// 파일명 추출.
	len	= strlen( szFileTitle );	// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )
	{
		if( szFileTitle[i] == '.' )
			break;
	}
	szFileTitle[i]	= '\0';
}


// 확장자만 리턴.
void	GetFileExt( LPCTSTR szSrc, LPTSTR szFileExt, size_t cchFileExt )
{
	int		len, i;
	len	= strlen( szSrc );		// 풀네임 길이 읽음
	i	= len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 .이 나오는곳을 찾는다.
		if( szSrc[i] == '.' )
		{
			FLStrcpy( szFileExt, cchFileExt, szSrc + i + 1 );	// .가 나온 다음 칸부터 확장자로 씀
			break;
		}
	}
}

// 풀네임에서 패스 부분만 리턴함
void	GetFilePath( LPCTSTR szSrc, LPTSTR szFilePath, size_t cchFilePath )
{
	int len	= strlen( szSrc );	// 풀네임 길이구함
	// 마지막 \찾음
	int i = len-1;
	for( ; i >= 0; i -- )
	{
		if( szSrc[i] == '\\' )
			break;
	}
	FLStrncpy( szFilePath, cchFilePath, szSrc, i+1 );		// 패스 부분만 카피
	szFilePath[i+1] = '\0';
}



//CPU 정보를 나타낸다.
#ifdef __CLIENT
static	char	_szTempBuff[256];
LPCTSTR GetCPUInfo( void ) 
{
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
		if( versionInformation.szCSDVersion[0] )
			strcat( _szTempBuff, versionInformation.szCSDVersion );

	}
	return _szTempBuff;
}
#endif // client

int StringFind( const char* string, int c )
{
	const char* pdest;
	return ( pdest = strchr( string, c ) ) ? pdest - string : -1;
}

void StringTrimRight( char* szString )
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)
	LPTSTR lpsz = szString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
	}
}

CString GetNumberFormatEx( LPCTSTR szNumber )
{
	FLASSERT( szNumber );

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

