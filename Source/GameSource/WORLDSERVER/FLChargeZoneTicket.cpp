
#include "StdAfx.h"
#include "FLChargeZoneTicket.h"

#include "FLItemUsing.h"
#include <worldmng.h>

extern	CWorldMng	g_WorldMng;


FLChargeZoneTicket::FLChargeZoneTicket()
{

}

FLChargeZoneTicket::~FLChargeZoneTicket()
{

}

FLChargeZoneTicket*		FLChargeZoneTicket::GetInstance()
{
	static FLChargeZoneTicket kChargeZoneTicket;

	return & kChargeZoneTicket;
}

void	FLChargeZoneTicket::OnDoUseChargeZoneTicket( FLWSUser* pUser, DWORD dwTicketObjID )
{
	if( CanUseChargeZoneTicket( pUser, dwTicketObjID ) != false )
	{
		if( g_xSpecManager->IsChargeZoneWorld( pUser->GetWorld()->GetID() ) == false )
		{
			FLSnapshotChargeZoneTicketEnteranceNoti kPacket;
			kPacket.dwTicketObjID = dwTicketObjID;
			pUser->AddPacket( &kPacket );
		}
		else
		{
			FLSnapshotChargeZoneTicketLeaveNoti kPacket;
			kPacket.dwTicketObjID = dwTicketObjID;
			pUser->AddPacket( &kPacket );
		}
	}
}

void	FLChargeZoneTicket::EnterChargeZoneWorld( FLWSUser* pUser, FLPacketChargeZoneTicketEnteranceReq* pMsg )
{
	if( pMsg == NULL )
	{
		return;
	}

	if( CanUseChargeZoneTicket( pUser, pMsg->dwTicketObjID ) == false )
	{
		return;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwTicketObjID );

	if( IsMatching( pItemElem->m_dwItemId, pMsg->dwWorldID, pMsg->byChannel ) == false )
	{
		return;
	}

	D3DXVECTOR3 kPosition;
	if( GetStartingPosition( pItemElem->m_dwItemId, pMsg->dwWorldID, kPosition ) == false )
	{
		pUser->AddDefinedText( TID_GAME_NOMOVING );
		return;
	}
	
	if( pItemElem->m_dwKeepTime == 0 )
	{
		pUser->UpdateItem( static_cast<BYTE>( pItemElem->m_dwObjId ), UI_KEEPTIME, pItemElem->GetProp()->dwAbilityMin );

		LogItemInfo	kLogItem;
		//kLogItem.Action		= "e";
		//kLogItem.SendName	= pUser->GetName();
		//kLogItem.RecvName	= "DoUseItemTicket";
		FLStrcpy( kLogItem.Action, _countof( kLogItem.Action ), "e" );
		FLStrcpy( kLogItem.SendName, _countof( kLogItem.SendName ), pUser->GetName() );
		FLStrcpy( kLogItem.RecvName, _countof( kLogItem.RecvName ), "DoUseItemTicket" );
		kLogItem.WorldId	= pUser->GetWorld()->GetID();
		kLogItem.Gold		= pUser->GetGold();
		kLogItem.Gold2		= pUser->GetGold();
		g_DPSrvr.OnLogItem( kLogItem, pItemElem, pItemElem->m_nItemNum );
	}

	pItemElem->SetFlag( FLItemElem::is_using );
	pUser->UpdateItem( static_cast<BYTE>( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );

	int nLayer	= static_cast<int>( pMsg->byChannel );
	pUser->REPLACE( g_uIdofMulti, pMsg->dwWorldID, kPosition, REPLACE_NORMAL, -nLayer );
}

void	FLChargeZoneTicket::LeaveChargeZoneWorld( FLWSUser* pUser, FLPacketChargeZoneTicketLeaveReq* pMsg )
{
	if( pMsg == NULL )
	{
		return;
	}

	if( CanUseChargeZoneTicket( pUser, pMsg->dwTicketObjID ) == false )
	{
		return;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwTicketObjID );
	if( pItemElem->IsFlag( FLItemElem::is_using ) == FALSE )		// 유료 지역 입장 시 사용했던 티켓이 아님.
	{
		return;
	}

	int nLayer	= pUser->GetLayer();
	if( nLayer < 0 )
	{
		nLayer	= -nLayer;
	}

	if( IsMatching( pItemElem->m_dwItemId, pUser->GetWorld()->GetID(), static_cast<BYTE>( nLayer ) ) == false )
	{
		return;
	}

	pItemElem->ResetFlag( FLItemElem::is_using );
	pUser->UpdateItem( static_cast<BYTE>( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );

	pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984F, 100.0F, 3336.884F ), REPLACE_FORCE, nDefaultLayer );
}

