// InstanceDungeonBase.h: interface for the CInstanceDungeonBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_)
#define AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <network/FLAr.h>
#ifdef __WORLDSERVER
#include "../worldserver/FLQuestInterface.h"
#endif // __WORLDSERVER


enum { IDTYPE_SOLO=0, IDTYPE_PARTY, IDTYPE_GUILD, IDTYPE_MAX };
enum { ID_NORMAL=0, ID_MIDBOSS, ID_BOSS };



const WORD CLASS_NORMAL=0x0001, CLASS_MASTER=0x0002, CLASS_HERO=0x0004;
const WORD CLASS_LEGEND_HERO=0x0008;


enum E_DUNGEON_LEVEL {	DUNGEON_LEVEL_NORMAL, DUNGEON_LEVEL_HARD, DUNGEON_LEVEL_MAX		};

enum {	IDKIND_GENERAL = 0, IDKIND_COLOSSEUM	};

//enum {
//	NORMAL_LEVEL				= 0,
//	HIGH_LEVEL, 
//	__MAX_LEVEL__,
//};//						};





const int COOLTIMECOUNT = 10;


//-----------------------------------------------------상태 ----------------------------------------------------------------------//

struct INDUN_INFO;



//--------------------------------------------------//
struct ITimeAlarmState
{
	ITimeAlarmState() : m_dwAlarmTick( 0 ), m_bEnable( FALSE )	{}

	virtual void		OnEnter( INDUN_INFO & info ) = 0;
	virtual void		OnLeave( INDUN_INFO & info ) = 0;
	virtual void		OnAlarm( INDUN_INFO & info )	= 0;

	virtual void 		OnUpdate( INDUN_INFO & info );

	void				SetAlarmInterval( const DWORD dwAlarmInterval );

protected:
	DWORD				m_dwAlarmTick;
	BOOL				m_bEnable;
};





struct FLAutoStartState : public ITimeAlarmState						//처음 Teleport되면. 
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};

struct FLStageWaitingState : public ITimeAlarmState		//정상으로 깼을때. NextStage하고 
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};

struct FLStageLimitState : public ITimeAlarmState
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};

struct FLRetriedStageWaitingState : public ITimeAlarmState
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};

struct FLAllDieWaitingState : public ITimeAlarmState
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};

struct FLExpiredTimeWaitingState : public ITimeAlarmState
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};


struct FLCompleteAllWaitingState : public ITimeAlarmState
{
	virtual void		OnEnter( INDUN_INFO & info );
	virtual void		OnLeave( INDUN_INFO & info );
	virtual	void		OnAlarm( INDUN_INFO & info );
};
//-------------------------------------------------------------------------------------------------------------------------------//








//---------------------------------------------------------------------------------------------------------------------------------------------//

#ifdef __WORLDSERVER
//enum {		UNLIMIT_TIME = -1,	};
const DWORD		UNLIMIT_TIME = static_cast< DWORD >( -1 );

//struct MONSTER
//{
//	int				nStage;
//	DWORD			dwMonsterId;
//	BOOL			bRed;
//	D3DXVECTOR3		vPos;
//	DWORD			dwLimitTime;
//};

struct PAIR_MONSTER
{
	//int				nStage;
	DWORD			dwFakeMonsterID;
	DWORD			dwRealMonsterID;
	DWORD			dw10000RealMonsterRate;
	BOOL			bRed;
	D3DXVECTOR3		vPos;

	DWORD			dwLimitTime;

	BOOL			bRandomMonster;
};
typedef					std::vector< PAIR_MONSTER >		PairMonsterVec;
typedef					std::vector< PairMonsterVec >	PairMonsterVec2D;


#endif // __WORLDSERVER


//#include <boost/noncopyable.hpp>

//진행 정보
struct INDUN_INFO //: boost::noncopyable
{
	INDUN_INFO( const DWORD dwWI, const ULONG uMK, const DWORD dungeonID, const DWORD guildID = 0, const DWORD dwDungeonLevel = DUNGEON_LEVEL_NORMAL );
	void	Serialize( CAr & ar );
	

#ifdef __WORLDSERVER		//---- 콜로세움
	BOOL					IsValidState_( ITimeAlarmState * pState );

