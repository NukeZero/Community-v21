
#include "StdAfx.h"
#include "FLItemUpgradeCostume.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeCostume::FLItemUpgradeCostume()
{
}

FLItemUpgradeCostume::~FLItemUpgradeCostume()
{
}

FLItemUpgradeCostume*	FLItemUpgradeCostume::GetInstance()
{
	static FLItemUpgradeCostume	sItemUpgradeCostume;

	return & sItemUpgradeCostume;
}

void	FLItemUpgradeCostume::Clear()
{
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

bool	FLItemUpgradeCostume::LoadScript( const TCHAR* pszFileName )
{
	CLuaBase kLua;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	// Load General Enchant Data
	if( LoadGeneralEnchantData( kLua, m_vecGeneralEnchantData ) == false )
	{
		return false;
	}
#endif

	// Load RandomOption Data
	if( LoadRandomOptionExtensionData( kLua, m_mapDstParameterData, m_vecRandomOptionData ) == false )
	{
		return false;
	}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	Load_GemProbability();
#endif
#ifdef COSTUME_UPGRADE_MIX
	Load_MixProbability();
#endif

	return true;
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
DWORD	FLItemUpgradeCostume::OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemGeneralEnchant( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemGeneralEnchant( pUser, pMsg );
	}

	return dwResult;
}
#endif

DWORD	FLItemUpgradeCostume::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerate( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeCostume::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeCostume::OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksChange( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksChange( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeCostume::OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemLooksInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemLooksInitialize( pUser, pMsg );
	}

	return dwResult;
}

size_t	FLItemUpgradeCostume::GetMaxSizeRandomOption( const DWORD dwItemID ) const
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

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
bool	FLItemUpgradeCostume::CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
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

	//	코스튬이 아닌경우
	if( !pMainProp->IsCostumeEnhanceParts())
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pMainItem->m_dwKeepTime != 0 )
	{
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
		// 변경필요
		/*if( pProtectionProp->dwID != ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) )
		{
			pUser->AddDefinedText( TID_GAME_SMELT_SAFETY_ERROR08 );
			return false;
		}*/

		// 강화 확률 증가 아이템을 사용하지 않습니다.
		/*if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
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
		}*/
	}

	return true;
}
#endif

bool	FLItemUpgradeCostume::CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
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
		pUser->AddDefinedText( TID_GAME_BLESSEDNESS_INVALID_ITEM );
		return false;
	}

	if( pMainItem->GetRandomOptionExtensionSize() > pRandomOptionData->mapSizeProb.size() )
	{
		pUser->AddDefinedText( TID_GAME_BLESSEDNESS_INVALID_ITEM );
		return false;
	}

	if( pRandomOptionData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pRandomOptionData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pRandomOptionData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

bool	FLItemUpgradeCostume::CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	if( pMsg->dwMaterialItemObjID != NULL_ID )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
DWORD	FLItemUpgradeCostume::DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const
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
		// 강화확률 증가 아이템 - 사용안함
		/*if( pMsg->dwProbIncreaseItemObjID != NULL_ID )
		{
			FLItemElem* pProbIncreaseItem	= pUser->m_Inventory.GetAtId( pMsg->dwProbIncreaseItemObjID );

			if( nAbilityOption <= pProbIncreaseItem->GetProp()->nTargetMaxEnchant )
			{
				dwAddSuccessProb	+= pProbIncreaseItem->GetProp()->nEffectValue;

				g_DPSrvr.PutItemLog( pUser, "N", szConText, pProbIncreaseItem );
				pUser->RemoveItem( pProbIncreaseItem->m_dwObjId, 1 );
			}
		}*/
	}
	else
	{
		FLStrcpy( szConText, _countof( szConText ), "UPGRADEITEM" );

		// 보호 두루마리 - 수정필요
		/*if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) ) )
		{
			bDestroyProtect		= true;
			pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) );
		}*/

		// 확률 증가 버프
		//dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GENERAL_ENCHANT_RATE, true );
		//dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GEN_ATT_ENCHANT_RATE, true );
		/*if( pMainItem->GetProp()->dwItemKind2 == IK2_CLOTH )
		{
			dwAddSuccessProb += g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pMainItem, IK3_GENERAL_WEAPON_ENCHANT_RATE, true );
		}*/
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

		// 보석
		if( nAbilityOption > 6 )
			pUser->UpdateItem( (BYTE)pMainItem->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nAbilityOption - 6 );

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

// 코스튬에 보석 합성
DWORD FLItemUpgradeCostume::OnDoUpgradeItemSetGem( FLWSUser* pUser, const FLPacketItemUpgradeSetGemReq* pMsg ) const
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pGemItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwGemItemObjID );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pGemItemElem ) )
		return COSTUME_GEM_CANCEL;

	// 코스튬이 아닐때
	if( !pItemElem->GetProp()->IsCostumeEnhanceParts())
		return COSTUME_GEM_ISNOTULTIMATE;


	// 장착되어 있으면 중단
	if( pUser->m_Inventory.IsEquip( pMsg->dwMainItemObjID ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return COSTUME_GEM_CANCEL;
	}

		
//sun: 12, 무기 피어싱
	size_t nCount=0;
	for( ; nCount < pItemElem->GetUltimatePiercingSize(); nCount++ )
		if( pItemElem->GetUltimatePiercingItemID( nCount ) == 0 )
			break;
	
	// 빈곳이 없으면 중단
//sun: 12, 무기 피어싱
	if( nCount == pItemElem->GetUltimatePiercingSize() )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_GEMSPACE, "" );
		return COSTUME_GEM_CANCEL;
	}

	int nJewelID =ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + pItemElem->GetProp()->dwParts - PARTS_HAT;
	if( pGemItemElem->GetItemIndex() != nJewelID )
		return COSTUME_GEM_CANCEL;

	DWORD dwAblity = GetGemAbilityKindRandom( pItemElem->GetProp()->dwParts );
	
	if( dwAblity == NULL_ID )
		return COSTUME_GEM_CANCEL;
	
	// 보석 삭제
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_PIERCING";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "COSTUME_GEM_PIERCING" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pGemItemElem, 1 );	
	pUser->RemoveItem( pMsg->dwGemItemObjID, 1 );
	
	//int nRandom = xRandom( 1000000 );
	//if( nRandom < m_nSetGemProb )
	{
		// 보석 합성
//sun: 12, 무기 피어싱
		pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( nCount, dwAblity ) );

		//aLogItem.RecvName = "ULTIMATE_PIERCING_SUCCESS";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "COSTUME_GEM_PIERCING_SUCCESS" );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
		return COSTUME_GEM_SUCCESS;	
	}

}

