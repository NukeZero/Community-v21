#if !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
#define AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../_CommonDefine/Spec/FLSpecStruct.h"
#include <defineNeuz.h>
#include "ModelMng.h"
#include "TerrainMng.h"
#include <ObjMap.h>
#include "ProjectCmn.h"
#include "Script.h" 
#include "guildquest.h"

#include "partyquest.h"
#include <set>

#include "MiniGame.h"		//sun: 10, __EVE_MINIGAME

#include "UltimateWeapon.h"	//sun: 9,10차 제련 __ULTIMATE

#if defined(__WORLDSERVER) // __WORLDSERVER
#include "EventLua.h"
#endif // __WORLDSERVER

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapInformationManager.h"
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

////////////////////////////////////////////////////////////////////////////////////////////////////
// define  
////////////////////////////////////////////////////////////////////////////////////////////////////

#define		MAX_RESPAWN			1536
#define		MAX_MONSTER_PROP	1024
#define		MAX_GUILDAPPELL		5

////////////////////////////////////////////////////////////////////////////////////////////////////
// extern 
////////////////////////////////////////////////////////////////////////////////////////////////////

extern CString GetLangFileName( int nLang, int nType );

////////////////////////////////////////////////////////////////////////////////////////////////////
// struct
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_QUESTCONDITEM  128
#define MAX_QUESTREMOVE    12

const size_t	MAX_QUEST_REWARD_BUFF	= 20;

struct QuestState
{
	TCHAR	m_szDesc[ 512 ];
};

struct QuestGoalData
{
	QuestGoalData( void );

	DWORD	m_dwGoalIndex;
	DWORD	m_dwGoalWorldID;
	FLOAT	m_fGoalPositionX;
	FLOAT	m_fGoalPositionZ;
	int		m_nOwnMonster;
	DWORD	m_dwGoalTextID;
};

struct QuestPropItem 
{
	char    m_nSex;
	char	m_nType; 
	int     m_nJobOrItem;
	int		m_nItemIdx;
	int		m_nItemNum;
	int		m_nAbilityOption;

	BYTE	m_byFlag;	//sun: 13, __CHIPI_QUESTITEM_FLAG

	QuestGoalData m_ItemGoalData;
};

typedef struct _T_SKILL_BASE_DATA
{
	DWORD dwSkillID;
	DWORD dwSkillLevel;

	_T_SKILL_BASE_DATA() : dwSkillID( 0 ), dwSkillLevel( 0 )
	{
	}

} T_SKILL_BASE_DATA, *PT_SKILL_BASE_DATA;

struct QuestProp
{
	WORD	m_wId;
	TCHAR	m_szTitle[ 128 ];
	TCHAR	m_szNpcName[ 32 ];
	WORD	m_nHeadQuest;
	CHAR	m_nQuestType;
	bool	m_bNoRemove; // TRUE일 경우 삭제 불가 

	// 시작 - 조건 
	int		m_nParam[ 4 ];
	char    m_nBeginCondPreviousQuestType;
	WORD	m_anBeginCondPreviousQuest[ 6 ];
	WORD	m_anBeginCondExclusiveQuest[ 6 ];
	char	m_nBeginCondJob[ MAX_JOB ];
	char	m_nBeginCondJobNum;
	BYTE	m_nBeginCondLevelMax;
	BYTE	m_nBeginCondLevelMin;
	char	m_nBeginCondParty; 
	char 	m_nBeginCondPartyNumComp; 
	char	m_nBeginCondPartyNum; 
	char	m_nBeginCondPartyLeader; 
	char	m_nBeginCondGuild; 
	char 	m_nBeginCondGuildNumComp; 
	WORD	m_nBeginCondGuildNum; 
	char	m_nBeginCondGuildLeader; 
	QuestPropItem* m_paBeginCondItem; 
	char	m_nBeginCondItemNum;
	char	m_nBeginCondSex; 
	int		m_nBeginCondSkillIdx;
	char	m_nBeginCondSkillLvl;
//sun: 8, // __S8_PK
	QuestPropItem* m_paBeginCondNotItem; 
	char	m_nBeginCondNotItemNum;
	int		m_nBeginCondPKValue;
	int		m_nBeginCondDisguiseMoverIndex;
	// 시작 - 추가 
	int  	m_nBeginSetAddItemIdx[ 4 ];
	int		m_nBeginSetAddItemNum[ 4 ];
	int		m_nBeginSetAddGold;
	int     m_nBeginSetDisguiseMoverIndex;
//sun: 9, 9-10차 펫
	int		m_nBeginCondPetLevel;
	int		m_nBeginCondPetExp;

	int		m_nBeginCondTutorialState;		//sun: 12, 튜토리얼 개선

	int		m_nBeginCondTSP;

#ifdef __GUILD_HOUSE_MIDDLE

	int		m_nBeginCondGuildWarWin;		// 길드워 승수..

#endif // __GUILD_HOUSE_MIDDLE

	//_SUN_RENDER_QUESTEMOTICON
	QuestGoalData m_kBeginNpcData;
	char m_szCharacterKey[64];

	// 종료 - 조건  
	int     m_nEndCondLimitTime; //  퀘스트 수행 제한 시간 
	QuestPropItem* m_paEndCondItem; 
	char	m_nEndCondItemNum;
	int     m_nEndCondKillNPCIdx[ MAX_QUEST_KILL_NPC_COUNT ]; // 죽여야할 NPC 인덱스 - 총 2개 
	int     m_nEndCondKillNPCNum[ MAX_QUEST_KILL_NPC_COUNT ]; // 죽여야할 NPC 갯수 - 총 2개    
	QuestGoalData m_KillNPCGoalData[ MAX_QUEST_KILL_NPC_COUNT ]; // 죽여야 할 NPC 목표 데이터
	DWORD   m_dwEndCondPatrolWorld; // 정찰해야될 맵 
	DWORD	m_dwPatrolDestinationID;	// 정찰 목적지 이름을 찾기 위한 ID
	QuestGoalData m_PatrolWorldGoalData; // 정찰해야 할 지역 목표 데이터
	CRect   m_rectEndCondPatrol; // 정찰해야될 장소 영역 
	CHAR    m_szEndCondCharacter[64]; // 퀘스트를 완수를 판단할 캐릭터 키(NULL이면 자신)
	QuestGoalData m_MeetCharacterGoalData; // 퀘스트 완수를 판단할 캐릭터 목표 데이터
	CHAR*   m_lpszEndCondMultiCharacter; 
	int		m_nEndCondSkillIdx;
	char	m_nEndCondSkillLvl;
//sun: 8, // __S8_PK
	QuestPropItem* m_paEndCondOneItem;
	char	m_nEndCondOneItemNum;
	int		m_nEndCondGold;
	BYTE	m_nEndCondLevelMin;	
	BYTE	m_nEndCondLevelMax;
	//sun: 10차 전승시스템	Neuz, World, Trans
	int		m_nEndCondExpPercentMin;	
	int		m_nEndCondExpPercentMax;

//sun: 9, 9-10차 펫
	int		m_nEndCondPetLevel;
	int		m_nEndCondPetExp;

