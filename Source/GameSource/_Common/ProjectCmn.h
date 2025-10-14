#ifndef __PROJECTCMN_H__
#define	__PROJECTCMN_H__

#define	MAX_OBJARRAY			8
#define	MAX_QUICKSLOT			21
#define	MAX_EQUIPMENT			18

#define	MAX_INVENTORY_OLD		42
#ifdef INVENTORY_GENERAL54
#define MAX_INVENTORY_GENERAL	54
#else 
#define MAX_INVENTORY_GENERAL	42
#endif	// INVENTORY_GENERAL54
#ifdef INVENTORY_PET_COSTUME42
#define MAX_INVENTORY_PET		42
#define MAX_INVENTORY_COSTUME	42
#else
#define MAX_INVENTORY_PET		24
#define MAX_INVENTORY_COSTUME	24
#endif
#define MAX_INVENTORY_QUEST		24

#define MAX_INVENINDEX_GENERAL		( MAX_HUMAN_PARTS + MAX_INVENTORY_GENERAL )
#define MAX_INVENINDEX_PET			( MAX_HUMAN_PARTS + MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET )
#define MAX_INVENINDEX_COSTUME		( MAX_HUMAN_PARTS + MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET + MAX_INVENTORY_COSTUME )
#define MAX_INVENINDEX_QUEST		( MAX_HUMAN_PARTS + MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET + MAX_INVENTORY_COSTUME + MAX_INVENTORY_QUEST )

#if __VER >= 17

#define	MAX_INVENTORY			( MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET + MAX_INVENTORY_COSTUME + MAX_INVENTORY_QUEST )

#else // __VER >= 17

#define	MAX_INVENTORY			MAX_INVENTORY_OLD

#endif // __VER >= 17


#define MAX_HUMAN_PARTS_OLD		31	// 기존 파츠 사이즈..
#define MAX_HUMAN_PARTS_ADD		19	// 인벤토리 확장이 되면서 차후 다시 확장이 용이하게 하기위해 파츠사이즈를 증가..

#define	MAX_HUMAN_PARTS         ( MAX_HUMAN_PARTS_OLD + MAX_HUMAN_PARTS_ADD )



#define MAX_GUILDBANK			42
#define	MAX_BANK				42
#define	MAX_DEFAULT_POCKET		6
#define	MAX_EXTENSION_POCKET	24
#define	MAX_REPAIR				16
//#define	MAX_TRADE				25
const	BYTE MAX_TRADE			=	25;
#define	MAX_VENDITEM			30
#define	MAX_REPAIRINGITEM		25
#define	MAX_VENDORNAME			48
#define	MAX_STUFF				( MAX_INVENTORY + MAX_QUICKSLOT )
#define	MAX_EXPLPPOINT			41
#define	MAX_EXPSKILL			21
#define	MAX_EXPCHARACTER		200
#define	MAX_FXPCHARACTER		200
#define	MAX_VENDOR_INVENTORY	100
#define	MAX_VENDOR_INVENTORY_TAB 4

//#define	MAX_QUEST				100
//#define	MAX_COMPLETE_QUEST		300 
//#define 	MAX_CHECKED_QUEST		5

const size_t	MAX_QUEST				= 100;
const size_t	MAX_DOING_QUEST			= 60;
const size_t	MAX_COMPLETE_QUEST		= 500;
const size_t	MAX_CHECKED_QUEST		= 5;
const size_t	MAX_QUEST_INDEX			= 9999;

#define MAX_VENDOR_NEW_INVENTAB 4


#define MAX_PARTYLEVEL			10
#define MAX_ADDEXPPARTY			16
#define MAX_RENEWEVENT			8000
#define MAX_REALITEM			8			// 이벤트 상품 아이템
#define MAX_EVENTREALITEM		4096
#define MAX_EXPJOBLEVEL			90
#define	ITEM_KIND_MAX			100

//#define	MAX_SKILL_PARAM			2
//#define	MAX_ITEM_PARAM			3

const int	MAX_PROPMOVER =		2000;		// MoverProp배열의 최대갯수 

#define	MAX_HONOR_TITLE		150	// 달인타이틀 제한

#define MAX_VENDOR_REVISION		20			//sun: 8, // __S8_VENDOR_REVISION
#define MAX_SHOP_BASESLOT		24

// #define dwDestParam1	dwDestParam[0]
// #define dwDestParam2	dwDestParam[1]
// #define nAdjParamVal1	nAdjParamVal[0]
// #define nAdjParamVal2	nAdjParamVal[1]
// #define dwChgParamVal1	dwChgParamVal[0]
// #define dwChgParamVal2	dwChgParamVal[1]
// 
// //sun: 10, __PROP_0827
// #define dwDestParam3	dwDestParam[2]
// #define nAdjParamVal3	nAdjParamVal[2]
// #define dwChgParamVal3	dwChgParamVal[2]

#include <defineitemkind.h>		//sun: 9,
#ifdef __DBSERVER
#include "definejob.h"
#endif


struct vector3 
{
	float x;
	float y; 
	float z;

	vector3() {
		x = .0f;
		y = .0f;
		z = .0f;
	}
	vector3(const float&& xx, const float&& yy, const float&&zz) 
	{
		x = xx; 
		y = yy; 
		z = zz; 
	}
};


struct tagColorText
{
	DWORD dwColor;
	TCHAR * lpszData;
}; 

struct ObjProp
{
	DWORD	dwID;			// 변수명	
	TCHAR	szName[64];	// 한글명칭 
	DWORD	dwType; 
	DWORD	dwAI;			// AIInterface
	DWORD	dwHP;
	ObjProp()
	{
		szName[0]	= '\0';
		dwID	= dwType	= dwAI	= dwHP	= 0;
	}
};

struct CtrlProp : ObjProp
{
	DWORD   dwCtrlKind1;
	DWORD   dwCtrlKind2;
	DWORD   dwCtrlKind3;
	DWORD   dwSfxCtrl;
	DWORD   dwSndDamage;
	CtrlProp() : ObjProp()
	{
		dwCtrlKind1	= dwCtrlKind2	= dwCtrlKind3	= dwSfxCtrl	= dwSndDamage	= 0;
	}

	BOOL IsGuildHousingObj( )	{ return CK1_GUILD_HOUSE == dwCtrlKind1; } //길드 하우징 전용 오브젝트 인가?
	BOOL IsHousingObj( )		{ return CK1_HOUSING == dwCtrlKind1; }		//개인 하우징 전용 오브젝트 

};

