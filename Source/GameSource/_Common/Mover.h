#if !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)
#define AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pocket.h"		//sun: 11, 주머니
#include "buff.h"

#include "Ctrl.h"
#include "..\_AIInterface\ActionMover.h"
#include "SkillInfluence.h"
#include "CooltimeMgr.h"
#include <memory/FLMemPooler.h>

#ifdef __WORLDSERVER
	#include "NpcProperty.h"
	#include "SfxHitArray.h"
	#include "..\_AIInterface\AIInterface.h"
	#include "FLAggro.h"
	#ifdef __ENCHANT_BARUNA16
	#include "NPCSummoner.h"
	#endif //__ENCHANT_BARUNA16
#else
	#include "CooltimeMgr.h"
#endif

#include "Synchronizer.h"
#include "Stat.h"


//#define	MAX_NAME				64


#define	COLOR_PLAYER			0xffffffff
#define	COLOR_NPC				0xffa0a0ff  
#define	COLOR_MONSTER			0xffffffa0
#define	COLOR_PARTY				0xff70f070
#define COLOR_GUILDCOMBAT		0xff00ff00

/// SetMotion() option
#define	MOP_NONE				0x00000000
#define	MOP_SWDFORCE			0x00000001		/// 검광 생성하게
#define	MOP_FIXED				0x00000002		/// dwMotion값을 그대로 로딩하게 ( +100 안하게.. )
#define	MOP_NO_TRANS			0x00000004		/// 모션 트랜지션 하지않음.
#define	MOP_HITSLOW				0x00000008		/// 타점이후부터 슬로우.

#define	REQ_USESKILL			0x00000001		/// 응답요청 플래그 - 스킬사용 응답
#define	REQ_MOTION				0x00000002		/// 응답요청 플래그 - 모션 응답

#define	OBJTYPE_PLAYER			0x00000001		
#define	OBJTYPE_MONSTER			0x00000002		
#define OBJTYPE_CTRL			0x00000004		
#define	OBJTYPE_FLYING			0x00010000		/// m_dwTypeFlag에서 사용.

#define	CRITICAL_PROBABILITY	3
#define	CRITICAL_BERSERK_HP		30
#define	CRITICAL_BERSERK_PROB	22
#define	CRITICAL_AFTER_DEATH	22

// UPDATEITEM
#define	UI_NUM							0
#define	UI_HP							1
#define	UI_RN							2
#define	UI_AO							3
#define	UI_RAO							4
#define	UI_IR							5
#define	UI_PIERCING_SIZE				6
#define	UI_PIERCING						7
#define UI_COOLTIME						8
#define	UI_FLAG							9
#define	UI_RANDOMOPTITEMID				10
#define	UI_KEEPTIME						11
#define UI_REPAIR_NUM					12
#define UI_ULTIMATE_PIERCING_SIZE		13
#define UI_ULTIMATE_PIERCING			14
#define UI_PETVIS_SIZE					15
#define UI_PETVIS_ITEM					16
#define UI_PETVIS_ITEMSWAP				17
#define UI_TRANSFORM_VISPET				18
#define UI_EQUIP_LEVEL					19
#define UI_LOOKS_CHANGE					20
// UPDATEITEM


const int PETVIS_DEFAULT_VIS_SLOT_SZIE	=	2;

#define RESTATE_LOW				50


//sun: 10차 전승시스템	Neuz, World, Trans

#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL )


#ifdef __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 1 )
#else	// __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 60 )
#endif	// __INTERNALSERVER

#define	MAX_CHEERPOINT	3
#define	NULL_CHGPARAM	0x7FFFFFFF

const DWORD NEXT_TICK_DUEL          = SEC(1);
const DWORD NEXT_TICK_ENDDUEL       = MIN(3);
const DWORD NEXT_TICK_RECOVERY      = SEC(2);
const DWORD NEXT_TICK_RECOVERYSTAND = SEC(3);


//sun: 10차 전승시스템	Neuz, World, Trans

#define	LEGEND_CLASS_NORMAL			0
#define	LEGEND_CLASS_MASTER			1
#define	LEGEND_CLASS_HERO			2
#define	LEGEND_CLASS_LEGEND_HERO	3


/// Replace함수에서 사용하는 타입 	
enum REPLACE_TYPE
{
	REPLACE_NORMAL,				/// 비행체 타고 인벤이 차면 막음 
	REPLACE_FORCE,				/// 무조건 이동 
};

/// 스킬 사용 타입
enum SKILLUSETYPE
{
	SUT_NORMAL,			/// 단축키에 넣고 사용하는 일반적인 방식.
	SUT_QUEUESTART,		/// 스킬큐로 처음 시작하는 스킬이다.(캐스팅을 몰아서함)
	SUT_QUEUEING,		/// 스킬큐의 연속발사중 스킬이다.(캐스팅동작없이 바로 발사)
};

////////////////////////////////////////////////////////////////////////////////
// PVP 관련 
////////////////////////////////////////////////////////////////////////////////

/// Player vs Player의 모드 
enum PVP_MODE
{
	PVP_MODE_NONE,				/// PVP불가 	
	PVP_MODE_GUILDWAR,			/// 길드워 
	PVP_MODE_PK,				/// PK
	PVP_MODE_DUEL				/// 듀얼 
};

/// 거래 확인 타입 
enum TRADE_CONFIRM_TYPE
{
	TRADE_CONFIRM_ERROR,		/// 인벤에 여유가 없거나, GOLD가 OVERFLOW나는 경우 
	TRADE_CONFIRM_OK,			/// 상대도 ok(최종 거래)
};

/// 거래상태 
enum TRADE_STATE
{
	TRADE_STEP_ITEM,			/// 아이템 단계 - 아이템을 올리고 내리고  
	TRADE_STEP_OK,			    /// OK누른 단계 - ok버튼을 눌렸다.        
	TRADE_STEP_CONFIRM,			/// CONFIRM단계 - 최종확인단계            
};

////////////////////////////////////////////////////////////////////////////////
// struct
////////////////////////////////////////////////////////////////////////////////

/// 장착 정보 구조체 
typedef	struct	_EQUIP_INFO
{
	DWORD	dwItemID;
	int		nOption;
	BYTE	byFlag;

	DWORD	dwLooksChangeItemID;
	bool	bIsCombined;

	_EQUIP_INFO()
	{
		Init();
	}

	void	Copy( FLItemElem & kItemElem )
	{
		dwItemID			= kItemElem.m_dwItemId;
		nOption				= kItemElem.GetAttrOption();
		byFlag				= kItemElem.m_byFlag;
		dwLooksChangeItemID	= kItemElem.GetLooksChangeItemID();
		bIsCombined			= kItemElem.IsSetCombinedOption();
	}

	void	Init()
	{
		dwItemID			= 0;
		nOption				= 0;
		byFlag				= 0;
		dwLooksChangeItemID	= 0;
		bIsCombined			= false;
	}

}	EQUIP_INFO,	*PEQUIP_INFO;

/// 장착 추가 정보 구조체 
// typedef	struct	_EQUIP_INFO_ADD
// {
// 	BYTE				byItemResist;
// 	int					nResistAbilityOption;
// 	WORD				wRandomOptionOriginId;
// 	int					nLevelDown;
// 
// 	DWORD				dwCombinedAddDamage;
// 	WORD				wCombinedRandomOptionOriginID;
// 
// 	FLPiercingOption	kPiercingOption;
// 	FLRandomOption		kRandomOption;
// 
// #ifdef __CLIENT
// 	CTexture*			pTexture;
// #endif	// __CLIENT
// 	_EQUIP_INFO_ADD()
// 	{
// 		byItemResist					= SAI79::NO_PROP;
// 		nResistAbilityOption			= 0;
// 		wRandomOptionOriginId			= 0;
// 		nLevelDown						= 0;
// 
// 		dwCombinedAddDamage				= 0;
// 		wCombinedRandomOptionOriginID	= 0;
// 
// 		kPiercingOption.Clear();
// 		kRandomOption.Clear();
// 
// #ifdef __CLIENT
// 		pTexture	= NULL;
// #endif	// __CLIENT
// 	}
// 
// 	void	CopyItemOption( FLItemElem & kItemElem )
// 	{
// 		byItemResist					= kItemElem.GetItemResist();
// 		nResistAbilityOption			= kItemElem.GetResistAbilityOption();
// 		wRandomOptionOriginId			= kItemElem.GetRandomOptionOriginID();
// 		nLevelDown						= kItemElem.GetLevelDown();
// 
// 		dwCombinedAddDamage				= kItemElem.GetCombinedAddDamage();
// 		wCombinedRandomOptionOriginID	= kItemElem.GetCombinedRandomOptionOriginID();
// 
// 		kItemElem.GetPiercingOption( kPiercingOption );
// 		kItemElem.GetRandomOption( kRandomOption );
// 	}
// 
// 	void	Serialize( CAr & ar )
// 	{
// 		ar << byItemResist;
// 		ar << nResistAbilityOption;
// 		ar << wRandomOptionOriginId;
// 		ar << nLevelDown;
// 
// 		ar << dwCombinedAddDamage;
// 		ar << wCombinedRandomOptionOriginID;
// 
// 		kPiercingOption.SerializePiercingOption( ar );
// 		kPiercingOption.SerializeCombinedPiercingOption( ar );
// 		kRandomOption.SerializeRandomOptionExtension( ar );
// 		kRandomOption.SerializeCombinedRandomOptionExtension( ar );
// 	}
// 
// 	void	Deserialize( CAr & ar )
// 	{
// 		ar >> byItemResist;
// 		ar >> nResistAbilityOption;
// 		ar >> wRandomOptionOriginId;
// 		ar >> nLevelDown;
// 
// 		ar >> dwCombinedAddDamage;
// 		ar >> wCombinedRandomOptionOriginID;
// 
// 		kPiercingOption.DeserializePiercingOption( ar );
// 		kPiercingOption.DeserializeCombinedPiercingOption( ar );
// 		kRandomOption.DeserializeRandomOptionExtension( ar );
// 		kRandomOption.DeserializeCombinedRandomOptionExtension( ar );
// 	}
// 
// }	EQUIP_INFO_ADD, *PEQUIP_INFO_ADD;

/// 플레이어 외양 구조체 
typedef struct tagPLAYER
{
	EQUIP_INFO	m_aEquipInfo[MAX_HUMAN_PARTS];
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

/// 퀘스트 구조체 
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
//	BYTE		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	WORD		m_nKillNPCNum[ MAX_QUEST_KILL_NPC_COUNT ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	BYTE		m_bPatrol   : 1;
	BYTE		m_bDialog   : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;

	QuestProp* GetProp() { return prj.m_aPropQuest.GetAt( m_wId ); }

} QUEST,* LPQUEST;

/// 피격 정보 구조체 
struct HIT_INFO
{
	int		nHit;		/// 입은 데미지 
	DWORD	dwTick;		/// 공격당한 시각 
};

typedef std::map<OBJID, HIT_INFO> SET_OBJID;		

/// 공격의 타입 
enum HITTYPE
{
	HITTYPE_FAIL	= 0,
	HITTYPE_GENERIC,			/// Player vs Monster
	HITTYPE_PVP,				/// Player vs Player
	HITTYPE_WAR,				/// 길드전 
	HITTYPE_SCHOOL,				/// 학교 대항 이벤트 타입 
	HITTYPE_PK,					/// Player Killing
	HITTYPE_GUILDCOMBAT,		/// 길드대전 
	HITTYPE_ARENA,				// 아래나
	HITTYPE_EVENTARENA,			// 이벤트 아레나
};

