#include "StdAfx.h"
#include "FLItemUsingActiveUI.h"

#include "FLChargeZoneTicket.h"
#include "FLTreasureChest.h"
#include "FLCharacterServerTransform.h"


FLItemUsingActiveUI::FLItemUsingActiveUI()
{

}

FLItemUsingActiveUI::~FLItemUsingActiveUI()
{

}

FLItemUsingActiveUI*	FLItemUsingActiveUI::GetInstance()
{
	static FLItemUsingActiveUI	kItemUsingActiveUI;

	return & kItemUsingActiveUI;
}

void	FLItemUsingActiveUI::OnDoUseActiveUIItem( FLWSUser* pUser, DWORD dwItemObjID )
{
	if( IsValidObj( pUser ) != FALSE )
	{
		FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwItemObjID );
		if( IsUsableItem( pItemElem ) != FALSE )
		{
			PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
			if( pItemSpec != NULL )
			{
				if( pItemSpec->dwItemKind3 == IK3_TICKET )
				{
					g_pChargeZoneTicket->OnDoUseChargeZoneTicket( pUser, dwItemObjID );
				}
				else if( pItemSpec->dwItemKind2 == IK2_TREASURE )
				{
					g_pTreasureChest->OnDoUseTreasureChest( pUser, dwItemObjID );
				}
				else if( pItemSpec->dwItemKind3 == IK3_SERVERMOVE )
				{
					g_pCharacterServerTransform->OnDoUseCharacterServerTransform( pUser, dwItemObjID );
				}
			}
		}
	}
}
