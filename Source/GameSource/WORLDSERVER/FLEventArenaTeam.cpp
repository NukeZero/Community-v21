
#include "stdafx.h"
#include "FLEventArenaTeam.h"



FLEventArenaTeam::FLEventArenaTeam( const T_EVENT_ARENA_TEAM_INFO* pInfo )
{
	m_tInfo = *pInfo;
}

FLEventArenaTeam::~FLEventArenaTeam()
{
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		SAFE_DELETE( *pos );
	}

	m_vecMembers.clear();
}

bool	FLEventArenaTeam::AddMember( const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember )
{
	if( pMember == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pMember == NULL" ) );
		return false;
	}

	FLEventArenaTeamMember* pNewMember = new FLEventArenaTeamMember( pMember );
	if( pNewMember == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "맴버 생성 실패, TeamName: %s, PartyID: %d, MemberName: %s" ), m_tInfo.szName, pMember->byPartyNumber, pMember->szCharName );
		return false;
	}

	m_vecMembers.push_back( pNewMember );
	return true;
}

bool	FLEventArenaTeam::Check()	const
{
	int nMemberCount[MAX_PARTY] = { 0, };

	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pMember = (*pos);
		if( pMember != NULL )
		{
			BYTE byPartyNumber = pMember->GetInfo().byPartyNumber;
			if( byPartyNumber <= 0 || byPartyNumber > MAX_PARTY )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "파티 인덱스 오류, TeamName: %s, PartyID: %d, MemberName: %s" ), m_tInfo.szName, byPartyNumber, pMember->GetInfo().szCharName );
				return false;
			}

			++nMemberCount[pMember->GetInfo().byPartyNumber-1];
		}
	}

	for( int i = 0; i < _countof( nMemberCount ); ++i )
	{
		if( nMemberCount[i] == 0 )
		{
			continue;
		}

		if( nMemberCount[i] < MIN_MEMBER || nMemberCount[i] > MAX_MEMBER )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "파티 맴버수 오류, TeamName: %s, PartyID: %d" ), m_tInfo.szName, i+1 );
			return false;
		}
	}

	return true;
}

bool	FLEventArenaTeam::OnLogin( FLWSUser* pUser )
{
	FLEventArenaTeamMember* pMember = Find( pUser->GetName() );
	if( pMember == NULL )
	{
		return false;
	}

	pMember->Login( pUser );

	FLSnapshotEventArenaTeamInfoNoti kInfoPacket;
	GetStream( &kInfoPacket.kTeamInfo );
	pMember->SendPacket( &kInfoPacket );
	return true;
}

bool	FLEventArenaTeam::OnLogout( FLWSUser* pUser )
{
	FLEventArenaTeamMember* pMember = Find( pUser );
	if( pMember == NULL )
	{
		return false;
	}

	pMember->Logout();
	return true;
}

void	FLEventArenaTeam::Sync()
{
	FLSnapshotEventArenaTeamMemberUpdateNoti kUpdatePacket;

	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pMember = (*pos);
		if( pMember != NULL )
		{
			pMember->Sync( kUpdatePacket.vecChangedMembers );
		}
	}

	if( !kUpdatePacket.vecChangedMembers.empty() )
	{
		SendPacket( &kUpdatePacket );
	}
}

void	FLEventArenaTeam::SendTeamChat( FLWSUser* pUser, const TCHAR* pszChat )
{
	if( IsValidObj( pUser ) )
	{
		MEMBER_ITER pos = m_vecMembers.begin();
		MEMBER_ITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeamMember* pMember = (*pos);
			if( pMember != NULL )
			{
				pMember->SendChat( pUser, pszChat );
			}
		}
	}
}

void	FLEventArenaTeam::StartStandby( DWORD dwCallWaitTime )
{
	FLSnapshotEventArenaCallPlayerNoti kCallPacket;
	kCallPacket.dwWaitTimeout = dwCallWaitTime;
	SendPacket( &kCallPacket );
}

void	FLEventArenaTeam::Standby( const T_WORLD_POSITION* pPos )
{
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( pTemp->CanStandby() )
			{
				pTemp->Standby( pPos );
			}
		}
	}
}

bool	FLEventArenaTeam::Standby( FLWSUser* pUser, const T_WORLD_POSITION* pPos )
{
	FLEventArenaTeamMember* pMember = Find( pUser );
	if( pMember != NULL )
	{
		if( pMember->CanStandby() )
		{
			pMember->Standby( pPos );
			return true;
		}
	}

	return false;
}

void	FLEventArenaTeam::Join( const T_WORLD_POSITION* pPos, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem )
{
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			pTemp->Join( pPos, vecGiveItem );
		}
	}
}

bool	FLEventArenaTeam::CallDeathNPC( DWORD dwNPCID, const FLEventArenaSpec* pSpec, std::vector< CObj* >& vecDeathNPCs )
{
	bool bResult = true;
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( pTemp->CallDeathNPC( dwNPCID, pSpec, vecDeathNPCs ) == false )
			{
				bResult = false;
			}
		}
	}

	return bResult;
}

