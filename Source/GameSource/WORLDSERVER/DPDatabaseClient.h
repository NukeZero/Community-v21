#ifndef __DPDATABASECLIENT_H__
#define __DPDATABASECLIENT_H__

#include <network/FLDPMng.h>
#include <MsgHdr.h>
#include <guild.h>

#include <eveschool.h>
#include <SecretRoom.h>
#include <GuildHouse.h>


class FLWSUser;
class CDPDatabaseClient : public CDPMng
{
private:
	volatile BOOL	m_bAlive;
public:
	BOOL GetAlive () { return m_bAlive; }
public:
//	Constructions
	CDPDatabaseClient();
	virtual	~CDPDatabaseClient();

	BOOL	Run( LPSTR lpszAddr, USHORT uPort, BOOL bSingleThreaded );

//	Operations
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendITEM_TBL_Update( void );
#ifdef __LAYER_1015
	void	SavePlayer( FLWSUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, int nLayer, BOOL bLogout = FALSE );
#else	// __LAYER_1015
	void	SavePlayer( FLWSUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL bLogout = FALSE );
#endif	// __LAYER_1015
	
	void	SendRecommend( FLWSUser* pUser, int nValue = 0 );

	void	SendLogConnect( FLWSUser* pUser );
	void	SendLogPlayDeath( CMover* pMover, CMover* pSender );
	void	SendLogLevelUp( CMover* pSender, int Action );
	void	SendLogSkillPoint( int nAction, int nPoint, CMover* pMover, LPSKILL pSkill );
	void	SendLogSMItemUse( const char *Action, FLWSUser* pUser, FLItemElem* pItemElem, PT_ITEM_SPEC pItemProp, const char* szName1 = "" );
	void	SendLogGamemaChat( FLWSUser* pUser, LPCTSTR lpszString );
	void	SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption );
	void	SendLogUniqueItem2( CMover* pMover, PT_ITEM_SPEC pItemProp, int nOption );
	void	SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer );
	void	SendLogPkPvp( CMover* pUser, CMover* pLose, int nPoint, char chState );
	void	SendLogSchool( u_long idPlayer, LPCTSTR szName );
	void	SendPing( void );
	void	SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya );
	void	SendOutGuildCombat( u_long idGuild );
	void	SendGuildCombatResult( void );
	void	SendGuildCombatStart( void );
	void	SendGCGetPenyaGuild( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendGCGetPenyaPlayer( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendGuildcombatContinue( int nGuildCombatID,  u_long uidGuild, int nWinCount );

	void	CalluspXXXMultiServer( u_long uKey, FLWSUser* pUser );

	void	SendLogExpBox( u_long idPlayer, OBJID objid, EXPINTEGER iExp, BOOL bGet=FALSE );
	void	UpdateGuildRanking(); // TRANS서버에게 길드랭킹을 업데잇하라는 패킷을 보내는 함수
	void	UpdateGuildRankingUpdate();
	void	SendQueryGuildQuest( void );
	void	SendInsertGuildQuest( u_long idGuild, int nId );
	void	SendUpdateGuildQuest( u_long idGuild, int nId, int nState );
	void	SendQueryGuildBank( DWORD dwWorldServerKey ); // TRANS서버에게 길드창고를 쿼리하라는 패킷을 보내는 함수
	void	SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info, BYTE nLevelUp, LONG nMemberLevel );
	void	SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild, DWORD dwPay );
	void	SendQuerySetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData );
	void	SendQueryMailBox( u_long idReceiver );


	//////////////////////////////////////////////////////////////////////////
	void	SendQueryMailBoxReq( u_long idReceiver );
	void	SendQueryMailBoxCount( u_long idReceiver, int nCount );
	void	OnMailBoxReq( CAr & ar, DPID, DPID );
	//////////////////////////////////////////////////////////////////////////


	void	OnMailBox( CAr & ar, DPID, DPID ); 
	void	SendQueryPostMail( u_long idReceiver, u_long idSender, FLItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText );
	void	SendQueryRemoveMail( u_long idReceiver, u_long nMail );
	void	SendQueryGetMailItem( u_long idReceiver, u_long nMail );
	void	SendQueryGetMailGold( u_long idReceiver, u_long nMail );
	void	SendQueryReadMail( u_long idReceiver, u_long nMail );
	void	OnPostMail( CAr & ar, DPID, DPID );
	void	OnRemoveMail( CAr & ar, DPID, DPID );
	void	OnGetMailItem( CAr & ar, DPID, DPID );
	void	OnGetMailGold( CAr & ar, DPID, DPID );
	void	OnReadMail( CAr & ar, DPID, DPID );
	void	OnAllMail( CAr & ar, DPID, DPID );
	void	CalluspLoggingQuest( u_long idPlayer, int nQuest, int nState, const char* pszComment = "" );
	void	SendPreventLogin( LPCTSTR szAccount, DWORD dwPreventTime );
	void	SendSchoolReport( PSCHOOL_ENTRY pSchool );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber );
	
	void	SendQueryGetGuildBankLogList( u_long idReceiver, DWORD	idGuild,BYTE byListType );

	void	SendQueryGetSealChar( u_long idReceiver ,const char* szAccount);
	void	SendQueryGetSealCharConm( u_long idReceiver );
	void	SendQueryGetSealCharGet( u_long idReceiver ,const char* szAccount,DWORD dwGetId);
	void	SendQueryGetSealCharSet( u_long idReceiver ,const char* szAccount,LONG lSetPlayerSlot,DWORD dwSetPlayerId);

	void	CalluspPetLog( u_long idPlayer, SERIALNUMBER iSerial, DWORD dwData, int nType, CPet* pPet );
	
	void	SendEventLuaChanged( void );

	void	SendGC1to1StateToDBSrvr( void );
	void	SendGC1to1Tender( char cGU, u_long uGuildId, int nPenya, char cState );
	void	SendGC1to1LineUp( u_long uGuildId, std::vector<u_long>& vecMemberId );
	void	SendGC1to1WarPerson( u_long uGuildId, u_long uIdPlayer, char cState );
	void	SendGC1to1WarGuild( DWORD dwWorldId, u_long uGuildId_0, u_long uGuildId_1, char cState );

	void	SendUpdatePlayerData( FLWSUser* pUser );

	void	SendPacket( const FLPacket* pPacket );
	void	SendPacket( const DPID dpidCache, const DPID dpidUser, const FLPacket* pPacket );
	