/// 리저렉션 데이타 구조체 
typedef struct RESURRECTION_DATA
{
	u_long		  dwPlayerID;
	BOOL		  bUseing;
	SkillProp	 *pSkillProp;
	AddSkillProp *pAddSkillProp;
	int			  nDamage;
} RESURRECTION_DATA;

/// 거래후 결과 구조체 
struct VENDOR_SELL_RESULT
{
	FLItemElem	item;			/// 팔린 아이템 
	int			nRemain;		/// 팔리고 남은 갯수 
	int			nErrorCode;		/// 에러코드 
};

/// 개인상점(Vendor)과 거래(Trade) 클래스 
class CVTInfo
{
public:
	CVTInfo();

private:
	DWORD					m_dwTradeGold;					/// 거래중인 돈 
	FLItemBase*				m_apItem_VT[MAX_VENDITEM];		/// vendor and trader share pointer array
	OBJID					m_objId;						/// 상대방 id
	CMover*					m_pOwner;						/// 클래스 소유자 
	TRADE_STATE				m_state;						/// 거래상태 

	std::string					m_strTitle;						/// 개인상점의 타이틀 

public:
	void					Clear();


	OBJID					GetOtherID() { return m_objId; }
	CMover*					GetOther() const;
	void					SetOther( CMover* pMover );
	void					Init( CMover* pMover );
	FLItemBase*				GetItem( BYTE byNth );
	void					SetItem( BYTE byNth, FLItemBase* pItemBase );
	LPCTSTR					GetTitle();
	void					SetTitle( LPCTSTR szTitle );
	BOOL					IsVendorOpen();

	void					TradeClear();
	void					TradeSetItem( DWORD dwItemObjID, BYTE byNth, int nItemNum );
	BOOL					TradeClearItem( BYTE byNth );
	void					TradeSetGold( DWORD dwGold );
	int						TradeGetGold();
	BOOL					TradeConsent( CAr & ar );
	DWORD					TradeSetItem2( DWORD dwItemObjID, BYTE byNth, int & nItemNum );	// Return: dwText 
	TRADE_CONFIRM_TYPE		TradeLastConfirm( CAr& ownerar, CAr& traderar, CAr & logar );
	TRADE_STATE				TradeGetState();
	void					TradeSetState( TRADE_STATE state );

	void					VendorClose( BOOL bClearTitle = TRUE );	// 개인 상점 닫기
	void					VendorCopyItems( FLItemBase** ppItemVd );
	void					VendorItemNum( BYTE byNth, int nNum );
	void					VendorSetItem( DWORD dwItemObjID, BYTE byNth, int nNum, int nCost );
	BOOL					VendorClearItem( BYTE byNth );
	BOOL					VendorSellItem( CMover* pBuyer, BYTE byNth, DWORD dwItemId, int nNum, VENDOR_SELL_RESULT& result );
	BOOL					VendorIsVendor(); 
	BOOL				IsTrading( FLItemElem* pItemElem );
};

/// 서버에서 SFX해킹 때문에 생긴 자료형 
struct SFXHIT_INFO
{
	OBJID	idTarget;
	int		nMagicPower;
	DWORD	dwSkill;
	int		nDmgCnt;
	float	fDmgAngle;
	float	fDmgPower;
	DWORD	dwAtkFlags;

	BOOL	bControl;		//sun: 8, // __S8_PK

	DWORD	dwTickCount;
};

struct	SFXHIT_COUNT
{
	DWORD	dwSkill;
	DWORD	dwTickCount;
};

class	CGuild;
class	CGuildWar;
class	CNpcProperty;
struct	ATTACK_INFO;
struct	REGIONELEM;
class	CParty;
class	CActionMover;

#ifdef __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type, nLayer )	Replace( (uMulti), (dwWorld), (vPos), (type), (nLayer) )
#else	// __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type )			Replace( (uMulti), (dwWorld), (vPos), (type) )
#endif	// __LAYER_1015

#ifdef __CLIENT
class CClientPet
{
public:
	CClientPet();
	virtual	~CClientPet();
public:
	void	SetObj( CMover* pObj )	{	m_pObj	= pObj;	}
	CMover*	GetObj()	{	return m_pObj;		}
	void	SetLevelup( BYTE nLevelup )		{	m_nLevelup	= nLevelup;		}
	BYTE	GetLevelup()	{	return m_nLevelup;	}
//sun: ?, __PET_1024
	void	SetName( char* szName );
	char*	GetName()	{	return m_szName;	}
	BOOL	HasName()	{	return strlen( m_szName ) > 0;	}

private:
	CMover*	m_pObj;
	BYTE	m_nLevelup;		// PLU_LEVEL_UP 이면, 펫 생성 시 펫 레벨업 효과 생성하고 0으로 초기화
	
//sun: ?, __PET_1024
	char	m_szName[MAX_PET_NAME];

};
#endif	// __CLIENT

typedef	struct	_T_MOVE_CHECK_DATA
{
	DWORD	dwPrevMoveTick;
	DWORD	dwLastMoveTick;

} T_MOVE_CHECK_DATA, *PT_MOVE_CHECK_DATA;

namespace nsRevival
{
	struct DECREASE_EXP_DATA
	{
		float	decreaseExpRate;
		bool	downLevel;

		DECREASE_EXP_DATA() : decreaseExpRate( 0.0f ), downLevel( false )	{}
	};
}

/// 플레이어와 NPC
class CMover : public CCtrl
{	
public:	
	CMover();
	virtual ~CMover();

	friend			CActionMover;
	enum			{ PLAYER, NPC };			/// CMover의 타입 

public:
	//DWORD			m_dwUpdateDestTick;

	T_MOVE_CHECK_DATA	m_tMoveCheckData;

	T_CONNECTION_POINT	m_kMadrigalGiftPoint;

#ifdef __WORLDSERVER
	int					m_nNearPCCount;
#endif 

	BOOL			m_bPlayer;					/// Player인가, 아니라면 NPC
	u_long			m_idPlayer;					/// player의 db번호, NPC는 0xffffffff
	CActionMover*	m_pActMover;				/// 무버를 움직이는 객체. 여기서 무버의 움직임을 컨트롤한다.
	DWORD			m_dwTypeFlag;				/// OBJTYPE_FLYING 만 사용 flag
	DWORD			m_dwMode;					/// 무적, 투명, 초필 등등 flag
	DWORD			m_dwFlag;					/// MVRF_ 관련 flag 시리즈
	DWORD			m_dwStateMode;				/// PK, PVP, 비행 시전 ... flag
	float			m_fAniSpeed;				/// 애니메이션 속도 배수. 1.0이 기본. 2.0이면 두배빠르기. 2.0이 맥스다.
	float			m_fArrivalRange;			/// 목표에 얼마만큼 근접해야하는가. 미터단위. 디폴트 0
	OBJID			m_idDest;					/// 목표 객체 아이디
	D3DXVECTOR3		m_vDestPos;					/// 목표 지점 
	CShip*          m_pIADestPos;				/// 여기에 포인터가 있다면 m_vDestPos의 좌표는 m_pIADestPos오브젝에 대한 상대좌표가 된다.
	BOOL			m_fWaitQueryGetPos;			/// 동기화 관련 
	bool			m_bForward;					/// 전진중?
	bool			m_bPositiveX;				/// GetPos().x - m_vDestPos.x > 0.0f
	bool			m_bPositiveZ;				/// GetPos().z - m_vDestPos.z > 0.0f
	DWORD			m_dwRegionAttr;				/// 현재 좌표의 region 속성 
	DWORD			m_dwOldRegionAttr;			/// 이전 좌표의 region 속성 
	DWORD			m_dwMotion;					/// 모션 번호 
	DWORD			m_dwMotionOption;			/// 모션 추가 옵션
	OBJMSG			m_dwMotionArrive;			/// 이동 완료 후 모션

	DWORD			m_dwTickRecovery;			/// 앉은 상태의 hp, mp 복구 timer
	DWORD			m_dwTickRecoveryStand;		/// stand 상태의 hp, mp 복구 timer

#if defined __WORLDSERVER && defined __AGGRO16
	FLAggro::MapAggroOwner		m_AggroOwnerList;	/// 내가 어그로를 먹은 상대 리스트 (상호참조)
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
	BOOL			m_IsReturnToBegin;

	BOOL			TryDelete_IntoIdleMode();
	void			SetDelete_IntoIdleMode( const BOOL bDelete );
private:
    BOOL			m_bDelete_IntoIdleMode;		///AI에서 IDLE 모드로 돌아올시 객체 삭제( 전투가 끝나면 삭제해야 하는 경우 )
#endif // __WORLDSERVER

public:
	int				m_nAccountPlayTime;		//ms단위 계정 플레이 시간

	BOOL	HasBuff( WORD wType, WORD wId );
	void	RemoveBuff( WORD wType, WORD wId );
	BOOL	HasBuffByIk3( DWORD dwIk3 );

	//////////////////////////////////////////////////////////////////////////
	// mirchang_101101
	bool	HasUseLimitGroupItemBuff( int nUseLimitGroup );
	int		GetCountBuffByItemGroup( int nItemGroup );
	int		GetCountBuffByItemId( DWORD dwId );
	int		GetAdjValueDuplicationBuff( DWORD dwItemKind3 );
	int		GetRemainCountBuff( const DWORD dwItemKind3 );

	IBuff*	GetBuffByIk3( DWORD dwItemKind3 );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void	DoApplyHitActiveSkill( CMover* pDest, const DWORD dwProb );
	void	DoApplyDamageActiveSkill( CMover* pDest, const DWORD dwProb );
	//////////////////////////////////////////////////////////////////////////

#ifdef __WORLDSERVER
	BOOL	HasPet();
	void	RemovePet();
#endif	// __WORLDSERVER
	DWORD	GetDisguise();
	BOOL	AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, DWORD tmPain, BYTE byBuffTickType, OBJID oiAttacker = NULL_ID );
	void	RemoveCommonBuffs();
	void	RemoveChrStateBuffs( DWORD dwChrState );
	void	RemoveIk3Buffs( DWORD dwIk3 );
	void	RemoveAllBuff();
	void	RemoveAttackerBuffs( OBJID oiAttacker );
	void	RemoveDstParamBuffs( DWORD dwDstParam );
	void	RemoveDebufBuffs();
	void	ProcessBuff();

	void	RemoveDebuffOnDamaged();
#ifdef __CLIENT
	void	ClearBuffInst();
	int		GetCurrentMaxSkillPoint();
#endif	// __CLIENT

	CBuffMgr		m_buffs;

	DWORD			m_dwSMTime[SM_MAX];			/// 유료 아이템 시간 값을 가지고 있음

	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];		/// 스킬 배열 
	DWORD			m_tmReUseDelay[ MAX_SKILL_JOB ];	/// 스킬 재사용시각
	
	Stat			m_Stat;
	LONG			m_nLevel;					/// 레벨 
	LONG			m_nDeathLevel;				/// 죽었을 때 레벨
	EXPINTEGER		m_nExp1, m_nDeathExp;		/// m_nExp1 - 경험치, DeathExp - 죽었을 때 경험치

