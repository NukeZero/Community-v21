#ifndef __FLSPECSTRUCT_H__
#define __FLSPECSTRUCT_H__


#include "../../_Common/data.h"


typedef struct _T_ITEM_SPEC
{
	int		nVer;							// 버전
	DWORD	dwID;							// 변수명	
	TCHAR	szName[ ITEM_NAME_BUFFER_LEN ];	// 한글명칭 
	DWORD	dwNum;							// 기본생성개수	
	DWORD	dwPackMax;						// 최대곂침개수	
	
	DWORD	dwItemKind1;					// 1차아이템종류, Skill: 직업 스킬 종류?(방랑자, 1,2,3차 직업스킬)
	DWORD	dwItemKind2;					// 2차아이템종류	
	DWORD	dwItemKind3;					// 3차아이템종류	
	DWORD	dwItemJob;						// 아이템직업소속 
	BOOL	bPermanence;					// 소비불가	
	
	DWORD	dwUseable;						// 사용시점	
	DWORD	dwItemSex;						// 사용성별	
	DWORD	dwCost;							// 가격	 
	DWORD	dwEndurance;					// 내구력	
	int		nLog;							// 로그
	
	int		nAbrasion;						// 마모율
	int		nMaxRepair;						// 수리횟수
	DWORD	dwHanded;						// 잡는형식	
	DWORD	dwFlag;							// 다목적 플래그	
	DWORD	dwParts;						// 장착위치	
	
	DWORD	dwPartsub;						// 장착위치	
	BOOL	bPartsFile;						// 파츠파일 사용여부 
	DWORD	dwExclusive;					// 제거될위치	
	DWORD	dwBasePartsIgnore;
	DWORD	dwItemLV;						// 아이템레벨	
	
	DWORD	dwItemRare;						// 희귀성	
	DWORD   dwShopAble;
	int		nShellQuantity;					// 최대장전개수	- IK가 채집도구일때는 최대 채집량이 된다.(삭제 예정)
	DWORD	dwActiveSkillLv;				// 발동 마법 레벨
	DWORD   dwFuelRe;						// 팩당연료충전량
	
	DWORD	dwAFuelReMax;					// 가속연료충전량
	DWORD	dwSpellType;					// 마법속성	
	DWORD	dwLinkKindBullet;				// 필요 소모장비 아이템	
	DWORD	dwLinkKind;						// 필요 아이템종류
	DWORD	dwAbilityMin;					// 최소능력치 - 공격력, 방어력, 성능 기타등등 
	
	DWORD	dwAbilityMax;					// 최대능력치 - 공격력, 방어력, 성능 기타등등 
	BOOL	bCharged;		
	SAI79::ePropType	eItemType;
	short	wItemEatk;						// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )
	DWORD   dwParry;						// 회피율 
	
	DWORD   dwblockRating;					// 블럭 수치 
	int		nAddSkillMin;					// 최소 추가 스킬
	int		nAddSkillMax;					// 최대 추가 스킬.
	DWORD	dwAtkStyle;						// 공격 스타일 
	DWORD	dwWeaponType;					// 무기종류 
	
	DWORD	dwItemAtkOrder1;				// 아이템1차공격순서
	DWORD	dwItemAtkOrder2;				// 아이템2차공격순서
	DWORD	dwItemAtkOrder3;				// 아이템3차공격순서
	DWORD	dwItemAtkOrder4;				// 아이템4차공격순서
	DWORD	tmContinuousPain;				// 지속 피해 
	
	DWORD	dwRecoil;						// 반동	
	DWORD	dwLoadingTime;					// 장전시간	- IK가 채집도구일때는 채집속도(능력)이 된다.
	LONG	nAdjHitRate;					// 추가공격성공률	
	FLOAT	fAttackSpeed;					// 공격속도	
	DWORD	dwDmgShift;						// 타격시 반동	
	
	DWORD	dwAttackRange;					// 공격범위	
	int		nProbability;					// 적용확률

	DWORD	dwDestParam[ MAX_ITEM_PARAM ];					// 적용대상1		// TODO : 
	LONG	nAdjParamVal[ MAX_ITEM_PARAM ];				// 적용값1	
	DWORD	dwChgParamVal[ MAX_ITEM_PARAM ];				// 적용변화값1	
	int		nDestData1[ MAX_ITEM_PARAM ];					// 적용데이타값 3개, destParam1에만 해당됨.
	
	DWORD	dwActiveSkill;					// 발동 마법
	DWORD	dwActiveSkillRate;				// 발동 마법 확률.
	DWORD	dwReqMp;						// 필요MP	
	DWORD	dwReqFp;						// 필요FP
	
	DWORD	dwReqDisLV;						// 필요 레벨
	DWORD   dwReSkill1;						// 선행 스킬1
	DWORD   dwReSkillLevel1;				// 선행 스킬1 필요 레벨
	DWORD   dwReSkill2;						// 선행 스킬2
	DWORD   dwReSkillLevel2;				// 선행 스킬2 필요 레벨
	
	DWORD	dwSkillReadyType;
	DWORD	dwSkillReady;					// 기술 준비시간	
	DWORD	_dwSkillRange;					// 기술 시전거리	
	DWORD	dwSfxElemental;
	DWORD	dwSfxObj;						// 생성할 효과 오브젝트 
	
	DWORD	dwSfxObj2;						// 생성할 효과 오브젝트 
	DWORD	dwSfxObj3;						// 발동효과, 타겟
	DWORD	dwSfxObj4;						// 지속되는 효과 이펙트.
	DWORD	dwSfxObj5;						// 발동효과, 시전자 
	DWORD	dwUseMotion;					// 사용 모션
	
	DWORD	dwCircleTime;					// 유지시간 
	DWORD	dwSkillTime;					// 지속시간	
	DWORD	dwExeTarget;					// 발동 대상
	DWORD	dwUseChance;					// 사용시기?
	DWORD	dwSpellRegion;					// 마법 범위 
	
	DWORD   dwReferStat1;
	DWORD   dwReferStat2;
	DWORD   dwReferTarget1;					// 변환될 무기설정
	DWORD   dwReferTarget2;					// 변환될 확률 설정		// 변환할때 소비되는 페냐
	DWORD   dwReferValue1;					// 레드칩
	
	DWORD   dwReferValue2;					// 참고수치
	DWORD	dwSkillType;					// 기술속성	
	int		nItemResistElecricity;
	int		nItemResistFire;
	
	int		nItemResistWind;
	int		nItemResistWater;
	int		nItemResistEarth;
	LONG	nEvildoing;						// 악행	
	DWORD	dwExpertLV;	
	
	DWORD	dwExpertMax;					// 최대숙련레벨
	DWORD	dwSubDefine;
	DWORD	dwExp;							// 현재경험치	
	DWORD	dwComboStyle;
	FLOAT	fFlightSpeed;					// 비행추진력(속도)
	
	FLOAT	fFlightLRAngle;					// 좌우 턴 각도.
	FLOAT	fFlightTBAngle;					// 상하 턴 각도.
	DWORD	dwFlightLimit;					// 비행제한레벨
	DWORD	dwFFuelReMax;					// 비행연료충전량
	DWORD	dwLimitLevel1;					// 제한레벨1		<< 사용하지 않음
	
	int		nReflect;						// 리플렉션 옵션.
	DWORD	dwSndAttack1;					// 효과음 : 공격 1
	DWORD	dwSndAttack2;					// 효과음 : 공격 2
	DWORD	dwQuestId;
	TCHAR	szTextFileName[64];				// item에 GM command에 넣는 것에 사용

	TCHAR	szIcon[64];						// dds파일 이름 
	TCHAR	szCommand[256];					// 설명문 

//////////////////////////////////////////////////////////////////////////
// 추가 컬럼...
//////////////////////////////////////////////////////////////////////////

	int		nMinLimitLevel;					// 최소 사용 레벨(<=)
	int		nMaxLimitLevel;					// 최대 사용 레벨(<=)


	int		nItemGroup;						// 아이템 그룹
	int		nUseLimitGroup;					// 사용 제한 그룹

	int		nMaxDuplication;				// 중첩 사용 개수
	int		nEffectValue;					// 효과 적용 수치
	int		nTargetMinEnchant;				// 사용 대상 최소 제련 수치(<=)
	int		nTargetMaxEnchant;				// 사용 대상 최대 제련 수치(<=)

	BOOL	bResetBind;						// 귀속 해제 가능 여부
	int		nBindCondition;					// 현재 귀속 상태
	int		nResetBindCondition;			// 귀속 해제 사용 후 귀속 상태

	DWORD	dwHitActiveSkillId;				// 타격 시 발동될 스킬 인덱스
	DWORD	dwHitActiveSkillLv;				// 타격 시 발동될 스킬 레벨
	DWORD	dwHitActiveSkillProb;			// 타격 시 발동될 스킬 확률
	DWORD	dwHitActiveSkillTarget;			// 타격 시 발동될 스킬 대상

	DWORD	dwDamageActiveSkillId;			// 데미지 입었을 시 발동될 스킬 인덱스
	DWORD	dwDamageActiveSkillLv;			// 데미지 입었을 시 발동될 스킬 레벨
	DWORD	dwDamageActiveSkillProb;		// 데미지 입었을 시 발동될 스킬 확률
	DWORD	dwDamageActiveSkillTarget;		// 데미지 입었을 시 발동될 스킬 대상

	DWORD	dwEquipActiveSkillId;			// 장착 시 발동될 스킬 인덱스
	DWORD	dwEquipActiveSkillLv;			// 장착 시 발동될 스킬 레벨

	DWORD	dwGeneralEnchantMode;			// 일반 제련 가능 여부( 1 : 가능, 0 : 불가능, = : 데이터 없음..tooltip 때문에..?)
	DWORD	dwAttributeEnchantMode;			// 속성 제련 가능 여부
	DWORD	dwGemEnchantMode;				// 보석 제련 가능 여부?
	DWORD	dwPiercingMode;					// 피어싱 가능 여부
	DWORD	dwRandomOptionMode;				// 각성 가능 여부

	BOOL	bAbsoluteTime;					// 시간제 아이템 절대시간 사용 유무

	DWORD	dwItemGrade;					// 아이템 등급
	BOOL	bCanTrade;						// 거래 가능 여부
	DWORD	dwMainCategory;					// 아이템 분류 1차
	DWORD	dwSubCategory;					// 아이템 분류 2차

	BOOL	bCanHaveServerTransform;		// 서버군 이동 시 가져갈 수 있는 아이템인가

	BOOL	bCanSavePotion;					// 저장 할수 있는 포션인지.

	BOOL	bCanLooksChange;				// 외형 변경 가능한 아이템인가
	BOOL	bIsLooksChangeMaterial;			// 외형 변경에 사용될 수 있는 재료 인가

	BOOL	bCanSellNPC;					// NPC 상점에 판매 가능한가


	//////////////////////////////////////////////////////////////////////////
	DWORD	GetCoolTime() const;
	BOOL	IsUltimate() const;
	BOOL	IsBaruna() const;
	BOOL	IsBarunaWeapon() const;
	BOOL	IsBarunaArmor() const;
#ifdef BARUNA_ULTIMATE_UPDATE
	BOOL	IsBarunaUltimate() const;
#endif
	BOOL	IsVisPet() const;
	BOOL	IsVis() const;
	BOOL	IsVisKey() const;
	BOOL	IsPickupToBuff() const;
	BOOL	IsGold() const;
	int		GetInventoryType() const;
	BOOL	IsUpgradeAble( int nUpgradeType ) const;
	BOOL	IsAbsoluteTime() const;
	BOOL	IsDebuff() const;

	BOOL	IsAccessory() const;
	BOOL	IsCollector() const;
	BOOL	IsWeaponParts() const;
	BOOL	IsArmorParts() const;
	BOOL	IsCostumeParts() const;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	BOOL	IsCostumeEnhanceParts() const;
#endif
	BOOL	IsSystemPet() const;
	BOOL	IsEatPet() const;
	//////////////////////////////////////////////////////////////////////////

} T_ITEM_SPEC, *PT_ITEM_SPEC;