private:
	USES_PFNENTRIES( CDPDatabaseClient, ( CAr & ar, DPID, DPID ) );

//	Handlers
	void	OnJoin( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnAllPlayerData( CAr & ar, DPID, DPID );
	void	OnAddPlayerData( CAr & ar, DPID, DPID );
	void	OnDeletePlayerData( CAr & ar, DPID, DPID );
	void	OnUpdatePlayerData( CAr & ar, DPID, DPID );

	void	OnGCPlayerPoint( CAr & ar );
	void	OnGCResultValue( CAr & ar );
	void	OnAllGuildData( CAr & ar, DPID, DPID );
	void	OnAllGuildCombat( CAr & ar, DPID, DPID );
	void	OnInGuildCombat( CAr & ar, DPID, DPID );
	void	OnOutGuildCombat( CAr & ar, DPID, DPID );
	void	OnResultGuildCombat( CAr & ar, DPID, DPID );
	void	OnPlayerPointGuildCombat( CAr & ar, DPID, DPID );
	void	OnGetPenyaGuildGC( CAr & ar, DPID, DPID );
	void	OnGetPenyaPlayerGC( CAr & ar, DPID, DPID );
	void	OnContinueGC( CAr & ar, DPID, DPID );
	void	OnBaseGameSetting( CAr & ar, DPID, DPID );
	void	OnMonsterRespawnSetting( CAr & ar, DPID, DPID );
	void	OnMonsterPropSetting( CAr & ar, DPID, DPID );
	void	OnCooperativeContributions_Spec( CAr & ar, DPID, DPID );

	void	OnGMChat( CAr & ar, DPID, DPID );
	void	OnGuildBank( CAr & ar, DPID, DPID );
	void	OnUpdateGuildRankingFinish( CAr & ar, DPID, DPID );
	void	OnQueryGuildQuest( CAr & ar, DPID, DPID );
	void	OnPing( CAr & ar, DPID, DPID );
	void	OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID );
	void	SendQueryRemoveGuildBankTbl( int nNo, DWORD dwRemoved );
	void	OnEventGeneric( CAr & ar, DPID, DPID );
	void	OnEventFlag( CAr & ar, DPID, DPID );

	void	OnEventStateLua( CAr & ar, DPID, DPID );
	void	OnEventLuaChanged( CAr & ar, DPID, DPID );

	void	OnGC1to1Open( CAr & ar, DPID, DPID );
	void	OnGC1to1TenderGuildFromDB( CAr & ar, DPID, DPID );

	void	OnGuildBankLogViewFromDB( CAr & ar, DPID, DPID );

	void	OnSealCharFromDB( CAr & ar, DPID, DPID );
	void	OnSealCharGetFromDB( CAr & ar, DPID, DPID );

