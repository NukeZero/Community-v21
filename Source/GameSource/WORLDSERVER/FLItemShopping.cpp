#include "StdAfx.h"


#include "FLCC_Reward.h"

#include "GlobalGameValueCalculator.h"

FLItemShopping::FLItemShopping()
{

}

FLItemShopping::~FLItemShopping()
{

}

FLItemShopping* FLItemShopping::GetInstance()
{
	static FLItemShopping xItemShopping;

	return	&xItemShopping;
}

int		FLItemShopping::BuyItemByBasket( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket )
{
	int nResult = ERR_GENERAL_ERROR;

	if( ptShoppingBasket != NULL )
	{
		if( ptShoppingBasket->nVendorKind == VENDOR_KIND_NPC )
		{
			nResult = BuyItemFromNPC( pUser, ptShoppingBasket );
		}
		else if( ptShoppingBasket->nVendorKind == VENDOR_KIND_PC )
		{
			nResult = BuyItemFromPC( pUser, ptShoppingBasket );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "nVendorKind is error. [%d]" ), ptShoppingBasket->nVendorKind );
			nResult = ERR_VENDOR_INVALID;
		}
	}
	else
	{
		nResult = ERR_SHOPPING_INFO_INVALID;
	}

	return nResult;
}

int		FLItemShopping::BuyItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket )
{
	int nResult = ERR_GENERAL_ERROR;

	nResult = CheckBuyItemFromNPC( pUser, ptShoppingBasket );
	if( nResult == ERR_SUCCESS )
	{
		nResult = CreateItemFromNPC( pUser, ptShoppingBasket );
		if( nResult != ERR_SUCCESS )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] FLItemShopping::BuyItemFromNPC..." ) );
		}
	}

	return nResult;
}

int		FLItemShopping::CheckBuyItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket )
{
//	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE || pUser->IsDie() == TRUE )
	{
		return ERR_USER_INVALID;
	}

	if( ptShoppingBasket == NULL )
	{
		return ERR_SHOPPING_INFO_INVALID;
	}

	if( ptShoppingBasket->nVendorNPCType != VENDOR_TYPE_PENYA && ptShoppingBasket->nVendorNPCType != VENDOR_TYPE_CHIP )
	{
		return ERR_SHOPPING_INFO_INVALID;
	}

	CMover* pVendor = pUser->m_vtInfo.GetOther();
	if( pVendor == NULL )
	{
		return ERR_VENDOR_INVALID;
	}

	LPCHARACTER pNpc = prj.GetCharacter( pVendor->m_szCharacterKey );
	if( pNpc == NULL || pVendor->IsNPC() == FALSE )
	{
		return ERR_VENDOR_INVALID;
	}

	if( pNpc->m_nVenderType != ptShoppingBasket->nVendorNPCType )
	{
		return ERR_VENDOR_INVALID;
	}

	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
	{
		return ERR_VENDOR_INVALID;
	}

	CItemContainer< FLItemElem > tmpInventory;
	tmpInventory.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_INVENTORY, MAX_INVENTORY, MAX_HUMAN_PARTS );
	tmpInventory.Copy( pUser->m_Inventory );

	__int64 n64SumCost	= 0;
	int nHaveChipNum	= pUser->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) );
	
	for( int i = 0; i < _MAX_SHOPPING_BASKET; ++i )
	{
		int		nShopTabId			= ptShoppingBasket->tBuyItemInfo[i].nShopTabId;
		DWORD	dwObjId				= ptShoppingBasket->tBuyItemInfo[i].dwObjId;
		DWORD	dwItemId			= ptShoppingBasket->tBuyItemInfo[i].dwItemId;
		int		nItemNum			= ptShoppingBasket->tBuyItemInfo[i].nItemNum;

		if( dwItemId == 0 && nItemNum == 0 )
		{
			break;
		}

		if( nShopTabId < 0 || nShopTabId >= MAX_VENDOR_INVENTORY_TAB )
		{
			return ERR_VENDOR_INVALID;
		}

		FLItemElem* pItemElem = pVendor->m_ShopInventory[ nShopTabId ]->GetAtId( dwObjId );
		if( pItemElem == NULL )
		{
			return ERR_ITEM_INVALID;
		}

		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
		if( pItemProp == NULL )
		{
			return ERR_ITEM_INVALID;
		}

		if( pItemElem->m_dwItemId != dwItemId || nItemNum <= 0 || pItemElem->m_nItemNum < nItemNum || (int)pItemProp->dwPackMax < nItemNum )
		{
			return ERR_ITEM_INVALID;
		}

		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) == TRUE && CTax::GetInstance()->IsOccupationGuildMember( pUser ) == FALSE )
		{
			return ERR_BUY_ONLY_OCCUPATION;
		}

		FLItemElem itemElem;
		itemElem.m_dwItemId = dwItemId;
		itemElem.m_nItemNum = nItemNum;
		itemElem.SetSerialNumber( static_cast< SERIALNUMBER >( CSerialNumber::SN_IGNORE_SERIAL_NUMBER ) );

		if( tmpInventory.Add( &itemElem ) == FALSE )
		{
			return ERR_LACK_SPACE;
		}

		if( pNpc->m_nVenderType == VENDOR_TYPE_PENYA )
		{
			//__int64 n64Cost = ( __int64 )( pItemElem->GetCost() * FLGetGlobalBuyFactor() );
			
			__int64 n64Cost	= pItemElem->GetCost();
			if( FLGetGlobalBuyFactor( n64Cost ) == FALSE )
			{
				return ERR_SUMCOST_OVERFLOW;
			}
			
			if( pItemElem->IsPerin() == TRUE )
			{
				n64Cost = PERIN_VALUE;
			}

			//if( n64Cost <= 0 || n64Cost > INT_MAX )
			//{
			//	return ERR_SUMCOST_OVERFLOW;
			//}

			__int64 n64Tax = 0;
			if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pVendor, pItemElem ) )
			{
				n64Tax = (__int64)( (float)n64Cost * CTax::GetInstance()->GetPurchaseTaxRate( pVendor ) );
			}
			n64Tax *= nItemNum;

			if( n64Tax < 0 || n64Tax > INT_MAX )
			{
				return ERR_SUMCOST_OVERFLOW;
			}

			n64Cost *= nItemNum;
			n64Cost += n64Tax;

			if( n64Cost <= 0 || n64Cost > INT_MAX )
			{
				return ERR_SUMCOST_OVERFLOW;
			}

			n64SumCost += n64Cost;
			if( n64SumCost <= 0 || n64SumCost > INT_MAX )
			{
				return ERR_SUMCOST_OVERFLOW;
			}
			else
			{
				if( pUser->CheckUserGold( (int)n64SumCost, false ) == false )
				{
					return ERR_LACK_MONEY;
				}
			}
		}
		else if( pNpc->m_nVenderType == VENDOR_TYPE_CHIP )
		{
			DWORD dwChipCost = pItemElem->GetChipCost();
			if( dwChipCost != NULL_ID )
			{
				dwChipCost *= nItemNum;
				n64SumCost += dwChipCost;
				if( n64SumCost <= 0 || n64SumCost > INT_MAX )
				{
					return ERR_SUMCHIP_OVERFLOW;
				}
				else
				{
					if( nHaveChipNum < (int)n64SumCost )
					{
						return ERR_LACK_CHIP;
					}
				}
			}
			else
			{
				return ERR_ITEM_INVALID;
			}
		}
		else
		{
			return ERR_VENDOR_INVALID;
		}
	}

	return ERR_SUCCESS;
}

