// Mover.h: interface for the CMover class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)
#define AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <D3D9.h>
#include <basetsd.h> 
#include <set>

#include "Ctrl.h"
#include "ModelObject.h"
#include "..\_AIInterface\AIInterface.h"
#include "..\_AIInterface\ActionMover.h"
#include "MoverMsg.h"
#include "SkillInfluence.h"
#include "authorization.h" 
#include "NpcProperty.h"
#include "party.h"
#include "CooltimeMgr.h"

#ifdef __BEAST
	#include "Respawn.h"
#else
	#include "mempooler.h"
	typedef	MemPooler<CMover>	CMoverPool;
#endif	// __BEAST

#define	CRIME_TIME				30				// 범죄를 저질렀다가 다시 풀리는 시간.
#define	MAX_NAME				64

#define	COLOR_PLAYER			0xffffffff
#define	COLOR_NPC				0xffa0a0ff  
#define	COLOR_MONSTER			0xffffffa0
#define	COLOR_PARTY				0xff70f070
#define COLOR_GUILDCOMBAT		0xff00ff00

#define	EXFLAG_SUPERPOWER		0x00000001
#define	EXFLAG_IMMORTALITY		0x00000002


#define	MCS_NORMAL				0
#define	MCS_DAMAGE				1
#define	MCS_DIE					2

// SetMotion's option
#define	MOP_NONE				0x00000000
#define	MOP_SWDFORCE			0x00000001		// 검광 생성하게...
#define	MOP_FIXED				0x00000002		// dwMotion값을 그대로 로딩하게.(+100안하게..)
#define	MOP_NO_TRANS			0x00000004		// 모션 트랜지션 하지않음.
#define	MOP_HITSLOW				0x00000008		// 타점이후부터 슬로우.

// 응답요청 플래그
#define	REQ_USESKILL			0x00000001		// 스킬사용 응답.
#define	REQ_MOTION				0x00000002		// 모션 응답

#define	OBJTYPE_PLAYER			0x00000001		
#define	OBJTYPE_MONSTER			0x00000002		// 이거 두개도 장차 m_bPlayer를 없애고 통합시켜야 할거 같다.
#define OBJTYPE_CTRL			0x00000004		// 
#define	OBJTYPE_FLYING			0x00010000		// m_dwTypeFlag에서 사용.

#define	CRITICAL_PROBABILITY	3
#define	CRITICAL_BERSERK_HP		30
#define	CRITICAL_BERSERK_PROB	22
#define	CRITICAL_AFTER_DEATH	22

#define	MAX_MOVING_ACTMSG		32
#define MAX_SKILL				16
#define MAX_SKILL_LEVEL			10

// UPDATEITEM
#define	UI_NUM					0
#define	UI_HP					1
#define	UI_RN					2
#define	UI_AO					3
#define	UI_RAO					4
#define	UI_IR					5
#define	UI_PIERCING_SIZE		6
#define	UI_PIERCING				7
#define UI_COOLTIME				8
#define	UI_FLAG		9

#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL )
	

#ifdef __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 1 )
#else	// __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 60 )
#endif	// __INTERNALSERVER
#define	MAX_CHEERPOINT	3
	
#ifdef __XITEMBUFF
#define	NULL_CHGPARAM	0x7FFFFFFF
#else	// __XITEMBUFF
#define	NULL_CHGPARAM	0
#endif	// __XITEMBUFF
	
	
// 스킬 사용 타입
enum SKILLUSETYPE
{
	SUT_NORMAL,			// 단축키에 넣고 사용하는 일반적인 방식.
	SUT_QUEUESTART,		// 스킬큐로 처음 시작하는 스킬이다.(캐스팅을 몰아서함)
	SUT_QUEUEING,		// 스킬큐의 연속발사중 스킬이다.(캐스팅동작없이 바로 발사)
};

////////////////////////////////////////////////////////////////////////////////
// PVP 관련 
////////////////////////////////////////////////////////////////////////////////

enum PVP_MODE
{
	PVP_MODE_NONE,				// PVP불가 	
	PVP_MODE_GUILDWAR,			// 길드워 
	PVP_MODE_PK,				// PK
	PVP_MODE_DUEL				// 듀얼 
};

enum PK_TYPE
{
	NORMAL_AND_NORMAL,			// 일반 유저 와 일반 유저 
	NORMAL_AND_SEMI,
	NORMAL_AND_CHAO,
	SEMI_AND_NORMAL,			// 준카오와 일반 유저 
	SEMI_AND_SEMI,
	SEMI_AND_CHAO,
	CHAO_AND_NORMAL,			// 카오와 일반 유저 
	CHAO_AND_SEMI,
	CHAO_AND_CHAO
};

enum CHANGE_SLAUGHTER_TYPE
{
	CHANGE_SLAUGHTER_ATTACK,	// 'PK시도'로 인한 카르마(=슬로터값) 변경 
	CHANGE_SLAUGHTER_KILL,		// PK로 인한 카르마(=슬로터값) 변경 
	CHANGE_SLAUGHTER_RECOVERY,	// 자연회복 
	CHANGE_SLAUGHTER_SET,		// 셑팅
};

enum TRADE_CONFIRM_TYPE
{
	TRADE_CONFIRM_ERROR,			// 인벤에 영유가 없거나, GOLD가 OVERFLOW나는 경우 
	TRADE_CONFIRM_OK,				// 상대도 ok(최종 거래)
};

enum TRADE_STATE
{
	TRADE_STEP_ITEM,			// 아이템 단계 - 아이템을 올리고 내리고  m_bTradeOk = FALSE
	TRADE_STEP_OK,			    // OK누른 단계 - ok버튼을 눌렸다.        m_bTradeOk = TRUE
	TRADE_STEP_CONFIRM,			// CONFIRM단계 - 최종확인단계            m_bTradeOkConfirm = TRUE
};

////////////////////////////////////////////////////////////////////////////////
// struct
////////////////////////////////////////////////////////////////////////////////
#ifdef __FINITEITEM0705
typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
}	EQUIP_INFO,	*PEQUIP_INFO;
#endif	// __FINITEITEM0705

#ifdef __QUERYEQUIP0630
typedef	struct	_EQUIP_INFO_ADD
{
	int		nRandomOptItemId;
	PIERCINGINFO	piercingInfo;
	BYTE	bItemResist;
	int		nResistAbilityOption;
#ifdef __CLIENT
	CTexture*	pTexture;
#endif	// __CLIENT
}	EQUIP_INFO_ADD, *PEQUIP_INFO_ADD;
#endif	// __QUERYEQUIP0630

typedef struct tagPLAYER
{
	TCHAR		m_szName[ 32 ];
#ifdef __FINITEITEM0705
	EQUIP_INFO	m_aEquipInfo[MAX_HUMAN_PARTS];
#else	// __FINITEITEM0705
	DWORD	m_adwEquipment[ MAX_HUMAN_PARTS ];
#endif	// __FINITEITEM0705
	char		m_byCostume;
	char		m_bySkinSet;
	char		m_byFace;
	char		m_byHairMesh;
	char		m_byHairColor;
	char		m_byHeadMesh;
	char		m_bySex;
	char		m_byJob;
	u_short		m_uSlot;
} PLAYER,* LPPLAYER;

typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
	// 추가 
#ifdef __V050322_CACHEQUEST
	BYTE		m_nKillNPCNum[ 2 ];
	BYTE		m_bPatrol   : 1;
	BYTE		m_bDialog   : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;

	QuestProp* GetProp() { return prj.m_aPropQuest.GetAt( m_wId ); }
#endif 
} QUEST,* LPQUEST;

typedef	struct	tagMOVING_ACTMSG
{
	D3DXVECTOR3	vPos;
	BYTE		fMoving, fForward;
	DWORD		dwMsg;
	int			nParam1, nParam2;
	float		fAngle;
} MOVING_ACTMSG, *LPMOVING_ACTMSG;

typedef	struct tagCORR_ACTION
{
	float		f;
	float		fSpeed;
	DWORD		dwState;
	DWORD		dwStateFlag;
	DWORD		dwMotion;
	int			nMotionEx;
	int			nLoop;
	DWORD		dwMotionOption;
	D3DXVECTOR3	v;
	D3DXVECTOR3	vd;
	float		fAngleX;
	float		fAccPower;
	float		fTurnAngle;
	BOOL		fValid;
	BOOL		fBehavior;
#ifdef __J0518
	BOOL		fHalf;
#endif	// __J0518
} CORR_ACTION, *PCORR_ACTION;

struct HIT_INFO
{
	int		nHit;		// 데미지 
	DWORD	dwTick;		// 막타의 틱 
};
typedef map<OBJID, HIT_INFO> SET_OBJID;

#ifdef __NPP_050308

enum HITTYPE
{
	HITTYPE_FAIL	= 0,
	HITTYPE_GENERIC,
	HITTYPE_PVP,
	HITTYPE_WAR,
	HITTYPE_SCHOOL,
	HITTYPE_PK,
#ifdef __GUILDCOMBAT
	HITTYPE_GUILDCOMBAT,
#endif // __GUILDCOMBAT
};

typedef struct RESURRECTION_DATA
{
	u_long		  dwPlayerID;
	BOOL		  bUseing;
	ItemProp	 *pSkillProp;
	AddSkillProp *pAddSkillProp;
	int			  nDamage;
} RESURRECTION_DATA;

#endif //__NPP_050308

struct VENDOR_SELL_RESULT
{
	CItemElem	item;			// 팔린 아이템 
	int			nRemain;		// 팔리고 남은 갯수 
	int			nErrorCode;		// 에러코드 
};

