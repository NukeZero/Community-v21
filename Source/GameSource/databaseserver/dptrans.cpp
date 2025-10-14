#include "stdafx.h"

#include "dpcoresrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
#include "dptrans.h"

#include "..\_network\objects\Obj.h"
#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;
#include <post.h>
#include <spevent.h>

#include "tlord.h"
#include "couplehelper.h"

extern	APP_INFO	g_appInfo;

#include "SecretRoomDBMng.h"
#include <Tax.h>
#include "RainbowRaceDBCtrl.h"
#include "HousingDBCtrl.h"
#include <FunnyCoin.h>
#include "AccountTimeLimitDBCtrl.h"
#include "QuizDBCtrl.h"
#include "GuildHouseDBCtrl.h"
#include "CampusDBCtrl.h"
#include "FLSpec.h"
#include "../_Common/FLServerDescription.h"

#include <FLDataBase.h>
#include <FLConsignmentSale_Query.h>
#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"
#include "../_CommonDefine/Packet/FLPacket_Certify.h"
#include <FLPlayer_Query.h>

CDPTrans::CDPTrans()
:m_bPCBangApply( TRUE )
{
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_SAVE_PLAYER, OnSavePlayer ); 
	ON_MSG( PACKETTYPE_SAVE_CONCURRENT_USER_NUMBER, OnSaveConcurrentUserNumber );

	ON_MSG( PACKETTYPE_LOG_PLAY_CONNECT, OnLogPlayConnect );
	ON_MSG( PACKETTYPE_PREVENT_LOGIN, OnPreventLogin );
	ON_MSG( PACKETTYPE_LOG_PLAY_DEATH, OnLogPlayDeath );
	ON_MSG( PACKETTYPE_LOG_LEVELUP, OnLogLevelUp );
	ON_MSG( PACKETTYPE_LOG_SERVER_DEATH, OnLogServerDeath );
	ON_MSG( PACKETTYPE_LOG_UNIQUEITEM, OnLogUniqueItem );
	ON_MSG( PACKETTYPE_LOG_TRADE, OnLogTradeItem );
	ON_MSG( PACKETTYPE_LOG_ITEM, OnLogItem );
	ON_MSG( PACKETTYPE_LOG_QUEST, OnLogQuest );
	ON_MSG( PACKETTYPE_CHANGEBANKPASS, OnChangeBankPass );
	ON_MSG( PACKETTYPE_LOG_GAMEMASTER_CHAT, OnGammaChat );
	ON_MSG( PACKETTYPE_LOG_PK_PVP, OnLogPkPvp );
	ON_MSG( PACKETTYPE_LOG_SCHOOL, OnLogSchool );
	ON_MSG( PACKETTYPE_SKILLPOINTLOG, OnLogSkillPoint );
	ON_MSG( PACKETTYPE_GUILD_BANK_QUERY, OnQueryGuildBank);
	ON_MSG( PACKETTYPE_GUILD_BANK_UPDATE, OnGuildBankUpdate);
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, OnGuildRealPay );
	ON_MSG( PACKETTYPE_WD_GUILD_CONTRIBUTION, OnGuildContribution );

	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING, OnUpdateGuildRanking );
	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING_DB, OnUpdateGuildRankingDB );

	ON_MSG( PACKETTYPE_SCHOOL_REPORT, OnSchoolReport );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );

	ON_MSG( PACKETTYPE_QUERYGUILDQUEST, OnQueryGuildQuest );
	ON_MSG( PACKETTYPE_INSERTGUILDQUEST, OnInsertGuildQuest );
	ON_MSG( PACKETTYPE_UPDATEGUILDQUEST, OnUpdateGuildQuest );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, OnQuerySetPlayerName );
	ON_MSG( PACKETTYPE_ITEM_TBL_UPDATE, OnItemTBLUpdate );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, OnQueryMailBox );


	//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_QUERYMAILBOX_COUNT, OnQueryMailBoxCount );
	ON_MSG( PACKETTYPE_QUERYMAILBOX_REQ, OnQueryMailBoxReq );
	//////////////////////////////////////////////////////////////////////////
	

	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, OnQueryPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL, OnQueryRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM, OnQueryGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD, OnQueryGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL, OnReadMail );

	ON_MSG( PACKETTYPE_START_GUILDCOMBAT, OnStartGuildCombat )
	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, OnGuidCombatInGuild );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, OnGuidCombatOutGuild );
	ON_MSG( PACKETTYPE_RESULT_GUILDCOMBAT, OnResultGuildCombat );
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, OnGCGetPenyaGuild );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, OnGCGetPenyaPlayer );
	ON_MSG( PACKETTYPE_CONTINUE_GUILDCOMBAT, OnGCContinue );

	ON_MSG( PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, OnQueryRemoveGuildBankTbl );
	ON_MSG( PACKETTYPE_CALL_USPLOGGINGQUEST, OnCalluspLoggingQuest );
	ON_MSG( PACKETTYPE_CALL_XXX_MULTI_SERVER, OnCalluspXXXMultiServer );
	ON_MSG( PACKETTYPE_LOG_EXPBOX, OnLogExpBox );

	ON_MSG( PACKETTYPE_CALL_USP_PET_LOG, OnCalluspPetLog );
	
	ON_MSG( PACKETTYPE_EVENTLUA_CHANGED, OnEventLuaChanged);

	ON_MSG( PACKETTYPE_EVE_RECOMMEND, OnEveRecommend );

	ON_MSG( PACKETTYPE_GC1TO1_STATETODB, OnGC1to1StateToDBSrvr );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERTODB, OnGC1to1Tender );
	ON_MSG( PACKETTYPE_GC1TO1_LINEUPTODB, OnGC1to1LineUp );
	ON_MSG( PACKETTYPE_GC1TO1_WARPERSONTODB, OnGC1to1WarPerson );
	ON_MSG( PACKETTYPE_GC1TO1_WARGUILDTODB, OnGC1to1WarGuild );

	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, OnUpdatePlayerData );

	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, OnGuildBankLogView );

	ON_MSG( PACKETTYPE_SEALCHAR_REQ, OnSealChar );
	ON_MSG( PACKETTYPE_SEALCHARCONM_REQ, OnSealCharConm );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, OnSealCharGet );
	ON_MSG( PACKETTYPE_SEALCHARSET_REQ, OnSealCharSet );

	// /군주입찰
	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, OnElectionAddDeposit );
	// /공약설정
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, OnElectionSetPledge );
	// /군주투표
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, OnElectionIncVote );
	// /군주이벤트시작
	ON_MSG( PACKETTYPE_L_EVENT_CREATE,	OnLEventCreate );
	// /군주스킬
	ON_MSG( PACKETTYPE_LORD_SKILL_USE,	OnLordSkillUse );
	// /군주프로세스
	ON_MSG( PACKETTYPE_ELECTION_PROCESS, OnElectionProcess );
	// /입후보시작
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, OnElectionBeginCandidacy );
	// /투표시작
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_VOTE, OnElectionBeginVote );
	// /투표종료
	ON_MSG( PACKETTYPE_ELECTION_END_VOTE, OnElectionEndVote );
	// /군주이벤트초기화
	ON_MSG( PACKETTYPE_L_EVENT_INITIALIZE,	OnLEventInitialize );

	ON_MSG( PACKETTYPE_TAX_SET_SECRETROOM_WINNER, OnSetSecretRoomWinGuild );
	ON_MSG( PACKETTYPE_TAX_SET_LORD, OnSetLord );
	ON_MSG( PACKETTYPE_TAX_SET_TAXRATE, OnTaxRate );
	ON_MSG( PACKETTYPE_TAX_ADDTAX, OnAddTax );
	ON_MSG( PACKETTYPE_TAX_APPLY_TAXRATE_NOW, OnApplyTaxRateNow );

	ON_MSG( PACKETTYPE_UPDATE_TAX_RATE, OnUpdateTaxRate );

	ON_MSG( PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, OnSecretRoomInsertToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, OnSecretRoomUpdateToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, OnSecretRoomLineUpToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_CLOSED, OnSecretRoomClosed );

	ON_MSG( PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, OnRainbowRaceLoadInfo );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPTODB, OnRainbowRaceApplication );
	ON_MSG( PACKETTYPE_RAINBOWRACE_FAILEDTODB, OnRainbowRaceFailedUser );
	ON_MSG( PACKETTYPE_RAINBOWRACE_RANKINGTODB, OnRainbowRaceRanking );

	ON_MSG( PACKETTYPE_HOUSING_LOADINFO, OnHousingLoadInfo );
	ON_MSG( PACKETTYPE_HOUSING_FURNITURELIST, OnHousingReqSetFurnitureList );
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, OnHousingReqSetupFurniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, OnHousingReqSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_GM_REMOVEALL, OnHousingReqGMRemoveAll );

	ON_MSG( PACKETTYPE_PROPOSE, OnPropose );
	ON_MSG( PACKETTYPE_COUPLE, OnCouple );
	ON_MSG( PACKETTYPE_DECOUPLE, OnDecouple );
	ON_MSG( PACKETTYPE_CLEAR_PROPOSE, OnClearPropose );
	ON_MSG( PACKETTYPE_ADD_COUPLE_EXPERIENCE, OnQueryAddCoupleExperience );

	ON_MSG( PACKETTYPE_LOG_GETHONORTIME, OnLogGetHonorTime );

	ON_MSG( PACKETTYPE_FUNNYCOIN_REQ_USE, OnFunnyCoinReqUse );

	ON_MSG( PACKETTYPE_TIMELIMIT_INFO, OnTimeLimitInfoReq );
	ON_MSG( PACKETTYPE_TIMELIMIT_UPDATE, OnTimeLimitUpdate );

	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_LOG, OnLogInstanceDungeon );

	ON_MSG( PACKETTYPE_QUIZ_OPEN, OnQuizEventOpen );
	ON_MSG( PACKETTYPE_QUIZ_STATE, OnQuizEventState );
	ON_MSG( PACKETTYPE_QUIZ_ENTRANCE, OnQuizEventEntrance );
	ON_MSG( PACKETTYPE_QUIZ_SELECT, OnQuizEventSelect );
	ON_MSG( PACKETTYPE_QUIZ_PRIZEITEM, OnPostPrizeItem );

	ON_MSG( PACKETTYPE_ERROR_LOG_TO_DB, OnErrorLog );

	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, OnGuildHousePacket );
	ON_MSG( PACKETTYPE_GUILDFURNITURE_LOG, OnLogGuildFurniture );
