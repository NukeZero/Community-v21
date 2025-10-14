#ifndef __USER_H__
#define __USER_H__

#pragma once

#include <MsgHdr.h>
#include "Snapshot.h"
#include <UserTaskBar.h>
#include <Party.h>
#include <rtmessenger.h>
#include <misc.h>
#include <guild.h>
#include <eveschool.h>
#include <Chatting.h>
#include <CooltimeMgr.h>

#include <playerdata.h>

#include <SecretRoom.h>
#include <RainbowRace.h>
#include <Housing.h>
#include <PCBang.h>
class CPCBangInfo;

#include <GuildHouse.h>

#include "CampusHelper.h"
#include "../_Common/InstanceDungeonBase.h"

#include "../_CommonDefine/FLPacketStruct.h"

#define GETID( pCtrl )		( (pCtrl) ? (pCtrl)->GetId() : NULL_ID )

#define	MAX_INPUT_LEN	100

class	CShip;
class CMail;
class CMailBox;

//////////////////////////////////////////////////////////////////////////
#define CHECK_TICK_FROM_CLIENT	SEC(3)
//////////////////////////////////////////////////////////////////////////



namespace nsCooperativeContributions		{	
	class FLContribution;
};


#include "FLConsignmentSale.h"


class CLordSkill;
/// 플레이어를 나타내는 클래스 

// typedef	struct	_T_HACK_CHECK_DATA
// {
// 	DWORD	dwLastAttackTick;
// 
// } T_HACK_CHECK_DATA, *PT_HACK_CHECK_DATA;
// 
// const DWORD		VALID_PLAYER_MOVE_TICK		= 100;

struct T_MENU_INPUT_CHECK_DATA
{
	DWORD	dwLastMenuInputTick;

	T_MENU_INPUT_CHECK_DATA() : dwLastMenuInputTick( 0 )
	{
	}
};

class FLWSUser : public CMover
{
public:
	FLWSUser();
	FLWSUser( DPID dpidCache, DPID dpidUser );
	virtual	~FLWSUser();

	//////////////////////////////////////////////////////////////////////////

#ifdef __AGGRO16
public:
	BOOL			m_bDisplayAggro;
#endif // __AGGRO16
	FLConsignmentSale::T_USER_DATA	m_kConsignmentSaleData;

private:
	BOOL			m_bCheckTransMailBox;
	int				m_nCountFromClient;
	DWORD			m_dwTickFromClient;

	//////////////////////////////////////////////////////////////////////////

public:
	//////////////////////////////////////////////////////////////////////////
//	T_HACK_CHECK_DATA	m_tHackCheckData;

	T_MENU_INPUT_CHECK_DATA	m_kMenuInputCheck;


	DWORD			m_dwSerial;							/// 캐쉬서버가 발급한 유저의 순차적 번호
	BOOL			m_bValid;							/// db server에서 join packet받고, 맵에 Add된후 TRUE
	CSnapshot		m_Snapshot;							/// SNAPSHOTTYPE_류 송신 누적 버퍼( Notify()에서 보내진다. )
	std::map<DWORD, CCtrl*>	m_2npc;							/// 주변 NPC를 담고 있는 container
	DWORD			m_dwTickSFS;						/// SubSMMode, Finite, SavePlyer()용 타이머 
	DWORD			m_dwTickNotify;						/// Notify()용 타이머 
	DWORD			m_dwTimeout4Save;					/// db 저장용 타이머 
	DWORD			m_dwLeavePenatyTime;				/// 종료요청 시각 	
	DWORD			m_dwDestroyTime;					/// Destory예약 시각 

	DWORD			m_dwAuthKey;						/// 인증용 key
	PLAY_ACCOUNT	m_playAccount;						/// Account정보 
	CUserTaskBar	m_playTaskBar;						/// player의 taskbar class(큇슬롯)
	OBJID			m_idSetTarget;						/// 이 유저가 잡고있는 타겟
	CRTMessenger	m_RTMessenger;
	char			m_szPartyName[32];					/// 파티 이름 
	char			m_szBankPass[5];					/// 은행 비빌번호 

	int				m_nOverHeal;						/// 오버힐 한 어시에게는 이 카운트가 붙는다.	
	CObj			*m_pWall[2];						/// 사이킥 월을 생성하면 여기에 저장

	u_long			m_idChatting;						/// 채팅방에 들어갔을 때의 채팅방 id
	u_long			m_idSnoop;							/// 나를 감청하는자 id 
	BOOL			m_bInstantBank;						/// 즉시 사용 뱅크 사용중? 
	CCooltimeMgr	m_cooltimeMgr;						/// 아이템 쿨타임 관리자 
	DWORD			m_tmEscape;							/// 탈출 인터페이스 시작시각
	D3DXVECTOR3		m_vReturnPos;						/// '귀환 두루마리' - 목표지점  
	DWORD			m_dwReturnWorldID;					/// '귀환 두루마리' - 월드 id
	LPCTSTR			m_lpszVillage;						/// '귀환 두루마리' - 마을 이름 

	DWORD			m_dwPlayerTime;						/// DB저장용 총 플레이 시간 
	DWORD			m_dwSavePlayerTime;					/// 마지막 저장 플레이 시간의 총합 

//	OBJID			m_idTargetCollect;					/// 자원몬스터 채집대상 - 채집중에 타겟을 바꿀 수도 있기때문
//	int				m_nCollect;							/// 현재 자원 채집량
//	DWORD			m_tmCollect;						/// 자원채집 타이머 - 1초에 한번씩

	__int64	m_nEventFlag;
	DWORD	m_dwEventTime;	// 분 단위 계수
	DWORD	m_dwEventElapsed;	// 경과 일
	__int64	GetEventFlagBit( int nBit );
	int		SetEventFlagBit( int nBit );
	void	AddCallTheRoll( void );

	DWORD	m_tmDuelRequest;

	DWORD	m_tickScript;		// 스크립트 패킷 처리 제한

	BOOL	m_bAllAction;
	
	int		m_nKawibawiboWin;
	int		m_nKawibawiboState;
	int		m_nBetFiveSystem[6];
	
	int		m_nExpLog;
	int		m_nAngelExpLog;