// 개인상점과 거래 클래스 
class CVTInfo
{
private:
	DWORD					m_dwTradeGold;					// 거래중인 돈 
	CItemBase*				m_apItem_VT[MAX_VENDITEM];		// vendor and trader share pointer array
	//CMover*					m_pOther;						// 거래나 개인상점의 상대방 
	OBJID					m_objId;
	CMover*					m_pOwner;						// 클래스 소유자 
	TRADE_STATE				m_state;						// 거래상태 
	string					m_strTitle;						// 개인상점의 타이틀 

public:
	OBJID					GetOtherID() { return m_objId; }
	CMover*					GetOther();
	void					SetOther( CMover* pMover );
	void					Init( CMover* pMover );
	CItemBase*				GetItem( BYTE i );
	void					SetItem( BYTE i, CItemBase* pItemBase );
	LPCTSTR					GetTitle();
	void					SetTitle( LPCTSTR szTitle );
	BOOL					IsVendorOpen();

	void					TradeClear();
	void					TradeSetItem( BYTE nId, BYTE i, short nItemNum );
	BOOL					TradeClearItem( BYTE i );
	void					TradeSetGold( DWORD dwGold );
	int						TradeGetGold();
	BOOL					TradeConsent();
	BOOL					TradeSetItem2( BYTE nId, BYTE i, short nItemNum );
	TRADE_CONFIRM_TYPE		TradeLastConfirm( CAr & ar );
	TRADE_STATE				TradeGetState();
	void					TradeSetState( TRADE_STATE state );

	void					VendorClose( BOOL bClearTitle = TRUE );	// 개인 상점 닫기
	void					VendorCopyItems( CItemBase** ppItemVd );
	void					VendorItemNum( BYTE i, short nNum );
	void					VendorSetItem( BYTE nId, BYTE i, short nNum, int nCost );
	BOOL					VendorClearItem( BYTE i );
	BOOL					VendorSellItem( CMover* pBuyer, BYTE nItem, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result );
	BOOL					VendorIsVendor(); 
};


class	CGuild;
class	CGuildWar;
class	CNpcProperty;
struct	ATTACK_INFO;
struct	REGIONELEM;

class CMover : public CCtrl
{	
#ifdef __BEAST
	int Null() { return 0; }
#endif
public:	
	CMover();
	virtual ~CMover();

	friend			CActionMover;
	enum			{ PLAYER, NPC };	// CMover의 타입 

private:
	SET_OBJID		m_idEnemies;		
	CRIT_SEC		m_LockofidEnemies;

	OBJACT			m_oaCmd;			// 오브젝트 명령  
	int				m_nCParam[16];		// 범용 명령 파라메터
	OBJACT			m_oaAct;			// 오브젝트 행동상태
	int				m_nAParam[16];		// 범용 행동 파라메터

	LONG			m_adjParamAry[ MAX_ADJPARAMARY ];
	LONG			m_chgParamAry[ MAX_ADJPARAMARY ];   

	BOOL			m_bRegenItem;		// NPC의 경우...

#if	defined(__XFLYAI0524) && !defined(__BEAST)
	int				m_nMovePattern;		// 패턴 이동 타입
	int				m_nMoveEvent;		// 패턴 이동때 쓰이는 상태변화  99면 상태끝.
	int				m_nMoveEventCnt;	// 패턴이동때 쓰이는 상태변화카운터.
#endif // defined(__XFLYAI0524) && !defined(__BEAST)

public:

#if !defined(__BEAST)
	NaviPoint		m_nvPoint;
	V_NaviPoint		m_vOtherPoint;
#endif

	int				m_nReflexDmg;				// 리플렉스 데미지 누적.(서버만 )

private:
#ifdef	__CLIENT
	#ifdef	__XLADOLF
	DWORD			m_dwLadolfFlag;
	CModelObject*   m_pLadolf;		// 라돌프모델
	#endif //__XLADOLF
	int				m_nDmgCnt;					// 데미지 카운트
	MOVING_ACTMSG	m_qMovingActMsg[MAX_MOVING_ACTMSG];	// circular queue
	u_long			m_uHead;
	u_long			m_uTail;
#endif // __CLIENT

protected:
	DWORD			m_dwGold;
	DWORD			m_dwRideItemIdx;			// 탈것의 아이템 인덱스, 그런데 말이나 그런 것들은 어케하지??
	CModelObject*	m_pRide;					// 탈것
	int				m_nCount;					// 무버가 범용으로 쓰는 순차적 카운터. 생성자 외엔 0으로 초기화 하지 말것.

#ifdef	__BEAST
public:
#endif	//__BEAST
	TCHAR			m_szName[MAX_NAME];			// 캐릭터의 이름 스트링 

public:
	u_long			m_idPlayer;					// NPC는, 0xffffffff
	BOOL			m_bPlayer;					// Player인가, 아니라면 NPC
	CActionMover*	m_pActMover;				// 무버를 움직이는 객체. 여기서 무버의 움직임을 컨트롤한다.
	DWORD			m_dwTypeFlag;				// 다용도 Type플래그(비행형이냐?...)  OBJTYPE_....
	DWORD			m_dwMode;

	LONG			m_nStr, m_nSta, m_nDex, m_nInt;
	LONG			m_nLevel;
	LONG			m_nDeathLevel;
	EXPINTEGER	m_nExp1, m_nExp2, m_nDeathExp; // DeathExp - 죽었을 때의 내 경험치
	LONG			m_nFlightLv, m_nFxp;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint; 
	LONG			m_nRemainGP, m_nRemainLP;
	LONG			m_nDefenseMin, m_nDefenseMax;
	int				m_nAdjHitRate, m_nAdjParry;
	BYTE			m_bySex;
	LONG			m_nJob;
	DWORD			m_aJobLv[MAX_JOB]; 
	DWORD			m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHairColor, m_dwHeadMesh;
	FLOAT			m_fHairColorR, m_fHairColorG, m_fHairColorB;	

	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];
	DWORD			m_dwSkillPoint;
	DWORD			m_tmActionPoint;	// 3초마다 액션포인트 참.
	TCHAR			m_szCharacterKey[32];

	OBJID			m_idDest;					// 현재 오브젝트가 포커스잡고 있는 오브젝트의 아이디
	float			m_fArrivalRange;			// m_idDest의 반경에 얼마만큼 근접해야하는가. 미터단위. 디폴트 0
	D3DXVECTOR3		m_vDestPos;
	CShip		   *m_pIADestPos;				// 여기에 포인터가 있다면 m_vDestPos의 좌표는 m_pIADestPos오브젝에 대한 상대좌표가 된다.
	CORR_ACTION		m_CorrAction;
	int				m_nCorr;					// 남아있는 동기 프레임 수(실체)
	u_long			m_uRemnantCorrFrm;			// 남아있는 동기 프레임 수(고스트)
	bool			m_bForward;
	bool			m_bPositiveX;
	bool			m_bPositiveZ;
	bool			m_bLeft;
	float			m_fDestAngle;
	FLOAT			m_fTurnAngle;	
	BOOL			m_fWaitQueryGetPos;
	DWORD			m_dwRegionAttr;
	DWORD			m_dwOldRegionAttr;

	BOOL			m_bActiveAttack;			// 선공몹인가?
	DWORD			m_dwVirtItem;				// npc가 사용(공격)할 아이템. 또는 플레이어가 무기 비장착시 사용할 기본 아이템. 
	DWORD			m_dwVirtType;				// m_dwVirtItem가 아이템인지 스킬인지 결정 VT_ITEM, VT_SKILL
	DWORD			m_dwBelligerence;			// 호전성 여부 

	OBJID			m_idPet;					// 펫을 소환하면 값이 있고 없으면 널아뒤.

	CNpcProperty*	m_pNpcProperty;				// 마을 NPC 관련 (대사, 퀘스트, 파일이름 ) 


	DWORD			m_dwAuthorization;
	u_long			m_idparty;					// 클라에선 특별한 경우가 아니면 이값을 사용해선 안된다. 정확하지 않다. g_Party.m_uPartyId를 사용할것.
	D3DXVECTOR3		m_vMarkingPos;				// 로드라이트에서 마킹한 좌표 
	OBJID			m_idMarkingWorld;			// 로드라이트에서 마킹한 월드 아이디 
	CTimer			m_timerRecovery;
	CTimer			m_timerRecoveryStand;
	CTimer			m_timerDuel;
	CTimer			m_timerEndDuel;
	int				m_nDuelState;
	DWORD			m_dwKarmaTick;				// 카르마 회복용 

	// 퀘스트 관련 
#ifdef  __V050322_CACHEQUEST
	LPQUEST			m_aQuest;
	LPWORD   		m_aCompleteQuest; 
	BYTE			m_nQuestSize;
	BYTE			m_nCompleteQuestSize;
	//CPtrArray       m_apCacheQuest;

#ifdef __CHEER0615
public:
	DWORD	m_dwTickCheer;
	int		m_nCheerPoint;
public:
	void	CheckTickCheer( void );
	void	SetCheerParam( int nCheerPoint, DWORD dwTickCount, DWORD dwRest );
#endif	// __CHEER0615

#ifdef __WORLDSERVER
	CTimer          m_timerQuestLimitTime;
	DWORD	        m_dwPKTargetLimit;			// PK선공제한시각
#ifdef __INSTANT_BANK0831
	DWORD		m_dwTickCreated;
	void	ProcInstantBanker( void );
#endif	// __INSTANT_BANK0831
#endif // __WORLDSERVER
#ifdef __CLIENT
// 퀘스트 아이콘 확축 프로세스 
	static BOOL m_bQuestEmoticonAdd;
	static FLOAT m_fQuestEmoticonScale;
	int				m_nQuestEmoticonIndex;
	bool			m_bShowQuestEmoticon;
#endif // __CLIENT
#else // __V050322_CACHEQUEST
//{{AFX
	QUEST			m_aQuest[MAX_QUEST];
	BYTE			m_nQuestSize;
	LPQUEST			m_pCacheQuest;
