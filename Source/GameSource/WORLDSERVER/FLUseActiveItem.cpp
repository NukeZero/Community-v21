#include "StdAfx.h"
#include <Party.h>
extern	CPartyMng			g_PartyMng;



FLUseActiveItem::FLUseActiveItem()
{

}

FLUseActiveItem::~FLUseActiveItem()
{

}

FLUseActiveItem*	FLUseActiveItem::GetInstance()
{
	static FLUseActiveItem xUseActiveItem;

	return & xUseActiveItem;
}

int		FLUseActiveItem::OnDoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	int nResult = DoUseActiveItem( pUser, pSrcItem, pDestItem );

	switch( nResult )
	{
	case ERR_MISMATCH_USE_LEVEL:		// 사용할수 없습니다. 이 아이템은 %s레벨에서만 사용할수 있습니다.
		{
			TCHAR szMessage[10] = { 0 };
			//mem_set( szMessage, 0, sizeof( szMessage ) );
			FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", pSrcItemProp->nMinLimitLevel, pSrcItemProp->nMaxLimitLevel );
			pUser->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", szMessage );
		}
		break;

	case ERR_MISMATCH_ENCHANT_LEVEL:	// 제련 재료의 레벨이 맞지 않습니다.
		{
			pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL );
		}
		break;

	case ERR_DONOT_RESET_BIND:			// 해당 아이템의 속성은 귀속 해제가 불가능한 속성입니다.
		{
			pUser->AddDefinedText( TID_ITEM_CANCEL_ERROR2 );
		}
		break;

	case ERR_NOT_BIND_ITEM:				// 귀속 상태가 아닌 아이템은 귀속 해제를 할 수 없습니다.
		{
			pUser->AddDefinedText( TID_ITEM_CANCEL_ERROR1 );
		}

	case ERR_SUCCESS_RESET_BIND:
	case ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT:
	case ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT_KEEP:
	case ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT:
	case ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT_KEEP:
	case ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROB:
	case ERR_SUCCESS_BARUNA_PIERCING_PROTECT:
	case ERR_SUCCESS_BARUNA_PIERCING_PROB:
		break;

	default:							// 아이템을 사용할 수 없습니다.
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		}
		break;
	}

	return nResult;
}

int		FLUseActiveItem::OnDoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	int nResult = DoUseActiveItem( pUser, pSrcItem );

	switch( nResult )
	{
	case ERR_MISMATCH_USE_LEVEL:		// 사용할수 없습니다. 이 아이템은 %s레벨에서만 사용할수 있습니다.
		{
			TCHAR szMessage[10] = { 0 };
			//mem_set( szMessage, 0, sizeof( szMessage ) );
			FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", pSrcItemProp->nMinLimitLevel, pSrcItemProp->nMaxLimitLevel );
			pUser->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", szMessage );
		}
		break;

	case ERR_SUCCESS_COLOSSEUM_BOSS_RATE:
	case ERR_SUCCESS_COLOSSEUM_RETRY:
		break;

	case ERR_FAIL_COLOSSEUM_RETRY:
		pUser->AddText( "파티가 없거나 파티장이 아니거나 해서 사용 못했다는 텍스트 아이디 필요" );
		break;

	default:							// 아이템을 사용할 수 없습니다.
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		}
		break;
	}

	return nResult;
}

int		FLUseActiveItem::DoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	nResult = CheckActiveItem( pUser, pSrcItem, pDestItem );

	if( nResult == ERR_SUCCESS_USE )
	{
		switch( pSrcItemProp->dwItemKind3 )
		{
		case IK3_RESET_BIND:
			{
				nResult = UseResetBind( pUser, pSrcItem, pDestItem );
			}
			break;

		case IK3_BARUNA_PROTECTION:
		case IK3_BARUNA_PROTECTION_KEEP:
			{
				nResult = UseBarunaGeneralEnchantProtection( pUser, pSrcItem, pDestItem );
			}
			break;

		case IK3_BARUNA_ELE_PROPTECTION:
		case IK3_BARUNA_ELE_PROPTECTION_KEEP:
			{
				nResult = UseBarunaAttributeEnchantProtection( pUser, pSrcItem, pDestItem );
			}
			break;

		case IK3_BARUNA_ELE_PROP_INCREASE:
			{
				nResult = UseBarunaAttributeEnchantProbability( pUser, pSrcItem, pDestItem );
			}
			break;

		case IK3_BARUNA_PROTECTION_RUNE:
			{
				nResult = UseBarunaPiercingProtection( pUser, pSrcItem, pDestItem );
			}
			break;

		case IK3_BARUNA_PIERCE_RATE:
			{
				nResult = UseBarunaPiercingProb( pUser, pSrcItem, pDestItem );
			}
			break;

		default:
			break;
		}
	}

	return nResult;
}

