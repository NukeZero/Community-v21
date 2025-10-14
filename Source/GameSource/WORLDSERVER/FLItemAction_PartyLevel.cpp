
#include "StdAfx.h"
#include "FLItemAction_PartyLevel.h"

#include "../_Common/party.h"
extern CPartyMng g_PartyMng;


FLItemAction_PartyLevelUp & FLItemAction_PartyLevelUp::GetInstance()
{
	static FLItemAction_PartyLevelUp sPartyLevel;
	
	return sPartyLevel;
}

bool	FLItemAction_PartyLevelUp::Use( FLWSUser & kUser, FLItemElem & kItemElem, CAr & /*ar*/ )
{
	const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemProp == NULL )
	{
		return false;
	}

	CParty* pParty	= g_PartyMng.GetParty( kUser.m_idparty );
	if( pParty == NULL )
	{
		kUser.AddDefinedText( TID_MMI_PARTYMASSAGEERROR02 );	// 극단 소속이 아닐 경우
		return false;
	}

	if( pParty->IsLeader( kUser.m_idPlayer ) == FALSE )
	{
		kUser.AddDefinedText( TID_MMI_PARTYMASSAGEERROR01 );	// 단장이 아닐 경우
		return false;
	}

	if( pParty->GetLevel() >= MIN_PARSKILL_MODE_LEVEL )			// 극단 레벨 체크
	{
		kUser.AddDefinedText( TID_MMI_PARTYMASSAGEERROR03 );
		return false;
	}

	/*
	극단 레벨이라던지 남아있는 포인트 체크를 하지 않고 강제로 레벨 조정을 하는것은 기획 의도임...
	*/

// 	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
// 	{
// 		if( pItemProp->dwDestParam[ Nth ] == DST_PARTY_LEVEL_UP )
// 		{
// 			pParty->SetPartyLevel( &kUser, pItemProp->nAdjParamVal[ Nth ], pItemProp->nEffectValue, 0 );
// 		}
// 	}

	pParty->SetPartyLevel( &kUser, MIN_PARSKILL_MODE_LEVEL, pItemProp->nEffectValue, 0 );

	g_dpDBClient.SendLogSMItemUse( "1", &kUser, &kItemElem, pItemProp );
	kUser.RemoveItem( kItemElem.m_dwObjId, 1 );

	return true;
}