//}}AFX
#endif // __V050322_CACHEQUEST

	DWORD           m_dwCurQuest;
	void	AllocShopInventory( LPCHARACTER pCharacter );
	BOOL	IsVendorNPC();
	// Item 관련 
	CItemContainer< CItemElem  >*	m_ShopInventory[ MAX_VENDOR_INVENTORY_TAB ];
	CItemContainer< CItemElem  >	m_Inventory ;

#ifdef __FINITEITEM0705
	EQUIP_INFO	m_aEquipInfo[MAX_HUMAN_PARTS];
	void	ClearEquipInfo( void )
		{
			memset( m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
			for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
				m_aEquipInfo[i].dwId	= NULL_ID;
		}
#ifdef __CLIENT
	void	CreateAbilityOption_SetItemSFX( int nAbilityOption );
	BOOL	GetEquipFlag( int nParts, BYTE* pbyFlag );
#endif	// __CLIENT
#else	// __FINITEITEM0705
	DWORD	m_adwEquipment[MAX_HUMAN_PARTS];
#endif	// __FINITEITEM0705


	int				m_nNumKill;			// 킬수
	DWORD			m_tmCrime;			// 범죄저지른 시간.
	int				m_nSlaughter;		// 슬로터 포인트
	
#ifdef __GUILDCOMBAT
	int				m_nGuildCombatState;
#endif // __GUILDCOMBAT
#ifdef __NPP_050308
	int				m_nRecoverChao_Slaughter;		// 시련모드일때 슬로터 포인트
	RESURRECTION_DATA m_Resurrection_Data;
#endif //__NPP_050308	
#ifdef __S0504_FLY
	int				m_nReadyTime;					// 비행 시전 시간
	OBJID			m_dwUseItemId;					// 비행 시전 시간
	DWORD			m_dwStateMode;					// PK, PVP, 비행 시전, 
	BOOL			m_bItemFind;
#endif // __S0504_FLY
	OBJID			m_idLastHitMover;	// this가 마지막으로 쳤던 무버아이디
	int				m_nFame;			// 명성
	int				m_nDuel;			// 듀얼중인가.( 0:없음 1:개인듀얼중 2:파티듀얼중.
	OBJID			m_idDuelOther;		// 듀얼 상대의 아이디
	u_long			m_idDuelParty;		// 파티듀얼중이면 상대방 파티의 id
	u_long			m_idMurderer;		// 날 마지막으로 죽였던넘 플레이어! 아이디
	DWORD			m_tmDead;			// PK당하면 PK당한 시간을 기록.
	int				m_nDead;			// 두번죽는경우때문에 추가함. 죽었을때 카운트를 달아서 그시간이내에 또 죽으면 로그남기고 무효

	BOOL			m_bBank;	
	BYTE			m_nSlot;
	CItemContainer< CItemElem  >	m_Bank[3] ;
	u_long			m_idPlayerBank[3];
	DWORD			m_dwGoldBank[3];

#ifdef __XOPT0617	
	// 클라에서 타플레이어의 경우 장착하고 있는 페이크아이템에 대한 부가 정보
	int				m_nWeaponOption;						// 손에쥔 무기 옵션 
#else
#ifndef __FINITEITEM0705
	int		m_anEquipOption[ MAX_HUMAN_PARTS ];		// 장착아이템들의 옵션.
#endif	// __FINITEITEM0705
#endif	// __XOPT0617
	
	LONG			m_nPlusMaxHitPoint;
	BYTE			m_nAttackResistLeft;
	BYTE			m_nAttackResistRight;
	BYTE			m_nDefenseResist;

#if !defined(__BEAST)
	DWORD			m_dwSMTime[SM_MAX];	// 유료 아이템 시간 값을 가지고 있음
	int				m_nFuel;			// 비행체 현재 연료.
	int				m_tmAccFuel;		// 가속연료(시간단위)

	CSkillInfluence m_SkillState;		// 지속성을 갖는 스킬의 상태 처리(예:프로텍션 ??초동안 방어력 얼마 증가)
	DWORD			m_dwFlag;			// 다용도 플래그.	MVRF_ 시리즈
	
	// 상태 이상
	DWORD			m_tmPoisonUnit;		// 독시간. 독적용단위 시간.
	DWORD			m_tmBleedingUnit;	// 몇초마다 한번씩인가?
	OBJID			m_idPoisonAttacker;	// 독 공격자.
	OBJID			m_idBleedingAttacker;	// 출혈 공격자.
	short			m_wStunCnt;			// 0이 아닌동안엔 스턴이다.
	short			m_wPoisonCnt; 
	short			m_wPoisonDamage;	// 독으로 깎일 데미지.
	short			m_wDarkCnt;			// 암흑 지속시간. - 이변수들이 많아지면 CState로 따로 만들어 쓰자
	short			m_wDarkVal;			// 암흑동안 깎일 명중률 수정치
	short			m_wBleedingCnt; 
	short			m_wBleedingDamage;	// 출혈로 깎일 데미지.
#ifdef __ACROBAT_0504	
	short			m_wDarkCover;		// 안보이게 숨기
#endif //__ACROBAT_0504
		
	OBJID			m_idAttacker;		// this를 실제 공격한넘
	OBJID			m_idTargeter;		// this를 타겟잡고 점유하고 있는넘.
	OBJID			m_idTracking;		// 비행중 자동추적하고 있는 타겟.
	float			m_fExpRatio;		// exp 지급률. this를 죽였을때 this의 exp의 몇퍼센트를 줄것인가. 디폴트는 1.0이다. 이것은 파티때는 적용되지 않는다.
	int				m_nAtkCnt;			// 공격하면 카운터가 0이되면서 계속 증가한다.
	float			m_fAniSpeed;		// 애니메이션 속도 배수. 1.0이 기본. 2.0이면 두배빠르기. 2.0이 맥스다.
	DWORD			m_tmReUseDelay[ MAX_SKILL_JOB ];	// m_aJobSkill의 크기와 같이 맞출것.
	DWORD			m_tmTimer;							// 매 Process마다 시간을 재서 경과된 시간만큼 빼주기 위한것.
	
	#ifdef __CLIENT
		int			m_idSfxHit;
		BYTE		m_nWaterCircleCount;
		int			m_dwReqFlag;		// 응답요청 플래그.
		char		m_szPartyName[32];
		LPDIRECT3DTEXTURE9		m_pCloakTexture;	// 망토텍스쳐.
		float		m_fDestScale;		// 쪼그라들때 부드럽게 하기위한..
		float		m_fDestScaleSlerp;	// 1.0f ~ 0.0으로 변화함.
	#endif // __CLIENT

	#ifdef __WORLDSERVER
		CSfxHitArray	m_sfxHitArray;
		BOOL		m_bLastPK;			// 마지막으로 죽었을때 플레이어에게(듀얼포함) 죽은건가 그외의 경우로 죽은건가.
		BOOL		m_bLastDuelParty;	
	#ifdef __GUILDCOMBAT
		BOOL		m_bGuildCombat;
	#endif // __GUILDCOMBAT
		CTime		m_tGuildMember;
		BYTE		m_nQuestKeeping;	// If rank is super, set it.
	#endif	// __WORLDSERVER
#else
	CRespawnInfo	m_respawnInfo;
	DWORD			m_dwExtraFlag; 
#endif // __BEAST

	#ifdef __WORLDSERVER
		// Monster 측
		OBJID		m_idCollecter;		// this의 채집소유자.
		int			m_nResource;		// 자원 보유량. - 몬스터측에서 사용.
		int			m_nCollectOwnCnt;	// > 0 인상황은 this는 m_idCollecter것이다.
	#endif // __WORLDSERVER

	BOOL			m_bGuildBank;		// 길드 뱅크를 이용중인지 확인
	DWORD			m_idGuildCloak;		// 망토의 길드번호를 가지고 있음
	u_long			m_idGuild;
	u_long			m_idWar;
	FLOAT			m_fCrrSpd;

#if defined(__ITEMCOOLTIME_VER2_050308) && defined(__CLIENT)
	CCooltimeMgr	m_cooltimeMgr;
#endif




	virtual BOOL	Read( CFileIO* pFile );
	virtual BOOL	Write( CFileIO* pFile );
	virtual void	Process( FLOAT fElapsedTime );
	virtual void	Execute();
	virtual	void	Serialize( CAr & ar ); // 시리얼라이즈 ; 네트웍 상태에서 서버와 클라이언트, 클라이언트 서버가 주고받을 패킷 내용 
	virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );
	virtual void	InitProp();		// 객체를 프로퍼티 내용으로 초기화 	

	void			Init();										// 객체 초기화 
	void			InitLevel( int nJob, LONG nLevel, BOOL bGamma = TRUE );			// 객체를 nLevel로 바꿔줌

	BOOL			EndMotion();
	int				GetMaxPoint(int nDest);

	BOOL			InitSkillExp();
	void			InitCharacter( LPCHARACTER lpCharacter );	// 이름 초기화 LoadDialog호출 
	LPCHARACTER		GetCharacter();								// 
	void			InitNPCProperty();							// NPC관련 스크립트 초기화 	
	BOOL			LoadDialog();								// dialog 스크립트 로드 
	void			ProcessRecovery();							// HP, MP, FP회복을 처리한다.

	static	CMover* GetActiveMover() { return (CMover*)m_pObjActive; }	// 주인공 객체 얻기 	
	BOOL			IsActiveMover() { return m_pObjActive == this; }	// 내가 주인공 객체인가?

	static	int		GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair );
	
	int				IsSteal( CMover *pTarget );		// pTarget을 스틸하려 하는가.
	int				IsSteal( OBJID idTaget );		// id로 검사하는 버전.
	u_long			GetPartyId() { return m_idparty; }

	BOOL			IsMode( DWORD dwMode ); 
	void			SetMode( DWORD dwMode )		{ m_dwMode |= dwMode; }	// 유저상태 셑팅
	void			SetNotMode( DWORD dwMode )	{ m_dwMode &= (~dwMode); } // 유저상태 리셑