private:
	int		m_nTutorialState;	//sun: 12, 튜토리얼 개선

public:
//sun: 12, 튜토리얼 개선
	LONG	GetFlightLv( void )	{	return ( GetLevel() >= 20? 1: 0 );		}
	void	SetFlightLv( LONG /*nFlightLv*/ )	{}
	int		GetTutorialState( void )	{	return m_nTutorialState;	}
	void	SetTutorialState( int nTutorialState )	{	m_nTutorialState	= nTutorialState;	}

	LONG			m_nFxp;		/// 비행레벨, 
	LONG			m_nHitPoint;				/// HP
	LONG			m_nManaPoint;				/// MP
	LONG			m_nFatiguePoint;			/// FP( 지구력 )
	LONG			m_nDefenseMin, m_nDefenseMax;	/// 방어력 min, 방어력 max
	int				m_nAdjHitRate, m_nAdjParry;		/// 수정치 
	LONG			m_nJob;						/// 직업

	BYTE			m_bySex;						/// 성별 
	DWORD			m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHairColor, m_dwHeadMesh;	/// 외양 
	FLOAT			m_fHairColorR, m_fHairColorG, m_fHairColorB;						/// 머리색 

	DWORD			m_dwAuthorization;			/// 유저 권한 (일반에서 최고 관리자까지 )

	TCHAR			m_szCharacterKey[32];		/// NPC key문자열 
	DWORD			m_dwBelligerence;			/// 호전성 여부 
	BOOL			m_bActiveAttack;			/// 선공몹인가?
	DWORD			m_dwVirtItem;				/// npc가 사용(공격)할 아이템. 또는 플레이어가 무기 비장착시 사용할 기본 아이템. 
	DWORD			m_dwVirtType;				/// m_dwVirtItem가 아이템인지 스킬인지 결정 VT_ITEM, VT_SKILL
//private:
	OBJID	m_oiEatPet;
public:
	int				m_nFuel;					/// 비행체 현재 연료.
	int				m_tmAccFuel;				/// 가속연료(시간단위)
	BOOL			m_bItemFind;				/// 캐스팅 후에 아이템이 있는지 검사할 것인가? 
	int				m_nReadyTime;				/// 비행체 타기 캐스팅 시간
	OBJID			m_dwUseItemId;				/// 비행체 타기 아이템 아이디 

	DWORD			m_dwCtrlReadyTime;			/// box open 캐스팅 타입  
	OBJID			m_dwCtrlReadyId;			/// 캐스팅 대상 box 아이디  

	OBJID			m_idAttacker;				/// this를 실제 공격한넘
	OBJID			m_idTargeter;				/// this를 타겟잡고 점유하고 있는넘.
	OBJID			m_idTracking;				/// 비행중 자동추적하고 있는 타겟.
	OBJID			m_idLastHitMover;			/// this가 마지막으로 쳤던 무버아이디
	float			m_fExpRatio;				/// exp 지급률. this를 죽였을때 this의 exp의 몇퍼센트를 줄것인가. 디폴트는 1.0이다. 이것은 파티때는 적용되지 않는다.
	int				m_nAtkCnt;					/// 공격하면 카운터가 0이되면서 계속 증가한다.
	int				m_nReflexDmg;				/// 리플렉스 데미지 누적 (서버만)
	LONG			m_nPlusMaxHitPoint;			/// 추가 MAX HP
	DWORD			m_tmPoisonUnit;				/// 독 적용 시간
	DWORD			m_tmBleedingUnit;			/// 몇초마다 한번씩인가?
	OBJID			m_idPoisonAttacker;			/// 독 공격자
	OBJID			m_idBleedingAttacker;		/// 출혈 공격자
	short			m_wStunCnt;					/// 스턴상태 시간 
	short			m_wPoisonCnt;				/// 독상태 시간 
	short			m_wPoisonDamage;			/// 독으로 깎일 데미지
	short			m_wDarkCnt;					/// 암흑 지속시간
	short			m_wDarkVal;					/// 암흑동안 깎일 명중률 수정치
	short			m_wBleedingCnt;				/// 출혈 상태 시간 
	short			m_wBleedingDamage;			/// 출혈로 깎일 데미지
	short			m_wDarkCover;				/// 안보이게 숨기

	short			m_nHealCnt;					//sun: 9, 9-10차 펫 //	0일 경우만 자동 치유	

//sun: 9, //__AI_0509
	FLOAT	m_fSpeedFactor;
	void	SetSpeedFactor( FLOAT fSpeedFactor );
	FLOAT	GetSpeedFactor( void )	{	return m_fSpeedFactor;	}
#ifdef __WORLDSERVER
	BOOL	IsReturnToBegin( void );
#endif	// __WORLDSERVER

//sun: 10, 메테오니커 AI 수정
	BOOL	IsRank( DWORD dwClass )	
		{
			MoverProp* pProp	= GetProp();
			return( pProp && pProp->dwClass == dwClass );
		}

	BYTE			m_nAttackResistLeft;		/// 코드 존재하지만 게임내 사용하지 않음 ( 왼손 속성 공격력 추가 ) 
	BYTE			m_nAttackResistRight;		/// 코드 존재하지만 게임내 사용하지 않음 ( 오른손 속성 공격력 추가 ) 
	BYTE			m_nDefenseResist;			/// 코드 존재하지만 게임내 사용하지 않음 ( 속성 방어력 ) 
	
	u_long			m_idparty;					/// 파티 번호 (클라에선 특별한 경우가 아니면 사용해선 안됨  g_Party.m_uPartyId를 사용 )
	u_long			m_idGuild;					/// 길드 번호 
	BOOL			m_bGuildBank;				/// 길드 뱅크를 이용중인지 확인
	DWORD			m_idGuildCloak;				/// 망토의 길드번호를 가지고 있음
	u_long			m_idWar;					/// 길드전 고유 번호 

	DWORD			m_tmActionPoint;			/// 액션포인트 자동상승 타이머(3초) TODO : 19차에서는 사용 안함.
	DWORD			m_dwTickCheer;				/// 응원 타이머 
	int				m_nCheerPoint;				/// 타인에게 응원 받은 값  

	DWORD			m_dwTickDuel;				/// 듀얼 상태 타이머 
	DWORD			m_dwTickEndDuel;			/// 듀얼 종료 타이머 
	int				m_nDuelState;				/// 듀얼상태 
	int				m_nDuel;					/// 듀얼중 상태 ( 0:없음 1:개인듀얼중 2:파티듀얼중 )
	OBJID			m_idDuelOther;				/// 듀얼 상대의 아이디
	u_long			m_idDuelParty;				/// 파티듀얼중이면 상대방 파티의 아이디 
	
	int				m_nFame;					/// 명성치 
	u_long			m_idMurderer;				/// 날 마지막으로 죽였던넘 플레이어 아이디
//sun: 8, // __S8_PK
	DWORD			m_dwPKTime;					/// 핑크 상태 시간
	int				m_nPKValue;					/// PK 수치
	DWORD			m_dwPKPropensity;			/// PK 성향
	DWORD			m_dwPKExp;					/// PK 성향 습득 경험치		
	
	EXPINTEGER		m_nAngelExp;				/// 엔젤 경험치
	LONG			m_nAngelLevel;				/// 엔젤 Level

	BYTE			m_nLegend;					//sun: 10차 전승시스템	Neuz, World, Trans

//sun, 11, 확율스킬 효과수정 world,neuz
	DWORD			dwRemoveSfxObj[MAX_SKILLBUFF_COUNT];			// 삭제할 효과 오브젝트 
	DWORD			GetRemoveSfxObj(DWORD	dwRemoveSfxObjID);
	void			SetRemoveSfxObj(DWORD	dwRemoveSfxObjID);

//sun: 13, 달인
	int				m_nHonor;					// 달인선택 
	DWORD			m_dwHonorCheckTime;			//달인 시간체크
	int				m_aHonorTitle[MAX_HONOR_TITLE];			// 달인수치
	int				GetHonorTitle(int nIdx)	{	return m_aHonorTitle[nIdx];	}
	void			SetHonorCount(int nIdx , int nCount );
	void			CheckHonorStat();
	void			CheckHonorTime();
	void			SetHonorAdd(int nSmallIdx,int nLargeGroup,int nDefault = 0);
	CString			m_strTitle;
	LPCTSTR			GetTitle();
	void			SetTitle(LPCTSTR	pTitle);

#ifdef __CLIENT
	CSfx*			m_pSfxWing;
	DWORD			m_dwWingTime;

	CSfx*			m_pSfxBuffPet;
	void			SetSfxBuffPet( const DWORD idEffect );
#endif //__CLIENT

	DWORD		m_dwMute;

	RESURRECTION_DATA				m_Resurrection_Data;		/// 리저렉션 스킬 정보 
	CItemContainer< FLItemElem  >*	m_ShopInventory[ MAX_VENDOR_INVENTORY_TAB ];	/// 상인NPC의 물품 정보 
	CItemContainer< FLItemElem  >	m_Inventory;				/// 인벤토리

	CPocketController	m_Pocket;	//sun: 11, 주머니

	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];				/// 장착정보 	
	CVTInfo			m_vtInfo;					/// 거래와 개인상점 번호 

	BYTE			m_nDBDataSlot;					/// 로그인 화면의 3개 캐릭터 순서 번호 
	BOOL			m_bBank;					/// 은행 이용 중?
	u_long			m_idPlayerBank[MAX_CHARACTER_SLOT];			/// 3개 캐릭터 idPlayer
	DWORD			m_dwGoldBank[MAX_CHARACTER_SLOT];			/// 3개 캐릭터 길드돈 
	CItemContainer< FLItemElem >	m_Bank[MAX_CHARACTER_SLOT];		/// 3개 캐릭터 은행 아이템 

	BYTE			m_nQuestKeeping;			/// 클락워크 길드 퀘스트 번호  
	BYTE			m_nPartyQuestKeeping;		/// 거대 몬스터 퀘스트 번호 
	size_t			m_nQuestSize;				/// 퀘스트 갯수 
	LPQUEST			m_aQuest;					/// 현재 진행중인 퀘스트 배열 
	size_t			m_nCompleteQuestSize;		/// 완료된 퀘스트 갯수 
	LPWORD   		m_aCompleteQuest;			/// 완료된 퀘스트 배열 	
	size_t			m_nCheckedQuestSize;
	LPWORD			m_aCheckedQuest;

	int				m_nDead;					/// 죽은 후 5초간은 무적
	int				m_nGuildCombatState;		/// 길드 대전 상태 
	D3DXVECTOR3		m_vMarkingPos;				/// 로드라이트에서 마킹한 좌표 
	OBJID			m_idMarkingWorld;			/// 로드라이트에서 마킹한 월드 아이디 

