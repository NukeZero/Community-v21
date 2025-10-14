#if !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
#define AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef	__XTOOL		// xuzhu 관련 툴


#include "defineNeuz.h"
#include "ModelMng.h"
#include "TerrainMng.h"
#include "ObjMap.h" 
#include "ProjectCmn.h"
#include "Script.h" 

#include "guildquest.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// define  
////////////////////////////////////////////////////////////////////////////////////////////////////

const int	MAX_PROPMOVER =		2000;		// MoverProp배열의 최대갯수 
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

#define MAX_QUESTPROPITEM  4
#define MAX_QUESTCONDITEM  64
#define MAX_QUESTREMOVE    12

struct QuestState
{
	TCHAR	m_szDesc[ 512 ];
#ifndef __V050322_CACHEQUEST
	//TCHAR	m_szCond[ 256 ];
	//TCHAR	m_szStatus[ 256 ];
#endif //  __V050322_CACHEQUEST
};
#ifdef __V050322_CACHEQUEST
struct QuestPropItem 
{
#ifdef __V050524_QUEST
	char    m_nSex;
	char	m_nType; 
	int     m_nJobOrItem;
	int		m_nItemIdx;
	int		m_nItemNum;
#else
//{{AFX
	char	m_nType; 
	int     m_nJobOrItem;

	int		m_anItemIdx[ 4 ];
	int		m_anItemNum[ 4 ];
//}}AFX
#endif
};
#endif // __V050322_CACHEQUEST
struct QuestProp
{
	WORD	m_wId;
	TCHAR	m_szTitle[ 64 ];
	TCHAR	m_szNpcName[ 32 ];
#ifndef __V050322_CACHEQUEST
//{{AFX
	TCHAR	m_szLinkChar[ 64 ];
//}}AFX
#endif
	WORD	m_nHeadQuest;
	CHAR	m_nQuestType;
	bool	m_bNoRemove; // TRUE일 경우 삭제 불가 

#ifdef __V050322_CACHEQUEST
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
	char	m_nBeginCondKarmaComp;
	int		m_nBeginCondKarmaPoint;
	char	m_nBeginCondChaotic; 
	int		m_nBeginCondDisguiseMoverIndex;
	// 시작 - 추가 
	int  	m_nBeginSetAddItemIdx[ 4 ];
	char	m_nBeginSetAddItemNum[ 4 ];
	int		m_nBeginSetAddGold;
	int     m_nBeginSetDisguiseMoverIndex;
	// 종료 - 조건  
	int     m_nEndCondLimitTime; //  퀘스트 수행 제한 시간 
	QuestPropItem* m_paEndCondItem; 
	char	m_nEndCondItemNum;
	int     m_nEndCondKillNPCIdx[ 2 ]; // 죽여야할 NPC 인덱스 - 총 2개 
	int     m_nEndCondKillNPCNum[ 2 ]; // 죽여야할 NPC 갯수 - 총 2개    
	DWORD   m_dwEndCondPatrolWorld; // 정찰해야될 맵 
	CRect   m_rectEndCondPatrol; // 정찰해야될 장소 영역 
	CHAR    m_szEndCondCharacter[64]; // 퀘스트를 완수를 판단할 캐릭터 키(NULL이면 자신)
	CHAR*   m_lpszEndCondMultiCharacter; 
	int		m_nEndCondSkillIdx;
	char	m_nEndCondSkillLvl;
	char	m_nEndCondKarmaComp;
	int		m_nEndCondKarmaPoint;
	char	m_nEndCondChaotic; 
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
	CHAR    m_szEndCondDlgAddKey[ 64 ];
	CHAR    m_szPatrolZoneName[ 64 ];
	// EndDialog 대화시 지급할 아이템(일종의 보상)
	int		m_nDlgRewardItemIdx[ 4 ];
	int		m_nDlgRewardItemNum[ 4 ];
	// 종료 - 제거 
	int		m_nEndRemoveItemIdx[ 4 ];
	int		m_nEndRemoveItemNum[ 4 ];
	int		m_nEndRemoveGold;
	int		m_anEndRemoveQuest[ MAX_QUESTREMOVE ];
	// 보상 
	QuestPropItem* m_paEndRewardItem; 
	int		m_nEndRewardItemNum;
	int		m_nEndRewardGoldMin;
	int		m_nEndRewardGoldMax;
	int		m_nEndRewardExpMin;
	int		m_nEndRewardExpMax;
	int     m_nEndRewardKarmaPoint;
	char	m_nEndRewardKarmaStyle; // 0은 적용 안함, 1은 세팅, 2는 추가 
	bool    m_bEndRewardHide; // 보상 목록을 보일지 여부
	bool    m_bRepeat;
#else // __V050322_CACHEQUEST
//{{AFX
	int		m_nTime;

	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	int		m_nParam4;

