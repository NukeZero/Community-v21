#ifndef __DPCLIENT_H__
#define __DPCLIENT_H__

#include <network/FLDPMng.h>
#include <network/FLAr.h>
//#include "MsgHdr.h"		// 이거 include 시키지 말것. 빌드 느려짐.
#include "Obj.h"
#include "playerdata.h"		//sun: 11, 캐릭터 정보 통합
#include "RainbowRace.h"	//sun: 13, 레인보우 레이스
#include "Housing.h"		//sun: 13, 하우징 시스템


typedef	struct	tagPLAYERPOS
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3	vDelta;
	BOOL fValid;
}
PLAYERPOS, *LPPLAYERPOS;

typedef	struct	tagPLAYERDESTPOS
{
	D3DXVECTOR3 vPos;
	BYTE	fForward;
	BOOL	fValid;
	FLOAT	d;
#ifdef __IAOBJ0622
	OBJID	objidIAObj;
#endif	// __IAOBJ0622
}
PLAYERDESTPOS, *LPPLAYERDESTPOS;

typedef	struct	tagPLAYERDESTANGLE
{
	float	fAngle;
	BOOL	fValid;
	BYTE	fLeft;
}
PLAYERDESTANGLE, *LPPLAYERDESTANGLE;

typedef	struct	tagPLAYERMOVINGACTMSG
{
	D3DXVECTOR3 vPos;
	BYTE	fMoving;
	BYTE	fForward;
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	float	fAngle;
	BOOL	fValid;
}
PLAYERMOVINGACTMSG, *LPPLAYERMOVINGACTMSG;

typedef	struct	tagPLAYERANGLE
{
	BOOL	fValid;
	int		nCounter;
}
PLAYERANGLE, *PPLAYERANGLE;

typedef struct tagSNAPSHOT
{
	PLAYERPOS	playerpos;
	PLAYERDESTPOS	playerdestpos;
	PLAYERDESTANGLE		playerdestangle;
	PLAYERMOVINGACTMSG	playermovingactmsg;
	u_long	uFrameMove;
}
SNAPSHOT, *LPSNAPSHOT;

class CWndGuildVote;
class CDPClient : public CDPMng
{
private:
	SNAPSHOT	m_ss;
	LONG		m_lError; 
	PLAYERANGLE		m_pa;

public:
	BOOL	m_fConn;
	BYTE	*m_pDump;
	int		m_nDumpSize;

	int		m_nCountdown;


	int		m_nMaxLoginGuild;
	u_long  m_uLoginPlayerIdGuild[ 200 ];
	u_long  m_uLoginGuildMulti[ 200 ];

	DWORD	m_dwReturnScroll;		// 귀환의 두루마리 

#ifdef __TRAFIC_1218
	CTraficLog	m_traficLog;
#endif	// __TRAFIC_1218

public:
//	Constructions
	CDPClient();
	virtual	~CDPClient();
//	Override
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	BOOL	Connect( LPSTR lpszAddr, USHORT uPort );
//	Writes
	void	SendHdr( DWORD dwHdr );
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();

	void	SendJoin( BYTE nSlot, DWORD dwWorldID, CMover* pMover, CRTMessenger* pRTMessenger, u_long uIdofMulti );