#ifdef __CLIENT
	static BOOL		m_bQuestEmoticonAdd;		/// 퀘스트 아이콘 확축 프로세스 
	static FLOAT	m_fQuestEmoticonScale;
	int				m_nQuestEmoticonIndex;
	bool			m_bShowQuestEmoticon;

	NaviPoint		m_nvPoint;
	V_NaviPoint		m_vOtherPoint;
	int				m_idSfxHit;
	BYTE			m_nWaterCircleCount;
	DWORD			m_dwReqFlag;				/// 응답요청 플래그.
	char			m_szPartyName[32];
	float			m_fDestScale;				/// 쪼그라들때 부드럽게 하기위한..
	float			m_fDestScaleSlerp;			/// 1.0f ~ 0.0으로 변화함.
	CCooltimeMgr	m_cooltimeMgr;	
	LPDIRECT3DTEXTURE9		m_pCloakTexture;	/// 망토텍스쳐.

	//sun: 눈깜박거리기... Neuz
	static LPDIRECT3DTEXTURE9		m_pTextureEye[2][MAX_HEAD];
	static LPDIRECT3DTEXTURE9		m_pTextureEyeFlash[2][MAX_HEAD];

#endif // __CLIENT

#ifdef __WORLDSERVER
	CNpcProperty*	m_pNpcProperty;				/// NPC 고유 정보( 대사, 퀘스트, 파일이름 ) 현재는 마을 NPC 대화처리자
	CTimer          m_timerQuestLimitTime;		/// 퀘스트 제한 시간 타이머 
	DWORD	        m_dwPKTargetLimit;			/// PK선공제한시각
	DWORD			m_dwTickCreated;			/// 생성 시각 
//	BOOL			m_bLastPK;					/// 마지막으로 죽었을때 플레이어에게(듀얼포함) 죽은건가 그외의 경우로 죽은건가.
//	BOOL			m_bLastDuelParty;			/// 파티듀얼로 사망?
	CTime			m_tGuildMember;				/// 길드 탈퇴 시각 
//	BOOL			m_bGuildCombat;				/// 길드 대전 중?
//	OBJID			m_idCollecter;				/// this의 채집소유자.
//	int				m_nResource;				/// 자원 보유량. - 몬스터측에서 사용.
//	int				m_nCollectOwnCnt;			/// > 0 인상황은 this는 m_idCollecter것이다.
	CSfxHitArray	m_sfxHitArray;				/// sfx hit 정보 배열 
	std::map< OBJID, std::queue< SFXHIT_INFO> >	m_mapSFXInfo;		/// sfx 해킹 때문에 ....	
//	std::map< OBJID, int>					m_mapSFXCount;		/// sfx가 발사되면 ++ 
	std::map< OBJID, std::queue< SFXHIT_COUNT > >	m_mapSFXCount;
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
//private:
	void	DoUseEatPet( FLItemElem* PitemElem );
	void	DoUseSystemPet( FLItemElem* pItemElem );
	void	ActivateEatPet( FLItemElem* pItemElem );
	void	ActivateSystemPet( FLItemElem* pItemElem );
public:
	void	InactivateEatPet( void );
	void	InactivateSystemPet( FLItemElem* pItemElem );

	void	SetPetVisDST( FLItemElem* pItemElem );
	void	ResetPetVisDST( FLItemElem* pItemElem );
	void	ProcessVisPet();

#if defined __WORLDSERVER && defined __ENCHANT_BARUNA16		
public:
	void					SetNPCOwner( const OBJID OwnerOBJID );
	OBJID					GetNPCOwner() const;

	void					DoUseSummonNPC( FLItemElem & ItemElem );
	FLNPCSummoner &			GetBarunaNPCSummoner();

protected:	
	FLNPCSummoner			m_BarunaNPCSummoner;			//NPC 소환객체 ( User객체용 )

	OBJID					m_NPCOwnerOBJID;					//이 객체(NPC)의 주인. (NPC 객체용)
#endif //__ENCHANT_BARUNA16		

#endif	// __WORLDSERVER

public:
	BOOL	IsUsingEatPet( FLItemElem* pItemElem ) const;	//sun: 12, 펫 각성 // 사용중인 먹펫인가?

	BOOL	HasActivatedEatPet( void ) const		{	return m_oiEatPet != NULL_ID;	}
	BOOL	HasActivatedSystemPet( void )	{	return m_dwPetId != NULL_ID;	}

	OBJID	GetEatPetId( void ) const		{	return m_oiEatPet;		}
	void	SetEatPetId( OBJID oiEatPet )		{	m_oiEatPet	= oiEatPet;	}
	DWORD	GetPetId( void ) const	{	return m_dwPetId;	}
	void	SetPetId( DWORD dwPetId )		{	m_dwPetId	= dwPetId;	}
//private:
	DWORD	m_dwPetId;	// 소환중이 펫의 인벤토리 위치(自), 소환중인 펫 인덱스(他)

	int		DoUseItemPetTonic( FLItemElem* pItemElem );	//sun: 12, 펫 각성 // 펫 영양제 사용	
private:
	BOOL	SetValidNeedVis( FLItemElem* pItemElem, int nPos, std::vector<BYTE> & vecValid );
public:
	std::vector<BYTE>	GetValidVisTable( FLItemElem* pItemElem );
	BYTE		IsSatisfyNeedVis( FLItemElem* pItemElemVisPet, PT_ITEM_SPEC ptItemSpecVis );			
	FLItemElem*	GetVisPetItem( void )	{ return m_Inventory.GetAtId( m_objIdVisPet ); }
	void		SetVisPetItem( OBJID objId )	{ m_objIdVisPet = objId; }
	BOOL		HasActivatedVisPet()	{ return m_objIdVisPet != NULL_ID; }
	DWORD		m_dwMoverSfxId;	// 무버에 붙는 이펙트
private:
	OBJID		m_objIdVisPet;	// 소환중인 비스펫의 인벤토리 위치

public:
//sun: 9, 9-10차 펫
	CPet*	GetPet( void );
	FLItemElem*	GetPetItem( void );
	void	PetLevelup( void );
	void	PetRelease( void );

#ifdef __CLIENT
	CClientPet	m_pet;
	CModelObject*	GetPetModel( void )		{	return (CModelObject*)( m_pet.GetObj()? m_pet.GetObj()->m_pModel: NULL );	}
	void	CreatePetSfx( void );

	BOOL	IsOptionRenderMask()	{	return !IsMode( MODE_OPTION_DONT_RENDER_MASK );	}	//sun: 12, 옵션창 개선
	BOOL	IsOptionRenderCostume(int nCostumeIdx);

#else	// __CLIENT	// __WORLDSERVER
	void	ProcessPetAvail( void );
	void	ProcessPetEnergy( void );
	void	ProcessPetExp( void );
	int		DoUseItemFeedPocket( FLItemElem* pItemElem );
#endif	// __CLIENT

//sun: 11, 주머니
	FLItemElem*	GetItemId2( int nPocket, DWORD dwItemObjID, BOOL bExpiration = TRUE );
	BOOL	CreateItem2( FLItemElem* pItem, int nPocket );
	void	RemoveItem2( DWORD dwItemObjID, int nNum, int nPocket, BOOL bExpiration = TRUE );

//sun: 11, 채집 시스템
	int						m_nMaxCltTime;
	int						m_nCltTime;
	BOOL	IsCollecting( void )	{	return	m_pActMover->GetActionState() == OBJSTA_COLLECT;	}
	virtual	void	ProcessCollecting( void );
	virtual	void	StartCollecting( void );
	virtual	void	StopCollecting( void );
	FLItemElem*	GetCollector( void );

	int				m_nSkillLevel;				/// 지금까지 올린 스킬레벨
	int				m_nSkillPoint;				/// 스킬 찍을 포인트 수치 

	CTimer			m_SkillTimer;
	BOOL			m_SkillTimerStop;
	
	//sun: 8차 엔젤 소환 Neuz, World, Trans
	BOOL			m_pAngelFlag;				/// Angel Buff Exist?
	CModelObject*	m_pAngel;					/// Angel Model Object.
	D3DXVECTOR3		m_AngelPos;					/// Angel Position.
	D3DXMATRIX		m_AngelWorldM;				/// Angel Matrix.
	DWORD			m_dwAngelKind;				/// Angel Kind.
	
//sun:10, __EVE_BALLOON
	BOOL			m_pBalloonFlag;				/// Balloon Buff Exist?
	CModelObject*	m_pBalloon;					/// Balloon Model Object.
	D3DXVECTOR3		m_BalloonPos;				/// Balloon Position.
	D3DXMATRIX		m_BalloonWorldM;			/// Balloon Matrix.
	DWORD			m_dwBalloonKind;			/// Balloon Kind.
	float			m_fBalloonYPos;				/// Balloon Y Position
	BOOL			m_bBalloon;

private:
	SET_OBJID		m_idEnemies;				/// 적에게 공격받은 정보 

	OBJACT			m_oaCmd;					/// 오브젝트 명령  
	int				m_nCParam[3];				/// 범용 명령 파라메터
	OBJACT			m_oaAct;					/// 오브젝트 행동상태

	int				m_nAParam[5];				/// 범용 행동 파라메터

	LONG			m_adjParamAry[ MAX_ADJPARAMARY ];		/// 수정 파라미터( 예: hp + 수정hp )
	LONG			m_chgParamAry[ MAX_ADJPARAMARY ];		/// 교체 파라미터( 예: 교체 hp )

	BOOL			m_bRegenItem;				/// NPC 상인이 팔 아이템 생성해 두었는가 flag  
	int				m_nMovePattern;				/// NPC 패턴 이동 타입
	int				m_nMoveEvent;				/// NPC 패턴 이동때 쓰이는 상태변화 99면 상태끝
	int				m_nMoveEventCnt;			/// NPC 패턴이동때 쓰이는 상태변화카운터

#ifdef	__CLIENT
	DWORD			m_dwLadolfFlag;
	CModelObject*   m_pLadolf;					/// 라돌프모델
	int				m_nDmgCnt;					/// 데미지 카운트
	enum WingStateFlag
	{
		FLOATING, FLYING, TURNING_L, TURNING_R
	};

	WingStateFlag	m_eWingStateFlag;		/// 애니메이션 플래그 스위치
	float			m_fOldLengthSq;			/// 날개 애니메이션 속도 변화량의 시작점을 저장하기 위한 변수
	CTimer			m_tmEye[2];				//sun: 눈깜박거리기... Neuz
public:
	BOOL			m_bExclusiveLowBody;
	
	// 날개를 망토에 부착할려고 시도한 코드		2016.07.11
	//CModelObject*   m_pCloakWing;					/// 망토 위치에 날개 착용
#endif // __CLIENT

private:
	u_long	m_idCampus;			// 사제 아이디
	int		m_nCampusPoint;		// 사제 포인트
public:
	u_long	GetCampusId()		{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )		{	m_idCampus = idCampus;	}
	int		GetCampusPoint()	{	return m_nCampusPoint;	}
	void	SetCampusPoint( int nMPPoint )		{	m_nCampusPoint = nMPPoint;	}

//protected:
	int				m_nCount;					/// 무버가 범용으로 쓰는 순차적 카운터. 생성자 외엔 0으로 초기화 하지 말것.
	DWORD			m_dwGold;					/// 페냐 
	DWORD			m_dwRideItemIdx;			/// 비행체의 아이템 인덱스
	CModelObject*	m_pRide;					/// 비행체 객체 포인터 
	TCHAR			m_szName[MAX_NAME];			/// 이름 
	