#ifdef __ACROBAT_0504	
	BOOL			SetDarkCover( BOOL bApply, DWORD tmMaxTime = 0 );
	CSfx*			CreateSfxArrow( DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget );
#endif //__ACROBAT_0504

	BOOL			SetStun( BOOL bApply, DWORD tmMaxTime = 0 );
	BOOL			SetPoison( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 독에 걸리게 한다.
	BOOL			SetDark( BOOL bApply, DWORD tmMaxTime = 0, int nAdjHitRate = 0 );	// this를 암흑상태에 빠지게 한다 .
	BOOL			SetBleeding( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 출혈시킨다.
	void			RemoveDebuff( DWORD dwState );		// 디버프 해제

#ifdef __S0504_FLY
	BOOL			IsStateMode( DWORD dwMode ); 
	void			SetStateMode( DWORD dwMode, BYTE nFlag );		// 유저상태 셑팅
	void			SetStateNotMode( DWORD dwMode, BYTE nFlag );	// 유저상태 리셑
	BOOL			IsUseItemReadyTime( ItemProp* pItemProp, OBJID dwObjItemId );
#endif // __S0504_FLY
#ifdef __CLIENT
	void			SetDmgCnt( int nDmgCnt ) { m_nDmgCnt = nDmgCnt; }
#endif // __CLIENT
	
	BOOL			IsNPC()				{ return !m_bPlayer; }
	BOOL			IsPlayer()			{ return m_bPlayer; }
	BOOL			IsFlyingNPC()		{ return (m_dwTypeFlag & OBJTYPE_FLYING) ? TRUE : FALSE; }		// 비행형 몹인가.
	BOOL			IsFly();

	MoverProp*		GetProp() { return prj.GetMoverProp( m_dwIndex ); }	// 객체의 프로퍼티 얻기 	
	ItemProp*		GetActiveHandItemProp( int nParts = PARTS_RWEAPON );							// 손에 들려진 아이템의 프로퍼티를 얻기 

	BOOL			IsEmptyDestObj()	{ return m_idDest == NULL_ID; }
	OBJID			GetDestId()			{ return m_idDest; }
	CCtrl*			GetDestObj()		{ return prj.GetCtrl( m_idDest ); }
	void			SetDestObj( CCtrl* pObj, float fRange = 0.0f )	{	chASSERT( pObj );	SetDestObj( pObj->GetId(), fRange );	}
#ifdef __XSYNC0711
 #ifdef __WORLDSERVER
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = FALSE );
 #else
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = TRUE );
 #endif
#else // xSync0711
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = TRUE );
#endif // not xSync0711
	void			ClearDestObj()		{ 
//		if( IsPlayer() )
//			TRACE( "ClearDestObj %d\n", m_oaCmd );
		m_idDest = NULL_ID; m_fArrivalRange = 0.0f; 
	}
	D3DXVECTOR3		GetDestPos()		{ return m_vDestPos; }
	void			SetDestPos( CONST D3DXVECTOR3& vDestPos, bool bForward = true, BOOL fTransfer = TRUE );
	void			SetDestPos( CShip *pIAObj, const D3DXVECTOR3 &vDestPos );
	void			ClearDestPos()		{ m_vDestPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); }		
	BOOL			IsEmptyDestPos()	{ return m_vDestPos.x == 0.0f && m_vDestPos.z == 0.0f; }
	void			ClearDest()			{ ClearDestObj(); ClearDestPos(); }
	BOOL			IsEmptyDest()		{ return m_vDestPos.x == 0.0f && m_vDestPos.z == 0.0f && m_idDest == NULL_ID; }
	void			SetStop()			{ SendActMsg( OBJMSG_STAND ); ClearDestObj(); } // 멈춤 
	void			SetDestAngle( float fDestAngle, bool bLeft )	{	m_fDestAngle	= fDestAngle;	m_bLeft	= bLeft;	}
	void			ClearDestAngle()	{ m_fDestAngle	= -1.0f; }
	void			SetMarkingPos();
	BOOL			IsRegionAttr( DWORD dwAttribite ) { return ( m_dwRegionAttr & dwAttribite ) == dwAttribite ? TRUE : FALSE; }
	REGIONELEM*		UpdateRegionAttr();

	DWORD			GetRideItemIdx()	{ return m_dwRideItemIdx; }
	void			SetRide( CModel *pModel, int nRideItemIdx = 0 ) { m_dwRideItemIdx = nRideItemIdx; m_pRide = (CModelObject*)pModel; 	};

	void			ClearDuel( void );
	void			ClearDuelParty( void );
	
#ifdef __S0504_FLY
	#ifdef __WORLDSERVER
		BOOL		IsItemRedyTime( ItemProp* pItemProp, OBJID dwObjid, BOOL bItemFind );
	#endif // __WORLDSERVER
#endif // __S0504_FLY	
	
	// 액션 매시지를 보냄 ; 객체의 움직임을 매시지로 제어함 
	int				SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0 ); 	
	// AI 매시지 보냄 : 객체의 행동 패턴, 현재 상태를 매시지로 제어함 
	void			SendAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );

#ifdef __POSTAIMSG1008
	void			PostAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
#endif	// __POSTAIMSG1008
	
	BOOL			IsAction();	// 캐릭터가 액션 중인가?
	BOOL			SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0 );	// MOP_SWDFORCE, MOP_FIXED, MOP_NO_TRANS, MOP_HITSLOW
	BOOL			InitMotion(DWORD dwMotion);
	void			SetDamagedMotion( CMover* pAttacker, DWORD dwAtkFlags );
	
	CItemElem*		GetWeaponItem( int nParts = PARTS_RWEAPON );					// 장착한 무기 얻기 
	CItemElem*		GetLWeaponItem();					// 왼손에 장착한 무기 얻기.
	CItemElem*		GetEquipItem( int nParts );			// 장착한 아이템 얻기 
#ifdef __FINITEITEM0705
	ItemProp*		GetEquipItemProp( CItemContainer<CItemElem>* pInventory, PEQUIP_INFO pEquipInfo, int nParts );
#else	// __FINITEITEM0705
	ItemProp*		GetEquipItemProp( CItemContainer<CItemElem>* pInventory, LPDWORD padwEquipment, int nParts );
#endif	// __FINITEITEM0705
	BOOL			IsDualWeapon( void );
	void			RedoEquip( BOOL fFakeParts );	
	void			UpdateParts( BOOL bFakeParts  = FALSE ); // normal or fake
	int				InvalidEquipOff( BOOL fFakeparts );		// 거시기한 장비를 벗김.

#ifdef __V050322_CACHEQUEST
	#ifdef __FINITEITEM0705
	static void		UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts = TRUE, CMover* pMover = NULL );
	#else	// __FINITEITEM0705
	static void		UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, LPDWORD pdwEquipment, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts = TRUE, CMover* pMover = NULL );
	#endif	// __FINITEITEM0705
#else // __V050322_CACHEQUEST
//{{AFX
	static void		UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, LPDWORD pdwEquipment, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts = TRUE );
//}}AFX
#endif // __V050322_CACHEQUEST
/*
#ifdef __NEWITEM0624
	BOOL	RemoveEquipedItem( int nParts );
#ifdef __WORLDSERVER
	BOOL	DropEquipedItem( int nParts );
#endif	// __WORLDSERVER
#endif	// __NEWITEM0624
*/
#ifdef __FINITEITEM0705
	void	CheckFiniteItem( void );
#endif	// __FINITEITEM0705

	BOOL			DoEquip( CItemElem* pItemElem, BOOL bEquip, int nPart = -1 ); // for normal
	BOOL			IsEquipAble( ItemProp *pItemProp, OBJID dwObjid );		// 장착가능한가?
	BOOL			IsUnEquipAble( ItemProp *pItemProp );					// 벗는게 가능한가?
	void			SetEquipDstParam();
	void			SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray );
	int				SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax );
#ifdef __J0608
	void			SetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem = FALSE );
#else	// __J0608
	void			SetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem );
#endif	// __J0608
	void			ResetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem );
	void			UpdateParam();		// 아이템에 의한 공격 파라미터 재계산 
	void			DoSMItemEquip( CItemElem* pItemElem, DWORD dwParts );
	void			DoSMItemUnEquip( CItemElem* pItemElem, DWORD dwParts );
	
#ifdef __XOPT0617
	static BOOL		DoEquip( int nSex, int nSkinSet, CItemElem* pItemElem, int nPart, DWORD dwItemIndex, int nOption, CItemContainer< CItemElem  >* pInventory, LPDWORD padwEquipment, CModelObject* pModel, BOOL bEquip, CMover *pMover );
#else
#ifdef __FINITEITEM0705
	static BOOL		DoEquip( int nSex, int nSkinSet, CItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< CItemElem  >* pInventory, PEQUIP_INFO pEquipeInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover );
#else	// __FINITEITEM0705
	static BOOL		DoEquip( int nSex, int nSkinSet, CItemElem* pItemElem, int nPart, DWORD dwItemIndex, int nOption, CItemContainer< CItemElem  >* pInventory, LPDWORD padwEquipment, int *paEquipOption, CModelObject* pModel, BOOL bEquip, CMover *pMover );
#endif	// __FINITEITEM0705
#endif	
	
	LPQUEST			FindQuest( int nQuestId );
	LPQUEST			GetQuest( int nQuestId );
	BOOL			RemoveQuest( int nQuestId );
#ifdef __V050322_CACHEQUEST
	BOOL			IsCompleteQuest( int nQuestId );
	BOOL			MakeCompleteQuest( int nQuestId, LPQUEST lpQuest );
	void			__MakeCompleteQuest( int nQuestId, LPQUEST lpQuest );

	BOOL			SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest );
	BOOL			SetQuest( LPQUEST lpQuest );
	void			RemoveAllQuest();
	void			RemoveCompleteQuest();

	BOOL            IsDisguise();
	BOOL			NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice = NULL );
	BOOL			Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex );
virtual	BOOL		SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE );
private:
	BOOL			__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest );


public:
#else // __V050322_CACHEQUEST
	void			SetQuest( int nQuestId, int nState, int nTime = 0 );
#endif // __V050322_CACHEQUEST
	DWORD			IsAuthorization( DWORD dwAuthorization ) { return dwAuthorization == m_dwAuthorization; }
	DWORD			IsAuthHigher( DWORD dwAuthorization ) { return dwAuthorization <= m_dwAuthorization; }

	int				GetParam( int nDestParameter, int nParam );
	void			SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL fSend = TRUE );
	void			SetDestParam( int nItemIdx2, BOOL fSend = TRUE );
	void			SetDestParam( int nIdx, ItemProp *pProp, BOOL bSend = TRUE );
	void			ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend = TRUE );
	void			ResetDestParam( int nIdx, ItemProp *pProp, BOOL bSend = TRUE );
	int				GetAdjParam( int nDestParameter );
	int				GetChgParam( int nDestParameter );
	int				GetPointParam( int nDstParameter );
	void			SetPointParam( int nDstParameter, int nValue );
	void			OnApplySM();

	LPSKILL			GetSkill( int nType, int nIdx );
	LPSKILL			GetSkill( DWORD dwSkill );
	int				GetSkillIdx( DWORD dwSkill );
	void			OnEndSkillState( DWORD dwSkill, DWORD dwLevel );
	BOOL			CheckSkill( DWORD dwSkill );


	void			SetHair( int nHair );
	void			SetHairColor( DWORD dwHairColor );
	void			SetHairColor( FLOAT r, FLOAT g, FLOAT b );
	void			SetHead( int nHead );
	void			SetSkinSet( int nSkinSet );

#ifdef __J0519
	void			SetName( const char* lpszName )		{	lstrcpy( m_szName, lpszName );	}
#endif	// __J0519

	LPCTSTR			GetName( BOOL bNickname = FALSE );	// 객체의 이름 얻기
	BYTE			GetSex() { return m_bySex; }
	void			SetSex( BYTE bySex ) { m_bySex = bySex; }
	BOOL			IsPeaceful() { return m_dwBelligerence == BELLI_PEACEFUL; }
	BOOL			IsBaseJob();
	BOOL			IsExpert();
	BOOL			IsPro();
	BOOL			IsInteriorityJob( int nJob );
	BOOL			SetExpert( int nExpert );
	BOOL			AddChangeJob( int nJob );
	int				GetJob();
	int				GetJobBase();
	DWORD			GetJobLevel() { return m_aJobLv[ m_nJob ]; }
	DWORD			GetJobLevel( int nJob ) { return m_aJobLv[ nJob ]; }
	BOOL			IsJobType( DWORD dwJobType ); 

	int				GetLevel() { return m_nLevel; }
	int				GetFlightLevel() { return m_nFlightLv; }
	int				GetFxp() { return m_nFxp; }
	int				GetTxp() { return m_nFxp; }
	EXPINTEGER		GetExp1()	{	return m_nExp1;	}
	EXPINTEGER		GetExp2()	{	return m_nExp2;	}
	EXPINTEGER		GetMaxExp1()	{	return prj.m_aExpCharacter[m_nLevel+1].nExp1;	}
	EXPINTEGER		GetMaxExp2()	{	return prj.m_aExpCharacter[m_nLevel].nExp2;	}
	int				GetRemainGP(); // growth
	int				GetRemainLP(); // learning
	void			IncHitPoint( int nVal) ;
	void			IncManaPoint(int nVal) ;
	void			IncFatiguePoint(int nVal) ;
	void			SetHitPoint( int nVal) ;
	void			SetManaPoint( int nVal ) ;
	void			SetFatiguePoint( int nVal ) ;
	void			DecPXP( DWORD dwExp, int nSkillIdx );
	void			IncSkillExp( int nType, int nId );
	float			GetExpFactor( void );
	BOOL			AddExperience( EXPINTEGER nExp, BOOL bFirstCall = TRUE  );
	BOOL			DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown );	// 경험치를 깎는다.
	BOOL			DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown  );	// 경험치를 퍼센트로 깎는다.
	BOOL			AddFxp( int nFxp );
	BOOL			SetExperience( EXPINTEGER nExp1, EXPINTEGER nExp2, int nLevel );
	BOOL			SetFxp( int nFxp, int nFlightLv );
	BOOL			IncJobLevel( int nJob );
#ifdef __NPP_050308
	int				GetKarma() { return m_nSlaughter; }
	BOOL			IsPVPTarget( CMover* pMover )	{	return( m_idDuelOther == pMover->GetId() || ( m_idDuelParty == pMover->m_idparty && m_nDuelState == 1 && pMover->m_nDuelState == 1 ) );		}
	BOOL			IsWarTarget( CMover* pMover );
	BOOL			IsSchoolTarget( CMover* pMover );
#ifdef __GUILDCOMBAT
	BOOL			IsGuildCombatTarget( CMover* pMover );
#endif // __GUILDCOMBAT
	HITTYPE			GetHitType( CMover* pMover, BOOL bTarget );
	HITTYPE			GetHitType2( CMover* pMover, BOOL bTarget );
	void			ReSetDuelTime( CMover* pAttacker , CMover* pDefender);
#endif // __NPP_050308
	BOOL			IncStrLevel();
	BOOL			IncStaLevel();
	BOOL			IncDexLevel();
	BOOL			IncIntLevel();

	int				GetHR();
	void			SetStr( int nStr ) { m_nStr = nStr; }
	void			SetSta( int nSta ) { m_nSta = nSta; }
	void			SetDex( int nDex ) { m_nDex = nDex; }
	void			SetInt( int nInt ) { m_nInt = nInt; }	
	int				GetStr();
	int				GetDex();
	int				GetInt();
	int				GetSta();
	FLOAT			GetSpeed(FLOAT fSrcSpeed);    
	int				GetGold();
	void			SetGold( int nGold );
	BOOL			AddGold( int nGold, BOOL bSend = TRUE );
	int				GetHitPointPercent( int nPercent = 100 );
	int				GetManaPointPercent( int nPercent = 100 );
	int				GetFatiguePointPercent( int nPercent = 100 );
	int				GetHitPoint();
	int				GetManaPoint();
	int				GetFatiguePoint();
	int				GetMaxHitPoint();
	int				GetMaxManaPoint();
	int				GetMaxFatiguePoint();
	int				GetMaxOriginHitPoint( BOOL bOrinal = TRUE );
	int				GetMaxOriginManaPoint( BOOL bOrinal = TRUE );
	int				GetMaxOriginFatiguePoint( BOOL bOrinal = TRUE );
	int				GetHPRecovery();
	int				GetMPRecovery();
	int				GetFPRecovery();
	int				GetNaturalArmor();
 
	float			GetAttackSpeed();
#ifdef __NEWWPN0901
	float			GetCastingAniSpeed( void );
	int				GetCastingTime( int nCastingTime );
#endif	// __NEWWPN0901
	float			GetDamageMultiplier( ATTACK_INFO* pInfo );
	int				PostCalcMagicSkill( int nATK, ATTACK_INFO* pInfo );
	int				PostCalcGeneric( int nATK, ATTACK_INFO* pInfo );
	float			GetMagicSkillFactor( CMover* pDefender, SAI79::ePropType skillType );
	float			GetATKMultiplier( CMover* pDefender, DWORD dwAtkFlags );
	float			GetDEFMultiplier( ATTACK_INFO* pInfo );
	float			GetBlockFactor( CMover* pAttacker, ATTACK_INFO* pInfo );

	int				GetWeaponATK( DWORD dwWeaponType );
	int				GetPlusWeaponATK( DWORD dwWeaponType );
	int				GetWeaponPlusDamage( int nDamage, BOOL bRandom = TRUE );
	void			GetDamagePropertyFactor( CMover* pDefender, int* pnATKFactor, int* pnDEFFactor, int nParts );

	int				GetPropATKPlus( int nParts );
	int				GetPropDEFPlus();
	void			GetDamageRange( int& nMin, int& nMax );
	BOOL			IsBlocking( CMover* pAttacker );
	BOOL			CanFlyByAttack();
	BOOL			IsCriticalAttack( CMover* pDefense, DWORD dwAtkFlags );
#ifdef __NEWWPN0818
	int				GetCriticalProb( void );
#endif	// __NEWWPN0818
	int					GetReqMp( int nReqMp );
	int					GetReqFp( int nReqFp );

	BOOL			GetAttackResult( CMover* pHitObj, DWORD dwOption );
	int				GetAdjHitRate( void );
	int				GetHitPower( ATTACK_INFO* pInfo );
	int				GetRangeHitPower( ATTACK_INFO* pInfo );
	int				GetParrying();
	int				GetDefenseByItem( BOOL bRandom = TRUE );
	int				GetShowDefense( BOOL bRandom );
	int				GetResistMagic();
	int				GetResistSpell( int nDestParam );
	int				GetMeleeSkillPower( ATTACK_INFO* pInfo );	// 근접공격 스킬데미지
	int				GetMagicHitPower( int nMagicPower );			// 완드공격 데미지
	int				GetItemAbility( int nItem );
	int				GetItemAbilityMin( int nItem );
	int				GetItemAbilityMax( int nItem );
	void			GetHitMinMax( int* pnMin, int* pnMax, int nParts = PARTS_RWEAPON );
	BOOL			IsAfterDeath();
	BOOL			IsDie() { return m_pActMover->IsDie() || m_nHitPoint == 0; }
	BOOL			IsLive() { return m_pActMover->IsDie() == FALSE || m_nHitPoint > 0; }		// && 를  ||로 바꿨음.  !=를 >로 바꿈
	int				GetCount( void ) { return m_nCount; }
	BOOL			IsGuardReactionChao() 
	{ 
		if( m_nSlaughter <= -2000 )
			return TRUE;

		return FALSE;
	}
	BOOL			IsGuardReactionNormal() 
	{ 
		if( m_nSlaughter > 0 )
			return TRUE;
		
		return FALSE;
	}
	

	BOOL			IsChaotic() { return GetSlaughterGrade() != SLAUGHTER_NORMAL; }
	
	SLAUGHTER_GRADE	GetSlaughterGrade();						
	void			SetRecoverMode();
	
	typedef float	(CMover::* FPTR_GetResist)();
	FPTR_GetResist	m_lpfGetResist[SAI79::END_PROP];
