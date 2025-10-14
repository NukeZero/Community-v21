#ifndef __PROJECTCMN_H__
#define	__PROJECTCMN_H__

#ifndef __DAISY

#define	MAX_OBJARRAY	8
#define	MAX_QUICKSLOT	21
#define	MAX_EQUIPMENT	18
#define	MAX_INVENTORY	42 // 60 //42
#define MAX_GUILDBANK	42
#define	MAX_BANK	42
#define	MAX_REPAIR	16
#define	MAX_TRADE	25

#define	MAX_VENDITEM	30

#define	MAX_REPAIRINGITEM		25
#define	MAX_VENDORNAME	48
#define	MAX_STUFF	( MAX_INVENTORY + MAX_QUICKSLOT )
#define	MAX_EXPLPPOINT	41
#define	MAX_EXPSKILL	21
#define	MAX_EXPCHARACTER	200
#define	MAX_FXPCHARACTER	200
#define	MAX_CHARACTER_LIST	3 
#define	MAX_VENDOR_INVENTORY	100
#define	MAX_VENDOR_INVENTORY_TAB	4
#define	MAX_QUEST	100
#define	MAX_COMPLETE_QUEST	300 
#define MAX_PARTYLEVEL 10
#define MAX_ADDEXPPARTY 16
#define MAX_RENEWEVENT 8000
#define MAX_REALITEM 8 // 이벤트 상품 아이템
#define MAX_EVENTREALITEM 4096

#define MAX_EXPJOBLEVEL 90
#define MAX_EXPCOMPANY	20
#define	ITEM_KIND_MAX	100

#include "propdamage.h"

// __XITEM0914 이후부터 배열로 바뀜.
#define dwDestParam1	dwDestParam[0]
#define dwDestParam2	dwDestParam[1]
#define nAdjParamVal1	nAdjParamVal[0]
#define nAdjParamVal2	nAdjParamVal[1]
#define dwChgParamVal1	dwChgParamVal[0]
#define dwChgParamVal2	dwChgParamVal[1]

struct ObjProp
{
	DWORD	dwID;		// 변수명	
	TCHAR	szName[64];	// 한글명칭 
	TCHAR	szNameEn[32];		// 영문명칭	
	DWORD	dwType; 
	DWORD	dwAI;		// AIInterface
	DWORD	dwHP;
};

struct CtrlProp : ObjProp
{
	DWORD   dwCtrlKind1;
	DWORD   dwCtrlKind2;
	DWORD   dwCtrlKind3;
	DWORD   dwSfxCtrl;
	DWORD   dwSndDamage;
	TCHAR	szCommand [128];
};

struct AddSkillProp
{
	DWORD	dwID;				// 변수명	
	DWORD	dwName;				// 명칭
	DWORD	dwSkillLvl;			// 스킬레벨
	DWORD	dwAbilityMin;		// 최소능력
	DWORD	dwAbilityMax;	// 최대능력
	DWORD	dwAttackSpeed;		// 공격속도
	DWORD	dwDmgShift;			// 타격시 반동
	DWORD	nProbability;		// 발동 확률
	DWORD	dwTaunt;			// 타운트
	DWORD	dwDestParam[2];		// 적용대상1
	DWORD	nAdjParamVal[2];	// 적용값1;
	DWORD	dwChgParamVal[2];		//적용변화값1
	int		nDestData1[3];		// 적용데이타3개, DestParam[0]에 해당.
#ifdef __XSKILL0425
	DWORD	dwActiveSkill;		// 발동 마법
	DWORD	dwActiveSkillRate;	// 발동 마법 확률.
#endif
	int		nReqMp;				//필요MP
	int		nReqFp;				//필요FP
	DWORD	dwSkillReady;		//기술 준비시간
	DWORD	dwSkillRange;		// 기술 시전거리	
	DWORD	dwCircleTime;		//유지시간
	DWORD   dwPainTime;         // 지속 피해시간
	DWORD	dwSkillTime;		//지속시간
	int		nSkillCount;		// 발사체 발사 개수. 개당공격력 = 총공격력 / nSkillCount;
	DWORD   dwSkillExp;         //스킬 경험치
	DWORD	dwExp;				//현재경험치
	DWORD	dwComboSkillTime;	//콤보스킬타임

