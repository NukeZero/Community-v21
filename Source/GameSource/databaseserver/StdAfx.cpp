// stdafx.cpp : source file that includes just the standard includes
//	DatabaseServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


TCHAR	PROGRAM_NAME[128] = _T( "DatabaseServer" );
TCHAR	NETWORK_LOG_NAME[128] = _T( "DSNetwork" );

HWND	hMainWnd;
HINSTANCE g_hInstance;	// current instance

APP_INFO	g_appInfo;

char	DSN_NAME_CHARACTER01[260];
char	DB_ADMIN_ID_CHARACTER01[260];
char	DSN_NAME_LOG[260];
char	DB_ADMIN_ID_LOG[260];

#ifdef __S1108_BACK_END_SYSTEM
char	DSN_NAME_BACKSYSTEM[260];
char	DB_ADMIN_ID_BACKSYSTEM[260];
#endif // __S1108_BACK_END_SYSTEM
char	DSN_NAME_ITEMUPDATE[260];
char	DB_ADMIN_ID_ITEMUPDATE[260];

T_SLOT_ACTION_FLAG	g_tSlotActionFlag = { false, false };
