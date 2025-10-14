
#include "StdAfx.h"
#include "FLItemUpgradeGeneral.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeGeneral::FLItemUpgradeGeneral()
{
}

FLItemUpgradeGeneral::~FLItemUpgradeGeneral()
{
}

FLItemUpgradeGeneral*	FLItemUpgradeGeneral::GetInstance()
{
	static FLItemUpgradeGeneral	sItemUpgradeGeneral;

	return & sItemUpgradeGeneral;
}

void	FLItemUpgradeGeneral::Clear()
{
	m_vecGeneralEnchantData.clear();
	m_vecAttributeEnchantData.clear();
	m_vecPiercingData.clear();

	for( RandomOptionDataVec::iterator pos = m_vecRandomOptionData.begin(); pos != m_vecRandomOptionData.end(); ++pos )
	{
		pos->mapSizeProb.clear();
		pos->vecDstProb.clear();
		pos->vecRetryDstProb.clear();
		pos->vecDummyDstProb.clear();
	}
	m_vecRandomOptionData.clear();

	for( DstParameterDataMap::iterator pos = m_mapDstParameterData.begin(); pos != m_mapDstParameterData.end(); ++pos )
	{
		pos->second.vecAdjProb.clear();
		pos->second.vecRetryAdjProb.clear();
		pos->second.vecDummyAdjProb.clear();
	}
	m_mapDstParameterData.clear();

	m_vecCombineData.clear();
#ifdef CARD_UPGRADE_SYSTEM
	m_vecCardCombineData.clear();
#endif	// CARD_UPGRADE_SYSTEM
}