//////////////////////////////////////////////////////////////////////////
// 텔레포트 지도
enum
{
	TELEPORT_MAP_TYPE_SPEC,	// 스펙에 등록된 지점
	TELEPORT_MAP_TYPE_USER,	// 유저 등록 지점
};


#ifdef __DBSERVER
#include "ProjectCmn.h"
#endif
#pragma pack( 1 )
typedef	struct	_T_WORLD_POSITION
{
	DWORD		dwWorldID;		// 월드 ID
#if !defined (__CLIENT) && !defined(__WORLDSERVER) && !defined(__DBSERVER)
	vector3 tPos;
#else
	D3DXVECTOR3	tPos;			// 위치
#endif
	int			layer;

	_T_WORLD_POSITION() : dwWorldID( 0 ), tPos( 0.0F, 0.0F, 0.0F ), layer( 0 )
	{
	}

} T_WORLD_POSITION, * PT_WORLD_POSITION;


typedef	struct	_T_TELEPORTMAP_POINT
{
	DWORD				dwIndex;		// 인덱스
	BYTE				byType;			// 텔레포트 타입

	T_WORLD_POSITION	tPosition;		// 위치

	TCHAR				szName[32+1];	// 이름(설명) // 주의 MAX_TELEPORT_MAP_USER_POINT_NAME

} T_TELEPORTMAP_POINT, * PT_TELEPORTMAP_POINT;