public:
	static CMover*  GetActiveMover() { return (CMover*)m_pObjActive; }	// 주인공 객체 얻기 	
	static	int		GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair );
	static void		UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< FLItemElem  >* pInventory, BOOL bIfParts = TRUE, CMover* pMover = NULL );
	static BOOL		DoEquip( int nSex, int nSkinSet, FLItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< FLItemElem  >* pInventory, PEQUIP_INFO pEquipeInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover );
	static	float	GetItemEnduranceInfluence( int nEndurance );	
	static	int		GetItemEnduranceWeight( int nEndurance );	

	virtual	BOOL	SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE, BOOL bDestParam = TRUE );
	virtual BOOL	Read( CFileIO* pFile );
	virtual void	Process();
	virtual	void	Serialize( CAr & ar, int nMethod ); // 시리얼라이즈 ; 네트웍 상태에서 서버와 클라이언트, 클라이언트 서버가 주고받을 패킷 내용 
	virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );
	virtual void	InitProp( BOOL bInitAI = TRUE );		// 객체를 프로퍼티 내용으로 초기화 	
//	virtual int		OnActCollecting();				// User만 사용되는 것이므로 FLWSUser가서 찾을것.
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamage( dwAtkFlag, idAttacker, nParam, bTarget );  }
	virtual int		SendDamageForce( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamageForce( dwAtkFlag, idAttacker, nParam, bTarget ); }	// 강공격

	void			Init();										// 객체 초기화 
	void			InitLevel( int nJob, LONG nLevel, BOOL bGamma = TRUE );			// 객체를 nLevel로 바꿔줌

	void			SetDefaultLowerBody(CModelObject* pModel);

	//////////////////////////////////////////////////////////////////////////
// #ifdef __WORLDSERVER
// public:
// 	void			CorrectErrorsSkillPoint();
// 	int				GetUseSkillPoint() const;
// #endif // __WORLDSERVER
	//////////////////////////////////////////////////////////////////////////

	void			ProcessAniSpeed();
	void			AllocShopInventory( LPCHARACTER pCharacter );
	BOOL			IsVendorNPC();
	void			CheckTickCheer();
	void			SetCheerParam( int nCheerPoint, DWORD dwTickCount, DWORD dwRest );
	void			ClearEquipInfo();
	int				GetMaxPoint(int nDest);
	DWORD			GetReuseDelay( int nIndex );
	BOOL			InitSkillExp();
	void			InitCharacter( LPCHARACTER lpCharacter );	// 이름 초기화 LoadDialog호출 
	LPCHARACTER		GetCharacter();								// 
	void			InitNPCProperty();							// NPC관련 스크립트 초기화 	
	BOOL			LoadDialog();								// dialog 스크립트 로드 
	void			ProcessRecovery();							// HP, MP, FP회복을 처리한다.
	BOOL			IsActiveMover() { return m_pObjActive == this; }	// 내가 주인공 객체인가?
	int				IsSteal( CMover *pTarget );		// pTarget을 스틸하려 하는가.
	int				IsSteal( OBJID idTaget );		// id로 검사하는 버전.
	u_long			GetPartyId() { return m_idparty; }
	BOOL			IsMode( DWORD dwMode ); 
	void			SetMode( DWORD dwMode )		{ m_dwMode |= dwMode; }	// 유저상태 셑팅
	void			SetNotMode( DWORD dwMode )	{ m_dwMode &= (~dwMode); } // 유저상태 리셑
	BOOL			SetDarkCover( BOOL bApply, DWORD tmMaxTime = 0 );
	BOOL			SetStun( BOOL bApply, DWORD tmMaxTime = 0 );
	BOOL			SetPoison( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 독에 걸리게 한다.
	BOOL			SetDark( BOOL bApply, DWORD tmMaxTime = 0, int nAdjHitRate = 0 );	// this를 암흑상태에 빠지게 한다 .
	BOOL			SetBleeding( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 출혈시킨다.
	void			RemoveDebuff( DWORD dwState );		// 디버프 해제
	void			RemoveBuffOne( DWORD dwSkill = 0 );		// 버프 하나 해제
	void			RemoveBuffAll();				// 버프 모두 해제.
	BOOL			IsStateMode( DWORD dwMode ); 
	void			SetStateMode( DWORD dwMode, BYTE nFlag );		// 유저상태 셑팅
	void			SetStateNotMode( DWORD dwMode, BYTE nFlag );	// 유저상태 리셑
	BOOL			IsUseItemReadyTime( PT_ITEM_SPEC pItemProp, OBJID dwObjItemId );
	BOOL			IsMonster();		// NPC & MONSTER
	BOOL			IsCitizen();		// NPC & !MONSTER
	BOOL			IsNPC()				{ return !m_bPlayer; }
	BOOL			IsPlayer() const			{ return m_bPlayer; }
	BOOL			IsEquipableNPC()	{	return( GetCharacter() && GetCharacter()->m_nEquipNum > 0 );	}
	BOOL			IsFlyingNPC()		{ return (m_dwTypeFlag & OBJTYPE_FLYING) ? TRUE : FALSE; }		// 비행형 몹인가.
	BOOL			IsFly();
	MoverProp*		GetProp() { return prj.GetMoverProp( m_dwIndex ); }	// 객체의 프로퍼티 얻기 	
	PT_ITEM_SPEC	GetActiveHandItemProp( int nParts = PARTS_RWEAPON );							// 손에 들려진 아이템의 프로퍼티를 얻기 
	PT_ITEM_SPEC	GetTransyItem( PT_ITEM_SPEC pItemProp, BOOL bCheck = FALSE, LPCTSTR lpszFileName = NULL );
	OBJID			GetDestId()			{ return m_idDest; }
	CCtrl*			GetDestObj()		{ return prj.GetCtrl( m_idDest ); }
	void			SetDestObj( CCtrl* pObj, float fRange = 0.0f );
	D3DXVECTOR3		GetDestPos()		{ return m_vDestPos; }
	void			SetDestPos( CONST D3DXVECTOR3& vDestPos, bool bForward = true, BOOL fTransfer = TRUE );
	void			SetDestPos( CShip *pIAObj, const D3DXVECTOR3 &vDestPos );
	
	void			ClearDestObj();
	void			ClearDestPos()		{ m_vDestPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); }		
	void			ClearDest();
	void			SetStop()			{ SendActMsg( OBJMSG_STAND ); ClearDestObj(); } // 멈춤 

	BOOL			IsEmptyDestObj()	{ return m_idDest == NULL_ID; }
	BOOL			IsEmptyDestPos()	{ return m_vDestPos.x == 0.0f && m_vDestPos.z == 0.0f; }
	BOOL			IsEmptyDest()		{ return IsEmptyDestPos() && IsEmptyDestObj(); }
	BOOL			IsRegionAttr( DWORD dwAttribite ) { return ( m_dwRegionAttr & dwAttribite ) == dwAttribite ? TRUE : FALSE; }
	REGIONELEM*		UpdateRegionAttr();
	//REGIONELEM*		UpdateRegion_ToIndun( const DWORD dwWorldID );
	


	DWORD			GetRideItemIdx()	{ return m_dwRideItemIdx; }
	void			SetRide( CModel *pModel, int nRideItemIdx = 0 ) { m_dwRideItemIdx = nRideItemIdx; m_pRide = (CModelObject*)pModel; 	};
	void			ClearDuel();
	void			ClearDuelParty();		
	int				SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0 ); 		
	void			SendAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	void			PostAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	BOOL			SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0 );	// MOP_SWDFORCE, MOP_FIXED, MOP_NO_TRANS, MOP_HITSLOW
	BOOL			InitMotion(DWORD dwMotion);
	void			SetDamagedMotion( CMover* pAttacker, DWORD dwAtkFlags );
	FLItemElem*		GetWeaponItem( int nParts = PARTS_RWEAPON );					// 장착한 무기 얻기 
	FLItemElem*		GetLWeaponItem();					// 왼손에 장착한 무기 얻기.
	FLItemElem*		GetEquipItem( int nParts );			// 장착한 아이템 얻기 
	PT_ITEM_SPEC	GetEquipItemProp( CItemContainer<FLItemElem>* pInventory, PEQUIP_INFO pEquipInfo, int nParts );
	BOOL			IsDualWeapon();
	void			RedoEquip( BOOL fFakeParts, BOOL bDestParam = TRUE );	
	void			UpdateParts( BOOL bFakeParts  = FALSE ); // normal or fake
	int				InvalidEquipOff( BOOL fFakeparts );		// 거시기한 장비를 벗김.
	BOOL			DoEquip( FLItemElem* pItemElem, BOOL bEquip, int nPart = -1 ); // for normal

	BOOL			IsEquipAble( FLItemElem* pItem,BOOL bIgnoreLevel = FALSE );						//sun: 11, 각성, 축복 // 장착가능한가?	

	BOOL			IsUnEquipAble( PT_ITEM_SPEC pItemProp );					// 벗는게 가능한가?
	void			SetEquipDstParam();
	void			SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray );
	int				SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax );
	void			SetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem, BOOL bIgnoreSetItem = FALSE );
	void			ResetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem );
	LPQUEST			FindQuest( int nQuestId );
	LPQUEST			GetQuest( int nQuestId );
	BOOL			RemoveQuest( int nQuestId );
	BOOL			IsCompleteQuest( int nQuestId );
	BOOL			MakeCompleteQuest( int nQuestId, LPQUEST lpQuest );
	BOOL			SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest );
	BOOL			SetQuest( LPQUEST lpQuest );
	void			RemoveAllQuest();
	void			RemoveCompleteQuest();

#ifdef	__WORLDSERVER
	DWORD			m_dwDisguiseID;
#endif
	BOOL            IsDisguise();
	BOOL			NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice = NULL );
	BOOL			Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex );
	BOOL			IsAuthorization( DWORD dwAuthorization ) { return dwAuthorization == m_dwAuthorization; }
	BOOL			IsAuthHigher( DWORD dwAuthorization ) { return dwAuthorization <= m_dwAuthorization; }
	void			UpdateParam();		
	int				GetParam( int nDestParameter, int nParam = 0 );
	void			SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL fSend = TRUE );
	void			SetDestParam( int nItemIdx2, BOOL fSend = TRUE );
	void			SetDestParam( int nIdx, SkillProp* pProp, BOOL bSend = TRUE );
	
	void			ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend = TRUE );

	void			ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL fSend = TRUE );

	void			ResetDestParam( int nIdx, SkillProp* pProp, BOOL bSend = TRUE );
	
	int				GetAdjParam( int nDestParameter );
	int				GetChgParam( int nDestParameter );
	int				GetPointParam( int nDstParameter );
	void			SetPointParam( int nDstParameter, int nValue, BOOL bTrans = FALSE ); // bTrans가 TRUE이면 강제전송
	void			OnApplySM();
	LPSKILL			GetSkill( int nType, int nIdx );
	LPSKILL			GetSkill( DWORD dwSkill );
	int				GetSkillIdx( DWORD dwSkill );
	BOOL			CheckSkill( DWORD dwSkill );
	bool			isLearnSkill( const int job_skill_array_index ) const;
	void			SetHair( int nHair );
	void			SetHairColor( DWORD dwHairColor );
	void			SetHairColor( FLOAT r, FLOAT g, FLOAT b );
	void			SetHead( int nHead );
	void			SetSkinSet( int nSkinSet );
	void			SetName( const char* lpszName )		{	FLStrcpy( m_szName, _countof( m_szName ), lpszName );	}
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
	int				GetExpPercent();
	int				SetLevel( int nSetLevel );
	int				AddGPPoint( int nAddGPPoint );
	void			SetJobLevel( int nLevel, int nJob );
	BOOL			IsJobType( DWORD dwJobType ); 
	int				GetLevel() const { return m_nLevel; }
	int				GetDeathLevel() const { return m_nDeathLevel; }
	int				GetFxp() { return m_nFxp; }
	int				GetTxp() { return m_nFxp; }
	EXPINTEGER		GetExp1()	{	return m_nExp1;	}
	EXPINTEGER		GetMaxExp1()	{	return prj.m_aExpCharacter[m_nLevel+1].nExp1;	}
	EXPINTEGER		GetMaxExp2()	{	return prj.m_aExpCharacter[m_nLevel].nExp2;	}
	int				GetRemainGP(); // growth
	void			IncHitPoint( int nVal) ;
	void			IncManaPoint(int nVal) ;
	void			IncFatiguePoint(int nVal) ;
	void			SetHitPoint( int nVal) ;
	void			SetManaPoint( int nVal ) ;
	void			SetFatiguePoint( int nVal ) ;
