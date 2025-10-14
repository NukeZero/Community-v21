
#include "stdafx.h"
#include "FLEventArenaGame.h"
#include "FLEventArenaHelper.h"

namespace
{
	const TCHAR	EVENT_ARENA_LOG_NAME[128] = _T( "EventArena" );
}

FLEventArenaGame::FLEventArenaGame( const FLEventArenaSpec* pSpec ) :
	m_pSpec( pSpec )
{
	Clear();

	m_pManager = NULL;
}

FLEventArenaGame::~FLEventArenaGame()
{
	Clear();

	m_pManager = NULL;
}

void	FLEventArenaGame::Clear()
{
	DestroyWall();
	DestroyDeathNPC();

	m_nState = EVENT_ARENA_STATE_CONFIG;

	::memset( &m_tGameStart, 0, sizeof( m_tGameStart ) );

	m_nBattleNumber = 0;
	::memset( &m_tBattleStart, 0, sizeof( m_tBattleStart ) );

	::memset( m_byRedWinFlag, 0, sizeof( m_byRedWinFlag ) );
	::memset( m_byBlueWinFlag, 0, sizeof( m_byBlueWinFlag ) );

	m_pRedTeam = NULL;
	m_pBlueTeam = NULL;

	m_pTonement = NULL;

	m_vecBattleLog.clear();
}


bool	FLEventArenaGame::IsManager( FLWSUser* pManager )	const
{
	if( IsValidObj( pManager ) && m_pManager == pManager )
	{
		if( pManager->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			return true;
		}
	}

	return false;
}

void	FLEventArenaGame::SendToManager( const FLPacket* pPacket )
{
	if( IsValidObj( m_pManager ) )
	{
		m_pManager->AddPacket( pPacket );
	}
}

void	FLEventArenaGame::OnLogin( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		if( m_nState != EVENT_ARENA_STATE_CONFIG )
		{
			FLSnapshotEventArenaGameJoinTeamInfoNoti kJoinTeamInfo;
			GetGameJoinTeamMembers( kJoinTeamInfo.vecRedTeamMembers, kJoinTeamInfo.vecBlueTeamMembers );
			pUser->AddPacket( &kJoinTeamInfo );
		}

		FLEventArenaHelper::ClearInven( pUser );
		FLEventArenaHelper::ClearBuff( pUser );
		FLEventArenaHelper::AllRecovery( pUser );
		FLEventArenaHelper::Disguise( pUser, m_pSpec->GetTransformMoverID() );

		FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
		GetGameScoreboard( kSocreboardPacket );
		pUser->AddPacket( &kSocreboardPacket );

		FLSnapshotEventArenaBattleResultLogNoti kBattleResultLog;
		kBattleResultLog.vecBattleResultLog = m_vecBattleLog;
		pUser->AddPacket( &kBattleResultLog );
	}
}

void	FLEventArenaGame::OnLogout( FLWSUser* pUser, bool bJoined )
{
	if( IsValidObj( pUser ) )
	{
		StopManage( pUser, _T( "Logout" ) );

		// 승패 확인
		if( bJoined && IsPlaying() && IsValidObj( pUser ) )
		{
			if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				const TCHAR* szTeamName = NULL;
				const TCHAR* szName = pUser->GetName();
				if( m_pBlueTeam->IsMember( szName ) )
				{
					szTeamName = m_pBlueTeam->GetName();
				}
				else if( m_pRedTeam->IsMember( szName ) )
				{
					szTeamName = m_pRedTeam->GetName();
				}

				if( szTeamName != NULL && szName != NULL )
				{
					FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "Logout Team: %s, Player: %s" ), szTeamName, szName );
#ifdef	__INTERNALSERVER
					FLINFO_LOG( PROGRAM_NAME, _T( "Logout Team: %s, Player: %s" ), szTeamName, szName );
#endif

					TryBattleOver();

					FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
					GetGameScoreboard( kSocreboardPacket );
					FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );
				}
			}
		}
	}
}

void	FLEventArenaGame::Run()
{
	switch( m_nState )
	{
	case EVENT_ARENA_STATE_CALL_WAIT:
		{
			if( m_kAlarm.IsOver() )
			{
				m_kAlarm.Stop();
				AllStandy();

				m_nState = EVENT_ARENA_STATE_CALL;
				SyncGameInfo();
			}
		}
		break;
	case EVENT_ARENA_STATE_CALL:
		{
			if( CanMoveCalledState() )
			{
				m_nState = EVENT_ARENA_STATE_CALLED;
				CreateWall();

				SyncGameInfo();
			}
		}
		break;
	case EVENT_ARENA_STATE_JOIN:
		{
			if( m_kAlarm.IsOver() )
			{
				m_nState = EVENT_ARENA_STATE_JOINED;
				SyncGameInfo();
			}
		}
		break;
	case EVENT_ARENA_STATE_COUNTDOWN:
		{
			int nTimeTag = m_kAlarm.TryPop();
			switch( nTimeTag )
			{
			case 10:
			case 5:
			case 4:
			case 3:
			case 2:
			case 1:
				{
					FLEventArenaHelper::BroadCastNotice( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_STARTCOUNT, _T( "%d" ), nTimeTag );
				}
				break;
			case 0:
				{
					DestroyWall();

					m_nState = EVENT_ARENA_STATE_PLAYING;

					m_kNoEnemyCheckAlarm.Start( NO_ENEMY_TEAM_CHECK );
					m_kAlarm.Start( m_pSpec->GetBattleTime() * 1000 );

					::GetLocalTime( &m_tBattleStart );

					FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
					GetGameScoreboard( kSocreboardPacket );
					FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );

					FLEventArenaHelper::BroadCastNotice( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_MATCHSTART, _T( "" ) );

					SyncGameInfo();

					if( m_pTonement != NULL && m_pRedTeam != NULL && m_pBlueTeam != NULL )
					{
						FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "EventArena Battle Start: Tonement: %s(%s), RedTeam: %s, BlueTeam: %s, BattleNumber: %d" ),
							m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName(), m_nBattleNumber );

#ifdef	__INTERNALSERVER
						FLINFO_LOG( PROGRAM_NAME, _T( "EventArena Battle Start: Tonement: %s(%s), RedTeam: %s, BlueTeam: %s, BattleNumber: %d" ),
							m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName(), m_nBattleNumber );
#endif
					}
				}
				break;
			}
		}
		break;
	case EVENT_ARENA_STATE_PLAYING:
		{
			if( m_kNoEnemyCheckAlarm.TryPop() >= 0 )
			{
				TryBattleOver();
			}

			if( m_kAlarm.TryPop() >= 0 )
			{
				CallDeathNPC();
			}
		}
		break;
	case EVENT_ARENA_STATE_RESULT:
		{
			if( m_kAlarm.IsOver() )
			{			
				if( IsGameOver() )
				{
					// 게임 종료면 GAME_STATE_END 상태 이동
					m_nState = EVENT_ARENA_STATE_END;
					SyncGameInfo();
				}
				else
				{
					// 그렇지 않다면 GAME_STATE_CALL 상태 투명벽 새우기 부활 및 100% 회복 대기지역 이동
					const T_WORLD_POSITION* pRedPos = m_pSpec->GetStandbyPos( EVENT_ARENA_RED );
					m_pRedTeam->NextBattle( pRedPos );

					const T_WORLD_POSITION* pBluePos = m_pSpec->GetStandbyPos( EVENT_ARENA_BLUE );
					m_pBlueTeam->NextBattle( pBluePos );

					m_nState = EVENT_ARENA_STATE_CALL;
					SyncGameInfo();

					FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
					GetGameScoreboard( kSocreboardPacket );
					FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );
				}
			}
		}
		break;
	}

	if( m_kTeamMoniter.IsOver() )
	{
		if( m_nState != EVENT_ARENA_STATE_CONFIG )
		{
			m_kTeamMoniter.Set( TEAM_MONITER );

			if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				FLSnapshotEventArenaManageRedBlueTeamInfoNoti kTeamInfo;
				m_pRedTeam->GetStream( &kTeamInfo.kRedTeamInfo );
				m_pBlueTeam->GetStream( &kTeamInfo.kBlueTeamInfo );
				SendToManager( &kTeamInfo );
			}
		}
	}
}

