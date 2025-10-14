
#include "StdAfx.h"

#include "../_Common/party.h"
extern CPartyMng	g_PartyMng;


FLUseEffectItem::FLUseEffectItem()
{

}

FLUseEffectItem::~FLUseEffectItem()
{

}

FLUseEffectItem*	FLUseEffectItem::GetInstance()
{
	static FLUseEffectItem xUseEffectItem;

	return & xUseEffectItem;
}

int		FLUseEffectItem::OnDoUseEffectItem( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	int nResult = DoUseEffectItem( pUser, pItemElem );

	switch( nResult )
	{
	case ERR_SUCCESS_USE:
		{
			if( pItemProp->dwItemKind3 == IK3_EXP_RATE )	// 경험치 증가 아이템이면 증가폭 출력.
			{
				TCHAR szMessage[256] = { 0 };
				//mem_set( szMessage, 0, sizeof( szMessage ) );
				int nPercent = pUser->GetAdjValueDuplicationBuff( pItemProp->dwItemKind3 );
				FLSPrintf( szMessage, _countof( szMessage ), "%s %d%% %s",
					prj.GetText( TID_GAME_EXPITEM_TOOLTIP ), nPercent, prj.GetText( TID_GAME_EXPITEM_TOOLTIP1 ) );

				pUser->AddText( szMessage );
			}
			else if( pItemProp->dwItemKind3 == IK3_GENERAL_WEAPON_ENCHANT_RATE )
			{
				TCHAR szMessage[10] = { 0 };
				//mem_set( szMessage, 0, sizeof( szMessage ) );
				if( pItemProp->nTargetMinEnchant != pItemProp->nTargetMaxEnchant )
				{
					FLSPrintf( szMessage, _countof( szMessage ), "+%d ~ +%d", pItemProp->nTargetMinEnchant, pItemProp->nTargetMaxEnchant );
				}
				else
				{
					FLSPrintf( szMessage, _countof( szMessage ), "+%d", pItemProp->nTargetMaxEnchant );
				}
				pUser->AddDefinedText( TID_SMELTING_NOTICE, "\"%s\"", szMessage );
			}
			else if( pItemProp->dwItemKind3 == IK3_COSTUME_DEFENDER )
			{
				pUser->AddDefinedText( TID_MMI_COSTUME_SAFEMIXITEM02 );
			}
			else if( pItemProp->dwItemKind3 == IK3_TRADEREGISTERITEM )
			{
				pUser->AddDefinedText( TID_MMI_TRADEREGISTERITEM );
			}
			else if( pItemProp->dwItemKind3 == IK3_PARTYPOINTFREE && pUser->m_idparty > 0 )
			{
				g_PartyMng.NotiSetPartyEffectModeToCS( pUser->m_idPlayer, pUser->m_idparty, PARTY_EFFECT_POINT_LIMITLESS, true );
			}
		}
		break;

	case ERR_ALEADY_USE:			// 이미 사용 중인 동일 아이템이 있어 사용이 불가능합니다. 아이템 만료 후에 사용하십시오.
		{
			pUser->AddDefinedText( TID_GAME_LIMITED_USE );
		}
		break;

	case ERR_DONOTUSE_ANOTHER:	// 이 아이템과 중복해서 사용할 수 없습니다.
		{
			if( pItemProp->nMaxDuplication > 1 )
			{
				pUser->AddDefinedText( TID_GAME_NOTOVERLAP_USE );
			}
			else
			{
				pUser->AddDefinedText( TID_SMELTING_ERROR1 );
			}
		}
		break;

	case ERR_MISMATCH_USE_LEVEL:		// 사용할수 없습니다. 이 아이템은 %s레벨에서만 사용할수 있습니다.
		{
			TCHAR szMessage[10] = { 0 };
			//mem_set( szMessage, 0, sizeof( szMessage ) );
			FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", pItemProp->nMinLimitLevel, pItemProp->nMaxLimitLevel );
			pUser->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", szMessage );
		}
		break;

	case ERR_MAX_OVER:			// 중복할수 없습니다. 이 아이템은 %s레벨에선 %s개까지 중복가능 합니다.
		{
			TCHAR szMessage[10] = { 0 };
			//mem_set( szMessage, 0, sizeof( szMessage ) );
			FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", pItemProp->nMinLimitLevel, pItemProp->nMaxLimitLevel );
			TCHAR szMaxCount[4] = {0, };
			FLSPrintf( szMaxCount, _countof( szMaxCount ), "%d", pItemProp->nMaxDuplication );
			pUser->AddDefinedText( TID_GAME_EXP_NOTUSE, "\"%s\" \"%s\"", szMessage, szMaxCount );
		}
		break;

	case ERR_NOT_PK_ENABLE_CHANNEL:
		{
			pUser->AddDefinedText( TID_MMI_PKPENALTY_TEXT02 );
		}
		break;

	case ERR_NOT_CHAO_PLAYER:
		{
			pUser->AddDefinedText( TID_MMI_PKPENALTY_TEXT01 );
		}
		break;

	default:					// 아이템을 사용할 수 없습니다.
		{
			pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		}
		break;
	}

	return nResult;
}