bool	FLItemUpgradeGeneral::LoadScript( const TCHAR* pszFileName )
{
	CLuaBase kLua;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	// Load General Enchant Data
	if( LoadGeneralEnchantData( kLua, m_vecGeneralEnchantData ) == false )
	{
		return false;
	}

	// Load Attribute Enchant Data
	if( LoadAttributeEnchantData( kLua, m_vecAttributeEnchantData ) == false )
	{
		return false;
	}

	// Load Piercing Data
	if( LoadPiercingData( kLua, m_vecPiercingData ) == false )
	{
		return false;
	}

	// Load RandomOption Data
	if( LoadRandomOptionExtensionData( kLua, m_mapDstParameterData, m_vecRandomOptionData ) == false )
	{
		return false;
	}

	// Load Combine Data
	if( LoadCombineData( kLua, m_vecCombineData ) == false )
	{
		return false;
	}
#ifdef CARD_UPGRADE_SYSTEM
	// Load CardCombine Data
	if( LoadCardCombineData( kLua, m_vecCardCombineData ) == false )
	{
		return false;
	}
#endif	// CARD_UPGRADE_SYSTEM
	return true;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemGeneralEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemGeneralEnchant( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeEnchant( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeChange( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeChange( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeRemove( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeRemove( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemPiercingSizeIncrease( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemPiercingSizeIncrease( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemPiercingItemInsert( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemPiercingItemInsert( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemPiercingItemRemove( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemPiercingItemRemove( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerate( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerateRetry( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerateRetry( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemEquipLevelDecrease( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemEquipLevelDecrease( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemEquipLevelInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemEquipLevelInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemCombine( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemCombine( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemCombineInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemCombineInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksChange( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksChange( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::GetAttributeDamageFactor( const FLItemElem & kItemElem ) const
{
	const BYTE byResist						= kItemElem.GetItemResist();
	if( byResist <= SAI79::NO_PROP || byResist >= SAI79::END_PROP )
	{
		return 0;
	}

	const int nResistAbilityOption			= kItemElem.GetResistAbilityOption();
	if( nResistAbilityOption <= 0 )
	{
		return 0;
	}

	const T_ITEM_SPEC* pItemSpec			= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	const T_ENCHANT_DATA* pEnchantData		= GetEnchantData( pItemSpec, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		return 0;
	}

	const int nMaxResistAbilityOption		= static_cast<int>( pEnchantData->mapUpgradeProb.size() );

	const int nFindOptionLevel				= ( nResistAbilityOption <= nMaxResistAbilityOption ) ? nResistAbilityOption : nMaxResistAbilityOption;

	AbilityPropDataMap::const_iterator itr	= pEnchantData->mapAbilityPropData.find( nFindOptionLevel );
	if( itr == pEnchantData->mapAbilityPropData.end() )
	{
		return 0;
	}

	return itr->second.dwAddDamageRate;
}

DWORD	FLItemUpgradeGeneral::GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const
{
	const BYTE byResist						= kItemElem.GetItemResist();
	if( byResist <= SAI79::NO_PROP || byResist >= SAI79::END_PROP )
	{
		return 0;
	}

	const int nResistAbilityOption			= kItemElem.GetResistAbilityOption();
	if( nResistAbilityOption <= 0 )
	{
		return 0;
	}

	const T_ITEM_SPEC* pItemSpec			= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	const T_ENCHANT_DATA* pEnchantData		= GetEnchantData( pItemSpec, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		return 0;
	}

	const int nMaxResistAbilityOption		= static_cast<int>( pEnchantData->mapUpgradeProb.size() );

	const int nFindOptionLevel				= ( nResistAbilityOption <= nMaxResistAbilityOption ) ? nResistAbilityOption : nMaxResistAbilityOption;

	AbilityPropDataMap::const_iterator itr	= pEnchantData->mapAbilityPropData.find( nFindOptionLevel );
	if( itr == pEnchantData->mapAbilityPropData.end() )
	{
		return 0;
	}

	return itr->second.dwAddDefenseRate;
}

DWORD	FLItemUpgradeGeneral::GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const
{
	const BYTE byResist						= kItemElem.GetItemResist();
	if( byResist <= SAI79::NO_PROP || byResist >= SAI79::END_PROP )
	{
		return 0;
	}

	const int nResistAbilityOption			= kItemElem.GetResistAbilityOption();
	if( nResistAbilityOption <= 0 )
	{
		return 0;
	}

	const T_ITEM_SPEC* pItemSpec			= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	const T_ENCHANT_DATA* pEnchantData		= GetEnchantData( pItemSpec, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		return 0;
	}

	const int nMaxResistAbilityOption		= static_cast<int>( pEnchantData->mapUpgradeProb.size() );

	const int nFindOptionLevel				= ( nResistAbilityOption <= nMaxResistAbilityOption ) ? nResistAbilityOption : nMaxResistAbilityOption;

	AbilityPropDataMap::const_iterator itr	= pEnchantData->mapAbilityPropData.find( nFindOptionLevel );
	if( itr == pEnchantData->mapAbilityPropData.end() )
	{
		return 0;
	}

	return itr->second.dwAddAtkDmgRate;
}

bool	FLItemUpgradeGeneral::GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const
{
	if( SetDummyRandomOptionExtension( vecDummyRandomOption, kItemElem, m_vecRandomOptionData, m_mapDstParameterData ) == false )
	{
		return false;
	}

	if( vecDummyRandomOption.empty() == true )
	{
		return false;
	}

	const size_t nDummySize	= vecDummyRandomOption.size();

	const size_t nPos		= ::xRandom( nDummySize );

	const WORD wDstID		= kItemElem.GetRandomOptionExtensionDstID( byIndex );
	const short shAdjValue	= kItemElem.GetRandomOptionExtensionAdjValue( byIndex );

	if( wDstID == 0 || shAdjValue == 0 )
	{
		return false;
	}

	byDummyIndex								= static_cast<BYTE>( nPos );
	vecDummyRandomOption.at( nPos ).wDstID		= wDstID;
	vecDummyRandomOption.at( nPos ).shAdjValue	= shAdjValue;

	return true;
}

bool	FLItemUpgradeGeneral::GetCombineInitializeData( const FLItemElem & kItemElem, T_COMBINE_DATA & kCombineData ) const
{
	const T_ITEM_SPEC* pItemSpec		= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	const T_COMBINE_DATA* pCombindData	= GetCombineData( pItemSpec, m_vecCombineData );
	if( pCombindData == NULL )
	{
		return false;
	}

	kCombineData = *pCombindData;

	return true;
}
// CARD_UPGRADE_SYSTEM
DWORD	FLItemUpgradeGeneral::OnDoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemCard( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemCard( pUser, pMsg );
	}

	return dwResult;
}


size_t	FLItemUpgradeGeneral::GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const
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

size_t	FLItemUpgradeGeneral::GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec		= g_xSpecManager->GetSpecItem( dwItemID );
	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pItemSpec, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		return 0;
	}

	const size_t nSize					= pEnchantData->mapUpgradeProb.size();

	return nSize;
}

size_t	FLItemUpgradeGeneral::GetMaxSizePiercing( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec			= g_xSpecManager->GetSpecItem( dwItemID );
	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pItemSpec, m_vecPiercingData );
	if( pPiercingData == NULL )
	{
		return 0;
	}

	const size_t nSize						= pPiercingData->mapSizeProb.size();

	return nSize;
}

size_t	FLItemUpgradeGeneral::GetMaxSizeRandomOption( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec					= g_xSpecManager->GetSpecItem( dwItemID );
	const T_RANDOMOPTION_DATA* pRandomOptionData	= GetRandomOptionData( pItemSpec, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return 0;
	}

	const size_t nSize								= pRandomOptionData->mapSizeProb.size();

	return nSize;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
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

		if( pProtectionProp->dwID != ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) )
		{
			pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR08 );
			return false;
		}

		if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
		{
			FLItemElem* pProbIncreaseItem	= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );
			if( IsUsableItem( pProbIncreaseItem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR06 );
				return false;
			}

			const T_ITEM_SPEC* pProbIncreaseProp	= pProbIncreaseItem->GetProp();
			if( pProbIncreaseProp == NULL )
			{
				pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR06 );
				return false;
			}

			if( pProbIncreaseProp->dwID != ITEM_INDEX( 10468, II_SYS_SYS_SCR_SMELTING ) )
			{
				pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR06 );
				return false;
			}
		}
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_ATTRIBUTE ) == false )
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

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecAttributeEnchantData );
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

	if( pMainItem->m_byItemResist != SAI79::NO_PROP )
	{
		if( pMainItem->m_byItemResist != pMaterialProp->eItemType )
		{
			pUser->AddDefinedText( TID_UPGRADE_ERROR_TWOELEMENT );								
			return false;
		}
	}

	const int nResistAbilityOption	= pMainItem->GetResistAbilityOption();
	if( nResistAbilityOption < 0 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	UpgradeProbDataMap::const_iterator kCItr = pEnchantData->mapUpgradeProb.find( nResistAbilityOption + 1 );
	if( kCItr == pEnchantData->mapUpgradeProb.end() )
	{
		pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR13 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_ATTRIBUTE ) == false )
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

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pMaterialProp->dwID != ITEM_INDEX( 20037, II_SYS_SYS_SCR_SOKCHANG ) )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMsg->byAttribute <= SAI79::NO_PROP || pMsg->byAttribute >= SAI79::END_PROP )
	{
		pUser->AddDefinedText( TID_GAME_NOTELEMENT );
		return false;
	}

	if( pMainItem->m_byItemResist <= SAI79::NO_PROP || pMainItem->m_byItemResist >= SAI79::END_PROP || pMainItem->GetResistAbilityOption() <= 0 )
	{
		pUser->AddDefinedText( TID_GAME_NOTELEMENT );
		return false;
	}

	if( pMainItem->m_byItemResist == pMsg->byAttribute )
	{
		pUser->AddDefinedText( TID_GAME_NOCHANGE_SAME_ATTRIBUTE );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_ATTRIBUTE ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pMainItem->m_byItemResist <= SAI79::NO_PROP || pMainItem->m_byItemResist >= SAI79::END_PROP || pMainItem->GetResistAbilityOption() <= 0 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pUser->CheckUserGold( pEnchantData->nAttributeRemovePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_PIERCING ) == false )
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

	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainProp, m_vecPiercingData );
	if( pPiercingData == NULL )
	{
		pUser->AddDefinedText( TID_PIERCING_POSSIBLE_ITEM );
		return false;
	}

	const size_t nPiercingSize	= pMainItem->GetGeneralPiercingSize();

	UpgradeProbDataMap::const_iterator kCItr = pPiercingData->mapSizeProb.find( nPiercingSize + 1 );
	if( kCItr == pPiercingData->mapSizeProb.end() )
	{
		pUser->AddDefinedText( TID_PIERCING_POSSIBLE_ITEM );
		return false;
	}

	if( pPiercingData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pPiercingData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pPiercingData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		return false;
	}

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem		= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
			return false;
		}

		if( pProtectionItem->m_dwItemId != ITEM_INDEX( 10465, II_SYS_SYS_SCR_PIEPROT ) && pProtectionItem->m_dwItemId != ITEM_INDEX( 25234, II_SYS_SYS_SCR_PIEPROT2 ))
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
			return false;
		}
	}

	if( pUser->CheckUserGold( pPiercingData->nSizeIncreasePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_PIERCING ) == false )
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

	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainProp, m_vecPiercingData );
	if( pPiercingData == NULL )
	{
		pUser->AddDefinedText( TID_PIERCING_POSSIBLE_ITEM );
		return false;
	}

	if( pPiercingData->kPiercedKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pPiercingData->kPiercedKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pPiercingData->kPiercedKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL );
		return false;
	}

	if( pMainItem->IsSetGeneralPiercingOption() == false )
	{
		pUser->AddDefinedText( TID_PIERCING_ERROR_NOPIERCING );
		return false;
	}

	if( pMainItem->IsExistGeneralPiercedItemID( 0 ) == false )
	{
		pUser->AddDefinedText( TID_PIERCING_ERROR_NOPIERCING );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_PIERCING ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();

	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainProp, m_vecPiercingData );
	if( pPiercingData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetGeneralPiercingOption() == false || pMainItem->GetGeneralPiercingItemID( 0 ) == 0 )
	{
		pUser->AddDefinedText( TID_GAME_REMOVE_PIERCING_ERROR );
		return false;
	}

	if( pUser->CheckUserGold( pPiercingData->nItemRemovePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
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

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pRandomOptionData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pRandomOptionData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pRandomOptionData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == true
		&& pMainItem->IsSetSafeGuardRandomOptionExtensionFlag() == false )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
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

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT02 );	// 부여된 랜덤 옵션이 없다.
		return false;
	}

	const size_t nRandomOptionSize	= pMainItem->GetRandomOptionExtensionSize();
	if( nRandomOptionSize < MIN_SIZE_RANDOMOPTION_RETRY )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT03 );	// 부여된 랜덤 옵션이 적다.
		return false;
	}

	if( pMainItem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT04 );	// 각성 보호 상태이다.
		return false;
	}

	if( nRandomOptionSize <= pMsg->byIndex )
	{
		// 재굴림 할 옵션 인덱스가 사이즈를 넘어선다.
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_RANDOMOPTION_RETRY )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT01 );	// 각성 재굴림의 두루마리가 아니다.
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
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

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_GENERAL_RANDOMOPTION_INIT )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false || pMainItem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
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

	if( pMaterialProp->dwItemKind3 != IK3_DECREASE_EQUIP_LEVEL )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainProp->dwParts == NULL_ID || pMainProp->nMinLimitLevel <= 1 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->GetLevelDown() > 0 )
	{
		if( pMainItem->GetLevelDown() >= pMaterialProp->nEffectValue )
		{
			pUser->AddDefinedText( TID_GAME_ITEM_LEVELDOWN01 );
			return false;
		}
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	if( pMainProp == NULL )
	{
		return false;
	}

	if( pMainProp->dwParts == NULL_ID || pMainProp->nMinLimitLevel <= 1 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->GetLevelDown() <= 0 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
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

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			return false;
		}

		if( pProtectionProp->dwItemKind2 != IK2_TWOWEAPONMERGE || pProtectionProp->dwItemKind3 != IK3_TWOWEAPONMERGE_PROTECTION )
		{
			pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT10 );
			return false;
		}
	}

	if( pUser->m_Inventory.IsEquip( pMaterialItem->m_dwObjId ) == TRUE )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		return false;
	}

	// 이미 합성된 아이템
	if( pMainItem->IsSetCombinedOption() == true || pMaterialItem->IsSetCombinedOption() == true )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT01 );
		return false;
	}

	// 둘 다 양손무기인가
	if( pMainProp->dwHanded != HD_TWO || pMaterialProp->dwHanded != HD_TWO )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 );
		return false;
	}

	// 합성이 불가능한 아이템일 경우
	const T_COMBINE_DATA* pCombineData = GetCombineData( pMainProp, m_vecCombineData );
	if( pCombineData == NULL )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 );
		return false;
	}

	if( pMaterialProp->IsBaruna() == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT04 );
		return false;
	}

	// 종류가 다른 아이템일 경우
	if( pMainProp->dwItemKind1 != pMaterialProp->dwItemKind1
		|| pMainProp->dwItemKind2 != pMaterialProp->dwItemKind2
		|| pMainProp->dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT05 );
		return false;
	}

	// 재료아이템이 거래 불가능인 경우
	if( pMaterialItem->IsOwnState() == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT06 );
		return false;
	}

	// 재료아이템의 레벨이 더 높은 경우
	if( pMainProp->dwItemJob <= pMaterialProp->dwItemJob && pMainProp->nMinLimitLevel < pMaterialProp->nMinLimitLevel )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT07 );
		return false;
	}

	if( pCombineData->dwCombineOption & IUTYPE_RANDOM_OPTION_EXTENSION && pMaterialItem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT13 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeGeneral::CanUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	if( pMainProp == NULL )
	{
		return false;
	}

	if( pMainItem->IsSetCombinedOption() == false )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT11 );
		return false;
	}

	const T_COMBINE_DATA* pCombindData = GetCombineData( pMainProp, m_vecCombineData );
	if( pCombindData == NULL )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 );
		return false;
	}

	if( pUser->CheckUserGold( pCombindData->nInitializePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT12, "%d", pCombindData->nInitializePenya );
		return false;
	}

	return true;
}
// CARD_UPGRADE_SYSTEM
bool	FLItemUpgradeGeneral::CanUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE )
	{
		return false;
	}

	if( pMainItem->m_nItemNum < 10 )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	if( pMainProp == NULL )
	{
		return false;
	}

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			return false;
		}

		/*const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			return false;
		}*/

		// 클라이언트에서 이미 올라갈 수 있는 아이템을 처리했기 때문에 다른 메세지 없이 리턴시킵니다.
		if( pProtectionItem->m_dwItemId != ITEM_INDEX( 24843, II_SYS_SYS_SCR_SMELPROT5 ) )
		{
			//pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT10 );
			return false;
		}
	}



	// 합성이 불가능한 아이템일 경우
	const T_CARD_COMBINE_DATA* pCombineCardData = GetCardCombineData( pMainProp, m_vecCardCombineData );
	if( pCombineCardData == NULL )
	{
		//pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 );
		// 메세지 변경해야 합니다.
		return false;
	}
	int nConsumption = pMainProp->dwReferTarget2;

	if( ( pUser->GetGold() - nConsumption ) < 0)
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );	// 페냐가 부족하다.
		return false;
	}


	return true;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const int nAbilityOption		= pMainItem->GetAbilityOption();

	TCHAR szConText[64]				= { 0, };
	bool bDestroyProtect			= false;
	DWORD dwAddSuccessProb			= 0;
	//////////////////////////////////////////////////////////////////////////
	// 안전 제련 여부 확인
	if( pMsg->bSafetyUpgrade == true )
	{
		FLStrcpy( szConText, _countof( szConText ), "UPGRADEITEM_SMELTSAFETY" );

		bDestroyProtect				= true;

		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		g_DPSrvr.PutItemLog( pUser, "N", szConText, pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );

		if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
		{
			FLItemElem* pProbIncreaseItem	= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );

			if( nAbilityOption <= pProbIncreaseItem->GetProp()->nTargetMaxEnchant )
			{
				dwAddSuccessProb	+= pProbIncreaseItem->GetProp()->nEffectValue;

				g_DPSrvr.PutItemLog( pUser, "N", szConText, pProbIncreaseItem );
				pUser->RemoveItem( pProbIncreaseItem->m_dwObjId, 1 );
			}
		}
	}
	else
	{
		FLStrcpy( szConText, _countof( szConText ), "UPGRADEITEM" );

		if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) ) )
		{
			bDestroyProtect		= true;
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) );
		}

		dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GENERAL_ENCHANT_RATE, true );
		dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GEN_ATT_ENCHANT_RATE, true );
		if( pMainItem->GetProp()->dwItemKind1 == IK1_WEAPON )
		{
			dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GENERAL_WEAPON_ENCHANT_RATE, true );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainItem->GetProp(), m_vecGeneralEnchantData );

	UpgradeProbDataMap::const_iterator kCItr			= pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	const DWORD dwRandomProb			= xRandom( MAX_UPGRADE_PROB );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( kEnchantProb.dwSuccessProb + dwAddSuccessProb > dwRandomProb )
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
		if( nAbilityOption >= 3 )	// TODO
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

