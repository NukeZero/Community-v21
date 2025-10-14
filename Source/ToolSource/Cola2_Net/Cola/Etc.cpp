#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
//#include <varargs.h>

#include "etc.h"

void Error(char *str, ...)
{
    char            szBuff[256];
    va_list         vl;

    va_start(vl, str);
    vsprintf(szBuff, str, vl);
    MessageBox(NULL, szBuff, "오류", MB_OK);
    va_end(vl);
}

void Message(char *str, ...)
{
    char            szBuff[256];
    va_list         vl;

    va_start(vl, str);
    vsprintf(szBuff, str, vl);
    MessageBox(NULL, szBuff, "알림", MB_OK);
    va_end(vl);
}




///////////////////////////////////////////////////////////////
unsigned long next;

int		xRand( void )
{
	next = next * 1103515245 + 12345;
//	return  (unsigned int)(next >> 16) & 32767;
	return  next;
}

int		xRandom( int num )
{
	return  xRand() % num;
}

void	xSRand( unsigned int seed )
{
   next = seed;
}

//--------------------------------------------------------------
//	Path
//
//------------------------------------------------------------

static	char _szTempBuff[256];
char	g_szWorkDir[256];		// 워킹폴더
char	g_szExeDir[256];		// 실행파일 경로

void	InitLIB( void )
{
	_getcwd( g_szWorkDir, 256 );		// 워킹 폴더를 읽음

	GetModuleFileName( NULL, g_szExeDir, 256 );	// 실행파일 경로
	char* spzRealDirEnd = strrchr( g_szExeDir, '\\' );
	*spzRealDirEnd = '\0';
}

LPCTSTR		MakePath( LPCTSTR szPath, LPCTSTR szFileName )
{
	strcpy( _szTempBuff, g_szWorkDir );
	strcat( _szTempBuff, "\\" );
	strcat( _szTempBuff, szPath );
	strcat( _szTempBuff, szFileName );

	return _szTempBuff;
}

// 실행파일 경로명에 패스와 파일명을 붙여서 리턴
LPCTSTR	MakeExePath( LPCTSTR szPath, LPCTSTR szFileName )
{
	strcpy( _szTempBuff, g_szExeDir );
	strcat( _szTempBuff, "\\" );
	strcat( _szTempBuff, szPath );
	strcat( _szTempBuff, szFileName );

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
			strcpy( _szTempBuff, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			strcpy( _szTempBuff, szSrc );
	}

	return _szTempBuff;
}

// 파일타이틀만 임시버퍼에 저장	ex) test
LPCTSTR		GetFileTitle( LPCTSTR szSrc )
{
	int		len, i;
	LPCTSTR	szName;
	char	buff[256];

	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	strcpy( buff, szName );
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



///////////////////////////////////////////////
DWORD	m_lTime, m_dwOldTime;

static BOOL	s_bFrameSkip = TRUE;

void InitFST( void )
{
	m_lTime = 0;
	m_dwOldTime = timeGetTime();
	s_bFrameSkip = TRUE;
}

// 프레임 스키핑을 켜고/끈다
void	SetFrameSkip( BOOL bFlag )
{
	s_bFrameSkip = bFlag;
	if( bFlag == TRUE )		InitFST();
}

// 현재 플래그 돌려줌
BOOL	GetFrameSkip( void )
{
	return s_bFrameSkip;
}

static void UpdateTime( void )
{
	DWORD	dwTime = timeGetTime();

	m_lTime += dwTime - m_dwOldTime;
	m_dwOldTime = dwTime;
}

BOOL	IsDrawTiming( void )
{
	if( s_bFrameSkip == FALSE )		return TRUE;		// 프레임 스킵이 꺼져있으면 무조건 드로우.
	UpdateTime();
	return (m_lTime < 1000 / FRAME_PER_SEC) ? TRUE : FALSE;
}

void	SetFST( void )
{
	if( s_bFrameSkip == FALSE )		return;
	UpdateTime();
	while (m_lTime < 1000 / FRAME_PER_SEC)
		UpdateTime();

	m_lTime -= 1000 / FRAME_PER_SEC;
}