	int		m_nCoupon;
	DWORD	m_dwTickCoupon;

#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	m_dwTickKeepConnect;
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __PERIN_BUY_BUG
	DWORD	m_dwLastTryBuyItem;
	DWORD	m_dwLastBuyItemTick;
#endif // __PERIN_BUY_BUG

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100723 give coupon item event

	DWORD	m_dwLastInputCouponNumberTick;

	// mirchang_100723 give coupon item event
	//////////////////////////////////////////////////////////////////////////

public:
	virtual void	Process();
	BOOL			TryRemoveNotUsingItem( const DWORD dwItemID, const int nItemCount );		//not using에는 장착개념도 포함
	void			DoSMItemEquip( FLItemElem* pItemElem, DWORD dwParts );
	void			DoSMItemUnEquip( FLItemElem* pItemElem, DWORD dwParts );
	void			CheckFiniteItem();
	void			LevelUpSetting( void );
	void			ExpUpSetting( void );
	void			SetValid( BOOL bValid ) { m_bValid = bValid; }
	BOOL			IsValid();
	void			SetReturnPos( const D3DXVECTOR3& vPos );
	D3DXVECTOR3&	GetReturnPos( DWORD* pdwWorldID );
	void			SetPlayerTime( DWORD dwSaveTime ) { m_dwPlayerTime = timeGetTime(); m_dwSavePlayerTime = dwSaveTime; };
	DWORD			GetPlayerTime() { return (((timeGetTime() - m_dwPlayerTime) / 1000) + m_dwSavePlayerTime ); };

	DWORD			GetKeepConnectionMinute() const	{	return ( ( timeGetTime() - m_dwPlayerTime ) / ( 1000 * 60 ) );	};

	//void			NPCSetAt( OBJID objid, CCtrl* pCtrl )	{	m_2npc[objid] = pCtrl;	}
	//BOOL			NPCRemoveKey( OBJID objid )		{	return m_2npc.erase( objid ) > 0; }
	//void			RemoveItFromView2( BOOL bRemoveall = FALSE );

	void			Open( DWORD dwWorldId );
//	void			Notify();
	int		Notify( void );													//Snapshot을 보낸다.
	//int				FlushSnapshot();


//	void			Flush()	{	Notify();	m_Snapshot.Flush();		}

	void			SetAllAction();
	void			TryRevival( const float fHPMPFPRate );


	void			OnRemoveGuildMember();


	void			AddRemoveMail( u_long nMail, int nType );
	void			AddMailBox( CMailBox* pMailBox );

	//////////////////////////////////////////////////////////////////////////
    void			SendCheckMailBoxReq( BOOL bCheckTransMailBox );
	void			CheckTransMailBox( BOOL bCheckTransMailBox );
	BOOL			GetCheckTransMailBox();
	bool			CheckClientReq();
	void			ResetCheckClientReq();
	int				GetCountClientReq();
	//////////////////////////////////////////////////////////////////////////

	BOOL			IsUsableState( DWORD dwId );

	void			OnMsgArrival( DWORD dwParam );
	void			ADDGameJoin();
	void			AddHdr( OBJID objid, int nPacketType );
	void			AddBlock( LPBYTE lpBlock, u_long uBlockSize );
	void			AddAddObj( CCtrl* pCtrl );
	void			AddRemoveObj( OBJID objid );
	void			AddDiagText( LPCSTR lpszText );
	void			AddText( LPCSTR lpszText );
	void			AddReplace( DWORD dwWorldID, D3DXVECTOR3 & vPos );
	void			AddCreateItem( FLItemBase* pItemBase, DWORD dwItemObjID, int nQuantity );
	void			AddMoveItem( DWORD dwSrcObjIndex, DWORD dwDestObjIndex );
#ifdef INVENTORY_ITEM_ALIGN
	void			AddAlignItem( DWORD dwIndex );
#endif	// INVENTORY_ITEM_ALIGN
	void			AddTrade( FLWSUser* pTrader, u_long uidPlayer );
	void			AddComfirmTrade( OBJID objid );
	void			AddComfirmTradeCancel( OBJID objid );
	void			AddTradePut( OBJID objid, BYTE byNth, BYTE byType, DWORD dwItemObjID, int nItemNum = 1 );
	void			AddTradePutError();
	void			AddTradePull( OBJID objid, BYTE byNth );
	void			AddTradePutGold( OBJID objid, DWORD dwGold );
//raiders.2006.11.28
//	void			AddTradeClearGold( OBJID objid );
	void			AddTradeCancel( OBJID objid, u_long uidPlayer, int nMode = 0 );
	void			AddTradeOk( OBJID objid )	{	AddHdr( objid, SNAPSHOTTYPE_TRADEOK );	}
	void			AddTradeConsent( CAr& ar );
	void			AddTradelastConfirm()	{	AddHdr( NULL_ID, SNAPSHOTTYPE_TRADELASTCONFIRM );	}
	void			AddTradelastConfirmOk( OBJID objid )	{	AddHdr( objid, SNAPSHOTTYPE_TRADELASTCONFIRMOK );	}
	void			AddOpenShopWnd( CMover* pVendor );

	void			AddUpdateItem( CHAR cType, DWORD dwId, CHAR cParam, DWORD dwValue, DWORD dwTime );

	void			AddUpdateItemEx( unsigned char id, char cParam, __int64 iValue );

	void			AddQuePetResurrectionResult( BOOL bResult );

	void			AddPocketAttribute( int nAttribute, int nPocket, int nData );
	void			AddPocketView( void );
	void			AddPocketAddItem( int nPocket, FLItemElem* pItem );
	void			AddPocketRemoveItem( int nPocket, DWORD dwItemObjID, int nNum, BOOL bExpiration );