	void					SetState( ITimeAlarmState * pState, const DWORD dwAlarmIntervalTick );
	BOOL					IsRandomMonsterStage( const int nStage );

public:
	typedef	std::vector< OBJID >	CalledMonsterVec;

	ITimeAlarmState *		m_pState;
	//BOOL					bNotEnter;		
	BOOL					bCreateAndEntered;				//비동기 텔레포트로 인해 입던한 사람이 있나 감지
	BOOL					bCallRealMonster;
	DWORD					dw10000AdditionalRealMonsterRate;

	PairMonsterVec2D		vec2DPairMonsterList;			//Stage별 몬스터 리스트
	CalledMonsterVec		vecCalledMonster;

	FLAutoStartState			AutoStartState;	
	FLStageWaitingState			StageWaitingState;
	FLStageLimitState			StageLimitState;
	FLRetriedStageWaitingState	RetriedStageWaitingState;
	FLCompleteAllWaitingState	CompleteAllWaitingState;

	FLAllDieWaitingState		AllDieWaitingState;
	FLExpiredTimeWaitingState	ExpiredTimeWaitingState;
#endif // __WORLDSERVER		//---- 콜로세움



	DWORD			dwWorldId;
	ULONG			uMultiKey;
	DWORD			dwGuildID;
	DWORD			dwDungeonID;
	int				eDungeonLevel;

#ifdef __WORLDSERVER
	DWORD			dwCreatedTick;

	int				nCurrStage;
	DWORD			dwStartStageTick;
	DWORD			dwTimeLimitTick;
	DWORD			dwTurnaroundTime;
	FLTm			tmStartedDate;

	int				nPlayerCount;
	int				nKillCount;

	u_long			idLeaderPlayer;

	DWORD			dwAddRealBossRate_ItemID;
	DWORD			dwAddRealBossRate_ItemObjID;
	u_long			idPlayer_UseAddRealBossRateItem;

	BOOL			bSuccess;
#endif // __WORLDSERVER


}; //INDUN_INFO;	


class FLWSUser;
class IDungeonReward
{
public:
	virtual BOOL			Reward( INDUN_INFO & Info, FLWSUser & User ) = 0;
	virtual void			PushSnapshotTail( CAr & ar ) = 0;
};


#include <map>
#include <vector>
typedef std::vector<INDUN_INFO*> VEC_IDINFO;
typedef std::map<DWORD/*DungeonID*/, VEC_IDINFO> MAP_IDBASE;			

struct COOLTIME_INFO
{
	DWORD dwWorldId;
	DWORD dwDungeonId;
	DWORD dwCoolTime;
	COOLTIME_INFO() : dwWorldId( NULL_ID), dwDungeonId( NULL_ID ), dwCoolTime( 0 ) {}
	COOLTIME_INFO( DWORD dwWI, DWORD dwDI, DWORD dwCT ) : dwWorldId( dwWI ), dwDungeonId( dwDI ), dwCoolTime( dwCT ) {}
	void	Serialize( CAr & ar );
};
typedef std::vector<COOLTIME_INFO> VEC_CTINFO;

//struct FLDungeonKey
//{
//	DWORD				dwDungeonID;
//	E_DUNGUEN_LEVEL::E	eLevel;
//};

//
//struct FLWorldID
//{
//	DWORD				dwWorldID;
//	int					eDungeonLevel;
//};
//
//
//struct FLDungeonID
//{
//	DWORD				dwDungeonID;
//	int					eDungeonLevel;
//};
typedef std::map<DWORD/*PlyaerID*/, VEC_CTINFO> MAP_CTINFO;					//DWORD -> FLDungeonID

//////////////////////////////////////////////////////////////////////
// CInstanceDungeonBase
//////////////////////////////////////////////////////////////////////
class CInstanceDungeonBase
{
public:
	CInstanceDungeonBase( int nIDType );
	virtual ~CInstanceDungeonBase();
	
	int		GetType()	{ return m_nIDType; }
	void	SerializeAllInfo( CAr & ar );