int		FLUseActiveItem::DoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	nResult = CheckActiveItem( pUser, pSrcItem );

	if( nResult == ERR_SUCCESS_USE )
	{
		switch( pSrcItemProp->dwItemKind3 )
		{
		case IK3_COLOSSEUM_RETRY:
			{
				nResult = UseColosseumRetry( pUser, pSrcItem );
			}
			break;

		case IK3_COLOSSEUM_BOSS_RATE:
			{
				nResult = UseColosseumBossRate( pUser, pSrcItem );
			}
			break;

		default:
			break;
		}
	}

	return nResult;
}

int		FLUseActiveItem::CheckActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() == TRUE )
	{
		return ERR_DONOT_USE_STATE;
	}

	if( pUser->m_Inventory.IsEquip( pDestItem->m_dwObjId ) == TRUE )
	{
		return ERR_DONOT_USE_STATE;
	}

	if( pUser->GetLevel() < pSrcItemProp->nMinLimitLevel || pUser->GetLevel() > pSrcItemProp->nMaxLimitLevel )
	{
		return ERR_MISMATCH_USE_LEVEL;
	}

	if( pSrcItemProp->nUseLimitGroup > 0 )		// 사용 제한 등급이 있는 경우
	{
		if( pUser->HasUseLimitGroupItemBuff( pSrcItemProp->nUseLimitGroup ) == true )
		{
			return ERR_DONOTUSE_ANOTHER;
		}
	}

	return ERR_SUCCESS_USE;
}

int		FLUseActiveItem::CheckActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() == TRUE )
	{
		return ERR_DONOT_USE_STATE;
	}

	if( pUser->m_Inventory.IsEquip( pSrcItem->m_dwObjId ) == TRUE )
	{
		return ERR_DONOT_USE_STATE;
	}

	if( pUser->GetLevel() < pSrcItemProp->nMinLimitLevel || pUser->GetLevel() > pSrcItemProp->nMaxLimitLevel )
	{
		return ERR_MISMATCH_USE_LEVEL;
	}

	if( pSrcItemProp->nUseLimitGroup > 0 )		// 사용 제한 등급이 있는 경우
	{
		if( pUser->HasUseLimitGroupItemBuff( pSrcItemProp->nUseLimitGroup ) == true )
		{
			return ERR_DONOTUSE_ANOTHER;
		}
	}

	return ERR_SUCCESS_USE;
}

int		FLUseActiveItem::UseColosseumRetry( FLWSUser* pUser, FLItemElem* pSrcItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty != NULL && pParty->GetLeader() != NULL && pParty->GetLeader()->m_idPlayer == pUser->m_idPlayer )
	{
		nResult = ERR_SUCCESS_COLOSSEUM_RETRY;
	}
	else
	{
		nResult = ERR_FAIL_COLOSSEUM_RETRY;
	}

	return nResult;
}

int		FLUseActiveItem::UseColosseumBossRate( FLWSUser* pUser, FLItemElem* pSrcItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	CParty* pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty != NULL && pParty->GetLeader() != NULL && pParty->GetLeader()->m_idPlayer == pUser->m_idPlayer )
	{
		CWorld* pLeaderWorld = pUser->GetWorld();
		if( pLeaderWorld != NULL && CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pLeaderWorld->GetID() ) == TRUE )
		{
			if( CInstanceDungeonHelper::GetInstance()->SetAdditinalRealMonsterRate( pUser, pSrcItemProp->nEffectValue ) == TRUE )
			{
				//////////////////////////////////////////////////////////////////////////
				// BEGIN110502 진보스 출현 확률 증가 주문서 버프를 띄워줍시다.. 같은 콜로세움 안에 있는 단원들에게만..
				for( int nTh = 0; nTh < pParty->GetSizeofMember(); ++nTh )
				{
					u_long uPartyMemberId	= pParty->GetPlayerId( nTh );
					FLWSUser* pPartyMember	= FLWSUserManager::GetInstance()->GetUserByPlayerID( uPartyMemberId );

					if( IsValidObj( pPartyMember ) == TRUE )
					{
						CWorld* pPartyMemberWorld = pPartyMember->GetWorld();
						if( pPartyMemberWorld != NULL && pPartyMemberWorld->GetID() == pLeaderWorld->GetID() )
						{
							g_xApplyItemEffect->DoApplyEffect( static_cast<CMover*>( pPartyMember ), static_cast<CMover*>( pPartyMember ), pSrcItemProp );
						}
					}
				}
				// END110502
				//////////////////////////////////////////////////////////////////////////

				nResult = ERR_SUCCESS_COLOSSEUM_BOSS_RATE;
			}
			else
			{
				nResult = ERR_FAIL_COLOSSEUM_BOSS_RATE;
			}
		}
		else
		{
			nResult = ERR_FAIL_COLOSSEUM_BOSS_RATE;
		}
	}
	else
	{
		nResult = ERR_FAIL_COLOSSEUM_BOSS_RATE;
	}

	return nResult;
}