	void			AddSetFxp( int nFxp, int nFlightLv );
	void			AddSetGrowthLearningPoint( long nRemainGP );
	void			AddSetStatLevel( CHAR chID, long nValue, long nRemainGP );
	void			AddDefinedText( int dwText, LPCSTR lpszFormat, ... );
	void			AddChatText( int dwText, LPCSTR lpszFormat, ... );
	void			AddDefinedText( int dwText );
	void			AddDefinedCaption( BOOL bSmall, int dwText, LPCSTR lpszFormat, ... );
	void			AddGoldText( int nPlus );
	void			AddExpBoxInfo( OBJID objid, DWORD dwSet, DWORD dwTime, u_long idPlayer );
	void			AddSetQuest( LPQUEST lpQuest );
	void			AddSetGuildQuest( int nQuestId, int nState );
	void			AddRemoveGuildQuest( int nQuestId );
	void			AddSetChangeJob( int nJob );
	void			AddReturnSay( int ReturnFlag, const CHAR* lpszPlayer );
	void			AddGameTimer( double dCurrentTime );
	void			AddDoEquip( DWORD dwItemObjID, DWORD dwItemId, BYTE fEquip );
	void			AddPartyChangeLeader( u_long uidChangeLeader );
	void			AddCancelQuest( DWORD dwQuestCancelID );
	void			AddRemoveQuest( DWORD dwQuestCancelID );
	void			AddRemoveAllQuest();
	void			AddRemoveCompleteQuest();
	void			AddGetItemBank( FLItemElem* pItemElem );	
	void			AddMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex );
	void			AddBankIsFull();
	void			AddRemoveGuildBankItem( u_long idGuild, DWORD dwId, DWORD dwItemNum );
	void			AddChangeBankPass( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddBankWindow( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddconfirmBankPass( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddTaskBar();
	void			AddSendErrorParty( DWORD dw, DWORD dwSkill = 0 );

	void			AddPartyMember( CParty *pParty, u_long idPlayer, const char* pszLeader, const char* pszMember );

	void			AddPartyRequest( FLWSUser * pLeader, FLWSUser * pMember, BOOL bTroup );
	void			AddPartyRequestCancel( u_long uLeaderid, u_long uMemberid, int nMode = 0 );
	void			AddPartyExpLevel( int Exp, int Level, int nPoint );

	void			AddPartyMemberFlightLevel( int nMemberIndex, int nLevel);
	void			AddPartyChangeTroup( const char * szPartyName );
	void			AddPartyChangeName( const char * szPartyName );

	void			AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint , DWORD dwSkillTime );

	void			AddPartyChangeItemMode( int nItemMode );
	void			AddPartyChangeExpMode( int nExpMode );
	void			AddSetPartyMemberParam( u_long idPlayer, BYTE nParam, int nVal );
	void			AddPartyName();
	void			AddPartySkillCall( const D3DXVECTOR3 &vLeader );
	void			AddPartySkillBlitz( OBJID idTarget );

	void			AddFriendGameJoin();
	void			AddFriendReqest( u_long uLeader, LONG nJob, BYTE nSex, const char * szName );
	void			AddFriendCancel();
	void			AddFriendError( BYTE nError, const char * szName );

	void			AddSeasonInfo();
	void			AddSeasonEffect();
	void			AddPartyChat( const CHAR* lpName, const CHAR* lpString, OBJID objid );
	void			AddAddFriend( u_long idPlayer, const char* lpszPlayer );
	void			AddRemoveFriend( u_long uidSender );
	void			AddDuelCount( int nCount );
	void			AddDuelRequest( u_long uidSrc, u_long uidDst );
	void			AddDuelStart( u_long uidTarget, int bStart = 0 );
	void			AddDuelNo( OBJID idTarget );
	void			AddDuelCancel( OBJID idTarget );	// 듀얼중 취소시킴
	void			AddDuelPartyRequest( u_long uidSrc, u_long uidDst );
	void			AddDuelPartyStart( LPCTSTR szPartyName, int nMax, OBJID *pMembers, u_long idParty, int bFlag = 0 );
	void			AddDuelPartyNo( OBJID idTarget );
	void			AddDuelPartyCancel( CParty *pDuelOther );	// 듀얼중 취소시킴
	void			AddDuelPartyResult( CParty *pDuelOther, BOOL bWin );		// 듀얼결과 통보.

	void			AddQueryPlayerData( u_long idPlayer, PlayerData* pPlayerData );
	void			AddPlayerData( void );

	void			AddSetSkill( DWORD dwSkill, DWORD dwSkillLevel );
// 	void			AddQueryGetPos( OBJID objid );
	void			AddGetPos( OBJID objid, const D3DXVECTOR3 & vPos, float fAngle );
	void			AddRevivalBySkillMessage();
	void			AddCorrReq( CMover *pMover );
	void			AddSetFuel( int nFuel, DWORD tmAccFuel );
	void			AddTag( short nTagCount, const TAG_ENTRY* tags );
	void			AddSetSkillState( CMover *pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime );
	void			AddPlaySound( u_long idSound );
	void			AddMoverFocus( CMover *pMover );
	void			AddPlayMusic( u_long idMusic );
	void			AddRegisterPVendorItem( BYTE byNth, BYTE byType, DWORD dwItemObjID, int nNum, int nCost );
	void			AddUnregisterPVendorItem( BYTE byNth );
	void			AddPVendorItem( FLWSUser* pUser, BOOL bState );	// locked
	void			AddPVendorClose( OBJID objidVendor );
	void			AddMotionError();
	void			AddPutItemBank( BYTE nSlot, FLItemElem* pItemElem );
	void			AddPutGoldBank( BYTE nSlot, DWORD dwGold, DWORD dwGoldBank );
	void			AddUpdateBankItem( BYTE nSlot, DWORD dwItemObjID, CHAR cParam, DWORD dwValue );
	void			AddGuildBankWindow(int nMode);
	void			AddPutItemGuildBank(FLItemElem* pItemElem);
	void			AddGetItemGuildBank( FLItemElem* pItemElem );
	void			AddGetGoldGuildBank( DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak );
	void			AddContribution( CONTRIBUTION_CHANGED_INFO& info );
	void			AddSetNotice( u_long idGuild, const char* szNotice );
	void			AddInsertedVote( VOTE_INSERTED_INFO& info );
	void			AddModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra );
	void			AddSetGuildAuthority( DWORD dwAuthority[] );
	void			AddSetGuildPenya( DWORD dwType, DWORD dwPenya );
	void			AddGuildRealPenya( int nGoldGuild, int nType );

	void			AddAllGuilds();
	void			AddMyGuild();
	void			AddGuildInvite( u_long idGuild, u_long idMaster );
	void			AddMyGuildWar();
	void			SendGuildRank();
	void			AddSMMode( int nType, DWORD dwTime );
	void			AddSMModeAll();
	void			AddResistSMMode( BYTE nAttackResistLeft, BYTE nAttackResistRight, BYTE nDefenceResist );
	void			AddCommercialElem( DWORD dwItemId, int nResistSMItemId );
	void			AddFlyffEvent();

	void			AddEventLuaDesc( int nState, string strDesc );

	void			AddQueryGetDestObj( OBJID objid );
	void			AddGetDestObj( OBJID objid, OBJID objidDest, FLOAT fRange );
//	BOOL			DoCollect( CMover *pTarget );		// 채집시작.
//	int				OnActCollecting();					// 채집중 계속 호출.
	void			AddCommonSkill( DWORD dwSkill, DWORD dwLevel );
	void			AddExpBoxCoolTime( OBJID objid, DWORD dwTime );
	void			AddPlaySound2( const char* szSound );
	void			AddRunScriptFunc( const RunScriptFunc & runScriptFunc );
	void			AddRemoveSkillInfluence( WORD wType, WORD wID );
	void			AddGCWindow( __int64 dwPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya );
	void			AddGCRequestStatus( __int64 nPrizePenya, std::vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus );
	void			AddGCSelectPlayerWindow( std::vector<CGuildCombat::__JOINPLAYER> &vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar );
	void			AddGCJoinWarWindow( int nMap, int nTelTime );
	void			AddGuildCombatNextTime( DWORD dwTime, DWORD dwState );
	void			AddGuildCombatEnterTime( DWORD dwTime );
	void			AddGCWinGuild( void );
	void			AddGCBestPlayer( void );
	void			AddGCIsRequest( BOOL bRequest );
	void			AddGCGetPenyaGuild( int nGetResult, __int64 nGetPenya );
	void			AddGCGetPenyaPlayer( int nGetResult, __int64 nGetPenya );
	void			AddGCDiagMessage( const char* str );
	void			AddGCTele( const char* str );
	void			AddGCWarPlayerlist( void );
	void			AddCtrlCoolTimeCancel( void );
	void			AddGuildCombatState( void );
	void			AddQuestTextTime( BOOL bFlag, int nState, DWORD dwTime );
	void			AddGCLog( void );
	void			AddGCPlayerPoint( void );
	void			AddFocusObj(OBJID objid);
	void			AddEnterChatting( FLWSUser* pUser );
	void			AddNewChatting( CChatting* pChatting );
	void			AddRemoveChatting( u_long uidPlayer );
	void			AddDeleteChatting( void );
	void			AddChatting( u_long uidPlayer, char* pszChat );
	void			AddChttingRoomState( BOOL bState );
	void			AddGameRate( FLOAT fRate, BYTE nFlag );
	void			AddGameSetting();
	void			AddInitSkill();	
	void			AddSetNaviPoint( const NaviPoint & nv, OBJID objid, const char* Name );
	void			AddEscape( DWORD tmMaxEscape );
	void			AddSetActionPoint( int nAP );	
	void			AddSetTarget( OBJID idTarget );
	void			AddSetDuel( CMover* pMover );

	void			AddPKValue();

	void			ScheduleDestory( DWORD dwDestoryTime  );
	void			AddWantedInfo( const D3DXVECTOR3& vPos, BYTE byOnline, DWORD dwWorldID, LPCTSTR lpszWorld );
	void			AddReturnScroll();
	void			AddSnoop( const char* lpString );
	void			AddSetCheerParam( int nCheerPoint, DWORD dwRest, BOOL bAdd = FALSE );
	void			AddQueryEquip( FLWSUser* pUser );
	void			AddSummonFriendUse( FLItemElem* pItemElem );
	void			AddSummonFriendConfirm( OBJID objid, DWORD dwData, const char* szName, const char* szWorldName );
	void			AddSummonPartyConfirm( OBJID objid, DWORD dwData, const char * szWorldName );

	void			AddDoUseSkillPoint( SKILL aJobSkill[], int nSkillPoint );
	void			AddPartyMapInfo( int nIndex, D3DXVECTOR3 vPos );
	void			AddSetExperience( EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel, EXPINTEGER nDeathExp = (EXPINTEGER)-1, WORD wDeathLevel = 0xffff );

	void			AddAllAction( BOOL bCharacter );

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM

//	void			AddCreateAngel(BOOL isSuccess, char* createAngel);
	void			AddAngelInfo( BOOL bComplete = FALSE );
	
	void			AddKawibawiboResult( int nResult, int nWinCount, DWORD dwItemId = 0, int nItemCount = 0, DWORD dwNextItemId = 0, int nNextItemCount = 0 );
	void			AddReassembleResult( DWORD dwItemId, int nItemCount );
	void			AddReassembleOpenWnd( std::vector<DWORD> vecItemId );
	void			AddAlphabetOpenWnd( int nQuestionID, int nPublicAlphabet, char chPublicChar, CString strQuestion );
	void			AddAlphabetResult( int nResult, DWORD dwItemId = 0, int nItemCount = 0 );
	void			AddFiveSystemOpenWnd( int nBetMinPenya, int nBetMaxPenya, int nMultiple );
	void			AddFiveSystemResult( int nResult, int nGetPenya = 0, int nBetNum = 0 );

	void			AddUltimateMakeItem( BOOL bResult );
	void			AddUltimateMakeGem( int nResult, int nNum );
	void			AddUltimateWeapon( BYTE state, int nResult );

	void			AddExchangeResult( BYTE state, int nResult );

	void	AddPet( CPet* pPet, BYTE nPetLevelup = PF_PET );
	void	AddPetState( DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp );
	void	AddPetSetExp( DWORD dwExp );
	void	AddPetFoodMill( int nResult, int nCount );

	void			AddLegendSkillResult( int nResult );
	
	void			AddRemoveAttribute( BOOL bSuccess );

	virtual	void	ProcessCollecting( void );
	virtual	void	StartCollecting( void );
	virtual	void	StopCollecting( void );
	BOOL	DoUseItemBattery( void );

	void	AddRestartCollecting( DWORD dwItemId );
	int		m_nCollecting;

	BOOL	HasInput()	{	return m_szInput[0] != '\0';	}
	void	ResetInput()	{	m_szInput[0]	= '\0';	}
	void	SetInput( char* szInput )		{	FLStrncpy( m_szInput, _countof( m_szInput ), szInput, MAX_INPUT_LEN );	}
	char*	GetInput()	{	return m_szInput;	}

	void	SummonPlayer( FLWSUser* pDestUser );

public:

	void	AddGC1to1TenderOpenWnd( int nPenya );
	void	AddGC1to1TenderGuildView(  int nPenya, int nRanking, time_t t, std::vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecTenderGuild );
	void	AddGC1to1MemberLineUpOpenWnd( std::vector<u_long>& vecMemberId );
	void	AddGC1to1NowState( int nState, int nTime, int nProgCount = 0 );
	void	AddGC1to1WarResult( int m_nState, u_long uIdPlayer, int nWinCount0, int nWinCount1 );

	void	RemoveAngel( void );

	void	AddGuildBankLogView( BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs );

	void	AddSealChar( short nLogCount, const __SEALCHAR_ENTRY* seals );
	void	AddSealCharSet();

	void	AddHonorListAck();

	void	AddCoupon( int nCoupon );

	void	AddSecretRoomMngState( int nState, DWORD dwRemainTime );
	void	AddSecretRoomContInfo( BYTE nContinent, BYTE nType, std::vector<__SECRETROOM_TENDER> & vecSecreetRoomTender, int nIndex );
	void	AddSecretRoomTenderOpenWnd( int nTenderPenya );
	void	AddSecretRoomLineUpOpenWnd( std::vector<DWORD>& vecLineUpMember );
	void	AddSecretRoomTenderView( int nTenderPenya, int nRanking, time_t t, std::vector<__SECRETROOM_TENDER>& vecSRTender );

	void	AddLord( void );
	void	AddLordSkillTick( CLordSkill* pSkills );
	int		GetMuteText( void );
	void	SetElection( u_long idElection )	{	m_idElection	= idElection;	}
	u_long	GetElection( void )	{	return m_idElection;	}
	BOOL	IsTeleportable( void );
	BOOL	IsQuerying( void )	{	return m_bQuerying;		}
	void	SetQuerying( BOOL bQuerying )	{	m_bQuerying		= bQuerying;	}

	void	AddSetTutorialState( void );

	void	AddTaxInfo( void );
	void	AddTaxSetTaxRateOpenWnd( BYTE nCont );

	BOOL	IsGuildMaster();

	void	ValidateItem( void );
	void	AdjustGuildQuest( DWORD dwWorldId );
	void	AdjustMailboxState( void );
	void	AddEventLuaDesc( void );
	void	AdjustPartyQuest( DWORD dwWorldId );
//	void	AdjustAuthority();

public:
	MEMPOOLER_DECLARE( FLWSUser );

	void AddRainbowRacePrevRankingOpenWnd();
	void AddRainbowRaceApplicationOpenWnd();
	void AddRainbowRaceState( CRainbowRace* pRainbowRace );
	void AddMiniGameState( __MINIGAME_PACKET MiniGamePacket );
	void AddMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket );

	void AddHousingAllInfo( CHousing* pHousing );
	void AddHousingSetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd );
	void AddHousingSetupFurniture( HOUSINGINFO* pHousingInfo );
	void AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup );
	void AddHousingSetVisitAllow( DWORD dwTargetId, BOOL bAllow );
	void AddHousingVisitableList( std::vector<DWORD> & vecVisitable );

	void AddNPCPos( const D3DXVECTOR3 & vPos );

	void AddClearTarget();

