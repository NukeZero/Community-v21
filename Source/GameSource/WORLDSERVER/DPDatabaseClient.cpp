#include "StdAfx.h"

#include "../_CommonDefine/FLPacketStruct.h"

#include "User.h"
#include <WorldMng.h>
#include <misc.h>

#include <playerdata.h>

#include <eveschool.h>

#include <party.h>
#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;

#include <post.h>
#include <spevent.h>

#include "slord.h"
#include <Tax.h>
#include "couplehelper.h"

#include <FunnyCoin.h>

#include <PCBang.h>

#include <InstanceDungeonBase.h>
#include <Quiz.h>
#include <GuildHouse.h>
#include "CampusHelper.h"

#include <serialnumber.h>
#include "NetSerialNumberRequest.h"


#include "../_Common/FLPeriodRepetitionAlarm.h"
#include "FLCooperativeContributions.h"

#include "FLTeleportMapUserPointManager.h"

#include "FLEventArenaGlobal.h"

#include <dump/FLDumpRunner.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include "../_CommonDefine/Packet/FLPacket_ConsignmentSale.h"
#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"
#include "../_CommonDefine/Packet/FLPacket_Certify.h"

#include "../_Common/FLServerDescription.h"
#include "FLCharacterServerTransform.h"

#include "../_Common/FLShutdownRule.h"

#include "FLShutdownRule_Mng.h"

#include "FLMadrigalGift.h"

#include "../_CommonDefine/Packet/FLPacketGuildBankUpdate.h"


extern	BOOL CanAdd( DWORD dwGold, int nPlus );

extern char				g_szDBAddr[16];

extern	CWorldMng		g_WorldMng;
extern	CPartyMng		g_PartyMng;
extern	CGuildCombat	g_GuildCombatMng;

CDPDatabaseClient		g_dpDBClient;


CDPDatabaseClient::CDPDatabaseClient()
{
	m_bAlive	= TRUE;

	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_ALL_PLAYER_DATA, OnAllPlayerData );
	ON_MSG( PACKETTYPE_ADD_PLAYER_DATA, OnAddPlayerData );
	ON_MSG( PACKETTYPE_DELETE_PLAYER_DATA, OnDeletePlayerData );
	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, OnUpdatePlayerData );
	ON_MSG( PACKETTYPE_GUILD_BANK, OnGuildBank );
	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING_END, OnUpdateGuildRankingFinish );
	ON_MSG( PACKETTYPE_QUERYGUILDQUEST, OnQueryGuildQuest );
	ON_MSG( PACKETTYPE_BASEGAMESETTING, OnBaseGameSetting );
	ON_MSG( PACKETTYPE_MONSTERRESPAWNSETTING, OnMonsterRespawnSetting );
	ON_MSG( PACKETTYPE_MONSTERPROPGAMESETTING, OnMonsterPropSetting );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_SPEC_NOTI, OnCooperativeContributions_Spec );
	ON_MSG( PACKETTYPE_GAMEMASTER_CHATTING, OnGMChat );
	ON_MSG( PACKETTYPE_PING, OnPing );

	ON_MSG( PACKETTYPE_ALL_GUILD_DATA_NOTI, OnAllGuildData );
	ON_MSG( PACKETTYPE_ADD_GUILDCOMBAT, OnAllGuildCombat );
	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, OnInGuildCombat );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, OnOutGuildCombat );
	ON_MSG( PACKETTYPE_RESULT_GUILDCOMBAT, OnResultGuildCombat );
	ON_MSG( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, OnPlayerPointGuildCombat );
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, OnGetPenyaGuildGC );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, OnGetPenyaPlayerGC );
	ON_MSG( PACKETTYPE_CONTINUE_GUILDCOMBAT, OnContinueGC );
	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, OnPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL,	OnRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM,	OnGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD,	OnGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL,	OnReadMail );
	ON_MSG( PACKETTYPE_ALLMAIL, OnAllMail );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, OnMailBox );


	//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_QUERYMAILBOX_REQ, OnMailBoxReq );
	//////////////////////////////////////////////////////////////////////////


	ON_MSG( PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, OnQueryRemoveGuildBankTbl );
	ON_MSG( PACKETTYPE_EVENT_GENERIC, OnEventGeneric );
	ON_MSG( PACKETTYPE_EVENT_FLAG, OnEventFlag );

	ON_MSG( PACKETTYPE_EVENTLUA_STATE, OnEventStateLua );
	ON_MSG( PACKETTYPE_EVENTLUA_CHANGED, OnEventLuaChanged );

	ON_MSG( PACKETTYPE_GC1TO1_OPEN, OnGC1to1Open );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERTOSRVR, OnGC1to1TenderGuildFromDB );

	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, OnGuildBankLogViewFromDB );

	ON_MSG( PACKETTYPE_SEALCHAR_REQ, OnSealCharFromDB );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, OnSealCharGetFromDB );

	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, OnElectionAddDeposit );
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, OnElectionSetPledge );
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, OnElectionIncVote );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, OnElectionBeginCandidacy );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_VOTE, OnElectionBeginVote );
	ON_MSG( PACKETTYPE_ELECTION_END_VOTE, OnElectionEndVote );
	ON_MSG( PACKETTYPE_LORD, OnLord );
	ON_MSG( PACKETTYPE_L_EVENT_CREATE, OnLEventCreate );
	ON_MSG( PACKETTYPE_L_EVENT_INITIALIZE, OnLEventInitialize );
	ON_MSG( PACKETTYPE_LORD_SKILL_USE, OnLordSkillUse );
	ON_MSG( PACKETTYPE_LORD_SKILL_TICK, OnLordSkillTick );
	ON_MSG( PACKETTYPE_L_EVENT_TICK, OnLEventTick );

	ON_MSG( PACKETTYPE_TAX_ALLINFO, OnTaxInfo );

	ON_MSG( PACKETTYPE_SECRETROOM_INFO_CLEAR, OnSecretRoomInfoClear );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, OnSecretRoomTenderInfo );

	ON_MSG( PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, OnRainbowRaceInfo );

	ON_MSG( PACKETTYPE_HOUSING_LOADINFO, OnHousingLoadInfo );
	ON_MSG( PACKETTYPE_HOUSING_FURNITURELIST, OnHousingSetFunitureList );
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, OnHousingSetupFuniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, OnHousingSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_DBFAILED, OnHousingDBFailed );

	ON_MSG( PACKETTYPE_PROPOSE, OnProposeResult );
	ON_MSG( PACKETTYPE_COUPLE, OnCoupleResult );
	ON_MSG( PACKETTYPE_DECOUPLE, OnDecoupleResult );
	ON_MSG( PACKETTYPE_ALL_COUPLES, OnCouple );

	ON_MSG( PACKETTYPE_ADD_COUPLE_EXPERIENCE, OnAddCoupleExperience );

	ON_MSG( PACKETTYPE_FUNNYCOIN_ACK_USE, OnFunnyCoinAckUse );

	ON_MSG( PACKETTYPE_PCBANG_SETAPPLY, OnPCBangToggle );

	ON_MSG( PACKETTYPE_TIMELIMIT_INFO, OnTimeLimitInfoAck );
	ON_MSG( PACKETTYPE_TIMELIMIT_RESET, OnTimeLimitReset );

	ON_MSG( PACKETTYPE_QUIZ_OPEN, OnQuizEventOpen );
	ON_MSG( PACKETTYPE_QUIZ_LOADQUIZ, OnQuizList );
	ON_MSG( PACKETTYPE_QUIZ_NOTICE, OnQuizEventNotice );
	ON_MSG( PACKETTYPE_QUIZ_CHANGED, OnQuizEventChanged );

	ON_MSG( PACKETTYPE_GUILDHOUSE_LOAD, OnLoadGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_REMOVE, OnRemoveGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, OnGuildHousePacket );
#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_INFO, OnGuildHouseTenderInfo );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, OnGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_STATE, OnGuildHouseTenderState );
	ON_MSG( PACKETTYPE_GUILDHOUSE_GRADE_UPDATE, OnGuildHouseGradeUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, OnGuildHouseLevelUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_EXPIRED, OnGuildHouseExpired );
#endif // __GUILD_HOUSE_MIDDLE

	ON_MSG( PACKETTYPE_CAMPUS_ALL, OnAllCampus );
	ON_MSG( PACKETTYPE_CAMPUS_ADD_MEMBER, OnAddCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, OnRemoveCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_UPDATE_POINT, OnUpdateCampusPoint );


	//////////////////////////////////////////////////////////////////////////
	// mirchang_100723 give coupon item event

	ON_MSG( PACKETTYPE_COUPON_ITEM_NUMBER, OnCouponNumber );

	// mirchang_100723 give coupon item event
	//////////////////////////////////////////////////////////////////////////

	ON_MSG( PACKETTYPE_SERIALNUMBER_ACK, OnSerialNumberAck );

	//협동 기부
	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTION_INFO_AND_CLEAR_EXPIRED_ACK, OnCooperativeContributions_ContributionInfo );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK, OnCooperativeContributions_Contribute );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK, OnCooperativeContributions_GetRanking );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED_ACK, OnCooperativeContributions_GetContributed );
	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM_ACK, OnCooperativeContributions_TryRewardItem );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO_ACK, OnCooperativeContributions_TryWriteItemRewardInfo );
	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_ACK, OnCooperativeContributions_RewardItem );
	//협동 기부

	//콜로세움
	ON_MSG( PACKETTYPE_COLOSSEUM_GET_RANKING_INFO, OnColosseum_GetDungeonRankingInfo );

	// 텔레포트 지도
	ON_MSG( PACKETTYPE_TELEPORT_MAP_POINT_LIST_NOTI, OnTeleportMapPointListNoti );

	//ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_USER_INFO_ACK, OnConsignmentSale_UserInfoAck );

	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_SELL_ITEM_NOTI, OnConsignmentSale_SellItemNoti );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGIST_ACK, OnConsignmentSale_RegistAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_ACK, OnConsignmentSale_CancelRegistedAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_ACK, OnConsignmentSale_RegistedInfoListAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_ACK, OnConsignmentSale_GetPriceAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_ACK, OnConsignmentSale_BuyItemAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_ACK, OnConsignmentSale_CollectSaleGoldAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_SEARCH_ACK, OnConsignmentSale_SearchAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_INFO_LIST_ACK, OnConsignmentSale_SaleGoldInfoListAck );

	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_ACK, OnConsignmentSale_GetRegistedCountAck );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_ACK, OnConsignmentSale_CheckRegistedCountAck );

	ON_MSG( PACKETTYPE_SERVER_LIST, OnServerList );
	ON_MSG( PACKETTYPE_GUILD_BANK_QUERY_FAIL_ACK, OnGuildBankQueryFailAck );
	ON_MSG( PACKETTYPE_RECORD_DUNGEON_PLAY_INFO, OnDungeonBestCompleteTimeAck );

	ON_MSG( PACKETTYPE_GUILD_BANK_OUTPUT_ITEM_ACK, OnGuildBankOutputItemAck );
	ON_MSG( PACKETTYPE_GUILD_BANK_OUTPUT_GOLD_ACK, OnGuildBankOutputGoldAck );
}

CDPDatabaseClient::~CDPDatabaseClient()
{
}

BOOL	CDPDatabaseClient::Run( LPSTR lpszAddr, USHORT uPort, BOOL bSingleThreaded )
{
	if( ConnectToServer( lpszAddr, uPort, FLNormalProtocol::GetInstance(), bSingleThreaded ) == FALSE )
	{
		return FALSE;
	}

	// 서버 구동 시 시리얼넘버를 제일 먼저 얻기위한 삽질
	if( CSerialNumber::GetInstance()->IsInit() == false )
	{
		CNetSerialNumberRequest::GetInstance()->Request();
		CNetSerialNumberRequest::GetInstance()->Request();

		while( CSerialNumber::GetInstance()->IsInit() == false )
		{
			ReceiveMessage();
			Sleep( 10 );
		}
	}

	return TRUE;
}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_DESTROYPLAYERORGROUP:
		break;
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - ( sizeof(DPID) + sizeof(DPID) ) );
	GETTYPE( ar );

	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );

	if( pfn ) 
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ) );
	else 
		FLERROR_LOG( PROGRAM_NAME, _T( "Handler not found(%08x)"), dw );
}

#ifdef __LAYER_1015
void CDPDatabaseClient::SavePlayer( FLWSUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, int nLayer, BOOL bLogout )
#else	// __LAYER_1015
void CDPDatabaseClient::SavePlayer( FLWSUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL bLogout )
#endif	//__LAYER_1015
{
	// 이벤트용 아레나에서는 저장하지 않는다.
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		return;
	}

	BEFORESENDDUAL( ar, PACKETTYPE_SAVE_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN );  

	ar << bLogout;

	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << ((CMover*)pUser)->m_nDBDataSlot;

	pUser->m_dwTimeout4Save	= SEC_SAVEPLAYER;

	ar << dwWorldId;
	ar << vPos;
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015

	// raiders.2006.11.28  저장될 돈 = 인벤돈 + 내 거래창 돈
	int nTrade = pUser->m_vtInfo.TradeGetGold();
	pUser->AddGold( nTrade, FALSE );
	pUser->Serialize( ar, METHOD_NONE );
	pUser->AddGold( -nTrade, FALSE );
	//

	pUser->m_playTaskBar.Serialize( ar );
	ar << pUser->m_RTMessenger.GetState();

	ar << pUser->m_dwReturnWorldID;
	ar << pUser->m_vReturnPos;

	ar << pUser->m_nEventFlag;
	ar << pUser->m_dwEventTime;
	ar << pUser->m_dwEventElapsed;

	ar << pUser->m_nExpLog;
	ar << pUser->m_nAngelExpLog;

	ar << pUser->m_nCoupon;

	ar << pUser->GetRestPoint();

	SEND( ar, this, DPID_SERVERPLAYER );
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
	{
		SendTimeLimitUpdateReq( pUser->m_playAccount.lpszAccount, pUser->m_nAccountPlayTime );
	}
	//	mulcom	END100315	베트남 시간 제한
}

void CDPDatabaseClient::SendRecommend( FLWSUser* pUser, int nValue )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVE_RECOMMEND, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << pUser->m_idPlayer;
	ar << pUser->GetLevel();
	ar << pUser->GetSex();
	ar << nValue;
	SEND( ar, this, DPID_SERVERPLAYER );	
}

void CDPDatabaseClient::SendITEM_TBL_Update()
{
	BEFORESENDDUAL( ar, PACKETTYPE_ITEM_TBL_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
	int nQuestCount = 0;
	ptrdiff_t nQuestCountTag = ar.ReserveSpace( sizeof( nQuestCount ) );
	for( int i = 0; i < prj.m_aPropQuest.GetSize(); i++ )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
			ar << i;
			ar.WriteString( pQuestProp->m_szTitle );
			nQuestCount++;
		}
	}
	ar.WriteReservedSpace( nQuestCountTag, &nQuestCount, sizeof( nQuestCount ) );

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPreventLogin( LPCTSTR szAccount, DWORD dwPreventTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PREVENT_LOGIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << dwPreventTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogConnect( FLWSUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_CONNECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetWorld()->GetID();	// world id
	ar.Write( &pUser->m_playAccount.m_stLogin, sizeof(SYSTEMTIME) );
	ar.WriteString( pUser->m_playAccount.lpAddr );	// ip

	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
	DWORD dwSeed = pUser->GetGold() + pUser->m_dwGoldBank[byProgramDataSlot];
	ar << dwSeed;	// seed
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << pUser->GetLevel();
	ar << pUser->GetJob();
	ar << CPCBang::GetInstance()->GetPCBangClass( pUser->m_idPlayer );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << dwPenya << dwExistingPenya;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendOutGuildCombat( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGuildCombatStart( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_START_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGuildCombatResult( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );

	//	float fRequstCanclePercent = g_GuildCombatMng.m_nRequestCanclePercent / 100.0f;
	float fNotRequestPercent = g_GuildCombatMng.m_nNotRequestPercent / 100.0f;
	ar << g_GuildCombatMng.m_uWinGuildId;
	ar << g_GuildCombatMng.m_nWinGuildCount;
	ar << g_GuildCombatMng.m_uBestPlayer;

	// 길드 저장
	ar << (u_long)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( size_t gcmi = 0 ; gcmi != g_GuildCombatMng.m_vecGuildCombatMem.size() ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem.at( gcmi );

		BOOL bSelectGuild = FALSE; // 출전한 길드인가
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.vecRequestRanking.size() ) ; ++veci )
		{
			if( veci >= g_GuildCombatMng.m_nMaxGuild )
				break;

			CGuildCombat::__REQUESTGUILD RequestGuild = g_GuildCombatMng.vecRequestRanking.at( veci );
			if( RequestGuild.uidGuild == pGCMember->uGuildId )
			{
				bSelectGuild = TRUE;
				break;
			}
		}

		ar << pGCMember->uGuildId;
		ar << pGCMember->nGuildPoint;
		if( bSelectGuild )	// 출전 길드
		{
			if( g_GuildCombatMng.m_uWinGuildId == pGCMember->uGuildId )	// 승리한 길드
			{
				ar << g_GuildCombatMng.GetPrizePenya( 0 );
				ar << (DWORD)0;
				ar << g_GuildCombatMng.m_nWinGuildCount;
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0;
				ar << (int)0;
			}
		}
		else
		{
			if( pGCMember->bRequest )	// 신청은 했는데 출전이 안된 길드 수수료 2%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fNotRequestPercent );
				ar << (int)0;

				// 바로 우편함으로 지급
				CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
				if( pGuild )
				{
					FLItemElem pItemElem;
					char szMsg[1000] = { 0, };
					FLSPrintf( szMsg, _countof( szMsg ), "%s", prj.GetText( TID_GAME_GC_NOTREQUEST ) );
					char szMsg1[1000] = { 0, };
					FLSPrintf( szMsg1, _countof( szMsg1 ), "%s", prj.GetText( TID_GAME_GC_NOTREQUEST1 ) );
					SendQueryPostMail( pGuild->m_idMaster, 0, pItemElem, (int)( pGCMember->dwPenya * fNotRequestPercent ), szMsg, szMsg1 );
				}
			}
			else						// 취소한 길드 수수료 20%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fRequstCanclePercent );
				ar << (int)0;
			}
		}

		// 참가자 포인트
		ar << (u_long)pGCMember->vecGCSelectMember.size();
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember.at( veci );
			ar << pJoinPlayer->uidPlayer;
			ar << pJoinPlayer->nPoint;
			if( pJoinPlayer->uidPlayer == g_GuildCombatMng.m_uBestPlayer )
			{
				ar << g_GuildCombatMng.GetPrizePenya( 1 );
			}
			else 
			{
				ar << (__int64)0;
			}
		}
	}

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildcombatContinue( int nGuildCombatID,  u_long uidGuild, int nWinCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGuildCombatID;
	ar << uidGuild;
	ar << nWinCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGCGetPenyaGuild( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGCGetPenyaPlayer( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnJoin( CAr & ar, DPID dpidCache, DPID dpidUser )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "WORLDSERVER.EXE\t// PACKETTYPE_JOIN\t// %d" ), g_uKey );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( pUser == NULL )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ Get User Failed. dpidCache(%d), dpidUser(%d) ]" ), dpidCache, dpidUser );
		return;
	}

	E_LOGIN_RESULT eRet;
	ar >> eRet;

	if( eRet == E_LOGIN_FAILURE )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ E_LOGIN_FAILURE. dpidCache(%d), dpidUser(%d) ]" ), dpidCache, dpidUser );
		return;
	}

	if( eRet == E_LOGIN_SHUTDOWN_RULE )
	{
		CAr ar;
		ar << pUser->m_Snapshot.dpidUser << PACKETTYPE_JOIN_IN_SHUTDOWN_RULE;
		u_long nBufSize;
		LPBYTE lpBuf		= ar.GetBuffer( &nBufSize );
		g_DPSrvr.Send( (LPVOID)lpBuf, nBufSize, dpidCache );
		return;
	}

	DWORD dwWorldId, dwIndex;
	DWORD dwSavePlayerTime = 0;
	//	DWORD dwSaveFlyTime = 0;
	int nMaximumLevel = 0;
	DWORD dwAuthKey;
	ar >> dwAuthKey;
	if( pUser->m_dwAuthKey != dwAuthKey )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Different dwAuthKey. User_AuthKey(%d), Receive_AuthKey(%d) ]" ), pUser->m_dwAuthKey, dwAuthKey );
		return;		//
	}

	CTime tmBirthDate;
	ar >> tmBirthDate;	//
	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR && FLShutdownRule::InShutDownRule( tmBirthDate ) == true )
	{
		CAr ar;
		ar << pUser->m_Snapshot.dpidUser << PACKETTYPE_JOIN_IN_SHUTDOWN_RULE;
		u_long nBufSize;
		LPBYTE lpBuf		= ar.GetBuffer( &nBufSize );
		g_DPSrvr.Send( (LPVOID)lpBuf, nBufSize, dpidCache );
		return;
	}



	char lpAddrtmp[16];

