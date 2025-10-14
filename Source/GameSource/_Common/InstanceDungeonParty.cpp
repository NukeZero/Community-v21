// InstanceDungeonParty.cpp: implementation of the CInstanceDungeonParty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstanceDungeonParty.h"

#ifdef __WORLDSERVER
#include "Mover.h"
#include "Obj.h"
#include "../WorldServer/User.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstanceDungeonParty::CInstanceDungeonParty()
: CInstanceDungeonBase( IDTYPE_PARTY )
{
}

CInstanceDungeonParty::~CInstanceDungeonParty()
{
#ifdef __WORLDSERVER
	m_mapPLTime.clear();
#endif // __WORLDSERVER
}

CInstanceDungeonParty* CInstanceDungeonParty::GetInstance()
{
	static CInstanceDungeonParty sIDP;
	return & sIDP;
}

#ifdef __WORLDSERVER
BOOL CInstanceDungeonParty::EnteranceDungeon( FLWSUser* pUser, DWORD dwWorldId, const DWORD dwGuildID, DWORD eDungeonLevel )
{
	if( pUser->m_idparty > 0 )
		return TeleportToDungeon( pUser, dwWorldId, pUser->m_idparty, dwGuildID, eDungeonLevel );
	else
		pUser->AddDefinedText( TID_GAME_INSTANCE_PARTY );
	
	return FALSE;
}

BOOL CInstanceDungeonParty::LeaveDungeon( FLWSUser* pUser, DWORD dwWorldId )
{
	return LeaveToOutside( pUser, dwWorldId, pUser->GetLayer() );
}

int	CInstanceDungeonParty::GetDungeonKind( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		CWorld* pWorld = pUser->GetWorld();
		if( pWorld != NULL )
		{
			INDUN_INFO* pInfo = GetDungeonInfo( pUser->GetPartyId(), pWorld->GetID() );
			if( pInfo != NULL )
			{
				DUNGEON_DATA* pData = GetDuneonData( *pInfo );
				if( pData != NULL )
				{
					if( pData->bColosseumStyle == TRUE )
					{
						return IDKIND_COLOSSEUM;
					}
				}
			}
		}
	}

	return IDKIND_GENERAL;
}

BOOL CInstanceDungeonParty::SetPartyLeaveTime( DWORD dwPlayerId )
{
	FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
	if( !IsValidObj( pUser ) )
		return FALSE;
	
	CWorld* pWorld = pUser->GetWorld();
	if( !pWorld )
		return FALSE;
	
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
	{
		MAP_PLTIME::iterator it = m_mapPLTime.find( dwPlayerId );
		if( it == m_mapPLTime.end() )
			m_mapPLTime.insert( MAP_PLTIME::value_type( dwPlayerId, GetTickCount() + MIN( 3 ) ) );
		else
			it->second = GetTickCount() + MIN( 3 );
	}
	return TRUE;
}

void CInstanceDungeonParty::Process()
{
	CInstanceDungeonBase::UpdateState_OnlyColosseum();			//neodeath 상태 처리

	DWORD dwTickCount = GetTickCount();
	for( MAP_PLTIME::iterator it=m_mapPLTime.begin(); it!=m_mapPLTime.end(); )		//party leave 시간
	{
		if( it->second < dwTickCount )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
			m_mapPLTime.erase( it++ );

			if( !IsValidObj( pUser ) )
				return;
			
			CWorld* pWorld = pUser->GetWorld();
			if( !pWorld )
				return;
			
			if( static_cast< u_long >( pUser->GetLayer() ) != pUser->m_idparty && IsPartyDungeon( pWorld->GetID() ) )
				CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}
		else
			it++;
	}
}
#endif // __WORLDSERVER
