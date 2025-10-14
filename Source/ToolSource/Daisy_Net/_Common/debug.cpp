// DebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Debug.h"

void FILEOUT(LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512] =  { NULL, };
	nBuf = _vsntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, lpszFormat, args);
#ifndef __PRINT_ITEM0718
#ifndef __OUTPUT_INFO_0803
	TRACE(szBuffer);
#endif	// __OUTPUT_INFO_0803
#endif	// __PRINT_ITEM0718
	FILE* fp;
	if(!(fp = _fsopen( lpszFileName, "a", _SH_DENYNO )))
		if(!(fp = _fsopen(lpszFileName,"wt", _SH_DENYNO)))
			return;
	fprintf(fp,szBuffer);
//	fprintf(fp,"\r\n");
	fprintf(fp,"\n");
	va_end(args);
	fclose(fp);
}

#if !defined( __XTOOL ) && !defined( __SFXEDITOR) //&& !defined( __DAISY )
//CRIT_SEC	s_csLog;
void WriteLog( LPCTSTR lpszFormat, ... )
{
	TCHAR szBuffer[4096] = { NULL, };

	va_list args;
	va_start( args, lpszFormat );
	_vsntprintf_s( szBuffer, _countof(szBuffer), _TRUNCATE, lpszFormat, args );
	va_end( args );

	FILE* fp;
#ifdef __IDC
	if( !( fp = _fsopen( "..\\error.log","a" ,_SH_DENYNO ) ) )
#else	// __IDC
	if( !( fp = _fsopen( "error.log", "a",_SH_DENYNO ) ) )
#endif	// __IDC
	{
#ifdef __IDC
		if( !( fp = _fsopen( "..\\error.log","wt", _SH_DENYNO ) ) )
#else	// __IDC
		if( !( fp = _fsopen( "error.log","wt", _SH_DENYNO ) ) )
#endif	// __IDC
		{
			return;
		}
	}

	// time
	SYSTEMTIME	time;
	GetLocalTime( &time );
	fprintf( fp, "%d/%02d/%02d\t%02d:%02d:%02d\n%s\n", 
		     time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
		     szBuffer );
	fclose( fp );
}

void WriteError( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start( args, lpszFormat );
	int nBuf;
	TCHAR szBuffer[4096] = { NULL, };
	nBuf = _vsntprintf_s( szBuffer, _countof(szBuffer), _TRUNCATE, lpszFormat, args );
//	TRACE( szBuffer );

	FILE* fp;
#ifdef __IDC
	if( !( fp = _fsopen( "..\\eh.log","a", _SH_DENYNO ) ) )
#else	// __IDC
	if( !( fp = _fsopen( "eh.log","a", _SH_DENYNO ) ) )
#endif	// __IDC
	{
#ifdef __IDC
		if( !( fp = _fsopen( "..\\eh.log","wt", _SH_DENYNO ) ) )
#else	// __IDC
		if( !( fp = _fsopen( "eh.log","wt", _SH_DENYNO ) ) )
#endif	// __IDC
		{
			return;
		}
	}

	SYSTEMTIME	time;
	char szTime[MAX_PATH]	= { 0, };
	GetLocalTime( &time );
	sprintf_s( szTime, _countof(szTime),"%d/%2d/%2d\t%02d:%02d:%02d\r\n", 
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
	fprintf( fp, szTime );
	fprintf( fp,szBuffer );
	fprintf( fp,"\r\n" );
	va_end( args );
	fclose( fp );
}
#endif
void DEBUGOUT(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512] = {  NULL, };
	nBuf = _vsntprintf_s(szBuffer, _countof( szBuffer ), lpszFormat, args);
	TRACE(szBuffer);
	FILE* fp;
	if(!(fp = _fsopen("Result\\debug.txt", "a", _SH_DENYNO)))
		if(!(fp = _fsopen("Result\\debug.txt", "wt", _SH_DENYNO)))
			return;
	fprintf(fp,szBuffer);
	fprintf(fp,"\r\n");
	va_end(args);
	fclose(fp);
}

void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName )
{
	FILE* fp;
/*#ifdef __IDC
	fp = fopen("..\\error.txt","a");
#else	// __IDC
	fp = fopen("error.txt","a");
#endif	// __IDC
	*/
	fp = _fsopen( szFileName, "a", _SH_DENYNO );
	if( fp == NULL )
	{
		return;
	}
//	if(!(fp = fopen("error.txt","a")))		// 폴더를 지정하니까 파일생성이 안됨.
//		if(!(fp = fopen("error.txt","wt")))
//			return;
	fprintf(fp,"%s", lpszStr );
	fprintf(fp,"\r\n");
	fclose(fp);
	fflush(fp);
}

// 접속 로그.
void ConnectLog( LPCTSTR szLog )
{
	static char	szStr[8192] = { NULL, };
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, _countof(szStr), "%d/%2d/%2d   %02d:%02d:%02d   %s", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, 
		szLog );		// 날짜까지 붙여서....

	DEBUGOUT2( szStr, "connect.txt" );
	
}

#ifdef _CLIENT
void mprintf(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];
	nBuf = _vsntprintf(szBuffer, 512, lpszFormat, args);
	CArcaneApp *pApp = (CArcaneApp *)::AfxGetApp();
	pApp->Print(szBuffer, nBuf);

	va_end(args);
}
#endif

CString		GetTimeString( void )
{
	SYSTEMTIME		time;
	CString			strTime;
	
	GetLocalTime( &time );
	strTime.Format( "%2d/%2d %02d:%02d:%02d ", 
			time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond );

	return	strTime;

}

LPCTSTR Message( LPCTSTR str, ... )
{
#ifndef __COLA
//	s_mcs.Enter();
#endif
	static char     szBuff[8192] = { NULL, };
    va_list         vl;

    va_start(vl, str);
    vsprintf_s(szBuff, _countof(szBuff), str, vl);
	int len = strlen(szBuff);
#if defined(__MASTER) || defined(__ERRORTXT)	// 마스터 버전이거나 월드서버는 에러를 파일로 쓰자.
    static char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf( szStr, "%d/%2d/%2d   %02d:%02d:%02d\r\n%s", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, 
		szBuff );		// 날짜까지 붙여서....
	
	DEBUGOUT2( szStr );	// 마스터 모드일땐 창으로 뜨면 안되고 파일로 써야 한다.	
#else
	HWND hWnd = NULL;
#ifdef __CLIENT
	hWnd = g_Neuz.GetSafeHwnd();
#endif
    MessageBox( hWnd, szBuff, "알림", MB_OK);
#endif
    va_end(vl);

#ifndef __COLA
//	s_mcs.Leave();
#endif

#if defined(__MASTER) || defined(__ERRORTXT)	// 마스터 버전이거나 월드서버는 에러를 파일로 쓰자.
	return szStr;	// 타임스탬프가 붙은 스트링
#else
	return szBuff;	// 타임 스탬프가 없는 스트링
#endif
}