	int		m_nBeginCondPreviousQuest;
	int		m_nBeginCondJob[ MAX_JOB ];
	int		m_nBeginCondJobNum;
	int		m_nBeginCondLevelMax;
	int		m_nBeginCondLevelMin;
	int		m_nBeginCondGroup;
	int		m_nBeginCondGroupNum;
	int		m_nBeginCondItem1;
	int		m_nBeginCondItem2;
	int		m_nBeginCondItem3;
	int		m_nBeginCondItemNum1;
	int		m_nBeginCondItemNum2;
	int		m_nBeginCondItemNum3;
	// 시작 - 추가 
	int		m_nBeginAddItem1;
	int		m_nBeginAddItem2;
	int		m_nBeginAddItem3;
	int		m_nBeginAddItemNum1;
	int		m_nBeginAddItemNum2;
	int		m_nBeginAddItemNum3;
	int		m_nBeginAddGold;
	// 종료 - 조건  
	int		m_nEndCondItem1;
	int		m_nEndCondItem2;
	int		m_nEndCondItem3;
	int		m_nEndCondItemNum1;
	int		m_nEndCondItemNum2;
	int		m_nEndCondItemNum3;
	// 종료 - 제거 
	int		m_nEndRemoveItem1;
	int		m_nEndRemoveItem2;
	int		m_nEndRemoveItem3;
	int		m_nEndRemoveItemNum1;
	int		m_nEndRemoveItemNum2;
	int		m_nEndRemoveItemNum3;
	int		m_nEndRemoveGold;
	// 보상 
	int		m_nRewardItemIdx;
	int		m_nRewardItemMin;
	int		m_nRewardItemMax;
	int		m_nRewardGoldMin;
	int		m_nRewardGoldMax;
	int		m_nRewardExpMin;
	int		m_nRewardExpMax;
//}}AFX
#endif // __V050322_CACHEQUEST 

#ifdef __V050322_CACHEQUEST 
	QuestState*	m_questState[ 16 ];
#else // __V050322_CACHEQUEST 
//{{AFX
	CHAR*		m_apQuestDialog[ 32 ];
	QuestState	m_questState[ 16 ];
//}}AFX
#endif // __V050322_CACHEQUEST 
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

typedef struct tagJobAbility
{
	DWORD	dwStr, dwStrAdd;
	DWORD	dwSta, dwStaAdd;
	DWORD	dwDex, dwDexAdd;
	DWORD	dwInt, dwIntAdd;

} JOBABILITY,* LPJOBABILITY;

typedef struct tagATKSTYLE
{
	DWORD	dwParam1, dwParam2;

} ATKSTYLE,* LPATKSTYLE;

typedef struct tagADDEXPPARTY
{
	DWORD	Exp, Level;	//  협동 추가 경험치 / 제공레벨
} ADDEXPPARTY,* LPADDEXPPARTY;

typedef struct tagEXPCOMPANY
{
	DWORD	dwExp;		// 경험치
	int		nPenya;		// 페냐
	int		nMaxMember;	// 가입가능한 최대인원.
} EXPCOMPANY,* LPEXPCOMPANY;


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
	int		m_nItemkind3;
	int		m_nItemJob;
	int		m_nUniqueMin;
	int		m_nUniqueMax;
	int		m_nTotalNum ;
	int		m_nMaterialCount;
} VENDOR_ITEM,* LPVENDOR_ITEM;

