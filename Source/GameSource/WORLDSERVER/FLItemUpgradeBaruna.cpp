
#include "StdAfx.h"
#include "FLItemUpgradeBaruna.h"
#include "FLBarunaUpgradeNpc.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeBaruna::FLItemUpgradeBaruna()
{
}

FLItemUpgradeBaruna::~FLItemUpgradeBaruna()
{
}

FLItemUpgradeBaruna*	FLItemUpgradeBaruna::GetInstance()
{
	static FLItemUpgradeBaruna	sItemUpgradeBaruna;

	return & sItemUpgradeBaruna;
}

void	FLItemUpgradeBaruna::Clear()
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
}

bool	FLItemUpgradeBaruna::LoadScript( const TCHAR* pszFileName )
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

#ifdef BARUNA_ULTIMATE_UPDATE
	// Load UltimateTrans Data
	if( LoadUltimateTransData( kLua, m_vecUltimateTransData ) == false )
	{
		return false;
	}
#endif

	return true;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemGeneralEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemGeneralEnchant( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeEnchant( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeChange( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeChange( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemAttributeRemove( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemAttributeRemove( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemPiercingItemInsert( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemPiercingItemInsert( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerate( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerateRetry( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerateRetry( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemEquipLevelDecrease( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemEquipLevelDecrease( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemEquipLevelInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemEquipLevelInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksChange( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksChange( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksInitialize( pUser, pMsg );
	}

	return dwResult;
}
#ifdef BARUNA_ULTIMATE_UPDATE
DWORD	FLItemUpgradeBaruna::OnDoUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemUltimateTrans( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemUltimateTrans( pUser, pMsg );
	}

	return dwResult;
}
#endif

DWORD	FLItemUpgradeBaruna::GetAttributeDamageFactor( const FLItemElem & kItemElem ) const
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

DWORD	FLItemUpgradeBaruna::GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const
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

DWORD	FLItemUpgradeBaruna::GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const
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

bool	FLItemUpgradeBaruna::GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const
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

size_t	FLItemUpgradeBaruna::GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const
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

size_t	FLItemUpgradeBaruna::GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const
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

size_t	FLItemUpgradeBaruna::GetMaxSizePiercing( const DWORD dwItemID ) const
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

size_t	FLItemUpgradeBaruna::GetMaxSizeRandomOption( const DWORD dwItemID ) const
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

bool	FLItemUpgradeBaruna::CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_GENERAL ) == false )
	{
		return false;
	}

	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
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
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_2 );
		return false;
	}

	if( pEnchantData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pEnchantData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pEnchantData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT02 );
		return false;
	}

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem		= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_BARUNASMELPROT );
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_BARUNASMELPROT );
			return false;
		}

		int nProtectionUseResult = g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMainItem );

		if( nProtectionUseResult != FLUseActiveItem::ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT
			&& nProtectionUseResult != FLUseActiveItem::ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT_KEEP )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_BARUNASMELPROT );
			return false;
		}
	}

	const int nAbilityOption	= pMainItem->GetAbilityOption();
	if( nAbilityOption < 0 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );
	if( kCItr == pEnchantData->mapUpgradeProb.end() )
	{
		pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR13 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeBaruna::CanUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_ATTRIBUTE ) == false )
	{
		return false;
	}

	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
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

	if( pMainItem->m_byItemResist != SAI79::NO_PROP && pMainItem->m_byItemResist != pMaterialProp->eItemType )
	{
		pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT05 );
		return false;
	}

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
	{
		pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT03 );
		return false;
	}

	if( pEnchantData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pEnchantData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pEnchantData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT07 );
		return false;
	}

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem		= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT09 );
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT09 );
			return false;
		}

		int nProtectionUseResult = g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMainItem );

		if( nProtectionUseResult != FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT
			&& nProtectionUseResult != FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT_KEEP )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT09 );
			return false;
		}
	}

	if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
	{
		FLItemElem* pProbabilityItem		= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );
		if( IsUsableItem( pProbabilityItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT10 );
			return false;
		}

		const T_ITEM_SPEC* pProbabilityProp	= pProbabilityItem->GetProp();
		if( pProbabilityProp == NULL )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT10 );
			return false;
		}

		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProbabilityItem, pMainItem ) != FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROB )
		{
			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT10 );
			return false;
		}

		if( static_cast<DWORD>( pProbabilityProp->nEffectValue ) > MAX_UPGRADE_PROB )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Prob Increase Value is Invalid. IncreaseProb:[%d]" ), pProbabilityProp->nEffectValue );
			return false;
		}
	}

	const int nResistAbilityOption	= pMainItem->GetResistAbilityOption();
	if( nResistAbilityOption < 0 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nResistAbilityOption + 1 );
	if( kCItr == pEnchantData->mapUpgradeProb.end() )
	{
		pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_E_TEXT04 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeBaruna::CanUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
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

bool	FLItemUpgradeBaruna::CanUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ENCHANT_ATTRIBUTE ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();

	if( pMainItem->m_byItemResist <= SAI79::NO_PROP || pMainItem->m_byItemResist >= SAI79::END_PROP || pMainItem->GetResistAbilityOption() <= 0 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainProp, m_vecAttributeEnchantData );
	if( pEnchantData == NULL )
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

bool	FLItemUpgradeBaruna::CanUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_PIERCING ) == false )
	{
		return false;
	}

	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
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
		FLItemElem* pProtectionItem		= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( IsUsableItem( pProtectionItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_7 );
			return false;
		}

		const T_ITEM_SPEC* pProtectionProp	= pProtectionItem->GetProp();
		if( pProtectionProp == NULL )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_7 );
			return false;
		}

		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMaterialItem ) != FLUseActiveItem::ERR_SUCCESS_BARUNA_PIERCING_PROTECT )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_7 );
			return false;
		}
	}

	if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
	{
		FLItemElem* pProbabilityItem		= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );
		if( IsUsableItem( pProbabilityItem ) == FALSE )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_8 );
			return false;
		}

		const T_ITEM_SPEC* pProbabilityProp	= pProbabilityItem->GetProp();
		if( pProbabilityProp == NULL )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_8 );
			return false;
		}

		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProbabilityItem, pMainItem ) != FLUseActiveItem::ERR_SUCCESS_BARUNA_PIERCING_PROB )
		{
			pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_8 );
			return false;
		}
	}

	const T_PIERCING_DATA* pPiercingData	= GetPiercingData( pMainProp, m_vecPiercingData );
	if( pPiercingData == NULL )
	{
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_2 );
		return false;
	}

	if( pPiercingData->kPiercedKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pPiercingData->kPiercedKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pPiercingData->kPiercedKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_5 );
		return false;
	}

	if( pMainItem->IsExistGeneralPiercedItemID( pMaterialItem->m_dwItemId ) == true )
	{
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_1 );
		return false;
	}

	const size_t nPiercingSize		= pMainItem->GetGeneralPiercingSize();

	UpgradeProbDataMap::const_iterator kCItr	= pPiercingData->mapSizeProb.find( nPiercingSize + 1 );

	if( kCItr == pPiercingData->mapSizeProb.end() )
	{
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_3 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeBaruna::CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE || pMsg->dwMaterialItemObjID != NULL_ID )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	if( pMainProp == NULL )
	{
		return false;
	}

	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == true )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pUser->CheckUserGold( pRandomOptionData->nRandomOptionGeneratePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return false;
	}

	return true;
}

