#include "stdafx.h"
#include "dpsrvr.h"
#include "user.h"
#include <worldmng.h>
#include <misc.h>
#include <lang.h>
#include "npchecker.h"
#include <guild.h>
#include "..\_Common\Ship.h"
#include <Party.h>
#include <Chatting.h>
#include <post.h>
#include "..\_Network\ErrorCode.h"
#include "WantedListSnapshot.h"

#include <pet.h>

#include "slord.h"
#include "lordskillexecutable.h"

#include <playerdata.h>
#include <CreateMonster.h>
#include <honor.h>
#include <SecretRoom.h>
#include <Tax.h>
#include "ItemUpgrade.h"
#include <tools.h>
#include "couplehelper.h"
#include <Quiz.h>
#include <GuildHouse.h>
#include "CampusHelper.h"

#include "FLCooperativeContributions.h"
#include "FLCC_Contribution.h"
#include "FLCC_Condition.h"
#include "FLCC_Reward.h"
#include "FLItemUsing.h"
#include "FLEventArenaGlobal.h"

#include "../_Common/InstanceDungeonParty.h"
#include "../_Common/FLSkillSystem.h"

#include "GlobalGameValueCalculator.h"

#include "../_CommonDefine/Packet/FLPacket_ConsignmentSale.h"
#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"
#include "../_CommonDefine/Packet/FLPacket_Certify.h"
#include "FLChargeZoneTicket.h"
#include "FLExchangeEvent.h"

#include "FLItemAction.h"
#include "FLItemExtract.h"

#include "../_CommonDefine/Packet/FLPacketGuildBankUpdate.h"


struct ItemCountSet{
	OBJID itemid;
	int extracount;
};

//#define	MAX_RANGE_ASYNC			1024
//#define	MAX_RANGE_NPC_MENU		1024

extern	CGuildMng			g_GuildMng;
extern	CPartyMng			g_PartyMng;
extern	CWorldMng			g_WorldMng;
extern	CChattingMng		g_ChattingMng;
extern	CGuildCombat		g_GuildCombatMng;

//static const TCHAR	LOG_BUYING_INFO[] = _T( "BuyingInfo" );


CCommonCtrl* CreateExpBox( FLWSUser* pUser );
CDPSrvr		g_DPSrvr;

// dwGold과 nPlus을 더 할 수 있는가?
BOOL CanAdd( DWORD dwGold, int nPlus )
{
	if( nPlus <= 0 )		// 더하려는 값이 0이하이면 넌센스 
	{
		return FALSE;
	}

	__int64 n64Gold = (__int64)dwGold;

	if( n64Gold < 0 || ( n64Gold + (__int64)nPlus ) < 0 || ( n64Gold + (__int64)nPlus ) > INT_MAX )
	{
		//		FLERROR_LOG( PROGRAM_NAME, _T( "CanAdd Invalid. HaveGold:[%d], AddGold:[%d]" ), dwGold, nPlus );
		return FALSE;
	}

	return TRUE;
}

CDPSrvr::CDPSrvr()
{
	ON_MSG( PACKETTYPE_JOIN, OnAddUser );
	ON_MSG( PACKETTYPE_LEAVE, OnRemoveUser );
	ON_MSG( PACKETTYPE_REPLACE, OnReplace );
//	ON_MSG( PACKETTYPE_SUMMONPLAYER, OnSummonPlayer );
//	ON_MSG( PACKETTYPE_TELEPORTPLAYER, OnTeleportPlayer );
	ON_MSG( PACKETTYPE_CORR_REQ, OnCorrReq );
	ON_MSG( PACKETTYPE_SCRIPTDLG, OnScriptDialogReq );
	ON_MSG( PACKETTYPE_TRADEPUT, OnTradePut );
	ON_MSG( PACKETTYPE_TRADEPULL, OnTradePull );
	ON_MSG( PACKETTYPE_TRADEPUTGOLD, OnTradePutGold );
	//raiders.2006.11.28
	//	ON_MSG( PACKETTYPE_TRADECLEARGOLD, OnTradeClearGold );
	ON_MSG( PACKETTYPE_SFX_ID, OnSfxID );
	ON_MSG( PACKETTYPE_SFX_CLEAR, OnSfxClear );
	ON_MSG( PACKETTYPE_CREATE_GUILDCLOAK, OnCreateGuildCloak );
	ON_MSG( PACKETTYPE_CHAT, OnChat );
	ON_MSG( PACKETTYPE_CTRL_COOLTIME_CANCEL, OnCtrlCoolTimeCancel );
	ON_MSG( PACKETTYPE_DOEQUIP, OnDoEquip );
	ON_MSG( PACKETTYPE_MOVEITEM, OnMoveItem );
#ifdef INVENTORY_ITEM_ALIGN
	ON_MSG( PACKETTYPE_ALIGNITEM, OnAlignItem );
#endif	// INVENTORY_ITEM_ALIGN
	ON_MSG( PACKETTYPE_SNAPSHOT, OnSnapshot );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_CHANGEJOB, OnChangeJob );
	ON_MSG( PACKETTYPE_SETLODELIGHT, OnSetLodelight );
	ON_MSG( PACKETTYPE_MODIFYMODE, OnModifyMode );
	ON_MSG( PACKETTYPE_REVIVAL_BY_SKILL, OnRevivalBySkill );
	ON_MSG( PACKETTYPE_REVIVAL_TO_CURRENT_POS, OnRevivalCurrentPos );
	ON_MSG( PACKETTYPE_REVIVAL_TO_LODESTAR, OnRevivalLodestar );
	ON_MSG( PACKETTYPE_REVIVAL_TO_LODELIGHT, OnRevivalLodelight );
	ON_MSG( PACKETTYPE_OPENSHOPWND, OnOpenShopWnd );
	ON_MSG( PACKETTYPE_CLOSESHOPWND, OnCloseShopWnd );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_EXP, OnExpUp );
	ON_MSG( PACKETTYPE_OPENBANKWND, OnOpenBankWnd );
	ON_MSG( PACKETTYPE_GUILD_BANK_WND, OnOpenGuildBankWnd );
	ON_MSG( PACKETTYPE_PUTITEMGUILDBANK, OnPutItemGuildBank );
	ON_MSG( PACKETTYPE_GETITEMGUILDBANK, OnGetItemGuildBank );
	ON_MSG( PACKETTYPE_GUILD_BANK_WND_CLOSE, OnCloseGuildBankWnd );
	ON_MSG( PACKETTYPE_GUILD_BANK_MOVEITEM, OnGuildBankMoveItem );
	ON_MSG( PACKETTYPE_CLOSEBANKWND, OnCloseBankWnd );
	ON_MSG( PACKETTYPE_PUTITEMBACK, OnPutItemBank );
	ON_MSG( PACKETTYPE_DOUSESKILLPOINT, OnDoUseSkillPoint );
	ON_MSG( PACKETTYPE_PUTBACKTOBANK, OnBankToBank );
	ON_MSG( PACKETTYPE_GETITEMBACK, OnGetItemBank );
	ON_MSG( PACKETTYPE_PUTGOLDBACK, OnPutGoldBank );
	ON_MSG( PACKETTYPE_GETGOLDBACK, OnGetGoldBank );
	ON_MSG( PACKETTYPE_MOVEBANKITEM, OnMoveBankItem );
	ON_MSG( PACKETTYPE_CHANGEBANKPASS, OnChangeBankPass );
	ON_MSG( PACKETTYPE_CONFIRMBANK, OnConfirmBank );	
	ON_MSG( PACKETTYPE_PLAYERBEHAVIOR, OnPlayerBehavior );
	ON_MSG( PACKETTYPE_PLAYERSETDESTOBJ, OnPlayerSetDestObj );
	ON_MSG( PACKETTYPE_TRADE, OnTrade );
	ON_MSG( PACKETTYPE_CONFIRMTRADE, OnConfirmTrade );
	ON_MSG( PACKETTYPE_CONFIRMTRADECANCEL, OnConfirmTradeCancel )
		ON_MSG( PACKETTYPE_TRADECANCEL, OnTradeCancel );
	ON_MSG( PACKETTYPE_DOUSEITEM, OnDoUseItem );
	ON_MSG( PACKETTYPE_DO_USE_ITEM_TARGET, OnDoUseItemTarget );
	//	ON_MSG( PACKETTYPE_REMOVE_ITEM_LEVEL_DOWN, OnRemoveItemLevelDown );
	//	ON_MSG( PACKETTYPE_AWAKENING, OnAwakening );
	//	ON_MSG( PACKETTYPE_BLESSEDNESS_CANCEL, OnBlessednessCancel );

	ON_MSG( PACKETTYPE_DO_USE_ITEM_INPUT, OnDoUseItemInput );

	ON_MSG( PACKETTYPE_CLEAR_PET_NAME, OnClearPetName );

	ON_MSG( PACKETTYPE_AVAIL_POCKET, OnAvailPocket );
	ON_MSG( PACKETTYPE_MOVE_ITEM_POCKET, OnMoveItemOnPocket );

	ON_MSG( PACKETTYPE_QUE_PETRESURRECTION, OnQuePetResurrection );

	ON_MSG( PACKETTYPE_ARENA_ENTER, OnArenaEnter );
	ON_MSG( PACKETTYPE_ARENA_EXIT, OnArenaExit );

	ON_MSG( PACKETTYPE_TRADECONFIRM, OnTradelastConfrim );
	ON_MSG( PACKETTYPE_MOVERFOCOUS, OnMoverFocus );
	ON_MSG( PACKETTYPE_DROPITEM, OnDropItem );
	ON_MSG( PACKETTYPE_DROPGOLD, OnDropGold );
	ON_MSG( PACKETTYPE_BUYITEM, OnBuyItem );
	ON_MSG( PACKETTYPE_BUYCHIPITEM, OnBuyChipItem );
	ON_MSG( PACKETTYPE_SELLITEM, OnSellItem );
	ON_MSG( PACKETTYPE_TRADEOK, OnTradeOk );
	ON_MSG( PACKETTYPE_MELEE_ATTACK, OnMeleeAttack );
	ON_MSG( PACKETTYPE_MELEE_ATTACK2, OnMeleeAttack2 );
	ON_MSG( PACKETTYPE_MAGIC_ATTACK, OnMagicAttack );
	ON_MSG( PACKETTYPE_RANGE_ATTACK, OnRangeAttack );
	ON_MSG( PACKETTYPE_SFX_HIT, OnSfxHit );
	ON_MSG( PACKETTYPE_USESKILL, OnUseSkill );
	ON_MSG( PACKETTYPE_SETTARGET, OnSetTarget );	// Core에 보내야 하는지 확인해 줄것.
	ON_MSG( PACKETTYPE_TELESKILL, OnTeleSkill );	
	ON_MSG( PACKETTYPE_SKILLTASKBAR, OnSkillTaskBar );
	ON_MSG( PACKETTYPE_ADDAPPLETTASKBAR, OnAddAppletTaskBar );
	ON_MSG( PACKETTYPE_REMOVEAPPLETTASKBAR, OnRemoveAppletTaskBar );
	ON_MSG( PACKETTYPE_ADDITEMTASKBAR, OnAddItemTaskBar );
	ON_MSG( PACKETTYPE_REMOVEITEMTASKBAR, OnRemoveItemTaskBar );
// 	ON_MSG( PACKETTYPE_QUERYGETPOS, OnQueryGetPos );
// 	ON_MSG( PACKETTYPE_GETPOS, OnGetPos );
	ON_MSG( PACKETTYPE_QUERYGETDESTOBJ, OnQueryGetDestObj );
	ON_MSG( PACKETTYPE_GETDESTOBJ, OnGetDestObj );
	ON_MSG( PACKETTYPE_MEMBERREQUEST, OnPartyRequest );
	ON_MSG( PACKETTYPE_MEMBERREQUESTCANCLE, OnPartyRequestCancle );
	ON_MSG( PACKETTYPE_ADDPARTYMEMBER, OnConfirmPartyRequest );
	ON_MSG( PACKETTYPE_PARTYSKILLUSE, OnPartySkillUse );
	ON_MSG( PACKETTYPE_ADDFRIENDREQEST, OnAddFriendReqest );
	ON_MSG( PACKETTYPE_ADDFRIENDNAMEREQEST, OnAddFriendNameReqest );
	ON_MSG( PACKETTYPE_ADDFRIENDCANCEL, OnAddFriendCancel );
	ON_MSG( PACKETTYPE_DUELREQUEST, OnDuelRequest );
	ON_MSG( PACKETTYPE_DUELYES, OnDuelYes );
	ON_MSG( PACKETTYPE_DUELNO, OnDuelNo );
	ON_MSG( PACKETTYPE_DUELPARTYREQUEST, OnDuelPartyRequest );
	ON_MSG( PACKETTYPE_DUELPARTYYES, OnDuelPartyYes );
	ON_MSG( PACKETTYPE_DUELPARTYNO, OnDuelPartyNo );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_AP, OnActionPoint );
	ON_MSG( PACKETTYPE_QUERY_PLAYER_DATA, OnQueryPlayerData );
	ON_MSG( PACKETTYPE_QUERY_PLAYER_DATA2, OnQueryPlayerData2 );

	ON_MSG( PACKETTYPE_GUILD_INVITE, OnGuildInvite );
	ON_MSG( PACKETTYPE_IGNORE_GUILD_INVITE, OnIgnoreGuildInvite );
	ON_MSG( PACKETTYPE_NW_GUILDLOGO, OnGuildLogo );			// 로고 변경 
	ON_MSG( PACKETTYPE_NW_GUILDCONTRIBUTION, OnGuildContribution );		// 공헌도 
	ON_MSG( PACKETTYPE_NW_GUILDNOTICE, OnGuildNotice );		// 공지사항  
	ON_MSG( PACKETTYPE_REQUEST_GUILD_RANKING, OnRequestGuildRank );
	ON_MSG( PACKETTYPE_PVENDOR_OPEN, OnPVendorOpen );
	ON_MSG( PACKETTYPE_PVENDOR_CLOSE, OnPVendorClose );
	ON_MSG( PACKETTYPE_REGISTER_PVENDOR_ITEM, OnRegisterPVendorItem );
	ON_MSG( PACKETTYPE_UNREGISTER_PVENDOR_ITEM, OnUnregisterPVendorItem );
	ON_MSG( PACKETTYPE_QUERY_PVENDOR_ITEM, OnQueryPVendorItem );
	ON_MSG( PACKETTYPE_BUY_PVENDOR_ITEM, OnBuyPVendorItem );
	ON_MSG( PACKETTYPE_REPAIRITEM, OnRepairItem );
	ON_MSG( PACKETTYPE_SET_HAIR, OnSetHair );
	ON_MSG( PACKETTYPE_BLOCK, OnBlock );
	ON_MSG( PACKETTYPE_MOTION, OnMotion );
	ON_MSG( PACKETTYPE_SHIP_ACTMSG, OnShipActMsg );
	ON_MSG( PACKETTYPE_LOCALPOSFROMIA, OnLocalPosFromIA );
	ON_MSG( PACKETTYPE_UPGRADEBASE, OnUpgradeBase );
	ON_MSG( PACKETTYPE_ENCHANT, OnEnchant );
	ON_MSG( PACKETTYPE_SMELT_SAFETY, OnSmeltSafety );
	ON_MSG( PACKETTYPE_REMVOE_ATTRIBUTE, OnRemoveAttribute );
	ON_MSG( PACKETTYPE_CHANGE_ATTRIBUTE, OnChangeAttribute );
	ON_MSG( PACKETTYPE_PIERCING_SIZE, OnPiercingSize );
	ON_MSG( PACKETTYPE_EXPBOXINFO, OnExpBoxInfo );
	ON_MSG( PACKETTYPE_RANDOMSCROLL, OnRandomScroll );
	ON_MSG( PACHETTYPE_ITEMTRANSY, OnItemTransy );
	ON_MSG( PACKETTYPE_PIERCING, OnPiercing );
	ON_MSG( PACKETTYPE_PIERCINGREMOVE, OnPiercingRemove );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
	ON_MSG( PACKETTYPE_ENTERCHTTING, OnEnterChattingRoom );
	ON_MSG( PACKETTYPE_CHATTING, OnChatting );
	ON_MSG( PACKETTYPE_OPENCHATTINGROOM, OnOpenChattingRoom );
	ON_MSG( PACKETTYPE_CLOSECHATTINGROOM, OnCloseChattingRoom );
	ON_MSG( PACKETTYPE_COMMONPLACE, OnCommonPlace );
	ON_MSG( PACKETTYPE_SETNAVIPOINT, OnSetNaviPoint );
	ON_MSG( PACKETTYPE_DO_ESCAPE, OnDoEscape );
	ON_MSG( PACKETTYPE_LOG_GAMEMASTER_CHAT, OnGameMasterWhisper );
	ON_MSG( PACKETTYPE_RETURNSCROLL, OnReturnScroll );
	ON_MSG( PACKETTYPE_ENDSKILLQUEUE, OnEndSkillQueue );
	ON_MSG( PACKETTYPE_FOCUSOBJ, OnFoucusObj );

	ON_MSG( PACKETTYPE_REMOVEQUEST, OnRemoveQuest );
	ON_MSG( PACKETTYPE_COMMERCIALELEM, OnCommercialElem );
	//	ON_MSG( PACKETTYPE_DO_COLLECT, OnDoCollect );

	ON_MSG( PACKETTYPE_NW_WANTED_GOLD, OnNWWantedGold );
	ON_MSG( PACKETTYPE_NW_WANTED_LIST, OnNWWantedList );
	ON_MSG( PACKETTYPE_NW_WANTED_INFO, OnNWWantedInfo );
	ON_MSG( PACKETTYPE_NW_WANTED_NAME, OnNWWantedName );
	ON_MSG( PACKETTYPE_REQ_LEAVE, OnReqLeave );
	ON_MSG( PACKETTYPE_MODE, OnChangeMode );

	ON_MSG( PACKETTYPE_STATEMODE, OnStateMode );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, OnQuerySetPlayerName );
	ON_MSG( PACKETTYPE_QUERYSETGUILDNAME, OnQuerySetGuildName );
	ON_MSG( PACKETTYPE_CHEERING, OnCheering );
	ON_MSG( PACKETTYPE_QUERYEQUIP, OnQueryEquip );
	ON_MSG( PACKETTYPE_QUERYEQUIPSETTING, OnQueryEquipSetting );
	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, OnQueryPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL, OnQueryRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM, OnQueryGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD, OnQueryGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL, OnQueryReadMail );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, OnQueryMailBox );
	ON_MSG( PACKETTYPE_CHANGEFACE, OnChangeFace );

	ON_MSG( PACKETTYPE_CREATEMONSTER, OnCreateMonster );

	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, OnGCApp );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, OnGCCancel );	
	ON_MSG( PACKETTYPE_REQUEST_STATUS, OnGCRequestStatus );
	ON_MSG( PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, OnGCSelectPlayer );
	ON_MSG( PACKETTYPE_SELECTMAP_GUILDCOMBAT, OnGCSelectMap );
	ON_MSG( PACKETTYPE_JOIN_GUILDCOMBAT, OnGCJoin );	
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, OnGCGetPenyaGuild );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, OnGCGetPenyaPlayer );

	ON_MSG( PACKETTYPE_TELE_GUILDCOMBAT, OnGCTele );
	ON_MSG( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, OnGCPlayerPoint );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND, OnSummonFriend );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND_CONFIRM, OnSummonFriendConfirm );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND_CANCEL, OnSummonFriendCancel );
	ON_MSG( PACKETTYPE_SUMMON_PARTY, OnSummonParty );
	ON_MSG( PACKETTYPE_SUMMON_PARTY_CONFIRM, OnSummonPartyConfirm );

	ON_MSG( PACKETTYPE_REMOVEINVENITEM, OnRemoveInvenItem );
	////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_CREATEANGEL, OnCreateAngel );
	ON_MSG( PACKETTYPE_ANGELBUFF, OnAngleBuff );

	ON_MSG( PACKETTYPE_KAWIBAWIBO_START, OnKawibawiboStart );
	ON_MSG( PACKETTYPE_KAWIBAWIBO_GETITEM, OnKawibawiboGetItem );
	ON_MSG( PACKETTYPE_REASSEMBLE_START, OnReassembleStart );
	ON_MSG( PACKETTYPE_REASSEMBLE_OPENWND, OnReassembleOpenWnd );
	ON_MSG( PACKETTYPE_ALPHABET_OPENWND, OnAlphabetOpenWnd );
	ON_MSG( PACKETTYPE_ALPHABET_START, OnAlphabetStart );
	ON_MSG( PACKETTYPE_FIVESYSTEM_OPENWND, OnFiveSystemOpenWnd );
	ON_MSG( PACKETTYPE_FIVESYSTEM_BET, OnFiveSystemBet );
	ON_MSG( PACKETTYPE_FIVESYSTEM_START, OnFiveSystemStart );
	ON_MSG( PACKETTYPE_FIVESYSTEM_DESTROYWND, OnFiveSystemDestroyWnd );

	ON_MSG( PACKETTYPE_ULTIMATE_MAKEITEM, OnUltimateMakeItem );
	ON_MSG( PACKETTYPE_ULTIMATE_MAKEGEM, OnUltimateMakeGem );
	ON_MSG( PACKETTYPE_ULTIMATE_TRANSWEAPON, OnUltimateTransWeapon );
	ON_MSG( PACKETTYPE_ULTIMATE_SETGEM, OnUltimateSetGem );
	ON_MSG( PACKETTYPE_ULTIMATE_REMOVEGEM, OnUltimateRemoveGem );
	ON_MSG( PACKETTYPE_ULTIMATE_ENCHANTWEAPON, OnUltimateEnchantWeapon );

//	ON_MSG( PACKETTYPE_EXCHANGE, OnExchange );

	ON_MSG( PACKETTYPE_PET_RELEASE, OnPetRelease );
	ON_MSG( PACKETTYPE_USE_PET_FEED, OnUsePetFeed );
	ON_MSG( PACKETTYPE_MAKE_PET_FEED, OnMakePetFeed );
	ON_MSG( PACKETTYPE_PET_TAMER_MISTAKE, OnPetTamerMistake );
	ON_MSG( PACKETTYPE_PET_TAMER_MIRACLE, OnPetTamerMiracle );
	ON_MSG( PACKETTYPE_FEED_POCKET_INACTIVE, OnFeedPocketInactive );

	ON_MSG( PACKETTYPE_LEGENDSKILLUP_START, OnLegendSkillStart );

	ON_MSG( PACKETTYPE_MODIFY_STATUS, OnModifyStatus );

	ON_MSG( PACKETTYPE_GC1TO1_TENDEROPENWND, OnGC1to1TenderOpenWnd );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERVIEW, OnGC1to1TenderView );
	ON_MSG( PACKETTYPE_GC1TO1_TENDER, OnGC1to1Tender );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERCANCEL, OnGC1to1CancelTender );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERFAILED, OnGC1to1FailedTender );
	ON_MSG( PACKETTYPE_GC1TO1_MEMBERLINEUPOPENWND, OnGC1to1MemberLineUpOpenWnd );
	ON_MSG( PACKETTYPE_GC1TO1_MEMBERLINEUP, OnGC1to1MemberLineUp );
	ON_MSG( PACKETTYPE_GC1TO1_TELEPORTTONPC, OnGC1to1TeleportToNPC );
	ON_MSG( PACKETTYPE_GC1TO1_TELEPORTTOSTAGE, OnGC1to1TeleportToStage );	

	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, OnQueryGuildBankLogList );
	ON_MSG( PACKETTYPE_SEALCHAR_REQ, OnSealCharReq );
	ON_MSG( PACKETTYPE_SEALCHARCONM_REQ, OnSealCharConmReq );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, OnSealCharGetReq );
	ON_MSG( PACKETTYPE_HONOR_LIST_REQ, OnHonorListReq );
	ON_MSG( PACKETTYPE_HONOR_CHANGE_REQ, OnHonorChangeReq );

	ON_MSG( PACKETTYPE_QUERY_START_COLLECTING, OnQueryStartCollecting );
	ON_MSG( PACKETTYPE_QUERY_STOP_COLLECTING, OnQueryStopCollecting );

	ON_MSG( PACKETTYPE_NPC_BUFF, OnNPCBuff );

	ON_MSG( PACKETTYPE_SECRETROOM_TENDEROPENWND, OnSecretRoomTenderOpenWnd );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDER, OnSecretRoomTender );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERCANCELRETURN, OnSecretRoomTenderCancelReturn );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUPOPENWND, OnSecretRoomLineUpOpenWnd );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUPMEMBER, OnSecretRoomLineUpMember );
	ON_MSG( PACKETTYPE_SECRETROOM_ENTRANCE, OnSecretRoomEntrance );
	ON_MSG( PACKETTYPE_SECRETROOM_TELEPORTTONPC, OnSecretRoomTeleportToNPC );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERVIEW, OnSecretRoomTenderView );
	ON_MSG( PACKETTYPE_SECRETROOM_TELEPORTTODUNGEON, OnTeleportSecretRoomDungeon );

	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, OnElectionAddDeposit );
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, OnElectionSetPledge );
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, OnElectionIncVote );
	ON_MSG( PACKETTYPE_L_EVENT_CREATE, OnLEventCreate );
	ON_MSG( PACKETTYPE_LORD_SKILL_USE, OnLordSkillUse );
	// 알변환 핸들러
	ON_MSG( PACKETTYPE_TRANSFORM_ITEM, OnTransformItem );

	ON_MSG( PACKETTYPE_TAX_SET_TAXRATE, OnSetTaxRate );

	ON_MSG( PACKETTYPE_HEAVENTOWER_TELEPORT, OnTeleportToHeavenTower );

	ON_MSG( PACKETTYPE_TUTORIAL_STATE, OnTutorialState );

	//	ON_MSG( PACKETTYPE_PICKUP_PET_AWAKENING_CANCEL, OnPickupPetAwakeningCancel );

	ON_MSG( PACKETTYPE_OPTION_ENABLE_RENDER_MASK, OnOptionEnableRenderMask );
	ON_MSG( PACKETTYPE_OPTION_ENABLE_RENDER_COSTUME, OnOptionEnableRenderCostume );

	ON_MSG( PACKETTYPE_RAINBOWRACE_PREVRANKING_OPENWND, OnRainbowRacePrevRankingOpenWnd );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPLICATION_OPENWND, OnRainbowRaceApplicationOpenWnd );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPLICATION, OnRainbowRaceApplication );
	ON_MSG( PACKETTYPE_RAINBOWRACE_MINIGAME_PACKET, OnRainbowRaceMiniGamePacket );
	ON_MSG( PACKETTYPE_RAINBOWRACE_REQ_FINISH, OnRainbowRaceReqFinish );

	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, OnHousingSetupFurniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, OnHousingSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_VISITROOM, OnHousingVisitRoom );
	ON_MSG( PACKETTYPE_HOUSING_REQVISITABLELIST, OnHousingVisitableList );
	ON_MSG( PACKETTYPE_HOUSING_GOOUT, OnHousingGoOut );


	ON_MSG( PACKETTYPE_QUESTHELPER_REQNPCPOS, OnReqQuestNPCPos );

	ON_MSG( PACKETTYPE_PROPOSE, OnPropose );
	ON_MSG( PACKETTYPE_REFUSE, OnRefuse );
	ON_MSG( PACKETTYPE_COUPLE, OnCouple );
	ON_MSG( PACKETTYPE_DECOUPLE, OnDecouple );

	ON_MSG( PACKETTYPE_MAP_KEY, OnMapKey );

	ON_MSG( PACKETTYPE_QUIZ_ENTRANCE, OnQuizEventEntrance );
	ON_MSG( PACKETTYPE_QUIZ_TELEPORT, OnQuizEventTeleport );

	ON_MSG( PACKETTYPE_VISPET_REMOVEVIS, OnRemoveVis );
	ON_MSG( PACKETTYPE_VISPET_SWAPVIS, OnSwapVis );

	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, OnGuildHousePacket );
	ON_MSG( PACKETTYPE_GUILDHOUSE_ENTER, OnGuildHouseEnter );
	ON_MSG( PACKETTYPE_GUILDHOUSE_GOOUT, OnGuildHouseGoOut );

	ON_MSG( PACKETTYPE_TELEPORTER, OnTeleporterReq );

	ON_MSG( PACKETTYPE_QUEST_CHECK, OnCheckedQuest );

	ON_MSG( PACKETTYPE_CAMPUS_INVITE, OnInviteCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_ACCEPT, OnAcceptCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REFUSE, OnRefuseCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, OnRemoveCampusMember );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	ON_MSG( PACKETTYPE_ITEM_SELECT_AWAKENING_VALUE, OnItemSelectAwakeningValue );
	//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_MAINWND, OnReqGuildHouseTenderMainWnd );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_INFOWND, OnReqGuildHouseTenderInfoWnd );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, OnReqGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_INFOWND, OnReqGuildHouseInfoWnd );
	ON_MSG( PACKETTYPE_GUILDHOUSE_COMMENT, OnReqGuildHouseCommentChange );
#endif // __GUILD_HOUSE_MIDDLE

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100723 give coupon item event

	ON_MSG( PACKETTYPE_COUPON_ITEM_NUMBER, OnCouponNumber );

	// mirchang_100723 give coupon item event
	//////////////////////////////////////////////////////////////////////////

#ifdef __HYPERLINK_ITEM16

	ON_MSG( PACKETTYPE_ITEM_LINK_INFO, OnReqItemLinkInfo );

#endif // __HYPERLINK_ITEM16


#ifdef __ENCHANT_BARUNA16

	ON_MSG( PACKETTYPE_BARUNA16_OPER_EXTRACT, OnReqExtractOper );
	ON_MSG( PACKETTYPE_BARUNA16_OPER_CREATE, OnReqCreateOper );
	ON_MSG( PACKETTYPE_BARUNA16_CID_CREATE, OnReqCreateCid );
	ON_MSG( PACKETTYPE_BARUNA16_CIDPIECE_UPGRADE, OnReqUpgradeCidPiece );
	ON_MSG( PACKETTYPE_BARUNA16_OPERCID_CREATE, OnReqCreateOperCid );

#endif // __ENCHANT_BARUNA16

	//////////////////////////////////////////////////////////////////////////
	// shopping basket
	ON_MSG( PACKETTYPE_BUY_SHOPPING_BASKET, OnReqBuyItemSoppingBasket );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// reset bind
	ON_MSG( PACKETTYPE_RESET_BIND, OnReqResetBind );
	//////////////////////////////////////////////////////////////////////////

	// 협동 기부
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_INFO_REQ, OnCooperativeContributions_GetInfoReq );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_REQ, OnCooperativeContributions_ContributeReq );
	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_REQ, OnCooperativeContributions_GetRanking );

	//	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_REQ, OnCooperativeContributions_ContributeItemReq );
	//	ON_MSG( PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_REQ, OnCooperativeContributions_ContributeGoldReq );
	// 협동 기부

	ON_MSG( PACKETTYPE_COLOSSEUM_ENTER_REQ, OnColosseum_Enter );
	ON_MSG( PACKETTYPE_COLOSSEUM_LEAVE_NOTI, OnColosseum_Leave );
	ON_MSG( PACKETTYPE_COLOSSEUM_RETRY_REQ, OnColosseum_Retry );
	ON_MSG( PACKETTYPE_COLOSSEUM_AUTO_INVITE_ACK, OnColosseum_InviteAck );
	ON_MSG( PACKETTYPE_COLOSSEUM_FORCE_START, OnColosseum_ForceStart );

	ON_MSG( PACKETTYPE_COLOSSEUM_REGIST_ADDITIONAL_REAL_MONSTER_RATE_ITEM, OnColosseum_RegistAdditionalRealMonsterRateItem );	//CS 리얼 몬스터 출연 확률 올려주는 아이템 등록
	ON_MSG( PACKETTYPE_COLOSSEUM_UNREGIST_ADDITIONAL_REAL_MONSTER_RATE_ITEM, OnColosseum_UnRegistAdditionalRealMonsterRateItem );	//CS 리얼 몬스터 출연 확률 올려주는 아이템 등록해제
	ON_MSG( PACKETTYPE_COLOSSEUM_GET_RANKING_INFO, OnColosseum_RankingInfo );		

	//////////////////////////////////////////////////////////////////////////
	// item merge
	ON_MSG( PACKETTYPE_ITEM_MERGE_REQ, OnReqItemMerge );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// item upgrade
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_GENERAL_ENCHANT_REQ,			OnReqUpgradeItemGeneralEnchant );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_ATTRIBUTE_ENCHANT_REQ,			OnReqUpgradeItemAttributeEnchant );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_PIERCING_INSERT_ITEM_REQ,		OnReqUpgradeItemPiercingInsertItem );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_REQ,		OnReqUpgradeItemRandomOptionGenerate );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_RETRY_REQ,	OnReqUpgradeItemRandomOptionGenerateRetry );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_INITIALIZE_REQ,	OnReqUpgradeItemRandomOptionInitialize );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_EQUIP_LEVEL_DECREASE_REQ,		OnReqUpgradeItemEquipLevelDecrease );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_EQUIP_LEVEL_INITIALIZE_REQ,		OnReqUpgradeItemEquipLevelInitialize );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_COMBINE_REQ,					OnReqUpgradeItemCombine );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_DATA_REQ,	OnReqUpgradeItemCombineInitializeData );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_REQ,			OnReqUpgradeItemCombineInitialize );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_LOOKS_CHANGE_REQ,				OnReqUpgradeItemLooksChange );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_LOOKS_INITIALIZE_REQ,			OnReqUpgradeItemLooksInitialize );
#ifdef BARUNA_ULTIMATE_UPDATE
	ON_MSG( PACKETTYPE_BARUNAULTIMATE_TRANSWEAPON_REQ,				OnReqUpgradeItemUltimateTrans );
#endif

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// teleportmap
	ON_MSG( PACKETTYPE_TELEPORT_MAP_TO_POINT_REQ, OnReqTeleportMapToPoint );
	ON_MSG( PACKETTYPE_TELEPORT_MAP_ADD_MY_POINT_REQ, OnReqTeleportMapAddMyPoint );		// 텔레포트 지도 아이템 현재 위치 등록 요청
	ON_MSG( PACKETTYPE_TELEPORT_MAP_CHG_POINT_NAME_REQ, OnReqTeleportMapChgPointName );	// 텔레포트 지도 아이템 등록 위치 이름 수정 요청
	ON_MSG( PACKETTYPE_TELEPORT_MAP_DEL_POINT_REQ, OnReqTeleportMapDelPoint );			// 텔레포트 지도 아이템 등록 위치 삭제 요청
	ON_MSG( PACKETTYPE_TELEPORT_MAP_DEL_ALL_POINT_REQ, OnReqTeleportMapDelAllPoint );		// 텔레포트 지도 아이템 등록 위치 모두 삭제 요청
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 이벤트용 아레나
	ON_MSG( PACKETTYPE_EVENT_ARENA_MOVE_BATTLE_WAIT_NOTI, OnNotiEventArenaMoveBattleWait );	// NPC 이용 선수대기 지역 이동 요청


	// 이벤트용 아레나(관리자)
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_SET_TONEMENT_NOTI, OnNotiEventArenaManageSetTonement );		// 토너먼트 설정 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_SET_RED_TEAM_NOTI, OnNotiEventArenaManageSetRedTeam );		// 레드팀 설정 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_SET_BLUE_TEAM_NOTI, OnNotiEventArenaManageSetBlueTeam );		// 블루팀 설정 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_CALL_PLAYERS_NOTI, OnNotiEventArenaManageCallPlayers );		// 선수소환 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_JOIN_PLAYERS_NOTI, OnNotiEventArenaManageJoinPlayers );		// 선수입장 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_BATTLE_START_NOTI, OnNotiEventArenaManageBattleStart );		// 전투 시작 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_GAME_END_NOTI, OnNotiEventArenaManageGameEnd );			// 게임 종료 통지
	ON_MSG( PACKETTYPE_EVENT_ARENA_MANAGE_GAME_STOP_NOTI, OnNotiEventArenaManageGameStop );			// 게임 중지 통지

	ON_MSG( PACKETTYPE_CHARGE_ZONE_TICKET_ENTERANCE_REQ, OnReqChargeZoneEnterance );
	ON_MSG( PACKETTYPE_CHARGE_ZONE_TICKET_LEAVE_REQ, OnReqChargeZoneLeave );

	ON_MSG( PACKETTYPE_TREASURE_CHEST_OPEN_BY_KEY_REQ, OnReqTreasureChestOpenByKey );

	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_USER_INFO_REQ, OnConsignmentSale_UserInfoReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGIST_REQ, OnConsignmentSale_RegistReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_REQ, OnConsignmentSale_CancelRegistedReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_REQ, OnConsignmentSale_RegistedInfoListReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_REQ, OnConsignmentSale_BuyItemReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_REQ, OnConsignmentSale_CollectSaleGoldReq );
	ON_MSG( PACKETTYPE_CONSIGNMENT_SALE_SEARCH_REQ, OnConsignmentSale_SearchReq );
	ON_MSG(	PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_INFO_LIST_REQ, OnConsignmentSale_SaleGoldInfoListReq );
	ON_MSG(	PACKETTYPE_CONSIGNMENT_SALE_CALC_TAX_REQ, OnConsignmentSale_CalcTaxReq );

	ON_MSG( PACKETTYPE_CHARACTER_SERVER_TRANSFORM_TO_OTHER_SERVER_REQ, OnReqCharacterServerTransform );

	//ON_MSG( PACKETTYPE_DO_APPLY_TARGET_ITEM, OnDoApplyItem );

	ON_MSG( PACKETTYPE_TELEPORT_TO_NPC, OnTeleportToNPC );

	ON_MSG( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_SYNC_REQ, OnItemUpgradeRandomOptionSyncReq );

	ON_MSG( PACKETTYPE_MADRIGAL_GIFT_ITEM_RECV_REQ, OnMadrigalGiftRewardItemRecvReq );

	ON_MSG( PACKETTYPE_FLYFF_PIECE_EXCHANGE_LIST_REQ, OnFlyffPieceExchangeListReq );
	ON_MSG( PACKETTYPE_EXCHANGE_FLYFF_PIECE_ITEM_REQ, OnExchangeFlyffPieceItemReq );

	ON_MSG( PACKETTYPE_ITEM_EXCHANGE_REQ, OnItemExchangeReq );
#ifdef PAT_LOOTOPTION
	ON_MSG( PACKETTYPE_PETLOOTTYPE_REQ, OnPetLootOption );
#endif // PAT_LOOTOPTION
#ifdef CARD_UPGRADE_SYSTEM
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_CARD_REQ,				OnReqUpgradeItemCard );
#endif	// CARD_UPGRADE_SYSTEM
#ifdef	KEYBOARD_SET
	ON_MSG( PACKETTYPE_KEYBOARDMODESETTING,				OnKeyBoardModeSetting );
#endif	// KEYBOARD_SET 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_SETGEM_REQ,				OnReqUpgradeItemSetGem );
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_REMOVEGEM_REQ,			OnReqUpgradeItemRemoveGem );
#endif	
#ifdef COSTUME_UPGRADE_MIX
	ON_MSG( PACKETTYPE_ITEM_UPGRADE_MIX_REQ,			OnReqUpgradeItemMix );
#endif
}

CDPSrvr::~CDPSrvr()
{
	m_dpidCache = DPID_UNKNOWN;			// 캐쉬서버 DPID
}

void CDPSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
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

void CDPSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + sizeof(DPID);
	u_long uBufSize		= dwMsgSize - sizeof(DPID);

	CAr ar( lpBuffer, uBufSize );
	GETTYPE( ar )	

	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	//FLWSUser* pUser	= g_xWSUserManager->GetUser( idFrom, *(UNALIGNED LPDPID)lpMsg );
	//if( pUser )
	//	FLINFO_LOG( PROGRAM_NAME, _T( "[ UserMessageHandler %d %x ]" ), pUser->m_idPlayer, dw );
	if( pfn ) 
	{
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, lpBuffer, uBufSize );
	}
}

void CDPSrvr::OnAddConnection( DPID /*dpid*/ )
{
}

void CDPSrvr::OnRemoveConnection( DPID dpid )
{
	// 실제 캐쉬 서버가 붙을 경우도 있고, 테스트 용으로 telnet이 붙을 경우도 있다.
	// 캐쉬서버와 연결이 끊기면, 캐쉬 서버와 연계된 유저들을 끊어야 한다.
	// 위 2가지를 고려해서, 유저가 등록된 연결만을 실제 캐쉬서버로 간주하고
	// 등록하게 한다.
	if( dpid == m_dpidCache )
	{
		g_xWSUserManager->RemoveAllUsers();		
		m_dpidCache = DPID_UNKNOWN;	
	}
}

void CDPSrvr::OnAddUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	m_dpidCache = dpidCache;		// 캐쉬서버의 DPID를 보관한다.

	TCHAR	lpszAccount[MAX_ACCOUNT] = { 0, };
	TCHAR	lpszpw[MAX_ENCRYPT_PWD] = { 0, };
	TCHAR	lpAddr[16] = { 0, };
	DWORD	dwAuthKey;
	u_long	idPlayer;
	BYTE	nSlot;
	DPID	dpidSocket;

	ar >> dwAuthKey >> idPlayer >> nSlot;
	ar >> dpidSocket;
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar.ReadString( lpszpw, _countof( lpszpw ) );
	ar.ReadString( lpAddr, _countof( lpAddr ) );

	if( nSlot >= 3 )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ nSlot Overflow Default Max Character Slot(%d) ]" ), nSlot );
		return;
	}

	FLWSUser* pUser = (FLWSUser*)prj.GetUserByID( idPlayer );
	if( pUser )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ Exist User. account(%s), idPlayer(%07d), DPID(%d) ]" ), lpszAccount, idPlayer, dpidUser );

		// 캐쉬서버에는 socket번호를 보내야 한다. ( pUser->m_Snapshot.dpidUser는 소켓번호 )
		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		QueryDestroyPlayer( dpidCache, dpidSocket, dpidUser, idPlayer );	
		return;
	}

	pUser = g_xWSUserManager->AddUser( dpidCache, dpidUser, dpidSocket );
	if( pUser == NULL )
	{
		return;
	}

	pUser->m_dwAuthKey = dwAuthKey;
	memcpy_s( pUser->m_playAccount.lpAddr, sizeof( pUser->m_playAccount.lpAddr ), lpAddr, 16 );

	//	TRANS
	//BEFORESENDDUAL( arJoin, PACKETTYPE_JOIN, dpidCache, dpidUser );
	//arJoin << dwAuthKey;
	//arJoin.WriteString( lpszAccount );
	//arJoin.WriteString( lpszpw );
	//arJoin << nSlot << idPlayer;
	//SEND( arJoin, &g_dpDBClient, DPID_SERVERPLAYER );
	FLPacketWtoDB_ChannelJoinReq req;
	req.m_kAccount.dwAuthKey		= dwAuthKey;
	req.m_kAccount.bySlot			= nSlot;
	req.m_kAccount.uPlayerID		= idPlayer;
	FLStrcpy( req.m_kAccount.szAccount, _countof( req.m_kAccount.szAccount ), lpszAccount );
	FLStrcpy( req.m_kAccount.szPassword, _countof( req.m_kAccount.szPassword ), lpszpw );

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &req );
}


void CDPSrvr::OnRemoveUser( CAr & /*ar*/, DPID /*dpidCache*/, DPID dpidUser, LPBYTE, u_long )
{
	g_xWSUserManager->RemoveUser( (DWORD)dpidUser ); // dpidUser는 CACHE에서 사용되는 serial한 값 
}

void CDPSrvr::OnChat( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long uBufSize )
{
	static	TCHAR	sChat[1024];
	if( uBufSize > 1031 || uBufSize < 0 )	// 4 + 4 + 1024 - 1		= 1031
		return;

	ar.ReadString( sChat, _countof( sChat ) );
	CString strChat	= sChat;
	strChat.Replace( "\\n", " " );

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->m_dwAuthorization >= AUTH_LOGCHATTING )		// 일반유저가 아니면 로그남김 모든 로그남김
		{
			g_dpDBClient.SendLogGamemaChat( pUser, strChat );
		}

		if( sChat[0] == '/'  && ParsingCommand( strChat, (CMover*)pUser ) )
			return;
		if( pUser->IsMode( TALK_MODE ) )
			return;

		int nText	= pUser->GetMuteText();
		if(  nText )
		{
			pUser->AddDefinedText( nText );
			return;
		}

		if( !( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ) ) ) )
		{
			//			strChat		= sChat;
			//			strChat.Replace( "\\n", " " );
			ParsingEffect( sChat, _countof( sChat ), strlen(sChat) );
		}
		strChat	= sChat;
		strChat.Replace( "\\n", " " );
		//		RemoveCRLF( sChat );


		g_xWSUserManager->AddChat( pUser, strChat );
	}
}

void CDPSrvr::OnCtrlCoolTimeCancel( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser;
	CCommonCtrl* pCtrl;
	pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	((CMover*)pUser)->m_dwCtrlReadyTime = 0xffffffff;

	pCtrl = (CCommonCtrl*)prj.GetCtrl( ((CMover*)pUser)->m_dwCtrlReadyId );
	if( IsValidObj( pCtrl ) == FALSE )
		return;

	((CMover*)pUser)->m_dwCtrlReadyId = NULL_ID;

	pCtrl->m_dwCtrlReadyTime = 0xffffffff;
	pCtrl->m_bAction         = FALSE;
}

void CDPSrvr::OnDoEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwItemObjID;
	int nPart;

	ar >> dwItemObjID;
	ar >> nPart;		

	if( nPart >= MAX_HUMAN_PARTS )	
		return;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwItemObjID );
	if( IsUsableItem( pItemElem ) == FALSE )
		return;
	if( nPart > 0 )
	{
		if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
		{
			if( pItemElem != pUser->m_Inventory.GetEquip( nPart ) )
				return;
		}
	}
	else
	{
		if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
			return;
	}
	PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
	if( pItemProp && pItemProp->dwParts == PARTS_RIDE )
	{
		if( !pUser->m_Inventory.IsEquip( dwItemObjID ) )
		{
			FLOAT fVal;
			ar >> fVal;
			if( fVal != pItemProp->fFlightSpeed )
			{
				pUser->AddDefinedText( TID_GAME_MODIFY_FLIGHT_SPEED );
				return;	
			}
		}
	}

	if( pUser->IsDie() == FALSE )
	{
		g_pItemUsing->DoUseEquipmentItem( pUser, pItemElem, dwItemObjID, nPart );
	}
}

void CDPSrvr::OnMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwSrcObjIndex, dwDestObjIndex;
	ar >> dwSrcObjIndex >> dwDestObjIndex;

	if( dwSrcObjIndex == dwDestObjIndex )
	{
		return;
	}

	if( dwSrcObjIndex < MAX_HUMAN_PARTS || dwDestObjIndex < MAX_HUMAN_PARTS )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	FLItemElem* pItemSrc = pUser->m_Inventory.GetAt( dwSrcObjIndex );
	FLItemElem* pItemDst = pUser->m_Inventory.GetAt( dwDestObjIndex );
	if( pItemDst == NULL || IsUsableItem( pItemDst ) ) // 빈 공간 or 거래중이지 않는 아이템 ?			
	{
		if( IsUsableItem( pItemSrc ) )					// 거래중이지 않는 아이템 ?
		{
			if( pUser->m_Inventory.Swap( dwSrcObjIndex, dwDestObjIndex ) == true )
			{
				pUser->AddMoveItem( dwSrcObjIndex, dwDestObjIndex );
			}
		}
	}
}
#ifdef INVENTORY_ITEM_ALIGN
void CDPSrvr::OnAlignItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	DWORD dwIndex;
	ar >> dwIndex;

	pUser->m_Inventory.Align(dwIndex);
	pUser->AddAlignItem(dwIndex);

}
#endif	// INVENTORY_ITEM_ALIGN
void CDPSrvr::OnDropItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// 유럽 아이템 복제 이슈로 인해 드랍불가 코드 추가
	//	return;
	
	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) {
		return;
	}
	if( !g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
	{
		static int min = ::GetTickCount()/60000;
		static int count = 0;
		int min2 = ::GetTickCount()/60000 ;
		if ( min2 != min )
		{
			min = min2;
			count = 0;
		}
		if ( count > 100 )
		{
			pUser->AddDefinedText( TID_GAME_DIALOGNODROPITEM );
			return;
		}
		count++;
	}

	DWORD dwItemType;
	DWORD dwItemId;
	int nDropNum;
	D3DXVECTOR3 vPos;

	ar >> dwItemType >> dwItemId >> nDropNum >> vPos;


	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( nDropNum <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nDropNum [%d]" ), nDropNum );

		return;
	}
	//	END100708
	//////////////////////////////////////////////////////////////////////////


	//FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		//////////////////////////////////////////////////////////////////////////
		// 이벤트 아레나 아이템 드랍 금지
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 아이템 드랍 금지 User: %s" ), pUser->GetName() );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
		{
			FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( dwItemId );
			if( IsUsableItem( pItemElem ) && pUser->IsDropable( pItemElem, FALSE ) )
				pUser->RemoveItem( dwItemId, nDropNum );
		}
		else
		{
			pUser->DropItem( (DWORD)dwItemId, nDropNum, vPos );
		}
	}
}

void CDPSrvr::OnDropGold( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	//8차게임내돈드롭금지
	return;
}


void CDPSrvr::OnReplace( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE, u_long )
{
	FLASSERT( 0 );
	FLERROR_LOG( PROGRAM_NAME, _T( "CDPSrvr::OnReplace called" ) );
}

void CDPSrvr::OnScriptDialogReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	static TCHAR lpKey[256];
	OBJID objid;
	int nGlobal1, nGlobal2, nGlobal3, nGlobal4;

	ar >> objid;
	ar.ReadString( lpKey, _countof( lpKey ) );

	ar >> nGlobal1 >> nGlobal2 >> nGlobal3 >> nGlobal4;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;

		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
		{

#ifdef __ENCHANT_BARUNA16
			if( pMover->GetNPCOwner() != NULL_ID && pMover->GetNPCOwner() != pUser->GetId() )
			{
				pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
				return;					
			}
#endif //__ENCHANT_BARUNA16

			FLTRACE_LOG( PROGRAM_NAME, _T( "npc = %s, key = %s, n1 = %d, n2 = %d, n3 = %d, n4 = %d" ),
				pMover->GetName(), lpKey, nGlobal1, nGlobal2, nGlobal3, nGlobal4 );

			D3DXVECTOR3 vOut	= pUser->GetPos() - pMover->GetPos();

			if( fabs( (double)D3DXVec3LengthSq( &vOut ) ) > MAX_LEN_MOVER_MENU )
			{
				return;
			}
#if !defined(__REMOVE_SCIRPT_060712)
			CScriptDialog::SetLatestDialog( pMover->GetName(), lpKey );
#endif
			if( pMover->m_pNpcProperty &&
				pMover->m_pNpcProperty->IsDialogLoaded() )
			{
				if( lstrlen( lpKey ) == 0 )
					FLStrcpy( lpKey, _countof( lpKey ), _T( "#init" ) );
				if( nGlobal3 == 0 )	
					nGlobal3 = (int)pMover->GetId();
				if( nGlobal4 == 0 )	
					nGlobal4 = (int)pUser->GetId();

				pMover->m_pNpcProperty->RunDialog( lpKey, NULL, nGlobal1, (int)pMover->GetId(), (int)pUser->GetId(), nGlobal2 );


				// 퀘스트 조건 대화에 맞는 키일 경우는 대화 성공 플렉을 세팅하고 퀘스트 정보를 클라이언트에 보내준다.
				for( size_t i = 0; i < pUser->m_nQuestSize; i++ )
				{
					LPQUEST lpQuest = &pUser->m_aQuest[ i ];
					QuestProp* pQuestProp = pUser->m_aQuest[ i ].GetProp();
					if( pQuestProp )
					{
						if( strcmp( pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey ) == 0 )
						{
							if( strcmp( pQuestProp->m_szEndCondDlgAddKey, lpKey ) == 0 )
							{
								lpQuest->m_bDialog = TRUE;
								pUser->AddSetQuest( lpQuest );
								break;
							}
						}
					}
				}

			}
		}
	}
}

// 사용자가 OK하면 부활을 쓰게 한다
void CDPSrvr::OnRevivalBySkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	bool allowRevival = false;
	ar >> allowRevival;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE || pUser->m_Resurrection_Data.bUseing == FALSE ) {
		return;
	}

	if( allowRevival == false || pUser->CanRevival() == false ) {
		pUser->m_Resurrection_Data.bUseing = FALSE;
		return;
	}

	// 부활 스킬 체크
	if( g_pEventArenaGlobal->IsArenaChannel() == true && g_pEventArena->IsPlaying() == false ) {
		pUser->m_Resurrection_Data.bUseing = FALSE;
		pUser->AddDefinedText( TID_MMI_EVENTARENA_REINSTATEERROR );
		return;
	}

	pUser->Revival();

	// 부활 SFX효과
	g_xWSUserManager->AddCreateSfxObj( pUser, XI_INDEX( 283, XI_SKILL_ASS_HEAL_RESURRECTION01 ) );

	// 부활하기
	g_xWSUserManager->AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_BY_SKILL );
	pUser->m_pActMover->SendActMsg( OBJMSG_RESURRECTION );
}

void CDPSrvr::OnRevivalCurrentPos( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) {
		return;
	}

	if( pUser->CanRevival() == false ) {
		return;
	}

	FLItemElem* pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 25196, II_SYS_SYS_SCR_RESURRECTION_02 ) );
	if( IsUsableItem( pItemElem ) == FALSE ) 
	{
		pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 10431, II_SYS_SYS_SCR_RESURRECTION ) );

		if( IsUsableItem( pItemElem ) == FALSE ) 
			return;
	}
		
	pUser->Revival( true, true );

	g_xWSUserManager->AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_TO_CURRENT_POS );

	if( pUser->GetWorld() != NULL && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR ) {
		g_GuildCombatMng.JoinObserver( pUser );
	}
	else {
		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
		if( pItemProp && pItemProp->dwSfxObj3 != NULL_ID ) {
			g_xWSUserManager->AddCreateSfxObj( pUser, pItemProp->dwSfxObj3 );
		}

		// 상용화 아이템 사용 로그 삽입
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		pUser->RemoveItem( pItemElem->m_dwObjId, 1 );
	}
}

void CDPSrvr::OnRevivalLodestar( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) {
		return;
	}

	if( pUser->CanRevival() == false ) {
		return;
	}

	pUser->Revival();

	g_dpDBClient.SendLogLevelUp( pUser, 9 );	// 로드스타로 부활 로그

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL ) {
		return;
	}

	if( pWorld->GetID() == WI_WORLD_GUILDWAR ) {
		g_xWSUserManager->AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_TO_CURRENT_POS );
		g_GuildCombatMng.JoinObserver( pUser );
	}
	else {
		g_xWSUserManager->AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_TO_LODESTAR );

		T_WORLD_POSITION pos;
		if( pUser->GetRevivalPos( pos ) == true ) {
			pUser->REPLACE( g_uIdofMulti, pos.dwWorldID, pos.tPos, REPLACE_FORCE, pos.layer );
		}
	}		
}

void CDPSrvr::OnRevivalLodelight( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나에서는 부활을 할 수 없습니다" ) );
		return;
	}
}

void CDPSrvr::OnSetLodelight( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_MARKING, pUser->GetWorld(), pUser->GetPos() ) )
			return;
		pUser->SetMarkingPos();
		pUser->AddDefinedText( TID_GAME_LODELIGHT, "" );
	}
}

void CDPSrvr::OnCorrReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID idObj;
	ar >> idObj;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );		// 어느유저로부터 날아온거냐.
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMover *pMover = prj.GetMover( idObj );	// 선택된 오브젝트의 포인터
		if( IsValidObj( pMover ) )
		{
			pUser->AddCorrReq( pMover );	// 요청한 클라에게 선택된 오브젝트의 정보를 보냄.
		}
	}
}

void CDPSrvr::OnCreateGuildCloak( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );		// 어느유저로부터 날아온거냐.
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CGuild *pGuild = pUser->GetGuild();
		if( pGuild == NULL )
			return;
		if( pGuild->m_dwLogo == 0 ||							//  로고가 지정되어 있지 않거나
			pGuild->IsMaster( pUser->m_idPlayer ) == FALSE )	// 마스터가 아니거나
		{
			if( pGuild->m_dwLogo == 0 )
			{
				// 로그가 지정되지 않아서 못만듬
				pUser->AddDefinedText( TID_GAME_GUILDSETTINGLOGO, "" );
			}
			else
			{
				// 마스터가 아니므로 만들수가 없음.
				pUser->AddDefinedText( TID_GAME_GUILDONLYMASTERLOGO, "" );
			}
			return;
		}

		// 길드창고가 망토를 넣을 공간이 충분한지 체크한다. 물론 길드 망토를 길드 창고에 넣을때이다. 아니라면 주석처리 해주세용
		if ( pGuild->m_GuildBank.GetEmptyCountByInventoryType( INVEN_TYPE_NONE ) <= 0 )
		{
			pUser->AddDefinedText( TID_GAME_GUILDBANKFULL, "" );		// 길드창고가 꽉찼시유~
			return;
		}

		if ( pGuild->m_nGoldGuild >= 10000 )	 // 길드창고에 돈이 충분하냐?
		{
			pGuild->m_nGoldGuild -= 10000;

			// 길드창고에서 돈 지불하고, 길드 망토를 길드창고에 생성시킴.
			//			BYTE nId;
			FLItemElem itemElem;
			if( pGuild->m_dwLogo == 999 )	// 커스텀 로고로 설정되어 있을때.
				itemElem.m_dwItemId	= ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK );		// 커스텀용 민짜 망토생성.
			else
			{
				if(pUser->IsAuthHigher(AUTH_GAMEMASTER) && pGuild->m_dwLogo > CUSTOM_LOGO_MAX - 7) // GM Guild Logo사용 시 임의로 망토 생성
					itemElem.m_dwItemId	= ITEM_INDEX( 4602, II_ARM_S_CLO_CLO_SYSCLOAK01 );
				else
					itemElem.m_dwItemId	= ITEM_INDEX( 4602, II_ARM_S_CLO_CLO_SYSCLOAK01 ) + (pGuild->m_dwLogo - 1);
			}
			itemElem.m_nItemNum		= 1;
			// 길드 아이디를 망토에 박음. 클라에선 숫자를 기반으로 커스텀 망토를 읽는다.
			// 커스텀 망토가 아닌경우는 이번호로 길드이름을 보여준다.
			itemElem.m_idGuild	= pGuild->m_idGuild;			


			// a. 요청한 클라에게 길드창고 페냐가 소모되었음을 알린다.
			// b. 현재 같은 서버에 있는 같은 길드원인 클라이언트에게 페냐가 소모되었음을 알린다.
			// c. 다른 멀티서버셋에 있는 같은 길드원인 클라이언트에게 페냐가 소모되었을을 알린다.
			itemElem.SetSerialNumber();
			PT_ITEM_SPEC pItemProp		= itemElem.GetProp();
			if( pItemProp )
				itemElem.m_nHitPoint	= ( pItemProp->dwEndurance == -1 ) ? 0 : pItemProp->dwEndurance;//pItemProp->dwEndurance;
			else
				itemElem.m_nHitPoint	= 0;

			pUser->AddPutItemGuildBank( &itemElem );
			pGuild->m_GuildBank.Add( &itemElem );
			g_xWSUserManager->AddPutItemElem( pUser, &itemElem );

			// 자신의 길드원들의 루프를 돌면서 길드망토를 사서 10000페냐가 소모되었다고 알려준다.
			// 물론 루프에서 요청한 클라이언트에게도 메시지를 함께 보낸다.
			CGuildMember*	pMember;
			FLWSUser*			pUsertmp;
			CGuild*			pGuild = pUser->GetGuild();
			for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
				i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) )
				{
					pUsertmp->AddGetGoldGuildBank( 10000, 2, pMember->m_idPlayer, 1 );	// 2는 업데이트 해야할 클라이게
				}
			}
			// 현 멀티셋 서버에는 위 루틴이 모두 10000페냐가 소모됨을 알렸으므로 DPCoreClient로 캐시서버에 요청하여 
			// 모든 멀티셋에 10000페냐가 소모되었다고 알린다. 물론 보내는 이 멀티셋 서버는 이 메시지를 무시해야 한다. ( 무시하게 해놨지만 잘 될런지 -_- )
			g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, 10000, 2, 1 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
			UpdateGuildBank(pGuild, GUILD_CLOAK, 0, pUser->m_idPlayer, &itemElem, 10000, 1 ); // 0은 길드 페냐를 업데이트 한다는 것이다.(실은 모든것을 업데이트하지만 -_-)
			pUser->AddDefinedText( TID_GAME_GUILDCREATECLOAK, "" );
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_GUILDNEEDGOLD, "" );		// 길드창고에 돈이 엄떵!
		}
	}
}

void CDPSrvr::OnQueryGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	ar >> objid;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) && !pMover->IsEmptyDestObj() )
			pUser->AddGetDestObj( objid, pMover->GetDestId(), pMover->m_fArrivalRange );
	}
}

void CDPSrvr::OnGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid, objidDest;
	ar >> objid >> objidDest;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		pUser->SetDestObj( objidDest );
	}
	/*
	{
	if( NULL_ID == objid )
	{
	pUser->SetDestObj( objidDest );
	}
	else
	{
	FLWSUser* ptr	= prj.GetUser( objid );
	if( IsValidObj( ptr ) )
	ptr->AddGetDestObj( pUser->GetId(), objidDest, ptr->m_fArrivalRange );
	}
	}
	*/
}

//  void CDPSrvr::OnQueryGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
//  {
//  	OBJID objid;
//  	ar >> objid;
//  
//  	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
//  	if( IsValidObj( pUser ) )
//  	{
//  		if( pUser->IsDie() == TRUE )
//  		{
//  			return;
//  		}
//  
//  		CMover* pMover	= prj.GetMover( objid );	// 상대방
//  		if( IsValidObj( pMover ) )
//  		{
//  			if( FALSE == pMover->IsPlayer() )
//  			{
//  				pUser->AddGetPos( objid, pMover->GetPos(), pMover->GetAngle() );
//  			}
//  			else
//  			{
//  				( (FLWSUser*)pMover )->AddQueryGetPos( pUser->GetId() );
//  			}
//  		}
//  	}
//  }
// 
//  void CDPSrvr::OnGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
//  {
//  	D3DXVECTOR3 vPos;
//  	float fAngle;
//  	OBJID objid;
//  	ar >> vPos >> fAngle >> objid;
//  
//  	if( _isnan((double)fAngle) )
//  		return;
//  
//  	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
//  	if( IsValidObj( pUser ) )
//  	{
//  		if( pUser->IsDie() == TRUE )
//  		{
//  			return;
//  		}
//  
//  		D3DXVECTOR3 vDistance	= pUser->GetPos() - vPos;
//  		if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
//  		{
//  			//			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_GETPOS" ) );
//  			return;
//  		}
//  
//  		if( NULL_ID == objid )
//  		{
//  			pUser->SetPos( vPos );
//  			pUser->SetAngle( fAngle );
//  
//  			pUser->m_fWaitQueryGetPos	= FALSE;
//  
//  			if( FALSE == pUser->IsEmptyDestPos() )
//  				pUser->SetDestPos( pUser->GetDestPos(), pUser->m_bForward, FALSE );
//  		}
//  		else
//  		{
//  			FLWSUser* ptr	= prj.GetUser( objid );
//  			if( IsValidObj( ptr ) )
//  				ptr->AddGetPos( pUser->GetId(), vPos, fAngle );
//  		}
//  	}
//  }

void CDPSrvr::OnPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid, uMemberid;
	BOOL bTroup;
	ar >> uLeaderid >> uMemberid;
	ar >> bTroup;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_idPlayer == uLeaderid )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		InviteParty( uLeaderid, uMemberid, bTroup );
	}
}

void CDPSrvr::OnPartyRequestCancle( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid, uMemberid;
	int nMode;
	ar >> uLeaderid >> uMemberid >> nMode;

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( uLeaderid );
	if( IsValidObj( pUser ) )
	{
		pUser->AddPartyRequestCancel( uLeaderid, uMemberid, nMode );
	}
}

void	CDPSrvr::OnConfirmPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderPlayerID	= NULL_PLAYER_ID;
	u_long uMemberPlayerID	= NULL_PLAYER_ID;

	ar >> uLeaderPlayerID >> uMemberPlayerID;

	FLWSUser* pMember	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pMember ) == FALSE || pMember->m_idPlayer != uMemberPlayerID )
	{
		return;
	}

	FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( uLeaderPlayerID );
	if( IsValidObj( pLeader ) == FALSE )
	{
		return;
	}

	const DWORD dwResult	= g_PartyMng.CanInviteParty( uLeaderPlayerID, uMemberPlayerID );
	if( dwResult == FSC_PARTY_INVITE_SUCCESS )
	{
		g_PartyMng.RequestAddPartyMemberToCS( uLeaderPlayerID, uMemberPlayerID );
	}
	else
	{
		switch( dwResult )
		{
		case FSC_PARTY_INVITE_MEMBER_HAVE_PARTY:
//			pLeader->AddPartyRequestCancel( uLeaderPlayerID, uMemberPlayerID, 1 );
			pLeader->AddDefinedText( TID_GAME_PARTYEXISTCHR, "\"%s\"", pMember->GetName() );
			break;

		case FSC_PARTY_INVITE_NOT_LEADER:
			pLeader->AddDefinedText( TID_GAME_PARTYNOINVATE );
			break;

		case FSC_PARTY_INVITE_MEMBER_OVERFLOW:
			pLeader->AddDefinedText( TID_GAME_FULLPARTY3 );
			break;

		case FSC_PARTY_INVITE_CANNOT_WORLD:
			pLeader->AddDefinedText( TID_GAME_GUILDCOMBAT_CANNOT_PARTY );
			break;

		case FSC_PARTY_INVITE_GUILD_COMBAT:
			pLeader->AddDefinedText( TID_GAME_GUILDCOMBAT_CANNOT_PARTY );
			break;

		case FSC_PARTY_INVITE_PVP:
			pLeader->AddDefinedText( TID_GAME_PPVP_ADDPARTY );
			break;

		case FSC_PARTY_INVITE_ATTACK_MODE:
			pLeader->AddDefinedText( TID_GAME_BATTLE_NOTPARTY );
			break;

		default:
			break;
		}
	}
}

void CDPSrvr::OnPartySkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid;
	int nSkill;
	ar >> uLeaderid >> nSkill;	

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsInvalidObj( pUser ) )
	{
		return;
	}

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		if( pParty->IsLeader( pUser->m_idPlayer ) )
		{
			pParty->DoUsePartySkill( pUser->m_idparty, pUser->m_idPlayer, nSkill );
		}
	}
}

void CDPSrvr::OnAddFriendReqest( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;

	FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( uLeaderid );
	FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( uMemberid );
	if( IsValidObj( pMember ) && IsValidObj( pLeader ) )
	{
		if( pLeader->IsDie() == TRUE || pMember->IsDie() == TRUE )
		{
			return;
		}

		if( 0 < pLeader->m_nDuel ||  0 < pMember->m_nDuel )
		{
			return;
		}

		// 길드대전장에는 친구추가를 할수 없습니다
		CWorld* pWorldLeader = pLeader->GetWorld();
		CWorld* pWorldMember = pMember->GetWorld();
		if( ( pWorldLeader && pWorldLeader->GetID() == WI_WORLD_GUILDWAR ) ||
			( pWorldMember && pWorldMember->GetID() == WI_WORLD_GUILDWAR ) )
		{			
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}

		if( g_GuildCombat1to1Mng.IsPossibleUser( pLeader ) || g_GuildCombat1to1Mng.IsPossibleUser( pMember ) )
		{
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}

		if( !pLeader->m_RTMessenger.GetFriend( uMemberid ) )
		{
			if( pMember->IsAttackMode() )
				pLeader->AddDefinedText( TID_GAME_BATTLE_NOTFRIEND, "" );
			else
				pMember->AddFriendReqest( uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), pLeader->GetName() );	// 친구 등록 여부 질의
		}
	}
}

// 다른 멀티서버에 있는 캐릭을 추가 시키려면 코어로 보내야 한다.
// 이름으로 오므로 월드에서 idPlayer로 바꿔서 보냄
void CDPSrvr::OnAddFriendNameReqest( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid, uMember;
	char szMemberName[64] = {0,};

	ar >> uLeaderid;
	ar.ReadString( szMemberName, _countof( szMemberName ) );

	uMember	= CPlayerDataCenter::GetInstance()->GetPlayerId( szMemberName );
	FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( uLeaderid );	

	if( IsValidObj( pLeader ) )
	{
		if( pLeader->IsDie() == TRUE )
		{
			return;
		}

		if( uMember > 0 )
		{
			if( !pLeader->m_RTMessenger.GetFriend( uMember ) )
				g_DPCoreClient.SendAddFriendNameReqest( uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), uMember, pLeader->GetName(), szMemberName );
			else
				pLeader->AddFriendError( 1, szMemberName );
		}
		else
		{
			// 이 이름을 가지고 잇는 캐릭은 없음.
			pLeader->AddFriendError( 2, szMemberName );
		}
	}
}


void CDPSrvr::OnAddFriendCancel( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;

	FLWSUser* pLeader = g_xWSUserManager->GetUserByPlayerID( uLeaderid );
	if( IsValidObj( pLeader ) )
		pLeader->AddFriendCancel();	// uMemberid
}

void CDPSrvr::OnActionPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( _GetContentState( CT_NEWUI_19 ) != CS_VER1 )
	{
		return;
	}

	int nAP;
	ar >> nAP;

	if( nAP < 0 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		pUser->m_playTaskBar.m_nActionPoint = nAP;
	}
}


void CDPSrvr::OnRemoveQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwQuestCancelID;
	ar >> dwQuestCancelID;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;

		LPQUEST lpQuest = pUser->GetQuest( dwQuestCancelID );
		if( lpQuest )
		{
			if( lpQuest->m_nState != QS_END )
			{
				QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp && pQuestProp->m_bNoRemove == FALSE )
				{
					pUser->RemoveQuest( dwQuestCancelID );
					pUser->AddCancelQuest( dwQuestCancelID );
					g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, dwQuestCancelID, 30 );
					// 시작시 변신을 했으면 퀘스트 삭제시 변신 해제시킨다.
					if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
					{
						pUser->NoDisguise();
						g_xWSUserManager->AddNoDisguise( pUser );
					}
				}
			}
		}
	}
}

void CDPSrvr::OnQueryPlayerData( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long idPlayer;
	ar >> idPlayer;
	int nVer;
	ar >> nVer;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		if( pPlayerData && pPlayerData->data.nVer != nVer )
			pUser->AddQueryPlayerData( idPlayer, pPlayerData );
	}
}

void CDPSrvr::OnQueryPlayerData2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nSize;
	//	u_long idPlayer;
	ar >> nSize;

	if( nSize > 1024 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		for( int i = 0; i < nSize; i++ )
		{
			PDVer	pdv;
			ar.Read( &pdv, sizeof(PDVer) );
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pdv.idPlayer );
			if( pPlayerData && pPlayerData->data.nVer != pdv.nVer )
				pUser->AddQueryPlayerData( pdv.idPlayer, pPlayerData );
		}
	}
}

void CDPSrvr::OnGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	ar >> objid;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	InviteCompany( pUser, objid );
}

void CDPSrvr::OnIgnoreGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long idPlayer;
	ar >> idPlayer;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		FLWSUser* pPlayer	= g_xWSUserManager->GetUserByPlayerID( idPlayer );		// kingpin
		if( IsValidObj( pPlayer ) )
		{
			if( pPlayer->IsDie() == TRUE )
			{
				return;
			}

			pPlayer->AddDefinedText( TID_GAME_COMACCEPTDENY, "%s", pUser->GetName( TRUE ) );
		}
	}
}

// 로고 변경 
void CDPSrvr::OnGuildLogo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwLogo;
	ar >> dwLogo;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( dwLogo > CUSTOM_LOGO_MAX )
		return;

	if( dwLogo > 20 && !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		return;

	g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
}

// 공헌도 
void CDPSrvr::OnGuildContribution( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE cbPxpCount, cbItemFlag;
	int nGold;

	cbItemFlag = 0;
	ar >> cbPxpCount >> nGold;
	ar >> cbItemFlag;

	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) 
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( nGold > 0 )
	{
		//		if( pUser->GetGold() >= nGold ) 
		if( pUser->CheckUserGold( nGold, false ) == true )
		{
			if( g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, nGold ) )
			{
				pUser->AddGold( -nGold );

				LogItemInfo aLogItem;
				//aLogItem.Action = "W";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = "GUILDBANK";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "W" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GUILDBANK" );
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() + nGold;
				aLogItem.Gold2 = pUser->GetGold();
				//aLogItem.ItemName = "SEED";
				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
				aLogItem.kLogItem.nQuantity = nGold;
				OnLogItem( aLogItem );
			}
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_GUILDNOTENGGOLD, "" );	// 인벤에 돈이부족
		}
	}
	else if( cbItemFlag )
	{
		for( DWORD i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAt( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;

			if( pItemElem->GetProp()->dwItemKind3 != IK3_GEM )
				continue;

			int nValue = 0;

			if( pItemElem->m_nItemNum > 0 )
			{
				// 아이템 레벨에 따라서 공헌도를 다르게 한다.
				nValue = (pItemElem->GetProp()->dwItemLV + 1) / 2;	
				nValue *= pItemElem->m_nItemNum;					
			}

			if( nValue > 0 )
			{
				if( g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, nValue ) )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action = "V";
					//aLogItem.SendName = pUser->GetName();
					//aLogItem.RecvName = "GUILDBANK";
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "V" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GUILDBANK" );
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
					pUser->RemoveItem( pItemElem->m_dwObjId, pItemElem->m_nItemNum );
				}
			}
		}
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

// 공지사항
void CDPSrvr::OnGuildNotice( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char szNotice[MAX_BYTE_NOTICE];
	ar.ReadString( szNotice, _countof( szNotice ) );

	if( strlen( szNotice ) == 0 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)szNotice );
}

void CDPSrvr::OnDuelRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	FLWSUser* pDstUser = g_xWSUserManager->GetUserByPlayerID( uidDst );
	if( IsValidObj( pUser ) && IsValidObj( pDstUser ) )
	{
		if( pUser->IsDie() == TRUE || pDstUser->IsDie() == TRUE )
		{
			return;
		}

		if( 0 < pUser->m_idparty && pUser->m_idparty == pDstUser->m_idparty )
		{
			pUser->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

		if( 0 < pUser->m_idparty ||  0 < pDstUser->m_idparty )
			return;

		if( g_pEventArenaGlobal->IsArenaChannel() )	// 이벤트 아레나 듀얼 불가
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나에서는 듀얼이 불가능 합니다. User: %s" ), pUser->GetName() );
			return;
		}

		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;
		if( pDstUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;

		int	nState = pUser->GetSummonState();
		if( nState != 0 )
			return;
		nState = pUser->GetSummonState();
		if( nState != 0 )
			return;

		if( pUser->IsPVPInspection( pDstUser, 1 ) )
		{
			if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
			{
				pUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
			}
			else
			{
				pUser->m_tmDuelRequest	= GetTickCount();
				pDstUser->AddDuelRequest( uidSrc, uidDst );
			}
		}
	}
}

// 듀얼승락을 받음.  두캐릭터에게 시작하라고 보내줘야 함.
void CDPSrvr::OnDuelYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	FLWSUser* pSrc = g_xWSUserManager->GetUserByPlayerID( uidSrc );
	FLWSUser* pDst	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{
		if( pSrc->IsDie() == TRUE || pDst->IsDie() == TRUE )
		{
			return;
		}

		if( 0 < pSrc->m_idparty && pSrc->m_idparty == pDst->m_idparty )
		{
			pSrc->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			pDst->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

		if( 0 < pSrc->m_idparty ||  0 < pDst->m_idparty )
		{
			return;
		}

		if( g_pEventArenaGlobal->IsArenaChannel() )	// 이벤트 아레나 듀얼 불가
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나에서는 듀얼이 불가능 합니다. User: %s" ), pSrc->GetName() );
			return;
		}

		//개인상점 중에는 듀얼 불가 
		if( pSrc->m_vtInfo.VendorIsVendor() || pSrc->m_vtInfo.IsVendorOpen() ||
			pDst->m_vtInfo.VendorIsVendor() || pDst->m_vtInfo.IsVendorOpen() )
		{
			return;	//
		}


		if( pSrc->IsPVPInspection( pDst, 1 ) )
		{
			if( pSrc->m_tmDuelRequest + SEC( 10 ) < GetTickCount() )	// 듀얼 신청 시간을 10초 초과하면
			{
				pSrc->m_tmDuelRequest	= 0;
				return;
			}
			pSrc->m_nDuel = 1;
			pSrc->m_nDuelState = 104;
			pSrc->m_idDuelOther = pDst->GetId();
			pDst->m_nDuel = 1;
			pDst->m_nDuelState = 104;
			pDst->m_idDuelOther = pSrc->GetId();
			pSrc->AddDuelStart( uidDst );	// 서로 상대방에 대한 아이디만 보내주면 된다.
			pDst->AddDuelStart( uidSrc );
			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );
			pDst->SetPosChanged( TRUE );
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		FLWSUser* pSrc = g_xWSUserManager->GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelNo( pUser->GetId() );	// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

// 파티듀얼 ----------------------------------------------------------------
// Src가 Dst에게 한판 붙자고 신청해왔다.
void CDPSrvr::OnDuelPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	FLWSUser* pSrcUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	// 신청자 유저.
	FLWSUser* pDstUser	= g_xWSUserManager->GetUserByPlayerID( uidDst );	// 상대 유저
	if( IsValidObj( pDstUser ) && IsValidObj( pSrcUser ) )
	{
		if( pSrcUser->IsDie() == TRUE || pDstUser->IsDie() == TRUE )
		{
			return;
		}

		if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
		{
			pSrcUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
		}
		else
		{

			if( pSrcUser->IsPVPInspection( pDstUser, 2 ) )
			{
				CParty* pSrcParty = g_PartyMng.GetParty( pSrcUser->m_idparty );		// 신청자의 파티꺼냄
				if( pSrcParty == NULL || pSrcParty->IsLeader( pSrcUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_IPARTYLEADER, "" );	// 파티장이 아닙니다
					return;
				}
				CParty *pDstParty = g_PartyMng.GetParty( pDstUser->m_idparty );		// 도전받는자의 파티꺼냄.
				if( pDstParty == NULL || pDstParty->IsLeader( pDstUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_UPARTYLEADER, "" );		// 상대방이 파티장이 아닙니다
					return;
				}
				pDstUser->AddDuelPartyRequest( uidSrc, uidDst );
			}
		}

	}
}

// 파티듀얼승락을 받음.  모든 양측 파티원들에게 듀얼이 시작됨을 알림.
void CDPSrvr::OnDuelPartyYes( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	FLWSUser* pSrc = g_xWSUserManager->GetUserByPlayerID( uidSrc );	
	FLWSUser* pDst = g_xWSUserManager->GetUserByPlayerID( uidDst );
	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{
		if( pSrc->IsDie() == TRUE || pDst->IsDie() == TRUE )
		{
			return;
		}

		if( pSrc->IsPVPInspection( pDst, 2 ) )
		{
			CParty* pSrcParty = g_PartyMng.GetParty( pSrc->m_idparty );		// 신청자의 파티꺼냄
			if( pSrcParty == NULL || pSrcParty->IsLeader( pSrc->m_idPlayer ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "신청자 파티 읽기 실패 %d %s" ), pSrc->m_idparty, pSrc->GetName() );
				return;
			}
			CParty *pDstParty = g_PartyMng.GetParty( pDst->m_idparty );		// 도전받는자의 파티꺼냄.
			//			if( pDstParty == NULL || pDstParty->IsMember( pDst->m_idPlayer ) == FALSE )
			if( pDstParty == NULL || pDstParty->IsLeader( pDst->m_idPlayer ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "상대 파티 읽기 실패 %d %s" ), pDst->m_idparty, pDst->GetName() );
				return;
			}

			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );	// UpdateRegionAttr
			pDst->SetPosChanged( TRUE );

			g_DPCoreClient.SendSetPartyDuel( pSrcParty->m_uPartyId, pDstParty->m_uPartyId, TRUE );

			pSrcParty->DoDuelPartyStart( pDstParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
			pDstParty->DoDuelPartyStart( pSrcParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelPartyNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		FLWSUser* pSrc = g_xWSUserManager->GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelPartyNo( pUser->GetId() );		// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

void CDPSrvr::OnMoverFocus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidPlayer;
	ar >> uidPlayer;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		FLWSUser* pFocus = g_xWSUserManager->GetUserByPlayerID( uidPlayer );
		if( IsValidObj(pFocus) )
		{
			if( pUser->IsDie() == TRUE )
			{
				return;
			}

			pUser->AddMoverFocus( pFocus );
		}
	}
}

void CDPSrvr::OnSkillTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nCount;
	ar >> nCount;

	if( nCount > MAX_SLOT_QUEUE )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		for( int i = 0 ; i < nCount ; i++)
		{
			BYTE nIndex;
			ar >> nIndex;

			if( nIndex >= MAX_SLOT_QUEUE )
				return;

			ar	>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwShortcut
				>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwId
				>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwType;
			ar	>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwIndex
				>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwUserId
				>> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwData;
		}
	}
}

void CDPSrvr::OnAddAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER2 )
	{
		return;
	}

	BYTE nIndex;
	ar >> nIndex;

	if( nIndex >= MAX_SLOT_APPLET )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwType;
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwData;
		if( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT )
		{
			ar.ReadString( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, _countof( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString ) );
		}
	}
}
void CDPSrvr::OnRemoveAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER2 )
	{
		return;
	}

	BYTE nIndex;
	ar >> nIndex;

	if( nIndex >= MAX_SLOT_APPLET )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT )
		{
			memset( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, 0, sizeof( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString ) );
		}
		pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}
void CDPSrvr::OnAddItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if( nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwShortCut;
		ar >> dwShortCut;

		// Chat Shortcut 10개로 제한
		if(dwShortCut == SHORTCUT_CHAT)
		{
			int nchatshortcut = 0;
			for( int i=0; i<MAX_SLOT_ITEM_COUNT; i++ )
			{
				for( int j=0; j<MAX_SLOT_ITEM; j++ )
				{
					if( pUser->m_playTaskBar.m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT )
						nchatshortcut++;
				}
			}

			if(nchatshortcut > 9)
			{
				pUser->AddDefinedText( TID_GAME_MAX_SHORTCUT_CHAT );
				return;
			}
		}

		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = dwShortCut;
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType;

		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData;
		if( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT )
		{
			ar.ReadString( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, _countof( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString ) );
		}
	}
}
void CDPSrvr::OnRemoveItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if( nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT )
		{
			memset( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, 0, sizeof( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString ) );
		}
		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}

void CDPSrvr::OnPlayerBehavior( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	//	FLTRACE_LOG( PROGRAM_NAME, _T( "OnPlayerBehavior()" ) );
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsInvalidObj( pUser ) )
		return;

	if( pUser->GetWorld() == NULL )
	{
		return;
	}

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( pUser->GetIndex() == 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_PLAYERBEHAVIOR" ) );
		return;
	}

	if( pUser->m_pActMover->IsState( OBJSTA_ATK_ALL ) )
		return;

	pUser->AutoSynchronizer()->Serialize( ar );
}

void CDPSrvr::OnPlayerSetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		OBJID objid;
		float fRange;
		ar >> objid >> fRange;

		CCtrl* pCtrl;

		pCtrl	= prj.GetCtrl( objid );
		if( IsValidObj( pCtrl ) )
		{
			if( pUser->GetDestId() == objid )	// 중복 패킷 전송 막기
				return;

			pUser->SetDestObj( objid, fRange, TRUE );

			//g_xWSUserManager->AddMoverSetDestObj( (CMover*)pUser, objid, fRange );
		}

	}
}


// raider_test 없는 아이템을 사용했다고 하면?
void CDPSrvr::OnDoUseItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser * pUser		= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

//#ifdef __INTERNALSERVER
//	CAr kTestAr( ar );
//	if( FLItemAction::GetInstance().ItemApplyHandler( *pUser, kTestAr ) == TRUE )
//		return;
//#endif



	// 개선된 아이템 Use 처리를 시도
	CAr kCloneAr( ar );
	if( FLItemAction::GetInstance().ItemUseHandler( *pUser, kCloneAr ) == TRUE )
		return;

	// 실패시 기존 아이템 Use 시도
	g_pItemUsing->HandleDoUseItem( pUser, ar );
	//g_pItemUsing->HandleDoUseItem( g_xWSUserManager->GetUser( dpidCache, dpidUser ), ar );
}

//void CDPSrvr::OnDoApplyItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
//{
//	FLWSUser * pUser		= g_xWSUserManager->GetUser( dpidCache, dpidUser );
//	if( IsValidObj( pUser ) == FALSE )
//		return;

//	if( FLItemAction::GetInstance().ItemApplyHandler( *pUser, ar ) == TRUE )
//		return;
//}

void CDPSrvr::OnOpenShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID objid;
		ar >> objid;

		CMover* pVendor	= prj.GetMover( objid );
		if( IsValidObj( pVendor ) && pUser->m_vtInfo.GetOther() == NULL )
		{
			if( pUser->IsDie() == TRUE || pVendor->IsDie() == TRUE )
			{
				return;
			}

			if( pVendor->IsNPC() == FALSE )		// 대상이 NPC가 아니면?
				return;

			if( pUser->IsChaotic() )
			{
				CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
				if( !Propensity.nShop )
					return;
			}

			if( pVendor->IsVendorNPC() == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "VENDOR//%s" ), pVendor->GetName() );
				return;
			}

			if( pUser->m_bBank )
			{
				pUser->AddDefinedText( TID_GAME_TRADELIMITNPC, "" );
				return;
			}

			if( pUser->m_vtInfo.VendorIsVendor() )
				return;

			if( pUser->m_bAllAction == FALSE )
				return;

			pUser->m_vtInfo.SetOther( pVendor );
			pUser->AddOpenShopWnd( pVendor );
		}
	}
}

void CDPSrvr::OnCloseShopWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CMover* pMover = pUser->m_vtInfo.GetOther();
		if( IsValidObj( pMover ) && pMover->IsNPC() )
			pUser->m_vtInfo.SetOther( NULL );
	}
}

void CDPSrvr::OnBuyItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CHAR cTab;
	DWORD dwItemObjID;
	int nNum;
	DWORD dwItemId;

	ar >> cTab >> dwItemObjID >> nNum >> dwItemId;
	if( cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_vtInfo.GetOther() )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMover* pVendor = pUser->m_vtInfo.GetOther();
		LPCHARACTER lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );
		if( lpChar && lpChar->m_nVenderType != VENDOR_TYPE_PENYA )	// 0 - 페냐 상인
			return;

		if( pVendor->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		FLItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId( dwItemObjID );
		if( NULL == pItemElem )
			return;

		if( dwItemId != pItemElem->m_dwItemId )
			return;

		if( nNum > pItemElem->m_nItemNum )
			nNum = pItemElem->m_nItemNum;

		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) && !CTax::GetInstance()->IsOccupationGuildMember( pUser ) )
		{
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			return;
		}

		//int nCost = (int)pItemElem->GetCost();
		//nCost = (int)( prj.m_fShopCost * nCost );
		////nCost = (int)( nCost * ( prj.m_EventLua.GetShopBuyFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue() ) );
		//const float fMulRate = ( prj.m_EventLua.GetShopBuyFactor() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue() - 1.0f ) + 1.0f;
		//nCost	= ( int )( nCost * fMulRate );

		//int nCost		= ( int )( pItemElem->GetCost() * FLGetGlobalBuyFactor() );

		__int64 n64Cost	= pItemElem->GetCost();
		if( FLGetGlobalBuyFactor( n64Cost ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR : ItemID(%u) ]" ), pItemElem->m_dwItemId );
			return;
		}

		if( pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
			n64Cost = PERIN_VALUE;

		if( n64Cost < 1 )
			n64Cost = 1;

		int nPracticable = ( int )( pUser->GetGold() / n64Cost );
		if( nNum > nPracticable )
			nNum = nPracticable;

		if( nNum <= 0 )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}

		int nTax = 0;
		if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pVendor, pItemElem ) )
			nTax = (int)( n64Cost * CTax::GetInstance()->GetPurchaseTaxRate( pVendor ) );
		n64Cost += nTax;
		nTax *= nNum;

		int nGold = ( int )( n64Cost * nNum );
		if( nGold <= 0 )
			return;

		//		if( pUser->GetGold() >= nGold )
		if( pUser->CheckUserGold( nGold, false ) == true )
		{
#ifdef __PERIN_BUY_BUG
			if( pUser->m_dwLastBuyItemTick + 500 > GetTickCount() ) // 아이템 구입시도 후 0.5초이내에 다시 구입시도한 경우
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "__PERIN_BUY_BUG -> [PlayerId:%07d(%s)], [LastTick:%d], [CurTick:%d], [LastTryItem:%d], [Packet:%d,%d,%d,%d]" ),
					pUser->m_idPlayer, pUser->GetName(), pUser->m_dwLastBuyItemTick, GetTickCount(), pUser->m_dwLastTryBuyItem, cTab, dwItemObjID, nNum, dwItemId );
				//g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
				return;
			}
			pUser->m_dwLastTryBuyItem = pItemElem->m_dwItemId;
			pUser->m_dwLastBuyItemTick = GetTickCount();
#endif // __PERIN_BUY_BUG

			FLItemElem itemElem;
			itemElem	= *pItemElem;
			itemElem.m_nItemNum	      = nNum;
			itemElem.SetSerialNumber();

			if( pUser->CreateItem( &itemElem ) )
			{
				LogItemInfo aLogItem;
				//aLogItem.Action = "B";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = pVendor->GetName();
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "B" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pVendor->GetName() );
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				aLogItem.Gold_1 = pVendor->GetGold();

				pItemElem->SetSerialNumber( itemElem.GetSerialNumber() );
				OnLogItem( aLogItem, pItemElem, nNum );		// why do not pass &itemElem as argument?
				pItemElem->SetSerialNumber( 0 );
				pUser->AddGold( -nGold );	

				if( nTax )
					CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_PURCHASE );
			}
			else
				pUser->AddDefinedText( TID_GAME_LACKSPACE, "" );
		}
	}
}

// 칩으로 아이템 구매
void CDPSrvr::OnBuyChipItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CHAR cTab;
	DWORD dwItemObjID;
	int nNum;
	DWORD dwItemId;

	ar >> cTab >> dwItemObjID >> nNum >> dwItemId;
	if( cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_vtInfo.GetOther() )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMover* pVendor = pUser->m_vtInfo.GetOther();
		LPCHARACTER lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );
		if( lpChar && lpChar->m_nVenderType != VENDOR_TYPE_CHIP )	// 1 - 칩 상인
			return;

		if( pVendor->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		FLItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId( dwItemObjID );
		if( NULL == pItemElem )
			return;

		if( dwItemId != pItemElem->m_dwItemId )
			return;

		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) && !CTax::GetInstance()->IsOccupationGuildMember( pUser ) )
		{
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			return;
		}

		if( nNum > pItemElem->m_nItemNum )
			nNum = pItemElem->m_nItemNum;

		// 소지한 칩의 개수가 부족할 때
		if( pUser->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) < (int)( pItemElem->GetChipCost() * nNum ) )
		{
			// 칩 개수 부족 텍스트 출력
			pUser->AddDefinedText( TID_GAME_LACKCHIP );
			return;
		}

		// 인벤토리가 꽉 찼을 때 
		if( pUser->m_Inventory.IsFull( pItemElem, nNum ) )
		{
			pUser->AddDefinedText( TID_GAME_LACKSPACE );
			return;

			// 			int nChipNum = pItemElem->GetChipCost() * nNum;
			// 			PT_ITEM_SPEC pChipItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26460, II_CHP_RED ) );
			// 			if( nChipNum < (int)( pChipItemProp->dwPackMax ) )
			// 			{
			// 				FLItemElem* pTempElem;
			// 				int bEmpty = FALSE;
			// 				for( DWORD i = 0; i < pUser->m_Inventory.GetMax(); i++ )
			// 				{
			// 					pTempElem = pUser->m_Inventory.GetAtId( i );
			// 					if( IsUsableItem(pTempElem) && pChipItemProp->dwID == pTempElem->m_dwItemId )
			// 					{
			// 						if( pTempElem->m_nItemNum <= nChipNum )
			// 							bEmpty = TRUE;
			// 						break;
			// 					}
			// 				}
			// 				if( !bEmpty )
			// 				{
			// 					// 인벤이 꽉찼다는 텍스트 출력
			// 					
			// 				}
			// 			}
		}

		// 구매 가격 만큼의 칩 삭제
		DWORD dwChipCost = pItemElem->GetChipCost() * nNum;
		pUser->RemoveItemA( ITEM_INDEX( 26460, II_CHP_RED ), dwChipCost );

		// 구매 아이템 생성
		FLItemElem itemElem;
		itemElem	= *pItemElem;
		itemElem.m_nItemNum = nNum;
		itemElem.SetSerialNumber();
		if( pUser->CreateItem( &itemElem ) )
		{
			// 로그 남김
			LogItemInfo aLogItem;
			//aLogItem.Action = "B";
			//aLogItem.SendName = pUser->GetName();
			//CString strTemp;
			//strTemp.Format( "%s_C", pVendor->GetName() );
			//aLogItem.RecvName = (LPCTSTR)strTemp;
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "B" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			CString strTemp;
			strTemp.Format( "%s_C", pVendor->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strTemp );

			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) ) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) );
			aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * nNum) ) );
			OnLogItem( aLogItem, &itemElem, nNum );
		}
		else
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "B";
			//aLogItem.SendName = pUser->GetName();
			//CString strTemp;
			//strTemp.Format( "%s_CF", pVendor->GetName() );
			//aLogItem.RecvName = (LPCTSTR)strTemp;
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "B" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			CString strTemp;
			strTemp.Format( "%s_CF", pVendor->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strTemp );


			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) ) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) );
			aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * nNum) ) );
			OnLogItem( aLogItem, &itemElem, nNum );
		}
	}
}

//NPC에게 파는 경우
void CDPSrvr::OnSellItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwItemObjID	= NULL_ID;
	int nSellQuantity	= 0;

	ar >> dwItemObjID >> nSellQuantity;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwItemObjID );
	if( pItemElem == NULL )
	{
		return;
	}

	T_SELL_ITEM_TO_NPC_ACK toClient;
	toClient.dwItemObjID	= dwItemObjID;
	toClient.bSuccess		= false;

	const DWORD dwResult = pUser->CanSellItemToNPC( dwItemObjID, nSellQuantity );

	if( dwResult == FSC_SELLITEM_SELL_SUCCESS )
	{
		int nSellCost = 0;
		pUser->GetUnitCostSellItemToNPC( nSellCost, dwItemObjID );

		if( nSellCost <= 0 )
		{
			nSellCost = 1;
		}

		nSellCost *= nSellQuantity;

		const BOOL bTax	= CTax::GetInstance()->IsApplySalesTaxRate( pUser, pItemElem );
		const int nTax	= bTax == TRUE ? static_cast<int>( nSellCost * CTax::GetInstance()->GetSalesTaxRate( pUser ) ) : 0;

		nSellCost -= nTax;

		if( nSellCost <= 0 )
		{
			nSellCost = 1;
		}

		if( pUser->CheckUserGold( nSellCost, true ) == true )
		{
			toClient.bSuccess	= true;

			if( nTax > 0 )
			{
				CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_SALES );
			}

			LogItemInfo aLogItem;
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "S" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->m_vtInfo.GetOther()->GetName() );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetGold();
			aLogItem.Gold2 = pUser->GetGold() + nSellCost;
			OnLogItem( aLogItem, pItemElem, nSellQuantity );

			pUser->RemoveItem( dwItemObjID, nSellQuantity );
			pUser->AddGold( nSellCost );
		}
		else
		{
			pUser->AddDefinedText( TID_GAME_VENDOR_MAX_ALL_GOLD );
		}
	}
	else
	{
		switch ( dwResult )
		{
		case FSC_SELLITEM_DO_NOT_SELL_ITEM:
			pUser->AddDefinedText( TID_MMI_NOTSALETOVENDORTEXT02 );
			break;

		case FSC_SELLITEM_USING_ITEM:
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			break;

		case FSC_SELLITEM_EQUIP_ITEM:
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE );
			break;

		case FSC_SELLITEM_GOLD_OVERFLOW:
			pUser->AddDefinedText( TID_GAME_VENDOR_MAX_ALL_GOLD );
			break;
		}
	}

	pUser->Add_SellItemToNPC_Ack( toClient );


// 	DWORD dwItemObjID;
// 	int nNum;
// 
// 	ar >> dwItemObjID >> nNum;
// 	if( nNum < 1 )
// 		return;
// 
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) && IsValidObj( pUser->m_vtInfo.GetOther() ) )
// 	{
// 		if( pUser->IsDie() == TRUE )
// 		{
// 			return;
// 		}
// 
// 		if( pUser->m_vtInfo.GetOther()->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
// 			return;
// 
// 		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
// 			return;
// 
// 		FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwItemObjID );		
// 		if( IsUsableItem( pItemElem ) == FALSE )
// 			return;
// 
// 		PT_ITEM_SPEC pProp	= pItemElem->GetProp();
// 		if( pProp->dwItemKind3 == IK3_EVENTMAIN )
// 			return;
// 
// 		if( pItemElem->IsQuest() )
// 			return;
// 
// 		if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
// 		{
// 			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
// 			return;
// 		}
// 		if( pItemElem->m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
// 			return;
// 
// 		if( pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
// 			return;
// 
// 		if( nNum > pItemElem->m_nItemNum )
// 			nNum = pItemElem->m_nItemNum;
// 		if( nNum < 1 )
// 			nNum = 1;
// 
// 		int nGold = ( pItemElem->GetCost() / 4 );
// 		//nGold	= ( int )( nGold * FLGetGlobalSellFactor() );
// 
// 		if( FLGetGlobalSellFactor( nGold ) == FALSE )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR : ItemID(%u) ]" ), pItemElem->m_dwItemId );
// 			return;
// 		}
// 
// 
// 		if( nGold == 0 )
// 			nGold = 1;
// 		int nTax = 0;
// 		if( CTax::GetInstance()->IsApplyTaxRate( pUser, pItemElem ) )
// 			nTax = (int)( nGold * CTax::GetInstance()->GetSalesTaxRate( pUser ) );
// 		nGold -= nTax;
// 		nGold *= nNum;
// 		nTax  *= nNum;
// 		float fTmpGold = (float)( pUser->GetGold() + nGold );
// 
// 		if( fTmpGold >= 2100000000 )
// 		{
// 			return;
// 		}
// 
// 		if( pUser->CheckUserGold( nGold, true ) == false )
// 		{
// 			return;
// 		}
// 
// 		//		if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) )
// 		//			return;
// 		if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
// 			return;
// 
// 		if( pUser->IsUsing( pItemElem ) )
// 		{
// 			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
// 			return;
// 		}
// 
// 		LogItemInfo aLogItem;
// 		//aLogItem.Action = "S";
// 		//aLogItem.SendName = pUser->GetName();
// 		//aLogItem.RecvName = pUser->m_vtInfo.GetOther()->GetName();
// 		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "S" );
// 		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
// 		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->m_vtInfo.GetOther()->GetName() );
// 		aLogItem.WorldId = pUser->GetWorld()->GetID();
// 		aLogItem.Gold = pUser->GetGold();
// 		aLogItem.Gold2 = pUser->GetGold() + nGold;
// 		OnLogItem( aLogItem, pItemElem, nNum );
// 		int nCost	= (int)pItemElem->GetCost() / 4;
// 
// 		if( nCost < 1 )	
// 			nCost = 1;
// 
// 		if( nGold < 1 )
// 			nGold = 1;
// 
// 		pUser->AddGold( nGold );
// 		if( nTax )
// 			CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_SALES );
// 
// 		pUser->RemoveItem( dwItemObjID, nNum );
// 	}
}

// 패스워드 변경창을 띄울것인지 패스워드 확인창을 띄을것인지를 알려준다
void CDPSrvr::OnOpenBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD	dwId, dwItemId;
	ar >> dwId >> dwItemId;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nBank )
				return;
		}

		if( 0 == strcmp( pUser->m_szBankPass, "0000") )
		{
			// 변경창을 띄우라고 함
			pUser->AddBankWindow( 0, dwId, dwItemId );
		}
		else
		{
			// 확인창을 띄우라고 함
			pUser->AddBankWindow( 1, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnOpenGuildBankWnd(CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		if( pUser->m_bBank )				// 창고를 열고 있으면?
			return;
		if( pUser->m_bAllAction == FALSE )
			return;

		pUser->AddGuildBankWindow( 0 );
		pUser->m_bGuildBank = TRUE;
	}
}

void CDPSrvr::OnCloseBankWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		pUser->m_bBank = FALSE;
		pUser->m_bInstantBank	= FALSE;
	}
}

void CDPSrvr::OnDoUseSkillPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	SKILL aJobSkill[ MAX_SKILL_JOB ] = {0, };

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		ar >> aJobSkill[i].dwSkill >> aJobSkill[i].dwLevel;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		// 바꿀수 있을지?  확인
		int nChangePoint = 0;
		for( int i = 0; i < MAX_SKILL_JOB; ++i )
		{
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if( aJobSkill[i].dwSkill == NULL_ID || lpSkill->dwSkill == NULL_ID )
				continue;

			if( aJobSkill[i].dwSkill != lpSkill->dwSkill )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Skill index no match. src: %d, recv: %d" ), lpSkill->dwSkill, aJobSkill[i].dwSkill );
				return;
			}

			SkillProp* pSkillProp    = lpSkill->GetProp();
			if( pSkillProp == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Skill Prop not found?????: %d" ), aJobSkill[i].dwSkill );
				return;
			}

			if( aJobSkill[i].dwLevel == 0 || aJobSkill[i].dwLevel == lpSkill->dwLevel )
			{
				continue;
			}

			if( aJobSkill[i].dwLevel < lpSkill->dwLevel || aJobSkill[i].dwLevel > pSkillProp->dwExpertMax )
			{
				pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 레벨 체크
			if( pSkillProp->dwReqDisLV != NULL_ID )
			{
				if( pUser->IsMaster() == FALSE && pUser->IsHero() == FALSE )	// 전승하지 않은 1-120 캐릭터와 3차 전직 캐릭터
				{
					if( pUser->GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
					{
						return;
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// 선행 스킬 레벨 체크
			if( pSkillProp->dwReqSkill1 != NULL_ID )
			{
				LPSKILL pReqSkill1 = NULL;
				pReqSkill1 = pUser->GetSkill( pSkillProp->dwReqSkill1 );
				if( pReqSkill1 == NULL || (pReqSkill1->dwLevel < pSkillProp->dwReqSkillLevel1) )
				{
					// 배울 것인지 확인
					for( int j = 0; j < MAX_SKILL_JOB; ++j )
					{
						if( aJobSkill[j].dwSkill == pSkillProp->dwReqSkill1 )
						{
							pReqSkill1 = &aJobSkill[j];
							break;
						}
					}
				}

				if( pReqSkill1 == NULL )
				{
					int nIdx = pUser->GetSkillIdx( lpSkill->dwSkill );
					FLERROR_LOG( PROGRAM_NAME, _T( "%s NULL GetSkill %d = dwReSkill1(%d, %d)" ), pUser->GetName(), nIdx, pSkillProp->dwReqSkill1, pSkillProp->dwReqSkill2 );
					return;
				}
				if( pReqSkill1->dwLevel < pSkillProp->dwReqSkillLevel1 )
				{
					return;
				}
			}
			if( pSkillProp->dwReqSkill2 != NULL_ID )
			{
				LPSKILL pReSkill2 = NULL;
				pReSkill2 = pUser->GetSkill( pSkillProp->dwReqSkill2 );
				if( pReSkill2 == NULL || (pReSkill2->dwLevel < pSkillProp->dwReqSkillLevel2) )
				{
					// 배울 것인지 확인
					for( int j = 0; j < MAX_SKILL_JOB; ++j )
					{
						if( aJobSkill[j].dwSkill == pSkillProp->dwReqSkill2 )
						{
							pReSkill2 = &aJobSkill[j];
							break;
						}
					}
				}

				if( pReSkill2 == NULL )
				{
					int nIdx = pUser->GetSkillIdx( lpSkill->dwSkill );
					FLERROR_LOG( PROGRAM_NAME, _T( "%s NULL GetSkill %d = dwReSkill2(%d, %d)" ), pUser->GetName(), nIdx, pSkillProp->dwReqSkill1, pSkillProp->dwReqSkill2 );
					return;
				}
				if( pReSkill2->dwLevel < pSkillProp->dwReqSkillLevel2 )
				{
					return;
				}
			}
			//////////////////////////////////////////////////////////////////////////

			if( aJobSkill[i].dwLevel > lpSkill->dwLevel )
			{
				int nPoint = (aJobSkill[i].dwLevel - lpSkill->dwLevel) * prj.GetSkillPoint( pSkillProp );
				nChangePoint += nPoint;
			}
		}

		// 변한게 없다.
		if( nChangePoint <= 0 )
			return;

		if( pUser->m_nSkillPoint < nChangePoint )
		{
			pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
			return;
		}

		// 스킬별 재분배 하기 // 스킬 레벨 셋팅
		pUser->m_nSkillPoint -= nChangePoint;
		for( int i = 0; i < MAX_SKILL_JOB; ++i )
		{
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if( aJobSkill[i].dwSkill == NULL_ID || lpSkill->dwSkill == NULL_ID )
				continue;

			if( aJobSkill[i].dwLevel > lpSkill->dwLevel )
			{
				//////////////////////////////////////////////////////////////////////////
				SkillProp* pSkillProp    = lpSkill->GetProp();
				if( pSkillProp != NULL )
				{
					int nPoint = (aJobSkill[i].dwLevel - lpSkill->dwLevel) * prj.GetSkillPoint( pSkillProp );
					g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, nPoint, (CMover*)pUser, &aJobSkill[i] );
				}
				//////////////////////////////////////////////////////////////////////////

				lpSkill->dwLevel = aJobSkill[i].dwLevel;
			}
		}

		g_xWSUserManager->AddCreateSfxObj( pUser, XI_INDEX( 108, XI_SYS_EXCHAN01 ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->AddDoUseSkillPoint( pUser->m_aJobSkill, pUser->m_nSkillPoint );
	}
}

void CDPSrvr::OnCloseGuildBankWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		pUser->m_bGuildBank = FALSE;
	}
}

void CDPSrvr::OnBankToBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nFlag, nPutSlot, nSlot;
	
	

	ar >> nFlag >> nPutSlot >> nSlot;

	if( nPutSlot >= MAX_CHARACTER_SLOT || nSlot >= MAX_CHARACTER_SLOT )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_idPlayerBank[nPutSlot] != 0 && pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() )
		{
			if( nFlag == 1 )	// Item
			{
				DWORD dwItemObjID;
				int nItemNum;
				ar >> dwItemObjID >> nItemNum;
				FLItemElem* pItemElem	= pUser->m_Bank[nPutSlot].GetAtId( dwItemObjID );
				if( pItemElem == NULL )
					return;

				if( nItemNum > pItemElem->m_nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
				if( nItemNum < 1 )
					nItemNum	= 1;

				if( pUser->m_Bank[nSlot].GetEmptyCountByInventoryType( INVEN_TYPE_NONE ) > 0 )
				{
					FLItemElem itemElem;
					itemElem	= *pItemElem;
					itemElem.m_nItemNum		= nItemNum;
					pUser->AddPutItemBank( nSlot, &itemElem );
					pUser->m_Bank[nSlot].Add( &itemElem );
					pUser->UpdateItemBank( nPutSlot, dwItemObjID, UI_NUM, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송

					LogItemInfo aLogItem;
					//aLogItem.Action = "A";
					//aLogItem.SendName = pUser->GetName();
					//aLogItem.RecvName = "BANK";
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "A" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BANK" );
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

					const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
					aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
					aLogItem.nSlot = nSlot;
					aLogItem.nSlot1 = nPutSlot;
					OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
				}
				else
				{
					// 꽉차서 넣을수 가 없음. 메세지 처리
					pUser->AddBankIsFull();
				}
			}
			else		// Gold
			{
				DWORD dwGold;
				ar >> dwGold;

				if( dwGold > pUser->m_dwGoldBank[nPutSlot] )
					dwGold = pUser->m_dwGoldBank[nPutSlot];

				// DWORD -> int
				int nGold			= dwGold;
				if( nGold < 0 )
				{
					return; 
				}

				if( CanAdd( pUser->m_dwGoldBank[nSlot], nGold ) )
				{
					pUser->m_dwGoldBank[nSlot]	+= nGold;
					pUser->m_dwGoldBank[nPutSlot] -= nGold;

					pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
					pUser->AddPutGoldBank( nPutSlot, pUser->GetGold(), pUser->m_dwGoldBank[nPutSlot] );

					LogItemInfo aLogItem;
					//aLogItem.Action = "A";
					//aLogItem.SendName = pUser->GetName();
					//aLogItem.RecvName = "BANK";
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "A" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BANK" );
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					//aLogItem.ItemName = "SEED";
					FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
					aLogItem.kLogItem.nQuantity = nGold;

					const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
					aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
					aLogItem.nSlot = nSlot;
					aLogItem.nSlot1 = nPutSlot;
					OnLogItem( aLogItem );
				}	
			}
		}
	}
}
void CDPSrvr::OnPutItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nSlot;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> nSlot >> dwItemObjID >> nItemNum;
	if( nSlot >= MAX_CHARACTER_SLOT )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
				return;
		}

		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
		if( byProgramDataSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwItemObjID );
			if( IsUsableItem( pItemElem ) == FALSE )
				return;

			if( nSlot != byProgramDataSlot )
			{
				if( pItemElem->IsQuest() )
					return;
			}

			if( pItemElem->IsOwnState() )
				return;

			PT_ITEM_SPEC pProp	= pItemElem->GetProp();
			if( !pProp )
				return;

			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}

			if( pProp->dwItemKind3 == IK3_EVENTMAIN ||
				pProp->dwItemKind3 == IK3_LINK ||
				( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 ) )
				return;

			//			if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) )
			//				return;
			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;


			if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
			{
				pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
				return;
			}

			if( nItemNum > pItemElem->m_nItemNum )
				nItemNum	= pItemElem->m_nItemNum;
			if( nItemNum < 1 )
				nItemNum	= 1;

			//			if( MAX_BANK > pUser->m_Bank[nSlot].GetCount() )
			if( pUser->m_Bank[nSlot].IsFull( pItemElem, nItemNum ) == TRUE )
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();

				return;
			}
			else
			{
				FLItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;

				// 일부만 가져 오는 것이라면 
// 				if( itemElem.m_nItemNum < pItemElem->m_nItemNum )
// 				{
// 					itemElem.SetSerialNumber();
// 				}

				LogItemInfo aLogItem;
				//aLogItem.Action = "P";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = "BANK";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "P" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BANK" );
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

				const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
				aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem, pItemElem, nItemNum );

				pUser->RemoveItem( dwItemObjID, nItemNum );
				pUser->m_Bank[nSlot].Add( &itemElem );

				pUser->AddPutItemBank( nSlot, &itemElem );
			}
		}
	}
}

void CDPSrvr::OnPutItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	DWORD dwItemObjID;
	int nItemNum;
	BYTE mode;

	ar >> dwItemObjID >> nItemNum >> mode;


	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDBANKING, pUser->GetWorld(), pUser->GetPos() ) )
				return;

		if( mode == 0 ) // 길드창고에는 Gold를 넣을수 없습니다.
			return;

		FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwItemObjID );		
		if( IsUsableItem( pItemElem ) == FALSE )
			return;

		if( pItemElem->IsQuest() )
			return;

		if( pItemElem->IsOwnState() )
			return;

		if( pUser->IsUsing( pItemElem ) )
		{
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			return;
		}

		PT_ITEM_SPEC pItemProp	= pItemElem->GetProp();
		if( pItemProp == NULL )
		{
			return;
		}
		if( pItemProp->dwParts == PARTS_RIDE && pItemProp->dwItemJob == JOB_VAGRANT )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
			return;
		}

		if( pItemElem->IsCharged() )
			return;

		if( nItemNum > pItemElem->m_nItemNum )
			nItemNum = pItemElem->m_nItemNum;
		if( nItemNum < 1 )
			nItemNum = 1;

		//	GUILD_BANK_STR 'S1','000000','01' 
		//	GUILD BANK 전체 불러오기 ex ) GUILD_BANK_STR 'S1',@im_idGuild,@iserverindex GUILD_BANK_STR 'S1','000000','01'  
		//	GUILD BANK 저장하기 ex ) GUILD_BANK_STR 'U1',@im_idGuild,@iserverindex,@im_nGoldGuild,@im_apIndex,@im_dwObjIndex,@im_GuildBank GUILD_BANK_STR 'U1','000001','01',0,'$','$','$' 			

		CGuild*	pGuild = pUser->GetGuild();
		if( pGuild )
		{
			if( pGuild->m_GuildBank.GetAllocedSerialNumber() == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "길드 은행 아이템 시리얼넘버가 초기화 되어있지 않습니다" ) );
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 조건 검사 및 데이터 설정
			if( pGuild->m_GuildBank.IsFull( pItemElem, nItemNum ) )
			{
				pUser->AddDefinedText( TID_GAME_GUILDBANKFULL, "" );		// 길드창고가 꽉찼시유~
				return;
			}

			FLItemElem itemElem;
			itemElem	= *pItemElem;
			itemElem.m_nItemNum	= nItemNum;

			// 일부만 가져 오는 것이라면 
// 			if( itemElem.m_nItemNum < pItemElem->m_nItemNum )
// 			{
// 				itemElem.SetSerialNumber();
// 			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// 교환
			pUser->RemoveItem( dwItemObjID, nItemNum );
			pGuild->m_GuildBank.Add( &itemElem );
			//////////////////////////////////////////////////////////////////////////


			//////////////////////////////////////////////////////////////////////////
			// 저장
			UpdateGuildBank( pGuild, GUILD_PUT_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, nItemNum );
			CWorld * pWorld = pUser->GetWorld();
			if( pWorld != NULL )
			{
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// 로그
			LogItemInfo aLogItem;
			//aLogItem.Action = "W";
			//aLogItem.SendName = pUser->GetName();
			//aLogItem.RecvName = "GUILDBANK";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "W" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GUILDBANK" );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, &itemElem, nItemNum );
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// 알림
			g_xWSUserManager->AddPutItemElem( pUser, &itemElem );
			pUser->AddPutItemGuildBank( &itemElem );
			//////////////////////////////////////////////////////////////////////////
		}
	}
}

// void CDPSrvr::OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
// {
// 	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
// 		return;
// 
// 	DWORD dwItemObjID;
// 	int nItemNum;
// 	BYTE mode;
// 
// 	ar >> dwItemObjID >> nItemNum >> mode;
// 
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	
// 	if( IsValidObj( pUser ) )
// 	{
// 		if( pUser->IsDie() == TRUE )
// 		{
// 			return;
// 		}
// 
// 		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
// 			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDBANKING, pUser->GetWorld(), pUser->GetPos() ) )
// 				return;
// 
// 		if (mode == 0) // Gold를 길드창고에서 빼낼때
// 		{
// 			// DOWRD -> int
// 			int nGetGold = nItemNum;
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			//	BEGIN100708
// 			if( nGetGold < 0 )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "mode 0 Error m_idPlayer [%d], UserGold:[%d], AddGold:[%d]" ),
// 					pUser->m_idPlayer, pUser->GetGold(), nGetGold );
// 
// 				return;
// 			}
// 			//	END100708
// 			//////////////////////////////////////////////////////////////////////////
// 
// 			if( pUser->CheckUserGold( nGetGold, true ) == false )
// 				return;
// 
// 			CGuild*	pGuild = pUser->GetGuild();
// 			if (pGuild && pGuild->IsGetPenya(pUser->m_idPlayer))
// 			{
// 				if( (DWORD)nGetGold > pGuild->m_nGoldGuild )
// 				{
// 					return;
// 				}
// 
// 				pUser->AddGold( nGetGold, FALSE );
// 				pGuild->m_nGoldGuild -= nGetGold;
// 				pUser->AddGetGoldGuildBank( nGetGold, 0, pUser->m_idPlayer, 0 );	// 0은 업데이트 시킨 클라에게 
// 				pGuild->DecrementMemberContribution( pUser->m_idPlayer, nGetGold, 0 );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				UpdateGuildBank( pGuild, GUILD_GET_PENYA, 1, pUser->m_idPlayer, NULL, nGetGold );
// 				//////////////////////////////////////////////////////////////////////////
// 				CWorld * pWorld = pUser->GetWorld();
// 				if( pWorld != NULL )
// 				{
// #ifdef __LAYER_1015
// 					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
// #else	// __LAYER_1015
// 					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
// #endif	// __LAYER_1015
// 				}
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				LogItemInfo aLogItem;
// 				//aLogItem.Action = "Y";
// 				//aLogItem.SendName = "GUILDBANK";
// 				//aLogItem.RecvName = pUser->GetName();
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "Y" );
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GUILDBANK" );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
// 
// 				aLogItem.WorldId = pUser->GetWorld()->GetID();
// 				aLogItem.Gold = pUser->GetGold() - nGetGold;
// 				aLogItem.Gold2 = pUser->GetGold();
// 				//aLogItem.ItemName = "SEED";
// 				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
// 				aLogItem.kLogItem.nQuantity = nGetGold;
// 				OnLogItem( aLogItem );
// 
// 				CGuildMember*	pMember;
// 				FLWSUser*			pUsertmp;
// 				std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
// 				for( ; i != pGuild->m_mapPMember.end(); ++i )
// 				{
// 					pMember		= i->second;
// 					pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
// 					if( IsValidObj( pUsertmp ) && pUsertmp != pUser )
// 					{
// 						pUsertmp->AddGetGoldGuildBank( nGetGold, 2, pUser->m_idPlayer, 0 );	// 2는 업데이트 해야할 클라이게
// 					}
// 				}
// 
// 				g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, nGetGold, 2, 0 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
// 			}
// 			//	Core 서버에 전 서버에 업데이트 되야함을 알린다.
// 		}
// 		else if (mode == 1) // 아이템을 길드창고에서 빼낼때
// 		{
// 			CGuild*			pGuild = pUser->GetGuild();
// 			if (pGuild && pGuild->IsGetItem(pUser->m_idPlayer))
// 			{
// 				if( pGuild->m_GuildBank.GetAllocedSerialNumber() == FALSE )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "길드 은행 아이템 시리얼넘버가 초기화 되어있지 않습니다" ) );
// 					return;
// 				}
// 
// 				FLItemElem* pItemElem	= pGuild->m_GuildBank.GetAtId( dwItemObjID );
// 
// 				if( NULL == pItemElem )
// 					return;
// 
// 				PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
// 				if( !pItemProp )
// 					return;
// 
// 				if( nItemNum > pItemElem->m_nItemNum )
// 				{
// 					nItemNum = pItemElem->m_nItemNum;
// 				}
// 				if( nItemNum < 1 )
// 				{
// 					nItemNum	= 1;
// 				}
// 
// 				if( pUser->m_Inventory.IsFull( pItemElem, nItemNum ) )
// 				{
// 					// 꽉차서 넣을수 가 없음. 메세지 처리
// 					pUser->AddBankIsFull();
// 					return;
// 				}
// 
// 				FLItemElem itemElem;
// 				itemElem	= *pItemElem;
// 				itemElem.m_nItemNum		= nItemNum;
// 				itemElem.m_dwObjId		= pItemElem->m_dwObjId;
// 
// 				// 일부만 가져 오는 것이라면 
// 				if( itemElem.m_nItemNum < pItemElem->m_nItemNum )
// 				{
// 					itemElem.SetSerialNumber();
// 				}
// 
// 				if (pItemElem->m_nItemNum > nItemNum )
// 					pItemElem->m_nItemNum	= pItemElem->m_nItemNum - nItemNum;
// 				else 
// 					pGuild->m_GuildBank.RemoveAtId( dwItemObjID );
// 
// 				pUser->m_Inventory.Add( &itemElem );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// 저장
// 				UpdateGuildBank(pGuild, GUILD_GET_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, nItemNum );
// 				//////////////////////////////////////////////////////////////////////////
// 				CWorld * pWorld = pUser->GetWorld();
// 				if( pWorld != NULL )
// 				{
// #ifdef __LAYER_1015
// 					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
// #else	// __LAYER_1015
// 					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
// #endif	// __LAYER_1015
// 				}
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				LogItemInfo aLogItem;
// 				//aLogItem.Action = "Y";
// 				//aLogItem.SendName = "GUILDBANK";
// 				//aLogItem.RecvName = pUser->GetName();
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "Y" );
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GUILDBANK" );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
// 
// 
// 				aLogItem.WorldId = pUser->GetWorld()->GetID();
// 				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
// 				OnLogItem( aLogItem, &itemElem, nItemNum );
// 
// 				// 클라이언트에게 아이템이 인벤토리에 추가됨을 알린다.
// 				pUser->AddGetItemGuildBank( &itemElem );
// 				// 자신을 제외한 모든 클라이언트에게 알려준다.
// 				g_xWSUserManager->AddGetItemElem( pUser, &itemElem );
// 			}
// 		}
// 	}
// }

void CDPSrvr::OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
	{
		return;
	}

	DWORD dwItemObjID	= NULL_ID;
	int nItemNum		= 0;
	BYTE byMode			= 0;

	ar >> dwItemObjID >> nItemNum >> byMode;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CGuild*	pGuild = pUser->GetGuild();
	if( pGuild == NULL )
	{
		return;
	}

	if( pGuild->IsMember( pUser->m_idPlayer ) == FALSE )
	{
		return;
	}

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( GuildHouseMng->IsGuildHouse( pWorld->GetID() ) == FALSE && CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDBANKING, pWorld, pUser->GetPos() ) == FALSE )
	{
		return;
	}

	if( byMode < 0 || byMode > 1 )	// mode == 0 : 골드를 뺀다. mode == 1 : 아이템을 뺀다.
	{
		return;
	}

	if( byMode == 0 )			// 골드를 뺀다.
	{
		if( pGuild->IsGetPenya( pUser->m_idPlayer ) == FALSE )
		{
			// 권한이 없어 골드를 꺼내지 못함.
			return;
		}

		const int nGetGold	= nItemNum;

		if( nGetGold <= 0 || nGetGold >= INT_MAX )
		{
			return;
		}

		if( static_cast<DWORD>( nGetGold ) > pGuild->m_nGoldGuild )
		{
			return;
		}

		if( pUser->CheckUserGold( nGetGold, true ) == false )
		{
			return;
		}

		pGuild->m_nGoldGuild -= nGetGold;
		pGuild->DecrementMemberContribution( pUser->m_idPlayer, nGetGold, 0 );

		// 길드뱅크 업데이트 Ack 후 유저에게 골드 지급
		SendGuildBankOutputGoldReq( pGuild, pUser->m_idPlayer, nGetGold, true );
	}
	else if( byMode == 1 )		// 아이템을 뺀다.
	{
		if( pGuild->IsGetItem(pUser->m_idPlayer) == FALSE )
		{
			// 권한이 없어 아이테을 꺼내지 못함.
			return;
		}

		FLItemElem* const pItemElem		= pGuild->m_GuildBank.GetAtId( dwItemObjID );
		if( NULL == pItemElem )
		{
			return;
		}

		const T_ITEM_SPEC* pItemSpec	= pItemElem->GetProp();
		if( pItemSpec == NULL )
		{
			return;
		}

		const int nGetItemQuantity		= nItemNum;
		if( nGetItemQuantity <= 0 || static_cast<DWORD>( nGetItemQuantity ) > pItemSpec->dwPackMax )
		{
			return;
		}

		if( nGetItemQuantity > pItemElem->m_nItemNum )
		{
			return;
		}

		if( pUser->m_Inventory.IsFull( pItemElem, nGetItemQuantity ) == TRUE )
		{
			// 꽉차서 넣을수 가 없음. 메세지 처리
			pUser->AddBankIsFull();
			return;
		}

		FLItemElem kOutputItemElem;
		kOutputItemElem					= *pItemElem;
		kOutputItemElem.m_nItemNum		= nGetItemQuantity;

		// 일부만 가져 오는 것이라면 
		if( kOutputItemElem.m_nItemNum < pItemElem->m_nItemNum )
		{
//			kOutputItemElem.SetSerialNumber();

			pItemElem->m_nItemNum	= pItemElem->m_nItemNum - nGetItemQuantity;
		}
		else
		{
			// remove
			pGuild->m_GuildBank.RemoveAtId( dwItemObjID );
		}

		// 길드뱅크 업데이트 Ack 후 유저에게 아이템 지급
		SendGuildBankOutputItemReq( pGuild, pUser->m_idPlayer, kOutputItemElem );
	}
	else
	{
		// error
		FLWARNING_LOG( PROGRAM_NAME, _T( "[ INVALID GUILD BANK OUTPUT MODE. byMode(%d), PLAYER_ID(%07d) ]" ), byMode, pUser->m_idPlayer );
		return;
	}
}

void CDPSrvr::OnGuildBankMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		BYTE nSrcIndex, nDestIndex;

		ar >> nSrcIndex >> nDestIndex;

		// 길드 창고의 아이템을 스왑한다.
		CGuild* pGuild = pUser->GetGuild();
		if ( pGuild )
		{
			if( pGuild->m_GuildBank.GetAllocedSerialNumber() == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "길드 은행 아이템 시리얼넘버가 초기화 되어있지 않습니다" ) );
				return;
			}

			// 모든 클라이언트에게 길드창고에서 아이템이 이동했음을 알려준다.
			// 길드창고를 업데이트한다.
			if( pGuild->m_GuildBank.Swap( nSrcIndex, nDestIndex ) == true )
			{
				UpdateGuildBank(pGuild, 4); // 4번은 아이템이 스왑된것임
			}
		}
	}
}

void CDPSrvr::UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate, u_long idPlayer, FLItemElem* pItemElem, DWORD dwPenya, int nItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_BANK_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );

	if (p_GuildBank)
	{
		ar << p_GuildBank->m_idGuild;
		ar << p_GuildBank->m_nGoldGuild;
		p_GuildBank->m_GuildBank.Serialize(ar);
		ar << cbUpdate;	// 멤버의 공헌페냐를 업뎃해야하는가? 
		ar << idPlayer;
		ar << p_Mode;
		if( pItemElem == NULL )
		{
			ar << (DWORD)0;
			ar << (int)0;
			ar << (SERIALNUMBER)0;
		}
		else
		{
			ar << pItemElem->m_dwItemId;
			ar << pItemElem->GetAbilityOption();
			ar << pItemElem->GetSerialNumber();
		}		
		ar << nItemCount;
		ar << dwPenya;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}


void CDPSrvr::OnGetItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nSlot;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> nSlot >> dwItemObjID >> nItemNum;
	if( nSlot >= MAX_CHARACTER_SLOT )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
				return;
		}

		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
		if( byProgramDataSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			FLItemElem* pItemElem	= pUser->m_Bank[nSlot].GetAtId( dwItemObjID );
			if( NULL == pItemElem )
				return;

			PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
			if( !pItemProp )
				return;

			if( nItemNum > pItemElem->m_nItemNum )
			{
				nItemNum = pItemElem->m_nItemNum;
			}

			if( nItemNum < 1 )
			{
				nItemNum	= 1;
			}

			if( pUser->m_Inventory.IsFull( pItemElem, nItemNum ) )
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();
				return;
			}

			FLItemElem itemElem;
			itemElem	= *pItemElem;
			itemElem.m_nItemNum		= nItemNum;

			// 일부만 가져 오는 것이라면 
// 			if( itemElem.m_nItemNum < pItemElem->m_nItemNum )
// 			{
// 				itemElem.SetSerialNumber();
// 			}

			LogItemInfo aLogItem;
			//aLogItem.Action = "G";
			//aLogItem.SendName = "BANK";
			//aLogItem.RecvName = pUser->GetName();
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "G" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "BANK" );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

			const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
			aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
			aLogItem.nSlot = nSlot;
			OnLogItem( aLogItem, pItemElem, nItemNum );

			pUser->UpdateItemBank( nSlot, dwItemObjID, UI_NUM, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송
			pUser->m_Inventory.Add( &itemElem );		// 인벤에 넣기

			pUser->AddGetItemBank( &itemElem );			// 유저에게 전송
		}		
	}
}

void CDPSrvr::OnPutGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;

	int nGold = dwGold;
	if( nGold <= 0 )
		return;

	if( nSlot >= MAX_CHARACTER_SLOT )
		return;

	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
				return;
		}

		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
		if( byProgramDataSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			if( pUser->CheckUserGold( nGold, false ) == false )
			{
				return;
			}

			// 은행돈이 overflow되지 않게 한다.
			if( CanAdd( pUser->m_dwGoldBank[nSlot], nGold ) == TRUE )
			{
				LogItemInfo aLogItem;
				//aLogItem.Action = "P";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = "BANK";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "P" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BANK" );


				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				//aLogItem.ItemName = "SEED";
				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
				aLogItem.kLogItem.nQuantity = nGold;

				const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
				aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem );

				pUser->m_dwGoldBank[nSlot] += nGold;
				pUser->AddGold( -nGold, FALSE );

				pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CanAddGold Fail. User:[%07d], BankGold:[%d], AddGold:[%d]" )
					, pUser->m_idPlayer, pUser->m_dwGoldBank[nSlot], nGold );
			}
		}		
	}
}

void CDPSrvr::OnGetGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser			= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	BYTE nSlot;
	DWORD dwMoveGold;
	ar >> nSlot >> dwMoveGold;

	// DWORD -> int
	int nMoveGold			= dwMoveGold;
	if( nMoveGold <= 0 )
		return;

	// slot 체크
	if( nSlot >= MAX_CHARACTER_SLOT )
		return;

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( !pUser->m_bInstantBank )
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
			return;
	}

	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
	if( byProgramDataSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
	{	
		// DWORD -> int
		int nBankGold	= pUser->m_dwGoldBank[nSlot];
		if( nBankGold < 0 )
			return;

		// 이동시킬 GOLD 보정.
#undef min
		nMoveGold		= std::min( nBankGold, nMoveGold );

		// 체크
		if( pUser->CheckUserGold( nMoveGold, true ) == false )
		{
			return;
		}

		LogItemInfo aLogItem;
		//aLogItem.Action = "G";
		//aLogItem.SendName = "BANK";
		//aLogItem.RecvName = pUser->GetName();
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "G" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "BANK" );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() + nMoveGold;
		//aLogItem.ItemName = "SEED";
		FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
		aLogItem.kLogItem.nQuantity = nMoveGold;

		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
		aLogItem.Gold_1 = pUser->m_dwGoldBank[byProgramDataSlot];
		aLogItem.nSlot = nSlot;
		OnLogItem( aLogItem );

		pUser->AddGold( nMoveGold, FALSE );						// CheckUserGold에서 검증됨
		pUser->m_dwGoldBank[nSlot] -= nMoveGold;				// if( safety_substitution( pUser->m_dwGoldBank[nSlot], nBankGold ) == false )에서 검증됨
		pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );		
	}		
}

void CDPSrvr::OnMoveBankItem( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
}

void CDPSrvr::OnChangeBankPass( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char szLastPass[10] ={0,};
	char szNewPass[10] ={0,};
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		ar.ReadString( szLastPass, _countof( szLastPass ) );
		ar.ReadString( szNewPass, _countof( szNewPass ) );

		if( strlen( szLastPass ) > 4 || strlen( szNewPass ) > 4 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), szLastPass, szNewPass );
			return;
		}

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;

		// 여기서 비밀번호 확인작업
		if( 0 == strcmp( szLastPass, pUser->m_szBankPass ) )
		{
			// 패스워드가 바꿨으므로 DB와 클라이언트에 게 바뀠다고 보내줌
			FLStrcpy( pUser->m_szBankPass, _countof( pUser->m_szBankPass ), szNewPass );
			g_dpDBClient.SendChangeBankPass( pUser->GetName(), szNewPass, pUser->m_idPlayer );
			pUser->AddChangeBankPass( 1, dwId, dwItemId );
		}
		else
		{
			// 다시 입력하라고 알려줌
			// 패스워드가 틀렸음
			pUser->AddChangeBankPass( 0, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnConfirmBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char szPass[10] ={0,};
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )
			return;
		if( pUser->m_bGuildBank )
			return;
		if( pUser->m_bAllAction == FALSE )
			return;

		// 여기서 비밀번호 확인작업
		ar.ReadString( szPass, _countof( szPass ) );

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;

		if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		if( 0 == strcmp( szPass, pUser->m_szBankPass ) )
		{
			// 비밀번호를 확인 하였으므로 은행을 열수 잇게 해줌
			if( dwId != NULL_ID )
			{
				FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
				if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != dwItemId )
				{
					return;
				}
				else
				{
					pUser->m_bInstantBank	= TRUE;
					pUser->RemoveItem( pItemElem->m_dwObjId, 1 );
				}
			}
			pUser->m_bBank = TRUE;
			pUser->AddconfirmBankPass( 1, dwId, dwItemId );
		}
		else
		{
			// 다시 입력하라고 알려줌
			// 패스워드가 틀렸음
			pUser->AddconfirmBankPass( 0, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int idSfxHit;
	int nMagicPower;
	DWORD dwSkill;
	OBJID idAttacker;
	int	nDmgCnt;	// 일반적으론 0, 지속데미지의 경우 첫빵이후는 1이상이 넘어온다. 이경우는 데미지의 10%만 준다.
	float fDmgAngle, fDmgPower;
	PSfxHit pSfxHit		= NULL;
	CMover* pAttacker	= NULL;

	ar >> idSfxHit >> nMagicPower >> dwSkill >> idAttacker >> nDmgCnt >> fDmgAngle >> fDmgPower;		

	// idAttacker가 NULL_ID면 어태커를 dpidUser로 한다.
	if( idAttacker == NULL_ID )
		pAttacker = g_xWSUserManager->GetUser( dpidCache, dpidUser );	
	else
		pAttacker = prj.GetMover( idAttacker );

	if( IsValidObj( pAttacker ) == FALSE ) 
		return;

	if( pAttacker->IsDie() == TRUE )
	{
		return;
	}

	pSfxHit	= pAttacker->m_sfxHitArray.GetSfxHit( idSfxHit );
	if( pSfxHit == NULL ) 
		return;

	CMover* pTarget	= prj.GetMover( pSfxHit->objid );

	/*
	// 康	// 06-10-23
	if( dwSkill == SI_MAG_FIRE_HOTAIR )	
	{
	if( IsValidObj( pTarget ) && pTarget->IsLive() )
	{
	SFXHIT_INFO si	=
	{ pTarget->GetId(), nMagicPower, dwSkill, nDmgCnt, fDmgAngle, fDmgPower, pSfxHit->dwAtkFlags	};
	AttackBySFX( pAttacker, si );
	}
	return;
	}
	*/

	pAttacker->RemoveSFX( pSfxHit->objid, idSfxHit, ( IsInvalidObj( pTarget ) || pTarget->IsDie() ), dwSkill );
	pAttacker->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );	// 무조건 제거
}

// 클라로부터 받은 idSfx를 어레이에 추가시켜둠
void CDPSrvr::OnSfxID( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		OBJID	idTarget;
		int		idSfxHit;
		DWORD	dwType,	dwSkill;
		int		nMaxDmgCnt;

		ar >> idTarget >> idSfxHit >> dwType >> dwSkill >> nMaxDmgCnt;
		pUser->m_sfxHitArray.Add( idSfxHit, idTarget, dwType, dwSkill, nMaxDmgCnt );		
	}
}

// 공격이 빗나가서 저절로 없어졌을때 삭제 명령.
void CDPSrvr::OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID	idMover;
	int	  idSfxHit;

	ar >> idSfxHit;
	ar >> idMover;

	CMover *pMover;

	if( idMover == NULL_ID )
		pMover = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	else
		pMover = prj.GetMover( idMover );

	if( IsValidObj( pMover ) )
	{
		PSfxHit pSfxHit	= pMover->m_sfxHitArray.GetSfxHit( idSfxHit );
		UNREFERENCED_PARAMETER( pSfxHit );
		pMover->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );
	}
}


void CDPSrvr::OnMeleeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJMSG dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;

	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;


	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTargetObj	= prj.GetMover( objid );
	if( IsValidObj( pTargetObj ) == FALSE )
	{
		return;
	}

	if( pUser->IsDisguise() == TRUE )
	{
		return;
	}

	if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE )
	{
		return;
	}

	pUser->DoAttackMelee( pTargetObj, dwAtkMsg, 0 );
/*
	int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );

	if( nRet == 0 && pUser->IsFly() == FALSE )
		pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( new ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );

	if( nRet != -2 )	// -2는 명령 완전 무시.
	{
		g_xWSUserManager->AddMeleeAttack( pUser, dwAtkMsg, objid, nParam2, nParam3 );
//		pUser->m_tHackCheckData.dwLastAttackTick = GetTickCount();
	}
*/

// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) )
// 	{
// 		if( pUser->GetIndex() == 0 )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_MELEE_ATTACK" ) );
// 			return;
// 		}
// 		PT_ITEM_SPEC pHandItemProp	= pUser->GetActiveHandItemProp();
// 		FLOAT f	= pHandItemProp? pHandItemProp->fAttackSpeed: 0.1F;
// 		if( f != fVal )
// 		{
// 			pUser->AddDefinedText( TID_GAME_MODIFY_ATTACK_SPEED );
// 			return;
// 		}
// 
// 		CMover* pTargetObj	= prj.GetMover( objid );
// 		if( IsValidObj( pTargetObj ) )
// 		{
// 			if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE )
// 			{
// 				return;
// 			}
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			//	공격 거리 검사
// 			//////////////////////////////////////////////////////////////////////////
// 			FLOAT fDistance	= 0.0F;
// 			D3DXVECTOR3 vDist = pUser->GetPos() - pTargetObj->GetPos();
// 			fDistance	=  sqrtf(vDist.x * vDist.x + vDist.z * vDist.z);
// 
// 			FLOAT fUserModelSize	= 2.0F;
// 			if( pUser->GetModel() != NULL )
// 			{
// 				fUserModelSize = pUser->GetModel()->GetMaxWidth() / 2.0F;
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pUser->GetModel() == NULL" ) );
// 			}
// 			
// 			FLOAT fTargetModelSize	= 2.0F;
// 			if( pTargetObj->GetModel() != NULL )
// 			{
// 				fTargetModelSize	= pTargetObj->GetModel()->GetMaxWidth();
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pTargetObject->GetModel() == NULL" ) );
// 			}
// 
// 			FLOAT	fAttackDistance = 0.0F;
// 			fAttackDistance	= fDistance - fUserModelSize - fTargetModelSize;
// 
// 			FLOAT	fAttackRange	= 0.0F;
// 			if( pHandItemProp != NULL )
// 			{
// 				fAttackRange = pUser->GetAttackRange( pHandItemProp->dwAttackRange );
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// 				fAttackRange = pUser->GetAttackRange( AR_SHORT );
// 			}
// 
// 			if( fAttackDistance > fAttackRange )
// 			{
// 				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackRange Hack pUserName [%s], AttackDistance[%f], AttackRange[%f]" ), pUser->GetName(), fAttackDistance, fAttackRange );
// 				return;
// 			}
// 
// 			DWORD	dwCurrentTick	= 0;
// 			DWORD	dwAttackTick	= 0;
// 			DWORD	dwAttackSpeed	= 1000;
// 			float	fAttackSpeed	= 0.0f;
// 
// 			dwCurrentTick	= GetTickCount();
// 
// 			if( dwCurrentTick > pUser->m_tHackCheckData.dwLastAttackTick )
// 			{
// 				dwAttackTick	= dwCurrentTick - pUser->m_tHackCheckData.dwLastAttackTick;
// 
// 				fAttackSpeed = pUser->GetAttackSpeed();
// 				if(fAttackSpeed <= 0.0f )
// 				{
// 					fAttackSpeed = 1.0f;
// 				}
// 
// 				if( pHandItemProp != NULL )
// 				{
// 					float	fMaxFrame = (float)( _GetAniFrameNAT(pHandItemProp->dwSubCategory) );
// 					dwAttackSpeed	= (DWORD)( ( fMaxFrame * 2.0f / 60.0f ) * (1.0f/fAttackSpeed) * 1000.0f * 0.9f);
// 				}
// 				else
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// 				}
// 			}
// // 			else
// // 			{
// // 				FLERROR_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// // 			}
// 
// 			if( dwAttackSpeed > dwAttackTick )
// 			{
// 				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackSpeed Hack pUserName [%s], dwAttackSpeed[%d], dwAttackTick[%d]" ), pUser->GetName(), dwAttackSpeed, dwAttackTick );
// 				return;
// 			}
// 			else
// 			{
// //				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackSpeed [%d]" ), dwAttackSpeed );
// 			}
// 
// //			FLTRACE_LOG( PROGRAM_NAME, _T( "AttackSpeed Hack pUserName [%s], dwAttackSpeed[%d], dwAttackTick[%d]" ), pUser->GetName(), dwAttackSpeed, dwAttackTick );
// 
// 			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
// 
// 			if( nRet == 0 && pUser->IsFly() == FALSE )
// 				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( new ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
// 
// 			if( nRet != -2 )	// -2는 명령 완전 무시.
// 			{
// 				g_xWSUserManager->AddMeleeAttack( pUser, dwAtkMsg, objid, nParam2, nParam3 );
// 				pUser->m_tHackCheckData.dwLastAttackTick = GetTickCount();
// 			}
// 		}
// 	}
}

void CDPSrvr::OnMeleeAttack2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_MELEE_ATTACK2" ) );
			return;
		}

		CMover* pTargetObj	= prj.GetMover( objid );
		if( IsValidObj( pTargetObj ) )
		{			
			if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE || pUser->IsFly() == FALSE )
			{
				return;
			}

			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
			if( nRet == 0 )
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( new ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );

			if( nRet != -2 )	// -2는 명령 완전 무시.
				g_xWSUserManager->AddMeleeAttack2( pUser, dwAtkMsg, objid, nParam2, nParam3 );
		}
	}
}


void CDPSrvr::OnMagicAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3, nMagicPower, idSfxHit;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3 >> nMagicPower >> idSfxHit;

	nParam2 = 0;		//  m_qMagicAtkMsg에서 nParam2가 0이면 range attack으로 간주된다.

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pTargetObj	= prj.GetMover( objid );

		if( IsValidObj( pTargetObj ) ) 
		{
			if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE )
			{
				return;
			}

			int nRet = pUser->DoAttackMagic( pTargetObj, nMagicPower, idSfxHit );

			if( nRet == 0 && pUser->IsFly() == FALSE )
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( new MAGICATKMSG( dwAtkMsg, objid, nParam2, nParam3, nMagicPower, idSfxHit ) );
		}
	}
}

void CDPSrvr::OnRangeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	
	DWORD dwAtkMsg		= 0;
	OBJID objid			= 0;
	DWORD dwItemID		= 0;
	int idSfxHit		= 0;

	ar >> dwAtkMsg >> objid >> dwItemID >> idSfxHit;


	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTargetObj	= prj.GetMover( objid );
	if( IsValidObj( pTargetObj ) == FALSE ) 
	{
		return;
	}

	if( pUser->IsDisguise() == TRUE )
	{
		return;
	}

	if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE )
	{
		return;
	}

	const PT_ITEM_SPEC pHandItemProp	= pUser->GetActiveHandItemProp();
	if( pHandItemProp == NULL )
	{
		return;
	}

	if( pUser->DoAttackRange( pTargetObj, dwItemID, idSfxHit ) == 0 )
	{
		pUser->m_pActMover->m_qMagicAtkMsg.AddTail( new MAGICATKMSG( dwAtkMsg, objid, 1, dwItemID, 0, idSfxHit ) );
//		pUser->m_tHackCheckData.dwLastAttackTick = GetTickCount();
	}

	
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) )
// 	{
// 		if( pUser->GetIndex() == 0 )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_RANGE_ATTACK" ) );
// 			return;
// 		}
// 		PT_ITEM_SPEC pHandItemProp	= pUser->GetActiveHandItemProp();
// 		if( pHandItemProp == NULL )
// 		{
// 			pUser->AddDefinedText( TID_GAME_MODIFY_ATTACK_SPEED );
// 			return;
// 		}
// 
// 
// 		CMover* pTargetObj	= prj.GetMover( objid );
// 
// 		if( IsValidObj( pTargetObj ) ) 
// 		{
// 			if( pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE )
// 			{
// 				return;
// 			}
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			//	공격 거리 검사
// 			//////////////////////////////////////////////////////////////////////////
// 			FLOAT fDistance	= 0.0F;
// 			D3DXVECTOR3 vDist = pUser->GetPos() - pTargetObj->GetPos();
// 			fDistance	=  sqrtf(vDist.x * vDist.x + vDist.z * vDist.z);
// 
// 			FLOAT fUserModelSize	= 2.0F;
// 			if( pUser->GetModel() != NULL )
// 			{
// 				fUserModelSize = pUser->GetModel()->GetMaxWidth() / 2.0F;
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pUser->GetModel() == NULL" ) );
// 			}
// 
// 			FLOAT fTargetModelSize	= 2.0F;
// 			if( pTargetObj->GetModel() != NULL )
// 			{
// 				fTargetModelSize	= pTargetObj->GetModel()->GetMaxWidth();
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pTargetObject->GetModel() == NULL" ) );
// 			}
// 
// 			FLOAT	fAttackDistance = 0.0F;
// 			fAttackDistance	= fDistance - fUserModelSize - fTargetModelSize;
// 
// 			FLOAT	fAttackRange	= 0.0F;
// 			if( pHandItemProp != NULL )
// 			{
// 				fAttackRange = pUser->GetAttackRange( pHandItemProp->dwAttackRange );
// 			}
// 			else
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// 				fAttackRange = pUser->GetAttackRange( AR_SHORT );
// 			}
// 
// 			if( fAttackDistance > fAttackRange )
// 			{
// 				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackRange Hack pUserName [%s], AttackDistance[%f], AttackRange[%f]" ), pUser->GetName(), fAttackDistance, fAttackRange );
// 				return;
// 			}
// 
// 			DWORD	dwCurrentTick	= 0;
// 			DWORD	dwAttackTick	= 0;
// 			DWORD	dwAttackSpeed	= 1000;
// 			float	fAttackSpeed	= 0.0f;
// 
// 			dwCurrentTick	= GetTickCount();
// 
// 			if( dwCurrentTick > pUser->m_tHackCheckData.dwLastAttackTick )
// 			{
// 				dwAttackTick	= dwCurrentTick - pUser->m_tHackCheckData.dwLastAttackTick;
// 
// 				fAttackSpeed = pUser->GetAttackSpeed();
// 				if(fAttackSpeed <= 0.0f )
// 				{
// 					fAttackSpeed = 1.0f;
// 				}
// 
// 				if( pHandItemProp != NULL )
// 				{
// 					float	fMaxFrame = (float)( _GetAniFrameNAT(pHandItemProp->dwSubCategory) );
// 					dwAttackSpeed	= (DWORD)( ( fMaxFrame * 2.0f / 60.0f ) * (1.0f/fAttackSpeed) * 1000.0f * 0.9f);
// 				}
// 				else
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// 				}
// 			}
// // 			else
// // 			{
// // 				FLTRACE_LOG( PROGRAM_NAME, _T( "pHandItemProp == NULL, pUserName [%s]" ), pUser->GetName() );
// // 			}
// 
// 			if( dwAttackSpeed > dwAttackTick )
// 			{
// 				FLTRACE_LOG( PROGRAM_NAME, _T( "RangeAttackSpeed Hack pUserName [%s], dwAttackSpeed[%d], dwAttackTick[%d]" ), pUser->GetName(), dwAttackSpeed, dwAttackTick );
// 				return;
// 			}
// 
// //			FLTRACE_LOG( PROGRAM_NAME, _T( "AttackSpeed Hack pUserName [%s], dwAttackSpeed[%d], dwAttackTick[%d]" ), pUser->GetName(), dwAttackSpeed, dwAttackTick );
// 
// 
// 			if( pUser->DoAttackRange( pTargetObj, dwItemID, idSfxHit ) == 0 )
// 			{
// 				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( new MAGICATKMSG( dwAtkMsg, objid, 1, dwItemID, 0, idSfxHit ) );
// 				pUser->m_tHackCheckData.dwLastAttackTick = GetTickCount();
// 			}
// 		}
// 	}
}

void CDPSrvr::OnTeleSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	D3DXVECTOR3 vPos;
	ar >> vPos;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		LPSKILL pSkill	= pUser->GetSkill( SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ) );
		if( pSkill == NULL || pSkill->dwLevel == 0 )
		{
			pSkill	= pUser->GetSkill( SKILL_INDEX( 412, SI_WIN_YOYO_BACKSTEP ) );
			if( pSkill == NULL || pSkill->dwLevel == 0 )
			{
				return;
			}
		}

		CWorld* pWorld	= pUser->GetWorld();
		if( pWorld == NULL )
		{
			return;
		}

		// vPos 좌표를 검증해야 할 것 같다.
		// 근대 어떻게 검증해야 할지 모르겠다.

		// 현재 위치 - 클락워크 지역 x, 대상 위치 - 클락워크 지역 o = 불가
		D3DXVECTOR3 v	= pUser->GetPos();
		if( prj.IsGuildQuestRegion( pWorld->GetID(), v ) == TRUE || prj.IsGuildQuestRegion( pWorld->GetID(), vPos ) == TRUE )		// 현 좌표, 혹은 대상 좌표가 클락워크 지역이다.
		{
			// 무슨 의미인지 모르겠다.
			if( fabs( pWorld->GetLandHeight( v ) - v.y ) > 1.0F || fabs( pWorld->GetLandHeight( vPos ) - vPos.y ) > 1.0F )
			{
				return;
			}
		}
		pUser->REPLACE( g_uIdofMulti, pUser->GetWorld()->GetID(), vPos, REPLACE_NORMAL, pUser->GetLayer() );
	}
}


void CDPSrvr::OnSetTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID idTarget;
		BYTE bClear;

		ar >> idTarget >> bClear;	// idTarget은 MOVER라고 가정하자.

		if( bClear == 2 )		// 2 : 타겟잡은놈을 기억.
			pUser->m_idSetTarget = idTarget;
		if( bClear < 2 )		// 0 / 1 : 타겟잡은놈에게 나를 기록 / 타겟잡은놈에게서 나를 지움.
		{
			CMover *pTarget = prj.GetMover( idTarget );		// 타겟의 포인터
			if( IsValidObj( pTarget ) )
			{
				if( pUser->IsDie() == TRUE || pTarget->IsDie() == TRUE )
				{
					return;
				}

				if( bClear )	// 타겟이 해제됬다.
				{
					if( pTarget->m_idTargeter == pUser->GetId() )	// 자기가 잡았던 타겟만 자기가 풀수있다.
					{
						pTarget->m_idTargeter = NULL_ID;
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = pUser->m_pActMover->GetCastingSKillID();
						if( pUser->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ) || nTmpSkillID == SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) ) )
						{
							pUser->m_pActMover->SetCastingEndTick(0);
							pUser->m_pActMover->ClearState();				// 상태 클리어하고 다시 맞춤.
						}
					}
				}
				else
				{
					if( pTarget->m_idTargeter == NULL_ID )			// 타겟잡은 사람이 없을때만 타게터를 박을 수 있다. 0819
						pTarget->m_idTargeter = pUser->GetId();		// pUser가 타겟을 잡았다.
				}
			}
		}
	}
}


void CDPSrvr::OnSnapshot( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	
	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BYTE c;
		ar >> c;
		while( c-- )
		{
			int	nPacketType;
			ar >> nPacketType;
			switch( nPacketType )
			{
			case SNAPSHOTTYPE_DESTPOS:	OnPlayerDestPos( ar, pUser );	break;
			default:
				{
					// handler not found
					FLASSERT( 0 );
					break;
				}
			}
		}
	}
}


void CDPSrvr::OnPlayerDestPos( CAr & ar, FLWSUser* pUser )
{
	D3DXVECTOR3 vPos;
	BYTE fForward;
	ar >> vPos >> fForward;

	if( IsValidObj( pUser ) == FALSE )
		return;
	if( pUser->IsDie() == TRUE )
		return;

#ifdef __IAOBJ0622
	CShip* pIAObj	= NULL;
	OBJID objidIAObj;
	ar >> objidIAObj;

	if( objidIAObj != NULL_ID )
	{
		pIAObj	= (CShip*)prj.GetCtrl( objidIAObj );
		if( IsValidObj( pIAObj ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CShip object not found" ) );
			return;
		}
	}
#endif	// __IAOBJ0622

	bool bForward = (fForward != 0);

#ifdef __IAOBJ0622
	if( pIAObj )
		pUser->SetDestPos( pIAObj, vPos );
	else
		pUser->SetDestPos( vPos, bForward );
#else	// __IAOBJ0622
	pUser->SetDestPos( vPos, bForward );
#endif	// __IAOBJ0622

#ifdef __IAOBJ0622
	g_xWSUserManager->AddSetDestPos( pUser, vPos, bForward, objidIAObj );
#else	// __IAOBJ0622
	g_xWSUserManager->AddSetDestPos( pUser, vPos, bForward );
#endif	// __IAOBJ0622
}

void CDPSrvr::OnModifyMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		DWORD dwMode;
		BYTE f;
		u_long idFrom;

		ar >> dwMode >> f >> idFrom;

		DPID dpid;
		ar >> dpid;
		if( pUser->m_Snapshot.dpidUser != dpid )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[%s] try to hack : PACKETTYPE_MODIFYMODE" ), pUser->GetName() );
			return;
		}

		if( f )
			pUser->m_dwMode		|= dwMode;
		else
			pUser->m_dwMode		&= ~dwMode;

		g_xWSUserManager->AddModifyMode( pUser );
	}
}

// 운영자의 소환 명령어 
// void CDPSrvr::OnSummonPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
// {
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) )
// 	{
// 		if( pUser->IsDie() == TRUE )
// 		{
// 			return;
// 		}
// 
// 		u_long idOperator;
// 		DWORD dwWorldID;
// 		D3DXVECTOR3 vPos;
// 		u_long uIdofMulti;
// 
// 		ar >> idOperator;
// 		ar >> dwWorldID;
// 		ar >> vPos;
// 		ar >> uIdofMulti;
// 
// 		DPID dpid;
// 		ar >> dpid;
// 		if( pUser->m_Snapshot.dpidUser != dpid )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[%s] try to hack : PACKETTYPE_SUMMONPLAYER" ), pUser->GetName() );
// 			return;
// 		}
// #ifdef __LAYER_1015
// 		int nLayer;
// 		ar >> nLayer;
// #endif	// __LAYER_1015
// 
// 		if( !pUser->GetWorld() )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_SUMMONPLAYER//1" ) );
// 			return;
// 		}
// 
// 		pUser->REPLACE( uIdofMulti, dwWorldID, vPos, REPLACE_FORCE, nLayer );
// 	}
// }

// void CDPSrvr::OnTeleportPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
// {
// 	u_long idOperator;
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) == FALSE )
// 	{
// 		return;
// 	}
// 
// 	CWorld* pWorld = pUser->GetWorld();
// 	if( pWorld != NULL )
// 	{
// 		if( pUser->IsDie() == TRUE )
// 		{
// 			return;
// 		}
// 
// 		ar >> idOperator;
// 		DPID dpid;
// 		ar >> dpid;
// 		if( pUser->m_Snapshot.dpidUser != dpid )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[%s] try to hack : PACKETTYPE_TELEPORTPLAYER" ), pUser->GetName() );
// 			return;
// 		}
// #ifdef __LAYER_1015
// 		g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pWorld->GetID(), pUser->GetPos(), idOperator, pUser->GetLayer() );
// #else	// __LAYER_1015
// 		g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pWorld->GetID(), pUser->GetPos(), idOperator );
// #endif	// __LAYER_1015
// 	}
// }

void CDPSrvr::OnChangeFace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long objId;
	DWORD dwFaceNum;
	int cost;

	BOOL bUseCoupon;
	ar >> objId >> dwFaceNum >> cost >> bUseCoupon;

	cost = CHANGE_FACE_COST;
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{	
		if(dwFaceNum == pUser->m_dwHeadMesh ) return;	// 헤어 바꾸기 오류 수정

		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if(!bUseCoupon)
		{
			//			if( pUser->GetGold() < cost )
			if( pUser->CheckUserGold( cost, false ) == false )
			{
				pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
				return;
			}
			pUser->AddGold( -( cost ) );
		}
		else
		{
			FLItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 26411, II_SYS_SYS_SCR_FACEOFFFREE ) );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_WARNNING_COUPON, "" );
				return;
			}
			pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
		}
		pUser->SetHead(dwFaceNum);

		g_xWSUserManager->AddChangeFace( objId, dwFaceNum );
	}
}

void CDPSrvr::OnExpUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( (DWORD)AUTH_GAMEMASTER <= pUser->m_dwAuthorization )
		{
			EXPINTEGER nExp;
			ar >> nExp;

			if( pUser->AddExperience( nExp, TRUE, TRUE, TRUE ) )
				pUser->LevelUpSetting();
			else
				pUser->ExpUpSetting();

			pUser->AddSetExperience( pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel );
		}
	}
}

void	CDPSrvr::OnChangeJob( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// 康: hacked
	/*
	int nJob;
	BOOL bGamma = TRUE;
	ar >> nJob;
	ar >> bGamma;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( TRUE == IsValidObj( ( CObj* )pUser ) )
	{
	if( bGamma )
	{			
	if( pUser->IsBaseJob() )	// 1차 전직
	{
	if( pUser->GetLevel() != MAX_JOB_LEVEL )
	{
	pUser->AddDefinedText( TID_GAME_CHGJOBLEVEL15 ); // "레벨 15가 되야 전직할수 있습니다"
	return;
	}			

	if( pUser->AddChangeJob( nJob ) )
	{
	( (FLWSUser*)pUser )->AddSetChangeJob( nJob );
	g_xWSUserManager->AddNearSetChangeJob( (CMover*)pUser, nJob, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
	g_dpDBClient.SendLogLevelUp( (FLWSUser*)pUser, 4 );
	g_dpDBClient.SendUpdatePlayerData( pUser );

	return;
	}

	}
	else
	if( pUser->IsExpert() )
	{
	if( pUser->GetLevel() < MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
	pUser->AddDefinedText( TID_LIMIT_CHANGEJOBLEVEL, "" );	// 60레벨이 되야 전직을 할수 있습니다
	return;
	}

	if( pUser->AddChangeJob( nJob ) )
	{
	( (FLWSUser*)pUser )->AddSetChangeJob( nJob );
	g_xWSUserManager->AddNearSetChangeJob( (CMover*)pUser, nJob, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
	g_dpDBClient.SendLogLevelUp( (FLWSUser*)pUser, 4 );
	g_dpDBClient.SendUpdatePlayerData( pUser );
	return;
	}
	}
	}
	else
	{
	FLItemElem* pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 10425, II_SYS_SYS_SCR_CHACLA ) );
	if( IsUsableItem( pItemElem ) == FALSE || pUser->IsBaseJob() || pUser->GetJob() == nJob ) 
	{	// 인벤토리에 아이템이 없거나 방랑자거나 같은 직업을 바구려면 리턴
	return;
	}

	if( pUser->IsExpert() )
	{
	if( JOB_VAGRANT == nJob || MAX_EXPERT <= nJob)	// Expert 계열이 아니면 리턴
	return;
	}
	else
	{
	if( nJob < MAX_EXPERT || MAX_PROFESSIONAL <= nJob )	// Pro 계열이 아니면 리턴
	return;
	}

	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
	{
	if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_RIDE )
	continue;
	FLItemElem* pArmor	= pUser->m_Inventory.GetEquip( dwParts );
	if( pArmor )
	return;
	}

	pUser->InitLevel( nJob, pUser->GetLevel(), FALSE );
	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp && pItemProp->dwSfxObj3 != -1 )
	g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, pItemElem->GetProp()->dwSfxObj3, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	pUser->AddDefinedText( TID_GAME_CHANGECLASS, "%s", prj.m_aJob[pUser->GetJob()].szName );

	// 상용화 아이템 사용 로그 삽입
	g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );
	pUser->RemoveItem( pItemElem->m_dwObjId, 1 );
	}
	}
	*/
}

void	CDPSrvr::OnLogItem( LogItemInfo & kLogItemInfo, FLItemElem* pItemElem, int nItemCount )
{
	if( pItemElem != NULL )
	{
		kLogItemInfo.kLogItem.CopyItemInfo( *pItemElem );
		kLogItemInfo.kLogItem.nQuantity	= nItemCount;
	}

	if( kLogItemInfo.SendName[0] != '\0' )
	{
		kLogItemInfo.idSendPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( kLogItemInfo.SendName );
	}

	if( kLogItemInfo.RecvName[0] != '\0' )
	{
		kLogItemInfo.idRecvPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( kLogItemInfo.RecvName );
	}

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	kLogItemInfo.Serialize( ar );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );


// 	if( pItemElem != NULL )
// 	{
// 		info.ItemNo = pItemElem->GetSerialNumber();
// 		//info.ItemName = pItemElem->GetProp()->szName;
// 		FLSPrintf( info.szItemName, _countof( info.szItemName ), "%d", pItemElem->GetProp()->dwID );
// 		info.itemNumber = nItemCount;
// 		info.nItemResist = pItemElem->m_byItemResist;
// 		info.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
// 		info.nAbilityOption = pItemElem->GetAbilityOption();
// 		info.Negudo = pItemElem->m_nHitPoint;
// 		info.MaxNegudo	= pItemElem->m_nRepair;
// 		info.m_bCharged = pItemElem->m_bCharged;
// 		info.m_dwKeepTime = pItemElem->m_dwKeepTime;
// 		info.nPiercedSize = pItemElem->GetGeneralPiercingSize();
// 		for( size_t i=0; i<pItemElem->GetGeneralPiercingSize(); i++ )
// 			info.adwItemId[i] = pItemElem->GetGeneralPiercingItemID( i );
// 		info.nUMPiercedSize = pItemElem->GetUltimatePiercingSize();
// 		for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
// 			info.adwUMItemId[i] = pItemElem->GetUltimatePiercingItemID( i );
// 
// 		if( pItemElem->m_pPet )
// 		{
// 			CPet* pPet = pItemElem->m_pPet;
// 
// 			info.nPetKind = pPet->GetKind();
// 			info.nPetLevel = pPet->GetLevel();
// 			info.dwPetExp = pPet->GetExp();
// 			info.wPetEnergy = pPet->GetEnergy();
// 			info.wPetLife = pPet->GetLife();
// 			info.nPetAL_D = pPet->GetAvailLevel( PL_D );
// 			info.nPetAL_C = pPet->GetAvailLevel( PL_C );
// 			info.nPetAL_B = pPet->GetAvailLevel( PL_B );
// 			info.nPetAL_A = pPet->GetAvailLevel( PL_A );
// 			info.nPetAL_S = pPet->GetAvailLevel( PL_S );
// 		}
// 		// mirchang_100514 TransformVisPet_Log
// 		else if( pItemElem->IsTransformVisPet() == TRUE )
// 		{
// 			info.nPetKind = (BYTE)100;
// 		}
// 		// mirchang_100514 TransformVisPet_Log
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		info.dwCouplePlayerId				= pItemElem->GetCoupleId();
// 		info.nLevelDown						= pItemElem->GetLevelDown();
// 		info.wRandomOptionOriginId			= pItemElem->GetRandomOptionOriginID();
// 		info.nRandomOptionExtensionSize		= pItemElem->GetRandomOptionExtensionSize();
// 		info.wRandomOptionExtensionFlag		= pItemElem->GetRandomOptionExtensionFlag();
// 
// 		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_SIZE; ++Nth )
// 		{
// 			info.awDstID[Nth]	= pItemElem->GetRandomOptionExtensionDstID( Nth );
// 			info.ashAdjValue[Nth]	= pItemElem->GetRandomOptionExtensionAdjValue( Nth );
// 		}
// 		//////////////////////////////////////////////////////////////////////////
// 	}
// 
// 	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );
// 	ar.WriteString( info.Action );
// 	ar.WriteString( info.SendName );
// 	ar.WriteString( info.RecvName );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 	if( info.SendName != NULL && _tcslen( info.SendName ) > 0 )
// 	{
// 		TCHAR szSendName[MAX_NAME] = { 0, };
// 		FLStrcpy( szSendName, _countof( szSendName ), info.SendName );
// 		info.idSendPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( szSendName );
// 	}
// 	if( info.RecvName != NULL && _tcslen( info.RecvName ) > 0 )
// 	{
// 		TCHAR szRecvName[MAX_NAME] = { 0, };
// 		FLStrcpy( szRecvName, _countof( szRecvName ), info.RecvName );
// 		info.idRecvPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( szRecvName );
// 	}
// 
// 	ar << info.idSendPlayer;
// 	ar << info.idRecvPlayer;
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	ar << info.WorldId;
// 	ar << info.Gold;
// 	ar << info.Gold2;
// 	ar << info.ItemNo; // 아이템 고유번호
// 	ar << info.Negudo; // 내구도 
// 	ar << info.MaxNegudo; // 내구도 
// 	if( _tcslen( info.szItemName ) == 0 )
// 	{
// 		FLSPrintf( info.szItemName, _countof( info.szItemName ), "%d", -1 );
// 	}
// 	ar.WriteString( info.szItemName );
// 	ar << info.itemNumber;
// 	ar << info.nAbilityOption;
// 	ar << info.Gold_1;
// 	ar << info.nSlot;
// 	ar << info.nSlot1;
// 	ar << info.nItemResist;
// 	ar << info.nResistAbilityOption;
// 	ar << info.m_bCharged;
// 	ar << info.m_dwKeepTime;
// 	ar << info.nPiercedSize;
// 	for( size_t i=0; i<info.nPiercedSize; i++ )
// 		ar << info.adwItemId[i];
// 	ar << info.nUMPiercedSize;
// 	for( size_t i=0; i<info.nUMPiercedSize; i++ )
// 		ar << info.adwUMItemId[i];
// 
// 	ar << info.nPetKind;
// 	ar << info.nPetLevel;
// 	ar << info.dwPetExp;
// 	ar << info.wPetEnergy;
// 	ar << info.wPetLife;
// 	ar << info.nPetAL_D;
// 	ar << info.nPetAL_C;
// 	ar << info.nPetAL_B;
// 	ar << info.nPetAL_A;
// 	ar << info.nPetAL_S;
// 
// 	ar << info.dwCouplePlayerId;
// 	ar << info.nLevelDown;
// 	ar << info.wRandomOptionOriginId;
// 	ar << info.nRandomOptionExtensionSize;
// 	ar << info.wRandomOptionExtensionFlag;
// 
// 	for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 	{
// 		ar << info.awDstID[nIndex];
// 		ar << info.ashAdjValue[nIndex];
// 	}
// 
// 	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
}


void CDPSrvr::OnMotion( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->GetIndex() == 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "PACKETTYPE_MOTION" ) );
			return;
		}

		DWORD dwMsg;
		ar >> dwMsg;

		if( pUser->SendActMsg( (OBJMSG)dwMsg ) == 1 )
		{
			{
				pUser->ClearDest();
			}
			g_xWSUserManager->AddMotion( pUser, dwMsg );
		}
		else
			pUser->AddMotionError();
	}
}

void CDPSrvr::OnRepairItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		BYTE c, nId;
		ar >> c;

		if( c == 0 || c > MAX_REPAIRINGITEM )
			return;

		FLItemElem* apItemRepair[MAX_REPAIRINGITEM];
		int		anRepair[MAX_REPAIRINGITEM] = { 0 };
		//mem_set( anRepair, 0, sizeof(anRepair) );

		FLItemElem* pItemElem;
		int nCost	= 0;
		for( int i = 0; i < c; i++ )
		{
			apItemRepair[i]		= NULL;
			ar >> nId;
			pItemElem	= pUser->m_Inventory.GetAtId( nId );
			if( pItemElem )
			{
				PT_ITEM_SPEC pItemProp	= pItemElem->GetProp();
				if( pItemProp && pItemProp->dwItemKind2 >= IK2_WEAPON_HAND && pItemProp->dwItemKind2 <= IK2_ARMORETC && 
					pItemElem->m_nHitPoint < (int)( pItemProp->dwEndurance ) && pItemElem->m_nRepair < pItemProp->nMaxRepair )
				{
					int nRepair	= 100 - ( ( pItemElem->m_nHitPoint * 100 ) / pItemProp->dwEndurance );
					if( nRepair == 0 )
						continue;
					nCost	+= nRepair * ( pItemProp->dwCost / 1000 + 1 );
					if( nCost < 0 )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "Overflow nCost - \tPenya: Cost[%d] nRepair[%d] ItemCost[%u]" ), nCost, nRepair, pItemProp->dwCost );
						return;
					}

					apItemRepair[i]		= pItemElem;
					anRepair[i]		= nRepair;
				}
			}
		}



		if( pUser->CheckUserGold( nCost, false ) == false )
		{
			return;
		}

		for( int i = 0; i < c; i++ )
		{
			if( apItemRepair[i] )
			{
				pUser->UpdateItem( (BYTE)apItemRepair[i]->m_dwObjId, UI_HP, apItemRepair[i]->GetProp()->dwEndurance );
				pUser->UpdateItem( (BYTE)apItemRepair[i]->m_dwObjId, UI_RN, apItemRepair[i]->m_nRepairNumber + anRepair[i] );
			}
		}

		pUser->AddDefinedText( TID_GAME_REPAIRITEM );
	}
}

void CDPSrvr::OnSetHair( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		BYTE nHair, nR, nG, nB;
		BOOL bUseCoupon;
		ar >> nHair >> nR >> nG >> nB >> bUseCoupon;

		int nCost;

		nCost	= CMover::GetHairCost( (CMover*)pUser, nR, nG, nB, nHair );

		//		if( pUser->GetGold() < nCost  && !bUseCoupon)
		if( pUser->CheckUserGold( nCost, false ) == false && bUseCoupon == FALSE )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}

		pUser->SetHair( nHair );
		float r, g, b;
		r	= (float)nR / 255.0f;
		g	= (float)nG / 255.0f;
		b	= (float)nB / 255.0f;

		if(!bUseCoupon)
			pUser->AddGold( -( nCost ) );
		else
		{
			FLItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 26410, II_SYS_SYS_SCR_HAIRCHANGE ) );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_WARNNING_COUPON, "" );
				return;
			}
			pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
		}

		pUser->SetHairColor( r, g, b );
		g_xWSUserManager->AddSetHair( pUser, nHair, nR, nG, nB );
	}
}

void CDPSrvr::OnBlock( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nGu;
	u_long uidPlayerTo, uidPlayerFrom;
	char szNameTo[MAX_NAME] = {0,};
	char szNameFrom[MAX_NAME] = {0,};
	ar >> nGu;
	ar.ReadString( szNameTo, _countof( szNameTo ) );
	ar.ReadString( szNameFrom, _countof( szNameFrom ) );

	uidPlayerTo	 = CPlayerDataCenter::GetInstance()->GetPlayerId( szNameTo );
	uidPlayerFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( szNameFrom );

	if( uidPlayerTo > 0 && uidPlayerFrom > 0 )
		g_DPCoreClient.SendBlock( nGu, uidPlayerTo, szNameTo, uidPlayerFrom );
}

DWORD WhatEleCard( DWORD dwItemType )
{	// 속성 제련 용 카드의 종류가 
	// 속성 당 하나로 통합됨
	switch( dwItemType )
	{
	case SAI79::FIRE:
		return ITEM_INDEX( 3206, II_GEN_MAT_ELE_FLAME );
	case SAI79::WATER:
		return ITEM_INDEX( 3211, II_GEN_MAT_ELE_RIVER );
	case SAI79::ELECTRICITY:
		return ITEM_INDEX( 3216, II_GEN_MAT_ELE_GENERATOR );
	case SAI79::EARTH:
		return ITEM_INDEX( 3221, II_GEN_MAT_ELE_DESERT );
	case SAI79::WIND:
		return ITEM_INDEX( 3226, II_GEN_MAT_ELE_CYCLON );
	default:
		return 0;
	}
}

void CDPSrvr::OnPiercingSize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwId1, dwId2, dwId3;

	ar >> dwId1;
	ar >> dwId2;
	ar >> dwId3;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CItemUpgrade::GetInstance()->OnPiercingSize( pUser, dwId1, dwId2, dwId3 );
	}
}

void CDPSrvr::OnItemTransy( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidTarget, objidTransy;
	ar >> objidTarget;
	ar >> objidTransy;

	DWORD dwChangeId;
	BOOL bCash;
	ar >> dwChangeId;
	ar >> bCash;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CItemUpgrade::GetInstance()->OnItemTransy( pUser, objidTarget, objidTransy, dwChangeId, bCash );
	}
}

void CDPSrvr::OnExpBoxInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long uIdPlayer;
	OBJID objid;

	ar >> uIdPlayer >> objid;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CCtrl* pCtrl;

		pCtrl	= prj.GetCtrl( objid );
		if( IsValidObj( pCtrl ) )
			pUser->AddExpBoxInfo( objid,  ((CCommonCtrl*)pCtrl)->m_CtrlElem.m_dwSet, ((CCommonCtrl*)pCtrl)->m_dwDelete - timeGetTime(), ((CCommonCtrl*)pCtrl)->m_idExpPlayer );
	}
}

void CDPSrvr::OnPiercing( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwId1, dwId2;

	ar >> dwId1;
	ar >> dwId2;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CItemUpgrade::GetInstance()->OnPiercing( pUser, dwId1, dwId2 );
	}
}

// 피어싱 옵션 제거(카드 제거)
void CDPSrvr::OnPiercingRemove( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	OBJID objId1, objId2;
	ar >> objId1;
	ar >> objId2;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	CItemUpgrade::GetInstance()->OnPiercingRemove( pUser, objId1, objId2 );
}

void CDPSrvr::OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwSfxId;
	u_long uIdPlayer;
	BOOL bFlag;
	ar >> dwSfxId >> uIdPlayer >> bFlag;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( uIdPlayer == NULL_ID )
		{
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, dwSfxId, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z, bFlag);
		}
		else
		{
			FLWSUser* pUsertmp	= (FLWSUser*)prj.GetUserByID( uIdPlayer );
			if( IsValidObj( pUsertmp ) )
				g_xWSUserManager->AddCreateSfxObj((CMover *)pUsertmp, dwSfxId, pUsertmp->GetPos().x, pUsertmp->GetPos().y, pUsertmp->GetPos().z, bFlag);
		}		
	}
}

// 1000 단위의 퍼센트를 넘긴다.
// dwID - 제련 아이템 아이디, n - 제련 단계 
int GetEnchantPercent( DWORD dwID, int n )
{
	static int nPersent1[10] = { 1000, 1000, 700, 500, 400, 300, 200, 100, 50, 20 };
	static int nPersent2[10] = { 1000, 1000, 900, 750, 550, 400, 250, 150, 80, 40 };

	float fFactor = 1.0f;
	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR && n >= 3 )	// 제련 4부터 10% 확률 감소 
		fFactor = 0.9f;

	if( dwID == ITEM_INDEX( 3233, II_GEN_MAT_DIE_TWELVE ) )
		return	( (int)( nPersent2[n] * fFactor ) );
	else
		return	( (int)( nPersent1[n] * fFactor ) );
}

// void CDPSrvr::OnRemoveItemLevelDown( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
// {
// 	DWORD dwId;
// 	ar >> dwId;
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) )
// 	{
// 		if( pUser->IsDie() == TRUE )
// 		{
// 			return;
// 		}
// 
// 		FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( dwId );
// 		if( !IsUsableItem( pItemElem ) )
// 			return;
// 		if( pItemElem->GetLevelDown() == 0 )
// 		{
// 			pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
// 			return;
// 		}
// 		PutItemLog( pUser, "v", "OnRemoveItemLevelDown", pItemElem );
// 		pItemElem->InitializeLevelDown();
// 		pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
// 	}
// }

void CDPSrvr::OnDoUseItemTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	// 1. 개선된 handler에 실행해보고 성공하면 리턴
	CAr kCloneAr( ar );
	if( FLItemAction::GetInstance().ItemApplyHandler( *pUser, kCloneAr ) == true )
		return;

	// 2. 기존 handler에 실행
	DWORD dwMaterial, dwTarget;
	ar >> dwMaterial >> dwTarget;
	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	FLItemElem* pMaterial	= (FLItemElem*)pUser->GetItemId( dwMaterial );
	FLItemElem* pTarget	= (FLItemElem*)pUser->GetItemId( dwTarget );
	if( !IsUsableItem( pMaterial ) || !IsUsableItem( pTarget ) )
		return;

	if( pTarget->GetProp() == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ PROP is NULL : itemId(%u) ]" ), pTarget->m_dwItemId );
		return;
	}

	if( pUser->m_Inventory.IsEquip( dwTarget ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		return;
	}
	BOOL	b	= FALSE;
	switch( pMaterial->m_dwItemId ) 
	{
	case ITEM_INDEX( 26522, II_SYS_SYS_QUE_PETRESURRECTION02_S ):
	case ITEM_INDEX( 26523, II_SYS_SYS_QUE_PETRESURRECTION02_A ):
	case ITEM_INDEX( 26524, II_SYS_SYS_QUE_PETRESURRECTION02_B ):
		{
			CPet* pPet	= pTarget->m_pPet;
			if( !pPet || !pTarget->IsFlag( FLItemElem::expired ) )
			{
				pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_01 );
			}
			else
			{
				BYTE nLevel		= pPet->GetLevel();
				if( ( nLevel == PL_B && pMaterial->m_dwItemId != ITEM_INDEX( 26524, II_SYS_SYS_QUE_PETRESURRECTION02_B ) )
					|| ( nLevel == PL_A && pMaterial->m_dwItemId != ITEM_INDEX( 26523, II_SYS_SYS_QUE_PETRESURRECTION02_A ) )
					|| ( nLevel == PL_S && pMaterial->m_dwItemId != ITEM_INDEX( 26522, II_SYS_SYS_QUE_PETRESURRECTION02_S ) )
					|| ( nLevel < PL_B )
					)
				{
					pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_02 );
				}
				else
				{
					PutItemLog( pUser, "r", "::PetResurrection", pTarget );

					pTarget->ResetFlag( FLItemElem::expired );
					pUser->UpdateItem( (BYTE)( pTarget->m_dwObjId ), UI_FLAG, MAKELONG( pTarget->m_dwObjIndex, pTarget->m_byFlag ) );
					pPet->SetLife( 0 );
					pPet->SetEnergy( pPet->GetMaxEnergy() / 2 );
					pUser->AddPetState( pTarget->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
					pUser->AddDefinedText( TID_GAME_PETRESURRECTION_SUCCESS );
					b	= TRUE;
				}
			}
			break;
		}
		// 		case ITEM_INDEX( 26462, II_SYS_SYS_SCR_AWAKECANCEL ):
		// 		case ITEM_INDEX( 26563, II_SYS_SYS_SCR_AWAKECANCEL02 ):
		// 			b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening,
		// 				TID_GAME_AWAKECANCEL_INFO, TID_GAME_AWAKECANCEL,
		// 				"r", "::AwakeCancel" );
		// 			break;
		// 		case ITEM_INDEX( 26461, II_SYS_SYS_SCR_AWAKE ):
		// 			b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening, 
		// 				0, TID_GAME_INVALID_TARGET_ITEM, TID_GAME_AWAKE_OR_BLESSEDNESS01,
		// 				"r", "::Awake" );
		// 			break;
		// 		case ITEM_INDEX( 26463, II_SYS_SYS_SCR_BLESSEDNESS ):
		// 		case ITEM_INDEX( 26564, II_SYS_SYS_SCR_BLESSEDNESS02 ):
		// 			b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eBlessing, 
		// 				0, TID_GAME_USE_BLESSEDNESS_INFO, TID_GAME_BLESSEDNESS_INVALID_ITEM,
		// 				"r", "::Blessedness" );
		// 			break;
		// 		case ITEM_INDEX( 26614, II_SYS_SYS_SCR_EATPETAWAKE ):	// 먹펫 각성
		// 			b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eEatPet,
		// 				TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
		// 				"r", "EATPETAWAKE" );
		// 			break;
		// 		case ITEM_INDEX( 20024, II_SYS_SYS_SCR_PETAWAKE ):	// 시스템 펫 각성
		// 			b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet, 
		// 				TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
		// 				"r", "PETAWAKE" );
		// 			break;
		// 		case ITEM_INDEX( 20025, II_SYS_SYS_SCR_PETAWAKECANCEL ):		// 시스템 펫 각성 취소
		// 			b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet,
		// 				TID_GAME_PETAWAKECANCEL_S00, TID_GAME_PETAWAKECANCEL_E00,
		// 				"r", "PETAWAKECANCEL" );
		// 			break;
		// 		case ITEM_INDEX( 26458, II_SYS_SYS_SCR_LEVELDOWN01 ):
		// 		case ITEM_INDEX( 26459, II_SYS_SYS_SCR_LEVELDOWN02 ):
		// 			b	= DoUseItemTarget_ItemLevelDown( pUser, pMaterial, pTarget );
		// 			break;
	default:
		break;
	}
	if( b )
	{
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_xWSUserManager->AddCreateSfxObj( pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterial );

		pUser->UpdateItem( (BYTE)( pMaterial->m_dwObjId ), UI_NUM, pMaterial->m_nItemNum - 1 );	
	}
}

void CDPSrvr::OnSmeltSafety( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	OBJID dwItemId, dwItemMaterialId, dwItemProtScrId, dwItemSmeltScrId;

	//	pItemSmeltScrId - 일반제련시의 제련두루마리(사용안할시엔 Client에서 NULL_ID를 입력)
	ar >> dwItemId >> dwItemMaterialId >> dwItemProtScrId >> dwItemSmeltScrId;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}
		// 康
		if( pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() )	// 거래중인 대상이 있으면?
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		FLItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );
		FLItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItemProtScrId );
		FLItemElem* pItemElem3	= NULL;
		if( dwItemSmeltScrId != NULL_ID )
		{
			pItemElem3	= pUser->m_Inventory.GetAtId( dwItemSmeltScrId );
			if( !IsUsableItem( pItemElem3 ) )
				return;
		}

		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE || IsUsableItem( pItemElem2 ) == FALSE )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 장착되어 있는 아이템은 제련 못함
		if( pUser->m_Inventory.IsEquip( dwItemId ) )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		BYTE nResult = CItemUpgrade::GetInstance()->OnSmeltSafety( pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3 );

		pUser->AddSmeltSafety( nResult );
	}
}

void CDPSrvr::OnEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwItemId, dwItemMaterialId;

	ar >> dwItemId;
	ar >> dwItemMaterialId;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
			return;

		// 康
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		FLItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );		

		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
		{
			return;
		}

		CItemUpgrade::GetInstance()->OnEnchant( pUser, pItemElem0, pItemElem1 );
	}	
}

void CDPSrvr::OnRemoveAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	int nPayPenya = 100000; //속성제련 제거시 필요한 페냐

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemId;
	ar >> objItemId;

	FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( objItemId );

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		pUser->AddRemoveAttribute( FALSE );
		return;
	}
	// 무기나 방어구만 가능
	if( !FLItemElem::IsEleRefineryAble(pItemElem->GetProp()) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
		return;
	}
	if( pItemElem->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTUPGRADE , "" );
		return;
	}
	// 장착되어 있는지 확인을 해야 함.
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}
	// 10만 페냐 이상을 소지해야만 속성 제거 가능.
	//	if( pUser->GetGold() < nPayPenya )
	if( pUser->CheckUserGold( nPayPenya, false ) == false )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_LACKMONEY , "" );
		return;
	}

	// 현재 무기에 속성이 적용되어 있어야 가능.
	if( (pItemElem->m_byItemResist != SAI79::NO_PROP) && (pItemElem->m_nResistAbilityOption > 0) )
	{
		pUser->AddGold( -nPayPenya );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem( (BYTE)pItemElem->m_dwObjId, UI_IR,  SAI79::NO_PROP );
		pUser->UpdateItem( (BYTE)pItemElem->m_dwObjId, UI_RAO,  0 );
		pUser->AddRemoveAttribute( TRUE );

		// 속성제련 제거 성공 로그
		LogItemInfo aLogItem;
		//aLogItem.Action = "O";
		//aLogItem.SendName = pUser->GetName();
		//aLogItem.RecvName = "REMOVE_ATTRIBUTE";
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "O" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "REMOVE_ATTRIBUTE" );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPayPenya;

		OnLogItem( aLogItem, pItemElem, 1 );
	}
	else
		pUser->AddRemoveAttribute( FALSE );

}

void CDPSrvr::OnChangeAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objTargetItem, objMaterialItem;
	int nAttribute;

	ar >> objTargetItem >> objMaterialItem;
	ar >> nAttribute;

	CItemUpgrade::GetInstance()->ChangeAttribute( pUser, objTargetItem, objMaterialItem, static_cast<SAI79::ePropType>(nAttribute) );
}

void CDPSrvr::OnRandomScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/ )
{
	DWORD dwId1, dwId2;

	ar >> dwId1;
	ar >> dwId2;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
		FLItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}			

		if( pItemElem1->GetProp()->dwItemKind3 != IK3_RANDOM_SCROLL )
		{
			return;					
		}

		if( !(pItemElem0->GetProp()->dwItemKind1 == IK1_WEAPON || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
		{
			pUser->AddDefinedText(  TID_GAME_RANDOMSCROLL_ERROR, "" );
			return;
		}

		int nSTR[4] = { 1,  9, 21, 37 };
		int nDEX[4] = { 2, 10, 22, 38 };
		int nINT[4] = { 3, 11, 23, 39 };
		int nSTA[4] = { 4, 12, 24, 40 };

		int nValue = 0;
		int nRandom = xRandom( 100 );
		if( nRandom < 64 )
			nValue = 1;
		else if( nRandom < 94 )
			nValue = 2;
		else if( nRandom < 99 )
			nValue = 3;
		else if( nRandom < 100 )
			nValue = 4;

		if( 0 < nValue )
		{
			int nKind = 0;
			int nToolKind = 0;
			if( pItemElem1->GetProp()->dwID == ITEM_INDEX( 30012, II_SYS_SYS_SCR_RANDOMSTR ) )
			{
				nKind = nSTR[nValue-1];
				nToolKind = DST_STR;
			}
			else if( pItemElem1->GetProp()->dwID == ITEM_INDEX( 30013, II_SYS_SYS_SCR_RANDOMDEX ) )
			{
				nKind = nDEX[nValue-1];
				nToolKind = DST_DEX;
			}
			else if( pItemElem1->GetProp()->dwID == ITEM_INDEX( 30015, II_SYS_SYS_SCR_RANDOMINT ) )
			{
				nKind = nINT[nValue-1];
				nToolKind = DST_INT;
			}
			else if( pItemElem1->GetProp()->dwID == ITEM_INDEX( 30014, II_SYS_SYS_SCR_RANDOMSTA ) )
			{
				nKind = nSTA[nValue-1];
				nToolKind = DST_STA;
			}

			pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_RANDOMOPTITEMID, nKind );

			LogItemInfo aLogItem;
			//aLogItem.SendName = pUser->GetName();
			//aLogItem.RecvName = "RANDOMSCROLL";

			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "RANDOMSCROLL" );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			//aLogItem.Action = "(";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "(" );
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			//aLogItem.Action = ")";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), ")" );
			OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

			pUser->RemoveItem( dwId2, 1 );		

			// 아이템 박기 성공~
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			

			DWORD dwStringNum = 0;
			switch( nToolKind )
			{
			case DST_STR:
				dwStringNum = TID_TOOLTIP_STR;
				break;
			case DST_DEX:
				dwStringNum = TID_TOOLTIP_DEX;
				break;
			case DST_STA:
				dwStringNum = TID_TOOLTIP_STA;
				break;
			default: //case DST_INT:
				dwStringNum = TID_TOOLTIP_INT;
				break;
			}
			CString strMessage;
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_SUCCESS ), pItemElem0->GetProp()->szName, prj.GetText( dwStringNum ), nValue );
			pUser->AddText( strMessage );
		}
	}
}

void CDPSrvr::OnUpgradeBase( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
}


void CDPSrvr::OnCommercialElem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwItemId0, dwItemId1;
	ar >> dwItemId0 >> dwItemId1;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		// 인벤토리에 있는지 검사
		FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId0 );
		FLItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemId1 );

		if( IsUsableItem( pItemElem0 ) == FALSE || 
			IsUsableItem( pItemElem1 ) == FALSE || 
			pItemElem0->GetProp()->nLog >=2 || 
			pItemElem1->GetProp()->nLog >=2 )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwItemId0 ) || pUser->m_Inventory.IsEquip( dwItemId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}

		// 방어구(슈트만), 무기류가 아니면 제련불가능
		if( !( ( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
			pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ) ||
			( ( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) 
			&& pItemElem0->GetProp()->dwItemKind3 == IK3_SUIT )
			) )		
		{
			return;			
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 이미적용한 아이템이면 불가능
		{
			return;	
		}

		BOOL bResistDelete = FALSE;

		if( pItemElem1->GetProp()->dwItemKind2 == IK2_SYSTEM )
		{
			// 속성 공격력 추가
			if( pItemElem1->m_dwItemId == ITEM_INDEX( 10277, II_CHR_SYS_SCR_FIREASTONE ) ||
				pItemElem1->m_dwItemId == ITEM_INDEX( 10278, II_CHR_SYS_SCR_WATEILSTONE ) ||
				pItemElem1->m_dwItemId == ITEM_INDEX( 10279, II_CHR_SYS_SCR_WINDYOSTONE ) ||
				pItemElem1->m_dwItemId == ITEM_INDEX( 10280, II_CHR_SYS_SCR_LIGHTINESTONE ) ||
				pItemElem1->m_dwItemId == ITEM_INDEX( 10281, II_CHR_SYS_SCR_EARTHYSTONE ) ) 

			{
				if( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR ||
					pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC )
				{
					return;	
				}
			}
			else // 속성 방어력 추가
				if(	pItemElem1->m_dwItemId == ITEM_INDEX( 10282, II_CHR_SYS_SCR_DEFIREASTONE ) ||
					pItemElem1->m_dwItemId == ITEM_INDEX( 10283, II_CHR_SYS_SCR_DEWATEILSTONE ) ||
					pItemElem1->m_dwItemId == ITEM_INDEX( 10284, II_CHR_SYS_SCR_DEWINDYOSTONE ) ||
					pItemElem1->m_dwItemId == ITEM_INDEX( 10285, II_CHR_SYS_SCR_DELIGHTINESTONE ) ||
					pItemElem1->m_dwItemId == ITEM_INDEX( 10286, II_CHR_SYS_SCR_DEEARTHYSTONE ) )
				{
					if( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
						pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT )
					{
						return;	
					}
				}
				else // 속성 제거
					if( pItemElem1->m_dwItemId == ITEM_INDEX( 10276, II_CHR_SYS_SCR_TINEINEDSTONE ) )
					{
						if( pItemElem0->m_byItemResist == SAI79::NO_PROP )
						{
							return;	
						}
						pItemElem0->m_byItemResist = SAI79::NO_PROP;
						pItemElem0->m_nResistAbilityOption = 0;
						bResistDelete = TRUE;
					}
					else
					{
						return;	
					}
		}
		else 
		{
			return;	
		}

		// 성공

		if( bResistDelete )
		{
			pUser->AddCommercialElem( pItemElem0->m_dwObjId, 9999 );
		}
		else
		{
			pItemElem0->m_nResistSMItemId = pItemElem1->m_dwItemId;
		}

		// 로그
		g_dpDBClient.SendLogSMItemUse( "5", pUser, pItemElem1, pItemElem1->GetProp() );
		LogItemInfo aLogItem;
		//aLogItem.Action = "5";
		//aLogItem.SendName = pUser->GetName();
		//aLogItem.RecvName = pUser->GetName();
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "5" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pUser->GetName() );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.kLogItem.dwSerialNumber = pItemElem0->GetSerialNumber();
		//aLogItem.ItemName = pItemElem0->GetProp()->szName;
		FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", pItemElem0->GetProp()->dwID );
		aLogItem.kLogItem.nQuantity = pItemElem0->m_nItemNum;
		aLogItem.kLogItem.nAbilityOption = pItemElem0->GetAbilityOption();
		aLogItem.kLogItem.nItemResist = pItemElem0->m_byItemResist;
		aLogItem.kLogItem.nResistAbilityOption = pItemElem0->m_nResistAbilityOption;
		aLogItem.kLogItem.nHitPoint = int((float)pItemElem0->m_nHitPoint * 100 / (float)pItemElem0->GetProp()->dwEndurance );
		OnLogItem( aLogItem );

		pUser->RemoveItem( dwItemId1, 1 );

		g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 107, XI_SYS_EXPAN01 ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddCommercialElem( pItemElem0->m_dwObjId, pItemElem0->m_nResistSMItemId );

	}
}

void CDPSrvr::OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CTime		tm			= CTime::GetCurrentTime();
	CTimeSpan	tm_update	= tm - CGuildRank::Instance()->m_UpdateTime;

	DWORD	ver;

	//	버젼 정보를 받는다.
	ar >> ver;

	// 갱신된지 하루가 지났는지 체크
	if ( tm_update.GetHours() >= 24 )
	{
		// TRANS 서버에게 다시 랭크 정보를 갱신할 것을 요청한다.
		g_dpDBClient.UpdateGuildRanking();
	}
	else
	{
		// 랭크 정보 버젼이 다를 경우엔 랭크 정보를 보내게 된다.
		if ( CGuildRank::Instance()->m_Version != ver )
		{
			// 랭킹 정보를 보낸다.
			FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
			if( IsValidObj( pUser ) )
			{
				if( pUser->IsDie() == TRUE )
				{
					return;
				}

				pUser->SendGuildRank();
			}
		}
	}
}

void CDPSrvr::OnBuyingInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BUYING_INFO2 bi2;
	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	const u_long uPlayerID	= pUser ? pUser->m_idPlayer : bi2.dwPlayerId;

	FLERROR_LOG( PROGRAM_NAME, _T( "[ HACK! PLAYER_ID(%07d), ITEM_ID(%d), QUANTITY(%d) ]" )
		, uPlayerID, bi2.dwItemId, bi2.dwItemNum );

	
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) == FALSE )
// 	{
// 		g_dpDBClient.SendBuyingInfo( &bi2, iSerialNumber );
// 		return;
// 	}
// 
// 	CWorld* pWorld = pUser->GetWorld();
// 	
// 	if( pWorld != NULL )
// 	{
// 		bi2.dwRetVal	= 0;
// 		FLItemElem itemElem;
// 		itemElem.m_dwItemId		= bi2.dwItemId;
// 		itemElem.m_nItemNum		= (int)bi2.dwItemNum;
// 		itemElem.m_bCharged		= TRUE;
// 		bi2.dwRetVal	= pUser->CreateItem( &itemElem );
// #ifdef __LAYER_1015
// 		g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
// #else	// __LAYER_1015
// 		g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
// #endif	// __LAYER_1015
// 	}
// 	g_dpDBClient.SendBuyingInfo( &bi2, iSerialNumber );
// 	//	FLINFO_LOG( LOG_BUYING_INFO, _T( "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d" ),
// 	//	bi2.dwServerIndex, bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum );
// 	FLTRACE_LOG( PROGRAM_NAME, _T( "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d" ),
// 		bi2.dwServerIndex, bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum );
}

void CDPSrvr::OnEnterChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long uidChattingRoom;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		//BOOL bChatting = FALSE;
		if( pUser->m_idChatting == 0 )
		{
			ar >> uidChattingRoom;

			CChatting* pChatting = g_ChattingMng.GetChttingRoom( uidChattingRoom );
			if( pChatting )
			{

				if( pChatting->AddChattingMember( pUser->m_idPlayer ) )
				{
					FLWSUser*			pUsertmp;
					pUser->m_idChatting = uidChattingRoom;

					for( int i = 0 ; i < pChatting->GetChattingMember() - 1 ; ++i )
					{
						pUsertmp	= (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
						if( IsValidObj( pUsertmp ) )
							pUsertmp->AddEnterChatting( pUser );
					}

					pUser->AddNewChatting( pChatting );
				}
				//else
				//{
				//	bChatting	= TRUE;
				//}
			}

		}
		//else
		//{
		//	bChatting	= TRUE;
		//}
	}
}

void CDPSrvr::OnChatting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	static	TCHAR	sChat[1024];
	ar.ReadString( sChat, _countof( sChat ) );

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nText	= pUser->GetMuteText();
		if(  nText )
		{
			pUser->AddDefinedText( nText );
			return;
		}

		if( !( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ))))
			ParsingEffect( sChat, _countof( sChat ), strlen(sChat) );

		RemoveCRLF( sChat, _countof( sChat ) );

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			int nFind = pChatting->FindChattingMember( pUser->m_idPlayer );
			if( nFind != -1 )
			{
				FLWSUser* pUsertmp;
				for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
				{
					pUsertmp	= (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
					if( IsValidObj( pUsertmp ) )
						pUsertmp->AddChatting( pUser->m_idPlayer, sChat );
				}
			}
			else
			{
				// 채팅멤버가 아님
				pUser->m_idChatting = 0;
				pUser->AddDeleteChatting();
			}
		}
		else
		{
			// 채팅방이 없음.
			pUser->m_idChatting = 0;
			pUser->AddDeleteChatting();
		}

	}
}


void CDPSrvr::OnOpenChattingRoom( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			pChatting->m_bState		= TRUE;
			FLWSUser* pUsertmp;
			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				pUsertmp	= (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUsertmp ) && pUser->m_idPlayer != pUsertmp->m_idPlayer )
					pUsertmp->AddChttingRoomState( pChatting->m_bState );
			}
		}
	}
}

void CDPSrvr::OnCloseChattingRoom( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			pChatting->m_bState = FALSE;
			FLWSUser*			pUsertmp;

			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				pUsertmp	= (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUsertmp ) && pUser->m_idPlayer != pUsertmp->m_idPlayer )
				{
					pUsertmp->AddChttingRoomState( pChatting->m_bState );
				}						
			}
		}

	}
}

void CDPSrvr::OnCommonPlace( CAr & /*ar*/, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	//		BYTE nType;
	//		ar >> nType;
}


void CDPSrvr::QueryDestroyPlayer( DPID dpidCache, DPID dpidSocket, DWORD dwSerial, u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERY_DESTROY_PLAYER, dpidSocket );
	ar << dwSerial;
	ar << idPlayer;
	SEND( ar, this, dpidCache );
}

void	CDPSrvr::SendPacket( DPID dwDPIDCache, DPID dwDPIDUser, const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;
	ar << dwDPIDUser;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, dwDPIDCache );
}

void CDPSrvr::OnSetNaviPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	NaviPoint nv;
	OBJID objidTarget;
	ar >> nv.Pos >> objidTarget;

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( objidTarget == NULL_ID )
		{
			CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
			if( pParty )
			{
				for( int i = 0 ; i < pParty->GetSizeofMember() ; ++i )
				{
					FLWSUser* pUsertmp = (FLWSUser *)prj.GetUserByID( pParty->GetPlayerId( i ) );
					if( IsValidObj( pUsertmp ) )
					{
						pUsertmp->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
					}
				}
			}
		}
		else
		{
			FLWSUser* pUsertmp	= prj.GetUser( objidTarget );
			if( IsValidObj( pUsertmp ) )
			{
				pUser->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
				pUsertmp->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
			}
		}
	}
}

void CDPSrvr::OnGameMasterWhisper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CHAR	sPlayerFrom[MAX_PLAYER], lpString[260], szChat[1024];
	ar.ReadString( sPlayerFrom, _countof( sPlayerFrom ) );
	ar.ReadString( lpString, _countof( lpString ) );

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{	
		FLSPrintf( szChat, _countof( szChat ), "%s -> %s", sPlayerFrom, lpString );
		g_dpDBClient.SendLogGamemaChat( pUser, szChat );
	}
}

// 현상금 걸기 패킷 
void CDPSrvr::OnNWWantedGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int		nGold;
	char	szMsg[WANTED_MSG_MAX + 1];

	ar >> nGold;
	ar.ReadString( szMsg, _countof( szMsg ) );

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( pUser->m_idMurderer == 0 )		// 나를 죽인자가 없었으면 현상금을 걸 수 없다.
		return;

	if( szMsg[0] == '\0' )
		return;

	if( strlen(szMsg) > WANTED_MSG_MAX )
		return;

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// 현상금은 최소 1000패냐에서 최대 2억 패냐까지 걸 수 있다. 
		return;

	int nTax = MulDiv( nGold, 10, 100 );					// 건 현상금의 10%는 수수료로 지급된다. 

	//	if( pUser->GetGold() >= (nGold + nTax) ) 
	if( pUser->CheckUserGold( ( nGold + nTax ), false ) == true )
	{
		pUser->AddGold( -(nGold + nTax) );
	}
	else
	{
		return;
	}

	const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
	if( lpszPlayer == NULL )
		lpszPlayer = "";
	g_DPCoreClient.SendWCWantedGold( lpszPlayer, pUser->m_idMurderer, nGold, szMsg );
}


// 현상금 리스트 요청 패킷
void CDPSrvr::OnNWWantedList( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	{
		BEFORESENDSOLE( out, PACKETTYPE_WN_WANTED_LIST, pUser->m_Snapshot.dpidUser );

		CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
		wantedListSnapshot.Write( out );

		SEND( out, this, dpidCache );
	}
}

void CDPSrvr::OnNWWantedName( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	LPCSTR lpszPlayer = "";
	if( pUser->m_idMurderer )		
	{
		lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
		if( lpszPlayer == NULL )
			lpszPlayer	= "";
	}

	{
		BEFORESENDSOLE( ar, PACKETTYPE_WN_WANTED_NAME, pUser->m_Snapshot.dpidUser );
		ar.WriteString( lpszPlayer );
		SEND( ar, this, dpidCache );
	}
}


// 현상범 자세한정보 요청 패킷 
void CDPSrvr::OnNWWantedInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char szPlayer[64];
	ar.ReadString( szPlayer, _countof( szPlayer ) );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
	int nIndex = wantedListSnapshot.GetPlayerIndex( szPlayer );
	if( nIndex < 0 )
		return;

	{
		int nGold = REQ_WANTED_GOLD;
		//		if( pUser->GetGold() >= nGold ) 
		if( pUser->CheckUserGold( nGold, false ) == true )
		{
			D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );		// 현상범의 위치 
			BYTE		byOnline = 0;					// 1 이면 online
			DWORD		dwWorldID = 0;
			LPCTSTR		lpszWorld = "";

			u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( szPlayer );
			FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idPlayer );	
			if( IsValidObj(pTarget) && pTarget->GetWorld() )
			{
				vPos      = pTarget->GetPos();
				byOnline  = 1;
				dwWorldID = pTarget->GetWorld()->GetID();
				lpszWorld = pTarget->GetWorld()->m_szWorldName;

				pUser->AddGold( -nGold );
			}

			pUser->AddWantedInfo( vPos, byOnline, dwWorldID, lpszWorld );
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );	// 인벤에 돈이부족
		}
	}
}

void CDPSrvr::OnReqLeave( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->m_dwLeavePenatyTime == 0 )	// 페널티 타임을 세팅한 적이 없는가?
		pUser->m_dwLeavePenatyTime = ::timeGetTime() + TIMEWAIT_CLOSE * 1000;	//  세팅 
}

void CDPSrvr::OnStateMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwStateMode;
	BYTE nFlag;
	ar >> dwStateMode;
	ar >> nFlag;


	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->IsStateMode( dwStateMode ) )
		{
			if( nFlag == STATEMODE_BASEMOTION_CANCEL )
			{
				pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
				pUser->m_nReadyTime = 0;
				pUser->m_dwUseItemId = 0;
			}
		}
	}
}

void CDPSrvr::OnChangeMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nFlag;
	DWORD dwMode;
	ar >> dwMode;
	ar >> nFlag;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		switch( nFlag )
		{
		case 0:	// 자동 PK ON
			pUser->SetMode( FREEPK_MODE );
			break;
		case 1:	// 자동 PK OFF
			pUser->SetNotMode( FREEPK_MODE );
			break;
		case 2:	// 자동 PVP ON
			pUser->SetMode( PVPCONFIRM_MODE );
			break;
		case 3:	// 자동 PVP OFF
			pUser->SetNotMode( PVPCONFIRM_MODE );
			break;
		}
		g_xWSUserManager->AddModifyMode( pUser );
	}
}

void CDPSrvr::OnQuerySetPlayerName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// ITEM_INDEX( 10424, II_SYS_SYS_SCR_CHANAM )
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };

	ar >> dwData;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
			return;

		if( prj.IsInvalidName( lpszPlayer ) || prj.IsAllowedLetter( lpszPlayer ) == FALSE )
			pUser->AddDiagText( prj.GetText( TID_DIAG_0020 ) ); 
		prj.Formalize( lpszPlayer, _countof( lpszPlayer ) );

		if( pUser->m_bAllAction )
		{
			WORD wId	= LOWORD( dwData );
			WORD wMode	= HIWORD( dwData );
			UNREFERENCED_PARAMETER( wMode );

			FLItemElem* pItemElem = (FLItemElem*)pUser->GetItemId( wId );
			if( IsUsableItem( pItemElem ) && pItemElem->m_dwItemId == ITEM_INDEX( 10424, II_SYS_SYS_SCR_CHANAM ) && pItemElem->m_bQuery == FALSE )
			{
				pItemElem->m_bQuery	= TRUE;
				g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
			}
		}
		else
		{
			g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
		}
	}
}

void CDPSrvr::OnQuerySetGuildName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE nId;
	char lpszGuild[MAX_G_NAME]	= { 0, };

	ar >> nId;
	ar.ReadString( lpszGuild, _countof( lpszGuild ) );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( prj.IsInvalidName( lpszGuild ) || prj.IsAllowedLetter( lpszGuild ) == FALSE )
			pUser->AddDiagText( prj.GetText( TID_DIAG_0020 ) ); 

		if( pUser->m_bAllAction )
		{
			FLItemElem* pItemElem = (FLItemElem*)pUser->GetItemId( nId );
			if( IsUsableItem( pItemElem ) && pItemElem->m_bQuery == FALSE )
			{
				pItemElem->m_bQuery	= TRUE;
				g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId );
			}
		}
		else
		{
			g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId );
		}
	}
}

void CDPSrvr::PutCreateItemLog( FLWSUser* pUser, FLItemElem* pItemElem, const char* szAction, const char* recv )
{
	LogItemInfo logitem;
	//logitem.Action		= szAction;
	//logitem.SendName = pUser->GetName();
	//logitem.RecvName = recv;
	FLStrcpy( logitem.Action, _countof( logitem.Action ), szAction );
	FLStrcpy( logitem.SendName, _countof( logitem.SendName ), pUser->GetName() );
	FLStrcpy( logitem.RecvName, _countof( logitem.RecvName ), recv );

	logitem.WorldId = pUser->GetWorld()->GetID();
	logitem.Gold = pUser->GetGold();
	logitem.Gold2 = 0;
	logitem.kLogItem.dwSerialNumber = pItemElem->GetSerialNumber();
	//logitem.ItemName = pItemElem->GetProp()->szName;
	FLSPrintf( logitem.kLogItem.szItemName, _countof( logitem.kLogItem.szItemName ), "%d", pItemElem->GetProp()->dwID );
	logitem.kLogItem.nQuantity = pItemElem->m_nItemNum;
	logitem.kLogItem.nAbilityOption = 0;
	logitem.Gold_1 = 0;
	logitem.kLogItem.nItemResist = pItemElem->m_byItemResist;
	logitem.kLogItem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
	logitem.kLogItem.nHitPoint		= 100;
	OnLogItem( logitem );
}

// 클라로부터 탈출요청이 들어옴
void CDPSrvr::OnDoEscape( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10435, II_CHR_SYS_SCR_ESCAPEBLINKWING ) );
		if( pItemProp )
		{
			if( !pUser->IsSMMode( SM_ESCAPE ) )
				pUser->DoUseItemVirtual( ITEM_INDEX( 10435, II_CHR_SYS_SCR_ESCAPEBLINKWING ), FALSE );
			else
				pUser->AddDefinedText( TID_GAME_STILLNOTUSE );		
		}
	}
}

void CDPSrvr::OnCheering( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	ar >> objid;

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetId() == objid )
			return;

		CMover* pTarget		= prj.GetMover( objid );
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer() )
		{
			if( pUser->IsDie() == TRUE || pTarget->IsDie() == TRUE )
			{
				return;
			}

			DWORD dwTickCount	= GetTickCount();
			if( pUser->m_nCheerPoint <= 0 )
			{
				pUser->AddDefinedText( TID_CHEER_NO1, "%d", (pUser->m_dwTickCheer - dwTickCount) / 60000 );
				return;
			}

			if( pUser->m_nCheerPoint == MAX_CHEERPOINT )
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, TICK_CHEERPOINT );
			else
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, pUser->m_dwTickCheer - dwTickCount );

			FLOAT fAngle = GetDegree(pTarget->GetPos(), pUser->GetPos());
			pUser->SetAngle(fAngle);

			if( pTarget->GetSex() == pUser->GetSex() )
			{
				((FLWSUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE3, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEERSAME, ANILOOP_1PLAY );
			}
			else
			{
				((FLWSUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE4, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEEROTHER, ANILOOP_1PLAY );
			}

			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1717, XI_CHEERSENDEFFECT ) );
			g_xWSUserManager->AddCreateSfxObj(pTarget, XI_INDEX( 1718, XI_CHEERRECEIVEEFFECT ) );


			CMover* pSrc = (CMover*)pUser ;
			g_xWSUserManager->AddMoverBehavior( pSrc, TRUE );
			//g_xWSUserManager->AddMoverBehavior( pSrc, pSrc->GetPos(), pSrc->m_pActMover->m_vDelta,
			//	pSrc->GetAngle(), pSrc->m_pActMover->GetState(), pSrc->m_pActMover->GetStateFlag(), 
			//	pSrc->m_dwMotion, pSrc->m_pActMover->m_nMotionEx, pSrc->m_pModel->m_nLoop, pSrc->m_dwMotionOption, g_TickCount.GetTickCount(), TRUE );

			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10445, II_CHEERUP ) ); // 응원 아이템
			if( pItemProp )
			{
				g_xApplyItemEffect->DoApplyEffect( pTarget, pTarget, pItemProp );
			}
		}
		else
		{
			pUser->AddDefinedText( TID_CHEER_NO2, "" );	
		}
	}
}

void CDPSrvr::OnQueryEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	ar >> objid;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		FLWSUser* pUsertmp		= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
			if( pUser->IsDie() == TRUE || pUsertmp->IsDie() == TRUE )
			{
				return;
			}

			if( pUsertmp->IsMode( EQUIP_DENIAL_MODE ) && pUser->IsAuthHigher( AUTH_GAMEMASTER ) == FALSE )
			{
				pUser->AddDefinedText( TID_DIAG_0088 );
				return;
			}
			pUser->AddQueryEquip( pUsertmp );
		}
	}
}

void CDPSrvr::OnQueryEquipSetting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BOOL bAllow;
	ar >> bAllow;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( bAllow )
			pUser->SetNotMode( EQUIP_DENIAL_MODE );
		else
			pUser->SetMode( EQUIP_DENIAL_MODE );
		g_xWSUserManager->AddModifyMode( pUser );
	}
}

void CDPSrvr::OnOptionEnableRenderMask( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BOOL bEnable;
		ar >> bEnable;
		if( bEnable )
			pUser->SetNotMode( MODE_OPTION_DONT_RENDER_MASK );
		else
			pUser->SetMode( MODE_OPTION_DONT_RENDER_MASK );
		g_xWSUserManager->AddModifyMode( pUser );
	}
}

void CDPSrvr::OnOptionEnableRenderCostume( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nCostumeIdx;
		BOOL bEnable;
		ar >> nCostumeIdx;
		ar >> bEnable;

		DWORD dwMode = 0;
		
		switch(nCostumeIdx)
		{
			case 0:
				dwMode = MODE_OPTION_DONT_RENDER_COSTUME1;
				break;
			case 1:
				dwMode = MODE_OPTION_DONT_RENDER_COSTUME2;
				break;
			case 2:
				dwMode = MODE_OPTION_DONT_RENDER_COSTUME3;
				break;
			case 3:
				dwMode = MODE_OPTION_DONT_RENDER_COSTUME4;
				break;
			case 4:
				dwMode = MODE_OPTION_DONT_RENDER_COSTUME5;
				break;

		}

		if( bEnable )
			pUser->SetNotMode( dwMode );
		else
			pUser->SetMode( dwMode );
		g_xWSUserManager->AddModifyMode( pUser );
	}
}


void CDPSrvr::OnReturnScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nSelect;
	ar >> nSelect;
	if( nSelect < -1 || nSelect > 2 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	if( nSelect >= 0 )
	{
		static char* szPos[] = { "flaris",	"saintmorning",	"darkon" };
		pUser->m_lpszVillage = szPos[nSelect];
		pUser->AddReturnScroll();	//응답을 보내면 '귀환의 두루마리'아이템을 사용한다.	
	}
	else
	{
		// 저장된 위치로 돌아가기 
		if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10469, II_SYS_SYS_SCR_RETURN ) ) )
			pUser->DoUseItemVirtual( ITEM_INDEX( 10469, II_SYS_SYS_SCR_RETURN ), TRUE );
	}
}

void CDPSrvr::OnEndSkillQueue( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		pUser->m_playTaskBar.OnEndSkillQueue( pUser );
	}
}

void CDPSrvr::OnQueryPostMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	char lpszText[MAX_MAILTEXT]		= { 0, };
	DWORD dwItemObjID;
	int nItemNum;
	int nGold;

	ar >> dwItemObjID >> nItemNum;
	ar.ReadString( lpszReceiver, _countof( lpszReceiver ) );

	ar >> nGold;
	ar.ReadString( lpszTitle, _countof( lpszTitle ) );
	ar.ReadString( lpszText, _countof( lpszText ) );

	//DWORD nPostGold = 100;

	//if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	//	nPostGold = 500;

	const int nCommissionGold		= ( g_xFlyffConfig->GetMainLanguage() == LANG_KOR ) ? 100 : 500;
	const int nTotGold				= nCommissionGold + nGold;				
	if( nGold < 0 || nTotGold <= 0 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		//raiders.2006.11.27
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		if( pUser->m_bBank )				// 창고를 열고 있으면?
			return;
		if( pUser->m_bAllAction == FALSE )
			return;
		//--
		u_long idReceiver	= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszReceiver );
		if( idReceiver > 0 )
		{
			if( CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() ) == idReceiver )
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_MSGSELFSENDERROR));
				return;
			}

			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_POST, pUser->GetWorld(), pUser->GetPos() ) )
			{
				return;
			}
			CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
			if( pMailBox && pMailBox->size() >= MAX_MAIL )
			{
				pUser->AddDefinedText( TID_GAME_MAILBOX_FULL, "%s", lpszReceiver );
				return;
			}

			FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwItemObjID );
			if( pItemElem )
			{
				if( IsUsableItem( pItemElem ) == FALSE )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pItemElem->IsQuest() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pItemElem->IsOwnState() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->IsUsing( pItemElem ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_DO_USINGITEM ) );
					return;
				}
				PT_ITEM_SPEC pProp	= pItemElem->GetProp();
				if( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				//				if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) )
				//				{
				//					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
				//					return;
				//				}
				if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				if( nItemNum <= 0 )			// hacking
					return;
				if( pItemElem->m_nItemNum < nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
				if( pItemElem->IsCharged() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
			}
			//			if( pUser->GetGold() < (int)( ( nPostGold + nGold ) ) )
			if( pUser->CheckUserGold( nTotGold, false ) == false )
			{
				pUser->AddDiagText( prj.GetText( TID_GAME_LACKMONEY ) );
				return;
			}

			pUser->AddGold( -nTotGold, TRUE );	// 사용료 지급

			FLItemElem	itemElem;
			if( pItemElem )
			{
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->RemoveItem( pItemElem->m_dwObjId, nItemNum );
				CWorld* pWorld	= pUser->GetWorld();
				if( pWorld )
#ifdef __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
			}

			// 			//	BEGINTEST
			// 			FLERROR_LOG( PROGRAM_NAME, _T( "Receiver[%d] Sender[%d]" ), idReceiver, pUser->m_idPlayer );

			g_dpDBClient.SendQueryPostMail( idReceiver, pUser->m_idPlayer, itemElem, nGold, lpszTitle, lpszText );
		}
		else
		{
			// input name is wrong
			pUser->AddDiagText(prj.GetText(TID_MAIL_UNKNOW));
		}
	}
	else
	{
		// 		//	BEGINTEST
		// 		FLERROR_LOG( PROGRAM_NAME, _T( "pUser == NULL [%d]" ), dpidUser );
	}

}

void CDPSrvr::OnQueryRemoveMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long nMail;
	ar >> nMail;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryRemoveMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid nMail. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid pMailBox. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryGetMailItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long nMail;
	ar >> nMail;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox )
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if( pMail && pMail->m_pItemElem )
			{
				PT_ITEM_SPEC pItemProp = pMail->m_pItemElem->GetProp();
				if( pItemProp != NULL )
				{
					int nSpace = pMail->m_pItemElem->m_nItemNum / pItemProp->dwPackMax;
					if( pMail->m_pItemElem->m_nItemNum % pItemProp->dwPackMax > 0 )
					{
						if( ++nSpace <= 0 )
						{
							return;
						}
					}
					if( pUser->m_Inventory.GetEmptyCountByItemId( pMail->m_pItemElem->m_dwItemId ) < nSpace )
					{
						pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
						return;
					}
				}
				else
				{
					return;
				}


				// 기본 보관일수 지났는지를 검사하여 보관료 부과한다.
				int nDay = 0;
				DWORD dwTime = 0;
				pMail->GetMailInfo( &nDay, &dwTime );

				// 기본 보관일수가 지났다!!!
				if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
				{
					FLOAT fCustody = 0.0f;
					FLOAT fPay = 0.0f;
					fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );
					fPay = pMail->m_pItemElem->GetCost() * fCustody;
					if( fPay < 0.0f )
						fPay = 0.0f;

					const int pay	= ( int )fPay;
					if( pay < 0  ||  pUser->CheckUserGold( pay, false ) == false )
					{
						pUser->AddDiagText( prj.GetText( TID_GAME_LACKMONEY ) );
						return;
					}
					pUser->AddGold( -pay );
					//pUser->AddGold( -((int)fPay) );
				}
			}
		}
		g_dpDBClient.SendQueryGetMailItem( pUser->m_idPlayer, nMail );
	}
}

void CDPSrvr::OnQueryGetMailGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long nMail;
	ar >> nMail;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				if( pMail->m_nGold <= INT_MAX/*std::numeric_limits< int >::max()*/ && pMail->m_nGold > 0 && pUser->CheckUserGold( pMail->m_nGold, true ) == true )
				{
					g_dpDBClient.SendQueryGetMailGold( pUser->m_idPlayer, nMail );
				}
				else
				{
					return;
				}
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid nMail. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid pMailBox. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryReadMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	u_long nMail;
	ar >> nMail;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryReadMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid nMail. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid pMailBox. idReceiver : %07d, nMail :[%d]" ), pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryMailBox( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// 	//	BEGINTEST
	// 	FLERROR_LOG( PROGRAM_NAME, _T( "[%d]" ), dpidUser );

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );

	

	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		int	nClientReqCount	= 1;
		if( pUser->CheckClientReq()== false )
		{
			nClientReqCount	= 1;
		}
		else
		{
			nClientReqCount	= pUser->GetCountClientReq();
		}

		CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox )
		{
			switch( pMailBox->m_nStatus )
			{
			case CMailBox::data:	// 데이터가 들어있는 메일 박스면 바로 사용자 전송
				{
					if( nClientReqCount <= 1 )
					{
						pUser->AddMailBox( pMailBox );
					}
					else
					{
						g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
					}

					// 						//	BEGINTEST
					// 						FLERROR_LOG( PROGRAM_NAME, _T( "CMailBox::data [%d]" ), dpidUser );
				}
				break;
			case CMailBox::nodata:	// 데이터가 없는 메일 박스면 트랜스 서버에 정보 요청, 상태는 읽는 중
				{
					if( nClientReqCount >= 2 )
					{
						g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
					}
					else
					{
						g_dpDBClient.SendQueryMailBox( pUser->m_idPlayer );
					}

					pMailBox->m_nStatus		= CMailBox::read;
				}
				break;
			case CMailBox::read:	// 데이터를 요청하고 대기하는 상태면 무시
				{
					if( nClientReqCount >= 2 )
					{
						g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
					}
					// 						//	BEGINTEST
					// 						FLERROR_LOG( PROGRAM_NAME, _T( "CMailBox::read [%d]" ), dpidUser );
				}
				break;
			default:
				{
					// 						//	BEGINTEST
					// 						FLERROR_LOG( PROGRAM_NAME, _T( "default [%d]" ), dpidUser );
				}
				break;
			}
		}
		else // 월드서버에 유저의 메일박스가 없다. 트랜스에 요청
		{
			if( pUser->GetCheckTransMailBox() == FALSE )
			{
				g_dpDBClient.SendQueryMailBoxReq( pUser->m_idPlayer );
			}
			else
			{
				pUser->SendCheckMailBoxReq( FALSE );
			}
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid User : %d" ), dpidUser );
	}
}

void CDPSrvr::OnGCApp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	BYTE nState;
	ar >> nState;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}
#ifdef GUILD_WINNER_BUFF
		if(pUser->m_idGuild == g_GuildCombatMng.m_uWinGuildId && g_GuildCombatMng.m_nWinGuildCount >= 2)
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_WIN_EXCESS ) );
			return;
		}
#endif
		CGuild *pGuild = pUser->GetGuild();

		// 길드가 없거나 길드장이 아니면 신청 불가
		if( pGuild == NULL || pGuild->IsMaster( pUser->m_idPlayer ) == FALSE )
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER ) );
			return;
		}

		if( nState == GC_IN_WINDOW )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
			{
				// 1:1길드대전에 입찰한 길드는 입찰 불가능하다.
				int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
				if( nIndex != NULL_ID )
				{
					pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_ISGC1TO1TENDER );
					return;
				}
				DWORD dwMinRequestPenya = pUser->m_idGuild == g_GuildCombatMng.m_uWinGuildId ? g_GuildCombatMng.m_nJoinPanya * (g_GuildCombatMng.m_nWinGuildCount + 1) : g_GuildCombatMng.m_nJoinPanya;
				pUser->AddGCWindow( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.GetRequstPenya( pUser->m_idGuild ), dwMinRequestPenya );
			}
		}
		else if( nState == GC_IN_APP )
		{
			DWORD dwPenya;
			ar >> dwPenya;
			g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );	// 
		}
	}
}
void CDPSrvr::OnGCCancel( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		g_GuildCombatMng.GuildCombatCancel( pUser );
	}
}
void CDPSrvr::OnGCRequestStatus( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		pUser->AddGCRequestStatus( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.vecRequestRanking );
	}
}
void CDPSrvr::OnGCSelectPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	BOOL bWindow;
	ar >> bWindow;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		// 캐릭터를 선택할수 있는 시간인지 검사
		if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE ) 
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.		
			return;
		}

		// 캐릭터를 선택할수 있는 길드 인지 검사
		if( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, FALSE ) == FALSE )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP_FAIL) );	//길드대전 입찰을 하지 않았거나 최종 선발 길드순위에 들지 못했습니다.		
			return;
		}

		CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
		// 캐릭터를 선택할수 잇는것은 마스터와 킹핀급이다.
		BOOL bMK = FALSE;
		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember )
			{
				if( pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER )
					bMK = TRUE;
			}
		}


		if( pGuild && bMK )
		{
			if( bWindow == FALSE )
			{
				// 윈도우 메세지가 아니므로 Settting
				int nSize = 0;
				u_long uidPlayer = NULL_PLAYER_ID, uidDefender = NULL_PLAYER_ID;
				std::vector< u_long > vecSelectPlayer;
				BOOL bMasterOrKinpin = FALSE;
				BOOL bDefender = FALSE;
//				BOOL bLevel = FALSE;
//				BOOL bLogOut = FALSE;
//				BOOL bGuildMember = FALSE;
				BOOL bMastertoDefender = FALSE;
				vecSelectPlayer.clear();
				ar >> uidDefender;
				ar >> nSize;
				if( nSize > g_GuildCombatMng.m_nMaxJoinMember )
					return;

				for( int i = 0 ; i < nSize ; ++i )
				{
					ar >> uidPlayer;
					vecSelectPlayer.push_back( uidPlayer );
					FLWSUser* pUsertmp = g_xWSUserManager->GetUserByPlayerID( uidPlayer );
					if( IsValidObj( pUsertmp ) )
					{
						CGuildMember* pGuildMember = pGuild->GetMember( uidPlayer );
						// 길드의 맴버
						if( pGuildMember )
						{
							// 마스터가 리스트에 있는지?
							if( pGuild->IsMaster( pUsertmp->m_idPlayer ) )
								bMasterOrKinpin = TRUE;
							// 킹핀이 리스트에 있는지?
							if( pGuildMember->m_nMemberLv == GUD_KINGPIN )
								bMasterOrKinpin = TRUE;
							// 디펜더가 리스트에 있는지?
							if( pUsertmp->m_idPlayer == uidDefender )
								bDefender = TRUE;
						}
					}
				}
				// 마스터는 디펜더가 될수 없음.
				if( 1 < nSize && pGuild->IsMaster( uidDefender ) )
					bMastertoDefender = TRUE;

				if( bMasterOrKinpin && bMastertoDefender == FALSE 
#ifndef _DEBUG
					&& bDefender
#endif // _DEBUG
					)
				{
					g_GuildCombatMng.SelectPlayerClear( pUser->m_idGuild );
					for( int veci = 0 ; veci < (int)( vecSelectPlayer.size() ) ; ++veci )
					{
						// 최대 인원수 이상은 안들어가짐.
						if( veci >= g_GuildCombatMng.m_nMaxJoinMember )
							break;

						u_long uidSelectPlayer = vecSelectPlayer[veci];
						FLWSUser* pUsertmp = g_xWSUserManager->GetUserByPlayerID( uidSelectPlayer );
						{
							if( IsValidObj( pUsertmp ) )
							{
								CGuildMember* pGuildMember = pGuild->GetMember( uidPlayer );
								// 길드의 맴버
								if( pGuildMember )
								{
									// 레벨이 30이상만 참여가능
									if( 30 <= pUsertmp->GetLevel() )
									{
										g_GuildCombatMng.AddSelectPlayer( pUser->m_idGuild, uidSelectPlayer );
									}
								}
							}
						}						
					}
					g_GuildCombatMng.SetDefender( pUser->m_idGuild, uidDefender );			

					FLERROR_LOG( PROGRAM_NAME, _T( "GuildCombat SelectPlayer GuildID=%d" ), pUser->m_idGuild );
				}
			}

			std::vector<CGuildCombat::__JOINPLAYER> vecSelectList;
			g_GuildCombatMng.GetSelectPlayer( pUser->m_idGuild, vecSelectList );
			pUser->AddGCSelectPlayerWindow( vecSelectList, g_GuildCombatMng.GetDefender(pUser->m_idGuild), bWindow, g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE) );
			if( bWindow == FALSE )
			{
				g_xWSUserManager->AddGCGuildStatus( pUser->m_idGuild );
				g_xWSUserManager->AddGCWarPlayerlist( pUser->m_idGuild );
			}
		}
	}
}
void CDPSrvr::OnGCSelectMap( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	int nMap;
	ar >> nMap;
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		g_GuildCombatMng.SetSelectMap( pUser, nMap );
	}

	return;
}
void CDPSrvr::OnGCJoin( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDWAR_JOIN, pUser->GetWorld(), pUser->GetPos() ) )
			return;
		g_GuildCombatMng.GuildCombatEnter( pUser );
	}
}
void CDPSrvr::OnGCGetPenyaGuild( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CGuild *pGuild = pUser->GetGuild();		
		if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			// 신청금액 및 보상이 있는가?
			__int64 nGetPenya = 0;
			BOOL bFind = FALSE;
			CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild = { 0 };
			for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValueGuild.size() ) ; ++veci )
			{
				ResultValueGuild = g_GuildCombatMng.m_GCResultValueGuild.at( veci );
				if( pGuild->m_idGuild == ResultValueGuild.uidGuild )
				{
					bFind = TRUE;
					nGetPenya = ResultValueGuild.nReturnCombatFee + ResultValueGuild.nReward;
					break;
				}
			}

			if( bFind )
			{
				__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				{
					pUser->AddGCGetPenyaGuild( 3, nGetPenya );
				}
				else
					g_dpDBClient.SendGCGetPenyaGuild( pUser->m_idPlayer, ResultValueGuild.nCombatID, ResultValueGuild.uidGuild );
			}
			else
			{
				pUser->AddGCGetPenyaGuild( 2, nGetPenya );
			}
		}
	}
}

void CDPSrvr::OnGCGetPenyaPlayer( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		// 보상이 있는가?
		__int64 nGetPenya = 0;
		BOOL bFind = FALSE;
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer = { 0 };
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValuePlayer.size() ) ; ++veci )
		{
			ResultValuePlayer = g_GuildCombatMng.m_GCResultValuePlayer.at( veci );
			if( pUser->m_idPlayer == ResultValuePlayer.uidPlayer )
			{
				bFind = TRUE;
				nGetPenya = ResultValuePlayer.nReward;
				break;
			}
		}

		if( bFind )
		{
			__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
			{
				pUser->AddGCGetPenyaPlayer( 2, nGetPenya );
			}
			else
				g_dpDBClient.SendGCGetPenyaPlayer( pUser->m_idPlayer, ResultValuePlayer.nCombatID, ResultValuePlayer.uidGuild );
		}
		else
		{
			pUser->AddGCGetPenyaPlayer( 1, nGetPenya );
		}
	}
}

void CDPSrvr::OnGCTele( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
		if( pRgnElem )
			((CMover*)pUser)->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_NORMAL, nDefaultLayer );
	}
}
void CDPSrvr::OnGCPlayerPoint( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		pUser->AddGCPlayerPoint();
	}
}

void CDPSrvr::OnSummonFriend( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };

	ar >> dwData;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );

	if( strlen(lpszPlayer) >= MAX_NAME )
		return;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nState = pUser->GetSummonState();
		if( nState != 0 )
		{
			DWORD dwMsgId = 0;
			if( nState == 1 )	// 거래중
				dwMsgId = TID_GAME_TRADE_NOTUSE;
			else if( nState == 2 ) // 죽음
				dwMsgId = TID_GAME_DIE_NOTUSE;
			else if( nState == 3 ) // 개인상점 중
				dwMsgId = TID_GAME_VENDOR_NOTUSE;
			else if( nState == 4 ) // 전투중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
			else if( nState == 5 ) // 비행중
				dwMsgId = TID_GAME_FLY_NOTUSE;
			else if( nState == 6 ) // 듀얼중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;

			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
			return;
		}

		WORD wId	= LOWORD( dwData );
		WORD wMode	= HIWORD( dwData );
		UNREFERENCED_PARAMETER( wMode );

		FLItemElem* pItemElem = (FLItemElem*)pUser->GetItemId( wId );
		if( IsUsableItem( pItemElem ) )
		{
			if( pItemElem->m_dwItemId != ITEM_INDEX( 26201, II_SYS_SYS_SCR_FRIENDSUMMON_A ) && pItemElem->m_dwItemId != ITEM_INDEX( 26217, II_SYS_SYS_SCR_FRIENDSUMMON_B ) )
				return;
			if( pItemElem->m_bQuery )
				return;

			FLWSUser* pUsertmp = g_xWSUserManager->GetUserByPlayerID( CPlayerDataCenter::GetInstance()->GetPlayerId( lpszPlayer ) );
			if( IsValidObj( (CObj*)pUsertmp ) )
			{
				nState = pUsertmp->GetSummonState();
				if( nState != 0 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE1;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE1;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE1;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
					else if( nState == 5 ) // 비행중
						dwMsgId = TID_GAME_FLY_NOTUSE1;
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;

					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
				}
				else if( pUser->m_idPlayer == pUsertmp->m_idPlayer )
				{
					pUser->AddDefinedText( TID_GAME_SUMMON_FRIEND_MY_NOUSE );
				}
				else if( prj.IsGuildQuestRegion( pUser->GetWorld()->GetID(), pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( prj.IsGuildQuestRegion( pUsertmp->GetWorld()->GetID(), pUsertmp->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
				}
				else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
					|| pUser->GetLayer() != pUsertmp->GetLayer()
					)
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", strtmp );
				}
				else if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer )
					|| CRainbowRaceMng::GetInstance()->IsEntry( pUsertmp->m_idPlayer ) )
				{
					pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
					return;
				}
				else if( pUser->GetWorld()->GetID() == WI_WORLD_QUIZ 
					|| pUsertmp->GetWorld()->GetID() == WI_WORLD_QUIZ )
					pUser->AddDefinedText( TID_GAME_QUIZ_DO_NOT_USE );
				else
				{
					if( pUsertmp->m_RTMessenger.IsBlock( pUser->m_idPlayer ) )
					{
						pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer );
						return;
					}
					pItemElem->m_bQuery		= TRUE;
					pUsertmp->AddSummonFriendConfirm( pUser->GetId(), dwData, pUser->GetName(), pUser->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", lpszPlayer );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer );
			}
		}	
		else
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26201, II_SYS_SYS_SCR_FRIENDSUMMON_A ) );
			if( pItemProp )
				pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, lpszPlayer );
		}
	}
}
void CDPSrvr::OnSummonFriendConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	DWORD dwData;

	ar >> objid >> dwData;
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		FLWSUser* pUsertmp = prj.GetUser( objid );
		if( IsValidObj( (CObj*)pUsertmp ) )
		{
			int nState = pUser->GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE;
				else if( nState == 6 ) // 듀얼중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;

				pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( dwMsgId ) );
				pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON, "\"%s\"", prj.GetText( dwMsgId + 1 ) );
				return;
			}

			nState = pUsertmp->GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE1;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE1;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE1;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE1;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE1;
				else if( nState == 6 ) // 듀얼중
					dwMsgId = TID_GAME_ATTACK_NOTUSE1;

				pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
				pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( dwMsgId - 1 ) );
				return;
			}

			WORD wId	= LOWORD( dwData );
			WORD wMode	= HIWORD( dwData );
			UNREFERENCED_PARAMETER( wMode );

			FLItemElem* pItemElem = (FLItemElem*)pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) )
			{
				if( ( pItemElem->m_dwItemId != ITEM_INDEX( 26201, II_SYS_SYS_SCR_FRIENDSUMMON_A ) && pItemElem->m_dwItemId != ITEM_INDEX( 26217, II_SYS_SYS_SCR_FRIENDSUMMON_B ) ) || pItemElem->m_bQuery == FALSE )
					return;

				if( prj.IsGuildQuestRegion( pUser->GetWorld()->GetID(), pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
				}
				else if( prj.IsGuildQuestRegion( pUsertmp->GetWorld()->GetID(), pUsertmp->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
					|| pUser->GetLayer() != pUsertmp->GetLayer()
					)
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUsertmp->GetWorld()->m_szWorldName, pUser->GetWorld()->m_szWorldName );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON, "\"%s\"", strtmp );
				}
				else if( pUser->GetWorld()->GetID() == WI_WORLD_QUIZ 
					|| pUsertmp->GetWorld()->GetID() == WI_WORLD_QUIZ )
					pUser->AddDefinedText( TID_GAME_QUIZ_DO_NOT_USE );
				else 
				{
					g_dpDBClient.SendLogSMItemUse( "1", pUsertmp, pItemElem, pItemElem->GetProp(), pUser->GetName() );

					pUser->REPLACE( g_uIdofMulti, pUsertmp->GetWorld()->GetID(), pUsertmp->GetPos(), REPLACE_NORMAL, pUsertmp->GetLayer() );

					pItemElem->m_bQuery		= FALSE;
					pUsertmp->RemoveItem( wId, 1 );
					pUser->AddDefinedText( TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pUsertmp->GetName() );
					pUsertmp->AddDefinedText( TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName() );
				}
			}
			else
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26201, II_SYS_SYS_SCR_FRIENDSUMMON_A ) );
				if( pItemProp )
					pUsertmp->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, pUser->GetName() );
			}
		}
	}
}

void CDPSrvr::OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid >> dwData;
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		FLWSUser* pUsertmp = prj.GetUser( objid );
		if( IsValidObj( (CObj*)pUsertmp ) )
		{
			WORD wId	= LOWORD( dwData );
			FLItemElem* pItemElem = (FLItemElem*)pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) && ( pItemElem->m_dwItemId == ITEM_INDEX( 26201, II_SYS_SYS_SCR_FRIENDSUMMON_A ) || pItemElem->m_dwItemId == ITEM_INDEX( 26217, II_SYS_SYS_SCR_FRIENDSUMMON_B ) ) )
				pItemElem->m_bQuery		= FALSE;
			pUsertmp->AddDefinedText( TID_GAME_SUMMON_FRIEND_CANCEL, "\"%s\"", pUser->GetName() );
		}
	}
}

void CDPSrvr::OnSummonParty( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwData;

	ar >> dwData;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nState = pUser->GetSummonState();
		if( nState != 0 )
		{
			DWORD dwMsgId = 0;
			if( nState == 1 )	// 거래중
				dwMsgId = TID_GAME_TRADE_NOTUSE;
			else if( nState == 2 ) // 죽음
				dwMsgId = TID_GAME_DIE_NOTUSE;
			else if( nState == 3 ) // 개인상점 중
				dwMsgId = TID_GAME_VENDOR_NOTUSE;
			else if( nState == 4 ) // 전투중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
			else if( nState == 5 ) // 비행중
				dwMsgId = TID_GAME_FLY_NOTUSE;
			else if( nState == 6 ) // 듀얼중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;

			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
			return;
		}

		if( prj.IsGuildQuestRegion( pUser->GetWorld()->GetID(), pUser->GetPos() ) )
		{
			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
			return;
		}
		if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer ) )
		{
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
			return;
		}

		WORD wId	= LOWORD( dwData );
		WORD wMode	= HIWORD( dwData );
		UNREFERENCED_PARAMETER( wMode );

		FLItemElem* pItemElem = (FLItemElem*)pUser->GetItemId( wId );
		if( IsUsableItem( pItemElem ) )
		{
			CParty* pParty;
			pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
			if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
			{
				if( !pUser->HasBuff( BUFF_ITEM, (WORD)( pItemElem->GetProp()->dwID ) ) )
				{
					pParty->m_dwWorldId = pUser->GetWorld()->GetID();

					g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemElem->GetProp(), pParty->m_sParty );

					PT_ITEM_SPEC pItemProptmp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) );
					for( int i = 1 ; i < pParty->m_nSizeofMember ; i++ )
					{
						FLWSUser* pUsertmp		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
						if( IsValidObj( (CObj*)pUsertmp ) )
						{
							g_xApplyItemEffect->DoApplyEffect( pUser, pUsertmp, pItemElem->GetProp() );
							pUsertmp->AddSummonPartyConfirm( pUser->GetId(), dwData, pUser->GetWorld()->m_szWorldName );
							if( pItemProptmp )
								g_xWSUserManager->AddCreateSfxObj((CMover *)pUsertmp, pItemProptmp->dwSfxObj3 );
							pUser->AddDefinedText( TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", pUsertmp->GetName() );
						}
						else
						{
							pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( i ) ) );
						}
					}
					g_xApplyItemEffect->DoApplyEffect( pUser, pUser, pItemElem->GetProp() );
					pUser->RemoveItem( wId, 1 );

					if( pItemProptmp )
						g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, pItemProptmp->dwSfxObj3 );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LIMITED_USE );
				}
			}
		}	
		else
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) );
			if( pItemProp )
				pUser->AddDefinedText( TID_ERROR_SUMMONPARTY_NOITEM, "\"%s\"", pItemProp->szName );
		}
	}	
}

void CDPSrvr::OnSummonPartyConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid;
	ar >> dwData;
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CParty* pParty;
		pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
		if( pParty )
		{
			CMover* pLeader = prj.GetUser( objid );
			if( IsValidObj( pLeader ) && pParty->IsLeader( pLeader->m_idPlayer ) && pLeader->HasBuff( BUFF_ITEM, ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) ) )
			{
				if( pUser == pLeader )
					return;
				int nState = pUser->GetSummonState();
				if( nState != 0 && nState != 5 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE;
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE;

					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
					return;
				}

				nState = pLeader->GetSummonState();
				if( nState != 0 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE1;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE1;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE1;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
					else if( nState == 5 ) // 비행중
						dwMsgId = TID_GAME_FLY_NOTUSE1;
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;

					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
					return;
				}

				if( prj.IsGuildQuestRegion( pLeader->GetWorld()->GetID(), pLeader->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1) );
				}
				else if( prj.IsGuildQuestRegion( pUser->GetWorld()->GetID(), pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( pUser->GetWorld()->GetID() != pParty->m_dwWorldId )
				{
					CWorld* pWorld = g_WorldMng.GetWorld( pParty->m_dwWorldId );
					if( pWorld )
					{
						CString strtmp;
						strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pWorld->m_szWorldName );
						pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					}
				}
				else if( pParty->m_dwWorldId != pLeader->GetWorld()->GetID() )
				{
					CWorld* pWorld = g_WorldMng.GetWorld( pParty->m_dwWorldId );
					if( pWorld )
					{
						CString strtmp;
						strtmp.Format( prj.GetText( TID_GAME_WORLDLEADER_NOTUSE ) );
						pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					}
				}
				else if( pLeader->GetLayer() != pUser->GetLayer() )
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ) );
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
				}
				else if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer ) )
				{
					pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
				}
				else if( pLeader->GetWorld()->GetID() == WI_WORLD_QUIZ 
					|| pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
					pUser->AddDefinedText( TID_GAME_QUIZ_DO_NOT_USE );
				else
				{
					pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) );
					pUser->REPLACE( g_uIdofMulti, pLeader->GetWorld()->GetID(), pLeader->GetPos(), REPLACE_FORCE, pLeader->GetLayer() );
					pUser->AddDefinedText( TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pLeader->GetName() );
					((FLWSUser*)pLeader)->AddDefinedText( TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName() );					
					PT_ITEM_SPEC pItemProptmp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26202, II_SYS_SYS_SCR_PARTYSUMMON ) );
					if( pItemProptmp )
						g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, pItemProptmp->dwSfxObj3 );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_ERROR_SUMMONPARTY_NOTTIME );
			}
		}
		else
		{
			pUser->AddDefinedText( TID_GAME_NOPARTY );
		}
	}
}

void CDPSrvr::OnRemoveInvenItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD	dwId;
	int		nNum;
	ar >> dwId;
	ar >> nNum;

	if( nNum <= 0 )
		return;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
	if( IsUsableItem( pItemElem ) == FALSE )
		return;

	if( pUser->m_Inventory.IsEquip( dwId ) )
		return;

	if( pUser->IsUsing( pItemElem ) )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
		return;
	}

	if( pItemElem->m_nItemNum < nNum )
		return ;

	if( pItemElem->IsUndestructable() )
		return;

	CString strNum;
	strNum.Format("%d", nNum );
	pUser->AddDefinedText( TID_GAME_SUCCESS_REMOVE_ITEM, "\"%s\" \"%s\"", pItemElem->GetProp()->szName, strNum );

	LogItemInfo aLogItem;
	//aLogItem.Action = "*";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "GARBAGE";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "*" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GARBAGE" );

	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	OnLogItem( aLogItem, pItemElem, nNum );

	pUser->RemoveItem( dwId, nNum );
}

void CDPSrvr::OnCreateMonster( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwItemIdRec;
	D3DXVECTOR3 vPos;
	ar >> dwItemIdRec;
	ar >> vPos;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	DWORD dwId = HIWORD( dwItemIdRec );

	if( pUser->IsUsableState( dwId ) == FALSE )
		return;

	CCreateMonster::GetInstance()->CreateMonster( pUser, dwId, vPos );
}

void CDPSrvr::OnFoucusObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objid;
	ar >> objid;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		pUser->AddFocusObj(objid);
	}
}

void CDPSrvr::OnTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidTrader;
	ar >> objidTrader;

	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pUser->m_vtInfo.GetOther() == NULL )
	{
		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
			if( pTrader->IsPlayer() )	// pc
			{
				if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
				{
					return;
				}

				if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
				{
					return;
				}


				//개인상점 중에는 거래 불가 
				if( pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() ||
					pTrader->m_vtInfo.VendorIsVendor() || pTrader->m_vtInfo.IsVendorOpen() )
				{
					return;	//
				}
				if( pUser->m_bAllAction == FALSE || ((FLWSUser*)pTrader)->m_bAllAction == FALSE )
					return;

				if( pUser->m_bBank || pTrader->m_bBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

				if( pUser->m_bGuildBank || pTrader->m_bGuildBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

				pUser->m_vtInfo.SetOther( pTrader );
				pTrader->m_vtInfo.SetOther( pUser );

				pUser->AddTrade( (FLWSUser*)pTrader, pUser->m_idPlayer );
				( (FLWSUser*)pTrader )->AddTrade( pUser, pUser->m_idPlayer );
			}
		}
	}
}

void CDPSrvr::OnConfirmTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidTrader;
	ar >> objidTrader;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pUser->m_vtInfo.GetOther() == NULL )
	{
		// 대전장에서는 거래를 할수 없습니다.
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}

		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
			if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
			{
				return;
			}

			if( pTrader->IsPlayer() == FALSE )
				return;

			if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
			{
				return;
			}

			if( pTrader->IsAttackMode() )
				pUser->AddDefinedText( TID_GAME_BATTLE_NOTTRADE, "" );
			else
				((FLWSUser*)pTrader)->AddComfirmTrade( pUser->GetId() );
		}

		pUser->RemoveInvisible();		// 거래를 하면 투명은 풀린다.
	}

	return;
}

void CDPSrvr::OnConfirmTradeCancel( CAr & ar, DPID /*dpidCache*/, DPID /*dpidUser*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CMover* pTrader		= prj.GetMover( objidTrader );
	if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		( (FLWSUser*)pTrader )->AddComfirmTradeCancel( objidTrader );
}

void CDPSrvr::OnTradePut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE byNth, byType;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> byNth >> byType >> dwItemObjID >> nItemNum;

	if( byNth >= MAX_TRADE )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( nItemNum < 1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nItemNumber [%d]" ), nItemNum );

		return;
	}
	//	END100708
	//////////////////////////////////////////////////////////////////////////

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
		{
			return;
		}

		FLItemElem* pItem	= (FLItemElem*)pUser->GetItemId( dwItemObjID );
		if( pItem && pItem->IsFlag( FLItemElem::expired ) )
			return;

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			int nItemNumResult	= nItemNum;
			DWORD dwText	= pUser->m_vtInfo.TradeSetItem2( dwItemObjID, byNth, nItemNumResult );
			if( dwText == 0 )
			{
				pUser->AddTradePut( pUser->GetId(), byNth, byType, dwItemObjID, nItemNumResult );
				( (FLWSUser*)pTrader )->AddTradePut( pUser->GetId(), byNth, byType, dwItemObjID, nItemNumResult );
			}
			else
			{
				pUser->AddDefinedText( dwText );
			}
		}
		else
		{
			pUser->AddTradePutError();
		}
	}
}

void CDPSrvr::OnTradePull( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100708 독일, 프랑스, 미국 기능 제한
	if( g_xFlyffConfig->GetMainLanguage() == LANG_GER || g_xFlyffConfig->GetMainLanguage() == LANG_FRE || g_xFlyffConfig->GetMainLanguage() == LANG_USA )
	{
		return;
	}
	//////////////////////////////////////////////////////////////////////////


	BYTE byNth;
	ar >> byNth;

	if( byNth >= MAX_TRADE )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( pUser->m_vtInfo.TradeClearItem( byNth ) )
			{
				pUser->AddTradePull( pUser->GetId(), byNth );
				( (FLWSUser*)pUser->m_vtInfo.GetOther() )->AddTradePull( pUser->GetId(), byNth );
			}
		}
	}
}

void CDPSrvr::OnTradePutGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwGold;
	ar >> dwGold;


	int nGold = (int)( dwGold );

	if( nGold <= 0 )
	{
		return;
	}

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( nGold > pUser->GetGold() )
			{
				nGold = pUser->GetGold();
			}

			if( pUser->CheckUserGold( nGold, false ) == false )
			{
				return;
			}

			pUser->m_vtInfo.TradeSetGold( nGold );
			pUser->AddGold( -nGold, FALSE );	// raiders.2006.11.28  인벤돈 = 인벤돈 - 거래창 돈 

			pUser->AddTradePutGold( pUser->GetId(), nGold );
			( (FLWSUser*)pTrader )->AddTradePutGold( pUser->GetId(), nGold );
		}
	}
}

void CDPSrvr::OnTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nMode;
	ar >> nMode;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		pUser->m_vtInfo.TradeClear();
		pTrader->m_vtInfo.TradeClear();

		pUser->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
		( (FLWSUser*)pTrader )->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
	}
}


// pPlayer는 지금 ok를 누른 사용자이고 pTrdaer는 먼저 ok를 누른 사용자이다.	
void CDPSrvr::OnTradelastConfrim( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.TradeGetState() != TRADE_STEP_OK )
			return;

		switch( pTrader->m_vtInfo.TradeGetState() )
		{
		case TRADE_STEP_OK:
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_CONFIRM );	

			pUser->AddTradelastConfirmOk( pUser->GetId() );					// ok를 눌렀음을 표시하게 한다.
			( (FLWSUser*)pTrader )->AddTradelastConfirmOk( pUser->GetId() );	// ok를 눌렀음을 표시하게 한다.
			break;

		case TRADE_STEP_CONFIRM:
			{
				CAr ownerar;
				ownerar << (OBJID)NULL_ID << (int)SNAPSHOTTYPE_TRADECONSENT;
				CAr traderar;
				traderar << (OBJID)NULL_ID << (int)SNAPSHOTTYPE_TRADECONSENT;

				BEFORESENDDUAL( logar, PACKETTYPE_LOG_TRADE, DPID_UNKNOWN, DPID_UNKNOWN );		// log용 ar
				TRADE_CONFIRM_TYPE type = pUser->m_vtInfo.TradeLastConfirm( ownerar, traderar, logar );
				switch( type )
				{
				case TRADE_CONFIRM_ERROR:	
					pUser->AddTradeCancel( NULL_ID, pUser->m_idPlayer );	
					( (FLWSUser*)pTrader )->AddTradeCancel( NULL_ID, pUser->m_idPlayer );
					break;
				case TRADE_CONFIRM_OK:
					SEND( logar, &g_dpDBClient, DPID_SERVERPLAYER );

					pUser->AddTradeConsent( ownerar );	
					( (FLWSUser*)pTrader )->AddTradeConsent( traderar );

					CWorld * pUserWorld = pUser->GetWorld();
					if( pUserWorld != NULL )
					{
#ifdef __LAYER_1015
						g_dpDBClient.SavePlayer( pUser, pUserWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
						g_dpDBClient.SavePlayer( pUser, pUserWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
					}

					FLWSUser * pTraderUser = (FLWSUser *)pTrader;
					CWorld * pTraderWorld = pTraderUser->GetWorld();
					if( pTraderWorld != NULL )
					{
#ifdef __LAYER_1015
						g_dpDBClient.SavePlayer( pTraderUser, pWorld->GetID(), pTraderUser->GetPos(), pTraderUser->GetLayer() );
#else	// __LAYER_1015
						g_dpDBClient.SavePlayer( pTraderUser, pWorld->GetID(), pTraderUser->GetPos() );
#endif	// __LAYER_1015
					}

					break;
				}					
			}
			break;	
		} 
	}
}

// 사용자가 ok 버튼을 눌렀을 때
void CDPSrvr::OnTradeOk( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CMover* pTrader	= pUser->m_vtInfo.GetOther();
	if( IsValidObj( pTrader ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->IsDie() == TRUE || pTrader->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_OK ); 

			if( pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_OK )	// 상대가 먼저 ok를 눌러서 교환이 성립되는 경우
			{
				( (FLWSUser*)pUser )->AddTradelastConfirm();
				( (FLWSUser*)pTrader )->AddTradelastConfirm();
			}
			else	// 내가 먼저 ok 버튼을 누른 경우
			{
				pUser->AddTradeOk( pUser->GetId() );				// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
				( (FLWSUser*)pTrader )->AddTradeOk( pUser->GetId() );	// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
			}
		}
	}
}

void CDPSrvr::OnPVendorOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long uBufSize )
{
	if( uBufSize > 55 )	// 4 + 4 + 48 - 1		= 55
		return;

	char szPVendor[MAX_VENDORNAME];	// 개인 상점 이름( 48 )
	ar.ReadString( szPVendor, _countof( szPVendor ) );	

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->GetWorld() == NULL )
			return;

		if( CS_NOT_SUPPORTED != _GetContentState( CT_CONSIGNMENT_MARKET ) )
		{
			if( FLConsignmentSale_Spec::GetInstance().IsVendible( pUser->GetWorld()->m_dwWorldID ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
				return;
			}
		}


		if( prj.IsInvalidName( szPVendor ) || prj.IsAllowedLetter( szPVendor, TRUE ) == FALSE )
			pUser->AddDiagText( prj.GetText( TID_DIAG_0020 ) ); 

		if( !( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ) ) ) )
			ParsingEffect(szPVendor, _countof( szPVendor ), strlen(szPVendor) );

		if( _tcslen( szPVendor ) < 6 || _tcslen( szPVendor ) > 32 )
		{
			pUser->AddDefinedText( TID_DIAG_0011 );
			return;
		}

		// 대전장에서는 개인상점을 열수 없습니다.
		CWorld* pWorld = pUser->GetWorld();
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;
		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 개인상점 불가 
			return;
		if( 0 < pUser->m_nDuel )
		{
			return;
		}

		if( pUser->IsAttackMode() )
			return;

		pUser->m_vtInfo.SetTitle( szPVendor );

		if( pUser->m_pActMover->IsFly() )
			return;

		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nVendor )
			{
				pUser->AddDefinedText( TID_GAME_CHAOTIC_NOT_VENDOR );
				return;
			}
		}

		if( pUser->m_bAllAction == FALSE )
			return;

		if( pUser->m_vtInfo.IsVendorOpen() )
		{
			if( pUser->m_vtInfo.VendorIsVendor() )	
			{
				g_ChattingMng.NewChattingRoom( pUser->m_idPlayer );
				CChatting * pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idPlayer );

				g_xWSUserManager->AddPVendorOpen( pUser );
				if( pChatting )
				{
					pChatting->m_bState		= TRUE;
					if( pChatting->AddChattingMember( pUser->m_idPlayer ) )
						pUser->m_idChatting		= pUser->m_idPlayer;
					pUser->AddNewChatting( pChatting );
				}
				pUser->m_dwHonorCheckTime = GetTickCount();
			}
		}
	}
}

void CDPSrvr::OnPVendorClose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidVendor;
	ar >> objidVendor;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ClosePVendor( pUser, objidVendor );
	}	
}

BOOL CDPSrvr::ClosePVendor( FLWSUser* pUser, OBJID objidVendor )
{
	if( pUser->GetId() == objidVendor )
	{
		if( pUser->m_vtInfo.IsVendorOpen() == FALSE && 
			pUser->m_vtInfo.VendorIsVendor() == FALSE )
		{
			return FALSE;
		}

		CMover* pTrader	= pUser->m_vtInfo.GetOther();   //raiders.2006.11.27

		pUser->m_vtInfo.VendorClose();

		//raiders.2006.11.27
		pUser->m_vtInfo.TradeClear();
		if( pTrader )
			pTrader->m_vtInfo.TradeClear();
		//--

		g_xWSUserManager->AddPVendorClose( pUser );

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				FLWSUser* pUserBuf = (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUserBuf ) )
				{
					// 채팅방이 없어짐
					pUserBuf->AddDeleteChatting();
					pUserBuf->m_idChatting	= 0;
				}
			}
		}
		g_ChattingMng.DeleteChattingRoom( pUser->m_idPlayer );
		pUser->m_idChatting		= 0;
	}
	else
	{
		if( IsValidObj( pUser->m_vtInfo.GetOther() ) )
		{
			pUser->m_vtInfo.SetOther( NULL );
			pUser->AddPVendorClose( objidVendor );

			CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
			if( pChatting )
			{
				for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
				{
					FLWSUser* pUserBuf = (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
					if( IsValidObj( pUserBuf ) )		// 채팅에서 나감
					{							
						pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
					}
				}
				pChatting->RemoveChattingMember( pUser->m_idPlayer );
				pUser->m_idChatting		= 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// mirchang_100817 거래중인 상대방이 로그아웃 했을 경우..
		else
		{
			pUser->m_vtInfo.SetOther( NULL );
			CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
			if( pChatting != NULL )
			{
				for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
				{
					FLWSUser* pUserBuf = (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
					if( IsValidObj( pUserBuf ) )		// 채팅에서 나감
					{							
						pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
					}
				}
				pChatting->RemoveChattingMember( pUser->m_idPlayer );
				pUser->m_idChatting		= 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	return TRUE;
}

void CDPSrvr::OnBuyPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidVendor;
	BYTE nItem;
	DWORD dwItemId;
	int nNum;

	ar >> objidVendor >> nItem >> dwItemId >> nNum;
	if( nItem >= MAX_VENDITEM || nNum <= 0 )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLWSUser* pPVendor	= prj.GetUser( objidVendor );
	if( IsValidObj( pPVendor ) )
	{
		if( pUser->IsDie() == TRUE || pPVendor->IsDie() == TRUE )
		{
			return;
		}

		VENDOR_SELL_RESULT result;
		BOOL bOK = pPVendor->m_vtInfo.VendorSellItem( pUser, nItem, dwItemId, nNum, result );
		if( bOK )
		{
			CWorld * pUserWorld = pUser->GetWorld();
			if( pUserWorld != NULL )
			{
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pUserWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pUserWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
			}

			CWorld * pPVendorWorld = pPVendor->GetWorld();
			if( pPVendorWorld != NULL )
			{
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pPVendor, pPVendorWorld->GetID(), pPVendor->GetPos(), pPVendor->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pPVendor, pPVendorWorld->GetID(), pPVendor->GetPos() );
#endif	// __LAYER_1015
			}

			LogItemInfo info;

			//info.Action = "Z";
			//info.SendName = pUser->GetName();
			//info.RecvName = pPVendor->GetName();
			FLStrcpy( info.Action, _countof( info.Action ), "Z" );
			FLStrcpy( info.SendName, _countof( info.SendName ), pUser->GetName() );
			FLStrcpy( info.RecvName, _countof( info.RecvName ), pPVendor->GetName() );
			info.WorldId = pUser->GetWorld()->GetID();
			info.Gold = pUser->GetGold() + ( result.item.m_nCost * nNum );
			info.Gold2 = pUser->GetGold();
			info.Gold_1 = pPVendor->GetGold();
			OnLogItem( info, &result.item, nNum );

			//info.Action = "X";
			//info.SendName = pPVendor->GetName();
			//info.RecvName = pUser->GetName();
			FLStrcpy( info.Action, _countof( info.Action ), "X" );
			FLStrcpy( info.SendName, _countof( info.SendName ), pPVendor->GetName() );
			FLStrcpy( info.RecvName, _countof( info.RecvName ), pUser->GetName() );
			info.WorldId = pPVendor->GetWorld()->GetID();
			info.Gold = pPVendor->GetGold() - ( result.item.m_nCost * nNum );
			info.Gold2 = pPVendor->GetGold();
			info.Gold_1 = pUser->GetGold();
			OnLogItem( info, &result.item, nNum );
		}
		else
		{
			if( result.nErrorCode )
				pUser->AddDefinedText( result.nErrorCode, "" );
		}
	}
}

void CDPSrvr::OnQueryPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidVendor;
	ar >> objidVendor;


	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_pActMover->IsFly() )
			return;

		FLWSUser* pPVendor	= prj.GetUser( objidVendor );
		if( IsValidObj( pPVendor ) )
		{
			if( pUser->IsDie() == TRUE || pPVendor->IsDie() == TRUE )
			{
				return;
			}

			BOOL bChatting = TRUE;
			CChatting* pChatting = g_ChattingMng.GetChttingRoom( pPVendor->m_idChatting );
			if( pChatting )
				bChatting = pChatting->m_bState;

			if( pPVendor->m_vtInfo.IsVendorOpen() )
			{
				pUser->m_vtInfo.SetOther( pPVendor );
				pUser->AddPVendorItem( pPVendor, bChatting );
			}
		}

	}
}

void CDPSrvr::OnUnregisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE i;
	ar >> i;
	if( i >= MAX_VENDITEM )
		return;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.IsVendorOpen() )
			return;

		if( pUser->m_vtInfo.VendorClearItem( i ) )
			pUser->AddUnregisterPVendorItem( i );
	}
}

void CDPSrvr::OnRegisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	BYTE byNth, byType;
	DWORD dwItemObjID;
	int nNum;
	int nCost;
	ar >> byNth >> byType >> dwItemObjID >> nNum >> nCost;


	if( byNth >= MAX_VENDOR_REVISION )
		return;
	if( nCost < 1 )	
		nCost = 1;

	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.IsVendorOpen() )
			return;

		if( pUser->IsRegionAttr( RA_FIGHT ) )
		{
			pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;

		if( CNpcChecker::GetInstance()->IsCloseNpc( pUser->GetWorld(), pUser->GetPos() ) )
		{
			// NPC근처 개인 상점 불가(3m) - 시도 시 확인창이 생성 되도록 처리
			pUser->AddDiagText( prj.GetText( TID_GAME_NPC_RADIUS ) );
			//			pUser->AddDefinedText( TID_GAME_NPC_RADIUS );
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
			return;
		}

		FLItemElem* pItemElem = (FLItemElem *)pUser->GetItemId( dwItemObjID );
		if( IsUsableItem( pItemElem ) )		
		{
			if( pItemElem->IsQuest() )
				return;

			if( pItemElem->IsOwnState() )
				return;
			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}

			PT_ITEM_SPEC pProp	= pItemElem->GetProp();
			if( pProp->dwItemKind3 == IK3_CLOAK  && pItemElem->m_idGuild != 0 )
				return;

			//			if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) )
			//				return;
			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;

			if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				return;

			if( pItemElem->IsFlag( FLItemElem::expired ) )
				return;
			/*
			if(pProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet) //사망한 펫은 거래 불가
			{
			if(pItemElem->m_pPet->GetLife() <= 0)
			return;
			}
			*/

			if( nNum > pItemElem->m_nItemNum )
				nNum = pItemElem->m_nItemNum;
			if( nNum < 1 )	
				nNum = 1;

//			PT_ITEM_SPEC pItemProp	= pItemElem->GetProp();

			pUser->m_vtInfo.VendorSetItem( dwItemObjID, byNth, nNum, nCost );
			pUser->AddRegisterPVendorItem( byNth, 0, dwItemObjID, nNum, nCost );
		}
	}
}

void CDPSrvr::OnCreateAngel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// check user valid
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
	{	return;
	}
	if( pUser->IsDie() == TRUE )
	{	return;
	}

	// check matr. count.
	int nOrichalcum = 0;
	int nMoonstone = 0;

	char str1[1024];
	ar.ReadString( str1, _countof( str1 ) );

	char * str2 = strstr(str1,"DD"); if  (str2==NULL) return;	
	*str2 = 0;	str2=str2+2;
	char * split1 = strchr(str1,'D');	if (split1==NULL) return;
	*split1 = 0; split1++;
	int id1  = atoi(str1);
	int cnt1 = atoi(split1);
	FLItemElem* pItemElem1 = (FLItemElem*)pUser->GetItemId( id1 );
	if( pItemElem1==NULL || !IsUsableItem( pItemElem1 ) ) 	return;
	int dwid1 = pItemElem1->GetProp()->dwID;
	if (dwid1 == ITEM_INDEX(2035,II_GEN_MAT_ORICHALCUM01)	|| (dwid1 == ITEM_INDEX(2082,II_GEN_MAT_ORICHALCUM01_1) ) )	nOrichalcum+= cnt1;
	if (dwid1 == ITEM_INDEX(2036,II_GEN_MAT_MOONSTONE)		|| (dwid1 == ITEM_INDEX(2083,II_GEN_MAT_MOONSTONE_1) ) )		nMoonstone += cnt1;


	FLItemElem* pItemElem2 =NULL;
	int id2  = 0;
	int cnt2 = 0;
	char * str3 = strstr(str2,"DD");
	if (str3) 
	{		
		char * split2 = strchr(str2,'D');	if (split2==NULL) return;
		*split2 = 0; split2++;
		id2 =atoi(str2);
		cnt2 = atoi(split2);
		pItemElem2 = (FLItemElem*)pUser->GetItemId( id2 ); 
		if(pItemElem2==NULL || !IsUsableItem( pItemElem2 ) ) 		return;
		int dwid2 = pItemElem2->GetProp()->dwID;
		if (dwid2 == ITEM_INDEX(2035,II_GEN_MAT_ORICHALCUM01)	|| (dwid2 == ITEM_INDEX(2082,II_GEN_MAT_ORICHALCUM01_1) ) )	nOrichalcum+= cnt2;
		if (dwid2 == ITEM_INDEX(2036,II_GEN_MAT_MOONSTONE)		|| (dwid2 == ITEM_INDEX(2083,II_GEN_MAT_MOONSTONE_1) ) )		nMoonstone += cnt2;
	}


///////////////////////////////////////////////////////

	// R/B/G/W
	float greenAngelRate = (float) (nOrichalcum + nMoonstone);    
	float blueAngelRate  = greenAngelRate * 2.0f;      
	float whiteAngelRate = greenAngelRate * 0.1f;      
	float redAngelRate   = 100.0f - ( whiteAngelRate + greenAngelRate + blueAngelRate );    
	//  30035, II_SYS_SYS_QUE_ANGEL_RED
	//  30036, II_SYS_SYS_QUE_ANGEL_BLUE
	//  30037, II_SYS_SYS_QUE_ANGEL_GREEN
	//  30038, II_SYS_SYS_QUE_ANGEL_WHITE 
	static DWORD adwItemId[4]	= { 30035, 30036,  30037, 30038};
	FLOAT fRate[4];
	fRate[0]	= redAngelRate;                     
	fRate[1]	= fRate[0] + blueAngelRate;           
	fRate[2]	= fRate[1] + greenAngelRate;         
	fRate[3]	= fRate[2] + whiteAngelRate;       
	float rand = xRandom(1000) / 10.0f;       
	DWORD dwItemId	= 0;
	for( int i = 0; i < 4; i++ )
	{	if( rand <= fRate[i] )
		{	dwItemId = adwItemId[i];
			break;
		}
	}	

	if( dwItemId <= 0 )
		return;




	if( pUser->m_Inventory.GetEmptyCountByItemId( dwItemId ) < 1 )
	{	pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
		return;
	}

	LogItemInfo aLogItem;
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_MATERIAL" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	if( pItemElem1->GetExtra() > 0 )
		pItemElem1->SetExtra(0);
	OnLogItem( aLogItem, pItemElem1, cnt1 );	
	pUser->RemoveItem( pItemElem1->m_dwObjId, cnt1 );

	if(pItemElem2)
	{
		if( pItemElem2->GetExtra() > 0 )
			pItemElem2->SetExtra(0);
		OnLogItem( aLogItem, pItemElem2, cnt2 );	
		pUser->RemoveItem( pItemElem2->m_dwObjId, cnt2 );
	}

	FLItemElem itemElem;
	itemElem.m_dwItemId	= dwItemId;
	itemElem.m_nItemNum	= 1;
	if( pUser->CreateItem( &itemElem ) )
	{
		LogItemInfo aLogItem;
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_CREATE" );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		OnLogItem( aLogItem, &itemElem, 1 );
	}
}

void CDPSrvr::OnAngleBuff( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// 엘젤을 없애고 아이템화 인벤 자리가 없다면 메세지 처리
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->HasBuffByIk3( IK3_ANGEL_BUFF ) == FALSE )
			return;

		IBuff* pBuff	= pUser->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		if( !pBuff )
			return;
		PT_ITEM_SPEC pItemProp	= pBuff->GetSpecItem();
		if( pItemProp == NULL )
			return;

		int nAngel = (int)( (float)pItemProp->nAdjParamVal[0] );
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( pUser->m_nAngelExp < nMaxAngelExp )
			return;

		FLItemElem itemElem;
		switch( pItemProp->dwID )
		{
		case ITEM_INDEX( 30035, II_SYS_SYS_QUE_ANGEL_RED ):
			itemElem.m_dwItemId	= ITEM_INDEX( 30039, II_SYS_SYS_QUE_ANGEL_RED100 );						
			break;
		case ITEM_INDEX( 30036, II_SYS_SYS_QUE_ANGEL_BLUE ):
			itemElem.m_dwItemId	= ITEM_INDEX( 30040, II_SYS_SYS_QUE_ANGEL_BLUE100 );
			break;
		case ITEM_INDEX( 30037, II_SYS_SYS_QUE_ANGEL_GREEN ):
			itemElem.m_dwItemId	= ITEM_INDEX( 30041, II_SYS_SYS_QUE_ANGEL_GREEN100 );
			break;
		default:
			itemElem.m_dwItemId	= ITEM_INDEX( 30042, II_SYS_SYS_QUE_ANGEL_WHITE100 );
			break;
		}

		itemElem.m_nItemNum		= 1;
		if( pUser->CreateItem( &itemElem ) )
		{
			pUser->RemoveIk3Buffs( IK3_ANGEL_BUFF );

			LogItemInfo aLogItem;
			//aLogItem.Action = "&";
			//aLogItem.SendName = pUser->GetName();
			//aLogItem.RecvName = "ANGEL_CREATE_COMPLETED";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_CREATE_COMPLETED" );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, &itemElem, 1 );

			char szMessage[512] = {0,};
			FLSPrintf( szMessage, _countof( szMessage ), prj.GetText( TID_EVE_REAPITEM ), itemElem.GetProp()->szName );
			pUser->AddText( szMessage );			
		}
		else
			pUser->AddDefinedText( TID_GAME_NOT_INVEN_ANGEL, "" );
	}
}

void CDPSrvr::OnKawibawiboStart( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	// chipi_130709 character.inc 파일 조작으로 비사용 교환 메뉴 사용 가능 문제 수정
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_KAWIBAWIBO, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
		return;

	pUser->m_nKawibawiboState = prj.m_MiniGame.Result_Kawibawibo( pUser );
	if( pUser->m_nKawibawiboState == CMiniGame::KAWIBAWIBO_WIN )
	{
		CMiniGame::__KAWIBAWIBO Kawibawibo = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin );
		CMiniGame::__KAWIBAWIBO KawibawiboNext = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin + 1 );
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin, Kawibawibo.dwItemId, Kawibawibo.nItemCount, KawibawiboNext.dwItemId, KawibawiboNext.nItemCount );
	}
	else
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin );
}
void CDPSrvr::OnKawibawiboGetItem( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	prj.m_MiniGame.ResultItem_Kawibawibo( pUser );
}

void CDPSrvr::OnReassembleStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	// chipi_130709 character.inc 파일 조작으로 비사용 교환 메뉴 사용 가능 문제 수정
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_REASSEMBLE, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
		return;

	OBJID objItemId[9];

	for( int i=0; i<9; ++i )
		ar >> objItemId[i];

	BOOL nResult = prj.m_MiniGame.Result_Reassemble( pUser, objItemId, 9 );
	if( nResult == TRUE )
		prj.m_MiniGame.ResultItem_Reassemble( pUser );

}

void CDPSrvr::OnReassembleOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	prj.m_MiniGame.OpenWnd_Reassemble( pUser );
}

void CDPSrvr::OnAlphabetOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	prj.m_MiniGame.OpenWnd_Alphabet( pUser );
}

void CDPSrvr::OnAlphabetStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	// chipi_130709 character.inc 파일 조작으로 비사용 교환 메뉴 사용 가능 문제 수정
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_FINDWORD, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
		return;

	int nQuestionID = 0;
	OBJID objItemId[5];

	ar >> nQuestionID;
	for( int i=0; i<5; ++i )
		ar >> objItemId[i];

	int nResult = prj.m_MiniGame.Result_Alphabet( pUser, objItemId, 5, nQuestionID );
	if( nResult != CMiniGame::ALPHABET_FAILED && nResult != CMiniGame::ALPHABET_NOTENOUGH_MONEY )
		prj.m_MiniGame.ResultItem_Alphabet( pUser, nResult );
	else
		pUser->AddAlphabetResult( nResult );
	//	else
}

void CDPSrvr::OnFiveSystemOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	prj.m_MiniGame.OpenWnd_FiveSystem( pUser );
}

void CDPSrvr::OnFiveSystemBet( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	int nBetNum;
	int nBetPenya;

	ar >> nBetNum;
	ar >> nBetPenya;

	int nResult = prj.m_MiniGame.Bet_FiveSystem( pUser, nBetNum, nBetPenya );
	if( nResult == CMiniGame::FIVESYSTEM_NOTENOUGH )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_OVERMAX )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_FAILED )
	{
		pUser->AddFiveSystemResult( nResult );
	}
}

void CDPSrvr::OnFiveSystemStart( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	// chipi_130709 character.inc 파일 조작으로 비사용 교환 메뉴 사용 가능 문제 수정
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_FIVESYSTEM, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
		return;

	int nResult = prj.m_MiniGame.Result_FiveSystem( pUser );

	if( !( prj.m_MiniGame.ResultPenya_FiveSystem( pUser, nResult ) ) )
		pUser->AddFiveSystemResult( CMiniGame::FIVESYSTEM_FAILED );
}	

void CDPSrvr::OnFiveSystemDestroyWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	prj.m_MiniGame.DestroyWnd_FiveSystem( pUser );
}

void CDPSrvr::OnUltimateMakeItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemId[MAX_JEWEL];
	//	for( int i=0; i<MAX_JEWEL; i++ )
	//		ar >> objItemId[i];
	ar.Read( objItemId, sizeof(OBJID) * MAX_JEWEL );

	int nResult = prj.m_UltimateWeapon.MakeItem( pUser, objItemId );
	pUser->AddUltimateMakeItem( nResult );
}

void CDPSrvr::OnUltimateMakeGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemId;
	ar >> objItemId;

	int nNum;
	int nResult = prj.m_UltimateWeapon.MakeGem( pUser, objItemId, nNum );
	pUser->AddUltimateMakeGem( nResult, nNum );
}

void CDPSrvr::OnUltimateTransWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemWeapon;
	OBJID objItemGem1;
	OBJID objItemGem2;

	ar >> objItemWeapon;
	ar >> objItemGem1;
	ar >> objItemGem2;

	int nResult = prj.m_UltimateWeapon.TransWeapon( pUser, objItemWeapon, objItemGem1, objItemGem2 );
	pUser->AddUltimateWeapon( ULTIMATE_TRANSWEAPON, nResult );
}

void CDPSrvr::OnUltimateSetGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.SetGem( pUser, objItemWeapon, objItemGem );
	pUser->AddUltimateWeapon( ULTIMATE_SETGEM, nResult );
}

void CDPSrvr::OnUltimateRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.RemoveGem( pUser, objItemWeapon, objItemGem );

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_SUCCESS, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_FAILED, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1715, XI_INT_FAIL ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_REMOVEGEM, nResult );
}

void CDPSrvr::OnUltimateEnchantWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.EnchantWeapon( pUser, objItemWeapon, objItemGem );

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_FAIL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1715, XI_INT_FAIL ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_ENCHANTWEAPON, nResult );
}

// void CDPSrvr::OnExchange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
// {
// 	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) == FALSE )
// 		return;
// 
// 	int nMMIid;
// 	int nListNum;
// 
// 	ar >> nMMIid;
// 	ar >> nListNum;
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	//	BEGIN100708
// 	if( nListNum < 0 )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "-- CDPSrvr::OnExchange" ) );
// 
// 		return;
// 	}
// 	//	END100708
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	g_pExchangeEvent->OnDoExchange( pUser, nMMIid, nListNum );
// }

void CDPSrvr::InviteParty( u_long uLeaderid, u_long uMemberid, BOOL bTroup )
{
	FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( uLeaderid );
	FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( uMemberid );
	
	if( IsValidObj( pLeader ) == FALSE )
	{
		return;
	}

	if( IsValidObj( pMember ) == FALSE )
	{
//		pLeader->AddPartyRequestCancel( uLeaderid, uMemberid, 4 );
		pLeader->AddDefinedText( TID_GAME_PARTYINVATEOTHERSVR );
		return;
	}

	const DWORD dwResult	= g_PartyMng.CanInviteParty( uLeaderid, uMemberid );
	if( dwResult == FSC_PARTY_INVITE_SUCCESS )
	{
		pMember->AddPartyRequest( pLeader, pMember, bTroup );
	}
	else
	{
		switch( dwResult )
		{
		case FSC_PARTY_INVITE_MEMBER_HAVE_PARTY:
//			pLeader->AddPartyRequestCancel( uLeaderid, uMemberid, 1 );
			pLeader->AddDefinedText( TID_GAME_PARTYEXISTCHR, "\"%s\"", pMember->GetName() );
			break;

		case FSC_PARTY_INVITE_NOT_LEADER:
			pLeader->AddDefinedText( TID_GAME_PARTYNOINVATE );
			break;

		case FSC_PARTY_INVITE_MEMBER_OVERFLOW:
			pLeader->AddDefinedText( TID_GAME_FULLPARTY3 );
			break;

		case FSC_PARTY_INVITE_CANNOT_WORLD:
			pLeader->AddDefinedText( TID_GAME_GUILDCOMBAT_CANNOT_PARTY );
			break;

		case FSC_PARTY_INVITE_GUILD_COMBAT:
			pLeader->AddDefinedText( TID_GAME_GUILDCOMBAT_CANNOT_PARTY );
			break;

		case FSC_PARTY_INVITE_PVP:
			pLeader->AddDefinedText( TID_GAME_PPVP_ADDPARTY );
			break;

		case FSC_PARTY_INVITE_ATTACK_MODE:
			pLeader->AddDefinedText( TID_GAME_BATTLE_NOTPARTY );
			break;

		default:
			break;
		}
	}



// 	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( uMemberid );
// 	FLWSUser* pLeaderUser = g_xWSUserManager->GetUserByPlayerID( uLeaderid );
// 
// 	if( IsValidObj( pLeaderUser ) && IsValidObj( pUser ) )
// 	{
// 		// 대전장에서는 파티를 할수 없습니다.
// 		CWorld* pWorld = pUser->GetWorld();
// 		if( ( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR ) || pLeaderUser->GetWorld() && pLeaderUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
// 		{			
// 			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
// 			return;
// 		}
// 
// 		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
// 		{
// 			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
// 			return;
// 		}
// 
// 		if( 0 < pUser->m_nDuel ||  0 < pLeaderUser->m_nDuel )
// 		{
// 			return;
// 		}
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		// 이벤트 아레나 극단 초대 불가
// 		if( g_pEventArenaGlobal->IsArenaChannel() )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 극단 초대 불가 User: %s" ), pLeaderUser->GetName() );
// 			return;
// 		}
// 		//////////////////////////////////////////////////////////////////////////
// 
// 		if( pLeaderUser->m_nDuel == 2 )
// 		{			
// 			pLeaderUser->AddDefinedText( TID_GAME_PPVP_ADDPARTY, "" );		// 극단 듀얼중엔 초청 못함다.
// 		} 
// 		else
// 		{
// 			if( 0 < (CMover*)pUser->GetPartyId() )	// 이미 파티가 있을때
// 			{
// 				pLeaderUser->AddPartyRequestCancel( uLeaderid, uMemberid, 1 );
// 			}
// 			else
// 			{
// 				if( pUser->IsAttackMode() )
// 					pLeaderUser->AddDefinedText( TID_GAME_BATTLE_NOTPARTY, "" );
// 				else
// 					pUser->AddPartyRequest( pLeaderUser, pUser, bTroup );
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if( IsValidObj( pLeaderUser ) )
// 			pLeaderUser->AddPartyRequestCancel( uLeaderid, uMemberid, 4 );
// 	}
}

void CDPSrvr::InviteCompany( FLWSUser* pUser, OBJID objid )
{
	if( !IsInviteAbleGuild( pUser ) )
	{
		return;
	}

	if( IsValidObj( pUser ) )
	{
		FLWSUser* pUsertmp	= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild )
			{
				CGuildMember* pMember	= pGuild->GetMember( pUser->m_idPlayer );
				if( !pMember )
				{
					// is not member
					return;
				}
				if( !pGuild->IsCmdCap( pMember->m_nMemberLv, PF_INVITATION ) )
				{
					// have no power
					pUser->AddDefinedText( TID_GAME_GUILDINVAITNOTWARR );
					return;
				}
				CGuild* pGuildtmp	= g_GuildMng.GetGuild( pUsertmp->m_idGuild );
				if( pGuildtmp && pGuildtmp->IsMember( pUsertmp->m_idPlayer ) )
				{
					// is already guild member
					pUser->AddDefinedText( TID_GAME_COMACCEPTHAVECOM, "%s", pUsertmp->GetName( TRUE ) );
				}
				else
				{
					if( 0 < pUsertmp->m_nDuel )
					{
						return;
					}

					if( !pGuild->GetWar() )
					{
						if( pUsertmp->IsAttackMode() )
						{
							pUser->AddDefinedText( TID_GAME_BATTLE_NOTGUILD, "" );
						} 
						else
						{

							pUsertmp->m_idGuild	= 0;
							pUsertmp->AddGuildInvite( pGuild->m_idGuild, pUser->m_idPlayer );
							pUser->AddDefinedText( TID_GAME_COMACCEPTKINGPIN, "%s", pUsertmp->GetName( TRUE ) );
						}
					}
					else
					{
						pUser->AddDefinedText( TID_GAME_GUILDWARNOMEMBER );
					}
				}
			}
		}
	}	
}

BOOL CDPSrvr::IsInviteAbleGuild( FLWSUser* pUser )
{
	CGuild* pGuild = pUser->GetGuild();
	if( !pGuild )
		return FALSE;

	// 길드대전
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		if( g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild ) &&
			g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild )->bRequest )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 1:1 길드대전
	if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
	{
		if( g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser ) != NULL_ID )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 비밀의 방
	if( CSecretRoomMng::GetInstance()->m_nState != SRMNG_CLOSE )
	{
		std::map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); ++it )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && ( pSRCont->GetTenderGuild( pGuild->GetGuildId() ) != NULL_ID ) )
			{
				pUser->AddDefinedText( TID_GAME_SECRETROOM_NOT_INVITATION_GUILD );
				return FALSE;
			}
		}
	}

	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나에서는 길드초대를 할 수 없습니다." ) );
		return FALSE;
	}

	return TRUE;
}


CCommonCtrl* CreateExpBox( FLWSUser* /*pUser*/ )
{
	return NULL;

	//CWorld* pWorld = pUser->GetWorld();

	//if( pWorld == NULL  )
	//	return NULL;

	/////////////////////////////////////////////////////////////////////////
	//// 경험치가 깎이지 않으면 드랍을 안함!!!
	//CMover* pMover = (CMover*)pUser;

	//float fRate = 0.1f, fDecExp = 0.0f;
	//BOOL  bPxpClear = FALSE, bLvDown = FALSE;
	//int   nLevel	= pMover->GetLevel();	
	//pMover->GetDieDecExp( nLevel, fRate, fDecExp, bPxpClear, bLvDown );
	//if( fDecExp )
	//{
	//	pMover->GetDieDecExpRate( fDecExp, 0, FALSE );
	//}

	//// 축복의 두루마리 사용시에는 경험치 상자 만들지 않는다...

	//if( pMover->IsSMMode( SM_REVIVAL ) )
	//	fDecExp = 0.0f;
	///*
	//if( pMover->GetExp1() == 0 )
	//fDecExp = 0.0f;	
	//*/	
	//if( pMover->m_bLastPK || pMover->m_bGuildCombat || pMover->m_bLastDuelParty )		// 무조건 경험치 안깍는다...
	//	fDecExp = 0.0f;			

	//if( fDecExp == 0.0f )
	//	return NULL;

	//CCommonCtrl* pCtrl	= (CCommonCtrl*)CreateObj( D3DDEVICE, OT_CTRL, 46 );

	//if( !pCtrl )
	//	return NULL;
	/////////////////////////////////////////////////////////////////////////

	//pCtrl->m_CtrlElem.m_dwSet    = UA_PLAYER_ID;
	//pCtrl->m_idExpPlayer = pUser->m_idPlayer;

	//EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[pUser->m_nLevel+1].nExp1 * fDecExp );	// 현재레벨의 최대경험치 * 퍼센트

	//if( nDecExp > pMover->GetExp1() )
	//	nDecExp = pMover->GetExp1();

	//pCtrl->m_nExpBox     = (EXPINTEGER)(nDecExp * 0.3f);
	//pCtrl->m_dwDelete    = timeGetTime() + MIN( 30 );

	//pCtrl->SetPos( pUser->GetPos() );

	//pUser->AddDefinedText(TID_GAME_EXPBOX_INFO_MSG, "" );
	////pUser->AddChatText( TID_GAME_EXPBOX_INFO_MSG, "");
	//return pCtrl;
}

void CDPSrvr::OnPetRelease( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( !pUser->HasActivatedSystemPet() )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
		}
		else
		{
			FLItemElem* pItemElem	= pUser->GetPetItem();
			if( pItemElem && pItemElem->m_pPet )
				pUser->PetRelease();
		}
	}
}

void CDPSrvr::OnUsePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		CPet* pPet	= pUser->GetPet();
		if( pPet == NULL )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			return;
		}
		DWORD dwFeedId;		// 먹이 식별자
		int nNum;
		ar >> dwFeedId;
		FLItemElem* pFeed	= static_cast<FLItemElem*>( pUser->GetItemId( dwFeedId ) );
		if( IsUsableItem( pFeed ) == FALSE )
			return;
		if( !pFeed->IsFeed() )
			return;

		nNum	= pFeed->m_nItemNum;

		int nMaxNum	= 0;
		if( pPet->GetLevel() == PL_EGG )
		{
			nMaxNum	= MAX_PET_EGG_EXP - pPet->GetExp();
		}
		else
		{
			nMaxNum	= pPet->GetMaxEnergy() - pPet->GetEnergy();
			nMaxNum	/= 2;	// 먹이 1당 기력 2회복	// 0723
		}

		if( nNum > nMaxNum )
			nNum	= nMaxNum;
		if( nNum == 0 )
			return;

		if( pPet->GetLevel() == PL_EGG )
		{
			pPet->SetExp( pPet->GetExp() + nNum );
			pUser->AddPetSetExp( pPet->GetExp() );
		}
		else
		{
			pPet->SetEnergy( static_cast< WORD >( pPet->GetEnergy() + nNum * 2 ) );	// 먹이 1당 기력 2회복	// 0723
			g_xWSUserManager->AddPetFeed( pUser, pPet->GetEnergy() );
		}
		pUser->UpdateItem( (BYTE)( pFeed->m_dwObjId ), UI_NUM, pFeed->m_nItemNum - nNum );
		pUser->AddDefinedText( TID_GAME_PETFEED_S01, "%d", nNum );

		// log
		FLItemElem* pPetItem	= pUser->GetPetItem();
		g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), nNum, PETLOGTYPE_FEED, pPet );
	}
}

void CDPSrvr::OnMakePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwMaterialId, dwToolId;
		int nNum;

		ar >> dwMaterialId >> dwToolId >> nNum;

		BOOL bTool	= FALSE;
		FLItemElem* pTool	= NULL;
		if( dwToolId != NULL_ID )
		{
			pTool	= (FLItemElem*)pUser->GetItemId( dwToolId );
			if( IsUsableItem( pTool ) )
			{
				if( pTool->m_dwItemId == ITEM_INDEX( 21038, II_SYS_SYS_FEED_MAKER ) )
					bTool	= TRUE;
			}
			if( !bTool )	// error
				return;
		}

		// 펫 테이머에 의한 먹이 제조 시, 펫 테이머와 인접해 있지 않으면 무시
		if( bTool == FALSE && CNpcChecker::GetInstance()->IsCloseNpc( MMI_PET_FOODMILL, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
			return;

		FLItemElem* pMaterial	= (FLItemElem*)pUser->GetItemId( dwMaterialId );
		if( IsUsableItem( pMaterial ) == FALSE )
		{
			// error
			return;
		}
		PT_ITEM_SPEC pProp	= pMaterial->GetProp();
		if( pProp == NULL )
		{
			// error
			return;
		}

		if( pMaterial->IsCharged() )
		{
			pUser->AddDefinedText( TID_GAME_PET_FEED_CHARGED );
			return;
		}

		if( pProp->dwItemKind3 != IK3_GEM )
		{
			pUser->AddDefinedText( TID_GAME_PET_IS_NOT_FEED );
			return;
		}

		if( nNum <= 0 )
			nNum	= 1;
		if( nNum > pMaterial->m_nItemNum )
			nNum	= pMaterial->m_nItemNum;

		if( pUser->m_Inventory.IsEquip( dwMaterialId ) )
		{
			pUser->AddDefinedText( TID_GAME_PET_FEED_EQUIPED );
			return;
		}

		FLItemElem itemElem;
		itemElem.m_dwItemId		= ITEM_INDEX( 21037, II_SYS_SYS_FEED_01 );
		int nTotalFeed	= 0;
		int nPackMax	= itemElem.GetProp()->dwPackMax;
		for( int i = 0; i < nNum; i++ )
		{
			int nFeed	= CPetProperty::GetInstance()->GetFeedEnergy( pProp->dwCost, (int)bTool );
			if( nTotalFeed + nFeed > nPackMax )
			{
				nNum	= i;
				break;
			}
			nTotalFeed	+= nFeed;
		}
		itemElem.m_nItemNum		= nTotalFeed;
		itemElem.m_nHitPoint	= 0;

		int nResult = pUser->CreateItem( &itemElem );
		if( nResult )
		{
			//먹이 만들기 로그 : pMaterial이 없어질 먹이 재료, itemElem.m_nItemNum이 새로 생성된 먹이 량
			LogItemInfo aLogItem;
			//aLogItem.Action = "~";
			//aLogItem.SendName = pUser->GetName();
			//aLogItem.RecvName = "PET_FOOD_MATERIAL_REMOVE";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "~" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "PET_FOOD_MATERIAL_REMOVE" );

			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, pMaterial, nNum );

			//aLogItem.RecvName = "PET_FOOD_CREATE";
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "PET_FOOD_CREATE" );
			OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );

			pUser->UpdateItem( (BYTE)( pMaterial->m_dwObjId ), UI_NUM, pMaterial->m_nItemNum - nNum );
			if( bTool )
				pUser->UpdateItem( (BYTE)( pTool->m_dwObjId ), UI_NUM, pTool->m_nItemNum - 1 );
		}
		pUser->AddPetFoodMill(nResult, itemElem.m_nItemNum);
	}
}

void CDPSrvr::OnPetTamerMistake( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwId;
		ar >> dwId;
		FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 21030, II_SYS_SYS_SCR_PET_TAMER_MISTAKE ) ) // m_dwItemId
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_C && pPet->GetLevel() <= PL_S )
				{
					// 현재 레벨에서 얻은 능력치가 취소되며,
					// 전 단계 레벨, 경험치 100%로 돌아간다.
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소
					pPet->SetLevel( pPet->GetLevel() - 1 );
					pPet->SetExp( MAX_PET_EXP );
					// 기존 버프 제거
					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_LEVEL_DOWN );	// 自
					g_xWSUserManager->AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );

					// log
					FLItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MISTAKE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_C_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
		else
		{
			// error
		}
	}
}

void CDPSrvr::OnPetTamerMiracle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		DWORD dwId;
		ar >> dwId;
		FLItemElem* pItemElem	= (FLItemElem*)pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 21031, II_SYS_SYS_SCR_PET_TAMER_MIRACLE ) )
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_B && pPet->GetLevel() <= PL_S )
				{
					// 현재 레벨과 그 전 레벨에서 얻은 능력치가 취소되며,
					// 다시 랜덤하게 얻게 된다.
					pPet->SetAvailLevel( pPet->GetLevel() - 1, 0 );		// 능력치 취소
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소
					// 임의 능력치 상승
					BYTE nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() - 1 );
					pPet->SetAvailLevel( pPet->GetLevel() - 1, nAvailLevel );
					nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() );
					pPet->SetAvailLevel( pPet->GetLevel(), nAvailLevel );

					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_GET_AVAIL );	// 自	// PF_PET_GET_AVAIL 
					g_xWSUserManager->AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );

					// log
					FLItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MIRACLE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_B_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
		else
		{
			// error
		}
	}
}

void CDPSrvr::OnFeedPocketInactive( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsDie() == TRUE )
		{
			return;
		}

		if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) ) )
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) );
	}
}

void CDPSrvr::OnModifyStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nStrCount, nStaCount, nDexCount, nIntCount;
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	ar >> nStrCount >> nStaCount >> nDexCount >> nIntCount;

	const int remainStatPoint = pUser->m_Stat.GetRemainStatPoint();

	if( ( nStrCount < 0 || nStaCount < 0 || nDexCount < 0 || nIntCount < 0 )
	|| ( remainStatPoint < nStrCount || remainStatPoint < nStaCount || remainStatPoint < nDexCount || remainStatPoint < nIntCount ) ) {
//	|| (nStrCount + nStaCount + nDexCount + nIntCount <= 0)) //양수 검사 및 합이 0이하일 경우 중단.
		return;
	}

	const __int64 sumModifyStatPoint = static_cast<__int64>(nStrCount) + static_cast<__int64>(nStaCount) + static_cast<__int64>(nDexCount) + static_cast<__int64>(nIntCount);
	if( sumModifyStatPoint <= 0 || remainStatPoint < sumModifyStatPoint ) {
		return;
	}

	if( remainStatPoint >= (nStrCount + nStaCount + nDexCount + nIntCount) )
	{
		pUser->m_Stat.SetOriginStr( pUser->m_Stat.GetOriginStr() + nStrCount );
		pUser->m_Stat.SetOriginSta( pUser->m_Stat.GetOriginSta() + nStaCount );
		pUser->m_Stat.SetOriginDex( pUser->m_Stat.GetOriginDex() + nDexCount );
		pUser->m_Stat.SetOriginInt( pUser->m_Stat.GetOriginInt() + nIntCount );

		pUser->m_Stat.SetRemainStatPoint( remainStatPoint - ( nStrCount + nStaCount + nDexCount + nIntCount ) );
		g_xWSUserManager->AddSetState( pUser );
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();//09.02.12
		g_xWSUserManager->AddHonorTitleChange( pUser, pUser->m_nHonor);

		g_dpDBClient.SendLogLevelUp( pUser, 2 );
	}
}	

void CDPSrvr::OnLegendSkillStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	OBJID objItemId[5] = { 0, };
	for( int i = 0; i < 5; ++i )
	{
		ar >> objItemId[i];
	}

	if( pUser->IsLegendClass() == FALSE )
	{
		return;
	}

	for( int i = 0; i < MAX_SKILL_JOB; ++i )
	{				
		LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
		if( lpSkill->dwSkill != NULL_ID )
		{
			SkillProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );
			if( pSkillProp == NULL )
				continue;
			if( pSkillProp->dwSkillKind1 != JTYPE_HERO )
				continue;

			if( lpSkill->dwLevel > 4 )	// 뭐야?
			{
				pUser->AddLegendSkillResult( -1 );
				return;
			}
		}
	}

	FLItemElem* pItemElem[5] = { NULL, };
	// 일치하는지 검사 (인벤토리에서 검사)
	for( int i = 0; i < 5; ++i )
	{
		pItemElem[i]	= (FLItemElem*)pUser->m_Inventory.GetAtId( objItemId[i] );
		if( IsUsableItem( pItemElem[i] ) == FALSE )
			return;
	}

	if( pItemElem[0]->m_dwItemId != ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
		pItemElem[1]->m_dwItemId != ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
		pItemElem[2]->m_dwItemId != ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
		pItemElem[3]->m_dwItemId != ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) ||
		pItemElem[4]->m_dwItemId != ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ ) )
	{
		return;
	}

	// 모두 일치하면 보석 아이템 삭제
	for( int i = 0; i < 5 ; ++i )
	{
		//////////////////////////////////////////////////////////////////////////
		LogItemInfo aLogItem;
		//aLogItem.Action = "+";
		//aLogItem.SendName = pUser->GetName();
		//aLogItem.RecvName = "LEGENDSKILL_USE";
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "+" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "LEGENDSKILL_USE" );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem( aLogItem, pItemElem[i], 1 );
		//////////////////////////////////////////////////////////////////////////

		pUser->RemoveItem( objItemId[i], 1 );
	}


	if( xRandom( 1000 ) > 766 )	// ???
	{
		for( int i = 0; i < MAX_SKILL_JOB; ++i ) 
		{				
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if( lpSkill && lpSkill->dwSkill != NULL_ID )
			{
				SkillProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );			
				if( pSkillProp == NULL )
					continue;
				if( pSkillProp->dwSkillKind1 != JTYPE_HERO )
					continue;

				lpSkill->dwLevel++;

				g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, 1, (CMover*)pUser, &(pUser->m_aJobSkill[i]) );
			}
		}

		g_xWSUserManager->AddCreateSfxObj( (CMover *)pUser, XI_INDEX( 108, XI_SYS_EXCHAN01 ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->AddDoUseSkillPoint( &(pUser->m_aJobSkill[0]), pUser->m_nSkillPoint );
		pUser->AddLegendSkillResult( TRUE );
	}
	else
	{
		pUser->AddLegendSkillResult( FALSE );
	}
}

void CDPSrvr::OnGC1to1TenderOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SendTenderGuildOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1TenderView( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SendTenderGuildView( pUser );
}

void CDPSrvr::OnGC1to1Tender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	int nPenya;
	ar >> nPenya;

	g_GuildCombat1to1Mng.SetTenderGuild( pUser, nPenya );
}

void CDPSrvr::OnGC1to1CancelTender( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SetCancelTenderGuild( pUser );
}

void CDPSrvr::OnGC1to1FailedTender( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SetFailedTenderGuild( pUser );
}


void CDPSrvr::OnGC1to1MemberLineUpOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SendMemberLineUpOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1MemberLineUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	std::vector<u_long> vecMemberId;
	u_long nTemp;
	int nSize;

	ar >> nSize;

	if( nSize > g_GuildCombat1to1Mng.m_nMaxJoinPlayer )
		return;

	for( int i=0; i<nSize; i++ )
	{
		ar >> nTemp;
		vecMemberId.push_back( nTemp );
	}

	g_GuildCombat1to1Mng.SetMemberLineUp( pUser, vecMemberId );
}

void CDPSrvr::OnGC1to1TeleportToNPC( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	g_GuildCombat1to1Mng.SetTeleportToNPC( pUser );
}

void CDPSrvr::OnGC1to1TeleportToStage( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SetTeleportToStage( pUser );
}

void CDPSrvr::OnQueryStartCollecting( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->IsDisguise() )
		{
			return;
		}
		if( pUser->IsDie() == TRUE )
		{
			return;
		}
		pUser->StartCollecting();
	}
}

void CDPSrvr::OnQueryStopCollecting( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		pUser->StopCollecting();
}


void CDPSrvr::OnQueryGuildBankLogList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	//	bool bMK = FALSE;
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		BYTE byListType;
		ar >> byListType;
		CGuild *pGuild = pUser->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember )
			{
				if( pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER )
					g_dpDBClient.SendQueryGetGuildBankLogList( pUser->m_idPlayer, pGuild->m_idGuild,byListType );
			}
		}
		// 길드가 없거나 길드장이 아니면 신청 불가
	}
}

void CDPSrvr::OnHonorListReq( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		pUser->AddHonorListAck();
	}
}
void CDPSrvr::OnHonorChangeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nChange;
		ar >> nChange;
		if( nChange > -1 && nChange < MAX_HONOR_TITLE )
		{
			int nNeed =  CTitleManager::Instance()->GetNeedCount( nChange,-1);
			if( nNeed < 0)
				return;
			if( pUser->GetHonorTitle(nChange) >= nNeed )
			{
				pUser->m_nHonor = nChange;
					
	
// 명성 버프 테스트
//				pUser->AddBuff( BUFF_EQUIP, ITEM_INDEX( 10490, II_SYS_SYS_SCR_SCUD ), 1, 999999999, 0, BT_TICK );
//				//pDest->AddBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ), 0, dwSkillTime, 0, BT_TICK, pSrc->GetId() );
//				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10490, II_SYS_SYS_SCR_SCUD ) );
//				g_xApplyItemEffect->ApplyParameter( pUser, pUser, pItemProp, TRUE );
				g_xWSUserManager->AddHonorTitleChange( pUser, nChange );
			}
		}
		else if(nChange == -1)
		{
			pUser->m_nHonor = nChange;
// 명성 버프 테스트
//			pUser->RemoveBuff( BUFF_EQUIP, ITEM_INDEX( 10490, II_SYS_SYS_SCR_SCUD ) );
			g_xWSUserManager->AddHonorTitleChange( pUser, nChange );
		}
	}
}

void CDPSrvr::OnSealCharReq( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild != NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANGUILD );
			return;
		}
		if( pUser->IsChaotic() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nSize	= 0;
		for( DWORD i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nSize++;
		}

		if( nSize > 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
			return;
		}
		else
		{
			FLItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 26475, II_SYS_SYS_SCR_SEAL ) );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			else if(pItemElem->m_nItemNum > 1)
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
		}

		if( pUser->GetGold() > 0 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nBankSize	= 0;
		const BYTE byProgramDataSlot = GET_PLAYER_SLOT( pUser->m_nDBDataSlot );

		for( DWORD i=0; i< pUser->m_Bank[byProgramDataSlot].GetMax(); ++i )
		{
			FLItemElem* pItemElem = pUser->m_Bank[byProgramDataSlot].GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nBankSize++;
		}
		if( !pUser->m_Pocket.IsAllClean() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
		if( nBankSize > 0 || pUser->m_dwGoldBank[byProgramDataSlot] > 0)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
		g_dpDBClient.SendQueryGetSealChar( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount);
		//		pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
	}
}
void CDPSrvr::OnSealCharConmReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	OBJID objidSend;
	ar >> objidSend;

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild != NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANGUILD );
			return;
		}
		int nSize	= 0;
		for( DWORD i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nSize++;
		}

		FLItemElem* pItemElemTrue = NULL;
		FLItemElem* pItemElemtmp = NULL;

		if( nSize > 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
			return;
		}
		else
		{
			pItemElemTrue = pUser->m_Inventory.GetAtByItemId( ITEM_INDEX( 26475, II_SYS_SYS_SCR_SEAL ) );
			if( IsUsableItem( pItemElemTrue ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			else if(pItemElemTrue->m_nItemNum > 1)
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			pItemElemtmp = pItemElemTrue;
		}

		if( pUser->GetGold() > 0 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nBankSize	= 0;
		const BYTE byProgramDataSlot = GET_PLAYER_SLOT( pUser->m_nDBDataSlot );

		for( DWORD i=0; i< pUser->m_Bank[byProgramDataSlot].GetMax(); ++i )
		{
			FLItemElem* pItemElem = pUser->m_Bank[byProgramDataSlot].GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nBankSize++;
		}
		if( !pUser->m_Pocket.IsAllClean() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}

		if( nBankSize > 0 || pUser->m_dwGoldBank[byProgramDataSlot] > 0)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}

		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( objidSend );

		if( lpszPlayer == NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			return;
		}


		FLItemElem itemElemSend;
		itemElemSend.m_dwItemId = ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER );
		itemElemSend.m_nItemNum	= 1;
		itemElemSend.SetSerialNumber();

		//		lstrcpy( itemElemSend.m_szItemText, pUser->GetName() );
		memcpy_s( itemElemSend.m_szItemText, sizeof( itemElemSend.m_szItemText ), pUser->GetName(), sizeof(itemElemSend.m_szItemText) );

		itemElemSend.m_nRepairNumber	= static_cast< BYTE >( pUser->GetLevel() );//nlevel
		itemElemSend.m_nHitPoint	= pUser->m_idPlayer;//m_idPlayer
		itemElemSend.m_nRepair	= pUser->GetJob();//njob

		itemElemSend.m_nResistAbilityOption	= pUser->GetRemainGP();//nPOINT

		itemElemSend.SetGeneralPiercingSize( 4 );
		itemElemSend.SetGeneralPiercingItemID( 0, static_cast<DWORD>( pUser->m_Stat.GetSta() ) );		//nSTA
		itemElemSend.SetGeneralPiercingItemID( 1, static_cast<DWORD>( pUser->m_Stat.GetStr() ) );		//nSTR
		itemElemSend.SetGeneralPiercingItemID( 2, static_cast<DWORD>( pUser->m_Stat.GetDex() ) );		//nDEX
		itemElemSend.SetGeneralPiercingItemID( 3, static_cast<DWORD>( pUser->m_Stat.GetInt() ) );		//nINT

		LogItemInfo aLogItem;
		//aLogItem.RecvName = "SEALCHAR";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "SEALCHAR" );
		g_dpDBClient.SendQueryPostMail( objidSend, 0, itemElemSend, 0, itemElemSend.GetProp()->szName, 	(char*)GETTEXT( TID_MMI_SEALCHARITEM ) );

		//aLogItem.Action = "+";
		//aLogItem.SendName = pUser->GetName();
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "+" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );

		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem( aLogItem, &itemElemSend, 1 );


		PT_ITEM_SPEC pItemProp  = pItemElemtmp->GetProp();
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemtmp, pItemProp );		
		//		pItemElemtmp->UseItem();
		OBJID       dwTmpObjId = pItemElemtmp->m_dwObjId;
		pUser->RemoveItem( dwTmpObjId, 1 );

		pUser->ClearAllSMMode();
		pUser->RemoveAllBuff();

		g_dpDBClient.SendQueryGetSealCharConm( pUser->m_idPlayer);

		//		pUser->UpdateItem( pItemElemtmp->m_dwObjId, UI_NUM, pItemElemtmp->m_nItemNum );
		//		g_xWSUserManager->DestroyPlayer( pUser );
		//		g_xWSUserManager->RemoveUser( pUser->m_dwSerial );
		//		g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, pUser->m_idPlayer );
		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
	}
}
void CDPSrvr::OnSealCharGetReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwData;
	

	ar >> dwData ;

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		DWORD dwId;
		dwId = HIWORD( dwData );

		if( pUser->IsUsableState( dwId ) == FALSE )
			return;

		FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
		if( IsUsableItem( pItemElem ) )
		{
			if(pItemElem->m_dwItemId != ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
				return;
			g_dpDBClient.SendQueryGetSealCharGet( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount,dwId);
		}
	}
}


void	CDPSrvr::OnMoveItemOnPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nPocket1, nPocket2;
		DWORD dwItemObjID;
		int nNum;
		ar >> nPocket1 >> dwItemObjID >> nNum >> nPocket2;


		//////////////////////////////////////////////////////////////////////////
		//	BEGIN100708
		if( nPocket1 < -1 || nPocket1 >= MAX_CHARACTER_POCKET )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "nPocket1 is wrong number. [%d]" ), nPocket1 );

			return;
		}

		if( nPocket2 < -1 || nPocket2 >= MAX_CHARACTER_POCKET )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "nPocket2 is wrong number. [%d]" ), nPocket2 );

			return;
		}
		//	END100708
		//////////////////////////////////////////////////////////////////////////

		if( nPocket1 == nPocket2 )
			return;

		// mirchang 091214 - 착용중인 아이템인지 체크
		if( nPocket1 == -1 )	// 원본이 인벤토리인지 검사!
		{
			if( pUser->m_Inventory.IsEquip( dwItemObjID ) )
				return;
		}

		int nExpiration = TRUE;

		if ( nPocket1 >= 0 && nPocket2 < 0 ) 
			nExpiration = FALSE;

		FLItemElem* pItem	= pUser->GetItemId2( nPocket1, dwItemObjID, nExpiration );	// 여기서 휴대가방 만료검사 같이함.
		if( IsUsableItem( pItem ) )
		{
			if( nPocket1 < 0 && pUser->IsUsing( pItem ) )
				return;

			PT_ITEM_SPEC pProp = pItem->GetProp();
			if( pProp == NULL )
			{
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			//	BEGIN100708
			if( nNum > pItem->m_nItemNum || nNum <= 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "m_idPlayer [%d], ItemNumber [%d]" ), pUser->m_idPlayer, nNum );

				return;
			}
			//	END100708
			//////////////////////////////////////////////////////////////////////////

			FLItemElem item;
			item	= *pItem;
			item.m_nItemNum		= nNum;

			if( nPocket2 < 0 )
			{
				if( pUser->m_Inventory.IsFull( &item, nNum ) == TRUE )
				{
					return;
				}
			}
			else
			{
				if( nPocket2 >= MAX_CHARACTER_POCKET )
				{
					return;
				}

				if( pUser->m_Pocket.IsAvailable( nPocket2, FALSE ) == FALSE )
				{
					return;
				}

				if( pUser->m_Pocket.m_kPocket[nPocket2].IsFull( &item, nNum ) == TRUE )
				{
					return;
				}
			}

			// 일부만 가져 오는 것이라면 
// 			if( item.m_nItemNum < pItem->m_nItemNum )
// 			{
// 				item.SetSerialNumber();
// 			}

			// log
			LogItemInfo	log;
			//log.Action	= "m";
			//log.SendName	= pUser->GetName();
			//log.RecvName	= "OnMoveItemOnPocket";

			FLStrcpy( log.Action, _countof( log.Action ), "m" );
			FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
			FLStrcpy( log.RecvName, _countof( log.RecvName ), "OnMoveItemOnPocket-Remove" );
			log.WorldId		= pUser->GetWorld()->GetID();
			log.Gold	= nPocket1;
			log.Gold2	= nPocket2;
			log.nSlot	= dwItemObjID;
			OnLogItem( log, pItem, pItem->m_nItemNum );

			int nReMainItemNum = pItem->m_nItemNum - nNum ;

			pUser->RemoveItem2( dwItemObjID, nNum, nPocket1, nExpiration );
			FLItemElem* pItem2	= pUser->GetItemId2( nPocket1, dwItemObjID, nExpiration );	// 여기서 휴대가방 만료검사 같이함.
			if( pProp->dwPackMax > 1 )
			{
				if( nReMainItemNum == 0 && pItem2 == NULL )
				{
					pUser->CreateItem2( &item, nPocket2 );
				}
				else if ( pItem2 && ( pItem2->m_nItemNum == nReMainItemNum ) )
				{
					pUser->CreateItem2( &item, nPocket2 );
				}
			}
			else
			{
				if ( pItem2 == NULL )
					pUser->CreateItem2( &item, nPocket2 );
			}
			
			FLStrcpy( log.Action, _countof( log.Action ), "m" );
			FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
			FLStrcpy( log.RecvName, _countof( log.RecvName ), "OnMoveItemOnPocket-Create" );
			log.WorldId		= pUser->GetWorld()->GetID();
			log.Gold	= nPocket1;
			log.Gold2	= nPocket2;
			log.nSlot = pItem2 ? pItem2->m_nItemNum : 0;
			log.nSlot1 = nReMainItemNum;
			OnLogItem( log, &item, nNum );
		}
	}
}

void	CDPSrvr::OnAvailPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nPocket;
		DWORD dwItemObjID;
		ar >> nPocket >> dwItemObjID;
		if( nPocket <= 0 && nPocket >= MAX_CHARACTER_POCKET )
			return;
		FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwItemObjID );
		if( IsUsableItem( pItemElem ) )
		{
			PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_POCKET )
			{
				if( !pUser->m_Pocket.IsAvailable( nPocket ) )
				{
					pUser->m_Pocket.SetAttribute( CPocketController::avail, nPocket, pItemProp->dwSkillTime );
					// log
					LogItemInfo	log;
					//log.Action	= "u";
					//log.SendName	= pUser->GetName();
					//log.RecvName	= "OnAvailPocket";
					FLStrcpy( log.Action, _countof( log.Action ), "u" );
					FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
					FLStrcpy( log.RecvName, _countof( log.RecvName ), "OnAvailPocket" );
					log.WorldId		= pUser->GetWorld()->GetID();
					log.Gold	= pUser->GetGold();
					log.Gold2	= pUser->GetGold();
					log.Gold_1	= nPocket;
					OnLogItem( log, pItemElem, 1 );
					pUser->UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum - 1 );
#ifdef __INTERNALSERVER
					pUser->AddPocketView();
#endif	// __INTERNALSERVER
				}
			}
		}
	}
}

// void	CDPSrvr::OnBlessednessCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
// {
// 	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) ) 
// 	{
// 		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BLESSING_CANCEL, pUser->GetWorld(), pUser->GetPos() ) )
// 			return;
// 
// 		int nItem;
// 		ar >> nItem;
// 		FLItemElem* pItem	= (FLItemElem*)pUser->GetItemId( nItem );
// 		if( IsUsableItem( pItem ) )
// 		{
// 			if( g_xRandomOptionProperty->GetRandomOptionKind( pItem ) == CRandomOptionProperty::eBlessing
// 				&& g_xRandomOptionProperty->GetRandomOptionSize( pItem->GetRandomOptItemId() ) > 0 )
// 			{
// 				g_xRandomOptionProperty->InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
// 				pUser->UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );
// 				pUser->AddDiagText( prj.GetText( TID_GAME_BLESSEDNESS_CANCEL_INFO ) );
// 				// log
// 				LogItemInfo	log;
// 				log.Action	= "r";
// 				log.SendName	= pUser->GetName();
// 				log.RecvName	= "OnBlessednessCancel";
// 				log.WorldId		= pUser->GetWorld()->GetID();
// 				log.Gold	= pUser->GetGold();
// 				log.Gold2	= pUser->GetGold();
// 				OnLogItem( log, pItem, 1 );
// 			}
// 			else
// 			{
// 				pUser->AddDefinedText( TID_GAME_BLESSEDNESS_CANCEL );	
// 			}
// 		}		
// 	}
// }

// void	CDPSrvr::OnAwakening( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
// {
// 	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( IsValidObj( pUser ) ) 
// 	{
// 		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_ITEM_AWAKENING, pUser->GetWorld(), pUser->GetPos() ) )
// 			return;
// 
// 		const int	nCost	= 100000;
// 		//		if( pUser->GetGold() < nCost )
// 		if( pUser->CheckUserGold( nCost, false ) == false )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "CheckUserGold - \tPenya:[%d]" ), nCost );
// 			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
// 			return;
// 		}
// 		int	nItem;
// 		ar >> nItem;
// 		FLItemElem* pItem	= pUser->m_Inventory.GetAtId( nItem );
// 		if( IsUsableItem( pItem ) )
// 		{
// 			int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItem );
// 
// 			if( nRandomOptionKind != CRandomOptionProperty::eAwakening )
// 			{
// 				pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
// 				return;
// 			}
// 			if( g_xRandomOptionProperty->GetRandomOptionSize( pItem->GetRandomOptItemId() ) > 0 )
// 			{
// 				pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
// 				return;
// 			}
// 			g_xRandomOptionProperty->InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
// 			g_xRandomOptionProperty->GenRandomOption( pItem->GetRandomOptItemIdPtr(), nRandomOptionKind, pItem->GetProp()->dwParts );
// 			pUser->UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );
// 			pUser->AddGold( -nCost );
// 			pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );
// 			// log
// 			LogItemInfo	log;
// 			log.Action	= "r";
// 			log.SendName	= pUser->GetName();
// 			log.RecvName	= "OnAwakening";
// 			log.WorldId		= pUser->GetWorld()->GetID();
// 			log.Gold	= pUser->GetGold() + nCost;
// 			log.Gold2	= pUser->GetGold();
// 			log.Gold_1	= -nCost;
// 			OnLogItem( log, pItem, 1 );
// 		}
// 	}
// }

void	CDPSrvr::OnNPCBuff( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CHAR	m_szKey[64];
		ar.ReadString(m_szKey, _countof( m_szKey ));
		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );

		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc(MMI_NPC_BUFF, pUser->GetWorld(), pUser->GetPos() ) )
				return;

			std::vector<NPC_BUFF_SKILL> vecNPCBuff = lpChar->m_vecNPCBuffSkill;
			for( int i=0; i<(int)( vecNPCBuff.size() ); i++ )
			{
				if( pUser->GetLevel() >= vecNPCBuff[i].nMinPlayerLV && pUser->GetLevel() <= vecNPCBuff[i].nMaxPlayerLV )
				{
					SkillProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
					{
						if( vecNPCBuff[i].dwSkillLV < MIN_SKILL_USE_LEVEL || vecNPCBuff[i].dwSkillLV > pSkillProp->dwExpertMax )
							continue;

						if(	( pSkillProp->dwID == SKILL_INDEX( 317, SI_GEN_EVE_QUICKSTEP ) && pUser->HasBuff(BUFF_SKILL, SKILL_INDEX( 114, SI_ASS_CHEER_QUICKSTEP )) )
							|| ( pSkillProp->dwID == SKILL_INDEX( 318, SI_GEN_EVE_HASTE ) && pUser->HasBuff(BUFF_SKILL, SKILL_INDEX( 20, SI_ASS_CHEER_HASTE )) )
							|| ( pSkillProp->dwID == SKILL_INDEX( 319, SI_GEN_EVE_HEAPUP ) && pUser->HasBuff(BUFF_SKILL, SKILL_INDEX( 49, SI_ASS_CHEER_HEAPUP )) )
							|| ( pSkillProp->dwID == SKILL_INDEX( 320, SI_GEN_EVE_ACCURACY ) && pUser->HasBuff(BUFF_SKILL, SKILL_INDEX( 116, SI_ASS_CHEER_ACCURACY )) ) )
						{
							pUser->AddDefinedText( TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName );
							continue;
						}

						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine + vecNPCBuff[i].dwSkillLV - 1 );
						if( pAddSkillProp )
						{
							// skill property를 수정하기 때문에 백업해두었다가
							// 스킬 시전후 restore시킨다.
							DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
							pSkillProp->dwReferTarget1 = NULL_ID;
							DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
							pSkillProp->dwReferTarget2 = NULL_ID;
							DWORD dwSkillTimeBackup = pAddSkillProp->dwSkillTime;
							pAddSkillProp->dwSkillTime = vecNPCBuff[i].dwSkillTime;

							g_cSkillSystem->DoApplySkill( pUser, pUser, pSkillProp, pAddSkillProp );
							g_xWSUserManager->AddDoApplySkill( pUser, pUser->GetId(), vecNPCBuff[i].dwSkillID, vecNPCBuff[i].dwSkillLV );

							pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
							pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;
						}
					}
				}
				else
				{
					SkillProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
						pUser->AddDefinedText( TID_GAME_NPCBUFF_LEVELLIMIT, "%d %d \"%s\"", vecNPCBuff[i].nMinPlayerLV, vecNPCBuff[i].nMaxPlayerLV, pSkillProp->szName );
				}
			}
		}
	}
}

void	CDPSrvr::OnArenaEnter( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	// 아레나 지역 입장
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsBaseJob() )	// 1차 전직을 완료한 유저만 가능
			return;
		pUser->SetMarkingPos();
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_ARENA, D3DXVECTOR3( 540.0F, 140.0F, 485.0F ), REPLACE_NORMAL, nDefaultLayer );
	}
}

void	CDPSrvr::OnArenaExit( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->REPLACE( g_uIdofMulti, pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_NORMAL, nTempLayer );
}

// 펫을 조각으로 교환
void	CDPSrvr::OnQuePetResurrection( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		DWORD dwItemObjID;
		ar >> dwItemObjID;
		FLItemElem*	pItem	= (FLItemElem*)pUser->GetItemId( dwItemObjID );
		if( !IsUsableItem( pItem ) )
			return;

		CPet* pPet	= pItem->m_pPet;
		if( !pPet || pUser->GetPetId() == pItem->m_dwObjId || pItem->IsFlag( FLItemElem::expired ) )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		BYTE nLevel		= pPet->GetLevel();
		if( nLevel < PL_B || nLevel > PL_S )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		FLItemElem itemElem;
		itemElem.m_nItemNum	= 1;
		switch( nLevel )
		{
		case PL_B:	itemElem.m_dwItemId		= ITEM_INDEX( 26521, II_SYS_SYS_QUE_PETRESURRECTION01_B );	break;
		case PL_A:	itemElem.m_dwItemId		= ITEM_INDEX( 26520, II_SYS_SYS_QUE_PETRESURRECTION01_A );	break;
		case PL_S:	itemElem.m_dwItemId		= ITEM_INDEX( 26519, II_SYS_SYS_QUE_PETRESURRECTION01_S );	break;
		}

		if( pUser->m_Inventory.GetEmptyCountByItemId( itemElem.m_dwItemId ) <= 0 )
		{
			pUser->AddDefinedText( TID_GAME_LACKSPACE );
			return;
		}

		// log
		LogItemInfo	log;
		//log.Action	= "x";
		//log.SendName	= pUser->GetName();
		//log.RecvName	= "OnQuePetResurrection";
		FLStrcpy( log.Action, _countof( log.Action ), "x" );
		FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
		FLStrcpy( log.RecvName, _countof( log.RecvName ), "OnQuePetResurrection" );
		OnLogItem( log, pItem, 1 );

		pUser->RemoveItem( dwItemObjID, 1 );
		pUser->CreateItem( &itemElem );
		pUser->AddQuePetResurrectionResult( TRUE );
	}
}

void	CDPSrvr::OnSecretRoomTenderOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	int nPenya;
	ar >> nPenya;

	if( nPenya <= 0 )
		return;

	CSecretRoomMng::GetInstance()->SetTender( pUser, nPenya );
}

void	CDPSrvr::OnSecretRoomTenderCancelReturn( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderCancelReturn( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetLineUpOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	set<DWORD> checker;
	std::vector<DWORD> vecLineUpMember;
	int nSize;
	ar >> nSize;

	if( nSize > CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum )
		return;

	for( int i=0; i<nSize; i++ )
	{
		DWORD dwIdPlayer;
		ar >> dwIdPlayer;

		// 유효한 Player ID 인가?
		PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( dwIdPlayer );
		if( !pData )
			return;

		// ID 중복체크		
		if( !checker.insert( dwIdPlayer ).second )
			return;

		vecLineUpMember.push_back( dwIdPlayer );
	}

	CSecretRoomMng::GetInstance()->SetLineUp( pUser, vecLineUpMember );
}

void	CDPSrvr::OnSecretRoomEntrance( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTeleportSecretRoom( pUser );
}

void	CDPSrvr::OnSecretRoomTeleportToNPC( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
	pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos( pUser ), REPLACE_NORMAL, nDefaultLayer );
}

void CDPSrvr::OnSecretRoomTenderView( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->GetTenderView( pUser );
}

void CDPSrvr::OnTeleportSecretRoomDungeon( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_SECRET_ENTRANCE_1, pUser->GetWorld(), pUser->GetPos() ) )
		return;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( CTax::GetInstance()->GetContinent( pUser ) );
	if( pTaxInfo && pUser->GetGuild() && (pTaxInfo->dwCurrPlayerID == pUser->GetGuild()->GetGuildId()) )
	{
		pUser->SetMarkingPos();
		pUser->SetAngle( 180.0f );
		int nRandx = xRandom(4) - 2;
		int nRandz = xRandom(4) - 2;
		pUser->REPLACE( g_uIdofMulti, WI_DUNGEON_SECRET_0, D3DXVECTOR3( (float)( 295 + nRandx ), 102.0f, (float)( 530 + nRandz ) ), REPLACE_NORMAL, nDefaultLayer );
	}
	else
		pUser->AddDefinedText( TID_GAME_SECRETROOM_NOENTRANCE_1 );
}

void CDPSrvr::OnElectionAddDeposit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		__int64 iTotal;
		ar >> iTotal;

		__int64 iDeposit;
		int nRet	= election::AddDepositRequirements( pUser, iTotal, iDeposit );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -static_cast<int>( iDeposit ) );
		g_dpDBClient.SendElectionAddDeposit( pUser->m_idPlayer, iDeposit );

		PutPenyaLog( pUser, "d", "DEPOSIT", static_cast<int>( iDeposit ) );
	}
}

void CDPSrvr::PutItemLog( FLWSUser* pUser, const char* szAction, const char* szContext, FLItemElem* pItem, int nNum )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	LogItemInfo	log;
	//log.Action		= szAction;
	//log.SendName	= pUser->GetName();
	//log.RecvName	= szContext;
	FLStrcpy( log.Action, _countof( log.Action ), szAction );
	FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
	FLStrcpy( log.RecvName, _countof( log.RecvName ), szContext );
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	log.Gold		= pUser->GetGold();
	log.Gold2		= pUser->GetGold();

	if( nNum == 0 && pItem != NULL )
	{
		nNum		= pItem->m_nItemNum;
	}

	OnLogItem( log, pItem, nNum );
}

void CDPSrvr::PutPenyaLog( FLWSUser* pUser, const char* szAction, const char* szContext,  int nCost )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	// 모든 결과 처리 후 호출되어야 함
	LogItemInfo	log;
	//log.Action		= szAction;
	//log.SendName	= pUser->GetName();
	//log.RecvName	= szContext;
	FLStrcpy( log.Action, _countof( log.Action ), szAction );
	FLStrcpy( log.SendName, _countof( log.SendName ), pUser->GetName() );
	FLStrcpy( log.RecvName, _countof( log.RecvName ), szContext );
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	//log.ItemName	= "SEED";
	FLSPrintf( log.kLogItem.szItemName, _countof( log.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
	log.Gold		= pUser->GetGold() + nCost;
	log.Gold2		= pUser->GetGold();
	log.Gold_1		= -nCost;

	OnLogItem( log );
}

void	CDPSrvr::SendPutItemLog( FLWSUser* pUser, const TCHAR* pszAction, const TCHAR* pszSendName, const TCHAR* pszRecvName, FLItemElem* pItemElem, const int nQuantity )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	if( pItemElem == NULL )
	{
		FLASSERT( 0 );
		return;
	}

	LogItemInfo	kLogItem;
	FLStrcpy( kLogItem.Action, _countof( kLogItem.Action ), pszAction );
	FLStrcpy( kLogItem.SendName, _countof( kLogItem.SendName ), pszSendName );
	FLStrcpy( kLogItem.RecvName, _countof( kLogItem.RecvName ), pszRecvName );
	kLogItem.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	kLogItem.Gold			= pUser->GetGold();
	kLogItem.Gold2			= pUser->GetGold();

	OnLogItem( kLogItem, pItemElem, nQuantity );
}

void	CDPSrvr::SendPutPenyaLog( FLWSUser* pUser, const TCHAR* pszAction, const TCHAR* pszSendName, const TCHAR* pszRecvName, const int nPenya )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	LogItemInfo	kLogItem;
	FLStrcpy( kLogItem.Action, _countof( kLogItem.Action ), pszAction );
	FLStrcpy( kLogItem.SendName, _countof( kLogItem.SendName ), pszSendName );
	FLStrcpy( kLogItem.RecvName, _countof( kLogItem.RecvName ), pszRecvName );
	FLSPrintf( kLogItem.kLogItem.szItemName, _countof( kLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
	
	kLogItem.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	kLogItem.Gold			= pUser->GetGold() + nPenya;
	kLogItem.Gold2			= pUser->GetGold();
	kLogItem.Gold_1			= -nPenya;

	OnLogItem( kLogItem );
}

void CDPSrvr::OnElectionSetPledge( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	ar.ReadString( szPledge, _countof( szPledge ) );
	if( strlen( szPledge ) == 0 )
		return;

	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nCost;
		int nRet	= election::SetPledgeRequirements( pUser, nCost );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -nCost );
		g_dpDBClient.SendElectionSetPledge( pUser->m_idPlayer, szPledge );

		PutPenyaLog( pUser, "p", "PLEDGE", nCost );
	}
}

void CDPSrvr::OnElectionIncVote( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		u_long idPlayer;
		ar >> idPlayer;

		int nRet	= election::IncVoteRequirements( pUser, idPlayer );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		g_dpDBClient.SendElectionIncVote( idPlayer, pUser->m_idPlayer );
	}
}

void CDPSrvr::OnLEventCreate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int iEEvent, iIEvent;
		ar >> iEEvent >> iIEvent;
		int nRet	= lordevent::CreateRequirements( pUser, iEEvent, iIEvent );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();

		int nPerin	= pUser->RemoveTotalGold( pEvent->GetCost( iEEvent, iIEvent ) );
		char szContext[100]		= { 0,};
		FLSPrintf( szContext, _countof( szContext ), "OnLEventCreate: cost: %d(perin), %I64d(penya)", nPerin, pEvent->GetCost( iEEvent, iIEvent ) - ( nPerin * PERIN_VALUE ) );
		PutPenyaLog( pUser, "e", szContext, 0 );

		g_dpDBClient.SendLEventCreate( pUser->m_idPlayer, iEEvent, iIEvent );
	}
}

void CDPSrvr::OnLordSkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nSkill = 0;
		char szTarget[MAX_PLAYER]	= { 0, };
		ar >> nSkill;
		ar.ReadString( szTarget, _countof( szTarget ) );

		u_long idTarget = 0;
		int nRet	= lordskill::UseRequirements( pUser, szTarget, nSkill, idTarget );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}

		g_dpDBClient.SendLordSkillUse( pUser->m_idPlayer, idTarget, nSkill );
	}
}

void CDPSrvr::OnSetTaxRate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	BYTE nCont;
	int nSalesTaxRate, nPurchaseTaxRate;
	ar >> nCont;
	ar >> nSalesTaxRate >> nPurchaseTaxRate;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
	if( pTaxInfo && pUser->m_idGuild == pTaxInfo->dwNextPlayerID && pUser->IsGuildMaster() )
		CTax::GetInstance()->SetNextTaxRate( nCont, nSalesTaxRate, nPurchaseTaxRate );
	else
		FLERROR_LOG( PROGRAM_NAME, _T( "[%s] User Is Not Next Win Guild Master!!!" ), pUser->GetName() );
}

void CDPSrvr::OnTeleportToHeavenTower( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_HEAVEN_TOWER, pUser->GetWorld(), pUser->GetPos() ) )
		return;

	int nFloor;
	ar >> nFloor;

	int nCost = 0;
	DWORD dwWorldId = NULL_ID;
	float fAngle = 0.0f;
	D3DXVECTOR3 vPos;

	switch( nFloor )
	{
	case 1 :	// 1층
		nCost = 10000;	dwWorldId = WI_WORLD_HEAVEN01; vPos = D3DXVECTOR3( 253, 102, 78 ); fAngle = 183.0f;
		break;
	case 2 :	// 2층
		nCost = 30000;	dwWorldId = WI_WORLD_HEAVEN02; vPos = D3DXVECTOR3( 251, 102, 95 ); fAngle = 183.0f;
		break;
	case 3 :	// 3층
		nCost = 50000;	dwWorldId = WI_WORLD_HEAVEN03; vPos = D3DXVECTOR3( 264, 102, 227 ); fAngle = 183.0f;
		break;
	case 4 :	// 4층
		nCost = 70000;	dwWorldId = WI_WORLD_HEAVEN04; vPos = D3DXVECTOR3( 253, 102, 86 ); fAngle = 174.0f;
		break;
	case 5 :	// 5층
		nCost = 100000;	dwWorldId = WI_WORLD_HEAVEN05; vPos = D3DXVECTOR3( 218, 102, 101); fAngle = 176.0f;
		break;

	default :
		FLERROR_LOG( PROGRAM_NAME, _T( "잘못된 층 : %d, Name = %s" ), nFloor, pUser->GetName() );
		return;
	}

	//	if( pUser->GetGold() < nCost )
	if( pUser->CheckUserGold( nCost, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

	BYTE nCont = CTax::GetInstance()->GetContinent( pUser );
	// 해당 층으로 텔레포트 -> 실패시 그냥 리턴...
	if( pUser->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nDefaultLayer ) )
	{
		pUser->AddGold( -nCost );
		pUser->SetAngle( fAngle );
		__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
		if( pTaxInfo && pTaxInfo->dwCurrPlayerID != NULL_PLAYER_ID )
			CTax::GetInstance()->AddTax( nCont, nCost, TAX_ADMISSION );
		CString strFloor;
		strFloor.Format( "HEAVEN_TOWER_%2d", nFloor );
		PutPenyaLog( pUser, "h", strFloor, nCost );
	}
	else
		return;
}

void CDPSrvr::OnTransformItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{	// 알변환
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CTransformStuff stuff;
	stuff.Serialize( ar );	// 재료를 수신

	// 변환 번호로부터 변환 함수를 결정한다.
	ITransformer* pTransformer	= ITransformer::Transformer( stuff.GetTransform() );

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( pTransformer == NULL )
	{
		return;
	}
	//	END100708
	//////////////////////////////////////////////////////////////////////////

	pTransformer->Transform( pUser, stuff );	// 변환
}

void CDPSrvr::OnTutorialState( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nState;
	char szState[64]	= { 0,};
	char szOut[64]	= { 0,};
	ar >> nState;
	ar.ReadString( szState, _countof( szState ) );
	MakeTutorialStateString( szOut, _countof( szOut ), nState, pUser->GetName() );
	if( lstrcmp( szOut, szState ) == 0 )
	{
		pUser->SetTutorialState( nState );
		pUser->AddSetTutorialState();
	}
}

// 픽업펫 각성 취소 메뉴 선택 핸들러
// void CDPSrvr::OnPickupPetAwakeningCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
// {
// 	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
// 	if( !IsValidObj( pUser ) )
// 		return;
// 	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_PET_AWAK_CANCEL, pUser->GetWorld(), pUser->GetPos() ) )
// 		return;
// 
// 	DWORD dwItem;
// 	ar >> dwItem;
// 	FLItemElem* pItem	= (FLItemElem*)pUser->GetItemId( dwItem );
// 	if( !IsUsableItem( pItem ) )
// 		return;
// 
// 	if( DoUseItemTarget_InitializeRandomOption( pUser, pItem, CRandomOptionProperty::eEatPet,
// 		TID_GAME_PICKUP_PET_AWAKENING_CANCEL_S001, TID_GAME_PICKUP_PET_AWAKENING_CANCEL_E001,
// 		"k", "PPAC" ) )
// 	{
// 		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
// 		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
// 			g_xWSUserManager->AddCreateSfxObj( pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
// 	}
// }

void CDPSrvr::OnDoUseItemInput( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	DWORD dwData;
	ar >> dwData;
	char szInput[MAX_INPUT_LEN]		= { 0,};
	ar.ReadString( szInput, _countof( szInput ) );
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	pUser->SetInput( szInput );

	g_pItemUsing->OnDoUseItem( pUser, dwData, 0, -1 );

	pUser->ResetInput();
}

void CDPSrvr::OnClearPetName( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CPet* pPet	= pUser->GetPet();
		if( !pPet )
		{
			pUser->AddDefinedText( TID_GAME_NAME_PET_E00 );
			return;
		}
		pPet->SetName( "" );
		g_xWSUserManager->AddSetPetName( pUser, pPet->GetName() );
	}
}

void CDPSrvr::OnRainbowRacePrevRankingOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRacePrevRankingOpenWnd();
}

void CDPSrvr::OnRainbowRaceApplicationOpenWnd( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRaceApplicationOpenWnd();
}
void CDPSrvr::OnRainbowRaceApplication( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CRainbowRaceMng::GetInstance()->SetApplicationUser( pUser );
}
void CDPSrvr::OnRainbowRaceMiniGamePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	BOOL bExt;
	ar >> bExt;

	__MINIGAME_PACKET* pMiniGamePacket;
	if( bExt )	pMiniGamePacket = new __MINIGAME_EXT_PACKET;
	else		pMiniGamePacket = new __MINIGAME_PACKET;

	pMiniGamePacket->Serialize( ar );
	CRainbowRaceMng::GetInstance()->OnMiniGamePacket( pUser, pMiniGamePacket );
	SAFE_DELETE( pMiniGamePacket );
}

void CDPSrvr::OnRainbowRaceReqFinish( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CRainbowRaceMng::GetInstance()->SetRanking( pUser );
}

void CDPSrvr::OnHousingSetupFurniture( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	HOUSINGINFO housingInfo;
	housingInfo.Serialize( ar );

	// 플레이어가 방에 있어야 하고 자신의 레이어에 들어가 있는 경우만 가능...
	CHousingMng::GetInstance()->ReqSetupFurniture( pUser, housingInfo );
}

void CDPSrvr::OnHousingSetVisitAllow( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	BOOL  bAllow;

	ar >> dwPlayerId >> bAllow;

	CHousingMng::GetInstance()->ReqSetAllowVisit( pUser, dwPlayerId, bAllow );
}

void CDPSrvr::OnHousingVisitRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousingMng::GetInstance()->SetVisitRoom( pUser, dwPlayerId );
}

void CDPSrvr::OnHousingVisitableList( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->OnReqVisitableList( pUser );
}

void CDPSrvr::OnHousingGoOut( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->GoOut( pUser );
}

void CDPSrvr::OnReqQuestNPCPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	char szCharKey[64] = {0,};
	ar.ReadString( szCharKey, _countof( szCharKey ) );

	LPCHARACTER lpChar = prj.GetCharacter( szCharKey );
	if( lpChar )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == lpChar->m_dwWorldId )
			pUser->AddNPCPos( lpChar->m_vPos );
		else
			pUser->AddDefinedText( TID_GAME_QUESTINFO_FAIL );
	}
}

void CDPSrvr::OnPropose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szPlayer[MAX_PLAYER]	= { 0,};
		ar.ReadString( szPlayer, _countof( szPlayer ) );
		CCoupleHelper::Instance()->OnPropose( pUser, szPlayer );
	}
}

void CDPSrvr::OnRefuse( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnRefuse( pUser );
}

void CDPSrvr::OnCouple( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnCouple( pUser );
}

void CDPSrvr::OnDecouple( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnDecouple( pUser );
}

void CDPSrvr::OnMapKey( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szFileName[33] = {0,}, szMapKey[33] = {0,};
		ar.ReadString( szFileName, _countof( szFileName ) );
		ar.ReadString( szMapKey, _countof( szMapKey ) );
		g_WorldMng.CheckMapKey( pUser, szFileName, szMapKey );
	}
}

void CDPSrvr::OnQuizEventEntrance( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( !IsValidObj( pUser ) )
		return;

	int nResult;
	nResult = CQuiz::GetInstance()->EntranceQuizEvent( pUser );
	if( nResult > 0 )
		pUser->AddDefinedText( nResult );
}
void CDPSrvr::OnQuizEventTeleport( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( !IsValidObj( pUser ) )
		return;

	int nZone;
	ar >> nZone;

	int nResult;
	nResult = CQuiz::GetInstance()->TeleportToQuizEvent( pUser, nZone );
	if( nResult > 0 )
		pUser->AddDefinedText( nResult );
}

void CDPSrvr::OnRemoveVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos;
	ar >> nPos;

	CItemUpgrade::GetInstance()->RemovePetVisItem( pUser, nPos );
}

void CDPSrvr::OnSwapVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos1, nPos2;
	ar >> nPos1 >> nPos2;

	CItemUpgrade::GetInstance()->SwapVis( pUser, nPos1, nPos2 );
}

void CDPSrvr::OnBuyGuildHouse( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->ReqBuyGuildHouse( pUser );
}

void CDPSrvr::OnGuildHousePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> nPacketType >> nIndex;
	gfi.Serialize( ar );

	if( nPacketType == GUILDHOUSE_PCKTTYPE_LISTUP )
		return;

	GuildHouseMng->SendWorldToDatabase( pUser, nPacketType, gfi, nIndex );
}

void CDPSrvr::OnGuildHouseEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwGHIndex = NULL_ID;

#ifdef __GUILD_HOUSE_MIDDLE

	ar >> dwGHIndex;

#endif // __GUILD_HOUSE_MIDDLE

	GuildHouseMng->EnteranceGuildHouse( pUser, NULL_ID, dwGHIndex );
}

void CDPSrvr::OnGuildHouseGoOut( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->GoOutGuildHouse( pUser );
}

void CDPSrvr::OnTeleporterReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser	=	g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CHAR	m_szKey[64];
		int		nIndex;
		ar.ReadString(m_szKey, _countof( m_szKey ));
		ar >> nIndex;

		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );
		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TELEPORTER, pUser->GetWorld(), pUser->GetPos() ) )
				return;

			if( (int)( lpChar->m_vecTeleportPos.size() ) <= nIndex )
				return;

			if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer ) == TRUE )
			{
				pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
				return;
			}

			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, lpChar->m_vecTeleportPos[nIndex], REPLACE_NORMAL, nDefaultLayer );
		}
	}
}

void CDPSrvr::OnCheckedQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nQuestid;
		BOOL bCheck;
		ar >> nQuestid >> bCheck;
		if( bCheck )
		{
			if( pUser->m_nCheckedQuestSize >= MAX_CHECKED_QUEST )
				return;

			for( size_t i = 0; i < pUser->m_nCheckedQuestSize; ++i )
			{
				if( pUser->m_aCheckedQuest[ i ] == nQuestid )
				{
					for( size_t j = i; j < pUser->m_nCheckedQuestSize -1; ++j )
						pUser->m_aCheckedQuest[ j ] = pUser->m_aCheckedQuest[ j+1 ];
					pUser->m_aCheckedQuest[ --pUser->m_nCheckedQuestSize ] = 0;
					break;
				}
			}
			pUser->m_aCheckedQuest[ pUser->m_nCheckedQuestSize++ ] = static_cast< WORD >( nQuestid );
		}
		else
		{
			if( pUser->m_nCheckedQuestSize <= 0 )
				return;

			for( size_t i = 0; i < pUser->m_nCheckedQuestSize; ++i )
			{
				if( pUser->m_aCheckedQuest[ i ] == nQuestid )
				{
					for( size_t j = i; j < pUser->m_nCheckedQuestSize -1; ++j )
						pUser->m_aCheckedQuest[ j ] = pUser->m_aCheckedQuest[ j+1 ];
					pUser->m_aCheckedQuest[ --pUser->m_nCheckedQuestSize ] = 0;
					break;
				}
			}
		}
		pUser->AddCheckedQuest();
	}
}

void CDPSrvr::OnInviteCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pRequest = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		FLWSUser* pTarget = g_xWSUserManager->GetUserByPlayerID( idTarget );
		if( IsValidObj( pTarget ) )
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pTarget = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		FLWSUser* pRequest = g_xWSUserManager->GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			CCampusHelper::GetInstance()->OnAcceptCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pTarget = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		FLWSUser* pRequest = g_xWSUserManager->GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			pRequest->AddDefinedText( TID_GAME_TS_REFUSAL, "\"%s\"", pTarget->GetName() );
	}
}

void CDPSrvr::OnRemoveCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pRequest = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		if( idTarget > 0 )
			CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
	}
}




//	mulcom	BEGIN100405	각성 보호의 두루마리
void	CDPSrvr::OnItemSelectAwakeningValue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pRequest = g_xWSUserManager->GetUser( dpidCache, dpidUser );

	if( IsValidObj( pRequest ) == TRUE )
	{
		DWORD			dwItemObjID		= 0;
		SERIALNUMBER	iSerialNumber	= 0;
		BYTE			bySelectFlag	= 0;

		ar >> dwItemObjID;
		ar >> iSerialNumber;
		ar >> bySelectFlag;

		pRequest->SelectAwakeningValue( dwItemObjID, iSerialNumber, bySelectFlag );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pUser is invalid in OnItemSelectAwakeningValue function." ) );
	}
}
//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
void CDPSrvr::OnReqGuildHouseTenderMainWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		DWORD dwGHType;
		OBJID objNpcId;
		ar >> dwGHType >> objNpcId;

		GuildHouseMng->ReqTenderGuildHouseList( pUser, dwGHType, objNpcId );
	}
}

void CDPSrvr::OnReqGuildHouseTenderInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		ar >> objGHId;

		GuildHouseMng->ReqTenderGuildHouseInfo( pUser, objGHId );
	}
}

void CDPSrvr::OnReqGuildHouseTenderJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		int nTenderPerin, nTenderPenya;
		ar >> objGHId >> nTenderPerin >> nTenderPenya;

		GuildHouseMng->ReqGuildHouseTenderJoin( pUser, objGHId, nTenderPerin, nTenderPenya );
	}
}

void CDPSrvr::OnReqGuildHouseInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_GUILDHOUSE_INFO tGuildHouseInfo = { 0 };
		//mem_set( &tGuildHouseInfo, 0, sizeof( tGuildHouseInfo ) );

		ar.Read( (void*)&tGuildHouseInfo, sizeof( T_PACKET_GUILDHOUSE_INFO ) );

		GuildHouseMng->ReqGuildHouseInfo( pUser, &tGuildHouseInfo );
	}
}

void CDPSrvr::OnReqGuildHouseCommentChange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_GUILDHOUSE_INFO tGuildHouseInfo = { 0 };
		//mem_set( &tGuildHouseInfo, 0, sizeof( tGuildHouseInfo ) );

		ar.Read( (void*)&tGuildHouseInfo, sizeof( T_PACKET_GUILDHOUSE_INFO ) );

		GuildHouseMng->ReqGuildHouseCommentChange( pUser, &tGuildHouseInfo );
	}
}

#endif // __GUILD_HOUSE_MIDDLE


//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

void	CDPSrvr::OnCouponNumber( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_COUPON_NUMBER tCouponNumber = { 0 };
		//mem_set( &tCouponNumber, 0, sizeof( tCouponNumber ) );
		ar.Read( (void*)(&tCouponNumber), sizeof( T_PACKET_COUPON_NUMBER ) );

		if( pUser->m_dwLastInputCouponNumberTick + 1000 > GetTickCount() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "User:[%07d], LastTick:[%d], CurTick:[%d], CouponNumber:[%s]" ),
				pUser->m_idPlayer, pUser->m_dwLastInputCouponNumberTick, GetTickCount(), tCouponNumber.szCouponNumber );
			return;
		}
		pUser->m_dwLastInputCouponNumberTick = GetTickCount();

		if( pUser->m_idPlayer != tCouponNumber.dwPlayerId )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Different idPlayer. S:[%07d], C:[%07d]" ), pUser->m_idPlayer, tCouponNumber.dwPlayerId );
			return;
		}

		if( _tcslen( tCouponNumber.szCouponNumber ) != _MAX_COUPON_NUMBER_LEN )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Coupon Size. [%s]" ), tCouponNumber.szCouponNumber );
			return;
		}

		FLStrcpy( tCouponNumber.szAccount, _countof( tCouponNumber.szAccount ), pUser->m_playAccount.lpszAccount );
		g_dpDBClient.SendCouponNumber( &tCouponNumber );
	}
}

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////


#ifdef __HYPERLINK_ITEM16
void	CDPSrvr::OnReqItemLinkInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_ITEM_LINK tItemLink = { 0 };
		//mem_set( &tItemLink, 0, sizeof( tItemLink ) );

		ar.Read( (void*)(&tItemLink), sizeof( T_PACKET_ITEM_LINK ) );

		FLWSUser* pItemOwner = g_xWSUserManager->GetUserByPlayerID( tItemLink.dwPlayerId );
		if( IsValidObj( pItemOwner ) == TRUE )
		{
			FLItemElem* pItemElem	= pItemOwner->m_Inventory.GetAtId( tItemLink.dwObjId );
			if( pItemElem != NULL )
			{
				PT_ITEM_SPEC pItemProp	= pItemElem->GetProp();
				if( pItemProp != NULL )
				{
					pUser->AddItemLinkInfo( &tItemLink, pItemElem );
				}
			}
		}
	}
}

#endif // __HYPERLINK_ITEM16


#ifdef __ENCHANT_BARUNA16

void	CDPSrvr::OnReqExtractOper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID )			//바루나 정령이 호출 되어 있지 않다면.
		{
			pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
			return;
		}

		T_PACKET_OPER_EXTRACT tOperExtract = { 0 };
		//mem_set( &tOperExtract, 0, sizeof( tOperExtract ) );

		ar.Read( (void*)&tOperExtract, sizeof( T_PACKET_OPER_EXTRACT ) );

		tOperExtract.bResult = false;

		FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( tOperExtract.dwMaterialObjid );

		if( IsUsableItem( pItemElem ) == TRUE )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp != NULL )
			{
				if( pItemProp->dwItemKind1 == IK1_WEAPON )
				{
					if( g_xItemExtract->OnExtractOperByWeapon( pUser, &tOperExtract ) == true )
					{
						tOperExtract.bResult = true;
					}
				}
				else if( pItemProp->dwItemKind1 == IK1_ARMOR )
				{
					if( g_xItemExtract->OnExtractOperByArmor( pUser, &tOperExtract ) == true )
					{
						tOperExtract.bResult = true;
					}
				}
				else
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT01 );
				}
			}
		}
		pUser->AddResultOperExtract( &tOperExtract );
	}
}

void	CDPSrvr::OnReqCreateOper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID )			//바루나 정령이 호출 되어 있지 않다면.
		{
			pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
			return;
		}

		T_PACKET_OPER_CREATE tOperCreate = { 0 };
		//mem_set( &tOperCreate, 0, sizeof( tOperCreate ) );

		ar.Read( (void*)&tOperCreate, sizeof( T_PACKET_OPER_CREATE ) );

		tOperCreate.bResult = false;

		FLItemElem* pOperPiece = pUser->m_Inventory.GetAtId( tOperCreate.dwOperPieceObjid );
		FLItemElem* pOperPieceCombine = pUser->m_Inventory.GetAtId( tOperCreate.dwOperPieceCombineObjid );

		if( IsUsableItem( pOperPiece ) == TRUE && IsUsableItem( pOperPieceCombine ) == TRUE )
		{
			if( g_xItemExtract->OnCreateOper( pUser, &tOperCreate) == true )
			{
				tOperCreate.bResult = true;
			}
		}
		pUser->AddResultOperCreate( &tOperCreate );
	}
}

void	CDPSrvr::OnReqCreateCid( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID )			//바루나 정령이 호출 되어 있지 않다면.
		{
			pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
			return;
		}

		T_PACKET_CID_CREATE tCidCreate = { 0 };
		//mem_set( &tCidCreate, 0, sizeof( tCidCreate ) );

		ar.Read( (void*)&tCidCreate, sizeof( T_PACKET_CID_CREATE ) );

		tCidCreate.bResult = false;

		FLItemElem* pCId = pUser->m_Inventory.GetAtId( tCidCreate.dwCidObjid );
		FLItemElem* pCidCombine = pUser->m_Inventory.GetAtId( tCidCreate.dwCidCombineObjid );

		if( IsUsableItem( pCId ) == TRUE && IsUsableItem( pCidCombine ) == TRUE )
		{
			if( g_xItemExtract->OnCreateCid( pUser, &tCidCreate ) == true )
			{
				tCidCreate.bResult = true;
			}
		}
		pUser->AddResultCidCreate( &tCidCreate );
	}
}

void	CDPSrvr::OnReqUpgradeCidPiece( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID )			//바루나 정령이 호출 되어 있지 않다면.
		{
			pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
			return;
		}

		T_PACKET_CIDPIECE_UPGRADE tCidPieceUpgrade = { 0 };
		//mem_set( &tCidPieceUpgrade, 0, sizeof( tCidPieceUpgrade ) );

		ar.Read( (void*)&tCidPieceUpgrade, sizeof( T_PACKET_CIDPIECE_UPGRADE ) );

		tCidPieceUpgrade.bResult = false;

		FLItemElem* pCIdPiece = pUser->m_Inventory.GetAtId( tCidPieceUpgrade.dwCidPieceObjid );
		FLItemElem* pCidPieceCombine = pUser->m_Inventory.GetAtId( tCidPieceUpgrade.dwCidPieceCombineObjid );

		if( IsUsableItem( pCIdPiece ) == TRUE && IsUsableItem( pCidPieceCombine ) == TRUE )
		{
			if( g_xItemExtract->OnUpgradeCidPiece( pUser, &tCidPieceUpgrade ) == true )
			{
				tCidPieceUpgrade.bResult = true;
			}
		}
		pUser->AddResultCidPieceUpgrade( &tCidPieceUpgrade );
	}
}

void	CDPSrvr::OnReqCreateOperCid( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID )			//바루나 정령이 호출 되어 있지 않다면.
		{
			pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
			return;
		}

		T_PACKET_OPERCID_CREATE tOperCidCreate = { 0 };
		//mem_set( &tOperCidCreate, 0, sizeof( tOperCidCreate ) );

		ar.Read( (void*)&tOperCidCreate, sizeof( T_PACKET_OPERCID_CREATE ) );

		tOperCidCreate.bResult = false;

		FLItemElem* pOper = pUser->m_Inventory.GetAtId( tOperCidCreate.dwOperObjid );
		FLItemElem* pCId = pUser->m_Inventory.GetAtId( tOperCidCreate.dwCidObjid );
		FLItemElem* pBaryummeal = pUser->m_Inventory.GetAtId( tOperCidCreate.dwBaryummealObjid );

		if( IsUsableItem( pOper ) == TRUE && IsUsableItem( pCId ) == TRUE && IsUsableItem( pBaryummeal ) == TRUE )
		{
			if( g_xItemExtract->OnCreateOperCid( pUser, &tOperCidCreate ) == true )
			{
				tOperCidCreate.bResult = true;
			}
		}
		pUser->AddResultOperCidCreate( &tOperCidCreate );
	}
}

#endif // __ENCHANT_BARUNA16

//협동기부
void	CDPSrvr::OnCooperativeContributions_GetInfoReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	using namespace nsCooperativeContributions;

	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;



	T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_REQ req;
	ar.Read( &req, sizeof( req ) );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	CMover * pNPC	= prj.GetMover( req.dwObjidNPC );
	if( IsValidObj( pNPC ) == FALSE )
		return;

	if( pUser->IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
		return;

	LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
	if( pCharacter == NULL )
		return;

	const DWORD dwContributionID	= pCharacter->m_dwContributionID;
	FLContribution * pContribution	= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	if( pContribution == NULL )
	{
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
		return;
	}

	if( pContribution->IsEventPeriod() == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
	}

	pUser->AddCooperativeContributions_Info( *pContribution );
}

void	CDPSrvr::OnCooperativeContributions_ContributeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	using namespace nsCooperativeContributions;

	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_REQ req;
	ar.Read( &req, sizeof( req ) );

	//	if( req.nItemCount <= 0 )
	//		return;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	CMover * pNPC	= prj.GetMover( req.dwObjidNPC );
	if( IsValidObj( pNPC ) == FALSE )
		return;

	if( pUser->IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
		return;

	LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
	if( pCharacter == NULL )
		return;

	const DWORD dwContributionID		= pCharacter->m_dwContributionID;
	FLContribution * pContribution		= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	if( pContribution == NULL )
		return;

	IGlobalEndCondition * pGlobalEndCondition = pContribution->GetGlobalEndCondition();
	if( pGlobalEndCondition == NULL )
		return;

	if( pContribution->IsBeginCondition( *pUser ) == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION01 );
		pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::INVALID_BEGIN_CONDITION );
		return;
	}

	if( pContribution->IsEventPeriod() == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
		pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::INVALID_PERIOD );
		return;
	}

	if( pContribution->IsComplete() == TRUE )
		//if( pGlobalEndCondition->IsComplete( pContribution->m_dwID ) == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION03 );
		pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::ALREADY_COMPLETE );
		return;
	}

	const BOOL bRet			= pGlobalEndCondition->Contribute( *pUser );
	if( bRet == FALSE )
	{
		//pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION04 );
		pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::NOT_ENOUGH );
		return;
	}

	pGlobalEndCondition->Query_Contribute( pContribution->m_dwID, pContribution->m_tmStartDate, pContribution->m_tmEndDate, pUser->m_idPlayer, pUser->GetName() );
	//pGlobalEndCondition->GetContributionUnit()
	pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION09 );
	pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::RESULT_SUCCESS );



	// 실제 기부
	//if( pUser->TryRemoveNotUsingItem( req.dwItemID, pGlobalEndCondition->GetContributionUnit() ) == FALSE )
	//{
	//	pUser->AddCooperativeContributions_Contribute( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::NOT_ENOUGH );
	//	return;
	//}
	//	COOPERATIVE_CONTRIBUTIONS().Query_ContributeItem( pUser->GetId(), pContribution->m_dwID, req.dwItemID, pGlobalEndCondition->GetContributionUnit() );	//db 다녀와서 보내줌

}


//
//void	CDPSrvr::OnCooperativeContributions_ContributeItemReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
//{
//	using namespace nsCooperativeContributions;
//
//	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_REQ req;
//	ar.Read( &req, sizeof( req ) );
//
////	if( req.nItemCount <= 0 )
////		return;
//
//	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
//	if( IsValidObj( pUser ) == FALSE )
//		return;
//
//	CMover * pNPC	= prj.GetMover( req.dwObjidNPC );
//	if( IsValidObj( pNPC ) == FALSE )
//		return;
//
//	if( pUser->IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
//		return;
//
//	LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
//	if( pCharacter == NULL )
//		return;
//
//	const DWORD dwContributionID		= pCharacter->m_dwContributionID;
//	FLContribution * pContribution		= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
//	if( pContribution == NULL )
//		return;
//
//	////조건이 여러개 이거나 없으면
//	//if( pContribution->IsValid_EndCondition() == false )
//	//{
//	//	Error( "[ MISSING END CONDITION INFO (%u) ]", dwContributionID );
//	//	return;
//	//}
//	
//	IGlobalEndCondition * pGlobalEndCondition = pContribution->GetGlobalEndCondition( CONTRIBUTION_ITEM, req.dwItemID );
//	if( pGlobalEndCondition == NULL )
//		return;
//
//	if( pContribution->IsBeginCondition( *pUser ) == FALSE )
//	{
//		pUser->AddCooperativeContributions_ContributeItem( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_ACK::INVALID_BEGIN_CONDITION
//			, pContribution->m_dwID, req.dwItemID, 0 );
//		return;
//	}
//
//	if( pContribution->IsEventPeriod() == FALSE )
//	{
//		pUser->AddCooperativeContributions_ContributeItem( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_ACK::INVALID_PERIOD
//			, pContribution->m_dwID, req.dwItemID, 0 );
//		return;
//	}
//
//	if( pGlobalEndCondition->IsComplete( pContribution->m_dwID ) == TRUE )
//	{
//		pUser->AddCooperativeContributions_ContributeItem( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_ACK::ALREADY_COMPLETE
//			, pContribution->m_dwID, req.dwItemID, 0 );
//		return;
//	}
//
//	// 실제 기부
//	if( pUser->TryRemoveNotUsingItem( req.dwItemID, pGlobalEndCondition->GetContributionUnit() ) == FALSE )
//	{
//		pUser->AddCooperativeContributions_ContributeItem( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_ACK::NOT_ENOUGH
//			, pContribution->m_dwID, req.dwItemID, 0 );
//		return;
//	}
//	
//	COOPERATIVE_CONTRIBUTIONS().Query_ContributeItem( pUser->GetId(), pContribution->m_dwID, req.dwItemID, pGlobalEndCondition->GetContributionUnit() );	//db 다녀와서 보내줌
//}
//
//void	CDPSrvr::OnCooperativeContributions_ContributeGoldReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
//{
//	using namespace nsCooperativeContributions;
//
//	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_REQ req;
//	ar.Read( &req, sizeof( req ) );
//
//	//if( req.nGoldCount <= 0 )
//	//	return;
//	//
//	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
//	if( IsValidObj( pUser ) == FALSE )
//		return;
//
//	CMover * pNPC	= prj.GetMover( req.dwObjidNPC );
//	if( IsValidObj( pNPC ) == FALSE )
//		return;
//
//	if( pUser->IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
//		return;
//
//	LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
//	if( pCharacter == NULL )
//		return;
//
//	const DWORD dwContributionID		= pCharacter->m_dwContributionID;
//	FLContribution * pContribution = COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
//	if( pContribution == NULL )
//		return;
//
//	////조건이 여러개 이거나 없으면
//	//if( pContribution->IsValid_EndCondition() == false )
//	//{
//	//	Error( "[ MISSING END CONDITION INFO (%u) ]", dwContributionID );
//	//	return;
//	//}
//
//	//조건이 없으면 에러
//	IGlobalEndCondition * pGlobalEndCondition = pContribution->GetGlobalEndCondition( CONTRIBUTION_GOLD, 0 );
//	if( pGlobalEndCondition == NULL )
//		return;
//
//	if( pContribution->IsBeginCondition( *pUser ) == FALSE )
//	{
//		pUser->AddCooperativeContributions_ContributeGold( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_ACK::INVALID_BEGIN_CONDITION, pContribution->m_dwID, 0 );
//		return;
//	}
//
//	if( pContribution->IsEventPeriod() == FALSE )
//	{
//		pUser->AddCooperativeContributions_ContributeGold( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_ACK::INVALID_PERIOD, pContribution->m_dwID, 0 );
//		return;
//	}
//
//	if( pGlobalEndCondition->IsComplete( pContribution->m_dwID ) == TRUE )
//	{
//		pUser->AddCooperativeContributions_ContributeGold( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_ACK::ALREADY_COMPLETE, pContribution->m_dwID, 0 );
//		return;
//	}
//
//	// 실제 기부
//	if( pUser->GetGold() < static_cast< int >( pGlobalEndCondition->GetContributionUnit() ) )
//	{
//		pUser->AddCooperativeContributions_ContributeGold( T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_ACK::NOT_ENOUGH, pContribution->m_dwID, 0 );
//		return;
//	}
//
//	pUser->AddGold( -( int )pGlobalEndCondition->GetContributionUnit() );
//	
//	COOPERATIVE_CONTRIBUTIONS().Query_ContributeGold( pUser->GetId(), pContribution->m_dwID, pGlobalEndCondition->GetContributionUnit() );		//DB다녀와서 기부 정보 보내줌.
//}

void CDPSrvr::OnCooperativeContributions_GetRanking( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	using namespace nsCooperativeContributions;

	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_REQ req;
	ar.Read( &req, sizeof( req ) );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	CMover * pNPC	= prj.GetMover( req.dwObjidNPC );
	if( IsValidObj( pNPC ) == FALSE )
		return;

	if( pUser->IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
		return;

	LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
	if( pCharacter == NULL )
		return;

	const DWORD dwContributionID		= pCharacter->m_dwContributionID;
	FLContribution * pContribution = COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	if( pContribution == NULL )
	{
		pUser->AddCooperativeContributions_GetRanking( T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::INVALID_PERIOD, NULL );
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
		return;
	}

	const FLTm tmCurr			= DATE_TIMER().GetCurrentTime();
	if( pContribution->m_tmStartDate <= tmCurr && tmCurr >= pContribution->m_tmEndDate )
	{
		pUser->AddCooperativeContributions_GetRanking( T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::INVALID_PERIOD, NULL );
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
		return;
	}

	COOPERATIVE_CONTRIBUTIONS().Query_GetRanking( pUser->m_idPlayer, *pContribution );
}

/////////////////////////////////////////////////////////////////////////
// 콜로세움
void CDPSrvr::OnColosseum_Enter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	T_PACKET_COLOSSEUM_ENTER_REQ req;
	ar.Read( &req, sizeof( req ) );

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	const u_long nPartyID	= pUser->GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_COLONOWPLAYING01 );
		return;
	}
	////파티 장인지.
	//if( pParty->IsLeader( pUser->m_idPlayer ) == FALSE )
	//	return;

	//if( req.bGuild == TRUE && pParty->IsGuildParty() == FALSE )		//길드 파티인지.
	//{
	//	pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
	//	//@@@@@  길드 파티가 아닙니다.
	//	return;
	//}


	//파티인지 자동 체크 됨.
//	BOOL DoEnter			= FALSE;
	const DWORD dwDungeonID = nPartyID;

	DWORD dwGuildID			= 0;
	if( req.bGuild == TRUE )
	{
		if( pUser->GetGuild() != NULL )
		{		
			dwGuildID		= pUser->GetGuild()->GetGuildId();
		}
	}

	if( req.bGuild == TRUE && dwGuildID == 0 )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_COLOGUILDGROUP01 );
		return;
	}

	//const DWORD dwGuildID	= ( req.bGuild == TRUE ) ? pUser->GetGuild()
	//CMover * pLeader		= pParty->GetLeader();
	//if( IsValidObj( pLeader ) == FALSE )
	//{
	//	pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
	//	pUser->AddDefinedText( TID_COLOSSEUM_NOTENTERTIME01 );
	//	return;
	//}
	//
	//const DWORD 


	//인던 있을때
	INDUN_INFO * pInfo			= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( dwDungeonID, WI_WORLD_COLOSSEUM );
	if( pInfo == NULL )		//새로 생성
	{
		if( pParty->IsLeader( pUser->m_idPlayer ) == TRUE )
		{
			if( CInstanceDungeonHelper::GetInstance()->EnteranceDungeon( pUser, /*WI_INSTANCE_OMINOUS*/WI_WORLD_COLOSSEUM, dwGuildID, req.dwDungeonLevel ) == FALSE ) //길드 던전인지 인자에 알려줘야함.
			{
				pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
				return;
			}
			else
			{
				pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_SUCESS );
				return;
			}
		}
		else
		{
			pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
			pUser->AddDefinedText( TID_COLOSSEUM_NOTPARTYREADERENGER );
			return;
		}
		return;
	}


	//인던 있을때
	if( pParty->IsLeader( pUser->m_idPlayer ) == TRUE )
	{
		pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
		pUser->AddDefinedText( TID_COLOSSEUM_NOTENTERTIME01 );
		return;
	}

	if( CInstanceDungeonHelper::GetInstance()->EnteranceDungeon( pUser, WI_WORLD_COLOSSEUM, dwGuildID, req.dwDungeonLevel ) == FALSE ) //길드 던전인지 인자에 알려줘야함.
	{
		pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_FAIL );
		return;
	}

	pUser->AddColosseum_Enter( T_PACKET_COLOSSEUM_ENTER_ACK::RST_SUCESS );
	pUser->AddDefinedText( TID_COLOSSEUM_COLOENTER01 );	
}



void CDPSrvr::OnColosseum_Leave( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	const u_long nPartyID	= pUser->GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return;
	//파티 장인지.
	//if( pParty->IsLeader( pUser->m_idPlayer ) == FALSE )
	//	return;

	const DWORD dwDungeonID = nPartyID;
	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( dwDungeonID, pUser->GetWorld()->GetID() );
	if( pInfo == NULL )
		return;

	const CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( *pInfo );
	if( pData == NULL )
		return;

	if( pData->bColosseumStyle == FALSE )
		return;

	// Retry 상태 이면
	if( pInfo->m_pState == &pInfo->CompleteAllWaitingState || pInfo->m_pState == &pInfo->AllDieWaitingState || pInfo->m_pState == &pInfo->ExpiredTimeWaitingState )
	{
		CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		return;
	}
}



void CDPSrvr::OnColosseum_Retry( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	DWORD dwObjId;
	ar >> dwObjId;

	FLItemElem* pRetryItem = pUser->m_Inventory.GetAtId( dwObjId );

	const u_long nPartyID	= pUser->GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return;
	//파티 장인지.
	if( pParty->IsLeader( pUser->m_idPlayer ) == FALSE )
		return;

	const DWORD dwDungeonID = nPartyID;
	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( dwDungeonID, pUser->GetWorld()->GetID() );
	if( pInfo == NULL )
		return;

	const CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( *pInfo );
	if( pData == NULL )
		return;

	if( pData->bColosseumStyle == FALSE )
		return;

	// Retry 상태 이면
	if( pInfo->m_pState == &pInfo->ExpiredTimeWaitingState || pInfo->m_pState == &pInfo->AllDieWaitingState )
	{
		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pRetryItem ) == FLUseActiveItem::ERR_SUCCESS_COLOSSEUM_RETRY )
		{
			g_DPSrvr.PutItemLog( pUser, "N", "ColosseumRetry_Use", pRetryItem );
			g_pItemUsing->CompleteUseItem( pUser, pRetryItem->m_dwObjId );

			pUser->AddColosseum_Retry( T_PACKET_COLOSSEUM_RETRY_ACK::RST_SUCESS );
			pInfo->SetState( & pInfo->RetriedStageWaitingState, pData->arrLevelProp[ pInfo->eDungeonLevel ].dwRetryStartStage_WaitingTick );
			return;
		}
	}

	pUser->AddColosseum_Retry( T_PACKET_COLOSSEUM_RETRY_ACK::RST_FAIL );
}

void CDPSrvr::OnColosseum_ForceStart( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{	
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	const u_long nPartyID	= pUser->GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return;
	//파티 장인지.
	if( pParty->IsLeader( pUser->m_idPlayer ) == FALSE )
		return;

	const DWORD dwDungeonID = nPartyID;
	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( dwDungeonID, pUser->GetWorld()->GetID() );
	if( pInfo == NULL )
		return;

	const CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( *pInfo );
	if( pData == NULL )
		return;

	if( pData->bColosseumStyle == FALSE )
		return;

	// AutoStart 상태이면
	if( pInfo->m_pState == &pInfo->AutoStartState )
	{
		pInfo->SetState( & pInfo->StageWaitingState, pData->arrLevelProp[ pInfo->eDungeonLevel ].dwStartStage_WaitingTick );			// 
	}
}

void CDPSrvr::OnColosseum_InviteAck( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	T_PACKET_COLOSSEUM_AUTO_INVITE_ACK req;
	ar.Read( &req, sizeof( req ) );

	if( req.bYes == FALSE )
		return;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	const u_long nPartyID	= pUser->GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_COLONOWPLAYING01 );
		return;
	}

	////파티 장이 아니여야함
	if( pParty->IsLeader( pUser->m_idPlayer ) == TRUE )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_NOTENTERMATCH01 );	
		return;
	}

	const DWORD dwDungeonID		= nPartyID;
	const DWORD dwGuildID		= pUser->GetGuild() == NULL ? 0 : pUser->GetGuild()->GetGuildId();

	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( dwDungeonID, req.dwWorldID );
	if( pInfo == NULL )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_NOTENTERTIME01 );	
		return;
	}

	if( pUser->IsFly() == TRUE )
	{
		pUser->AddDefinedText( TID_COLOSSEUM_NOTENTER_FLYING );
		return;
	}

	// 입던 시도
	if( CInstanceDungeonHelper::GetInstance()->EnteranceDungeon( pUser, WI_WORLD_COLOSSEUM, dwGuildID, pInfo->eDungeonLevel  ) == TRUE ) //길드 던전인지 인자에 알려줘야함.
	{
		pUser->AddDefinedText( TID_COLOSSEUM_COLOENTER01 );	
	}
}

void CDPSrvr::OnColosseum_RegistAdditionalRealMonsterRateItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	DWORD dwItemObjID;
	ar >> dwItemObjID;

	if( CInstanceDungeonHelper::GetInstance()->RegistAdditionalRealMonsterRateItem( *pUser, dwItemObjID ) == FALSE )
	{
		pUser->AddColosseum_RegistAdditionalRealMonsterRateItemAck( FALSE );
		return;
	}

	pUser->AddColosseum_RegistAdditionalRealMonsterRateItemAck( TRUE );
}

void CDPSrvr::OnColosseum_UnRegistAdditionalRealMonsterRateItem( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->GetWorld() == NULL )
		return;

	if( CInstanceDungeonHelper::GetInstance()->UnRegistAdditionalRealMonsterRateItem( *pUser ) == FALSE )
	{
		pUser->AddColosseum_UnRegistAdditionalRealMonsterRateItemAck( FALSE );
		return;
	}

	pUser->AddColosseum_UnRegistAdditionalRealMonsterRateItemAck( TRUE );
}

void CDPSrvr::OnColosseum_RankingInfo( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	// Send DB 
	T_W2DB_PACKET_COLOSSEUM_GET_RANKING_INFO query;
	query.idPlayer			= pUser->m_idPlayer;
	query.dwWorldID			= WI_WORLD_COLOSSEUM;
	//query.eDungeonLevel		= eDungeonLevel;

	BEFORESENDDUAL( send, PACKETTYPE_COLOSSEUM_GET_RANKING_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	send.Write( &query, sizeof( query ) );
	SEND( send, &g_dpDBClient, DPID_SERVERPLAYER );	


	//#ifdef _DEBUG
	//	{
	//		using namespace nsConsignmentSale;
	//		static int i = 0;
	//
	//		const int maxi = 7;
	//
	//
	//
	//		if( ( i % maxi ) == 0 )
	//		{
	//			for( int c = 0, j = 0; c < 900100; ++j )
	//			{
	//				FLItemElem* pItem		= pUser->m_Inventory.GetAtId( j % pUser->m_Inventory.GetMax() );
	//				if( pItem == NULL )
	//				{ 
	//					continue;
	//				}
	//
	//				PT_ITEM_SPEC pSpec		= pItem->GetProp();
	//
	//				//				2. 보내준다.
	//				FLPacketWDB_RegistReq kReqDB;
	//				kReqDB.m_uPlayerID			= pUser->m_idPlayer;
	//				kReqDB.m_nPrice				= ::xRand() % 1000 + 100;
	//				kReqDB.m_nOnePrice			= kReqDB.m_nPrice / pItem->m_nItemNum;
	//
	//				FLStrcpy( kReqDB.m_szItemName, _countof( kReqDB.m_szItemName ), pSpec->szName );
	//				FLStrcpy( kReqDB.m_szPlayerName, _countof( kReqDB.m_szPlayerName ), pUser->GetName() );
	//
	//				kReqDB.m_dwItemType1		= pSpec->dwMainCategory;	//@@@@@@@@@@@@@@@@@@
	//				kReqDB.m_dwItemType2		= pSpec->dwSubCategory;//@@@@@@@@@@@@@@@@@@
	//				kReqDB.m_dwGrade			= pSpec->dwItemGrade;	//@@@@@@@@@@@@@@@@@@
	//				kReqDB.m_nLimitLevel		= pSpec->nLimitLevel;	
	//				kReqDB.m_uRemainHour		= 24 * 7;//@@@@@@@@@@@@@@@@@@
	//				kReqDB.m_kItemElem			= *pItem;
	//
	//				g_dpDBClient.SendPacket( &kReqDB );
	//				++c;
	//			}
	//
	//			/*FLPacket_RegistReq req;
	//			req.m_dwItemObjID		= 0;
	//			req.m_nItemCount		= 1;
	//			req.m_nItemCost		= 200;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_RegistReq( ar2, dpidCache, dpidUser, 0, 0 );*/
	//		}
	//
	//		if( ( i % maxi ) == 1 )
	//		{
	//			FLPacket_CancelRegistedReq req;
	//			req.m_u64SaleSRL		= 0;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_CancelRegistedReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//		if( ( i % maxi ) == 2 )
	//		{
	//			FLPacket_RegistedInfoListReq req;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_RegistedInfoListReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//		if( ( i % maxi ) == 3 )
	//		{
	//			FLPacket_BuyItemReq req;
	//			req.m_u64SaleSRL		= 0;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_BuyItemReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//		if( ( i % maxi ) == 4 )
	//		{
	//			FLPacket_CollectSaleGoldReq req;
	//			req.m_u64SaleGoldSRL		= 0;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_CollectSaleGoldReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//		if( ( i % maxi ) == 5 )
	//		{
	//			FLPacket_SearchReq req;
	//			req.m_eOrderingOption	= FLPacketWDB_SearchReq::E_ONE_PRICE;
	//			req.m_bASC				= TRUE;
	//			req.m_dwPage				= 2;
	//			req.m_arrGrade[ 0 ]		= ITEM_GRADE_NONE;
	//			req.m_arrGrade[ 1 ]		= ITEM_GRADE_NONE;
	//			req.m_arrGrade[ 2 ]		= ITEM_GRADE_NONE;
	//			req.m_arrGrade[ 3 ]		= ITEM_GRADE_NONE;
	//			req.m_nLimitLevelMin		= 0;
	//			req.m_nLimitLevelMax		= 100;
	//
	//			req.m_dwItemType1		= TYPE1_NONE;
	//			req.m_dwItemType2		= TYPE2_NONE;
	//
	//			req.m_szSearchName[0]	= '\0';
	//			req.m_bSearchBySameName	= FALSE;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//
	//			OnConsignmentSale_SearchReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//		if( ( i % maxi ) == 6 )
	//		{
	//			FLPacket_SaleGoldInfoListReq req;
	//
	//			CAr ar;
	//			req.Serialize( ar );
	//			u_long size;
	//			LPBYTE buff		= 	ar.GetBuffer( &size ) +  sizeof(DWORD);
	//			CAr ar2( buff, size );
	//			OnConsignmentSale_SaleGoldInfoListReq( ar2, dpidCache, dpidUser, 0, 0 );
	//		}
	//
	//
	//		++i;
	//
	//	}
	//#endif //_DEBUG

}



void CDPSrvr::OnConsignmentSale_UserInfoReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_UserInfoReq kReq;
	kReq.Deserialize( ar );


	//FLSnapshot_UserInfoAck ack;
	//ack.m_dwDefaultRegistCount		= CONSIGNMENT_SALE_SPEC().m().nDefaultRegistLimit;
	//ack.m_dwDefaultRegistCountMax	= pUser->m_kConsignmentSaleData.nDefaultRegistCount;
	//ack.m_dwExtendRegistCount		= pUser->m_kConsignmentSaleData.nItemRegistCount;
	//ack.m_dwExtendRegistCountMax	= FLConsignmentSale::GetExtendRegistCountMax( *pUser );




	//pUser->AddPacket( &ack );
	FLPacketWDB_GetRegistedCountReq kReqDB;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );
}

void CDPSrvr::OnConsignmentSale_RegistReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser			= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->m_kConsignmentSaleData.bRegisting == TRUE ) //이미 다른 것을 등록 중이라면 
	{
		return;
	}

	FLPacket_RegistReq kReq;
	kReq.Deserialize( ar );

	if( kReq.m_nItemCount <= 0 || kReq.m_nItemCost <= 0 )
		return;

	const int MAX_ITEM_COST	= 2100000000;
	if( kReq.m_nItemCost > MAX_ITEM_COST )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_SELLTEXT04 );
		return;
	}

	////1. 
	//FLItemElem* pItem		= pUser->m_Inventory.GetAtId( kReq.m_dwItemObjID );
	//if( pItem == NULL )
	//	return;

	//if( pItem->IsOwnState() == TRUE || IsUsableItem( pItem ) == FALSE )
	//{
	//	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR01 );
	//	return;
	//}

	//if( pUser->IsUsing( pItem ) == TRUE )
	//{
	//	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMRESEARCH01 );
	//	return;
	//}

	//PT_ITEM_SPEC pSpec		= pItem->GetProp();
	//if( pSpec == NULL )
	//{
	//	FLERROR_LOG( PROGRAM_NAME, "[ INVALID ITEM(%u) ]", pItem->m_dwItemId );
	//	return;
	//}

	//if( pSpec->bCanTrade == FALSE )
	//{
	//	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR01, "\"%s\"", ( pSpec->szName[ 0 ] == '\0' ) ? "no item" : pSpec->szName );
	//	FLSnapshot_RegistAck ack;
	//	ack.m_eResult		= FLSnapshot_RegistAck::E_OVERFLOW;
	//	return;
	//}

	//if( pItem->m_nItemNum < kReq.m_nItemCount )
	//{
	//	pUser->AddDefinedText( TID_MMI_TRADESYSTEM_ITEMREGISTERERROR02 );
	//	FLSnapshot_RegistAck ack;
	//	ack.m_eResult		= FLSnapshot_RegistAck::E_OVERFLOW;
	//	return;
	//}

	FLPacketWDB_CheckRegistedCountReq kReqDB;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	kReqDB.m_dwItemObjID	= kReq.m_dwItemObjID;
	kReqDB.m_nItemCost		= kReq.m_nItemCost;		
	kReqDB.m_nItemCount		= kReq.m_nItemCount;	

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );

	pUser->m_kConsignmentSaleData.bRegisting	= TRUE;
}




void CDPSrvr::OnConsignmentSale_CancelRegistedReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_CancelRegistedReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_CancelRegistedReq kReqDB;
	//static_cast< FLPacket_CancelRegistedReq >( kReqDB )= kReq;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	kReqDB.m_u64SaleSRL		= kReq.m_u64SaleSRL;

	FLStrcpy( kReqDB.m_szPlayerName, _countof( kReqDB.m_szPlayerName ), pUser->GetName() );
	kReqDB.m_dwWorldID		= ( pUser->GetWorld() ) ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;

	PutItemLog( pUser, "1", "ConsignementSale CancelStart", NULL, 0 );

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );
}

void CDPSrvr::OnConsignmentSale_RegistedInfoListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;


	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_RegistedInfoListReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_RegistedInfoListReq kReqDB;
	//static_cast< FLPacket_RegistedInfoListReq >( kReqDB )= kReq;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );
}

void CDPSrvr::OnConsignmentSale_BuyItemReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_BuyItemReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_GetPriceReq kReqDB;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	kReqDB.m_u64SaleSRL		= kReq.m_u64SaleSRL;
	//FLPacketWDB_BuyItemReq kReqDB;
	//static_cast< FLPacket_BuyItemReq >( kReqDB )= kReq;
	//kReqDB.m_uPlayerID		= pUser->m_idPlayer;

	//FLItemElem kItemElem;
	//PutItemLog( pUser, "1", "ConsignementSale BuyStart", kItemElem, 0 );

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );
}

void CDPSrvr::OnConsignmentSale_CollectSaleGoldReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_CollectSaleGoldReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_CollectSaleGoldReq kReqDB;
	//static_cast< FLPacket_CollectSaleGoldReq >( kReqDB )= kReq;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	FLStrcpy( kReqDB.m_szPlayerName, _countof( kReqDB.m_szPlayerName ), pUser->GetName() );
	kReqDB.m_u64SaleGoldSRL = kReq.m_u64SaleGoldSRL;

	PutPenyaLog( pUser, "s", "ConsignmentSale Collect", 0 );

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );

}

void CDPSrvr::OnConsignmentSale_SearchReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;

	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( FLConsignmentSale::IsQueryableTime_TryUpdateTime( pUser->m_kConsignmentSaleData ) == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_EXCEPTIONTEXT01 );
		return;
	}

	FLPacket_SearchReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_SearchReq kReqDB;
	//static_cast< FLPacket_SearchReq >( kReqDB )= kReq;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;

	kReqDB.m_eOrderingOption	= kReq.m_eOrderingOption;	
	kReqDB.m_bASC				= kReq.m_bASC;				
	kReqDB.m_dwPage				= kReq.m_dwPage;				
	kReqDB.m_arrGrade[ 0 ]		= kReq.m_arrGrade[ 0 ];		
	kReqDB.m_arrGrade[ 1 ]		= kReq.m_arrGrade[ 1 ];		
	kReqDB.m_arrGrade[ 2 ]		= kReq.m_arrGrade[ 2 ];		
	kReqDB.m_arrGrade[ 3 ]		= kReq.m_arrGrade[ 3 ];		

	kReqDB.m_nLimitLevelMin		= kReq.m_nLimitLevelMin;		
	kReqDB.m_nLimitLevelMax		= kReq.m_nLimitLevelMax;	

	kReqDB.m_nAbilityOptionMin	= kReq.m_nAbilityOptionMin;
	kReqDB.m_nAbilityOptionMax	= kReq.m_nAbilityOptionMax;

	kReqDB.m_dwItemType1		= kReq.m_dwItemType1;		
	kReqDB.m_dwItemType2		= kReq.m_dwItemType2;		

	FLStrcpy( kReqDB.m_szSearchName, _countof( kReqDB.m_szSearchName ), kReq.m_szSearchName );
	kReqDB.m_bSearchBySameName	= kReq.m_bSearchBySameName;


	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );
}

void CDPSrvr::OnConsignmentSale_SaleGoldInfoListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;


	using namespace nsConsignmentSale;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_SaleGoldInfoListReq kReq;
	kReq.Deserialize( ar );

	FLPacketWDB_SaleGoldInfoListReq kReqDB;
	//static_cast< FLPacket_SaleGoldInfoListReq >( kReqDB )= kReq;
	kReqDB.m_uPlayerID		= pUser->m_idPlayer;
	FLStrcpy( kReqDB.m_szPlayerName, _countof( kReqDB.m_szPlayerName ), pUser->GetName() );

	g_dpDBClient.SendPacket( dpidCache, dpidUser, &kReqDB );

}


void	CDPSrvr::OnConsignmentSale_CalcTaxReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;


	using namespace nsConsignmentSale;

	FLWSUser* pUser		= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	FLPacket_CalcTaxReq kReq;
	kReq.Deserialize( ar );

	FLSnapshot_CalcTaxAck kAck;
	kAck.m_nTax			= FLConsignmentSale::GetTax( pUser, kReq.m_nPrice );

	pUser->AddPacket( &kAck );
}


//void CDPSrvr::OnXTrapCommandResponse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
//{
//	if( FLXTrap_Server::GetInstance().IsActive() == FALSE )
//		return;
//
//	FLWSUser * pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
//	if( IsValidObj( pUser ) == FALSE )
//		return;
//
//	char recvBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ] = { 0 };
//	ar.Read( recvBuffer, sizeof( recvBuffer ) );
//
//	FLXTrap_Server::GetInstance().Response( pUser->m_XTrapSession, recvBuffer );
//}


/////////////////////////////////////////////////////////////////////////



void	CDPSrvr::SendGuildBankOutputItemReq( const CGuild* pGuild, const u_long idPlayer, const FLItemElem & kOutputItemElem )
{
	if( pGuild == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pGuild is NULL. PLAYER_ID(%07d)" ), idPlayer );
		return;
	}

	if( kOutputItemElem.IsEmpty() == TRUE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "kOutputItemElem is Empty. PLAYER_ID(%07d)" ), idPlayer );
		return;
	}

	FLPacketGuildBankOutputItemReq	toTrans;

	toTrans.dwGuildID				= pGuild->m_idGuild;
	toTrans.dwPlayerID				= idPlayer;
	toTrans.GuildBank.Copy( pGuild->m_GuildBank );
	toTrans.kOutputItemElem			= kOutputItemElem;

	g_dpDBClient.SendPacket( &toTrans );
}

void	CDPSrvr::SendGuildBankOutputGoldReq( const CGuild* pGuild, const u_long idPlayer, const int nOutputPenya, const bool bUpdateContribution )
{
	if( pGuild == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pGuild is NULL. PLAYER_ID(%07d)" ), idPlayer );
		return;
	}

	if( nOutputPenya <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nOutputPenya is underflow. PLAYER_ID(%07d), GOLD(%d)" ), idPlayer, nOutputPenya );
		return;
	}

	FLPacketGuildBankOutputGoldReq	toTrans;

	toTrans.dwGuildID				= pGuild->m_idGuild;
	toTrans.dwPlayerID				= idPlayer;
	toTrans.dwGuildGold				= pGuild->m_nGoldGuild;
	toTrans.dwOutputGold			= nOutputPenya;
	toTrans.bUpdateContribution		= bUpdateContribution;

	g_dpDBClient.SendPacket( &toTrans );
}
#ifdef PAT_LOOTOPTION
void	CDPSrvr::OnPetLootOption( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( pUser->IsDie() == TRUE )
	{
		return;
	}

	int nLootType;
	ar >> nLootType;

	((CMover*)pUser)->m_nPetLootType = nLootType;

}
#endif // PAT_LOOTOPTION