private:
	void	OnElectionAddDeposit( CAr & ar, DPID, DPID );
	void	OnElectionSetPledge( CAr & ar, DPID, DPID );
	void	OnElectionIncVote( CAr & ar, DPID, DPID );
	void	OnElectionBeginCandidacy( CAr & ar, DPID, DPID );
	void	OnElectionBeginVote( CAr & ar, DPID, DPID );
	void	OnElectionEndVote( CAr & ar, DPID, DPID );
	void	OnLord( CAr & ar, DPID, DPID );
	void	OnLEventCreate( CAr & ar, DPID, DPID );
	void	OnLEventInitialize( CAr & ar, DPID, DPID );
	void	OnLordSkillUse( CAr & ar, DPID, DPID );
	void	OnLordSkillTick( CAr & ar, DPID, DPID );
	void	OnLEventTick( CAr & ar, DPID, DPID );
public:
	void	SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit );
	void	SendElectionSetPledge( u_long idPlayer, const char* szPledge );
	void	SendElectionIncVote( u_long idPlayer, u_long idElector );
	void	SendLEventCreate( u_long idPlayer, int iEEvent, int iIEvent );
	void	SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill );
// operator commands
	void	SendElectionProcess( BOOL bRun );
	void	SendElectionBeginCandidacy( void );
	void	SendElectionBeginVote( void );
	void	SendElectionBeginEndVote( void );
	void	SendLEventInitialize( void );

private:
	void	OnTaxInfo( CAr & ar, DPID, DPID );
public:
	void	SendSecretRoomWinGuild( BYTE nCont, DWORD dwGuildId );
	void	SendLord( DWORD dwIdPlayer );
	void	SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate );
	void	SendAddTax( BYTE nCont, int nTax, BYTE nTaxKind );
	void	SendApplyTaxRateNow();

	void	SendUpdateTaxRate( const int nTaxRate );

private:
	void	OnSecretRoomInfoClear( CAr & ar, DPID, DPID );
	void	OnSecretRoomTenderInfo( CAr & ar, DPID, DPID );
public:
	void	SendSecretRoomInsertToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender );
	void	SendSecretRoomUpdateToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender, char chState );
	void	SendSecretRoomInsertLineUpToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender );
	void	SendSecretRoomClosed();

private:
	void	OnRainbowRaceInfo( CAr & ar, DPID, DPID );

public:
	void	SendRainbowRaceReqLoad();
	void	SendRainbowRaceApplication( DWORD dwPlayerId );
	void	SendRainbowRaceFailedUser( DWORD dwPlayerId );
	void	SendRainbowRaceRanking( DWORD dwPlayerId, int nRanking );

	void	OnHousingLoadInfo( CAr & ar, DPID, DPID );
	void	OnHousingSetFunitureList( CAr & ar, DPID, DPID );
	void	OnHousingSetupFuniture( CAr & ar, DPID, DPID );
	void	OnHousingSetVisitAllow( CAr & ar, DPID, DPID );
	void	OnHousingDBFailed( CAr & ar, DPID, DPID );

	void	SendPropose( u_long idProposer, u_long idTarget );
	void	SendCouple( u_long idProposer, u_long idTarget );
	void	SendDecouple( u_long idPlayer );
	void	SendClearPropose();
	void	OnProposeResult( CAr & ar, DPID, DPID );
	void	OnCoupleResult( CAr & ar, DPID, DPID );
	void	OnDecoupleResult( CAr & ar, DPID, DPID );
	void	OnCouple( CAr & ar, DPID, DPID );

	void	SendQueryAddCoupleExperience( u_long idPlayer, int nExperience );
	void	OnAddCoupleExperience( CAr & ar, DPID, DPID );

	void	SendLogGetHonorTime(CMover* pMover, int nGetHonor );

	void	SendFunnyCoinReqUse( DWORD dwPlayerId, FLItemElem* pItemElem );	// 퍼니코인을 사용했음을 TransServer에 알린다.
	void	OnFunnyCoinAckUse( CAr & ar, DPID, DPID );		// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.

	void	OnPCBangToggle( CAr & ar, DPID, DPID );

	void	SendTimeLimitInfoReq( DWORD dwPlayerId, char* szAccount );
	void	OnTimeLimitInfoAck( CAr & ar, DPID, DPID );
	void	SendTimeLimitUpdateReq( char* szAccount, int nPlayTime );
	void	OnTimeLimitReset( CAr & ar, DPID, DPID );

	void	SendLogInstanceDungeon( DWORD dwDungeonId, DWORD dwWorldId, u_long uChannel, int nDungeonType, char chState );

	void	OnQuizEventOpen( CAr & ar, DPID, DPID );
	void	OnQuizList( CAr & ar, DPID, DPID );
	void	OnQuizEventNotice( CAr & ar, DPID, DPID );
	void	OnQuizEventChanged( CAr & ar, DPID, DPID );
	void	SendQuizEventOpen( int nType );
	void	SendQuizEventState( int nState, int nChannel, int nWinnerCount = 0, int nQuizCount = 0 );
	void	SendQuizEventEntrance( u_long idPlayer, int nChannel );
	void	SendQuizEventSelect( u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer );
	void	SendPostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum );

	void	SendErrorLogToDB( FLWSUser* pUser, char chType, LPCTSTR szError );

	void	OnLoadGuildHouse( CAr & ar, DPID, DPID );
	void	OnBuyGuildHouse( CAr & ar, DPID, DPID );
	void	OnRemoveGuildHouse( CAr & ar, DPID, DPID );
	void	OnGuildHousePacket( CAr & ar, DPID, DPID );
	void	SendLogGuildFurniture( DWORD dwGuildId, GH_Fntr_Info & gfi, char chState );
