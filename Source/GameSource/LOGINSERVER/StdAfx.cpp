// stdafx.cpp : source file that includes just the standard includes
//	LoginServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

TCHAR	PROGRAM_NAME[128] = _T( "LoginServer" );
TCHAR	NETWORK_LOG_NAME[128] = _T( "LSNetwork" );

char	g_szClientBindIP[16] = { 0, };
char	g_lpCoreAddr[16];
char	g_lpDBAddr[16];

//////////////////////////////////////////////////////////////////////////
//	BEGIN100616
//BOOL	g_bNProtectAuth = FALSE;
//	END100616
//////////////////////////////////////////////////////////////////////////

FLTickCount	g_TickCount;