	void	PostPlayerAngle( BOOL f );
	void	FlushPlayerAngle( void );
	void	SendBlock( BYTE Gu, const char *szName, const char *szFrom );
	void	SendSkillFlag( int nSkill );
	void	SendChat( LPCSTR lpszChat );
	void	SendDoEquip( FLItemElem* pItemElem, int nPart = -1, BOOL bResult = TRUE );
	void	SendError( int nCode, int nData );
	void	SendShipActMsg( CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 );
	void	SendLocalPosFromIA( const D3DXVECTOR3 &vLocal, OBJID idIA );
	void	SendRemoveItem( FLItemElem* pItemElem, int nNum );
	void	SendQueryPostMail( DWORD dwItemObjID, int nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	void	SendQueryRemoveMail( u_long nMail );
	void	SendQueryGetMailItem( u_long nMail );
	void	SendQueryGetMailGold( u_long nMail );
	void	SendQueryReadMail( u_long nMail );
	void	SendQueryMailBox( void );
	void	SendMoveItem( DWORD dwSrcObjIndex, DWORD dwDestObjIndex );
#ifdef INVENTORY_ITEM_ALIGN
	void	SendAlignItem(DWORD dwIndex);
#endif	// INVENTORY_ITEM_ALIGN
	void	SendDropItem( DWORD dwItemType, DWORD dwItemId, int nITemNum, const D3DXVECTOR3 & vPos );
	void	SendDropGold( DWORD dwGold, const D3DXVECTOR3 & vPlayerPos, const D3DXVECTOR3 & vPos );
	void	SendConfirmPKPVP( u_long uidPlayer );
	void	OnSetDuel( OBJID objid, CAr & ar );
//sun: 8, // __S8_PK
	void	OnPKRelation( OBJID objid, CAr & ar );
	void	OnPKPink( OBJID objid, CAr & ar );
	void	OnPKPropensity( OBJID objid, CAr& ar );
	void	OnPKValue( OBJID objid, CAr& ar );

	void	SendConfirmTrade( CMover* pTrader );
	void	SendConfirmTradeCancel( OBJID objid );
	void	SendTrade( CMover* pTrader );
	void	SendTradePut( BYTE i, BYTE ItemType, DWORD dwItemObjID, int ItemNum = 1 );
	void	SendTradePull( BYTE byNth );
	void	SendTradeOk( void );//	{	SendHdr( PACKETTYPE_TRADEOK );	}
	void	SendTradeCancel( int nMode = 0 );
	void	SendTradeConfirm( void );
	void	SendMessageNote( u_long uidTo, LPSTR strMessage );
	void	SendTradePutGold( DWORD dwGold );
//raiders.2006.11.28
//	void	SendTradeClearGold( void );//	{	SendHdr( PACKETTYPE_TRADECLEARGOLD );	}

	void	SendPVendorOpen( const char* szVendor );
	void	SendPVendorClose( OBJID objidVendor );
	void	SendRegisterPVendorItem( BYTE byNth, BYTE byType, DWORD dwItemObjID, int nNum, int nCost );
	void	SendUnregisterPVendorItem( BYTE byNth );
	void	SendQueryPVendorItem( OBJID objidVendor );
	void	SendBuyPVendorItem( OBJID objidVendor, BYTE nItem, DWORD dwItemId, int nNum );
	void	SendRepairItem( DWORD* pdwIdRepair );
	void	SendMotion( DWORD dwMsg );
	void	OnMotion( OBJID objid, CAr & ar );
	void	SendSetHair( BYTE nHair, float r, float g, float b );	//, int nCost );
	void	SendPartySkillUse( int nSkill );
	void	SendPartyMemberCancle( u_long uLeader, u_long uMember, int nMode = 0 );
	void	SendPartyMemberRequest( CMover* pLeader, u_long uMemberId, BOOL bTroup );
	void	SendAddPartyMember( u_long uLeader, LONG nLLevel, LONG nLJob, DWORD dwLSex, u_long uMember, LONG nMLevel, LONG nMJob, DWORD dwMSex );
	void	SendRemovePartyMember( u_long LeaderId, u_long MemberId );
	void	SendChangeShareItem( int nItemMode );
	void	SendChangeShareExp( int nExpMode );
	void	SendExpBoxInfo( OBJID objid );		
	void	SendChangeTroup( BOOL bSendName, const char * szPartyName = "" );
	void	SendChangePartyName( const char * szPartyName );
	void	SendDuelRequest( CMover* pSrc, CMover* pDst );
	void	SendDuelYes( CMover* pSrc, CMover* pDst );
	void	SendDuelNo( CMover* pSrc );
	void	SendDuelPartyRequest( CMover* pSrc, CMover* pDst );
	void	SendDuelPartyYes( CMover* pSrc, CMover* pDst );
	void	SendDuelPartyNo( CMover* pSrc );
	void	SendPartyChangeLeader( u_long uLeaderId, u_long uChangerLeaderid );
	void	SendMoverFocus( u_long uidPlayer );
	void	SendChangeFace( u_long objid, DWORD dwFaceNum, int cost);
	
	void	SendScriptDialogReq( OBJID objid, LPCTSTR lpKey, int nGlobal1, int nGlobal2, int nGlobal3, int nGlobal4 );
	void	SendOpenShopWnd( OBJID objid );
	void	SendCloseShopWnd( void );
	void	SendBuyItem( CHAR cTab, DWORD dwItemObjID, int nNum, DWORD dwItemId );

	void	SendBuyChipItem( CHAR cTab, DWORD dwItemObjID, int nNum, DWORD dwItemId );		//sun: 11, 길드대전 칩보상 및 칩을 통한 상점 이용

	void	SendSellItem( DWORD dwItemObjID, int nNum );
	void	SendMeleeAttack( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3/*, FLOAT fVal*/ );
	void	SendMeleeAttack2( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
	void	SendMagicAttack( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit );
	void	SendRangeAttack( OBJMSG dwAtkMsg, OBJID objid, DWORD dwItemID, int idSfxHit );

//sun: 8, // __S8_PK
	void	SendUseSkill( WORD wType, WORD wId, OBJID objid, int nUseType = 0, int bControl = FALSE );

	void	SendSfxID( OBJID idTarget, int idSfxHit, DWORD dwType, DWORD dwSkill = NULL_ID, int nMaxDmgCnt = 1 );
	void	SendSetTarget( OBJID idTarget, BYTE bClear );
	void	SendTeleSkill( OBJID objid, D3DXVECTOR3 vPos );
		
	void	SendIncJobLevel( CHAR chID );
	void	SendExp( EXPINTEGER nExp );
	void	SendChangeJob( int nJob, BOOL bGama = TRUE );

	void	SendOpenBankWnd( DWORD dwId, DWORD dwItemId );
	
	void	SendFocusObj();

	void	SendOpenGuildBankWnd();
	void	SendCloseGuildBankWnd();
	void	SendCloseBankWnd( void );

	void	SendDoUseSkillPoint( SKILL aJobSkill[] );
	void	SendEnterChattingRoom( u_long uidChatting );
	void	SendChatting( char * pszChat );
	void	SendOpenChattingRoom( void );
	void	SendCloseChattingRoom( void );

	void	SendPutItemGuildBank( DWORD dwItemObjID, int ItemNum, BYTE p_Mode );
	void	SendGetItemGuildBank( DWORD dwItemObjID, int ItemNum, BYTE p_Mode );
	void	SendPutItemBank( BYTE nSlot, DWORD dwItemObjID, int ItemNum );
	void	SendGetItemBank( BYTE nSlot, DWORD dwItemObjID, int ItemNum );
	void	SendPutItemBankToBank( BYTE nPutSlot, BYTE nSlot, DWORD dwItemObjID, int ItemNum );
	void	SendPutGoldBank( BYTE nSlot, DWORD dwGold );
	void	SendGetGoldBank( BYTE nSlot, DWORD dwGold );
	void	SendPutGoldBankToBank( BYTE nPutSlot, BYTE nSlot, DWORD dwGold );
	void	SendStateModeCancel( DWORD dwStateMode, BYTE nFlag );

	
	void	SendMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex );

	void	SendChangeBankPass( const char *szLastPass, const char *szNewPass, DWORD dwId, DWORD dwItemId );
	void	SendConfirmBank( const char *szPass, DWORD dwId, DWORD dwItemId );

	void	SendCorrReq( CObj *pObj );
	void	SendCorrReq( OBJID idObj );
	void	SendCommandPlace( BYTE nType );
	void	SendScriptRemoveAllItem( DWORD dwItemId );
	void	SendScriptEquipItem( DWORD dwItemId, int nOption );
	void	SendScriptCreateItem( BYTE nItemType, DWORD dwItemId, int nNum, int nOption );
	void	SendScriptAddGold( int nGold );
	void	SendScriptRemoveQuest( int nQuestId );
	void	SendSetQuest( int nQuestIdx, int nQuest );		
	void	SendScriptReplace( DWORD dwWorld, D3DXVECTOR3 vPos );
	void	SendScriptReplaceKey( DWORD dwWorld, LPCSTR lpszKey );
	void    SendScriptAddExp( int nExp );
	void	SendCreateGuildCloak( void );
//________________________________________________________________________________
	void	SendPlayerBehavior( void );
		
	void	SendPlayerDestObj( OBJID objid, float fRange = 0.0f );
// 	void	SendQueryGetPos( CMover* pMover );
// 	void	SendGetPos( const D3DXVECTOR3 & vPos, float fAngle, OBJID objid );
	void	SendCtrlCoolTimeCancel();		
	void	SendQueryGetDestObj( CMover* pMover );
	void	SendGetDestObj( OBJID objid, OBJID objidDest );
	void	SendSkillTaskBar( void );
	void	SendRemoveAppletTaskBar( BYTE nIndex );
	void	SendAddAppletTaskBar( BYTE nIndex, LPSHORTCUT pAppletShortcut );
	void	SendRemoveItemTaskBar( BYTE nSlotIndex, BYTE nIndex );
	void	SendAddItemTaskBar( BYTE nSlotIndex, BYTE nIndex, LPSHORTCUT pItemShortcut );
	void	SendAddFriend( u_long uidPlayer, LONG nJob, BYTE nSex );
	void	SendAddFriendReqest( u_long uidPlayer );
	void	SendAddFriendNameReqest( const char * szName );
	void	SendFriendCancel( u_long uidLeader, u_long uidMember );


//sun: 11, 주머니
	void	SendAvailPocket( int nPocket, DWORD dwItemObjID );
	void	SendMoveItem_Pocket( int nPocket1, DWORD dwItemObjID, int nNum, int nPocket2 );

	void	SendQuePetResurrection( DWORD dwItemObjID );
	void	SendGetFriendState();
	void	SendSetState( int state );
	void	SendFriendInterceptState( u_long uidPlayer );
	void	SendRemoveFriend( u_long uidPlayer );
	void	SendUpgradeBase( DWORD dwItemId0, DWORD dwItemId1, 
							DWORD dwItemId2, DWORD dwItemCount2, 
							DWORD dwItemId3, DWORD dwItemCount3, 
							DWORD dwItemId4, DWORD dwItemCount4,
							DWORD dwItemId5, DWORD dwItemCount5 );

	void	SendRandomScroll( OBJID objid, OBJID objid2 );
	void	SendEnchant( OBJID objid, OBJID objMaterialId );

	void	SendRemoveAttribute( OBJID objid );	//sun: 10, 속성제련 제거(10차로 변경)
	void	SendChangeAttribute( OBJID objTargetItem, OBJID objMaterialItem, int nAttribute );	//sun:13, 제련 확장(속성, 일반)

	void	SendPiercingSize( OBJID objid1, OBJID objid2, OBJID objid3 );
	void	SendItemTransy( OBJID objid0, OBJID objid1, DWORD dwChangeId = NULL_ID, BOOL bCash = TRUE );
	void	SendPiercing( OBJID objid1, OBJID objid2 );

	void	SendPiercingRemove( OBJID objid1, OBJID objid2 );		//sun: 11, 피어싱 옵션 제거

	void	SendCommercialElem( DWORD dwItemId, DWORD dwItemId1 );
	void	SendCreateSfxObj( DWORD dwSfxId, u_long idPlayer = NULL_ID, BOOL bFlag = FALSE );
	void	SendSetNaviPoint( const D3DXVECTOR3 & Pos, OBJID objidTarget );
	void	OnSetNaviPoint( OBJID objid, CAr & ar );
	void	SendGuildInvite( OBJID objid );
	void	SendIgnoreGuildInvite( u_long idPlayer );
	void	SendCreateGuild( const char* szGuild );
	void	SendDestroyGuild( u_long idMaster );
	void	SendAddGuildMember( u_long idMaster, const GUILD_MEMBER_INFO & info, BOOL bGM );
	void	SendRemoveGuildMember( u_long idMaster, u_long idPlayer );
	void	SendGuildLogo( DWORD dwLogo );
	void	SendGuildContribution( BYTE cbPxpCount, int nGold, BYTE cbItemFlag = 0);
	void    SendGuildNotice( const char* szNotice );
	void	SendGuildAuthority( u_long uGuildId, DWORD dwAuthority[] );
	void	SendGuilPenya( u_long uGuildId, DWORD dwType, DWORD dwPenty );
	void	SendGuildSetName( LPCTSTR szName );
	void	SendGuildRank( DWORD nVer );
	void	SendGuildMemberLv( u_long idMaster, u_long idPlayer, int nMemberLv );
	void	SendAddVote( const char* szTitle, const char* szQuestion, const char* szSelections[] );
	void	SendRemoveVote( u_long idVote );
	void	SendCloseVote( u_long idVote );
	void	SendCastVote( u_long idVote, BYTE cbSelection );
	void	UpdateGuildWnd();
	void	SendGuildClass( u_long idMaster, u_long idPlayer, BYTE nFlag );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2 );
	void	OnChgMaster( CAr & ar );
	void	OnSetWar( OBJID objid, CAr & ar );
	void	SendDeclWar( u_long idMaster, const char* szGuild );
	void	SendAcptWar( u_long idMaster, u_long idDecl );
	