#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, OnGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, OnGuildHouseLevelUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_STATE, OnGuildHouseTenderState );
	ON_MSG( PACKETTYPE_GUILDHOUSE_EXPIRED, OnGuildHouseExpired );
#endif // __GUILD_HOUSE_MIDDLE

	ON_MSG( PACKETTYPE_CAMPUS_ADD_MEMBER, OnAddCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, OnRemoveCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_UPDATE_POINT, OnUpdateCampusPoint );


//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

	ON_MSG( PACKETTYPE_COUPON_ITEM_NUMBER, OnCouponNumber );

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_SERIALNUMBER_REQ, OnSerialNumberReq );
	ON_MSG( PACKETTYPE_BASEGAMESETTING_REQ, OnBaseGameSettingReq );

	//협동 기부
	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTION_INFO_AND_CLEAR_EXPIRED_REQ, OnCooperativeContributions_GetContributionInfo_N_ClearExpired );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED_REQ, OnCooperativeContributions_GetContributed );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_REQ, OnCooperativeContributions_Contribute );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_REQ, OnCooperativeContributions_GetRanking );
	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM_REQ, OnCooperativeContributions_TryRewardItem );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO_REQ, OnCooperativeContributions_TryWirteItemRewardInfo );

	//ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_REQ, OnCooperativeContributions_RewardItem );
	//ON_MSG( PACKETTYPE_REWARD_ITEM_NOTI, OnRewardItem );

	//콜로세움
	ON_MSG( PACKETTYPE_RECORD_DUNGEON_PLAY_INFO, OnRecord_DungeonBestCompleteTime );
	ON_MSG( PACKETTYPE_COLOSSEUM_GET_RANKING_INFO, OnGetDungeonRankingInfo );

	ON_MSG( PACKETTYPE_TELEPORT_MAP_ADD_POINT_NOTI, OnTeleportMapAddPointNoti );		// 텔레포트 지도 유저 등록 통보(WorldServer->DatabaseServer)
	ON_MSG( PACKETTYPE_TELEPORT_MAP_CHG_POINT_NAME_NOTI, OnTeleportMapChgPointNameNoti );	// 텔레포트 지도 유저 등록 위치 이름 수정 통보(WorldServer->DatabaseServer)
	ON_MSG( PACKETTYPE_TELEPORT_MAP_DEL_POINT_NOTI, OnTeleportMapDelPointNoti );		// 텔레포트 지도 유저 등록 위치 삭제 통보(WorldServer->DatabaseServer)


	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_REQ, OnConsignmentSale_GetRegistedCount );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_REQ, OnConsignmentSale_CheckRegistedCount );
	
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGIST_REQ, OnConsignmentSale_RegistReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_REQ, OnConsignmentSale_CancelRegistedReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_REQ, OnConsignmentSale_RegistedInfoListReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_REQ, OnConsignmentSale_GetPriceReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CLEAR_RESERVE_INFO_NOTI, OnConsignmentSale_ClearReservedInfoNoti );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_REQ, OnConsignmentSale_BuyItemReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_REQ, OnConsignmentSale_CollectSaleGoldReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_SEARCH_REQ, OnConsignmentSale_SearchReq );
	ON_MSG(	PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_INFO_LIST_REQ, OnConsignmentSale_SaleGoldInfoListReq );

	//ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_USER_INFO_REQ, OnConsignmentSale_GetRegistedCount );

	ON_MSG( PACKETTYPE_CHARACTER_SERVER_TRANSFORM_TO_OTHSER_SERVER_NOTI, OnCharacterServerTransformNoti );

	ON_MSG( PACKETTYPE_GUILD_BANK_INPUT_ITEM_REQ, OnGuildBankInputItem );
	ON_MSG( PACKETTYPE_GUILD_BANK_OUTPUT_ITEM_REQ, OnGuildBankOutputItem );
//	ON_MSG( PACKETTYPE_GUILD_BANK_INPUT_GOLD_REQ, OnGuildBankInputGold );
	ON_MSG( PACKETTYPE_GUILD_BANK_OUTPUT_GOLD_REQ, OnGuildBankOutputGold );
}

CDPTrans::~CDPTrans()
{
}

CDPTrans* CDPTrans::GetInstance( void )
{
	static	CDPTrans sDPTrans;
	return &sDPTrans;
}

void CDPTrans::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
//	SYSTEMTIME st;

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

void CDPTrans::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	CAr ar( (LPBYTE)lpMsg + ( nSize + nSize ), dwMsgSize - ( nSize + nSize ) );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + nSize ), (LPBYTE)lpMsg + nSize + nSize + nSize, dwMsgSize - ( nSize + nSize + nSize ) );
	}
	else {
	
	}
}

void CDPTrans::OnAddConnection( DPID dpid )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	pov->nQueryMode	= QM_ALL_PLAYER_DATA;
	pov->dpid	= dpid;

	TCHAR szIP[16] = { 0, };
	GetPlayerAddr( pov->dpid, szIP, _countof( szIP ) );
	FLINFO_LOG( PROGRAM_NAME, _T( "Connected WorldServer IP:[%s]" ), szIP );

	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &pov->Overlapped );

	// 월드 서버에 군주 정보 전송
	CTLord::Instance()->PostRequest( CTLord::eInit, NULL, 0, dpid );

	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eTransfer, NULL, 0, dpid );