	int		m_nEndCondDisguiseMoverIndex; // 변신 
	char	m_nEndCondParty; // 파티 여부 (솔로,파티,길드)  
	char 	m_nEndCondPartyNumComp; 
	WORD 	m_nEndCondPartyNum; 
	char	m_nEndCondPartyLeader;
	char	m_nEndCondGuild; 
	char 	m_nEndCondGuildNumComp; 
	WORD	m_nEndCondGuildNum; 
	char	m_nEndCondGuildLeader; 
	BYTE	m_nEndCondState; // 스테이트 여부  
	BYTE    m_nEndCondCompleteQuestOper; // 0 = or, 1 = and
	WORD	m_nEndCondCompleteQuest[ 6 ]; // 완료 퀘스트 여부. 시나리오 퀘스트에 필요 
	CHAR    m_szEndCondDlgCharKey[ 64 ];
	QuestGoalData m_DialogCharacterGoalData; // 대화해야 할 캐릭터 목표 데이터
	CHAR    m_szEndCondDlgAddKey[ 64 ];
	CHAR    m_szPatrolZoneName[ 64 ];
	int		m_nEndCondTSP;

#ifdef __GUILD_HOUSE_MIDDLE

	int		m_nEndCondGuildWarWin;		// 길드워 승수..

#endif // __GUILD_HOUSE_MIDDLE

	// EndDialog 대화시 지급할 아이템(일종의 보상)
	int		m_nDlgRewardItemIdx[ 4 ];
	int		m_nDlgRewardItemNum[ 4 ];
	// 종료 - 제거 
	DWORD		m_dwEndRemoveItemIdx[ 8 ];
	int		m_nEndRemoveItemNum[ 8 ];
	int		m_nEndRemoveGold;
	int		m_anEndRemoveQuest[ MAX_QUESTREMOVE ];
	int		m_nEndRemoveTSP;
	// 보상 
	QuestPropItem* m_paEndRewardItem; 
	int		m_nEndRewardItemNum;
	int		m_nEndRewardGoldMin;
	int		m_nEndRewardGoldMax;
	EXPINTEGER		m_n64EndRewardExpMin;
	EXPINTEGER		m_n64EndRewardExpMax;
//sun: 8, // __S8_PK
	int		m_nEndRewardPKValueMin;
	int		m_nEndRewardPKValueMax;
//sun: 9차 추가분
	int		m_nEndRewardTeleport;
	D3DXVECTOR3		m_nEndRewardTeleportPos;

	BOOL	m_bEndRewardPetLevelup;	//sun: 9, 9-10차 펫

	int		m_nEndRewardSkillPoint;
	bool    m_bEndRewardHide; // 보상 목록을 보일지 여부
	bool    m_bRepeat;
	int		m_nEndRewardTSP;

	T_SKILL_BASE_DATA	m_kEndRewardSkillBuff[ MAX_QUEST_REWARD_BUFF ];

#if defined( __WORLDSERVER ) 
	CHAR*		m_apQuestDialog[ 32 ];
#endif
	QuestState*	m_questState[ 16 ];
#ifdef DAILY_QUEST
	int		m_nCheckBuffIndex;		// 퀘스트 진행 시 중복 버프 인덱스(해당 버프가 있으면 퀘스트 진행 불가)
	int		m_nRewardBuffIndex;			// 퀘스트 보상 버프 인덱스
	int		m_nRewardBuffTime;			// 퀘스트 보상 버프 유지시간
#endif // DAILY_QUEST
};

typedef struct 
{
	TCHAR	m_szSrc[ 64 ];
	TCHAR	m_szDst[ 64 ];

} FILTER,* LPFILTER;

typedef struct tagJOB
{
	TCHAR	szName[ 32 ];
	TCHAR	szEName[ 32 ];
	DWORD	dwJobBase;
	DWORD	dwJobType;

} JOB,* LPJOB;

typedef struct tagSTRUCTURE
{
	TCHAR	szName[ 32 ];
	TCHAR	szEName[ 32 ];
	
} STRUCTURE,* LPSTRUCTURE;


typedef struct tagGUILD_APPELL
{
	TCHAR	szName[ 32 ];
	TCHAR	szEName[ 32 ];
	
} GUILD_APPELL,* LPGUILD_APPELL;

typedef struct tagFxpCharacter
{
	DWORD	dwFxp;
	DWORD	dwFxp2;
} FXPCHARACTER,* LPFXPCHARACTER;


typedef struct tagATKSTYLE
{
	DWORD	dwParam1, dwParam2;

} ATKSTYLE,* LPATKSTYLE;

typedef struct tagADDEXPPARTY
{
	DWORD	Exp, Level;	//  협동 추가 경험치 / 제공레벨
} ADDEXPPARTY,* LPADDEXPPARTY;



struct MotionProp
{
	DWORD	dwID;   
	DWORD	dwAction;
	DWORD	dwMotion;
	DWORD	dwPlay;				// 0:1play    1:cont		2:loop
	DWORD	dwRequireLv;		// 여구 레벨 
	DWORD	dwRequireExp;		// 요구 경험치 
	TCHAR	szName[ 32 ];		// 표시 단어 
	TCHAR	szRoot[ 32 ];		// 루트 명칭 
	TCHAR	szLink[ 128];		// 링크 단어 
	TCHAR	szIconName[ 32 ];	// 아이콘 파일 네임 
	TCHAR	szDesc[ 128 ];		// 설명 
	CTexture*	pTexture;
};

typedef struct _VENDOR_ITEM 
{
	DWORD	m_dwItemId;		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)

	int		m_nItemkind3;
	int		m_nItemJob;
	int		m_nUniqueMin;
	int		m_nUniqueMax;
	int		m_nTotalNum ;
	int		m_nMaterialCount;
} VENDOR_ITEM,* LPVENDOR_ITEM;

typedef struct _NPC_BUFF_SKILL
{
	DWORD	dwSkillID;
	DWORD	dwSkillLV;
	int		nMinPlayerLV;
	int		nMaxPlayerLV;
	DWORD	dwSkillTime;
} NPC_BUFF_SKILL;