#ifdef __ON_ERROR
	int nOnError	= 0;
	try
	{
#endif	// __ON_ERROR

		// DatabaseServer에서 IP는 설정하지 않고 보내준다.
		memcpy_s( lpAddrtmp, sizeof( lpAddrtmp ), pUser->m_playAccount.lpAddr, sizeof( lpAddrtmp ) );
		ar >> pUser->m_playAccount;
		memcpy_s( pUser->m_playAccount.lpAddr, sizeof( pUser->m_playAccount.lpAddr ), lpAddrtmp, sizeof( pUser->m_playAccount.lpAddr ) );


		ar >> dwWorldId;
#ifdef __LAYER_1015
		int nLayer;
		ar >> nLayer;
#endif // __LAYER_1015

		ar >> dwIndex;

#ifdef __ON_ERROR
		nOnError	= 1;
#endif	// __ON_ERROR

		pUser->SetIndex( NULL, dwIndex, FALSE );
		pUser->Serialize( ar, METHOD_NONE );
		pUser->InitMotion( pUser->m_dwMotion );
		pUser->m_nCheerPoint	= 0;
		pUser->m_dwTickCheer	= GetTickCount() + TICK_CHEERPOINT;

		pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기

#ifdef __GUILD_HOUSE_MIDDLE
		GuildHouseMng->CheckGuildHouseQuest( pUser );
#endif // __GUILD_HOUSE_MIDDLE

		if( pUser->GetLevel() > MAX_CHARACTER_LEVEL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "MAX_CHARACTER_LEVEL//%s//%d" ), pUser->GetName(), pUser->GetLevel() );
			pUser->m_nLevel	= MAX_CHARACTER_LEVEL;
		}

		//////////////////////////////////////////////////////////////////////////
		// skill point check
//		pUser->CorrectErrorsSkillPoint();
		//////////////////////////////////////////////////////////////////////////

		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10445, II_CHEERUP ) );

		g_xApplyItemEffect->RefreshAllEquipActiveSkill( pUser );

		if( !pUser->GetPet() )
			pUser->SetPetId( NULL_ID );

		pUser->m_pActMover->RemoveStateFlag( OBJSTAF_ACC );			// 비행은 멈춤상태  		
		pUser->SetAngle( 0.0f );	// 각도 초기화, 임시 코드이므로 원인을 분석하여 수정하도록 하자.

		//////////////////////////////////////////////////////////////////////////
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			g_pEventArena->OnJoinHook( pUser, &dwWorldId, &nLayer );
		}
		//////////////////////////////////////////////////////////////////////////

		// 이상타 땜방 dwWorldId가 200이면서 못 들어가는 유저들이 생김
		CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
		if( pWorld && pWorld->VecInWorld( pUser->GetPos() ) == FALSE )
		{
			dwWorldId = 1;
			pUser->SetPos( D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f ) );
		}

		pUser->AdjustGuildQuest( dwWorldId );
		pUser->m_nDuel	= pUser->m_idDuelParty = 0;

#ifdef __ON_ERROR
		nOnError	= 2;
#endif	// __ON_ERROR

		pUser->m_playTaskBar.Serialize( ar );

#ifdef __ON_ERROR
		nOnError	= 3; 
#endif	// __ON_ERROR

		pUser->m_RTMessenger.Serialize( ar );

#ifdef __ON_ERROR
		nOnError	= 4;
#endif	// __ON_ERROR
		ar.ReadString( pUser->m_szPartyName, _countof( pUser->m_szPartyName ) );
		ar.ReadString( pUser->m_szBankPass, _countof( pUser->m_szBankPass ) );
		ar >> dwSavePlayerTime;
		pUser->SetPlayerTime( dwSavePlayerTime );

		ar >> nMaximumLevel;

		// 쪽지를 읽어들인다.
		short nTagCount;
		ar >> nTagCount;

		TAG_ENTRY tags[MAX_TAGS];
		for( short i=0; i<nTagCount; ++i )
		{
			ar >> tags[i].idFrom;					// 누구에게서 
			ar >> tags[i].dwDate;					// 언제 
			ar.ReadString( tags[i].szString, _countof( tags[i].szString ) );		// 쪽지 내용 
		}

		char GuildMemberTime[15] = {0,};
		ar.ReadString( GuildMemberTime, _countof( GuildMemberTime ) );


		//////////////////////////////////////////////////////////////////////////
		// mirchang_100802 CTime check

		if( _tcslen( GuildMemberTime ) == 0 )
		{
			CTime tCurrentTime	= CTime::GetCurrentTime();
			pUser->m_tGuildMember = tCurrentTime;

		}
		else
		{
			char cYear[5] = {0,};
			char cMonth[3] = {0,};
			char cDay[3] = {0,};
			char cHour[3] = {0,};
			char cMin[3] = {0,};

			FLStrncpy(cYear, _countof( cYear ), GuildMemberTime, 4);
			FLStrncpy(cMonth, _countof( cMonth ), GuildMemberTime + 4, 2 );
			FLStrncpy(cDay, _countof( cDay ), GuildMemberTime + 6, 2 );
			FLStrncpy(cHour, _countof( cHour ), GuildMemberTime + 8, 2 );
			FLStrncpy(cMin, _countof( cMin ), GuildMemberTime + 10, 2 );

			int nYear, nMonth, nDay, nHour, nMin;
			nYear = nMonth = nDay = nHour = nMin = 0;

			nYear = atoi( cYear );
			nMonth = atoi( cMonth );
			nDay = atoi( cDay );
			nHour = atoi( cHour );
			nMin = atoi( cMin );

			if( g_pCommonCheck->CheckTime( nYear, nMonth, nDay, nHour, nMin, 0 ) == true )
			{
				CTime tGuildMember( nYear, nMonth, nDay, nHour, nMin, 0 );
				pUser->m_tGuildMember = tGuildMember;
			}
			else
			{
				CTime tCurrentTime	= CTime::GetCurrentTime();
				pUser->m_tGuildMember	= tCurrentTime;
			}
		}

		// mirchang_100802 CTime check
		//////////////////////////////////////////////////////////////////////////


		ar >> pUser->m_dwReturnWorldID;
		ar >> pUser->m_vReturnPos;

		ar >> pUser->m_nEventFlag;
		ar >> pUser->m_dwEventTime;
		ar >> pUser->m_dwEventElapsed;

		ar >> pUser->m_nExpLog;
		ar >> pUser->m_nAngelExpLog;

		ar >> pUser->m_nCoupon;

		DWORD dwPCBangClass;
		ar >> dwPCBangClass;

		int nRestPoint; time_t tLogOut;
		ar >> nRestPoint >> tLogOut;

		if( pUser->m_idGuild != 0 )
		{
			pUser->SetRestPoint( nRestPoint );
		}
		else
		{
			pUser->SetRestPoint( 0 );
		}

#ifdef __ON_ERROR
		nOnError	= 5;
#endif	// __ON_ERROR

		pUser->m_pActMover->ClearState();
		pUser->RedoEquip( FALSE );

		if( pUser->m_nHitPoint == 0 )
			pUser->m_nHitPoint	= pUser->GetMaxHitPoint();

		pUser->SetHitPoint( pUser->GetHitPoint() );			// max넘은 경우를 대비한다.
		pUser->SetManaPoint( pUser->GetManaPoint() );		// max넘은 경우를 대비한다.
		pUser->SetFatiguePoint( pUser->GetFatiguePoint() ); // max넘은 경우를 대비한다.

		pUser->SetId( NULL_ID );
		pUser->CheckHonorStat();

		if( prj.GetUserByID( pUser->m_idPlayer ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "an ocular spectrum %d" ), pUser->m_idPlayer );
			pUser->m_idPlayer	= (u_long)0xffffffff;
			/*
			1. 이 경우는 현재 발생하지 않는다. 그러나 만약을 대비하여 추적 로그를 남기도록 한다.
			*/
			return;
		}

#ifdef __ON_ERROR
		nOnError	= 6;
#endif	// __ON_ERROR

		pUser->ValidateItem();
		pUser->m_nGuildCombatState	= 0;

#ifdef __ON_ERROR
		nOnError	= 700;
#endif	// __ON_ERROR

//		pUser->AdjustAuthority();

		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) );

#ifdef __ON_ERROR
		nOnError	= 701;
#endif	// __ON_ERROR

		if( !g_eLocal.GetState( EVE_PK ) || pUser->GetPKValue() == 0 )
			pUser->RemoveIk3Buffs( IK3_ANGEL_BUFF );

#ifdef __ON_ERROR
		nOnError	= 702;
#endif	// __ON_ERROR

		pUser->AdjustMailboxState();

#ifdef __ON_ERROR
		nOnError	= 703;
#endif	// __ON_ERROR

#ifdef __LAYER_1015
		if( 
			GuildHouseMng->IsLoginAble( pUser, dwWorldId, nLayer ) &&
			pWorld && pWorld->m_linkMap.GetLinkMap( nLayer )							// 해당 레이어가 존재하고
			&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId )	// 인스턴스 던전이 아닌 경우...
			&& dwWorldId != WI_WORLD_QUIZ												// 퀴즈이벤트 지역이 아닌 경우
			)
		{
			g_xWSUserManager->AddPlayer( pUser, dwWorldId, nLayer );
		}
		else	// 레이어가 존재하지 않으면 무조건 마을로 이동한다.
		{
			pUser->m_vPos = D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f );
			g_xWSUserManager->AddPlayer( pUser, WI_WORLD_MADRIGAL, nDefaultLayer );
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
		}
#else	// __LAYER_1015
		g_xWSUserManager->AddPlayer( pUser, dwWorldId );
#endif	// __LAYER_1015

		if( !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			// 무적, 투명, 초필, 반무적 
			if( pUser->m_dwMode & ( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Abnormal User!! - %s(%07d), m_dwMode:%08x" ), pUser->GetName(), pUser->m_idPlayer, pUser->m_dwMode );
				pUser->SetNotMode( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE );
			}
		}

#ifdef __ON_ERROR
		nOnError	= 704;
#endif	// __ON_ERROR

		pUser->AddEventLuaDesc();

#ifdef __ON_ERROR
		nOnError	= 705;
#endif	// __ON_ERROR

		pUser->AdjustPartyQuest( dwWorldId );

#ifdef __ON_ERROR
		nOnError	= 706;
#endif	// __ON_ERROR

		pUser->AddTag( nTagCount, tags );	// 클라이언트에게 쪽지를 보낸다.

#ifdef __ON_ERROR
		nOnError	= 8;
#endif	// __ON_ERROR

		g_DPCoreClient.SendJoin( pUser->m_idPlayer, pUser->GetName(), ( pUser->m_dwAuthorization >= AUTH_GAMEMASTER ) );

		if( pUser->m_idGuild != 0 )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( !pGuild || !pGuild->IsMember( pUser->m_idPlayer ) )
				pUser->m_idGuild	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 9;
#endif	// __ON_ERROR

		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
		if( pParty && pParty->IsMember( pUser->m_idPlayer ) )
		{
			const char* pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( 0 ) );
			pUser->AddPartyMember( pParty, 0, ( pszLeader? pszLeader: "" ), "" );
			if( pParty->m_idDuelParty )		// 듀얼중인 파티였다면.
			{
				pUser->m_nDuel = 2;			// 듀얼상태를 세팅
				pUser->m_idDuelParty = pParty->m_idDuelParty;
			}

			//////////////////////////////////////////////////////////////////////////
			g_PartyMng.CheckAddMemberPartyEffect( pUser->m_idPlayer, pParty->m_uPartyId );
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			pUser->m_idparty	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 10;
#endif	// __ON_ERROR

		pUser->AddSMModeAll();

		pUser->AddGameSetting();
#ifdef __S1108_BACK_END_SYSTEM
		pUser->AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM

		// 서버 통합 작업
		pUser->SetAllAction();

#ifdef __ON_ERROR
		nOnError	= 11;
#endif	// __ON_ERROR

		pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		pUser->AddGuildCombatState();
		if( g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam == CGuildCombat::MAINTENANCE_STATE )
			pUser->AddGuildCombatEnterTime( g_GuildCombatMng.m_dwTime - GetTickCount() );
		else
			pUser->AddGuildCombatNextTime( g_GuildCombatMng.m_dwTime - GetTickCount(), g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam );
		pUser->AddGCWinGuild();
		pUser->AddGCBestPlayer();
#ifdef GUILD_WINNER_BUFF
		CTime tNextTime = g_GuildCombatMng.GetNextGuildCobmatTime();
		time_t d = (time_t)( tNextTime.GetTime() - time_null() );
		
		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 25582, II_SYS_SYS_LS_WINNERDIGNITY ) );
		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 25583, II_SYS_SYS_LS_SPLENDIDGLORY ) );

		if( pUser->m_idGuild == g_GuildCombatMng.m_uWinGuildId && g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
		{
			if (g_GuildCombatMng.m_nWinGuildCount == 1)
				pUser->AddBuff( BUFF_ITEM, ITEM_INDEX( 25582, II_SYS_SYS_LS_WINNERDIGNITY ), 0, d * 1000, 0, BT_TICK );
			else if (g_GuildCombatMng.m_nWinGuildCount == 2)
				pUser->AddBuff( BUFF_ITEM, ITEM_INDEX( 25583, II_SYS_SYS_LS_SPLENDIDGLORY ), 0, d * 1000, 0, BT_TICK );
		}
		
#endif
		//pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
		{
			g_xWSUserManager->AddGCGuildStatus( pUser->m_idGuild, pUser );
			g_xWSUserManager->AddGCWarPlayerlist( pUser->m_idGuild, pUser );
		}
#ifdef __ON_ERROR
		nOnError	= 12;
#endif	// __ON_ERROR

		CalluspXXXMultiServer( g_uIdofMulti, pUser );

#ifdef __ON_ERROR
		nOnError	= 13;
#endif	// __ON_ERROR

		pWorld = pUser->GetWorld();
		g_GuildCombat1to1Mng.SendNowState( pUser );
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		{
			if( pWorld  && pWorld->GetID() >= WI_WORLD_GUILDWAR1TO1_0
				&& pWorld->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
			{
				pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_NORMAL, nDefaultLayer );
			}
		}

		// 		if( !pUser->GetWorld() )
		// 			pUser->Replace( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ) );

		if( pUser->IsCollecting() )
			pUser->StopCollecting();

#ifdef __ON_ERROR
		nOnError	= 14;
#endif	// __ON_ERROR

		CSecretRoomMng::GetInstance()->GetAllInfo( pUser );
		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_FLARIS ), REPLACE_NORMAL, nDefaultLayer );

		if( pWorld->GetID() == WI_DUNGEON_SECRET_0 )
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_DARKON12 ), REPLACE_NORMAL, nDefaultLayer );

#ifdef __ON_ERROR
		nOnError	= 15;
#endif	// __ON_ERROR

		pUser->AddTaxInfo();
		CTax::GetInstance()->SendNoSetTaxRateOpenWnd( pUser );

#ifdef __ON_ERROR
		nOnError	= 16;
#endif	// __ON_ERROR

		{
			// 게임 진입 시 시스템 펫 각성 효과 적용
			FLItemElem* pItem	= pUser->GetPetItem();
			if( pItem )
				pUser->SetDestParamRandomOptExtension( pItem );
		}


#ifdef __ON_ERROR
		nOnError	= 17;