#ifdef __WORLDSERVER
	float			GetExpFactor();
#endif // __WORLDSERVER
	float			GetItemDropRateFactor( CMover* pAttacker );
	float			GetPieceItemDropRateFactor( CMover* pAttacker );
	BOOL			AddExperience( EXPINTEGER nExp, BOOL bFirstCall = TRUE, BOOL bMultiply = TRUE, BOOL bMonster = FALSE );	// bMultiply : 상용화 아이템 적용? // bMonster : 몬스터를 죽여서 온 경험치 인가?
//	BOOL			DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown );	// 경험치를 깎는다.
//	BOOL			DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown  );	// 경험치를 퍼센트로 깎는다.
	BOOL			AddFxp( int nFxp );
	BOOL			SetFxp( int nFxp, int nFlightLv );

	BOOL			IsPVPTarget( CMover* pMover );
	BOOL			IsWarTarget( CMover* pMover );
	BOOL			IsSchoolTarget( CMover* pMover );
	HITTYPE			GetHitType( CMover* pMover, BOOL bTarget, int nReflect );

	BOOL	IsArenaTarget( CMover* pMover );
	BOOL			IsEventArenaTarget( CMover* pMover );

//sun: 8, // __S8_PK
	HITTYPE			GetHitType2( CMover* pMover, BOOL bTarget, BOOL bGood );

	void			ReSetDuelTime( CMover* pAttacker , CMover* pDefender);
	int				GetHR();
	int				GetStr();
	int				GetDex();
	int				GetInt();
	int				GetSta();
	FLOAT			GetSpeed(FLOAT fSrcSpeed);    
	int				GetGold();

	bool			HasZeroSpeedStateParam_hardcoding();


	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100705
	bool			CheckUserGold( int nGold, bool bAdd );
	//////////////////////////////////////////////////////////////////////////


//sun: 12, 군주
	int		GetPerinNum( void );
	__int64		GetTotalGold( void );
#ifdef __WORLDSERVER
	int		RemoveTotalGold( __int64 iGold );	// 제거된 페린 개수
	int		RemovePerin( int nPerin );
#endif	// __WORLDSERVER

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
	float			GetCastingAniSpeed();
	int				GetCastingTime( int nCastingTime );

	//////////////////////////////////////////////////////////////////////////
	// 계산 공식
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
	int				GetCriticalProb();

	int				GetReqMp( int nReqMp );
	int				GetReqFp( int nReqFp );
	bool			isUnconditionedHit( CMover* pDefender ) const;
	int				calculateHitRate( CMover* pDefender );
	BOOL			GetAttackResult( CMover* pHitObj );
	int				GetAdjHitRate();
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
	void			GetHitMinMax( int* pnMin, int* pnMax, ATTACK_INFO *pInfo = NULL );
	BOOL			IsAfterDeath();
	BOOL			IsDie() const { return m_pActMover->IsDie() || m_nHitPoint == 0; }
	BOOL			IsLive() { return m_pActMover->IsDie() == FALSE || m_nHitPoint > 0; }		// && 를  ||로 바꿨음.  !=를 >로 바꿈
	int				GetCount() { return m_nCount; }
//sun: 8, // __S8_PK
	void			SetPKPink( DWORD dwTime ) { if( dwTime == 0 || m_dwPKTime < dwTime ) m_dwPKTime = dwTime; }
	DWORD			GetPKPink( void ) { return m_dwPKTime; }
	BOOL			IsPKPink( void )	{ return m_dwPKTime > 0; }			/// PK 핑크 모드인지
	BOOL			IsChaotic( void ) { return m_dwPKPropensity > 0; }	/// 카오인지
	void			SetPKValue( int nValue );
	int				GetPKValue( void ) { return m_nPKValue; }
	void			SetPKPropensity( DWORD dwValue );
	DWORD			GetPKPropensity( void ) { return m_dwPKPropensity; }
	DWORD			NextPKPropensity( int nPKValue );

	float			GetResist(SAI79::ePropType p_PropType);
	int				GetSetItemParts(DWORD dwParts);
	int				GetSetItemClient();
	int				GetSetItem( FLItemElem* pItemElem = NULL );
	BOOL			IsSetItemPart( DWORD dwParts );
	void			SetSetItemAvail( int nAbilityOption );
	void			ResetSetItemAvail( int nAbilityOption );
	void			DestParamPiercingAvail( FLItemElem* pItemElem, BOOL bSET = TRUE );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void			DestParamGemAvail( FLItemElem* pItemElem, BOOL bSET );
#endif
	BOOL			Pierce( FLItemElem *pItemElem, const DWORD dwPierceItemID );
	void			SetDestParamSetItem( FLItemElem* pItemElem );
	void			ResetDestParamSetItem( FLItemElem* pItemElem );
	int				GetEquipedSetItemNumber( CSetItem* pSetItem, FLItemElem* pUnequipItem = NULL );
	void			GetEquipedSetItem( CSetItem* pSetItem, int* pbEquipedCount,  int* pnTotal, int* pnEquip );
	void			SetDestParamRandomOpt( FLItemElem* pItemElem );
	void			ResetDestParamRandomOpt( FLItemElem* pItemElem );
	
	void	SetDestParamRandomOptOrigin( FLItemElem* pItemElem );
	void	SetDestParamRandomOptExtension( FLItemElem* pItemElem );
	void	ResetDestParamRandomOptOrigin( FLItemElem* pItemElem );
	void	ResetDestParamRandomOptExtension( FLItemElem* pItemElem );

	void			GetItemATKPower( int *pnMin, int *pnMax, PT_ITEM_SPEC pItemProp, FLItemElem *pWeapon ); 
	float			GetItemMultiplier( FLItemElem* pItemElem );
	BOOL			SubLootDropNotMob( CItem *pItem );
	BOOL			SubLootDropMobSingle( CItem *pItem );
	BOOL			SubLootDropMobParty( CItem *pItem, CParty *pParty );
	BOOL			SubLootDropMob( CItem *pItem );
	BOOL			DoLoot( CItem *pItem );		// 바닥에 떨어진 pItem을 줍는다.
	void			PickupGoldCore( int nGold );
	void			PickupGold( int nGold, BOOL bDropMob );
	BOOL			IsDropable( FLItemElem* pItemElem, BOOL bPK );
 	BOOL			IsDropableState( BOOL bPK );
	CItem*			DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK = FALSE );

	void			UpdateItem( DWORD dwId, CHAR cParam, DWORD dwValue, DWORD dwTime = 0 );
//	void			UpdateItemEx( unsigned char id, char cParam, __int64 iValue );		//sun: 11, 각성, 축복

	CItem*			_DropItemNPC( DWORD dwItemType, DWORD dwID, int nDropNum, const D3DXVECTOR3 &vPos );
	CItem*			DropItem( DWORD dwID, int nDropNum, const D3DXVECTOR3 &vPos, BOOL bPlayer = FALSE );
	int				DoDropItemRandom( BOOL bExcludeEquip, CMover* pAttacker, BOOL bOnlyEquip = FALSE );
	int				GetItemNum( DWORD dwItemId );
#ifdef __CLIENT
	int				GetItemNumForClient( DWORD dwItemId ); // Client에서만 사용하는 아이템 갯수 구하기(Null check 이외의 Usable check안함)
#endif //__CLIENT
	int				RemoveAllItem( DWORD dwItemId );
	BOOL			AddItem( FLItemBase* pItemBase );
	FLItemBase*		GetItemId( DWORD dwId );
	void			RemoveItemId( DWORD dwId  );
	void			SetKeeptimeInven( DWORD dwItemId, DWORD dwTime );
	void			SetKeeptimeBank( DWORD dwItemId, DWORD dwTime );
	void			SetKeeptimeItem( FLItemElem* pItemElem, DWORD dwTime );
	void			OnTradeRemoveUser();
	BOOL			AddItemBank( int nSlot, FLItemElem* pItemElem );
	void			UpdateItemBank( int nSlot, DWORD dwItemObjID, CHAR cParam, DWORD dwValue );
	FLItemBase*		GetItemBankId( int nSlot, DWORD dwId );
	void			RemoveItemBankId( int nSlot, DWORD dwId );
	void			GenerateVendorItem( T_ITEM_SPEC** pItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor );
	BOOL			DropItemByDied( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
	BOOL			OnDie( CMover & kAttacker );

	void			SetPKTargetLimit( int nSec );
	void			ClearCmd(); 
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
	BOOL			IsAttackAbleNPC( CMover* pNPC );
	BOOL			IsPKInspection( CMover* pMover );
	int				IsPKPVPInspectionBase( DWORD dwRegionAttr, BOOL bPVP = TRUE );
//sun: 8,     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	int				IsPVPInspectionBase( DWORD dwRegionAttr,DWORD dwWorldID, BOOL bPVP = TRUE );//8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World

	BOOL			SubPKPVPInspectionBase( CMover* pMover, CMover* pMover1, DWORD dwPKAttr, int nFlag );
	void			PrintString( CMover* pMover, DWORD dwId );
	int				CMD_SetUseSkill( OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType = SUT_NORMAL );
	void			CMD_SetMeleeAttack( OBJID idTarget, FLOAT fRange = 0.0f );
	void			CMD_SetMagicAttack( OBJID idTarget, int nMagicPower );
	void			CMD_SetRangeAttack( OBJID idTarget, int nPower );
	void			CMD_SetMoveToPos( const D3DXVECTOR3 &vPos );
	void			CMD_SetMoveToObj( OBJID idObj );
	void			CMD_SetUseItem( CCtrl *pCtrl );
//	void			CMD_SetCollect( CObj *pObj );
	void			OnAfterUseItem( const PT_ITEM_SPEC pItemProp );
	void			UnequipRide();		
	void			EquipItem( FLItemElem *pItemElem, BOOL bEquip, int nPart );
	BOOL			DoUseItemVirtual( DWORD dwItemId, BOOL bEffectSkip );

//	int				GetCountApplyESScroll();
//	bool			HasBuffESScroll();

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
	void			SubPVP( CMover *pAttacker, int nReflect );	// 사람과 사람의 싸움에서 죽었을때.
	int				SubDuel( CMover *pAttacker );	// 듀얼중에 죽었을때 
	int				SubPK( CMover *pAttacker, int nReflect );		// PK에의해 죽었을때.
	int				SubWar( CMover *pAttacker );	// 전쟁때 죽었음
	int				SubExperience( CMover *pDead );
	void			AddKillRecovery();
	BOOL			IsValidArea( CMover* pMover, float fLength );
	void			SetJJim( CMover *pJJimer );		// this를 pJJimer가 찜했다는걸 표시.
	void			SubReferTime( DWORD *pTime1, DWORD *pTime2, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp );

	BOOL			GetSkillProp( SkillProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	void			RemoveInvisible();		// 투명화 상태걸려있으면 해제함.
	void			SendDamageAround( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio );	// this를 중심으로 주변에 데미지를 입힘.
	void			SendDamageAroundByItem( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio );
	void			ChangeExpRatio( CMover* pAttacker, CMover* pDefender );
	void			OnAttacked( CMover* pAttacker, int nDamage, BOOL bTarget, int nReflect );
	BOOL			OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos = NULL, DWORD dwAtkFlag = AF_GENERIC );

	void			SetCoolTime( AddSkillProp* pAddSkillProp, LPCTSTR szCall );	//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용

	BOOL			OnAttackRange();		// 장거리무기가 발사되는 시점에 발생하는 이벤트
	BOOL			OnAttackMelee( DWORD dwState, CMover *pHitObj );		// 근거리공격시 타점이 되었을때 발생하는 이벤트.
	BOOL			OnAttackMeleeContinue( DWORD dwState );		// 첫번째 타점이후부터 애니끝날때까지 계속.
	void			OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj );
	void			OnAttackMelee_BigMuscle( DWORD dwState, CMover *pHitObj );	
	void			OnAttackMelee_Krrr( DWORD dwState, CMover *pHitObj );	
	void			OnAttackMelee_Bear( DWORD dwState, CMover *pHitObj );	
	void			OnAttackMelee_Meteonyker( DWORD dwState, CMover *pHitObj );	//sun: 메테오니커 파이어 발사!!!
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

	BOOL			IsBullet( PT_ITEM_SPEC pItemProp );
	BOOL			IsBullet( SkillProp* pSkillProp );
	void			ProcessMovePattern();
	int				GetMovePattern() { return m_nMovePattern; }
	int				GetMoveEvent() { return m_nMoveEvent; }
	void			SetMovePattern( int nPattern );		// 이동패턴 설정.