typedef struct tagCHARACTER
{
	CHAR    m_szKey[ 64 ];
	CString m_strName;
	CHAR    m_szChar[ 64 ];
	CHAR    m_szDialog[ 64 ];
	CHAR    m_szDlgQuest[ 64 ];
	int     m_nStructure;
	int		m_nEquipNum;
#ifdef __V050322_CACHEQUEST
	DWORD   m_dwMoverIdx;
	DWORD   m_dwMusicId;
#endif
	DWORD	m_adwEquip[ MAX_HUMAN_PARTS ];
	DWORD	m_dwHairMesh;
	DWORD	m_dwHairColor;
	DWORD	m_dwHeadMesh;
	RANDOM_ITEM m_randomItem;
	BOOL		m_abMoverMenu[ MAX_MOVER_MENU ];
	CString		m_venderSlot[ 4 ];
	CPtrArray	m_venderItemAry[ 4 ];

#ifdef __V050322_CACHEQUEST
	CWordArray   m_awSrcQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
	CWordArray   m_awDstQuest; // 이 캐릭터가 소유한 퀘스트 아이디 목록 
	CUIntArray   m_anSrcQuestItem; 
	CUIntArray   m_anDstQuestItem; 
#endif
	void Clear();

} CHARACTER,* LPCHARACTER;

struct tagColorText
{
	DWORD dwColor;
	TCHAR * lpszData;
}; 