void	FLEventArenaGame::OnPlayerDied( CMover* pMover )
{
	if( IsPlaying() && IsValidObj( pMover ) && pMover->IsPlayer() )
	{
		FLWSUser* pPlayer = static_cast< FLWSUser* >( pMover );
		if( IsValidObj( pPlayer ) )
		{
			if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				const TCHAR* szTeamName = NULL;
				if( m_pBlueTeam->IsJoined( pPlayer ) )
				{
					szTeamName = m_pBlueTeam->GetName();
				}
				else if( m_pRedTeam->IsJoined( pPlayer ) )
				{
					szTeamName = m_pRedTeam->GetName();
				}

				if( szTeamName != NULL )
				{
					FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "Death Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );
#ifdef	__INTERNALSERVER
					FLINFO_LOG( PROGRAM_NAME, _T( "Death Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );
#endif

					TryBattleOver();

					FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
					GetGameScoreboard( kSocreboardPacket );
					FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );
				}
			}
		}
	}
}

void	FLEventArenaGame::OnPlayerResurrection( CMover* pMover )
{
	if( IsPlaying() && IsValidObj( pMover ) && pMover->IsPlayer() )
	{
		FLWSUser* pPlayer = static_cast< FLWSUser* >( pMover );
		if( IsValidObj( pPlayer ) )
		{
			if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				const TCHAR* szTeamName = NULL;
				if( m_pBlueTeam->IsJoined( pPlayer ) )
				{
					szTeamName = m_pBlueTeam->GetName();
				}
				else if( m_pRedTeam->IsJoined( pPlayer ) )
				{
					szTeamName = m_pRedTeam->GetName();
				}

				if( szTeamName != NULL )
				{
					FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "Resurrection Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );
#ifdef	__INTERNALSERVER
					FLINFO_LOG( PROGRAM_NAME, _T( "Resurrection Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );
#endif

					FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
					GetGameScoreboard( kSocreboardPacket );
					FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );
				}
			}
		}
	}
}

void	FLEventArenaGame::TryBattleOver()
{
	int nWinTeam = TryNoEnemyTeamWin();
	if( nWinTeam != EVENT_ARENA_RED && nWinTeam != EVENT_ARENA_BLUE )
	{
		nWinTeam = DecideBattleOver();
	}

	BattleEnd( nWinTeam );
}

int		FLEventArenaGame::TryNoEnemyTeamWin()	const
{
	if( IsPlaying() && m_pRedTeam != NULL && m_pBlueTeam != NULL )
	{
		if( m_pRedTeam->GetJoinedMemberCount() <= 0 )
		{
			return EVENT_ARENA_BLUE;
		}
		else if( m_pBlueTeam->GetJoinedMemberCount() <= 0 )
		{
			return EVENT_ARENA_RED;
		}
	}

	return 0;
}

int		FLEventArenaGame::DecideBattleOver()	const
{
	if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
	{
		if( m_pRedTeam->IsAllDied() == true )
		{
			return EVENT_ARENA_BLUE;
		}

		if( m_pBlueTeam->IsAllDied() == true )
		{
			return EVENT_ARENA_RED;
		}
	}

	return 0;
}

bool	FLEventArenaGame::IncWinCount( int nWinTeam )
{
	if( nWinTeam == EVENT_ARENA_RED )
	{
		if( 0 < m_nBattleNumber && m_nBattleNumber <= _countof( m_byRedWinFlag ) )
		{
			m_byRedWinFlag[m_nBattleNumber-1] = 1;
			return true;
		}
	}
	else if( nWinTeam == EVENT_ARENA_BLUE )
	{
		if( 0 < m_nBattleNumber && m_nBattleNumber <= _countof( m_byBlueWinFlag ) )
		{
			m_byBlueWinFlag[m_nBattleNumber-1] = 1;
			return true;
		}
	}

	return false;
}