private:
	void			Init( DPID dpidCache = DPID_UNKNOWN, DPID dpidUser = DPID_UNKNOWN );

	u_long	m_idElection;
	BOOL	m_bQuerying;

	char	m_szInput[MAX_INPUT_LEN];

public:
	void	SetProposer( u_long idProposer )	{	m_idProposer	= idProposer;	}
	u_long	GetProposer()	{	return m_idProposer;	}
	void	AddCouple();
	void	AddProposeResult( u_long idProposer, const char* pszProposer );
	void	AddCoupleResult( u_long idPartner, const char* pszPartner );
	void	AddDecoupleResult();

	void	ProcessCouple();
	void	ActiveCoupleBuff( int nLevel );
	void	AddAddCoupleExperience( int nExperience );

	void	AddPCBangInfo( CPCBangInfo* pPI );

	void	AddAccountPlayTime();

	void AddSmeltSafety( BYTE nResult );

	void AddWorldReadInfo( DWORD dwWorldId, D3DXVECTOR3 vPos );

	void AddQuizEventState( int nType, int nState );
	void AddQuizEventMessage( const char* lpszMessage, int nState = 0 );
	void AddQuizQuestion( int nType, const char* lpszQuestion, int nCount );

	void AddActivateVisPet( OBJID objIdVisPetItem, OBJID objIdVisPetId );

	void AddCheckedQuest();