int		FLUseEffectItem::DoUseEffectItem( FLWSUser* pUser, FLItemElem* pItemElem )
{
	int nResult = ERR_GENERAL_ERROR;

	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return ERR_ITEM_INVALID;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	nResult = CheckEffectItem( pUser, pItemProp );

	if( nResult == ERR_SUCCESS_USE )
	{
		g_xApplyItemEffect->DoApplyEffect( pUser, pUser, pItemProp );
	}

	return nResult;
}

int		FLUseEffectItem::CheckEffectItem( FLWSUser* pUser, PT_ITEM_SPEC pItemProp )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( pItemProp == NULL )
	{
		return ERR_ITEM_INVALID;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() == TRUE )
	{
		return ERR_DONOT_USE_STATE;
	}

	if( pUser->GetLevel() < pItemProp->nMinLimitLevel || pUser->GetLevel() > pItemProp->nMaxLimitLevel )
	{
		return ERR_MISMATCH_USE_LEVEL;
	}

	if( pItemProp->nUseLimitGroup > 0 )		// 사용 제한 등급이 있는 경우
	{
		if( pUser->HasUseLimitGroupItemBuff( pItemProp->nUseLimitGroup ) == true )
		{
			return ERR_DONOTUSE_ANOTHER;
		}
	}

	if( pItemProp->nMaxDuplication > 1 )	// 중첩이 되는 경우
	{
		int nMaxNum = GetLimitCountUseMax( pItemProp );
		if( pItemProp->nMaxDuplication <= pUser->GetCountBuffByItemId( pItemProp->dwID )
			|| nMaxNum <= pUser->GetCountBuffByItemGroup( pItemProp->nItemGroup ) )
		{
			return ERR_MAX_OVER;
		}
	}
	else
	{
		if( pUser->HasBuff( BUFF_ITEM, (WORD)pItemProp->dwID ) == TRUE )	// hm...
		{
			return ERR_ALEADY_USE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( pItemProp->dwItemKind3 == IK3_PKPENALTYSCROLL_A || pItemProp->dwItemKind3 == IK3_PKPENALTYSCROLL_C )
	{
		if( g_eLocal.GetState( EVE_PK ) == 0 )
		{
			return ERR_NOT_PK_ENABLE_CHANNEL;
		}

		if( pItemProp->dwItemKind3 == IK3_PKPENALTYSCROLL_C && pUser->IsChaotic() == FALSE )
		{
			return ERR_NOT_CHAO_PLAYER;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	return ERR_SUCCESS_USE;
}

int		FLUseEffectItem::GetLimitCountUseMax( PT_ITEM_SPEC pItemProp )
{
	int nLimitCount = 0;

	if( pItemProp != NULL )
	{
		if( pItemProp->dwItemKind3 == IK3_EXP_RATE )
		{
			nLimitCount = _MAX_ES_SCROLL_NUM;
		}
		else
		{
			nLimitCount = pItemProp->nMaxDuplication;
		}
	}

	return nLimitCount;
}