//typedef struct tagCHARACTER
//{	
//	tagCHARACTER()
//	{
//		Clear();
//	}
//
//	CHAR			m_szKey[ 64 ];
//	CString			m_strName;
//	CHAR			m_szChar[ 64 ];
//	CHAR			m_szDialog[ 64 ];
//	CHAR			m_szDlgQuest[ 64 ];		// TODO_raiders: 사용하지 않는다. 제거 
//	int				m_nStructure;
//	int				m_nEquipNum;
//	DWORD			m_dwMoverIdx;
//	DWORD			m_dwMusicId;
//	DWORD			m_adwEquip[ MAX_HUMAN_PARTS ];
//	DWORD			m_dwHairMesh;
//	DWORD			m_dwHairColor;
//	DWORD			m_dwHeadMesh;
//	RANDOM_ITEM		m_randomItem;
//	BOOL			m_abMoverMenu[ MAX_MOVER_MENU ];
//	CString			m_venderSlot[ 4 ];
//	CPtrArray		m_venderItemAry[ 4 ];
//
//	CWordArray		m_awSrcQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
//	CWordArray		m_awDstQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
//	CUIntArray		m_anSrcQuestItem; 
//	CUIntArray		m_anDstQuestItem; 
//	void Clear();
//
//	DWORD			m_dwContributionID;				//협동 기부 ID : 
//
////	typedef			std::vector< DWORD >		DWORDArray;
////	DWORDArray		m_anContributionIDList;
//
////sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
//	int				m_nVenderType;
//	CPtrArray		m_venderItemAry2[ 4 ];
//
//	std::vector<NPC_BUFF_SKILL> m_vecNPCBuffSkill;
//
//	std::vector<DWORD>	m_vecdwLanguage;
//	BOOL			bOutput;
////sun: 13, 퀘스트 NPC위치 알림
//	DWORD			m_dwWorldId;
//	D3DXVECTOR3		m_vPos;
//
//	std::vector<D3DXVECTOR3> m_vecTeleportPos;
//
//	//////////////////////////////////////////////////////////////////////////
//	std::map< DWORD /*MMI_ID*/, T_WORLD_POSITION >	m_mapTeleportMenu;
//
//	bool	IsTeleportMenu( const int nMMI_ID ) const;
//	const T_WORLD_POSITION*	GetTeleportPos( const int nMMI_ID ) const;
//	//////////////////////////////////////////////////////////////////////////
//
//	BOOL m_bForcedMenu;
//
//} CHARACTER,* LPCHARACTER;


//---------------------------------------------------------------------------------------------
// NPC  
//---------------------------------------------------------------------------------------------
typedef struct tagCHARACTER
{	
	tagCHARACTER();

	CString												m_strName;

	CHAR												m_szKey[ 64 ];
	CHAR												m_szChar[ 64 ];
	CHAR												m_szDialog[ 64 ];
	CHAR												m_szDlgQuest[ 64 ];		// TODO_raiders: 사용하지 않는다. 제거 
	CString												m_venderSlot[ 4 ];

	D3DXVECTOR3											m_vPos;

	int													m_nEquipNum;
	int													m_nStructure;
	int													m_nVenderType;
	bool												m_bIgnorePurchaseTax;

	BOOL												bOutput;
	BOOL												m_bForcedMenu;

	DWORD												m_dwWorldId;
	DWORD												m_dwMusicId;
	DWORD												m_dwMoverIdx;
	DWORD												m_dwHairMesh;
	DWORD												m_dwHeadMesh;
	DWORD												m_dwHairColor;
	DWORD												m_dwContributionID;				//협동 기부 ID : 

	DWORD												m_adwEquip[ MAX_HUMAN_PARTS ];
	BOOL												m_abMoverMenu[ MAX_MOVER_MENU ];

	RANDOM_ITEM											m_randomItem;

	CWordArray											m_awSrcQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
	CWordArray											m_awDstQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
	CUIntArray											m_anSrcQuestItem; 
	CUIntArray											m_anDstQuestItem; 
	CPtrArray											m_venderItemAry[ 4 ];
	CPtrArray											m_venderItemAry2[ 4 ];

	std::vector<DWORD>										m_vecdwLanguage;
	std::vector<D3DXVECTOR3>									m_vecTeleportPos;
	std::vector<NPC_BUFF_SKILL>								m_vecNPCBuffSkill;

	std::map< DWORD /*MMI_ID*/, T_WORLD_POSITION >		m_mapTeleportMenu;

	bool												IsTeleportMenu( const int nMMI_ID ) const;
	const T_WORLD_POSITION*								GetTeleportPos( const int nMMI_ID ) const;

	void												Clear( void );

} CHARACTER,* LPCHARACTER;


#ifdef __S1108_BACK_END_SYSTEM

typedef struct	_MONSTER_RESPAWN
{
	char		szMonsterName[32];
	int			nRespawnIndex;
	D3DXVECTOR3	vPos;
	int			nQuantity;
	int			nAggressive;
	int			nRange;
	int			nTime;

	_MONSTER_RESPAWN()
	{	
		szMonsterName[0] = '\0';
		vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		nQuantity = 0;
		nAggressive = 0;
		nRange = 0;
		nTime = 0;
		nRespawnIndex = 0;
	}
}
MONSTER_RESPAWN, *PMONSTER_RESPAWN;

typedef struct _MONSTER_PROP
{
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;
	_MONSTER_PROP()
	{
		szMonsterName[0] = '\0';
		nHitPoint = 0;
		nAttackPower = 0;
		nDefence = 0;
		nExp = 0;
		nItemDrop = 0;
		nPenya = 0;
	}
}
MONSTER_PROP, *PMONSTER_PROP;
#endif // __S1108_BACK_END_SYSTEM

//sun: 8,
typedef struct _DIE_PENALTY
{
	int		nLevel;
	int		nValue;
	_DIE_PENALTY()
	{
		nLevel = 0;
		nValue = 0;
	}
} DIE_PENALTY, *PDIE_PENALTY;


