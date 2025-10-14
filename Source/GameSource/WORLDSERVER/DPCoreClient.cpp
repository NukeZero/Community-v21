#include "StdAfx.h"
#include "DPCoreClient.h"
#include "User.h"
#include <ServerDesc.h>
#include <WorldMng.h>

#include <playerdata.h>

#include "FLCooperativeContributions.h"


#include <eveschool.h>
extern	CGuildCombat	g_GuildCombatMng;

extern	CWorldMng	g_WorldMng;

#include <Party.h>
extern	CPartyMng	g_PartyMng;

#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;

#include "WantedListSnapshot.h"

#include "../_Common/InstanceDungeonParty.h"

#include <dump/FLDumpRunner.h>
#include "../_CommonDefine/FLCommonDumper.h"

#include "FLFlyffPieceDailyPayment.h"

#include "../_Common/SeasonEffect.h"


#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
	u_long nBufSize;	\
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK

extern char				g_szCoreAddr[16];

CDPCoreClient	g_DPCoreClient;


CDPCoreClient::CDPCoreClient()
{
	ON_MSG( PACKETTYPE_LOAD_WORLD, OnLoadWorld );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_RECHARGE_IDSTACK, OnRecharge );
	ON_MSG( PACKETTYPE_SHOUT, OnShout );
	ON_MSG( PACKETTYPE_PLAYMUSIC, OnPlayMusic );
	ON_MSG( PACKETTYPE_PLAYSOUND, OnPlaySound );
//	ON_MSG( PACKETTYPE_MODIFYMODE, OnModifyMode );
	
	ON_MSG( PACKETTYPE_ERRORPARTY, OnErrorParty );
	ON_MSG( PACKETTYPE_ADDPARTYMEMBER, OnAddPartyMember );
	ON_MSG( PACKETTYPE_REMOVEPARTYMEMBER, OnRemovePartyMember );
	ON_MSG( PACKETTYPE_ADDPLAYERPARTY, OnAddPlayerParty );
	ON_MSG( PACKETTYPE_REMOVEPLAYERPARTY, OnRemovePlayerParty );

	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, OnGuildMemberLv );
	ON_MSG( PACKETTYPE_SETPARTYEXP, OnSetPartyExp );
	ON_MSG( PACKETTYPE_REMOVEPARTYPOINT, OnRemovePartyPoint );
	ON_MSG( PACKETTYPE_CHANGETROUP, OnPartyChangeTroup );
	ON_MSG( PACKETTYPE_CHANPARTYNAME, OnPartyChangeName );
	ON_MSG( PACKETTYPE_SETPARTYMODE, OnSetPartyMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEITEMMODE, OnPartyChangeItemMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEEXPMODE, OnPartyChangeExpMode );
	ON_MSG( PACKETTYPE_ADDFRIEND, OnAddFriend );
	ON_MSG( PACKETTYPE_REMOVEFRIEND, OnRemovefriend );

	ON_MSG( PACKETTYPE_SEASON_EFFECT, OnSeasonEffect );

	ON_MSG( PACKETTYPE_PARTYCHAT, OnPartyChat );

	ON_MSG( PACKETTYPE_CREATE_GUILD, OnCreateGuild );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_CW_GUILDLOGO, OnGuildLogoACK );
	ON_MSG( PACKETTYPE_CW_GUILDCONTRIBUTION, OnGuildContributionACK );
	ON_MSG( PACKETTYPE_CW_GUILDNOTICE, OnGuildNoticeACk );
	ON_MSG( PACKETTYPE_CW_ADDVOTERESULT, OnAddVoteResultACk );
	ON_MSG( PACKETTYPE_CW_MODIFYVOTE, OnModifyVote );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, OnGuildAuthority );
	ON_MSG( PACKETTYPE_GUILD_PENYA, OnGuildPenya );
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, OnGuildRealPenya );
	ON_MSG( PACKETTYPE_GUILD_SETNAME, OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_MSG_CONTROL, OnGuildMsgControl)
	ON_MSG( PACKETTYPE_GUILD_CLASS, OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, OnGuildNickName );

	ON_MSG( PACKETTYPE_GUILD_MEMBERLOGOUT, OnGuildMemberLogOut );
	ON_MSG( PACKETTYPE_ACPT_GUILD_WAR, OnAcptWar );
	ON_MSG( PACKETTYPE_WAR_DEAD, OnWarDead );
	ON_MSG( PACKETTYPE_WAR_END, OnWarEnd );
	ON_MSG( PACKETTYPE_SURRENDER, OnSurrender );

	ON_MSG( PACKETTYPE_FRIENDINTERCEPTSTATE, OnFriendInterceptState );
	ON_MSG( PACKETTYPE_SETFRIENDSTATE, OnSetFriendState );

	ON_MSG( PACKETTYPE_GAMERATE, OnGameRate );
	ON_MSG( PACKETTYPE_LOADCONSTANT, OnLoadConstant );
	ON_MSG( PACKETTYPE_SETMONSTERRESPAWN, OnSetMonsterRespawn );
	ON_MSG( PACKETTYPE_PARTYCHANGELEADER, OnPartyChangeLeader );

	ON_MSG( PACKETTYPE_CW_WANTED_LIST, OnCWWantedList );
	ON_MSG( PACKETTYPE_CW_WANTED_REWARD, OnCWWantedReward );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, OnSetPlayerName );
	ON_MSG( PACKETTYPE_SETSNOOP, OnSetSnoop );

	ON_MSG( PACKETTYPE_EVENT, OnEvent );
	ON_MSG( PACKETTYPE_CHG_MASTER, OnChgMaster );
	ON_MSG( PACKETTYPE_GUILDCOMBAT_STATE, OnGuildCombatState );
	ON_MSG( PACKETTYPE_UNIFY, OnRemoveUserFromCORE );
	ON_MSG( PACKETTYPE_PING, OnPing );

	ON_MSG( PACKETTYPE_DESTROY_PLAYER, OnDestroyPlayer );
    //////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_ALLINFO, OnInstanceDungeonAllInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_CREATE, OnInstanceDungeonCreate );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DESTROY, OnInstanceDungeonDestroy );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, OnInstanceDungeonSetCoolTimeInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_RESET_COOLTIME, OnInstanceDungeonResetCoolTimeInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, OnInstanceDungeonDeleteCoolTimeInfo );

	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );

	ON_MSG( PACKETTYPE_SET_PARTY_EFFECT_MODE_NOTI, OnSetPartyEffectModeNoti );

	ON_MSG( PACKETTYPE_REFRESH_PARTY_EFFECT_MODE_NOTI, OnRefreshPartyEffectModeNoti );

	ON_MSG( PACKETTYPE_START_FLYFF_PIECE_BURNING_MODE, OnStartFlyffPieceBurningMode );
	ON_MSG( PACKETTYPE_END_FLYFF_PIECE_BURNING_MODE, OnEndFlyffPieceBurningMode );
	
	m_bAlive	= TRUE;
	m_hWait		= WSACreateEvent();
	m_uRecharge		= 0;
}

CDPCoreClient::~CDPCoreClient()
{
	CLOSE_HANDLE( m_hWait );
}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC /*lpMsg*/, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{

}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - ( sizeof(DPID) + sizeof(DPID) ) );
	GETTYPE( ar );

	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), NULL_ID );
	}
	else {
		switch( dw )
		{
			case PACKETTYPE_PASSAGE:
			{
				DWORD objid, dwtmp;
				ar >> objid >> dwtmp;

				pfn		= GetHandler( dwtmp );
				FLASSERT( pfn != NULL );
				( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), (OBJID)objid );
				break;
			}
			case PACKETTYPE_BROADCAST:
				{
					DWORD dwtmp;
					ar >> dwtmp;

					pfn		= GetHandler( dwtmp );
					FLASSERT( pfn != NULL );
					( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), NULL_ID );
					break;
				}
			default:
				FLERROR_LOG( PROGRAM_NAME, _T( "Handler not found(%08x)" ), dw );
				break;
		}
	}


}

BOOL CDPCoreClient::Run( LPSTR lpszAddr, USHORT uPort, u_long uKey )
{
	if( ConnectToServer( lpszAddr, uPort, FLNormalProtocol::GetInstance(), TRUE ) )
	{
		MyRegister( uKey );
		return ( WaitForSingleObject( m_hWait, INFINITE ) != WAIT_TIMEOUT );
	}
	return FALSE;
}

void CDPCoreClient::MyRegister( u_long uKey )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MYREG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uKey;	// uKey는 g_uKey와 동일한 값 
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );
}

BOOL CDPCoreClient::CheckIdStack( void )
{
	int cbUser	= g_xWSUserManager->GetCount();
	u_long uMax		= cbUser * 128 + 20480;
	int cbStack		= prj.m_objmap.m_idStack.GetCount();
	
	if( cbStack + m_uRecharge < uMax * 5 / 10 )	// 50%
	{
		m_uRecharge		= uMax - cbStack;
		Recharge( m_uRecharge );
		return FALSE;
	}
	return ( m_uRecharge == 0 );
}

void CDPCoreClient::Recharge( u_long uBlockSize )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uBlockSize;
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPCoreClient::SendToServer( LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser )
{
	CAr ar;
	u_long nBufSize;
	BYTE* lpBuf;

	ar << dpidCache << dpidUser;
	ar.Write( lpBuffer, uBufSize );	// overhead

	lpBuf	= ar.GetBuffer( &nBufSize );
	Send( (LPVOID)lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendEventRealItem( u_long uIdPlayer, int nRealItemIndex, int nRealItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RENEWEVNET, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uIdPlayer << nRealItemIndex << nRealItemCount;
	PASS( ar );
}

void CDPCoreClient::SendPartyLevel( FLWSUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idparty << pUser->m_idPlayer << dwLevel << dwPoint << dwExp;	
	PASS( ar );
}

void CDPCoreClient::SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader , BOOL bLeaderSMExpUp )
{
	//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << nMonLv << bSuperLeader << bLeaderSMExpUp;
	PASS( ar );
}

void CDPCoreClient::SendRemovePartyPoint( u_long uPartyId, int nRemovePoint )
{
	if( nRemovePoint != 0 )
	{
		//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << nRemovePoint;
		PASS( ar );
	}
}

void CDPCoreClient::SendGameRate( FLOAT fRate, BYTE nFlag )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << fRate;
	ar << nFlag;
	PASS( ar );	
}