	void	UpdateState_OnlyColosseum();
	BOOL	HasLeader( INDUN_INFO & io_info );


	BOOL	GetDungeonLevel( const DWORD dwWorldID, const DWORD dwDungeonID, DWORD & o_dwDungoenLevel );
	
	INDUN_INFO *	CreateDungeon( const DWORD dwDungeonId, const DWORD dwWorldId, const ULONG uMultiKey, const DWORD dwGuildID, const int eDungeonLevel );
	BOOL	DestroyDungeon( INDUN_INFO & ID_Info, DWORD dwDungeonId );
	void	DestroyAllDungeonByDungeonID( DWORD dwDungeonId );
	void	DestroyAllDungeonByMultiKey( ULONG uMultiKey );		//현재까지는 CoreServer만 사용.

	void	SetDungeonCoolTimeInfo( COOLTIME_INFO CT_Info, DWORD dwPlayerId );
	void	ResetDungeonCoolTimeInfo( DWORD dwWorldId, DWORD dwDungeonId );


	void		Teleport( INDUN_INFO & o_Info );

	void		RespawnMonster_AndTrySwitchToStageLimit( DWORD dwDungeonId, DWORD dwWorldId );

	void		ClearCalledMonster( DWORD dwDungeonId, DWORD dwWorldId );

	INDUN_INFO*	GetDungeonInfo( DWORD dwDungeonId, DWORD dwWorldId );

private:
	BOOL		CheckAllPlayerDie( const DWORD dwWorldID, const DWORD dwDungeonID );
	BOOL		UpdateWhenChangedPartyLeader( INDUN_INFO & info );

	BOOL	CreateDungeonLayer( /*OUT*/ INDUN_INFO & ID_Info, DWORD dwLayer );
	BOOL	DestroyDungeonLayer( const INDUN_INFO & ID_Info, DWORD dwLayer );

	VEC_IDINFO* GetDungeonVector( DWORD dwDungeonId );
	

	VEC_CTINFO*		GetCoolTimeVector( DWORD dwPlayerId );
	COOLTIME_INFO*	GetCoolTimeInfo( DWORD dwPlayerId, DWORD dwWorldId );
	
	
	void			TeleportToNextStage( INDUN_INFO & o_Info, const DWORD dwDungeonID );


	int				m_nIDType;
	MAP_IDBASE		m_mapID;						//파티 던전 정보 리스트
	MAP_CTINFO		m_mapCTInfo;

#ifdef __CORESERVER
public:
	void	DungeonCoolTimeCountProcess();
	void	UpdateDungeonCoolTimeInfo();

private:
	int		m_nCoolTimeCount;
#endif // __CORESERVER

#ifdef __WORLDSERVER

public:
	struct DUNGEON_DATA
	{
		DUNGEON_DATA() : bColosseumStyle( FALSE ), dwClass( 65535 ), dwCoolTime( 0 )
#ifdef PARTY_WARP
		, bPartyWarp ( FALSE ) 
		, nTicketIndex ( 0 )
#endif // PARTY_WARP
		{}

		struct FLLevelProp
		{
			FLLevelProp() : 
				nMinLevel( 0 ), nMaxLevel( 0 ), nMaxStage( 0 ),
				dwAutoStart_WaitingTick( 0 ), 
				dwStartStage_WaitingTick( 0 ), 
				dwComplete_WaitingTick( 0 ),
				dwRetry_WaitingTick( 0 ), 
				dwRetryStartStage_WaitingTick( 0 ),	
				dwMailTitleTID( 0 ), dwMailTextTID( 0 )
				{}

			//---- 콜로세움
			//typedef					std::vector< DWORD /*TimeLimitTick */>	TimeLimitVec;
			typedef					FLVector_autoDelete< IDungeonReward * >	RewardVec;



			int						nMinLevel;
			int						nMaxLevel;
			
			//정보
			int						nMaxStage;

			//시간 간격
			DWORD					dwAutoStart_WaitingTick;
			DWORD					dwStartStage_WaitingTick;
			DWORD					dwComplete_WaitingTick;
			DWORD					dwRetry_WaitingTick;
			DWORD					dwRetryStartStage_WaitingTick;