//sun: 8, // __S8_PK
typedef struct _CHAO_PROPENSITY
{
	DWORD	dwPropensityMin;		/// 성향 수치 Min
	DWORD	dwPropensityMax;		/// 성향 수치 Max
	int		nEquipDorpMin;			/// 착용아이템 드롭 개수 Min
	int		nEquipDorpMax;			/// 착용아이템 드롭 개수 Max
	int		nInvenDorpMin;			/// 인벤아이템 드롭 개수 Min
	int		nInvenDorpMax;			/// 인벤아이템 드롭 개수 Max
	int		nShop;					/// 상점 이용 유/무
	int		nBank;					/// 창고 이용 유/무
	int		nFly;					/// 비행 이용 유/무
	int		nGuardAttack;			/// 경비병 공격 유/무
	int		nVendor;				/// 개인상점 이용 및 개설 유/무
	_CHAO_PROPENSITY()
	{
		dwPropensityMin = 0;
		dwPropensityMax = 0;
		nEquipDorpMin = 0;
		nEquipDorpMax = 0;
		nInvenDorpMin = 0;
		nInvenDorpMax = 0;
		nShop = 1;
		nBank = 1;
		nFly = 1;
		nGuardAttack = 1;
		nVendor = 1;
	}
} CHAO_PROPENSITY, *PCHAO_PROPENSITY;
typedef struct _PK_SETTING
{
	DWORD	dwGeneralColor;			/// 색 : 일반(흰색)
	DWORD	dwReadyColor;			/// 색 : 준카오(핑크색)
	DWORD	dwChaoColor;			/// 색 : 카오(레드색)
	int		nGeneralAttack;			/// 일반인을 공격했을시 준카오가 되는 시간
	int		nReadyAttack;			/// 준카오를 공격했을시 준카오가 되는 시간
	int     nLimitLevel;			/// 카오를 할수 있는 레벨
	int		nDiePropensityMin;		/// 죽으면 성향치 깍이는 Min
	int		nDiePropensityMax;		/// 죽으면 성향치 깍이는 Max
	std::map<int, CHAO_PROPENSITY> mapPropensityPenalty;	/// 성향수치 단계별 Penalty
	std::map<int, DWORD> mapLevelExp;					/// 레벨에 따른 경험치
	_PK_SETTING()
	{
		dwGeneralColor = 0;
		dwReadyColor = 0;
		dwChaoColor = 0;
		nGeneralAttack = 0;
		nReadyAttack = 0; 
		nLimitLevel = 0;
		nDiePropensityMin = 0;
		nDiePropensityMax = 0;
		mapPropensityPenalty.clear();
		mapLevelExp.clear();
	}
} PK_SETTING, *PPK_SETTING;

////////////////////////////////////////////////////////////////////////////////////////////////////
// enum 
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// macro and inline 
////////////////////////////////////////////////////////////////////////////////////////////////////

inline void LOG_RANGE( const char* szMsg, int nMin, int nMax, int nIndex )
{
	FLERROR_LOG( PROGRAM_NAME, _T( "%s min:%d, max:%d, index:%d" ), szMsg, nMin, nMax, nIndex ); 
}

inline void LOG_CALLSTACK()
{
#ifdef _DEBUG
	__asm int 3
#endif

#if defined(__INTERNALSERVER)
	__asm int 3
	//int *p = NULL;		// 콜스택을 추적하기 위해 죽인다
	//*p = 1;
#endif
}

#define VERIFY_RANGE( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			return (result); \
		}  \
	} while (0)

#define VERIFY_RANGE_ASSERT( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			LOG_CALLSTACK(); \
			return (result); \
		}  \
	} while (0)


////////////////////////////////////////////////////////////////////////////////////////////////////
// KARMAPROP 
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __WORLDSERVER
#define	MAX_GIFTBOX_ITEM	128
typedef	struct	_GIFTBOX
{
	DWORD	dwGiftbox;
	int		nSum;
	int		nSize;
	DWORD	adwItem[MAX_GIFTBOX_ITEM];
	DWORD	adwProbability[MAX_GIFTBOX_ITEM];
	int		anNum[MAX_GIFTBOX_ITEM];
	BYTE	anFlag[MAX_GIFTBOX_ITEM];
	int		anSpan[MAX_GIFTBOX_ITEM];
	int		anAbilityOption[MAX_GIFTBOX_ITEM];

}	GIFTBOX,	*PGIFTBOX;

typedef struct	_GIFTBOXRESULT
{
	DWORD	dwItem;
	int		nNum;
	BYTE nFlag;
	int nSpan;
	int	nAbilityOption;
	_GIFTBOXRESULT()
		{
			dwItem	= 0;
			nNum	= 0;
			nFlag	= 0;
			nSpan	= 0;
			nAbilityOption	= 0;
		}
}	GIFTBOXRESULT,	*PGIFTBOXRESULT;

class	CGiftboxMan
{
private:
	std::vector<GIFTBOX> m_vGiftBox;
	std::map<DWORD, int>	m_mapIdx;
	int	m_nQuery;

public:
	CGiftboxMan();
	virtual	~CGiftboxMan()	{}
	
	static	CGiftboxMan* GetInstance( void );

	BOOL	AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag = 0, int nSpan	= 0, int nAbilityOption = 0 );
	BOOL	IsEnoughEmptySpace( FLWSUser* pUser, DWORD dwGiftBoxId );
	BOOL	Open( DWORD dwGiftBox, PGIFTBOXRESULT pGiftboxResult );

	void	Verify( void );
};
#endif	// __WORLDSERVER

//#define	MAX_ITEM_PER_PACK	16
//#define	MAX_PACKITEM		1024

//typedef	struct	_PACKITEMELEM
//{
//	DWORD dwPackItem;
//	int		nSize;
//	int		nSpan;
//	DWORD	adwItem[MAX_ITEM_PER_PACK];
//	int		anAbilityOption[MAX_ITEM_PER_PACK];
//	int		anNum[MAX_ITEM_PER_PACK];
//}	PACKITEMELEM,	*PPACKITEMELEM;
//
//class CPackItem
//{
//private:
//	int		m_nSize;
//	PACKITEMELEM	m_packitem[MAX_PACKITEM];
//	std::map<DWORD, int>	m_mapIdx;
//
//public:
//	CPackItem();
//	virtual	~CPackItem()	{}
//
//	static	CPackItem*	GetInstance( void );
//
//	BOOL	AddItem( DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum );
//	PPACKITEMELEM	Open( DWORD dwPackItem );
//};

//"PPACKITEMELEM" "CPackItem" 테스트 끝난후 두개의 키워드 지우기
class FLPackItemProp
{
public:
	typedef struct _T_ITEM_INFO
	{
		_T_ITEM_INFO( const DWORD itemID, const int abilityOption, const int num ) : dwItemID( itemID ), nAbilityOption( abilityOption ), nNum( num )	{}

		DWORD					dwItemID;
		int						nAbilityOption;
		int						nNum;
	} T_ITEM_INFO, *PT_ITEM_INFO;

	typedef	struct _T_ELEM
	{
		//int						nSize;
		DWORD					dwPackItemID;
		int						nSpan;
		std::vector< _T_ITEM_INFO >		vecItemInfo;	
	} T_ELEM, *PT_ELEM;

public:
	FLPackItemProp();
	virtual ~FLPackItemProp();