DWORD FLItemUpgradeCostume::OnDoUpgradeItemRemoveGem( FLWSUser* pUser, const FLPacketItemUpgradeRemoveGemReq* pMsg ) const
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pItemElemMat ) )
		return COSTUME_GEM_CANCEL;
	
	if( !pItemElem->GetProp()->IsCostumeEnhanceParts() )
		return COSTUME_GEM_ISNOTULTIMATE;
	
	// 합성된 보석이 없을 때 X
//sun: 12, 무기 피어싱
	if( pItemElem->GetUltimatePiercingItemID( 0 ) == 0 )
		return COSTUME_GEM_CANCEL;

	if( pItemElemMat->m_dwItemId != ITEM_INDEX( 25337, II_GEN_MAT_COSTUME_GEM_CANCEL ) )
		return COSTUME_GEM_CANCEL;
 
	// 장착 되어 있을 때 X
	if( pUser->m_Inventory.IsEquip( pMsg->dwMainItemObjID ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return COSTUME_GEM_CANCEL;
	}	

	// 문스톤 삭제
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "COSTUME_PIERCING_REMOVE" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemMat, 1 );
	pUser->RemoveItem( pMsg->dwMaterialItemObjID, 1 );
	

		// 성공시 - 보석 제거
//sun: 12, 무기 피어싱
	for( int i=pItemElem->GetUltimatePiercingSize()-1; i>=0; i-- )
	{
		if( pItemElem->GetUltimatePiercingItemID( i ) != 0 )
		{
			pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( i, 0 ) );
			//aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_SUC";
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "COSTUME_PIERCING_REMOVE_SUC" );
			g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
			break;
		}
	}


	return COSTUME_GEM_SUCCESS;
}
#endif

#ifdef COSTUME_UPGRADE_MIX
DWORD FLItemUpgradeCostume::OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg, DWORD &dwItemIndex ) const
{
	FLItemElem* pItemElemMat = NULL;

	int nMaterialCount = 0;
	const int MaterialSIZE = 5;

	for( int i = 0; i < MaterialSIZE; i++ )
	{
		if( pMsg->dwMaterialItemObjID[i] != NULL_ID)
			nMaterialCount++;
	}

	if ( nMaterialCount < 3 )
		return FSC_ITEMUPGRADE_UPGRADE_FAIL;

	for( int i = 0; i < MaterialSIZE; i++ )
	{
		if ( pMsg->dwMaterialItemObjID[i] == NULL_ID )
			continue;
		
		pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[i] );
		if( !IsUsableItem( pItemElemMat ) )
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		if( pItemElemMat->m_dwKeepTime != 0 || pItemElemMat->IsBindState())	// 기간제나 귀속아이템일경우
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		if( !pItemElemMat->GetProp()->IsCostumeEnhanceParts() )
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		// 장착 되어 있을 때 X
		if( pUser->m_Inventory.IsEquip( pMsg->dwMaterialItemObjID[i] ) )
		{
			pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		}
		
		int nSameMaterialCount = 0;
		for ( int j = 0; j < MaterialSIZE; j++ )
		{
			if( pMsg->dwMaterialItemObjID[i] == pMsg->dwMaterialItemObjID[j] )
			{
				nSameMaterialCount++;

				if ( nSameMaterialCount == 2 )
					return FSC_ITEMUPGRADE_UPGRADE_FAIL;
			}
		}
	}


	for( int i = 0; i < MaterialSIZE; i++ )
	{
		if ( pMsg->dwMaterialItemObjID[i] == NULL_ID )
			continue;
		pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[i] );

		// 위에서 체크했지만, 한번더 체크합니다. 
		if( !IsUsableItem( pItemElemMat ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FLItemUpgradeCostume::OnDoUpgradeItemMix User : %s ]" ), pUser->GetName() );
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		}
		LogItemInfo aLogItem;
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "COSTUME_MIX_MATERIAL" );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		g_DPSrvr.OnLogItem( aLogItem, pItemElemMat, 1 );
		pUser->RemoveItem( pMsg->dwMaterialItemObjID[i], 1 );
	}

	dwItemIndex = GetMixRandom(nMaterialCount-3);

	FLItemElem CreateItem;
	//CreateItem	= *pMainItem;
	CreateItem.m_dwItemId = dwItemIndex;
	CreateItem.m_nItemNum = 1;
	CreateItem.SetSerialNumber();

	if( pUser->CreateItem( &CreateItem ) )
	{
		return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}

	return FSC_ITEMUPGRADE_UPGRADE_FAIL;
}
#endif