bool	FLItemUpgradeBaruna::CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
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

bool	FLItemUpgradeBaruna::CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
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

	if( pMaterialProp->dwItemKind3 != IK3_BARUNA_RANDOMOPTION_INIT )	// TODO
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_GAME_AWAKECANCEL );
		return false;
	}

	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
	{
		return false;
	}

	return true;
}

bool	FLItemUpgradeBaruna::CanUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
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

	if( pMainProp->dwParts == NULL_ID || pMainProp->nMinLimitLevel <= 1 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_DECREASE_EQUIP_LEVEL )
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

bool	FLItemUpgradeBaruna::CanUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
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

	if( pMainProp->dwParts == NULL_ID )
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
#ifdef BARUNA_ULTIMATE_UPDATE
bool	FLItemUpgradeBaruna::CanUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const
{
	// 업그레이드가 가능한 아이템인가?
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_ULTIMATE_TRANS ) == false )
	{
		return false;
	}
	// NPC가 있나?
	if( g_xBarunaUpgradeNpc->IsValidStateUpgradeNPC( pUser, pMsg->dwUpgradeNpcObjID ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem1	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID1 );
	FLItemElem* pMaterialItem2	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID2 );	
	if( IsUsableItem( pMainItem ) == FALSE || IsUsableItem( pMaterialItem1 ) == FALSE || IsUsableItem( pMaterialItem2 ) == FALSE)
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	const T_ITEM_SPEC* pMaterialProp1	= pMaterialItem1->GetProp();
	const T_ITEM_SPEC* pMaterialProp2	= pMaterialItem2->GetProp();
	if( pMainProp == NULL || pMaterialProp1 == NULL || pMaterialItem2 == NULL)
	{
		return false;
	}

	const T_ULTIMATETRANS_DATA* pUltimateTransData	= GetUltimateData( pMainProp, m_vecUltimateTransData );
	if( pUltimateTransData == NULL )
	{
		pUser->AddDefinedText( TID_BW_PEARCING_ERROR_MSG_2 );
		return false;
	}

	if( pUltimateTransData->kMaterialKind1.dwItemKind1 != pMaterialProp1->dwItemKind1
		|| pUltimateTransData->kMaterialKind1.dwItemKind2 != pMaterialProp1->dwItemKind2
		|| pUltimateTransData->kMaterialKind1.dwItemKind3 != pMaterialProp1->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT02 );
		return false;
	}

	if( pUltimateTransData->kMaterialKind2.dwItemKind1 != pMaterialProp2->dwItemKind1
		|| pUltimateTransData->kMaterialKind2.dwItemKind2 != pMaterialProp2->dwItemKind2
		|| pUltimateTransData->kMaterialKind2.dwItemKind3 != pMaterialProp2->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT02 );	// 보석으로 바꿔야함.
		return false;
	}


	if ( pMaterialItem1->m_nItemNum < 5 || pMaterialItem2->m_nItemNum < 5 )
	{
		// 재료가 부족합니다.
		return false;
	}
	

	const int nAbilityOption	= pMainItem->GetAbilityOption();
	if( nAbilityOption != 20 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pUser->CheckUserGold( pUltimateTransData->nInitializePenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return false;
	}

	return true;
}
#endif
DWORD	FLItemUpgradeBaruna::DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	// 보호의 두루마리 체크
	bool bDestroyProtect	= false;
	bool bMinusProtect		= false;

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );

		int nProtectionResult = g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMainItem );

		if( nProtectionResult == FLUseActiveItem::ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT )
		{
			bDestroyProtect = true;
		}
		else if( nProtectionResult == FLUseActiveItem::ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT_KEEP )
		{
			bDestroyProtect = true;
			bMinusProtect = true;
		}

		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_GENERAL_ENCHANT_PROT", pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}

	const int nAbilityOption					= pMainItem->GetAbilityOption();

	const T_ENCHANT_DATA* pEnchantData			= GetEnchantData( pMainItem->GetProp(), m_vecGeneralEnchantData );

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	DWORD dwAddSuccessProb				= g_xBarunaUpgradeNpc->GetAdditionalSuccessProb( pUser );

	const DWORD dwRandomProb					= ::xRandom( MAX_UPGRADE_PROB );

	dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_BARUNA_ENCHANT_RATE, true );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( dwRandomProb < kEnchantProb.dwSuccessProb + dwAddSuccessProb )
	{
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_AO, ( nAbilityOption + 1 ) );
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		if( pMainItem->GetProp()->dwItemKind1 == IK1_WEAPON && (nAbilityOption + 1) >= 12 )
		{
			int nPiercingSize = ((nAbilityOption + 1) - 10) * 0.5f;
			pUser->UpdateItem( pMainItem->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nPiercingSize );
		}
