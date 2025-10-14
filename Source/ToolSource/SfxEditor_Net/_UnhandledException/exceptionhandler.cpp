#include "stdafx.h"

#ifndef __MINIDUMP

static int		s_nErrorSeq[ 16 ];
static DWORD	s_nErrorTick[ 16 ];

#ifndef _DEBUG

#include "pch.h"
#include "BugslayerUtil.h"
#include "CrashHandler.h"
#include "ExceptionHandler.h"

#ifndef __XTOOL
#include "..\_Common\HwOption.h"	// COLA에선 include되면 안됨.
#endif

extern BYTE g_hdr, g_Prev;

//#pragma comment(lib, "UnhandledException/NeuzD.lib")

#if defined( __CLIENT ) // && defined( __ONLINE )
#include "dpcertified.h"
#include "dploginclient.h"
#include "dpclient.h"
extern	CDPCertified	g_dpCertified;
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPClient	g_DPlay;
#endif	// defined( __CLIENT ) && defined( __ONLINE )

#ifdef __DBSERVER
#include "..\_database\DbManager.h"
#endif	// __DBSERVER

#ifdef __PATCHCLIENT
	#ifdef __PATCHVER2
		#include <winsock2.h>
		#pragma comment(lib, "ws2_32.lib")
		#define	PACKETTYPE_ERROR_TEXT	0x0000000d
		#define Message					printf
	#else
		#include "..\_Network\MsgHdr.h"
	#endif
#endif // __PATCHCLIENT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char	g_szVersion[];
extern HANDLE g_hMutex;
static char g_szErrorAdd[ 0x7fff ];

LONG __stdcall CrashHandlerExceptionFilter( EXCEPTION_POINTERS* pExPtrs);

static volatile long s_nRecrvCnt = 0;

extern char   g_szLastFile[512];


// ExceptionHandler 시작
void	InitEH( void )
{
	SetErrorMode(SetErrorMode(0) |
		SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
	
	SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
	memset( g_szErrorAdd, 0, sizeof(g_szErrorAdd) );
	memset( s_nErrorSeq, 0, sizeof(s_nErrorSeq) );
}

// 추가 에러코드를 기록한다.
void	_AddErrorMsg( LPCTSTR str, LPCTSTR strHead )
{
	if( str[0] == 0 )	return;
	if( strlen( g_szErrorAdd ) >= 0x7fff )
		return;
	strcat( g_szErrorAdd, strHead );		// 메모장에서 찾기 쉽에 이거 붙이자 ㅎㅎ
	strcat( g_szErrorAdd, str );
	strcat( g_szErrorAdd, "\n" );
	//	g_strErrorAdd += str;
//	g_strErrorAdd += "\n";
}

#ifdef __TRANSFER_ERROR_TEXT

void SendErrorText( const char* pszErrorText )
{
	int len;
	WSADATA	wsd;
	WSAStartup( MAKEWORD( 2, 2 ), &wsd );

	struct sockaddr_in server;
	struct hostent* host	= NULL;
	SOCKET sClient	= socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sClient == INVALID_SOCKET )
	{
		OutputDebugString( "SendErrorText:socket - INVALID_SOCKET\n" );
		return;
	}
	server.sin_family	= AF_INET;
	server.sin_port		= htons( 13000 );
	server.sin_addr.s_addr	= inet_addr( ADDR_ACCUMULATOR );
	if( server.sin_addr.s_addr == INADDR_NONE )
	{
		host	= gethostbyname( ADDR_ACCUMULATOR );
		if( host == NULL )
		{
			OutputDebugString( "SendErrorText:gethostbyname\n" );
			return;
		}
		CopyMemory( &server.sin_addr, host->h_addr_list[0],
			host->h_length );
	}
	if( connect( sClient, (struct sockaddr *)&server, 
		sizeof(server) ) == SOCKET_ERROR )
	{
		OutputDebugString( "SendErrorText:connect\n" );
		return;
	}

	////////////////////////////////////////////////////////////////////
	//	Send data
	////////////////////////////////////////////////////////////////////
	DWORD dwIndex = 0;		// ACCUMULATOR와 일치시킬 것( - "KOR","CHI","JAP","TWN","THA","ENG" )
#if __CURRENT_LANG == LANG_JAP
	dwIndex = 2;
#elif __CURRENT_LANG == LANG_THA
	dwIndex = 4;
#elif __CURRENT_LANG == LANG_CHI
	dwIndex = 1;
#elif __CURRENT_LANG == LANG_TWN
	dwIndex = 3;
#elif __CURRENT_LANG == LANG_ENG
	dwIndex = 5;
#else 
	dwIndex = 0;
#endif	
	DWORD dwProtocol = PACKETTYPE_ERROR_TEXT + dwIndex;

#ifdef __PATCHVER2
	BYTE* buf	= new BYTE[1024 * 64];
	buf[0] = '^'; 
	//*(UNALIGNED DWORD*)&buf[1]	= (DWORD)len;
	*(UNALIGNED DWORD*)&buf[ 5] = dwProtocol;
	*(UNALIGNED DWORD*)&buf[ 9] = 0; // ar.WriteString( "" );
	*(UNALIGNED DWORD*)&buf[13] = 0; // ar.WriteString( "" );
	int n = strlen( pszErrorText );
	*(UNALIGNED DWORD*)&buf[17] = n; 
	if( n > 0 )
		memcpy( buf + 21, pszErrorText, n );
	len = 21 + n - 5;	// len은 header를 제외한 길이 
	*(UNALIGNED DWORD*)&buf[ 1] = len;
#else
	#ifdef __CLIENT
		CAr ar;
		ar << dwProtocol; 
		ar.WriteString( g_Neuz.m_szAccount );
		if( CMover::GetActiveMover() )
			ar.WriteString( CMover::GetActiveMover()->GetName() );
		else
			ar.WriteString( "" );
		ar.WriteString( pszErrorText );
	#else
		CAr ar;
		ar << dwProtocol; 
		ar.WriteString( "" );
		ar.WriteString( "" );
		ar.WriteString( pszErrorText );
	#endif
	BYTE* lpData	= ar.GetBuffer( &len );
	BYTE* buf	= new BYTE[sizeof(DWORD) + 1 + len];
	*buf	= '^';
	*(UNALIGNED DWORD*)&buf[1]	= (DWORD)len;
	memcpy( (void*)&buf[sizeof(DWORD) + 1], lpData, len );
#endif	// !__PATCHVER2

	send( sClient, (const char*)buf, len + sizeof(DWORD) + 1, 0 );
	closesocket( sClient );
	delete[] buf;
}
#endif	// __TRANSFER_ERROR_TEXT