	void	SendSurrender( u_long idPlayer );	// 항복
	void	SendQueryTruce( u_long idPlayer );	// 정전 요청
	void	SendAcptTruce( u_long idPlayer );	// 정전 수락
	void	OnSurrender( CAr & ar );	// 항복
	void	OnQueryTruce( CAr & ar );

	void	OnDeclWar( CAr & ar );
	void	OnAcptWar( CAr & ar );
	void	OnWar( CAr & ar );
	void	OnWarDead( CAr & ar );
	void	OnWarEnd( CAr & ar );

	void	OnRemoveSkillInfluence( OBJID objid, CAr & ar );
	void	OnRemoveAllSkillInfluence( OBJID objid, CAr & ar );
	
	void	OnRequestGuildRank( CAr & ar );
	void	SendActionPoint( int nAP );
		
//________________________________________________________________________________

	void	SendDoUseItem( DWORD dwItemId, OBJID objid, int nPart = -1 , BOOL bResult = TRUE );
//sun: 11, 각성, 축복
	void	SendDoUseItemTarget( DWORD dwMaterial, DWORD dwTarget );
	void	SendRemoveItemLevelDown( DWORD dwId );
	void	SendAwakening( int nItem );
	void	SendBlessednessCancel( int nItem );

	void	SendSnapshot( BOOL fUnconditional = FALSE );
#ifdef __IAOBJ0622
	void	PutPlayerDestPos( CONST D3DXVECTOR3 & vPos, bool bForward, BYTE f = 0, OBJID objidIAObj = NULL_ID );
#else	// __IAOBJ0622
	void	PutPlayerDestPos( CONST D3DXVECTOR3 & vPos, bool bForward, BYTE f = 0 );
#endif	// __IAOBJ0622
	void	PutPlayerDestAngle( float fAnlge, BYTE fLeft, BYTE f = 0 );
	void	ClearPlayerDestPos( void );
	void	SendSfxHit( int idSfxHit, int nMagicPower, DWORD dwSkill = NULL_ID, OBJID idAttacker = NULL_ID, int nDmgCnt = 0, float fDmgAngle = 0, float fDmgPower = 0 );
	void	SendSfxClear( int idSfxHit, OBJID idMover = NULL_ID );
	void	SendQuerySetPlayerName( DWORD dwData, const char* lpszPlayer );
	void	SendSummonFriend( DWORD dwData, const char* lpszPlayer );
	void	SendSummonFriendConfirm( OBJID objid, DWORD dwData );
	void	SendSummonFriendCancel( OBJID objid, DWORD dwData );
	void	SendSummonParty( DWORD dwData );
	void	SendSummonPartyConfirm( OBJID objid, DWORD dwData );
//sun: 9, 9-10차 펫
	void	SendPetRelease( void );
	void	SendUsePetFeed( DWORD dwFeedId );				//sun: 12, 펫 각성 // dwFeedId : 먹이	// 입력 개수 제거

	void	SendMakePetFeed( DWORD dwMaterialId, int nNum, DWORD dwToolId );	// dwFeedId : 무기/방어구/전리품, dwToolId : 분쇄기( npc일 경우 NULL_ID )
	void	SendPetTamerMistake( DWORD dwId );
	void	SendPetTamerMiracle( DWORD dwId );
	void	SendFeedPocketInactive( void );

	void	SendLegendSkillUp(OBJID* pdwItemId, int count);	//sun: 9차 전승관련 Clienet

	void	SendModifyStatus(int nStrCount, int nStaCount, int nDexCount, int nIntCount);		//sun: 9차 상태창 변경 관련 Neuz

	void	SendRemoveQuest( DWORD dwQuest );
	void	SendWantedGold( int nGold, LPCTSTR szMsg );
	void	SendWantedList();
	void	SendWantedName();

	void	SendWantedInfo( LPCTSTR szPlayer );
	void	SendReqLeave();	
	void	SendReqRevivalBySkill( const bool allowRevival );
	void	SendChangePKPVPMode( DWORD dwMode, int nFlag );
	void	SendDoEscape( void );
	void	SendCheering( OBJID objid );
	void	OnSetCheerParam( OBJID objid, CAr & ar );
	void	SendQuerySetGuildName( LPCSTR pszGuild, BYTE nId );
	void	SendQueryEquip( OBJID objid );
	void	SendQueryEquipSetting( BOOL bAllow );
	void	SendReturnScroll( int nSelect );
	void	SendEndSkillQueue( void );
	void	SendGuildCombatWindow( void );
	void	SendGuildCombatApp( DWORD dwPenya );
	void	SendGuildCombatCancel( void );
	void	SendGCRequestStatusWindow( void );
	void	SendGCSelectPlayerWindow( void );
	void	SendGCSelectPlayer( std::vector<u_long> vecSelectPlayer, u_long uidDefender );
	void	SendGCSelectMap( int nMap );
	void	SendGCJoin( void );
	void	SendGCGetPenyaGuild( void );
	void	SendGCGetPenyaPlayer( void );
	void	SendGCTele( void );
	void	SendGCPlayerPoint( void );
	void	SendCreateMonster( DWORD dwItemId, D3DXVECTOR3 vPos );		//sun: 12, 몬스터 생성 Neuz, World

//sun: 10, __EVE_MINIGAME
	void SendKawibawiboStart();
	void SendKawibawiboGetItem();
	void SendReassembleStart(OBJID* pdwItemId, int count);
	void SendReassembleOpenWnd();
	void SendAlphabetOpenWnd();
	void SendAlphabetStart(OBJID* pdwItemId, int count, int nQuestionID);
	void SendFiveSystemOpenWnd();
	void SendFiveSystemBet( int nBetNum, int nBetPenya );
	void SendFiveSystemStart();
	void SendFiveSystemDestroyWnd();

//sun: 9,10차 제련 __ULTIMATE
	void OnUltimateWeapon( OBJID objid, CAr & ar );
	void OnUltimateMakeItem( OBJID objid, CAr & ar );
	void OnUltimateTransWeapon( OBJID objid, CAr & ar );
	void OnUltimateMakeGem( OBJID objid, CAr & ar );
	void OnUltimateSetGem( OBJID objid, CAr & ar );
	void OnUltimateRemoveGem( OBJID objid, CAr & ar );
	void OnUltimateEnchantWeapon( OBJID objid, CAr & ar );

	void SendUltimateMakeItem( OBJID* pdwItemId );
	void SendUltimateMakeGem( OBJID objItemId );
	void SendUltimateTransWeapon( OBJID objItemWeapon, OBJID objItemGem1, OBJID objItemGem2 );
	void SendUltimateSetGem( OBJID objItemWeapon, OBJID objItemGem );
	void SendUltimateRemoveGem( OBJID objItemWeapon, OBJID objItemGem );
	void SendUltimateEnchantWeapon( OBJID objItemWeapon, OBJID objItemGem );

//sun: 11, 일대일 길드 대전
	void	SendGC1to1TenderOpenWnd();
	void	SendGC1to1TenderView();
	void	SendGC1to1Tender( int nPenya );
	void	SendGC1to1TenderCancel();
	void	SendGC1to1TenderFailed();
	void	OnGC1to1TenderOpenWnd( CAr & ar );
	void	OnGC1to1TenderView( CAr & ar );
	
	void	OnGC1to1NowState( CAr & ar );
	void	OnGC1to1WarResult( CAr & ar );
	
	void	SendGC1to1MemberLienUpOpenWnd();
	void	SendGC1to1MemberLienUp( std::vector<u_long>& vecMemberId );
	void	OnGC1to1MemberLineUpOpenWnd( CAr & ar );

	void	SendGC1to1TeleportToNPC();
	void	SendGC1to1TeleportToStage();
	
// 길드 창고 로그 기능 world,database,neuz
	void	SendReqGuildBankLogList( BYTE byListType );
	void	OnGuildBankLogList( CAr & ar );

//sun, 11 케릭터 봉인 거래 기능 world,database,neuz
	void	SendSealChar(  );
	void	OnSealChar( CAr & ar );
	void	SendSealCharConm( OBJID objid  );
	void	OnSealCharGet( CAr & ar );
	void	SendSealCharSet( DWORD dwData );

//sun: 13, 달인
	void	SendReqHonorList();
	void	SendReqHonorTitleChange( int nChange );
	void	OnHonorListAck( CAr & ar ); 
	void	OnHonorChangeAck( OBJID objid,CAr & ar ); 