void	FLEventArenaGame::BattleEnd( int nWinTeam )
{
	if( IncWinCount( nWinTeam ) == true )
	{
		DestroyDeathNPC();

		m_nState = EVENT_ARENA_STATE_RESULT;
		m_kNoEnemyCheckAlarm.Stop();
		m_kAlarm.Start( m_pSpec->GetWinnerDisplayTime() * 1000 );

		FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
		GetGameScoreboard( kSocreboardPacket );
		FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );

		SyncGameInfo();

		BattleResultLog( nWinTeam );

		if( IsGameOver() )
		{
			FLSnapshotEventArenaGameResultNoti kBattleResult;
			kBattleResult.nGameWinTeam = nWinTeam;
			kBattleResult.nRedWinCount = GetRedWinCount();
			kBattleResult.nBlueWinCount = GetBlueWinCount();
			FLStrcpy( kBattleResult.szRedName, _countof( kBattleResult.szRedName ), m_pRedTeam->GetName() );
			FLStrcpy( kBattleResult.szBlueName, _countof( kBattleResult.szBlueName ), m_pBlueTeam->GetName() );

			kBattleResult.dwWinnerDisplayTime = m_pSpec->GetWinnerDisplayTime();
			FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kBattleResult );

			const TCHAR* szWinTeamName = NULL;
			if( nWinTeam == EVENT_ARENA_RED )
			{
				szWinTeamName = m_pRedTeam->GetName();
			}
			else if( nWinTeam == EVENT_ARENA_BLUE )
			{
				szWinTeamName = m_pBlueTeam->GetName();
			}

			if( szWinTeamName )
			{
				FLEventArenaHelper::BroadCastChat( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_FINALWINNER, _T( "%s" ), szWinTeamName );
			}

			GameResultLog( _T( "GameEnd" ), _T( "Winner" ), nWinTeam );
		}
		else
		{
			FLSnapshotEventArenaBattleResultNoti kBattleResult;
			kBattleResult.nBattleWinTeam = nWinTeam;
			kBattleResult.nRedWinCount = GetRedWinCount();
			kBattleResult.nBlueWinCount = GetBlueWinCount();
			FLStrcpy( kBattleResult.szRedName, _countof( kBattleResult.szRedName ), m_pRedTeam->GetName() );
			FLStrcpy( kBattleResult.szBlueName, _countof( kBattleResult.szBlueName ), m_pBlueTeam->GetName() );

			kBattleResult.dwWinnerDisplayTime = m_pSpec->GetWinnerDisplayTime();
			FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kBattleResult );

			const TCHAR* szWinTeamName = NULL;
			if( nWinTeam == EVENT_ARENA_RED )
			{
				szWinTeamName = m_pRedTeam->GetName();
			}
			else if( nWinTeam == EVENT_ARENA_BLUE )
			{
				szWinTeamName = m_pBlueTeam->GetName();
			}

			if( szWinTeamName )
			{
				FLEventArenaHelper::BroadCastChat( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_WINNER, _T( "%s" ), szWinTeamName );
			}
		}
	}
}

void	FLEventArenaGame::BattleResultLog( int nWinTeam )
{
	FLEventArenaBattleResultInfo kBattleResultInfo;
	if( GetBattleResultInfo( nWinTeam, &kBattleResultInfo ) == false )
	{
		return;
	}

	SendBattleResultLog( &kBattleResultInfo );
	PrintBattleResultLog( &kBattleResultInfo );
}

bool	FLEventArenaGame::GetBattleResultInfo( int nWinTeam, FLEventArenaBattleResultInfo* pBattleResultInfo )
{
	if( m_nState != EVENT_ARENA_STATE_RESULT || m_pTonement == NULL || m_pRedTeam == NULL || m_pBlueTeam == NULL )
	{
		return false;
	}

	pBattleResultInfo->tTonement = *m_pTonement;
	pBattleResultInfo->tRedTeam = *(m_pRedTeam->GetInfo());
	pBattleResultInfo->tBlueTeam = *(m_pBlueTeam->GetInfo());

	//////////////////////////////////////////////////////////////////////////
	pBattleResultInfo->tBattleStart = m_tBattleStart;
	::GetLocalTime( &pBattleResultInfo->tBattleEnd );

	FILETIME tStartTime = { 0 };
	FILETIME tEndTime = { 0 };
	//::mem_set( &tStartTime, 0, sizeof( tStartTime ) );
	//::mem_set( &tEndTime, 0, sizeof( tEndTime ) );

	::SystemTimeToFileTime( &pBattleResultInfo->tBattleStart, &tStartTime );
	::SystemTimeToFileTime( &pBattleResultInfo->tBattleEnd, &tEndTime );

#define	ONEHUNDRED_NANOSECONDS	(10000000i64)	// 100-nanoseconds -> 10^-7

	ULONGLONG u64StartTime = (static_cast< ULONGLONG >( tStartTime.dwHighDateTime ) << 32) + tStartTime.dwLowDateTime;
	ULONGLONG u64EndTime = (static_cast< ULONGLONG >( tEndTime.dwHighDateTime ) << 32) + tEndTime.dwLowDateTime;
	pBattleResultInfo->u64BattleTime = (u64EndTime - u64StartTime + ONEHUNDRED_NANOSECONDS - 1) / ONEHUNDRED_NANOSECONDS;
	//////////////////////////////////////////////////////////////////////////

	pBattleResultInfo->nWinTeam = nWinTeam;
	if( pBattleResultInfo->nWinTeam == EVENT_ARENA_RED )
	{
		pBattleResultInfo->strWinTeam = m_pRedTeam->GetName();
	}
	else if( pBattleResultInfo->nWinTeam == EVENT_ARENA_BLUE )
	{
		pBattleResultInfo->strWinTeam = m_pBlueTeam->GetName();
	}
	else
	{
		return false;
	}

	pBattleResultInfo->nRedWinCount = GetRedWinCount();
	pBattleResultInfo->nBlueWinCount = GetBlueWinCount();

	m_pRedTeam->GetSurvivalMembers( pBattleResultInfo->vecRedSurvivalMembers );
	m_pBlueTeam->GetSurvivalMembers( pBattleResultInfo->vecBlueSurvivalMembers );
	return true;
}