#endif
		g_DPSrvr.PutItemLog( pUser, "S", "BARUNA_GENERAL_ENCHANT_SUCCESS", pMainItem );

		pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT03 );
		
		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	// fail
	else if( dwRandomProb < kEnchantProb.dwSuccessProb + kEnchantProb.dwFailProb )
	{
		if( bDestroyProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "F", "BARUNA_GENERAL_ENCHANT_FAIL", pMainItem );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT05 );
		}
		else
		{
			g_DPSrvr.PutItemLog( pUser, "L", "BARUNA_GENERAL_ENCHANT_FAIL", pMainItem );
			pUser->RemoveItem( pMainItem->m_dwObjId, 1 );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT06 );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}
	// minus
	else if( dwRandomProb < kEnchantProb.dwSuccessProb + kEnchantProb.dwFailProb + kEnchantProb.dwMinusProb )
	{
		if( bMinusProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "F", "BARUNA_GENERAL_ENCHANT_MINUS", pMainItem );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT05 );
		}
		else
		{
			pUser->UpdateItem( pMainItem->m_dwObjId, UI_AO, nAbilityOption - 1 );
			g_DPSrvr.PutItemLog( pUser, "M", "BARUNA_GENERAL_ENCHANT_MINUS", pMainItem );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT04 );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_MINUS;
	}
	// error
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR : dwItemKind1(%u) probSuccess(%u) AddSuccessProp(%u) probFail(%u) probMinus(%u) ]" )
			, pMainItem->GetProp()->dwItemKind1, kEnchantProb.dwSuccessProb, dwAddSuccessProb, kEnchantProb.dwFailProb, kEnchantProb.dwMinusProb );

		return dwResult;
	}
	//////////////////////////////////////////////////////////////////////////

	// NPC 경험치 부여
	g_xBarunaUpgradeNpc->AddExpToBarunaNPC( pUser, IUTYPE_ENCHANT_GENERAL, nAbilityOption + 1, dwResult );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_GENERAL_ENCHANT_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	// 보호의 두루마리 체크
	bool bDestroyProtect	= false;
	bool bMinusProtect		= false;

	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );

		int nResult = g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMainItem );

		if( nResult == FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT )
		{
			bDestroyProtect = true;
		}
		else if( nResult == FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT_KEEP )
		{
			bDestroyProtect = true;
			bMinusProtect = true;
		}

		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ATT_ENCHANT_PROTECTION", pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}

	// 성공 확률 증가 체크
	DWORD dwAddSuccessProb = 0;
	if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
	{
		FLItemElem* pProbabilityItem	= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );

		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProbabilityItem, pMainItem ) == FLUseActiveItem::ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROB )
		{
			dwAddSuccessProb = static_cast<DWORD>( pProbabilityItem->GetProp()->nEffectValue );
		}

		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ATT_ENCHANT_PROB_INC", pProbabilityItem );
		pUser->RemoveItem( pProbabilityItem->m_dwObjId, 1 );
	}

	const int nResistAbilityOption				= pMainItem->GetResistAbilityOption();

	const T_ENCHANT_DATA* pEnchantData			= GetEnchantData( pMainItem->GetProp(), m_vecAttributeEnchantData );

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nResistAbilityOption + 1 );

	const T_UPGRADE_PROBABILITY kEnchantProb	= kCItr->second;

	dwAddSuccessProb							+= g_xBarunaUpgradeNpc->GetAdditionalSuccessProb( pUser );

	const DWORD dwRandomProb					= ::xRandom( MAX_UPGRADE_PROB );

	//////////////////////////////////////////////////////////////////////////
	// success
	if( dwRandomProb < kEnchantProb.dwSuccessProb + dwAddSuccessProb )
	{
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_IR,  pMaterialItem->GetProp()->eItemType );
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_RAO, nResistAbilityOption + 1 );
		g_DPSrvr.PutItemLog( pUser, "O", "BARUNA_ATT_ENCHANT_SUCCESS", pMainItem );

		pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_TEXT02 );
		
		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	// fail
	else if( dwRandomProb < kEnchantProb.dwSuccessProb + kEnchantProb.dwFailProb )
	{
		if( bDestroyProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "F", "BARUNA_ATT_ENCHANT_FAIL", pMainItem );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT05 );
		}
		else
		{
			g_DPSrvr.PutItemLog( pUser, "L", "BARUNA_ATT_ENCHANT_FAIL", pMainItem );
			pUser->RemoveItem( pMainItem->m_dwObjId, pMainItem->m_nItemNum );

			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_TEXT04 );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}
	// minus
	else if( dwRandomProb < kEnchantProb.dwSuccessProb + kEnchantProb.dwFailProb + kEnchantProb.dwMinusProb )
	{
		if( bMinusProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "F", "BARUNA_ATT_ENCHANT_MINUS", pMainItem );

			pUser->AddDefinedText( TID_MMI_NEWSMELT_NEWSMELT05 );
		}
		else
		{
			pUser->UpdateItem( pMainItem->m_dwObjId, UI_RAO, nResistAbilityOption - 1 );
			g_DPSrvr.PutItemLog( pUser, "M", "BARUNA_ATT_ENCHANT_MINUS", pMainItem );

			pUser->AddDefinedText( TID_MMI_BARUNA_ELESMELT_TEXT03 );
		}

		dwResult = FSC_ITEMUPGRADE_UPGRADE_MINUS;
	}
	// error
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR : dwItemKind1(%u) probSuccess(%u) AddSuccessProp(%u) probFail(%u) probMinus(%u) ]" )
			, pMainItem->GetProp()->dwItemKind1, kEnchantProb.dwSuccessProb, dwAddSuccessProb, kEnchantProb.dwFailProb, kEnchantProb.dwMinusProb );

		return dwResult;
	}
	//////////////////////////////////////////////////////////////////////////

	// NPC 경험치 부여
	g_xBarunaUpgradeNpc->AddExpToBarunaNPC( pUser, IUTYPE_ENCHANT_ATTRIBUTE, nResistAbilityOption + 1, dwResult );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ATT_ENCHANT_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const
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