	//sun: 8차 엔젤 소환 Neuz, World, Trans
	void SendCreateAngel(CString sendstr);
	void SendAngelBuff();
	void OnAngel( OBJID objid, CAr& ar );
	void OnCreateAngel( OBJID objid, CAr & ar );
	void OnAngelInfo( OBJID objid, CAr & ar );

#ifdef PAT_LOOTOPTION
	void SendPetLootOption(int nLootType);
#endif // PAT_LOOTOPTION

#ifdef	KEYBOARD_SET
	void SendKeyBoard (int nKeyType);
#endif	// KEYBOARD_SET 

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void SendCostumeSetGem( OBJID objItemWeapon, OBJID objItemGem );
	void SendCostumeRemoveGem( OBJID objItemWeapon, OBJID objItemGem );
#endif

	// Handlers
	USES_PFNENTRIES( CDPClient, ( CAr & ar ) );
	void	OnKeepAlive( CAr & ar );
	void	OnError( CAr & ar );
	void	OnSnapshot( CAr & ar );
	void	OnJoin( CAr & ar );
	void	OnReplace( CAr & ar );
	void	OnWhisper( CAr & ar );
	void	OnSay( CAr & ar );
	void	OnGMSay( CAr & ar );
//	Snapshot handlers
	void	OnAddObj( OBJID objid, CAr & ar );
	void	OnRemoveObj( OBJID objid );
	void	OnChat( OBJID objid, CAr & ar );
	void	OnEventMessage( OBJID objid, CAr & ar );
	void	OnDamage( OBJID objid, CAr & ar );
	void	OnMoverDeath( OBJID objid, CAr & ar );
	void	OnCreateItem( OBJID objid, CAr & ar );
	void	OnMoveItem( CAr & ar );
#ifdef INVENTORY_ITEM_ALIGN
	void	OnAlignItem( CAr & ar );
#endif	// INVENTORY_ITEM_ALIGN
	void	OnDoEquip( OBJID objid, CAr & ar );
	void	OnShipActMsg( OBJID objid, CAr & ar );
		
	void	OnTrade( OBJID objid, CAr & ar );
	void	OnConfirmTrade( OBJID objid, CAr & ar );
	void	OnConfirmTradeCancel( OBJID objid, CAr & ar );
	void	OnTradePut( OBJID objid, CAr & ar );
	void	OnTradePutError( OBJID objid, CAr & ar );
	void	OnTradePull( OBJID objid, CAr & ar );
	void	OnTradePutGold( OBJID objid, CAr & ar );
//raiders.2006.11.28
//	void	OnTradeClearGold( OBJID objid );
	void	OnTradeCancel( OBJID objid, CAr & ar );
	void	OnTradeOk( OBJID objid, CAr & ar );
	void	OnTradeConsent( CAr & ar );
	void	OnTradelastConfirm( void );
	void	OnTradelastConfirmOk( OBJID objid, CAr & ar );

	void	OnOpenShopWnd( OBJID objid, CAr & ar );
	void	OnPutItemBank( OBJID objid, CAr & ar );
	void	OnGetItemBank( OBJID objid, CAr & ar );
	void	OnPutGoldBank( OBJID objid, CAr & ar );
	void	OnMoveBankItem( OBJID objid, CAr & ar );
	void	OnUpdateBankItem( OBJID objid, CAr & ar );
	void	OnErrorBankIsFull( OBJID objid, CAr & ar );
	void	OnBankWindow( OBJID objid, CAr & ar );

	void    OnFocusObj( CAr & ar );
	
	void	OnGuildBankWindow( OBJID objid, CAr & ar );
	void	OnPutItemGuildBank( OBJID objid, CAr & ar );
	void	OnGetItemGuildBank( OBJID objid, CAr & ar );
	void	OnChangeBankPass( OBJID objid, CAr & ar );
	void	OnConfirmBankPass( OBJID objid, CAr & ar );
	void	OnVendor( OBJID objid, CAr & ar );
	void	OnUpdateVendor( OBJID objid, CAr & ar );
	void	OnUpdateItem( OBJID objid, CAr & ar );

//	void	OnUpdateItemEx( OBJID objid, CAr & ar );		//sun: 11, 각성, 축복

//sun: 11, 주머니
	void	OnPocketAttribute( CAr & ar );
	void	OnPocketAddItem( CAr & ar );
	void	OnPocketRemoveItem( CAr & ar );

	void	OnQuePetResurrectionResult( CAr & ar );
	void	OnSetDestParam( OBJID objid, CAr & ar );
	void	OnResetDestParam( OBJID objid, CAr & ar );

	void	OnResetDestParamSync( OBJID objid, CAr & ar );

	void	OnSetPointParam( OBJID objid, CAr & ar );
	void	OnSetScale( OBJID objid, CAr & ar );
		
	void	OnSetPos( OBJID objid, CAr & ar );
	void	OnSetPosAngle( OBJID objid, CAr & ar );
	void	OnSetLevel( OBJID objid, CAr & ar );
	void	OnSetFlightLevel( OBJID objid, CAr & ar );
	void	OnSetExperience( OBJID objid, CAr & ar );
	void	OnSetFxp( OBJID objid, CAr & ar );
	void	OnSetSkillLevel( OBJID objid, CAr & ar );
	void	OnSetSkillExp( OBJID objid, CAr & ar );
	void	OnText( CAr & ar );

	void	OnAllAction( CAr & ar );
	
	void	OnRevivalBySkill( OBJID objid );
	void	OnRevivalCurrentPos( OBJID objid );
	void	OnRevivalLodestar( OBJID objid );
	void	OnRevivalLodelight( OBJID objid );

	void	OnSetGrowthLearningPoint( OBJID objid, CAr & ar );
	void	OnSetStatLevel( OBJID objid, CAr & ar );

	void	OnSetDestPos( OBJID objid, CAr & ar );
	void	OnSetMovePattern( OBJID objid, CAr & ar );
	void	OnMeleeAttack( OBJID objid, CAr & ar );
	void	OnMeleeAttack2( OBJID objid, CAr & ar );
	void	OnMagicAttack( OBJID objid, CAr & ar );
	void	OnRangeAttack( OBJID objid, CAr & ar );
	void	OnAttackSP( OBJID objid, CAr & ar );
	void	OnMoverSetDestObj( OBJID objid, CAr & ar );
	void	OnUseSkill( OBJID objid, CAr & ar );
	
	void	OnCreateSfxObj( OBJID objid, CAr & ar );
	void	OnRemoveSfxObj( OBJID objid, CAr & ar );	//sun, 11, 확율스킬 효과수정 world,neuz

	void	OnCreateSfxAllow( OBJID objid, CAr & ar );				
	void	OnDefinedText( CAr & ar );
	void	OnChatText( CAr & ar );
	void	OnDefinedText1( CAr & ar );
	void	OnDefinedCaption( CAr & ar );
	void	OnCtrlCoolTimeCancel( OBJID objid, CAr & ar );
	void	OnGameTimer( CAr & ar );
	void	OnGameJoin( CAr & ar );
	void	OnTaskBar( CAr & ar );
	void	OnErrorParty( CAr & ar );
	void	OnAddPartyMember( CAr & ar );
	void	OnPartyRequest( CAr & ar );
	void	OnPartyRequestCancel( CAr & ar );
	void	OnPartyExpLevel( CAr & ar );
	void	OnPartyChangeTroup( CAr & ar );
	void	OnPartyChangeName( CAr & ar );
	void	OnPartySkillCall( OBJID objid, CAr & ar );
	void	OnPartySkillBlitz( CAr & ar );
	void	OnPartySkillRetreat( OBJID objid );
	void	OnPartySkillSphereCircle( OBJID objid );
	void	OnSetPartyMode( CAr & ar );
	void	OnPartyChangeItemMode( CAr & ar );
	void	OnPartyChangeExpMode( CAr & ar );

	void	OnSetPartyMemberParam( CAr & ar );

	void	OnSeasonInfo( CAr & ar );
	void	OnSeasonEffect( CAr & ar );

	void	OnPartyChat( CAr & ar );
	void	OnMyPartyName( CAr & ar );
	void	OnPartyChangeLeader( CAr  & ar );
	void	OnSMMode( CAr & ar );
	void	OnSMModeAll( CAr & ar );
	void	OnResistSMMode( CAr & ar );
	void	OnCommercialElem( CAr & ar );
	void	OnMoverFocus( CAr & ar );
	void	OnPartyMapInfo( CAr & ar );

	void	OnChatting( OBJID objid, CAr & ar );
	