	static FLPackItemProp &	GetInstance();

	BOOL					AddItem( const DWORD dwPackItemID, const DWORD dwItemID, const int nAbilityOption, const int nNum );
	_T_ELEM *				Find( const DWORD dwPackItemID );

private: 
	typedef					std::map< DWORD/*PackItemID*/, _T_ELEM * > 
							PackItemMap;

	PackItemMap				m_mapPackItem;
};

inline static FLPackItemProp & PACK_ITEM_PROP()		{	return FLPackItemProp::GetInstance();	}



#define	MAX_ITEMAVAIL	32
typedef struct	__ITEMAVAIL
{
	int		nSize;
	int		anDstParam[MAX_ITEMAVAIL];
	int		anAdjParam[MAX_ITEMAVAIL];
	__ITEMAVAIL()
	{
		nSize	= 0;
		memset( anDstParam, 0, sizeof( anDstParam ) );
		memset( anAdjParam, 0, sizeof( anAdjParam ) );
	}
}	ITEMAVAIL, *PITEMAVAIL;

typedef	struct	__NEWSETITEMAVAIL : public ITEMAVAIL
{
	int		anEquiped[MAX_ITEMAVAIL];
}	NEWSETITEMAVAIL, *PNEWSETITEMAVAIL;

#define	MAX_SETITEM_STRING		64
#define	MAX_SETITEM_ELEM		8
class CSetItem
{
public:
	int		m_nId;
	char	m_pszString[MAX_SETITEM_STRING];
	int		m_nElemSize;
	int		m_anEquipCount[MAX_SETITEM_ELEM];
	DWORD	m_adwItemId[MAX_SETITEM_ELEM];
	NEWSETITEMAVAIL		m_avail;

public:
//	Constructions
	CSetItem( int nId, const char* pszString );
	virtual	~CSetItem()		{}

	BOOL	AddSetItemElem( DWORD dwItemId );
	BOOL	AddItemAvail( int nDstParam, int nAdjParam, int nEquiped );
	void	SortItemAvail( void );

	void	GetItemAvail( PITEMAVAIL pItemAvail, int nEquiped, BOOL bAll );
	char* GetString( void )		{	return m_pszString;		}
};

class CSetItemFinder
{
private:
	std::map<DWORD, CSetItem*>		m_mapItemId;
	std::map<int, CSetItem*>		m_mapSetId;
public:
	CSetItemFinder()	{}
	virtual	~CSetItemFinder()	{	Free();	}

	void	AddSetItem( CSetItem* pSetItem );
	CSetItem*	GetSetItem( int nSetItemId );
	CSetItem*	GetSetItemByItemId( DWORD dwItemId );
	void	Free();
	static	CSetItemFinder*	GetInstance( void );
};

typedef struct	_SETITEMAVAIL
{
	int		nHitRate;
	int		nBlock;
	int		nMaxHitPointRate;
	int		nAddMagic;
	int		nAdded;
}	SETITEMAVAIL, *PSETITEMAVAIL;

#define	MAX_PIERCING_DSTPARAM	32
typedef struct _PIERCINGAVAIL
{
	DWORD	dwItemId;
	int		nSize;
	int		anDstParam[MAX_PIERCING_DSTPARAM];
	int		anAdjParam[MAX_PIERCING_DSTPARAM];
} PIERCINGAVAIL, *PPIERCINGAVAIL;

#define	MAX_PIERCING_MATERIAL	128
class CPiercingAvail
{
private:
	int		m_nSize;
	std::map<DWORD, int>		m_mapIdx;
	PIERCINGAVAIL	m_pPiercingAvail[MAX_PIERCING_MATERIAL];

public:
	CPiercingAvail();
	~CPiercingAvail();
	BOOL	AddPiercingAvail( DWORD dwItemId, int nDstParam, int nAdjParam );
	PPIERCINGAVAIL	GetPiercingAvail( DWORD dwItemId );

	static	CPiercingAvail*	GetInstance( void );
};



#define	MAX_RANDOMOPTITEMSTRING		32
typedef	struct	_RANDOMOPTITEM
{
	int	nId;
	int nLevel;
	char pszString[MAX_RANDOMOPTITEMSTRING];
	DWORD	dwProbability;
	ITEMAVAIL	ia;
	_RANDOMOPTITEM()
		{	nId	= 0;	nLevel	= 0;	*pszString	= '\0';	dwProbability	= 0;	}
}	RANDOMOPTITEM, *PRANDOMOPTITEM;

//sun: 11, 각성, 축복
#define	MAX_RANDOMOPTITEM		256

class CRandomOptItemGen
{
	int		m_nSize;
	RANDOMOPTITEM	m_aRandomOptItem[MAX_RANDOMOPTITEM];
	std::map<int, int>	m_mapid;

//sun: 10차 전승시스템	Neuz, World, Trans
	int		m_anIndex[MAX_MONSTER_LEVEL];	// 15차 히어로 레벨확장

public:
//	Constructions
	CRandomOptItemGen();
	virtual	~CRandomOptItemGen()	{	Free();	}

	BOOL	AddRandomOption( PRANDOMOPTITEM pRandomOptItem );
	PRANDOMOPTITEM	GetRandomOptItem( int nId );
	const char*	GetRandomOptItemString( int nId );
	int		GenRandomOptItem( int nLevel, FLOAT fPenalty, PT_ITEM_SPEC pItemProp, DWORD dwClass );
	void	Arrange( void );
	void	Free( void );

	static	CRandomOptItemGen*	GetInstance();
};

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM

struct __GEMABILITYKIND
{
	int						nAbility;
	std::vector<int>	vecAbility;
};

class CSetGem
{
private:
	std::vector<__GEMABILITYKIND>	m_vecGemAbilityKind;

public:
	CSetGem();
	~CSetGem();

	static	CSetGem*	GetInstance();

	void	AddGemAvail( __GEMABILITYKIND gembilitykind );
	int		GetGemAvail( int nAblity, int num );
};

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef 
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<u_long, std::string>	ULONG2STRING;
typedef std::map<std::string, u_long>	STRING2ULONG;

////////////////////////////////////////////////////////////////////////////////////////////////////
// CProject
////////////////////////////////////////////////////////////////////////////////////////////////////

class CObj;
class CCtrl;
class CSfx;
class CItem;
class CMover;
class CShip;
#ifdef __WORLDSERVER
class FLWSUser;
#endif