DWORD	FLItemUpgradeGeneral::DoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const int nResistAbilityOption	= pMainItem->GetResistAbilityOption();

	//////////////////////////////////////////////////////////////////////////
	// 보호의 두루마리 체크
	bool bDestroyProtect	= false;

	if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) ) )
	{
		bDestroyProtect		= true;
		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 확률 증가 체크
	DWORD dwAddSuccessProb	= 0;
	dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueAttributeEnchantRateBuff( pUser, pMainItem, IK3_GEN_ATT_ENCHANT_RATE, true );

	if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10489, II_SYS_SYS_SCR_SMELTING2 ) ) )	// 속성 제련의 두루마리
	{
		if( nResistAbilityOption < 10 )
		{
			dwAddSuccessProb	+= 1000;
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10489, II_SYS_SYS_SCR_SMELTING2 ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainItem->GetProp(), m_vecAttributeEnchantData );

	UpgradeProbDataMap::const_iterator kCItr			= pEnchantData->mapUpgradeProb.find( nResistAbilityOption + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	const DWORD dwRandomProb			= xRandom( MAX_UPGRADE_PROB );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( kEnchantProb.dwSuccessProb + dwAddSuccessProb > dwRandomProb )
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_IR,  pMaterialItem->GetProp()->eItemType );
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_RAO, nResistAbilityOption + 1 );

		g_DPSrvr.PutItemLog( pUser, "O", "UPGRADEITEM", pMainItem );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	// fail
	else
	{
		// exist option
		if( nResistAbilityOption >= 3 )		// TODO
		{
			if( bDestroyProtect == true )
			{
				g_DPSrvr.PutItemLog( pUser, "J", "UPGRADEITEM", pMainItem );
			}
			else
			{
				g_DPSrvr.PutItemLog( pUser, "L", "UPGRADEITEM", pMainItem );
				pUser->RemoveItem( pMainItem->m_dwObjId, pMainItem->m_nItemNum );
			}
		}
		// not exist option
		else
		{
			g_DPSrvr.PutItemLog( pUser, "J", "UPGRADEITEM", pMainItem );
		}

		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ATT_ENCHANT_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
{
	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_IR, static_cast<DWORD>( pMsg->byAttribute ) );

	g_DPSrvr.PutItemLog( pUser, "O", "CHANGE_ATTRIBUTE_TARGET", pMainItem );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "O", "CHANGE_ATTRIBUTE_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_IR, SAI79::NO_PROP );
	pUser->UpdateItem( pMainItem->m_dwObjId, UI_RAO, 0 );

	g_DPSrvr.PutItemLog( pUser, "O", "REMOVE_ATTRIBUTE", pMainItem );

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainItem->GetProp(), m_vecAttributeEnchantData );

	// 페냐 제거
	pUser->AddGold( -( pEnchantData->nAttributeRemovePenya ) );
	g_DPSrvr.PutPenyaLog( pUser, "O", "REMOVE_ATTRIBUTE", pEnchantData->nAttributeRemovePenya );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	bool bDestroyProtect	= false;
	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( pProtectionItem->m_dwItemId == ITEM_INDEX( 10465, II_SYS_SYS_SCR_PIEPROT ) || pProtectionItem->m_dwItemId == ITEM_INDEX( 25234, II_SYS_SYS_SCR_PIEPROT2 ))
		{
			bDestroyProtect = true;
		}

		g_DPSrvr.PutItemLog( pUser, "!", "PIERCING_PROTECTED", pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}

	const size_t nPiercingSize					= pMainItem->GetGeneralPiercingSize();

	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainItem->GetProp(), m_vecPiercingData );

	UpgradeProbDataMap::const_iterator kCItr				= pPiercingData->mapSizeProb.find( nPiercingSize + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	const DWORD dwRandomProb			= xRandom( MAX_UPGRADE_PROB );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( kEnchantProb.dwSuccessProb > dwRandomProb )
	{
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_PIERCING_SIZE, nPiercingSize + 1 );
		g_DPSrvr.PutItemLog( pUser, "#", "PIERCING", pMainItem );
		
		pUser->AddDefinedText( TID_MMI_PIERCINGSUCCESS );
		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	// fail
	else
	{
		if( bDestroyProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "!", "PIERCING_PROTECTED", pMainItem );
		}
		else
		{
			g_DPSrvr.PutItemLog( pUser, "!", "PIERCING", pMainItem );
			pUser->RemoveItem( pMainItem->m_dwObjId, pMainItem->m_nItemNum );
		}

		pUser->AddDefinedText( TID_MMI_PIERCINGFAIL );
		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	// 페냐 제거
	pUser->AddGold( -( pPiercingData->nSizeIncreasePenya ) );
	g_DPSrvr.PutPenyaLog( pUser, "!", "PIERCING", pPiercingData->nSizeIncreasePenya );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "!", "PIERCING", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	if( pUser->Pierce( pMainItem, pMaterialItem->m_dwItemId ) == TRUE )
	{
		g_DPSrvr.PutItemLog( pUser, "$", "PIERCING", pMainItem );
		g_DPSrvr.PutItemLog( pUser, "!", "PIERCING", pMaterialItem );

		// 재료 아이템 제거
		pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem	= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	const size_t nPiercingSize = pMainItem->GetGeneralPiercingSize();

	for( int nNth = ( nPiercingSize - 1 ); nNth >= 0; --nNth )
	{
		if( pMainItem->GetGeneralPiercingItemID( nNth ) != 0 )
		{
			pUser->UpdateItem( pMainItem->m_dwObjId, UI_PIERCING, MAKELONG( nNth, 0 ) );
			g_DPSrvr.PutItemLog( pUser, "$", "PIERCING_REMOVE", pMainItem );

			// 페냐 제거
			const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainItem->GetProp(), m_vecPiercingData );
			
			pUser->AddGold( -( pPiercingData->nItemRemovePenya ) );
			g_DPSrvr.PutPenyaLog( pUser, "!", "PIERCING_REMOVE", pPiercingData->nItemRemovePenya );
			
			pUser->AddDefinedText( TID_GAME_REMOVE_PIERCING_SUCCESS );
			
			dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;

			break;
		}
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const bool bIsSetSafeGuardFlag	= pMainItem->IsSetSafeGuardRandomOptionExtensionFlag();

	const bool bResult = ( bIsSetSafeGuardFlag == true )
		? SetNewRandomOptionExtension( *pMainItem, m_vecRandomOptionData, m_mapDstParameterData )
		: SetRandomOptionExtension( *pMainItem, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		if( bIsSetSafeGuardFlag == true )
		{
			pMainItem->ResetSafeGuardRandomOptionExtensionFlag();
		}

		g_DPSrvr.PutItemLog( pUser, "r", "::Awake", pMainItem );
		g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
		pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

		pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const bool bResult = SetRetryRandomOptionExtension( *pMainItem, pMsg->byIndex, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		g_DPSrvr.PutItemLog( pUser, "r", "AWAKE_RETRY", pMainItem, pMainItem->m_nItemNum );
		g_DPSrvr.PutItemLog( pUser, "u", "AWAKE_RETRY_MATERIAL", pMaterialItem );
		pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

//		pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RETRY RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	if( pUser->HasBuffByIk3( IK3_GENERAL_RANDOMOPTION_SAFE ) == TRUE )
	{
		pUser->RemoveIk3Buffs( IK3_GENERAL_RANDOMOPTION_SAFE );

		pMainItem->SetSafeGuardRandomOptionExtensionFlag();

		g_DPSrvr.PutItemLog( pUser, "z", "USE_AWAKESAVE_ITEM", pMainItem, pMainItem->m_nItemNum );

		pUser->AddDiagText( prj.GetText( TID_GAME_REGARDLESS_USE04 ) );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		pMainItem->InitializeRandomOptionExtension();

		g_DPSrvr.PutItemLog( pUser, "r", "::AwakeCancel", pMainItem, pMainItem->m_nItemNum );

		pUser->AddDefinedText( TID_GAME_PETAWAKECANCEL_S00 );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}

	g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_EQUIP_LEVEL, pMaterialItem->GetProp()->nEffectValue );

	g_DPSrvr.PutItemLog( pUser, "r", "::ItemLevelDown", pMainItem );
	g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_EQUIP_LEVEL, 0 );

	g_DPSrvr.PutItemLog( pUser, "v", "OnRemoveItemLevelDown", pMainItem );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;
	TCHAR szComment[ 64 ] = { 0, };
	bool bProtection = false;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );
	FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );

	const T_COMBINE_DATA* pCombineData = GetCombineData( pMainItem->GetProp(), m_vecCombineData );
	
	const DWORD dwRandomProb = ::xRandom( MAX_UPGRADE_PROB );

	if( dwRandomProb < pCombineData->dwSuccessProb )	// success
	{
		pMainItem->SetCombinedOption( true );

		if( pCombineData->dwCombineOption & IUTYPE_ENCHANT_GENERAL )
		{
			const DWORD dwCombinedAddDamage = pMaterialItem->GetMinAddDamage();

			pMainItem->SetCombinedAddDamage( dwCombinedAddDamage );
		}
		if( pCombineData->dwCombineOption & IUTYPE_PIERCING )
		{
			pMainItem->SetCombinedGeneralPiercingSize( pMaterialItem->GetGeneralPiercingSize() );

			for( size_t Nth = 0; Nth < pMaterialItem->GetGeneralPiercingSize(); ++Nth )
			{
				const DWORD dwPiercedItemID = pMaterialItem->GetGeneralPiercingItemID( Nth );
				const T_ITEM_SPEC* pPiercedItemProp = g_xSpecManager->GetSpecItem( dwPiercedItemID );
				if( pPiercedItemProp != NULL )
				{
					pMainItem->SetCombinedGeneralPiercingItemID( Nth, dwPiercedItemID );
				}
			}
		}
		if( pCombineData->dwCombineOption & IUTYPE_RANDOM_OPTION_ORIGIN )
		{
			const PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pMaterialItem->GetRandomOptionOriginID() );
			if( pRandomOptItem != NULL )
			{
				pMainItem->SetCombinedRandomOptionOriginID( pMaterialItem->GetRandomOptionOriginID() );
			}
		}
		if( pCombineData->dwCombineOption & IUTYPE_RANDOM_OPTION_EXTENSION && pMaterialItem->IsSetRandomOptionExtension() == true )
		{
			for( size_t Nth = 0; Nth < pMaterialItem->GetRandomOptionExtensionSize(); ++Nth )
			{
				T_RANDOMOPTION_EXT kRandomOptionExtension;

				kRandomOptionExtension.wDstID		= pMaterialItem->GetRandomOptionExtensionDstID( Nth );
				kRandomOptionExtension.shAdjValue	= pMaterialItem->GetRandomOptionExtensionAdjValue( Nth );

				pMainItem->SetCombinedRandomOptionExtension( kRandomOptionExtension );
			}
		}

		pUser->AddCombinedOption( pMainItem );

		FLStrcpy( szComment, _countof( szComment ), _T( "COMBINE_SUCCESS" ) );

		pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_SUCCESSTEXT01 );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else	// fail
	{
		if( pProtectionItem != NULL )
		{
			bProtection = true;
			FLStrcpy( szComment, _countof( szComment ), _T( "COMBINE_FAIL_PROCTION" ) );

			pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_FAILTEXT02 );
		}
		else
		{
			FLStrcpy( szComment, _countof( szComment ), _T( "COMBINE_FAIL_DESTROY" ) );

			pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_FAILTEXT01, "\"%s\"", pMaterialItem->GetProp()->szName );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	g_DPSrvr.PutItemLog( pUser, "O", szComment, pMainItem );
	g_DPSrvr.PutItemLog( pUser, "R", szComment, pMaterialItem );

	if( bProtection == false )
	{
		pUser->RemoveItem( pMaterialItem->m_dwObjId, pMaterialItem->m_nItemNum );
	}

	if( pProtectionItem != NULL )
	{
		g_DPSrvr.PutItemLog( pUser, "R", szComment, pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}

	return dwResult;
}

