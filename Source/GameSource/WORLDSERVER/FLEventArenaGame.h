
#ifndef	__FLEVENTARENAGAME_H__
#define	__FLEVENTARENAGAME_H__


#include "FLEventArenaSpec.h"
#include "FLEventArenaTeamManager.h"
#include "FLEventArenaStartPos.h"
#include "FLEventArenaAlarm.h"


class	FLEventArenaGame
{
private:

	enum	Time
	{
		TEAM_MONITER		= 3*1000,	// 팀원 정보 갱신 주기
		NO_ENEMY_TEAM_CHECK	= 5 * 1000,	// 5초
	};

public:

	explicit FLEventArenaGame( const FLEventArenaSpec* pSpec );
	~FLEventArenaGame();

	void	OnLogin( FLWSUser* pUser );
	void	OnLogout( FLWSUser* pUser, bool bJoined );

	void	OnPlayerDied( CMover* pMover );
	void	OnPlayerResurrection( CMover* pMover );

	void	Run();

	bool	IsEnemy( CMover* pSrc, CMover* pTarget )	const;
	bool	IsJoined( FLWSUser* pPlayer )	const;
	bool	IsPlaying()	const;

	void	StartManage( FLWSUser* pManager, const FLEventArenaTeamManager* pTeamManager );
	void	StopManage( FLWSUser* pManager, const TCHAR* szWhere );

	void	SetTonement( FLWSUser* pManager, const T_EVENT_ARENA_TONEMENT* pTonement );
	void	SetRedTeam( FLWSUser* pManager, FLEventArenaTeam* pRedTeam );
	void	SetBlueTeam( FLWSUser* pManager, FLEventArenaTeam* pBlueTeam );
 
	void	CallPlayers( FLWSUser* pManager );
	void	AcceptCallPlayer( FLWSUser* pPlayer );

	void	JoinPlayers( FLWSUser* pManager );

	void	StartBattle( FLWSUser* pManager );
	void	EndGame( FLWSUser* pManager );
	void	StopGame( FLWSUser* pManager, const TCHAR* szReason );

private:

	bool	IsManager( FLWSUser* pManager )	const;
	void	SendToManager( const FLPacket* pPacket );

	void	TerminateGame();

	T_EVENT_ARENA_GAME_INFO	GetInfo()	const;
	int		GetRedWinCount()	const;
	int		GetBlueWinCount()	const;

	void	SyncGameInfo();
	void	GetGameScoreboard( FLSnapshotEventArenaGameScoreboardNoti& kScoreboardPacket );

	void	GetGameJoinTeamMembers( std::vector< tstring >& vecRedTeamMembers, std::vector< tstring >& vecBlueTeamMembers )	const;

	void	AllStandy();
	bool	TryStandby( FLWSUser* pPlayer );
	bool	CanMoveCalledState();

	void	CreateWall();
	void	DestroyWall();

	void	CallDeathNPC();
	void	DestroyDeathNPC();

	void	TryBattleOver();

	int		TryNoEnemyTeamWin()	const;
	int		DecideBattleOver()	const;
	void	BattleEnd( int nWinTeam );
	bool	IncWinCount( int nWinTeam );
	bool	IsGameOver()	const;

	void	BattleResultLog( int nWinTeam );
	bool	GetBattleResultInfo( int nWinTeam, FLEventArenaBattleResultInfo* pBattleResultInfo );
	void	SendBattleResultLog( FLEventArenaBattleResultInfo* pBattleResultInfo );
	void	PrintBattleResultLog( FLEventArenaBattleResultInfo* pBattleResultInfo );

	void	GameResultLog( const TCHAR* szCode, const TCHAR* szReason, int nWinTeam );
	bool	GetGameResultInfo( const TCHAR* szCode, const TCHAR* szReason, int nWinTeam, T_GAME_RESULT_INFO* pGameResultInfo );
	void	PrintGameResultLog( T_GAME_RESULT_INFO* pGameResultInfo );

	void	Clear();

private:

	typedef	std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >	GIVE_ITEM_LIST;
	typedef	GIVE_ITEM_LIST::iterator					GIVE_ITEM_ITER;

	typedef	std::vector< T_BATTLE_RESULT_LOG >			BATTLE_LOG_LIST;
	typedef	BATTLE_LOG_LIST::iterator					BATTLE_LOG_ITER;

	const FLEventArenaSpec*			m_pSpec;

	FLWSUser*						m_pManager;
	CTimer							m_kTeamMoniter;		// 팀원 정보 갱신 타이머

	int								m_nState;			// 상태
	FLEventArenaAlarm				m_kAlarm;
	FLEventArenaAlarm				m_kNoEnemyCheckAlarm;
	FLEventArenaTeam*				m_pRedTeam;
	FLEventArenaTeam*				m_pBlueTeam;

	const T_EVENT_ARENA_TONEMENT*	m_pTonement;

	SYSTEMTIME						m_tGameStart;		// 현재 진행 중인 게임 시작 시간

	int								m_nBattleNumber;	// 현재 진행 중인 전투 넘버(1부터 시작)
	SYSTEMTIME						m_tBattleStart;		// 현재 진행 중인 전투 시작 시간

	BYTE							m_byRedWinFlag[MAX_EVENT_ARENA_WIN_COUNT];	// 레드팀 승리 플래그
	BYTE							m_byBlueWinFlag[MAX_EVENT_ARENA_WIN_COUNT];	// 블루팀 승리 플래그

	BATTLE_LOG_LIST					m_vecBattleLog;

	std::vector< CObj* >			m_vecWalls;
	std::vector< CObj* >			m_vecDeathNPCs;

};






#endif