#ifdef __GUILD_HOUSE_MIDDLE
private:
	void	OnGuildHouseTenderInfo( CAr & ar, DPID, DPID );
	void	OnGuildHouseTenderJoin( CAr & ar, DPID, DPID );
	void	OnGuildHouseTenderState( CAr & ar, DPID, DPID );
	void	OnGuildHouseGradeUpdate( CAr & ar, DPID, DPID );
	void	OnGuildHouseLevelUpdate( CAr & ar, DPID, DPID );
	void	OnGuildHouseExpired( CAr & ar, DPID, DPID );
public:
	void	SendGuildHouseTenderState( const PT_PACKET_GUILDHOUSE_TENDER_STATE ptGHTenderState );
	void	SendGuildHouseExpired( DWORD dwGuildId );
#endif // __GUILD_HOUSE_MIDDLE


private:
	void	OnAllCampus( CAr & ar, DPID, DPID );
	void	OnAddCampusMember( CAr & ar, DPID, DPID );
	void	OnRemoveCampusMember( CAr & ar, DPID, DPID );
	void	OnUpdateCampusPoint( CAr & ar, DPID, DPID );
public:
	void	SendAddCampusMember( u_long idMaster, int nMasterPoint, u_long idPupil, int nPupilPoint );
	void	SendRemoveCampusMember( u_long idCampus, u_long idPlayer );
	void	SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint, BOOL bAdd, char chState );


//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

private:
	void	OnCouponNumber( CAr & ar, DPID, DPID );
public:
	void	SendCouponNumber( const PT_PACKET_COUPON_NUMBER ptCouponNumber );

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////

private:
	void	OnSerialNumberAck( CAr & ar, DPID, DPID );
public:
	void	SendSerialNumberReq( DWORD dwCount );

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100813
public:
	void	SendBaseGameSettingReq();
	//////////////////////////////////////////////////////////////////////////

	// 협동 기부
private:
	//void	OnCooperativeContributions_ContributionInfo( CAr & ar, DPID, DPID );
	void	OnCooperativeContributions_Contribute( CAr & ar, DPID, DPID );
	void	OnCooperativeContributions_GetRanking( CAr & ar, DPID, DPID );
	void	OnCooperativeContributions_GetContributed( CAr & ar, DPID, DPID );
	BOOL	cooperativeContributions_SetContributedAndTryReward( const DWORD dwContributionID, const DWORD dwContirbutedCount );
//	void	OnCooperativeContributions_TryRewardItem( CAr & ar, DPID, DPID );
	void	OnCooperativeContributions_TryWriteItemRewardInfo( CAr & ar, DPID, DPID );
	//void	OnCooperativeContributions_RewardItem( CAr & ar, DPID, DPID );
	// 협동 기부

	//콜로세움
	void	OnColosseum_GetDungeonRankingInfo( CAr & ar, DPID, DPID );

	// 텔레포트 지도
	void	OnTeleportMapPointListNoti( CAr & ar, DPID, DPID );

	void	OnConsignmentSale_GetRegistedCountAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_SellItemNoti( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_RegistAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_CancelRegistedAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_RegistedInfoListAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_GetPriceAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_BuyItemAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_CollectSaleGoldAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_SearchAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_SaleGoldInfoListAck( CAr & ar, DPID, DPID );
	void	OnConsignmentSale_CheckRegistedCountAck( CAr & ar, DPID, DPID );

	void	OnServerList( CAr & ar, DPID, DPID );

	void	OnGuildBankQueryFailAck( CAr & ar, DPID, DPID );

	void	OnDungeonBestCompleteTimeAck( CAr & ar, DPID, DPID );

	void	OnGuildBankOutputItemAck( CAr & ar, DPID, DPID );
	void	OnGuildBankOutputGoldAck( CAr & ar, DPID, DPID );
};

#endif	// __DPDATABASECLIENT_H__