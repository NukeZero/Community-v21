
#include "stdafx.h"
#include "FLEventArenaTeamMember.h"
#include "FLEventArenaHelper.h"


FLEventArenaTeamMember::FLEventArenaTeamMember( const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember )
{
	Clear();

	m_tInfo.byPartyNumber = pMember->byPartyNumber;
	FLStrcpy( m_tInfo.szCharName, _countof( m_tInfo.szCharName ), pMember->szCharName );
	m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_LOGOUT;

	m_tOldInfo = m_tInfo;
}

FLEventArenaTeamMember::~FLEventArenaTeamMember()
{
	Clear();
}

void	FLEventArenaTeamMember::Clear()
{
	::memset( &m_tInfo, 0, sizeof( m_tInfo ) );
	::memset( &m_tOldInfo, 0, sizeof( m_tOldInfo ) );

	m_pUser = NULL;
}

void	FLEventArenaTeamMember::Login( FLWSUser* pUser )
{
	m_pUser = pUser;
	m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_LOGIN;
}

void	FLEventArenaTeamMember::Logout()
{
	m_pUser = NULL;
	m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_LOGOUT;
}

void	FLEventArenaTeamMember::Sync( std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >& vecChangedMembers )
{
	Update();
	if( IsChanged() )
	{
		m_tOldInfo = m_tInfo;
		vecChangedMembers.push_back( m_tInfo );
	}
}

void	FLEventArenaTeamMember::Update()
{
	if( IsValidObj( m_pUser ) )
	{
		m_tInfo.dwObjID = m_pUser->GetId();
		m_tInfo.nCurrentHP = m_pUser->GetHitPoint();
		m_tInfo.nMaxHP = m_pUser->GetMaxHitPoint();
	}
	else
	{
		m_tInfo.dwObjID = 0;
		m_tInfo.nCurrentHP = 0;
		m_tInfo.nMaxHP = 0;
	}
}

bool	FLEventArenaTeamMember::IsChanged()	const
{
	if( ::memcmp( &m_tInfo, &m_tOldInfo, sizeof( m_tOldInfo ) ) == 0 )
	{
		return false;
	}

	return true;
}

void	FLEventArenaTeamMember::SendChat( FLWSUser* pUser, const TCHAR* pszChat )
{
	if( IsValidObj( pUser ) && pszChat && IsValidObj( m_pUser ) )
	{
		m_pUser->AddPartyChat( pUser->GetName(), pszChat, pUser->GetId() );
	}
}

void	FLEventArenaTeamMember::Standby( const T_WORLD_POSITION* pPos )
{
	if( IsLogined() )
	{
		FLEventArenaHelper::ClearInven( m_pUser );
		FLEventArenaHelper::ClearBuff( m_pUser );
		FLEventArenaHelper::AllRecovery( m_pUser );
		FLEventArenaHelper::Move( m_pUser, pPos, m_pUser->GetLayer() );
		FLEventArenaHelper::NoDisguise( m_pUser );

		m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT;
	}
}

void	FLEventArenaTeamMember::Join( const T_WORLD_POSITION* pPos, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem )
{
	if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT )
	{
		FLEventArenaHelper::GiveItems( m_pUser, vecGiveItem );
		FLEventArenaHelper::Move( m_pUser, pPos, m_pUser->GetLayer() );

		m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE;
	}
}

bool	FLEventArenaTeamMember::CallDeathNPC( DWORD dwNPCID, const FLEventArenaSpec* pSpec, std::vector< CObj* >& vecDeathNPCs )
{
	if( IsValidObj( m_pUser ) && IsJoined() )
	{
		T_WORLD_POSITION tPos = pSpec->GetDeathNPCPos();
		CObj* pObj = FLEventArenaHelper::CreateNPC( dwNPCID, m_pUser->GetId(), tPos.dwWorldID, tPos.tPos, m_pUser->GetLayer() );
		if( pObj )
		{
			vecDeathNPCs.push_back( pObj );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "NPC 생성 실패 NPC: %u, User: %s, World: %u (%f, %f, %f) Layer: %d" ),
				dwNPCID, m_pUser->GetName(), tPos.dwWorldID, tPos.tPos.x, tPos.tPos.y, tPos.tPos.z, m_pUser->GetLayer() );

			return false;
		}
	}

	return true;
}

void	FLEventArenaTeamMember::NextBattle( const T_WORLD_POSITION* pPos )
{
	if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE )
	{
		FLEventArenaHelper::ClearInven( m_pUser );
		FLEventArenaHelper::ClearBuff( m_pUser );
		FLEventArenaHelper::AllRecovery( m_pUser );
		FLEventArenaHelper::Move( m_pUser, pPos, m_pUser->GetLayer() );

		m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT;
	}
}

void	FLEventArenaTeamMember::EndGame( const FLEventArenaSpec* pSpec, DWORD dwTransformMoverID )
{
	if( IsLogined() )
	{
		if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT || m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE )
		{
			const T_WORLD_POSITION* pPos = pSpec->GetStartPos();
			if( pPos != NULL )
			{
				FLEventArenaHelper::ClearInven( m_pUser );
				FLEventArenaHelper::ClearBuff( m_pUser );
				FLEventArenaHelper::AllRecovery( m_pUser );
				FLEventArenaHelper::Move( m_pUser, pPos, m_pUser->GetLayer() );
				FLEventArenaHelper::Disguise( m_pUser, dwTransformMoverID );

				m_tInfo.nState = EVENT_ARENA_TEAM_MEMBER_STATE_LOGIN;
			}
		}
	}
}

OBJID	FLEventArenaTeamMember::GetObjID()	const
{
	OBJID dwOjbID = 0;
	if( IsValidObj( m_pUser ) )
	{
		dwOjbID = m_pUser->GetId();
	}

	return dwOjbID;
}

const T_EVENT_ARENA_TEAM_MEMBER_INFO&	FLEventArenaTeamMember::GetInfo()	const
{
	return m_tInfo;
}

bool	FLEventArenaTeamMember::IsEqualName( const TCHAR* pszName )	const
{
	if( pszName == NULL )
	{
		return false;
	}

	if( ::_tcscmp( pszName, m_tInfo.szCharName ) == 0 )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeamMember::IsEqualUser( FLWSUser* pUser )	const
{
	if( IsLogined() == false )
	{
		return false;
	}

	if( m_pUser == pUser )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeamMember::IsLogined()	const
{
	if( IsValidObj( m_pUser ) && m_tInfo.nState != EVENT_ARENA_TEAM_MEMBER_STATE_LOGOUT )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeamMember::CanStandby()	const
{
	if( IsLogined() == false )
	{
		return false;
	}

	if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_LOGIN )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeamMember::IsStandby()	const
{
	if( IsLogined() == false )
	{
		return true;
	}

	if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT )
	{
		if( IsValidObj( m_pUser ) )
		{
			if( !m_pUser->IsDie() )
			{
				return true;
			}
		}
	}

	return false;
}

bool	FLEventArenaTeamMember::IsJoined()	const
{
	if( m_tInfo.nState == EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE )
	{
		return true;
	}

	return false;
}

bool	FLEventArenaTeamMember::IsDied()	const
{
	if( m_tInfo.nState != EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE )
	{
		return true;
	}

	if( IsValidObj( m_pUser ) )
	{
		if( m_pUser->IsDie() )
		{
			return true;
		}
	}

	return false;
}

void	FLEventArenaTeamMember::SendPacket( const FLPacket* pPacket )
{
	if( pPacket != NULL )
	{
		if( IsValidObj( m_pUser ) )
		{
			m_pUser->AddPacket( pPacket );
		}
	}
}