#ifdef __S1108_BACK_END_SYSTEM

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100813 코어접속 후 send 되도록...

	//SendBaseGameSetting( TRUE, lpCreatePlayer->dpId );
	//SendMonsterRespawnSetting( 0, NULL, lpCreatePlayer->dpId );
	//SendMonsterPropSetting( TRUE, lpCreatePlayer->dpId );

	//////////////////////////////////////////////////////////////////////////

#endif // __S1108_BACK_END_SYSTEM

	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, dpid, NULL );

	SendEventGeneric( dpid );
	prj.m_EventLua.m_Access.Enter();
	std::vector<BYTE> vecEventList = prj.m_EventLua.GetEventList();
	prj.m_EventLua.m_Access.Leave();
	if( !vecEventList.empty() )
	{
		std::map<BYTE, BOOL> mapEventList;
		for( DWORD i=0; i<vecEventList.size(); i++ )
			mapEventList.insert(std::make_pair( vecEventList.at( i ), TRUE ) );
		SendEventLuaState( mapEventList, FALSE, dpid );
	}

	//g_DbManager.LoadGC1to1TenderGuild();
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_LOAD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, dpid, &lpDbOverlappedPlus->Overlapped );

	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_LOAD, NULL, 0, dpid );
	CTax::GetInstance()->LoadTaxInfo( dpid );
	SendPCBangSetApply( dpid );
	GuildHouseDBMng->PostRequest( GUILDHOUSE_SEND, NULL, 0, dpid );

	CCampusHelper::GetInstance()->PostRequest( CAMPUS_SEND, 0, 0, dpid );

	FLPacketServerDescriptionNoti kPacket;
	kPacket.dwServerIndex = g_pServerDescription->GetServerIndex();
	g_pServerDescription->GetAllServerDescription( &kPacket.vecServerDesc );
	SendPacket( dpid, &kPacket );
}

void CDPTrans::OnRemoveConnection( DPID dpid )
{
	if( CQuiz::GetInstance()->IsRun() )
	{
		CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_CLOSE, 0, 0, dpid );
	}
}

void CDPTrans::SendAllGuildData( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ALL_GUILD_DATA_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	g_GuildMng.Serialize( ar, FALSE );
	g_GuildWarMng.Serialize( ar );
	SEND( ar, this, dpId );

	FLINFO_LOG( PROGRAM_NAME, _T( "[ Send Guild Data to World ]" ) );
}

void CDPTrans::SendAllGuildCombat( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	g_DbManager.SerializeGuildCombat( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendAllPlayerData( DPID dpid )
{
/*
//	BEFORESENDDUAL( ar, PACKETTYPE_ALL_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	CAr ar( 27002420 );
	int nBufSize;
	ar << DPID_UNKNOWN << DPID_UNKNOWN << PACKETTYPE_ALL_PLAYER_DATA;

	CPlayerDataCenter::GetInstance()->Serialize( ar );
	SEND( ar, this, dpid );
*/
	int nTotal	= CPlayerDataCenter::GetInstance()->size();
	int nFirst	= 0;
	int nCount	= 10000;
	while( nFirst < nTotal )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_ALL_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
		int nRet	= CPlayerDataCenter::GetInstance()->Serialize( ar, nFirst, nCount );
		nFirst	+= nRet;
		SEND( ar, this, dpid );
	}

//	FLTRACE_LOG( PROGRAM_NAME, _T( "SendAllPlayerData: nTotal = %d, nFirst = %d" ), nTotal, nFirst );
}

void CDPTrans::SendAddPlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( pPlayerData->szPlayer );
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendDeletePlayerData( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DELETE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESENDDUAL( ar, dwHdr, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpid );
}

void	CDPTrans::SendPacket( DPID dwDPID, const FLPacket* pPacket )
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
	Send( lpBuf, nBufSize, dwDPID );
}

void CDPTrans::SendUpdateGuildRankFinish()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING_END, DPID_UNKNOWN, DPID_UNKNOWN );
	CGuildRank::Instance()->Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendBaseGameSetting( BOOL bFirst, DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_BASEGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bFirst;
	ar << prj.m_bBaseGameSetting;
	if( prj.m_bBaseGameSetting )
	{
		ar << prj.m_fMonsterExpRate	<< prj.m_fGoldDropRate << prj.m_fItemDropRate << prj.m_fMonsterHitRate;
		ar << prj.m_fShopCost << prj.m_fMonsterRebirthRate << prj.m_fMonsterHitpointRate << prj.m_fMonsterAggressiveRate << prj.m_fMonsterRespawnRate;
	}
//	SEND( ar, this, DPID_ALLPLAYERS );
	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterRespawnSetting( int nRemoveSize, int nRemoveRespawn[], DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_MONSTERRESPAWNSETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	if( nRemoveRespawn == NULL )
	{
		ar << prj.m_nMonsterRespawnSize;
		for( DWORD i = 0 ; i < prj.m_nMonsterRespawnSize ; ++i )
		{
			ar << prj.m_aMonsterRespawn[ i ].nRespawnIndex;
			ar.WriteString( prj.m_aMonsterRespawn[ i ].szMonsterName );
			ar << prj.m_aMonsterRespawn[ i ].vPos;
			ar << prj.m_aMonsterRespawn[ i ].nQuantity;
			ar << prj.m_aMonsterRespawn[ i ].nAggressive;
			ar << prj.m_aMonsterRespawn[ i ].nRange;
			ar << prj.m_aMonsterRespawn[ i ].nTime;
		}
		ar << static_cast<int>( 0 );
	}
	else
	{
		ar << prj.m_nAddMonsterRespawnSize;
		for( int i = 0 ; i < prj.m_nAddMonsterRespawnSize ; ++i )
		{
			ar << prj.n_aAddRespawn[ i ].nRespawnIndex;
			ar.WriteString( prj.n_aAddRespawn[ i ].szMonsterName );
			ar << prj.n_aAddRespawn[ i ].vPos;
			ar << prj.n_aAddRespawn[ i ].nQuantity;
			ar << prj.n_aAddRespawn[ i ].nAggressive;
			ar << prj.n_aAddRespawn[ i ].nRange;
			ar << prj.n_aAddRespawn[ i ].nTime;
		}

		ar << nRemoveSize;
		for( int i = 0 ; i < nRemoveSize ; ++i )
		{
			ar << nRemoveRespawn[i];
		}
	}

	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterPropSetting( BOOL bFirst, DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_MONSTERPROPGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	if( bFirst == TRUE )
	{
		ar << prj.m_nMonsterPropSize;
		for( DWORD i = 0 ; i < prj.m_nMonsterPropSize ; ++i )
		{
			ar.WriteString( prj.m_aMonsterProp[ i ].szMonsterName );
			ar << prj.m_aMonsterProp[ i ].nHitPoint;
			ar << prj.m_aMonsterProp[ i ].nAttackPower;
			ar << prj.m_aMonsterProp[ i ].nDefence;
			ar << prj.m_aMonsterProp[ i ].nExp;
			ar << prj.m_aMonsterProp[ i ].nItemDrop;
			ar << prj.m_aMonsterProp[ i ].nPenya;
		}
	}
	else
	{
		ar << prj.m_nAddMonsterPropSize;
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			ar.WriteString( prj.m_aAddProp[ i ].szMonsterName );
			ar << prj.m_aAddProp[ i ].nHitPoint;
			ar << prj.m_aAddProp[ i ].nAttackPower;
			ar << prj.m_aAddProp[ i ].nDefence;
			ar << prj.m_aAddProp[ i ].nExp;
			ar << prj.m_aAddProp[ i ].nItemDrop;
			ar << prj.m_aAddProp[ i ].nPenya;
		}
	}
	
	ar << prj.m_nRemoveMonsterPropSize;
	for( int i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aRemoveProp[i] );
	}

	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendGMChat( int nCount )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_GAMEMASTER_CHATTING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCount;
	for( int i = 0 ; i < nCount ; ++i )
	{
		ar.WriteString( prj.m_chGMChat[i] );
	}
	SEND( ar, this, DPID_ALLPLAYERS );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::OnJoin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// PACKETTYPE_JOIN\t// %d" ), dpid );

	//CAr ar1;
	//ar1 << PACKETTYPE_JOIN;
	//ar1.Write( lpBuf, uBufSize );
	//ar1 << dpid << dpidCache << dpidUser;


	//ar1 << static_cast<DWORD>( 0 );		//@@@@@@@@@@@@@@ ?????????????????

	//u_long nBlockSize;
	//LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	//g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );

	FLPacketWtoDB_ChannelJoinReq fromW;
	fromW.Deserialize( ar );

	FLPacketDBtoA_ChannelJoinReq toA;
	toA.m_kAccount				= fromW.m_kAccount;
	toA.m_kDpids.dpid			= dpid;
	toA.m_kDpids.dpidCache		= dpidCache;
	toA.m_kDpids.dpidUser		= dpidUser;	
	
	g_dpAccountClient.SendPacket( &toA );
}