// TODO SkillLevelProp로 이름을 바꾸는 것이 나을 것같다.
struct AddSkillProp
{
	// TODO 대인 능력치가 필요한가?
	// 통일 할 수 있으면 통일하자.
	DWORD	dwID;							// ID
	DWORD	dwSkillPropID;					// 스킬Prop ID

	DWORD	dwSkillLvl;						// 스킬레벨

	DWORD	dwAbilityMin;					// 최소능력
	DWORD	dwAbilityMax;					// 최대능력

	DWORD	dwDmgShift;						// 타격시 반동
	DWORD	dwProbability;					// 발동 확률

	DWORD	dwDestParam[MAX_SKILL_PARAM];	// 적용대상1
	DWORD	dwAdjParamVal[MAX_SKILL_PARAM];	// 적용값1;
	DWORD	dwChgParamVal[MAX_SKILL_PARAM];	// 적용변화값1
	int		arrAddSkillDestNumData1[3];			// 적용데이타3개, DestParam[0]에 해당.TODO 이건 왜 3개?


	DWORD	dwActiveSkill;					// 발동 마법(어떤상황에서 이값을 참조하는지 모호하다.?)
	DWORD	dwActiveSkillRate;				// 발동 마법 확률.
	DWORD	dwActiveSkillRatePVP;			// 발동 마법 확률(대인)

	int		nReqMp;							// 필요MP
	int		nReqFp;							// 필요FP

	DWORD	dwCooldown;						// 쿨다운
	DWORD	dwCastingTime;					// 기술 준비시간
	DWORD	dwSkillRange;					// 기술 시전거리	

	DWORD	dwCircleTime;					// 유지시간(이해할 수 없다.)
	DWORD   dwPainTime;						// 지속 피해시간
	DWORD	dwSkillTime;					// 지속시간

	int		nSkillCount;					// 발사체 발사 개수. 개당공격력 = 총공격력 / nSkillCount;

	DWORD	dwAbilityMinPVP;				// 최소능력(대인)
	DWORD	dwAbilityMaxPVP;				// 최대능력(대인)
	DWORD	dwProbabilityPVP;				// 발동 확률(대인)

	DWORD	dwAttackSpeed_;					// 공격속도	// 사용안함
	DWORD	dwTaunt_;						// 타운트	// 사용안함
	DWORD   dwSkillExp_;						// 스킬 경험치	// 사용안함
	DWORD	dwExp__;							// 현재경험치	// 사용안함
	DWORD	dwComboSkillTime_;				// 콤보스킬타임	// 사용안함


	AddSkillProp()
	{
		dwID = 0;				// 변수명	
		dwSkillPropID = 0;		// 스킬Prop ID
		dwSkillLvl = 0;			// 스킬레벨
		dwAbilityMin = 0;		// 최소능력
		dwAbilityMax = 0;		// 최대능력
		dwDmgShift = 0;			// 타격시 반동
		dwProbability = 0;		// 발동 확률

		memset( dwDestParam, 0, sizeof( dwDestParam ) );		// 적용대상1
		memset( dwAdjParamVal, 0, sizeof( dwAdjParamVal ) );	// 적용값1;
		memset( dwChgParamVal, 0, sizeof( dwChgParamVal ) );	//적용변화값1
		memset( arrAddSkillDestNumData1, 0, sizeof( arrAddSkillDestNumData1 ) );		// 적용데이타3개, DestParam[0]에 해당.

		dwActiveSkill = 0;		// 발동 마법
		dwActiveSkillRate = 0;	// 발동 마법 확률.

		nReqMp = 0;				//필요MP
		nReqFp = 0;				//필요FP

		dwCooldown = 0;			// 쿨다운
		dwCastingTime = 0;		//기술 준비시간

		dwSkillRange = 0;		// 기술 시전거리	
		dwCircleTime = 0;		//유지시간
		dwPainTime = 0;         // 지속 피해시간
		dwSkillTime = 0;		//지속시간
		nSkillCount = 0;		// 발사체 발사 개수. 개당공격력 = 총공격력 / nSkillCount;

		dwActiveSkillRatePVP = 0;	// 발동 마법 확률(대인)
		dwAbilityMinPVP = 0;	// 최소능력(대인)
		dwAbilityMaxPVP = 0;	// 최대능력(대인)
		dwProbabilityPVP = 0;	// 발동 확률(대인)

		dwAttackSpeed_ = 0;		// 공격속도
		dwTaunt_ = 0;			// 타운트
		dwSkillExp_ = 0;         //스킬 경험치
		dwExp__ = 0;				//현재경험치
		dwComboSkillTime_ = 0;	//콤보스킬타임
	}
};

enum 
{	
	FILE_FILTER	= 0, 
	FILE_INVALID = 1, 
	FILE_NOTICE = 2,	
	FILE_GUILDCOMBAT_TEXT_1 = 3, 
	FILE_GUILDCOMBAT_TEXT_2 = 4, 
	FILE_GUILDCOMBAT_TEXT_3 = 5, 
	FILE_GUILDCOMBAT_TEXT_4 = 6, 
	FILE_GUILDCOMBAT_TEXT_5 = 7,
	FILE_GUILDCOMBAT_TEXT_6 = 8,
	FILE_ALLOWED_LETTER		= 9		//sun: 10, __RULE_0615

//sun: 11, 일대일 길드 대전
	,
	FILE_GUILDCOMBAT_1TO1_TEXT_1 = 10,
	FILE_GUILDCOMBAT_1TO1_TEXT_2 = 11,
	FILE_GUILDCOMBAT_1TO1_TEXT_3 = 12,
	FILE_GUILDCOMBAT_1TO1_TEXT_4 = 13,
	FILE_GUILDCOMBAT_1TO1_TEXT_5 = 14

#ifdef __VENDOR_1106
	,FILE_ALLOWED_LETTER2	= 15
#endif	// __VENDOR_1106
	,FILE_SERVERMOVEINFO	= 16
};

// Item Property Type
enum IP_TYPE 
{
	IP_FLAG_NONE	= 0x00,				// 플래그 없음 
//	IP_FLAG_BINDS	= 0x01,				// 1 - 귀속 아이템			// item property의 귀속 여부 컬럼 값에 따라 귀속 판단함. 해당 값은 사용하지 않음.
	IP_FLAG_UNDESTRUCTABLE	= 0x02	//	2 - 삭제 불가 아이템
//	,IP_FLAG_EQUIP_BIND	= 0x04			// 장착 시 귀속				// item property의 귀속 여부 컬럼 값에 따라 귀속 판단함. 해당 값은 사용하지 않음.
	, IP_FLAG_PERIOD = 0x08		// 아이템 생성 시 사용 기간 셋팅..
};


