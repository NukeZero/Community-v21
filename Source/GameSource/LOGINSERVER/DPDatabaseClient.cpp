#include "stdafx.h"
#include "dpcoreclient.h"
#include "dploginsrvr.h"
#include "dpdatabaseclient.h"
#include <logging/FLMyTrace.h>
#include "user.h"
#include "LoginProtect.h"
extern	CDPCoreClient	g_dpCoreClient;

extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	FLMyTrace		g_MyTrace;
extern	char			g_lpDBAddr[16];

extern	CUserMng	g_UserMng;

CDPDatabaseClient::CDPDatabaseClient()
{
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, OnOneHourNotify );
	ON_MSG( PACKETTYPE_PLAYER_LIST, OnPlayerList );
	ON_MSG( PACKETTYPE_FAIL, OnFail );
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_CERT, OnLoginProtect );
}

CDPDatabaseClient::~CDPDatabaseClient()
{

}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				break;
			}
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, dpidUser, lpBuf, uBufSize );
	}
	else {
		g_dpLoginSrvr.Send( lpBuf, uBufSize, dpidUser );
	}
}

void CDPDatabaseClient::SendLeave( const char* lpszAccount, u_long idPlayer, DWORD dwPlayTime )
{
#ifdef __INTERNALSERVER
	FLERROR_LOG( PROGRAM_NAME, _T( "Login CDPDatabaseClient::SendLeave (%s)" ), lpszAccount );
#endif

//	if( idPlayer == 0 )
//		return;
			
	BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar << dwPlayTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendCloseError( const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_ERROR, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}
/*
void CDPDatabaseClient::OnDisconnCertifier( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
static	\
	TCHAR	lpszAccount[MAX_ACCOUNT];

	ar.ReadString( lpszAccount );

	CUser* pUser;
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );

	pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
		g_dpLoginSrvr.Send( lpBuf, uBufSize, pUser->m_dpid );
}
*/
void CDPDatabaseClient::OnCloseExistingConnection( CAr & ar, DPID /*dpid*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> lError;

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		if( lError )
			g_dpLoginSrvr.SendError( lError, pUser->m_dpid );

		g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
	else
	{
		SendCloseError( lpszAccount );	// not found
		FLERROR_LOG( PROGRAM_NAME, _T( "pUser is NULL, %s" ), lpszAccount );
	}
}

void CDPDatabaseClient::OnOneHourNotify( CAr & ar, DPID /*dpid*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		g_dpLoginSrvr.SendHdr( PACKETTYPE_ONE_HOUR_NOTIFY, pUser->m_dpid );
	}
}

void CDPDatabaseClient::SendGetPlayerList( DPID idFrom, const char* lpszAccount, const char* lpszPassword, DWORD dwAuthKey, u_long uIdofMulti, BYTE bySelectPage )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERLIST, idFrom );
	ar << dwAuthKey;
	ar << uIdofMulti;

	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPassword );

	// BEGIN110520
	ar << bySelectPage;
	// END110520

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;

	CUser* pUser;
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );

	pUser	= g_UserMng.GetUser( dpid );
	if( pUser && pUser->m_dwAuthKey == dwAuthKey )
	{
		g_dpLoginSrvr.Send( lpBuf, uBufSize, dpid );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Send Player List Failed. pUser is NULL or dwAuthKey Different" ) );
	}
}

void CDPDatabaseClient::OnFail( CAr & ar, DPID /*dpid*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
				CUser* pUser	= g_UserMng.GetUser( lpszAccount );
				if( pUser )
				{
					pUser->m_bIllegal	= TRUE;
					g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
					FLERROR_LOG( PROGRAM_NAME, _T( "ERROR_FLYFF_ACCOUNT, %s" ), lpszAccount );
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "pUser is NULL, %s" ), lpszAccount );
				}
				break;
			}
	}
}

void CDPDatabaseClient::OnLoginProtect( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT] = {0, };
	char lpszPlayer[MAX_PLAYER] = {0, };
	u_long idPlayer;
	BOOL bLogin;

	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );
	ar >> idPlayer >> bLogin;

	CLoginProtect::GetInstance()->SetError( idPlayer, bLogin );

	if( bLogin )
	{
		CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
		CUser* pUser	= g_UserMng.GetUser( dpid );
		if( pUser )
		{
			if( lstrcmp( lpszAccount, pUser->m_pKey ) == 0 )
			{
				pUser->m_idPlayer	= idPlayer;
				g_dpCoreClient.SendPreJoin( pUser->m_dwAuthKey, lpszAccount, idPlayer, lpszPlayer );	// o
				CLoginProtect::GetInstance()->SetNumPadId( dpid, TRUE );
			}
			else
				FLERROR_LOG( PROGRAM_NAME, _T( "Different lpszAccount, %s" ), lpszAccount );
		}
		else
			FLERROR_LOG( PROGRAM_NAME, _T( "dpid not found, %s" ), lpszAccount );
	}
	else
		g_dpLoginSrvr.SendLoginProtect( FALSE, dpid );
}

void CDPDatabaseClient::SendLoginProtect( const char* lpszAccount, const char* lpszPlayer, u_long idPlayer, int nBankPW, DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGIN_PROTECT_CERT, dpid );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPlayer );
	ar << idPlayer << nBankPW;
	SEND( ar, this, DPID_SERVERPLAYER );
}

CDPDatabaseClient	g_dpDBClient;