void CDPCoreClient::SendLoadConstant()
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN );
	PASS( ar );		
}

void CDPCoreClient::SendSetMonsterRespawn( u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
	ar << bFlying;
	PASS( ar );	
}

void CDPCoreClient::SendGuildMsgControl_Bank_Item( FLWSUser* pUser, FLItemElem* pItemElem, BYTE p_Mode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::ITEM;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		pItemElem->Serialize( ar );
	}
	
	PASS( ar );
}

void CDPCoreClient::SendGuildMsgControl_Bank_Penya( FLWSUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::PENYA;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		ar << p_Penya;
		ar << cbCloak;
	}
	
	PASS( ar );
}

void CDPCoreClient::SendBlock( BYTE nGu, u_long uidPlayerTo, char *szNameTo, u_long uidPlayerFrom )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BLOCK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGu;
	ar << uidPlayerTo << uidPlayerFrom;
	ar.WriteString( szNameTo );
	PASS( ar );
	
}

// Handlers
void CDPCoreClient::OnLoadWorld( CAr & ar, DPID, DPID, OBJID )
{
	CServerDesc desc;
	desc.Serialize( ar );
	g_PartyMng.Serialize( ar );
//	g_GuildMng.Serialize( ar, FALSE );
//	g_GuildWarMng.Serialize( ar );

	g_pSeasonEffect->deserializeEffect( ar );

	DWORD dwEvent;
	ar >> dwEvent;
	if( dwEvent & EVE_FLAG1219 )
	{
		g_eLocal.SetState( EVE_EVENT1219, 1 );
	}
	if( dwEvent & EVE_FLAG0127 )
	{
		g_eLocal.SetState( EVE_EVENT0127, 1 );
	}
	if( dwEvent & EVE_FLAG0214 )
	{
		g_eLocal.SetState( EVE_EVENT0214, 1 );
	}
	
	char szMsg[512] = "";
	for( list<CJurisdiction*>::iterator i = desc.m_lspJurisdiction.begin(); i != desc.m_lspJurisdiction.end(); ++i )
	{
		CJurisdiction* pJurisdiction = *i;
		
		LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( pJurisdiction->m_dwWorldID );

		if( lpWorld == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ LOAD WORLD FAILED. Check world.inc file. WORLD_ID(%d) ]" ), pJurisdiction->m_dwWorldID);
			continue;
		}

		if( strlen( szMsg ) + strlen( lpWorld->m_szFileName ) < 510 )
		{
			FLStrcat( szMsg, _countof( szMsg ), lpWorld->m_szFileName );
			FLStrcat( szMsg, _countof( szMsg ), " " );
		}

		g_WorldMng.Add( pJurisdiction );
	}
	SetLogInfo( LOGTYPE_MAP, szMsg );
	g_WorldMng.ReadObject();

	WSASetEvent( m_hWait );

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100813 월드 정보를 받았으니 트랜스로부터 리스폰정보를 받자..
	g_dpDBClient.SendBaseGameSettingReq();
	//////////////////////////////////////////////////////////////////////////


	//switch( _GetContentState( CT_DONATION ) )
	//{
	//case CS_VER1:
	//	{
	//		COOPERATIVE_CONTRIBUTIONS().Query_GetContributedInfo();
	//		COOPERATIVE_CONTRIBUTIONS().Query_TryWriteRewardInfo();				// db로 보상 item 리스트를 전송한다..( 보상은 db에서 하므로 )
	//	}
	//	break;
	//default:
	//	__noop;
	//	break;
	//}
}

void CDPCoreClient::OnRecharge( CAr & ar, DPID, DPID, OBJID )
{
	OBJID idBase;
	u_long uBlockSize;
	
	ar >> idBase >> uBlockSize;
//	u_long uMax		= 4096 * 32 + 10240;	// MAX_USER
	u_long uMax		= 2048 * 128 + 20480;	// MAX_USER
	if( uBlockSize > uMax )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "BLOCK SIZE TOO BIG, %d" ), uBlockSize );
		uBlockSize	= uMax;
	}
	prj.m_objmap.m_idStack.PushIdBlock( idBase, uBlockSize );
	m_uRecharge	= 0;
}
/*
void CDPCoreClient::OnModifyMode( CAr & ar, DPID, DPID, OBJID objid )
{
	FLWSUser* pUser;

	pUser	= prj.GetUser( objid );
	if( IsValidObj( (CObj*)pUser ) )
	{
		DWORD dwMode;
		BYTE f;
		u_long idFrom;

		ar >> dwMode >> f >> idFrom;

		if( f )
			pUser->m_dwMode		|= dwMode;
		else
			pUser->m_dwMode		&= ~dwMode;

		g_xWSUserManager->AddModifyMode( pUser );
	}
}
*/