public:
	void	AddGuildHousePakcet( int nPacketType, GH_Fntr_Info & gfi, int nIndex );
	void	AddGuildHouseAllInfo( CGuildHouseBase* pGuildHouse );
	void	AddGuildHouseRemvoe();
	int		GetRestPoint()						{ return m_nRestPoint; }
	int		SetIncRestPoint( int nRestPoint )	{ return SetAddRestPoint( (int)( nRestPoint * static_cast<float>( GetParam( DST_RESTPOINT_RATE, 0 ) / 100.0f ) ) ); }
	int		SetDecRestPoint( int nRestPoint )	{ return SetAddRestPoint( -nRestPoint ); }
	void	SetRestPoint( const int nRestPoint );
	void	SetRestPointTick()					{ m_tRestPointTick = time_null() + REST_POINT_TICK; }
	EXPINTEGER	GetAddExpAfterApplyRestPoint( EXPINTEGER nExp );

private:
	int		SetAddRestPoint( int nRestPoint );
	void	AddRestPoint();
	
	int		m_nRestPoint;
	int		m_nPrevRestPoint;
	time_t	m_tRestPointTick;

public:
	void	AddInviteCampusMember( FLWSUser* pRequest );
	void	AddUpdateCampus( CCampus* pCampus );
	void	AddRemoveCampus( u_long idCampus );
	void	AddUpdateCampusPoint( int nCampusPoint );
	void	ProcessCampus();
	void	ActiveCampusBuff( int nLevel );
	DWORD	GetCampusTick()	{	return m_dwCampusTick;	}
	void	SetCampusTick( DWORD dwTick )		{	m_dwCampusTick = dwTick;	}
	
	int		GetPointByType( int nType );	// 100122 define.h 에 정의되어 있는 포인트 타입에 따른 유저의 포인트 리턴