struct SkillProp : CtrlProp
{
	DWORD	dwMotion_;			// 동작		// 선언만 되어 있음
	DWORD	dwNum_;				// 기본생성개수
	DWORD	dwPackMax_;			// 최대곂침개수

	BOOL	bPermanence_;		// 소비불가
	DWORD	dwUseable_;			// 사용시점
	DWORD	dwItemSex_;			// 사용성별
	DWORD	dwCost_;				// 가격	
	DWORD	dwEndurance_;		// 내구력

	int		nAbrasion_;			// 마모율
	int		nMaxRepair_;			// 수리횟수	// nHardness

	DWORD	dwFlag_;				// 다목적 플래그	// dwHeelH
	DWORD	dwParts_;			// 장착위치	
	DWORD	dwPartsub_;			// 장착위치	
	DWORD	bPartsFile_;			// 파츠파일 사용여부 
	DWORD	dwExclusive_;		// 제거될위치	
	DWORD	dwBasePartsIgnore_;
	DWORD	dwItemLV_;			// 아이템레벨	

	DWORD   dwShopAble_;
	int		nShellQuantity_;		// 최대장전개수	- IK가 채집도구일때는 최대 채집량이 된다.(삭제 예정)
	DWORD   dwFuelRe_;			// 팩당연료충전량
	DWORD	dwAFuelReMax_;		// 가속연료충전량

	BOOL	bCharged_;			//

	short	wItemEatk_;			// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )
	DWORD   dwParry_;			// 회피율 
	DWORD   dwblockRating_;		// 블럭 수치 
	int		nAddSkillMin_;		// 최소 추가 스킬
	int		nAddSkillMax_;		// 최대 추가 스킬.

	DWORD	dwWeaponType_;		// 무기종류 
	DWORD	dwItemAtkOrder1_;	// 아이템1차공격순서
	DWORD	dwItemAtkOrder2_;	// 아이템2차공격순서
	DWORD	dwItemAtkOrder3_;	// 아이템3차공격순서
	DWORD	dwItemAtkOrder4_;	// 아이템4차공격순서

	DWORD	dwRecoil_;			// 반동	
	DWORD	dwLoadingTime_;		// 장전시간	- IK가 채집도구일때는 채집속도(능력)이 된다.
	LONG	nAdjHitRate_;		// 추가공격성공률	
	FLOAT	fAttackSpeed_;		// 공격속도	

	DWORD	dwSkillReadyType_;

	DWORD	dwSfxElemental_;

	DWORD	dwCircleTime_;		// 유지시간 

	int		nItemResistElecricity_;
	int		nItemResistDark_;	// 선언만 되어 있음
	int		nItemResistFire_;
	int		nItemResistWind_;
	int		nItemResistWater_;
	int		nItemResistEarth_;

	FLOAT	fFlightSpeed_;		// 비행추진력(속도)
	FLOAT	fFlightLRAngle_;		// 좌우 턴 각도.
	FLOAT	fFlightTBAngle_;		// 상하 턴 각도.
	DWORD	dwFlightLimit_;		// 비행제한레벨
	DWORD	dwFFuelReMax_;		// 비행연료충전량
	DWORD	dwLimitLevel1_;		// 제한레벨1
	int		nReflect_;			// 리플렉션 옵션.

	DWORD	dwSndAttack2_;		// 효과음 : 공격 2
	DWORD	dwQuestId_;
	TCHAR	szTextFileName_[64];	// item에 GM command에 넣는 것에 사용

	DWORD	dwSkillRare_;		// 희귀성(TODO 입력값없음, 삭제가능, 테스트 필요)
	int		nSkillLog_;			// 로그(입력값없음, 삭제가능)
	DWORD	dwSkillKind3_;		// 3차스킬종류, 아이템의 잔제같다 삭제해도 될듯하다.
	DWORD	dwExpertLV_;			// (뭔지 잘 파악이 안되네)
	DWORD	dwSkillJob_;			// 스킬직업소속(TODO DatabaseServer에서 사용 뭐에 쓰는 건지 모르겠다. skillKine2 랑 중복되는 것 같다. 삭제해도 될듯.)


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// 삭제 예정?
	DWORD	dwReqMp_;			// 필요MP	- 극단 스킬 툴팁 표시에만 사용중. 스크립트의 해당 컬럼에 데이터는 없음.
	DWORD	dwReqFp_;			// 필요FP	- 극단 스킬 툴팁 표시에만 사용중. 스크립트의 해당 컬럼에 데이터는 없음.
	


	// 공용 특성?
	DWORD	dwSkillKind1;		// 전승 같은것?
	DWORD	dwSkillKind2;		// 직업

	DWORD	dwSkillType;		// 기술속성(여러가지로 쓰인다. KT_SKILL... BUFF_SKILL...)

	LONG	nEvildoing;			// 악행

	DWORD	dwSubDefine;		// AddSkillProp 인덱스?
	DWORD	dwComboStyle;

	
	DWORD	dwExpertMax;		// 최대숙련레벨

	// 배우는 조건
	DWORD	dwReqDisLV;			// 필요 레벨
	DWORD   dwReqSkill1;		// 선행 스킬1
	DWORD   dwReqSkillLevel1;	// 선행 스킬1 필요 레벨
	DWORD   dwReqSkill2;		// 선행 스킬2
	DWORD   dwReqSkillLevel2;	// 선행 스킬2 필요 레벨

	// 사용 조건
	DWORD	dwNeedItemHanded;	// 잡는형식
	DWORD	dwNeedItemKind3;	// 필요 아이템종류??
	DWORD	dwNeedItemBulletKind2;	// 발사체? 소모장비 아이템

	DWORD	dwExeTarget;		// 발동 대상(좀 애매하다, 대상으로도 사용되고 스킬 형태로도 사용된다.)
	DWORD	dwUseChance;		// 사용시기?

	DWORD	dwSpellRegion;		// 마법 범위


	DWORD	dwSkillReady;		// 기술 준비시간(쿨타임)
	DWORD	dwSkillRange;		// 기술 시전거리

	