void CDPCoreClient::SendJoin( u_long idPlayer, const char* szPlayer, BOOL bOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPlayer );
	ar << bOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWhisper( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WHISPER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSay( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendModifyMode( DWORD dwMode, BYTE fAdd, u_long idFrom, u_long idTo )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MODIFYMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwMode << fAdd << idFrom << idTo;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendShout( FLWSUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPartyChat( FLWSUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << GETID( pUser );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendForceSetPartySkillTime( u_long uLeaderID, int nMode, DWORD dwSkillTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FORCE_SET_PARTY_SKILL_TIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uLeaderID;
	ar << nMode;
	ar << dwSkillTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint ,int nCachMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nMode;
	ar << dwSkillTime;
	ar << nRemovePoint;
	ar << nCachMode;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGMSay( u_long idPlayer, DWORD dwWorldID, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GMSAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar << dwWorldID;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlayMusic( DWORD dwWorldID, u_long idmusic )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idmusic;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlaySound( DWORD dwWorldID, u_long idsound )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idsound;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// #ifdef __LAYER_1015
// void CDPCoreClient::SendSummonPlayer( u_long idOperator, DWORD dwWorldID, const D3DXVECTOR3 & vPos, u_long idPlayer, int nLayer )
// #else	// __LAYER_1015
// void CDPCoreClient::SendSummonPlayer( u_long idOperator, DWORD dwWorldID, const D3DXVECTOR3 & vPos, u_long idPlayer )
// #endif	// __LAYER_1015
// {
// 	BEFORESENDDUAL( ar, PACKETTYPE_SUMMONPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
// 	ar << idOperator;
// 	ar << g_uIdofMulti;
// 	ar << dwWorldID;
// 	ar << vPos << idPlayer;
// #ifdef __LAYER_1015
// 	ar << nLayer;
// #endif	// __LAYER_1015
// 	SEND( ar, this, DPID_SERVERPLAYER );
// }

// void CDPCoreClient::SendTeleportPlayer( u_long idOperator, u_long idPlayer )
// {
// 	BEFORESENDDUAL( ar, PACKETTYPE_TELEPORTPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
// 	ar << idOperator << idPlayer;
// 	SEND( ar, this, DPID_SERVERPLAYER );
// }

void CDPCoreClient::SendKillPlayer( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_KILLPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerAddr( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERADDR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerCount( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERCOUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetCorePlayer( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETCOREPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSystem( const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SYSTEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCaption( const CHAR* lpString, DWORD dwWorldId, BOOL bSmall, DWORD dwPartyId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAPTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bSmall;
	ar << dwWorldId;
	ar << dwPartyId;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSeasonEffectByGM( const bool effect )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEASON_EFFECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << effect;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildCombatState( int nState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nState;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGCRemoveParty( u_long uidPartyid, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPartyid << uidPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}
// void CDPCoreClient::SendGCAddParty( u_long idLeader, LONG nLeaderLevel, LONG nLeaderJob, DWORD dwLSex, 
// 								   u_long idMember, LONG nMemberLevel, LONG nMemberJob, DWORD dwMSex )
// {
// 	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
// 	ar << idLeader << nLeaderLevel << nLeaderJob << dwLSex;
// 	ar << idMember << nMemberLevel << nMemberJob << dwMSex;
// 	SEND( ar, this, DPID_SERVERPLAYER );
// }

void CDPCoreClient::OnShout( CAr & ar, DPID, DPID, OBJID )
{
	char	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString( lpString, _countof( lpString ) );

	const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
	if( lpszPlayer )
	{
		CAr ar1;
		ar1 << NULL_ID << SNAPSHOTTYPE_SHOUT;
		ar1 << NULL_ID;	//GETID( pUser );
		ar1.WriteString( lpszPlayer );
		ar1.WriteString( lpString );
		ar1 << (DWORD)0xffff99cc;
		GETBLOCK( ar1, lpBlock, uBlockSize );
		g_xWSUserManager->AddBlock( lpBlock, uBlockSize );
	}
}

void CDPCoreClient::OnPlayMusic( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwWorldID;
	u_long idmusic;

	ar >> idmusic;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYMUSIC << idmusic;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_xWSUserManager->AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnPlaySound( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwWorldID;
	u_long idsound;

	ar >> idsound;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYSOUND;
	arBlock << (BYTE)0;
	arBlock << idsound;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_xWSUserManager->AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnErrorParty( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dw;
	u_long uLeader;
	ar >> dw;
	ar >> uLeader;

	FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( uLeader );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->AddSendErrorParty( dw );		
}

void CDPCoreClient::OnAddPartyMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long	idParty, idLeader, idMember;
// 	LONG nLeaderLevel, nMemberLevel, nLeaderJob, nMemberJob;
// 	BYTE nLeaderSex, nMemberSex;
	
	ar >> idParty;
	ar >> idLeader;		// >> nLeaderLevel >> nLeaderJob >> nLeaderSex;
	ar >> idMember;		// >> nMemberLevel >> nMemberJob >> nMemberSex;

	char pszLeader[MAX_PLAYER]	= { 0,};
	char pszMember[MAX_PLAYER]	= { 0,};
	const char* lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
	if( lpPlayer )
		FLStrcpy( pszLeader, _countof( pszLeader ), lpPlayer );
	lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
	if( lpPlayer )
		FLStrcpy( pszMember, _countof( pszMember ), lpPlayer );
		
	CParty* pParty = NULL;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		if( pParty->NewMember( idMember ) )
		{
			for( int i = 0; i < pParty->m_nSizeofMember; i++ )
			{
				FLWSUser* pMember		= (FLWSUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
				if( IsValidObj( (CObj*)pMember ) )
				{
					pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
					if( pMember->m_idparty != idParty )
					{
						pMember->m_idparty	= idParty;
					}
				}
			}
// 			if( IsValidObj( (CObj*)pMember ) )
// 				pMember->m_idparty	= idParty;

			//////////////////////////////////////////////////////////////////////////
			g_PartyMng.CheckAddMemberPartyEffect( idMember, idParty );
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			// error
		}
	}
	else
	{
		if( idParty == g_PartyMng.NewParty( idLeader, idMember, idParty ) )
		{
			pParty	= g_PartyMng.GetParty( idParty );
			if( pParty )
			{
				FLWSUser* pMember;
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
				{
					pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( (CObj*)pMember ) )
					{
						pMember->m_idparty	= idParty;
						pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
					}
				}

				//////////////////////////////////////////////////////////////////////////
				g_PartyMng.CheckAddMemberPartyEffect( idLeader, idParty );
				g_PartyMng.CheckAddMemberPartyEffect( idMember, idParty );
				//////////////////////////////////////////////////////////////////////////
			}
			else
			{
				// error
			}
		}
		else
		{
			// error
		}
	}

	
}

void CDPCoreClient::OnRemovePartyMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idLeader, idMember;
	ar >> idParty >> idLeader >> idMember;

	CParty* pParty;
	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		char pszLeader[MAX_PLAYER]	= { 0,};
		char pszMember[MAX_PLAYER]	= { 0,};
		const char* lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
		if( lpPlayer )
			FLStrcpy( pszLeader, _countof( pszLeader ), lpPlayer );
		lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
		if( lpPlayer )
			FLStrcpy( pszMember, _countof( pszMember ), lpPlayer );

		if( pParty->DeleteMember( idMember ) )
		{
			if( pParty->GetSizeofMember() < MIN_PTMEMBER_SIZE )
			{
				FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( 0 ) );
				if( IsValidObj( pMember ) )
				{
					pMember->AddPartyMember( NULL, 0, pszLeader, pszMember );
					pMember->m_idparty	= 0;
				}
				pMember	= g_xWSUserManager->GetUserByPlayerID( idMember );	// 먼저 삭제됐던 넘의 포인터.
				if( IsValidObj( pMember ) && pMember->m_nDuel )
				{
					CParty* pDuelParty = g_PartyMng.GetParty( pParty->m_idDuelParty );
					if( pDuelParty )
						pDuelParty->DoDuelPartyCancel( pParty );
					else
						FLERROR_LOG( PROGRAM_NAME, _T( "파티멤버 %s의 정보이상. %d %d" ), pMember->GetName(), pMember->m_idDuelParty, pParty->m_idDuelParty );
				}
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
				pParty = NULL;
			}
			else
			{
				// leave
				FLWSUser* pMember;
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
				{
					pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( (CObj*)pMember ) )
						pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
				}
			}

			FLWSUser* pRemovd	= g_xWSUserManager->GetUserByPlayerID( idMember ); 
			if( IsValidObj( (CObj*)pRemovd ) )
			{
				pRemovd->m_idparty	= 0;
				pRemovd->AddPartyMember( NULL, idMember, pszLeader, pszMember );

				//////////////////////////////////////////////////////////////////////////
				g_PartyMng.CheckRemoveMemberPartyEffect( pRemovd->m_idPlayer, idParty );
				//////////////////////////////////////////////////////////////////////////
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Not DeleteMember" ) );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not Party" ) );
	}
}

void CDPCoreClient::OnAddPlayerParty( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i	= pParty->FindMember( idPlayer );
		if( i < 0 )
			return;	//

		pParty->m_aMember[i].m_bRemove	= FALSE;

		FLWSUser* pMember;
		for( int j = 0 ; j < pParty->m_nSizeofMember ; j++ )
		{
			pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( j ) );
			if( IsValidObj( (CObj*)pMember ) )
				pMember->AddSetPartyMemberParam( idPlayer, PP_REMOVE, 0 );
		}
	}
}

void CDPCoreClient::OnRemovePlayerParty( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i	= pParty->FindMember( idPlayer );
		if( i < 0 )
			return;

		pParty->m_aMember[i].m_bRemove	= TRUE;

		FLWSUser* pMember;
		for( int j = 0; j < pParty->m_nSizeofMember; j++ )
		{
			pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( j ) );
			if( IsValidObj( (CObj*)pMember ) )
				pMember->AddSetPartyMemberParam( idPlayer,PP_REMOVE, 1 );
		}

//		if( i == 0 )		// 극단장이 나갈경우
		if( pParty->IsLeader( idPlayer ) == TRUE )
		{
			if( pParty->m_idDuelParty > 0 )
			{
				CParty* pDuelParty	= g_PartyMng.GetParty( pParty->m_idDuelParty );
				if( pDuelParty )
					pParty->DoDuelPartyCancel( pDuelParty );
			}

			bool fRemoveParty	= true;
			for( int j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if( pParty->m_aMember[j].m_bRemove == FALSE )
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					// swap
					break;
				}
			}

			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_dwModeTime[k] )
				{
// 					if( k == PARTY_PARSKILL_MODE)
// 						continue;
					pParty->m_dwModeTime[k] = 0;
				}
			}

// 			CMover* pMover = pParty->GetLeader();
// 			if( pMover )
// 			{
// 				if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
// 					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
// 				else
// 					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
// 			}

			if( fRemoveParty )
			{
				FLWSUser* pMember;
				for( int j = 0; j < pParty->m_nSizeofMember; j++ )
				{
					pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( j ) );
					if( IsValidObj( (CObj*)pMember ) )
						pMember->m_idparty = 0;
				}
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				FLWSUser* pLeader	= static_cast<FLWSUser*>( pParty->GetLeader() );
				if( IsValidObj( pLeader ) == TRUE )
				{
					const bool bActivate	= ( pLeader->HasBuffByIk3( IK3_PARTYPOINTFREE ) == TRUE );

					g_PartyMng.NotiSetPartyEffectModeToCS( pLeader->m_idPlayer, pParty->m_uPartyId, PARTY_EFFECT_POINT_LIMITLESS, bActivate );
				}
				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
}

void CDPCoreClient::OnSetPartyMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId = NULL_PLAYER_ID;
	int nMode = PARTY_LINKATTACK_MODE;
	BOOL bOnOff = FALSE;
	LONG nPoint = 0;
	DWORD	dwSkillTime = 0;
	ar >> uPartyId >> nMode >> dwSkillTime >> bOnOff;
	if( bOnOff == TRUE )
		ar >> nPoint;

	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		if( bOnOff == TRUE )
			pParty->m_nPoint = nPoint;

		// 090917 mirchang - 모드가 파스킬풀 이고 bOnOff가 FALSE 일땐 극단원중 파스킬풀 아이템 사용중인지 체크하여 사용중인 극단원이 있을땐 다시 모드 설정
// 		if( nMode == PARTY_PARSKILL_MODE && !bOnOff )
// 		{
// 			for( int j = 0; j < pParty->m_nSizeofMember; ++j )
// 			{
// 				if( pParty->m_aMember[j].m_bRemove )
// 					continue;
// 
// 				FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[j].m_uPlayerId );
// 				if( IsValidObj( pUser ) && ( pUser->IsSMMode( SM_PARTYSKILL30 ) || pUser->IsSMMode( SM_PARTYSKILL15 ) || pUser->IsSMMode( SM_PARTYSKILL1 ) ) )
// 				{
// 					bOnOff = TRUE;
// 					g_DPCoreClient.SendUserPartySkill( pUser->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
// 					break;
// 				}
// 			}
// 		}

		pParty->m_dwModeTime[nMode] = bOnOff;
//		pParty->m_dwModeTime[nMode] = dwSkillTime;
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddSetPartyMode( nMode, bOnOff, pParty->m_nPoint , dwSkillTime );
		}
	}
}

void CDPCoreClient::OnPartyChangeItemMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nTroupeShareItem = nMode;

		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeItemMode( nMode );
		}
	}
}

void CDPCoreClient::OnPartyChangeExpMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nTroupsShareExp = nMode;
		
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeExpMode( nMode );
		}
	}
}


void CDPCoreClient::OnSetPartyExp( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	LONG nExp, nPoint, nLevel;
	ar >> uPartyId >> nExp >> nPoint >> nLevel;

	CParty* pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nExp		= nExp;
		pParty->m_nPoint	= nPoint;
		pParty->m_nLevel	= nLevel;

		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
			{
				pUser->AddPartyExpLevel( pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint() );
				
				if( pParty->GetKind() == PARTY_KIND_BASE && pParty->m_nLevel > MAX_PARTY_BASE_LEVEL && pParty->IsLeader( pUser->m_idPlayer ) )
				{
					pUser->AddHdr( pUser->GetId(), SNAPSHOTTYPE_CHANGE_PARTY_KIND_NOTI );
				}
			}
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnRemovePartyPoint( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	LONG nPartyPoint;
	ar >> uPartyId;
	ar >> nPartyPoint;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nPoint = nPartyPoint;
	
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyExpLevel( pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint() );
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnPartyChangeName( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidParty;
	TCHAR	sParty[128];

	ar >> uidParty;	
	ar.ReadString( sParty, _countof( sParty ) );
	
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		FLStrcpy( pParty->m_sParty, _countof( pParty->m_sParty ), sParty );
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeName( pParty->m_sParty );
		}
	}
}

