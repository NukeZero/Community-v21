#include "stdafx.h"

#ifndef _DEBUG

#include <dump/FLDumpInstaller.h>

#include "pch.h"
#include "BugslayerUtil.h"
#include "CrashHandler.h"
#include "ExceptionHandler.h"
#include "..\_Common\HwOption.h"	// COLA에선 include되면 안됨.

#include "../Neuz/dpcertified.h"
#include "../Neuz/dploginclient.h"
#include "../Neuz/dpclient.h"

extern	CDPCertified	g_dpCertified;
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPClient		g_DPlay;


extern int g_hdr, g_Prev;
extern char	g_szVersion[];
extern HANDLE g_hMutex;
static volatile long s_nRecrvCnt = 0;
extern char   g_szLastFile[];


LONG	CrashHandlerExceptionFilter( EXCEPTION_POINTERS* pExPtrs );

//////////////////////////////////////////////////////////////////////////
class	FLClientDumper : public FLDumper
{
public:

	static FLDumper*	GetInstance();

private:

	bool	DoDump( T_DUMP_PARAM* pParam );

};

FLDumper*	FLClientDumper::GetInstance()
{
	static FLClientDumper cClientDumper;
	return &cClientDumper;
}

bool	FLClientDumper::DoDump( T_DUMP_PARAM* pParam )
{
	FLLOG_FLUSH();

	if( CrashHandlerExceptionFilter( pParam->pException ) == EXCEPTION_CONTINUE_SEARCH )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


// ExceptionHandler 시작
void	InitEH()
{
	SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

	FLDumpInstaller::Install( FLClientDumper::GetInstance() );
}

LONG	CrashHandlerExceptionFilter( EXCEPTION_POINTERS* pExPtrs )
{
	LONG lRet;
	if( InterlockedIncrement( &s_nRecrvCnt ) >= 3 )
		return EXCEPTION_CONTINUE_SEARCH;

	// 디버깅 모드 일때에는 추가 정보를 보여주지 않고
	// EXCEPTION_CONTINUE_SEARCH  값을 리턴함으로써
	// 프로그래머가 디버깅 할 수 있도록 한다.
#ifdef _DEBUG
	lRet = EXCEPTION_CONTINUE_SEARCH ;
	return lRet;
#else
	lRet = EXCEPTION_EXECUTE_HANDLER;
#endif
	
	// 왜 죽었는지 알아내도록 합니다.
	char szReason[ 1024*16 ] = {0, };
	FLSPrintf( szReason, _countof( szReason ), "%s rCnt=%d\r\n%s", g_szVersion, s_nRecrvCnt, GetFaultReason(pExPtrs) );

	FLStrcat( szReason, _countof( szReason ), "\n\n - Registers\n\n" );
	FLStrcat( szReason, _countof( szReason ), GetRegisterString(pExPtrs) );
	
	DWORD	dwOp = GSTSO_MODULE | GSTSO_SYMBOL;
	
	LPCTSTR szPtr = GetFirstStackTraceString( dwOp, pExPtrs);
	FLStrcat( szReason, _countof( szReason ), "\n\n" );
	if( szPtr )
		FLStrcat( szReason, _countof( szReason ), szPtr );
	else
		FLStrcat( szReason, _countof( szReason ), "(null)" );
	while(1)
	{
		szPtr = GetNextStackTraceString( dwOp, pExPtrs);
		if( szPtr == NULL )		
			break;
		FLStrcat( szReason, _countof( szReason ), "\n" );
		FLStrcat( szReason, _countof( szReason ), szPtr );
	}
	
	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szReason );
	FLStrcat( szReason, _countof( szReason ), "\n" );

	{
		char szPacket[512] = {0, };	// 마지막 패킷을 로그에 추가함.
		FLSPrintf( szPacket, _countof( szPacket ), "%d:%d-", g_hdr, g_Prev );
		FLStrcat( szReason, _countof( szReason ), szPacket );

		FLSPrintf( szPacket, _countof( szPacket ), "YS%d %d %d-", g_Error_State.m_dwWndId, g_Error_State.m_bIsShowWnd, (int)g_Error_State.m_ch );
		FLStrcat( szReason, _countof( szReason ), szPacket );

		FLSPrintf( szPacket, _countof( szPacket ), "last Model:%s", g_szLastFile );
		FLStrcat( szReason, _countof( szReason ), szPacket );
	}

	if( g_DPlay.m_pDump != NULL && ( 8192 - lstrlen( szReason ) ) > ( g_DPlay.m_nDumpSize * 2 ) )
	{
		FLStrcat( szReason, _countof( szReason ), "\r\ndump\r\n" );
		char s[8];
		for( int i = 0; i < g_DPlay.m_nDumpSize; i++ )
		{
			FLSPrintf( s, _countof( s ), "%02x", g_DPlay.m_pDump[i] );
			FLStrcat( szReason, _countof( szReason ), s );
		}
	}

	//gmpbigsun( 2010_0624 ) : 추가정보를 log로 남기자
	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szReason );

	// 클라이언트는 싱글스레드이므로 이코드가 가능할 듯 싶다.
	g_dpCertified.DeleteDPObject();
	g_DPlay.DeleteDPObject();
	g_dpLoginClient.DeleteDPObject();

	if( g_hMutex )
		CloseHandle( g_hMutex );

	return lRet;
}

#endif // not debug
