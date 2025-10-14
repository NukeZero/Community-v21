#include "StdAfx.h"

#include "../_Common/FLSkillSystem.h"

#include "FLMapGrid.h"
#include "LinkMap.h"

#include "../_Common/Ctrl.h"


FLApplyItemEffect::FLApplyItemEffect()
{

}

FLApplyItemEffect::~FLApplyItemEffect()
{

}

FLApplyItemEffect*	FLApplyItemEffect::GetInstance()
{
	static FLApplyItemEffect xApplyItemEffect;
	
	return & xApplyItemEffect;
}

void	FLApplyItemEffect::DoApplyHitActiveSkill( CMover* pSrc, CMover* pDest, const DWORD dwProb )
{
	if( IsValidObj( pSrc ) == FALSE || IsValidObj( pDest ) == FALSE )
	{
		return;
	}

	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; ++dwParts )
	{
		FLItemElem* pItemElem = pSrc->m_Inventory.GetEquip( dwParts );
		if( pItemElem != NULL && pItemElem->IsExpiring() == FALSE )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp != NULL )
			{
				if( pItemProp->dwHitActiveSkillProb > dwProb )
				{
					if( pItemProp->dwHitActiveSkillId != NULL_ID && pItemProp->dwHitActiveSkillLv != NULL_ID )
					{
						if( pItemProp->dwHitActiveSkillTarget == IST_SELF )
						{
							g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, pSrc, true );
						}
						else if( pItemProp->dwHitActiveSkillTarget == IST_ANOTHER )
						{
							g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, pDest, true );
						}
					}
				}
				DoApplyHitActiveSkillPiercing( pSrc, pDest, pItemElem, dwProb );
			}
		}
	}
}

void	FLApplyItemEffect::DoApplyDamageActiveSkill( CMover* pSrc, CMover* pDest, const DWORD dwProb )
{
	if( IsValidObj( pSrc ) == FALSE || IsValidObj( pDest ) == FALSE )
	{
		return;
	}

	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; ++dwParts )
	{
		FLItemElem* pItemElem = pSrc->m_Inventory.GetEquip( dwParts );
		if( pItemElem != NULL && pItemElem->IsExpiring() == FALSE )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp != NULL )
			{
				if( pItemProp->dwDamageActiveSkillProb > dwProb )
				{
					if( pItemProp->dwDamageActiveSkillId != NULL_ID && pItemProp->dwDamageActiveSkillLv != NULL_ID )
					{
						if( pItemProp->dwDamageActiveSkillTarget == IST_SELF )
						{
							g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, pSrc, true );
						}
						else if( pItemProp->dwDamageActiveSkillTarget == IST_ANOTHER )
						{
							g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, pDest, true );
						}
					}
				}
				DoApplyDamageActiveSkillPiercing( pSrc, pDest, pItemElem, dwProb );
			}
		}
	}
}

void	FLApplyItemEffect::DoApplyEquipActiveSkill( CMover* pSrc )
{
	if( IsValidObj( pSrc ) == FALSE )
	{
		return;
	}

	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; ++dwParts )
	{
		FLItemElem* pItemElem = pSrc->m_Inventory.GetEquip( dwParts );
		if( pItemElem != NULL && pItemElem->IsExpiring() == FALSE )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp != NULL )
			{
				if( pItemProp->dwEquipActiveSkillId != NULL_ID && pItemProp->dwEquipActiveSkillLv != NULL_ID )
				{
					g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwEquipActiveSkillId, pItemProp->dwEquipActiveSkillLv, pSrc, true );
				}
				DoApplyEquipActiveSkillPiercing( pSrc, pItemElem );
			}
		}
	}
}

void	FLApplyItemEffect::RefreshAllEquipActiveSkill( CMover* pSrc )
{
	RemoveAllEquipActiveSkill( pSrc );
	DoApplyEquipActiveSkill( pSrc );
}

void	FLApplyItemEffect::RemoveAllEquipActiveSkill( CMover* pSrc )
{
	if( IsValidObj( pSrc ) == FALSE )
	{
		return;
	}

	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; ++dwParts )
	{
		FLItemElem* pItemElem = pSrc->m_Inventory.GetEquip( dwParts );
		if( pItemElem != NULL )
		{
			RemoveEquipActiveSkill( pSrc, pItemElem );
		}
	}
}