#pragma pack()

typedef struct _T_WORLD_EXPAND_LAYER
{
	DWORD			dwWorldID;			// 월드 인덱스
	BYTE			byExpandLayerCount;	// 확장할 레이어 수

} T_WORLD_EXPAND_LAYER, *PT_WORLD_EXPAND_LAYER;

typedef struct _T_CHARGE_ZONE_WORLD
{
	DWORD			dwIndex;			// 인덱스
	DWORD			dwWorldID;			// 월드 인덱스
	BYTE			byChannelCount;		// 채널 수
#if !defined (__CLIENT) && !defined(__WORLDSERVER) 
	vector3		kStartingPos;		// 입장 위치
#else
	D3DXVECTOR3		kStartingPos;		// 입장 위치
#endif
	TCHAR			szWorldName[128+1];	// 월드명

} T_CHARGE_ZONE_WORLD, *PT_CHARGE_ZONE_WORLD;

typedef struct	_T_CHARGE_ZONE_TICKET
{
	DWORD			dwItemID;			// 유료 지역 입장권 아이템 인덱스
	DWORD			dwPointID;			// 유료 지역 월드 데이터 인덱스

} T_CHARGE_ZONE_TICKET, *PT_CHARGE_ZONE_TICKET;


enum
{
	TREASURE_KEY_LOW_LEVEL = 0
	, TREASURE_KEY_MEDIUM_LEVEL
	, TREASURE_KEY_SUPERIOR_LEVEL
	, TREASURE_KEY_MAX
};