void	FLEventArenaTeam::NextBattle( const T_WORLD_POSITION* pPos )
{
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			pTemp->NextBattle( pPos );
		}
	}
}

void	FLEventArenaTeam::EndGame( const FLEventArenaSpec* pSpec, DWORD dwTransformMoverID )
{
	MEMBER_ITER pos = m_vecMembers.begin();
	MEMBER_ITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			pTemp->EndGame( pSpec, dwTransformMoverID );
		}
	}
}

DWORD	FLEventArenaTeam::GetID()	const
{
	return m_tInfo.dwID;
}

const TCHAR*	FLEventArenaTeam::GetName()	const
{
	return m_tInfo.szName;
}

const T_EVENT_ARENA_TEAM_INFO*	FLEventArenaTeam::GetInfo()	const
{
	return &m_tInfo;
}

void	FLEventArenaTeam::GetStream( FLEventArenaTeamStream* pStream )	const
{
	if( pStream != NULL )
	{
		pStream->tTeamInfo = m_tInfo;

		MEMBER_CITER pos = m_vecMembers.begin();
		MEMBER_CITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeamMember* pTemp = (*pos);
			if( pTemp != NULL )
			{
				pStream->vecMembers.push_back( pTemp->GetInfo() );
			}
		}
	}
}

void	FLEventArenaTeam::GetAllMemberName( std::vector< tstring >& vecMembers )	const
{
	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			vecMembers.push_back( pTemp->GetInfo().szCharName );
		}
	}
}

void	FLEventArenaTeam::GetSurvivalMembers( std::vector< tstring >& vecSurvivalMembers )	const
{
	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( !pTemp->IsDied() )
			{
				vecSurvivalMembers.push_back( pTemp->GetInfo().szCharName );
			}
		}
	}
}

int		FLEventArenaTeam::GetSurvivalMembersCount()	const
{
	int nCount = 0;

	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( !pTemp->IsDied() )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

size_t	FLEventArenaTeam::GetJoinedMemberCount()	const
{
	size_t nCount = 0;
	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		FLEventArenaTeamMember* pTemp = (*pos);
		if( pTemp != NULL )
		{
			if( pTemp->IsJoined() == true )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

bool	FLEventArenaTeam::IsEqualName( const TCHAR* pszName )	const
{
	if( pszName == NULL )
	{
		return false;
	}

	if( ::_tcscmp( pszName, m_tInfo.szName ) == 0 )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeam::IsMember( FLWSUser* pUser )	const
{
	if( IsValidObj( pUser ) )
	{
		MEMBER_CITER pos = m_vecMembers.begin();
		MEMBER_CITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			const FLEventArenaTeamMember* pMember = (*pos);
			if( pMember != NULL )
			{
				if( pMember->IsEqualUser( pUser ) == true )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool	FLEventArenaTeam::IsMember( const TCHAR* pszCharName )	const
{
	if( Find( pszCharName ) != NULL )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeam::IsAllStandby()	const
{
	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		const FLEventArenaTeamMember* pMember = (*pos);
		if( pMember != NULL )
		{
			if( !pMember->IsStandby() )
			{
				return false;
			}
		}
	}

	return true;
}

bool	FLEventArenaTeam::IsJoined( FLWSUser* pUser )	const
{
	FLEventArenaTeamMember* pMember = Find( pUser );
	if( pMember )
	{
		return pMember->IsJoined();
	}

	return false;
}

bool	FLEventArenaTeam::IsAllDied()	const
{
	MEMBER_CITER pos = m_vecMembers.begin();
	MEMBER_CITER end = m_vecMembers.end();

	for( ; pos != end; ++pos )
	{
		const FLEventArenaTeamMember* pMember = (*pos);
		if( pMember != NULL )
		{
			if( !pMember->IsDied() )
			{
				return false;
			}
		}
	}

	return true;
}

void	FLEventArenaTeam::SendPacket( const FLPacket* pPacket )
{
	if( pPacket != NULL )
	{
		MEMBER_ITER pos = m_vecMembers.begin();
		MEMBER_ITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeamMember* pMember = (*pos);
			if( pMember != NULL )
			{
				pMember->SendPacket( pPacket );
			}
		}
	}
}

FLEventArenaTeamMember*	FLEventArenaTeam::Find( const TCHAR* pszName )	const
{
	if( pszName != NULL )
	{
		MEMBER_CITER pos = m_vecMembers.begin();
		MEMBER_CITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeamMember* pMember = (*pos);
			if( pMember != NULL )
			{
				if( pMember->IsEqualName( pszName ) == true )
				{
					return pMember;
				}
			}
		}
	}

	return NULL;
}

FLEventArenaTeamMember*	FLEventArenaTeam::Find( FLWSUser* pUser )	const
{
	if( IsValidObj( pUser ) )
	{
		MEMBER_CITER pos = m_vecMembers.begin();
		MEMBER_CITER end = m_vecMembers.end();

		for( ; pos != end; ++pos )
		{
			FLEventArenaTeamMember* pMember = (*pos);
			if( pMember != NULL )
			{
				if( pMember->IsEqualUser( pUser ) == true )
				{
					return pMember;
				}
			}
		}
	}

	return NULL;
}