void	FLEventArenaGame::SendBattleResultLog( FLEventArenaBattleResultInfo* pBattleResultInfo )
{
	if( pBattleResultInfo == NULL )
	{
		return;
	}

	// Client 전투 결과 정보
	T_BATTLE_RESULT_LOG tBattleLog = { 0 };
	//::mem_set( &tBattleLog, 0, sizeof( tBattleLog ) )

	tBattleLog.nBattleNumber = m_nBattleNumber;
	tBattleLog.nWinTeam = pBattleResultInfo->nWinTeam;
	FLStrcpy( tBattleLog.szWinTeamName, _countof( tBattleLog.szWinTeamName ), pBattleResultInfo->strWinTeam.c_str() );
	tBattleLog.u64BattleTime = pBattleResultInfo->u64BattleTime;
	tBattleLog.nRedSurvivalMembers = pBattleResultInfo->vecRedSurvivalMembers.size();
	tBattleLog.nBlueSurvivalMembers = pBattleResultInfo->vecBlueSurvivalMembers.size();

	m_vecBattleLog.push_back( tBattleLog );

	FLSnapshotEventArenaBattleResultLogNoti kBattleResultLog;
	kBattleResultLog.vecBattleResultLog = m_vecBattleLog;
	FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kBattleResultLog );
}

void	FLEventArenaGame::PrintBattleResultLog( FLEventArenaBattleResultInfo* pBattleResultInfo )
{
	if( pBattleResultInfo == NULL )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 파일 로그
	//////////////////////////////////////////////////////////////////////////
	TCHAR szStartTime[32] = _T( "" );
	TCHAR szEndTime[32] = _T( "" );
	FLSystemTimeToDBDateTimeString( &pBattleResultInfo->tBattleStart, szStartTime, _countof( szStartTime ) );
	FLSystemTimeToDBDateTimeString( &pBattleResultInfo->tBattleEnd, szEndTime, _countof( szEndTime ) );

	tstring strRedSurvialMembers;
	tstring strBlueSurvialMembers;
	FLEnumStringsIntoString( pBattleResultInfo->vecRedSurvivalMembers, strRedSurvialMembers, _T( "," ) );
	FLEnumStringsIntoString( pBattleResultInfo->vecBlueSurvivalMembers, strBlueSurvialMembers, _T( "," ) );

	FLINFO_LOG( EVENT_ARENA_LOG_NAME,
		_T( "[Tonement: %s, RedTeam: %s, BlueTeam: %s] Battle End. Winner: %s(R: %d / B: %d), BattleTime: %I64u second[StartTime: %s, EndTime: %s], RedSurvialMembers(%u): %s, BlueSurvialMembers(%u): %s" ),
		pBattleResultInfo->tTonement.szName, pBattleResultInfo->tRedTeam.szName, pBattleResultInfo->tBlueTeam.szName,
		pBattleResultInfo->strWinTeam.c_str(), pBattleResultInfo->nRedWinCount, pBattleResultInfo->nBlueWinCount,
		pBattleResultInfo->u64BattleTime, szStartTime, szEndTime,
		pBattleResultInfo->vecRedSurvivalMembers.size(), strRedSurvialMembers.c_str(),
		pBattleResultInfo->vecBlueSurvivalMembers.size(), strBlueSurvialMembers.c_str() );

#ifdef	__INTERNALSERVER
	FLINFO_LOG( PROGRAM_NAME,
		_T( "[Tonement: %s, RedTeam: %s, BlueTeam: %s] Battle End. Winner: %s(R: %d / B: %d), BattleTime: %I64u second[StartTime: %s, EndTime: %s], RedSurvialMembers(%u): %s, BlueSurvialMembers(%u): %s" ),
		pBattleResultInfo->tTonement.szName, pBattleResultInfo->tRedTeam.szName, pBattleResultInfo->tBlueTeam.szName,
		pBattleResultInfo->strWinTeam.c_str(), pBattleResultInfo->nRedWinCount, pBattleResultInfo->nBlueWinCount,
		pBattleResultInfo->u64BattleTime, szStartTime, szEndTime,
		pBattleResultInfo->vecRedSurvivalMembers.size(), strRedSurvialMembers.c_str(),
		pBattleResultInfo->vecBlueSurvivalMembers.size(), strBlueSurvialMembers.c_str() );
#endif
	//////////////////////////////////////////////////////////////////////////
}