void CDPTrans::OnSavePlayer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHAR sAccount[MAX_ACCOUNT]	= { 0, };
	DWORD dwWorldID;
	D3DXVECTOR3 vPos;
	BOOL bLogout = FALSE;
	CMover* pMover	= NULL;
	BYTE byDBDataSlot;
	ACCOUNT_CACHE*	pCache;

	ar >> bLogout;
	ar.ReadString( sAccount, _countof( sAccount ) );
	ar >> byDBDataSlot;
	ar >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	const BYTE byProgramDataSlot = GET_PLAYER_SLOT( byDBDataSlot );

	g_DbManager.m_AddRemoveLock.Enter();
	pCache	= g_DbManager.m_AccountCacheMgr.Find( sAccount );
	if( pCache )
	{
		pMover	= pCache->pMover[byProgramDataSlot];
	}

	if( pCache == NULL || pMover == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Cache Hit Failed, AccountCahce = %p, pMover = %p, Account = %s, byDBDataSlot = %d" )
			, pCache, pMover, sAccount, byDBDataSlot );
		g_DbManager.m_AddRemoveLock.Leave();
		return;
	}


	pCache->m_tmLastAccess	= ::time( NULL );

	pMover->Lock();
	pMover->Serialize( ar, METHOD_NONE );
	pMover->m_fAngle	= 0.0f;
	pMover->m_UserTaskBar.Serialize( ar );
	DWORD dwState;
	ar >> dwState;
	pMover->m_RTMessenger.SetState( dwState );

	ar >> pMover->m_dwReturnWorldID;
	ar >> pMover->m_vReturnPos;

	pMover->m_dwWorldID		= dwWorldID;
	pMover->m_vPos	= vPos;
#ifdef __LAYER_1015
	pMover->SetLayer( nLayer );
#endif	// __LAYER_1015
	pMover->m_dwMode	&= ~FRESH_MODE;	// ata2k

	ar >> pMover->m_nEventFlag;
	ar >> pMover->m_dwEventTime;
	ar >> pMover->m_dwEventElapsed;
	ar >> pMover->m_nExpLog;
	ar >> pMover->m_nAngelExpLog;

	ar >> pMover->m_nCoupon;

	ar >> pMover->m_nRestPoint;
	pMover->m_tLogOut = time_null();

	for( int j = 0 ; j < MAX_CHARACTER_SLOT; ++j )
	{
		pCache->m_Bank[j].Copy( pMover->m_Bank[j] );
		pCache->m_idPlayerBank[j] = pMover->m_idPlayerBank[j];
		pCache->m_dwGoldBank[j] = pMover->m_dwGoldBank[j];
	}

	if( bLogout )
	{
		pMover->m_dwPeriodicTick	= 0;
	}

	CMover tMoverTmp;
	tMoverTmp.Copy( pMover, TRUE );

	pMover->PeriodTick();

	pMover->Unlock();

	g_DbManager.m_AddRemoveLock.Leave();

	tMoverTmp.m_bLogout	= bLogout;
	FLStrcpy( tMoverTmp.m_szAccount, _countof( tMoverTmp.m_szAccount ), sAccount );

	CAr arRestPoint;
	arRestPoint << tMoverTmp.m_idPlayer << tMoverTmp.m_nRestPoint << tMoverTmp.m_tLogOut;
	u_long nBufRestPointSize;
	LPBYTE lpBufRestPoint = arRestPoint.GetBuffer( &nBufRestPointSize );
	GuildHouseDBMng->PostRequest( GUILDHOUSE_SAVE_RESTPOINT, lpBufRestPoint, nBufRestPointSize );

	CHARACTER_DB().PostQueryCommand( tMoverTmp.m_szAccount[0], new FLQuery_SavePlayer( dpid, dpidCache, dpidUser, lpBuf, uBufSize, &tMoverTmp ) );
}