typedef struct	_T_TREASURE_CHEST
{
	DWORD		dwItemID;							// 보물 상자 아이템 인덱스
	DWORD		arrKeyItemID[ TREASURE_KEY_MAX ];	// 보물 상자 열쇠 아이템 인덱스

} T_TREASURE_CHEST, *PT_TREASURE_CHEST;

typedef struct	_T_TREASURE_KEY
{
	DWORD		dwItemID;					// 보물 상자 열쇠 아이템 인덱스
	DWORD		dwTreasureItemID;			// 보물 아이템 인덱스
	DWORD		dwProb;						// 보물 아이템 확률
	DWORD		dwMaxQuantity;				// 보물 아이템 최대 수량
	DWORD		dwLoopCount;				// 굴림 횟수
	DWORD		dwLimitCount;				// 제한 수량
	bool		bNotice;					// 알림 여부

} T_TREASURE_KEY, *PT_TREASURE_KEY;

typedef struct	_T_TREASURE_ITEM
{
	DWORD		dwItemID;
	DWORD		dwQuantity;
	bool		bNotice;

} T_TREASURE_ITEM, *PT_TREASURE_ITEM;


// imParkth - 2012. 05. 17 - FLSpecStruct.h - Struct
typedef struct	_T_VIS_GROUP_LIST
{
	DWORD		dwVisGroupID;							// 비스 그룹
	DWORD		dwGroupName;							// 비스 아이템명
	DWORD		dwVisID[MAX_VIS_GROUP_ITEM_COUNT];		// 해당 그룹의 아이템들

} T_VIS_GROUP_LIST, *PT_VIS_GROUP_LIST;

typedef struct	_T_VIS_GROUP_LIST_BY_ITEM_ID
{
	DWORD		dwVisItemID;							// 해당 그룹의 아이템
	DWORD		dwVisGroupID;							// 비스 그룹

} T_VIS_GROUP_LIST_BY_ITEM_ID, *PT_VIS_GROUP_LIST_BY_ITEM_ID;

typedef struct	_T_VIS_CHECK_LIST
{
	DWORD		dwVisItemID;							// 비스 아이템 ID
	DWORD		dwGroupID[MAX_VIS_CONDITION_COUNT];		// 선행 조건의 그룹들 ( AND 조건 )

} T_VIS_CHECK_LIST, *PT_VIS_CHECK_LIST;


#endif // __FLSPECSTRUCT_H__