	DWORD	dwReqPartyPoint;	// 현재경험치(파티스킬일때 삭제할 포인트)


	// 적용
	DWORD	dwSkillSpellType;		// 마법속성(데미지 산출?)(마법스킬?)
	SAI79::ePropType	eSkillType;	// dwSkillSpellType과 중복되는 듯 하면서도 다르다. 애매하다.

	BYTE	byBuffTickType;		// 버프 효과 적용 타입(시작과 매틱, 매틱만)

	DWORD	dwAtkStyle;			// 공격 스타일 (dwAtkStyle == AS_BACK 여기에서만 사용된다. 뭐야.)

	DWORD   dwReferStat1;
	DWORD   dwReferStat2;
	DWORD   dwReferTarget1;
	DWORD   dwReferTarget2;
	DWORD   dwReferValue1;
	DWORD   dwReferValue2;


	DWORD	dwAbilityMin;		// 최소능력치 - 공격력, 방어력, 성능 기타등등 AddSkillProp가 없는 스킬의 기본값?
	DWORD	dwAbilityMax;		// 최대능력치 - 공격력, 방어력, 성능 기타등등 AddSkillProp가 없는 스킬의 기본값?

    DWORD	tmContinuousPain;	// 지속 피해 

	DWORD	dwDmgShift;			// 타격시 반동	
	DWORD	dwAttackRange;		// 공격범위(한군대만 사용중)
	int		nProbability;		// 적용확률

	DWORD	dwDestParam[3];		// 적용대상1
	LONG	nAdjParamVal[3];	// 적용값1
	DWORD	dwChgParamVal[3];	// 적용변화값1
	int		nDestData1[3];		// 적용데이타값 3개, destParam1에만 해당됨.

	DWORD	dwSkillTime;		// 지속시간


	DWORD	dwActiveSkill;		// 발동 마법
	DWORD	dwActiveSkillLv;	// 발동 마법 레벨 (사용안하지만 사용해야 할듯.)
	DWORD	dwActiveSkillRate;	// 발동 마법 확률.

	DWORD	dwSfxObj;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj2;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj3;			// 발동효과, 타겟
	DWORD	dwSfxObj4;			// 지속되는 효과 이펙트.
	DWORD	dwSfxObj5;			// 발동효과, 시전자 
	DWORD	dwUseMotion;		// 사용 모션

	DWORD	dwSndAttack1;		// 효과음 : 공격 1 //client만 사용

#ifdef __CLIENT
    TCHAR	szIcon[64];			// dds파일 이름 
	TCHAR	szCommand[256];		// 설명문 
#endif
	int		nVer;

	DWORD	dwAdjMonsterGrade;		// 적용 가능 몬스터 등급

	// 유지 조건
	DWORD	dwEquipItemKeepSkill;	// 사용된 스킬 유지시 필요한 아이템

	bool	bCanUseActionSlot;		// 액션 슬롯에 등록할 수 있는 스킬인가


//////////////////////////////////////////////////////////////////////////
	
	BOOL	IsDebuff()
	{
		if( nEvildoing < 0 )
		{
			return TRUE;
		}

		return FALSE;
	}
};


// 직업에 따른 factor ENUM
enum JOB_PROP_TYPE
{
	JOB_PROP_SWD,
	JOB_PROP_AXE,	
	JOB_PROP_STAFF,
	JOB_PROP_STICK,
	JOB_PROP_KNUCKLE,
	JOB_PROP_WAND,
	JOB_PROP_BLOCKING,
	JOB_PROP_YOYO,
	JOB_PROP_CRITICAL,
};

// 직업에 따른 프로퍼티 ( propJob.inc에서 읽어들임 )
struct JobProp
{
	float	fAttackSpeed;			//공속 
	float	fFactorMaxHP;			//최대 HP 계산에 사용되는 factor
	float	fFactorMaxMP;			//최대 MP 계산에 사용되는 factor
	float	fFactorMaxFP;			//최대 FP 계산에 사용되는 factor
	float   fFactorDef;				//물리 방어력 계산에 사용되는 factor
	float	fFactorHPRecovery;		//HP회복 factor
	float	fFactorMPRecovery;		//MP회복 factor
	float	fFactorFPRecovery;		//FP회복 factor
	float	fMeleeSWD;				//WT_MELEE_SWD의 ATK factor
	float	fMeleeAXE;				//WT_MELEE_AXE의 ATK factor
	float   fMeleeSTAFF; 			//WT_MELEE_STAFF의 ATK factor
	float   fMeleeSTICK;			//WT_MELEE_STICK의 ATK factor
	float   fMeleeKNUCKLE;			//WT_MELEE_KNUCKLE의 ATK factor
	float   fMagicWAND;				//WT_MAGIC_WAND의 ATK factor 
	float   fBlocking;				//블록킹 factor
	float	fMeleeYOYO;				//요요의 ATK factor 
	float   fCritical;				//크리티컬 처리
	TCHAR	szMarkFileName[ 32 ];		//마크파일명
};

typedef	struct	tagRANDOM_ITEM
{
	BYTE	m_lpQuestFlagArray[ITEM_KIND_MAX / 8 + 1];

	void	SetItemKind( int nItemKind,BOOL bBool );
	BOOL	IsItemKind( int nItemKind );

	BYTE	m_nTotalNum;	// 발생 아이템 갯수 
	BYTE	m_nUniqueMax;	// 유니크 아이템 최대 
	BYTE	m_nUniqueMin;	// 유니크 아이템 최소  
	WORD	m_nGoldMax;		// 골드 맥스 
	WORD	m_nAmmoMax;		// 총알 맥스 
	WORD	m_nDartMax;		// 표창 맥스 
	WORD	m_nGoldMin;		// 골드 맥스 
	WORD	m_nAmmoMin;	// 총알 맥스 
	WORD	m_nDartMin;	// 표창 맥스 
	tagRANDOM_ITEM()
	{
		::memset( m_lpQuestFlagArray, 0, sizeof( m_lpQuestFlagArray ) );

		m_nTotalNum		= 0;
		m_nUniqueMax	= 0;
		m_nUniqueMin	= 0;
		m_nGoldMax		= 0;
		m_nAmmoMax		= 0;
		m_nDartMax		= 0;
		m_nGoldMin		= 0;
		m_nAmmoMin		= 0;
		m_nDartMin		= 0;
	}
}
RANDOM_ITEM,* LPRANDOM_ITEM;