DWORD	FLItemUpgradeGeneral::DoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	const T_COMBINE_DATA* pCombineData = GetCombineData( pMainItem->GetProp(), m_vecCombineData );

	if( pCombineData->dwCombineOption & IUTYPE_ENCHANT_GENERAL )
	{
		pMainItem->InitializeCombinedAddDamage();
	}

	if( pCombineData->dwCombineOption & IUTYPE_PIERCING )
	{
		pMainItem->InitializeCombinedPiercingItem();
	}

	if( pCombineData->dwCombineOption & IUTYPE_RANDOM_OPTION_ORIGIN )
	{
		pMainItem->InitializeCombinedRandomOptionOriginID();
	}

	if( pCombineData->dwCombineOption & IUTYPE_RANDOM_OPTION_EXTENSION )
	{
		pMainItem->InitializeCombinedRandomOptionExtension();
	}

	pMainItem->SetCombinedOption( false );

	pUser->AddCombinedOption( pMainItem );

	pUser->AddGold( -( pCombineData->nInitializePenya ) );

	g_DPSrvr.PutItemLog( pUser, "O", "COMBINE_INITIALIZE", pMainItem );
	g_DPSrvr.PutPenyaLog( pUser, "R", "COMBINE_INITIALIZE", pCombineData->nInitializePenya );

	pUser->AddDefinedText( TID_MMI_TWOHANDEDWEAPONMERGE_SUCCESSTEXT02 );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}