//	BOOL			DoCollect( CMover *pTarget );
	CGuild*			GetGuild();
	CGuildWar*		GetWar();
	BOOL			IsSMMode( int nType ) { return ( m_dwSMTime[nType] > 0 ) ? TRUE : FALSE; }
	void			ReState();
	void			ReStateOne( int nKind );	//sun: 10, __S_ADD_RESTATE

	void			ReStateOneLow( int nKind );

	float			GetJobPropFactor( JOB_PROP_TYPE type );
#ifdef ADD_CHARACTER_INFO_DISPLAY
	int				CalcDefense( ATTACK_INFO* pInfo, BOOL bRandom = TRUE, BOOL bItemDivision = TRUE );
	int				CalcDefenseCore( CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom = TRUE, BOOL bItemDivision = TRUE);	
#else
	int				CalcDefense( ATTACK_INFO* pInfo, BOOL bRandom = TRUE );
	int				CalcDefenseCore( CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom = TRUE );
#endif
	int				CalcDefensePlayer( CMover* pAttacker, DWORD dwAtkFlags );
	int				CalcDefenseNPC( CMover* pAttacker, DWORD dwAtkFlags );
	void			PutLvUpSkillName_1( DWORD dwLevel );
	void			PutLvUpSkillName_2( DWORD dwSkill );
	BOOL			IsRegionMove( DWORD dwOlgRegionAttr, DWORD dwRegionAttr );
	DWORD			GetPKPVPRegionAttr();
	FLItemBase*		GetVendorItem();		
	BOOL			IsAttackMode();
	void			AddSkillPoint( int nPoint);
	
//sun: 8,     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	void			DoPVPEnd( CCtrl *pAttackCtrl, bool bWinner , DWORD dwMsg = 0 );	// 듀얼끝난뒤 처리
	void			EndPVP(int	nPVPHP);	// 듀얼끝난뒤 처리
	BOOL			m_bPVPEnd;			//듀얼이 끝났는가

//sun: 10차 전승시스템	Neuz, World, Trans
	BYTE			GetLegendChar()	;
	void			SetLegendChar(	BYTE	nLegend	)	{	m_nLegend	=	nLegend;	}
	BOOL			IsMaster();
	BOOL			IsHero();
	BOOL			IsLegendClass();	// 히어로 이상이냐..

	BOOL			IsLegendHero();

	void			SetMasterSkillPointUp();


	DWORD			GetJobType( int nJob = NULL_ID )	{	if( nJob == NULL_ID ) nJob = m_nJob;	return prj.m_aJob[ nJob ].dwJobType;	}	// 직업 타입

	void			AngelMoveProcess();		//sun: 8차 엔젤 소환 Neuz, World, Trans
	void			BalloonMoveProcess();	//sun:10, __EVE_BALLOON

//sun: 8, // __S8_PK
	void			OnAttackSFX( OBJID	idTarget, int nMagicPower, DWORD dwSkill, int nDmgCnt, float	fDmgAngle, float fDmgPower,  DWORD dwAtkFlags, BOOL bControl );

	BOOL			IsGuildCombatTarget( CMover* pMover );
#ifdef __WORLDSERVER
	void	ProcessSFXDamage( void );
	void	ProcessSFXExpire( void );
#endif	// __WORLDSERVER

	void	ProcessPet( void );		//sun: 9, 9-10차 펫

#ifdef __WORLDSERVER
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = FALSE );
#else
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = TRUE );
#endif