	void	OnCommonPlace( OBJID objid, CAr & ar );
	void	OnDoApplySkill( OBJID objid, CAr & ar );
	void	OnCommonSkill( OBJID objid, CAr & ar );
	void	OnFlyffEvent( CAr & ar );
	void	OnSetLocalEvent( CAr & ar );
	void	OnGameRate( CAr & ar );
	void	OnClearTarget( CAr & ar );
	
	void	OnEventLuaDesc( CAr & ar );					//sun: 9, 이벤트 (루아 스크립트 적용)
	void	OnRemoveAttributeResult( CAr & ar );		//sun: 10, 속성제련 제거(10차로 변경)

	void    OnMotionArrive( OBJID objid, CAr & ar );
#ifdef __S1108_BACK_END_SYSTEM
	void	OnMonsterProp( CAr & ar );
	void	OnGMChat( CAr & ar );
#endif // __S1108_BACK_END_SYSTEM

	void	OnChangeFace( CAr & ar );
	
	void	OnGuildCombat( CAr& ar );
	void	OnQuestTextTime( CAr& ar );
	
	void	OnGCInWindow( CAr& ar );
	void	OnGCRequestStatus( CAr& ar );
	void	OnGCSelectPlayer( CAr& ar );	
	void	OnGuildCombatEnterTime( CAr & ar );
	void	OnGuildCombatNextTimeState( CAr & ar );
	void	OnGuildCombatState( CAr & ar );
	void	OnGCUserState( CAr & ar );
	void	OnGCGuildStatus( CAr & ar );
	void	OnGCGuildPrecedence( CAr & ar );
	void	OnGCPlayerPrecedence( CAr & ar );
	void	OnGCJoinWarWindow( CAr & ar );
	void	OnGCGetPenyaGuild( CAr & ar );
	void	OnGCGetPenyaPlayer( CAr & ar );
	void	OnGCWinGuild( CAr & ar );
	void	OnGCBestPlayer( CAr & ar );
	void	OnGCWarPlayerList( CAr & ar );
	void	OnGCTele( CAr & ar );
	void	OnGCDiagMessage( CAr & ar );	
	void	OnIsRequest( CAr & ar );
	void	OnGCLog( CAr & ar );
	void	OnGCLogRealTime( CAr & ar );
	void	OnGCPlayerPoint( CAr & ar );

//sun: 10, __EVE_MINIGAME
	void	OnMiniGame( OBJID objid, CAr & ar );
	void	OnKawibawibo_Result( CAr & ar );
	void	OnReassemble_Result( CAr & ar );
	void	OnReassemble_OpenWnd( CAr & ar );
	void	OnAlphabet_OpenWnd( CAr & ar );
	void	OnAlphabet_Result( CAr & ar );
	void	OnFiveSystem_OpenWnd( CAr & ar );
	void	OnFiveSystem_Result( CAr & ar );

	void	OnSExpBoxInfo( OBJID objid, CAr & ar );
	void	OnSExpBoxCoolTime( OBJID objid, CAr & ar );
	
	void	OnFriendGameJoin( CAr & ar );
	void	OnAddFriend( CAr & ar );
	void	OnRemoveFriend( CAr & ar );
	void	OnAddFriendReqest( CAr & ar );
	void	OnAddFriendCancel( CAr & ar );
	void	OnFriendJoin( CAr & ar );
	void	OnFriendLogOut( CAr & ar );
	void	OnFriendNoIntercept( CAr & ar );
	void	OnFriendIntercept( CAr & ar );
	void	OnGetFriendState( CAr & ar );
	void	OnSetFriendState( CAr & ar );
	void	OnAddFriendError( CAr & ar );
	void	OnAddFriendNameReqest( CAr & ar );
	void	OnAddFriendNotConnect( CAr & ar );
	void	OnOneFriendState( CAr & ar );
	void	OnRemoveFriendState( CAr & ar );
	void	OnBlock( CAr & ar );
	void	OnDuelCount( CAr & ar );
	void	OnDuelRequest( CAr & ar );
	void	OnDuelStart( CAr & ar );
	void	OnDuelNo( CAr & ar );
	void	OnDuelCancel( CAr & ar );
	void	OnDuelPartyRequest( CAr & ar );
	void	OnDuelPartyStart( CAr & ar );
	void	OnDuelPartyNo( CAr & ar );
	void	OnDuelPartyCancel( CAr & ar );
	void	OnDuelPartyResult( CAr & ar );
	void	OnSkillFlag( CAr & ar );
	void	OnSetSkillState( CAr & ar );
	void	OnTagResult( CAr & ar );
	void	OnChangeShopCost( CAr & ar );
	void	OnItemDropRate( CAr & ar );
	void	OnSetGuildQuest( CAr & ar );
	void	OnRemoveGuildQuest( CAr & ar );
	void	OnSetQuest( OBJID objid, CAr & ar );
	void	OnScriptRemoveQuest( OBJID objid, CAr & ar );
	void	OnSetChangeJob( OBJID objid, CAr & ar );
	void	OnSetNearChangeJob( OBJID objid, CAr & ar );
	void	OnModifyMode( OBJID objid, CAr & ar );
	void	OnStateMode( OBJID objid, CAr & ar );
	void	OnReturnSay( OBJID objid, CAr & ar );
	void	OnClearUseSkill( OBJID objid );
	void	OnSetFame( OBJID objid, CAr & ar );
	void	OnSetFuel( OBJID objid, CAr & ar );
		
		//________________________________________________________________________________
	void	OnMoverBehavior( OBJID objid, CAr & ar );
// 	void	OnQueryGetPos( CAr & ar );
// 	void	OnGetPos( OBJID objid, CAr & ar );
	void	OnQueryGetDestObj( CAr & ar );
	void	OnGetDestObj( OBJID objid, CAr & ar );
//sun: 11, 캐릭터 정보 통합
	void	OnQueryPlayerData( CAr & ar );
	void	OnLogout( CAr & ar );

	void	OnCreateGuild( CAr & ar );
	void	OnDestroyGuild( CAr & ar );
	void	OnGuild( CAr & ar );
	void	OnSetGuild( OBJID objid, CAr & ar );
	void	OnAddGuildMember( CAr & ar );
	void	OnRemoveGuildMember( CAr & ar );
	void	OnGuildInvite( CAr & ar );
	void	OnAllGuilds( CAr & ar );
	void	OnGuildChat( CAr & ar );
	void	OnGuildMemberLv( CAr & ar );
	void	OnGuildClass( CAr & ar );
	void	OnGuildNickName( CAr & ar );
	void	OnGuildMemberLogin( CAr & ar );
	void	OnGuldMyGameJoin( CAr & ar );
	void	OnGuildError( CAr & ar );
	void	OnGuildLogo( CAr & ar );
	void	OnGuildContribution( CAr & ar );
	void	OnGuildNotice( CAr & ar );
	void	OnGuildAuthority( CAr & ar );
	void	OnGuildPenya( CAr & ar );
	void	OnGuildRealPenya( CAr & ar );
	void	OnGuildSetName( CAr & ar );
	void	OnGuildAddVote( CAr & ar );
	void	OnGuildModifyVote( CAr & ar );

	void	OnGuildRank( CAr & ar );
	void	OnCorrReq( OBJID objid, CAr & ar );
	void	OnPVendorOpen( OBJID objid, CAr & ar );
	void	OnPVendorClose( OBJID objid, CAr & ar );
	void	OnRegisterPVendorItem( OBJID objid, CAr & ar );
	void	OnUnregisterPVendorItem( OBJID objid, CAr & ar );
	void	OnPVendorItem( OBJID objid, CAr & ar );
	void	OnPVendorItemNum( OBJID objid, CAr & ar );

	void	OnSetHair( OBJID objid, CAr & ar );
	void	OnSetState( OBJID objid, CAr & ar );
	void	OnCmdSetSkillLevel( CAr & ar );
	void	OnCreateSkillEffect( OBJID objid, CAr & ar );
	void	OnSetStun( OBJID objid, CAr & ar );
	void	OnSendActMsg( OBJID objid, CAr & ar );
	void	OnPushPower( OBJID objid, CAr & ar );
		
public:
//	void	SendDoCollect( CObj *pObj );
private:
//	void	OnDoCollect( OBJID objid, CAr & ar );
//	void	OnStopCollect( OBJID objid, CAr & ar );


	void	OnTag( OBJID objid, CAr & ar );
	void	OnRunScriptFunc( OBJID objid, CAr & ar );
	void	OnSchoolReport( CAr & ar );
	void	OnSexChange( OBJID objid, CAr & ar );
	void	OnRemoveQuest( CAr & ar );
	void	OnInitSkillPoint( CAr & ar );
	void	OnDoUseSkillPoint( CAr & ar );