/*----------------------------------------------------------------------------------------------------*/
enum DROPTYPE
{
	DROPTYPE_NORMAL,
	DROPTYPE_SEED,
};

typedef struct	tagDROPITEM
{
	DROPTYPE	dtType;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwLevel;
	DWORD	dwNumber;
	DWORD	dwNumber2;	// Min, Max중 Max로 씀.
}
DROPITEM,	*LPDROPITEM;

typedef	struct	tagDROPKIND
{
	DWORD	dwIK3;
	short	nMinUniq;
	short	nMaxUniq;
}
DROPKIND,	*LPDROPKIND;

typedef	struct	tagQUESTITEM
{
	DWORD	dwQuest;
	DWORD   dwState;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwNumber; 
}
QUESTITEM,	*PQUESTITEM;

typedef struct tagEVENTITEM
{
	DWORD	dwIndex;
	DWORD	dwPrabability;
}
EVENTITEM,	*PEVENTITEM;

#define MAX_QUESTITEM	16
#define	MAX_DROPKIND	80

class CDropItemGenerator
{
private:
	std::vector<DROPITEM>	m_dropItems;

public:
	DWORD				m_dwMax;

public:
//	Contructions
	CDropItemGenerator() { m_dwMax = 0; }
	virtual	~CDropItemGenerator()	{	Clear();	}

//	Operations
	void		AddTail( CONST DROPITEM & rDropItem, const char* s );
	void	Clear( void )	{	m_dropItems.clear();	}
	DWORD		GetSize( void )	{	return m_dropItems.size();	}
	DROPITEM*	GetAt( int nIndex, BOOL bUniqueMode, float fProbability = 0.0f );
};

class CDropKindGenerator
{
private:
	int		m_nSize;
	DROPKIND	m_aDropKind[MAX_DROPKIND];
public:
//	Contructions
	CDropKindGenerator()
		{	
			m_nSize	= 0;	
			memset( m_aDropKind, 0, sizeof( m_aDropKind ) );
		}
	virtual	~CDropKindGenerator()	{}
//	Operations
	void	AddTail( const DROPKIND & rDropKind );	// memcpy
	int		GetSize( void )		{	return m_nSize;	}
	LPDROPKIND	GetAt( int nIndex );
};

class CQuestItemGenerator
{
private:
	u_long	m_uSize;
	QUESTITEM	m_pQuestItem[MAX_QUESTITEM];
public:
	CQuestItemGenerator()		{	m_uSize	= 0;	::memset( m_pQuestItem, 0, sizeof( m_pQuestItem ) );	}
	~CQuestItemGenerator()	{}
	void	AddTail( const QUESTITEM & rQuestItem );
	u_long	GetSize( void )	{	return m_uSize;	}
	QUESTITEM*	GetAt( int nIndex );
};

struct MonsterTransform
{
	float fHPRate;
	DWORD dwMonsterId;
	MonsterTransform() : fHPRate( 0.0f ), dwMonsterId( NULL_ID ) {}
};

/*----------------------------------------------------------------------------------------------------*/

struct MoverProp : CtrlProp
{
#ifdef __WORLDSERVER
	struct FLDropItemAttr
	{
		FLDropItemAttr() : bDropOwnerShipOff( FALSE ), bLevelDropPanaltyOff( FALSE ) {}
		BOOL			bDropOwnerShipOff;
		BOOL			bLevelDropPanaltyOff;
	};

	FLDropItemAttr		m_kDropItemAttr;
#endif //__WORLDSERVER

	DWORD	dwStr;	// 힘,                  
	DWORD	dwSta;	// 체력,
	DWORD	dwDex;	// 민첩,
	DWORD	dwInt;	// 지능,
	DWORD	dwHR;
	DWORD   dwER;
	DWORD	dwRace;		// 종족,
	DWORD	dwBelligerence;		// 호전성,
	DWORD	dwGender;	// 성별,
	DWORD	dwLevel;	// 레벨,
	DWORD	dwFlightLevel;	// 비행레벨
	DWORD	dwSize;		// 크기,
	DWORD   dwClass;
	BOOL	bIfParts;	// 파츠냐?
	int		nChaotic;	// 나쁜놈 마이너스/ 좋은넘 플러스
#ifdef __S1108_BACK_END_SYSTEM
	DWORD	dwUseable;	// 방어 캐릭수,
#else // __S1108_BACK_END_SYSTEM
	DWORD	dwDefExtent;	// 방어 캐릭수,
#endif // __S1108_BACK_END_SYSTEM
	DWORD	dwActionRadius;		// 전투행동번경,
	DWORD	dwAtkMin;	// 최소타격치,
	DWORD	dwAtkMax;	// 최대타격치,
	DWORD	dwAtk1;
	DWORD	dwAtk2;
	DWORD	dwAtk3;
	DWORD	dwAtk4;		// dwHorizontalRate가 이걸로 바뀜.
//sun: 9,	//__AI_0509
	FLOAT	fFrame;	// 이동 시 프레임 가중치
	DWORD	dwOrthograde;

	DWORD	dwThrustRate;	// 찌르기비율,

	DWORD	dwChestRate;
	DWORD	dwHeadRate;  
	DWORD	dwArmRate;
	DWORD	dwLegRate;

	DWORD	dwAttackSpeed;	// 공격속도,
	DWORD	dwReAttackDelay;
	DWORD	dwAddHp;		// ,
	DWORD	dwAddMp;		// ,
	DWORD	dwNaturalArmor;	// 자연방어력 
	int		nAbrasion;	// 마모도
	int		nHardness;	// 경도
	DWORD	dwAdjAtkDelay;	// 추가공격속도저하,

	SAI79::ePropType	eElementType;
	short				wElementAtk;		// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )

	DWORD	dwHideLevel;	// 레벨 안보이는넘이냐..
	FLOAT	fSpeed;	// 이동속도,
	DWORD	dwShelter;	// 거주지역,
	DWORD	dwFlying;	// 비행유무,
	DWORD	dwJumpIng;	// 높이뛰기 
	DWORD	dwAirJump;	// 멀리뛰기
	DWORD	bTaming;	// 조련유무 
	DWORD	dwResisMgic;	//마법저항 

	int		nResistElecricity;
	int		nResistDark;
	int		nResistFire;
	int		nResistWind;
	int		nResistWater;
	int		nResistEarth;
	
