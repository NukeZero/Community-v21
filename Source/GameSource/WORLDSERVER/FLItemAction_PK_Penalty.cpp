
#include "StdAfx.h"
#include "FLItemAction_PK_Penalty.h"


FLItemAction_PK_Penalty & FLItemAction_PK_Penalty::GetInstance()
{
	static FLItemAction_PK_Penalty sPK_Penalty;
	
	return sPK_Penalty;
}

bool	FLItemAction_PK_Penalty::Use( FLWSUser & kUser, FLItemElem & kItemElem, CAr & /*ar*/ )
{
	const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemProp == NULL )
	{
		return false;
	}

	if( g_eLocal.GetState( EVE_PK ) == 0 )
	{
		kUser.AddDefinedText( TID_MMI_PKPENALTY_TEXT02 );
		return false;
	}

	if( kUser.IsChaotic() == FALSE )
	{
		kUser.AddDefinedText( TID_MMI_PKPENALTY_TEXT01 );
		return false;
	}

	const int nDecreasePKPropensity	= kUser.GetPKPropensity() - pItemProp->nEffectValue;

	const int nSetPKPropensity		= nDecreasePKPropensity > 0 ? nDecreasePKPropensity : 0;

	kUser.SetPKPropensity( nSetPKPropensity );

	g_xWSUserManager->AddPKPropensity( &kUser );

	g_dpDBClient.SendLogSMItemUse( "1", &kUser, &kItemElem, pItemProp );
	kUser.RemoveItem( kItemElem.m_dwObjId, 1 );

	//////////////////////////////////////////////////////////////////////////
	// PK 성향치가 0이 됐을땐 엔젤버프를 날려버린다..
	if( kUser.HasBuffByIk3( IK3_ANGEL_BUFF ) == TRUE && kUser.GetPKPropensity() == 0 )
	{
//		kUser.RemoveIk3Buffs( IK3_ANGEL_BUFF );
		kUser.RemoveAngel();
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}
