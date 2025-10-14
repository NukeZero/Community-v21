#include "stdafx.h"
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
#include "dpcoresrvr.h"

#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;

#include "project.h"
extern	CProject	prj;

CDPCoreSrvr::CDPCoreSrvr()
{
	ON_MSG( PACKETTYPE_ADD_MESSENGER, OnAddMessenger );
	ON_MSG( PACKETTYPE_DELETE_MESSENGER, OnDeleteMessenger );
	ON_MSG( PACKETTYPE_UPDATE_MESSENGER, OnUpdateMessenger );

	ON_MSG( PACKETTYPE_ADDPARTYNAME, OnAddPartyName );
	ON_MSG( PACKETTYPE_TAG, OnTag );

	ON_MSG( PACKETTYPE_CREATE_GUILD, OnCreateGuild );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_CD_GUILD_LOGO, OnGuildLogo );
	ON_MSG( PACKETTYPE_CD_GUILD_NOTICE, OnGuildNotice );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, OnGuildAuthority );
	ON_MSG( PACKETTYPE_GUILD_PENYA, OnGuildPenya );
	ON_MSG( PACKETTYPE_GUILD_SETNAME, OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, OnGuildMemberLv );
	ON_MSG( PACKETTYPE_GUILD_CLASS, OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, OnGuildNickName );
	ON_MSG( PACKETTYPE_ACPT_GUILD_WAR, OnAcptWar );
	ON_MSG( PACKETTYPE_WAR_END, OnWarEnd );
	ON_MSG( PACKETTYPE_SURRENDER, OnSurrender );
	ON_MSG( PACKETTYPE_WAR_DEAD, OnWarDead );
	ON_MSG( PACKETTYPE_WAR_MASTER_ABSENT, OnWarMasterAbsent );
	ON_MSG( PACKETTYPE_CD_ADDVOTE ,			OnAddVote );
	ON_MSG( PACKETTYPE_CD_REMOVEVOTE,		OnRemoveVote );
	ON_MSG( PACKETTYPE_CD_CLOSEVOTE	,		OnCloseVote );
	ON_MSG( PACKETTYPE_CD_CASTVOTE	,		OnCastVote );
	ON_MSG( PACKETTYPE_CT_WANTED	,		OnWanted );
	ON_MSG( PACKETTYPE_SETSNOOPGUILD, OnSnoopGuild );
	ON_MSG( PACKETTYPE_CHG_MASTER, OnChgMaster );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
}

CDPCoreSrvr::~CDPCoreSrvr()
{

}

void CDPCoreSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*dpid*/ )
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

void CDPCoreSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*dpid*/ )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	//	FLASSERT( pfn );
	if( pfn )
		( this->*( pfn ) )( ar, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPCoreSrvr::OnAddConnection( DPID dpid )
{
	TCHAR szIP[16] = { 0, };
	GetPlayerAddr( dpid, szIP, _countof( szIP ) );
	FLINFO_LOG( PROGRAM_NAME, _T( "Connected CoreServer IP:[%s]" ), szIP );

	SendPartyName( dpid );
	BEFORESEND( ar, PACKETTYPE_GLOBAL_DATA );
//	g_DbManager.OpenGuild();
	g_GuildMng.Serialize( ar, FALSE );
	CGuildTable::GetInstance().Serialize( ar );
	g_GuildWarMng.Serialize( ar );
	ar.Write( (const void*)prj.m_aExpParty, sizeof(prj.m_aExpParty) );

#if defined(__INTERNALSERVER )
	g_DbManager.OpenWanted( ar );	
#endif

	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnRemoveConnection( DPID /*dpid*/ )
{

}

void CDPCoreSrvr::SendPartyName( DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_PARTYNAME );

	ar << (u_long)g_DbManager.m_2PartyNamePtr.size();
	for( ULONG2STRING::iterator i = g_DbManager.m_2PartyNamePtr.begin(); i != g_DbManager.m_2PartyNamePtr.end(); ++i )
	{
		ar << (*i).first;
		ar.WriteString( (*i).second.c_str() );
	}
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnCreateGuild( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnCreateGuild" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CREATE_GUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnDestroyGuild( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnDestroyGuild" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= DESTROY_GUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnAddGuildMember( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnAddGuildMember" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_GUILD_MEMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnRemoveGuildMember( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnRemoveGuildMember" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_GUILD_MEMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildMemberLv( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnGuildMemberLv" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_MEMBER_LEVEL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildClass( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnGuildClass" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_CLASS;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPCoreSrvr::OnGuildNickName( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnGuildNickName" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_NICKNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPCoreSrvr::OnChgMaster( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnChgMaster" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_UPDATEGUILDMASTER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPCoreSrvr::OnGuildLogo( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildLogo" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_LOGO;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::OnGuildNotice( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildNotice" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_NOTICE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::OnAddVote( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_CD_ADDVOTE" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_GUILD_VOTE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnRemoveVote( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_CD_REMOVEVOTE" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_GUILD_VOTE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnCloseVote( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_CD_CLOSEVOTE" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CLOSE_GUILD_VOTE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnCastVote( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_CD_CASTVOTE" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CAST_GUILD_VOTE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

// 투표입력의 결과
void CDPCoreSrvr::SendAddVoteResult( VOTE_INSERTED_INFO & info )
{
	BEFORESEND( ar, PACKETTYPE_DC_ADDVOTERESULT );
	ar << info;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::OnGuildAuthority( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildAuthority" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_AUTHORITY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildPenya( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildPenya" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_PENYA;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildSetName( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildSetName" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GUILD_SETNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::OnAcptWar( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_ACPT_GUILD_WAR" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ACPT_GUILD_WAR;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnWarEnd( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_WAR_END" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= WAR_END;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnSurrender( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_SURRENDER" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SURRENDER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnWarDead( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_WAR_DEAD" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= WAR_DEAD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnWarMasterAbsent( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_WAR_MASTER_ABSENT" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= WAR_MASTER_ABSENT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnAddPartyName( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_PARTYNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::OnTag( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPCoreSrvr::OnTag" ) );

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= INSERT_TAG;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::SendTagResult( u_long idFrom, BYTE cbResult )
{
	BEFORESEND( ar, PACKETTYPE_INSERTTAG_RESULT );
	ar << idFrom;
	ar << cbResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendDelPlayer( u_long idPlayer, u_long idGuild )
{
	BEFORESEND( ar, PACKETTYPE_DEL_PLAYER );
	ar << idPlayer << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendMemberTime( u_long idPlayer, LPCTSTR tGuildMember )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_MEMBERTIME );
	ar << idPlayer;
	ar.WriteString( tGuildMember );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendBuyingInfo( PBUYING_INFO2 pbi2 )	// 3
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( pbi2, sizeof(BUYING_INFO2) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

// 현상금시스템 관련 요청처리 
void CDPCoreSrvr::OnWanted( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_CD_CASTVOTE" ) );

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= WANTED_OP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f )
{
	BEFORESEND( ar, PACKETTYPE_QUERYSETPLAYERNAME );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwData << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void	CDPCoreSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG lError )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );
	ar << lError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnSnoopGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	if( uBufSize > 1000 )	// 1032
		return;

	u_long idGuild;
	char lpszChat[1024]		= { 0, };
	ar >> idGuild;
	ar.ReadString( lpszChat, _countof( lpszChat ) );

	FLTRACE_LOG( PROGRAM_NAME, _T( "recv PACKETTYPE_SETSNOOPGUILD" ) );

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SNOOPGUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void	CDPCoreSrvr::OnBuyingInfo( CAr & ar, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BUYING_INFO2 bi2;
	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );

	g_dpAccountClient.SendBuyingInfo( &bi2 );
}

void CDPCoreSrvr::SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESEND( ar, PACKETTYPE_UPDATE_PLAYER_DATA );
	ar << idPlayer;
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnAddMessenger( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_ADD_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::OnDeleteMessenger( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_DELETE_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::OnUpdateMessenger( CAr & /*ar*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_UPDATE_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::SendRemovePlayerFriend( u_long uPlayerId, u_long uFriendId )
{
	BEFORESEND( ar, PACKETTYPE_REMOVEFRIEND );
	ar << uPlayerId << uFriendId;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendEventLuaNotice()
{
	BEFORESEND( ar, PACKETTYPE_EVENTLUA_NOTICE );
	std::vector<std::string> vecstrMessage;
	vecstrMessage = prj.m_EventLua.GetNoticeMessage();
	ar << vecstrMessage.size();
	for( DWORD i=0; i<vecstrMessage.size(); i++ )
	{
		ar.WriteString( vecstrMessage.at( i ).c_str() );
	}
	SEND( ar, this, DPID_ALLPLAYERS );
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPCoreSrvr::SendNoticeMessage( const TCHAR* szNotice )
{
	if( _tcslen( szNotice ) > 0 )
	{
		BEFORESEND( ar, PACKETTYPE_EVENTLUA_NOTICE );
		ar << 1;
		ar.WriteString( szNotice );
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}
#endif // __GUILD_HOUSE_MIDDLE

CDPCoreSrvr		g_dpCoreSrvr;