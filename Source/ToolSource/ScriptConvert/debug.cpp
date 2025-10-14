// DebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void FILEOUT(LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512];
	nBuf = _vsntprintf(szBuffer, 512, lpszFormat, args);
	TRACE(szBuffer);
	FILE* fp;
	if(!(fp = fopen(lpszFileName,"a")))
		if(!(fp = fopen(lpszFileName,"wt")))
			return;
	fprintf(fp,szBuffer);
	fprintf(fp,"\r\n");
	va_end(args);
	fclose(fp);
}
#if !defined( __XTOOL ) && !defined( __SFXEDITOR) && !defined( __DAISY )
//CMclCritSec	s_csLog;
void WriteLog( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start( args, lpszFormat );
	int nBuf;
	TCHAR szBuffer[4096];
	nBuf = _vsntprintf( szBuffer, 4096, lpszFormat, args );
//	TRACE( szBuffer );
//	s_csLog.Enter();

	FILE* fp;
#ifdef __IDC
	if( !( fp = fopen( "..\\error.log","a" ) ) )
#else	// __IDC
	if( !( fp = fopen( "error.log","a" ) ) )
#endif	// __IDC
	{
#ifdef __IDC
		if( !( fp = fopen( "..\\error.log","wt" ) ) )
#else	// __IDC
		if( !( fp = fopen( "error.log","wt" ) ) )
#endif	// __IDC
		{
//			s_csLog.Leave();
			return;
		}
	}
	fprintf( fp,szBuffer );
	fprintf( fp,"\r\n" );
	va_end( args );
	fclose( fp );

//	s_csLog.Leave();
}

void WriteError( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start( args, lpszFormat );
	int nBuf;
	TCHAR szBuffer[4096];
	nBuf = _vsntprintf( szBuffer, 4096, lpszFormat, args );
//	TRACE( szBuffer );

	FILE* fp;
#ifdef __IDC
	if( !( fp = fopen( "..\\eh.log","a" ) ) )
#else	// __IDC
	if( !( fp = fopen( "eh.log","a" ) ) )
#endif	// __IDC
	{
#ifdef __IDC
		if( !( fp = fopen( "..\\eh.log","wt" ) ) )
#else	// __IDC
		if( !( fp = fopen( "eh.log","wt" ) ) )
#endif	// __IDC
		{
			return;
		}
	}

	SYSTEMTIME	time;
	char szTime[MAX_PATH]	= { 0, };
	GetLocalTime( &time );
	sprintf( szTime, "%d/%2d/%2d\t%02d:%02d:%02d\r\n", 
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
	TCHAR szBuffer[512];
	nBuf = _vsntprintf(szBuffer, 512, lpszFormat, args);
	TRACE(szBuffer);
	FILE* fp;
	if(!(fp = fopen("Result\\debug.txt","a")))
		if(!(fp = fopen("Result\\debug.txt","wt")))
			return;
	fprintf(fp,szBuffer);
	fprintf(fp,"\r\n");
	va_end(args);
	fclose(fp);
}

void DEBUGOUT2( LPCTSTR lpszStr )
{
	FILE* fp;
#ifdef __IDC
	fp = fopen("..\\error.txt","a");
#else	// __IDC
	fp = fopen("error.txt","a");
#endif	// __IDC
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
}
#endif

CString		GetTimeString( void )
{
	SYSTEMTIME		time;
	CString			strTime;
	
	GetLocalTime( &time );
	strTime.Format( "%2d/%2d %02d:%02d", 
			time.wMonth, time.wDay,
			time.wHour, time.wMinute );

	return	strTime;

}