class CProject  
{ 
public:
	CProject();
	virtual ~CProject();

private:
	std::map<std::string, DWORD>			m_mapII;
	std::map<std::string, DWORD>			m_mapMVI;
	std::map<std::string, DWORD>			m_mapCtrl;
#ifdef __CLIENT
	CDWordArray					m_aStateQuest; // 스테이트 사용 여부를 체크할 배열 
#endif

	std::map< int, CString >			m_mapQuestDestination;		// 퀘스트 목적지 설명
	std::map< int, CString >			m_mapPatrolDestination;		// 정찰 목적지 이름

public:
	static FLOAT				m_fItemDropRate;			// 몬스터 아이템 드롭률
	static FLOAT				m_fGoldDropRate;			// 몬스터 페냐 드롭률
	static FLOAT				m_fMonsterExpRate;			// 몬스터 경험치룰
	static FLOAT				m_fMonsterHitRate;			// 몬스터 공격률
	static FLOAT				m_fShopCost;				// 상점가격
	static FLOAT				m_fSkillExpRate;

	static DWORD				m_dwVagSP;					// 방랑자 스킬의 레벨업때 필요한 SP포인트
	static DWORD				m_dwExpertSP;				// 1차직업 스킬의 레벨업때 필요한 SP포인트
	static DWORD				m_dwProSP;					// 2차직업 스킬의 레벨업때 필요한 SP포인트
	static DWORD				m_dwLegendSP;				// 3차직업 스킬의 레벨업때 필요한 SP포인트

	std::map<u_long, CMover*>		m_idPlayerToUserPtr;
	CObjMap						m_objmap;
	CModelMng					m_modelMng;
	int							m_nMoverPropSize;
	MoverProp*					m_pPropMover;				// m_aPropMover배열에 메모리 침범이 있어서 수정함.04.10.14
	CIndexArray< SkillProp >	m_aPartySkill;
	CIndexArray< CtrlProp >		m_aPropCtrl;
	CIndexArray< MotionProp >	m_aPropMotion;
	CIndexArray< SkillProp >	m_aPropSkill;
	CIndexArray< AddSkillProp > m_aPropAddSkill;
	CIndexArray< tagColorText >	m_colorText;
//sun: 10, __FILTER_0705
	std::map<std::string, std::string>	m_mapAlphaFilter;
	std::map<std::string, std::string>	m_mapNonalphaFilter;

	CIndexArray< QuestProp >	m_aPropQuest ;
	CIndexArray<GUILDQUESTPROP>	m_aPropGuildQuest;
	CMapStringToPtr				m_mapCharacter;
	JobProp						m_aPropJob[MAX_JOB];
	SkillProp*					m_aJobSkill[ MAX_JOB ][ 40 ];
	DWORD						m_aJobSkillNum[ MAX_JOB ];
	JOB							m_aJob[ MAX_JOB ];
	JOBITEM						m_jobItem[ MAX_JOBITEM ];
	STRUCTURE					m_aStructure[ MAX_STRUCTURE ];
	GUILD_APPELL				m_aGuildAppell[ MAX_GUILDAPPELL ];
	EXPCHARACTER				m_aExpCharacter[ MAX_EXPCHARACTER ];
	FXPCHARACTER				m_aFxpCharacter[ MAX_FXPCHARACTER ];

	DWORD						m_aExpLPPoint[ MAX_EXPLPPOINT ];
	DWORD						m_aExpSkill[ MAX_EXPSKILL ];
	EXPPARTY					m_aExpParty[MAX_PARTYLEVEL];
	ADDEXPPARTY					m_aAddExpParty[ MAX_ADDEXPPARTY ];
	CPtrArray					m_itemKindAry[ MAX_ITEM_KIND3 ];
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	int							m_aExpUpItem[7][_MAX_ENCHANT_LEVEL+1];	// +0 ~ +10까지의 추가능력치.
#else
	int							m_aExpUpItem[6][_MAX_ENCHANT_LEVEL+1];	// +0 ~ +10까지의 추가능력치.
#endif
	DWORD						m_adwExpDropLuck[122][11];
	SETITEMAVAIL				m_aSetItemAvail[11];
	SIZE						m_minMaxIdxAry[MAX_ITEM_KIND3][MAX_UNIQUE_SIZE];
	std::map<int, PARTYQUESTPROP>	m_propPartyQuest;
//sun: 8,
	std::vector< DIE_PENALTY >		m_vecRevivalPenalty;
	std::vector< DIE_PENALTY >		m_vecDecExpPenalty;
	std::vector< DIE_PENALTY >		m_vecLevelDownPenalty;

	PK_SETTING					m_PKSetting;		//sun: 8, // __S8_PK
//!	KarmaProp					m_aKarmaProp[ MAX_KARMAPROP ];

	std::set<DWORD>					m_setExcept;

#ifdef __CLIENT
	CTerrainMng					m_terrainMng;
	CMapStringToString			m_mapHelp;
	CMapStringToString			m_mapWordToolTip;	
#endif
	
#ifdef __S1108_BACK_END_SYSTEM
	FLOAT						m_fMonsterRebirthRate;	// 몬스터 리스폰률(시간)
	FLOAT						m_fMonsterHitpointRate;	// 몬스터 생명력률
	FLOAT						m_fMonsterAggressiveRate;	// 선공몬스터률
	FLOAT						m_fMonsterRespawnRate;	// 몬스터 리스폰률(몬스터숫자)
	MONSTER_PROP				m_aMonsterProp[MAX_MONSTER_PROP];	// Monster Prop
	MONSTER_PROP				m_aAddProp[MAX_MONSTER_PROP];	// Monster Prop
	char						m_aRemoveProp[MAX_MONSTER_PROP][32];	// Monster Prop
	int							m_nAddMonsterPropSize;
	int							m_nRemoveMonsterPropSize;
	int							m_nMonsterPropSize;
	char						m_chGMChat[10][256];
#endif // __S1108_BACK_END_SYSTEM
	
//sun: 제련 이팩트 향상 버전
	int				m_nEnchantLimitLevel[3];
	float			m_fEnchantLevelScal[2][10];

	CMiniGame		m_MiniGame;			//sun: 10, __EVE_MINIGAME 미니게임

	CUltimateWeapon	m_UltimateWeapon;	//sun: 9,10차 제련 __ULTIMATE

#if defined(__WORLDSERVER) // __WORLDSERVER
	CEventLua m_EventLua;
#endif // __WORLDSERVER
	
#ifdef __CLIENT
	enum { BLOCKING_NUMBER_MAX = 1000 };
	std::set< CString > m_setBlockedUserID;
#endif // __CLIENT

#ifdef __CLIENT
public:
	float	m_fShopBuyRate;
	float	m_fShopSellRate;
#endif // __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	CMapInformationManager m_MapInformationManager;
	BOOL m_bMapTransparent;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

public:
	BOOL			LoadPropEnchant( LPCTSTR lpszFileName );		//sun: 제련 이팩트 향상 버전
	static void		ReadConstant( CScript& script );
	static BOOL		LoadConstant( LPCTSTR lpszFileName );
	static void		SetGlobal( UINT type, float fValue );

