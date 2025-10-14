#include "StdAfx.h"



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


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL	FLApplyItemEffect::ApplyParameter( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp, BOOL bSend, int nDamage )
{
	if( pSrc == NULL )		return FALSE;

	int		i;

	DWORD	dwDestParam, dwSkillTime, dwPainTime;
	int		nAdjParam, nChgParam;
	int		nDestData[3];	// TODO : ????????????

	//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
	BOOL bPVP	= pSrc->GetType() == OT_MOVER && pSrc->IsPlayer() && pDest->IsPlayer();

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
				if( pDest->SetPoison( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )	// nDestData[0] 틱당 들어가는 데미지
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
				if( pDest->SetBleeding( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

