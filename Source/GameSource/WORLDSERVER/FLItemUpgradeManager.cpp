
#include "StdAfx.h"
#include "FLItemUpgradeManager.h"
#include "../_CommonDefine/Packet/FLPacketItemOption.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"

//////////////////////////////////////////////////////////////////////////
#include "FLItemUpgradeAccessory.h"
#include "FLItemUpgradeBaruna.h"
#include "FLItemUpgradeCostume.h"
#include "FLItemUpgradeEatPet.h"
#include "FLItemUpgradeGeneral.h"
#include "FLItemUpgradeSystemPet.h"
#include "FLItemUpgradeMaterial.h"
//////////////////////////////////////////////////////////////////////////


FLItemUpgradeManager::FLItemUpgradeManager()
{
}

FLItemUpgradeManager::~FLItemUpgradeManager()
{
}

FLItemUpgradeManager* FLItemUpgradeManager::GetInstance()
{
	static FLItemUpgradeManager xItemUpgradeManager;

	return & xItemUpgradeManager;
}

void	FLItemUpgradeManager::Load()
{
	if( FLItemUpgradeAccessory::GetInstance()->LoadScript( "ItemUpgradeAccessory.lua" ) == true )
	{
		// set
	}
	
	if( FLItemUpgradeGeneral::GetInstance()->LoadScript( "ItemUpgradeGeneral.lua" ) == true )
	{
		// set
	}

	if( FLItemUpgradeBaruna::GetInstance()->LoadScript( "ItemUpgradeBaruna.lua" ) == true )
	{
		// set
	}

	if( FLItemUpgradeCostume::GetInstance()->LoadScript( "ItemUpgradeCostume.lua" ) == true )
	{
		// set
	}

	if( FLItemUpgradeSystemPet::GetInstance()->LoadScript( "ItemUpgradeSystemPet.lua" ) == true )
	{
		// set
	}

	if( FLItemUpgradeEatPet::GetInstance()->LoadScript( "ItemUpgradeEatPet.lua" ) == true )
	{
		// set
	}
}