	void	OnNewYear( CAr & ar )	{	m_nCountdown	= 60;	}

	void	OnResetBuffSkill( OBJID objid, CAr & ar );
//	void	OnEndRecoverMode( OBJID objid, CAr & ar );
	void	OnWantedInfo( CAr & ar );
	void	OnWantedList( CAr & ar );
	void	OnWantedName( CAr & ar );		
	void	OnRevivalBySkillMessage();
	void	OnWorldMsg( OBJID objid, CAr & ar );
	void	OnSetPlayerName( CAr & ar );

	void	OnUpdatePlayerData( CAr & ar );		//sun: 11, 캐릭터 정보 통합

	void	OnEscape( OBJID objid, CAr & ar );
	void	OnSetActionPoint( OBJID objid, CAr & ar );
	void	OnEndSkillQueue( OBJID objid );
	void	OnSnoop( CAr & ar );
	void	OnQueryEquip( OBJID objid, CAr & ar );
	void	OnReturnScrollACK( CAr & ar );
	void	OnSetTarget( OBJID objid, CAr & ar );
	void	OnRemoveMail( CAr & ar );
	void	OnRemoveMailItem( CAr & ar );
	void	OnMailBox( CAr & ar );


	//////////////////////////////////////////////////////////////////////////
	void	OnMailBoxReq( CAr & ar );
	//////////////////////////////////////////////////////////////////////////


	void	OnSummon( CAr & ar );
	void	OnSummonFriend( CAr & ar );
	void	OnSummonFriendConfirm( CAr & ar );
	void	OnSummonPartyConfirm( CAr & ar );
	void	OnRemoveGuildBankItem( CAr & ar );
	void	OnAddRegion( CAr & ar );

	void	OnCallTheRoll( CAr & ar );

#ifdef __TRAFIC_1218
	void	OnTraficLog( CAr & ar );
#endif	// __TRAFIC_1218	

//sun: 9, 9-10차 펫
	void	OnPetCall( OBJID objid, CAr & ar );
	void	OnPetRelease( OBJID objid, CAr & ar );
	void	OnPetSetExp( OBJID objid, CAr & ar );
	void	OnPet( OBJID objid, CAr & ar );
	void	OnPetLevelup( OBJID objid, CAr & ar );
	void	OnPetState( OBJID objid, CAr & ar ); 
	void	OnPetFeed( OBJID objid, CAr & ar );
	void	OnPetFoodMill( OBJID objid, CAr & ar );

	void	OnSetSpeedFactor( OBJID objid, CAr & ar );	//sun: 9,	//__AI_0509
	void	OnLegendSkillUp( CAr & ar );				//sun: 9차 전승관련 Clienet

public:
//sun: 11, 채집 시스템
	void	SendQueryStartCollecting( void );
	void	SendQueryStopCollecting( void );
	void	OnStartCollecting( OBJID objid );
	void	OnStopCollecting( OBJID objid );
	void	OnRestartCollecting( OBJID objid, CAr & ar );

	void	SendOptionEnableRenderMask( BOOL bEnable );		//sun: 12, 옵션창 개선

	void	SendOptionEnableRenderCostume( int nCostumeIdx, BOOL bEnable );

	void	SendNPCBuff( const char* szKey );

	void	OnEventCoupon( CAr & ar );

//sun: 11, 캐릭터 정보 통합
	void	SendQueryPlayerData( u_long idPlayer, int nVer = 0 );
	void	SendQueryPlayerData( const std::vector<PDVer> & vecPlayer );


//sun: 12, 비밀의 방
public:
	void	SendSecretRoomTender( int nPenya );
	void	SendSecretRoomTenderCancelReturn();
	void	SendSecretRoomLineUpMember( std::vector<DWORD> vecLineUpMember );
	void	OnSecretRoomMngState( CAr & ar );
	void	OnSecretRoomInfo( CAr & ar );
	void	SendSecretRoomTenderOpenWnd();
	void 	OnSecretRoomTenderOpenWnd( CAr & ar );
	void	SendSecretRoomLineUpOpenWnd();
	void	OnSecretRoomLineUpOpenWnd( CAr & ar );
	void	SendSecretRoomEntrance();
	void	SendSecretRoomTeleportToNPC();
	void	SendSecretRoomTenderView();
	void	OnSecretRoomTenderView( CAr & ar );
	void	SendTeleportToSecretRoomDungeon();
private:

//sun: 12, 세금
public:
	void	OnTaxInfo( CAr & ar );
	void	OnTaxSetTaxRateOpenWnd( CAr & ar );
	void	SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate );

	void	SendTeleportToHeavenTower( int nFloor );		//sun: 12, 심연의 탑
private:

//sun: 12, 군주
private:
	// 군주 입찰 결과 갱신
	void	OnElectionAddDeposit( CAr & ar );
	// 공약 설정 결과 갱신
	void	OnElectionSetPledge( CAr & ar );
	// 투표 결과 갱신
	void	OnElectionIncVote( CAr & ar );
	// 입후보 시작 상태로 변경
	void	OnElectionBeginCandidacy( CAr & ar );
	// 투표 시작 상태로 변경
	void	OnElectionBeginVote( CAr & ar );
	// 투표 종료 상태로 변경
	void	OnElectionEndVote( CAr & ar );
	// 군주 시스템 정보를 수신하여 복원
	void	OnLord( CAr & ar );
	// 군주 이벤트 시작 처리
	void	OnLEventCreate( CAr & ar );
	// 군주 이벤트 초기화 처리
	void	OnLEventInitialize( CAr & ar );
	// 군주 스킬 재사용 대기 시간 처리
	void	OnLordSkillTick( CAr & ar );
	// 군주 이벤트 지속 시간 처리
	void	OnLEventTick( CAr & ar );
	// 군주 스킬 사용
	void	OnLordSkillUse( OBJID objid, CAr & ar );
public:
	// 군주 입찰 처리 요청
	void	SendElectionAddDeposit( __int64 iDeposit );
	// 공약 설정 처리 요청
	void	SendElectionSetPledge( const char* szPledge );
	// 투표 처리 요청
	void	SendElectionIncVote( u_long idPlayer );
	// 군주 이벤트 시작 요청
	void	SendLEventCreate( int iEEvent, int iIEvent );
	// 군주 스킬 사용 요청
	void	SendLordSkillUse( int nSkill, const char* szTarget = "" );

//sun: 12, 펫 각성
	void	SendTransformItem( CTransformStuff & stuff );	// 알변환 요청
	void	SendPickupPetAwakeningCancel( DWORD dwItem );	// 픽업펫 각성 취소 요청

	void	SendDoUseItemInput( DWORD dwData, char* szInput );
//sun: ?, __PET_1024
	void	SendClearPetName();
	void	OnSetPetName( OBJID objid, CAr & ar );

//sun: 12, 튜토리얼 개선
	void	SendTutorialState( int nState );
	void	OnTutorialState( CAr & ar );

//sun: 13, 레인보우 레이스
private:
	void	OnRainbowRacePrevRankingOpenWnd( CAr & ar );
	void	OnRainbowRaceApplicationOpenWnd( CAr & ar );
	void	OnRainbowRaceNowState( CAr & ar );
	void	OnRainbowRaceMiniGameState( CAr & ar, BOOL bExt );
public:
	void	SendRainbowRacePrevRankingOpenWnd();
	void	SendRainbowRaceApplicationOpenWnd();
	void	SendRainbowRaceApplication();
	void	SendRainbowRaceMiniGameState( __MINIGAME_PACKET MiniGamePacket );
	void	SendRainbowRaceMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket );
	void	SendRainbowRaceReqFinish();

//sun: 13, 하우징 시스템
private:
	void	OnHousingAllInfo( CAr & ar );
	void	OnHousingSetFunitureList( CAr & ar );
	void	OnHousingSetupFurniture( CAr & ar );
	void	OnHousingPaperingInfo( CAr & ar );
	void	OnHousingSetVisitAllow( CAr & ar );
	void	OnHousingVisitableList( CAr & ar );
public:
	void	SendHousingReqSetupFurniture( HOUSINGINFO housingInfo );
	void	SendHousingReqSetVisitAllow( DWORD dwPlayerId, BOOL bAllow );
	void	SendHousingVisitRoom( DWORD dwPlayerId );
	void	SendHousingReqVisitableList();
	void	SendHousingGoOut();