#endif	// __ON_ERROR

		// 완주하지 않고 접속을 끊은 놈은 탈락이므로 완주한 놈만 정보를 받게된다. chipi_090317
		if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_PROGRESS )
		{
			CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
			if( pRainbowRace )
				pUser->AddRainbowRaceState( pRainbowRace );
		}

		CHousingMng::GetInstance()->CreateRoomLayer( pUser->m_idPlayer );
		CHousingMng::GetInstance()->ReqLoadHousingInfo( pUser->m_idPlayer );

		CPCBang::GetInstance()->SetPCBangPlayer( pUser, dwPCBangClass );

		//	mulcom	BEGIN100315	베트남 시간 제한
		if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
		{
			SendTimeLimitInfoReq( pUser->m_idPlayer, pUser->m_playAccount.lpszAccount );
		}
		//	mulcom	END100315	베트남 시간 제한

		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
		{
			// 인던 안에서 접속 종료 후 재접속 시 플라리스로 가지 않고 인던 출구로 강제 이동
			CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}

		pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( pUser->m_idGuild ) );
		GuildHouseMng->SetApplyDST( pUser );
		if( pWorld && GuildHouseMng->IsGuildHouse( pWorld->GetID() ) && static_cast< u_long >( pUser->GetLayer() ) == pUser->m_idGuild )	// 접속지가 길드하우스 이면..
			pUser->SetIncRestPoint( tLogOut / REST_POINT_TICK * REST_POINT_LOGOUT_INC );	// 로그아웃 휴식 포인트를 증가 시켜준다.

		u_long idCampus = pUser->GetCampusId();
		if( idCampus )
		{
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( idCampus );
			if( pCampus && pCampus->IsMember( pUser->m_idPlayer ) )
				pUser->AddUpdateCampus( pCampus );
			else
				pUser->SetCampusId( 0 );
		}

		switch( _GetContentState( CT_DONATION ) )
		{
		case CS_VER1:
			{
				COOPERATIVE_CONTRIBUTIONS().TryReward_LoginUser( *pUser );		// 협동 기부 보상 체크
			}
			break;
		default:
			__noop;
			break;
		}

		if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
			FLShutdownRule_Mng::GetInstance().AddUser( pUser->m_idPlayer, tmBirthDate );

		g_pMadrigalGift->OnLogin( pUser, tLogOut );

#ifdef __ON_ERROR
	}
	catch( ... )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s, %d" ), pUser->GetName(), nOnError );
	}
#endif	// __ON_ERROR


}

void CDPDatabaseClient::OnGCResultValue( CAr & ar )
{
	u_long uSize;
	g_GuildCombatMng.m_GCResultValueGuild.clear();
	ar >> uSize;

	int	i = 0;

	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild;
		ar >> ResultValueGuild.nCombatID;
		ar >> ResultValueGuild.uidGuild;
		ar >> ResultValueGuild.nReturnCombatFee;
		ar >> ResultValueGuild.nReward;
		g_GuildCombatMng.m_GCResultValueGuild.push_back( ResultValueGuild );
	}

	// 베스트 플레이어 관련 정보
	g_GuildCombatMng.m_GCResultValuePlayer.clear();
	ar >> uSize;
	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer;
		ar >> ResultValuePlayer.nCombatID;
		ar >> ResultValuePlayer.uidGuild;
		ar >> ResultValuePlayer.uidPlayer;
		ar >> ResultValuePlayer.nReward;
		g_GuildCombatMng.m_GCResultValuePlayer.push_back( ResultValuePlayer );
	}
}
void CDPDatabaseClient::OnGCPlayerPoint( CAr & ar )
{
	g_GuildCombatMng.m_vecGCPlayerPoint.clear();
	// 길드대전 개인 포인트
	u_long uSize;
	ar >> uSize;
	for( int i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint;
		ar >> GCPlayerPoint.uidPlayer;
		ar >> GCPlayerPoint.nJob;
		ar >> GCPlayerPoint.nPoint;
		g_GuildCombatMng.m_vecGCPlayerPoint.push_back( GCPlayerPoint );
	}
}

void CDPDatabaseClient::OnAllGuildData( CAr & ar, DPID, DPID )
{
	g_GuildMng.Serialize( ar, FALSE );
	g_GuildWarMng.Serialize( ar );

	FLINFO_LOG( PROGRAM_NAME, _T( "[ Receive Guild Data from Trans ]" ) );

	if ( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
	{
		g_dpDBClient.SendQueryGuildBank( g_uKey );
	}
}

void CDPDatabaseClient::OnAllGuildCombat( CAr & ar, DPID, DPID )
{
	g_GuildCombatMng.GuildCombatClear(1);
	u_long uSize, idGuild;
	DWORD dwPenya;
	BOOL bRequest;
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	// 참가한 인원수
	ar >> uSize;
	for( u_long i = 0 ; i < uSize ; ++i )
	{
		ar >> idGuild;
		ar >> dwPenya;
		ar >> bRequest;

		CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
		if( pGuild )
		{
			g_GuildCombatMng.JoinGuildCombat( idGuild, dwPenya, bRequest );
		}
	}

	// 길드 보상 관련 정보
	OnGCResultValue( ar );

	// 길드대전 플레이어 포인트
	OnGCPlayerPoint( ar );

	g_GuildCombatMng.SetNpc();

}
// 참가 신청
void CDPDatabaseClient::OnInGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	DWORD dwPenya, dwExistingPenya;
	ar >> idGuild;
	ar >> dwPenya;
	ar >> dwExistingPenya;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		g_GuildCombatMng.JoinGuildCombat( idGuild, dwPenya, TRUE );
		FLWSUser* pMaster = (FLWSUser*)prj.GetUserByID( pGuild->m_idMaster );
		if( IsValidObj( pMaster ) )
		{
			CString str;

			// 전체 페냐가 0일경우
			if( dwExistingPenya == 0 )
			{
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), 0, dwPenya );
			}
			else
			{
				// 전체 금액이 있으면 추가되는 페냐를 빼서 계산..
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), dwExistingPenya, dwPenya-dwExistingPenya  );
			}

			pMaster->AddDiagText( str );
			g_xWSUserManager->AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}

		CString strMsg;
		// 길드대전 신청이 완료되었습니다.
		strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_GUILDCOMBAT ) );	// "수정해야함 : 길드대전에 신청하였습니다. 매주 토요일 17시에 길드대전이 시작됩니다 로 수정할까?? ㅎㅎ " );
		g_xWSUserManager->AddGuildMsg( pGuild, strMsg );
		strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP) );
		g_xWSUserManager->AddGuildMsg( pGuild, strMsg );
	}
}

void CDPDatabaseClient::OnOutGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	ar >> idGuild;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		BOOL bJoin = TRUE;
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( idGuild );
		if( pGCMember == NULL )
		{
			bJoin = FALSE;
		}
		else
		{
			if( pGCMember->bRequest == FALSE )
				bJoin = FALSE;
		}

		if( bJoin )
		{
			g_GuildCombatMng.OutGuildCombat( idGuild );
			FLWSUser* pMaster = (FLWSUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pMaster ) )
			{
				CString strMsg;
				strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_EXPENSE_RETURN ) );	// "수정해야함 : 대전 참가비는 대전이 끝나고 반환됩니다" );
				pMaster->AddText( strMsg );
			}

			CString strMsg;
			//신청하신 길드대전을 취소했습니다.
			strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL) );
			g_xWSUserManager->AddGuildMsg( pGuild, strMsg );
			g_xWSUserManager->AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}
	}
}

void CDPDatabaseClient::OnResultGuildCombat( CAr & ar, DPID, DPID )
{
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	OnGCResultValue( ar );
	g_GuildCombatMng.GuildCombatGameClear();

	g_xWSUserManager->AddGCWinGuild();
	g_xWSUserManager->AddGCBestPlayer();
}
void CDPDatabaseClient::OnPlayerPointGuildCombat( CAr & ar, DPID, DPID )
{
	OnGCPlayerPoint( ar );
}
void CDPDatabaseClient::OnGetPenyaGuildGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		if( uidGuild == pMover->m_idGuild )
		{
			int nGetResult = 0;
			__int64 nGetPenya = 0;

			std::vector<CGuildCombat::__GCRESULTVALUEGUILD>::iterator erase_pos = g_GuildCombatMng.m_GCResultValueGuild.end();
			std::vector<CGuildCombat::__GCRESULTVALUEGUILD>::iterator pos = g_GuildCombatMng.m_GCResultValueGuild.begin();
			std::vector<CGuildCombat::__GCRESULTVALUEGUILD>::iterator end = g_GuildCombatMng.m_GCResultValueGuild.end();
			CGuildCombat::__GCRESULTVALUEGUILD ResultValuetmp;
			for( ; pos != end; ++pos )
			{
				ResultValuetmp = (*pos);
				if( nGuildCombatId == ResultValuetmp.nCombatID && uidGuild == ResultValuetmp.uidGuild )
				{
					erase_pos = pos;
					if( ResultValuetmp.nReturnCombatFee )
						nGetResult = 1;
					else
						nGetResult = 0;
					nGetPenya = ResultValuetmp.nReturnCombatFee + ResultValuetmp.nReward;
					break;
				}
			}

			if( erase_pos != end && nGetResult )
			{				
				__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				{
					((FLWSUser*)pMover)->AddGCGetPenyaGuild( 3, nGetPenya );
				}

				pMover->AddGold( (int)( nGetPenya ) );
				((FLWSUser*)pMover)->AddGCGetPenyaGuild( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				//aLogItem.Action		= "9";
				//aLogItem.SendName	= pMover->GetName();
				//aLogItem.RecvName	= "GUILDCOMBAT_VG";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "9" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GUILDCOMBAT_VG" );
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
				aLogItem.kLogItem.nQuantity	= static_cast<int>( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );

				g_GuildCombatMng.m_GCResultValueGuild.erase( erase_pos );		
			}
		}
	}
}
void CDPDatabaseClient::OnGetPenyaPlayerGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		int nGetResult = 0;
		__int64 nGetPenya = 0;

		std::vector<CGuildCombat::__GCRESULTVALUEPLAYER>::iterator erase_pos = g_GuildCombatMng.m_GCResultValuePlayer.end();
		std::vector<CGuildCombat::__GCRESULTVALUEPLAYER>::iterator pos = g_GuildCombatMng.m_GCResultValuePlayer.begin();
		std::vector<CGuildCombat::__GCRESULTVALUEPLAYER>::iterator end = g_GuildCombatMng.m_GCResultValuePlayer.end();
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuetmp;
		for( ; pos != end; ++pos )
		{
			ResultValuetmp = (*pos);
			if( nGuildCombatId == ResultValuetmp.nCombatID && uidPlayer == ResultValuetmp.uidPlayer && uidGuild == ResultValuetmp.uidGuild )
			{
				erase_pos = pos;
				nGetPenya = ResultValuetmp.nReward;
				break;
			}
		}

		if( erase_pos != end )
		{				
			__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
			{
				((FLWSUser*)pMover)->AddGCGetPenyaPlayer( 3, nGetPenya );
				erase_pos = end;
			}

			if( erase_pos != end )
			{
				pMover->AddGold( (int)( nGetPenya ) );
				((FLWSUser*)pMover)->AddGCGetPenyaPlayer( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				//aLogItem.Action		= "9";
				//aLogItem.SendName	= pMover->GetName();
				//aLogItem.RecvName	= "GUILDCOMBAT_VP";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "9" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GUILDCOMBAT_VP" );
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
				aLogItem.kLogItem.nQuantity	= static_cast<int>( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );
			}

			g_GuildCombatMng.m_GCResultValuePlayer.erase( erase_pos );		
		}
	}
}

void CDPDatabaseClient::OnContinueGC( CAr & /*ar*/, DPID, DPID )
{
	g_GuildCombatMng.m_uWinGuildId = 0;
	g_GuildCombatMng.m_nWinGuildCount = 0;

	g_xWSUserManager->AddGCWinGuild();
}

void CDPDatabaseClient::OnAllPlayerData( CAr & ar , DPID, DPID )
{
	CPlayerDataCenter::GetInstance()->Serialize( ar );
}

void CDPDatabaseClient::OnAddPlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	PlayerData pd;
	ar >> idPlayer;
	ar.ReadString( pd.szPlayer, _countof( pd.szPlayer ) );
	ar.Read( &pd.data, sizeof(sPlayerData) );
	CPlayerDataCenter::GetInstance()->AddPlayerData( idPlayer, pd );
}

void CDPDatabaseClient::OnDeletePlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;
	CPlayerDataCenter::GetInstance()->DeletePlayerData( idPlayer );
}

void CDPDatabaseClient::SendUpdatePlayerData( FLWSUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );  
	sPlayerData data;
	data.nJob	= static_cast< BYTE >( pUser->GetJob() );
	data.nLevel	= static_cast< BYTE >( pUser->GetLevel() );
	data.nSex	= pUser->GetSex();
	ar << pUser->m_idPlayer;
	ar.Write( &data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_SERVERPLAYER );	
}

void	CDPDatabaseClient::SendPacket( const FLPacket* pPacket )
{
	SendPacket( DPID_UNKNOWN, DPID_UNKNOWN, pPacket );
	//if( pPacket == NULL )
	//{
	//	return;
	//}

	//CAr ar;
	//u_long nBufSize = 0;
	//ar << DPID_UNKNOWN << DPID_UNKNOWN;

	//if( pPacket->Serialize( ar ) == false )
	//{
	//	return;
	//}

	//LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	//Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void	CDPDatabaseClient::SendPacket( const DPID dpidCache, const DPID dpidUser, const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;
	ar << dpidCache << dpidUser;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnUpdatePlayerData( CAr & ar , DPID, DPID )
{
	u_long idPlayer;
	sPlayerData data;
	ar >> idPlayer;
	ar.Read( &data, sizeof(sPlayerData) );

	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( pPlayerData )
	{
		memcpy_s( &pPlayerData->data, sizeof( pPlayerData->data ), &data, sizeof(sPlayerData) );
		CCoupleHelper::Instance()->OnUpdatePlayerData( idPlayer, pPlayerData );
		CCampusHelper::GetInstance()->OnUpdatePlayerData( idPlayer, pPlayerData );
	}
}

void CDPDatabaseClient::OnBaseGameSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	BOOL bBaseGameSetting = FALSE;
	BOOL bFirst = FALSE;
	ar >> bFirst;
	ar >> bBaseGameSetting;
	if( bBaseGameSetting )
	{
		ar >> prj.m_fMonsterExpRate	>> prj.m_fGoldDropRate >> prj.m_fItemDropRate >> prj.m_fMonsterHitRate;
		ar >> prj.m_fShopCost >> prj.m_fMonsterRebirthRate >> prj.m_fMonsterHitpointRate >> prj.m_fMonsterAggressiveRate >> prj.m_fMonsterRespawnRate;
	}

	g_xWSUserManager->AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );
	g_xWSUserManager->AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
	g_xWSUserManager->AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );	
	g_xWSUserManager->AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
	g_xWSUserManager->AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
	g_xWSUserManager->AddGameRate( prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH );
	g_xWSUserManager->AddGameRate( prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT );
	g_xWSUserManager->AddGameRate( prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE );
	g_xWSUserManager->AddGameRate( prj.m_fMonsterRespawnRate, GAME_RATE_RESPAWN );	

	if( bFirst )
	{
		CWorld* pWorld;
		pWorld	= g_WorldMng.GetFirstActive();
		while( pWorld )
		{
			pWorld->LoadRegion();
#ifdef __EVENT_0117
#ifdef __LAYER_1021
			CRespawner* pRespawner	= pWorld->m_respawner.Proto();
#else	// __LAYER_1021
			CRespawner* pRespawner	= &pWorld->m_respawner;
#endif	// __LAYER_1021
			for( int i = 0; i < (int)( pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION].size() ); ++i )
			{
				CRespawnInfo* pRespawnInfo	= &( pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION][i] );

				if( pRespawnInfo->m_dwType == OT_MOVER )	// 몬스터 리스폰
				{
					MoverProp* pMoverProp	= prj.GetMoverProp( pRespawnInfo->m_dwIndex );
					if( pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_MONSTER_LEVEL )
					{
#ifdef _DEBUG
						FLTRACE_LOG( PROGRAM_NAME, _T( "%d, (%d, %d, %d, %d)" ), pRespawnInfo->m_dwIndex, pRespawnInfo->m_rect.left, pRespawnInfo->m_rect.top, pRespawnInfo->m_rect.right, pRespawnInfo->m_rect.bottom );
#endif	// _DEBUG
						CEventGeneric::GetInstance()->AddRegionGeneric( pMoverProp->dwLevel, pWorld->GetID(), pRespawnInfo );
#ifdef __EVENTLUA_SPAWN
						if( pWorld->GetID() == WI_WORLD_MADRIGAL )	// 현재까지는 마드리갈 대륙에만 출현한다.
							prj.m_EventLua.SetAddSpawnRegion( pWorld->GetID(), pRespawnInfo->m_rect );		
#endif // __EVENTLUA_SPAWN
					}	// if
				}	// if
			}	// for
#endif	// __EVENT_0117
			pWorld	= pWorld->nextptr;
		}	// while
#ifdef __EVENT_0117
		CEventGeneric::GetInstance()->SortRegionGeneric();
#endif	// __EVENT_0117
	}	// if
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterRespawnSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	char szMonsterName[32];
	D3DXVECTOR3	vPos;
	int		nAddIndex;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;

	int nMonsterRespawnSize;
	ar >> nMonsterRespawnSize;
	for( int i = 0 ; i < nMonsterRespawnSize ; ++i )
	{
		ar >> nAddIndex;
		ar.ReadString( szMonsterName, _countof( szMonsterName ) );
		ar >> vPos;
		ar >> nQuantity;
		ar >> nAggressive;
		ar >> nRange;
		ar >> nTime;

		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );

		if( pMoverProp )
		{
			CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );

			if( pWorld == NULL )
				return;

			CRespawnInfo ri;
			ri.m_dwType = OT_MOVER;
			ri.m_dwIndex = pMoverProp->dwID;
			ri.m_nMaxcb = nQuantity;
			ri.m_nMaxAttackNum = nAggressive;
			if( pMoverProp->dwFlying != 0 )
				ri.m_vPos = vPos;
			ri.m_rect.left		= (LONG)( vPos.x - nRange );
			ri.m_rect.right		= (LONG)( vPos.x + nRange );
			ri.m_rect.top		= (LONG)( vPos.z - nRange );
			ri.m_rect.bottom	= (LONG)( vPos.z + nRange );
			ri.m_uTime = static_cast< u_short >( nTime );
			ri.m_nGMIndex = nAddIndex;

			pWorld->m_respawner.Add( ri, TRUE );
		}
	} 

	int nRemoveSize;
	int nRemoveIndex;
	ar >> nRemoveSize;
	for( int i = 0 ; i < nRemoveSize ; ++ i )
	{
		ar >> nRemoveIndex;

		CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );

		if( pWorld == NULL )
			return;
		pWorld->m_respawner.Remove( nRemoveIndex, TRUE );
	}
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterPropSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;

	int		nMonsterPropSize;

	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;

	ar >> nMonsterPropSize;
	for( int i = 0 ; i < nMonsterPropSize ; ++i )
	{
		ar.ReadString( szMonsterName, _countof( szMonsterName ) );
		ar >> nHitPoint;
		ar >> nAttackPower;
		ar >> nDefence;
		ar >> nExp;
		ar >> nItemDrop;
		ar >> nPenya;

		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = (float)nHitPoint / 100.0f;
			pMoverProp->m_fAttackPower_Rate = (float)nAttackPower / 100.0f;
			pMoverProp->m_fDefence_Rate = (float)nDefence / 100.0f;
			pMoverProp->m_fExp_Rate = (float)nExp / 100.0f;
			pMoverProp->m_fItemDrop_Rate = (float)nItemDrop / 100.0f;
			pMoverProp->m_fPenya_Rate = (float)nPenya / 100.0f;
			pMoverProp->m_bRate = TRUE;

			MONSTER_PROP MonsterProp;
			FLStrcpy( MonsterProp.szMonsterName, _countof( MonsterProp.szMonsterName ), szMonsterName );
			MonsterProp.nHitPoint = nHitPoint;
			MonsterProp.nAttackPower = nAttackPower;
			MonsterProp.nDefence = nDefence;
			MonsterProp.nExp = nExp;
			MonsterProp.nItemDrop = nItemDrop;
			MonsterProp.nPenya = nPenya;
			prj.AddMonsterProp( MonsterProp );
		}
	} 

	int	nRemovePropSize;
	ar >> nRemovePropSize;
	for( int i = 0 ; i < nRemovePropSize ; ++i )
	{
		ar.ReadString( szMonsterName, _countof( szMonsterName ) );
		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = 1.0f;
			pMoverProp->m_fAttackPower_Rate = 1.0f;
			pMoverProp->m_fDefence_Rate = 1.0f;
			pMoverProp->m_fExp_Rate = 1.0f;
			pMoverProp->m_fItemDrop_Rate = 1.0f;
			pMoverProp->m_fPenya_Rate = 1.0f;
			pMoverProp->m_bRate = FALSE;
			prj.RemoveMonsterProp( szMonsterName );
		}
	}

	g_xWSUserManager->AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM


	//neodeath OnMonsterPropSetting이 마지막 서버 세팅이므로, 기간제 알람을 가동한다.
	PERIOD_REPETITION_ALARM_MNG().Start();
}