LONG __stdcall CrashHandlerExceptionFilter( EXCEPTION_POINTERS* pExPtrs )
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
#ifdef __CLIENT
	sprintf( szReason, "%s rCnt=%d\r\n%s", g_szVersion, s_nRecrvCnt, GetFaultReason(pExPtrs) );
#else
	#ifdef __PATCHCLIENT
		#ifdef __PATCHVER2
			sprintf( szReason, "%d rCnt=%d\r\n%s", PATCH_VERSION, s_nRecrvCnt, GetFaultReason(pExPtrs) );
		#else
			sprintf( szReason, "%s rCnt=%d\r\n%s", PATCH_VERSION, s_nRecrvCnt, GetFaultReason(pExPtrs) );
		#endif
	#else
		sprintf( szReason, "rCnt=%d \r\n%s", s_nRecrvCnt, GetFaultReason(pExPtrs) );
	#endif
#endif

#ifdef __WORLDSERVER
		char szLatestDialog[512]	= { 0, };
		sprintf( szLatestDialog, "\n\nlatest dialog : %s\t%s\t%s\t%s\n", CScriptDialog::s_pszName, CScriptDialog::s_pszKey, CScriptDialog::s_pszBeforeRun, CScriptDialog::s_pszAfterRun );
		strcat( szReason, szLatestDialog );
#endif	// __WORLDSERVER

	strcat( szReason, "\n\n - Registers\n\n" );
	strcat( szReason, GetRegisterString(pExPtrs) );
	
#ifdef __CLIENT
//	DWORD	dwOp = GSTSO_PARAMS | GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
	DWORD	dwOp = GSTSO_MODULE | GSTSO_SYMBOL;
#else
//	DWORD	dwOp = GSTSO_MODULE | GSTSO_SYMBOL;
	DWORD	dwOp = GSTSO_PARAMS | GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
#endif
	
	LPCTSTR szPtr = GetFirstStackTraceString( dwOp, pExPtrs);
	strcat( szReason, "\n\n" );
	if( szPtr )
		strcat( szReason, szPtr );
	else
		strcat( szReason, "(null)" );
	while(1)
	{
		szPtr = GetNextStackTraceString( dwOp, pExPtrs);
		if( szPtr == NULL )		
			break;
		strcat( szReason, "\n" );
		strcat( szReason, szPtr );
	}
	
#ifdef __ERRORSEQ
	{
		char szTemp[16];
		memset( szTemp, 0, sizeof(szTemp) );
		for( int i = 0; i < 16; i ++ )
		{
			sprintf( szTemp, "%d ", s_nErrorSeq[i] );
			strcat( szReason, szTemp );
		}
	}
#endif	// not client

#ifdef __DBSERVER
	{
		char szTemp[MAX_PLAYER+2]	= { 0, };

		sprintf( szTemp, "\r\n// %d", g_DbManager.m_nJoin );
		strcat( szReason, szTemp );

		for( int i = 0; i < MAX_JOIN_SIZE; i++ )
		{
			sprintf( szTemp, "\r\n%s", g_DbManager.m_aszJoin[i] );
			strcat( szReason, szTemp );
		}
	}