private:
	DWORD	m_dwCampusTick;

#ifdef __GUILD_HOUSE_MIDDLE
public:
	void	AddGuildHouseTenderMainWnd( std::map<DWORD, DWORD> & mapGHTenderList );
	void	AddGuildHouseTenderInfoWnd( DWORD dwGHIndex, int nTenderMinPenya, __int64 n64TenderPenya, std::vector<DWORD>& vecGuildId );
	void	AddGuildHouseTenderResult( DWORD dwGHIndex, BOOL bResult );
	void	AddGuildHouseInfoWnd( const PT_PACKET_GUILDHOUSE_INFO ptGHInfo );
#endif // __GUILD_HOUSE_MIDDLE

#ifdef __HYPERLINK_ITEM16

public:
	void	AddItemLinkInfo( const PT_PACKET_ITEM_LINK ptItemLink, FLItemElem* pItemElem );

#endif  __HYPERLINK_ITEM16

#ifdef __ENCHANT_BARUNA16
public:
	void	AddSummonBarunaNPC();
	void	AddBarunaNPCInfo(/* const DWORD dwOldLevel */);
	void	AddReleaseBarunaNPC( const OBJID NPCOBJID );

	void	AddResultOperExtract( const PT_PACKET_OPER_EXTRACT ptOperExtract );
	void	AddResultOperCreate( const PT_PACKET_OPER_CREATE ptOperCreate );
	void	AddResultCidCreate( const PT_PACKET_CID_CREATE ptCidCreate );
	void	AddResultCidPieceUpgrade( const PT_PACKET_CIDPIECE_UPGRADE ptCidPieceUpgrade );
	void	AddResultOperCidCreate( const PT_PACKET_OPERCID_CREATE ptOperCidCreate );

private:
	BOOL getBarunaNPCInfo( /* const DWORD dwOldLevel, */DWORD & o_dwExp, DWORD & o_dwLevel, DWORD & o_dwExpRatio );

#endif // __ENCHANT_BARUNA16

public:
	//////////////////////////////////////////////////////////////////////////
	// shopping basket
	void	AddResultBuyItem( const PT_PACKET_SHOPPING_BASKET ptBasket );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// reset bind
	void	AddResultResetBind( const PT_PACKET_RESET_BIND ptResetBind );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// item merge
	void	AddResultItemMerge( const PT_PACKET_ITEM_MERGE ptItemMerge );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// packet send
	void	AddPacket( const FLPacket* pPacket );
	//////////////////////////////////////////////////////////////////////////

	//협동 기부
	void	AddCooperativeContributions_Info( nsCooperativeContributions::FLContribution & contribution );
    void	AddCooperativeContributions_Contribute( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::E_RESULT eResult );
	void	AddCooperativeContributions_GetRanking( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::E_RESULT eResult, const T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_ACK * pRanking );

	void	AddCooperativeContributions_SetGlobalBuffReward( const E_REWARD_TYPE eRewardType );

	//협동 기부

	//콜로세움
	void	AddColosseum_Enter( const T_PACKET_COLOSSEUM_ENTER_ACK::E_RESULT eResult );
	void	AddColosseum_Retry( const T_PACKET_COLOSSEUM_RETRY_ACK::E_RESULT eResult );

	void	AddColosseum_AutoInviteReq( const char * szInvitePlayerName, const DWORD dwWorldID );
	void	AddColosseum_AutoStartWaitingTick( const DWORD dwRemainTick );
	void	AddColosseum_StageWaitingTick( const int nStage, const DWORD dwRemainTick );

	void	AddColosseum_StatedStage( const int nStage, const DWORD dwTimeLimitRemainTick );
	void	AddColosseum_ClearedStage( const int nStage );
	void	AddColosseum_StageTimeLimit( const int nStage, const DWORD dwRetryRemainTick );
	void	AddColosseum_AllDie( const int nStage, const DWORD dwRetryRemainTick );

	void	AddColosseum_Retried( const int nStage, const DWORD dwRetryStageWaitingTick );
//	void	AddColosseum_AllClear( const DWORD dwRemainTick, const DWORD dwCompleteTimeTick, CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec & vecReward, const int nWeeklyRank = 0 );
	void	AddColosseum_AllClear( const T_PACKET_COLOSSEUM_ALL_CLEAR_HEADER & toClient );
	//void	AddColosseum_Kick();

	void	AddColosseum_RegistAdditionalRealMonsterRateItemAck( const BOOL bSuccess );
	void	AddColosseum_UnRegistAdditionalRealMonsterRateItemAck( const BOOL bSuccess );
	void	AddColosseum_RegistAdditionalRealMonsterRateItemNoti( const DWORD dwItemID );

	void	AddColosseum_ColosseumRanking( const T_PACKET_COLOSSEUM_GET_RANKING_INFO & ranking );

	//void	AddColosseum_RewardInfo( CInstanceDungeonBase::DUNGEON_DATA::RewardVec & vecReward );
	//void	AddColosseum_RewardItemInfo( CInstanceDungeonBase::DUNGEON_DATA::RewardVec & vecReward );
	//-콜로세움

	//////////////////////////////////////////////////////////////////////////
	void	AddCombinedOption( FLItemElem* pItemElem );
	//////////////////////////////////////////////////////////////////////////

	void	Add_SellItemToNPC_Ack( const T_SELL_ITEM_TO_NPC_ACK & toClient );
#ifdef PAT_LOOTOPTION
	void			AddPetLootOption();
#endif // PAT_LOOTOPTION
private:
	u_long	m_idProposer;
	int		m_cbProcessCouple;

	//	mulcom	BEGIN100405	각성 보호의 두루마리
public:
	void	SelectAwakeningValue( DWORD dwItemObjID, SERIALNUMBER iSerialNumber, BYTE bySelectFlag );
	//	mulcom	END100405	각성 보호의 두루마리

	BOOL	ReallocSerialNumber();

	void	RegisterItemSerialNumber();
	void	UnregisterItemSerialNumber();