void	FLApplyItemEffect::RemoveEquipActiveSkill( CMover* pSrc, FLItemElem* pItemElem )
{
	if( IsValidObj( pSrc ) == FALSE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp != NULL )
	{
		if( pItemProp->dwEquipActiveSkillId != NULL_ID && pItemProp->dwEquipActiveSkillLv != NULL_ID )
		{
			pSrc->RemoveBuff( BUFF_SKILL, (WORD)pItemProp->dwEquipActiveSkillId );

			//pSrc->OnEndSkillState( pItemProp->dwEquipActiveSkillId, pItemProp->dwEquipActiveSkillLv );
		}
		RemoveEquipActiveSkillPiercing( pSrc, pItemElem );
	}
}

void	FLApplyItemEffect::DoApplyHitActiveSkillPiercing( CMover* pSrc, CMover* pDest, FLItemElem* pItemElem, const DWORD dwProb )
{
	if( IsValidObj( pSrc ) == FALSE || IsValidObj( pDest ) == FALSE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return;
	}

	if( pItemElem->GetGeneralPiercingSize() <= 0 )
	{
		return;
	}

	for( size_t i = 0; i < pItemElem->GetGeneralPiercingSize(); ++i )
	{
		DWORD dwItemId = pItemElem->GetGeneralPiercingItemID( i );
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
		if( pItemProp != NULL && pItemProp->dwHitActiveSkillProb > dwProb )
		{
			if( pItemProp->dwHitActiveSkillId != NULL_ID && pItemProp->dwHitActiveSkillLv != NULL_ID )
			{
				if( pItemProp->dwHitActiveSkillTarget == IST_SELF )
				{
					g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, pSrc, true );
				}
				else if( pItemProp->dwHitActiveSkillTarget == IST_ANOTHER )
				{
					g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, pDest, true );
				}
			}
		}
	}
}

void	FLApplyItemEffect::DoApplyDamageActiveSkillPiercing( CMover* pSrc, CMover* pDest, FLItemElem* pItemElem, const DWORD dwProb )
{
	if( IsValidObj( pSrc ) == FALSE || IsValidObj( pDest ) == FALSE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return;
	}

	if( pItemElem->GetGeneralPiercingSize() <= 0 )
	{
		return;
	}

	for( size_t i = 0; i < pItemElem->GetGeneralPiercingSize(); ++i )
	{
		DWORD dwItemId = pItemElem->GetGeneralPiercingItemID( i );
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
		if( pItemProp != NULL && pItemProp->dwDamageActiveSkillProb > dwProb )
		{
			if( pItemProp->dwDamageActiveSkillId != NULL_ID && pItemProp->dwDamageActiveSkillLv != NULL_ID )
			{
				if( pItemProp->dwDamageActiveSkillTarget == IST_SELF )
				{
					g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, pSrc, true );
				}
				else if( pItemProp->dwDamageActiveSkillTarget == IST_ANOTHER )
				{
					g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, pDest, true );
				}
			}
		}
	}
}

void	FLApplyItemEffect::DoApplyEquipActiveSkillPiercing( CMover* pSrc, FLItemElem* pItemElem )
{
	if( IsValidObj( pSrc ) == FALSE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return;
	}

	if( pItemElem->GetGeneralPiercingSize() <= 0 )
	{
		return;
	}

	for( size_t i = 0; i < pItemElem->GetGeneralPiercingSize(); ++i )
	{
		DWORD dwItemId = pItemElem->GetGeneralPiercingItemID( i );
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
		if( pItemProp != NULL )
		{
			if( pItemProp->dwEquipActiveSkillId != NULL_ID && pItemProp->dwEquipActiveSkillLv != NULL_ID )
			{
				g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwEquipActiveSkillId, pItemProp->dwEquipActiveSkillLv, pSrc, true );
			}
		}
	}
}