void CDPDatabaseClient::OnCooperativeContributions_Spec( CAr & ar, DPID, DPID )
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	DWORD dwSize = 0;
	ar >> dwSize;

	BYTE * pBuffer	= new BYTE[ dwSize ];
	ar.Read( pBuffer, dwSize );
	COOPERATIVE_CONTRIBUTIONS().Load( pBuffer, dwSize );
	SAFE_DELETE_ARRAY( pBuffer );

	COOPERATIVE_CONTRIBUTIONS().Query_TryWriteRewardInfo();
	COOPERATIVE_CONTRIBUTIONS().Query_GetContributedInfo();
}



void CDPDatabaseClient::OnGMChat( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	int nSize;
	ar >> nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		ar.ReadString( prj.m_chGMChat[i], _countof( prj.m_chGMChat[i] ) );
	}
	if( 0 < nSize )
		g_xWSUserManager->AddGMChat( nSize );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::SendLogPlayDeath(CMover* pMover, CMover* pSender)
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_DEATH, DPID_UNKNOWN, DPID_UNKNOWN );

	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
	ar << pMover->GetLevel();
	ar << (int)0;	// MaxHitPower
	ar << pMover->GetMaxHitPoint();
	if( pSender )
		ar.WriteString( pSender->GetName() );
	else
		ar.WriteString( "" );
	ar << pMover->m_idPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogSkillPoint( int nAction, int nPoint, CMover* pMover, LPSKILL pSkill )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SKILLPOINTLOG, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << nAction;
	ar << pMover->m_idPlayer;
	ar << pMover->m_nSkillLevel;
	ar << pMover->m_nSkillPoint;
	if( pSkill )
	{
		ar << pSkill->dwSkill;
		ar << pSkill->dwLevel;
	}
	else
	{
		ar << (DWORD)0;
		ar << (DWORD)0;
	}
	ar << nPoint;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogExpBox( u_long idPlayer, OBJID objid, EXPINTEGER iExp, BOOL bGet )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_EXPBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << objid << iExp << bGet;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogLevelUp( CMover* pSender, int Action )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_LEVELUP, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << pSender->GetExp1();
	ar << pSender->GetFlightLv();
	ar << pSender->GetLevel();
	ar << pSender->GetJob();
	pSender->m_Stat.Serialize( ar );
	ar << pSender->GetGold();
	ar << ( (FLWSUser*)pSender )->GetPlayerTime();
	ar << Action;
	ar << pSender->m_idPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogGamemaChat( FLWSUser* pUser, LPCTSTR lpszString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_GAMEMASTER_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << pUser->m_idPlayer;
	ar.WriteString( lpszString );

	SEND( ar, this, DPID_SERVERPLAYER );

}

void CDPDatabaseClient::SendLogSMItemUse( const char *Action, FLWSUser* pUser, FLItemElem* pItemElem, PT_ITEM_SPEC pItemProp, const char* szName1 )
{
	LogItemInfo kLogItemInfo;

	FLStrcpy( kLogItemInfo.Action, _countof( kLogItemInfo.Action ), Action );
	FLStrcpy( kLogItemInfo.SendName, _countof( kLogItemInfo.SendName ), pUser->GetName() );
	FLStrcpy( kLogItemInfo.RecvName, _countof( kLogItemInfo.RecvName ), szName1[0] != '\0' ? szName1 : pUser->GetName() );

	const int nItemNum	= pItemElem ? pItemElem->m_nItemNum : 1;

	if( pItemElem == NULL && pItemProp != NULL )
	{
		FLSPrintf( kLogItemInfo.kLogItem.szItemName, _countof( kLogItemInfo.kLogItem.szItemName ), "%d", pItemProp->dwID );
	}

	g_DPSrvr.OnLogItem( kLogItemInfo, pItemElem, nItemNum );


// 	// 	Action값의 의미..
// 	// 	숫자일때..
// 	// 	1 - 유료아이템 사용.
// 	// 	2 - 유료아이템 사용 만료.
// 
// 	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ITEM, DPID_UNKNOWN, DPID_UNKNOWN );
// 	ar.WriteString( Action );
// 	ar.WriteString( pUser->GetName() );
// 	if( strlen( szName1 ) > 0 )
// 		ar.WriteString( szName1 );
// 	else 
// 		ar.WriteString( pUser->GetName() );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 	u_long idSendPlayer, idRecvPlayer;
// 	idSendPlayer = idRecvPlayer = 0;
// 
// 	TCHAR szSendName[MAX_NAME] = { 0, };
// 	TCHAR szRecvName[MAX_NAME] = { 0, };
// 
// 	FLStrcpy( szSendName, _countof( szSendName ), pUser->GetName() );
// 	if( _tcslen( szName1 ) > 0 )
// 	{
// 		FLStrcpy( szRecvName, _countof( szRecvName ), szName1 );
// 	}
// 	else
// 	{
// 		FLStrcpy( szRecvName, _countof( szRecvName ), pUser->GetName() );
// 	}
// 
// 	idSendPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( szSendName );
// 	idRecvPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( szRecvName );
// 
// 	ar << idSendPlayer;
// 	ar << idRecvPlayer;
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. // Begin
// 	// ar << pUser->GetWorld()->GetID();
// 	DWORD dwWorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
// 	ar << dwWorldId;
// 	// chipi_090623 // End
// 	ar << pUser->GetGold();
// 	ar << pUser->GetGold();
// 
// 
// 	if( pItemElem == NULL )
// 	{
// 		ar << static_cast<SERIALNUMBER>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 		char szItemId[32] = {0, };
// 		FLSPrintf( szItemId, _countof( szItemId ), "%d", pItemProp->dwID );
// 		ar.WriteString( szItemId );
// 		ar << static_cast<DWORD>( 1 );
// 		ar << static_cast<int>( 0 );
// 
// 		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
// 		ar << pUser->m_dwGoldBank[byProgramDataSlot];
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<BOOL>( 0 );
// 		ar << static_cast<DWORD>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<DWORD>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 
// 		ar << static_cast<DWORD>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 		ar << static_cast<DWORD>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 
// 		for( DWORD dwNth = 0; dwNth < MAX_RANDOMOPTION_SIZE; ++dwNth )
// 		{
// 			ar << static_cast<DWORD>( 0 );
// 			ar << static_cast<int>( 0 );
// 		}
// 	}
// 	else
// 	{
// 		ar << pItemElem->GetSerialNumber();
// 		ar << pItemElem->m_nHitPoint;
// 		ar << pItemElem->m_nRepair;
// 		//ar.WriteString( (char*)pItemElem->GetProp()->szName );
// 		char szItemId[32] = {0, };
// 		FLSPrintf( szItemId, _countof( szItemId ), "%d", pItemElem->GetProp()->dwID );
// 		ar.WriteString( szItemId );
// 		ar << static_cast<DWORD>( pItemElem->m_nItemNum );
// 		ar << pItemElem->GetAbilityOption();
// 
// 		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
// 		ar << pUser->m_dwGoldBank[byProgramDataSlot];
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( 0 );
// 		ar << static_cast<int>( pItemElem->m_byItemResist );
// 		ar << static_cast<int>( pItemElem->m_nResistAbilityOption );
// 		ar << pItemElem->m_bCharged;
// 		ar << pItemElem->m_dwKeepTime;
// 
// 		ar << pItemElem->GetGeneralPiercingSize();
// 		for( size_t i=0; i<pItemElem->GetGeneralPiercingSize(); i++ )
// 		{
// 			ar << pItemElem->GetGeneralPiercingItemID( i );
// 		}
// 
// 		ar << pItemElem->GetUltimatePiercingSize();
// 		for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
// 		{
// 			ar << pItemElem->GetUltimatePiercingItemID( i );
// 		}
// 
// 		// mirchang_100514 TransformVisPet_Log
// 		if( pItemElem->IsTransformVisPet() == TRUE )
// 		{
// 			ar << static_cast<BYTE>( 100 );
// 		}
// 		else
// 		{
// 			ar << static_cast<BYTE>( 0 );
// 		}
// 
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<DWORD>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 		ar << static_cast<WORD>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		ar << static_cast<BYTE>( 0 );
// 		// mirchang_100514 TransformVisPet_Log
// 
// 		ar << pItemElem->GetCoupleId();
// 		ar << pItemElem->GetLevelDown();
// 		ar << pItemElem->GetRandomOptionOriginID();
// 		ar << pItemElem->GetRandomOptionExtensionSize();
// 		ar << pItemElem->GetRandomOptionExtensionFlag();
// 
// 		for( DWORD dwNth = 0; dwNth < MAX_RANDOMOPTION_SIZE; ++dwNth )
// 		{
// 			ar << pItemElem->GetRandomOptionExtensionDstID( dwNth );
// 			ar << pItemElem->GetRandomOptionExtensionAdjValue( dwNth );
// 		}
// 	}
// 	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN, DPID_UNKNOWN );

	ar.WriteString( szName );
	ar.WriteString( szNewPass );
	ar << uidPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}
void	CDPDatabaseClient::SendLogGetHonorTime(CMover* pMover, int nGetHonor )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_GETHONORTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGetHonor;
	ar << pMover->m_idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption )
{
	SendLogUniqueItem2( pMover, pItem->GetProp(), nOption );
}

void CDPDatabaseClient::SendLogUniqueItem2( CMover* pMover, PT_ITEM_SPEC pItemProp, int nOption )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_UNIQUEITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
	//	if( rItemElem.m_szItemText[0] == '\0' )
	//ar.WriteString( pItemProp->szName );
	char szItemId[32] = {0, };
	FLSPrintf( szItemId, _countof( szItemId ), "%d", pItemProp->dwID );
	ar.WriteString( szItemId );
	//	else
	//		ar.WriteString( rItemElem.m_szItemText );
	ar << nOption;
	ar << pMover->m_idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSchoolReport( PSCHOOL_ENTRY pSchool )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SCHOOL_REPORT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( pSchool, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BUYING_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogPkPvp( CMover* pUser, CMover* pLose, int nPoint, char chState )
{
	if( !g_eLocal.GetState( EVE_PK ) )
		return;

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PK_PVP, DPID_UNKNOWN, DPID_UNKNOWN );

	int nSendBuf = 0;

	if( pUser == NULL || pLose == NULL )
	{
		if( pUser == NULL )
			nSendBuf = 1;
		else
			nSendBuf = 2;
	}
	else
	{
		nSendBuf = 3;
	}

	ar << nSendBuf;
	ar << nPoint;
	ar << chState;

	if( nSendBuf == 1 || nSendBuf == 3 )
	{
		ar << pLose->m_idPlayer;
		ar << pLose->GetWorld()->GetID();
		ar << pLose->GetLevel();
		ar << pLose->GetPos();
		ar << pLose->m_nFame;
		ar << pLose->m_nPKValue;
		ar << pLose->m_dwPKPropensity;
	}

	if( nSendBuf == 2 || nSendBuf == 3 )
	{
		ar << pUser->m_idPlayer;
		ar << pUser->GetWorld()->GetID();
		ar << pUser->GetLevel();
		ar << pUser->GetPos();
		ar << pUser->m_nFame;
		ar << pUser->m_nPKValue;
		ar << pUser->m_dwPKPropensity;
	}

	SEND( ar, this, DPID_SERVERPLAYER );		
}

void CDPDatabaseClient::SendLogSchool( u_long idPlayer, LPCTSTR szName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_SCHOOL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szName );
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPDatabaseClient::SendGuildGetPay( u_long idGuild, DWORD nGoldGuild, DWORD dwPay )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nGoldGuild << dwPay;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info, BYTE nLevelUp, LONG nMemberLevel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WD_GUILD_CONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << info; 
	ar << nLevelUp;	// 레벨업 1 : 0( 했으면 1 )
	ar << nMemberLevel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGuildBank( DWORD dwWorldServerKey )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_BANK_QUERY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwWorldServerKey;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::UpdateGuildRanking()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << static_cast<int>(1); // 월드 서버에서 요청
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::UpdateGuildRankingUpdate()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING_DB, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << static_cast<int>(1); // 월드 서버에서 요청
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnGuildBank( CAr & ar, DPID, DPID )
{
	int nCount;

	// 1. 몇개의 길드 뱅크 정보가 들어왔는지 Serialize한다.
	ar >> nCount;

	if( nCount <= 0 )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ GUILD BANK DATA NOT EXIST ]" ) );
	}
	else
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ RECEIVE GUILD BANK DATA. COUNT:(%d) ]" ), nCount );
	}

	// 2. 루프를 돌며 길드창고 정보를 Serialize한다.
	for (int i=0; i<nCount; ++i)
	{
		int nGuildId = 0, nGoldGuild = 0;

		// 2-1 길드 핸들값을 받는다.
		ar >> nGuildId;
		ar >> nGoldGuild;

		// 2-2 핸들값으로 길드매니져에서 길드 정보를 포인트하여 창고정보를 Serialize한다.
		CGuild* pGuild = g_GuildMng.GetGuild( nGuildId );
		if( pGuild )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
			pGuild->m_GuildBank.Deserialize( ar );

			/*
			//////////////////////////////////////////////////////////////////////////
			if( pGuild->m_GuildBank.m_bAllocedSerialNumber == FALSE )
			{
			pGuild->m_GuildBank.ReallocSerialNumber();
			g_DPSrvr.UpdateGuildBank( pGuild, GUILD_PUT_ITEM );	// GUILD_PUT_ITEM 임시
			}
			//////////////////////////////////////////////////////////////////////////
			*/

			if( pGuild->m_GuildBank.GetAllocedSerialNumber() == TRUE )
			{
				pGuild->m_GuildBank.InsertAllItem();
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ GUILD NOT FOUND. GUILD_ID(%06d) ]" ), nGuildId );
			// 임시로 템프변수 만들어서 받기 banktmp
			CGuild tempGuild;
			tempGuild.m_GuildBank.Deserialize( ar );
		}
	}
}

void CDPDatabaseClient::OnUpdateGuildRankingFinish( CAr & ar, DPID, DPID )
{
	CGuildRank::Instance()->Serialize( ar );
}


void CDPDatabaseClient::SendQueryGuildQuest( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendInsertGuildQuest( u_long idGuild, int nId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSERTGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendUpdateGuildQuest( u_long idGuild, int nId, int nState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATEGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nId << nState;	
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnQueryGuildQuest( CAr & ar, DPID, DPID )
{
	int nCount;
	u_long idGuild;
	CGuild* pGuild;

	ar >> nCount;

	for( int i = 0; i < nCount; i++ )
	{
		ar >> idGuild;
		pGuild	= g_GuildMng.GetGuild( idGuild );	
		if( pGuild )
		{
			ar >> pGuild->m_nQuestSize;
			ar.Read( (void*)pGuild->m_aQuest, sizeof(GUILDQUEST) * pGuild->m_nQuestSize );
		}
		else
		{
			CGuild waste;
			ar >> waste.m_nQuestSize;
			ar.Read( (void*)waste.m_aQuest, sizeof(GUILDQUEST) * waste.m_nQuestSize );
		}
	}
}

void CDPDatabaseClient::SendQuerySetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwData;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TRANS : is not alive" ) );


		if( ConnectToServer( g_szDBAddr, PN_DBSRVR_1, FLNormalProtocol::GetInstance(), TRUE ) == FALSE )
		{
			return;
		}

// #ifdef	__MAINSERVER
// 		FLCommonDumper kDumper( _T( "Assert_" ) );
// 		FLDumpRunner::DumpNow( &kDumper, FLDUMP_LEVEL_HEAVY, 0, _T( "TRANS : is not alive" ), _T( __FUNCTION__ ), __T( __FILE__ ), __LINE__ );
// 		::PostQuitMessage( 0 );
// #endif
	}

	m_bAlive	= FALSE;
	BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << time_null();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPing( CAr & ar, DPID, DPID )
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

#if !defined(_DEBUG)
	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );

	if( d >  10 )
		FLERROR_LOG( PROGRAM_NAME, _T( "ping TRANS - %d, trans:%s" ), d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif 

	m_bAlive	= TRUE;
}

void CDPDatabaseClient::SendQueryMailBox( u_long idReceiver )
{
	// 	//	BEGINTEST
	// 	FLERROR_LOG( PROGRAM_NAME, _T( "[%d]" ), idReceiver );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryMailBoxReq( u_long idReceiver )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryMailBoxCount( u_long idReceiver, int nCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_COUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnMailBox( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	ar >> idReceiver;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	if( pMailBox )
	{
		// 		//	BEGINTEST
		// 		FLERROR_LOG( PROGRAM_NAME, _T( "[%d]" ), idReceiver );

		pMailBox->Read( ar );	// pMailBox->m_nStatus	= CMailBox::data;
		FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			// 			//	BEGINTEST
			// 			FLERROR_LOG( PROGRAM_NAME, _T( "AddMailBox [%d]" ), idReceiver );

			pUser->AddMailBox( pMailBox );
			pUser->ResetCheckClientReq();
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GetMailBox - pMailBox is NULL. idReceiver : %d" ), idReceiver );
	}
}

void CDPDatabaseClient::OnMailBoxReq( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	BOOL bHaveMailBox = FALSE;
	ar >> idReceiver >> bHaveMailBox;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		pUser->CheckTransMailBox( TRUE );
		pUser->SendCheckMailBoxReq( bHaveMailBox );
	}

	if( bHaveMailBox == TRUE )
	{
		CMailBox* pMailBox	= NULL;
		pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

		if( pMailBox == NULL )
		{
			CMailBox* pNewMailBox = new CMailBox( idReceiver );
			if( pNewMailBox != NULL )
			{
				if( CPost::GetInstance()->AddMailBox( pNewMailBox ) == TRUE )
				{
					pNewMailBox->ReadReq( ar );	// pMailBox->m_nStatus	= CMailBox::data;

					FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
					if( IsValidObj( pUser ) )
					{
						pUser->AddMailBox( pNewMailBox );
						pUser->ResetCheckClientReq();
					}
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "AddMailBox Failed. idReceiver : %d" ), idReceiver );
					SAFE_DELETE( pNewMailBox );
				}
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "MailBox Create Failed" ) );
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//	??????????
			FLERROR_LOG( PROGRAM_NAME, _T( "pMailBox is NOT NULL. idReceiver : %d" ), idReceiver );
		}
	}
	else
	{
		FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->ResetCheckClientReq();
		}
	}
}