#ifdef __BEAST
	float			GetResistNULL()							{ return 0.0f; } 
	float			GetResist(SAI79::ePropType p_PropType)	{ return 0.0f; }
	float			GetResistElectricity() { return 0.0f; }
	float			GetResistFire()        { return 0.0f; }
	float			GetResistWater()       { return 0.0f; }
	float			GetResistEarth()       { return 0.0f; }
	float			GetResistWind()        { return 0.0f; }
#else
	float			GetResistNULL()							{ return 0.0f; } 
	float			GetResist(SAI79::ePropType p_PropType)	{ return (this->*m_lpfGetResist[p_PropType])(); }
	float			GetResistElectricity() { return GetResistSpell( DST_RESIST_ELECTRICITY )/100.0f; }
	float			GetResistFire()        { return GetResistSpell( DST_RESIST_FIRE        )/100.0f; }
	float			GetResistWater()       { return GetResistSpell( DST_RESIST_WATER       )/100.0f; }
	float			GetResistEarth()       { return GetResistSpell( DST_RESIST_EARTH       )/100.0f; }
	float			GetResistWind()        { return GetResistSpell( DST_RESIST_WIND        )/100.0f; }
#endif

#ifdef __WORLDSERVER
	void			Abrade( CMover* pAttacker, int nParts = PARTS_RWEAPON );
	DWORD			GetRandomPartsAbraded();
#ifdef __J0519
	void			QuerySetPlayerName( const char* lpszPlayer, DWORD dwData = MAKELONG( -1, 0 ) );
#endif	// __J0519
#endif	// __WORLDSERVER

#ifdef __J0608
	int		GetSetItemParts(DWORD dwParts);
	int		GetSetItemClient();
	int		GetSetItem( CItemElem* pItemElem = NULL );
	BOOL	IsSetItemPart( DWORD dwParts );
	void	SetSetItemAvail( int nAbilityOption );
	void	ResetSetItemAvail( int nAbilityOption );
#endif	// __J0608
#ifdef __PIERCING_SM_TIME
	void	SetDestParamPiercingAvail( CItemElem* pItemElem);
	void	ResetDestParamPiercingAvail( CItemElem* pItemElem );
	BOOL	Pierce( CItemElem *pSuit, DWORD dwItemId );
#endif	// __PIERCING_SM_TIME

#ifdef __SETITEM0624
	void	SetDestParamSetItem( CItemElem* pItemElem );
	void	ResetDestParamSetItem( CItemElem* pItemElem );
	int		GetEquipedSetItemNumber( CSetItem* pSetItem );
	void	GetEquipedSetItem( int nSetItemId, BOOL* pbEquiped, int* pnEquip );
#endif	// __SETITEM0624

#ifdef __RANDOMOPTITEM0628
	void	SetDestParamRandomOpt( CItemElem* pItemElem );
	void	ResetDestParamRandomOpt( CItemElem* pItemElem );
#endif	// __RANDOMOPTITEM0628

	void			GetItemATKPower( int *pnMin, int *pnMax, ItemProp* pItemProp, CItemElem *pWeapon ); 
	float			GetItemMultiplier( CItemElem* pItemElem );

	static	float	GetItemEnduranceInfluence( int nEndurance );		// 100
	static	int		GetItemEnduranceWeight( int nEndurance );	// 100


	BOOL			SubLootDropNotMob( CItem *pItem );
	BOOL			SubLootDropMobSingle( CItem *pItem );
	BOOL			SubLootDropMobParty( CItem *pItem, CParty *pParty );
	BOOL			SubLootDropMob( CItem *pItem );
	BOOL			DoLoot( CItem *pItem );		// 바닥에 떨어진 pItem을 줍는다.
	void			PickupGoldCore( int nGold );
	void			PickupGold( int nGold, BOOL bDropMob );
	BOOL			IsDropable( CItemElem* pItemElem, BOOL bPK );
 	BOOL			IsDropableState( BOOL bPK );
	CItem*			DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK = FALSE );
	void			UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue );
	CItem*			_DropItemNPC( DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos );
	CItem*			DropItem( DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos, BOOL bPlayer = FALSE );
	int				DoDropItemRandom( BOOL bExcludeEquip, CMover* pAttacker );
	int				GetItemNum( DWORD dwItemId );
	int				RemoveAllItem( DWORD dwItemId );
	BOOL			AddItem( CItemBase* pItemBase );
	CItemBase*		GetItemId( DWORD dwId );
	void			RemoveItemId( DWORD dwId  );
	void			OnTradeRemoveUser();
	BOOL			AddItemBank( int nSlot, CItemElem* pItemElem );
	void			UpdateItemBank( int nSlot, BYTE nId, CHAR cParam, DWORD dwValue );
	CItemBase*		GetItemBankId( int nSlot, DWORD dwId );
	void			RemoveItemBankId( int nSlot, DWORD dwId );
	void			GenerateVendorItem( ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor );
#ifdef __NPP_050308
private:
	BOOL			DropItem( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
public:
	BOOL			DropItemByDied( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
#else // __NPP_050308
	BOOL			DropItem( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
#endif // __NPP_050308

#ifdef __XSKILL0425	
	BOOL			DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType = SUT_NORMAL, const int nCastingTime = 0 );		// dwSkill/nLevel만 가지고도 사용할 수 있는 버전.
#endif
#if defined(__WORLDSERVER)
	int				GetQueueCastingTime();
	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType );
	float			SubDieDecExp( BOOL bTransfer = TRUE, DWORD dwDestParam = 0 );	// 죽었을때 겸치 깎는 부분.
	void			SubAroundExp( CMover *pAttacker, float fRange );		// this를 중심으로 fRange범위안에 있는 유저에게 경험치를 배분한다.
	void			AddPartyMemberExperience( CUser * pUser, EXPINTEGER nExp, int nFxp );

//	virtual	void	_Copy( CMover* pMover );

	void			AddItemEvent( BOOL f, int nLevelbuf );
	void			AddItemLevelUpEvent( BOOL f, int nLevelbuf );
	void			AddItemCommercialEvent( BOOL f, int nLevelbuf );

	BOOL			CreateItem( CItemBase* pItemBase, BYTE* pnId = NULL, short* pnNum = NULL, BYTE nCount = 0 );
	void			RemoveItem( BYTE nId, short nNum );
	int				RemoveItemA( DWORD dwItemId, short nNum );
	void			RemoveVendorItem( CHAR chTab, BYTE nId, short nNum );
	void			RemoveItemIK3( DWORD dwItemKind3 );

#ifdef __CHARGEDITEM0620
	BOOL			DoUseItemSexChange( int nFace );
#else	// __CHARGEDITEM0620
	BOOL			DoUseItemSexChange( );
#endif	// __CHARGEDITEM0620

 #ifdef __N_TRIGGER_AREASET_050203
	BOOL	ReplaceInspection( REGIONELEM* pPortkey );
 #endif // __N_TRIGGER_AREASET_050203
 #ifdef __MSVR
	BOOL	Replace( u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3 & vPos, DWORD dpid = 0xFFFFFFFF );
 #else	// __MSVR
	BOOL	Replace( DWORD dwWorldID, D3DXVECTOR3 & vPos, DWORD dpid = 0xFFFFFFFF );
 #endif	// __MSVR

	int		ChangeSlaughter( CHANGE_SLAUGHTER_TYPE type, CMover* pDefender, int nSetCarmaPoint = 0 );
	int		IncSlaughterPoint2( CMover *pDead );
	int		IncSlaughterPoint( CMover *pDead );
	BOOL	IsLoot( CItem *pItem );

#else	// __WORLDSERVER
	
	BOOL			IsLoot( CItem *pItem ) { return TRUE; }
	LPCTSTR			GetFameName();						// 명성 이름 얻기
	LPCTSTR			GetSlaughterName();					// 슬러터이름 얻기

	LPCTSTR			GetJobString();						// 직업 이름 얻기 

	void			DialogOut( LPCTSTR lpszText );		// 말풍선에 의한 대사 출력

	MOVING_ACTMSG*	PeekMovingActMsg();
	void			RemoveMovingActMsg();
	void			AddMovingActMsg( const D3DXVECTOR3 & vPos, float fAngle, BYTE fMoving, BYTE fForward,
												DWORD dwMsg, int nParam1, int nParam2 );

#ifdef __FINITEITEM0705
	BOOL			DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel = NULL ); // for Fake
#else	// __FINITEITEM0705
	BOOL			DoFakeEquip( DWORD dwItemIndex, int nOption, BOOL bEquip, int nPart, CModelObject* pModel = NULL ); // for Fake