			PairMonsterVec			vecRandomMonster;
			PairMonsterVec2D		vec2DPairMonster;

			DWORD					dwMailTitleTID;
			DWORD					dwMailTextTID;

			RewardVec				vecReward;
			//TimeLimitVec			vecTimeLimit;
		};

		D3DXVECTOR3				vStartPos;

		DWORD					dwClass;
		DWORD					dwCoolTime;
		BOOL					bColosseumStyle;			//콜로세움 스타일
		
		FLLevelProp				arrLevelProp[ DUNGEON_LEVEL_MAX ];

		std::map<int/*Stage*/, D3DXVECTOR3>	mapTeleportPos;
#ifdef PARTY_WARP
		BOOL					bPartyWarp;					// 파티워프 여부
		int						nTicketIndex;
#endif // PARTY_WARP
	};


private:
	typedef std::map<DWORD/*WorldID*/, DUNGEON_DATA> MAP_IDDATA;			//WorldID -> FLWorldID
	MAP_IDDATA m_mapDungeonData;

	// 난이도 들어가야함.
	BOOL		CheckClassLevel( FLWSUser* pUser, DWORD dwWorldId, DWORD eDungeonLevel );

	int			GetCurrStage( DWORD dwWorldId, DWORD dwDungeonId );
	void		SetNextStage( INDUN_INFO* pInfo, DWORD dwDungeonId );
	D3DXVECTOR3 GetTeleportPosByStage( DWORD dwWorldId, DWORD dwDungeonId, const DWORD dwStage );

	BOOL		HasNowStateMonsterInfo( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	int			GetObjCount_CurrStage( const DWORD dwWorldId, const DWORD dwDungeonId );
	DWORD		GetCoolTime( DWORD dwWorldId );

	BYTE		m_nMaxInstanceDungeon;

protected:
	BOOL		respawnOneMonster( CWorld & World, const PAIR_MONSTER & Monster, const int nStage, INDUN_INFO & io_info );

	BOOL	TeleportToDungeon( FLWSUser* pUser, DWORD dwWorldId, DWORD dwDungeonId, const DWORD dwGuildID, DWORD eDungeonLevel );	//없으면 던전 생성
	void	SetLeaveMarkingPos( FLWSUser* pUser, DWORD dwWorldId, D3DXVECTOR3 vPos );
public:
	BOOL	LeaveToOutside( FLWSUser* pUser, DWORD dwWorldId, DWORD dwDungeonId );
	

public:
	DUNGEON_DATA *		GetDuneonData( const INDUN_INFO & info );		
	DUNGEON_DATA *		GetDuneonData( const DWORD dwWorldID, const DWORD dwDungeonLevel );

	BOOL		IncreasePlayerCount( DWORD dwDungeonId, DWORD dwWorldId );
	BOOL		DecreasePlayerCount( FLWSUser* pUser, DWORD dwDungeonId, DWORD dwWorldId );

	void	LoadScript( const char* szFileName );
	virtual BOOL EnteranceDungeon( FLWSUser* pUser, DWORD dwWorldId, const DWORD dwGuildID, DWORD eDungeonLevel ) = FALSE;
	virtual BOOL LeaveDungeon( FLWSUser* pUser, DWORD dwWorldId ) = FALSE;
	virtual	int	GetDungeonKind( FLWSUser* pUser ) = 0;

	void	SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	void	DeleteDungeonCoolTimeInfo( DWORD dwPlayerId );
#endif // __WORLDSERVER
};


//////////////////////////////////////////////////////////////////////
// CInstanceDungeonHelper
//////////////////////////////////////////////////////////////////////
class CInstanceDungeonHelper
{
public:
	CInstanceDungeonHelper();
	~CInstanceDungeonHelper();
	static CInstanceDungeonHelper* GetInstance();

	BOOL	GetDungeonLevel( const DWORD dwWorldID, const DWORD dwDungeonID, DWORD & o_dwDungoenLevel );
	BOOL	SetAdditinalRealMonsterRate( FLWSUser* pUser, const DWORD n10000AdditionalRate );
	BOOL	RetryCurrStage( FLWSUser & User );
	BOOL	RegistAdditionalRealMonsterRateItem( FLWSUser & User, const DWORD dwAddRealBossRate_ItemObjID );
	BOOL	UnRegistAdditionalRealMonsterRateItem( FLWSUser & User );