void CDPCoreClient::OnPartyChangeTroup( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidParty;
	TCHAR	sParty[33];		
	
	ar >> uidParty;	
	ar.ReadString( sParty, _countof( sParty ) );
		
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		pParty->SetKind( PARTY_KIND_EXTEND );
		FLStrcpy( pParty->m_sParty, _countof( pParty->m_sParty ), sParty );
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeTroup( pParty->m_sParty );
		}
/*
		CMover* pMover = pParty->GetLeader();
		if( pMover )
		{
			if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
                g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
		}
*/
	}
}

void CDPCoreClient::OnAddFriend( CAr & ar, DPID, DPID, OBJID )
{
	int bAdd = 0; // 0이면 추가를 아무도 안한것 1 Sender만 추가, 2 Friend만 추가, 3 : 두명다 추가됨
	u_long uidSend, uidFriend;
	BYTE nSendSex, nFriendSex;
	LONG nSendJob, nFriendJob;
	ar >> uidSend >> uidFriend;
	ar >> nSendSex >> nFriendSex;
	ar >> nSendJob >> nFriendJob;

	FLWSUser* pSender;
	FLWSUser* pFriend;
	
	pSender = g_xWSUserManager->GetUserByPlayerID( uidSend );
	pFriend = g_xWSUserManager->GetUserByPlayerID( uidFriend );
	
	const char* lpszFriendPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( uidFriend );
	if( lpszFriendPlayer == NULL )	//
		return;
	char lpszFriend[MAX_PLAYER] = { 0, };
	FLStrcpy( lpszFriend, _countof( lpszFriend ), lpszFriendPlayer );

	if( IsValidObj( (CObj*)pSender ) )
	{
		if( MAX_FRIEND <= pSender->m_RTMessenger.size() )
		{
			pSender->AddDefinedText( TID_GAME_MSGMAXUSER, "" );
		}
		else
		{
			pSender->m_RTMessenger.SetFriend( uidFriend, NULL );
			pSender->AddAddFriend( uidFriend, lpszFriend ); 
			bAdd++;
		}
	}

	const char* lpszSendPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( uidSend );
	if( lpszSendPlayer == NULL )
		return;
	char lpszSend[MAX_PLAYER] = { 0, };
	FLStrcpy( lpszSend, _countof( lpszSend ), lpszSendPlayer );

	if( IsValidObj( (CObj*)pFriend ) )
	{
		if( MAX_FRIEND <= pFriend->m_RTMessenger.size() )
		{
			if( IsValidObj( (CObj*)pSender ) )
				pSender->AddDefinedText( TID_GAME_MSGMAXUSER, "" );
		}
		else
		{
			pFriend->m_RTMessenger.SetFriend( uidSend, NULL );
			pFriend->AddAddFriend( uidSend, lpszSend );
			bAdd	+= 2;
		}
	}

	if( IsValidObj( (CObj*)pSender ) && strlen( lpszFriend ) && ( bAdd == 2 || bAdd == 3 ) )
		pSender->AddDefinedText( TID_GAME_MSGINVATECOM, "%s", lpszFriend );

	if( IsValidObj( (CObj*)pFriend ) && strlen( lpszSend ) && ( bAdd == 1 || bAdd == 3 )  )
		pFriend->AddDefinedText( TID_GAME_MSGINVATECOM, "%s", lpszSend );
}

void CDPCoreClient::OnRemovefriend( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidSend, uidFriend;
	ar >> uidSend >> uidFriend;
	
	FLWSUser* pSender;

	pSender		= (FLWSUser*)prj.GetUserByID( uidSend );
	if( FALSE == IsValidObj( (CObj*)pSender ) )
		return;

	CHousingMng::GetInstance()->ReqSetAllowVisit( pSender, uidFriend, FALSE );
	pSender->m_RTMessenger.RemoveFriend( uidFriend );
	pSender->AddRemoveFriend( uidFriend );
	FLWSUser* pFriend	= g_xWSUserManager->GetUserByPlayerID( uidFriend );
	if( IsValidObj( pFriend ) )
	{
		CHousingMng::GetInstance()->ReqSetAllowVisit( pFriend, uidSend, FALSE );
		pFriend->m_RTMessenger.RemoveFriend( uidSend );
		pFriend->AddRemoveFriend( uidSend );
	}
}

void CDPCoreClient::OnQueryTickCount( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwTime;
	__int64 nTickCount;
	double dCurrentTime;
	ar >> dwTime >> nTickCount >> dCurrentTime;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );
	g_GameTimer.SetCurrentTime( dCurrentTime );
}

void CDPCoreClient::OnSeasonEffect( CAr & ar, DPID, DPID, OBJID )
{
	g_pSeasonEffect->deserializeEffect( ar );

	if( g_pSeasonEffect->getSeason() != SEASON_NONE ) {
		g_xWSUserManager->AddSeasonEffect();
	}
}

void CDPCoreClient::OnPartyChat( CAr & ar , DPID, DPID, OBJID )
{
	u_long idParty;
static	\
	CHAR	lpName[MAX_PLAYER], lpString[260], lpSnoopString[512];
	
	OBJID objid;
	ar >> objid;
	ar >> idParty;
	ar.ReadString( lpName, _countof( lpName ) );
	ar.ReadString( lpString, _countof( lpString ) );

	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		FLWSUser* pMember	= NULL;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember		= (FLWSUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pMember ) )
			{
				pMember->AddPartyChat( lpName, lpString, objid );
				if( pMember->m_idSnoop )
				{
					FLWSUser* pSnoop		= g_xWSUserManager->GetUserByPlayerID( pMember->m_idSnoop );
					if( IsValidObj( (CObj*)pSnoop ) )
					{
						FLSPrintf( lpSnoopString, _countof( lpSnoopString ), "[p][%s->%s] %s", lpName, pMember->GetName(), lpString );
						pSnoop->AddSnoop( lpSnoopString );
					}
				}
			}
		}
	}
}

void CDPCoreClient::SendAddFriendNameReqest( u_long uLeaderid, LONG nLeaderJob, BYTE nLeaderSex, u_long uMember, const char * szLeaderName, const char * szMemberName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDFRIENDNAMEREQEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uLeaderid << uMember;
	ar << nLeaderJob << nLeaderSex;
	ar.WriteString( szLeaderName );
	ar.WriteString( szMemberName );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, const char* szGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildChat( FLWSUser* pUser, const char* sChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetId();
	ar << pUser->m_idPlayer;
	ar.WriteString( sChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnCreateGuild( CAr & ar, DPID, DPID, OBJID )
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];

	ar >> nSize;

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( nSize < 0 || nSize > MAX_PTMEMBER_SIZE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Error" ) );
		return;
	}
	//	END100708
	//////////////////////////////////////////////////////////////////////////

	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar >> idGuild;
	ar.ReadString( szGuild, _countof( szGuild ) );

	int	cb	= 0;
	FLWSUser* apUser[MAX_PTMEMBER_SIZE];

	CGuild* pGuild	= new CGuild;
	pGuild->m_idGuild	= idGuild;
	FLStrcpy( pGuild->m_szGuild, _countof( pGuild->m_szGuild ), szGuild );
	pGuild->m_idMaster		= info[0].idPlayer;
	pGuild->m_adwPower[ GUD_MASTER ] = 0x000000FF;
	idGuild	= g_GuildMng.AddGuild( pGuild );
	if( idGuild > 0 )
	{
		// master
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info[0].idPlayer;
		pMember->m_nMemberLv	= GUD_MASTER;

		if( pGuild->AddMember( pMember ) )
		{
			FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( info[0].idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				apUser[cb++]	= pUser;
			}
			// member
			for( int i = 1; i < nSize; i++ )
			{
				pMember		= new CGuildMember;
				pMember->m_idPlayer		= info[i].idPlayer;
				pMember->m_nMemberLv	= GUD_ROOKIE;

				if( pGuild->AddMember( pMember ) )
				{
					pUser	= (FLWSUser*)prj.GetUserByID( info[i].idPlayer );
					if( IsValidObj( pUser ) )
					{
						pUser->m_idGuild	= idGuild;
						apUser[cb++]	= pUser;
					}
				}
				else
				{
					info[i].idPlayer	= 0;
					SAFE_DELETE( pMember );
				}
			}
		}
		else
		{
			SAFE_DELETE( pMember );
			g_GuildMng.RemoveGuild( idGuild );
		}
	}
	else
	{
		SAFE_DELETE( pGuild );
	}

	if( cb > 0 )
	{
		g_xWSUserManager->AddCreateGuild( info[0].idPlayer, CPlayerDataCenter::GetInstance()->GetPlayerString( info[0].idPlayer ), idGuild, szGuild );	// g_xWSUserManager->Lock();
	}

	for( int i = 0; i < cb; i++ )
	{
		g_xWSUserManager->AddSetGuild( apUser[i], idGuild );
		apUser[i]->AddMyGuild();
	}
}

void CDPCoreClient::OnDestroyGuild( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	ar >> idGuild;

	
	FLWSUser* pUserIk3[ MAX_GM_SIZE ];

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		FLWSUser* pUsertmp;
		CGuildMember* pMember;
		int nSize = 0;

		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;

			pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj( pUsertmp ) )
			{
				//User :: OnDestroyGuild
				//// 길드 인던 처리
				//if( pUsertmp->GetWorld() == NULL )
				//	continue;

				//if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pUsertmp->GetWorld()->GetID() ) == FALSE )
				//	continue;

				//if( pUsertmp->GetGuild() == NULL )
				//	continue;

				//if( pUsertmp->GetPartyId() == 0 )
				//	continue;

				//CParty* pParty		= g_PartyMng.GetParty( pUsertmp->GetPartyId() );
				//if( pParty == NULL )
				//	continue;

				//if( pParty->GetLeader() == NULL )
				//	continue;

				//INDUN_INFO * pInfo		= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( pParty->m_uPartyId, pUsertmp->GetWorld()->GetID() );
				//if( pInfo == NULL )	
				//	continue;

				//if( pInfo->dwGuildID == 0 )			
				//	continue; 

				//CInstanceDungeonHelper::GetInstance()->GoOut( pUsertmp );			//튕겨냄
				pUsertmp->OnRemoveGuildMember();
				//g_DPCoreClient.SendRemovePartyMember( pParty->GetLeader()->m_idPlayer, pUsertmp->m_idPlayer );
				
				// reset
				pUsertmp->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUsertmp );
				pUsertmp->m_idGuild		= 0;
				g_xWSUserManager->AddSetGuild( pUsertmp, 0 );
				pUserIk3[nSize] = pUsertmp;
				++nSize;		
			}
		}

		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pGuild->m_idMaster );

		/*
		if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
		{
			FLWSUser* pUserMaster = (FLWSUser*)prj.GetUserByID( pGuild->m_idMaster );
			CSecretRoomMng::GetInstance()->SetFailGuild( pUserMaster );
		}
		*/
		
		g_GuildMng.RemoveGuild( pGuild->m_idGuild );
		g_xWSUserManager->AddDestroyGuild( lpszPlayer, idGuild );	// g_xWSUserManager->Lock();

		for( int cou = 0 ; cou < nSize ; ++cou )
		{
			if( IsValidObj( pUserIk3[cou] ) )
			{
				pUserIk3[cou]->RemoveItemIK3( IK3_CLOAK );
				pUserIk3[cou]->m_tGuildMember = CTime::GetCurrentTime();
				pUserIk3[cou]->m_tGuildMember += CTimeSpan( 7, 0, 0, 0 );
				////////////////
				// 康
				if( pUserIk3[cou]->GetWorld() )
				{
					////
					PRegionElem pRgnElem	= NULL;
					DWORD dwWorldID	= pUserIk3[cou]->GetWorld()->GetID();
					CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
					int nId	= pProcessor->PtInQuestRect( pUserIk3[cou]->GetPos() );
					if( nId > -1 )
					{
						PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
						if( !pElem || pElem->idGuild != pUserIk3[cou]->m_idGuild )
						{
							CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
							if( pWorld )
							{
								if( pUserIk3[cou]->IsChaotic() )
								{
									if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
										pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
									if( !pRgnElem )	// Find near revival pos
										pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUserIk3[cou]->GetPos() );
								}
								else
								{
									if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
										pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
									if( !pRgnElem )	// Find near revival pos
										pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUserIk3[cou]->GetPos() );
								}
							}
						}
					}
					////
					if( pRgnElem )
						pUserIk3[cou]->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
				}
				////////////////
			}
		}	
		return;
	}
}