int		FLUseActiveItem::UseResetBind( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItem->IsFlag( FLItemElem::binds ) == TRUE
		|| ( pDestItemProp->nBindCondition == BIND_CONDITION_GET && pDestItem->IsFlag( FLItemElem::reset_bind ) == FALSE ) )
	{
		if( pDestItemProp->bResetBind == TRUE )
		{
			pDestItem->SetFlag( FLItemElem::reset_bind );
			pDestItem->ResetFlag( FLItemElem::binds );
			pUser->UpdateItem( pDestItem->m_dwObjId, UI_FLAG, MAKELONG( pDestItem->m_dwObjIndex, pDestItem->m_byFlag ) );

			g_DPSrvr.PutItemLog( pUser, "C", "RESET_BIND", pDestItem );

			nResult = ERR_SUCCESS_RESET_BIND;
		}
		else
		{
			nResult = ERR_DONOT_RESET_BIND;
		}
	}
	else
	{
		nResult = ERR_NOT_BIND_ITEM;
	}

	return nResult;
}

int		FLUseActiveItem::UseBarunaGeneralEnchantProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp	= pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp	= pDestItem->GetProp();

	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItemProp->IsBaruna() == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	if( pSrcItemProp->nTargetMinEnchant > pDestItem->GetAbilityOption()
		|| pSrcItemProp->nTargetMaxEnchant < pDestItem->GetAbilityOption() )
	{
		return ERR_MISMATCH_ENCHANT_LEVEL;
	}

	if( pSrcItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION )
	{
		nResult = ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT;
	}
	else if( pSrcItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION_KEEP )
	{
		nResult = ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT_KEEP;
	}

	return nResult;
}

int		FLUseActiveItem::UseBarunaAttributeEnchantProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp	= pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp	= pDestItem->GetProp();

	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItemProp->IsBaruna() == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	if( pSrcItemProp->nTargetMinEnchant > pDestItem->GetResistAbilityOption()
		|| pSrcItemProp->nTargetMaxEnchant < pDestItem->GetResistAbilityOption() )
	{
		return ERR_MISMATCH_ENCHANT_LEVEL;
	}

	if( pSrcItemProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION )
	{
		nResult = ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT;
	}
	else if( pSrcItemProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION_KEEP )
	{
		nResult = ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT_KEEP;
	}

	return nResult;
}

int		FLUseActiveItem::UseBarunaAttributeEnchantProbability( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();

	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItemProp->IsBaruna() == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	if( pSrcItemProp->nTargetMinEnchant > pDestItem->GetResistAbilityOption()
		|| pSrcItemProp->nTargetMaxEnchant < pDestItem->GetResistAbilityOption() )
	{
		return ERR_MISMATCH_ENCHANT_LEVEL;
	}

	if( pSrcItemProp->dwItemKind2 == IK2_BARUNA && pSrcItemProp->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE )
	{
		nResult = ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROB;
	}

	return nResult;
}

int		FLUseActiveItem::UseBarunaPiercingProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();

	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItemProp->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE || pDestItemProp->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE )
	{
		nResult = ERR_SUCCESS_BARUNA_PIERCING_PROTECT;
	}

	return nResult;
}

int		FLUseActiveItem::UseBarunaPiercingProb( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pSrcItem ) == FALSE || IsUsableItem( pDestItem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pSrcItemProp = pSrcItem->GetProp();
	PT_ITEM_SPEC pDestItemProp = pDestItem->GetProp();

	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pDestItemProp->IsBaruna() == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	nResult = ERR_SUCCESS_BARUNA_PIERCING_PROB;

	return nResult;
}