void	FLEventArenaGame::GameResultLog( const TCHAR* szCode, const TCHAR* szReason, int nWinTeam )
{
	T_GAME_RESULT_INFO tGameResultInfo = { 0 };
	//::mem_set( &tGameResultInfo, 0, sizeof( tGameResultInfo ) );
	if( GetGameResultInfo( szCode, szReason, nWinTeam, &tGameResultInfo ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Game Result Info failed." ) );
		return;
	}

	PrintGameResultLog( &tGameResultInfo );
}

bool	FLEventArenaGame::GetGameResultInfo( const TCHAR* szCode, const TCHAR* szReason, int nWinTeam, T_GAME_RESULT_INFO* pGameResultInfo )
{
	if( pGameResultInfo == NULL || szCode == NULL || szReason == NULL )
	{
		return false;
	}

	if( m_nState == EVENT_ARENA_STATE_CONFIG || m_pTonement == NULL || m_pRedTeam == NULL || m_pBlueTeam == NULL )
	{
		return false;
	}

	pGameResultInfo->tTonement = *m_pTonement;
	pGameResultInfo->tRedTeam = *(m_pRedTeam->GetInfo());
	pGameResultInfo->tBlueTeam = *(m_pBlueTeam->GetInfo());

	FLStrcpy( pGameResultInfo->szCode, _countof( pGameResultInfo->szCode ), szCode );
	FLStrcpy( pGameResultInfo->szStopReason, _countof( pGameResultInfo->szStopReason ), szReason );

	pGameResultInfo->tGameStart = m_tGameStart;
	::GetLocalTime( &pGameResultInfo->tGameEnd );

	pGameResultInfo->nWinTeam = nWinTeam;
	if( pGameResultInfo->nWinTeam == EVENT_ARENA_RED )
	{
		FLStrcpy( pGameResultInfo->szWinTeam, _countof( pGameResultInfo->szWinTeam ), m_pRedTeam->GetName() );
	}
	else if( pGameResultInfo->nWinTeam == EVENT_ARENA_BLUE )
	{
		FLStrcpy( pGameResultInfo->szWinTeam, _countof( pGameResultInfo->szWinTeam ), m_pBlueTeam->GetName() );
	}

	pGameResultInfo->nRedWinCount = GetRedWinCount();
	pGameResultInfo->nBlueWinCount = GetBlueWinCount();
	return true;
}

void	FLEventArenaGame::PrintGameResultLog( T_GAME_RESULT_INFO* pGameResultInfo )
{
	if( pGameResultInfo == NULL )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 파일 로그
	//////////////////////////////////////////////////////////////////////////
	TCHAR szStartTime[32] = _T( "" );
	TCHAR szEndTime[32] = _T( "" );
	FLSystemTimeToDBDateTimeString( &pGameResultInfo->tGameStart, szStartTime, _countof( szStartTime ) );
	FLSystemTimeToDBDateTimeString( &pGameResultInfo->tGameEnd, szEndTime, _countof( szEndTime ) );

	FLINFO_LOG( EVENT_ARENA_LOG_NAME,
		_T( "[Tonement: %s, RedTeam: %s, BlueTeam: %s] %s(%s). Winner: %s(R: %d / B: %d), StartTime: %s, EndTime: %s" ),
		pGameResultInfo->tTonement.szName, pGameResultInfo->tRedTeam.szName, pGameResultInfo->tBlueTeam.szName,
		pGameResultInfo->szCode, pGameResultInfo->szStopReason,
		pGameResultInfo->szWinTeam, pGameResultInfo->nRedWinCount, pGameResultInfo->nBlueWinCount,
		szStartTime, szEndTime );

#ifdef	__INTERNALSERVER
	FLINFO_LOG( PROGRAM_NAME,
		_T( "[Tonement: %s, RedTeam: %s, BlueTeam: %s] %s(%s). Winner: %s(R: %d / B: %d), StartTime: %s, EndTime: %s" ),
		pGameResultInfo->tTonement.szName, pGameResultInfo->tRedTeam.szName, pGameResultInfo->tBlueTeam.szName,
		pGameResultInfo->szCode, pGameResultInfo->szStopReason,
		pGameResultInfo->szWinTeam, pGameResultInfo->nRedWinCount, pGameResultInfo->nBlueWinCount,
		szStartTime, szEndTime );
#endif
	//////////////////////////////////////////////////////////////////////////
}

bool	FLEventArenaGame::IsGameOver()	const
{
	if( m_pTonement != NULL )
	{
		if( GetRedWinCount() >= m_pTonement->nNeedWinCount )
		{
			return true;
		}

		if( GetBlueWinCount() >= m_pTonement->nNeedWinCount )
		{
			return true;
		}
	}

	return false;
}

bool	FLEventArenaGame::IsEnemy( CMover* pSrc, CMover* pTarget )	const
{
	if( IsValidObj( pSrc ) && IsValidObj( pTarget ) )
	{
		if( IsPlaying() )
		{
			if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				if( pSrc->IsPlayer() && pTarget->IsPlayer() )
				{
					FLWSUser* pSrcUser = static_cast< FLWSUser* >( pSrc );
					FLWSUser* pTargetUser = static_cast< FLWSUser* >( pTarget );
					if( IsValidObj( pSrcUser ) && IsValidObj( pTargetUser ) )
					{
						if( m_pRedTeam->IsJoined( pSrcUser ) && m_pBlueTeam->IsJoined( pTargetUser ) )
						{
							return true;
						}
						else if( m_pBlueTeam->IsJoined( pSrcUser ) && m_pRedTeam->IsJoined( pTargetUser ) )
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool	FLEventArenaGame::IsJoined( FLWSUser* pPlayer )	const
{
	if( IsPlaying() && IsValidObj( pPlayer ) )
	{
		if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
		{
			if( m_pRedTeam->IsJoined( pPlayer ) || m_pBlueTeam->IsJoined( pPlayer ) )
			{
				return true;
			}
		}
	}

	return false;
}

bool	FLEventArenaGame::IsPlaying()	const
{
	if( m_nState == EVENT_ARENA_STATE_PLAYING )
	{
		return true;
	}

	return false;
}

void	FLEventArenaGame::StartManage( FLWSUser* pManager, const FLEventArenaTeamManager* pTeamManager )
{
	if( m_pManager == NULL && IsValidObj( pManager ) )
	{
		if( pManager->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			//////////////////////////////////////////////////////////////////////////
			m_pManager = pManager;
			//////////////////////////////////////////////////////////////////////////

			FLSnapshotEventArenaManageWindowNoti kOpenPacket;
			kOpenPacket.tGameInfo = GetInfo();

			m_pSpec->GetAllTonement( kOpenPacket.vecTonements );
			pTeamManager->GetAllTeamInfo( kOpenPacket.vecTeams );

			SendToManager( &kOpenPacket );

			//////////////////////////////////////////////////////////////////////////
			if( m_nState != EVENT_ARENA_STATE_CONFIG && m_pRedTeam != NULL && m_pBlueTeam != NULL )
			{
				FLSnapshotEventArenaManageRedBlueTeamInfoNoti kTeamInfo;
				m_pRedTeam->GetStream( &kTeamInfo.kRedTeamInfo );
				m_pBlueTeam->GetStream( &kTeamInfo.kBlueTeamInfo );
				SendToManager( &kTeamInfo );
			}
			//////////////////////////////////////////////////////////////////////////

			FLINFO_LOG( PROGRAM_NAME, _T( "%s(%u) EventArena manager setting" ), m_pManager->GetName(), m_pManager->GetId() );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "User is not gamemaster User: %s(%u), Auth: %u" ),
				pManager->GetName(), pManager->GetId(), pManager->m_dwAuthorization );
		}
	}
	else
	{
		pManager->AddDefinedText( TID_MMI_EVENTARENA_DUPLICATIONADMIN );
	}
}

void	FLEventArenaGame::StopManage( FLWSUser* pManager, const TCHAR* szWhere )
{
	if( IsManager( pManager ) )
	{
		FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti kTeamInfoClosePacket;
		SendToManager( &kTeamInfoClosePacket );

		FLSnapshotEventArenaManageWindowCloseNoti kClosePacket;
		SendToManager( &kClosePacket );

		FLINFO_LOG( PROGRAM_NAME, _T( "%s(%s) EventArena manager unsetting" ), pManager->GetName(), szWhere );

		//////////////////////////////////////////////////////////////////////////
		m_pManager = NULL;
		//////////////////////////////////////////////////////////////////////////
	}
}

void	FLEventArenaGame::SetTonement( FLWSUser* pManager, const T_EVENT_ARENA_TONEMENT* pTonement )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_CONFIG )
	{
		m_pTonement = pTonement;

		SyncGameInfo();
	}
}

void	FLEventArenaGame::SetRedTeam( FLWSUser* pManager, FLEventArenaTeam* pRedTeam )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_CONFIG )
	{
		if( pRedTeam != NULL )
		{
			if( m_pBlueTeam == pRedTeam )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Blue Team equal RedTeam. BlueTeam: %s, RedTeam: %s" ), m_pBlueTeam->GetName(), m_pRedTeam->GetName() );
				return;
			}
		}

		m_pRedTeam = pRedTeam;

		SyncGameInfo();
	}
}