	PSETITEMAVAIL	GetSetItemAvail( int nAbilityOption );
	int				GetMinIdx( int nItemKind3, DWORD dwItemRare );
	int				GetMaxIdx( int nItemKind3, DWORD dwItemRare );
	ObjProp*		GetProp( int nType, int nIndex );
	JobProp*		GetJobProp( int nIndex );
	GUILDQUESTPROP*	GetGuildQuestProp( int nQuestId );
	PARTYQUESTPROP*	GetPartyQuestProp( int nQuestId );
	BOOL			IsGuildQuestRegion( DWORD dwWorldID, const D3DXVECTOR3 & vPos );
	CtrlProp*		GetCtrlProp( int nIndex );
	SkillProp*		GetSkillProp( int nIndex ); 
	CPtrArray*		GetItemKindAry( int nKind );
	MoverProp*		GetMoverProp( int nIndex ) ;
	MoverProp*		GetMoverPropEx( int nIndex );
	DWORD			GetSkillPoint( const SkillProp* pSkillProp ) const;
	MoverProp*		GetMoverProp( LPCTSTR lpszMover ); 
	CtrlProp*		GetCtrlProp( LPCTSTR lpszMover );
	MotionProp*		GetMotionProp ( int nIndex ) { return m_aPropMotion.GetAt( nIndex ); }
//	LPATKSTYLE		GetAttackStyle( int nIndex ) { return &m_aAttackStyle[ nIndex ]; }
	int				GetExpUpItem( FLItemElem* pItemElem, int nOption ); 
	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel );
	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine );
	SkillProp*		GetPartySkill ( int nIndex ) { return m_aPartySkill.GetAt( nIndex ); }
	bool			LoadDefines();
	bool			LoadPreFiles();
	bool			LoadStrings();
	BOOL			OpenProject( LPCTSTR lpszFileName );
	BOOL			LoadPropJob( LPCTSTR lpszFileName );
	BOOL			LoadPropMover( LPCTSTR lpszFileName );
	BOOL			LoadPropSkill( LPCTSTR lpszFileName, CIndexArray< SkillProp >* apObjProp );
	void			OnAfterLoadPropItem();
	BOOL			LoadPropSfx( LPCTSTR lpszFileName );
	BOOL			LoadPropCtrl( LPCTSTR lpszFileName, CIndexArray<CtrlProp>*	apObjProp);
	BOOL			LoadMotionProp( LPCTSTR lpszFileName );
	BOOL			LoadText( LPCTSTR lpszFileName );
	BOOL			LoadExpTable( LPCTSTR lpszFileName );
	BOOL			LoadPropMoverEx_AI_SCAN( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI_BATTLE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI_MOVE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx( LPCTSTR szFileName );
	BOOL			LoadJobItem( LPCTSTR szFileName );
	CString			GetLangScript( CScript& script );
	BOOL			LoadCharacter( LPCTSTR szFileName );
	BOOL			LoadEtc( LPCTSTR szFileName );
	BOOL			LoadPropAddSkill( LPCTSTR lpszFileName );
	void			InterpretRandomItem( LPRANDOM_ITEM pRandomItem, CScript& script );
  	
	BOOL			LoadScriptDiePenalty( LPCTSTR lpszFileName );	//sun: 8,
	BOOL			LoadScriptPK( LPCTSTR lpszFileName );			//sun: 8, // __S8_PK
//!	BOOL			LoadPropKarma( LPCTSTR lpszFileName );

	BOOL			LoadPropQuest( LPCTSTR szFileName, BOOL bOptimize = TRUE );
	BOOL			LoadPropGuildQuest( LPCTSTR szFilename );
	BOOL			LoadPropPartyQuest( LPCTSTR szFilename );
	BOOL			LoadDropEvent( LPCTSTR lpszFileName );
	BOOL			LoadGiftbox( LPCTSTR lpszFileName );
	BOOL			LoadPackItem( LPCTSTR lpszFileName );
	BOOL			LoadPiercingAvail( LPCTSTR lpszFileName );
	DWORD			GetTextColor( DWORD dwIndex ); 
	LPCTSTR			GetText( DWORD dwIndex );
	LPCTSTR			GetGuildAppell( int nAppell );
	CCtrl*			GetCtrl( OBJID objid );
	CItem*			GetItem( OBJID objid );
	CMover*			GetMover( OBJID objid );
	CShip*			GetShip( OBJID objid );
	CMover*			GetUserByID( u_long idPlayer );
	LPCHARACTER		GetCharacter( LPCTSTR lpStrKey );
	void			ProtectPropMover();

//sun: 8, // __S8_PK // __S8_PK
	DWORD			GetLevelExp( int nLevel );
	CHAO_PROPENSITY GetPropensityPenalty( DWORD dwPropensity );

	BOOL			LoadExcept( LPCTSTR lpszFilename );
	void			LoadSkill();

	BOOL			LoadMiniGame();				//sun: 10, __EVE_MINIGAME
	BOOL			LoadUltimateWeapon();		//sun: 9,10차 제련 __ULTIMATE

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	BOOL			Load_GemAbility();
#endif

#ifdef __WORLDSERVER
	FLWSUser*			GetUser( OBJID objid );
	BOOL			SortDropItem( void );
//sun: 12, 튜토리얼 개선
private:
	int			m_nMaxSequence;
public:
	int		GetGuildMaxSeq( void )	{	return m_nMaxSequence;	}
	BOOL		LoadGuideMaxSeq();

	BOOL	LoadServerScript( const char* sFile );
#endif	// __WORLDSERVER

#ifdef __CLIENT
	BOOL			LoadFilter( LPCTSTR lpszFileName );
	BOOL			LoadWordToolTip( LPCTSTR lpszFileName );
	BOOL			LoadHelp( LPCTSTR lpszFileName ); 
	CString			GetHelp( LPCTSTR lpStr );
	CString			GetWordToolTip( LPCTSTR lpStr );
	CSfx*			GetSfx( OBJID objid );
#endif	// __CLIENT

//sun: 10, __RULE_0615
	std::set<std::string>		m_sInvalidNames;
	BOOL	LoadInvalidName( void );
	BOOL	IsInvalidName( LPCSTR szName );

	std::set<char>	m_sAllowedLetter;
#ifdef __VENDOR_1106
	std::set<char>	m_sAllowedLetter2;
	BOOL	LoadAllowedLetter( BOOL bVendor = FALSE );
#else	// __VENDOR_1106
	BOOL	LoadAllowedLetter( void );
#endif	// __VENDOR_1106
	BOOL	IsAllowedLetter( LPCSTR szName, BOOL bVendor = FALSE );
	void	Formalize( LPSTR szName, size_t cchName );

	void			OutputDropItem( void );

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp( MONSTER_PROP MonsterProp );
	void			RemoveMonsterProp( char* lpszMonsterName );
#endif 

#if defined( __CLIENT )
	BOOL LoadQuestDestination( void );
	const CString& GetQuestDestination( DWORD dwKey ) const;
	BOOL LoadPatrolDestination( void );
	const CString& GetPatrolDestination( DWORD dwKey ) const;
#endif // defined( __CLIENT )

#ifdef __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
	BOOL LoadPropMapComboBoxData( const CString& strFileName );
#endif // __IMPROVE_MAP_SYSTEM

	BOOL m_bLoadedMoverProp;		//gmbpigsun( 20120719 ) : 캐릭터 선택창을 위한 필수로딩 요소, for __CLIENT
#endif // __CLIENT

	
};