DWORD	FLItemUpgradeBaruna::DoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const
{
	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_IR, SAI79::NO_PROP );
	pUser->UpdateItem( pMainItem->m_dwObjId, UI_RAO, 0 );

	g_DPSrvr.PutItemLog( pUser, "O", "REMOVE_ATTRIBUTE", pMainItem );

	// 페냐 제거
	const T_ENCHANT_DATA* pEnchantData	= GetEnchantData( pMainItem->GetProp(), m_vecAttributeEnchantData );

	pUser->AddGold( -( pEnchantData->nAttributeRemovePenya ) );
	g_DPSrvr.PutPenyaLog( pUser, "O", "REMOVE_ATTRIBUTE", pEnchantData->nAttributeRemovePenya );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	bool bDestroyProtect	= false;
	if( pMsg->dwProtectionItemObjID != NULL_ID )
	{
		FLItemElem* pProtectionItem	= pUser->m_Inventory.GetAtId( pMsg->dwProtectionItemObjID );
		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProtectionItem, pMaterialItem ) == FLUseActiveItem::ERR_SUCCESS_BARUNA_PIERCING_PROTECT )
		{
			bDestroyProtect = true;
		}

		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_PIERCE_PROTECT", pProtectionItem );
		pUser->RemoveItem( pProtectionItem->m_dwObjId, 1 );
	}

	DWORD dwAddSuccessProb	= 0;
	if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
	{
		FLItemElem* pProbabilityItem	= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );

		if( g_xUseActiveItem->OnDoUseActiveItem( pUser, pProbabilityItem, pMainItem ) == FLUseActiveItem::ERR_SUCCESS_BARUNA_PIERCING_PROB )
		{
			dwAddSuccessProb += static_cast<DWORD>( pProbabilityItem->GetProp()->nEffectValue );
		}

		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_PIERCE_PROB_INC", pProbabilityItem );
		pUser->RemoveItem( pProbabilityItem->m_dwObjId, 1 );
	}

	dwAddSuccessProb			+= g_xBarunaUpgradeNpc->GetAdditionalSuccessProb( pUser );

	const DWORD dwSuccessProb	= static_cast<DWORD>( pMaterialItem->GetProp()->nEffectValue );

	const size_t nPiercingSize	= pMainItem->GetGeneralPiercingSize();

	const DWORD dwRandomProb	= ::xRandom( MAX_UPGRADE_PROB );

	if( dwRandomProb < dwSuccessProb + dwAddSuccessProb )	// success
	{
		pUser->UpdateItem( pMainItem->m_dwObjId, UI_PIERCING_SIZE, nPiercingSize + 1 );

		pUser->Pierce( pMainItem, pMaterialItem->m_dwItemId );

		g_DPSrvr.PutItemLog( pUser, "$", "BARUNA_PIERCING_SUCCESS", pMainItem );

		g_DPSrvr.PutItemLog( pUser, "!", "BARUNA_PIERCING_MATERIAL", pMaterialItem );
		pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );
		
		pUser->AddDefinedText( TID_BW_PEARCING_CONFIRM_MSG_1 );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else	// fail
	{
		if( bDestroyProtect == true )
		{
			g_DPSrvr.PutItemLog( pUser, "!", "BARUNA_PIERCE_MATERIAL_PROTECT", pMaterialItem );
			
			pUser->AddDefinedText( TID_BW_PEARCING_CONFIRM_MSG_3 );
		}
		else
		{
			g_DPSrvr.PutItemLog( pUser, "!", "BARUNA_PIERCING_MATERIAL", pMaterialItem );
			pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

			pUser->AddDefinedText( TID_BW_PEARCING_CONFIRM_MSG_2 );
		}

		g_DPSrvr.PutItemLog( pUser, "$", "BARUNA_PIERCING_FAIL", pMainItem );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult	= FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	const bool bResult = SetRandomOptionExtension( *pMainItem, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainItem->GetProp(), m_vecRandomOptionData );

		pUser->AddGold( -( pRandomOptionData->nRandomOptionGeneratePenya ) );

		g_DPSrvr.PutItemLog( pUser, "r", "BARUNA_AWAKE_GEN", pMainItem );
		g_DPSrvr.PutPenyaLog( pUser, "r", "BARUNA_AWAKE_GEN", pRandomOptionData->nRandomOptionGeneratePenya );

		pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	// NPC 경험치 부여
	g_xBarunaUpgradeNpc->AddExpToBarunaNPC( pUser, IUTYPE_RANDOM_OPTION_EXTENSION, pMainItem->GetRandomOptionExtensionSize(), dwResult );

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
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

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RETRY RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	return dwResult;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pMainItem->InitializeRandomOptionExtension();

	g_DPSrvr.PutItemLog( pUser, "r", "BARUNA_AWAKE_INIT", pMainItem );
	g_DPSrvr.PutItemLog( pUser, "r", "BARUNA_AWAKE_INIT_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	pUser->AddDefinedText( TID_GAME_PETAWAKECANCEL_S00 );
	
	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_EQUIP_LEVEL, static_cast<DWORD>( pMaterialItem->GetProp()->nEffectValue ) );

	g_DPSrvr.PutItemLog( pUser, "r", "::ItemLevelDown", pMainItem );
	g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeBaruna::DoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_EQUIP_LEVEL, 0 );

	g_DPSrvr.PutItemLog( pUser, "v", "OnRemoveItemLevelDown", pMainItem );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}