void CDPCoreClient::OnAddGuildMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
//	ar >> idPlayer >> idGuild;
	GUILD_MEMBER_INFO info;
	ar.Read( &info, sizeof(GUILD_MEMBER_INFO) );
	ar >> idGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->GetSize() >= MAX_GM_SIZE )
			return;

		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info.idPlayer;
		pMember->m_nMemberLv	= GUD_ROOKIE;

		if( pGuild->AddMember( pMember ) )
		{
			FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( info.idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				g_xWSUserManager->AddSetGuild( pUser, idGuild );
				pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( idGuild ) );
				GuildHouseMng->SetApplyDST( pUser );

#ifdef GUILD_WINNER_BUFF
				CTime tNextTime = g_GuildCombatMng.GetNextGuildCobmatTime();
				time_t d = (time_t)( tNextTime.GetTime() - time_null() );

				if( pUser->m_idGuild == g_GuildCombatMng.m_uWinGuildId && g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
				{
					if (g_GuildCombatMng.m_nWinGuildCount == 1)
						pUser->AddBuff( BUFF_ITEM, ITEM_INDEX( 25582, II_SYS_SYS_LS_WINNERDIGNITY ), 0, d * 1000, 0, BT_TICK );
					else if (g_GuildCombatMng.m_nWinGuildCount == 2)
						pUser->AddBuff( BUFF_ITEM, ITEM_INDEX( 25583, II_SYS_SYS_LS_SPLENDIDGLORY ), 0, d * 1000, 0, BT_TICK );
				}			
#endif
			}
		}
		else
		{
			SAFE_DELETE( pMember );
		}
	}
}

void CDPCoreClient::OnRemoveGuildMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	ar >> idPlayer >> idGuild;

	BOOL bRemove = FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	FLWSUser* pUser = NULL;
	if( pGuild )
	{
		if( pGuild->RemoveMember( idPlayer ) )
		{
			pUser	= (FLWSUser*)prj.GetUserByID( idPlayer );
			if( IsValidObj(pUser) )
			{
				pUser->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUser );
				GuildHouseMng->GoOutGuildHouse( pUser );
				pUser->m_idGuild	= 0;
				g_xWSUserManager->AddSetGuild( pUser, 0 );
				bRemove = TRUE;
			}
		}
	}

	if( bRemove )
	{
		if( IsValidObj(pUser) )
		{
			pUser->RemoveItemIK3( IK3_CLOAK );
			pUser->m_tGuildMember = CTime::GetCurrentTime();
			pUser->m_tGuildMember += CTimeSpan( 7, 0, 0, 0 );
			// 康
			if( pUser->GetWorld() )
			{
				//// 길드 퀘스트 관련 처리
				PRegionElem pRgnElem	= NULL;
				DWORD dwWorldID	= pUser->GetWorld()->GetID();
				CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
				int nId	= pProcessor->PtInQuestRect( pUser->GetPos() );
				if( nId > -1 )
				{
					PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
					if( !pElem || pElem->idGuild != pUser->m_idGuild )
					{
						CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
						if( pWorld )
						{
							if( pUser->IsChaotic() )
							{
								if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
									pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
								if( !pRgnElem )	// Find near revival pos
									pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
							}
							else
							{
								if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
									pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
								if( !pRgnElem )	// Find near revival pos
									pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
							}
						}
					}
				}
				if( pRgnElem )
					pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );


				pUser->OnRemoveGuildMember();
	//			//// 길드 인던 처리  OnRemoveGuildMember
	//			if( pUser->GetWorld() == NULL )
	//				return;

	//			if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pUser->GetWorld()->GetID() ) == FALSE )
	//				return;

	////			if( pUser->GetGuild() == NULL )
	////				return;

	//			if( pUser->GetPartyId() == 0 )
	//				return;

	//			CParty* pParty		= g_PartyMng.GetParty( pUser->GetPartyId() );
	//			if( pParty == NULL )
	//				return;

	//			if( pParty->GetLeader() == NULL )
	//				return;
	//			
	//			INDUN_INFO * pInfo		= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( pParty->m_uPartyId, pUser->GetWorld()->GetID() );
	//			if( pInfo == NULL )	
	//				return;

	//			if( pInfo->dwGuildID == 0 )			
	//				return; 

	//			CInstanceDungeonHelper::GetInstance()->GoOut( pUser );			//튕겨냄
				//pParty->DeleteMember( pUser->m_idPlayer );
				//g_DPCoreClient.SendRemovePartyMember( pParty->GetLeader()->m_idPlayer, pUser->m_idPlayer );
			}
		}
	}
}

void CDPCoreClient::OnGuildMemberLv( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= static_cast< BYTE >( nMemberLv );
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildClass( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	int nClass;
	ar >> idPlayer >> nClass >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			pMember->m_nClass	= nClass;
	}
}

void CDPCoreClient::OnGuildNickName( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	char strNickName[MAX_GM_ALIAS] = {0,};
	ar >> idPlayer >> idGuild;
	ar.ReadString( strNickName, _countof( strNickName ) );
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			FLStrcpy( pMember->m_szAlias, _countof( pMember->m_szAlias ), strNickName );
	}
}

void CDPCoreClient::OnChgMaster( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idPlayer2, idGuild;
	ar >> idPlayer >> idPlayer2 >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->m_idMaster	= idPlayer2;
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_ROOKIE;
			pMember->m_nClass = 0;
		}
		pMember	= pGuild->GetMember( idPlayer2 );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_MASTER;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildMemberLogOut( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	ar >> idGuild >> idPlayer;
}

void CDPCoreClient::SendWarDead( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_DEAD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarMasterAbsent( u_long idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_MASTER_ABSENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarTimeout( u_long idWar )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_TIMEOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnAcptWar( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar, idDecl, idAcpt;
	ar >> idWar >> idDecl >> idAcpt;

	CGuild* pDecl	= g_GuildMng.GetGuild( idDecl );
	CGuild* pAcpt	= g_GuildMng.GetGuild( idAcpt );

	if( !pDecl || !pAcpt )
		return;

	CGuildWar* pWar		= new CGuildWar;
	pWar->m_idWar	= idWar;
	pWar->m_Decl.idGuild	= pDecl->m_idGuild;
	pWar->m_Decl.nSize	= pDecl->GetSize();
	pWar->m_Acpt.idGuild	= pAcpt->m_idGuild;
	pWar->m_Acpt.nSize	= pAcpt->GetSize();
	pWar->m_nFlag	= WF_WARTIME;
	pWar->m_time	= CTime::GetCurrentTime();

	idWar	= g_GuildWarMng.AddWar( pWar );
	if( idWar > 0 )
	{
		pDecl->m_idWar	= idWar;
		pDecl->m_idEnemyGuild	= pAcpt->m_idGuild;
		pAcpt->m_idWar	= idWar;
		pAcpt->m_idEnemyGuild	= pDecl->m_idGuild;

		FLWSUser* pUser;
		for( std::map<u_long, CGuildMember*>::iterator i = pDecl->m_mapPMember.begin();
			i != pDecl->m_mapPMember.end(); ++i )
		{
			pUser	= (FLWSUser*)prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_xWSUserManager->AddSetWar( pUser, idWar );
			}
		}
		for( std::map<u_long, CGuildMember*>::iterator i = pAcpt->m_mapPMember.begin();
			i != pAcpt->m_mapPMember.end(); ++i )
		{
			pUser	= (FLWSUser*)prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_xWSUserManager->AddSetWar( pUser, idWar );
			}
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "OnAcptWar()" ) );
		SAFE_DELETE( pWar );
	}
}

