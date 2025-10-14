
#include "stdafx.h"
#include "FLEventArenaTeamManager.h"


FLEventArenaTeamManager::FLEventArenaTeamManager()
{
}

FLEventArenaTeamManager::~FLEventArenaTeamManager()
{
	Clear();
}

bool	FLEventArenaTeamManager::AddTeamMember( const TCHAR* pszName, const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember )
{
	FLEventArenaTeam* pTeam = FindTeam( pszName );
	if( pTeam == NULL )
	{
		pTeam = NewTeam( pszName );
	}

	if( pTeam == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "팀 생성에 실패하였습니다. TeamName: %s, MemberName: %s" ), pszName, pMember->szCharName );
		return false;
	}

	if( pTeam->AddMember( pMember ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "팀맴버 추가에 실패하였습니다. TeamName: %s, PartyID: %d, MemberName: %s" ), pszName, pMember->byPartyNumber, pMember->szCharName );
		return false;
	}

	return true;
}

bool	FLEventArenaTeamManager::Check()
{
	TEAM_CITER pos = m_vecTeams.begin();
	TEAM_CITER end = m_vecTeams.end();

	for( ; pos != end; ++pos )
	{
		const FLEventArenaTeam* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( !pTemp->Check() )
			{
				return false;
			}
		}
	}

	return true;
}

void	FLEventArenaTeamManager::Run()
{
	TEAM_ITER pos = m_vecTeams.begin();
	TEAM_ITER end = m_vecTeams.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeam* pTemp = (*pos);
		if( pTemp != NULL )
		{
			pTemp->Sync();
		}
	}
}

void	FLEventArenaTeamManager::GetAllTeamInfo( std::vector< T_EVENT_ARENA_TEAM_INFO >& vecTeams )	const
{
	TEAM_CITER pos = m_vecTeams.begin();
	TEAM_CITER end = m_vecTeams.end();

	for( ; pos != end; ++pos )
	{
		const FLEventArenaTeam* pTemp = (*pos);
		if( pTemp != NULL )
		{
			vecTeams.push_back( *(pTemp->GetInfo()) );
		}
	}
}

void	FLEventArenaTeamManager::Clear()
{
	TEAM_ITER pos = m_vecTeams.begin();
	TEAM_ITER end = m_vecTeams.end();

	for( ; pos != end; ++pos )
	{
		SAFE_DELETE( *pos );
	}

	m_vecTeams.clear();
}

FLEventArenaTeam*	FLEventArenaTeamManager::FindTeam( const TCHAR* pszName )	const
{
	FLEventArenaTeam* pFind = NULL;

	if( pszName != NULL )
	{
		TEAM_CITER pos = m_vecTeams.begin();
		TEAM_CITER end = m_vecTeams.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeam* pTemp = (*pos);
			if( pTemp != NULL )
			{
				if( pTemp->IsEqualName( pszName ) == true )
				{
					pFind = pTemp;
					break;
				}
			}
		}
	}

	return pFind;
}

FLEventArenaTeam*	FLEventArenaTeamManager::FindTeam( DWORD dwTeamID )	const
{
	if( 0 >= dwTeamID || dwTeamID > m_vecTeams.size() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "팀 인덱스 범위를 넘어 섰습니다. TeamID: %d" ), dwTeamID );
		return NULL;
	}

	return m_vecTeams[dwTeamID-1];
}

FLEventArenaTeam*	FLEventArenaTeamManager::FindMyTeam( const TCHAR* pszCharName )	const
{
	FLEventArenaTeam* pFind = NULL;

	TEAM_CITER pos = m_vecTeams.begin();
	TEAM_CITER end = m_vecTeams.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeam* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( pTemp->IsMember( pszCharName ) )
			{
				pFind = pTemp;
				break;
			}
		}
	}

	return pFind;
}

FLEventArenaTeam*	FLEventArenaTeamManager::FindMyTeam( FLWSUser* pUser )	const
{
	FLEventArenaTeam* pFind = NULL;

	if( IsValidObj( pUser ) )
	{
		TEAM_CITER pos = m_vecTeams.begin();
		TEAM_CITER end = m_vecTeams.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeam* pTemp = (*pos);
			if( pTemp != NULL )
			{
				if( pTemp->IsMember( pUser ) )
				{
					pFind = pTemp;
					break;
				}
			}
		}
	}

	return pFind;
}

FLEventArenaTeam*	FLEventArenaTeamManager::NewTeam( const TCHAR* pszName )
{
	T_EVENT_ARENA_TEAM_INFO tTeamInfo;// = { 0 };
	//::mem_set( &tTeamInfo, 0, sizeof( tTeamInfo ) );

	tTeamInfo.dwID = m_vecTeams.size()+1;
	FLStrcpy( tTeamInfo.szName, _countof( tTeamInfo.szName ), pszName );


	FLEventArenaTeam* pNewTeam = new FLEventArenaTeam( &tTeamInfo );
	if( pNewTeam == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 팀 생성 실패 ID: %d, Name: %s" ), tTeamInfo.dwID, tTeamInfo.szName );
		return NULL;
	}

	m_vecTeams.push_back( pNewTeam );
	return pNewTeam;
}