	void OnCreateDungeon( int nType, DWORD dwDungeonId, const DWORD dwPlayerID, const DWORD dwWorld, const ULONG uMultiKey, const DWORD dwGuildID, const int eDungeonLevel );
	void OnDestroyDungeon( int nType, INDUN_INFO & ID_Info, DWORD dwDungeonId );
	void OnSetDungeonCoolTimeInfo( ULONG uKey, int nType, COOLTIME_INFO CT_Info, DWORD dwPlayerId );
	void OnResetDungeonCoolTimeInfo( int nType, DWORD dwWorldId, DWORD dwDungeonId );

	void OnColosseumFail( INDUN_INFO & info );

	void SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info, const DWORD dwPlayerID );
	void SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info );
	void SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info );
	void SendInstanceDungeonResetCoolTimeInfo( const int nType, const DWORD dwWorldId, const DWORD dwDungeonId );

	void DestroyAllDungeonByMultiKey( ULONG uMultiKey );


	

private:
	CInstanceDungeonBase* GetDungeonPtr( int nType );

	typedef std::map<DWORD/*WorldID*/, int/*IDTYPE_*/> MAP_WORLD2TYPE;
	MAP_WORLD2TYPE	m_mapDungeonType;

#ifdef __CORESERVER
public:
	void SendInstanceDungeonAllInfo( DWORD dpId );
	void SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId );
#endif // __CORESERVER
#ifdef __WORLDSERVER
public:
	void	IncreasePlayerCount( const DWORD dwWorldID, const DWORD dwDungeonId );

	void	OnInstanceDungeonAllInfo( CAr & ar );
	BOOL	IsDungeonType( DWORD dwWorldId, int nType )	{ return GetType( dwWorldId ) == nType; }
	BOOL	IsInstanceDungeon( DWORD dwWorldId )	{ return GetType( dwWorldId ) != NULL_ID; }
	void	SetDungeonType( DWORD dwWorldId, int nType );
	BOOL	EnteranceDungeon( FLWSUser* pUser, DWORD dwWorldId, const DWORD dwGuildID, DWORD eDungeonLevel );
	BOOL	LeaveDungeon( FLWSUser* pUser, DWORD dwWorldId );
	void	GoOut( FLWSUser* pUser );
	void	GoOutAll( const DWORD dwWorldID, const DWORD dwDungeonID );

	void	SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	void	OnDeleteDungeonCoolTimeInfo( int nType, DWORD dwPlayerId );

	int		GetDungeonKind( FLWSUser* pUser );
	void	DungeonFailLogQuery( const INDUN_INFO & info );

	void	OnDungeonBestCompleteTimeAck( const T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO & fromTrans );

	BYTE	m_nIDLayerNum;

private:
	

	//void	SetCertifyKey( ULONG uMultiKey = NULL_ID );
	//BOOL	IsCertifying()	{ return m_uCertifyKey != NULL_ID; } 
	int		GetType( DWORD dwWorldId );

	ULONG	m_uCertifyKey;
#endif // __WORLDSERVER
};

//
//
//
//class FLReward_Item : public IDungeonReward
//{
//public:
//	FLReward_Item( const DWORD dwItemID, const short nItemCount );
//	virtual ~FLReward_Item();
//
//	virtual BOOL			Reward( INDUN_INFO & Info, FLWSUser & User );
//	virtual void			PushSnapshotTail( CAr & ar );
//private:
//	const DWORD				m_dwItemID;
//	const short				m_nItemCount;
//};
//
//class FLReward_Gold : public IDungeonReward
//{
//public:
//	FLReward_Gold( const int nGold );
//	virtual ~FLReward_Gold();
//
//	virtual BOOL			Reward( INDUN_INFO & Info, FLWSUser & User );
//	virtual void			PushSnapshotTail( CAr & ar );
//private:
//	const int				m_nGold;
//};


#endif // !defined(AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_)