void CDPCoreClient::OnSurrender( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar, idPlayer;
	BOOL bDecl;
	ar >> idWar >> idPlayer >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		CGuild* pGuild;
		if( bDecl )
		{
			pWar->m_Decl.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
		}
		else
		{
			pWar->m_Acpt.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
		}
		if( pGuild )
		{
			CGuildMember* pMember	= pGuild->GetMember( idPlayer );
			if( pMember )
				pMember->m_nSurrender++;
		}
		FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			pUser->m_idWar	= 0;
			g_xWSUserManager->AddSetWar( pUser, 0 );
		}
	}
}

void CDPCoreClient::OnWarDead( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar;
	BOOL bDecl;
	ar >> idWar >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
			pWar->m_Decl.nDead++;
		else
			pWar->m_Acpt.nDead++;
	}
}

void CDPCoreClient::OnWarEnd( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar;
	int nWptDecl, nWptAcpt;
	int nType;
	ar >> idWar >> nWptDecl >> nWptAcpt >> nType;
	

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;
	g_GuildWarMng.Result( pWar, pDecl, pAcpt, nType, nWptDecl, nWptAcpt );
}

void CDPCoreClient::OnGuildLogoACK( CAr & ar, DPID, DPID, OBJID )
{
/*  // 시야안의 유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d" ), idGuild, dwLogo);

	prj.m_AddRemoveLock.Enter();	// lock1
	g_GuildMng.m_AddRemoveLock.Enter();	// lock2

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetLogo( dwLogo );

		FLWSUser* pUsertmp;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( pUsertmp )
				g_xWSUserManager->AddSetLogo( pUsertmp, dwLogo );
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave();	// unlock2
	prj.m_AddRemoveLock.Leave();	// unlock1
*/
	// 전체유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d" ), idGuild, dwLogo);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
		pGuild->SetLogo( dwLogo );
	g_xWSUserManager->AddSetLogo( idGuild, dwLogo );	// g_UserMng와 교착 상태를 피하기 위해서 unlock된후에 한다.
}

void CDPCoreClient::OnGuildContributionACK( CAr & ar, DPID, DPID, OBJID )
{
	CONTRIBUTION_CHANGED_INFO info;
	ULONG uServerID;

	ar >> uServerID;
	ar >> info;

	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_GUILDCONTRIBUTION GUILD:%d idPlayer:%d dwPxpCount:%d dwPenya:%d" )
          "dwGuildPxpCount:%d dwGuildPenya:%d nGuildLevel:%d\n", 
		info.idGuild, info.idPlayer, info.dwPxpCount, info.dwPenya, 
		info.dwGuildPxpCount, info.dwGuildPenya, info.nGuildLevel);

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		if( uServerID != ::g_uKey )		// 이 메세지의 원본서버가 아닌 경우만 update
			pGuild->SetContribution( info );	

		FLWSUser* pUser;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
		
			if( IsValidObj( pUser ) )
				pUser->AddContribution( info );
		}
	}
}

void CDPCoreClient::OnModifyVote( CAr & ar, DPID, DPID, OBJID )
{
	u_long idVote, idGuild;
	BYTE cbOperation;
	BYTE cbExtra;

	ar >> idGuild;
	ar >> idVote;
	ar >> cbOperation;
	ar >> cbExtra;

	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_MODIFYVOTE guild:%d vote:%d case:%d extra:%d" ), idGuild, idVote, cbOperation, cbExtra );

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->ModifyVote( idVote, cbOperation, cbExtra ) )
		{
			FLWSUser* pUser;
			CGuildMember* pMember;
			for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddModifyVote( idVote, cbOperation, cbExtra );
			}
		}
	}
}

void CDPCoreClient::OnAddVoteResultACk( CAr & ar, DPID, DPID, OBJID )
{
	VOTE_INSERTED_INFO	info;
	ar >> info;

	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_ADDVOTERESULT - GUILD:%d VOTE:%d" ),  info.idGuild, info.idVote);
	if( info.idVote == 0 )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		FLWSUser* pUser;
		if( info.idVote )
		{
			pGuild->AddVote( info );

			CGuildMember* pMember;
			for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddInsertedVote( info );
			}
		}
		else
		{
			// 마스터에게 실패를 알린다.
			pUser	= (FLWSUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pUser ) ) 
				pUser->AddInsertedVote( info );
		}
	}
}

void CDPCoreClient::OnGuildNoticeACk( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];

	ar >> idGuild;
	ar.ReadString( szNotice, _countof( szNotice ) );
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_CW_GUILDNOTICE - GUILD:%d Notice:%s" ), idGuild, szNotice);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetNotice( szNotice );

		FLWSUser* pUser;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetNotice( idGuild, szNotice );
		}
	}
}

void CDPCoreClient::OnGuildAuthority( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	DWORD dwAuthority[MAX_GM_LEVEL];
	
	ar >> uGuildId;

	ar.Read( dwAuthority, sizeof(dwAuthority) );

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		memcpy_s( pGuild->m_adwPower, sizeof( pGuild->m_adwPower ), dwAuthority, sizeof(dwAuthority) );

		FLWSUser* pUser;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetGuildAuthority( dwAuthority );
		}
	}
}

void CDPCoreClient::OnGuildPenya( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	DWORD dwType, dwPenya;

	ar >> uGuildId;
	ar >> dwType >> dwPenya;

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		pGuild->m_adwPenya[dwType] = dwPenya;
		FLWSUser* pUser;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetGuildPenya( dwType, dwPenya );
		}
	}
}

void CDPCoreClient::OnGuildRealPenya( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	int nGoldGuild;
	ar >> uGuildId >> nGoldGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		if( !(g_eLocal.GetState( ENABLE_GUILD_INVENTORY )) )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
		}

		FLWSUser* pUser;
		CGuildMember* pMember;
		for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
			{
				pUser->AddGuildRealPenya( nGoldGuild, pMember->m_nMemberLv );
			}				
		}
	}
}

// raiders_test 유저가 아이템을 사용하고 나가면?
void CDPCoreClient::OnGuildSetName( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];
	
	ar >> idGuild;
	ar.ReadString( lpszGuild, _countof( lpszGuild ) );
	u_long idPlayer;
	BYTE nId, nError;
	ar >> idPlayer >> nId >> nError;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( nError == 0 )	// no error
			g_GuildMng.SetName( pGuild, lpszGuild );
		FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			FLItemElem* pItemElem = NULL;
			if( pUser->m_bAllAction )
			{
				pItemElem	= (FLItemElem*)pUser->GetItemId( nId );
				if( IsUsableItem( pItemElem ) )
					pItemElem->m_bQuery	= FALSE;
			}
			switch( nError )
			{
				case 1:
					// error message	// have not guild
					pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );
					break;
				case 2:
					// error message	// is not kingpin
					pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
					break;
				case 3:
					// error message	// already exists
					pUser->AddDefinedText( TID_GAME_COMOVERLAPNAME );
					break;
				case 0:
					if( pUser->m_bAllAction )
					{
						if( IsUsableItem( pItemElem ) )
							pUser->UpdateItem( nId, UI_NUM, pItemElem->m_nItemNum - 1 );
					}
					break;
			}
			pUser->SetAllAction();
		}
	}
}

void CDPCoreClient::OnGuildMsgControl( CAr & ar, DPID, DPID, OBJID )
{
	if ( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
	{
		GUILD_MSG_HEADER	Header;
		DWORD				dwPenya;
		BYTE				cbCloak;

		ar.Read( &Header, sizeof(GUILD_MSG_HEADER));
		ar >> dwPenya;
		ar >> cbCloak;		// 망토의 경우 

		CGuild* pGuild	= g_GuildMng.GetGuild( Header.HeadASub );
		if( pGuild )
		{
			CGuildMember*	pMember;
			FLWSUser*			pUsertmp;
			for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();	i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) ) 
				{
					pUsertmp->AddGetGoldGuildBank( dwPenya, 2, pMember->m_idPlayer, cbCloak );	// 2는 업데이트 해야할 클라이게
				}
			}
		}
	}
}

BOOL CDPCoreClient::Contribute( FLWSUser* pUser, DWORD dwPxpCount, DWORD dwPenya )
{
	u_long idGuild, idPlayer;
	idGuild = pUser->m_idGuild;
	idPlayer = pUser->m_idPlayer;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild == NULL )
		return FALSE;

	CGuildMember* pGuildMember = pGuild->GetMember( idPlayer );
	if( pGuildMember == NULL )
		return FALSE;
		

	int nLastGuildLv = pGuild->m_nLevel;	// 길드 레벨업을 판단하기 위하여 저장
	if( pGuild->AddContribution( dwPxpCount, dwPenya, idPlayer ) == FALSE )
		return FALSE;




	CONTRIBUTION_CHANGED_INFO info;

	info.idGuild		= idGuild;
	info.idPlayer		= idPlayer;
	info.dwPxpCount		= dwPxpCount;
	info.dwPenya		= dwPenya;
	info.dwGuildPxpCount= pGuild->m_dwContributionPxp;
	info.dwGuildPenya	= pGuild->m_nGoldGuild;
	info.nGuildLevel	= static_cast< WORD >( pGuild->m_nLevel );

	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( pPlayerData )
		g_dpDBClient.SendGuildContribution( info, ( nLastGuildLv < pGuild->m_nLevel? 1: 0 ), pPlayerData->data.nLevel );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << ::g_uKey;
		ar << info;
		SEND( ar, this, DPID_SERVERPLAYER );
	}
	return TRUE;
}

// 길드 스탯변경 요청 
BOOL CDPCoreClient::SendGuildStat( FLWSUser* pUser, GUILD_STAT stat, DWORD data )
{
	BOOL bResult = TRUE;

	switch (stat)
	{
	case GUILD_STAT_LOGO:		// 로고 변경 
		{
			BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDLOGO, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << pUser->m_idGuild << pUser->m_idPlayer << data;	
			SEND( ar, this, DPID_SERVERPLAYER );
		}
		break;

	case GUILD_STAT_PXPCOUNT:
		bResult =  Contribute( pUser, data, 0 );
		break;

	case GUILD_STAT_PENYA:
		bResult = Contribute( pUser, 0, data );
		break;

	case GUILD_STAT_NOTICE:		// 공지사항 변경 
		{
			BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << pUser->m_idGuild << pUser->m_idPlayer;
			
			// 128바이트보다 큰 경우를 예방하기 위해서 버퍼에 복사한 후에 send
			char szNotice[MAX_BYTE_NOTICE];
			FLStrncpy(szNotice, _countof( szNotice ), (char *)data, MAX_BYTE_NOTICE);

			ar.WriteString(szNotice);
			SEND( ar, this, DPID_SERVERPLAYER );
		}
		break;
	default:
		FLASSERT( 0 );
	}

	return bResult;
}