#ifdef BARUNA_ULTIMATE_UPDATE
DWORD	FLItemUpgradeBaruna::DoUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem			= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem1		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID1 );
	FLItemElem* pMaterialItem2		= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID2 );

	

	const int nAbilityOption					= pMainItem->GetAbilityOption();

	const T_ENCHANT_DATA* pEnchantData			= GetEnchantData( pMainItem->GetProp(), m_vecGeneralEnchantData );

	UpgradeProbDataMap::const_iterator kCItr	= pEnchantData->mapUpgradeProb.find( nAbilityOption + 1 );


	//////////////////////////////////////////////////////////////////////////
	// 100% success
	FLItemElem CreateItem;
	CreateItem	= *pMainItem;
	CreateItem.m_dwItemId = pMainItem->GetProp()->dwReferTarget1;
	CreateItem.SetSerialNumber();

	LogItemInfo aLogItem;

	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BARUNAULTIMATE_TRANS" );


	CreateItem.SetAbilityOption( 0 );
	g_DPSrvr.OnLogItem( aLogItem, pMainItem, 1 );
	pUser->RemoveItem( pMsg->dwMainItemObjID, 1 );
	if( pUser->CreateItem( &CreateItem ) )
	{

		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "BARUNAULTIMATE_TRANS_SUCCESS" );
		g_DPSrvr.OnLogItem( aLogItem, &CreateItem, 1 );

			// NPC 경험치 부여
		//g_xBarunaUpgradeNpc->AddExpToBarunaNPC( pUser, IUTYPE_ENCHANT_GENERAL, 1, dwResult );

		// 재료 아이템 제거
		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ULTIMATE_UPDATE_TRANS_MATERIAL1", pMaterialItem1 );
		pUser->RemoveItem( pMaterialItem1->m_dwObjId, 5 );
		g_DPSrvr.PutItemLog( pUser, "N", "BARUNA_ULTIMATE_UPDATE_TRANS_MATERIAL2", pMaterialItem2 );
		pUser->RemoveItem( pMaterialItem2->m_dwObjId, 5 );

		// 페냐 소비
		pUser->AddGold( -( 100000000 ) );
		g_DPSrvr.PutPenyaLog( pUser, "O", "REMOVE_ULTIMATE_TRANS", 100000000 );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;

	}

	
	//////////////////////////////////////////////////////////////////////////

	

	return dwResult;
}

