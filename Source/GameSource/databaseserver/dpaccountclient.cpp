#include "StdAfx.h"

#include "dploginsrvr.h"
extern	CDPLoginSrvr	g_dpLoginSrvr;

#include "dpcoresrvr.h"
extern	CDPCoreSrvr		g_dpCoreSrvr;

extern	APP_INFO	g_appInfo;

#include "dpaccountclient.h"
#include <serialnumber.h>
#include "../_Common/FLServerDescription.h"

#include "../_Database/FLDataBase.h"
#include "../_Database/FLPlayer_Query.h"

#include "../_CommonDefine/Packet/FLPacket_Certify.h"

#include "dptrans.h"

CDPAccountClient::CDPAccountClient()
{
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, OnPlayerCount );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, OnOneHourNotify )
	ON_MSG( PACKETTYPE_FAIL, OnFail );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
	ON_MSG( PACKETTYPE_SERVER_LIST, OnServerList );
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
				break;
	}
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	FLASSERT( pfn );
	( this->*( pfn ) )( ar, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPAccountClient::SendAddIdofServer( DWORD dwIdofServer )
{
	BEFORESEND( ar, PACKETTYPE_MYREG );
	ar << dwIdofServer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAccount( char* lpszAccount )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ACCOUNT );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAllAccounts( void )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ALLACCOUNTS );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::OnRemovePlayer( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode = REMOVEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
/*
	account
	password
	szNo
	idPlayer
	dwAuthKey
	dpid
*/
}

void CDPAccountClient::OnGetPlayerList( CAr & ar, LPBYTE, u_long )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, _countof( lpDbOverlappedPlus->AccountInfo.szAccount ) );
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, _countof( lpDbOverlappedPlus->AccountInfo.szPassword ) );

	// BEGIN110520
	ar >> lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage;
	// END110520

	ar >> lpDbOverlappedPlus->dpid;
	lpDbOverlappedPlus->nQueryMode	= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey	= dwAuthKey;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
	FLPacketAtoDB_ChannelJoinAck fromA;
	fromA.Deserialize( ar );
	
//	DWORD dwAuthKey = 0;
//	ar >> dwAuthKey;	// o
//	char szAccount[ MAX_ACCOUNT ] = { 0 };
//	ar.ReadString( szAccount, _countof( szAccount ) );		//@@@@@@@@@@@@@@@@@@@@@@@@@ FLQuery_LoadPlayer에서 또읽는다. 
	if( fromA.m_eResult == E_LOGIN_SHUTDOWN_RULE )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_JOIN, fromA.m_kDpids.dpidCache, fromA.m_kDpids.dpidUser )
		ar << E_LOGIN_SHUTDOWN_RULE;
		SEND( ar, CDPTrans::GetInstance(), fromA.m_kDpids.dpid );
		return;
	}

	if( fromA.m_eResult == E_LOGIN_SUCCESS )
	{
		CHARACTER_DB().PostQueryCommand( fromA.m_kAccount.szAccount[ 0 ], new FLQuery_LoadPlayer( NULL_ID, NULL_ID, NULL_ID, lpBuf, uBufSize ) );
		return;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID RESULT (%d) ]" ), fromA.m_eResult );

#else
	UNREFERENCED_PARAMETER( ar );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= JOIN;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
#endif

}

void CDPAccountClient::OnPlayerCount( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnCloseExistingConnection( CAr & ar, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	g_dpLoginSrvr.SendCloseExistingConnection( lpszAccount, lError );
	g_dpCoreSrvr.SendCloseExistingConnection( lpszAccount, lError );
}


void CDPAccountClient::OnOneHourNotify( CAr & ar, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	g_dpLoginSrvr.SendOneHourNotify( lpszAccount );
}

void CDPAccountClient::OnFail( CAr & ar, LPBYTE, u_long )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				g_dpLoginSrvr.SendFail( lError, lpszAccount );
				break;
			}
	}
}

void CDPAccountClient::OnBuyingInfo( CAr & ar, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )	// 2
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// recv from account" ) );

	BUYING_INFO2 bi2;
	ar.Read( &bi2, sizeof(BUYING_INFO2) );
	if( bi2.dwServerIndex == g_appInfo.dwSys )
	{
		g_dpCoreSrvr.SendBuyingInfo( &bi2 );
	}
}

void CDPAccountClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPAccountClient::OnServerList( CAr & ar, LPBYTE, u_long )
{
	FLPacketServerDescriptionNoti kPacket;
	kPacket.Deserialize( ar );

	g_pServerDescription->Clear();

	for( std::vector< T_SERVER_DESC >::const_iterator kCItr = kPacket.vecServerDesc.begin(); kCItr != kPacket.vecServerDesc.end(); ++kCItr )
	{
		const T_SERVER_DESC kServerDesc = *kCItr;
		g_pServerDescription->Add( &kServerDesc );
	}
}

void CDPAccountClient::SendPacket( const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

CDPAccountClient	g_dpAccountClient;