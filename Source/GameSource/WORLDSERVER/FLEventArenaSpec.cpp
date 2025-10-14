
#include "stdafx.h"
#include "FLEventArenaSpec.h"


FLEventArenaSpec::FLEventArenaSpec()
{
	::memset( &m_tTimeConfig, 0, sizeof( m_tTimeConfig ) );

	m_dwWallCtrlID = 0;
	m_dwDeathNPCID = 0;
	m_dwTransformMoverID = 0;
}

FLEventArenaSpec::~FLEventArenaSpec()
{
}

void	FLEventArenaSpec::Clear()
{
	::memset( &m_tTimeConfig, 0, sizeof( m_tTimeConfig ) );

	m_vecTonements.clear();
	m_vecGiveItems.clear();

	m_kStartPos.Clear();

	m_dwWallCtrlID = 0;
	m_dwDeathNPCID = 0;
	m_dwTransformMoverID = 0;
}

bool	FLEventArenaSpec::AddTonement( const T_EVENT_ARENA_TONEMENT_SPEC* pTonement )
{
	if( pTonement == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pTonement == NULL" ) );
		return false;
	}

	if( pTonement->nNeedWinCount > MAX_EVENT_ARENA_WIN_COUNT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "토너먼트 필요 승수가 너무 많습니다. Tonement: %s, NeedWinCount: %d" ), pTonement->szName, pTonement->nNeedWinCount );
	}

	T_EVENT_ARENA_TONEMENT tTonement;
	tTonement.dwID = m_vecTonements.size()+1;
	::FLStrcpy( tTonement.szName, _countof( tTonement.szName ), pTonement->szName );
	tTonement.nNeedWinCount = pTonement->nNeedWinCount;
	::FLStrcpy( tTonement.szDesc, _countof( tTonement.szDesc ), pTonement->szDesc );

	m_vecTonements.push_back( tTonement );
	return true;
}

bool	FLEventArenaSpec::AddGiveItem( const T_EVENT_ARENA_GIVE_ITEM_SPEC* pGiveItem )
{
	if( pGiveItem == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pGiveItem == NULL" ) );
		return false;
	}

	m_vecGiveItems.push_back( *pGiveItem );
	return true;
}

bool	FLEventArenaSpec::SetTimeConfig( const T_EVENT_ARENA_TIME_CONFIG_SPEC* pTimeConfig )
{
	if( pTimeConfig == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pTimeConfig == NULL" ) );
		return false;
	}

	m_tTimeConfig = *pTimeConfig;
	return true;
}

bool	FLEventArenaSpec::AddStartPos( const T_EVENT_ARENA_START_POSITION_SPEC* pStartPos )
{
	if( pStartPos == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pTimeConfig == NULL" ) );
		return false;
	}

	if( m_kStartPos.AddStartPos( pStartPos ) == false )
	{
		return false;
	}

	return true;
}

void	FLEventArenaSpec::SetDeathNPCID( DWORD dwID )
{
	m_dwDeathNPCID = dwID;
}

void	FLEventArenaSpec::SetWallCtrlID( DWORD dwID )
{
	m_dwWallCtrlID = dwID;
}

void	FLEventArenaSpec::SetTransformMoverID( DWORD dwID )
{
	m_dwTransformMoverID = dwID;
}

const std::vector< DWORD >&	FLEventArenaSpec::GetArenaWorldID()			const
{
	return m_kStartPos.GetWorldID();
}

const T_WORLD_POSITION*	FLEventArenaSpec::GetStartPos()	const
{
	return m_kStartPos.GetStart();
}

const T_WORLD_POSITION*	FLEventArenaSpec::GetStandbyPos( int nTeam )	const
{
	return m_kStartPos.GetStandby( nTeam );
}

const T_WORLD_POSITION*	FLEventArenaSpec::GetFightPos( int nTeam )	const
{
	return m_kStartPos.GetFight( nTeam );
}

const T_WORLD_POSITION*	FLEventArenaSpec::GetWallPos()	const
{
	return m_kStartPos.GetWall();
}

T_WORLD_POSITION	FLEventArenaSpec::GetDeathNPCPos()	const
{
	return m_kStartPos.GetDeathNPC();
}

const T_EVENT_ARENA_TONEMENT*	FLEventArenaSpec::GetTonement( DWORD dwTonementID )	const
{
	if( 0 >= dwTonementID || dwTonementID > m_vecTonements.size() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "토너먼트 인덱스를 넘어섰습니다. TonementID: %d" ), dwTonementID );
		return NULL;
	}

	return &m_vecTonements[dwTonementID-1];
}

void	FLEventArenaSpec::GetAllTonement( std::vector< T_EVENT_ARENA_TONEMENT >& vecTonements )	const
{
	vecTonements = m_vecTonements;
}

DWORD	FLEventArenaSpec::GetCallWaitTime()	const
{
	return m_tTimeConfig.dwCallWaitTime;
}

DWORD	FLEventArenaSpec::GetBattleStandbyTime()	const
{
	return m_tTimeConfig.dwBattleStandbyTime;
}

DWORD	FLEventArenaSpec::GetBattleTime()	const
{
	return m_tTimeConfig.dwBattleTime;
}

DWORD	FLEventArenaSpec::GetWinnerDisplayTime()	const
{
	return m_tTimeConfig.dwWinnerDisplayTime;
}

const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >&	FLEventArenaSpec::GetGiveItems()	const
{
	return m_vecGiveItems;
}

DWORD	FLEventArenaSpec::GetWallCtrlID()	const
{
	return m_dwWallCtrlID;
}

DWORD	FLEventArenaSpec::GetDeathNPCID()	const
{
	return m_dwDeathNPCID;
}

DWORD	FLEventArenaSpec::GetTransformMoverID()	const
{
	return m_dwTransformMoverID;
}