	AddSkillProp()
	{
		dwID = dwName = 0;
		nProbability = 0;
		dwSkillLvl = dwAbilityMin = dwAbilityMax = dwAttackSpeed = dwDmgShift = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		nReqMp = nReqFp = dwSkillReady = 0;
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

struct PartySkillProp
{
	DWORD	dwID;				// 변수명	
	DWORD	dwName;				// 명칭
	DWORD	dwSkillLvl;			// 스킬레벨
	DWORD	dwAbilityMin;		// 최소능력
	DWORD	dwAtkAbilityMax;	// 최대능력
	DWORD	dwAttackSpeed;		// 공격속도
	DWORD	dwDmgShift;			// 타격시 반동
	DWORD	dwGroggy;			// 그로키
	DWORD	dwTaunt;			// 타운트
	DWORD	dwDestParam[2];		// 적용대상1
	DWORD	nAdjParamVal[2];		// 적용값1;
	DWORD	dwChgParamVal[2];		//적용변화값1
	DWORD	dwReqMp;			//필요MP
	DWORD	dwReqFp;			//필요FP
	DWORD	dwSkillReady;		//기술 준비시간
	//	DWORD	dwUseMotion;		//사용 모션
	DWORD	dwCircleTime;		//유지시간
	DWORD	dwSkillTime;		//지속시간
	DWORD	dwExp;				//현재경험치
	DWORD	dwComboSkillTime;	//콤보스킬타임
	PartySkillProp()
	{
		dwID = dwName = 0;
		dwSkillLvl = dwAbilityMin = dwAtkAbilityMax = dwAttackSpeed = dwDmgShift = dwGroggy = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		dwReqMp = dwReqFp = dwSkillReady = 0;
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

#define		nCOLLECT_SPEED		dwLoadingTime		// IK3_COLLECT에서는 채집속도로 씀
#define		nCOLLECT_MAX		nShellQuantity		// IK3_COLLECT에서는 채집통크기로 씀.

// raiders_060112
// Item Property Type
enum IP_TYPE 
{
	IP_FLAG_NONE,				// 플래그 없음 
	IP_FLAG_BINDS,				// 1 - 귀속 아이템 
};

struct ItemProp : CtrlProp
{
	DWORD	dwMotion;			// 동작 
	DWORD	dwNum;				// 기본생성개수	
	DWORD	dwPackMax;			// 최대곂침개수	
	DWORD	dwItemKind1;		// 1차아이템종류	
	DWORD	dwItemKind2;		// 2차아이템종류	
	DWORD	dwItemKind3;		// 3차아이템종류	
	DWORD	dwItemJob;			// 아이템직업소속 
	BOOL	bPermanence;		// 소비불가	
	DWORD	dwUseable;			// 사용시점	
	DWORD	dwItemSex;			// 사용성별	
	DWORD	dwWeight;			// 무게	 
	DWORD	dwCost;				// 가격	 
	DWORD	dwCash;				// 게임머니	
	DWORD	dwEndurance;		// 내구력	
	DWORD	dwRepairNumber;		// 수리횟수
	BOOL	bRepairUnable;		// 수리불가능	
	BOOL	bJammed;			// 고장유무
	int		nLog;				// 로그
	int		nAbrasion;			// 마모율
	int		nHardness;			// 경도
	DWORD	dwHanded;			// 잡는형식	
	//DWORD	dwHeelH;			// 굽 높이			// raiders_060112 제거 	
	DWORD	dwFlag;				// 다목적 플래그	// raiders_060112 추가
	DWORD	dwParts;			// 장착위치	
	DWORD	dwPartsub;			// 장착위치	
	DWORD	bPartsFile;			// 파츠파일 사용여부 
	DWORD	dwExclusive;		// 제거될위치	
	DWORD	dwBasePartsIgnore;
	DWORD	dwItemLV;			// 아이템레벨	
	DWORD	dwAddLV;			// 추가레벨 
	DWORD	dwItemRare;			// 희귀성	
	DWORD   dwShopAble;
	DWORD	dwSpeedPenalty;		// 공격속도 감소	
	DWORD	dwLinkKindBullet;	// 필요 소모장비 아이템	
	DWORD	dwLinkKind;			// 필요 아이템종류
	DWORD	dwAbilityMin;		// 최소능력치 - 공격력, 방어력, 성능 기타등등 
	DWORD	dwAbilityMax;		// 최대능력치 - 공격력, 방어력, 성능 기타등등 
	BOOL	bCharged;		
	DWORD	dwSetItem;			// 세트아이템 식별.
	SAI79::ePropType	eItemType;
	short	wItemEatk;			// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )
	DWORD   dwParry;			// 회피율 
	DWORD   dwAtkRating;		// 사용하지 않는다.(04/11/22현재)
	DWORD   dwblockRating;		// 블럭 수치 
	int		nAddSkillMin;		// 최소 추가 스킬
	int		nAddSkillMax;		// 최대 추가 스킬.
	DWORD	dwAtkStyle;			// 공격 스타일 
	DWORD	dwAtkType;			// 타격방식	
	DWORD	dwWeaponType;		// 무기종류 
	DWORD	dwItemAtkOrder1;	// 아이템1차공격순서
	DWORD	dwItemAtkOrder2;	// 아이템2차공격순서
	DWORD	dwItemAtkOrder3;	// 아이템3차공격순서
	DWORD	dwItemAtkOrder4;	// 아이템4차공격순서
	DWORD	dwItemMotion1;		// 아이템1차공격동작
	DWORD	dwItemMotion2;		// 아이템2차공격동작
	DWORD	dwItemMotion3;		// 아이템3차공격동작
	DWORD	dwItemMotion4;		// 아이템4차공격동작
    DWORD	tmContinuousPain;	// 지속 피해 
	int		nShellQuantity;		// 최대장전개수	- IK가 채집도구일때는 최대 채집량이 된다.
	DWORD	dwRecoil;			// 반동	
	DWORD	dwCaliber;			// 구경	
	DWORD	dwBarrageRate;		// 연발률	
	DWORD	dwLoadingTime;		// 장전시간	- IK가 채집도구일때는 채집속도(능력)이 된다.
	LONG	nAdjHitRate;		// 추가공격성공률	
	LONG	nAdjSpellRate;		// 추가 마법성공률
	FLOAT	fAttackSpeed;		// 공격속도	
	DWORD	dwDmgShift;			// 타격시 반동	
	DWORD	dwAttackRange;		// 공격범위	
	int		nProbability;		// 적용확률
	DWORD	dwDestParam[2];		// 적용대상1	
	LONG	nAdjParamVal[2];	// 적용값1	
	DWORD	dwChgParamVal[2];	// 적용변화값1	
	int		nDestData1[3];		// 적용데이타값 3개, destParam1에만 해당됨.
	DWORD	dwActiveSkill;		// 발동 마법
	DWORD	dwActiveSkillLv;	// 발동 마법 레벨
	DWORD	dwActiveSkillRate;	// 발동 마법 확률.
	DWORD	dwReqJob;
	DWORD	dwReqStr;			// 필요힘	
	DWORD	dwReqSta;			// 필요체력	
	DWORD	dwReqDex;			// 필요민첩	
	DWORD	dwReqInt;			// 필요지능	
	DWORD	dwReqMp;			// 필요MP	
	DWORD	dwReqFp;			// 필요FP	
	DWORD	dwReqJobLV;
	DWORD	dwReqDisLV;
	DWORD   dwReSkill1;
	DWORD   dwReSkillLevel1;
	DWORD   dwReSkill2;
	DWORD   dwReSkillLevel2;
	DWORD   dwBaseSkill;
	DWORD	dwSkillReadyType;
	DWORD	dwSkillReady;		// 기술 준비시간	
	DWORD	_dwSkillRange;		// 기술 시전거리	
	DWORD	dwSfxElemental;
	DWORD	dwSfxObj;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj2;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj3;			// 발동효과, 타겟
	DWORD	dwSfxObj4;			// 지속되는 효과 이펙트.
	DWORD	dwSfxObj5;			// 발동효과, 시전자 
	DWORD	dwUseMotion;		// 사용 모션
	DWORD	dwCircleTime;		// 유지시간 
	DWORD	dwSkillTime;		// 지속시간	
	DWORD	dwExeChance;
	DWORD	dwExeTarget;
	DWORD	dwUseChance;
	DWORD	dwSpellRegion;		// 마법 범위 
	DWORD	dwSpellType;		// 마법속성	
	DWORD   dwReferStat1;
	DWORD   dwReferStat2;
	DWORD   dwReferTarget1;
	DWORD   dwReferTarget2;
	DWORD   dwReferValue1;
	DWORD   dwReferValue2;
	DWORD	dwSkillType;		// 기술속성	
	DWORD	dwItemResistMagic;	// 마법 저항
	int		nItemResistElecricity;
	int		nItemResistDark;
	int		nItemResistFire;
	int		nItemResistWind;
	int		nItemResistWater;
	int		nItemResistEarth;
	BOOL	bAmplification;		// 마법증폭	
	BOOL	bFusion;			// 마법퓨전	
	LONG	nEvildoing;			// 악행	
	BOOL	bGrow;				// 성장유무	
	BOOL	bCraft;				// 생산유무 
	DWORD	dwExpertLV;	
	DWORD	dwExpertMax;		// 최대숙련레벨 
	DWORD	dwSubDefine;
	DWORD	dwExp;				// 현재경험치	
	DWORD	LVRatio;			// 기술 숙련치 
	DWORD	dwComboStyle;
	DWORD	dwComboSkillTime;
	DWORD	dwCardType;			// 카드타입	
	FLOAT	fFlightSpeed;		// 비행추진력(속도)
	FLOAT	fFlightLRAngle;		// 좌우 턴 각도.
	FLOAT	fFlightTBAngle;		// 상하 턴 각도.
	DWORD	dwFlightLimit;		// 비행제한레벨
	DWORD	dwFFuelReMax;		// 비행연료충전량
	DWORD	dwFFuelSp;			// 비행연료소비량
	DWORD	dwAFuelReMax;		// 가속연료충전량
	DWORD   dwFuelRe;			// 팩당연료충전량
	DWORD	dwLimitJob1;		// 직업제한1
	DWORD	dwLimitLevel1;		// 제한레벨1
	DWORD	dwLimitJob2;		// 직업제한2
	DWORD	dwLimitLevel2;		// 제한레벨2
	int		nReflect;			// 리플렉션 옵션.
	TCHAR	szCommand[128];		// 설명문 
	DWORD	dwSndAttack1;		// 효과음 : 공격 1
	DWORD	dwSndAttack2;		// 효과음 : 공격 2
	DWORD	dwSndAttack3;		// 효과음 : 공격 3
	DWORD	dwSndAttack4;		// 효과음 : 공격 4
	DWORD	dwSndDefense;		// 효과음 : 방어
	DWORD	dwSndHit;			// 효과음 : 방어
	DWORD	dwSndDamage;		// 효과음 : 피해 
	DWORD   dwSndClang;			// 효과음 : 흔들림 
    TCHAR	szIcon[64];			// 효과음 : 피해 
#ifdef __V050322_CACHEQUEST
	DWORD	dwQuestId;
	TCHAR	szTextFileName[ 64 ];
#endif

	DWORD	GetCoolTime()  { return dwSkillReady; }
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

#define	MAX_DROPITEM	160
#define MAX_QUESTITEM	16
#define	MAX_DROPKIND	80

class CDropItemGenerator
{
private:
	DWORD	m_dwSize;
	DROPITEM	m_lpDropItem[MAX_DROPITEM];
public:
	DWORD	m_dwMax;
public:
//	Contructions
	CDropItemGenerator()
		{	m_dwSize	= m_dwMax	= 0;	}
	~CDropItemGenerator()	{}
//	Operations
	void	AddTail( CONST DROPITEM & rDropItem );
	DWORD	GetSize( void )	{	return m_dwSize;	}
	DROPITEM*	GetAt( int nIndex, BOOL bUniqueMode, BOOL fProbability = FALSE );
};

class CDropKindGenerator
{
private:
	int		m_nSize;
	DROPKIND	m_aDropKind[MAX_DROPKIND];
public:
//	Contructions
	CDropKindGenerator()
		{	m_nSize	= 0;	}
	~CDropKindGenerator()	{}
//	Operations
	void	AddTail( const DROPKIND & rDropKind );	// memcpy
	int		GetSize( void )		{	return m_nSize;	}
	LPDROPKIND	GetAt( int nIndex );
};

class CQuestItemGenerator
{
private:
	unsigned	m_uSize;
	QUESTITEM	m_pQuestItem[MAX_QUESTITEM];
public:
	CQuestItemGenerator()
		{	m_uSize	= 0;	}
	~CQuestItemGenerator()	{}
	void	AddTail( const QUESTITEM & rQuestItem );
	unsigned	GetSize( void )	{	return m_uSize;	}
	QUESTITEM*	GetAt( int nIndex );
};
/*----------------------------------------------------------------------------------------------------*/

struct MoverProp : CtrlProp
{
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
	DWORD	dwVerticalRate;		// 세로비율,
	DWORD	dwDiagonalRate;		// 사선비율,
	DWORD	dwThrustRate;	// 찌르기비율,

	DWORD	dwChestRate;
	DWORD	dwHeadRate;  
	DWORD	dwArmRate;
	DWORD	dwLegRate;

	DWORD	dwAttackSpeed;	// 공격속도,
	DWORD	dwReAttackDelay;
	DWORD	dwAddHp;		// ,
#ifdef __X1014_PROP
	DWORD	dwAddMp;		// ,
#endif
	DWORD	dwNaturalArmor;	// 자연방어력 
	int		nAbrasion;	// 마모도
	int		nHardness;	// 경도
	DWORD	dwAdjAtkDelay;	// 추가공격속도저하,

	SAI79::ePropType	eElementType;
	short				wElementAtk;		// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )

//	DWORD	dwBloodColor;	// 피색,
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

	//TCHAR	szSndAttack[64];	// 효과음 : 공격 
	//TCHAR	szSndDamage[64];	// 효과음 : 피해
	//TCHAR	szSndDie[64];	// 효과음 : 죽음 
	//TCHAR	szSndRoar[64];	// 효과음 : 울부짖음 

	DWORD   dwSndAtk1  ;
	DWORD   dwSndAtk2  ;
	DWORD   dwSndDie1  ;
	DWORD   dwSndDie2  ;
	DWORD   dwSndDmg1  ;
	DWORD   dwSndDmg2  ;
	DWORD   dwSndDmg3  ;
	DWORD   dwSndIdle1 ;
	DWORD   dwSndIdle2 ;

	/*
	TCHAR   szSndAttack [ 64 ];
	TCHAR   szSndAtk2 [ 64 ];
	TCHAR   szSndDie  [ 64 ];
	TCHAR   szSndDie2 [ 64 ];
	TCHAR   szSndDamage[ 64 ];
	TCHAR   szSndDmg2 [ 64 ];
	TCHAR   szSndDmg3 [ 64 ];
	TCHAR   szSndRoar [ 64 ];
	TCHAR   szSndIdle2[ 64 ];
	*/

	// new
	short   m_nEvasionHP;
	short	m_nEvasionSec;

	short   m_nRunawayHP          ; // HP가 10 이하면 도주 

	short   m_nCallHelperMax       ; // 총 갯수 
	short   m_nCallHP              ; // 도움요청하기 위한 HP
	short   m_nCallHelperIdx  [ 5 ]; // 도움요청하기 Id
	short   m_nCallHelperNum  [ 5 ]; // 도움요청하기 Id
	short   m_bCallHelperParty[ 5 ]; // 도움요청하기 Id

	//DWORD	m_nRunawayHP;
	//DWORD	m_bRunawayToHelper;
	//DWORD	m_nCallHelperHP;
	//DWORD	m_nCallHelperSec;
	//DWORD	m_nCallHelperId;
	short   m_dwAttackMoveDelay;
	short   m_dwRunawayDelay;
	short   m_bPartyAttack;
	short   m_nHelperNum;

#ifdef __XAI1013
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
#ifdef __V050324_AISCAN
	DWORD   m_dwScanQuestId; 
	DWORD   m_dwScanItemIdx; 
#endif
	
#ifdef __NPP_050308
	int		m_nScanChao;		// 카오, 비카오 검색
#endif //__NPP_050308
	
#endif // xAI1013

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
	unsigned	uSize;
	D3DXVECTOR3 avPos[MAX_BEGINPOS];
}
BEGINPOSARR, *LPBEGINPOSARR;

#ifdef __J0103
typedef	struct	_ExpCompany
{
	int		nExp;
	int		nPenya;
	int		cbMax;
}
ExpCompany, *PExpCompany;
#define	MAX_COMPANYLEVEL	25
#endif	// __J0103

#define TASKBAR_TOP    0
#define TASKBAR_BOTTOM 1
#define TASKBAR_LEFT   2
#define TASKBAR_RIGHT  3

// short cut
#define MAX_SLOT_APPLET 18
#define MAX_SLOT_ITEM   9
#define MAX_SLOT_QUEUE  5 
#define	MAX_SLOT_ITEM_COUNT		4

#endif // __DAISY

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


class CWndBase;
typedef struct tagSHORTCUT
{
#if defined ( __CLIENT ) || defined(__DAISY )
	CWndBase* m_pFromWnd   ;
	CTexture* m_pTexture   ;
#endif	// __CLIENT
	DWORD     m_dwShortcut ; 
	DWORD     m_dwId       ; 
	DWORD     m_dwType     ; 
	DWORD     m_dwIndex    ; 
	DWORD     m_dwUserId   ; 
	DWORD     m_dwData     ; 
#ifdef __DAISY
	TCHAR     m_szString[128]; // daisy일 경우는 고정 값으로
#else
	TCHAR     m_szString[MAX_SHORTCUT_STRING]; // SHORTCUT_CHAT일 경우 저장.
#endif
	BOOL IsEmpty() { return m_dwShortcut == SHORTCUT_NONE; }
	void Empty() { m_dwShortcut = SHORTCUT_NONE; }
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
#ifdef __S0517
	CTime	EndTime;
#else // __S0517
	TCHAR strTime[32];
#endif // __S0517
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