	DWORD	dwCash;		// 제공금액
	DWORD	dwSourceMaterial;	// 제공재료
	DWORD	dwMaterialAmount;	// 재료양
	DWORD	dwCohesion;	// 재료응집도
	DWORD	dwHoldingTime;	// 시체유지시간
	DWORD	dwCorrectionValue;	// 아이템생성보정값
	EXPINTEGER	nExpValue;
	int		nFxpValue;		// 비행경험치.
	DWORD	nBodyState;		// 몸상태,
	DWORD	dwAddAbility;	// 추가능력,
	DWORD	bKillable;	// 죽음유무,

	DWORD	dwVirtItem[3];
	DWORD	bVirtType[3]; 

	DWORD   dwSndAtk1  ;
	DWORD   dwSndAtk2  ;
	DWORD   dwSndDie1  ;
	DWORD   dwSndDie2  ;
	DWORD   dwSndDmg1  ;
	DWORD   dwSndDmg2  ;
	DWORD   dwSndDmg3  ;
	DWORD   dwSndIdle1 ;
	DWORD   dwSndIdle2 ;

	//gmpbigsun( 20111226 ) 추가됨
	DWORD	dwAreaColor;
	TCHAR	szMarkFileName[32];

	int		nMadrigalGiftPoint;

	short   m_nEvasionHP;
	short	m_nEvasionSec;
	short   m_nRunawayHP          ; // HP가 10 이하면 도주 
	short   m_nCallHelperMax       ; // 총 갯수 
	short   m_nCallHP              ; // 도움요청하기 위한 HP
	short   m_nCallHelperIdx  [ 5 ]; // 도움요청하기 Id
	short   m_nCallHelperNum  [ 5 ]; // 도움요청하기 Id
	short   m_bCallHelperParty[ 5 ]; // 도움요청하기 Id

	short   m_dwAttackMoveDelay;
	short   m_dwRunawayDelay;
	short   m_bPartyAttack;
	short   m_nHelperNum;

#if !defined(__CORESERVER) 
	int		m_nScanJob;		// 타겟을 검색할때 특정 직업으로 검색하는가. 0 이면 ALL
	// 공격조건들은 비스트에서 선공으로 지정되어 있을때 얘기다.
	int		m_nHPCond;		// 타겟이 hp% 이하일때 공격을 함.
	int		m_nLvCond;		// 타겟과 레벨비교하여 낮으면 공격을 함.
	int		m_nRecvCondMe;	// 회복 조건.  내hp가 몇%이하로 떨어졌을때?
	int		m_nRecvCondHow;	// 회복할때 회복할 양 %단위
	int		m_nRecvCondMP;	// 회복할때 MP소모량 %단위
	BYTE	m_bMeleeAttack;	// 근접 공격 AI가 있는가?
	BYTE	m_bRecvCondWho;	// 누구를 치료할꺼냐. 0:지정안됨 1:다른놈 2:나 3:모두.
	BYTE	m_bRecvCond;	// 치료하냐? 0:치료안함 1:전투중에만 치료함 2:전투/비전투 모두 치료
	BYTE	m_bHelpWho;		// 도움요청시 - 0:부르지않음 1:아무나  2:같은종족만.
	BYTE	m_bRangeAttack[ MAX_JOB ];		// 각 직업별 원거리 공격 거리.
	int		m_nSummProb;	// 소환 확률 : 0이면 소환능력 없음.
	int		m_nSummNum;		// 한번에 몇마리나 소환하냐.
	int		m_nSummID;		// 어떤 몬스터?
	int		m_nHelpRangeMul;	// 도움요청 거리. 시야의 배수
	DWORD	m_tmUnitHelp;			// 헬프 타이머.
	int		m_nBerserkHP;		// 버서커가 되기 위한 HP%
	float	m_fBerserkDmgMul;	// 버서커가 되었을때 데미지 배수.
	int		m_nLoot;			// 루팅몹인가.
	int		m_nLootProb;		// 루팅 확률
	DWORD   m_dwScanQuestId; 
	DWORD   m_dwScanItemIdx; 
	int		m_nScanChao;		// 카오, 비카오 검색
#endif // !__CORESERVER
	
#ifdef __S1108_BACK_END_SYSTEM
	float	m_fHitPoint_Rate;		// 몬스터 최대 HP률 // dwAddHp * m_nHitPoint_Rate
	float	m_fAttackPower_Rate;	// 몬스터 최대 공격률 // dwAtkMin * m_nAttackPower_Rate
	float	m_fDefence_Rate;		// 몬스터 최대 방어률 // dwAddHp * m_nDefence_Rate
	float	m_fExp_Rate;			// 몬스터 최대 경험치률 // dwAddHp * m_nExp_Rate
	float	m_fItemDrop_Rate;		// 몬스터 최대 아이템 드롭률 // dwAddHp * m_nItemDrop_Rate
	float	m_fPenya_Rate;			// 몬스터 최대 페냐률 // dwAddHp * m_nPenya_Rate
	BOOL	m_bRate;
#endif // __S1108_BACK_END_SYSTEM

	
	short	m_nAttackItemNear;
	short	m_nAttackItemFar;
	short	m_nAttackItem1;
	short	m_nAttackItem2;
	short	m_nAttackItem3;
	short	m_nAttackItem4;
	short	m_nAttackItemSec;
	short	m_nMagicReflection;
	short	m_nImmortality;
	BOOL 	m_bBlow;
	short	m_nChangeTargetRand;

	short   m_nAttackFirstRange;
	RANDOM_ITEM		m_randomItem  ;
	CDropItemGenerator	m_DropItemGenerator;
	CQuestItemGenerator		m_QuestItemGenerator;
	CDropKindGenerator	m_DropKindGenerator;
	MonsterTransform	m_MonsterTransform;


// 파티 몬스터
	struct	FLMonsterKey		//중복된 MoverIndex를 사용할수 있도록하는 객체
	{
		DWORD				dwMoverID;
		DWORD				dwSequence;
		bool operator < ( const FLMonsterKey & l ) const	{	return ( dwMoverID < l.dwMoverID ) || ( dwMoverID == l.dwMoverID && dwSequence < l.dwSequence ); }
		bool operator == ( const FLMonsterKey & l ) const	{	return ( dwMoverID == l.dwMoverID && dwSequence == l.dwSequence ); }
	};
	typedef					std::map< FLMonsterKey, DWORD /* MoverIndex */ >	SummonMonstersMap;
	SummonMonstersMap		m_mapSummonMonsters;
// 파티 몬스터
	