void CDPCoreClient::SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId << nGoldGuild;
	SEND( ar, this, DPID_SERVERPLAYER );		
}






void CDPCoreClient::OnSetFriendState( CAr & ar, DPID, DPID, OBJID )
{
	FLWSUser* pUser;
	u_long uidPlayer;
	DWORD dwState;
	ar >> uidPlayer >> dwState;

	pUser	= (FLWSUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( pUser ) )
		pUser->m_RTMessenger.SetState( dwState );
}

void CDPCoreClient::OnFriendInterceptState( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidPlayer;
	u_long uidFriend;
	ar >> uidPlayer >> uidFriend;
	
	FLWSUser* pUser, *pUserFriend;

	pUser = (FLWSUser*)prj.GetUserByID( uidPlayer );
	pUserFriend = (FLWSUser*)prj.GetUserByID( uidFriend );

	if( IsValidObj( pUser ) == FALSE )
		return;

	Friend* pFriend	= pUser->m_RTMessenger.GetFriend( uidFriend );
	if( pFriend )
	{
		if( pFriend->bBlock )
		{
			pFriend->bBlock		= FALSE;
			if( IsValidObj( pUserFriend ) )
				pFriend->dwState	= pUserFriend->m_RTMessenger.GetState();
			else
				pFriend->dwState	= FRS_OFFLINE;
		}
		else
		{
			pFriend->bBlock		= TRUE;
			pFriend->dwState	= 0;
		}
	}
}

void CDPCoreClient::OnPartyChangeLeader( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId, idChangeLeader;
	ar >> uPartyId >> idChangeLeader;
	
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->ChangeLeader( idChangeLeader );

		FLWSUser* pMember;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember		= (FLWSUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( pMember ) )
				pMember->AddPartyChangeLeader( idChangeLeader );
		}

		//////////////////////////////////////////////////////////////////////////
		FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( idChangeLeader );
		if( IsValidObj( pLeader ) == TRUE )
		{
			const bool bActivate	= ( pLeader->HasBuffByIk3( IK3_PARTYPOINTFREE ) == TRUE );

			g_PartyMng.NotiSetPartyEffectModeToCS( idChangeLeader, uPartyId, PARTY_EFFECT_POINT_LIMITLESS, bActivate );
		}
		//////////////////////////////////////////////////////////////////////////
	}
}

void CDPCoreClient::OnGameRate( CAr & ar, DPID, DPID, OBJID )
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;
	switch( nFlag )
	{
	case GAME_RATE_SHOPCOST:
		{
			prj.m_fShopCost = fRate;
		}
		break;
	case GAME_RATE_ITEMDROP:
		{
			prj.m_fItemDropRate = fRate;
		}
		break;
	case GAME_RATE_GOLDDROP:
		{
			prj.m_fGoldDropRate = fRate;
		}
		break;
	case GAME_RATE_MONSTEREXP:
		{
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	case GAME_RATE_MONSTERHIT:
		{
			prj.m_fMonsterHitRate = fRate;
		}
		break;
	case GAME_RATE_X3:
		{
			prj.m_fItemDropRate = fRate;
			prj.m_fGoldDropRate = fRate;
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	}
}

void CDPCoreClient::OnLoadConstant( CAr & /*ar*/, DPID, DPID, OBJID )
{
	CProject::LoadConstant( "Constant.inc" );
	g_xWSUserManager->AddGameSetting();
}

void CDPCoreClient::OnSetMonsterRespawn( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidPlayer = NULL_PLAYER_ID;
	DWORD dwMonsterID;
	DWORD dwRespawnNum;
	DWORD dwAttackNum;
	DWORD dwRect;
	DWORD dwRespawnTime;
	BOOL  bFlying;
	ar >> uidPlayer;
	ar >> dwMonsterID >> dwRespawnNum >> dwAttackNum >> dwRect >> dwRespawnTime;
	ar >> bFlying;

	D3DXVECTOR3 vPos;
	CWorld* pWorld = NULL;
	
	FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		vPos = pUser->GetPos();
		pWorld = pUser->GetWorld();
	}

	if( pWorld != NULL )
	{
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = dwMonsterID;
		ri.m_cb = dwRespawnNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( bFlying )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwRespawnTime );
		ri.m_cbTime = 0;
		
#ifdef __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM
		
		char chMessage[512] = {0,};
		FLSPrintf( chMessage, _countof( chMessage ), "Add Respwan MonsterID = %d(%d/%d) Rect(%d, %d, %d, %d) Time = %d", 
			ri.m_dwIndex, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
		
	}
}


// 코어서버로부터 리스트를 받을 때 
void CDPCoreClient::OnCWWantedList( CAr & ar, DPID, DPID, OBJID )
{
	CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
	wantedListSnapshot.Read( ar );
}

// 코어서버로 부터 현상금을 받을 때 
void CDPCoreClient::OnCWWantedReward( CAr & ar, DPID, DPID, OBJID )
{
	u_long		idPlayer, idAttacker;
	__int64		nGold;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;

	ar >> idPlayer >> idAttacker >> nGold >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	if( !pWorld )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "cannot found world." ), dwWorldID );
		return;
	}

	int nDrop = 0;
	while( nGold > 0 )
	{
		if( nGold > MAX_GOLD_REWARD )
			nDrop = MAX_GOLD_REWARD;
		else
			nDrop = (int)( nGold );
		
		nGold -= nDrop;

		FLItemElem* pItemElem	= new FLItemElem;
		pItemElem->m_dwItemId	= ITEM_INDEX( 10382, II_GOLD_REWARD );
		pItemElem->m_nItemNum	= 0;					// 보통 돈 계열에서 사용되는 변수지만 II_GOLD_REWARD는 사용하지 않는다.
		pItemElem->m_nHitPoint  = nDrop;				// II_GOLD_REWARD는 여기서 사용한다. 

		CItem* pItem			= new CItem;
		pItem->m_pItemBase		= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		if( pWorld->ADDOBJ( pItem, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pItem );
		}
	}
}

// 코어서버에 현상금을 누적 요청
void CDPCoreClient::SendWCWantedGold( LPCTSTR szPlayer, u_long idPlayer, int nGold, LPCTSTR szMsg )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_GOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szPlayer );
	ar << idPlayer << nGold;
	ar.WriteString( szMsg );

	SEND( ar, this, DPID_SERVERPLAYER );
}

// 코어서버에 (현상금얻기 & 현상금 clear) 요청  
#ifdef __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos, int nLayer )
#else	// __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos )
#endif	// __LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_REWARD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idAttacker << dwWorldID << vPos;
	ar.WriteString( szFormat );
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetPartyDuel( u_long idParty1, u_long idParty2, BOOL bDuel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYDUEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty1 << idParty2 << bDuel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// raiders_test 사용하고 나가면?
void CDPCoreClient::OnSetPlayerName( CAr& ar, DPID, DPID, OBJID )
{
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	DWORD dwData;
	BOOL f;

	ar >> idPlayer;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );
	ar >> dwData >> f;
	WORD wId	= LOWORD( dwData );
	WORD wMode	= HIWORD( dwData );

	FLTRACE_LOG( PROGRAM_NAME, _T( "WORLDSERVER.EXE\t// %d\t// %s\t// %08x\t// %d" ), idPlayer, lpszPlayer, dwData, f );

	FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		if( f )
		{
			if( wMode )
			{
				pUser->SetNotMode( QUERYSETPLAYERNAME_MODE );
				g_xWSUserManager->AddModifyMode( pUser );
			}
			pUser->SetName( lpszPlayer );

			if( pUser->m_bAllAction )
			{
				FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( wId );
				if( pItemElem )
				{
					FLASSERT( pItemElem->m_bQuery );
					pItemElem->m_bQuery	= FALSE;
					pUser->UpdateItem( (BYTE)( wId ), UI_NUM, pItemElem->m_nItemNum - 1 );
				}
			}
		}
		else
		{
			if( pUser->m_bAllAction )
			{
				if( (int)wId >= 0 )
				{
					FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( wId );
					if( pItemElem )
					{
						FLASSERT( pItemElem->m_bQuery );
						pItemElem->m_bQuery	= FALSE;
					}
				}
			}
			pUser->AddDefinedText( TID_DIAG_0034 );
		}
		pUser->SetAllAction();
	}

	if( f )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "WORLDSERVER.EXE\tPACKETTYPE_QUERYSETPLAYERNAME\t//%d:%s\t-%08x" ), idPlayer, lpszPlayer, dwData );
//		char* lpszOldPlayer	= prj.GetPlayerString( idPlayer );
//		if( lpszOldPlayer )
//			CWantedListSnapshot::GetInstance().Rename( lpszOldPlayer, lpszPlayer );
		CPlayerDataCenter*	pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( idPlayer );
		if( pPlayerData )
		{
			PlayerData pd;
			memcpy_s( &pd, sizeof( pd ), pPlayerData, sizeof(PlayerData) );
			pPlayerDataCenter->DeletePlayerData( idPlayer );
			FLStrcpy( pd.szPlayer, _countof( pd.szPlayer ), lpszPlayer );
			pPlayerDataCenter->AddPlayerData( idPlayer, pd );
		}
	}
}