void CDPDatabaseClient::SendQueryPostMail( u_long idReceiver, u_long idSender, FLItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText )
{
	// 	//	BEGINTEST
	// 	FLERROR_LOG( PROGRAM_NAME, _T( "Receiver[%d] Sender[%d]" ), idReceiver, idSender );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << idSender;
	if( FALSE == itemElem.IsEmpty() )
	{
		ar << (BYTE)1;
		itemElem.Serialize( ar );
	}
	else
	{
		ar << (BYTE)0;
	}
	ar << nGold;
	ar.WriteString( lpszTitle );
	ar.WriteString( lpszText );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryRemoveMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetMailItem( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetMailGold( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryReadMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPostMail( CAr & ar, DPID, DPID )
{
	// 	//	BEGINTEST
	// 	FLERROR_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient::OnPostMail" ) );

	BOOL	bResult;
	u_long idReceiver;
	ar >> bResult >> idReceiver;
	CMail* pMail	= new CMail;
	pMail->Serialize( ar );

	FLWSUser* pSenderUser	= (FLWSUser*)prj.GetUserByID( pMail->m_idSender );

	if( TRUE == bResult )
	{
		if( CPost::GetInstance()->AddMail( idReceiver, pMail ) <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "pMail->m_nMail : %d" ), pMail->m_nMail );
			SAFE_DELETE( pMail );
		}
		else	// SUCCESS
		{
			if( IsValidObj( pSenderUser) == TRUE )
			{
				TCHAR szMessage[128] = {0, };
				FLStrcpy( szMessage, _countof( szMessage ), prj.GetText(TID_MAIL_SEND_OK) );
				pSenderUser->AddDiagText( szMessage );
			}

			FLWSUser* pReceiverUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pReceiverUser ) == TRUE )
			{
				pReceiverUser->SetMode( MODE_MAILBOX );
				g_xWSUserManager->AddModifyMode( pReceiverUser );
			}
		}
	}
	else	// FAIL
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Send Mail Failed. idSender : %d, idReceiver : %d" ), pMail->m_idSender, idReceiver );

		if( IsValidObj( pSenderUser ) == TRUE )
		{
			if( pMail->m_pItemElem != NULL )
			{
				pSenderUser->CreateItem( pMail->m_pItemElem );
			}

			if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
			{
				pSenderUser->AddGold( 100 + pMail->m_nGold, TRUE );
			}
			else
			{
				pSenderUser->AddGold( 500 + pMail->m_nGold, TRUE );
			}

			pSenderUser->AddDiagText( prj.GetText(TID_GAME_POST_ERROR) );
			CWorld* pWorld	= pSenderUser->GetWorld();
			if( pWorld != NULL )
			{
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pSenderUser, pWorld->GetID(), pSenderUser->GetPos(), pSenderUser->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pSenderUser, pWorld->GetID(), pSenderUser->GetPos() );
#endif	// __LAYER_1015
			}
		}
		else
		{
			// 康
		}
		SAFE_DELETE( pMail );
	}
}

void CDPDatabaseClient::OnRemoveMail( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

	if( pMailBox )
	{
		pMailBox->RemoveMail( nMail );

		FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->AddRemoveMail( nMail, CMail::mail );
			if( pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode( MODE_MAILBOX ) )
			{
				pUser->SetNotMode( MODE_MAILBOX );
				g_xWSUserManager->AddModifyMode( pUser );
			}
		}
	}
}

void CDPDatabaseClient::OnGetMailItem( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	FLItemElem	itemRestore;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_pItemElem )
		{
			itemRestore		= *( pMail->m_pItemElem );
			FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti== uQuery )		// 편지 받기 요청 주체면 아이템 생성, 우편 아이템 제거
				{
					if( pUser->CreateItem( pMail->m_pItemElem ) )
					{
						pUser->AddRemoveMail( nMail, CMail::item );
						itemRestore.Empty();
					}
					else
					{
						FLERROR_LOG( _T( "Mail_Fail" ), _T( "CreateItem() failed. Multi: %u, idReceiver: %u, Receiver: %s, Mail: %u, Item Serial: %u, Index: %u, Count: %u" ),
							uQuery, idReceiver, pUser->GetName(), nMail, itemRestore.GetSerialNumber(), itemRestore.m_dwItemId, itemRestore.m_nItemNum );
						itemRestore.Empty();
					}
				}
				else	// 우편 아이템 받기 요청 주체가 아니면, 우편 아이템 제거
				{
					pUser->AddRemoveMail( nMail, CMail::item );
					itemRestore.Empty();
				}

				CWorld * pWorld = pUser->GetWorld();
				if( pWorld != NULL )
				{
#ifdef __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
				}

			}
			else
			{
				if( g_uIdofMulti == uQuery )
				{
					FLERROR_LOG( _T( "Mail_Fail" ), _T( "IsValidObj() failed. Multi: %u, idReceiver: %u, Mail: %u, Item Serial: %u, Index: %u, Count: %u" ),
						uQuery, idReceiver, nMail, itemRestore.GetSerialNumber(), itemRestore.m_dwItemId, itemRestore.m_nItemNum );
				}

				itemRestore.Empty();
			}
		}
		pMailBox->RemoveMailItem( nMail );
	}

	if( !itemRestore.IsEmpty() && g_uIdofMulti== uQuery )
	{
		//g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemRestore, 0, "", "" );

		FLERROR_LOG( _T( "Mail_Fail" ), _T( "Unkonwn failed. Multi: %u, idReceiver: %u, Mail: %u, Item Serial: %u, Index: %u, Count: %u" ),
			uQuery, idReceiver, nMail, itemRestore.GetSerialNumber(), itemRestore.m_dwItemId, itemRestore.m_nItemNum );
	}
}

/*
void CDPDatabaseClient::OnGetMailGold( CAr & ar, DPID, DPID )
{
u_long idReceiver, nMail, uQuery;
ar >> idReceiver >> nMail >> uQuery;

CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
if( pMailBox == NULL)		
return;

CMail* pMail	= pMailBox->GetMail( nMail );
if( pMail == NULL )	
return;

// DWORD -> int
int nMailGold	= pMail->m_nGold;
if( nMailGold < 0 )
return;


	FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
	if( IsValidObj( pUser ) == false )
	{
		//restore 
		if( nMailGold > 0 && g_uIdofMulti == uQuery )
		{
			FLItemElem itemElem;		// empty item
			g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nMailGold, "", "" );
		}
	}

	if( g_uIdofMulti == uQuery )
	{
		if( pUser->CheckUserGold( nMailGold, true ) == true )
		{
			pUser->AddGold( pMail->m_nGold );
			pUser->AddRemoveMail( nMail, CMail::gold );
		}
		else
		{
			//restore 
			if( nMailGold > 0 && g_uIdofMulti == uQuery )
			{
				FLItemElem itemElem;		// empty item
				g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nMailGold, "", "" );
			}
		}
	}
	else
	{
		pUser->AddRemoveMail( nMail, CMail::gold );
	}
}
else
{
//restore 
if( nMailGold > 0 && g_uIdofMulti == uQuery )
{
CItemElem itemElem;		// empty item
g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nMailGold, "", "" );
}
}
}
else
{
pUser->AddRemoveMail( nMail, CMail::gold );
}
}
*/


void CDPDatabaseClient::OnGetMailGold( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	int nRestore	= 0;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_nGold <= INT_MAX )
		{
			nRestore	= pMail->m_nGold;
			FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti == uQuery )
				{
					if( CanAdd( pUser->GetGold(), pMail->m_nGold ) )
					{
						pUser->AddGold( pMail->m_nGold );
						pUser->AddRemoveMail( nMail, CMail::gold );
						nRestore	= 0;
					}
					else
					{
						FLERROR_LOG( _T( "Mail_Fail" ), _T( "CanAdd() failed. Multi: %u, idReceiver: %u, Receiver: %s, Mail: %u, Gold: %u" ),
							uQuery, idReceiver, pUser->GetName(), nMail, pMail->m_nGold );

						pUser->AddRemoveMail( nMail, CMail::gold );
						nRestore	= 0;
					}
				}
				else
				{
					pUser->AddRemoveMail( nMail, CMail::gold );
					nRestore	= 0;
				}

				CWorld * pWorld = pUser->GetWorld();
				if( pWorld != NULL )
				{
#ifdef __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
				}
			}
			else
			{
				if( g_uIdofMulti == uQuery )
				{
					FLERROR_LOG( _T( "Mail_Fail" ), _T( "IsValidObj() failed. Multi: %u, idReceiver: %u, Mail: %u, Gold: %u" ), uQuery, idReceiver, nMail, pMail->m_nGold );
				}

				nRestore	= 0;
			}
		}
		pMailBox->RemoveMailGold( nMail );
	}

	if( nRestore > 0 && g_uIdofMulti == uQuery )
	{
		//FLItemElem itemElem;		// empty item
		//g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nRestore, "", "" );

		FLERROR_LOG( _T( "Mail_Fail" ), _T( "Unkonwn failed. Multi: %u, idReceiver: %u, Mail: %u, Gold: %u" ), uQuery, idReceiver, nMail, nRestore );
	}
}

void CDPDatabaseClient::OnReadMail( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

	if( pMailBox )
	{
		pMailBox->ReadMail( nMail );
		FLWSUser* pUser	= (FLWSUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->ResetCheckClientReq();
			pUser->AddRemoveMail( nMail, CMail::read );
			if( pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode( MODE_MAILBOX ) )
			{
				pUser->SetNotMode( MODE_MAILBOX );
				g_xWSUserManager->AddModifyMode( pUser );
			}
		}
	}
}

void CDPDatabaseClient::OnAllMail( CAr & ar , DPID, DPID )
{
	if( g_uKey == 101 )
	{
		SendITEM_TBL_Update();
	}

	CPost::GetInstance()->Serialize( ar, FALSE );
}

void CDPDatabaseClient::OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == 0 )
		return;

	int nNo;
	u_long idGuild;
	DWORD dwItemId;
	SERIALNUMBER iSerialNumber;
	DWORD dwItemNum;
	ar >> nNo >> idGuild >> dwItemId >> iSerialNumber >> dwItemNum;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	DWORD dwRemoved	= 0;
	if( pGuild )
	{
		if( dwItemId == 0 )		// penya
		{
			dwRemoved	= dwItemNum;
			if( pGuild->m_nGoldGuild - dwRemoved > pGuild->m_nGoldGuild )	// overflow
				dwRemoved	= pGuild->m_nGoldGuild;
			pGuild->m_nGoldGuild	-= dwRemoved;
			if( dwRemoved > 0 )
			{
				g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );
				FLWSUser* pUsertmp;
				CGuildMember* pMember;
				for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
				{
					pMember		= i->second;
					pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
						pUsertmp->AddRemoveGuildBankItem( idGuild,  NULL_ID, dwRemoved );
				}
			}
		}
		else	// item
		{
			FLItemElem* pItemElem	= pGuild->GetItem( dwItemId, iSerialNumber );
			if( pItemElem )
			{
				DWORD dwId	= pItemElem->m_dwObjId;
				if( static_cast<short>( dwItemNum ) < 0 )
					dwItemNum	= 0;

				dwRemoved	= (DWORD)pGuild->RemoveItem( dwId, static_cast<int>( dwItemNum ) );
				if( dwRemoved > 0 )
				{
					g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );

					FLTRACE_LOG( PROGRAM_NAME, _T( "WORLD: OnQueryRemoveGuildBankTbl: dwId = %d, nRemoved=%d" ), dwId, dwRemoved );

					FLWSUser* pUsertmp;
					CGuildMember* pMember;
					for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					{
						pMember		= i->second;
						pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
						if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
							pUsertmp->AddRemoveGuildBankItem( idGuild,  dwId, dwRemoved );
					}
				}
			}
		}
	}
	SendQueryRemoveGuildBankTbl( nNo, dwRemoved );
}

void CDPDatabaseClient::SendQueryRemoveGuildBankTbl( int nNo, DWORD dwRemoved )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nNo << dwRemoved;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnEventGeneric( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	CEventGeneric::GetInstance()->Serialize( ar );
	ar >> dwFlag;
	list<PEVENT_GENERIC>* pList	= CEventGeneric::GetInstance()->GetEventList();
	for( list<PEVENT_GENERIC>::iterator i = pList->begin(); i != pList->end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;

		// 康
		FLTRACE_LOG( PROGRAM_NAME, _T( "dwFlag=0x%08x, nId=%d, nFlag=%d" ), dwFlag, pEvent->nId, pEvent->nFlag );

		if( dwFlag & pEvent->nFlag )
			g_eLocal.SetState( pEvent->nId, 1 );
	}
}

void CDPDatabaseClient::OnEventFlag( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	ar >> dwFlag;

	// 康
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnEventFlag: dwFlag=0x%08x" ), dwFlag );

	list<PEVENT_GENERIC>* pList	= CEventGeneric::GetInstance()->GetEventList();
	for( list<PEVENT_GENERIC>::iterator i = pList->begin(); i != pList->end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		FLTRACE_LOG( PROGRAM_NAME, _T( "OnEventFlag: nId=%d, nFlag=%d" ), pEvent->nId, pEvent->nFlag );

		if( dwFlag & pEvent->nFlag )
		{
			if( g_eLocal.GetState( pEvent->nId ) == 0 )
			{
				if( g_eLocal.SetState( pEvent->nId, 1 ) )
				{
					g_xWSUserManager->AddSetLocalEvent( static_cast< short >( pEvent->nId ), 1 );
					FLERROR_LOG( PROGRAM_NAME, _T( "event: %d: 1" ), pEvent->nId );
				}
			}
		}
		else
		{
			if( g_eLocal.GetState( pEvent->nId ) == 1 )
			{
				if( g_eLocal.ClearEvent( pEvent->nId ) )
				{
					g_xWSUserManager->AddSetLocalEvent( static_cast< short >( pEvent->nId ), 0 );
					FLERROR_LOG( PROGRAM_NAME, _T( "OnEvent: %d: 0" ), pEvent->nId );
				}
			}
		}
	}
}

void CDPDatabaseClient::OnEventStateLua( CAr & ar, DPID, DPID )
{
	BOOL bTextOut = FALSE;
	int nMapSize = 0;
	ar >> bTextOut;
	ar >> nMapSize;

	BYTE nId;
	BOOL bState;
	for( int i=0; i<nMapSize; i++ )
	{
		ar >> nId;
		ar >> bState;
		prj.m_EventLua.SetState( nId, bState );
		if( bTextOut )
			g_xWSUserManager->AddEventLua( nId, bState );
	}
	prj.m_EventLua.PrepareProxy();
}

void CDPDatabaseClient::OnEventLuaChanged( CAr & /*ar*/, DPID, DPID )
{
	prj.m_EventLua.LoadScript();
	if( prj.m_EventLua.m_bRun )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "Event.lua 재실행 완료!!!" ) );
	}
}

void CDPDatabaseClient::OnGC1to1Open( CAr & /*ar*/, DPID, DPID )
{
	g_GuildCombat1to1Mng.GuildCombat1to1Open();
}

void CDPDatabaseClient::OnGC1to1TenderGuildFromDB( CAr & ar, DPID, DPID )
{
	int nVecGuildSize = 0, nVecMemberSize = 0;
	u_long uGuildId = 0;
	int nPenya = 0;
	u_long uPlayerId = NULL_PLAYER_ID;

	CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1Tender;

	g_GuildCombat1to1Mng.m_vecTenderGuild.clear();
	ar >> nVecGuildSize;
	for( int i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderGuild.push_back( gc1to1Tender );
		ar >> nVecMemberSize;
		for( int j=0; j<nVecMemberSize; j++ )
		{
			ar >> uPlayerId;
			if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
				g_GuildCombat1to1Mng.m_vecTenderGuild.at( i ).vecMemberId.push_back( uPlayerId );
		}
	}

	g_GuildCombat1to1Mng.m_vecTenderFailGuild.clear();
	ar >> nVecGuildSize;
	for( int i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderFailGuild.push_back( gc1to1Tender );
	}
}