#endif	// __DBSERVER

	Message( "%s", szReason);
	strcat( szReason, "\n" );
	strcat( szReason, g_szErrorAdd );

#ifdef 	__CLIENT
	{
	char szPacket[32] = {0, };	// 마지막 패킷을 로그에 추가함.
	sprintf( szPacket, "0x%02x:0x%02x-", g_hdr, g_Prev );
	strcat( szReason, szPacket );

	sprintf( szPacket, "YS%d %d %d-", g_Error_State.m_dwWndId, g_Error_State.m_bIsShowWnd, (int)g_Error_State.m_ch );
	strcat( szReason, szPacket );

	sprintf( szPacket, "마지막으로 읽은 Model파일 : %s", g_szLastFile );
	strcat( szReason, szPacket );
	}

	#ifdef __DUMP0920
	if( g_DPlay.m_pDump != NULL && ( 8192 - lstrlen( szReason ) ) > ( g_DPlay.m_nDumpSize * 2 ) )
	{
		strcat( szReason, "\r\ndump\r\n" );
		char s[8];
		for( int i = 0; i < g_DPlay.m_nDumpSize; i++ )
		{
			sprintf( s, "%02x", g_DPlay.m_pDump[i] );
			lstrcat( szReason, s );
		}
	}
	#endif	// __DUMP0920
#endif	// __CLIENT
	
#if defined( __CLIENT ) && defined( __ONLINE )	// 클라이언트는 싱글스레드이므로 이코드가 가능할 듯 싶다.
	g_dpCertified.DeleteDPObject();
	g_DPlay.DeleteDPObject();
	g_dpLoginClient.DeleteDPObject();
#endif	// defined( __CLIENT ) && defined( __ONLINE )

#ifdef __TRANSFER_ERROR_TEXT
	SendErrorText( szReason );
#endif	// __TRANSFER_ERROR_TEXT
	
#ifdef __CLIENT
	if( g_hMutex )
		CloseHandle( g_hMutex );
#endif	// __CLIENT

	return lRet;
}
#endif // not debug

void	_AddErrorSeq( int nIdx, int nVal )
{
	if( nIdx < 0 || nIdx >= 16 )	return;
	s_nErrorSeq[nIdx] = nVal;
}

void	_AddErrorTick( int nIdx, bool bStop )
{
	if( nIdx < 0 || nIdx >= 16 )	return;
	if( bStop )
		s_nErrorTick[nIdx] = 0;
	else
		s_nErrorTick[nIdx] = ::GetTickCount();
}


#ifdef __DEADLOCK__

static DWORD GetDuration( DWORD dwTick )
{
	if( dwTick == 0 )
		return 0;
	else
		return ::GetTickCount() - dwTick;
}

void PrintErrorSeq( void )
{
	FILE* f;
#ifdef __IDC
	if( !( f = fopen( "../errorseq.txt", "a" ) ) )
#else	// __IDC
	if( !( f = fopen( "errorseq.txt", "a" ) ) )
#endif	// __IDC
	{
#ifdef __IDC
		if( !( f = fopen( "../errorseq.txt", "wt" ) ) )
#else	// __IDC
		if( !( f = fopen( "errorseq.txt", "wt" ) ) )
#endif	// __IDC
		{
			return;
		}
	}
	char pszText[256];
	sprintf( pszText, "%d %d %d %d %d %d %d %d \r\n%d %d %d %d %d %d %d %d",
								s_nErrorSeq[0], s_nErrorSeq[1], s_nErrorSeq[2], s_nErrorSeq[3],
								s_nErrorSeq[4], s_nErrorSeq[5], s_nErrorSeq[6], s_nErrorSeq[7],
								s_nErrorSeq[8], s_nErrorSeq[9], s_nErrorSeq[10], s_nErrorSeq[11],
								s_nErrorSeq[12], s_nErrorSeq[13], s_nErrorSeq[14], s_nErrorSeq[15]
								);
	char szTick[256];
	sprintf( szTick, "%d %d %d  %d %d %d", 
		                        GetDuration(s_nErrorTick[0]), GetDuration(s_nErrorTick[1]), GetDuration(s_nErrorTick[2]), 
								GetDuration(s_nErrorTick[3]), GetDuration(s_nErrorTick[4]), GetDuration(s_nErrorTick[5]) );

	SYSTEMTIME	st;
	GetLocalTime( &st );
	fprintf( f, "%d/%02d/%02d\t%02d:%02d:%02d\n%s\n%s\n", 
		     st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
		     pszText, szTick );
	fclose( f );
}
#endif	// __DEADLOCK__


#endif // __MINIDUMP