#include "StdAfx.h"

#ifdef __WORLDSERVER

#include "FLPartyAggroFSM.h"



FLPartyAggroSubMonsterFSM::FLPartyAggroSubMonsterFSM( CMover & Owner, const DWORD dwAggroID ) : FLAggroNormalFSM( Owner, dwAggroID ) /* : m_PartyOwnerObjid( NULL_ID )*/
{

}

FLPartyAggroSubMonsterFSM::~FLPartyAggroSubMonsterFSM()
{

}

FLAggro * FLPartyAggroSubMonsterFSM::GetAggroMng()
{
	CMover * pOwner = prj.GetMover( m_dwPartyOwnerOBJID );
	if( IsValidObj( pOwner ) == TRUE && pOwner->IsLive() )
	{
		return pOwner->GetAggroMng();
	}

	return m_pAggro;//.get();
}

void FLPartyAggroSubMonsterFSM::sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 )
{
	// 파티 장이 살아 있다면
	CMover * pOwner = prj.GetMover( m_dwPartyOwnerOBJID );
	if( IsValidObj( pOwner ) == TRUE && pOwner->IsLive() == TRUE && pOwner->GetAggroMng() != NULL )
	{
		switch( dwMessage )		
		{
		case AIMSG_DSTDAMAGE:
			{
				const OBJID objid	= ( OBJID )dwParam1;
				const int nDamage	= ( int )dwParam2;
				UNREFERENCED_PARAMETER( nDamage );

				if( pOwner->GetAggroMng() != NULL )
					pOwner->GetAggroMng()->SumFirstTargetAggro( *pOwner, objid );
			}
			break;

		case AIMSG_DAMAGE:
			{
				const OBJID objid	= ( OBJID )dwParam1;
				const int nDamage	= ( dwParam2 == 0 ) ? 1 : ( int )dwParam2;

				if( pOwner->GetAggroMng() != NULL )
					if( pOwner->GetAggroMng()->SumFirstAttackerAggro( *pOwner, objid ) == FALSE )
						GetAggroMng()->SumAttackAggro( *pOwner, objid, nDamage );
			}
			break;
		}
		return;
	}

	// 파티장이 없다면
	FLAggroNormalFSM::sendAIMsg( dwMessage, dwParam1, dwParam2 );
}


#endif //__WORLDSERVER