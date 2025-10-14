// stdafx.cpp : source file that includes just the standard includes
//	CoreServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


TCHAR	PROGRAM_NAME[128] = _T( "CoreServer" );
TCHAR	NETWORK_LOG_NAME[128] = _T( "CONetwork" );

FLTickCount	g_TickCount;
CGameTimer	g_GameTimer;

FLOAT	s_fPartyExpRate	= 1.0f;
BOOL	s_bTest			= FALSE;

DWORD	s_dwEvent		= 0;