void	FLItemUpgradeManager::OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeGeneralEnchantResultAck kPacket;
	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemGeneralEnchant( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}
	
	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeAttributeEnchantResultAck kPacket;
	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemAttributeEnchant( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradePiercingSizeIncreaseResultAck kPacket;
	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemPiercingSizeIncrease( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradePiercingItemInsertResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemPiercingItemInsert( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradePiercingItemRemoveResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemPiercingItemRemove( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeRandomOptionGenerateResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}

		FLSnapshotItemUpgradeRandomOptionAck toClient;
		toClient.dwItemObjID		= pMainItem->m_dwObjId;
		toClient.dwSerialNumber		= pMainItem->GetSerialNumber();
		pMainItem->GetRandomOption( toClient.kRandomOption );
		
		pUser->AddPacket( &toClient );
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck kPacket;
	kPacket.dwMainItemObjID		= pMsg->dwMainItemObjID;
	kPacket.byIndex				= pMsg->byIndex;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemRandomOptionGenerateRetry( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}

		if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS )
		{
			pItemUpgradeBase->GetRandomOptionDummy( *pMainItem, pMsg->byIndex, kPacket.vecDummyRandomOption, kPacket.byDummyIndex );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeRandomOptionInitializeResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}

		FLSnapshotItemUpgradeRandomOptionAck toClient;
		toClient.dwItemObjID		= pMainItem->m_dwObjId;
		toClient.dwSerialNumber		= pMainItem->GetSerialNumber();
		pMainItem->GetRandomOption( toClient.kRandomOption );
		pUser->AddPacket( &toClient );
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeEquipLevelDecreaseResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemEquipLevelDecrease( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeEquipLevelInitializeResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemEquipLevelInitialize( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const
{
	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeCombineResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemCombine( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const
{
	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeCombineInitializeResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemCombineInitialize( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	if( _GetContentState( CT_LOOKS_CHANGE ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeLooksChangeResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemLooksChange( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	if( _GetContentState( CT_LOOKS_CHANGE ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeLooksInitializeResultAck kPacket;
//	kPacket.bSafetyUpgrade		= pMsg->bSafetyUpgrade;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemLooksInitialize( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

DWORD	FLItemUpgradeManager::GetAttributeDamageFactor( const FLItemElem & kItemElem ) const
{
	const T_ITEM_SPEC* pItemSpec = g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase == NULL )
	{
		return 0;
	}

	return pItemUpgradeBase->GetAttributeDamageFactor( kItemElem );
}

DWORD	FLItemUpgradeManager::GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const
{
	const T_ITEM_SPEC* pItemSpec = g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase == NULL )
	{
		return 0;
	}

	return pItemUpgradeBase->GetAttributeDefenseFactor( kItemElem );
}

DWORD	FLItemUpgradeManager::GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const
{
	const T_ITEM_SPEC* pItemSpec = g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase == NULL )
	{
		return 0;
	}

	return pItemUpgradeBase->GetAttributeAddAtkDmgFactor( kItemElem );
}

bool	FLItemUpgradeManager::GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase == NULL )
	{
		return false;
	}

	return pItemUpgradeBase->GetRandomOptionDummy( kItemElem, byIndex, vecDummyRandomOption, byDummyIndex );
}

bool	FLItemUpgradeManager::GetCombineInitializeData( const FLItemElem & kItemElem, T_COMBINE_DATA & kCombineData ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase == NULL )
	{
		return false;
	}

	return pItemUpgradeBase->GetCombineInitializeData( kItemElem, kCombineData );
}

size_t	FLItemUpgradeManager::GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( dwItemID );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase != NULL )
	{
		return pItemUpgradeBase->GetMaxSizeGeneralEnchant( dwItemID );
	}

	return 0;
}

size_t	FLItemUpgradeManager::GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( dwItemID );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase != NULL )
	{
		return pItemUpgradeBase->GetMaxSizeAttributeEnchant( dwItemID );
	}

	return 0;
}

size_t	FLItemUpgradeManager::GetMaxSizePiercing( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( dwItemID );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase != NULL )
	{
		return pItemUpgradeBase->GetMaxSizePiercing( dwItemID );
	}

	return 0;
}

size_t	FLItemUpgradeManager::GetMaxSizeRandomOption( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec				= g_xSpecManager->GetSpecItem( dwItemID );

	const FLItemUpgradeBase* pItemUpgradeBase	= GetItemUpgradeType( pItemSpec );

	if( pItemUpgradeBase != NULL )
	{
		return pItemUpgradeBase->GetMaxSizeRandomOption( dwItemID );
	}

	return 0;
}

const FLItemUpgradeBase* FLItemUpgradeManager::GetItemUpgradeType( const T_ITEM_SPEC* pItemProp ) const
{
	if( pItemProp != NULL )
	{
		switch( pItemProp->dwMainCategory )
		{
		case TYPE1_WEAPON:
		case TYPE1_ARMOR:
			{
				if( pItemProp->dwItemGrade == ITEM_GRADE_NORMAL )
				{
					return FLItemUpgradeGeneral::GetInstance();
				}
				else if( pItemProp->dwItemGrade == ITEM_GRADE_UNIQUE )
				{
					return FLItemUpgradeGeneral::GetInstance();
				}
				if( pItemProp->dwItemGrade == ITEM_GRADE_ULTIMATE )
				{
					return FLItemUpgradeGeneral::GetInstance();
				}
				if( pItemProp->dwItemGrade == ITEM_GRADE_BARUNA )
				{
					return FLItemUpgradeBaruna::GetInstance();
				}
			}
			break;
		case TYPE1_ACCESSORY:
			{
				return FLItemUpgradeAccessory::GetInstance();
			}
			break;
		case TYPE1_CONSUME:
			{
				return FLItemUpgradeGeneral::GetInstance();
			}
			break;

		case TYPE1_COSTUME:
			{
				return FLItemUpgradeCostume::GetInstance();
			}
			break;

		case TYPE1_PET:
			{
				if( pItemProp->dwSubCategory == TYPE2_PET_REAR )
				{
					return FLItemUpgradeSystemPet::GetInstance();
				}
				else if( pItemProp->dwSubCategory == TYPE2_PET_PICKUP || pItemProp->dwSubCategory == TYPE2_PET_BUFF )
				{
					return FLItemUpgradeEatPet::GetInstance();
				}
			}
			break;
#ifdef ENCHANT_ABSOLUTE_MIX
		case TYPE1_MATERIAL:
			{
				return FLItemUpgradeMaterial::GetInstance();
			}
			break;
#endif
		default:
			break;
		}
	}

	return NULL;
}


void	FLItemUpgradeManager::OnDoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const
{

	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradCardResultAck kPacket;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemCard( pUser, pMsg );
	if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
	{
		const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}

	kPacket.dwResult			= dwResult;

	pUser->AddPacket( &kPacket );
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
void	FLItemUpgradeManager::OnDoUpgradeItemSetGem( FLWSUser* pUser, const FLPacketItemUpgradeSetGemReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeSetGemResultAck kPacket;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemSetGem( pUser, pMsg );
	
	DWORD dwResult2 = ( dwResult == FLItemUpgradeCostume::COSTUME_GEM_SUCCESS ) ? FSC_ITEMUPGRADE_UPGRADE_SUCCESS : FSC_ITEMUPGRADE_UPGRADE_FAIL	;
	if( dwResult2 == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult2 == FSC_ITEMUPGRADE_UPGRADE_FAIL )
	{
		const DWORD dwSoundID	= ( dwResult2 == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
		const DWORD dwSfxObjID	= ( dwResult2 == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

		pUser->AddPlaySound( dwSoundID );
		if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
		{
			g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		}
	}
	
	kPacket.dwResult			= dwResult2;

	pUser->AddPacket( &kPacket );
}

void	FLItemUpgradeManager::OnDoUpgradeItemRemoveGem( FLWSUser* pUser, const FLPacketItemUpgradeRemoveGemReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeRemoveGemResultAck kPacket;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
		pUser->AddPacket( &kPacket );
		return;
	}

	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemRemoveGem( pUser, pMsg );
	
	DWORD dwResult2 = ( dwResult == FLItemUpgradeCostume::COSTUME_GEM_SUCCESS ) ? FSC_ITEMUPGRADE_UPGRADE_SUCCESS : FSC_ITEMUPGRADE_UPGRADE_FAIL	;
	if( dwResult2 == FSC_ITEMUPGRADE_UPGRADE_SUCCESS )
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_SUCCESS, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);	
	}
	else
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_FAILED, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1715, XI_INT_FAIL ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);	
	}
	
	kPacket.dwResult			= dwResult2;

	pUser->AddPacket( &kPacket );
}
#endif