//sun: 13, 커플 시스템
	void	SendPropose( const char* pszTarget );
	void	SendRefuse();
	void	SendCouple();
	void	SendDecouple();
	void	OnCouple( CAr & ar );
	void	OnProposeResult( CAr & ar );
	void	OnCoupleResult( CAr & ar );
	void	OnDecoupleResult( CAr & ar );

	void	OnAddCoupleExperience( CAr & ar );		//sun: 13, 커플 보상

//sun: 13, 퀘스트 NPC위치 알림
private:
	void	OnNPCPos( CAr & ar );
public:
	void	SendReqNPCPos( const char* szCharKey );

	void	OnPCBangInfo( CAr & ar );
	void	OnAccountPlayTime( CAr & ar );

	void	SendSmeltSafety( OBJID objid, OBJID objMaterialId, OBJID objProtScrId, OBJID objSmeltScrId = NULL_ID );
	void	OnSmeltSafety( CAr & ar );

	void	OnWorldReadInfo( CAr & ar );
	void	SendMapKey( const char* szFileName, const char* szMapKey );

	void	OnQuizSystemMessage( CAr & ar );
	void	OnQuizEventState( CAr & ar );
	void	OnQuizEventMessage( CAr & ar );
	void	OnQuizQuestion( CAr & ar );
	void	SendQuizEventEntrance();
	void	SendQuizEventTeleport( int nZone );

	void	SendRemoveVis( int nPos );
	void	SendSwapVis( int nPos1, int nPos2 );
	void	OnActivateVisPet( CAr & ar );
	void	OnChangeMoverSfx( OBJID objId, CAr & ar );

	void	SendBuyGuildHouse();
	void	SendGuildHouseEnter( DWORD dwNpcIndex = NULL_ID );
	void	SendGuildHouseGoOut();
	void	OnGuildHousePacket( CAr & ar );
	void	OnGuildHouseAllInfo( CAr & ar );
	void	OnGuildHouseRemove( CAr & ar );
	void	OnRestPoint( CAr & ar );

	void	SendTeleporterReq( const char* szCharKey, int nIndex );

	void	OnCheckedQuest( CAr & ar );
	void	SendCheckedQuestId( int nQuestId, BOOL bCheck );

	void	OnInviteCampusMember( CAr & ar );
	void	OnUpdateCampus( CAr & ar );
	void	OnRemoveCampus( CAr & ar );
	void	OnUpdateCampusPoint( CAr & ar );
	void	SendInviteCampusMember( u_long idTarget );
	void	SendAcceptCampusMember( u_long idRequest );
	void	SendRefuseCampusMember( u_long idRequest );
	void	SendRemoveCampusMember( u_long idTarget );

	void	SendSelectedAwakeningValue( DWORD dwObjID, SERIALNUMBER iSerialNum, BYTE bySelectFlag );
//	void	OnSafeAwakening( CAr& ar );

//////////////////////// 엘딘의 항아리 //////////////////////////////////////

	void	SendUnSealItem( DWORD dwUnSealKeyItemIndex, DWORD dwUnSealItemIndex );
	void	SendFillElldinsJarWithPotion( DWORD dwPotionItemIndex, DWORD dwElldinsJarItemIndex );

//////////////////////// 협동 기부 //////////////////////////////////////
	void	SendCooperativeContiributions_Info( const DWORD dwObjidNPC );
	//void	SendCooperativeContributions_ContributeItem();
	//void	SendCooperativeContributions_ContributeGold();
	void	SendCooperativeContributions_Contribute( const DWORD dwObjidNPC );
	void	SendCooperativeContributions_GetRanking( const DWORD dwObjidNPC );

	void	OnCooperativeContributions_Info( CAr & ar );
	//void	OnCooperativeContributions_ContributeItemAck( CAr & ar );
	//void	OnCooperativeContributions_ContributeGoldAck( CAr & ar );
	void	OnCooperativeContributions_ContributeAck( CAr & ar );
	void	OnCooperativeContributions_GetRankingAck( CAr & ar );
	void	OnCooperativeContributions_SetGlobalBuffReward( CAr & ar );


//////////////////////// 협동 기부 //////////////////////////////////////

#ifdef __GUILD_HOUSE_MIDDLE
private:
	void	OnGuildHouseTenderMainWnd( CAr & ar );
	void	OnGuildHouseTenderInfoWnd( CAr & ar );
	void	OnGuildHouseTenderResult( CAr & ar );
	void	OnGuildHouseInfoWnd( CAr & ar );
public:
	void	SendReqGuildHouseTenderMainWnd( DWORD dwGHType, DWORD dwNpcIndex );
	void	SendReqGuildHouseTenderInfoWnd( DWORD dwGHIndex );
	void	SendReqGuildHouseTenderJoin( DWORD dwGHIndex, int nTenderPerin, int nTenderPenya );
	void	SendReqGuildHouseInfoWnd( const PT_PACKET_GUILDHOUSE_INFO ptGuildHouseInfo );
	void	SendChangeGuildHouseComment( const PT_PACKET_GUILDHOUSE_INFO ptGuildHouseInfo );
#endif // __GUILD_HOUSE_MIDDLE

//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

public:
	void	SendCouponNumber( const PT_PACKET_COUPON_NUMBER ptCouponNumber );

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// mirchang_20100722 item link
#ifdef __HYPERLINK_ITEM16

private:
	void	OnItemLinkInfo( CAr & ar );
public:
	void	SendReqItemLinkInfo( const PT_PACKET_ITEM_LINK ptItemLink );

#endif // __HYPERLINK_ITEM16
// mirchang_20100722 item link
//////////////////////////////////////////////////////////////////////////


	void	OnSetSleeping( OBJID objid, CAr & ar );


#ifdef __ENCHANT_BARUNA16

public:
	void	SendReqOperExtract( const PT_PACKET_OPER_EXTRACT ptOperExtract );
	void	SendReqOperCreate( const PT_PACKET_OPER_CREATE ptOperCreate );
	void	SendReqCidCreate( const PT_PACKET_CID_CREATE ptCidCreate );
	void	SendReqCidPieceUpgrade( const PT_PACKET_CIDPIECE_UPGRADE ptCidPieceUpgrade );
	void	SendReqOperCidCreate( const PT_PACKET_OPERCID_CREATE ptOperCidCreate );

private:
	void	OnResultOperExtract( CAr & ar );
	void	OnResultOperCreate( CAr & ar );
	void	OnResultCidCreate( CAr & ar );
	void	OnResultCidPieceUpgrade( CAr & ar );
	void	OnResultOperCidCreate( CAr & ar );

	void	OnCallBarunaNPC( CAr & ar );
	void	OnReleaseBarunaNPC( CAr & ar );
	void	OnBarunaNPCInfo( CAr & ar );

#endif // __ENCHANT_BARUNA16


private:			//콜로세움
	void	OnColosseum_Enter( CAr & ar );
	void	OnColosseum_Retry( CAr & ar );

	void	OnColosseum_AutoInviteReq( CAr & ar );
	void	OnColosseum_AutoStartWaitingTick( CAr & ar );
	void	OnColosseum_StageWaitingTick( CAr & ar );

	void	OnColosseum_StartedStage( CAr & ar );
	void	OnColosseum_ClearedStage( CAr & ar );
	void	OnColosseum_StageTimeLimit( CAr & ar );
	void	OnColosseum_AllDie( CAr & ar );
	void	OnColosseum_Retried( CAr & ar );
	void	OnColosseum_AllClear( CAr & ar );
	void	OnColosseum_RegistAdditionalRealMonsterRateItemAck( CAr & ar );		//등록 : 콜로세움 진 몬스터 확률 증가 아이템
	void	OnColosseum_UnregistAdditionalRealMonsterRateItemAck( CAr & ar );	//해제 : 콜로세움 진 몬스터 확률 증가 아이템
	void	OnColosseum_RegistedAdditionalRealMonsterRateItemNoti( CAr & ar );	//등록 정보 : 콜로세움 진 몬스터 확률 증가 아이템
	void	OnColosseum_RankingInfo( CAr & ar );								//랭킹 정보 : 콜로세움 

public:
	//////////////////////////////////////////////////////////////////////////
	// shopping basket
	void	SendReqBuyItem( const PT_PACKET_SHOPPING_BASKET ptBasket );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// reset bind
	void	SendReqResetBind( const PT_PACKET_RESET_BIND ptResetBind );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// item merge
	void	SendReqItemMerge( const PT_PACKET_ITEM_MERGE ptItemMerge );
	//////////////////////////////////////////////////////////////////////////

	void	SendPacket( const FLPacket* pPacket );

	void	SendTeleportToNPC( const DWORD dwNPC_ObjID, const int nMMI_ID );

