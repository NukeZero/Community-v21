#include "StdAfx.h"
#include "DPCacheSrvr.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include <ServerDesc.h>
#include <logging/FLMyTrace.h>
#include "Player.h"

extern CDPCacheSrvr		g_DPCacheSrvr;
extern	CDPClientArray	g_DPClientArray;
extern	FLMyTrace		g_MyTrace;

CDPCoreClient::CDPCoreClient()
{
	ON_MSG( PACKETTYPE_PROCSERVER_LIST, OnProcServerList );
	ON_MSG( PACKETTYPE_LOAD_WORLD, OnProcServer );
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_LEAVE, OnLeave );
	ON_MSG( PACKETTYPE_DESTROY_ALLPLAYERS, OnDestroyAllPlayers );
	ON_MSG( PACKETTYPE_KILLPLAYER, OnKillPlayer );

	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );

//	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
}

CDPCoreClient::~CDPCoreClient()
{

}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				g_MyTrace.Add( FLMyTrace::Key( "main" ), TRUE, "destroy player or group." );
				break;
			}
	}
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );

	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn )
		( this->*( pfn ) )( ar, dpidUser );
	else
	{
		switch( dw )
		{
//			case PACKETTYPE_SUMMONPLAYER:
//			case PACKETTYPE_TELEPORTPLAYER:
			case PACKETTYPE_MODIFYMODE:
//			case PACKETTYPE_BUYING_INFO:
				{
					CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
					g_DPClientArray.SendToServer( dpidUser, lpMsg, dwMsgSize );

					break;
				}
			default:
				g_DPCacheSrvr.Send( lpBuf, uBufSize, dpidUser );
				break;
		}
	}
}

void CDPCoreClient::SendDestroyPlayer( CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, pPlayer->GetSerial() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendAddPlayer( CPlayer* pPlayer, CRTMessenger & rtmessenger )
{
	BEFORESENDSOLE( ar, PACKETTYPE_JOIN, pPlayer->GetSerial() );
	ar << pPlayer->GetAuthKey();
	ar << pPlayer->GetPlayerId();
	ar << pPlayer->GetNetworkId();
	ar << pPlayer->GetParty();
	ar << pPlayer->GetGuild();
	ar << pPlayer->GetWar();
	ar << pPlayer->GetChannel();
	rtmessenger.Serialize( ar );
	ar.WriteString( pPlayer->GetPlayer() );
	ar.WriteString( pPlayer->GetAddr() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnProcServerList( CAr & ar, DPID )
{
	CServerDesc *pServer;
	short nSize;

	ar >> nSize;
	for( int i = 0; i < nSize; i++ )
	{
		pServer		= new CServerDesc;
		pServer->Serialize( ar );

		if( !g_DPClientArray.Connect( pServer ) ) {
			SAFE_DELETE( pServer );
		}
	}
	g_MyTrace.AddLine( '-' );
}

void CDPCoreClient::OnProcServer( CAr & ar, DPID )
{
	CServerDesc* pServer	= new CServerDesc;
	pServer->Serialize( ar );
	if( !g_DPClientArray.Connect( pServer ) ) {
		SAFE_DELETE( pServer );
	}
}

void CDPCoreClient::OnJoin( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	BYTE byData;
	ar >> dwSerial;
	ar >> byData;	// 사용하지 않는다.

	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );
	if( pPlayer == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "player not found" ) );
		return;
	}

	CDPClient* pClient = g_DPClientArray.GetClient( pPlayer->GetChannel(), pPlayer->GetWorld(), vector3(0.0f, 0.0f, 0.0f) );
	if( pClient )
	{
		pPlayer->SetClient( pClient );
		pClient->SendJoin( pPlayer );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "server not found id: %d account: %s player: %s channel: %d world: %d" ),
			      pPlayer->GetPlayerId(), pPlayer->GetAccount(), pPlayer->GetPlayer(), pPlayer->GetChannel(), pPlayer->GetWorld() );
		g_DPCacheSrvr.DestroyPlayer( dpid );
	}
}

void CDPCoreClient::OnLeave( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	ar >> dwSerial;
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );  // dpid가 serial번호이다.
	if( pPlayer )
		g_DPCacheSrvr.DestroyPlayer( dpid );
}

void CDPCoreClient::OnDestroyAllPlayers( CAr & /*ar*/, DPID /*dpid*/ )
{
	CPlayerMng::Instance()->DestroyPlayer( NULL );
}

void CDPCoreClient::OnKillPlayer( CAr & /*ar*/, DPID dpid )
{
	g_DPCacheSrvr.DestroyPlayer( dpid );
}

void	CDPCoreClient::OnCloseExistingConnection( CAr & ar, DPID /*dpid*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> lError;

	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	ar >> dwSerial;
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer( lpszAccount );
	if( pPlayer )
	{
		g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );
	}
}

void CDPCoreClient::SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize )
{
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpidUser );
	if( !pPlayer )
		return;
	*reinterpret_cast<UNALIGNED DPID*>( lpMsg )		= pPlayer->GetSerial();
	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );
}

// void CDPCoreClient::OnBuyingInfo( CAr & ar, DPID dpid )
// {
// 	BUYING_INFO2 bi2;
// 	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
// 
// 	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
// 	
// 	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer( dpid );
// 	if( pPlayer != NULL && pPlayer->GetClient() != NULL && pPlayer->GetClient()->m_pServer != NULL )
// 	{
// 		pPlayer->GetClient()->SendBuyingInfo( &bi2, pPlayer );
// 	}
// 	else
// 	{
// 		SendBuyingInfo( &bi2 );
// 	}
// }

// void CDPCoreClient::SendBuyingInfo( PBUYING_INFO2 pbi2 )
// {
// 	BEFORESENDSOLE( ar, PACKETTYPE_BUYING_INFO, DPID_UNKNOWN );
// 	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
// 	SEND( ar, this, DPID_SERVERPLAYER );
// }

CDPCoreClient	g_DPCoreClient;