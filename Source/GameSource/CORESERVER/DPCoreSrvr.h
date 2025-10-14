#ifndef __DPCORESRVR_H__
#define __DPCORESRVR_H__

#include <network/FLDPMng.h>
#include <msghdr.h>
#include <ServerDesc.h>
#include <ObjMap.h>
#include <guild.h>
#include <InstanceDungeonBase.h>


class CDPCoreSrvr : public CDPMng
{
public:
	int					m_nGCState;
public:
	CServerDescArray	m_apSleepServer;
	CServerDescArray	m_apServer;	// active
	std::map<u_long, DPID>	m_mapWorldKeyToDPID;	// WorldServer Key( ex. 101, 201.. equal g_uKey ) to DPID
	std::map<u_long, DPID>	m_mapWorldIDToDPID;		// WorldServer MultiID( ex. 1, 2.. equal m_uIdofMulti ) to DPID
	CMclCritSec		m_AccessLock;
	CObjMap		m_objmap;
public:
	// Constructions
	CDPCoreSrvr();
	virtual	~CDPCoreSrvr();
	
	// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendRecharge( u_long uBlockSize, DPID dpid );
	void	SendDuplicate( u_long uWorldSrvr );
	void	SendUnify( u_long uWorldSrvr );
	void	SendQueryTickCount( DWORD dwTime, DPID dpid, double dCurrentTime );
//	Operator commands
	void	SendShout( u_long idPlayer, const CHAR* lpString, DPID dpid );
	void	SendPlayMusic( u_long idmusic, DWORD dwWorldID, DPID dpid );
	void	SendPlaySound( u_long idsound, DWORD dwWorldID, DPID dpid );

	void	SendSetPartyMode( u_long idParty, int nMode, BOOL bOnOff, LONG nPoint = 0 , DWORD dwSkillTime = 0 );

	void	SendRemoveParty( u_long idParty, u_long idLeader, u_long idMember );
	void	SendPartyChangeTroup( u_long idParty, const char * szPartyName );
	void	SendPartyChangeName( u_long idParty, const char * szPartyName );
	void	SendPartyChangeItemMode( u_long idParty, int nItemMode );
	void	SendPartyChangeExpMode( u_long idParty, int nExpMode );

//	void	SendAddPartyMember( u_long uPartyId, u_long idLeader, long nLeaderLevel, long nLeaderJob, BYTE nLeaderSex, u_long idMember, long nMemberLevel, long nMemberJob, BYTE nMemberSex );
	void	SendAddPartyMember( const u_long uPartyId, const u_long idLeader, const u_long idMember );
	void	SendAddFriend( u_long uidSender, u_long uidFriend, BYTE nSenderSex, BYTE nFriendSex, LONG nSendJob, LONG nFriendJob );
	void	SendRemoveFriend( u_long uidSender, u_long uidFriend );
	void	SendSeasonEffect();
	void	SendPartyChat( u_long idParty, const CHAR* lpName, const CHAR* lpString, OBJID objid );
	void	SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f );

	void	SendDestroyPlayer( u_long idPlayer, DPID dpid );

	void	SendEvent( DWORD dwEvent );
	void	SendGuildCombatState( void );
	void	SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease );

	// Handlers
	USES_PFNENTRIES( CDPCoreSrvr, ( CAr & ar, DPID, DPID, DPID, u_long ) );
	void	OnAddConnection( CAr & ar, DPID dpid, DPID, DPID, u_long );
	void	OnRemoveConnection( DPID dpid );
	void	OnRecharge( CAr & ar, DPID dpid, DPID, DPID, u_long );
	void	OnJoin( CAr & ar, DPID dpid, DPID, DPID, u_long );		// idPlayer, szPlayer, bOperator

