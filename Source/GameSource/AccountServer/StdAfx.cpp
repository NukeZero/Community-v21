// stdafx.cpp : source file that includes just the standard includes
//	AccountServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


TCHAR	PROGRAM_NAME[128] = _T( "AccountServer" );
TCHAR	NETWORK_LOG_NAME[128] = _T( "ASNetwork" );

char	DSN_NAME_LOG[256];
char	DB_ADMIN_ID_LOG[256];

char	DSN_NAME_LOGIN[256];
char	DB_ADMIN_ID_LOGIN[256];

char	DSN_NAME_BILLING[256];
char	DB_ADMIN_ID_BILLING[256];


char	g_szCertifierBindIP[16] = { 0, };
char	g_szDatabaseBindIP[16] = { 0, };
char	g_szAdbillBindIP[16] = { 0, };
char	g_szBillingIP[16] = { 0, };