void	FLApplyItemEffect::RemoveEquipActiveSkillPiercing( CMover* pSrc, FLItemElem* pItemElem )
{
	if( IsValidObj( pSrc ) == FALSE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return;
	}

	if( pItemElem->GetGeneralPiercingSize() <= 0 )
	{
		return;
	}

	for( size_t i = 0; i < pItemElem->GetGeneralPiercingSize(); ++i )
	{
		DWORD dwItemId = pItemElem->GetGeneralPiercingItemID( i );
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
		if( pItemProp != NULL )
		{
			if( pItemProp->dwEquipActiveSkillId != NULL_ID && pItemProp->dwEquipActiveSkillLv != NULL_ID )
			{
				pSrc->RemoveBuff( BUFF_SKILL, (WORD)pItemProp->dwEquipActiveSkillId );
				
				//pSrc->OnEndSkillState( pItemProp->dwEquipActiveSkillId, pItemProp->dwEquipActiveSkillLv );
			}
		}
	}
}

int		FLApplyItemEffect::GetAdjValueDuplicationBuff( FLWSUser* pUser, DWORD dwItemKind3, bool bRemove )
{
	int nValue = 0;

	if( IsValidObj( pUser ) == TRUE )
	{
		nValue = pUser->GetAdjValueDuplicationBuff( dwItemKind3 );

		if( bRemove == true )
		{
			pUser->RemoveIk3Buffs( dwItemKind3 );
		}
	}

	return nValue;
}

// int		FLApplyItemEffect::GetAdjValueSingularBuff( FLWSUser* pUser, FLItemElem* pTargetItem, DWORD dwItemKind3 )
// {
// 	int nValue = 0;
// 
// 	if( IsValidObj( pUser ) == TRUE && IsUsableItem( pTargetItem ) == TRUE )
// 	{
// 		IBuff* pBuff = pUser->GetBuffByIk3( dwItemKind3 );
// 		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
// 		{
// 			PT_ITEM_SPEC pBuffProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
// 			if( pBuffProp != NULL )
// 			{
// 				if( pBuffProp->nTargetMinEnchant <= pTargetItem->GetAbilityOption()
// 					&& pTargetItem->GetAbilityOption() <= pBuffProp->nTargetMaxEnchant )
// 				{
// 					nValue = pBuffProp->nEffectValue;
// 				}
// 			}
// 		}
// 	}
// 
// 	return nValue;
// }

int		FLApplyItemEffect::GetAdjValueGeneralEnchantRateBuff( FLWSUser* pUser, FLItemElem* pTargetItem, DWORD dwItemKind3, bool bRemove )
{
	int nValue = 0;

	if( IsValidObj( pUser ) == TRUE && IsUsableItem( pTargetItem ) == TRUE )
	{
		IBuff* pBuff = pUser->GetBuffByIk3( dwItemKind3 );
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pBuffProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pBuffProp != NULL )
			{
				if( pBuffProp->nTargetMinEnchant <= pTargetItem->GetAbilityOption()
					&& pTargetItem->GetAbilityOption() <= pBuffProp->nTargetMaxEnchant )
				{
					nValue = pBuffProp->nEffectValue;
					
					if( bRemove == true )
					{
						pUser->RemoveBuff( pBuff->GetType(), pBuff->GetId() );
					}
				}
			}
		}
	}

	return nValue;
}