// CARD_UPGRADE_SYSTEM
DWORD	FLItemUpgradeGeneral::DoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;
	TCHAR szComment[ 64 ] = { 0, };
	bool bProtection = false;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );

	const T_CARD_COMBINE_DATA* pCombineCardData = GetCardCombineData( pMainItem->GetProp(), m_vecCardCombineData );
	
	const DWORD dwRandomProb = ::xRandom( MAX_UPGRADE_PROB );


	// 페냐 소비
	int nConsumption = pMainItem->GetProp()->dwReferTarget2;

	if( dwRandomProb < pCombineCardData->dwSuccessProb )	// success
	{
		
		FLItemElem CreateItem;
		//CreateItem	= *pMainItem;
		CreateItem.m_dwItemId = pMainItem->GetProp()->dwReferTarget1;
		CreateItem.m_nItemNum = 1;
		CreateItem.SetSerialNumber();

		if( pUser->CreateItem( &CreateItem ) )
		{

			FLStrcpy( szComment, _countof( szComment ), _T( "CARD_COMBINE_SUCCESS" ) );

			pUser->AddDefinedText( TID_MMI_CARDUPGRADE_SUCCESSTEXT01 );

			dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
			
			
		}
		else
		{
			FLStrcpy( szComment, _countof( szComment ), _T( "CARD_COMBINE_CRATE_FAIL" ) );
			//pUser->AddDefinedText( TID_MMI_CARDUPGRADE_SUCCESSTEXT02 );
			dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
		}
		g_DPSrvr.PutItemLog( pUser, "O", szComment, pMainItem );
		pUser->RemoveItem( pMainItem->m_dwObjId, 10 );
	}
	else	// fail
	{
		if( pProtectionItem != NULL )
		{
			bProtection = true;
			FLStrcpy( szComment, _countof( szComment ), _T( "CARD_COMBINE_FAIL_PROCTION" ) );
			g_DPSrvr.PutItemLog( pUser, "O", szComment, pMainItem );
			pUser->AddDefinedText( TID_MMI_CARDUPGRADE_FAILTEXT02 );
		}
		else
		{
			FLStrcpy( szComment, _countof( szComment ), _T( "CARD_COMBINE_FAIL_DESTROY" ) );

			pUser->AddDefinedText( TID_MMI_CARDUPGRADE_FAILTEXT01, "\"%s\"", pMainItem->GetProp()->szName );
			g_DPSrvr.PutItemLog( pUser, "O", szComment, pMainItem );
			pUser->RemoveItem( pMainItem->m_dwObjId, 1 );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	


	/*if( bProtection == false )
	{
		pUser->RemoveItem( pMainItem->m_dwObjId, 1 );
	}*/

	if( pProtectionItem != NULL )
	{
		g_DPSrvr.PutItemLog( pUser, "R", szComment, pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}
	
	pUser->AddGold( -nConsumption );
	g_DPSrvr.PutPenyaLog( pUser, "O", "CARD_COMBINE_PENYA", nConsumption );

	return dwResult;
}
