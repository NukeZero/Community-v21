
#include "stdafx.h"
#include "FLEventArenaHelper.h"

#include "../_Common/worldmng.h"
#include "../_AIInterface/FLFSM.h"

extern	CWorldMng	g_WorldMng;


CObj*	FLEventArenaHelper::CreateCtrl( DWORD dwCtrlID, DWORD dwWorldID, const D3DXVECTOR3& tPos, float fAngle, int nLayer )
{
	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldID );
	if( !pWorld )
	{
		return NULL;
	}

	CObj* pObj	= CreateObj( D3DDEVICE, OT_CTRL, dwCtrlID );
	if( pObj )
	{
		pObj->SetPos( tPos );
		pObj->SetAngle( fAngle );
		if( pWorld->ADDOBJ( pObj, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj );
		}
		return pObj;
	}

	return NULL;
}

CObj*	FLEventArenaHelper::CreateNPC( DWORD dwNPCID, OBJID dwTargetObjID, DWORD dwWorldID, const D3DXVECTOR3& tPos, int nLayer )
{
	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldID );
	if( !pWorld )
	{
		return NULL;
	}

	MoverProp*  pMoverProp = prj.GetMoverPropEx( dwNPCID );
	if( pMoverProp )
	{
		CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( pObj )
		{
			pObj->SetPos( tPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();

			CMover* pNPC = static_cast< CMover* >( pObj );
			if( pNPC )
			{
				if( pNPC->m_pFSM != NULL )
				{
					pNPC->m_pFSM->SetManualTarget( dwTargetObjID );
				}
			}

			if( pWorld->ADDOBJ( pObj, TRUE, nLayer ) == FALSE )
			{
				SAFE_DELETE( pObj );
			}
			return pObj;
		}
	}

	return NULL;
}

void	FLEventArenaHelper::AllRecovery( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		pUser->SetPointParam( DST_HP, pUser->GetMaxHitPoint() );
		pUser->SetPointParam( DST_MP, pUser->GetMaxManaPoint() );
		pUser->SetPointParam( DST_FP, pUser->GetMaxFatiguePoint() );

		pUser->m_Resurrection_Data.bUseing = FALSE;
		g_xWSUserManager->AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_BY_SKILL );

		pUser->m_pActMover->SendActMsg( OBJMSG_RESURRECTION );
	}
}

void	FLEventArenaHelper::ClearInven( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		// 장착 아이템과 팻인벤토리를 제외하고 모든 아이템 삭제
		DWORD dwSize = pUser->m_Inventory.GetMax();
		for( DWORD i = 0 ; i < dwSize; ++i )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) && !pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
			{
				const PT_ITEM_SPEC pSpec = pItemElem->GetProp();
				if( pSpec != NULL )
				{
					if( pSpec->GetInventoryType() == INVEN_TYPE_PET )
						continue;
				}

				pUser->UpdateItem( i, UI_NUM, 0 );
			}
		}

		// 포켓 삭제
		pUser->m_Pocket.RemoveAllItem();

		// 비행체 아이템 삭제
		FLItemElem* pFlyItem = pUser->GetEquipItem( PARTS_RIDE );
		if( pFlyItem )
		{
			pUser->UpdateItem( pFlyItem->m_dwObjId, UI_NUM, 0 );
		}
	}
}

void	FLEventArenaHelper::ClearBuff( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		pUser->ClearAbnormalState();
		pUser->RemoveAllBuff();
		pUser->ClearAllSMMode();
	}
}

void	FLEventArenaHelper::GiveItems( FLWSUser* pUser, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem )
{
	if( IsValidObj( pUser ) )
	{
		std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >::const_iterator pos = vecGiveItem.begin();
		std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >::const_iterator end = vecGiveItem.end();

		for( ; pos != end; ++pos )
		{
			const T_EVENT_ARENA_GIVE_ITEM_SPEC& tGiveItem = (*pos);
			FLItemElem itemElem;
			itemElem.m_dwItemId = tGiveItem.dwItemIndex;
			itemElem.m_nItemNum = tGiveItem.byCount;
			itemElem.SetSerialNumber();

			if( pUser->CreateItem( &itemElem ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "아이템 지급 실패: User: %s, ItemID: %u, Num: %n" ),
					pUser->GetName(), tGiveItem.dwItemIndex, tGiveItem.byCount );
			}
		}
	}
}

void	FLEventArenaHelper::Move( FLWSUser* pUser, const T_WORLD_POSITION* pPosition, int nLayer )
{
	if( pPosition != NULL && IsValidObj( pUser ) )
	{
		pUser->m_pActMover->SendActMsg( OBJMSG_STOP );
		if( pUser->REPLACE( g_uIdofMulti, pPosition->dwWorldID, pPosition->tPos, REPLACE_NORMAL, nLayer ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Replace 실패 World(%u) (%f, %f, %f) Layer: %d" ),
				pPosition->dwWorldID, pPosition->tPos.x, pPosition->tPos.y, pPosition->tPos.z, nLayer );
		}

		if( nLayer != nDefaultLayer )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Event Arena Move PC: %s, Layer: %d" ), pUser->GetName(), nLayer );
		}
	}
}