#if defined(__WORLDSERVER)
	BOOL			IsPVPInspection( CMover* pMover, int nFlag = 0 );
	void			AddExperienceKillMember( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue );
	void			AddExperienceSolo( EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, BOOL bParty );
	void			AddExperienceParty( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, FLWSUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	void			AddExperiencePartyContribution( CMover *pDead, FLWSUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	void			AddExperiencePartyLevel( FLWSUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	BOOL			GetPartyMemberFind( CParty* pParty, FLWSUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	float			GetExperienceReduceFactor( int nLevel, int nMaxLevel );
//	BOOL			IsResourceMonster() { return m_nResource != -1; }		// 자원몬스터냐? -1이면 자원몬스터가 아니다. 0 ~ 자원몬스터라는 뜻.
	void			ArrowDown( int nCount );
	int				GetQueueCastingTime();

	DWORD			DoUseSkill( const int nType, const int nIdx, const OBJID idFocusObj, const SKILLUSETYPE sutType, const BOOL bControl );
//	float			SubDieDecExp( BOOL bTransfer = TRUE, DWORD dwDestParam = 0, BOOL bResurrection = FALSE  );	// 죽었을때 겸치 깎는 부분.

	void			SubAroundExp( CMover *pAttacker, float fRange );		// this를 중심으로 fRange범위안에 있는 유저에게 경험치를 배분한다.
	void			AddPartyMemberExperience( FLWSUser * pUser, EXPINTEGER nExp, int nFxp );
//	void			GetDieDecExp( int nLevel, FLOAT& fRate, FLOAT& fDecExp, BOOL& bPxpClear, BOOL& bLvDown );

//	void			GetDieDecExpRate( FLOAT& fDecExp, DWORD dwDestParam, BOOL bResurrection );

	BOOL			CreateItem( FLItemBase* pItemBase, DWORD* pItemObjID = NULL, int* pQuantity = NULL, DWORD* pCount = NULL );
	void			RemoveItem( DWORD dwItemObjID, int nNum );
	int				RemoveItemByItemID( const DWORD dwItemID, const int nTotalQuantity, const TCHAR* pszLogAction = NULL, const TCHAR* pszLogContext = NULL );
	int				RemoveItemA( DWORD dwItemId, int nNum );
	void			RemoveVendorItem( CHAR chTab, DWORD dwItemObjID, int nNum );
	void			RemoveItemIK3( DWORD dwItemKind3 );
	BOOL			DoUseItemSexChange( int nFace );
	BOOL			ReplaceInspection( REGIONELEM* pPortkey );
#ifdef __LAYER_1015
	BOOL			Replace( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3& vPos, REPLACE_TYPE type, int nLayer );
#else	// __LAYER_1015
	BOOL			Replace( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3& vPos, REPLACE_TYPE type );
#endif	// __LAYER_1015

	BOOL			IsLoot( CItem *pItem, BOOL bPet = FALSE );
	void			ProcInstantBanker();
	void			ProcInstantGC();
	BOOL			IsItemRedyTime( PT_ITEM_SPEC pItemProp, OBJID dwObjid, BOOL bItemFind );
	int				ApplyDPC( int ATK, ATTACK_INFO* pInfo );
	int				CalcLinkAttackDamage( int nDamage ); // 파티스킬:링크어택의 추가 데이미계산 	
	int				CalcPropDamage( CMover* pDefender, DWORD dwAtkFlags );
	int				CalcGenericDamage( CMover* pDefender, DWORD& dwAtkFlags );
	int				GetMagicSkillPower( ATTACK_INFO* pInfo );
	void			SubSMMode();
	void			ClearAllSMMode();
	BOOL			SetSMMode( int nType, DWORD dwTime );
	void			Abrade( CMover* pAttacker, int nParts = PARTS_RWEAPON );
	DWORD			GetRandomPartsAbraded();
	void			SetMarkingPos();
	void			RemoveSFX( OBJID idTarget, int id, BOOL bForce, DWORD dwSkill );
	int				GetSFXCount( OBJID idTarget );
	void			IncSFXCount( OBJID idTarget, DWORD dwSkill );
	void			AddSFXInfo( OBJID idTarget, SFXHIT_INFO& info );
	void			ClearSFX( OBJID idTarget );
	int				GetSummonState();
	BOOL			IsDoUseBuff( PT_ITEM_SPEC pItemProp );
#ifdef __AGGRO16
	FLAggro *		GetAggroMng();
#endif //__AGGRO16

	void			ClearAbnormalState();

	
#endif // __WORLDSERVER

#ifdef __CLIENT
	virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice );

	BOOL			IsStateDbuff();
	BOOL			EndMotion();
	BOOL			IsPKAttackAble( CMover* pMover );
	BOOL			IsPVPAttackAble( CMover* pMover );
	CSfx*			CreateSfxArrow( DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget );
	void			SetDmgCnt( int nDmgCnt ) { m_nDmgCnt = nDmgCnt; }

	void			GetItemSFXIndex( IN const int nAbilityOption, OUT DWORD& dwSFXIndex, OUT DWORD& dwSFXHandsIndex );
	void			CreateAbilityOption_SetItemSFX( int nAbilityOption );
	BOOL			GetEquipFlag( int nParts, BYTE* pbyFlag );
	void			OverCoatItemRenderCheck(CModelObject* pModel);
	void			PlayCombatMusic();
	BOOL			IsLoot( CItem *pItem ) { return TRUE; }
	LPCTSTR			GetFameName();						// 명성 이름 얻기

	LPCTSTR			GetJobString();						// 직업 이름 얻기 
	void			DialogOut( LPCTSTR lpszText );		// 말풍선에 의한 대사 출력
	BOOL			DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel = NULL ); // for Fake
	void			RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, int nValue );
	void			RenderTurboGauge( LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue );
	void			SetRenderPartsEffect( int nParts );
	void			RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	void			RenderHP(LPDIRECT3DDEVICE9 pd3dDevice);

	void			RenderCltGauge(LPDIRECT3DDEVICE9 pd3dDevice);	//sun: 11, 채집 시스템

	void			RenderChrState(LPDIRECT3DDEVICE9 pd3dDevice);
	void			RenderFlag( int nType );
	void			RenderCasting( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderCtrlCasting( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderSkillCasting( LPDIRECT3DDEVICE9 pd3dDevice );		//sun: 10차 전승시스템	Neuz, World, Trans

	void			RenderPVPCount( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderQuestEmoticon( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderGuildNameLogo( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor );
	void			RenderNameBox( LPDIRECT3DDEVICE9 pd3dDevice, const CRect& rect );
//sun: 8, // __S8_PK
	DWORD			DoUseSkill( const int nType, const int nIdx, const OBJID idFocusObj, const SKILLUSETYPE sutType, const BOOL bControl, const int nCastingTime, DWORD dwSkill = 0, DWORD dwLevel = 0 );

	void			RenderAngelStatus(LPDIRECT3DDEVICE9 pd3dDevice);		//sun: 8차 엔젤 소환 Neuz, World, Trans

#endif	// __CLIENT

	int				GetSkillLevel( SKILL* pSkill );
	BOOL			SetExperience( EXPINTEGER nExp1, int nLevel );

	BOOL	IsUsing( FLItemElem* pItemElem );

	BOOL	IsShoutFull( void );
	BOOL	IsCommBank( void );

	//////////////////////////////////////////////////////////////////////////
	void			SetActParam( OBJACT act, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0, int nParam5 = 0 );
	int				GetActParam( int nIdx ) { return m_nAParam[nIdx]; }
	//////////////////////////////////////////////////////////////////////////

private:
	void			ProcessMove();
	void			ProcessMoveArrival( CCtrl *pObj );
	void			ProcessRegenItem();
	void			ProcessRegion();
	void			ProcessQuest();
	void			ProcessIAObjLink( D3DXVECTOR3& vPos);
	void			ProcessActionPoint();
#ifdef __CLIENT
	void			ProcessScaleSlerp();
	void			ProcessWaterCircle( const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vPos );
	void			ProcessWalkSound();
	void			ProcessDustSFX();
	void			ProcessETC();
#else
	void			ProcessScript();
	void			ProcessAbnormalState();
	void			ProcessTarget();
#endif // __CLIENT

#ifdef __WORLDSERVER
	bool			IsForceItemDropMonster( CMover* pAttacker );
#endif // __WORLDSERVER

	BOOL			DropItem( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.

	BOOL			DropItem( CMover & kAttacker );

	void			OnArriveAtPos();
	void			OnArrive( DWORD dwParam1, DWORD dwParam2 );

	void			SetCmd( OBJACT cmd, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	void			SetCmdParam( int nIdx, int nValue ) { m_nCParam[nIdx] = nValue; }
	BOOL			__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest );

public:
#ifdef __CLIENT

	void			CreateAngelParticle(D3DXVECTOR3 vPos);		//sun: 8차 엔젤 소환 Neuz, World, Trans
	void			CreatePetParticle(D3DXVECTOR3 vPos);		//sun: 9, 9-10차 펫

	void			ProcessEyeFlash();
	void			WingMotionSetting( const CModelObject* pModel );
	float			GetRideFrameSpeed( void );

#ifdef __BS_EFFECT_LUA
	const char*		GetNameO3D( );
	BOOL			SetDataMTE( const char* fname1, const char* fname2 );
#endif

	void			SetAngle( FLOAT fAngle );

#ifdef _DEBUG
	//신제련 test
	BOOL			CreateEnchantEffect_NEW( const int nKind, const TCHAR* fname );
	CModelObject*	DeleteEnchantEffect_NEW( const int nKind );
#endif //_DEBUG

#endif //__CLIENT

private:	

	OBJID			m_dwRegisterElldinsJarID;	


public:		

	void			SetRegisterElldinsJarID( OBJID dwID )	{	m_dwRegisterElldinsJarID = dwID;	}
	OBJID			GetRegisterElldinsJarID( void )			{	return m_dwRegisterElldinsJarID;	}


private:
	DWORD	m_dwSleepingDuration;

public:
	bool	SetSleeping( bool bApply, DWORD dwDuration = 0 );

	void	SetSleepingDuration( DWORD dwSleepingDuration )	{	m_dwSleepingDuration = dwSleepingDuration;	}

	bool	IsNoActionState();

	int		GetUsableItemQuantityByItemID( const DWORD dwItemID ) const;

	DWORD	CanSellItemToNPC( const DWORD dwItemObjID, const int nSellQuantity );

	bool	GetUnitCostSellItemToNPC( int & nSellCost, const DWORD dwItemObjID );

#ifdef __CLIENT
	//Event Arena
	int		m_nEATeam;		// 관전자, 레드팀, 블루팀 : 이벤트아레나 월드에선 이값을 참조해서 피아식별 렌더링을 수행
#endif //__CLIENT

	bool	IsAbnormalState( const int nCHS );

	bool	CheckAttackDistance( CObj* pTarget, const DWORD AttackRangeType );

	MEMPOOLER_DECLARE( CMover );


#ifdef __CLIENT
	int		GetSetItemSFX_EffectValue( void );
#endif
private:
	AutoSynchronizer	m_autoSynchronizer;
public:
	AutoSynchronizer*	AutoSynchronizer()	{	return &m_autoSynchronizer;	}
	virtual D3DXVECTOR3		GetScrPos()		{	return AutoSynchronizer()->GetSyncPos();	}
	float	GetScrAngle()	{	return AutoSynchronizer()->GetSyncAngle();	}
	float	GetScrAngleX()	{	return AutoSynchronizer()->GetSyncAngleX();	}
#ifdef __WORLDSERVER
	void	ProcessMoveArrivalBySynchronizer();
#endif // __WORLDSERVER


	/************************************************************************/
#ifdef __WORLDSERVER
	private:
		virtual float	GetRateRecoveryRevival()	{	return 0.0f;	}
		virtual bool	GetDecreaseExperienceRevival( nsRevival::DECREASE_EXP_DATA& /*decreaseExpData*/ ) const	{	return false;	}
		virtual void	DoRecoveryRevival( const float /*recoveryRate*/ ) {	return;	}
		virtual bool	DoDecreaseExperience( const nsRevival::DECREASE_EXP_DATA& /*decreaseExpData*/, bool& /*downLevel*/ )	{	return false;	}

		void			DecreaseExperience( const bool Transfer, const bool Resurrection );
		void			RecoveryRevival();

		bool			m_decreaseExperience;

	public:
		void			Revival( const bool bTransfer = true, const bool bResurrection = false );
		bool			CanRevival() const;
		
		void			SetDecreaseExperienceRevival( const bool decreaseExperience )	{	m_decreaseExperience = decreaseExperience;	}
		bool			CanDecreaseExperienceRevival() const {	return m_decreaseExperience;	}

		bool			pushedByAnotherPower( const float angle, const float power );
#endif // __WORLDSERVER
	/************************************************************************/

public:
#ifdef PAT_LOOTOPTION
	int m_nPetLootType;		// 줍기펫의 주울 타입을 정함.
#endif // PAT_LOOTOPTION
#ifdef SCRIPT_ENTER_INSTANCEDUNGEON
	bool				EnterInstanceDungeon( DWORD dwTeleWorld, int nX, int nY, int nZ );
#endif	// SCRIPT_ENTER_INSTANCEDUNGEON
};

// 유저상태 알아봄
inline	BOOL CMover::IsMode( DWORD dwMode ) 
{ 
	switch( dwMode )
	{
		case MATCHLESS_MODE:		// 무적 상태
		case ONEKILL_MODE:		// 초필 상태
		case MATCHLESS2_MODE:	// 무적 상태2
			if( m_dwAuthorization == AUTH_GENERAL )
				return FALSE;
			break;
	}
	if( (dwMode & TRANSPARENT_MODE) )	// 투명화를 검사하러 들어왔을때
		if( GetAdjParam(DST_CHRSTATE) & CHS_INVISIBILITY )	// 마법투명화가 걸렸는가?
			return TRUE;
	return ( ( m_dwMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	

inline	BOOL CMover::IsStateMode( DWORD dwMode ) 
{ 
	return ( ( m_dwStateMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	

inline BOOL CMover::IsFly() 
{ 
	if( IsPlayer() )
	{
		return m_pActMover->IsFly();
	}
	else
	{
		return IsFlyingNPC();
	}
}

inline int	CMover::DoAttackMelee( OBJID idTarget, OBJMSG dwMsg, DWORD dwItemID )
{
	CMover *pTarget = (CMover *)prj.GetMover( idTarget );
	if( IsInvalidObj(pTarget) )
		return 0;

	return DoAttackMelee( pTarget, dwMsg, dwItemID );
}

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

// 액션 매시지를 보냄 ; 객체의 움직임을 매시지로 제어함 
inline int CMover::SendActMsg( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 ) 	
{	
	if( m_pActMover )	
		return m_pActMover->SendActMsg( dwMsg, nParam1, nParam2, nParam3, nParam4 );	
	else
		return 0;
}

//raiders.2006.11.28	 trade돈을 계산에 포함하던 것을 제거
inline int CMover::GetGold()
{
	int nGold = m_dwGold;
//	nGold -= m_vtInfo.TradeGetGold();
	if( nGold < 0 || nGold > INT_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid. UserId:[%07d], Gold:[%d]" ), m_idPlayer, m_dwGold );
	}
	return nGold;
}

#ifndef __CLIENT //_SUN_CHECKDATA_
inline void CMover::SetGold( int nGold )
{
	if( nGold < 0 || nGold > INT_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid. UserId:[%07d], Gold:[%d]" ), m_idPlayer, nGold );
		return;
	}
	m_dwGold = (DWORD)nGold;
}
#endif 

inline void CMover::ClearCmd() 
{ 
//	FLTRACE_LOG( PROGRAM_NAME, _T( "ClearCmd %d->0" ), m_oaCmd );
	m_oaCmd = OBJACT_NONE; 
//	memset( m_nCParam, 0xcd, sizeof(m_nCParam) );	// 0xcd??
	memset( m_nCParam, 0, sizeof(m_nCParam) );
}

inline void CMover::ClearActParam() 
{ 
	m_oaAct = OBJACT_NONE; 
//	memset( m_nAParam, 0xcd, sizeof(m_nAParam) );	// 0xcd??
	memset( m_nAParam, 0, sizeof(m_nAParam) );
}


extern int GetWeaponPlusDamage( int nDamage, BOOL bRandom, PT_ITEM_SPEC pItemProp , int nOption );
extern bool __IsEndQuestCondition( CMover* pMover, int nQuestId );

extern bool __IsBeginQuestCondition( CMover* pMover, int nQuestId );

extern bool __IsNextLevelQuest( CMover* pMover, int nQuestId );
extern	BOOL AttackBySFX( CMover* pAttacker, SFXHIT_INFO & info );

#endif // !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)