void CDPTrans::OnSaveConcurrentUserNumber( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );

	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogPlayConnect( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PLAY_CONNECT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnLogPlayDeath( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PLAY_DEATH;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogLevelUp( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_LEVELUP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogServerDeath( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SERVER_DEATH;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void	CDPTrans::OnLogGetHonorTime( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= LOG_GETHONORTIME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogUniqueItem( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_UNIQUEITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogTradeItem( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_TRADE_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogItem( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogQuest( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_QUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnChangeBankPass( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= CHANGE_BANKPASS;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnGammaChat( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GAMEMA_CHAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );		
}

void CDPTrans::OnLogPkPvp( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PK_PVP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogSchool( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SCHOOL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogSkillPoint( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SKILLPOINT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGuildBank( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_QUERY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildRanking( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_RANKING_UPDATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildRankingDB( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_RANKING_UPDATE_DB;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnGuildBankUpdate( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_UPDAATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGuildRealPay( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "D CDPCoreSrvr::OnGuildRealPay" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_REALPAY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGuildContribution( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_CONTRIBUTION;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnSchoolReport( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SCHOOL_REPORT;
	PostQueuedCompletionStatus(  g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnBuyingInfo( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// recv from world" ) );

	BUYING_INFO2 bi2;
	SERIALNUMBER iSerialNumber = 0;

	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	ar >> iSerialNumber;

	g_dpAccountClient.SendBuyingInfo( &bi2, iSerialNumber );
}

void CDPTrans::OnQueryGuildQuest( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->nQueryMode	= QM_QUERYGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnInsertGuildQuest( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_INSERTGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildQuest( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_UPDATEGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnQuerySetPlayerName( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// PACKETTYPE_QUERYSETPLAYERNAME" ) );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SET_PLAYER_NAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnItemTBLUpdate( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( prj.m_bItemUpdate == FALSE )
		return;

	int nQuestCount;
	ar >> nQuestCount;

	int nQuestID;
	char szQuestName[256];
	for( int i = 0 ; i < nQuestCount ; ++i )
	{
		ZeroMemory( szQuestName, sizeof( szQuestName ) );
		ar >> nQuestID;
		ar.ReadString( szQuestName, _countof( szQuestName ) );
		g_DbManager.m_int2StrItemUpdate.insert( std::map<int, std::string>::value_type( nQuestID, szQuestName ) );
	}
	g_DbManager.m_nItemUpdate = 1;
}

void CDPTrans::OnPing( CAr & ar, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	time_t tSend;
	ar >> tSend;

	{
		BEFORESENDDUAL( arSend, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
		arSend << tSend << time( NULL );
		SEND( arSend, this, dpid );
	}
}

void CDPTrans::OnQueryMailBox( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_QUERY_MAIL_BOX;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnQueryMailBoxCount( CAr & ar, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long idReceiver;
	int nCount;
	ar >> idReceiver >> nCount;

	CPost *pPost    = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );

	CMailBox* pMailBox	= NULL;
	pMailBox = pPost->GetMailBox( idReceiver );
	
	if( pMailBox != NULL )
	{
		SendMailBox( pMailBox, dpid );
	}
	else
	{
		SendMailBoxReq( idReceiver, dpid, FALSE, pMailBox );
	}

	return;
}

void CDPTrans::OnQueryMailBoxReq( CAr & ar, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long idReceiver;
	ar >> idReceiver;

	CPost *pPost    = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );

	CMailBox* pMailBox	= NULL;
	pMailBox = pPost->GetMailBox( idReceiver );

	BOOL	bHaveMailBox = FALSE;
	if( pMailBox != NULL )
	{
		bHaveMailBox = TRUE;
	}

	SendMailBoxReq( idReceiver, dpid, bHaveMailBox, pMailBox );

	return;
}

void CDPTrans::SendMailBox( CMailBox* pMailBox, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pMailBox->m_idReceiver;
	pMailBox->Write( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendMailBoxReq( u_long idReceiver, DPID dpid, BOOL bHaveMailBox, CMailBox* pMailBox )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	ar << bHaveMailBox;
	if( pMailBox != NULL )
	{
		pMailBox->Write( ar );
	}
	SEND( ar, this, dpid );
}

void CDPTrans::OnQueryPostMail( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryRemoveMail( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGetMailItem( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGetMailGold( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL_GOLD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnReadMail( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= READ_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendPostMail( BOOL bResult, u_long idReceiver, CMail* pMail )
{
// 	//	BEGINTEST
// 	FLINFO_LOG( PROGRAM_NAME, _T( "Result[%d], Receiver[%d]" ), bResult, idReceiver );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bResult << idReceiver;
	pMail->Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendRemoveMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGetMailItem( u_long idReceiver, u_long nMail, u_long uQuery )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << uQuery;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGetMailGold( u_long idReceiver, u_long nMail, u_long uQuery )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << uQuery;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendReadMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendAllMail( DPID dpid )
{
//	BEFORESENDDUAL( ar, PACKETTYPE_ALLMAIL, DPID_UNKNOWN, DPID_UNKNOWN );

	CAr ar( 5100016 );

	u_long nBufSize;
	ar << DPID_UNKNOWN << DPID_UNKNOWN << PACKETTYPE_ALLMAIL;

	CPost::GetInstance()->m_csPost.Enter();
	CPost::GetInstance()->Serialize( ar, FALSE );
	CPost::GetInstance()->m_csPost.Leave();

	SEND( ar, this, dpid );
}

void CDPTrans::OnPreventLogin( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwPreventTime;
	char  szAccount[MAX_ACCOUNT]	= { 0, };

	ar.ReadString( szAccount, _countof( szAccount ) );
	ar >> dwPreventTime;

	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE *pCache = g_DbManager.m_AccountCacheMgr.Find( szAccount );
	if( pCache )
		pCache->m_tmPrevent = time( NULL ) + (long)dwPreventTime;
	g_DbManager.m_AddRemoveLock.Leave();
}

void CDPTrans::OnGuidCombatInGuild( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= IN_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGuidCombatOutGuild( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= OUT_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnStartGuildCombat( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= START_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnResultGuildCombat( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= RESULT_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCGetPenyaGuild( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GETPENYAGUILD_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCGetPenyaPlayer( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GETPENYAPLAYER_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCContinue( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CONTINUE_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << dwPenya;
	ar << dwExistingPenya;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendOutGuildCombat( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendResultGuildCombat( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_DbManager.m_nGuildCombatIndex;
	ar << g_DbManager.m_uWinGuildId;
	ar << g_DbManager.m_nWinGuildCount;
	ar << g_DbManager.m_uBestPlayer;
	g_DbManager.SerializeResultValue( ar );
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendPlayerPointGuildCombat( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	g_DbManager.SerializePlayerPoint( ar );
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendGetPenyaGuildGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendGetPenyaPlayerGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::SendContinueGC()
{
	BEFORESENDDUAL( ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::SendQueryRemoveGuildBankTbl( int nNo, u_long idGuild, DWORD dwItemId, SERIALNUMBER iSerialNumber, DWORD dwItemNum )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nNo << idGuild << dwItemId << iSerialNumber << dwItemNum;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::OnQueryRemoveGuildBankTbl( CAr & /*ar*/, DPID, DPID, DPID, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_DELETE_REMOVE_GUILD_BANK_TBL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendEventGeneric( DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT_GENERIC, DPID_UNKNOWN, DPID_UNKNOWN );
	CEventGeneric::GetInstance()->Serialize( ar );
	ar << CEventGeneric::GetInstance()->GetFlag();
	SEND( ar, this, dpid );
}

void CDPTrans::SendEventFlag( DWORD dwFlag )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT_FLAG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwFlag;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendEventLuaState( std::map<BYTE, BOOL> mapState, BOOL bTextOut, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bTextOut;
	ar << mapState.size();
	for( std::map<BYTE, BOOL>::iterator it = mapState.begin(); it != mapState.end(); ++it )
		ar << it->first << it->second;
	SEND( ar, this, dpId );
}

void CDPTrans::SendEventLuaChanged( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnEventLuaChanged( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	prj.m_EventLua.m_Access.Enter();
	prj.m_EventLua.LoadScript();
	SendEventLuaChanged();
	if( prj.m_EventLua.CheckEventState() )
		SendEventLuaState( prj.m_EventLua.m_mapState, FALSE );
	prj.m_EventLua.m_Access.Leave();
}

void CDPTrans::OnEveRecommend( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= RECOMMEND_EVE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnCalluspLoggingQuest( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_USPLOGGINGQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnCalluspXXXMultiServer( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	char	szAccount[MAX_ACCOUNT];
	u_long uKey, idPlayer;
	ar >> uKey >> idPlayer;
	ar.ReadString( szAccount, _countof( szAccount ) );
	if( uKey > 0 )
	{
		if( idPlayer > 0 )
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer( szAccount, uKey );
		else
			g_DbManager.m_AccountCacheMgr.InitMultiServer( uKey );
	}

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_XXX_MULTI_SERVER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );

	if( idPlayer > 0 )
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		if( pPlayerData )
		{
			pPlayerData->data.uLogin	= (BYTE)( uKey );
			pPlayerData->data.nVer++;
			SendUpdatePlayerData( idPlayer, pPlayerData );
			g_dpCoreSrvr.SendUpdatePlayerData( idPlayer, pPlayerData );
		}
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	}
}

void CDPTrans::OnLogExpBox( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_LOG_EXPBOX;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnCalluspPetLog( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_USP_PET_LOG;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendGC1to1Open( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGC1to1TenderGuild( std::vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecT, std::vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecF, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_TENDERTOSRVR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << vecT.size();
	for( DWORD i=0; i<vecT.size(); i++ )
	{
		ar << vecT.at( i ).ulGuildId << vecT.at( i ).nPenya;
		ar << vecT.at( i ).vecMemberId.size();
		for( DWORD j=0; j<vecT.at( i ).vecMemberId.size(); j++ )
			ar << vecT.at( i ).vecMemberId.at( j );
	}

	ar << vecF.size();
	for( DWORD i=0; i<vecF.size(); i++ )
		ar << vecF.at( i ).ulGuildId << vecF.at( i ).nPenya;
	SEND( ar, this, dpId );
}

void CDPTrans::OnGC1to1StateToDBSrvr( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_STATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1Tender( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_TENDER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1LineUp( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_LINEUP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1WarPerson( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_WARPERSON;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1WarGuild( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_WARGUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdatePlayerData( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long idPlayer;
	sPlayerData	data;
	ar >> idPlayer;
	ar.Read( &data, sizeof(sPlayerData) );

	CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
	pPlayerDataCenter->m_Access.Enter();
	PlayerData* pPlayerData		= pPlayerDataCenter->GetPlayerData( idPlayer );
	if( !pPlayerData )
	{
		pPlayerDataCenter->m_Access.Leave();
		return;
	}
	
	pPlayerData->data.nJob	= data.nJob;
	pPlayerData->data.nLevel	= data.nLevel;
	pPlayerData->data.nSex	= data.nSex;
	pPlayerData->data.nVer++;
	PlayerData pd;
	memcpy( &pd, pPlayerData, sizeof(PlayerData) );
	pPlayerDataCenter->m_Access.Leave();
	SendUpdatePlayerData( idPlayer, &pd );
	g_dpCoreSrvr.SendUpdatePlayerData( idPlayer, &pd );
}

void CDPTrans::OnGuildBankLogView( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_GUILDBANK_LOG_VIEW;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnSealChar( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHAR;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharConm( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARCONM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharGet( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARGET;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharSet( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARSET;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate, BOOL bRet )
{	// 월드 서버에 군주입찰 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iDeposit << tCreate << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionSetPledge( u_long idPlayer, const char* szPledge, BOOL bRet )
{	// 월드 서버에 공약 설정 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPledge );
	ar << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionIncVote( u_long idPlayer, u_long idElector, BOOL bRet )
{	// 월드 서버에 투표 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idElector << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionBeginCandidacy( void )
{	// 월드 서버에 입후보 시작 상태 전송
	SendHdr( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionBeginVote( int nRequirement )
{	// 월드 서버에 투표 시작 상태 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nRequirement;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionEndVote( u_long idPlayer )
{	// 월드 서버에 투표 종료 상태 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnElectionAddDeposit( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 입찰 처리를 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionAddDeposit()" ) );
	CTLord::Instance()->PostRequest( CTLord::eAddDeposit, lpBuf, uBufSize );
}

void CDPTrans::OnElectionSetPledge( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 공약 설정 처리를 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionSetPledge()" ) );
	CTLord::Instance()->PostRequest( CTLord::eSetPledge, lpBuf, uBufSize );
}

void CDPTrans::OnElectionIncVote( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 투표 처리를 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionIncVote()" ) );
	CTLord::Instance()->PostRequest( CTLord::eIncVote, lpBuf, uBufSize );
}

void CDPTrans::OnLEventCreate( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 이벤트 시작을 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnLEventCreate()" ) );
	CTLord::Instance()->PostRequest( CTLord::eLEventCreate, lpBuf, uBufSize );
}

void CDPTrans::OnLordSkillUse( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 스킬 사용을 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnLordSkillUse()" ) );
	CTLord::Instance()->PostRequest( CTLord::eLordSkillUse, lpBuf, uBufSize );
}

void CDPTrans::OnLEventInitialize( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 이벤트 초기화를 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnLEventInitialize()" ) );
	CTLord::Instance()->PostRequest( CTLord::eLEventInitialize, lpBuf, uBufSize );
}								 

// operator commands
void CDPTrans::OnElectionProcess( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	// 군주 프로세스 제어 처리
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionProcess()" ) );
	BOOL bRun;
	ar >> bRun;
	CTElection* pElection	=static_cast<CTElection*>( CTLord::Instance()->GetElection() );
	pElection->SetRun( bRun );
}
void CDPTrans::OnElectionBeginCandidacy( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	// 군주 스레드에 입후보 시작을 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionBeginCandidacy()" ) );
	CTLord::Instance()->PostRequest( CTLord::eBeginCandidacy );
}
void CDPTrans::OnElectionBeginVote( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	// 군주 스레드에 투표 시작을 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionBeginVote()" ) );
	CTLord::Instance()->PostRequest( CTLord::eBeginVote );
}
void CDPTrans::OnElectionEndVote( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	// 군주 스레드에 투표 종료를 요청
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans::OnElectionEndVote()" ) );
	CTLord::Instance()->PostRequest( CTLord::eEndVote );
}

void CDPTrans::SendLord( DPID dpid )
{	// 모든 군주 시스템 정보를 월드 서버에 전송
	BEFORESENDDUAL( ar, PACKETTYPE_LORD, DPID_UNKNOWN, DPID_UNKNOWN );
	CTLord::Instance()->Serialize( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendLEventCreate( CLEComponent* pComponent, BOOL bResult )
{	// 월드 서버에 군주 이벤트 현재 상태를 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	pComponent->Serialize( ar );
	ar << bResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLEventInitialize( void )
{	// 월드 서버에 군주 이벤트 초기화를 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill, int nRet )
{	// 월드 서버에 군주 스킬 사용 결과를 전송
	// 결과가 참이면 월드 서버는 실제 군주 스킬 효과를 적용
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idTarget << nSkill << nRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLordSkillTick( CLordSkill* pSkills )
{	// 해당 군주 스킬의 재사용 대기 시간을 전송
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_TICK, DPID_UNKNOWN, DPID_UNKNOWN );
	pSkills->SerializeTick( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLEventTick( ILordEvent* pEvent )
{	// 해당 군주 이벤트의 남아있는 틱을 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_TICK, DPID_UNKNOWN, DPID_UNKNOWN );
	pEvent->SerializeTick( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendTaxInfo( DPID dpId, BOOL bConnect, BOOL bToAllClient )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_ALLINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	CTax::GetInstance()->Serialize( ar );
	ar << bConnect;
	if( bConnect )
	{
		ar << CTax::GetInstance()->m_nMinTaxRate;
		ar << CTax::GetInstance()->m_nMaxTaxRate;
		ar << CTax::GetInstance()->m_vecdwOccupationShopItem.size();
		for( size_t i=0; i<CTax::GetInstance()->m_vecdwOccupationShopItem.size(); i++ )
			ar << CTax::GetInstance()->m_vecdwOccupationShopItem[i];
	}
	ar << bToAllClient;
	SEND( ar, this, dpId );
}

void CDPTrans::OnSetSecretRoomWinGuild( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_SECRETROOMWINGUILD, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnSetLord( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_LORD, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnTaxRate( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_TAXRATE, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnAddTax( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_ADDTAX, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnApplyTaxRateNow( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CTax::GetInstance()->SetApplyTaxRateNow();
}

void	CDPTrans::OnUpdateTaxRate( CAr & ar, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nTaxRate = 0;
	ar >> nTaxRate;
	CTax::GetInstance()->UpdateTaxRate( nTaxRate );
}

void CDPTrans::OnSecretRoomInsertToDB( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_TENDER_INSERT, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomUpdateToDB( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_TENDER_UPDATE, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomLineUpToDB( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_LINEUP_INSERT, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomClosed( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_CLOSED, lpBuf, uBufSize );
}

void CDPTrans::SendSecretRoomInfoClear( DPID dpId )
{	
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_INFO_CLEAR, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendSecretRoomTenderInfo( BYTE nContinent, DWORD dwGuildId, int nPenya, std::vector<DWORD> & vecMemberId, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << dwGuildId << nPenya;
	ar << vecMemberId.size();
	for( DWORD i=0; i<vecMemberId.size(); i++ )
		ar << vecMemberId.at( i );
	SEND( ar, this, dpId );
}

void CDPTrans::SendRainbowRaceInfo( std::vector<DWORD> & vec_dwNowPlayerId, std::vector<DWORD> & vec_prevRanking, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << vec_dwNowPlayerId.size();
	for( DWORD i=0; i<vec_dwNowPlayerId.size(); i++ )
		ar << vec_dwNowPlayerId[i];
	ar << vec_prevRanking.size();
	for( DWORD i=0; i<vec_prevRanking.size(); i++ )
		ar << vec_prevRanking[i];
	SEND( ar, this, dpId );
}

void CDPTrans::OnRainbowRaceLoadInfo( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_LOAD, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnRainbowRaceApplication( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_APPLICATION, lpBuf, uBufSize );
}

void CDPTrans::OnRainbowRaceFailedUser( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_FAILEDUSER, lpBuf, uBufSize );
}
void CDPTrans::OnRainbowRaceRanking( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_RANKING, lpBuf, uBufSize );
}

void CDPTrans::SendHousingLoadInfo( DWORD dwPlayerId, CHousing* pHousing, DPID dpId )
{
	FLASSERT( pHousing );
	if( pHousing == NULL )
	{
		return;
	}

	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_LOADINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	pHousing->Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingFurnitureList( DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_FURNITURELIST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	housingInfo.Serialize( ar );
	ar << bAdd;
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingSetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETUPFURNITURE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	housingInfo.Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingSetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwTargetId << bAllow;
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingDBFailed( DWORD dwPlayerId, DWORD dwItemId, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_DBFAILED, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwItemId;
	SEND( ar, this, dpId );
}

void CDPTrans::OnHousingLoadInfo( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_LOAD, lpBuf, uBufSize, dpid );
}
void CDPTrans::OnHousingReqSetFurnitureList( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_INSERT_LIST, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqSetupFurniture( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_SETUP_FURNITURE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqSetVisitAllow( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_SET_VISITALLOW, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqGMRemoveAll( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_GM_REMOVEALL, lpBuf, uBufSize );
}

void CDPTrans::OnPropose( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans.OnPropose" ) );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::ePropose, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnCouple( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans.OnCouple" ) );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eCouple, lpBuf, uBufSize );
}

void CDPTrans::OnDecouple( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans.OnDecouple" ) );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eDecouple, lpBuf, uBufSize );
}

void CDPTrans::OnClearPropose( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans.OnClearPropose" ) );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eClearPropose, lpBuf, uBufSize );
}

void CDPTrans::OnQueryAddCoupleExperience( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CDPTrans.OnQueryAddCoupleExperience" ) );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eAddExperience, lpBuf, uBufSize );
}

void CDPTrans::SendAddCoupleExperience( u_long idPlayer, int nExperience )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nExperience;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendCouple( CCoupleMgr* /*pMgr*/, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ALL_COUPLES, DPID_UNKNOWN, DPID_UNKNOWN );
	CCoupleHelper::Instance()->Serialize( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendProposeResult( u_long idProposer, u_long idTarget, int nResult, time_t t, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idProposer << idTarget << nResult << t;
	SEND( ar, this, dpid );
}

void CDPTrans::SendCoupleResult( u_long idProposer, u_long idTarget, int nResult )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idProposer << idTarget << nResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendDecoupleResult( u_long idPlayer, int nResult )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnFunnyCoinReqUse( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CFunnyCoinDbMng::GetInstance()->PostRequest( CFunnyCoinDbCtrl::FC_USE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendFunnyCoinAckUse( DWORD dwPlayerId, DWORD dwItemId, SERIALNUMBER iSerialNumber, int nResult, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FUNNYCOIN_ACK_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwItemId << iSerialNumber << nResult;
	SEND( ar, this, dpId );
}

void CDPTrans::SendPCBangSetApply( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PCBANG_SETAPPLY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_bPCBangApply;
	SEND( ar, this, dpId );
}

void CDPTrans::OnTimeLimitInfoReq( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CAccountTimeLimitDBMng::GetInstance()->PostRequest( QUERY_TIMELIMIT_LOAD, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendTimeLimitAck( DWORD dwPlayerId, int nPlayTime, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << nPlayTime;
	SEND( ar, this, dpId );
}

void CDPTrans::OnTimeLimitUpdate( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CAccountTimeLimitDBMng::GetInstance()->PostRequest( QUERY_TIMELIMIT_UPDATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendTimeLimitReset()
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_RESET, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnLogInstanceDungeon( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_INSTANCEDUNGEON;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnQuizEventOpen( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_OPEN, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventState( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_STATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventEntrance( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_ENTRANCE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventSelect( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_SELECT, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnPostPrizeItem( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_POST_PRIZE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendQuizEventOpen( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizList( DPID dpId, CQuiz::QUIZLIST & QL, int nSize )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_LOADQUIZ, DPID_UNKNOWN, DPID_UNKNOWN );
	QL.Serialize( ar );
	ar << nSize;
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizEventNotice( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_NOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizEventChanged()
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnErrorLog( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_ERROR;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnBuyGuildHouse( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_CREATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHousePacket( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnLogGuildFurniture( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_GUILDFURNITURE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPTrans::OnGuildHouseTenderJoin( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_TENDER_JOIN, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHouseLevelUpdate( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_LEVEL_UPDATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHouseTenderState( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_TENDER_STATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHouseExpired( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_SET_EXPIRED, lpBuf, uBufSize, dpid );
}
#endif // __GUILD_HOUSE_MIDDLE

void CDPTrans::OnAddCampusMember( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_ADD_MEMBER, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnRemoveCampusMember( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_REMOVE_MEMBER, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnUpdateCampusPoint( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_UPDATE_POINT, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendAllCampus( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ALL, DPID_UNKNOWN, DPID_UNKNOWN );
	CCampusHelper::GetInstance()->Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendAddCampusMember( u_long idCampus, u_long idMaster, u_long idPupil )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ADD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idMaster << idPupil;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendRemoveCampusMember( u_long idCampus, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_REMOVE_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_UPDATE_POINT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nCampusPoint;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void	CDPTrans::SendDungeonBestCompleteTimeAck( const DPID dpID, const T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO & toWorld )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECORD_DUNGEON_PLAY_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &toWorld, sizeof( toWorld ) );
	SEND( ar, CDPTrans::GetInstance(), dpID );
}

//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

void	CDPTrans::OnCouponNumber( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= QM_QUERY_COUPON_NUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void	CDPTrans::SendCouponNumber( const PT_PACKET_COUPON_NUMBER ptCouponNumber, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPON_ITEM_NUMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)ptCouponNumber, sizeof( T_PACKET_COUPON_NUMBER ) );
	SEND( ar, this, dpid );
}

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////


void	CDPTrans::OnSerialNumberReq( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= QM_QUERY_SERIALNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void	CDPTrans::SendSerialNumberAck( const PT_PACKET_SERIALNUMBER_ACK ptSerialNumber, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SERIALNUMBER_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( ptSerialNumber, sizeof( T_PACKET_SERIALNUMBER_ACK ) );
	SEND( ar, this, dpid );
}

void	CDPTrans::OnBaseGameSettingReq( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	SendBaseGameSetting( TRUE, dpid );
	SendMonsterRespawnSetting( 0, NULL, dpid );
	SendMonsterPropSetting( TRUE, dpid );
	FLSpec::GetInstance().TrySend_CooperativeContributions( dpid );
}


//void CDPTrans::OnCooperativeContributions_GetContributionInfo_N_ClearExpired( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
//{
//	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTION_INFO_AND_CLEAR_EXPIRED, g_DbManager.m_hIOCPGuild, 1, NULL
//		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
//}

void CDPTrans::OnCooperativeContributions_Contribute( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE, g_DbManager.m_hIOCPGuild, 1, NULL
		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void CDPTrans::OnCooperativeContributions_GetContributed( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED, g_DbManager.m_hIOCPGuild, 1, NULL
		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void CDPTrans::OnCooperativeContributions_GetRanking( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_GET_RANKING, g_DbManager.m_hIOCPGuild, 1, NULL
		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

//void CDPTrans::OnCooperativeContributions_TryRewardItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
//{
//	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM, g_DbManager.m_hIOCPGuild, 1, NULL
//		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
//}
//

void CDPTrans::OnCooperativeContributions_TryWirteItemRewardInfo( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO, g_DbManager.m_hIOCPGuild, 1, NULL
		, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}



//
//void CDPTrans::OnRewardItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
//{
//	PostOverlappedPlus( QM_REWARD_ITEM, g_DbManager.m_hIOCPGuild, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
//}

void CDPTrans::OnRecord_DungeonBestCompleteTime( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_RECORD_DUNGEON_BEST_COMPLETE_TIME, g_DbManager.m_hIOCPGuild, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void CDPTrans::OnGetDungeonRankingInfo( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_RECORD_GET_DUNGEON_RANKING, g_DbManager.m_hIOCPGuild, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void	CDPTrans::OnTeleportMapAddPointNoti( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_TELEPORT_MAP_ADD_POINT, g_DbManager.m_hIOCPPut, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void	CDPTrans::OnTeleportMapChgPointNameNoti( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_TELEPORT_MAP_CHG_POINT_NAME, g_DbManager.m_hIOCPPut, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void	CDPTrans::OnTeleportMapDelPointNoti( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_TELEPORT_MAP_DEL_POINT, g_DbManager.m_hIOCPPut, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}


void CDPTrans::OnConsignmentSale_GetRegistedCount( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	CAr recv( lpBuf, uBufSize );
	FLPacketWDB_GetRegistedCountReq req;
	req.Deserialize( recv );

	DB_CONSIGNMENT_SALE()._GetRegistedCount_AndSendRegistedCount( dpid, req.m_uPlayerID );
}


void CDPTrans::OnConsignmentSale_CheckRegistedCount( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;
	
	CAr recv( lpBuf, uBufSize );
	FLPacketWDB_CheckRegistedCountReq req;
	req.Deserialize( recv );

	FLDBConsignmentSale::FLRegistCount kRegistCount = DB_CONSIGNMENT_SALE()._SendGetRegistedCount( dpid, req.m_uPlayerID, req );

	//FLPacketWDB_CheckRegistedCountAck ack;
	//ack.m_uPlayerID		= req.m_uPlayerID;
	//ack.m_dwDefaultRegistCount	= kRegistCount.arrCount[ DEFAULT_REGIST_MODE ];
	//ack.m_dwItemRegistCount		= kRegistCount.arrCount[ ITEM_REGIST_MODE ];
	//	
	//ack.m_dwItemObjID			= req.m_dwItemObjID;
	//ack.m_nItemCost				= req.m_nItemCost;
	//ack.m_nItemCount			= req.m_nItemCount;

	//CDPTrans::GetInstance()->SendPacket( dpid, &ack );
}

void CDPTrans::OnConsignmentSale_RegistReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_RegistReq req;
	req.Deserialize( ar );

	DEBUG_ASSERT( req.m_szPlayerName[ 0 ] != '\0' );
	CHARACTER_DB().PostQueryCommand( req.m_szPlayerName[ 0 ], new FLQuery_Regist( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void CDPTrans::OnConsignmentSale_CancelRegistedReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_CancelRegistedReq req;
	req.Deserialize( ar );

	DEBUG_ASSERT( req.m_szPlayerName[ 0 ] != '\0' );
	CHARACTER_DB().PostQueryCommand( req.m_szPlayerName[ 0 ], new FLQuery_CancelRegisted( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void CDPTrans::OnConsignmentSale_RegistedInfoListReq( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	CAr recv( lpBuf, uBufSize );
	FLPacketWDB_RegistedInfoListReq req;
	req.Deserialize( recv );

	FLPacketWDB_RegistedInfoListAck ack;
	ack.m_uPlayerID				= req.m_uPlayerID;
	DB_CONSIGNMENT_SALE()._GetRegistedInfoList( ack, req.m_uPlayerID );

	CDPTrans::GetInstance()->SendPacket( dpid, &ack );
}

void CDPTrans::OnConsignmentSale_GetPriceReq( CAr & /*ar*/, DPID dpid, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

    CAr recv( lpBuf, uBufSize );
	FLPacketWDB_GetPriceReq req;
	req.Deserialize( recv );

	FLPacketWDB_GetPriceAck ack;
	ack.m_eResult			= FLPacketWDB_GetPriceAck::E_SUCCESS;
	ack.m_uPlayerID			= req.m_uPlayerID;
	ack.m_u64SaleSRL		= req.m_u64SaleSRL;
	

	//누군가 사용 중이라면
	if( DB_CONSIGNMENT_SALE()._AddReserve( req.m_u64SaleSRL ) == false )
	{	
		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_BUSY;		//@@@@@@@@@@@@@@이미 구매중입니다.
		CDPTrans::GetInstance()->SendPacket( dpid, &ack );
		return;
	}

	//u_long uSalerID				= NULL_PLAYER_ID;
	if( DB_CONSIGNMENT_SALE()._GetPriceInfo( ack.m_uSalerID, ack.m_nPrice, req.m_u64SaleSRL ) == FALSE )
	{
		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );

		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_ALREADY_DELETE;
		CDPTrans::GetInstance()->SendPacket( dpid, &ack );
		return;
	}

#ifndef __INTERNALSERVER
	if( req.m_uPlayerID == ack.m_uSalerID )
	{
		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );

		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_CANNOT_BUY_SAME_OWNER_ITEM;	
		CDPTrans::GetInstance()->SendPacket( dpid, &ack );
		return;
	}
#endif

	CDPTrans::GetInstance()->SendPacket( dpid, &ack );
}

void CDPTrans::OnConsignmentSale_ClearReservedInfoNoti( CAr & /*ar*/, DPID /*dpid*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	CAr recv( lpBuf, uBufSize );

	FLPacketWDB_ClearReservedInfoNoti noti;
	noti.Deserialize( recv );

	DB_CONSIGNMENT_SALE()._RemoveReserve( noti.m_u64SaleSRL );
}

void CDPTrans::OnConsignmentSale_BuyItemReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_BuyItemReq req;
	req.Deserialize( ar );

	DEBUG_ASSERT( req.m_szPlayerName[ 0 ] != '\0' );
	CHARACTER_DB().PostQueryCommand( req.m_szPlayerName[ 0 ], new FLQuery_BuyItem( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void CDPTrans::OnConsignmentSale_CollectSaleGoldReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_CollectSaleGoldReq req;
	req.Deserialize( ar );
	
	DEBUG_ASSERT( req.m_szPlayerName[ 0 ] != '\0' );
	CHARACTER_DB().PostQueryCommand( req.m_szPlayerName[ 0 ], new FLQuery_CollectSaleGold( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void CDPTrans::OnConsignmentSale_SearchReq( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;		
	CHARACTER_DB().PostQueryCommand( dpidUser/*동기화가 불필요*/, new FLQuery_Search( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void CDPTrans::OnConsignmentSale_SaleGoldInfoListReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_SaleGoldInfoListReq req;
	req.Deserialize( ar );
	
	DEBUG_ASSERT( req.m_szPlayerName[ 0 ] != '\0' );
	CHARACTER_DB().PostQueryCommand( req.m_szPlayerName[ 0 ], new FLQuery_SaleGoldInfoList( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) );
}

void	CDPTrans::OnCharacterServerTransformNoti( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	PostOverlappedPlus( QM_CHARACTER_SERVER_TRANSFORM, g_DbManager.m_hIOCPPut, 1, NULL, dpid, dpidCache, dpidUser, lpBuf, uBufSize );
}

void	CDPTrans::OnGuildBankInputItem( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_INPUT_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void	CDPTrans::OnGuildBankOutputItem( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_OUTPUT_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

// void	CDPTrans::OnGuildBankInputGold( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
// {
// 	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
// 	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
// 	lpDbOverlappedPlus->dpid		= dpid;
// 	lpDbOverlappedPlus->dpidCache	= dpidCache;
// 	lpDbOverlappedPlus->dpidUser	= dpidUser;
// 	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_INPUT_GOLD;
// 	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
// }

void	CDPTrans::OnGuildBankOutputGold( CAr & /*ar*/, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_OUTPUT_GOLD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