void	FLEventArenaHelper::Disguise( FLWSUser* pUser, DWORD dwTransformMoverID )
{
	if( IsValidObj( pUser ) && dwTransformMoverID != 0 )
	{
		pUser->Disguise( NULL, dwTransformMoverID );
		g_xWSUserManager->AddDisguise( pUser, dwTransformMoverID );
	}
}

void	FLEventArenaHelper::NoDisguise( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		pUser->NoDisguise( NULL );
		g_xWSUserManager->AddNoDisguise( pUser );
	}
}

void	FLEventArenaHelper::BroadCast( const std::vector< DWORD >& vecWorldIDs, const FLPacket* pPacket )
{
	if( pPacket != NULL )
	{
		std::vector< DWORD >::const_iterator pos = vecWorldIDs.begin();
		std::vector< DWORD >::const_iterator end = vecWorldIDs.end();

		for( ; pos != end; ++pos )
		{
			CWorld* pWorld = g_WorldMng.GetWorld( *pos );
			if( pWorld != NULL )
			{
				BroadCast( pWorld, pPacket );
			}
		}
	}
}

void	FLEventArenaHelper::BroadCastChat( const std::vector< DWORD >& vecWorldIDs, int nTextID, const TCHAR* pszChat, ... )
{
	TCHAR szBuffer[1024] = _T( "" );

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, pszChat );
	int nBuf = FLVSPrintf( szBuffer, _countof( szBuffer ), pszChat, args );
	va_end( args );

	if( nBuf < 0 )
		return;

	std::vector< DWORD >::const_iterator pos = vecWorldIDs.begin();
	std::vector< DWORD >::const_iterator end = vecWorldIDs.end();

	for( ; pos != end; ++pos )
	{
		CWorld* pWorld = g_WorldMng.GetWorld( *pos );
		if( pWorld != NULL )
		{
			BroadCastChat( pWorld, nTextID, szBuffer );
		}
	}
}

void	FLEventArenaHelper::BroadCastNotice( const std::vector< DWORD >& vecWorldIDs, int nTextID, const TCHAR* pszNotice, ... )
{
	TCHAR szBuffer[1024] = _T( "" );

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, pszNotice );
	int nBuf = FLVSPrintf( szBuffer, _countof( szBuffer ), pszNotice, args );
	va_end( args );

	if( nBuf < 0 )
		return;

	std::vector< DWORD >::const_iterator pos = vecWorldIDs.begin();
	std::vector< DWORD >::const_iterator end = vecWorldIDs.end();

	for( ; pos != end; ++pos )
	{
		CWorld* pWorld = g_WorldMng.GetWorld( *pos );
		if( pWorld != NULL )
		{
			BroadCastNotice( pWorld, nTextID, szBuffer );
		}
	}
}

void	FLEventArenaHelper::BroadCastChat( CWorld* pWorld, int nTextID, const TCHAR* pszBuffer )
{
	if( pWorld != NULL )
	{
		for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
		{
			CCtrl* pCtrl = pWorld->m_apObject[i];
			if( pCtrl && !pCtrl->IsDelete() && pCtrl->GetType() == OT_MOVER )
			{
				CMover* pMover = static_cast< CMover* >( pCtrl );
				if( pMover && pMover->IsPlayer() )
				{
					FLWSUser* pUser = static_cast< FLWSUser* >( pMover );
					if( pUser )
					{
						pUser->AddChatText( nTextID, pszBuffer );
					}
				}
			}
		}
	}
}

void	FLEventArenaHelper::BroadCastNotice( CWorld* pWorld, int nTextID, const TCHAR* pszBuffer )
{
	if( pWorld != NULL )
	{
		for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
		{
			CCtrl* pCtrl = pWorld->m_apObject[i];
			if( pCtrl && !pCtrl->IsDelete() && pCtrl->GetType() == OT_MOVER )
			{
				CMover* pMover = static_cast< CMover* >( pCtrl );
				if( pMover && pMover->IsPlayer() )
				{
					FLWSUser* pUser = static_cast< FLWSUser* >( pMover );
					if( pUser )
					{
						pUser->AddDefinedCaption( TRUE, nTextID, pszBuffer );
					}
				}
			}
		}
	}
}

void	FLEventArenaHelper::BroadCast( CWorld* pWorld, const FLPacket* pPacket )
{
	if( pWorld != NULL && pPacket != NULL )
	{
		for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
		{
			CCtrl* pCtrl = pWorld->m_apObject[i];
			if( pCtrl && !pCtrl->IsDelete() && pCtrl->GetType() == OT_MOVER )
			{
				CMover* pMover = static_cast< CMover* >( pCtrl );
				if( pMover && pMover->IsPlayer() )
				{
					FLWSUser* pUser = static_cast< FLWSUser* >( pMover );
					if( pUser )
					{
						pUser->AddPacket( pPacket );
					}
				}
			}
		}
	}
}
