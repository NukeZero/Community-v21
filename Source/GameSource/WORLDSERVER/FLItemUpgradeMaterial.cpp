#include "StdAfx.h"
#include "FLItemUpgradeMaterial.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"

FLItemUpgradeMaterial::FLItemUpgradeMaterial(void)
{
}

FLItemUpgradeMaterial::~FLItemUpgradeMaterial(void)
{
}

FLItemUpgradeMaterial*	FLItemUpgradeMaterial::GetInstance()
{
	static FLItemUpgradeMaterial	sItemUpgradeMaterial;

	return & sItemUpgradeMaterial;
}

#ifdef COSTUME_UPGRADE_MIX
DWORD FLItemUpgradeMaterial::OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg, DWORD &dwItemIndex ) const
{

	FLItemElem* pItemElemMat;

	int nMaterialCount = 0;
	int nTotalCount = 0;

	const int MaterialSIZE = 5;

	std::map<DWORD,int> map;
	std::map<DWORD,int>::iterator iter;
	

	for( int i = 0; i < MaterialSIZE; i++ )
	{
		if( pMsg->dwMaterialItemObjID[i] != NULL_ID)
			nMaterialCount++;

		DWORD dwMaterialID = pMsg->dwMaterialItemObjID[i];

		pItemElemMat	= pUser->m_Inventory.GetAtId( dwMaterialID );
		if( !IsUsableItem( pItemElemMat ) )
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;

		iter = map.find(dwMaterialID);	// 아이템 아이디로 검색
		if(iter == map.end())
		{
			map[dwMaterialID] = pItemElemMat->m_nItemNum-1;
			nTotalCount += pItemElemMat->m_nItemNum;
		}
		else
		{
			map[dwMaterialID]--;
			if( map[dwMaterialID] < 0 )
			{
				return FSC_ITEMUPGRADE_UPGRADE_FAIL;
			}
		}
		
	}

	

	if ( nMaterialCount != MaterialSIZE || nTotalCount < MaterialSIZE)
		return FSC_ITEMUPGRADE_UPGRADE_FAIL;

	
	int nMaterialIndex = 0;
	for( int i = 0; i < MaterialSIZE; i++ )
	{
		pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[i] );
		
		if( nMaterialIndex == 0 )
			nMaterialIndex = pItemElemMat->GetItemIndex();

		if( nMaterialIndex != pItemElemMat->GetItemIndex() )
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		if( pItemElemMat->m_dwKeepTime != 0 || pItemElemMat->IsBindState())	// 기간제나 귀속아이템일경우
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		//if( !pItemElemMat->GetProp()->IsCostumeEnhanceParts() )
		//	return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		if( nMaterialIndex < ITEM_INDEX( 25435, II_SYS_SYS_SCR_ENCHANT_ABSOLUTE06 ) || nMaterialIndex >= ITEM_INDEX( 25439, II_SYS_SYS_SCR_ENCHANT_ABSOLUTE10 ))
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		// 장착 되어 있을 때 X
		if( pUser->m_Inventory.IsEquip( pMsg->dwMaterialItemObjID[i] ) )
		{
			pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		}
	}

	

	pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[0] );
	dwItemIndex =  pItemElemMat->GetProp()->dwReferTarget1;

	// 인벤토리가 부족할 때 - 취소
	FLItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = dwItemIndex;
	itemElemTemp.m_nItemNum = 1;
	PT_ITEM_SPEC pItemprop = itemElemTemp.GetProp();
	if( !pItemprop )
	{
		return FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	if( pUser->m_Inventory.IsFull( &itemElemTemp, itemElemTemp.m_nItemNum ) == TRUE )
	{
		return FSC_ITEMUPGRADE_UPGRADE_FAIL;
	}

	for( int i = 0; i < MaterialSIZE; i++ )
	{
		pItemElemMat	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID[i] );
		if( !IsUsableItem( pItemElemMat ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FLItemUpgradeMaterial::OnDoUpgradeItemMix User : %s ]" ), pUser->GetName() );
			return FSC_ITEMUPGRADE_UPGRADE_FAIL;
		}
		LogItemInfo aLogItem;
		FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "MATERIAL_MIX_MATERIAL" );
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		g_DPSrvr.OnLogItem( aLogItem, pItemElemMat, 1 );
		pUser->RemoveItem( pMsg->dwMaterialItemObjID[i], 1 );
	}


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