void CDPDatabaseClient::CalluspLoggingQuest( u_long idPlayer, int nQuest, int nState, const char* pszComment )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_USPLOGGINGQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nQuest << nState;
	ar.WriteString( pszComment );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::CalluspXXXMultiServer( u_long uKey, FLWSUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_XXX_MULTI_SERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	if( pUser )
	{
		ar << uKey << pUser->m_idPlayer;
		ar.WriteString( pUser->m_playAccount.lpszAccount );
	}
	else
	{
		ar << uKey << 0;
		ar.WriteString( "" );	// not used
	}
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::CalluspPetLog( u_long idPlayer, SERIALNUMBER iSerial, DWORD dwData, int nType, CPet* pPet )
{
	if( pPet == NULL )
		return;
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_USP_PET_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iSerial << dwData << nType;
	pPet->Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendEventLuaChanged( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1StateToDBSrvr( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_STATETODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_GuildCombat1to1Mng.m_nState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1Tender( char cGU, u_long uGuildId, int nPenya, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_TENDERTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << cGU << uGuildId << nPenya << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1LineUp( u_long uGuildId, std::vector<u_long>& vecMemberId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_LINEUPTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	ar << vecMemberId.size();
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
		ar << vecMemberId.at( i );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1WarPerson( u_long uGuildId, u_long uIdPlayer, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_WARPERSONTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId << uIdPlayer << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1WarGuild( DWORD dwWorldId, u_long uGuildId_0, u_long uGuildId_1, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_WARGUILDTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwWorldId << uGuildId_0 << uGuildId_1 << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPDatabaseClient::SendQueryGetGuildBankLogList( u_long idReceiver, DWORD idGuild, BYTE byListType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << idGuild << byListType << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnGuildBankLogViewFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	BYTE	byListType = 0;
	ar >> nCount;
	ar >> byListType;	
	ar >> idReceiver;	

	if( nCount > GUILDBANKLOGVIEW_MAX )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__GUILDBANKLOG_ENTRY	logs[GUILDBANKLOGVIEW_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar.ReadString( logs[i].szPlayer, _countof( logs[i].szPlayer ) );		// 
			ar >> logs[i].nDate;					// 
			ar >> logs[i].nItemID;					// 
			ar >> logs[i].nItemAbilityOption;					// 
			ar >> logs[i].nItemCount;					// 
		}
		pUser->AddGuildBankLogView( byListType, (short)nCount, logs);
	}
}

void CDPDatabaseClient::SendQueryGetSealChar( u_long idReceiver ,const char* szAccount)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHAR_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetSealCharConm( u_long idReceiver )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARCONM_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSealCharFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	ar >> nCount;
	ar >> idReceiver;	

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		bool	bAbleSealChar	= false;

		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, _countof( seals[i].szPlayer ) );		// 
		}

		//////////////////////////////////////////////////////////////////////////
		if( nCount == 1 && seals[0].idPlayer != idReceiver )
		{
			bAbleSealChar	= true;
		}
		//////////////////////////////////////////////////////////////////////////

		if( nCount > 1 || bAbleSealChar == true )
		{
			pUser->AddSealChar( static_cast<short>( nCount ), seals);
		}
		else
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
		}
	}
}

void CDPDatabaseClient::SendQueryGetSealCharGet( u_long idReceiver ,const char* szAccount,DWORD dwGetId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARGET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << dwGetId;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendQueryGetSealCharSet( u_long idReceiver ,const char* szAccount,LONG lSetPlayerSlot,DWORD dwSetPlayerId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARSET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << lSetPlayerSlot;
	ar << dwSetPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSealCharGetFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	
	DWORD dwGetID;
	ar >> nCount;
	ar >> idReceiver;
	ar >> dwGetID;

	if( nCount > MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nCount is Invalid. PlayerId:[%d]" ) );
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, _countof( seals[i].szPlayer ) );		// 
		}

		if( nCount >= MAX_CHARACTER_SLOT || nCount < 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		}
		else
		{
			if( pUser->IsUsableState( dwGetID ) == FALSE )
				return;

			FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwGetID );
			if( IsUsableItem( pItemElem ) )
			{
				if(pItemElem->m_dwItemId != ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
					return;

				LONG	lSetSlot = -1;

				if( nCount == 1 )
				{
					if(seals[0].nPlayerSlot == 0)
						lSetSlot = 1;
					else if(seals[0].nPlayerSlot == 1 || seals[0].nPlayerSlot == 2 )
						lSetSlot = 0;
				}
				else if(nCount == 2)
				{
					if(seals[0].nPlayerSlot == 0)
					{
						if(seals[1].nPlayerSlot == 1)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 1;
					}
					else if(seals[0].nPlayerSlot == 1 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 0;
					}
					else if(seals[0].nPlayerSlot == 2 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 1;
						else if(seals[1].nPlayerSlot == 1)
							lSetSlot = 0;
					}
				}
				g_dpDBClient.SendQueryGetSealCharSet( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount,lSetSlot,pItemElem->m_nHitPoint);
				pUser->AddSealCharSet();

				PT_ITEM_SPEC pItemProp  = pItemElem->GetProp();

				g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );		
				//				pItemElem->UseItem();
				OBJID       dwTmpObjId = pItemElem->m_dwObjId;
				pUser->RemoveItem( dwTmpObjId, 1 );
				//		pUser->UpdateItem( pItemElemtmp->m_dwObjId, UI_NUM, pItemElemtmp->m_nItemNum );
				//		g_xWSUserManager->DestroyPlayer( pUser );
				//		g_xWSUserManager->RemoveUser( pUser->m_dwSerial );
				//		g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, pUser->m_idPlayer );
				g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
			}
		}
	}
}

void CDPDatabaseClient::OnElectionAddDeposit( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	__int64 iDeposit;
	time_t tCreate;
	BOOL bRet;
	ar >> idPlayer >> iDeposit >> tCreate >> bRet;

	FLWSUser* pUser	= static_cast<FLWSUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->AddDeposit( idPlayer, iDeposit, tCreate );
	}
	else
	{
		// *Restore
		if( IsValidObj( pUser ) )
			pUser->AddGold( static_cast<int>( iDeposit ) );
	}
}

void CDPDatabaseClient::OnElectionSetPledge( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	char szPledge[CCandidate::nMaxPledgeLen]	= {0, };
	BOOL bRet;
	ar >> idPlayer;
	ar.ReadString( szPledge, _countof( szPledge ) );
	ar >> bRet;

	FLWSUser* pUser	= static_cast<FLWSUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	IElection* pElection	= CSLord::Instance()->GetElection();
	if( bRet )
	{
		pElection->SetPledge( idPlayer, szPledge );
	}
	else
	{
		// *Restore
		if( pElection->HasPledge( idPlayer ) )
		{
			if( IsValidObj( pUser ) )
				pUser->AddGold( static_cast<int>( CCandidate::nSetPledgeCost ) );
		}
	}
}

void CDPDatabaseClient::OnElectionIncVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer, idElector;
	BOOL bRet;
	ar >> idPlayer >> idElector >> bRet;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idElector );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->IncVote( idPlayer, idElector );
		if( IsValidObj( pUser ) )	//
		{
			pUser->SetElection( pElection->GetId() );
			// 투표 참여 버프
			PT_ITEM_SPEC pProp		= g_xSpecManager->GetSpecItem( ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) );
			if( pProp != NULL )
			{
				g_xApplyItemEffect->DoApplyEffect( pUser, pUser, pProp );
			}
		}
	}
	else
	{
		// *print
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( TID_GAME_ELECTION_INC_VOTE_E004 );	// 이미 투표에 참여하셨습니다.
	}
}

void CDPDatabaseClient::OnElectionBeginCandidacy( CAr & /*ar*/, DPID, DPID )
{
	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginCandidacy();
}

void CDPDatabaseClient::OnElectionBeginVote( CAr & ar, DPID, DPID )
{
	int nRequirement;
	ar >> nRequirement;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginVote( nRequirement );
}

void CDPDatabaseClient::OnElectionEndVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->EndVote( idPlayer );
}

void CDPDatabaseClient::OnLord( CAr & ar, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLord" ) );
	CSLord::Instance()->Serialize( ar );
}

void CDPDatabaseClient::OnLEventCreate( CAr & ar, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLEventCreate" ) );
	CLEComponent* pComponent	= new CLEComponent;
	pComponent->Serialize( ar );
	BOOL bResult;
	ar >> bResult;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( pComponent->GetIdPlayer() );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bResult )
	{
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
		pEvent->AddComponent( pComponent );
	}
	else
		SAFE_DELETE( pComponent );
}

void CDPDatabaseClient::OnLEventInitialize( CAr & /*ar*/, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLEventInitialize" ) );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->Initialize();
}

void CDPDatabaseClient::OnLordSkillUse( CAr & ar, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLordSkillUse" ) );
	u_long idPlayer, idTarget;
	int nSkill, nRet;
	ar >> idPlayer >> idTarget >> nSkill >> nRet;

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( !nRet )
	{
		CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent	= pSkills->GetSkill( nSkill );
		pComponent->Execute( idPlayer, idTarget, NULL );
		if( IsValidObj( pUser ) )
			g_xWSUserManager->AddLordSkillUse( pUser, idTarget, nSkill );
	}
	else
	{
		FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( nRet );
	}
}

void CDPDatabaseClient::OnLordSkillTick( CAr & ar, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLordSkillTick" ) );

	CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
	pSkills->SerializeTick( ar );
	if( CSLord::Instance()->Get() == NULL_ID )
		return;
	FLWSUser* pLord	= g_xWSUserManager->GetUserByPlayerID( CSLord::Instance()->Get() );
	if( IsValidObj( pLord ) )
		pLord->AddLordSkillTick( pSkills );
}

void CDPDatabaseClient::OnLEventTick( CAr & ar, DPID, DPID )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPDatabaseClient.OnLEventTick" ) );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->SerializeTick( ar );
	g_xWSUserManager->AddLEventTick( pEvent );
	pEvent->EraseExpiredComponents();
}

