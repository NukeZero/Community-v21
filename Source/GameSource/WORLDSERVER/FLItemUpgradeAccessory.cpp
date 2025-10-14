
#include "StdAfx.h"
#include "FLItemUpgradeAccessory.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeAccessory::FLItemUpgradeAccessory()
{
}

FLItemUpgradeAccessory::~FLItemUpgradeAccessory()
{
}

FLItemUpgradeAccessory*	FLItemUpgradeAccessory::GetInstance()
{
	static FLItemUpgradeAccessory	sItemUpgradeAccessory;

	return & sItemUpgradeAccessory;
}

void	FLItemUpgradeAccessory::Clear()
{
	m_vecGeneralEnchantData.clear();
}

bool	FLItemUpgradeAccessory::LoadScript( const TCHAR* pszFileName )
{
	CLuaBase kLua;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		RELEASE_BREAK;
		return false;
	}

	// Load General Enchant Data
	if( LoadGeneralEnchantData( kLua, m_vecGeneralEnchantData ) == false )
	{
		return false;
	}

	return true;
}

DWORD	FLItemUpgradeAccessory::OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemGeneralEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemGeneralEnchant( pUser, pMsg );
	}

	return dwResult;
}

size_t	FLItemUpgradeAccessory::GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec		= g_xSpecManager->GetSpecItem( dwItemID );
	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pItemSpec, m_vecGeneralEnchantData );
	if( pEnchantData == NULL )
	{
		return 0;
	}

	const size_t nSize					= pEnchantData->mapUpgradeProb.size();

	return nSize;
}

bool	FLItemUpgradeAccessory::CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_GENERAL ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE || IsUsableItem( pMaterialItem ) == FALSE )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	const T_ITEM_SPEC* pMaterialProp	= pMaterialItem->GetProp();
	if( pMainProp == NULL || pMaterialProp == NULL )
	{
		return false;
	}

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecGeneralEnchantData );
	if( pEnchantData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pEnchantData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pEnchantData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pEnchantData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	const int nAbilityOption	= pMainItem->GetAbilityOption();
	if( nAbilityOption < 0 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	UpgradeProbDataMap::const_iterator kCItr = pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );
	if( kCItr == pEnchantData->mapUpgradeProb.end() )
	{
		pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR13 );
		return false;
	}

	if( pMsg->bSafetyUpgrade == true )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR08 );
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR08 );
			return false;
		}

		if( pProtectionProp->dwID != ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 ) 
			&& pProtectionProp->dwID != ITEM_INDEX( 25233, II_SYS_SYS_SCR_SMELPROT6 ))
		{
			pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR08 );
			return false;
		}

		if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
		{
			// 악세사리 안전제련에는 사용 가능한 확률 증가 아이템이 없음.
			return false;
		}
	}

	return true;
}

DWORD	FLItemUpgradeAccessory::DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const int nAbilityOption		= pMainItem->GetAbilityOption();

	TCHAR szConText[64]				= { 0, };
	bool bDestroyProtect			= false;
	//////////////////////////////////////////////////////////////////////////
	// 안전 제련 여부 확인
	if( pMsg->bSafetyUpgrade == true )
	{
		FLStrcpy( szConText, _countof( szConText ), "UPGRADEITEM_SMELTSAFETY" );

		bDestroyProtect				= true;

		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		g_DPSrvr.PutItemLog( pUser, "N", szConText, pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}
	else
	{
		FLStrcpy( szConText, _countof( szConText ), "UPGRADEITEM" );

		if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 ) ) )
		{
			bDestroyProtect		= true;
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 ) );
		}

		if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 25233, II_SYS_SYS_SCR_SMELPROT6 ) ) )
		{
			bDestroyProtect		= true;
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 25233, II_SYS_SYS_SCR_SMELPROT6 ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	const T_ENCHANT_DATA* pEnchantData			= GetEnchantData( pMainItem->GetProp(), m_vecGeneralEnchantData );

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	const DWORD dwRandomProb					= ::xRandom( MAX_UPGRADE_PROB );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( kEnchantProb.dwSuccessProb > dwRandomProb )
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_AO, ( nAbilityOption + 1 ) );

		g_DPSrvr.PutItemLog( pUser, "H", szConText, pMainItem );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	// fail
	else
	{
		// exist option
		if( nAbilityOption > MAX_SAFE_LEVEL_GENERAL_ENCHANT )	// TODO
		{
			if( bDestroyProtect == true )
			{
				g_DPSrvr.PutItemLog( pUser, "F", szConText, pMainItem );
			}
			else
			{
				g_DPSrvr.PutItemLog( pUser, "L", szConText, pMainItem );
				pUser->RemoveItem( pMainItem->m_dwObjId, pMainItem->m_nItemNum );
			}
		}
		// not exist option
		else
		{
			g_DPSrvr.PutItemLog( pUser, "F", szConText, pMainItem );
		}

		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		
		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "N", szConText, pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}