#endif	// __FINITEITEM0705

	void			RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, int nValue );
	void			RenderTurboGauge( LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue );
	virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	void			SetRenderPartsEffect( int nParts );
	void			RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	void			RenderHP(LPDIRECT3DDEVICE9 pd3dDevice);
 #ifdef __NPP_050308
	void			RenderFlag( int nType );
 #else // __NPP_050308
	void			RenderFlag();
 #endif // __NPP_050308
 #ifdef __S0504_FLY
	void			RenderCasting( LPDIRECT3DDEVICE9 pd3dDevice );
 #endif // __S0504_FLY
 #ifdef __NPP_050308
	void			RenderPVPCount( LPDIRECT3DDEVICE9 pd3dDevice );
 #endif // __NPP_050308
 #ifdef __V050322_CACHEQUEST
	void			RenderQuestEmoticon( LPDIRECT3DDEVICE9 pd3dDevice );
 #endif

	void			RenderGuildNameLogo( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor );

	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime, DWORD dwSkill = 0, DWORD dwLevel = 0 );
#endif	// not __WORLDSERVER

	void			SetPKTargetLimit( int nSec );
#ifndef __BEAST
	void			ClearCmd() { m_oaCmd = OBJACT_NONE; }
	OBJACT			GetCmd() { return m_oaCmd; }
	OBJACT			GetAct() { return m_oaAct; }
	int				GetCmdParam( int nIdx ) { return m_nCParam[nIdx]; }	
	void			ClearActParam(); 

	int				GetEnemyCount();
	OBJID			GetMaxEnemyHitID();
	DWORD			AddEnemy( OBJID objid, int nHit );
	int				GetEnemyHit( OBJID objid, DWORD* pdwTick = NULL );
	void			RemoveEnemy( OBJID objid  );
	void			RemoveAllEnemies();

	BOOL			IsAttackAble( CObj *pObj );
	FLOAT			GetAttackRange( DWORD dwAttackRange );

#ifdef __CLIENT
	BOOL			IsPKAttackAble( CMover* pMover );
	BOOL			IsPVPAttackAble( CMover* pMover );
#endif
	BOOL			IsAttackAbleNPC( CMover* pNPC );
	
#ifdef __NPP_050308
  #ifdef __WORLDSERVER
	void			UpgradeKarma();
	BOOL			IsPVPInspection( CMover* pMover, int nFlag = 0 );
  #endif // __WORLDSERVER
	BOOL			IsPKInspection( CMover* pMover );
	int				IsPKPVPInspectionBase( DWORD dwRegionAttr, BOOL bPVP = TRUE );
#else	// __NPP_050308
	BOOL			IsPKInspection( CMover* pMover, int nFlag = 0 );
	BOOL			IsPKPVPInspectionBase( CMover* pMover, int nFlag = 0 );	
#endif	// __NPP_050308
	BOOL			SubPKPVPInspectionBase( CMover* pMover, CMover* pMover1, DWORD dwPKAttr, int nFlag );
	void			PrintString( CMover* pMover, DWORD dwId );

	int				CMD_SetUseSkill( OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType = SUT_NORMAL );
	void			CMD_SetMeleeAttack( OBJID idTarget, FLOAT fRange = 0.0f );
	void			CMD_SetMagicAttack( OBJID idTarget, int nMagicPower );
	void			CMD_SetRangeAttack( OBJID idTarget, int nPower );
	
	void			CMD_SetMoveToPos( const D3DXVECTOR3 &vPos );
	void			CMD_SetMoveToObj( OBJID idObj );
	void			CMD_SetUseItem( CCtrl *pCtrl );
	void			CMD_SetCollect( CObj *pObj );

	void			UnequipRide();		
	void			EquipItem( CItemElem *pItemElem, BOOL bEquip, int nPart );
	BOOL			DoUseEquipmentItem( CItemElem* pItemElem, DWORD dwId, int nPart );
	BOOL			DoUseItemVirtual( DWORD dwItemId, BOOL bEffectSkip );
	BOOL			DoUseItem( DWORD dwItemId, DWORD dwFocusId, int nPart = -1 );
	BOOL			DoUseItemBlinkWing( ItemProp *pItemProp, CItemElem* pItemElem, BOOL bUes = TRUE );
	BOOL			DoUseItemFood_SM( ItemProp *pItemProp, CItemElem* pItemElem );
	BOOL			DoUseItemFood( ItemProp *pItemProp, CItemElem* pItemElem );
#ifdef __CHARGEDITEM0620
	int				DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem, int nPart = -1 );
#else	// __CHARGEDITEM0620
	int				DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem );
#endif	// __CHARGEDITEM0620
	void			DoPickupItemAround();
	int				DoAttackMagic( CObj *pTargetObj, int nMagicPower,int idSfxHit = 0 );
	int				DoAttackRange( CObj *pTargetObj, DWORD dwItemID, int idSfxHit );
	int				DoAttackSP( CObj *pTargetObj, DWORD dwItemID );
	int				DoAttackMelee( CMover *pTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0 );
	int				DoAttackMelee( OBJID idTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0 );
	int				DoDie( CCtrl *pAttackCtrl, DWORD dwMsg = 0 );	// 뒈져라.	

	void			ChangeFame( CMover *pLose );
	int				DuelResult( CMover *pLose );
	PVP_MODE		GetPVPCase( CMover *pAttacker );
	void			SubPVP( CMover *pAttacker );	// 사람과 사람의 싸움에서 죽었을때.
	int				SubDuel( CMover *pAttacker );	// 듀얼중에 죽었을때 
	int				SubPK( CMover *pAttacker );		// PK에의해 죽었을때.
	int				SubWar( CMover *pAttacker );	// 전쟁때 죽었음
	int				SubExperience( CMover *pDead );
	void			AddKillRecovery();