//sun: 8, // __S8_PK
inline DWORD CProject::GetLevelExp( int nLevel )
{
	std::map<int, DWORD>::iterator it = m_PKSetting.mapLevelExp.find( nLevel );
	if( it != m_PKSetting.mapLevelExp.end() )
		return it->second;
	return 4400000;
}
inline CHAO_PROPENSITY CProject::GetPropensityPenalty( DWORD dwPropensity )
{
	std::map<int, CHAO_PROPENSITY >::iterator it = m_PKSetting.mapPropensityPenalty.begin();
	for( ; it != m_PKSetting.mapPropensityPenalty.end() ; ++it )
	{
		CHAO_PROPENSITY Propensity = it->second;
		if( Propensity.dwPropensityMin <= dwPropensity && dwPropensity <= Propensity.dwPropensityMax )
			return Propensity;
	}
	
	it = m_PKSetting.mapPropensityPenalty.find( 8 );	// 못찾으면 마지막 8단계로 보냄 // 꼭 있어야함
	if( it != m_PKSetting.mapPropensityPenalty.end() )
		return it->second;

	CHAO_PROPENSITY Propensity;
	return Propensity;
}

inline CtrlProp* CProject::GetCtrlProp( LPCTSTR lpszCtrl )
{
	std::map<std::string, DWORD>::iterator i	= m_mapCtrl.find( lpszCtrl );
	if( i != m_mapCtrl.end() )
		return (CtrlProp*)GetCtrlProp( i->second );
	return NULL;
}

inline MoverProp* CProject::GetMoverProp( LPCTSTR lpszMover )
{
	std::map<std::string, DWORD>::iterator i	= m_mapMVI.find( lpszMover );
	if( i != m_mapMVI.end() )
		return GetMoverProp( i->second );
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp( int nIndex ) 
{ 
	VERIFY_RANGE_ASSERT( nIndex, 0, m_aPropCtrl.GetSize(), "GetCtrlProp range_error", NULL );
	return m_aPropCtrl.GetAt( nIndex ); 
}

inline SkillProp* CProject::GetSkillProp( int nIndex ) 
{ 
	VERIFY_RANGE( nIndex, 0, m_aPropSkill.GetSize(), "GetSkillProp range_error", NULL );
	return m_aPropSkill.GetAt( nIndex ); 
}

inline CPtrArray* CProject::GetItemKindAry( int nKind )  
{ 
	return &m_itemKindAry[nKind]; 
}

inline MoverProp* CProject::GetMoverProp( int nIndex ) 
{ 
	VERIFY_RANGE( nIndex, 0, m_nMoverPropSize, "GetMoverProp range_error", NULL );
	return m_pPropMover + nIndex;
}

inline MoverProp* CProject::GetMoverPropEx( int nIndex )
{
	if( nIndex < 0 || nIndex >= m_nMoverPropSize )
		return NULL;
	return GetMoverProp( nIndex );
}

inline DWORD CProject::GetTextColor( DWORD dwIndex ) 
{ 
	VERIFY_RANGE( (int)( dwIndex ), 0, m_colorText.GetSize(), "GetTextColor range_error", 0xFFFFFFFF );
	if( m_colorText.GetAt( dwIndex ) == NULL )
		return 0;
	return m_colorText.GetAt( dwIndex )->dwColor; 
}

inline LPCTSTR CProject::GetText( DWORD dwIndex ) 
{ 
	VERIFY_RANGE( (int)( dwIndex ), 0, m_colorText.GetSize(), "GetText range_error", "error" );
	if( m_colorText.GetAt( dwIndex ) == NULL )
		return "";
	return m_colorText.GetAt( dwIndex )->lpszData; 
}

inline CMover* CProject::GetUserByID( u_long idPlayer )
{
	std::map<u_long, CMover*>::iterator i = m_idPlayerToUserPtr.find( idPlayer );
	if( i != m_idPlayerToUserPtr.end() )
		return i->second;
	return NULL;
}

inline LPCHARACTER CProject::GetCharacter( LPCTSTR lpStrKey )
{ 
	if( lpStrKey[0] == 0 )	return NULL;
	LPCHARACTER lpCharacter = NULL; 
	TCHAR szStrKeyLwr[ 64 ] = { 0, };
	FLStrcpy( szStrKeyLwr, _countof( szStrKeyLwr ), lpStrKey );
	FLStrLowercase( szStrKeyLwr, _countof( szStrKeyLwr ) );
	m_mapCharacter.Lookup( szStrKeyLwr, (void*&)lpCharacter ); 
	return lpCharacter; 
}

extern CProject prj;

#define GETTEXTCOLOR( x )	prj.GetTextColor( x )
#define GETTEXT( x )		prj.GetText( x )
#define GETANIM( x )		prj.GetAnim( x )
#define GETITEM( x )		prj.GetItem( x )
#define GETMOVER( x )		prj.GetMover( x )

#ifdef __PERF_0226
class CPartsItem
{
private:
	std::vector<PT_ITEM_SPEC>		m_items[3][MAX_HUMAN_PARTS];
private:
	CPartsItem();
public:
	~CPartsItem();
	static	CPartsItem*	GetInstance( void );
	void	Init();
	PT_ITEM_SPEC	GetItemProp( int nSex, int nParts );
};
#endif	// __PERF_0226

#endif // !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