	MoverProp()
	{
		dwStr	= dwSta	= dwDex	= dwInt	= dwHR	= dwER	= dwRace	= dwBelligerence	=dwGender
		= dwLevel	= dwFlightLevel	= dwSize	= dwClass	= bIfParts	= nChaotic
		#ifdef __S1108_BACK_END_SYSTEM
		= dwUseable
		#else // __S1108_BACK_END_SYSTEM
		= dwDefExtent
		#endif // __S1108_BACK_END_SYSTEM
		= dwActionRadius	= dwAtkMin	= dwAtkMax	= dwAtk1	= dwAtk2	= dwAtk3	= dwAtk4	= 0;
		//sun: 9,	//__AI_0509
			fFrame	= 1.0F;
			dwOrthograde	= 0;
		
		//!	dwVerticalRate	= dwDiagonalRate	= 0;
		
		dwThrustRate		= 0;
		dwChestRate			= 0;
		dwHeadRate			= 0;
		dwArmRate			= 0;
		dwLegRate			= 0;
		dwAttackSpeed		= 0;
		dwReAttackDelay		= 0;
		dwAddHp				= 0;
		dwAddMp				= 0;
		dwNaturalArmor		= 0;
		nAbrasion			= 0;
		nHardness			= 0;
		dwAdjAtkDelay		= 0;
		dwHideLevel			= 0;
		eElementType		= SAI79::NO_PROP;
		wElementAtk			= 0;
		fSpeed				= 0.0F;
		dwShelter			= 0;
		dwFlying			= 0;
		dwJumpIng			= 0;
		dwAirJump			= 0;
		bTaming				= 0;
		dwResisMgic			= 0;
		nResistElecricity	= 0;
		nResistDark			= 0;
		nResistFire			= 0;
		nResistWind			= 0;
		nResistWater		= 0;
		nResistEarth		= 0;
		dwCash				= 0;
		dwSourceMaterial	= 0;
		dwMaterialAmount	= 0;
		dwCohesion			= 0;
		dwHoldingTime		= 0;
		dwCorrectionValue	= 0;
		nExpValue			= 0;
		nFxpValue			= 0;
		nBodyState			= 0;
		dwAddAbility		= 0;
		bKillable			= 0;
		
		memset( dwVirtItem, 0, sizeof(dwVirtItem) );
		memset( bVirtType, 0, sizeof(bVirtType) );

		dwSndAtk1			= 0;
		dwSndAtk2			= 0;
		dwSndDie1			= 0;
		dwSndDie2			= 0;
		dwSndDmg1			= 0;
		dwSndDmg2			= 0;
		dwSndDmg3			= 0;
		dwSndIdle1			= 0;
		dwSndIdle2			= 0;
		m_nEvasionHP		= 0;
		m_nEvasionSec		= 0;
		m_nRunawayHP		= 0;
		m_nCallHelperMax	= 0;
		m_nCallHP			= 0;
		
		memset( m_nCallHelperIdx, 0, sizeof(m_nCallHelperIdx) );
		memset( m_nCallHelperNum, 0, sizeof(m_nCallHelperNum) );
		memset( m_bCallHelperParty, 0, sizeof(m_bCallHelperParty) );
		
		m_dwAttackMoveDelay	= 0;
		m_dwRunawayDelay	= 0;
		m_bPartyAttack		= 0;
		m_nHelperNum		= 0;

#if !defined(__CORESERVER) 
		m_nScanJob			= 0;
		m_nHPCond			= 0;
		m_nLvCond			= 0;
		m_nRecvCondMe		= 0;
		m_nRecvCondHow		= 0;
		m_nRecvCondMP		= 0;
		m_bMeleeAttack		= 0;
		m_bRecvCondWho		= 0;
		m_bRecvCond			= 0;
		m_bHelpWho			= 0;
		memset( m_bRangeAttack, 0, sizeof(m_bRangeAttack) );
		m_nSummProb			= 0;
		m_nSummNum			= 0;
		m_nSummID			= 0;
		m_nHelpRangeMul		= 0;
		m_tmUnitHelp		= 0;
		m_nBerserkHP		= 0;
		m_fBerserkDmgMul	= 0;
		m_nLoot				= 0;
		m_nLootProb			= 0;
		m_dwScanQuestId		= 0;
		m_dwScanItemIdx		= 0;
		m_nScanChao			= 0;
#endif // !__CORESERVER

#ifdef __S1108_BACK_END_SYSTEM
		m_fHitPoint_Rate	=
		m_fAttackPower_Rate	= 
		m_fDefence_Rate	=
		m_fExp_Rate	= 
		m_fItemDrop_Rate	= 
		m_fPenya_Rate	= 0.0F;
		m_bRate	=		FALSE;
#endif // __S1108_BACK_END_SYSTEM
		m_nAttackItemNear	=
		m_nAttackItemFar	=
		m_nAttackItem1	=
		m_nAttackItem2	=
		m_nAttackItem3	=
		m_nAttackItem4	=
		m_nAttackItemSec	=
		m_nMagicReflection	=
		m_nImmortality	=	0;
		m_bBlow	=		FALSE;
		m_nChangeTargetRand	=
		m_nAttackFirstRange	= 0;
	}
};

typedef	struct	tagBeginItem
{
	DWORD	dwItemType;
	DWORD	dwItemId;
	DWORD	dwNum;
}
BEGINITEM, *LPBEGINITEM; 

#define MAX_BEGINEQUIP  10
#define MAX_BEGINITEM   42 //20 
#define MAX_JOBITEM     10 

typedef struct tagJOBITEM
{
	DWORD adwMale[ MAX_BEGINEQUIP ][ 2 ];
	BEGINITEM beginItem[ MAX_BEGINITEM ];

} JOBITEM,* LPJOBITEM;

#define	MAX_BEGINPOS	16

typedef	struct tagBEGINPOSARR
{
	u_long uSize;
#if !defined(__CLIENT) && !defined(__WORLDSERVER) && !defined(__DBSERVER)
	vector3 avPos[MAX_BEGINPOS];
#else
	D3DXVECTOR3 avPos[MAX_BEGINPOS];
#endif
}
BEGINPOSARR, *LPBEGINPOSARR;

#define TASKBAR_TOP    0
#define TASKBAR_BOTTOM 1
#define TASKBAR_LEFT   2
#define TASKBAR_RIGHT  3

// short cut
#if __VER < 19