void CDPCoreClient::SendQuerySetGuildName( u_long idPlayer, u_long idGuild, const char* lpszPlayer, BYTE nId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETGUILDNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	ar.WriteString( lpszPlayer );
	ar << nId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idSnoop << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendChat( u_long idPlayer1, u_long idPlayer2, const char* lpszChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer1 << idPlayer2;
	ar.WriteString( lpszChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnSetSnoop( CAr & ar, DPID, DPID, OBJID )
{
//	FLTRACE_LOG( PROGRAM_NAME, _T( "WORLDSERVER.EXE\t// PACKETTYPE_SETSNOOP" ) );
	u_long idPlayer, idSnoop;
	BOOL bRelease;
	ar >> idPlayer >> idSnoop >> bRelease;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->m_idSnoop	= idSnoop;
}

void CDPCoreClient::SendSetSnoopGuild( u_long idGuild, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOPGUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnEvent( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwEvent;
	ar >> dwEvent;
	// xmas
	if( dwEvent & EVE_FLAG1219 )
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT1219, 1 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT1219, 1 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-Xmas:1" ) );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT1219 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT1219, 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-Xmas:0" ) );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0127 )
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0127, 1 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT0127, 1 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-NewYear:1" ) );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0127 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT0127, 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-NewYear:0" ) );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0214 )
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0214, 1 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT0214, 1 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-ValentinesDay:1" ) );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0214 ) )
			{
				g_xWSUserManager->AddSetLocalEvent( EVE_EVENT0214, 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent-ValentinesDay:0" ) );
			}
		}
	}
}

void CDPCoreClient::OnGuildCombatState( CAr & ar, DPID, DPID, OBJID )
{
	int nState;
	ar >> nState;
	
	g_GuildCombatMng.m_nState = nState;
	if( g_GuildCombatMng.m_nState == CGuildCombat::OPEN_STATE )
		g_GuildCombatMng.m_dwTime = 0;

	g_xWSUserManager->AddGuildCombatState();
}

void CDPCoreClient::OnRemoveUserFromCORE( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwSerial;
	ar >> dwSerial;		// CACHE에서 생성된 serial한 값 

	g_xWSUserManager->RemoveUser( dwSerial ); 
}

void CDPCoreClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CORE : is not alive" ) );

		/*
		FLCommonDumper kDumper( _T( "Assert_" ) );
		FLDumpRunner::DumpNow( &kDumper, FLDUMP_LEVEL_HEAVY, 0, _T( "CORE : is not alive" ), _T( __FUNCTION__ ), __T( __FILE__ ), __LINE__ );
		::PostQuitMessage( 0 );
		*/
		CLOSE_HANDLE( m_hWait );

		if( ConnectToServer( g_szCoreAddr, PN_CORESRVR + 0, FLNormalProtocol::GetInstance(), TRUE ) )
		{
			MyRegister( g_uKey );
			WaitForSingleObject( m_hWait, INFINITE );
		}


	}

	m_bAlive	= FALSE;
	BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << time_null();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnPing( CAr & ar, DPID, DPID, OBJID )
{
//	g_PartyMng.PartyMapInfo();

	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

#if !defined(_DEBUG)
	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );

	if( d > 10 )
		FLERROR_LOG( PROGRAM_NAME, _T( "ping CORE - %d, trans:%s" ), d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif
	
	m_bAlive	= TRUE;
}

void CDPCoreClient::OnDestroyPlayer( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer;
	ar >> idPlayer;
	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( pUser )
		g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
}

void CDPCoreClient::OnInstanceDungeonAllInfo( CAr & ar, DPID, DPID, OBJID )
{
	CInstanceDungeonHelper::GetInstance()->OnInstanceDungeonAllInfo( ar );
}

void CDPCoreClient::OnInstanceDungeonCreate( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwDungeonId;
	DWORD dwPlayerID;
	ar >> nType >> dwDungeonId >> dwPlayerID;

	INDUN_INFO ID_Info( NULL_ID, NULL_ID, dwDungeonId );
	ID_Info.Serialize( ar );

	if( g_uKey == ID_Info.uMultiKey )
		CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, dwDungeonId, dwPlayerID, ID_Info.dwWorldId, ID_Info.uMultiKey, ID_Info.dwGuildID, ID_Info.eDungeonLevel );

	//DWORD dwWorldId;
	//ULONG uMultiKey;
	//DWORD dwGuildID;
	//int eDungeonLevel;
	//ar >> dwWorldId >> uMultiKey >> dwGuildID >> eDungeonLevel;
	//CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, dwDungeonId, dwPlayerID, dwWorldId, uMultiKey, dwGuildID, eDungeonLevel );
}

void CDPCoreClient::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info, const DWORD dwPlayerID )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId << dwPlayerID;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwDungeonId;
	ar >> nType >> dwDungeonId;

	INDUN_INFO ID_Info( NULL_ID, NULL_ID, dwDungeonId );
	ID_Info.Serialize( ar );
	
	if( g_uKey == ID_Info.uMultiKey )
		CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;
	
	ar >> uKey >> nType >> dwPlayerId;
	CT_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo( uKey, nType, CT_Info, dwPlayerId );
}

void CDPCoreClient::OnInstanceDungeonResetCoolTimeInfo( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwWorldId;
	DWORD dwDungeonId;

	ar >> nType >> dwWorldId >> dwDungeonId;

	CInstanceDungeonHelper::GetInstance()->OnResetDungeonCoolTimeInfo( nType, dwWorldId, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonSetCoolTimeInfo( int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uKey << nType << dwPlayerId;
	CT_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPCoreClient::SendInstanceDungeonResetCoolTimeInfo( const int nType, const DWORD dwWorldId, const DWORD dwDungeonId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_RESET_COOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwWorldId << dwDungeonId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwPlayerId;
	
	ar >> nType >> dwPlayerId;
	
	CInstanceDungeonHelper::GetInstance()->OnDeleteDungeonCoolTimeInfo( nType, dwPlayerId );
}

void CDPCoreClient::SendQuizSystemMessage( int nDefinedTextId, BOOL bAll, int nChannel, int nTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_NOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nDefinedTextId << bAll << nChannel << nTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendRemovePartyMember( u_long LeaderId, u_long MemberId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYMEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << LeaderId << MemberId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPCoreClient::SendPacket( const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;
	ar << DPID_UNKNOWN << DPID_UNKNOWN;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void	CDPCoreClient::OnBuyingInfo( CAr & ar, DPID, DPID, OBJID )
{
	BUYING_INFO2 bi2;
	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	SERIALNUMBER iSerialNumber	= 0;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( bi2.dwPlayerId );
	if( IsValidObj( pUser ) == FALSE )
	{
		g_dpDBClient.SendBuyingInfo( &bi2, iSerialNumber );
		return;
	}

	CWorld* pWorld = pUser->GetWorld();

	if( pWorld != NULL )
	{
		bi2.dwRetVal			= 0;
		FLItemElem itemElem;
		itemElem.m_dwItemId		= bi2.dwItemId;
		itemElem.m_nItemNum		= (int)bi2.dwItemNum;
		itemElem.m_bCharged		= TRUE;
		bi2.dwRetVal			= pUser->CreateItem( &itemElem );
		iSerialNumber			= itemElem.GetSerialNumber();

		g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
	}

	g_dpDBClient.SendBuyingInfo( &bi2, iSerialNumber );
}

void	CDPCoreClient::OnSetPartyEffectModeNoti( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPlayerID	= NULL_PLAYER_ID;
	u_long uPartyID		= 0;
	DWORD dwEffectMode	= PARTY_EFFECT_NONE;
	bool bActivate		= false;

	ar >> uPlayerID >> uPartyID >> dwEffectMode >> bActivate;

	if( g_PartyMng.SetPartyEffectMode( uPlayerID, uPartyID, dwEffectMode, bActivate ) == true )
	{
		g_PartyMng.NotiSetPartyEffectModeToClient( uPartyID, dwEffectMode, bActivate );
	}
}

void	CDPCoreClient::OnRefreshPartyEffectModeNoti( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyID		= 0;
	DWORD dwEffectMode	= PARTY_EFFECT_NONE;

	ar >> uPartyID >> dwEffectMode;

	g_PartyMng.RefreshPartyEffectMode( uPartyID, dwEffectMode );
}

void	CDPCoreClient::OnStartFlyffPieceBurningMode( CAr & ar, DPID, DPID, OBJID )
{
	using namespace nsFlyffPieceDailyPayment;

	u_long uPlayerID	= NULL_PLAYER_ID;
	T_BURNING_MODE kMode;

	ar >> uPlayerID;
	ar >> kMode;

	const bool bRet = g_pFlyffPieceDailyPayment->SetBurningMode( kMode );

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( uPlayerID );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( bRet == true )
		{
// 			FLPacketBroadCastMessageNoti toCoreSrvr;
// 			toCoreSrvr.byBroadCastType	= BROADCAST_TYPE_PUTSTRING;
// 			FLStrcpy( toCoreSrvr.szMessage, _countof( toCoreSrvr.szMessage ), prj.GetText( TID_MMI_FLYFFPIECE_TEXT06 ) );
// 
// 			SendPacket( &toCoreSrvr );

			g_DPCoreClient.SendSystem( prj.GetText( TID_MMI_FLYFFPIECE_TEXT06 ) );
		}
		else
		{
			pUser->AddText( "[ Start Burning Mode Failed. Please Check Mode Index or Already Exist Set Mode ]" );
		}
	}
}

void	CDPCoreClient::OnEndFlyffPieceBurningMode( CAr & ar, DPID, DPID, OBJID )
{
	using namespace nsFlyffPieceDailyPayment;

	u_long uPlayerID	= NULL_PLAYER_ID;

	ar >> uPlayerID;

	const bool bRet = g_pFlyffPieceDailyPayment->InitializeBurningMode();

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( uPlayerID );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( bRet == true )
		{
// 			FLPacketBroadCastMessageNoti toCoreSrvr;
// 			toCoreSrvr.byBroadCastType	= BROADCAST_TYPE_PUTSTRING;
// 			FLStrcpy( toCoreSrvr.szMessage, _countof( toCoreSrvr.szMessage ), prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
// 
// 			SendPacket( &toCoreSrvr );

			g_DPCoreClient.SendSystem( prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
		}
		else
		{
			pUser->AddText( "[ End Burning Mode Failed. Not Exist Set Mode ]" );
		}
	}
}