private:
	//////////////////////////////////////////////////////////////////////////
	// reset bind
	void	OnResultResetBind( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// shopping basket
	void	OnResultBuyItem( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// item merge
	void	OnResultItemMerge( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open TelePort
	void	OnResultOpenTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// random option
	void	OnResultRandomOptionExtension( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// item upgrade
	void	OnResultUpgradeItemGeneralEnchant( CAr & ar );
	void	OnResultUpgradeItemAttributeEnchant( CAr & ar );
	void	OnResultUpgradeItemPiercingItemInsert( CAr & ar );
	void	OnResultUpgradeItemRandomOptionGenerate( CAr & ar );
	void	OnResultUpgradeItemRandomOptionGenerateRetry( CAr & ar );
	void	OnResultUpgradeItemRandomOptionInitialize( CAr & ar );
	void	OnResultUpgradeItemEquipLevelDecrease( CAr & ar );
	void	OnResultUpgradeItemEquipLevelInitialize( CAr & ar );
#ifdef BARUNA_ULTIMATE_UPDATE
	void	OnResultUpgradeItemUltimateTrans( CAr & ar );
#endif
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Complete TelePort
	void OnResultCompleteTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// AddPoint TelePort
	void OnResultAddPointTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// DelPoint TelePort
	void OnResultDelPointTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// DelAllPoint TelePort
	void OnResultDelAllPointTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ChangePointName TelePort
	void OnResultChangePointNameTeleport( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	//_JIN_WORLD_FREETICKET
	void OnResultChargeZoneTicketEnterance(  CAr& ar );
	void OnResultChargeZoneTicketLeave(  CAr& ar );

	//	_JIN_TREASURE_CHEST
	void OnResultTreasureChestOpen(  CAr& ar );
	void OnResultTreasureChestOpen_ByKey(  CAr& ar );

	//////////////////////////////////////////////////////////////////////////
	// broadcast message
	void	OnBroadCastMessage( CAr & ar );
	//////////////////////////////////////////////////////////////////////////

	void	OnResultCharacterServerTransformOpen( CAr & ar );
	void	OnCharacterServerTransformAck( CAr & ar );
	void	OnJoin_InShutdownRule( CAr & ar );

	void	OnSetAuthorizationNoti( CAr & ar );

// 콜로세움
public:
	void SendColosseumEnter( const T_PACKET_COLOSSEUM_ENTER_REQ* ptData );				//입장요청
	void SendColosseumInviteRst( const T_PACKET_COLOSSEUM_AUTO_INVITE_ACK* ptData );	//입창 초대에 대한 응답
	void SendColosseumStart( );															// 시작요청
	void SendColosseumRetry( DWORD dwRetryItemObjId = 0 );								// 재시도
	void SendColosseumOut( );															//나가기 요청

	void SendColosseumRegistAdditionalRealMonsterRateItem( const DWORD dwItemObjID );	//콜로세움 진몬스터 확률 증가 아이템 등록
	void SendColosseumUnregistAdditionalRealMonsterRateItem();							//콜로에숨 진몬스터 확률 증가 아이템 등록해제
	void SendColosseumGetRankingInfo();													//랭킹 정보 요청
	
private:
	void OnColosseumEnter( CAr & ar );				//입장 , 파티장만 수신함.
	void OnColosseumInvite( CAr & ar );				//파티장으로부터 콜로세움 입장 초대
	void OnColosseumWaitingStart( CAr& ar );		//시작대기 ( 파티원 초대하는.. ) 
	void OnColosseumWaitingStageReady( CAr& ar );   //스테이지 준비 시작
	void OnColosseumStartedStage( CAr& ar );		//스테이지 시작됨
	void OnColosseumEndedState( CAr& ar );			//스테이지 종료
	void OnColosseumFailedOverTime( CAr& ar );  	// 실패, 시간초과
	void OnColosseumFailedAllDie( CAr& ar );		// 실패, 전부 죽음
	void OnColosseumRetry( CAr & ar );				// 재도전 결과 / 대기 시작
	void OnColosseumAllCleared( CAr & ar );			// 콜로세움 종료

// EVENT ARENA
	
private:
	void OnEArena_TeamInfo( CAr& ar );				//팀정보
	void OnEArena_TeamMemberLogin( CAr& ar );		//팀원 로긴
	void OnEArena_TeamMemberLogOut( CAr& ar );		//팀원 로그아웃
	void OnEArena_TeamMemberState( CAr& ar );		//팀원 상태업데이트
	void OnEArena_CallPlayer( CAr& ar );			//소환요청
	void OnEArena_BattleResult( CAr& ar );			//전투결과
	void OnEArena_GameResult( CAr& ar );			//경기결과
	void OnEArena_TeamInfoForView( CAr& ar );		//뷰정보를 위한
	void OnEArena_ScoreBoard( CAr& ar );			//스코어보드
	void OnEArena_BattleResultLog( CAr& ar );		//전투 결과 로그

	//mng
	void OnEArena_Mng_OpenWindow( CAr& ar );		//이하 관리자
	void OnEArena_Mng_CloseWindow( CAr& ar );
	void OnEArena_Mng_SyncInfo( CAr& ar );
	void OnEArena_Mng_WaitPlayer( CAr& ar );
	void OnEArena_Mng_WaitEntrance( CAr& ar );
	void OnEArena_Mng_TeamInfo( CAr& ar );
	void OnEARena_Mng_CloseTeamerInfo( CAr& ar );


	void	OnConsignmentSale_UserInfoAck( CAr & ar );
	void	OnConsignmentSale_SellItemNoti( CAr & ar );
	void	OnConsignmentSale_RegistAck( CAr & ar );
	void	OnConsignmentSale_CancelRegistedAck( CAr & ar );
	void	OnConsignmentSale_BuyItemAck( CAr & ar );
	void	OnConsignmentSale_CollectSaleGoldAck( CAr & ar );
	void	OnConsignmentSale_SearchAck( CAr & ar );
	void	OnConsignmentSale_SaleGoldInfoListAck( CAr & ar );
	void	OnConsignmentSale_RegistedInfoListAck( CAr & ar );
	void	OnConsignmentSale_TaxAck( CAr & ar );

	//양손무기 합성
	void	OnCombine_Result( CAr & ar );
	void	OnCombineCancel_Result( CAr & ar );
	void	OnCombineCancel_Data( CAr& ar );
	void	OnCombinedOption( CAr & ar );

	//외형변경
	void	OnChangedLooks( CAr& ar );
	void	OnInitChangedLooks( CAr& ar );

	void	OnUseActionSlotNoti( CAr & ar );

	void	OnRandomOptionSyncAck( CAr & ar );

	void	OnChangePartyKindNoti( CAr & ar );

	void	OnSetPartyEffectModeNoti( CAr & ar );

	// 마드리갈의 선물
	void	OnMadrigalGiftPointNoti( CAr & ar );
	void	OnMadrigalGiftItemRectAck( CAr & ar );

	// 프리프 조각 시스템
	void	OnFlyffPieceExchangeList( CAr & ar );
	void	OnExchangeFlyffPieceItemAck( CAr & ar );
	
//________________________________________________________________________________
//	Operator commands
	void	OnShout( CAr & ar );
	void	OnPlayMusic( CAr & ar );
	void	OnPlaySound( CAr & ar );
	void	OnGetPlayerAddr( CAr & ar );
	void	OnGetPlayerCount( CAr & ar );
	void	OnGetCorePlayer( CAr & ar );
	void	OnSystem( CAr & ar );
	void	OnCaption( CAr & ar );
	
	void    OnDisguise( OBJID objid, CAr & ar );
	void    OnNoDisguise( OBJID objid, CAr & ar );

	void	OnSellItemToNPCAck( CAr & ar );

	void	OnActionSlotSkillSkip( CAr & ar );

	void	OnRefreshBuffAdjValue( CAr & ar );

	void	OnMadrigalSystemActivation( CAr & ar );
	void	OnCommonItemExchangeAck( CAr & ar );		// 범용 아이템 교환 
#ifdef PAT_LOOTOPTION
	void	OnPetLootOption( CAr & ar );
#endif // PAT_LOOTOPTION

#ifdef CARD_UPGRADE_SYSTEM
	void	OnCardUpgrade( CAr & ar );
#endif	// CARD_UPGRADE_SYSTEM
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void	OnResultUpgradeItemSetGem( CAr & ar );
	void	OnResultUpgradeItemRemoveGem( CAr & ar );
#endif
#ifdef COSTUME_UPGRADE_MIX
	void	OnResultUpgradeItemMix( CAr & ar );
#endif

private:
	BOOL	m_bEventTextColor;
};

#endif	// __DPCLIENT_H__