void	FLEventArenaGame::SetBlueTeam( FLWSUser* pManager, FLEventArenaTeam* pBlueTeam )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_CONFIG )
	{
		if( pBlueTeam != NULL )
		{
			if( m_pRedTeam == pBlueTeam )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Red Team equal Blue Team. RedTeam: %s, BlueTeam: %s" ), m_pRedTeam->GetName(), m_pBlueTeam->GetName() );
				return;
			}
		}

		m_pBlueTeam = pBlueTeam;

		SyncGameInfo();
	}
}

void	FLEventArenaGame::CallPlayers( FLWSUser* pManager )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_CONFIG && m_pTonement != NULL && m_pBlueTeam != NULL && m_pRedTeam != NULL )
	{
		m_nState = EVENT_ARENA_STATE_CALL_WAIT;
		m_kAlarm.Start( m_pSpec->GetCallWaitTime() * 1000 );

		::GetLocalTime( &m_tGameStart );

		m_pRedTeam->StartStandby( m_pSpec->GetCallWaitTime() );
		m_pBlueTeam->StartStandby( m_pSpec->GetCallWaitTime() );

		FLSnapshotEventArenaGameJoinTeamInfoNoti kJoinTeamInfo;
		GetGameJoinTeamMembers( kJoinTeamInfo.vecRedTeamMembers, kJoinTeamInfo.vecBlueTeamMembers );
		FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kJoinTeamInfo );

		FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
		GetGameScoreboard( kSocreboardPacket );
		FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );

		// 매니저 전달
		m_kTeamMoniter.Set( TEAM_MONITER, TRUE );

		FLSnapshotEventArenaManageWaitCallPlayersNoti kCallPacket;
		kCallPacket.dwWaitTimeout = m_pSpec->GetCallWaitTime();
		SendToManager( &kCallPacket );

		SyncGameInfo();

		FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "EventArena Game Start: Tonement: %s(%s), Red: %s, Blue: %s" ),
			m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName() );

#ifdef	__INTERNALSERVER
		FLINFO_LOG( PROGRAM_NAME, _T( "EventArena Game Start: Tonement: %s(%s), Red: %s, Blue: %s" ),
			m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName() );
#endif
	}
}

void	FLEventArenaGame::AcceptCallPlayer( FLWSUser* pPlayer )
{
	if( IsValidObj( pPlayer ) )
	{
		if( TryStandby( pPlayer ) == true )
		{
			const TCHAR* szTeamName = NULL;
			if( m_pBlueTeam->IsMember( pPlayer ) )
			{
				szTeamName = m_pBlueTeam->GetName();
			}
			else if( m_pRedTeam->IsMember( pPlayer ) )
			{
				szTeamName = m_pRedTeam->GetName();
			}

			if( szTeamName != NULL )
			{
				FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "Player called. Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );

#ifdef	__INTERNALSERVER
				FLINFO_LOG( PROGRAM_NAME, _T( "Player called. Team: %s, Player: %s" ), szTeamName, pPlayer->GetName() );
#endif
			}

			return;
		}

		pPlayer->AddDefinedText( TID_MMI_EVENTARENA_NPCENTERERROR );
	}
}

void	FLEventArenaGame::AllStandy()
{
	if( m_nState == EVENT_ARENA_STATE_CALL_WAIT )
	{
		if( m_pRedTeam != NULL )
		{
			const T_WORLD_POSITION* pPos = m_pSpec->GetStandbyPos( EVENT_ARENA_RED );
			if( pPos != NULL )
			{
				m_pRedTeam->Standby( pPos );
			}
		}

		if( m_pBlueTeam != NULL )
		{
			const T_WORLD_POSITION* pPos = m_pSpec->GetStandbyPos( EVENT_ARENA_BLUE );
			if( pPos != NULL )
			{
				m_pBlueTeam->Standby( pPos );
			}
		}
	}
}

bool	FLEventArenaGame::TryStandby( FLWSUser* pPlayer )
{
	if( m_nState != EVENT_ARENA_STATE_CONFIG && m_nState != EVENT_ARENA_STATE_END )
	{
		if( m_pRedTeam != NULL )
		{
			const T_WORLD_POSITION* pPos = m_pSpec->GetStandbyPos( EVENT_ARENA_RED );
			if( m_pRedTeam->Standby( pPlayer, pPos ) == true )
			{
				return true;
			}
		}

		if( m_pBlueTeam != NULL )
		{
			const T_WORLD_POSITION* pPos = m_pSpec->GetStandbyPos( EVENT_ARENA_BLUE );
			if( m_pBlueTeam->Standby( pPlayer, pPos ) == true )
			{
				return true;
			}
		}
	}

	return false;
}

bool	FLEventArenaGame::CanMoveCalledState()
{
	if( m_nState == EVENT_ARENA_STATE_CALL && m_pRedTeam != NULL && m_pBlueTeam != NULL )
	{
		if( m_pRedTeam->IsAllStandby() && m_pBlueTeam->IsAllStandby() )
		{
			return true;
		}
	}

	return false;
}

