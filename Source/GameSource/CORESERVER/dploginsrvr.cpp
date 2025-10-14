#include "stdafx.h"
#include "dpcachesrvr.h"
#include "dploginsrvr.h"
#include <msghdr.h>
#include "player.h"

extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CPlayerMng	g_PlayerMng;

// %Y%m%d%H%M%S

CDPLoginSrvr::CDPLoginSrvr() 
{
	m_dpid	= 0xFFFFFFFF;

	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_PRE_JOIN, OnPreJoin );
	ON_MSG( PACKETTYPE_LEAVE, OnLeave );
}

CDPLoginSrvr::~CDPLoginSrvr()
{

}

void CDPLoginSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPLoginSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar );
	}
	else {
		FLASSERT( 0 );
	}
}

void CDPLoginSrvr::OnAddConnection( DPID dpid )
{
	if( m_dpid == 0xFFFFFFFF )
		m_dpid	= dpid;
}

void CDPLoginSrvr::OnRemoveConnection( DPID dpid )
{
	if( m_dpid == dpid )
	{
		g_DPCacheSrvr.SendHdr( PACKETTYPE_DESTROY_ALLPLAYERS, DPID_ALLPLAYERS, DPID_ALLPLAYERS );
		m_dpid	= 0xFFFFFFFF;
		g_PlayerMng.Free();
	}
}

void CDPLoginSrvr::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	ar >> dwTime;

	BEFORESEND( ar1, PACKETTYPE_QUERYTICKCOUNT );
	ar1 << dwTime << g_TickCount.GetTickCount();
	SEND( ar1, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::OnPreJoin( CAr & ar )
{
	TCHAR lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	u_long idPlayer;
	DWORD dwAuthKey;

	ar >> dwAuthKey;
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	BOOL f	= g_PlayerMng.AddPlayer( idPlayer, lpszPlayer, lpszAccount );	// lock
	if( f == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "AddPlayer() return false, %d, %s" ), idPlayer, lpszPlayer );
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
//		if( !pPlayer )
//		{
//			FLERROR_LOG( PROGRAM_NAME, _T( "ID not found" ) );
//			pPlayer		= g_PlayerMng.GetPlayer( lpszPlayer );
//		}
		if( pPlayer )
		{
			if( *pPlayer->lpszAccount != '\0' )
				SendQueryRemovePlayer( pPlayer->lpszAccount );
			else
				FLERROR_LOG( PROGRAM_NAME, _T( "lpszAccount == '\0'" ) );

			g_PlayerMng.RemovePlayer( pPlayer );
		}
		else
			FLERROR_LOG( PROGRAM_NAME, _T( "Player not found" ) );
	}

	BEFORESEND( ar1, PACKETTYPE_PRE_JOIN );	// result
	ar1 << dwAuthKey;
	ar1.WriteString( lpszAccount );
	ar1 << idPlayer;
	ar1.WriteString( lpszPlayer );
	ar1 << (BYTE)f;	// o
	SEND( ar1, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::OnLeave( CAr & ar )
{
	u_long idPlayer;
	ar >> idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		g_PlayerMng.RemovePlayer( pPlayer );
	}
	else
		FLERROR_LOG( PROGRAM_NAME, _T( "player not found, %d" ), idPlayer );
}

void CDPLoginSrvr::SendQueryRemovePlayer( const CHAR* lpszAccount )
{
	BEFORESEND( ar, PACKETTYPE_DESTROY_PLAYER );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

CDPLoginSrvr	g_dpLoginSrvr;