void CDPDatabaseClient::SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iDeposit;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionSetPledge( u_long idPlayer, const char* szPledge )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPledge );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionIncVote( u_long idPlayer, u_long idElector )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idElector;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLEventCreate( u_long idPlayer, int iEEvent, int iIEvent )
{
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iEEvent << iIEvent;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idTarget << nSkill;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// operator commands
void CDPDatabaseClient::SendLEventInitialize( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionProcess( BOOL bRun )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_PROCESS, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bRun;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginCandidacy( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginVote( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginEndVote( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomWinGuild( BYTE nCont, DWORD dwGuildId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_SECRETROOM_WINNER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont << dwGuildId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLord( DWORD dwIdPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_LORD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwIdPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_TAXRATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont;
	ar << nSalesTaxRate << nPurchaseTaxRate;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendApplyTaxRateNow()
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_APPLY_TAXRATE_NOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPDatabaseClient::SendUpdateTaxRate( const int nTaxRate )
{
#ifdef __INTERNALSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_TAX_RATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nTaxRate;
	SEND( ar, this, DPID_SERVERPLAYER );
#else
	UNREFERENCED_PARAMETER( nTaxRate );
#endif // __INTERNALSERVER
}

void CDPDatabaseClient::OnTaxInfo( CAr & ar, DPID, DPID )
{
	CTax::GetInstance()->Serialize( ar );

	BOOL bConnect, bToAllClient;
	ar >> bConnect;
	if( bConnect )
	{
		ar >> CTax::GetInstance()->m_nMinTaxRate;
		ar >> CTax::GetInstance()->m_nMaxTaxRate;
		size_t nSize;
		ar >> nSize;
		for( size_t i=0; i<nSize; i++ )
		{
			DWORD dwItemId;
			ar >> dwItemId;
			CTax::GetInstance()->m_vecdwOccupationShopItem.push_back( dwItemId );
		}
	}
	ar >> bToAllClient;
	if( bToAllClient )	// 세율 변경 시점
	{
		g_xWSUserManager->AddTaxInfo();
		if( g_eLocal.GetState( EVE_SECRETROOM ) )
			g_DPCoreClient.SendCaption( GETTEXT(TID_GAME_TAX_CHANGENEXT) );

		// 사냥터용 비밀의 방에 있는 놈들을 쫓아낸다.
		CWorld* pWorld = g_WorldMng.GetWorld( WI_DUNGEON_SECRET_0 );
		if( !pWorld )
			return;

		CObj* pObj;
		for( int i=0; i<(int)( pWorld->m_dwObjNum ); i++ )
		{
			pObj	= pWorld->m_apObject[i];
			if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && !pObj->IsDelete() )
				((FLWSUser*)pObj)->REPLACE( g_uIdofMulti, ((FLWSUser*)pObj)->m_idMarkingWorld, ((FLWSUser*)pObj)->m_vMarkingPos, REPLACE_NORMAL, nTempLayer );
		}
	}
}

void CDPDatabaseClient::SendAddTax( BYTE nCont, int nTax, BYTE nTaxKind )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_ADDTAX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont << nTax << nTaxKind;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomInsertToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId << srTender.nPenya;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomUpdateToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId << srTender.nPenya << chState;
	if( chState == 'W' || chState == 'L' )
		ar << srTender.dwWorldId << srTender.nWarState << srTender.nKillCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomInsertLineUpToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId;
	ar << srTender.vecLineUpMember.size();
	for( int i=0; i<(int)( srTender.vecLineUpMember.size() ); i++ )
		ar << srTender.vecLineUpMember[i];
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomClosed()
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_CLOSED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSecretRoomInfoClear( CAr & /*ar*/, DPID, DPID )
{
	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	std::map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.begin();
	for( ; it!=pSRMng->m_mapSecretRoomContinent.end(); ++it )
	{
		CSecretRoomContinent* pSRCont = it->second;
		pSRCont->m_vecSecretRoomTender.clear();
	}
}

void CDPDatabaseClient::OnSecretRoomTenderInfo( CAr & ar, DPID, DPID )
{
	BYTE nContinent; DWORD dwGuildId, dwMemberId; int nPenya, nSize;
	std::vector<DWORD> vecMemberId;
	ar >> nContinent >> dwGuildId >> nPenya;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwMemberId;
		vecMemberId.push_back( dwMemberId );
	}

	if( !g_eLocal.GetState( EVE_SECRETROOM ) )
		return;

	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	std::map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.find( nContinent );
	if( it!=pSRMng->m_mapSecretRoomContinent.end() )
	{
		CSecretRoomContinent* pSRCont = it->second;
		__SECRETROOM_TENDER srTender;
		srTender.dwGuildId = dwGuildId;
		srTender.nPenya = nPenya;
		srTender.vecLineUpMember.assign( vecMemberId.begin(), vecMemberId.end() );
		pSRCont->m_vecSecretRoomTender.push_back( srTender );
	}
}

void CDPDatabaseClient::OnRainbowRaceInfo( CAr & ar, DPID, DPID )
{
	CRainbowRaceMng::GetInstance()->Clear();

	int nSize;
	DWORD dwPlayerId;

	// 신청자 목록을 받아서 등록한다.
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		CRainbowRaceMng::GetInstance()->SetApplication( dwPlayerId );
	}

	// 이전 대회 랭킹 정보를 받아온다.
	std::vector<DWORD> vec_dwPrevRanking;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		vec_dwPrevRanking.push_back( dwPlayerId );
	}
	CRainbowRaceMng::GetInstance()->SetPrevRanking( vec_dwPrevRanking );
}

void CDPDatabaseClient::SendRainbowRaceReqLoad()
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceApplication( DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_APPTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceFailedUser( DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_FAILEDTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceRanking( DWORD dwPlayerId, int nRanking )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_RANKINGTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << nRanking;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnHousingLoadInfo( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( !pHousing )
		pHousing = CHousingMng::GetInstance()->CreateHousing( dwPlayerId );
	else
		FLERROR_LOG( PROGRAM_NAME, _T( "%d" ), dwPlayerId );

	if( pHousing )
	{
		pHousing->Serialize( ar );
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
			pUser->AddHousingAllInfo( pHousing );
		pHousing->AddAllFurnitureControl();
	}
}

void CDPDatabaseClient::OnHousingSetFunitureList( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	BOOL bAdd;

	ar >> dwPlayerId;
	housingInfo.Serialize( ar );
	ar >> bAdd;

	CHousingMng::GetInstance()->SetFurnitureList( dwPlayerId, housingInfo, bAdd );
}

void CDPDatabaseClient::OnHousingSetupFuniture( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	ar >> dwPlayerId;
	housingInfo.Serialize( ar );

	CHousingMng::GetInstance()->SetupFurniture( dwPlayerId, housingInfo );
}

void CDPDatabaseClient::OnHousingSetVisitAllow( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwTargetId;
	BOOL bAllow;
	ar >> dwPlayerId >> dwTargetId >> bAllow;

	CHousingMng::GetInstance()->SetVisitAllow( dwPlayerId, dwTargetId, bAllow );
}

void CDPDatabaseClient::OnHousingDBFailed( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwItemId;
	ar >> dwPlayerId >> dwItemId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( pHousing )	pHousing->Setting( FALSE );

	if( dwItemId != NULL_ID )
	{
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
		{
			FLItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = 1;
			if( !pUser->CreateItem( &itemElem ) )
				FLERROR_LOG( PROGRAM_NAME, _T( "CreateItem Failed! - %d, %s, %d" ), dwPlayerId, pUser->GetName(), dwItemId );
		}
		else
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid User - %d, %d" ), dwPlayerId, dwItemId );
	}
}

void CDPDatabaseClient::SendPropose( u_long idProposer, u_long idTarget )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idProposer << idTarget;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendCouple( u_long idProposer, u_long idTarget )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idProposer << idTarget;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendDecouple( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendClearPropose()
{
	BEFORESENDDUAL( ar, PACKETTYPE_CLEAR_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );  
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnProposeResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnProposeResult( ar );
}

void CDPDatabaseClient::OnCoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnCoupleResult( ar );
}

void CDPDatabaseClient::OnDecoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnDecoupleResult( ar );
}

void CDPDatabaseClient::OnCouple( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->Serialize( ar );
}

void CDPDatabaseClient::SendQueryAddCoupleExperience( u_long idPlayer, int nExperience )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idPlayer << nExperience;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnAddCoupleExperience( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnAddCoupleExperience( ar );
}

void CDPDatabaseClient::SendFunnyCoinReqUse( DWORD dwPlayerId, FLItemElem* pItemElem )	// 퍼니코인을 사용했음을 TransServer에 알린다.
{
	BEFORESENDDUAL( ar, PACKETTYPE_FUNNYCOIN_REQ_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	ar << pItemElem->m_dwItemId << pItemElem->GetSerialNumber();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnFunnyCoinAckUse( CAr & ar, DPID, DPID )	// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
{
	CFunnyCoin::GetInstance()->OnFunnyCoinAckUse( ar );
}

void CDPDatabaseClient::OnPCBangToggle( CAr & ar, DPID, DPID )
{
	BOOL bApply;
	ar >> bApply;
	CPCBang::GetInstance()->SetApply( bApply );
}

void CDPDatabaseClient::SendTimeLimitInfoReq( DWORD dwPlayerId, char* szAccount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitInfoAck( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	int nPlayTime;
	ar >> dwPlayerId >> nPlayTime;

	FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
	if( IsValidObj( pUser ) )
	{
		pUser->m_nAccountPlayTime = nPlayTime;
		pUser->AddAccountPlayTime();

		//	mulcom	BEGIN100315	베트남 시간 제한
		if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
		{
			if( pUser->m_nAccountPlayTime != -1 )
			{
				pUser->AddBuff( BUFF_EQUIP, ITEM_INDEX( 26811, II_VIETNAM_BUFF01 ), 1, 999999999, 0, BT_TICK );	// TODO
			}
		}
		//	mulcom	END100315	베트남 시간 제한
	}
}

void CDPDatabaseClient::SendTimeLimitUpdateReq( char* szAccount, int nPlayTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << nPlayTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitReset( CAr & /*ar*/, DPID, DPID )
{
	g_xWSUserManager->ResetAccountPlayTime();
}


void CDPDatabaseClient::SendLogInstanceDungeon( DWORD dwDungeonId, DWORD dwWorldId, u_long uChannel, int nDungeonType, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwDungeonId << dwWorldId << uChannel << nDungeonType << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnQuizEventOpen( CAr & /*ar*/, DPID, DPID )
{
	CQuiz::GetInstance()->OpenQuizEvent();
}

void CDPDatabaseClient::OnQuizList( CAr & ar, DPID, DPID )
{
	CQuiz::QUIZLIST QL;
	QL.Serialize( ar );
	CQuiz::GetInstance()->MakeQuizList( QL );
	ar >> CQuiz::GetInstance()->m_nQuizSize;
}

void CDPDatabaseClient::OnQuizEventNotice( CAr & /*ar*/, DPID, DPID )
{
	CQuiz::GetInstance()->NoticeMessage();
}

void CDPDatabaseClient::OnQuizEventChanged( CAr & /*ar*/, DPID, DPID )
{
	CQuiz::GetInstance()->LoadScript();
	if( !CQuiz::GetInstance()->IsRun() )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "QuizEvent.lua 재실행 완료!!!" ) );
	}
}

void CDPDatabaseClient::SendQuizEventOpen( int nType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventState( int nState, int nChannel, int nWinnerCount, int nQuizCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nState << nChannel << nWinnerCount << nQuizCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventEntrance( u_long idPlayer, int nChannel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_ENTRANCE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nChannel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventSelect( u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_SELECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nChannel << nQuizId << nSelect << nAnswer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_PRIZEITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << dwItemId << nItemNum;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendErrorLogToDB( FLWSUser* pUser, char chType, LPCTSTR szError )
{
	if( !IsValidObj( pUser ) )
		return;

	BEFORESENDDUAL( ar, PACKETTYPE_ERROR_LOG_TO_DB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << g_uIdofMulti << chType;
	ar.WriteString( szError );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnLoadGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->Serialize( ar );
}
void CDPDatabaseClient::OnBuyGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnBuyGuildHouse( ar );
}
void CDPDatabaseClient::OnRemoveGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnRemoveGuildHouse( ar );
}

void CDPDatabaseClient::OnGuildHousePacket( CAr & ar, DPID, DPID )
{
	BOOL bResult; DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> bResult >> dwGuildId;
	ar >> nPacketType >> nIndex;
	gfi.Serialize( ar );

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse )
	{
		if( bResult )
		{
			if( pGuildHouse->OnGuildHousePacket( nPacketType, gfi, nIndex ) )
				pGuildHouse->SendWorldToClient( nPacketType, gfi, nIndex );
		}
		else	// 테스트용 코드
		{
			pGuildHouse->AfterFailedGuildHousePacket( nPacketType, gfi, nIndex );
		}
		pGuildHouse->SetWaitDBAckPlayerId( NULL_ID );
	}
}

void CDPDatabaseClient::SendLogGuildFurniture( DWORD dwGuildId, GH_Fntr_Info & gfi, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDFURNITURE_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwGuildId;
	gfi.Serialize( ar );
	ar << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPDatabaseClient::OnGuildHouseTenderInfo( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderInfo( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderJoin( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderJoin( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderState( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderState( ar );
}

void CDPDatabaseClient::OnGuildHouseGradeUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseGradeUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseLevelUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseLevelUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseExpired( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseExpired( ar );
}

void CDPDatabaseClient::SendGuildHouseTenderState( const PT_PACKET_GUILDHOUSE_TENDER_STATE ptGHTenderState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)ptGHTenderState, sizeof( T_PACKET_GUILDHOUSE_TENDER_STATE ) );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildHouseExpired( DWORD dwGuildId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_EXPIRED, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwGuildId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#endif // __GUILD_HOUSE_MIDDLE

void CDPDatabaseClient::OnAllCampus( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->Serialize( ar );
}

void CDPDatabaseClient::OnAddCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnAddCampusMember( ar );
}

void CDPDatabaseClient::OnRemoveCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnRemoveCampusMember( ar );
}

void CDPDatabaseClient::OnUpdateCampusPoint( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnUpdateCampusPoint( ar );
}

void CDPDatabaseClient::SendAddCampusMember( u_long idMaster, int nMasterPoint, u_long idPupil, int nPupilPoint )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ADD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idMaster << nMasterPoint << idPupil << nPupilPoint;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRemoveCampusMember( u_long idCampus, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_REMOVE_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint, BOOL bAdd, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_UPDATE_POINT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nCampusPoint << bAdd << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}


//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

void	CDPDatabaseClient::SendCouponNumber( const PT_PACKET_COUPON_NUMBER ptCouponNumber )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPON_ITEM_NUMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)ptCouponNumber, sizeof( T_PACKET_COUPON_NUMBER ) );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPDatabaseClient::OnCouponNumber( CAr & ar, DPID, DPID )
{
	T_PACKET_COUPON_NUMBER tCouponNumber = { 0 };
	//mem_set( &tCouponNumber, 0, sizeof( tCouponNumber ) );
	ar.Read( (void*)(&tCouponNumber), sizeof( T_PACKET_COUPON_NUMBER ) );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( tCouponNumber.dwPlayerId );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( tCouponNumber.nError == 0 )		// success
		{
			FLItemElem itemElem;
			itemElem.m_dwItemId = tCouponNumber.dwItemId;
			itemElem.m_nItemNum = 1;
			itemElem.SetSerialNumber();
			//itemElem.SetFlag( FLItemElem::binds );

			PT_ITEM_SPEC pItemProp = itemElem.GetProp();
			if( pItemProp != NULL )
			{
				if( pUser->CreateItem( &itemElem ) == TRUE )
				{
					pUser->AddDefinedText( TID_GAME_EVENT_LEVELUP_GIFT, "\"%s\"", pItemProp->szName );
				}
				else
				{
					// mail..
					g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, "", "" );
					pUser->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST );
				}
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid ItemId:[%d], Account:[%s], UserId:[%07d], Coupon:[%s]" ),
					tCouponNumber.dwItemId, tCouponNumber.szAccount, tCouponNumber.dwPlayerId, tCouponNumber.szCouponNumber );
				return;
			}
		}
		else	// fail
		{
			pUser->AddDefinedText( TID_MMI_EVENT_AROTTO_KOR_COND );
			return;
		}
	}

	return;
}

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////


void	CDPDatabaseClient::SendSerialNumberReq( DWORD dwCount )
{
	T_PACKET_SERIALNUMBER_REQ tMsg;
	tMsg.dwCount = dwCount;

	BEFORESENDDUAL( ar, PACKETTYPE_SERIALNUMBER_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &tMsg, sizeof( T_PACKET_SERIALNUMBER_REQ ) );
	SEND( ar, this, DPID_SERVERPLAYER );
}


void	CDPDatabaseClient::OnSerialNumberAck( CAr & ar, DPID, DPID )
{
	T_PACKET_SERIALNUMBER_ACK tMsg = { 0 };
	//mem_set( &tMsg, 0, sizeof( tMsg ) );

	ar.Read( &tMsg, sizeof( T_PACKET_SERIALNUMBER_ACK ) );

	if( CSerialNumber::GetInstance()->Add( tMsg.iStartNumber, tMsg.iEndNumber ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Serial Number 추가 실패 Start: %d, End: %d" ),
			tMsg.iStartNumber, tMsg.iEndNumber );

		return;
	}
}

void	CDPDatabaseClient::SendBaseGameSettingReq()
{
	BEFORESENDDUAL( ar, PACKETTYPE_BASEGAMESETTING_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPDatabaseClient::OnCooperativeContributions_Contribute( CAr & ar, DPID, DPID )
{
	using namespace nsCooperativeContributions;

	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	T_W2DB_PACKET_CONTRIBUTE_ACK ack;
	ar.Read( &ack, sizeof( ack ) );

	cooperativeContributions_SetContributedAndTryReward( ack.dwContributionID, ack.dwContiributionCount );

	FLWSUser * pUser		= g_xWSUserManager->GetUserByPlayerID( ack.idPlayer );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLContribution * pContribution		= COOPERATIVE_CONTRIBUTIONS().FindContribution( ack.dwContributionID );
	if( pContribution == NULL )
		return;

	pUser->AddCooperativeContributions_Info( *pContribution );					//갱신정보만 보내줘야하나 갑작스런 기획 변경으로 귀차니즘에 의해 정보 모두 보내줌
}

void CDPDatabaseClient::OnCooperativeContributions_GetRanking( CAr & ar, DPID, DPID )
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_ACK ack;
	ar.Read( &ack, sizeof( ack ) );

	FLWSUser * pUser		= g_xWSUserManager->GetUserByPlayerID( ack.idPlayer );
	if( IsValidObj( pUser ) == FALSE )
		return;

	pUser->AddCooperativeContributions_GetRanking( T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::RESULT_SUCCESS, &ack );
}

void CDPDatabaseClient::OnCooperativeContributions_GetContributed( CAr & ar, DPID, DPID )
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;
	//using namespace nsCooperativeContributions;

	T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_ACK ack;
	ar.Read( &ack, sizeof( ack ) );

	cooperativeContributions_SetContributedAndTryReward( ack.dwContributionID, ack.dwContiributionCount );
}
//
//void CDPDatabaseClient::OnCooperativeContributions_TryRewardItem( CAr & ar, DPID, DPID )
//{
//	if( _GetContentState( CT_DONATION ) != CS_VER1 )
//		return;
//
//	T_W2DB_PACKET_TRY_REWARD_ITEM_ACK ack;
//	ar.Read( &ack, sizeof( ack ) );
//
//	//ack.dwContributionID;
//	switch( ack.eResult )
//	{
//	case T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::RST_SUCCESS:
//		break;
//	case T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::RST_FAIL:
//		break;
//	case T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::RST_ALREADY_REWARD:
//		break;
//	default:
//		{
//			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID RESULT (%d) : ContributionID(%u) ]" ), ack.dwContributionID );
//		}
//		break;
//	}
//}


BOOL	CDPDatabaseClient::cooperativeContributions_SetContributedAndTryReward( const DWORD dwContributionID, const DWORD dwContirbutedCount )
{
	using namespace nsCooperativeContributions;

	FLContribution * pContribution	= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	if( pContribution == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST ContributionID(%u) ]" ), dwContributionID );
		return FALSE;
	}

	IGlobalEndCondition * pGlobalEndCondition = pContribution->GetGlobalEndCondition();
	if( pGlobalEndCondition == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST GlobalEndCondition(%u) ]" ), dwContributionID );
		return FALSE;
	}

	pGlobalEndCondition->SetContributed( dwContirbutedCount );

	if( pContribution->IsComplete() == TRUE )
	{
		//COOPERATIVE_CONTRIBUTIONS().TryRewardAllUser( dwContributionID );
		pContribution->TryRewardAllUser();
		//		pContribution->Query_TryRewardItem();
	}

	return TRUE;
}

void	CDPDatabaseClient::OnCooperativeContributions_TryWriteItemRewardInfo( CAr & ar, DPID, DPID )
{
	T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK ack;
	ar.Read( &ack, sizeof( ack ) );

	switch( ack.eResult )
	{
	case T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::RST_FAIL:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - WRITE ITEM REWARD INFO : ContiributionID(%u) tmStart(%s) tmEnd(%s) dwItemID(%d) ItemCount(%d) ]")
				, ack.dwContributionID, FLTmToString( ack.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( ack.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
				, ack.dwItemID, ack.shItemCount );
		}
		break;
	case T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::RST_SUCCESS:
		__noop;
		break;

	case T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::RST_ALREADY_WRITE:
		__noop;
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ UNKNOWN RESULT - WRITE ITEM REWARD INFO : ret(%d) ContiributionID(%u) tmStart(%s) tmEnd(%s) dwItemID(%d) ItemCount(%d) ]")
				, ack.eResult
				, ack.dwContributionID, FLTmToString( ack.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( ack.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
				, ack.dwItemID, ack.shItemCount );
		}
		break;
	}
}


void	CDPDatabaseClient::OnColosseum_GetDungeonRankingInfo( CAr & ar, DPID, DPID )
{
	T_PACKET_COLOSSEUM_GET_RANKING_INFO ack;

	u_long idPlayer;
	ar >> idPlayer;
	ar.Read( &ack, sizeof( ack ) );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( IsValidObj( pUser ) == FALSE )
		return;

	pUser->AddColosseum_ColosseumRanking( ack );
}

void	CDPDatabaseClient::OnTeleportMapPointListNoti( CAr & ar, DPID, DPID )
{
	FLPacketTeleportMapPointListNoti kFromDB;
	kFromDB.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kFromDB.dwPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	g_pTeleportMapUserPointMng->Add( kFromDB.dwPlayerID, &kFromDB.vecUserPoints );
}


void CDPDatabaseClient::OnConsignmentSale_CheckRegistedCountAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_CheckRegistedCountAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLAutoAssigner< BOOL > kAuto_SetRegistingFlagFale( pUser->m_kConsignmentSaleData.bRegisting, FALSE );

	if( kDBAck.m_nItemCount <= 0 || kDBAck.m_nItemCost <= 0 )
	{
		return;
	}

	FLSnapshot_RegistAck ack;

	//1. 등록 카운트 체크
	const int nRemainDefaultCount	= CONSIGNMENT_SALE_SPEC().m().nDefaultRegistLimit - kDBAck.m_dwDefaultRegistCount;
	const int nRemainItemCount		= FLConsignmentSale::GetExtendRegistCountMax( *pUser ) - kDBAck.m_dwItemRegistCount;
	if( nRemainDefaultCount <= 0 && nRemainItemCount <= 0 )
	{
		ack.m_eResult					= FLSnapshot_RegistAck::E_OVERFLOW;
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_SELLTEXT02 );	//@@@@@@@@ %s
		pUser->AddPacket( &ack );
		return;
	}

	E_REGIST_MODE eRegistMode = DEFAULT_REGIST_MODE;
	if( nRemainDefaultCount <= 0 )
		eRegistMode			= ITEM_REGIST_MODE;


	//2. 아이템 찾아서
	FLItemElem* pItem		= pUser->m_Inventory.GetAtId( kDBAck.m_dwItemObjID );
	if( pItem == NULL )
	{
		return;
	}

	PT_ITEM_SPEC pSpec		= pItem->GetProp();
	if( pSpec == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ INVALID ITEM(%u) ]", pItem->m_dwItemId );
		return;
	}

	if( pItem->IsOwnState() == TRUE || IsUsableItem( pItem ) == FALSE || pItem->m_dwItemId  == ITEM_INDEX( 3600, II_GEN_WARP_COUPLERING ))
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR01, "\"%s\"", ( pSpec->szName[ 0 ] == '\0' ) ? "no item" : pSpec->szName );
		return;
	}


	if( pUser->IsUsing( pItem ) == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMRESEARCH01 );
		ack.m_eResult					= FLSnapshot_RegistAck::E_FAILURE;		//@@@@@@@@@ 사용 중인 아이템 입니다.
		pUser->AddPacket( &ack );
		return;
	}



	if( pSpec->bCanTrade == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR01, "\"%s\"", ( pSpec->szName[ 0 ] == '\0' ) ? "no item" : pSpec->szName );
		ack.m_eResult					= FLSnapshot_RegistAck::E_NO_TRADABLE_ITEM;
		pUser->AddPacket( &ack );
		return;
	}

	if( pItem->m_nItemNum < kDBAck.m_nItemCount )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR02 );
		ack.m_eResult					= FLSnapshot_RegistAck::E_FAILURE;
		pUser->AddPacket( &ack );
		return;
	}


	//3. 세금
	const int nTotPrice			= kDBAck.m_nItemCost;// * kReq.m_shItemCount;
	const int nTax				= FLConsignmentSale::GetTax( pUser, nTotPrice );

	//4. 골드 까고
	if( pUser->CheckUserGold( nTax, false ) == false )
	{
		ack.m_eResult					= FLSnapshot_RegistAck::E_FAILURE;
		pUser->AddPacket( &ack );
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_BUYTEXT01 );
		return;
	}

	pUser->AddGold( -nTax );
	CDPSrvr::PutPenyaLog( pUser, "s", "ConsignmentSale RegistStart", nTax );

	//5. 인벤에서 아이템 삭제
	FLItemElem kRegistItem		= *pItem;
	kRegistItem.m_nItemNum		= kDBAck.m_nItemCount;

	CDPSrvr::PutItemLog( pUser, "s", "ConsignmentSale RegistStart", pItem, kDBAck.m_nItemCount );
	pUser->RemoveItem( kDBAck.m_dwItemObjID, kDBAck.m_nItemCount );

	const DWORD dwWorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
#ifdef __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, pUser->GetPos() );
#endif	// __LAYER_1015

	// 등록 가능 개수 확장 아이템 버프의 남은 시간을 계산해서 기본등록시간 미만일 경우 남은 시간 만큼만 등록가능시간을 셋팅한다.
	size_t uRegRemainHour	= CONSIGNMENT_SALE_SPEC().m().nSaleHour;
	IBuff* pBuff			= pUser->GetBuffByIk3( IK3_TRADEREGISTERITEM );
	if( pBuff != NULL && nRemainDefaultCount <= 0 && nRemainItemCount > 0 )
	{
		const DWORD dwBuffRemainSec	= pBuff->GetRemainSec();
		const DWORD dwBuffRemainHour= dwBuffRemainSec / FLTm::ONE_HOUR_SECOND;
		uRegRemainHour		= min( static_cast< DWORD >( CONSIGNMENT_SALE_SPEC().m().nSaleHour ), ( dwBuffRemainHour + 1 ) );
	}

	//6. 보내준다.
	FLPacketWDB_RegistReq kReqDB;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	kReqDB.m_nPrice			= kDBAck.m_nItemCost;//nTotPrice;
	kReqDB.m_nOnePrice		= static_cast<int>( kDBAck.m_nItemCost / kDBAck.m_nItemCount );

	FLStrcpy( kReqDB.m_szItemName, _countof( kReqDB.m_szItemName ), pSpec->szName );
	FLStrcpy( kReqDB.m_szPlayerName, _countof( kReqDB.m_szPlayerName ), pUser->GetName() );
	kReqDB.m_dwItemType1	= pSpec->dwMainCategory;
	kReqDB.m_dwItemType2	= pSpec->dwSubCategory;
	kReqDB.m_dwGrade		= pSpec->dwItemGrade;
	kReqDB.m_nLimitLevel	= pSpec->nMinLimitLevel;
	kReqDB.m_uRemainHour	= uRegRemainHour;
	kReqDB.m_kItemElem		= kRegistItem;
	kReqDB.m_eRegistMode	= eRegistMode;
	kReqDB.m_dwWorldID		= dwWorldId;//( pUser->GetWorld() ) ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	g_dpDBClient.SendPacket( &kReqDB );	

	kAuto_SetRegistingFlagFale.Disable();
}