bool	FLItemUpgradeBaruna::LoadUltimateTransData( CLuaBase & kLua, UltimateTransDataVec & vecUltimateTransData ) const
{
	kLua.GetGloabal( _T( "tUltimate" ) );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_ULTIMATETRANS_DATA kUltimateData;

		kUltimateData.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kUltimateData.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kUltimateData.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );

		if( kUltimateData.dwItemKind1 == NULL_ID || kUltimateData.dwItemKind2 == NULL_ID || kUltimateData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kUltimateData.dwItemKind1, kUltimateData.dwItemKind2, kUltimateData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kUltimateData.kMaterialKind1.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1_1" ) ) );
		kUltimateData.kMaterialKind1.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1_2" ) ) );
		kUltimateData.kMaterialKind1.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1_3" ) ) );

		if( kUltimateData.kMaterialKind1.dwItemKind1 == NULL_ID || kUltimateData.kMaterialKind1.dwItemKind2 == NULL_ID || kUltimateData.kMaterialKind1.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL1 ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kUltimateData.kMaterialKind1.dwItemKind1, kUltimateData.kMaterialKind1.dwItemKind2, kUltimateData.kMaterialKind1.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kUltimateData.kMaterialKind2.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2_1" ) ) );
		kUltimateData.kMaterialKind2.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2_2" ) ) );
		kUltimateData.kMaterialKind2.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2_3" ) ) );

		if( kUltimateData.kMaterialKind2.dwItemKind1 == NULL_ID || kUltimateData.kMaterialKind2.dwItemKind2 == NULL_ID || kUltimateData.kMaterialKind2.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL2 ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kUltimateData.kMaterialKind2.dwItemKind1, kUltimateData.kMaterialKind2.dwItemKind2, kUltimateData.kMaterialKind2.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}
		kUltimateData.nInitializePenya	= static_cast<int>( kLua.GetFieldToNumber( -1, "nInitializePenya" ) );
		
		

		vecUltimateTransData.push_back( kUltimateData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecUltimateTransData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY GENERAL ENCHANT DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

const T_ULTIMATETRANS_DATA*	FLItemUpgradeBaruna::GetUltimateData( const T_ITEM_SPEC* pItemSpec, const UltimateTransDataVec & vecUltimateTransData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( UltimateTransDataVec::const_iterator pos = vecUltimateTransData.begin(); pos != vecUltimateTransData.end(); ++pos )
	{
		if( pos->dwItemKind1 == pItemSpec->dwItemKind1
			&& ( pos->dwItemKind2 == pItemSpec->dwItemKind2 || pos->dwItemKind2 == IK2_NONE )
			&& ( pos->dwItemKind3 == pItemSpec->dwItemKind3 || pos->dwItemKind3 == IK3_NONE ) )
		{
			return &( *pos );
		}
	}

	return NULL;
}

#endif