bool	FLChargeZoneTicket::CanStayChargeZoneWorld( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		return false;
	}

	CWorld* pWorld	= pUser->GetWorld();
	if( pWorld == NULL )
	{
		return false;
	}

	if( g_xSpecManager->IsChargeZoneWorld( pWorld->GetID() ) == false )
	{
		return false;
	}

	FLItemElem* pItemElem	= GetUsingChargeZoneTicket( pUser );
	if( pItemElem == NULL )
	{
		return false;
	}

	int nLayer	= pUser->GetLayer();
	if( nLayer < 0 )
	{
		nLayer	= -nLayer;
	}

	if( IsMatching( pItemElem->m_dwItemId, pWorld->GetID(), static_cast<BYTE>( nLayer ) ) == false )
	{
		pItemElem->ResetFlag( FLItemElem::is_using );
		pUser->UpdateItem( static_cast<BYTE>( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		return false;
	}

	return true;
}

bool	FLChargeZoneTicket::CanUseChargeZoneTicket( FLWSUser* pUser, DWORD dwItemObjID )
{
	if( IsValidStateUser( pUser ) == false )
	{
		return false;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwItemObjID );
	if( IsValidStateItem( pUser, pItemElem ) == false )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		return false;
	}

	if( HasSameActiveChargeZoneTicket( pUser, pItemElem ) != false )
	{
		pUser->AddDefinedText( TID_GAME_MUST_STOP_OTHER_TICKET );
		return false;
	}

	if( g_xSpecManager->IsChargeZoneWorld( pUser->GetWorld()->GetID() ) != false )
	{
		FLItemElem* pUsingTicket	= GetUsingChargeZoneTicket( pUser );
		if( IsValidStateItem( pUser, pUsingTicket ) != false )
		{
			if( pUsingTicket->m_dwObjId != dwItemObjID )
			{
				pUser->AddDefinedText( TID_GAME_MUST_STOP_OTHER_TICKET );
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool	FLChargeZoneTicket::IsValidStateUser( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->IsDie() != FALSE )
	{
		return false;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		return false;
	}

	if( pUser->GetWorld() == NULL )
	{
		return false;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() != FALSE )
	{
		pUser->AddDefinedText( TID_GAME_TRADELIMITUSING );
		return false;
	}

	if( !g_pItemUsing->canUseTeleportation( pUser ) ) {
		return false;
	}

	return true;
}

bool	FLChargeZoneTicket::IsValidStateItem( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
	if( pItemSpec == NULL )
	{
		return false;
	}

	if( pItemElem->IsExpiring() != FALSE || pItemElem->IsFlag( FLItemElem::expired ) != FALSE )
	{
		return false;
	}

	if( g_xSpecManager->IsChargeZoneTicket( pItemElem->m_dwItemId ) == false )
	{
		return false;
	}

	return true;
}

bool	FLChargeZoneTicket::HasSameActiveChargeZoneTicket( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return true;
	}

	if( IsValidStateItem( pUser, pItemElem ) == false )
	{
		return true;
	}

	PT_ITEM_SPEC pItemSpec		= pItemElem->GetProp();

	FLItemElem* pTmpItemElem		= NULL;
	PT_ITEM_SPEC pTmpItemSpec	= NULL;

	for( DWORD dwObjID = 0; dwObjID < pUser->m_Inventory.GetMax(); ++dwObjID )
	{
		pTmpItemElem	= pUser->m_Inventory.GetAtId( dwObjID );

		if( IsValidStateItem( pUser, pTmpItemElem ) == false )
		{
			continue;
		}

		pTmpItemSpec	= pTmpItemElem->GetProp();

		if( pTmpItemElem != pItemElem && pTmpItemElem->m_dwKeepTime > 0
			&& ( pTmpItemSpec->nItemGroup == pItemSpec->nUseLimitGroup || pTmpItemSpec->nItemGroup == pItemSpec->nItemGroup ) )
		{
			return true;
		}
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( pUser->m_Pocket.IsAvailable( nPocket, FALSE ) != FALSE )
		{
			for( DWORD dwObjID = 0; dwObjID < pUser->m_Pocket.m_kPocket[nPocket].GetMax(); ++dwObjID )
			{
				pTmpItemElem	= pUser->m_Pocket.m_kPocket[nPocket].GetAtId( dwObjID );
				
				if( IsValidStateItem( pUser, pTmpItemElem ) == false )
				{
					continue;
				}

				pTmpItemSpec	= pTmpItemElem->GetProp();

				if( pTmpItemElem != pItemElem && pTmpItemElem->m_dwKeepTime > 0
					&& ( pTmpItemSpec->nItemGroup == pItemSpec->nUseLimitGroup || pTmpItemSpec->nItemGroup == pItemSpec->nItemGroup ) )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool	FLChargeZoneTicket::IsMatching( DWORD dwItemID, DWORD dwWorldID, BYTE byChannel )
{
	SpecChargeZoneTicketVec vecTicket;
	if( g_xSpecManager->GetSpecChargeZoneTicket( dwItemID, &vecTicket ) == false )
	{
		return false;
	}

	for( SpecChargeZoneTicketVecCItr kCItr = vecTicket.begin(); kCItr != vecTicket.end(); ++kCItr )
	{
		PT_CHARGE_ZONE_WORLD pWorldSpec	= g_xSpecManager->GetSpecChargeZoneWorldByPointID( kCItr->dwPointID );
		if( pWorldSpec != NULL && pWorldSpec->dwWorldID == dwWorldID )
		{
			if( byChannel == 0 )
			{
				return true;
			}
			else
			{
				PT_WORLD_EXPAND_LAYER pLayerSpec	= g_xSpecManager->GetSpecWorldExpandLayer( dwWorldID );
				if( pLayerSpec != NULL && pLayerSpec->byExpandLayerCount >= byChannel )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool	FLChargeZoneTicket::GetStartingPosition( DWORD dwItemID, DWORD dwWorldID, D3DXVECTOR3 & kPosition )
{
	SpecChargeZoneTicketVec vecTicket;
	if( g_xSpecManager->GetSpecChargeZoneTicket( dwItemID, &vecTicket ) == false )
	{
		return false;
	}

	std::vector< D3DXVECTOR3 > vecPosition;
	vecPosition.clear();
	for( SpecChargeZoneTicketVecCItr kCItr = vecTicket.begin(); kCItr != vecTicket.end(); ++kCItr )
	{
		PT_CHARGE_ZONE_WORLD pSpec	= g_xSpecManager->GetSpecChargeZoneWorldByPointID( kCItr->dwPointID );
		if( pSpec!= NULL && pSpec->dwWorldID == dwWorldID )
		{
			vecPosition.push_back( pSpec->kStartingPos );
		}
	}

	if( vecPosition.empty() == false )
	{
		kPosition	= vecPosition[ xRandom( vecPosition.size() ) ];

		CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
		if( pWorld != NULL
			&& pWorld->VecInWorld( kPosition ) != FALSE
			&& pWorld->GetHeightAttribute( kPosition.x, kPosition.z ) == HATTR_NONE
			)
		{
			return true;
		}
	}

	return false;
}

bool	FLChargeZoneTicket::IsUsingChargeZoneTicket( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) != FALSE )
	{
		if( IsValidStateItem( pUser, pItemElem ) != false )
		{
			if( pItemElem->IsFlag( FLItemElem::is_using ) != FALSE )
			{
				return true;
			}
		}
	}

	return false;
}

FLItemElem*	FLChargeZoneTicket::GetUsingChargeZoneTicket( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return NULL;
	}

	FLItemElem* pItemElem	= NULL;
	for( DWORD dwObjID = 0; dwObjID < pUser->m_Inventory.GetMax(); ++dwObjID )
	{
		pItemElem	= pUser->m_Inventory.GetAtId( dwObjID );

		if( IsUsingChargeZoneTicket( pUser, pItemElem ) != false )
		{
			return pItemElem;
		}
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( pUser->m_Pocket.IsAvailable( nPocket, FALSE ) != FALSE )
		{
			for( DWORD dwObjID = 0; dwObjID < pUser->m_Pocket.m_kPocket[nPocket].GetMax(); ++dwObjID )
			{
				pItemElem	= pUser->m_Pocket.m_kPocket[nPocket].GetAtId( dwObjID );

				if( IsUsingChargeZoneTicket( pUser, pItemElem ) != false )
				{
					return pItemElem;
				}
			}
		}
	}

	return NULL;
}