void CDPDatabaseClient::OnConsignmentSale_GetRegistedCountAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_GetRegistedCountAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLSnapshot_UserInfoAck ack;
	ack.m_dwDefaultRegistCount		= kDBAck.m_dwDefaultRegistModeCount;
	ack.m_dwDefaultRegistCountMax	= CONSIGNMENT_SALE_SPEC().m().nDefaultRegistLimit;
	ack.m_dwExtendRegistCount		= kDBAck.m_dwItemRegistModeCount;
	ack.m_dwExtendRegistCountMax	= FLConsignmentSale::GetExtendRegistCountMax( *pUser );

	pUser->AddPacket( &ack );
}


void CDPDatabaseClient::OnConsignmentSale_SellItemNoti( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_SellItemNoti kNoti;
	kNoti.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kNoti.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@ 판매되었습니다.

}

void CDPDatabaseClient::OnConsignmentSale_RegistAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_RegistAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	pUser->m_kConsignmentSaleData.bRegisting = FALSE;


	FLSnapshot_RegistAck kAck;
	kAck.m_eResult					= FLPacket_RegistAck::E_SUCCESS;
	pUser->AddPacket( &kAck );

	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_SELLTEXT05 );

}



void CDPDatabaseClient::OnConsignmentSale_CancelRegistedAck( CAr & ar, DPID, DPID )
{	
	using namespace nsConsignmentSale;

	FLPacketWDB_CancelRegistedAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_CancelRegistedAck kAck;
	kAck.m_eResult			= kDBAck.m_eResult;
	kAck.m_u64SaleSRL		= kDBAck.m_u64SaleSRL;
	//	kAck.m_eRegistMode		= kDBAck.m_eRegistMode;

	if( kDBAck.m_eResult != FLPacketWDB_CancelRegistedAck::E_SUCCESS )
	{
		switch( kDBAck.m_eResult )
		{
		case FLPacketWDB_CancelRegistedAck::E_AREADY_CANCELED:
			//@@@@@@@@이미 지워진 아이템 입니다.
			break;
		case FLPacketWDB_CancelRegistedAck::E_FAILURE:	
			__noop;
			break;
		case FLPacketWDB_CancelRegistedAck::E_BUSY:
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMRESEARCH01 );
			break;

		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ERESULT(%d) ]" ), kDBAck.m_eResult );
			break;
		}
		pUser->AddPacket( &kAck );
		return;
	}


	pUser->AddPacket( &kAck );
	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_CANCELTEXT02 );
}

void CDPDatabaseClient::OnConsignmentSale_RegistedInfoListAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_RegistedInfoListAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_RegistedInfoListAck kAck;
	kAck.m_vecSaleInfoList	= kDBAck.m_vecSaleInfoList;


	pUser->AddPacket( &kAck );
}

void CDPDatabaseClient::OnConsignmentSale_GetPriceAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_GetPriceAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		//예약 취소
		FLPacketWDB_ClearReservedInfoNoti kNoti;
		kNoti.m_u64SaleSRL	= kDBAck.m_u64SaleSRL;
		g_dpDBClient.SendPacket( &kNoti );
		return;
	}

	if( kDBAck.m_eResult != FLPacketWDB_GetPriceAck::E_SUCCESS )
	{
		FLPacketWDB_ClearReservedInfoNoti kNoti;
		kNoti.m_u64SaleSRL	= kDBAck.m_u64SaleSRL;
		g_dpDBClient.SendPacket( &kNoti );

		FLSnapshot_BuyItemAck ack;
		ack.m_u64SaleSRL		= kDBAck.m_u64SaleSRL;

		switch( kDBAck.m_eResult )
		{
		case FLPacketWDB_GetPriceAck::E_CANNOT_BUY_SAME_OWNER_ITEM:
			ack.m_eResult	= FLSnapshot_BuyItemAck::E_FAILURE;
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_MYITEMREGISTBUYFAILD01 );		//@@@@@ 자신의 아이템은 살수 없습니다.
			break;
		case FLPacketWDB_GetPriceAck::E_ALREADY_DELETE:
			ack.m_eResult	= FLSnapshot_BuyItemAck::E_ALREADY_DELETE;
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_BUYTEXT02 );
			break;
		case FLPacketWDB_GetPriceAck::E_FAILURE:	
			ack.m_eResult	= FLSnapshot_BuyItemAck::E_FAILURE;
			__noop;
			break;
		case FLPacketWDB_GetPriceAck::E_BUSY:
			ack.m_eResult	= FLSnapshot_BuyItemAck::E_BUSY;
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMRESEARCH01 );
			break;
		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ERESULT(%d) ]" ), kDBAck.m_eResult );
			break;
		}

		pUser->AddPacket( &ack );
		return;
	}

	//4. 골드 까고
	if( pUser->CheckUserGold( kDBAck.m_nPrice, false ) == false )
	{
		//예약 취소
		FLPacketWDB_ClearReservedInfoNoti kNoti;
		kNoti.m_u64SaleSRL	= kDBAck.m_u64SaleSRL;
		g_dpDBClient.SendPacket( &kNoti );

		FLSnapshot_BuyItemAck ack;
		ack.m_eResult					= FLPacket_BuyItemAck::E_FAILURE;
		pUser->AddPacket( &ack );
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_BUYTEXT01 );
		return;
	}

	pUser->AddGold( -kDBAck.m_nPrice );
	CDPSrvr::PutPenyaLog( pUser, "s", "ConsignmentSale BuyStart", kDBAck.m_nPrice );

	const DWORD dwWorldId	= ( pUser->GetWorld() == NULL ) ? WI_WORLD_NONE : pUser->GetWorld()->GetID();
#ifdef __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, pUser->GetPos() );
#endif	// __LAYER_1015

	FLPacketWDB_BuyItemReq kDBReq;
	kDBReq.m_uPlayerID		= kDBAck.m_uPlayerID;		
	kDBReq.m_u64SaleSRL		= kDBAck.m_u64SaleSRL;
	kDBReq.m_nPrice			= kDBAck.m_nPrice;	
	kDBReq.m_uSalerID		= kDBAck.m_uSalerID;
	kDBReq.m_dwWorldID		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE; FLStrcpy( kDBReq.m_szPlayerName, _countof( kDBReq.m_szPlayerName ), pUser->GetName() );

	g_dpDBClient.SendPacket( &kDBReq );
}

void CDPDatabaseClient::OnConsignmentSale_BuyItemAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_BuyItemAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_BuyItemAck kAck;
	kAck.m_u64SaleSRL		= kDBAck.m_u64SaleSRL;
	kAck.m_eResult			= kDBAck.m_eResult;
	kAck.m_dwItemID			= kDBAck.m_dwItemID;

	if( kDBAck.m_eResult != FLPacketWDB_BuyItemAck::E_SUCCESS )
	{
		switch( kDBAck.m_eResult )
		{
		case FLPacketWDB_BuyItemAck::E_ALREADY_DELETE:
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMBUTNOTFOUND );
			break;
		case FLPacketWDB_BuyItemAck::E_FAILURE:	
			__noop;
			break;
		case FLPacketWDB_BuyItemAck::E_BUSY:	
			pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMRESEARCH01 );
			break;
		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ERESULT(%d) ]" ), kDBAck.m_eResult );
			break;
		}
		pUser->AddPacket( &kAck );
		return;
	}

	//구매 ack
	PT_ITEM_SPEC pSpec		= g_xSpecManager->GetSpecItem( kDBAck.m_dwItemID );
	if( pSpec == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM ID(%u) ]" ), kDBAck.m_dwItemID );

	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_BUYTEXT03, "\"%s\"", ( pSpec == NULL ) ? "empty" : pSpec->szName ); //@@@@@@@@@@
	pUser->AddPacket( &kAck );
}

void CDPDatabaseClient::OnConsignmentSale_CollectSaleGoldAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_CollectSaleGoldAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_CollectSaleGoldAck kAck;
	kAck.m_u64SaleGoldSRL	= kDBAck.m_u64SaleGoldSRL;
	kAck.m_eResult			= kDBAck.m_eResult;

	if( kDBAck.m_eResult != FLPacketWDB_CollectSaleGoldAck::E_SUCCESS )
	{
		switch( kDBAck.m_eResult )
		{
		case FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT:
			//@@@@@@@@이미 지워진 정산 입니다.
			break;
		case FLPacketWDB_CollectSaleGoldAck::E_FAILURE:	
			__noop;
			break;
		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ERESULT(%d) ]" ), kDBAck.m_eResult );
			break;
		}

		pUser->AddPacket( &kAck );
		return;
	}

	//구매 ack


	pUser->AddPacket( &kAck );
	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_SUMTEXT01 );
}

void CDPDatabaseClient::OnConsignmentSale_SearchAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_SearchAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_SearchAck kAck;

	kAck.m_dwCountMax		= kDBAck.m_dwCountMax;
	kAck.m_dwPage			= kDBAck.m_dwPage;		
	kAck.m_vecSaleInfoList	= kDBAck.m_vecSaleInfoList;

	pUser->AddPacket( &kAck );
}

void CDPDatabaseClient::OnConsignmentSale_SaleGoldInfoListAck( CAr & ar, DPID, DPID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_SaleGoldInfoListAck kDBAck;
	kDBAck.Deserialize( ar );

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( kDBAck.m_uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshot_SaleGoldInfoListAck kAck;
	kAck.m_vecSaleGoldInfo	= kDBAck.m_vecSaleGoldInfo;

	pUser->AddPacket( &kAck );
}

void	CDPDatabaseClient::OnServerList( CAr & ar, DPID, DPID )
{
	FLPacketServerDescriptionNoti kPacket;
	kPacket.Deserialize( ar );

	g_pServerDescription->Clear();

	g_pServerDescription->SetServerIndex( kPacket.dwServerIndex );

	for( std::vector< T_SERVER_DESC >::const_iterator kCItr = kPacket.vecServerDesc.begin(); kCItr != kPacket.vecServerDesc.end(); ++kCItr )
	{
		const T_SERVER_DESC kServerDesc = *kCItr;
		g_pServerDescription->Add( &kServerDesc );
	}
}

void	CDPDatabaseClient::OnGuildBankQueryFailAck( CAr & ar, DPID, DPID )
{
	T_GUILD_BANK_QUERY_FAIL_ACK	fromTrans;

	ar.Read( &fromTrans, sizeof( fromTrans ) );

	if( fromTrans.dwErrorCode == E_EXIST_ALREADY_USE_WORLD )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ GUILD_BANK ERROR - EXIST ALREADY USE WORLD. Already Use World:(%d) ]" ), fromTrans.dwGuildBankUseWorldID );
	}
	else if( fromTrans.dwErrorCode == E_QUERY_FAIL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ GUILD_BANK ERROR - FAILED GUILD BANK QUERY. ]" ) );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ GUILD_BANK ERROR - NOT DEFINED ERROR CODE. ErrorCode:(%d) ]" ), fromTrans.dwErrorCode );
	}

	::ExitProcess( 0 );
}

void	CDPDatabaseClient::OnDungeonBestCompleteTimeAck( CAr & ar, DPID, DPID )
{
	T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO fromTrans;
	ar.Read( &fromTrans, sizeof( fromTrans ) );

	CInstanceDungeonHelper::GetInstance()->OnDungeonBestCompleteTimeAck( fromTrans );
}

void	CDPDatabaseClient::OnGuildBankOutputItemAck( CAr & ar, DPID, DPID )
{
	FLPacketGuildBankOutputItemAck fromTrans;

	fromTrans.Deserialize( ar );

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( fromTrans.dwPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ FAILED FIND PLAYER. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ FAILED FIND PLAYER WORLD. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	CGuild* pGuild	= g_GuildMng.GetGuild( fromTrans.dwGuildID );
	if( pGuild == NULL )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ FAILED FIND GUILD. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	if( pGuild->IsMember( fromTrans.dwPlayerID ) == FALSE )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ PLAYER is not GUILD MEMBER. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	if( fromTrans.bSuccess == true )	// 길드 뱅크 업데이트 성공
	{
		// 유저에게 아이템 지급
		if( pUser->m_Inventory.Add( &fromTrans.kOutputItemElem ) == FALSE )
		{
			FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ FAILED CREATE ITEM for PLAYER. GUILD_ID(%06d), PLAYER_ID(%07d), ITEM_SERIAL(%u) ]" )
				, fromTrans.dwGuildID, fromTrans.dwPlayerID, fromTrans.kOutputItemElem.GetSerialNumber() );

//			g_DPSrvr.PutItemLog( pUser, "F", "FAIL_GUILDBANK", &fromTrans.kOutputItemElem );
			g_DPSrvr.SendPutItemLog( pUser, "F", "FAIL_GUILDBANK", pUser->GetName(), &fromTrans.kOutputItemElem, fromTrans.kOutputItemElem.m_nItemNum );
			
			return;
		}

		g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );

//		g_DPSrvr.PutItemLog( pUser, "Y", "GUILDBANK", &fromTrans.kOutputItemElem );
		g_DPSrvr.SendPutItemLog( pUser, "Y", "GUILDBANK", pUser->GetName(), &fromTrans.kOutputItemElem, fromTrans.kOutputItemElem.m_nItemNum );

		// 클라이언트에게 아이템이 인벤토리에 추가됨을 알린다.
		pUser->AddGetItemGuildBank( &fromTrans.kOutputItemElem );
		// 자신을 제외한 모든 클라이언트에게 알려준다.
		g_xWSUserManager->AddGetItemElem( pUser, &fromTrans.kOutputItemElem );

		return;
	}
	else								// 길드 뱅크 업데이트 실패
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_ITEM" ), _T( "[ FAILED UPDATE DB GUILD BANK TABLE. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		
		g_DPSrvr.SendPutItemLog( pUser, "F", "FAIL_GUILDBANK", pUser->GetName(), &fromTrans.kOutputItemElem, fromTrans.kOutputItemElem.m_nItemNum );
		
		return;
	}
}

void	CDPDatabaseClient::OnGuildBankOutputGoldAck( CAr & ar, DPID, DPID )
{
	FLPacketGuildBankOutputGoldAck fromTrans;

	fromTrans.Deserialize( ar );

	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( fromTrans.dwPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ FAILED FIND PLAYER. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ FAILED FIND PLAYER WORLD. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	CGuild* pGuild	= g_GuildMng.GetGuild( fromTrans.dwGuildID );
	if( pGuild == NULL )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ FAILED FIND GUILD. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	if( pGuild->IsMember( fromTrans.dwPlayerID ) == FALSE )
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ PLAYER is not GUILD MEMBER. GUILD_ID(%06d), PLAYER_ID(%07d) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID );
		return;
	}

	const int nOutputGold	= static_cast<int>( fromTrans.dwOutputGold );

	//////////////////////////////////////////////////////////////////////////
	// log
	LogItemInfo aLogItem;
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
	aLogItem.kLogItem.nQuantity = nOutputGold;
	//////////////////////////////////////////////////////////////////////////

	if( fromTrans.bSuccess == true )	// 길드 뱅크 업데이트 성공
	{
		// 유저에게 골드 지급
		if( pUser->AddGold( nOutputGold, FALSE ) == FALSE )
		{
			FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ FAILED ADD GOLD for PLAYER. GUILD_ID(%06d), PLAYER_ID(%07d), GOLD(%d) ]" )
				, fromTrans.dwGuildID, fromTrans.dwPlayerID, nOutputGold );

//			g_DPSrvr.SendPutPenyaLog( pUser, "F", "FAIL_GUILDBANK", pUser->GetName(), -nOutputGold );

			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "F" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "FAIL_GUILDBANK" );
			g_DPSrvr.OnLogItem( aLogItem );

			return;
		}

		pUser->AddGetGoldGuildBank( nOutputGold, 0, pUser->m_idPlayer, 0 );	// 0은 업데이트 시킨 클라에게 

		g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );

//		g_DPSrvr.SendPutPenyaLog( pUser, "Y", "GUILDBANK", pUser->GetName(), -nOutputGold );
		
		aLogItem.Gold = pUser->GetGold() - nOutputGold;
		aLogItem.Gold2 = pUser->GetGold();
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "Y" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GUILDBANK" );
		g_DPSrvr.OnLogItem( aLogItem );

		for( std::map<u_long, CGuildMember*>::const_iterator pos = pGuild->m_mapPMember.begin(); pos != pGuild->m_mapPMember.end(); ++pos )
		{
			CGuildMember* pGuildMember		= pos->second;
			FLWSUser* pOtherUser			= g_xWSUserManager->GetUserByPlayerID( pGuildMember->m_idPlayer );
			if( IsValidObj( pOtherUser ) && pOtherUser != pUser )
			{
				pOtherUser->AddGetGoldGuildBank( nOutputGold, 2, pUser->m_idPlayer, 0 );	// 2는 업데이트 해야할 클라이게
			}
		}

		return;
	}
	else								// 길드 뱅크 업데이트 실패
	{
		FLERROR_LOG( _T( "FAIL_GUILDBANK_OUTPUT_GOLD" ), _T( "[ FAILED UPDATE DB GUILD TABLE. GUILD_ID(%06d), PLAYER_ID(%07d), GOLD(%u) ]" )
			, fromTrans.dwGuildID, fromTrans.dwPlayerID, fromTrans.dwOutputGold );

//		g_DPSrvr.SendPutPenyaLog( pUser, "F", "FAIL_GUILDBANK", pUser->GetName(), -nOutputGold );
		
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "F" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "FAIL_GUILDBANK" );
		g_DPSrvr.OnLogItem( aLogItem );

		return;
	}
}