#ifdef __WORLDSERVER
	void			AddExperienceKillMember( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue );
	void			AddExperienceSolo( EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue );
	void			AddExperienceParty( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	void			AddExperiencePartyContribution( CMover *pDead, CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	void			AddExperiencePartyLevel( CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	BOOL			GetPartyMemberFind( CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	void			AddExperienceLevelReduce( EXPFLOAT* fExpValue, MoverProp* pMoverProp, float* fFxpValue, int nMaxLevel );
#endif // __WORLDSERVER
	BOOL			IsValidArea( CMover* pMover, float fLength );
	void			SetJJim( CMover *pJJimer );		// this를 pJJimer가 찜했다는걸 표시.
	
	// CActionMover로부터의 메시지 핸들러.
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamage( dwAtkFlag, idAttacker, nParam, bTarget );  }
	virtual int		SendDamageForce( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamageForce( dwAtkFlag, idAttacker, nParam, bTarget ); }	// 강공격
	BOOL			IsFinishCastingTime( int nCount );	// 마법캐스팅 시간이 지났는지 검사.
	void			SubReferTime( DWORD *pTime1, DWORD *pTime2, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	BOOL			ApplyParam( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend = TRUE, int nDamage = 0 );	// this에게 파라메터를 적용. pSrc는 시전자.
	BOOL			ApplyMagicSkill( CMover *pMover, DWORD dwSkill, DWORD dwLevel, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );	// pMover에게 스킬효과를 적용함.
	BOOL			GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	BOOL			CreateSkillEffect( OBJID idTarget, int nSkill, DWORD dwLevel );		// 실제 스킬 발동.
	void			RemoveInvisible();		// 투명화 상태걸려있으면 해제함.
	void			SendDamageAround( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio );	// this를 중심으로 주변에 데미지를 입힘.
	void			SendDamageLine( int nDmgType, int nApplyType, int nAttackID, float fWidth, float fHeight, float fDepth );
	int				EstimateMagicHit( CMover* pAttacker );

	void			ChangeExpRatio( CMover* pAttacker, CMover* pDefender );
	void			OnAttacked( CMover* pAttacker, int nDamage, BOOL bTarget );
	BOOL			OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos = NULL, DWORD dwAtkFlag = AF_GENERIC );
	void			SetCoolTime( ItemProp *pSkillProp, LPCTSTR szCall );
	BOOL			OnMeleeSkill( int nType, int nCount = 1 );		// OBJMSG_ATK_MELEESKILL실행후 타점이 되었을때 발생하는 이벤트.
	BOOL			OnMagicSkill( int nType, int nCount = 1 );		// OBJMSG_ATK_MAGICSKILL실행후 타점이 되었을때 발생하는 이벤트.
	BOOL			OnAttackRange();		// 장거리무기가 발사되는 시점에 발생하는 이벤트
	BOOL			OnAttackMelee( DWORD dwState, CMover *pHitObj );		// 근거리공격시 타점이 되었을때 발생하는 이벤트.
	BOOL			OnAttackMeleeContinue( DWORD dwState );		// 첫번째 타점이후부터 애니끝날때까지 계속.
	void			OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj );
	BOOL			OnAttackMagic();		// 완드공격이 발사되는 시점에 발생하는 이벤트.
	BOOL			OnAttackSP();			// 특수공격.
	void			OnActDrop();			// 캐릭터가 높은곳에서 떨어지기 시작하는 시점에 발생.
	void			OnActCollision();		//
	void			OnActEndMeleeSkill();	// 근접스킬 동작이 끝나는 시점에 발생.
	void			OnActEndMagicSkill();	// 마법스킬 동작이 끝나는 시점에 발생.
	void			OnEndMeleeAttack( DWORD dwState );		// 일반동작 공격 끝날때 발생(몹만).
	void			OnActEndJump4();		// 착지 플래그가 사라지는 순간.
	void			OnActLanding();		// 착지 하는 순간.
	void			OnActFMove();
	void			OnActIALanding( CObj *pIAObj, const D3DXVECTOR3 &vPos );		// Interact오브젝트에 착지했을때.
	void			OnAttackStart( CMover *pTarget, OBJMSG dwMsg );					// 공격을 막 시작했을때(일반/스킬/완드 등)
	BOOL			IsBullet( ItemProp* pItemProp );
#ifdef __ACROBAT_0504
#ifdef __WORLDSERVER
	void			ArrowDown( int nCount );
#endif // __WORLDSERVER
#endif // __ACROBAT_0504

	#ifdef __XFLYAI0524
		void		ProcessMovePattern();
		int			GetMovePattern() { return m_nMovePattern; }
		int			GetMoveEvent() { return m_nMoveEvent; }
		void		SetMovePattern( int nPattern )		// 이동패턴 설정.
		{
			m_nMovePattern = nPattern;
			m_nMoveEvent = 0;
			m_nMoveEventCnt = 0;
			ClearDest();		// 목표는 없다.
		}
	#endif // __XFLYAI0524

		#ifdef __WORLDSERVER
			BOOL	IsResourceMonster() { return m_nResource != -1; }		// 자원몬스터냐? -1이면 자원몬스터가 아니다. 0 ~ 자원몬스터라는 뜻.
		#endif // __WORLDSERVER
		BOOL		DoCollect( CMover *pTarget );
		virtual int	OnActCollecting();				// User만 사용되는 것이므로 CUser가서 찾을것.
			
#else // !__BEAST
		BOOL		IsSuperPower() { return m_dwExtraFlag & EXFLAG_SUPERPOWER ? TRUE : FALSE; }
		BOOL		IsImmortality() { return m_dwExtraFlag & EXFLAG_IMMORTALITY ? TRUE : FALSE; }
		BOOL		IsExtraFlag( DWORD dwExtraFlag ) { return m_dwExtraFlag & dwExtraFlag ? TRUE : FALSE; } 
		void		AddExtraFlag( DWORD dwExtraFlag ) { m_dwExtraFlag |= dwExtraFlag; }
		void		DelExtraFlag( DWORD dwExtraFlag ) { m_dwExtraFlag &= ~dwExtraFlag; }
		BOOL		IsRespawn() { return m_respawnInfo.m_dwIndex ? TRUE : FALSE; }				
#endif // __BEAST

	void			BehaviorActionForceSet();		// 명령이 행동 명령이면 강제 동기하고, 무빙 명령이면 무시한다.
	void			ActionForceSet( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
									 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption );
	void			ActionForceSet2( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle,
									FLOAT fAngleX, FLOAT fAccPower, FLOAT fTurnAngle,
									DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption );
#ifdef __J0518
	void			HalfForceSet( D3DXVECTOR3 & vPos, D3DXVECTOR3 & vd,
									float fAngle, float fAnlgeX, float fAccPower, float fTurnAngle );
#endif	// __J0518

	CGuild*			GetGuild();
	CGuildWar*		GetWar();

#if !defined(__BEAST)
	BOOL			IsSMMode( int nType ) { return ( m_dwSMTime[nType] > 0 ) ? TRUE : FALSE; }
#endif

#ifdef __CHARGEDITEM0620
	BOOL			DoUseItemSM( ItemProp *pItemProp, CItemElem* pItemElem, D3DXVECTOR3* pFos, int nPart = -1 );
#else	// __CHARGEDITEM0620
	BOOL			DoUseItemSM( ItemProp *pItemProp, CItemElem* pItemElem, D3DXVECTOR3* pFos );
#endif	// __CHARGEDITEM0620
	#ifdef __WORLDSERVER
		void		SubSMMode();
		BOOL		SetSMMode( int nType, DWORD dwTime );
	#endif // __WORLDSERVER

	void			ReState();
	float			GetJobPropFactor( JOB_PROP_TYPE type );
	int				CalcDefense( ATTACK_INFO* pInfo, BOOL bRandom = TRUE );
	int				CalcDefenseCore( CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom = TRUE );	
	int				CalcDefensePlayer( CMover* pAttacker, DWORD dwAtkFlags );
	int				CalcDefenseNPC( CMover* pAttacker, DWORD dwAtkFlags );
// 데미지관련 모두 월드서버만 컴파일되게 	
#ifdef	__WORLDSERVER
	void			_SubParry( CMover* pAttacker, int ATK, int& nHit, DWORD& dwAtkFlags );
	int				ApplyDPC( int ATK, ATTACK_INFO* pInfo );
	int				CalcLinkAttackDamage( int nDamage ); // 파티스킬:링크어택의 추가 데이미계산 	
	int				CalcPropDamage( CMover* pDefender, DWORD dwAtkFlags );
	int				CalcGenericDamage( CMover* pDefender, DWORD& dwAtkFlags );
	int				GetMagicSkillPower( ATTACK_INFO* pInfo );
#endif	// __WORLDSERVER


#if !defined(__BEAST)
	static			CMoverPool*		m_pPool;
	void*			operator new( size_t nSize )									{ return CMover::m_pPool->Alloc();	}
	void*			operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{ return CMover::m_pPool->Alloc();	}
	void			operator delete( void* lpMem )									{ CMover::m_pPool->Free( (CMover*)lpMem );	}
	void			operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{ CMover::m_pPool->Free( (CMover*)lpMem );	}
#endif // !defined(__BEAST)
	
	 // CheckSkill( DWORD dwSkill );와 혼란이 있을 것 같아서 삭제했음. CheckSkill( DWORD dwSkill ); 써도 될 것 같습니다
	//BOOL			CheckSkill( int i );
	void			PutLvUpSkillName_1( DWORD dwLevel );
	void			PutLvUpSkillName_2( DWORD dwSkill );

#ifdef __CLIENT
	void PlayCombatMusic();
#endif

#ifdef __NPP_050308
	BOOL	IsRegionMove( DWORD dwOlgRegionAttr, DWORD dwRegionAttr );
	DWORD	GetPKPVPRegionAttr( void );
#endif // __NPP_050308

	CItemBase*		GetVendorItem();		

private:
	void			ProcessMove();
	void			ProcessMoveArrival( CCtrl *pObj );
	void			ProcessRegenItem();
	void			ProcessScript();
	void			ProcessRegion();

#ifdef __V050322_CACHEQUEST
	void			ProcessQuest();
#ifdef __CLIENT
public:
	void OverCoatItemRenderCheck(CModelObject* pModel);
	static void	ProcessQuestEmoticon();
private:
#endif // __CLIENT
#endif // __V050322_CACHEQUEST

	virtual void	MessageFromModel( DWORD dwMessage );

	void			EnforcedGhostCorr();
	void			ApproachGhostAngle();
	void			OnArriveAtPos();
	
	void			SetActParam( OBJACT act, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	int				GetActParam( int nIdx ) { return m_nAParam[nIdx]; }
	void			SetCmd( OBJACT cmd, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	void			SetCmdParam( int nIdx, int nValue ) { m_nCParam[nIdx] = nValue; }


public:


// trade 관련 코드 다시 조정할 것임 
	CVTInfo			m_vtInfo;
};

inline void CMover::SendAIMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	if( m_pAIInterface )
		m_pAIInterface->SendAIMsg( dwMsg, dwParam1, dwParam2 );
}

#ifdef __POSTAIMSG1008
inline void CMover::PostAIMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	if( m_pAIInterface )
		m_pAIInterface->PostAIMsg( dwMsg, dwParam1, dwParam2 );
}
#endif	// __POSTAIMSG1008

// 유저상태 알아봄
inline	BOOL CMover::IsMode( DWORD dwMode ) 
{ 
	if( (dwMode & TRANSPARENT_MODE) )	// 투명화를 검사하러 들어왔을때
		if( GetAdjParam(DST_CHRSTATE) & CHS_INVISIBILITY )	// 마법투명화가 걸렸는가?
			return TRUE;
	return ( ( m_dwMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	

#ifdef __S0504_FLY
inline	BOOL CMover::IsStateMode( DWORD dwMode ) 
{ 
	return ( ( m_dwStateMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	
#endif // __S0504_FLY

inline BOOL CMover::IsFly() 
{ 
#ifdef _DEBUG
	if( IsNPC() )
	{
		Message( "CMover::IsFly : NPC는 IsFly()를 사용하면 안된다. IsFlyingNPC()를 사용하셔! %s", m_szName );
		return FALSE;
	}
#endif
	return m_pActMover->IsFly();
}

#if !defined(__BEAST)
inline void CMover::ClearActParam() 
{ 
	m_oaAct = OBJACT_NONE; 
	memset( m_nAParam, 0xcd, sizeof(m_nAParam) );
}

inline int	CMover::DoAttackMelee( OBJID idTarget, OBJMSG dwMsg, DWORD dwItemID )
{
	CMover *pTarget = (CMover *)prj.GetMover( idTarget );
	if( IsInvalidObj(pTarget) )
		return 0;

	return DoAttackMelee( pTarget, dwMsg, dwItemID );
}
#endif // __BEAST

#if !defined(__WORLDSERVER)
inline int CMover::IsSteal( OBJID idTaget )		// id로 검사하는 버전.
{
	CMover *pTarget = prj.GetMover( idTaget );
	if( IsInvalidObj(pTarget) )		
		return 0;
	else
		return 	IsSteal( pTarget );
}
#endif	// __WORLDSERVER

inline int CMover::SendActMsg( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 ) 	
{	
	if( m_pActMover )	
		return m_pActMover->SendActMsg( dwMsg, nParam1, nParam2, nParam3, nParam4 );	
	else
		return 0;
}

inline int CMover::GetGold()
{
	int nGold = m_dwGold;
#ifndef __BEAST
	nGold -= m_vtInfo.TradeGetGold();
	ASSERT( nGold >= 0 );
#endif
	return nGold;
}

inline void CMover::SetGold( int nGold )
{
	ASSERT( nGold >= 0 );
	m_dwGold = (DWORD)nGold;
}


extern int GetWeaponPlusDamage( int nDamage, BOOL bRandom, ItemProp* pItemProp , int nOption );

#ifdef __BEAST
inline void CVTInfo::Init( CMover* )
{
}
#endif // __BEAST

#if defined( __CLIENT ) && defined( __XUZHU )
	#define DEBUG_CHATMSG( A, B, C )		g_WndMng.PutString( A, B, C );
#else	// client && xuzhu
	#define DEBUG_CHATMSG( A, B, C )		/##/
#endif // not (client && xuzhu)

#endif // !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)