int		FLApplyItemEffect::GetAdjValueAttributeEnchantRateBuff( FLWSUser* pUser, FLItemElem* pTargetItem, DWORD dwItemKind3, bool bRemove )
{
	int nValue = 0;

	if( IsValidObj( pUser ) == TRUE && IsUsableItem( pTargetItem ) == TRUE )
	{
		IBuff* pBuff = pUser->GetBuffByIk3( dwItemKind3 );
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pBuffProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pBuffProp != NULL )
			{
				if( pBuffProp->nTargetMinEnchant <= pTargetItem->GetResistAbilityOption()
					&& pTargetItem->GetResistAbilityOption() <= pBuffProp->nTargetMaxEnchant )
				{
					nValue = pBuffProp->nEffectValue;

					if( bRemove == true )
					{
						pUser->RemoveBuff( pBuff->GetType(), pBuff->GetId() );
					}
				}
			}
		}
	}

	return nValue;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void	FLApplyItemEffect::DoApplyEffect( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp )
{
	ApplyEffect( pSrc, pDest, pItemProp );

	// 2차 스킬 발동. ActiveSkill
	if( pItemProp->dwActiveSkill != NULL_ID )
	{
		SkillProp* pSkillProp = prj.GetSkillProp( pItemProp->dwActiveSkill );
		if( pSkillProp != NULL )
		{
			if( pItemProp->dwActiveSkillRate == NULL_ID || xRandom(100) <= pItemProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
			{
				g_cSkillSystem->DoActiveSkill( pSrc, pItemProp->dwActiveSkill, MIN_SKILL_USE_LEVEL, pDest, false, FALSE );		// dwActiveSkill Lv1이 발동됨.
			}
		}
	}
}

void	FLApplyItemEffect::ApplyEffect( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp )
{
	if( pItemProp->IsDebuff() == TRUE && ( pDest->m_dwMode & MATCHLESS_MODE ) )
		return;

	if( pSrc->IsPlayer() == TRUE && pDest->IsPlayer() == TRUE && pItemProp->dwSkillType != NULL_ID )
	{
		DWORD dwPKRegionAttr = pDest->GetPKPVPRegionAttr();
		int nAttackerPK = ((CMover *)pSrc)->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
		int nDefenDerPK = pDest->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
		if( nAttackerPK == 1 || nDefenDerPK == 1 )
		{
			if( pItemProp->IsDebuff() == TRUE )
			{
				if( pSrc->IsPVPTarget( pDest ) == FALSE
					|| pSrc->IsWarTarget( pDest ) == FALSE
					|| pSrc->IsGuildCombatTarget( pDest ) == FALSE
					|| pSrc->IsArenaTarget( pDest ) == FALSE
					|| pSrc->IsEventArenaTarget( pDest ) == FALSE )
				{
					return;
				}
			}
		}
	}

	if( pSrc != pDest )		// 광역 디버프 중 타겟이아니면 skip
	{
		if( pDest->IsDie() == FALSE )
		{
			if( pItemProp->IsDebuff() == TRUE )													// 나쁜마법은
			{
				if( pSrc->GetHitType2( pDest, FALSE, FALSE ) == HITTYPE_FAIL )  	// 평화적인상대에게
				{
					return;																			// 적용안됨
				}
			}

			CWorld* pWorld	= pSrc->GetWorld();
			if(  pWorld && pWorld->IsArena() == FALSE )		// 아레나가 아니면,
			{
				if( pItemProp->IsDebuff() == FALSE )													// 좋은 마법은
				{
					if( pSrc->GetHitType2( pDest, FALSE, TRUE ) != HITTYPE_FAIL )  	// 적대적인상대에게
					{
						return;																			// 적용안됨
					}
				}
			}
		}
		else
		{
			if( pItemProp->dwExeTarget != EXT_MAGICATKSHOT )
			{
				if( pItemProp->dwSfxObj2 != NULL_ID )
				{
					g_xWSUserManager->AddCreateSfxObj( pDest, pItemProp->dwSfxObj2 );
				}
			}

			if( pItemProp->IsDebuff() == TRUE )
			{
				return;
			}
		}
	}

	int nProb	= pItemProp->nProbability;

	if( nProb == NULL_ID || (int)( xRandom( 100 ) ) < nProb  )		// 적용 확률, =로 되어 있으면 100% 적용
	{
		const DWORD dwSkillTime = pItemProp->IsAbsoluteTime() == TRUE ? pItemProp->dwAbilityMin : pItemProp->dwSkillTime;

		BOOL bNew = TRUE;	// 스킬이 첨 적용됐는가.
		//sun: 팻, 변신 아이템 토글버프 적용... Neuz, World
		if( dwSkillTime != NULL_ID )	// 지속시간이 있는 스킬(프로텍션 등)
		{
			if( pItemProp->dwSkillType == BUFF_SKILL )	
			{
				// 왜 스킬이 이쪽으로 들어오는 것이냐..
				FLERROR_LOG( PROGRAM_NAME, _T( "FLApplyItemEffect::ApplyEffect." ) );
				return;
			}

			// pTarget은 this가 될수도있고 타인이 될수도 있다.
			if( pItemProp->IsAbsoluteTime() == TRUE )
			{
				CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, dwSkillTime, 0 );
				time_t t	= (time_t)( tm.GetTime() );
				
				bNew = pDest->AddBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ), t, 0, 0, BT_TICK, pSrc->GetId() );	// TODO
			}
			else
			{
				bNew = pDest->AddBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ), 0, dwSkillTime, 0, BT_TICK, pSrc->GetId() );	// TODO
			}