//	Operator commands
	void	OnWhisper( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnSay( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnModifyMode( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnShout( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGMSay( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPlayMusic( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPlaySound( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSummonPlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnTeleportPlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnKillPlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetPlayerAddr( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetPlayerCount( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetCorePlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSystem( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnCaption( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnAddPartyExp( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnRemovePartyPoint( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnSeasonEffectByGM( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnForceSetPartySkillTime( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPartySkillUse( CAr & ar, DPID, DPID, DPID, u_long uBufSize );
	void	OnPartyChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPartyLevel( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnLoadConstant( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGameRate( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetMonsterRespawn( CAr & ar, DPID, DPID, DPID, u_long );

	void    OnGuildLogo( CAr & ar, DPID, DPID, DPID, u_long );
	void    OnGuildContribution( CAr & ar, DPID, DPID, DPID, u_long );
	void    OnGuildNotice( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGuildRealPenya( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnSetSnoop( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetSnoopGuild( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPing( CAr & ar, DPID dpid, DPID, DPID, u_long );

	void	OnGCState( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGCRemoveParty( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGCAddParty( CAr & ar, DPID, DPID, DPID, u_long );

public:
	DPID	GetWorldSrvrDPIDByMultiID( u_long uIdofMulti );

private:
	DPID	GetWorldSrvrDPID( u_long uWorldSrvr );
private:
	DPID	GetWorldSrvrDPID( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos );
	u_long	GetIdofMulti( DPID dpid );

public:
	void	OnGuildMsgControl( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnCreateGuild( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGuildChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	SendCreateGuild( GUILD_MEMBER_INFO* pInfo, int nSize, u_long idGuild, const char* szGuild );
	void	SendDestroyGuild( u_long idGuild );
	void	SendAddGuildMember( const GUILD_MEMBER_INFO & info, u_long idGuild );
	void	SendRemoveGuildMember( u_long idMember, u_long idGuild );
	void	SendGuildMemberLv( u_long idPlayer, int nMemberLv, u_long idGuild );
	
	void	SendGuildClass( u_long idPlayer, int nClass, u_long idGuild );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, u_long idGuild );
	void	SendGuildMemberLogOut( u_long idGuild, u_long idPlayer );
	void	SendGuildAuthority( u_long uGuildId, DWORD dwAuthority[] );
	void	SendGuildPenya( u_long uGuildId, DWORD dwType, DWORD dwPenya );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2, u_long idGuild );
	void	SendGuildSetName( u_long idGuild, const char * lpszGuild , u_long idPlayer = 0, BYTE nId = 0xff, BYTE nError = 0 );

	void	SendAddVoteResult( VOTE_INSERTED_INFO& info );
	void	SendAddRemoveVote( u_long idGuild, u_long idVote );		
	void	SendAddCloseVote( u_long idGuild, u_long idVote );
	void	SendAddCastVote( u_long idGuild, u_long idVote, BYTE cbSelection );

	void	SendAcptWar( u_long idWar, u_long idDecl, u_long idAcpt );
	void	SendWarDead( u_long idWar, BOOL bDecl );
	void	SendWarEnd( u_long idWar, int nWptDecl, int nWptAcpt, int nType );
	void	SendSurrender( u_long idWar, u_long idPlayer, BOOL bDecl );
	void	SendWarMasterAbsent( u_long idWar, BOOL bDecl );
	void	OnWarDead( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWarMasterAbsent( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWarTimeout( CAr & ar, DPID, DPID, DPID, u_long );

public:
	void	SendSetFriendState( u_long uidPlayer, DWORD dwState );
	void	SendChangeLeader( u_long uPartyId, u_long uidChangeLeader );
	void	SendFriendInterceptState( u_long uIdPlayer, u_long uIdFriend );
		
	void	OnAddFriendNameReqest( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnBlock( CAr & ar, DPID, DPID, DPID, u_long );

	void	UpdateWantedList();
	void	SendCWWantedList( DPID dpid = DPID_ALLPLAYERS );
	void	OnWCWantedGold( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWCWantedReward( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetPartyDuel( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnQuerySetGuildName( CAr & ar, DPID, DPID, DPID, u_long );
	void	SendRemoveUser( DWORD dwSerial );

public:
	void	SendInstanceDungeonAllInfo( int nType, CInstanceDungeonBase* pID, DPID dpId );
	void	SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info, const DWORD dwPlayerID );
	void	SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info );
	void	SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO CT_Info );
	void	SendInstanceDungeonResetCoolTimeInfo( const int nType, const DWORD dwWorldId, const DWORD dwDungeonId );
	void	SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId );
private:
	void	OnInstanceDungeonCreate( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnInstanceDungeonResetCoolTimeInfo( CAr & ar, DPID, DPID, DPID, u_long );

private:
	void	OnQuizSystemMessage( CAr & ar, DPID, DPID, DPID, u_long );

	//void	OnRemoveGuildMember( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnRemovePartyMember( CAr & ar, const DPID /*dpidFrom*/, const DPID dpidCache, const DPID dpidUser, const u_long uBufSize );

	void	OnBroadCastMessage( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnSetPartyEffectModeNoti( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnRefreshPartyEffectModeNoti( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnAddPartyMember( CAr & ar, DPID, DPID, DPID, u_long );

public:
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, DPID dpid );
};

inline DPID CDPCoreSrvr::GetWorldSrvrDPID( u_long uWorldSrvr )
{
	std::map<u_long, DPID>::iterator i	= m_mapWorldKeyToDPID.find( uWorldSrvr );
	if( i != m_mapWorldKeyToDPID.end() )
		return i->second;
	return DPID_UNKNOWN;
}

inline DPID CDPCoreSrvr::GetWorldSrvrDPID( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos )
{
	if( uIdofMulti == NULL_ID )
		return DPID_UNKNOWN;

	for( CServerDescArray::iterator i = m_apServer.begin(); i != m_apServer.end(); ++i )
	{
		CServerDesc* pServer	= i->second;
		if( pServer->GetIdofMulti() == uIdofMulti && pServer->IsUnderJurisdiction( dwWorldID, vPos ) )
			return (DPID)i->first;
	}
	return DPID_UNKNOWN;	
}

inline u_long CDPCoreSrvr::GetIdofMulti( DPID dpid )
{
	CServerDescArray::iterator i	= m_apServer.find( dpid );
	if( i != m_apServer.end() )
		return i->second->GetIdofMulti();
	return NULL_ID;
}

inline DPID CDPCoreSrvr::GetWorldSrvrDPIDByMultiID( u_long uIdofMulti )
{
	std::map<u_long, DPID>::iterator i	= m_mapWorldIDToDPID.find( uIdofMulti );
	if( i != m_mapWorldIDToDPID.end() )
	{
		return i->second;
	}

	return DPID_UNKNOWN;
}

#endif	// __DPCORESRVR_H__