//	DWORD	CheckValidMove( const D3DXVECTOR3 & vPos );

	enum {	MOVE_NONE = 0, MOVE_INVALID, MOVE_VALID	};

	bool	CanTeleportState( const T_WORLD_POSITION & kPos );
	void	TeleportToNPC( const DWORD dwNPC_ObjID, const int nMMI_ID );

	void	AddActionSlotSkillSkip_HardCording( const int nSkillUseType );

	void	AddBuffAdjValueRefresh( const DWORD dwItemKind3, const int nAdjValue );


	/************************************************************************/
#ifdef __WORLDSERVER
	private:
		virtual float	GetRateRecoveryRevival();
		virtual bool	GetDecreaseExperienceRevival( nsRevival::DECREASE_EXP_DATA& decreaseExpData ) const;
		virtual void	DoRecoveryRevival( const float recoveryRate );
		virtual bool	DoDecreaseExperience( const nsRevival::DECREASE_EXP_DATA& decreaseExpData, bool& downLevel );
	
	public:
		bool			GetRevivalPos( T_WORLD_POSITION& pos );
#endif // __WORLDSERVER
	/************************************************************************/
};

#define	SEC_SAVEPLAYER	900

class CDPSrvr;
class CLEComponent;
class ILordEvent;
	
class FLWSUserManager
{
	public:
		static	FLWSUserManager *	GetInstance();

	public:
		FLWSUserManager();
		virtual	~FLWSUserManager();

	private:
		long				m_lCount;
		std::map<DWORD, FLWSUser*>	m_users;

	private:
		void			RemoveUserFromCacheMsg( FLWSUser *pUser );

	public:
		void			DestroyPlayer( FLWSUser* pUser );
		LONG			GetCount()	{	return m_lCount;	}
		void			RemoveAllUsers();
		FLWSUser*			AddUser( DPID dpidCache, DPID dpidUser, DPID dpidSocket );
		void			RemoveUser( DWORD dwSerial );
		FLWSUser*			GetUser( DPID dpidCache, DPID dpidUser );
		FLWSUser*			GetUserByPlayerID( u_long idPlayer );
	//	void			Notify();
	#ifdef __LAYER_1015
		BOOL			AddPlayer( FLWSUser* pUser, DWORD dwWorldID, int nLayer );
	#else	// __LAYER_1015
		BOOL			AddPlayer( FLWSUser* pUser, DWORD dwWorldID );
	#endif	// __LAYER_1015
		void			AddDisguise( CMover* pMover, DWORD dwMoverIdx );
		void			AddNoDisguise( CMover* pMover );
		void			AddSetState( CMover* pMover, BYTE statDice = Stat::StatDice_None );
		void			AddSetStateLevel( CMover* pMover, CHAR chID, long nValue );
		void			AddHdr( CCtrl* pCtrl, int nPacketType );
		void			AddChat( CCtrl* pCtrl, const TCHAR* szChat );

		void			AddStartCollecting( FLWSUser* pUser );
		void			AddStopCollecting( FLWSUser* pUser );

		void			AddDlgEmoticon( CCtrl* pCtrl, int nIdx );
		void			AddDamage( CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags, DWORD dwFakeDamage = 0 );
		void			AddDoEquip( CMover* pMover, int nPart, DWORD dwItemObjID, const EQUIP_INFO & rEquipInfo, BYTE fEquip );
		void			AddDoEquip( CMover* pMover, int nPart, FLItemElem *pItemElem, BYTE fEquip );	// 간단 파라메터 버전.
		void			AddVendor( CMover* pVendor );
		void			AddUpdateVendor( CMover* pVendor, CHAR cTab, DWORD dwItemObjID, int nNum );
		void			AddSetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue );
		void			AddResetDestParam( CMover* pMover, int nDstParamter, int nAdjParameterValue );

		void			AddResetDestParamSync( CMover* pMover, int nDstParameter,int nAdjParameterValue, int nParameterValue );

		void			AddSetPointParam( CMover* pMover, int nDstParameter, int nValue );
		void			AddSetPos( CMover* pMover, const D3DXVECTOR3 & vPos );
		void			AddSetPosAngle( CCtrl* pCtrl, const D3DXVECTOR3 &vPos, FLOAT fAngle );
		void			AddSetLevel( CMover* pMover, WORD wLevel );
		void			AddSetFlightLevel( CMover* pMover, int nFlightLv );
		void			AddSetSkillLevel( CMover* pMover, DWORD dwSkill, DWORD dwLevel );
		void			AddSetScale( CMover* pMover, float fScalePercent );
		void			AddWorldShout( const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld );
		void			AddSetSkillState( CMover* pCenter, CMover *pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime  );
		void			AddShipActMsg( CMover* pMover, CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 );		
		void			AddMeleeAttack( CMover* pMover, OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
		void			AddMeleeAttack2( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
		void			AddMagicAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit );
		void			AddRangeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int idSfxHit );
		void			AddAttackSP( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 );		
		void			AddMoverSetDestObj( CMover* pMover, OBJID objid, float fRange = 0.0f, BOOL fTransferToMe = FALSE );
		void			AddMoverDeath( CMover* pMover, CMover* pAttacker, DWORD dwMsg );
		void			AddSetMovePattern( CMover* pMover, int nPattern, const D3DXVECTOR3 &vPos, FLOAT fAngle, FLOAT fAngleX );
		
		void			AddCreateSfxAllow( CMover *pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget );
		void			AddCreateSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x = 0, float y = 0, float z = 0, BOOL bFlag = FALSE );

		void			AddRemoveSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x = 0, float y = 0, float z = 0, BOOL bFlag = FALSE );

		void			AddNearSetChangeJob( CMover* pMover, int nJob, LPSKILL lpSkill );
		void			AddModifyMode( FLWSUser* pUser );
		void			AddStateMode( FLWSUser* pUser, BYTE nFlag );

		void			AddSetFame( CMover* pMover, int nFame );
		void			AddDefinedText( CMover* pMover, int dwText, LPCSTR lpszFormat, ... );
		void			AddBlock( LPBYTE lpBlock, u_long uBlockSize );
		void			AddBlock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld );
		void			AddBlock( CWorld* pWorld, const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize );
		void			AddPVendorOpen( FLWSUser* pUser );
		void			AddPVendorClose( FLWSUser* pUser );

		void			AddPVendorItemNum( FLWSUser* pUser, BYTE byNth, int nVend, const char* sBuyer );

		void			AddSetHair( FLWSUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b );

	//	void			AddDoCollect( FLWSUser* pUser, OBJID idTarget );
		void			AddCreateSkillEffect( CMover *pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel );
		void			AddSetStun( CMover *pMover, int nMaxTime );
		void			AddSendActMsg( CMover *pMover, OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
		void			AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fAngle, FLOAT fPushAngle, FLOAT fPower );
		void			AddRemoveSkillInfluence( CMover *pMover, WORD wType, WORD wID );
		void			AddDoApplySkill( CCtrl *pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel );
		void			AddWorldCreateSfxObj( DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId );
		void			AddMoverBehavior( CMover* pMover, BOOL bTransferToMe = FALSE );
		void			AddUseSkill( CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime );
//		void			AddEventMessage( CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId = 0);
		void			AddMotion( CMover* pMover, DWORD dwMsg );
		void			AddRemoveAllSkillInfluence( CMover* pMover );
		void			AddCreateGuild( u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild );
		void			AddDestroyGuild( const char* lpszPlayer, u_long idGuild );
		void			AddSetGuild( FLWSUser* pUser, u_long idGuild );
		void			AddSetLogo( u_long idGuild, DWORD dwLogo );
		void			AddGetItemElem( FLWSUser* pUser, FLItemElem* pItemElem );
		void			AddPutItemElem( FLWSUser* pUser, FLItemElem* pItemElem );

		void			AddPutItemElem( u_long uidGuild, FLItemElem* pItemElem );

		void			AddSetWar( FLWSUser* pUser, u_long idWar );
		void			ModifyMode( DWORD dwMode );
		void			AddSchoolReport( PSCHOOL_ENTRY pSchool, short nElapse = 0 );
		void			AddSexChange( CMover* pMover );
		void			AddSetLocalEvent( short id, BYTE nState );
		void			AddGameRate( FLOAT fRate, BYTE nFlag );
		void			AddChangeFace( u_long uidPlayer, DWORD dwFace );

		void			AddShout( FLWSUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize );

		void			AddWorldMsg( const CRect* pRect, LPCTSTR lpszString );
		void			AddGameSetting();
		void			AddCommonPlace( CCtrl* pCtrl, BYTE nType );
		void			OutputStatistics( void );
		void			AddGuildCombatNextTimeWorld( DWORD dwTime, DWORD dwState );
		void			AddGuildCombatState( void );
		void			AddGuildCombatUserState( CMover* pMover );
	#ifdef __LAYER_1015
		void			ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer );
		void			ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer );
	#else	// __LAYER_1015
		void			ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ );
		void			ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag );
	#endif	// __LAYER_1015
		void			AddWorldMsg( DWORD dwWorldId, LPCTSTR lpszString );
		void			AddGCGuildStatus( u_long uidGuild, FLWSUser* pSendUser = NULL );
		void			AddGCGuildPrecedence( FLWSUser* pSendUser = NULL );
		void			AddGCPlayerPrecedence( FLWSUser* pSendUser = NULL );
		void			AddGCWinGuild( void );
		void			AddGCBestPlayer( void );
		void			AddGCWarPlayerlist( u_long uidGuild = 0, FLWSUser* pSendUser = NULL  );
		void			AddGuildCombatEnterTime( DWORD dwTime );
		void			AddGCLogWorld( void );
		void			AddGCLogRealTimeWorld( CGuildCombat::__GCGETPOINT GCGetPoint );
		void			AddGCIsRequest( u_long uidGuild, BOOL bRequest );
		void			AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize );
		void			AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld );
		void			AddGuildMsg( u_long idGuild, LPCSTR lpsz );
		void			AddGuildMsg( CGuild* pGuild, LPCSTR lpsz );

	#ifdef __S1108_BACK_END_SYSTEM
		void			AddMonsterProp( );
		void			AddGMChat( int nSize );
	#endif // __S1108_BACK_END_SYSTEM

		void			AddMotionArrive( CMover* pMover, OBJMSG objmsg );

		void			AddPKPink( CMover* pMover, BYTE byPink );
		void			AddPKPropensity( CMover* pMover );
		
	#ifdef __IAOBJ0622
		void			AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward, OBJID objidIAObj = NULL_ID );
	#else	// __IAOBJ0622
		void			AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward );
	#endif	// __IAOBJ0622
		void	AddAddRegion( DWORD dwWorldId, REGIONELEM & re );

		void	CallTheRoll( int nBit );

		void	AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName );
		void	AddSetPetName( FLWSUser* pUser, const char* szPetName );
		void	AddPetRelease( CMover* pMover );
		void	AddPetLevelup( CMover* pMover, DWORD dwPetId );
		void	AddPetFeed( CMover* pMover, WORD wEnergy );

		void	AddHonorTitleChange( CMover* pMover, int nChange );

		void	AddSetSpeedFactor( CMover* pMover, FLOAT fSpeedFactor );
		
		void	AddEventLua( BYTE nId, BOOL bState );

		void	AddElectionBeginCandidacy( void );
		void	AddElectionBeginVote( int nRequirement );
		void	AddElectionEndVote( u_long idPlayer );
		void	AddElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
		void	AddElectionSetPledge( u_long idPlayer, const char* szPledge );
		void	AddElectionIncVote( u_long idPlayer, u_long idElector );
		void	AddLEventCreate( CLEComponent* pComponent );
		void	AddLEventInitialize( void );
		void	AddLEventTick( ILordEvent* pEvent );
		void	AddLordSkillUse( FLWSUser* pUser, u_long idTarget, int nSkill );
		void	AddQueryPlayerData( u_long idPlayer );

		void	AddTaxInfo( void );

		void	AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup, DWORD dwMasterId );

		void	ResetAccountPlayTime();

		void	AddChangeMoverSfxId( CMover* pMover );

		BOOL	HasUserSameWorldnLayer( FLWSUser* pUserSrc );

		void	AddSetSleeping( CMover *pMover, DWORD dwDuration );
		void	AddSeasonEffect();
};

#define g_xWSUserManager	FLWSUserManager::GetInstance()

#endif	// __USER_H__