#if defined __AGGRO16
			if( bNew == TRUE )
			{
				if( pItemProp->IsDebuff() == TRUE )	//나쁜 스킬
				{
					if( pSrc->GetAggroMng() != NULL )
						pSrc->GetAggroMng()->SumDebuffAggro( *pSrc, *pDest );
				}
				else
				{
					FLAggro::SumBuffAggro( *pSrc, *pDest );
				}
			}
#endif // __AGGRO16

			// 최초 적용된 스킬에만
			if( bNew )	// 이미 걸려있는 상태에서 또 쓰면 bNew는 FALSE가 된다.
			{
				// 부활스킬셋팅이 되어있다면 아랫것들 무시
				if( pDest->m_Resurrection_Data.bUseing )
				{
					return;
				}

				ApplyParameter( pSrc, pDest, pItemProp, TRUE );
			} // if( bNew )
		}	// this == OT_MOVER
	}	// if( nProb == NULL_ID || xRandom(100) < nProb  )
} // ApplyEffect

BOOL	FLApplyItemEffect::ApplyParameter( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp, BOOL bSend, int nDamage )
{
	if( pSrc == NULL )		return FALSE;

	int		i;

	DWORD	dwDestParam, dwSkillTime, dwPainTime;
	int		nAdjParam, nChgParam;
	int		nDestData[3];	// TODO : ????????????

	//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
//	BOOL bPVP	= pSrc->GetType() == OT_MOVER && pSrc->IsPlayer() && pDest->IsPlayer();

	for( i = 0; i < MAX_ITEM_PARAM; i ++ )
	{
		dwDestParam = pItemProp->dwDestParam[i];
		dwSkillTime = pItemProp->dwSkillTime;
		dwPainTime  = pItemProp->tmContinuousPain;
		nAdjParam	= pItemProp->nAdjParamVal[i];
		nChgParam	= (int)pItemProp->dwChgParamVal[i];
		nDestData[0] = pItemProp->nDestData1[0];
		nDestData[1] = pItemProp->nDestData1[1];
		nDestData[2] = pItemProp->nDestData1[2];

		// destParam 1
		switch( dwDestParam )
		{
		case NULL_ID:
			break;

		case DST_CHRSTATE:		// 상태 변경
			if( nAdjParam & CHS_INVISIBILITY )	// 다크일루션상태로...
			{
				if( pDest->SetDarkCover( TRUE, dwSkillTime ) == FALSE )
				{
					return FALSE;
				}
				pDest->m_wDarkCover = 0;		
			}
			else if( nAdjParam & CHS_STUN )	// 스턴상태로...
			{
				if( pDest->SetStun( TRUE, dwSkillTime ) == FALSE )
				{
					return FALSE;
				}
				pDest->m_wStunCnt = 0;		
			}
			else if( nAdjParam & CHS_POISON )	// 독상태로..
			{
				if( pDest->SetPoison( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, static_cast< short >( nDestData[0] ) ) == FALSE )	// nDestData[0] 틱당 들어가는 데미지
				{
					return FALSE;
				}
				pDest->m_wPoisonCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_DARK )	// 암흑상태
			{
				if( pDest->SetDark( TRUE, dwSkillTime, nDestData[1] ) == FALSE )		// nDestData1[1] : 명중률 수정치.
				{
					return FALSE;
				}
				pDest->m_wDarkCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_BLEEDING )	// 출혈상태로..
			{
				if( nDestData[0] < 0 )
				{
					nDestData[0]	= nDamage / 10;
				}
				if( pDest->SetBleeding( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, static_cast< short >( nDestData[0] ) ) == FALSE )
				{
					return FALSE;
				}
				pDest->m_wBleedingCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_SLEEPING )	// 슬리핑상태로...
			{
				if( pDest->SetSleeping( true, dwSkillTime ) == false )
				{
					return FALSE;
				}
				pDest->SetSleepingDuration( 0 );
			}
			else
			{
				pDest->SetDestParam( dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE );
			}
			break;

		case DST_CHR_STEALHP: 			// HP흡수.  흡혈은 DST_로 하기가 좀 안좋다.
			{
				int nStealHP = (int)( nDamage * (nAdjParam / 100.0f) );	// 실제가격한 데미지에서 흡수할양을 정함.
				if( pSrc->GetType() == OT_MOVER )
				{
					pSrc->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );		// 시전자 HP로 흡수.
				}
			}
			break;

		case DST_CHR_STEALHP_IMM: 			// 지정된양의 HP흡수.  
			{
				if( pDest->GetType() == OT_MOVER )
				{
					int nHP = pDest->GetHitPoint();		// 방어자의 hp
					int nStealHP = nAdjParam;	// 흡수해야할 hp
					if( nHP < nStealHP )	// 피를 뽑아와야하는데 남은피가 더 적으면.
					{
						nStealHP = nHP;		// 남은피만큼 뽑아옴
					}

					if( pSrc->GetType() == OT_MOVER )
					{
						pDest->m_pActMover->SendDamage( AF_FORCE,	((CMover*)pSrc)->GetId(), nStealHP );
						pSrc->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM, TRUE  );		// 공격자 HP로 흡수.
					}
				}
			}
			break;

		case DST_CURECHR:		// 상태 해제
			pDest->RemoveDebuff( nAdjParam );
			break;

		case DST_CLEARBUFF:		// 대상의 버프를 파괴.
			if( nAdjParam == NULL_ID )
			{
				pDest->RemoveBuffAll();
			}
			else
			{
				for( int i = 0; i < nAdjParam; i ++ )
					pDest->RemoveBuffOne();
			}
			break;

		case DST_CHR_CHANCESTUN:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEPOISON:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEDARK:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEBLEEDING:// adj:확률		data1:스킬
		case DST_CHR_CHANCESTEALHP:	// adj:확률		data1:스킬
			pDest->SetDestParam( dwDestParam, nAdjParam, nChgParam, TRUE );
			break;

		default: // 그외 DST_는 일반적인 루틴을 탄다.
			// 일반 DST_처리
			pDest->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
			break;
		}
	}

	return TRUE;
}