#define MAX_SLOT_APPLET			18
#define MAX_SLOT_ITEM			9
#define MAX_SLOT_QUEUE			5
#define	MAX_SLOT_ITEM_COUNT		8

#else

const size_t	MAX_SLOT_APPLET			= 18;
const size_t	MAX_SLOT_ITEM_COUNT		= 5;
const size_t	MAX_SLOT_ITEM			= 10;
const size_t	MAX_SLOT_QUEUE			= 6;

#endif // __VER < 19

#define	SHORTCUT_NONE    0
#define	SHORTCUT_ETC     1
#define	SHORTCUT_APPLET  2
#define	SHORTCUT_MOTION  3
#define	SHORTCUT_SCRIPT  4
#define	SHORTCUT_ITEM    5
//#define	SHORTCUT_CARD    5
//#define	SHORTCUT_JACKBOX 6
#define	SHORTCUT_SKILL   6
#define SHORTCUT_OBJECT  7
#define SHORTCUT_CHAT    8
#define SHORTCUT_SKILLFUN 9
#define SHORTCUT_EMOTICON 10
#define SHORTCUT_LORDSKILL	 11


#if defined ( __CLIENT )
class CWndBase;
#endif	// __CLIENT



//---------------------------------------------------------------------------------------------
// 단축 아이콘 정보 - lenahyang 초기화 수정
//---------------------------------------------------------------------------------------------
typedef struct tagSHORTCUT
{
// 원본코드 
//#if defined ( __CLIENT )
//	CWndBase* m_pFromWnd   ;
//	CTexture* m_pTexture   ;
//	DWORD	m_dwItemId;			//sun: 8차 작업
//	int m_nExtraFrame;			//v19 quick slot bar
//
//	tagSHORTCUT::tagSHORTCUT( void ) : m_pFromWnd( NULL ), m_pTexture( NULL ), m_nExtraFrame ( 0 ) {}
//#endif	// __CLIENT
//	DWORD     m_dwShortcut ; 
//	DWORD     m_dwId       ; 
//	DWORD     m_dwType     ; 
//	DWORD     m_dwIndex    ; 
//	DWORD     m_dwUserId   ; 
//	DWORD     m_dwData     ; 
//	TCHAR     m_szString[MAX_SHORTCUT_STRING]; // SHORTCUT_CHAT일 경우 저장.
//	BOOL IsEmpty() { return m_dwShortcut == SHORTCUT_NONE; }
//	void Empty() { m_dwShortcut = SHORTCUT_NONE; }


#if defined ( __CLIENT )

	CWndBase*		m_pFromWnd;
	CTexture*		m_pTexture;
	DWORD			m_dwItemId;							//sun: 8차 작업
	int				m_nExtraFrame;						//v19 quick slot bar
	DWORD			m_dwShortcut; 
	DWORD     		m_dwId; 
	DWORD     		m_dwType; 
	DWORD     		m_dwIndex; 
	DWORD     		m_dwUserId; 
	DWORD     		m_dwData; 
	TCHAR     		m_szString[MAX_SHORTCUT_STRING];	// SHORTCUT_CHAT일 경우 저장.

	tagSHORTCUT::tagSHORTCUT( void )
	:	m_pFromWnd( NULL ) 
	,	m_pTexture( NULL )
	,	m_dwItemId( NULL )
	,	m_nExtraFrame( NULL )
	,	m_dwShortcut( SHORTCUT_NONE )
	,	m_dwId( NULL ) 
	,	m_dwType( NULL )
	,	m_dwIndex( NULL )
	,	m_dwUserId( NULL )
	,	m_dwData( NULL ) 
	{
		ZeroMemory( m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING );	
	}

	void	Empty( void )
	{
		m_pFromWnd		= NULL; 
		m_pTexture		= NULL;
		m_dwItemId		= NULL;
		m_nExtraFrame	= NULL;
		m_dwShortcut	= SHORTCUT_NONE;
		m_dwId			= NULL; 
		m_dwType		= NULL;
		m_dwIndex		= NULL;
		m_dwUserId		= NULL;
		m_dwData		= NULL; 

		ZeroMemory( m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING );	
	}

#else

	DWORD			m_dwShortcut; 
	DWORD     		m_dwId; 
	DWORD     		m_dwType; 
	DWORD     		m_dwIndex; 
	DWORD     		m_dwUserId; 
	DWORD     		m_dwData; 
	TCHAR     		m_szString[MAX_SHORTCUT_STRING];	// SHORTCUT_CHAT일 경우 저장.

	tagSHORTCUT::tagSHORTCUT( void )
	:	m_dwShortcut( SHORTCUT_NONE )
	,	m_dwId( NULL ) 
	,	m_dwType( NULL )
	,	m_dwIndex( NULL )
	,	m_dwUserId( NULL )
	,	m_dwData( NULL ) 
	{
		ZeroMemory( m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING );	
	}

	void	Empty( void )
	{
		m_dwShortcut	= SHORTCUT_NONE;
		m_dwId			= NULL; 
		m_dwType		= NULL;
		m_dwIndex		= NULL;
		m_dwUserId		= NULL;
		m_dwData		= NULL; 

		ZeroMemory( m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING );	
	}

#endif

	BOOL	IsEmpty( void ) const		{ return m_dwShortcut == SHORTCUT_NONE; }

} SHORTCUT,* LPSHORTCUT;


typedef struct tagEXPPARTY
{
	DWORD	Exp;
	DWORD	Point;
}
EXPPARTY, *LPEXPPARTY;

typedef struct tagRENEWEVENT
{
	int		nLevel;
	DWORD	dwItemId;
	TCHAR	strItemName[64];
	float	fPercent;
}
RENEWEVENT, *LPRENEWEVENT;

typedef struct tagRENEWEVENTITEM
{
	TCHAR strTime[32];
	int nLevel;
	int	nRealitem[ MAX_REALITEM ];
	int nRealitemCount[ MAX_REALITEM ];
	int nSendRealitemCount[ MAX_REALITEM ];
	int nPercent[ MAX_REALITEM ];
	int nNextIndex;
}
RENEWEVENTITEM, *LPRENEWEVENTITEM;


typedef struct tagExpCharacter
{
	EXPINTEGER	nExp1;
	EXPINTEGER	nExp2;
	DWORD dwLPPoint;
	EXPINTEGER	nLimitExp;
}
EXPCHARACTER, *LPEXPCHARACTER;

#endif	// __PROJECTCMN_H__