DWORD	FLItemUpgradeCostume::DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const bool bResult = SetRandomOptionExtension( *pMainItem, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		g_DPSrvr.PutItemLog( pUser, "r", "::Blessedness", pMainItem, pMainItem->m_nItemNum );
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

DWORD	FLItemUpgradeCostume::DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	pMainItem->InitializeRandomOptionExtension();

	g_DPSrvr.PutItemLog( pUser, "r", "::BlessednessCancel", pMainItem, pMainItem->m_nItemNum );

	pUser->AddDefinedText( TID_GAME_BLESSEDNESS_CANCEL_INFO );
	
	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM

BOOL FLItemUpgradeCostume::Load_GemProbability()
{
	CScanner s;
	
	const TCHAR szFileName[]	= _T( "Costume_GemProbability.txt" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( !s.Load( szFullPath ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}
	

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PART_ABILITY_PROB" ) )
		{
			__PARTGEMABILITYPROB	PartGemAbilityProb;
			s.GetToken();
			PartGemAbilityProb.nPart = CScript::GetDefineNum(s.Token );
			s.GetToken(); // {
			s.GetToken();
			
			while( *s.token != '}' )
			{
				
				__GEMABILITYPROB GemAbilityProb;
				GemAbilityProb.nAbility = CScript::GetDefineNum( s.Token );	
				GemAbilityProb.nProbability = s.GetNumber();
				PartGemAbilityProb.vecAbilityProb.push_back(GemAbilityProb);
				s.GetToken();
			}

			m_vecPartGemAbilityProb.push_back(PartGemAbilityProb);
		}
		s.GetToken();
	}	
	return TRUE;
}

// 보석 합성시 부여 되는 능력(능력)
DWORD FLItemUpgradeCostume::GetGemAbilityKindRandom( int nPart ) const
{
	if( m_vecPartGemAbilityProb.empty() == TRUE )
		return NULL_ID;

	

	__PARTGEMABILITYPROB PartGemAbilityProb;

	for(DWORD i=0; i<m_vecPartGemAbilityProb.size(); i++)
	{
		if ( m_vecPartGemAbilityProb[i].nPart == nPart )
		{
			PartGemAbilityProb = m_vecPartGemAbilityProb[i];
			break;
		}
	}

	int nRandom = xRandom( 10000 );

	DWORD j=0;
	int oldSum=0,nSum=0;
	for(j=0; j<PartGemAbilityProb.vecAbilityProb.size(); j++ )
	{
		oldSum = nSum;
		nSum += PartGemAbilityProb.vecAbilityProb[j].nProbability;
		if( oldSum <= nRandom && nSum >= nRandom)
			return PartGemAbilityProb.vecAbilityProb[j].nAbility;
	}
	return NULL_ID;

}
#endif

#ifdef COSTUME_UPGRADE_MIX
BOOL FLItemUpgradeCostume::Load_MixProbability()
{
	CScanner s;
	
	const TCHAR szFileName[]	= _T( "Costume_MixProbability.txt" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( !s.Load( szFullPath ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}
	

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "ITEM_PROB" ) )
		{
			s.GetToken(); // {
			s.GetToken();
			
			while( *s.token != '}' )
			{
				
				__MIXPROB	MixProb;
				MixProb.dwItemIndex = CScript::GetDefineNum( s.Token );	
				MixProb.nProbability[0] = s.GetNumber();
				MixProb.nProbability[1] = s.GetNumber();
				MixProb.nProbability[2] = s.GetNumber();
				m_vecMixProb.push_back(MixProb);
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 보석 합성시 부여 되는 능력(능력)
DWORD FLItemUpgradeCostume::GetMixRandom( int nIndex ) const
{
	if( m_vecMixProb.empty() == TRUE )
		return NULL_ID;

	int nRandom = xRandom( 10000 );

	int oldSum=0,nSum=0;
	for(DWORD i=0; i<m_vecMixProb.size(); i++)
	{
		oldSum = nSum;
		nSum += m_vecMixProb[i].nProbability[nIndex];
		if( oldSum <= nRandom && nSum >= nRandom)
			return m_vecMixProb[i].dwItemIndex;
	}
	return NULL_ID;

}
#endif