void	FLApplyItemEffect::SetDestParameter( CMover* pMover, int nIdx, PT_ITEM_SPEC pItemProp, BOOL bSend )
{
	if( pMover == NULL || pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA. pMover is NULL or pItemProp is NULL ]" ) );
		
		return;
	}
	// TODO : range check nIdx
	if( nIdx < 0 || nIdx >= MAX_ITEM_PARAM )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM PARAM RANGE INVALID. PLAYER(%07d), ITEM_ID(%d), nIdx(%d) ]" )
			, pMover->m_idPlayer, pItemProp->dwID, nIdx );
		
		return;
	}

	pMover->SetDestParam( pItemProp->dwDestParam[nIdx], pItemProp->nAdjParamVal[nIdx], pItemProp->dwChgParamVal[nIdx], bSend );
}

void	FLApplyItemEffect::ResetDestParameter( CMover* pMover, int nIdx, PT_ITEM_SPEC pItemProp, BOOL bSend )
{
	if( pMover == NULL || pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA. pMover is NULL or pItemProp is NULL ]" ) );

		return;
	}
	// TODO : range check nIdx
	if( nIdx < 0 || nIdx >= MAX_ITEM_PARAM )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM PARAM RANGE INVALID. PLAYER(%07d), ITEM_ID(%d), nIdx(%d) ]" )
			, pMover->m_idPlayer, pItemProp->dwID, nIdx );

		return;
	}

	pMover->ResetDestParam( pItemProp->dwDestParam[nIdx], pItemProp->nAdjParamVal[nIdx], bSend );
}