////////////////////////////////////////////////////////////////////////////////////////////////////
// enum 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum 
{	
	FILE_FILTER	= 0, 
	FILE_INVALID = 1, 
	FILE_NOTICE = 2	
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// macro and inline 
////////////////////////////////////////////////////////////////////////////////////////////////////

inline void LOG_RANGE( const char* szMsg, int nMin, int nMax, int nIndex )
{
	LPCTSTR szErr = Error( "%s min:%d, max:%d, index:%d", szMsg, nMin, nMax, nIndex ); 
	ADDERRORMSG( szErr ); 
}

inline void LOG_CALLSTACK()
{
#ifdef _DEBUG
	__asm int 3
#endif

}

#define VERIFY_RANGE( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( (int)( nIndex ) < (int)( nMin ) || (int)( nIndex ) >= (int)( nMax ) ) \
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

const int MAX_KARMAPROP = 13;

enum SLAUGHTER_GRADE
{
	SLAUGHTER_NORMAL,			// 일반유저 
	SLAUGHTER_SEMI_CHAOTIC,		// 준 카오 
	SLAUGHTER_CHAOTIC,			// 카오 
};

// 카르마별 프로퍼티 
struct KarmaProp
{
	int		nGrade;					// 등급
	TCHAR	szName[64];				// 명칭 
	TCHAR	szNameEn[32];			// 영문명칭	
	DWORD   dwKarmaRecoverPoint;	// 카르마 회복 수치 (초 단위)
	int		nKarmaPoint;			// 포인트
	DWORD	dwKarmaGrade;			// 유저 등급 ( 0 - 일반, 1 - 준카오, 2 - 카오 )
	DWORD   dwColor;				// 명칭 색깔

	FLOAT   fDiscountRate;			// 상점 물품 구입 할인
	FLOAT   fSellPenaltyRate;		// 상점 물품 판매 패널티
	BOOL    bGuardReaction;			// 경비병 반응
	int		nSubtractExpRate;		// 경험치 하락률 
	int		nDropGoldPercent;		// 소지 페냐중 드롭할 비율 
	int		nDropItem;				// 드롭할 아이템 갯수 
	int		nDropPercent;			// 아이템 드롭할 확률 
	int		nKarmaRecoverNum;		// 카르마 회복수치
	DWORD	dwStatLimitTime;		// 스탯 제한 시간
	int		nStatLimitNum;			// 스탯 제한 갯수

	int		nStatLimitRate;			// 스탯 제한 비율
	TCHAR	szCommand [128];		// 설명

};


////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef 
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef map<u_long, string>	ULONG2STRING;
typedef map<string, u_long>	STRING2ULONG;
typedef vector< CObj* >		CObjArray;
typedef CObjArray::iterator ObjItor;

////////////////////////////////////////////////////////////////////////////////////////////////////
// CProject
////////////////////////////////////////////////////////////////////////////////////////////////////

class CObj;
class CCtrl;
class CSfx;
class CItem;
class CMover;
class CShip;

class CProject  
{ 
public:
	CProject();
	virtual ~CProject();

private:
	ULONG2STRING				m_ulong2str;
	STRING2ULONG				m_str2ulong;
	map<string, DWORD>			m_mapII;
	map<string, DWORD>			m_mapMVI;
	map<string, DWORD>			m_mapCtrl;

protected:
	BOOL						m_bOpenProject;
	
public:
	map<u_long, CMover*>		m_idPlayerToUserPtr;
	DWORD						m_nLanguage; 
	TCHAR						m_szPrjFile[32];
	TCHAR						m_szPrjPath[128];
	CModelMng					m_modelMng;
	CTerrainMng					m_terrainMng;

	FLOAT						m_fItemDropRate;			// 몬스터 아이템 드롭률

	FLOAT						m_fGoldDropRate;			// 몬스터 페냐 드롭률

	FLOAT						m_fMonsterExpRate;			// 몬스터 경험치룰

	FLOAT						m_fMonsterHitRate;			// 몬스터 공격률

	FLOAT						m_fShopCost;				// 상점가격
	FLOAT						m_fSkillExpRate;		
	JOBITEM						m_jobItem[ MAX_JOBITEM ];

	int							m_nMoverPropSize;
	MoverProp*					m_pPropMover;				// m_aPropMover배열에 메모리 침범이 있어서 수정함.04.10.14
	CFixedArray< tagColorText >	m_colorText;
	CFixedArray< MotionProp >	m_aPropMotion;
	CFixedArray< ObjProp >		m_aPropObj;
	CFixedArray< ObjProp >		m_aPropSfx; 
	CFixedArray< ItemProp >		m_aPropItem;
	CFixedArray< ItemProp >		m_aPropSkill;
	CFixedArray< ItemProp >		m_aPropCard;
	CFixedArray< FILTER >		m_aWordFilter;
	CFixedArray< QuestProp >	m_aPropQuest ;
	CFixedArray< AddSkillProp > m_aPropAddSkill;
	CFixedArray< ItemProp >		m_aPartySkill;
	JobProp						m_aPropJob[MAX_JOB];
	ItemProp*					m_aJobSkill[ MAX_JOB ][ 40 ];
	DWORD						m_aJobSkillNum[ MAX_JOB ];
	JOB							m_aJob[ MAX_JOB ];
	STRUCTURE					m_aStructure[ MAX_STRUCTURE ];
	GUILD_APPELL				m_aGuildAppell[ MAX_GUILDAPPELL ];
	EXPCHARACTER				m_aExpCharacter[ MAX_EXPCHARACTER ];
	FXPCHARACTER				m_aFxpCharacter[ MAX_FXPCHARACTER ];
	JOBABILITY					m_aJobAbillity [ MAX_JOB + 1      ];
	DWORD						m_aExpLPPoint  [ MAX_EXPLPPOINT   ];
	DWORD						m_aExpSkill    [ MAX_EXPSKILL     ];
	ATKSTYLE					m_aAttackStyle [ MAX_AS           ];
	EXPPARTY					m_aExpParty[MAX_PARTYLEVEL];
	ADDEXPPARTY					m_aAddExpParty[ MAX_ADDEXPPARTY ];
	CMapStringToPtr				m_mapCharacter;
	CPtrArray					m_itemKindAry[ MAX_ITEM_KIND3 ];
	int							m_aExpUpItem[6][11];	// +0 ~ +10까지의 추가능력치.
#ifdef __EXPDROPLUCK0524
	DWORD						m_adwExpDropLuck[122][11];
#else	// __EXPDROPLUCK0524
	DWORD						m_adwExpDropLuck[102][11];
#endif	// __EXPDROPLUCK0524

	CObjMap						m_objmap;
	SIZE						m_minMaxIdxAry[MAX_ITEM_KIND3][MAX_UNIQUE_SIZE];
	BOOL						m_bInvent;
	CFixedArray<CtrlProp>		m_aPropCtrl;

	CFixedArray<GUILDQUESTPROP>	m_aPropGuildQuest;

	RENEWEVENT					m_aReNewEvent[ MAX_RENEWEVENT ];
	CTime						m_tReNewEventStartTime;
	CTime						m_tReNewEventEndTime;
	int 						m_nReNewItemFound[ 256 ];
	DWORD						m_nRealItem[ MAX_REALITEM ];
	RENEWEVENTITEM				m_aRealEventItem[ MAX_EVENTREALITEM ];
	int							m_nMaxReNewEventItem;
	int 						m_nReNewRealItemFound[ 256 ];


	DWORD						m_aExpJobLevel[ MAX_EXPJOBLEVEL+1 ];

	KarmaProp					m_aKarmaProp[ MAX_KARMAPROP ];

public:
	int		GetMinIdx( int nItemKind3, DWORD dwItemRare )
	{
		if( dwItemRare >= MAX_UNIQUE_SIZE )
			return -1;
		return m_minMaxIdxAry[nItemKind3][dwItemRare].cx;
	}

	int		GetMaxIdx( int nItemKind3, DWORD dwItemRare )
	{
		if( dwItemRare >= MAX_UNIQUE_SIZE )
			return -1;
		return m_minMaxIdxAry[nItemKind3][dwItemRare].cy;
	}


	ObjProp*    GetProp( int nType, int nIndex );
	ObjProp*    GetObjProp( int nIndex )	{ return m_aPropObj.GetAt( nIndex ); }
	ObjProp*    GetSfxProp( int nIndex )	{ return m_aPropSfx.GetAt( nIndex ); }
	JobProp*	GetJobProp( int nIndex );

	GUILDQUESTPROP*	GetGuildQuestProp( int nQuestId )
	{
		VERIFY_RANGE( nQuestId, 0, m_aPropGuildQuest.GetSize(), "GetGuildQuestProp range_error", NULL );
		return m_aPropGuildQuest.GetAt( nQuestId );
	}
	BOOL	IsGuildQuestRegion( const D3DXVECTOR3 & vPos );
	CtrlProp*   GetCtrlProp( int nIndex ) 
	{ 
		VERIFY_RANGE_ASSERT( nIndex, 0, m_aPropCtrl.GetSize(), "GetCtrlProp range_error", NULL );
		return m_aPropCtrl.GetAt( nIndex ); 
	}

	ItemProp*   GetItemProp( int nIndex ) 
	{ 
		VERIFY_RANGE_ASSERT( nIndex, 0, m_aPropItem.GetSize(), "GetItemProp range_error", NULL );
		return m_aPropItem.GetAt( nIndex ); 
	}

	ItemProp*   GetSkillProp( int nIndex ) 
	{ 
		VERIFY_RANGE( nIndex, 0, m_aPropSkill.GetSize(), "GetSkillProp range_error", NULL );
		return m_aPropSkill.GetAt( nIndex ); 
	}

	ItemProp*   GetCardProp( int nIndex ) { return m_aPropCard.GetAt( nIndex ); }
	CPtrArray*  GetItemKindAry( int nKind )  { return &m_itemKindAry[nKind]; }

	MoverProp*  GetMoverProp( int nIndex ) 
	{ 
		VERIFY_RANGE( nIndex, 0, m_nMoverPropSize, "GetMoverProp range_error", NULL );
		return m_pPropMover + nIndex;
	}

	MoverProp*  GetMoverPropEx( int nIndex )
	{
		if( nIndex < 0 || nIndex >= m_nMoverPropSize )
			return NULL;
		return GetMoverProp( nIndex );
	}

	ItemProp*	GetItemProp( LPCTSTR lpszItem );
	MoverProp*	GetMoverProp( LPCTSTR lpszMover );
	TCHAR*		GetPropMoverID( DWORD dwIndex );
	CtrlProp*	GetCtrlProp( LPCTSTR lpszMover );
	MotionProp* GetMotionProp ( int nIndex ) { return m_aPropMotion.GetAt( nIndex ); }
	LPATKSTYLE  GetAttackStyle( int nIndex ) { return &m_aAttackStyle[ nIndex ]; }

	int			GetExpUpItem( DWORD dwItemKind3, int nOption ); 

	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel );
	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine );
	ItemProp*		GetPartySkill ( int nIndex ) { return m_aPartySkill.GetAt( nIndex ); }

	BOOL	IsOpenProject() { return m_bOpenProject; }
	BOOL	OpenProject( LPCTSTR lpszFileName );
	void	CloseProject();
 	void    ReadConstant( CScript& script );
	BOOL    LoadConstant( LPCTSTR lpszFileName );
	BOOL	LoadPropJob( LPCTSTR lpszFileName );
	BOOL	LoadPropMover( LPCTSTR lpszFileName );
	BOOL	LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp );
	void	OnAfterLoadPropItem();
	BOOL	LoadPropObj( LPCTSTR lpszFileName );
	BOOL	LoadPropSfx( LPCTSTR lpszFileName );
	BOOL	LoadPropCtrl( LPCTSTR lpszFileName, CFixedArray<CtrlProp>*	apObjProp);
	BOOL	LoadPropObj( LPCTSTR lpszFileName, CFixedArray< ObjProp >* apObjProp );
	BOOL	LoadMotionProp( LPCTSTR lpszFileName );
	BOOL	LoadText( LPCTSTR lpszFileName );
	BOOL	LoadExpTable( LPCTSTR lpszFileName );
	BOOL	LoadPropMoverEx_AI_SCAN( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL	LoadPropMoverEx_AI_BATTLE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL	LoadPropMoverEx_AI_MOVE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL	LoadPropMoverEx_AI( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL	LoadPropMoverEx( LPCTSTR szFileName );
	BOOL    LoadJobItem( LPCTSTR szFileName );
	CString GetLangScript( CScript& script );
	BOOL	LoadCharacter( LPCTSTR szFileName );
	BOOL    LoadEtc( LPCTSTR szFileName );
	BOOL	LoadPropAddSkill( LPCTSTR lpszFileName );
	void	InterpretRandomItem( LPRANDOM_ITEM pRandomItem, CScript& script );


	BOOL	LoadCharacter_All( LPCTSTR szFileName );
	BOOL    LoadPropQuest_All( LPCTSTR szFileName );
	BOOL    LoadPropQuest( LPCTSTR szFileName, BOOL bOptimize = TRUE );
	BOOL	LoadPropGuildQuest( LPCTSTR szFilename );
	BOOL	LoadDropEvent( LPCTSTR lpszFileName );
	BOOL	LoadGiftbox( LPCTSTR lpszFileName );

	
	DWORD GetTextColor( DWORD dwIndex ) 
	{ 
		VERIFY_RANGE( dwIndex, 0, m_colorText.GetSize(), "GetTextColor range_error", 0xFFFFFFFF );
		if( m_colorText.GetAt( dwIndex ) == NULL )
			Error( "GetTextColor : dwIndex=%d not found.", dwIndex );
		return m_colorText.GetAt( dwIndex )->dwColor; 
	}

	LPCTSTR GetText( DWORD dwIndex ) 
	{ 
		VERIFY_RANGE( dwIndex, 0, m_colorText.GetSize(), "GetText range_error", "error" );
		if( m_colorText.GetAt( dwIndex ) == NULL )
			Error( "GetText : dwIndex=%d not found.", dwIndex );
		return m_colorText.GetAt( dwIndex )->lpszData; 
	}

	LPCTSTR GetGuildAppell( int nAppell )
	{
		VERIFY_RANGE( nAppell, 0, MAX_GUILDAPPELL, "GetGuildAppell range_error", "error" );
		return m_aGuildAppell[ nAppell ].szName;
	}

	CCtrl*	GetCtrl ( OBJID objid );
	CSfx*	GetSfx  ( OBJID objid );
	CItem*	GetItem ( OBJID objid );
	CMover*	GetMover( OBJID objid );
	CShip*	GetShip( OBJID objid );
		
	CMover* GetUserByID( u_long idPlayer )
	{
		map<u_long, CMover*>::iterator i = m_idPlayerToUserPtr.find( idPlayer );
		if( i != m_idPlayerToUserPtr.end() )
			return i->second;
		return NULL;
	}

	LPCHARACTER GetCharacter( LPCTSTR lpStrKey )
	{ 
		if( lpStrKey[0] == 0 )	return NULL;
		LPCHARACTER lpCharacter = NULL; 
		TCHAR szStrKeyLwr[ 64 ];
		_tcscpy( szStrKeyLwr, lpStrKey );
		_tcslwr( szStrKeyLwr );
		m_mapCharacter.Lookup( szStrKeyLwr, (void*&)lpCharacter ); 
		return lpCharacter; 
	}

	void	ProtectPropMover();


	u_long	GetPlayerID( const CHAR* lpszPlayer );
	LPCSTR	GetPlayerString( u_long idPlayer );
	BOOL	SetPlayerID( u_long idPlayer, const CHAR* lpszPlayer );
	BOOL	RemovePlayerID( u_long idPlayer );	

	KarmaProp*		GetKarmaProp( int nSlaughter ); 
	KarmaProp*		GetKarmaPropByGrade( int nGrade ); 
	SLAUGHTER_GRADE GetSlaughterGrade( int nSlaughter );
};


inline SLAUGHTER_GRADE CProject::GetSlaughterGrade( int nSlaughter )
{
	KarmaProp* pProp = GetKarmaProp( nSlaughter );

	switch( pProp->dwKarmaGrade )
	{
	case 0: return SLAUGHTER_NORMAL;
	case 1: return SLAUGHTER_SEMI_CHAOTIC;
	case 2: return SLAUGHTER_CHAOTIC;
	default:	ASSERT( FALSE ); break;
	}
	return SLAUGHTER_NORMAL;
}

inline KarmaProp* CProject::GetKarmaProp( int nSlaughter ) 
{ 
	for( int i=0; i<MAX_KARMAPROP-1; i++ )
	{
		if( nSlaughter >= m_aKarmaProp[i].nKarmaPoint )
			return &m_aKarmaProp[i];
	}

	return &m_aKarmaProp[MAX_KARMAPROP-1];
}

inline KarmaProp* CProject::GetKarmaPropByGrade( int nGrade ) 
{ 
	for( int i=0; i<MAX_KARMAPROP; i++ )
	{
		if( nGrade == m_aKarmaProp[i].nGrade )
			return &m_aKarmaProp[i];
	}

	ASSERT( FALSE );
	return NULL;
}

inline ItemProp* CProject::GetItemProp( LPCTSTR lpszItem )
{
	map<string, DWORD>::iterator i	= m_mapII.find( lpszItem );
	if( i != m_mapII.end() )
		return GetItemProp( i->second );
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp( LPCTSTR lpszCtrl )
{
	map<string, DWORD>::iterator i	= m_mapCtrl.find( lpszCtrl );
	if( i != m_mapCtrl.end() )
		return (CtrlProp*)GetCtrlProp( i->second );
	return NULL;
}

inline MoverProp* CProject::GetMoverProp( LPCTSTR lpszMover )
{
	map<string, DWORD>::iterator i	= m_mapMVI.find( lpszMover );
	if( i != m_mapMVI.end() )
		return GetMoverProp( i->second );
	return NULL;
}

inline TCHAR*	CProject::GetPropMoverID( DWORD dwIndex )
{
	MoverProp* pMoverProp	= NULL;
	pMoverProp	= GetMoverProp( dwIndex );

	if( pMoverProp != NULL )
	{
		return	pMoverProp->m_szDefineString;
	}
	else
	{
		return	NULL;
	}
}

inline int CProject::GetExpUpItem( DWORD dwItemKind3, int nOption ) 
{ 
	if( nOption == 0 )	
		return 0;

	if( nOption < 0 || nOption > 11 )	
	{
		Error( "CProject::GetExpUpItem range_error. %d", nOption );
		return 0;
	}

	switch( dwItemKind3 )
	{
	case IK3_SWD:
	case IK3_CHEERSTICK:
	case IK3_WAND:	
	case IK3_AXE:
	case IK3_KNUCKLEHAMMER:
	case IK3_STAFF:
	case IK3_YOYO: 
	case IK3_BOW:
		return m_aExpUpItem[0][ nOption ];
	case IK3_HELMET:
		return m_aExpUpItem[1][ nOption ];
	case IK3_SUIT:
		return m_aExpUpItem[2][ nOption ];
	case IK3_GAUNTLET:
		return m_aExpUpItem[3][ nOption ];
	case IK3_BOOTS:
		return m_aExpUpItem[4][ nOption ];
	case IK3_SHIELD:
		return m_aExpUpItem[5][ nOption ];
	}

	Message( "GetExpUpItem : itemkind오류 %d, %d", dwItemKind3, nOption );
	return 0;
}


extern CProject prj;

#define GETTEXTCOLOR( x )	prj.GetTextColor( x )
#define GETTEXT( x )		prj.GetText( x )
#define GETANIM( x )		prj.GetAnim( x )
#define GETITEM( x )		prj.GetItem( x )
#define GETMOVER( x )		prj.GetMover( x )

#endif	// not define _XTOOL

#endif // !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