void	FLEventArenaGame::JoinPlayers( FLWSUser* pManager )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_CALLED )
	{
		if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
		{
			const T_WORLD_POSITION* pRedPos = m_pSpec->GetFightPos( EVENT_ARENA_RED );
			const T_WORLD_POSITION* pBluePos = m_pSpec->GetFightPos( EVENT_ARENA_BLUE );
			if( pRedPos != NULL && pBluePos != NULL )
			{
				m_pRedTeam->Join( pRedPos, m_pSpec->GetGiveItems() );
				m_pBlueTeam->Join( pBluePos, m_pSpec->GetGiveItems() );

				m_nState = EVENT_ARENA_STATE_JOIN;
				m_kAlarm.Start( m_pSpec->GetBattleStandbyTime() * 1000 );

				FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
				GetGameScoreboard( kSocreboardPacket );
				FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );

				FLSnapshotEventArenaManageWaitJoinPlayersNoti kJoinPacket;
				kJoinPacket.dwWaitTimeout = m_pSpec->GetBattleStandbyTime();
				SendToManager( &kJoinPacket );

				SyncGameInfo();
			}
		}
	}
}

void	FLEventArenaGame::StartBattle( FLWSUser* pManager )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_JOINED )
	{
		m_nState = EVENT_ARENA_STATE_COUNTDOWN;
		m_nBattleNumber += 1;

		std::vector< T_ALARM_DATA > vecCountdown;
		vecCountdown.push_back( T_ALARM_DATA( 0, 10 ) );		// 10
		vecCountdown.push_back( T_ALARM_DATA( 5 * 1000, 5 ) );	// 5
		vecCountdown.push_back( T_ALARM_DATA( 1 * 1000, 4 ) );	// 4
		vecCountdown.push_back( T_ALARM_DATA( 1 * 1000, 3 ) );	// 3
		vecCountdown.push_back( T_ALARM_DATA( 1 * 1000, 2 ) );	// 2
		vecCountdown.push_back( T_ALARM_DATA( 1 * 1000, 1 ) );	// 1
		vecCountdown.push_back( T_ALARM_DATA( 1 * 1000, 0 ) );	// 0
		m_kAlarm.Start( vecCountdown );

		SyncGameInfo();
	}
}

void	FLEventArenaGame::EndGame( FLWSUser* pManager )
{
	if( IsManager( pManager ) && m_nState == EVENT_ARENA_STATE_END )
	{
		if( m_pTonement != NULL && m_pRedTeam != NULL && m_pBlueTeam != NULL )
		{
			FLINFO_LOG( EVENT_ARENA_LOG_NAME, _T( "EventArena Game End. Tonement: %s(%s), RedTeam: %s, BlueTeam: %s" ),
				m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName() );

#ifdef	__INTERNALSERVER
			FLINFO_LOG( PROGRAM_NAME, _T( "EventArena Game End. Tonement: %s(%s), RedTeam: %s, BlueTeam: %s" ),
				m_pTonement->szName, m_pTonement->szDesc, m_pRedTeam->GetName(), m_pBlueTeam->GetName() );
#endif
		}

		TerminateGame();
	}
}

void	FLEventArenaGame::StopGame( FLWSUser* pManager, const TCHAR* szReason )
{
	if( IsManager( pManager ) && szReason != NULL )
	{
		GameResultLog( _T( "GameStop" ), szReason, 0 );

		TerminateGame();

		FLEventArenaHelper::BroadCastChat( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_MATCHSTOP_MESSAGE, _T( "" ) );
		FLEventArenaHelper::BroadCastNotice( m_pSpec->GetArenaWorldID(), TID_MMI_EVENTARENA_MATCHSTOP_MESSAGE, _T( "" ) );
	}
}

void	FLEventArenaGame::TerminateGame()
{
	DWORD dwTransformMoverID = m_pSpec->GetTransformMoverID();
	if( m_pRedTeam != NULL )
	{
		m_pRedTeam->EndGame( m_pSpec, dwTransformMoverID );
	}

	if( m_pBlueTeam != NULL )
	{
		m_pBlueTeam->EndGame( m_pSpec, dwTransformMoverID );
	}

	if( m_nState != EVENT_ARENA_STATE_CONFIG )
	{
		FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti kTeamInfoClosePacket;
		SendToManager( &kTeamInfoClosePacket );
	}

	Clear();

	FLSnapshotEventArenaGameJoinTeamInfoNoti kJoinTeamInfo;
	GetGameJoinTeamMembers( kJoinTeamInfo.vecRedTeamMembers, kJoinTeamInfo.vecBlueTeamMembers );
	FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kJoinTeamInfo );

	FLSnapshotEventArenaBattleResultLogNoti kBattleResultLog;
	kBattleResultLog.vecBattleResultLog = m_vecBattleLog;
	FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kBattleResultLog );

	FLSnapshotEventArenaGameScoreboardNoti kSocreboardPacket;
	GetGameScoreboard( kSocreboardPacket );
	FLEventArenaHelper::BroadCast( m_pSpec->GetArenaWorldID(), &kSocreboardPacket );

	SyncGameInfo();
}

T_EVENT_ARENA_GAME_INFO	FLEventArenaGame::GetInfo()	const
{
	T_EVENT_ARENA_GAME_INFO tInfo = { 0 };
	//::mem_set( &tInfo, 0, sizeof( tInfo ) );

	tInfo.nState = m_nState;
	if( m_pTonement != NULL )
	{
		tInfo.dwTonementID = m_pTonement->dwID;
	}

	if( m_pRedTeam != NULL )
	{
		tInfo.dwRedTeamID = m_pRedTeam->GetID();
	}

	if( m_pBlueTeam != NULL )
	{
		tInfo.dwBlueTeamID = m_pBlueTeam->GetID();
	}

	::memcpy( tInfo.byRedWinFlag, m_byRedWinFlag, sizeof( tInfo.byRedWinFlag ) );
	::memcpy( tInfo.byBlueWinFlag, m_byBlueWinFlag, sizeof( tInfo.byBlueWinFlag ) );

	tInfo.dwRemainBattleTime = m_pSpec->GetBattleTime() * 1000;
	if( m_nState == EVENT_ARENA_STATE_PLAYING )
	{
		tInfo.dwRemainBattleTime = m_kAlarm.GetReamin();
	}

	return tInfo;
}