void	FLApplyItemEffect::SendDamageAroundPos( const D3DXVECTOR3 *pvPos, int nDmgType, CMover* pAttacker, int nApplyType, int nAttackID, float fRange )
{
	if( nDmgType == AF_MAGICSKILL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not Item Effect. Attacker:[%s], Skill ID:[%d]" ), pAttacker->GetName(), nAttackID );
		return;
	}

	int nRange	= 4;	// 4, 8, 16 단위로 넣자.
	float fDistSq;
	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = *pvPos;
	D3DXVECTOR3 vDist;

	if( fRange <= 4.0f )
	{
		nRange = 4;
	}
	else if( fRange <= 8.0f )
	{
		nRange = 8;
	}
	else if( fRange <= 16.0f )
	{
		nRange = 16;
	}
	else
	{
		nRange = 32;
	}

	if( fRange <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "D:%d,%d,%d A:%s %d %f" ), pvPos->x, pvPos->y, pvPos->z, pAttacker->GetName(), nAttackID, fRange );
		return;
	}

	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( nAttackID );
	if( pProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Item Property is Null. Attacker:[%s], Item ID:[%d]" ), pAttacker->GetName(), nAttackID );
		return;
	}

//	BOOL	bDamage = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( pAttacker->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, pAttacker->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj != pAttacker )		// 어태커는 검색대상에서 제외.
					{
						pTarget = (CMover*)pObj;
						if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
						{
							if( pProp->dwComboStyle == CT_FINISH )
							{
								pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
							else
							{
								pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( pAttacker->GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, pAttacker->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPeaceful() == FALSE )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj != pAttacker )		// 공격자는 검사대상에서 제외.
					{
						pTarget = (CMover*)pObj;
						if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
						{
							if( pProp->dwComboStyle == CT_FINISH )
							{
								pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
							else
							{
								pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}
}

void	FLApplyItemEffect::SendDamageAroundTarget( int nDmgType, CMover* pAttacker, CMover* pTarget, int nApplyType, int nAttackID, float fRange, float fTargetRatio )
{
	if( nDmgType == AF_MAGICSKILL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not Item Effect. Attacker:[%s], Skill ID:[%d]" ), pAttacker->GetName(), nAttackID );
		return;
	}

	int nRange	= 4;	// 4, 8, 16 단위로 넣자.
	float fDistSq;
	CObj* pObj;
	D3DXVECTOR3 vPos = pTarget->GetPos();
	D3DXVECTOR3 vDist;

	if( fRange <= 4.0f )
	{
		nRange = 4;
	}
	else if( fRange <= 8.0f )
	{
		nRange = 8;
	}
	else if( fRange <= 16.0f )
	{
		nRange = 16;
	}
	else
	{
		nRange = 32;
	}

	if( fRange <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "D:%s A:%s %d %f" ), pTarget->GetName(), pAttacker->GetName(), nAttackID, fRange );
		return;
	}

	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( nAttackID );
	if( pProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Item Property is Null. Attacker:[%s], Item ID:[%d]" ), pAttacker->GetName(), nAttackID );
		return;
	}

	BOOL	bTarget = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( pTarget->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, pTarget->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( (CMover*)pObj == pTarget && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					}
					else
					{
						if( (CMover*)pObj != pAttacker )		// 어태커는 검색대상에서 제외.
						{
							bTarget = ( (CMover*)pObj == pTarget );
							if( IsValidObj( pObj ) && ((CMover*)pObj)->IsLive() )
							{
								if( pProp->dwComboStyle == CT_FINISH )
								{
									pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
								}
								else
								{
									pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
								}
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( pTarget->GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, pTarget->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj == pTarget && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					}
					else
					{
						if( pObj != pAttacker )		// 공격자는 검사대상에서 제외.
						{
							if( IsValidObj( pObj ) && ((CMover*)pObj)->IsLive() )
							{
								if( pProp->dwComboStyle == CT_FINISH )
								{
									pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
								}
								else
								{
									pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
								}
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