int		FLItemShopping::CreateItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		if( ptShoppingBasket != NULL )
		{
			CMover* pVendor = pUser->m_vtInfo.GetOther();
			if( pVendor != NULL )
			{
				LPCHARACTER pNpc = prj.GetCharacter( pVendor->m_szCharacterKey );
				if( pNpc == NULL || pVendor->IsNPC() == FALSE )
				{
					return ERR_VENDOR_INVALID;
				}

				nResult = ERR_SUCCESS;

				for( int i = 0; i < _MAX_SHOPPING_BASKET; ++i )
				{
					int		nShopTabId			= ptShoppingBasket->tBuyItemInfo[i].nShopTabId;
					DWORD	dwObjId				= ptShoppingBasket->tBuyItemInfo[i].dwObjId;
					DWORD	dwItemId			= ptShoppingBasket->tBuyItemInfo[i].dwItemId;
					int		nItemNum			= ptShoppingBasket->tBuyItemInfo[i].nItemNum;

					if( dwItemId == 0 && nItemNum == 0 )
					{
						break;
					}

					FLItemElem* pItemElem = pVendor->m_ShopInventory[ nShopTabId ]->GetAtId( dwObjId );
					if( pItemElem == NULL )
					{
						return ERR_ITEM_INVALID;
					}

					if( pNpc->m_nVenderType == VENDOR_TYPE_PENYA )
					{
						__int64 n64Cost	= pItemElem->GetCost();
						if( FLGetGlobalBuyFactor( n64Cost ) == FALSE )
						{
							return ERR_SUMCOST_OVERFLOW;
						}

						//__int64 n64Cost = ( __int64 )( pItemElem->GetCost() * FLGetGlobalBuyFactor() );
						//__int64 n64Cost = (__int64)pItemElem->GetCost();
						//n64Cost = (__int64)( n64Cost * prj.m_fShopCost );
						////n64Cost = (__int64)( (float)n64Cost * ( prj.m_EventLua.GetShopBuyFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue() ) );
						//const float fMulRate = ( prj.m_EventLua.GetShopBuyFactor() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue() - 1.0f ) + 1.0f;
						//n64Cost	= ( __int64 )( n64Cost * fMulRate );

						if( pItemElem->IsPerin() == TRUE )
						{
							n64Cost = PERIN_VALUE;
						}

						/*if( n64Cost <= 0 || n64Cost > INT_MAX )
						{
							return ERR_SUMCOST_OVERFLOW;
						}*/

						__int64 n64Tax = 0;
						if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pVendor, pItemElem ) )
						{
							n64Tax = (__int64)( (float)n64Cost * CTax::GetInstance()->GetPurchaseTaxRate( pVendor ) );
						}
						n64Tax *= nItemNum;

						if( n64Tax < 0 || n64Tax > INT_MAX )
						{
							return ERR_SUMCOST_OVERFLOW;
						}

						n64Cost *= nItemNum;
						n64Cost += n64Tax;

						if( n64Cost <= 0 || n64Cost > INT_MAX )
						{
							return ERR_SUMCOST_OVERFLOW;
						}

						if( pUser->CheckUserGold( (int)n64Cost, false ) == false )
						{
							return ERR_LACK_MONEY;
						}

						FLItemElem itemElem;
						itemElem.m_dwItemId = dwItemId;
						itemElem.m_nItemNum = nItemNum;
						itemElem.SetSerialNumber();

						if( pUser->CreateItem( &itemElem ) == TRUE )
						{
							LogItemInfo aLogItem;
							//aLogItem.Action = "B";
							//aLogItem.SendName = pUser->GetName();
							//aLogItem.RecvName = pVendor->GetName();

							FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "B" );
							FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
							FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pVendor->GetName() );


							aLogItem.WorldId = pUser->GetWorld()->GetID();
							aLogItem.Gold = pUser->GetGold();
							aLogItem.Gold2 = pUser->GetGold() - (int)n64Cost;
							aLogItem.Gold_1 = pVendor->GetGold();
							g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );

							pUser->AddGold( (int)(-n64Cost) );

							if( (int)n64Tax > 0 )
							{
								CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), (int)n64Tax, TAX_PURCHASE );
							}
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "Create Item Failed. ItemId:[%d], Num:[%d]" ), dwItemId, nItemNum );
						}
					}
					else if( pNpc->m_nVenderType == VENDOR_TYPE_CHIP )
					{
						DWORD dwChipCost = pItemElem->GetChipCost();
						if( dwChipCost != NULL_ID )
						{
							dwChipCost *= nItemNum;
							if( dwChipCost > INT_MAX )
							{
								return ERR_SUMCHIP_OVERFLOW;
							}
							else
							{
								if( pUser->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) < (int)dwChipCost )
								{
									return ERR_LACK_CHIP;
								}
							}
						}
						else
						{
							return ERR_ITEM_INVALID;
						}

						FLItemElem itemElem;
						itemElem.m_dwItemId = dwItemId;
						itemElem.m_nItemNum = nItemNum;
						itemElem.SetSerialNumber();

						if( pUser->CreateItem( &itemElem ) == TRUE )
						{
							pUser->RemoveItemA( ITEM_INDEX( 26460, II_CHP_RED ), static_cast<int>( dwChipCost ) );

							LogItemInfo aLogItem;
							//aLogItem.Action = "B";
							//aLogItem.SendName = pUser->GetName();
							//TCHAR szRecvName[128] = {0, };
							//FLSPrintf( szRecvName, _countof( szRecvName ), "%s_C", pVendor->GetName() );
							//aLogItem.RecvName = (LPCTSTR)szRecvName;
							FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "B" );
							FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
							TCHAR szRecvName[128] = {0, };
							FLSPrintf( szRecvName, _countof( szRecvName ), "%s_C", pVendor->GetName() );
							FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)szRecvName );

							aLogItem.WorldId = pUser->GetWorld()->GetID();
							aLogItem.Gold = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) ) + itemElem.GetChipCost() * itemElem.m_nItemNum;
							aLogItem.Gold2 = pUser->GetItemNum( ITEM_INDEX( 26460, II_CHP_RED ) );
							aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * itemElem.m_nItemNum) ) );
							g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "Create Item Failed. ItemId:[%d], Num:[%d]" ), dwItemId, nItemNum );
						}
					}
				}
			}
			else
			{
				nResult = ERR_VENDOR_INVALID;
			}
		}
		else
		{
			nResult = ERR_SHOPPING_INFO_INVALID;
		}
	}
	else
	{
		nResult = ERR_USER_INVALID;
	}

	return nResult;
}

int		FLItemShopping::BuyItemFromPC( FLWSUser* /*pUser*/, const PT_PACKET_SHOPPING_BASKET /*ptShoppingBasket*/ )
{
	int nResult = ERR_GENERAL_ERROR;

	return nResult;
}