#ifdef COSTUME_UPGRADE_MIX
void	FLItemUpgradeManager::OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	FLSnapshotItemUpgradeMixResultAck kPacket;

	FLItemElem* pMainItem;

	for( int i = 0; i < 5; i++ )
	{
		if ( pMsg->dwMaterialItemObjID[i] == NULL_ID )
			continue;
		pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[i] );
		if( IsUsableItem( pMainItem ) == FALSE )
		{
			kPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
			pUser->AddPacket( &kPacket );
			return;
		}
	}

	//pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[0] );
	const FLItemUpgradeBase* pItemUpgradeBase = GetItemUpgradeType( pMainItem->GetProp() );
	if( pItemUpgradeBase == NULL )
	{
		kPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
		pUser->AddPacket( &kPacket );
		return;
	}

	DWORD	dwItemIndex = NULL_ID;
	const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemMix( pUser, pMsg, dwItemIndex );
	
	//if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS )
	//{
	//	//성공 메세지 출력
	//	pUser->AddDefinedText( TID_GAME_REMOVEGEM_SUCCESS, "" );
	//	pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
	//	if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
	//		g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);	
	//}
	//else
	//{
	//	// 실패 메세지 출력
	//	pUser->AddDefinedText( TID_GAME_REMOVEGEM_FAILED, "" );
	//	pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
	//	if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
	//		g_xWSUserManager->AddCreateSfxObj((CMover *)pUser, XI_INDEX( 1715, XI_INT_FAIL ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);	
	//}
	
	kPacket.dwResult					= dwResult;
	kPacket.dwResultItemObjID			= dwItemIndex;

	pUser->AddPacket( &kPacket );
}
#endif