int		FLEventArenaGame::GetRedWinCount()	const
{
	int nCount = 0;
	for( int i = 0; i < _countof( m_byRedWinFlag ); ++i )
	{
		if( m_byRedWinFlag[i] != 0 )
		{
			++nCount;
		}
	}

	return nCount;
}

int		FLEventArenaGame::GetBlueWinCount()	const
{
	int nCount = 0;
	for( int i = 0; i < _countof( m_byBlueWinFlag ); ++i )
	{
		if( m_byBlueWinFlag[i] != 0 )
		{
			++nCount;
		}
	}

	return nCount;
}

void	FLEventArenaGame::SyncGameInfo()
{
	FLSnapshotEventArenaManageGameInfoSyncNoti kSyncPacket;
	kSyncPacket.tGameInfo = GetInfo();
	SendToManager( &kSyncPacket );
}

void	FLEventArenaGame::GetGameScoreboard( FLSnapshotEventArenaGameScoreboardNoti& kScoreboardPacket )
{
	if( m_pTonement != NULL )
	{
		FLStrcpy( kScoreboardPacket.szTonementName, _countof( kScoreboardPacket.szTonementName ), m_pTonement->szName );
		FLStrcpy( kScoreboardPacket.szTonementDesc, _countof( kScoreboardPacket.szTonementDesc ), m_pTonement->szDesc );
	}

	if( m_pRedTeam != NULL )
	{
		FLStrcpy( kScoreboardPacket.szRedTeamName, _countof( kScoreboardPacket.szRedTeamName ), m_pRedTeam->GetName() );
		kScoreboardPacket.nRedWinCount = GetRedWinCount();

		if( m_nState >= EVENT_ARENA_STATE_JOIN )
		{
			kScoreboardPacket.nRedSurvivalMembers = m_pRedTeam->GetSurvivalMembersCount();
		}
	}

	if( m_pBlueTeam != NULL )
	{
		FLStrcpy( kScoreboardPacket.szBlueTeamName, _countof( kScoreboardPacket.szBlueTeamName ), m_pBlueTeam->GetName() );
		kScoreboardPacket.nBlueWinCount = GetBlueWinCount();

		if( m_nState >= EVENT_ARENA_STATE_JOIN )
		{
			kScoreboardPacket.nBlueSurvivalMembers = m_pBlueTeam->GetSurvivalMembersCount();
		}
	}

	switch( m_nState )
	{
	case EVENT_ARENA_STATE_CALL:
	case EVENT_ARENA_STATE_CALL_WAIT:
	case EVENT_ARENA_STATE_CALLED:
		kScoreboardPacket.byGameState = EVENT_ARENA_GAME_STATE_CALL;
		break;
	case EVENT_ARENA_STATE_JOIN:
	case EVENT_ARENA_STATE_JOINED:
	case EVENT_ARENA_STATE_COUNTDOWN:
		kScoreboardPacket.byGameState = EVENT_ARENA_GAME_STATE_JOIN;
		break;
	case EVENT_ARENA_STATE_PLAYING:
	case EVENT_ARENA_STATE_RESULT:
	case EVENT_ARENA_STATE_END:
		kScoreboardPacket.byGameState = EVENT_ARENA_GAME_STATE_PLAYING;
		break;

	case EVENT_ARENA_STATE_CONFIG:
	default:
		kScoreboardPacket.byGameState = EVENT_ARENA_GAME_STATE_NONE;
		break;
	}
}

void	FLEventArenaGame::GetGameJoinTeamMembers( std::vector< tstring >& vecRedTeamMembers, std::vector< tstring >& vecBlueTeamMembers )	const
{
	if( m_pRedTeam != NULL && m_pBlueTeam != NULL )
	{
		m_pRedTeam->GetAllMemberName( vecRedTeamMembers );
		m_pBlueTeam->GetAllMemberName( vecBlueTeamMembers );
	}
}

void	FLEventArenaGame::CreateWall()
{
	DWORD dwWallCtrl = m_pSpec->GetWallCtrlID();
	if( dwWallCtrl != 0 )
	{
		const T_WORLD_POSITION* pPos = m_pSpec->GetWallPos();
		if( pPos != NULL )
		{
			CObj* pWall = FLEventArenaHelper::CreateCtrl( dwWallCtrl, pPos->dwWorldID, pPos->tPos, 180, nDefaultLayer );
			if( IsValidObj( pWall ) )
			{
				m_vecWalls.push_back( pWall );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Wall create failed. Wall: %u, World: %u(%f, %f, %f) Layer: %d" ),
					dwWallCtrl, pPos->dwWorldID, pPos->tPos.x, pPos->tPos.y, pPos->tPos.z, nDefaultLayer );
			}
		}
	}
}

void	FLEventArenaGame::DestroyWall()
{
	// 투명벽 지우기
	std::vector< CObj* >::const_iterator pos = m_vecWalls.begin();
	std::vector< CObj* >::const_iterator end = m_vecWalls.end();

	for( ; pos != end; ++pos )
	{
		CObj* pWall = (*pos);
		if( IsValidObj( pWall ) )
		{
			pWall->Delete();
		}
	}

	m_vecWalls.clear();
}

void	FLEventArenaGame::CallDeathNPC()
{
	DWORD dwDeathNPC = m_pSpec->GetDeathNPCID();
	if( dwDeathNPC != 0 )
	{
		if( m_pRedTeam->CallDeathNPC( dwDeathNPC, m_pSpec, m_vecDeathNPCs ) == false ||
			m_pBlueTeam->CallDeathNPC( dwDeathNPC, m_pSpec, m_vecDeathNPCs ) == false )
		{
			//DestroyDeathNPC();
			FLERROR_LOG( PROGRAM_NAME, _T( "모든 사신이 소환되지 못했습니다." ) );
		}
	}
}

void	FLEventArenaGame::DestroyDeathNPC()
{
	// 사신 죽이기
	std::vector< CObj* >::const_iterator pos = m_vecDeathNPCs.begin();
	std::vector< CObj* >::const_iterator end = m_vecDeathNPCs.end();

	for( ; pos != end; ++pos )
	{
		CObj* pNPC = (*pos);
		if( IsValidObj( pNPC ) )
		{
			pNPC->Delete();
		}
	}

	m_vecDeathNPCs.clear();
}
