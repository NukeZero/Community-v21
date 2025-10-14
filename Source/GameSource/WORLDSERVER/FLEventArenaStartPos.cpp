
#include "stdafx.h"
#include "FLEventArenaStartPos.h"


namespace
{
	D3DXVECTOR3	GetRandomRadiusPos( const D3DXVECTOR3& tPos, DWORD dwRadius )
	{
		D3DXVECTOR3 tRandPos = tPos;
		tRandPos.x = tPos.x + xRandom( dwRadius*2 ) - dwRadius;
		tRandPos.z = tPos.z + xRandom( dwRadius*2 ) - dwRadius;

		return tRandPos;
	}
}

FLEventArenaStartPos::FLEventArenaStartPos()
{
	m_nNextStartIdx = 0;
}

FLEventArenaStartPos::~FLEventArenaStartPos()
{
}

void	FLEventArenaStartPos::Clear()
{
	m_nNextStartIdx = 0;

	m_vecWorldIDs.clear();
	m_vecStart.clear();
	m_vecRedStandby.clear();
	m_vecRedFigth.clear();
	m_vecBlueStandby.clear();
	m_vecBlueFigth.clear();
	m_vecWall.clear();
	m_vecDeathNPC.clear();
}

bool	FLEventArenaStartPos::AddStartPos( const T_EVENT_ARENA_START_POSITION_SPEC* pStartPos )
{
	if( pStartPos == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pStartPos == NULL" ) );
		return false;
	}

	switch( pStartPos->nStartKind )
	{
	case EVENT_ARENA_START:
		{
			m_vecStart.push_back( *pStartPos );
			AddWorldID( pStartPos->tPosition.dwWorldID );
			return true;
		}
		break;

	case EVENT_ARENA_STANDBY:
		{
			if( pStartPos->nTeam == EVENT_ARENA_RED )
			{
				m_vecRedStandby.push_back( *pStartPos );
				AddWorldID( pStartPos->tPosition.dwWorldID );
				return true;
			}
			else if( pStartPos->nTeam == EVENT_ARENA_BLUE )
			{
				m_vecBlueStandby.push_back( *pStartPos );
				AddWorldID( pStartPos->tPosition.dwWorldID );
				return true;
			}
		}
		break;

	case EVENT_ARENA_FIGHT:
		{
			if( pStartPos->nTeam == EVENT_ARENA_RED )
			{
				m_vecRedFigth.push_back( *pStartPos );
				AddWorldID( pStartPos->tPosition.dwWorldID );
				return true;
			}
			else if( pStartPos->nTeam == EVENT_ARENA_BLUE )
			{
				m_vecBlueFigth.push_back( *pStartPos );
				AddWorldID( pStartPos->tPosition.dwWorldID );
				return true;
			}
		}
		break;

	case EVENT_ARENA_MONSTERSPAWN:
		{
			m_vecDeathNPC.push_back( *pStartPos );
			AddWorldID( pStartPos->tPosition.dwWorldID );
			return true;
		}
		break;

	case EVENT_ARENA_TRANSWALL:
		{
			m_vecWall.push_back( *pStartPos );
			AddWorldID( pStartPos->tPosition.dwWorldID );
			return true;
		}
		break;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 Kind 혹은 Team입니다. Kind: %d, Team: %d, WorldID: %d (%f, %f, %f)" ),
		pStartPos->nStartKind, pStartPos->nTeam,
		pStartPos->tPosition.dwWorldID, pStartPos->tPosition.tPos.x, pStartPos->tPosition.tPos.y, pStartPos->tPosition.tPos.z );

	return false;
}

void	FLEventArenaStartPos::AddWorldID( DWORD dwWorldID )
{
	std::vector< DWORD >::const_iterator kFound = std::find( m_vecWorldIDs.begin(), m_vecWorldIDs.end(), dwWorldID );
	if( kFound == m_vecWorldIDs.end() )
	{
		m_vecWorldIDs.push_back( dwWorldID );
	}
}

const std::vector< DWORD >&	FLEventArenaStartPos::GetWorldID()	const
{
	return m_vecWorldIDs;
}

const T_WORLD_POSITION*	FLEventArenaStartPos::GetStart()	const
{
	const T_WORLD_POSITION* pPos = NULL;

	if( !m_vecStart.empty() )
	{
		pPos = &(m_vecStart[m_nNextStartIdx].tPosition);
		++m_nNextStartIdx;
		m_nNextStartIdx = m_nNextStartIdx % m_vecStart.size();
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "위치정보가 존재하지 않습니다." ) );
	}

	return pPos;
}

const T_WORLD_POSITION*	FLEventArenaStartPos::GetStandby( int nTeam )	const
{
	if( nTeam == EVENT_ARENA_RED )
	{
		const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecRedStandby );
		if( pSpec != NULL )
		{
			return &pSpec->tPosition;
		}
	}
	else if( nTeam == EVENT_ARENA_BLUE )
	{
		const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecBlueStandby );
		if( pSpec != NULL )
		{
			return &pSpec->tPosition;
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 팀입니다. Team: %d" ), nTeam );
	return NULL;
}

const T_WORLD_POSITION*	FLEventArenaStartPos::GetFight( int nTeam )	const
{
	if( nTeam == EVENT_ARENA_RED )
	{
		const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecRedFigth );
		if( pSpec != NULL )
		{
			return &pSpec->tPosition;
		}
	}
	else if( nTeam == EVENT_ARENA_BLUE )
	{
		const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecBlueFigth );
		if( pSpec != NULL )
		{
			return &pSpec->tPosition;
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 팀입니다. Team: %d" ), nTeam );
	return NULL;
}

const T_WORLD_POSITION*	FLEventArenaStartPos::GetWall()	const
{
	const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecWall );
	if( pSpec != NULL )
	{
		return &pSpec->tPosition;
	}

	return NULL;
}

T_WORLD_POSITION	FLEventArenaStartPos::GetDeathNPC()	const
{
	T_WORLD_POSITION tPos;
	//::mem_set( &tPos, 0, sizeof( tPos ) );

	const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = GetRandPos( m_vecDeathNPC );
	if( pSpec != NULL )
	{
		tPos.dwWorldID = pSpec->tPosition.dwWorldID;
		tPos.tPos = GetRandomRadiusPos( pSpec->tPosition.tPos, pSpec->dwRadius );
	}

	return tPos;
}


const T_EVENT_ARENA_START_POSITION_SPEC*	FLEventArenaStartPos::GetRandPos( const START_POS_LIST& vecPosList )
{
	const T_EVENT_ARENA_START_POSITION_SPEC* pSpec = NULL;

	if( !vecPosList.empty() )
	{
		DWORD dwRandIndex = xRandom( vecPosList.size() );
		pSpec = &vecPosList[dwRandIndex];
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "위치정보가 존재하지 않습니다." ) );
	}

	return pSpec;
}
