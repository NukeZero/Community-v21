#ifndef __OBJ_H__
#define __OBJ_H__

#pragma once

#include <d3dx9math.h>
#include <Define.h>
#include <Data.h>
#include <network/FLAr.h>
#include <string>
#include <map>

#include "../../_CommonDefine/FLTypes.h"
#include <SkillInfluence.h>
#include <UserTaskBar.h>
#include <rtmessenger.h>
#include <buff.h>

#ifdef __DBSERVER
#include <dbmanager.h>
#include <FLItemElem.h>
#include "..\DatabaseServer\Project.h"
extern	CProject	prj;
#include <pocket.h>
#endif // __DBSERVER

#include "Stat.h"


#define ITYPE_ITEM      0
#define ITYPE_CARD      1
#define ITYPE_CUBE      2 
#define ITYPE_PET		3

enum 
{
	METHOD_NONE, METHOD_EXCLUDE_ITEM
};


#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL )


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CObj
{
public:
	DWORD	m_dwType;
	DWORD	m_dwIndex;
	D3DXVECTOR3		m_vScale;
	DWORD	m_dwMotion;
	D3DXVECTOR3		m_vPos;
	FLOAT	m_fAngle;
public:
//	Constructions
	CObj();
	virtual	~CObj()		{}
//	Operations
	virtual	void	Serialize( CAr & ar, int nMethod );

#ifdef __LAYER_1015
protected:
	int		m_nLayer;
public:
	void	SetLayer( int nLayer )	{	m_nLayer	= nLayer;	}
	int		GetLayer()	{	return m_nLayer;	}
#endif	// __LAYER_1015
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CCtrl : public CObj
{
public:
	OBJID	m_objid;

public:
//	Constructions
	CCtrl();
	virtual	~CCtrl()	{}
//	Operations
	void	SetId( OBJID objid )	{	m_objid		= objid;	}
	OBJID	GetId( void )	{	return m_objid;	}
	virtual	void	Serialize( CAr & ar, int nMethod );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

typedef	struct	tagSKILL
{
	DWORD	dwSkill;
	DWORD	dwLevel;
	void Init()	{};
}
SKILL, *LPSKILL;


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef __CORESERVER

class CItem : public CCtrl
{
public:
	FLItemBase* m_pItemBase;

//	Constructions
	CItem();
	virtual ~CItem();
//	Operations
	virtual void	Serialize( CAr & ar, int nMethod );
};

#endif // __CORESERVER


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CAction
{
public:
	DWORD	m_dwState;
	DWORD	m_dwStateFlag;

public:
	CAction()
		{
			m_dwState	= 0;
			m_dwStateFlag		= 0;
		}
	void	ClearStateFlag() { m_dwStateFlag = 0; }
	void	AddStateFlag( DWORD dwStateFlag ) { m_dwStateFlag |= dwStateFlag; }
	DWORD	GetStateFlag() { return m_dwStateFlag; }
	virtual	~CAction()	{}
};


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CMover;
class CActionMover : public CAction
{
public:
	CActionMover()	{}
	CActionMover( CMover* /*pMover*/ )	{}
	virtual	~CActionMover()	{}
	DWORD	GetState( void )	{	return m_dwState;	}
	void	__ForceSetState( DWORD dwState );
};


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
	// 추가 
	//BYTE		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	WORD		m_nKillNPCNum[ MAX_QUEST_KILL_NPC_COUNT ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	BYTE		m_bPatrol   : 1;
	BYTE		m_bReserve2 : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;
} QUEST,* LPQUEST;

#ifdef __DBSERVER
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
#endif // __DBSERVER

class CMover: public CCtrl
{
private:
	DWORD			m_dwGold;

public:

#ifdef __DBSERVER
	LONG			m_adjParamAry[ MAX_ADJPARAMARY ];		/// 수정 파라미터( 예: hp + 수정hp )
	LONG			m_chgParamAry[ MAX_ADJPARAMARY ];		/// 교체 파라미터( 예: 교체 hp )
#endif // __DBSERVER

	T_CONNECTION_POINT	m_kMadrigalGiftPoint;

/// 몬스터에 해당되는 것 
	TCHAR			m_szCharacterKey[ 32 ];
	BOOL			m_bActiveAttack;
	DWORD			m_dwBelligerence;

	int				m_nMovePattern;
	int				m_nMoveEvent;
	int				m_nMoveEventCnt;
/// 몬스터에 해당되는 것 

	TCHAR			m_szName[32];
	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint;
	DWORD			m_dwSkinSet;
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;

private:
	int		m_nTutorialState;

public:
	LONG	GetFlightLv( void )	{	return ( m_nLevel >= 20? 1: 0 );		}
	void	SetFlightLv( LONG /*nFlightLv*/ )	{}
	int		GetTutorialState( void )	{	return m_nTutorialState;	}
	void	SetTutorialState( int nTutorialState )	{	m_nTutorialState	= nTutorialState;	}

	BOOL	IsEquipableNPC()	{	return FALSE;	}

	LONG			m_nFxp;
	BYTE			m_bySex;
	DWORD			m_dwRideItemIdx;
	int				m_nFuel;
	LONG			m_nJob;
	BYTE			m_nDBDataSlot;
	char			m_szBankPass[5];
	int				m_idGuildCloak;

	Stat			m_Stat;
	LONG			m_nLevel, m_nDeathLevel;
	EXPINTEGER		m_nExp1, m_nDeathExp;
	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];

#ifdef __SKILL_0205
	BYTE	m_abUpdateSkill[MAX_SKILL_JOB];
#endif	// __SKILL_0205

	OBJID			m_idMarkingWorld;
	D3DXVECTOR3		m_vMarkingPos;
	DWORD			m_dwAuthorization;
	DWORD			m_dwMode;

	CUserTaskBar	m_UserTaskBar;
	CRTMessenger	m_RTMessenger;

	u_long			m_idparty;
	u_long			m_idGuild;
	u_long			m_idWar;

	DWORD			m_dwPKTime;					/// 핑크 상태 시간
	int				m_nPKValue;					/// PK 수치
	DWORD			m_dwPKPropensity;			/// PK 성향
	DWORD			m_dwPKExp;					/// PK 성향 습득 경험치

	EXPINTEGER		m_nAngelExp;				/// 엔젤 경험치
	LONG			m_nAngelLevel;				/// 엔젤 Level

	int				m_nFame;
	u_long			m_idMurderer;
	int				m_nDuel;			// 듀얼중인가.( 0:없음 1:개인듀얼중 2:파티듀얼중.
	u_long			m_idDuelParty;		// 파티듀얼중이면 상대방 파티의 id
	int				m_nGuildCombatState;
	int				m_nSkillLevel;						// 지금까지 올린 스킬레벨
	int				m_nSkillPoint;						// SP

	CActionMover*					m_pActMover; 

	u_long							m_idPlayerBank[ MAX_CHARACTER_SLOT ];
	
	DWORD							m_dwGoldBank[ MAX_CHARACTER_SLOT ];

	D3DXVECTOR3		m_vReturnPos;						//'귀환 두루마리'관련 
	DWORD			m_dwReturnWorldID;
	DWORD			m_tmAccFuel;
#ifdef __DBSERVER
	CItemContainer< FLItemElem  >	m_Inventory ;

	CItemContainer< FLItemElem  >	m_Bank[ MAX_CHARACTER_SLOT ] ;

	CPocketController	m_Pocket;

	DWORD	m_dwMute;
	DWORD			m_dwWorldID;
	CMclCritSec		m_AccessLock;
	__int64 m_nEventFlag; // 64비트 경과 일 번째 비트
	DWORD	m_dwEventTime;	// 분 단위 계수
	DWORD	m_dwEventElapsed;	// 경과 일
	FLItemElem*	GetPetItem( void );
#endif // __DBSERVER


private:
	OBJID	m_objIdVisPet;	// 소환중인 비스펫의 인벤토리 위치
	DWORD	m_dwMoverSfxId; // 무버에 붙는 이펙트

private:
	DWORD	m_dwPetId;	// 소환중이 펫의 인벤토리 위치(自), 소환중인 펫 인덱스(他)
public:
	DWORD	GetPetId( void ) const	{	return m_dwPetId;	}
	void	SetPetId( DWORD dwPetId )		{	m_dwPetId	= dwPetId;	}
	

	FLOAT	m_fSpeedFactor;

	int		m_nExpLog;
	int		m_nAngelExpLog;

	int		m_nCoupon;

	int				m_nHonor;					// 달인선택 
	int				m_aHonorTitle[MAX_HONOR_TITLE];			// 달인수치
	void			SetHonorCount(int nIdx , int nCount )	{	m_aHonorTitle[nIdx] = nCount;	}

	LONG			m_nPlusMaxHitPoint;
	DWORD			m_dwSMTime[SM_MAX];
	BYTE			m_nAttackResistLeft;
	BYTE			m_nAttackResistRight;
	BYTE			m_nDefenseResist;

	CBuffMgr	m_buffs;

#ifdef __DBSERVER
	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
 	void	ClearEquipInfo( void )
 	{
 		memset( m_aEquipInfo, 0, sizeof(m_aEquipInfo) );
// 		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 			m_aEquipInfo[i].dwId	= NULL_ID;
 	}
#endif // __DBSERVER
	
	DWORD			m_dwStateMode;
	OBJID			m_dwUseItemId;					// 비행 시전 시간
	LPQUEST			m_aQuest;
	LPWORD			m_aCompleteQuest; 
	size_t			m_nQuestSize;
	size_t			m_nCompleteQuestSize;
	DWORD			m_dwPeriodicTick;
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;

	DWORD			m_dwPCBangClass;

	LPWORD			m_aCheckedQuest;
	size_t			m_nCheckedQuestSize;

private:
	u_long	m_idCampus;
	int		m_nCampusPoint;
public:
	u_long	GetCampusId()	{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )	{	m_idCampus = idCampus;	}
	int		GetCampusPoint()	{	return m_nCampusPoint;	}
	void	SetCampusPoint( int nMPPoint )		{	m_nCampusPoint = nMPPoint;	}

public:
	int		m_nRestPoint;
	time_t	m_tLogOut;

public:
//	Constructions
	CMover();
	virtual	~CMover();

private:
	void	Init();

public:
//	Operations
	void	InitProp( void );
	int		InitSkillExp( void );
#if defined(__DBSERVER)
	MoverProp*	GetProp( void )		{	return( prj.GetMoverProp( m_dwIndex ) );	}
#endif	// __DBSERVER

	BOOL	RemoveQuest( int nQuestId );

	virtual void	Serialize( CAr & ar, int nMethod );

//	BOOL	IsAbnormalPlayerData();
//	BOOL	NormalizePlayerData();
	BOOL	RemoveItemIK3();
	BYTE	GetSex() { return m_bySex; }
	void	SetSex( BYTE bySex ) { m_bySex = bySex; }
	int		GetGold();

	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100705
	bool	CheckUserGold( int nGold, bool bAdd );
	//////////////////////////////////////////////////////////////////////////

	void	SetGold( int nGold );
	BOOL	AddGold( int nGold, BOOL bSend = TRUE );

	void	Copy( CMover* pMover, BOOL bCopyAll );
	void	PeriodTick();

#if defined(__DBSERVER)
	void	Lock( void )	{	m_AccessLock.Enter();	}
	void	Unlock( void )	{	m_AccessLock.Leave();	}
	BOOL	m_bLogout;
	char	m_szAccount[MAX_ACCOUNT];

public:
	MEMPOOLER_DECLARE( CMover );
#endif	// __DBSERVER


	int		GetUsableItemQuantityByItemID( const DWORD dwItemID ) const;

	//////////////////////////////////////////////////////////////////////////
#ifdef __DBSERVER

	private:
		bool			InitializeJobSkill( const DWORD dwJob, const DWORD dwLevel );

	public:
		bool			InitializePlayerSkill();

		bool			InitializeSkillLevel();

		bool			CheckPlayerSkill() const;

		void			CorrectErrorsSkillPoint();
		int				GetUseSkillPoint() const;

		bool			CheckPlayerOriginStat();

#endif // __DBSERVER
	//////////////////////////////////////////////////////////////////////////
};


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

inline int CMover::GetGold()
{
	if( m_dwGold > INT_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid. UserId:[%07d], Gold:[%d]" ), m_idPlayer, m_dwGold );
	}
	return m_dwGold;
}

inline void CMover::SetGold( int nGold )
{
	if( nGold < 0 || nGold > INT_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid. UserId:[%07d], Gold:[%d]" ), m_idPlayer, nGold );
		return;
	}
	m_dwGold = (DWORD)nGold;
}

inline BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	__int64 n64Gold = (__int64)nGold;

	if( n64Gold == 0 )
	{
		return TRUE;
	}

	__int64 n64Total = static_cast<__int64>( GetGold() ) + n64Gold;
	if( n64Total > static_cast<__int64>( INT_MAX ) )
	{
		n64Total = static_cast<__int64>( INT_MAX );
	}
	else if( n64Total < 0 )
	{
		n64Total = 0;
	}

	SetGold( static_cast<int>( n64Total ) );

	if( bSend == TRUE )
	{
#ifdef __WORLDSERVER
		g_UserMng.AddSetPointParam( this, DST_GOLD, (int)n64Total );
#endif	// __WORLDSERVER
	}

	return TRUE;
}

#endif	// __OBJ_H__