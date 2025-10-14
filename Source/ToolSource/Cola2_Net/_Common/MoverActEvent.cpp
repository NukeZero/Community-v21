#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "defineSkill.h"
#ifdef __CLIENT
 #include "AppDefine.h"
 #include "defineSound.h"
#endif
#include "Ship.h"
#include "ModelObject.h"
#include "CreateObj.h"
#include "Sfx.h"

#ifdef __CLIENT
 #include "DPClient.h"
 extern	CDPClient	g_DPlay;
#else	// __CLIENT
 #ifdef __WORLDSERVER
  #include "User.h"
  #include "WorldMng.h"
  #include "DPSrvr.h"
  #include "DPCoreClient.h"
  #include "dpdatabaseclient.h"
  extern	CUserMng	g_UserMng;
  extern	CWorldMng	g_WorldMng;
  extern	CDPSrvr		g_DPSrvr;
  extern	CDPDatabaseClient	g_dpDBClient;
  extern	CDPCoreClient	g_DPCoreClient;
  #include "Party.h"
  extern	CPartyMng	g_PartyMng;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __BEAST		// 비스트에선 이 모듈이 사용되지 않는다.
/*--------------------------------------------------------------------------------------------------------


											Action Event Handler
										   ----------------------
						CActionMover내에서 동작에 관련된 이벤트가 발생하면 호출되는 함수군


----------------------------------------------------------------------------------------------------------*/

//
// 신버전 ..
// 스킬 모션이 작동하다 타격 프레임이 되면 이 함수를 호출함 
// 근접전 스킬.    
//
BOOL CMover::OnMeleeSkill( int nType, int nCount )
{
	if( m_oaAct != OBJACT_USESKILL )	
		return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.
	int		nSkill = GetActParam( 0 );
	OBJID	idTarget = GetActParam( 1 );
	DWORD	dwLevel	= GetActParam( 3 );

	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;

	// 스킬프로퍼티 가져옴.
	GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "OnMeleeSkill" );
	
	// .타겟에게 스킬효과(데미지같은..)를 준다.
	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )	
		return FALSE;

#if defined(__CLIENT) && defined(__YSPARK)
	CModelObject* pModel =(CModelObject*)m_pModel;
	if( pModel )
	{
		if( pFocusObj )
		{
			pModel->m_SparkInfo.m_bUsed  = TRUE;
			pModel->m_SparkInfo.m_v3Color.x = 2.0f;
			pModel->m_SparkInfo.m_v3Color.y = 2.0f;
			pModel->m_SparkInfo.m_v3Color.z = 2.0f;
			pModel->m_SparkInfo.m_nCnt = 0;
			pModel->m_SparkInfo.m_fLerp= 1.0f;
			
			D3DXVECTOR3 vDir;
			if( pFocusObj == this )
				vDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			else
				vDir = GetPos() - pFocusObj->GetPos();

			D3DXVec3Normalize( &pModel->m_SparkInfo.m_v3SparkDir, &vDir );
		}
	}
#endif //defined(__CLIENT) && defined(__YSPARK)	

	int nDamage = 0;

	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT 
#ifdef __XSKILL0425
		|| pSkillProp->dwExeTarget == EXT_MAGICSHOT 		// 쏘는 형태만 sfx발생.
#endif
		)	
		nDamage = ShootSkill( pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	else
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
		nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp );
	TRACE( "OnMeleeSkill%s, ", pSkillProp->szName );

#ifdef __YMOTIONHITCOUNT
	if( nDamage && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
#else //__YMOTIONHITCOUNT
	if( nDamage )		// == Success
#endif //__YMOTIONHITCOUNT
	{
		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
#ifdef __CLIENT
		if( IsActiveMover() )
#endif
			if( pSkillProp->dwSkillReady != 0xffffffff )	// 쿨타임이 있는 스킬의 경우
				SetCoolTime( pSkillProp, "OnMeleeSkill" );
		
	}

	return TRUE;
} // 신버전 OnMeleeSkill


// 캐스팅 시간이 끝났는지 판단함.
// 서버에서 캐스팅 타임 계산해서 클라로 보내주게 해야할거 같다.
// 스핵의 악용 가능성 있음.
BOOL CMover::IsFinishCastingTime( int nCount )
{
	if( m_oaAct != OBJACT_USESKILL )	
	{
		Message( "경고 : CMover:IsFinishCastingTime : oaAct=%d", m_oaAct );
		return TRUE;	// 만약 이런상황이 오면 TRUE를 리턴해서 영원히 시전하고 있지 않게 하자.
	}
	int		nSkill = GetActParam( 0 );
	int nSkillIdx	= GetActParam( 2 );
	DWORD dwLevel	= GetActParam( 3 );
	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	if( pSkillProp == NULL )
	{
		Error( "CMover::OnMagicSkill : %s. skill(%d) empty property", m_szName, nSkill );	
		return TRUE;	// property not found
	}

	AddSkillProp* pAddSkillProp;
	if( nSkillIdx < 0 )
	{
		pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	}
	else
	{
		LPSKILL pSkill	= GetSkill( 0, nSkillIdx );
		pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );	// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	}
	
	if( pAddSkillProp == NULL )
	{
		Error( "CMover::OnMagicSkill : %s. add skill(%d) property is empty.", m_szName, nSkill );
		return TRUE;
	}
	
	int nCastingTime = (int)((pAddSkillProp->dwSkillReady / 1000.0f) * SEC1);	// 프로퍼티의 1/1000초단위를 1/60단위로 바꾼다.
#ifdef __NEWWPN0901
	nCastingTime	= GetCastingTime( nCastingTime );
#endif	// __NEWWPN0901
	if( nCount >= nCastingTime )
		return TRUE;

	return FALSE;
}

// RT_TIME을 쓰는 스킬들의 스킬 시간을 계산.
void	CMover::SubReferTime( DWORD *pTime1, DWORD *pTime2, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	DWORD dwTime1 = 0, dwTime2 = 0;

	if( pSkillProp->dwReferTarget1 == RT_TIME )
	{
		switch( pSkillProp->dwReferStat1 )
		{
		case DST_STA:	dwTime1 = GetSta();		break;
		case DST_DEX:	dwTime1 = GetDex();		break;
		case DST_INT:	dwTime1 = GetInt();		break;
		}
		
		dwTime1 = ((pSkillProp->dwReferValue1/10.0f)*dwTime1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime1/50.0f));
	}
	
	if( pSkillProp->dwReferTarget2 == RT_TIME )
	{
		switch( pSkillProp->dwReferStat2 )
		{
		case DST_STA:	dwTime2 = GetSta();		break;
		case DST_DEX:	dwTime2 = GetDex();		break;
		case DST_INT:	dwTime2 = GetInt();		break;
		}
		
		dwTime2 = ((pSkillProp->dwReferValue2/10.0f)*dwTime2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime2/50.0f));
	}

	*pTime1 = dwTime1;
	*pTime2 = dwTime2;
}
/*
BOOL CMover::IsPossibleApplyParam( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
}
*/
//
// this에게 파라메터를 적용한다.
// SetDestParam을 일괄적으로 하는 부분.
// m_SkillState.Serialize()후에도 이게 수행된다.
// pSrc는 시전자.
BOOL	CMover::ApplyParam( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend, int nDamage )
{
	int		i;
	CMover *pTarget = (CMover *)this;

	DWORD	dwDestParam, dwSkillTime, dwPainTime, dwAbilityMin;
	int		nAdjParam, nChgParam;
	int		nDestData[3];

	for( i = 0; i < 2; i ++ )
	{
		if( pAddSkillProp )
		{
			dwDestParam = pAddSkillProp->dwDestParam[i];
			dwSkillTime = pAddSkillProp->dwSkillTime;
			dwPainTime = pAddSkillProp->dwPainTime;
			dwAbilityMin = pAddSkillProp->dwAbilityMin;
			nAdjParam	= pAddSkillProp->nAdjParamVal[i];
			nChgParam	= (int)pAddSkillProp->dwChgParamVal[i];
			nDestData[0] = pAddSkillProp->nDestData1[0];
			nDestData[1] = pAddSkillProp->nDestData1[1];
			nDestData[2] = pAddSkillProp->nDestData1[2];
		} else
		{	
			dwDestParam = pSkillProp->dwDestParam[i];
			dwSkillTime = pSkillProp->dwSkillTime;
			dwPainTime  = pSkillProp->tmContinuousPain;
			dwAbilityMin = pSkillProp->dwAbilityMin;
			nAdjParam	= pSkillProp->nAdjParamVal[i];
			nChgParam	= (int)pSkillProp->dwChgParamVal[i];
			nDestData[0] = pSkillProp->nDestData1[0];
			nDestData[1] = pSkillProp->nDestData1[1];
			nDestData[2] = pSkillProp->nDestData1[2];
		}
		// destParam 1
		switch( dwDestParam )
		{
		case NULL_ID:
			break;
		case DST_CHRSTATE:		// 상태 변경
#ifdef __ACROBAT_0504			
			if( nAdjParam & CHS_INVISIBILITY )	// 다크일루션상태로...
			{
				if( pTarget->SetDarkCover( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				pTarget->m_wDarkCover = 0;		
			}
			else
#endif //__ACROBAT_0504
			if( nAdjParam & CHS_STUN )	// 스턴상태로...
			{
				if( pTarget->SetStun( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				// 일단 땜빵. 
				// 스턴이 걸리는건 두가지 루트가 있는데
				// 1. 스킬에 맞았을때.
				// 2. 스턴아이템에 맞았을때
				// 3. 기타 상황(트랩같은..)
				// 스킬을 사용해서 스턴이 걸린건 SkillInflu에서 처리해서
				// 해제까지 시켜주기땜에 m_wStunCnt에 값이 들어가면 두군데서 해제를 시키게 된다.
				// 아이템에 의해 스턴이 걸린건 SkillInflu에서 처리를 안하므로 m_wStunCnt가 필요하다.
				// 이곳은 스킬에 의해서 들어오는 곳이기 때문.
				// 구조가 잘못설계됐다.  스킬중심이 아니고 상태중심으로 처리가 됐어야 했다.
				pTarget->m_wStunCnt = 0;		
			} else
			if( nAdjParam & CHS_POISON )	// 독상태로..
			{
 #ifdef __XSKILL0425
				if( pTarget->SetPoison( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )	// nDestData[0] 틱당 들어가는 데미지
					return FALSE;
				pTarget->m_wPoisonCnt = 0;		// 스턴이랑 같은 이유
 #else // xSkill0425
				if( pTarget->SetPoison( TRUE, pSrc->GetId(), dwSkillTime, nDestData[1], nDestData[2] ) == FALSE )
					return FALSE;
				pTarget->m_wPoisonCnt = 0;		// 스턴이랑 같은 이유
 #endif // not xSkill0425
			} else
			if( nAdjParam & CHS_DARK )	// 암흑상태
			{
				if( pTarget->SetDark( TRUE, dwSkillTime, nDestData[1] ) == FALSE )		// nDestData1[1] : 명중률 수정치.
					return FALSE;
				pTarget->m_wDarkCnt = 0;		// 스턴이랑 같은 이유
			} else

			if( nAdjParam & CHS_BLEEDING )	// 출혈상태로..
			{
 #ifdef __XSKILL0425
				if( pTarget->SetBleeding( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )
					return FALSE;
				pTarget->m_wBleedingCnt = 0;		// 스턴이랑 같은 이유
 #else
				if( pTarget->SetBleeding( TRUE, pSrc->GetId(), dwSkillTime, nDestData[1], nDestData[2] ) == FALSE )
					return FALSE;
				pTarget->m_wBleedingCnt = 0;		// 스턴이랑 같은 이유
 #endif
			} else
			{
				// 다른 상태들은 걍 세팅함.
				pTarget->SetDestParam( dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE );
			}
			break;

 #ifndef __XSKILL0425
		case DST_CHR_CHANCESTUN:		// 스턴 공격 속성. Adj:확률  Chg:스턴시간.
			pTarget->SetDestParam( DST_CHR_CHANCESTUN, nAdjParam, nChgParam, TRUE );
			break;
 #endif // not xSkill0425
		case DST_CHR_STEALHP: 			// HP흡수.  흡혈은 DST_로 하기가 좀 안좋다.
			{
				int nStealHP = nDamage * (nAdjParam / 100.0f);	// 실제가격한 데미지에서 흡수할양을 정함.
				if( pSrc->GetType() == OT_MOVER )
					((CMover *)pSrc)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );		// 시전자 HP로 흡수.
			}
			break;
#ifdef __XSKILL0425
		case DST_CHR_STEALHP_IMM: 			// 지정된양의 HP흡수.  
			{
				if( GetType() == OT_MOVER )
				{
					int nHP = GetHitPoint();		// 방어자의 hp
					int nStealHP = nAdjParam;	// 흡수해야할 hp
					if( nHP < nStealHP )	// 피를 뽑아와야하는데 남은피가 더 적으면.
						nStealHP = nHP;		// 남은피만큼 뽑아옴
					//SetDestParam( DST_HP, -nStealHP, 0 );		// 피를 뺏김.
					if( pSrc->GetType() == OT_MOVER )
					{
						((CMover*)this)->m_pActMover->SendDamage( AF_FORCE,		// 타겟은 데미지입고
													      GetId(), 
														  nStealHP );

						((CMover *)pSrc)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM, TRUE  );		// 공격자 HP로 흡수.
					}
				}
			}
			break;
#endif // xSkill0425
	#ifndef __XSKILL0425
			//{{AFX
		case DST_CHR_CHANCEPOISON:	// adj:확률  Chg:시간
			pTarget->SetDestParam( DST_CHR_CHANCEPOISON, nAdjParam, nChgParam, TRUE );
			pTarget->m_tmPoisonUnit = nDestData[0];		// 몇초마다 데미지를 먹냐.
			pTarget->m_wPoisonDamage = nDestData[1];	// 얼마나 데미지를 먹냐.
			break;
		case DST_CHR_CHANCEDARK:	// adj:확률  Chg:시간
			pTarget->SetDestParam( DST_CHR_CHANCEDARK, nAdjParam, nChgParam, TRUE );
			pTarget->m_wDarkVal = nDestData[0];		// 명중률이 얼마나 떨어지냐.
			break;
			//}}AFX
	#endif // not xSkill0425

		case DST_CURECHR:		// 상태 해제
#ifdef __XITEM0722
			pTarget->RemoveDebuff( nAdjParam );
#else
			pTarget->RemoveDebuff( CHS_DARK_POISON_STUN );
#endif
//			// 스킬로 걸린건 이걸로 해제해야하고
//			pTarget->m_SkillState.RemoveSkillInfluenceState( nAdjParam );	// 상태이상을 해제함.
//			// 스킬이 아닌 아이템같은걸로 상태만 걸린건 이걸로 풀어야 한다.
//			if( nAdjParam & CHS_DARK )
//				pTarget->SetDark( FALSE );
//			if( nAdjParam & CHS_POISON )
//				pTarget->SetPoison( FALSE );
//			if( nAdjParam & CHS_STUN )
//				pTarget->SetStun( FALSE );
//			// 이거 안에서 Reset을 호출한다.
//			pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, TRUE );
			break;
#ifdef __XSKILL0425
//		case DST_CHR_CHANCESKILL:	// adj:확률		data1:스킬
		case DST_CHR_CHANCESTUN:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEPOISON:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEDARK:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEBLEEDING:// adj:확률		data1:스킬
		case DST_CHR_CHANCESTEALHP:	// adj:확률		data1:스킬
			pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, TRUE );
			break;
#endif // xSkill0425
			
#ifdef __WORLDSERVER
		case DST_HP:	// 힐링.
			if( pAddSkillProp )
			{
				if( pSkillProp->dwReferTarget1 == RT_HEAL || pSkillProp->dwReferTarget2 == RT_HEAL )
				{
					DWORD dwHp1 = 0, dwHp2 = 0;
					
					if( pSrc->GetType() == OT_MOVER )	// 시전자가 사람일때만 스탯에 의한 영향
					{
						if( pSkillProp->dwReferTarget1 == RT_HEAL )
						{
							switch( pSkillProp->dwReferStat1 )
							{
							case DST_STA:		dwHp1 = ((CMover *)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp1 = ((CMover *)pSrc)->GetDex();			break;
							case DST_INT:		dwHp1 = ((CMover *)pSrc)->GetInt();			break;
							}
							dwHp1 = ((pSkillProp->dwReferValue1 / 10.0f) * dwHp1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp1 / 50.0f));
						}
						
						if( pSkillProp->dwReferTarget2 == RT_HEAL )
						{
							switch( pSkillProp->dwReferStat2 )
							{
							case DST_STA:		dwHp2 = ((CMover *)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp2 = ((CMover *)pSrc)->GetDex();			break;
							case DST_INT:		dwHp2 = ((CMover *)pSrc)->GetInt();			break;
							}
							dwHp2 = ((pSkillProp->dwReferValue2 / 10.0f) * dwHp2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp2 / 50.0f));
						}
					}
					
					// 증가될 HP량.
					int nIncHP = pAddSkillProp->nAdjParamVal1 + dwHp1 + dwHp2;
					// 힐링계열 스킬을 썼냐?
					if( (pAddSkillProp->dwDestParam1 == DST_HP || pAddSkillProp->dwDestParam2 == DST_HP) && nIncHP > 0 )
					{
						if( pSrc->GetType() == OT_MOVER )
							if( ((CMover *)pSrc)->IsPlayer() )	// 시전자가 플레이어일때
								if( pTarget->GetMaxHitPoint() > pTarget->GetHitPoint() + nIncHP )		// 타겟이 오버힐 됐냐?
									((CUser *)pSrc)->m_nOverHeal = PROCESS_COUNT * 30;				// 시전자는 30초간 오버힐 상태. 
					}
					
					// 힐시작.
					pTarget->SetDestParam( pAddSkillProp->dwDestParam1, nIncHP, pAddSkillProp->dwChgParamVal1, TRUE );
					
				}
			}
			break;
#endif // WorldServer

		default: // 그외 DST_는 일반적인 루틴을 탄다.
			// 일반 DST_처리
			pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
			break;
		}
	}

	return TRUE;
}

#ifndef __XSKILL0425
//{{AFX
//
// pMover에게 pSkillProp의 효과를 적용함.
//
BOOL CMover::ApplyMagicSkill( CMover *pTarget, DWORD dwSkill, DWORD dwLevel, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	BOOL	bNew = TRUE;	// 스킬이 첨 적용됐는가.
	if( (int)pAddSkillProp->dwSkillTime > 0 )	// 지속시간이 있는 스킬(프로텍션 등)
	{
		// pTarget은 this가 될수도있고 타인이 될수도 있다.
		if( pSkillProp->dwReferTarget1 == RT_TIME || pSkillProp->dwReferTarget2 == RT_TIME )
		{
			DWORD dwTime1 = 0, dwTime2 = 0;

			SubReferTime( &dwTime1, &dwTime2, pSkillProp, pAddSkillProp );
			
			bNew = pTarget->m_SkillState.Set( dwSkill, dwLevel, pAddSkillProp->dwSkillTime + dwTime1 + dwTime2 );
		}
		else
			bNew = pTarget->m_SkillState.Set( dwSkill, dwLevel, pAddSkillProp->dwSkillTime );

		pTarget->m_SkillState.SetMover( pTarget );		// 이건 나중에 초기화 쪽으로 옮겨야 한다.
	} // if 지속시간이 있는 스킬

#ifdef __WORLDSERVER
	if( bNew )	// 스킬이 첨 적용됐을때만 파라메터를 수정한다. 이미 적용중인스킬이 또 사용됐을땐 이게 실행되면 좆치안타.
	{
		if( pTarget->IsPlayer() && pTarget->IsDie() && pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )	// 타겟이 죽어있고 부활을 사용했다면.
		{
			if( pTarget->m_pActMover->GetDmgState() == OBJSTA_RESURRECTION )
			{
				return FALSE;		// 이미 부활중인 사람에게 쓴거라면 취소.
			} else
			{
				g_dpDBClient.SendLogLevelUp( pTarget, 10 );	// 로드스타로 부활 로그

				g_UserMng.AddHdr( pTarget, SNAPSHOTTYPE_RESURRECTION );		// 부활일땐 따로 보내자.
				pTarget->m_pActMover->SendActMsg( OBJMSG_RESURRECTION );	// 부활모드로 전환.
			}
		}
		if( pSkillProp->dwID == SI_MAG_MAG_MENTALSTRIKE )		// 멘탈 스트라이크
		{
			int nVar = GetInt() - pTarget->GetInt();		// 타겟과의 지능차.
			nVar = max(nVar, 0);

			if( nVar >= 0 )
			{
				nVar++;		// 지능차의 +1값을 쓴다.
				int nMin = pAddSkillProp->dwAbilityMin * nVar;		// 능력치 최소/최대값에 지능차를 곱한다.
				int nMax = pAddSkillProp->dwAbilityMax * nVar;
				int nDamage = nMin + xRandom((nMax - nMin) + 1);
				if( nDamage < 0 )			nDamage = 0;
				if( nDamage > 0xffff )		nDamage = 0xffff;
				// 멘탈스트라이크는 하위 16비트에 데미지를 넣어서 보낸다.  아 SendDamage()인터페이스 꿀꿀하다~
				nDamage = pTarget->m_pActMover->SendDamage( AF_MAGICSKILL, GetId(), (pSkillProp->dwID << 16) | nDamage );	
			}
		}
		
		if( pSkillProp->dwReferTarget1 == RT_HEAL || pSkillProp->dwReferTarget2 == RT_HEAL )
		{
			DWORD dwHp1 = 0, dwHp2 = 0;

			if( pSkillProp->dwReferTarget1 == RT_HEAL )
			{
				switch( pSkillProp->dwReferStat1 )
				{
				case DST_STA:		dwHp1 = GetSta();			break;
				case DST_DEX:		dwHp1 = GetDex();			break;
				case DST_INT:		dwHp1 = GetInt();			break;
				}
				dwHp1 = ((pSkillProp->dwReferValue1/10.0f)*dwHp1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp1/50.0f));
			}

			if( pSkillProp->dwReferTarget2 == RT_HEAL )
			{
				switch( pSkillProp->dwReferStat2 )
				{
				case DST_STA:		dwHp2 = GetSta();			break;
				case DST_DEX:		dwHp2 = GetDex();			break;
				case DST_INT:		dwHp2 = GetInt();			break;
				}
				dwHp2 = ((pSkillProp->dwReferValue2/10.0f)*dwHp2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp2/50.0f));
			}
			
			// 증가될 HP량.
			int nIncHP = pAddSkillProp->nAdjParamVal1 + dwHp1 + dwHp2;
			// 힐링계열 스킬을 썼냐?
			if( pAddSkillProp->dwDestParam1 == DST_HP && nIncHP > 0 )
			{
				if( IsPlayer() )
					if( pTarget->GetMaxHitPoint() > pTarget->GetHitPoint() + nIncHP )		// 오버힐 했냐?
						((CUser *)this)->m_nOverHeal = PROCESS_COUNT * 30;				// 30초간 오버힐 상태.
			}

			pTarget->SetDestParam( pAddSkillProp->dwDestParam1, nIncHP, pAddSkillProp->dwChgParamVal1, TRUE );

			// 부활에 dwReferTaget이 RT_HEAL이면 부활할때 경험치가 조금 깎임.
			if( pTarget->IsPlayer() && pTarget->IsDie() && pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
			{
				if( pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP )
					pTarget->SubDieDecExp(TRUE, pAddSkillProp->nAdjParamVal2 );	// 부활이 되면서 겸치가 조금 깎임.
			}
		}
		else
		// not RT_HEAL
		{
			if( pTarget->IsPlayer() && pTarget->IsDie() && pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
			{
				if( pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP )
					pTarget->SubDieDecExp(TRUE, pAddSkillProp->nAdjParamVal2);	// 부활로 살렸을때도 겸치 조금 깎임.
			}

//			SetDestParam( pAddSkillProp, pAddSkillProp->dwDestParam1, pAddSkillProp->nAdjParamVal1, pAddSkillProp->dwChgParamVal1 );
			pTarget->ApplyParam( this, pSkillProp, pAddSkillProp );
		}
		
	}
#endif // WorldServer
	
	return bNew;
}
//}}AFX
#endif // not xSkill0425

BOOL	CMover::GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	char szStr[256];

	memset( szStr, 0, sizeof(szStr) );
	strcat( szStr, m_szName );
	strcat( szStr, "-" );
	strcat( szStr, szErr );

	BOOL bSuccess = CCtrl::GetSkillProp( ppSkillProp, ppAddSkillProp, nSkill, dwLevel, szStr );

	return bSuccess;
}

#ifndef __XSKILL0425
//{{AFX
// 스킬 발동 OnMagicSkill이나 무기타격때 발동.
BOOL CMover::CreateSkillEffect( OBJID idTarget, int nSkill, DWORD dwLevel )
{
	AddSkillProp	*pAddSkillProp;
	ItemProp		*pSkillProp;
	int				idSfxHit;	// = GetActParam(3);

	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )		return FALSE;
	
	GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "CreateSkillEffect" );

#ifdef __WORLDSERVER
	int nDamage = 0;
	// 적용범위
	switch( pSkillProp->dwSpellRegion )
	{
	case NULL_ID:	// 스펠리젼이 없는건 데미지를 주지 않는다.
		break;
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
		if( pSkillProp->dwUseChance == WUI_NOW )	// 즉시 발동 마법의경우만 여기서 데미지를 준다(파이어봄버 같은..)
		{
			float fCenter = 1.0f;	// 대상
			float fAround = 1.0f;	// 주위
			if( pSkillProp->dwSpellRegion == SRO_AROUND )
			{	
				fCenter = 0;		// AROUND는 중앙은 데미지 없슴.
				fAround = 1.0f;
			}
			int nApplyType = 0;
			if( pFocusObj->IsPlayer() )		// 타겟이 플레이어면.
				nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 플레이어랑 몬스터 모두에게 데미지.
			else
				nApplyType = OBJTYPE_MONSTER;
			// pFocusObj(타겟), 데미지종류, 시전자, 스킬종류, 범위(미터), 타겟에게주는 효과, 주변에 주는효과.
			pFocusObj->SendDamageAround( AF_MAGICSKILL, this, nApplyType, nSkill, pAddSkillProp->dwSkillRange, fCenter, fAround ); 
		}
		break;
	case SRO_DIRECT:	// 대상 하나
	default:
		if( pSkillProp->dwExeTarget == EXT_MAGICATK && pSkillProp->dwID != SI_MAG_MAG_MENTALSTRIKE )	// 즉시 공격 마법의 경우만 데미지줌. / 멘탈스트라이크는 예외
		{
			// 타겟에게 데미지를 주는 형태.
			if( pSkillProp->dwComboStyle == CT_FINISH )
				nDamage = pFocusObj->m_pActMover->SendDamageForce( AF_MELEESKILL, GetId(), (nSkill << 16) );		// 이거 왜 MELEESKILL이지? ㅡㅡ;
			else
				nDamage = pFocusObj->m_pActMover->SendDamage( AF_MELEESKILL, GetId(), (nSkill << 16) );
		}
		break;
	}

	if( nDamage > 0 )
	{
		if( pAddSkillProp->dwDestParam1 == DST_CHR_STEALHP )	// HP흡수.  흡혈은 DST_로 하기가 좀 안좋다.
		{
			int nStealHP = nDamage * (pAddSkillProp->nAdjParamVal1 / 100.0f);	// 실제가격한 데미지에서 흡수할양을 정함.
			SetDestParam( DST_HP, nStealHP, 0 );		// 내 HP로 흡수.  마법의 경우 다이렉트 데미지만 흡혈이 적용됨
		}
	}
	

	BOOL bNew = TRUE;
	switch( pSkillProp->dwExeTarget )	// 발동 대상에 따른 처리
	{
	case EXT_TROUPE:	// 극단을 대상으로 사용
		{
//			CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
			
			CParty *pParty	= g_PartyMng.GetParty( m_idparty );
			if( pParty )
			{
				int i;
				CUser *pMember;
				D3DXVECTOR3	vDist;
				float	fDist;
				float	fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
				fMaxDistSq *= fMaxDistSq;
//				bNew = FALSE;
				for( i = 0; i < pParty->m_nSizeofMember; i++ )
				{
					pMember	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( pMember ) && !pMember->IsDie() )
					{
						vDist = pMember->GetPos() - GetPos();
						fDist = D3DXVec3LengthSq( &vDist );			// 파티 멤버와 시전자와의 거리.
						if( fDist < fMaxDistSq )					// xx미터보다 작으면
						{
							bNew = ApplyMagicSkill( pMember, (DWORD)nSkill, dwLevel, pSkillProp, pAddSkillProp );	// 멤버 각각에게 효과 적용.
							if( bNew && pSkillProp->dwSfxObj3 != NULL_ID && this != pMember )		// 일단 자기자신에게는 sfx안보냄 아래에 sfx생성땜에 중복될수있음.
								g_UserMng.AddCreateSfxObj( pMember, pSkillProp->dwSfxObj3 );
						}
					}
				}
			}
			else
			{
				// 파티가 없음. 자기 자신에게만 사용.
				bNew = ApplyMagicSkill( this, (DWORD)nSkill, dwLevel, pSkillProp, pAddSkillProp );
			}

			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
		}
		break;
	default:			// 그외는 단일 대상.
		{
			// 대상오브젝트에 마법스킬 적용. 리턴값은 처음 적용된것인지 결과.
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )		// 원거리 공격은 여기서 적용하면 안된다. 맞았을때 해야한다.
				bNew = ApplyMagicSkill( pFocusObj, (DWORD)nSkill, dwLevel, pSkillProp, pAddSkillProp );
		}
	}
#endif	// WORLDSERVER

#ifdef __CLIENT		
	// 이곳에서 공격체 sfx 생성
	// 생성( sfx_파이어볼 );
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT )	// 발사체 sfx 생성.
	{
		CSfxShoot *pShootSfx = NULL;
		DWORD dwShootObj = pSkillProp->dwSfxObj2;
		if( dwShootObj != NULL_ID )
		{
			// CreateShootSfx를 만들자.
			pShootSfx = CreateShootSfx( g_Neuz.m_pd3dDevice, dwShootObj, GetPos(), GetId(), D3DXVECTOR3(.0f,.0f,.0f), idTarget );
			if( pShootSfx )
			{
				D3DXVECTOR3 vPos = GetPos();
				PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
				
				if( IsActiveMover() )		// g_pPlayer가 쏠때만 생성해서 전송.
				{
					idSfxHit = ++m_idSfxHit;	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
					int		nMaxDmgCnt = 1;
					if( pSkillProp->tmContinuousPain != NULL_ID )		// 지속스킬이 있는 데미지 형태면
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.
					
					g_DPlay.SendSfxID( idTarget, idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt );
					pShootSfx->m_idSfxHit	= idSfxHit;
				}
				pShootSfx->SetSkill( nSkill );
//				pShootSfx->m_dwSkillLevel	= dwLevel;
				pShootSfx->m_nMagicPower = dwLevel;		// 패킷이 늘어날거 같아 magicPower로 같이 쓴다.
				pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;	// 폭발 이펙트 등록.
			}
			// 지속시간동안 이펙트도 유지되어야 하는것은 CSkillInfluence::Process()에서 처리한다.
		} else
			Message( "MAGICATKSHOT : %s dwSfxObj2가 지정되지 않음", pSkillProp->szName );
	} 

	CSfx *pSfx = NULL;
	// 타점에 발생되는 이펙트 있다면.
	if( (int)pAddSkillProp->dwSkillTime > 0 )	// 지속시간이 있는것중...
	{
		if( pSkillProp->dwSfxObj4 == NULL_ID )	// 유지되는 이펙트가 아닌건 여기서 dwSfxObj3를 생성.
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )	// 발사체 sfx는 sfx3가 폭발씬이므로 여기서 지금 생성하지 않는다.
				pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj3, GetPos(), GetId(), D3DXVECTOR3(.0f,.0f,.0f), idTarget );
	} else
	// 지속시간이 없는 스킬은 타점에서 바로 생성.
	{
		if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )	// 발사체 sfx는 sfx3가 폭발씬이므로 여기서 지금 생성하지 않는다.
			pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj3, GetPos(), GetId(), D3DXVECTOR3(.0f,.0f,.0f), idTarget );
	}
	if( pSfx )
	{
		D3DXVECTOR3 vPos = GetPos();
		PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
	}
	
	if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		
		if( IsActiveMover() )
			g_DPlay.SendTeleSkill( GetId(), pWndWorld->m_vTelePos );
	}
#endif // CLIENT

	return TRUE;
}
//}}AFX
#endif // not xSkill0425

// 쿨타임 시작!
void	CMover::SetCoolTime( ItemProp *pSkillProp, LPCTSTR szCall )
{
	int nIdx = GetSkillIdx( pSkillProp->dwID );		// 스킬리스트 인덱스를 찾음.
	
	if( nIdx < 0 || nIdx >= MAX_SKILL_JOB )
		Message( "szCall SetCoolTime : %d %d스킬을 찾을 수 없음 %s", nIdx, pSkillProp->dwID, GetName() );
	else
		m_tmReUseDelay[ nIdx ] = pSkillProp->dwSkillReady;	// 1/1000단위
}

//
//	마법 스킬의 타점때 호출.
//  nCount 워터볼의 경우 여러번 호출이 되기때문에 nCount==0 의 경우만 경험치가 올라간다.
//
BOOL CMover::OnMagicSkill( int nType, int nCount )
{
	if( m_oaAct != OBJACT_USESKILL )	return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.
	int		nSkill = GetActParam(0);
	OBJID	idTarget = GetActParam(1);
	DWORD dwLevel = GetActParam( 3 );
	
	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	LPSKILL pSkill	= NULL;
	
	BOOL bRet = GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "CMover::OnMagicSkill" );
	if( bRet == FALSE )
	{
		Message( "OnMagicSkill : 스킬프롭읽기 실패함. 현재진행중인 Motion=%d %d", m_dwMotion, dwLevel );
		return FALSE;
	}
	
	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )		return FALSE;

	if( pSkillProp->dwID == SI_ASS_HEAL_HEALING )	// 힐링 상대가 죽었으면 취소
	{
		if( pFocusObj->IsDie() )
			return FALSE;
	}

	BOOL bSuccess = FALSE;
#ifdef __WORLDSERVER
 #ifdef __XPXP0303
	if( IsPlayer() )
	{
		int nSkillIdx = GetSkillIdx( nSkill );
		if( nSkillIdx >= 0 )
		{
			LPSKILL pSkill = GetSkill( nSkillIdx );
			DecPXP( pAddSkillProp->dwExp, nSkillIdx );		// 일단 PXP를 깎음.
			if( pSkill && IsPlayer() )
				((CUser*)this)->AddSetSkillExp( nType, nSkillIdx, m_nExp2, pSkill->dwExp );
		}
	}
 #endif
#endif	// __WORLDSERVER
	
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT  
#ifdef __XSKILL0425
		|| pSkillProp->dwExeTarget == EXT_MAGICSHOT 		// 쏘는 형태만 sfx발생.
#endif
		)	
		bSuccess = ShootSkill( pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	else
	{
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
		int nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp );
		if( nDamage == 0 )
			return FALSE;
		bSuccess = (BOOL)nDamage;

#ifdef __CLIENT		
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( IsActiveMover() )
			{
				D3DXVECTOR3 vStart;
				D3DXVECTOR3 vEnd;

				vStart = GetPos();		vStart.y += 1.0f;
				vEnd = pWndWorld->m_vTelePos;
				
				CWorld *pWorld = GetWorld();
				
				if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
					g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
					return FALSE;
				}
			
				g_DPlay.SendTeleSkill( GetId(), pWndWorld->m_vTelePos );
			}
		}
#endif // Client
		
	}
	TRACE( "OnMagicSkill%s, ", pSkillProp->szName );
	
	if( bSuccess )	// 스킬사용 실패하면 마나안깎임. / 경험치 안오름.
	{
#ifdef __WORLDSERVER
		// 발사한 순간에 마나등을 깍는다.
		int nReqMp = GetReqMp( pAddSkillProp->nReqMp );
		if( pAddSkillProp->nSkillCount > 1 )
			nReqMp /= pAddSkillProp->nSkillCount;
		IncManaPoint( -nReqMp );
		if( pAddSkillProp->nReqFp != NULL_ID )
			IncFatiguePoint( -GetReqFp( pAddSkillProp->nReqFp ) );
		if( nCount == 0 )
		{
#ifdef __NPP_050308
			if( pSkillProp->dwID != SI_ASS_HEAL_RESURRECTION )
#endif //__NPP_050308
			{
				int nSkillIdx = GetSkillIdx( nSkill );
				if( nSkillIdx >= 0 )
				{
					IncSkillExp( nType, nSkillIdx );			// skill exp
				}
			}
		}
#endif // WORLDSERVER
		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
#ifdef __CLIENT
#ifdef __YMOTIONHITCOUNT
		if( IsActiveMover() && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
#else //__YMOTIONHITCOUNT
		if( IsActiveMover() )
#endif //__YMOTIONHITCOUNT
#endif // __CLIENT
			if( pSkillProp->dwSkillReady != 0xffffffff )	// 쿨타임이 있는 스킬의 경우
				SetCoolTime( pSkillProp, "OnMagicSkill" );
		
	} // bSuccess

#if defined(__CLIENT) && defined(__YSPARK)
	CModelObject* pModel = (CModelObject*)m_pModel;
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;
		
		AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, GetAngle(), -1.0f );
	}
#endif //defined(__CLIENT) && defined(__YSPARK)

	return TRUE;
} // OnMagicSkill


#ifdef	__ACROBAT_0504
//
//	장거리무기가 발사되는 시점에 발생하는 이벤트
//
BOOL	CMover::OnAttackRange( void )
{
//#ifdef __CLIENT
	int		idTarget = GetActParam( 0 );
	DWORD	dwItemID = GetActParam( 1 );		// 사용할 아이템 아이디 (player이면 충전레벨)
	int		idSfxHit = GetActParam( 2 );		// 몬스터가 사용한것이면 여기에 서버로부터 받은값이 들어있다.
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	
	if( IsInvalidObj(pHit) )	
		return FALSE;							// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.
	
	ItemProp *pItemProp = NULL;
	if( IsPlayer() )
		pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else 
	{
		if( dwItemID == NULL_ID || dwItemID == 0 )	
			pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
		else
			pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	}

	if( pItemProp == NULL )
		return FALSE;

	DWORD dwSfxObj = pItemProp->dwSfxObj2;
	
	D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
	D3DXVECTOR3 vPosSrc  = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

	CSfx* pSfx = NULL;
	// 아크로뱃은 이펙트 출력이 항상 왼손에...
	if( IsPlayer() )		
	{
		ItemProp *pItemElem = GetActiveHandItemProp( PARTS_BULLET );  // 화살아이템을 참조로 화살이펙트 생성
		if( pItemElem && pItemElem->dwSfxObj2 != NULL_ID )
		{
			/*
			// 화살은 왼손에 연결...
			D3DXVECTOR3 vPos;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// 주먹 월드좌표 구함	
			
			pSfx = CreateSfx( D3DDEVICE, pItemElem->dwSfxObj2, vPos,GetId(), vPosDest ,idTarget );

			if( pItemProp->dwSfxObj3 != NULL_ID )
			{
				if( pSfx ) 
				{
					((CSfxShoot*)pSfx)->m_dwSfxHit = pItemProp->dwSfxObj3;
				}
			}
			*/
#ifdef __CLIENT
			if( IsActiveMover() )
			{
				pSfx = CreateSfxArrow( pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
			}
#endif //__CLIENT
#ifdef __WORLDSERVER
			g_UserMng.AddCreateSfxAllow( this, pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
#endif //__WORLDSERVER
		}
	}
	else
	{
		if( dwSfxObj == NULL_ID )	
			return FALSE;
		
		pSfx = CreateSfx( D3DDEVICE, dwSfxObj, vPosSrc,GetId(), vPosDest ,idTarget );
	}

#ifdef __CLIENT
	if( pSfx )
	{
		if( IsActiveMover() )		
		{
			DWORD dwAF = AF_GENERIC;
			if( IsPlayer() )
				dwAF |= AF_RANGE;			// 플레이어만 원거리 데미지 공식을 적용케한다.

			idSfxHit = ++m_idSfxHit;		// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, dwAF );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = IsPlayer() ? dwItemID : 0;		// 충전레벨도 넣어준다.
		}
		// 쏜놈이 g_pPlayer거나 / 쏜놈은 몹이고 맞는놈이 g_pPlayer면
		if( IsActiveMover() || (IsNPC() && pHit->IsActiveMover()) )
			pSfx->m_idSfxHit	= idSfxHit;		// id를 등록함
	}
#endif //__CLIENT

//#endif	// __CLIENT
	return TRUE;
}

#else  // !__ACROBAT_0504
//
//	장거리무기가 발사되는 시점에 발생하는 이벤트
//
BOOL	CMover::OnAttackRange( void )
{
	int		idTarget = GetActParam( 0 );
	DWORD	dwItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	int		idSfxHit = GetActParam( 2 );	// 몬스터가 사용한것이면 여기에 서버로부터 받은값이 들어있다.
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	
	if( IsInvalidObj(pHit) )	return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.
	
	DWORD dwSfxObj = NULL_ID;
	ItemProp *pItemProp;

	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)

	dwSfxObj = pItemProp->dwSfxObj;
	if( dwSfxObj == NULL_ID )	return FALSE;	

	D3DXVECTOR3 vPosDest = pHit->GetPos(); 

	// 발사 목표지점은 상대의 중앙.
	float fHeight = fabs(pHit->m_pModel->m_vMax.y/* - pHit->m_pModel->m_vMin.y*/) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
	fHeight *= pHit->GetScale().x;
	vPosDest.y += fHeight;	
	D3DXVECTOR3 vPosSrc  = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
	
#ifdef __CLIENT
	CSfx* pSfx = CreateSfx( D3DDEVICE, dwSfxObj, vPosSrc,GetId(), vPosDest ,idTarget );
	if( pSfx )
	{
		if( IsActiveMover() )		// g_pPlayer만 아래짓을 하게 한다.
		{
			idSfxHit = ++m_idSfxHit;		// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, AF_GENERIC );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = 0;								// 레인지는 충전레벨이 없다.
		}
	}
	
	// 쏜놈이 g_pPlayer거나 / 쏜놈은 몹이고 맞는놈이 g_pPlayer면
	if( IsActiveMover() || (IsNPC() && pHit->IsActiveMover()) )
		pSfx->m_idSfxHit	= idSfxHit;		// id를 등록함.
	//	memcpy( pSfx->m_auRHit, m_m_pActMover->m_auRHit, sizeof(u_short) * 3 );
	//	GetWorld()->AddObj( pSfx );
#endif	// not WORLDSERVER
	
	return TRUE;
}
#endif // !
//
//	완드(매직)어택이 발사되는 시점에 발생하는 이벤트.
//
BOOL	CMover::OnAttackMagic( void )
{
	int		idTarget = GetActParam( 0 );
	int		nMagicPower = GetActParam( 1 );
//	int		idSfxHit	= GetActParam( 2 );
	int		idSfxHit;
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.

	if( IsInvalidObj(pHit) )	return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.
//	if( pHit->IsLive() == FALSE )		return FALSE;	// 타겟이 죽어있는 상태라고 해도 걍 날아가게 하는게 멋질거 같다..

	DWORD dwWeaponId;
#ifndef __ONLINE
	CItemElem* pWeapon = GetWeaponItem();
	if( pWeapon == NULL )	return FALSE;	// 타겟을 클릭했다가 아이템을 벗겨버렸다거나 했을땐 이런현상이 생길것 같다.
	dwWeaponId	= pWeapon->m_dwItemId;
#else	// __ONLINE
	if( IsActiveMover() ) {
		CItemElem* pWeapon = GetWeaponItem();
		if( pWeapon == NULL )	return FALSE;
		dwWeaponId	= pWeapon->m_dwItemId;
	}
	else {
#ifdef __FINITEITEM0705
		dwWeaponId	= m_aEquipInfo[PARTS_RWEAPON].dwId;
#else	// __FINITEITEM0705
		dwWeaponId	= m_adwEquipment[PARTS_RWEAPON];
#endif	// __FINITEITEM0705
		if( dwWeaponId == NULL )	return FALSE;
	}
#endif	// __ONLINE

#ifdef __CLIENT
	if( IsActiveMover() )
	{
		CItemElem* pWeapon = GetWeaponItem();
		ItemProp* pItemProp		= pWeapon->GetProp();
		if( pItemProp )
			if( ( ( pWeapon->m_nHitPoint * 100 ) / pItemProp->dwEndurance ) == 0 )
				g_WndMng.PutString( prj.GetText( TID_GAME_WASTEENDURANCE ), NULL, prj.GetTextColor( TID_GAME_WASTEENDURANCE ) );
	}
#endif	// __CLIENT

	D3DXVECTOR3 vPosDest = pHit->GetPos(); 
	// 발사 목표지점은 상대의 중앙.
	float fHeight = fabs(pHit->m_pModel->m_vMax.y/* - pHit->m_pModel->m_vMin.y*/) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
	fHeight *= pHit->GetScale().x;
	vPosDest.y += fHeight;	

	D3DXVECTOR3 vPosSrc  = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

#ifndef __WORLDSERVER
	ItemProp *pHandItemProp = GetActiveHandItemProp( PARTS_RWEAPON );
	CSfx* pSfx = NULL;
	if( m_pActMover->IsFly() )
	{
		pSfx	= CreateSfx( D3DDEVICE, XI_FIR_WAND02, vPosSrc, GetId(), vPosDest, idTarget );
		if( pSfx )
//			pSfx->ShootSfx( GetAngle(), -GetAngleX(), 1.0f );
			pSfx->ShootSfx( GetAngle(), -GetAngleX(), 0.75f );		// 
	} else
	{
		DWORD dwSfxObj2 = XI_FIR_WAND01;
		if( pHandItemProp )
			dwSfxObj2 = pHandItemProp->dwSfxObj2;

		pSfx	= CreateSfx( D3DDEVICE, dwSfxObj2, vPosSrc,GetId(), vPosDest ,idTarget );
		if( pSfx )
		{
			switch(nMagicPower) 
			{
			case 1:		pSfx->SetScale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );	break;
			case 2:		pSfx->SetScale( D3DXVECTOR3( 1.7f, 1.7f, 1.7f ) );	break;
			case 3:
			case 4:		pSfx->SetScale( D3DXVECTOR3( 2.5f, 2.5f, 2.5f ) );	break;
			}
			((CSfxItemWandAtk1 *)pSfx)->m_fCenter = fHeight;		// << 이따우로 캐스팅해서 쓰지말자. 완드클래스가 바뀌면 우짤라 카노.
		}
	}

	if( pSfx )
	{
		if( IsActiveMover() )		// g_pPlayer만 아래짓을 하게 한다.
		{
			idSfxHit = ++m_idSfxHit;	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, AF_MAGIC );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = nMagicPower;		// 충전레벨도 넣어준다.
		}
	}

#endif	// not __WORLDSERVER

#if defined(__CLIENT) && defined(__YSPARK)
	CModelObject* pModel = (CModelObject*)m_pModel;
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;
		
		AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, GetAngle(), -1.0f );
	}
#endif //defined(__CLIENT) && defined(__YSPARK)
	
	return TRUE;
}


void	CMover::OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
	UpdateMatrix( FALSE );		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	CModelObject *pModel = (CModelObject *)m_pModel;
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:		// 칼로 내려치기
		{
			D3DXVECTOR3 vPos;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 0 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );
//			vPos.y -= 10.0f;

			OnDamage( nItemID, dwState, NULL, &vPos );		// 이펙트가 발생하는 위치에 데미지를 주고
#ifdef __CLIENT
			GetWorld()->m_pCamera->SetQuake( 15, 0.5f );
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRFIRESP, vPos );	// 이펙트를 발생 - 서버에선 생성 안됨.
			if( pSfx )
				pSfx->SetAngle( -GetAngle() );
#endif 
		}
		break;
	case OBJSTA_ATK2:		// 포쏘기
		{
			D3DXVECTOR3 vPos;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 1 );	// 왼손 집게발 위치.
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCANNON, vPos, GetId() );	// 발사이펙트.
			if( pSfx )
			{
				pSfx->SetAngle( -GetAngle() );
				pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
			}
			CMover *pTarget = pHitObj;
			if( IsValidObj(pTarget) )
			{
				OnDamage( nItemID, dwState, NULL, &pTarget->GetPos() );		// 타겟의 좌표를 중심으로 데미지를 줌.
#ifdef __CLIENT
				pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1RANBALL, vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1 );		// 포탄
				if( pSfx )
				{
					pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
					{
						FLOAT fAngXZ, fAngH;
						D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
						xGetDegree( &fAngXZ, &fAngH, vDist );		// 목표와의 각도구함.
						((CSfxAtkStraight *)pSfx)->ShootSfx( fAngXZ, -fAngH, 2.0f, XI_NPCSP1RANBOOM );		// 날아가는 방향 설정.
					}
				}
#endif
			}
		}
		break;
	case OBJSTA_ATK3:
#ifdef __CLIENT
		GetWorld()->m_pCamera->SetQuake( 30, 0.7f );
#endif
		break;
	case OBJSTA_ATK4:	// 발로 밟기.
		{
			D3DXVECTOR3 vPos;

			vPos = GetPos();
			DWORD dwFlag = AF_GENERIC;
			if( xRandom(100) < 15 )
				dwFlag |= AF_FLYING;
			OnDamage( nItemID, dwState, NULL, &vPos, dwFlag );		// 데미지는 몸중심을 기준으로 한다.

			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 3 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

#ifdef __CLIENT
			GetWorld()->m_pCamera->SetQuake( 60, 1.5f );
#endif

			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
			pModel->GetEventPos( &vPos, 4 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

//			OnDamage( nItemID, dwState, NULL, &vPos );		// 이펙트가 발생하는 위치에 데미지를 주고
			
			pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
		}
		break;
	}
}

// 데미지를 적용하는 부분. - 공격방식에 따라서 타점한번에 어러번 호출될 수도 있다.
// pHitObj : 데미지를 먹을 대상. NULL이면 vDamagePos의 좌표를 중심으로 데미지를 준다.
BOOL	CMover::OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos, DWORD dwAtkFlag )
{
	int nParam = 0x01;

	if( IsPlayer() && IsDualWeapon() )		// 쌍칼모드냐?
	{
		if( m_pActMover->m_dwAtkFlags & AF_GENERIC )	// 평타면
		{
			switch( dwState )
			{
			case OBJSTA_ATK1:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK2:	nParam |= 0x02;		break; // 왼손
			case OBJSTA_ATK3:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK4:	nParam |= 0x03;		break; // 양손
			}
		}
	}


#if defined(__CLIENT) && defined(__YSPARK)	
	if( IsValidObj(pHitObj) )
	{
		CModelObject* pHitModel = (CModelObject*)pHitObj->m_pModel;
		if( pHitModel )
		{
			D3DXVECTOR3 vDir;
			pHitModel->m_SparkInfo.m_bUsed = TRUE;
			pHitModel->m_SparkInfo.m_v3Color.x = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.y = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.z = 2.0f;
			pHitModel->m_SparkInfo.m_nCnt = 0;
			pHitModel->m_SparkInfo.m_fLerp= 1.0f;

			vDir = pHitObj->GetPos() - GetPos();
			D3DXVec3Normalize( &pHitModel->m_SparkInfo.m_v3SparkDir, &vDir );
		}
	}
#endif //defined(__CLIENT) && defined(__YSPARK)	
	
	int	nDamage = 0;
	if( IsPlayer() )
	{
		// 플레이어
		nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), nParam );
	}
	else
	{
		// 몬스터
		ItemProp *pItemProp = prj.GetItemProp( nItemID );
		if( pItemProp )
		{
			if( pItemProp->dwSpellRegion == SRO_AROUND )
			{
				int nType;
//#ifdef __INTERNALSERVER
//				nType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;		// 플레이어들만 데미지.
//#else
				nType = OBJTYPE_PLAYER;		// 플레이어들만 데미지.
//#endif
//				nType = OBJTYPE_MONSTER;		// 플레이어들만 데미지.
				
				if( pHitObj )
					pHitObj->SendDamageAround( dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange, 1.0f, 1.0f ); 
				else
					GetWorld()->SendDamageAround( pvDamagePos, dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange ); 
			} else
			{
				if( pHitObj )
					nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), 1 );
			}
		} else
		{
			if( pHitObj )
				nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), 1 );
		}
	}


 #ifdef __WORLDSERVER
	if( nDamage > 0 )		// THIS가 공격했는데 데미지 먹이는데 성공했다
	{
		if( pHitObj->m_pActMover->GetState() & OBJSTA_ATK_MELEESKILL )	// 맞는놈이 스킬 모션중이었다.
		{
			DWORD dwSkill = pHitObj->GetActParam(0);		// 현재 사용하고 있는 스킬. dwSkill
			if( dwSkill == SI_MER_ONE_REFLEXHIT )			// 리플렉히트 스킬을 쓰고 있는 중에 맞았다.
			{
				pHitObj->m_nReflexDmg += nDamage;			// 리플렉데미지는 쌓임.
			}
		}
	}
 #endif
	return nDamage > 0;
}

//
// 일반 공격의 타점때 호출되는 핸들러.
BOOL	CMover::OnAttackMelee( DWORD dwState, CMover *pHitObj )
{
	CModelObject	*pModel = (CModelObject *)m_pModel;
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_ATTACK_MELEE );
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_ATTACK_MELEE );
#endif	// __POSTAIMSG1008
#endif
	
 	if( m_dwIndex == MI_CLOCKWORK1 )	// 클락워크의 경우 때릴때 이펙트.
	{
		OnAttackMelee_ClockWorks( dwState, pHitObj );
		return TRUE;;
	}

	BOOL bSuccess = FALSE;
	BOOL bDamage = TRUE;

#ifdef __WORLDSERVER
	if( IsNPC() && pHitObj->IsPlayer() && pHitObj->IsRangeObj( this, 10.0f ) == FALSE )		// 두번죽는 경우땜에 넣어봤다. 몹->유저 10미터 이상 떨어져있을때는 안맞게.
		bDamage = FALSE;
#endif
	if( bDamage )
	{
		bSuccess = OnDamage( nItemID, dwState, pHitObj );	// 데미지를 준다.
	}

#ifdef __WORLDSERVER
	if( bSuccess )
	{
		if( IsPlayer() )
		{
			if( dwState == OBJSTA_ATK1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
				((CUser*)this)->m_playTaskBar.m_nActionPoint ++;
		}
	}
#endif	// worldserver

#ifdef __CLIENT
	if( bSuccess )		// 타격에 성공했다면 타격음을 플레이.
	{
		if( CMover::GetActiveMover() == this )
		{
			if( dwState == OBJSTA_ATK1 && g_WndMng.m_pWndTaskBar->m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
				g_WndMng.m_pWndTaskBar->m_nActionPoint ++;
		}
		//  플레이어는 들고 있는 무기에 따라 타격 음이 다르다.
		ItemProp* pItemProp = GetActiveHandItemProp();
		if( pItemProp )
			PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
		
		if( IsPlayer() )
		{
			pModel->m_nPause = 5;	// frame 멈춤

#ifdef __XSLOW1018			
			int nMin, nTemp;
			GetHitMinMax( &nMin, &nTemp );
			if( dwState == OBJSTA_ATK3 || dwState == OBJSTA_ATK3 )
				if( GetLevel() >= 12 && pHitObj->GetHitPoint() < (int)(nMin * 1.5f) )
					pModel->m_bSlow = TRUE;
#endif // xSlow1018
//			pHitObj->m_pModel->m_nPause = 4;
		}
		else
		{
			pModel->m_nPause = 0;	// 몬스터는 멈추지 않음
			pHitObj->m_pModel->m_nPause = 0;
		}

#if defined(__CLIENT) && defined(__YSPARK)
		{
			if( IsValidObj( pHitObj ) )
			{
				pModel->m_SparkInfo.m_bUsed  = TRUE;
				pModel->m_SparkInfo.m_v3Color.x = 2.0f;
				pModel->m_SparkInfo.m_v3Color.y = 2.0f;
				pModel->m_SparkInfo.m_v3Color.z = 2.0f;
				pModel->m_SparkInfo.m_nCnt = 0;
				pModel->m_SparkInfo.m_fLerp= 1.0f;
				
				D3DXVECTOR3 vDir = GetPos() - pHitObj->GetPos();
				D3DXVec3Normalize( &pModel->m_SparkInfo.m_v3SparkDir, &vDir );
			}
		}
#endif //defined(__CLIENT) && defined(__YSPARK)
		
		
	}
#endif // client
	
	
	return TRUE;
}

//
//
// 타점 이후부터 끝날때까지 계속 호출.
BOOL	CMover::OnAttackMeleeContinue( DWORD dwState )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

 #ifdef __WORLDSERVER
	UpdateMatrix( FALSE );		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	CModelObject *pModel = (CModelObject *)m_pModel;
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	if( m_dwIndex == MI_CLOCKWORK1 )	// 클락워크의 경우 때릴때 이펙트.
	{
		switch( dwState )
		{
		case OBJSTA_ATK3:
			{
				D3DXVECTOR3 v1, v2;
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetEventPos( &v1, 2 );		// 레이저 나가는 부분.
				D3DXVec3TransformCoord( &v1, &v1, GetMatrixWorldPtr() );	// 월드공간으로 변환.
				
				v2 = D3DXVECTOR3( 0, 0, 40.0f );
				D3DXVec3TransformCoord( &v2, &v2, pModel->GetMatrixBone(6) );	// 6이 머리.  머리에서 v2지점의 위치를 계산.
				D3DXVec3TransformCoord( &v2, &v2, GetMatrixWorldPtr() );	// 월드 공간으로 변환.
				
#ifdef __CLIENT
				extern CPartsLaser g_Laser;
				g_Laser.SetPos( v1, v2, 0.5f );
#endif
#ifdef __CLIENT
				if( (m_pActMover->m_nCount & 1) == 0 )
#else				
				if( (m_pActMover->m_nCount & 3) == 0 )
#endif
				{
					CWorld *pWorld = GetWorld();
					D3DXVECTOR3 vOut;
					if( pWorld->IntersectObjLine( &vOut, v1, v2, FALSE, TRUE, FALSE ) )
					{
						OnDamage( nItemID, dwState, NULL, &vOut );		// 이펙트가 터지는 위치에 데미지를 줌.
//#ifdef __INTERNALSERVER
#ifdef __WORLDSERVER
#ifdef __XUZHU
						// 테스트용이다.
//						g_UserMng.AddCreateSfxObj( this, XI_NPCSP1RANSPARK, vOut.x, vOut.y, vOut.z );
#endif
#endif
//#else
						CSfx *pSfx = CreateSfx( D3DDEVICE,  XI_NPCSP1RANSPARK, vOut );
						if( pSfx )
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
//#endif
					}
						
					//						FLOAT fHeight = pWorld->GetFullHeight( v2 );
//						v2.y = fHeight;
				}
			}
			break;
		}
	}
	return TRUE;
}

// 몬스터의 특수공격 타격부분에 호출
BOOL	CMover::OnAttackSP( void )
{
	int		idTarget = GetActParam( 0 );
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj(pHit) )	return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.

	ItemProp *pItemProp;
	if( nItemID == NULL_ID || nItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( nItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)

	if( pItemProp == NULL )
	{
		Message( "CMover::OnAttackSP : %s의 itemProp이 없다. %d", m_szName, nItemID );
		return FALSE;
	}

	if( pItemProp->dwSpellRegion == SRO_AROUND )
	{	
		SendDamageAround( AF_GENERIC, this, OBJTYPE_PLAYER, nItemID, pItemProp->_dwSkillRange, 0.0, 1.0f );	// 범위공격스따일
	} else
	{
		if( pHit->IsLive() )
			pHit->m_pActMover->SendDamage( AF_GENERIC, GetId(), (nItemID << 16) );	// 직접 공격 스따일.
	}
	

	return TRUE;
}

//
//	캐릭터가 높은곳에서 떨어진 시점에 호출
//
void	CMover::OnActDrop( void )
{
#ifdef __WORLDSERVER
//#ifdef __XUZHU
	if( m_pActMover->GetDmgState() & OBJSTA_DMG_FLY_ALL )		// 데미지 플라이땐 이거 안하는게 좋다.
		return;
//#endif
	if( FALSE == IsPlayer() )
	{
		if( m_nCorr < 0 )
		{
//			ClearDest();
			ClearDestPos();
			SendActMsg( OBJMSG_STAND );
			if( m_pActMover->IsFly() )
				g_UserMng.AddMoverCorr2( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(),
					m_pActMover->m_fAccPower, m_pActMover->m_fTurnAngle, m_pActMover->GetState(), m_pActMover->GetStateFlag(),
					m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			else
				g_UserMng.AddMoverCorr( this, GetPos(), m_pActMover->m_vDelta, GetAngle(),
					m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx,
					m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			m_nCorr		= 0;
		}
	}
	else
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			( (CUser*)this )->AddQueryGetPos( NULL_ID );
			m_fWaitQueryGetPos	= TRUE;
		}
	}
#else	// __WORLDSERVER
 #ifdef __ONLINE
	if( TRUE == IsActiveMover() )
	{
		if( m_nCorr != 0 )
		{
			if( m_nCorr < 0 ) {
				ClearDest();
				SendActMsg( OBJMSG_STAND );
			}
			if( m_pActMover->IsFly() )
				g_DPlay.SendPlayerCorr2();
			else
				g_DPlay.SendPlayerCorr();
			m_nCorr		= 0;
		}
	}
	else
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			g_DPlay.SendQueryGetPos( this );
			m_fWaitQueryGetPos	= TRUE;
		}
	}
 #endif	// __ONLINE
#endif	// not __WORLDSERVER
}

void	CMover::OnActCollision( void )
{
#ifdef __WORLDSERVER
	if( FALSE == IsPlayer() )
	{
		if( m_nCorr < 0 )
		{

			ClearDest();
			SendActMsg( OBJMSG_STAND );

			if( m_pActMover->IsFly() )
				g_UserMng.AddMoverCorr2( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(),
					m_pActMover->m_fAccPower, m_pActMover->m_fTurnAngle, m_pActMover->GetState(), m_pActMover->GetStateFlag(),
					m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			else
				g_UserMng.AddMoverCorr( this, GetPos(), m_pActMover->m_vDelta, GetAngle(),
					m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx,
					m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );

			m_nCorr		= 0;

#ifdef __POSTAIMSG1008
			PostAIMsg( AIMSG_COLLISION );
#else	// __POSTAIMSG1008
			SendAIMsg( AIMSG_COLLISION );
#endif	// __POSTAIMSG1008
		}
	}
	else
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			( (CUser*)this )->AddQueryGetPos( NULL_ID );
			m_fWaitQueryGetPos	= TRUE;
		}
	}

#else	// __WORLDSERVER
#ifdef __ONLINE
	if( TRUE == IsActiveMover() )
#endif	// __ONLINE
	{
#ifdef __ONLINE
		if( m_nCorr != 0 )
#endif	// __ONLINE
		{
#ifdef __ONLINE
			if( m_nCorr < 0 )
#endif	// __ONLINE
			{
				ClearDest();
				SendActMsg( OBJMSG_STAND );
#ifdef __POSTAIMSG1008
				PostAIMsg( AIMSG_COLLISION );
#else	// __POSTAIMSG1008
				SendAIMsg( AIMSG_COLLISION );
#endif	// __POSTAIMSG1008
	//			tmp
				CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
				if( pWndWorld )
					pWndWorld->m_bLButtonDown	= FALSE;
			}
			if( m_pActMover->IsFly() )
				g_DPlay.SendPlayerCorr2();
			else
				g_DPlay.SendPlayerCorr();

//			TRACE( "OnActCollision(): SendPlayerCorr():\n" );

#ifdef __J0823
			g_DPlay.ClearPlayerDestPos();
#endif	// __J0823

			m_nCorr	= 0;
		}
	}
#ifdef __ONLINE
	else if( FALSE == IsActiveMover() )
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			g_DPlay.SendQueryGetPos( this );
			m_fWaitQueryGetPos	= TRUE;
		}
	}
#endif	// __ONLINE

#endif	// __WORLDSERVER
}

// 근접스킬 동작 끝났을때.
void	CMover::OnActEndMeleeSkill( void )
{
	TRACE( "OnActEndMeleeSkill, " );
//	ClearDestObj();				// 여기서 이걸 굳이 클리어 해줄필요는 없을거 같다. 이미 멈춰있는 상태에서 스킬을쓰다 끝난거기땜시.
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
#ifdef __XSYNC0711
#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		// OnActEndMeleeSkill()에도 아래코드 넣어야함.
		//
		if( ((CUser *)this)->m_playTaskBar.m_nUsedSkillQueue != -1 )
		{
			((CUser *)this)->m_playTaskBar.SetNextSkill( (CUser *)this );
		}
	}
#endif 
#else
#ifdef __CLIENT
	if( IsActiveMover() )
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		pTaskBar->OnEndSkill();
	}
#endif
#endif // not xSync0711
	
}

// 마법스킬 동작 끝났을때.
void	CMover::OnActEndMagicSkill( void )
{
//	ClearDestObj();
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
#ifdef __XSYNC0711
 #ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		// OnActEndMeleeSkill()에도 아래코드 넣어야함.
		//
		if( ((CUser *)this)->m_playTaskBar.m_nUsedSkillQueue != -1 )
		{
			((CUser *)this)->m_playTaskBar.SetNextSkill( (CUser *)this );
		}
	}
 #endif 
#else
 #ifdef __CLIENT
	if( IsActiveMover() )
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		pTaskBar->OnEndSkill();
	}
 #endif
#endif // not xSync0711
	
}

// 착지(JUMP4)플래그가 사라지는 순간.
void	CMover::OnActEndJump4( void )
{
	if( FALSE == IsEmptyDestPos() )
		SetDestPos( GetDestPos(), m_bForward, FALSE );

	if( m_dwIndex == MI_CLOCKWORK1 )
	{
		SendActMsg( OBJMSG_APPEAR );
	}
}

// 착지 하는 순간에 발생.
void	CMover::OnActLanding( void )
{
#ifdef __CLIENT	
	if( m_dwIndex == MI_CLOCKWORK1 )
	{
		GetWorld()->m_pCamera->SetQuake( 60, 1.0f );		// 보스가 떨어지면 땅이 흔들림.
	}
#endif
}

// 전진 하는 순간에 발생.
void	CMover::OnActFMove( void )
{
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_BEGINMOVE );
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_BEGINMOVE );
#endif	// __POSTAIMSG1008
}

// Interact 오브젝트에 착지 했을때.
// pIAObj : 어떤 IA오브젝트에 착지했는가.
// vPos : 착지한 this의 좌표.
void	CMover::OnActIALanding( CObj *pIAObj, const D3DXVECTOR3 &vPos )
{
	if( pIAObj->GetType() != OT_SHIP )	
		return;
	CShip *pIA = (CShip *)pIAObj;

	CObj *pOldIA = GetIAObjLink();
	SetIAObjLink( pIA );
	
	if( IsActiveMover() )
	{
		if( pIA != pOldIA )	// 걸어다닐때도 계속 갱신되기땜에 매번 보낼필요는 없다.
		{
#ifdef __CLIENT			
			D3DXVECTOR3 vLocal = GetPos() - pIA->GetPos();		// IA오브젝트로부터의 상대 좌표를 뽑음.
			g_DPlay.SendLocalPosFromIA( vLocal, pIA->GetId() );				// 상대좌표 서버로 전송
			TRACE( "SendLocalPosFromIA\n" );
#endif //__CLIENT
		}
	}

}

int		CMover::OnActCollecting( void ) 
{ 
#ifdef __CLIENT
	if( (m_dwFlag & MVRF_COLLECT) == 0 )
	{
		ItemProp *pHandProp = GetActiveHandItemProp();
		if( pHandProp->dwSfxObj2 != NULL_ID )
		{
			D3DXVECTOR3 vSrc, vLocal = D3DXVECTOR3(0, 0.5f, 0);
			((CModelObject *)m_pModel)->GetForcePos( &vLocal, 0, PARTS_RWEAPON, GetMatrixWorld() );
			vSrc = vLocal;
			CSfx *pSfx = CreateSfx( D3DDEVICE, pHandProp->dwSfxObj2, vSrc, GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
			if( pSfx )
			{
				pSfx->SetAngle( -GetAngle() + 90.0f );
				m_dwFlag |= MVRF_COLLECT;		// SFX가 생성됐음.
			}
		}
	}
#endif // Client
	return 0; 
}				

// 공격을 막 시작했을때(일반/스킬/완드 등)
void	CMover::OnAttackStart( CMover *pTarget, OBJMSG dwMsg )
{
#ifdef __WORLDSERVER
	#ifdef __ACROBAT_0504
		return;
	#endif //__ACROBAT_0504
	
	RemoveInvisible();
	
#endif // WorldServer
 
}

// this가 투명화 상태 걸려있으면 해제시킴.
void	CMover::RemoveInvisible( void )
{
#ifdef __WORLDSERVER

	if( GetAdjParam( DST_CHRSTATE ) & CHS_INVISIBILITY )		// 투명화 상태다.
	{
		ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );
	}

#ifdef __ACROBAT_0504
	#ifdef __XITEMBUFF
		m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION );		// 해제.				
	#else
		m_SkillState.RemoveSkillInfluence( SI_ACR_SUP_DARKILLUSION );		// 해제.				
	#endif
#endif //__ACROBAT_0504


#endif // __WORLDSERVER
}

#endif	// not BEAST



#ifdef __CLIENT
void CMover::AddMovingActMsg( const D3DXVECTOR3 & vPos, float fAngle, BYTE fMoving, BYTE fForward, 
														DWORD dwMsg, int nParam1, int nParam2 )
{
	m_qMovingActMsg[m_uTail].dwMsg	= dwMsg;
	m_qMovingActMsg[m_uTail].nParam1	= nParam1;
	m_qMovingActMsg[m_uTail].nParam2	= nParam2;
	m_qMovingActMsg[m_uTail].vPos	= vPos;
	m_qMovingActMsg[m_uTail].fAngle	= fAngle;
	m_qMovingActMsg[m_uTail].fMoving	= fMoving;
	m_qMovingActMsg[m_uTail].fForward	= fForward;
	m_uTail	= ( m_uTail + 1 ) % MAX_MOVING_ACTMSG;
}

void CMover::RemoveMovingActMsg( void )
{
	if( m_uHead == m_uTail )
		return;
	LPMOVING_ACTMSG	lpMovingActMsg	= m_qMovingActMsg + m_uHead;
	m_uHead		= ( m_uHead + 1 ) % MAX_MOVING_ACTMSG;
}

LPMOVING_ACTMSG	CMover::PeekMovingActMsg( void )
{
	if( m_uHead == m_uTail )
		return NULL;
	return( m_qMovingActMsg + m_uHead );
}
#endif __CLIENT

// pItem이 루팅가능한가를 판별.
#ifdef __WORLDSERVER
BOOL CMover::IsLoot( CItem *pItem )
{
	BOOL bTake	= FALSE;
	
	if( pItem->IsDelete() || ( pItem->m_idHolder != 0 && pItem->m_idHolder != m_idPlayer ) )
	{
		return bTake;
	}

#if !defined(__NPP_050308)			
	// 크게 나눠 플레이어가 pk당해떨어진 아이템인가 아닌가에 따라 나뉜다.
	if( pItem->m_idFrom != NULL_ID )		// PK당해 떨어진 아이템임.
	{
		// pk로 떨어진 아이템은 m_idLastHitMover가 idFrom인 사람만 집을 권한이 생긴다. 이때는 먼저집는놈이 임자다.
		if( pItem->m_idFrom == m_idLastHitMover )	// pMover가 쳤던(마지막타만)놈이 떨어트린 아이템이면 아무나 집을수 있다.
			bTake = TRUE;
	} else
#endif // !__NPP_050308			
	{
		if( pItem->m_idOwn != NULL_ID )		// 아이템에 주인이 있을때
		{
			// 일반적으로 떨어진 아이템.
			CMover *pOther = prj.GetMover( pItem->m_idOwn );		// pItem을 집을 권한이 있는사람.(가령...막타)
			if( IsValidObj(pOther) )
			{
				if( m_idparty && (pOther->m_idparty == m_idparty) )		// 파티가 있고 나랑 같은 파티원이면 줏을수 있어요~~~ ㅋㅋㅋ
					bTake = TRUE;
			} else
				bTake = TRUE;		// idOwn이 Invalid상태면 아무나 줏을 수 있음.
			
			if( pItem->m_idOwn == GetId() )		// 아이템의 소유권자면 집을 수 있다.
				bTake = TRUE;
			
		} else
			bTake = TRUE;	// 주인이 없는거면 아무나 줏을수 있다.
	}
	if( (timeGetTime() - pItem->m_dwDropTime) >= SEC(40) )	// 20초가 지난건 아무나 줏을 수 있다.
		bTake = TRUE;

	return bTake;
}
#endif // WorldServer

// DoLoot의 하위 - 몹이 떨어트린 아이템을 솔로잉일때 집으려 할때 처리부
BOOL CMover::SubLootDropMobSingle( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER	
	CItemBase* pItemBase = pItem->m_pItemBase;
	bSuccess	= CreateItem( pItemBase );
	
	if( TRUE == IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
			((CUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
		else
			((CUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
#ifdef __WORLDSERVER
	if( pItemBase->GetProp()->nLog != -1 && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}
#endif // __WORLDSERVER

#endif // WorldServer
	return bSuccess;
} // SubLootDropMobSingle

// DoLoot의 하위 - 몹이 떨어트린 아이템을 파티가 있을때 집으려 할때 처리부
BOOL CMover::SubLootDropMobParty( CItem *pItem, CParty *pParty )
{
	BOOL bSuccess = TRUE;;
#ifdef __WORLDSERVER
	CUser *pListMember[ 8 ];
	int	nMaxListMember = 0;
	CUser* pMember	= NULL;
	CUser* pGetUser = NULL;
	float fDist;
	D3DXVECTOR3	vDist;
	CItemBase* pItemBase = pItem->m_pItemBase;
	
	// 아이템줍는사람 반경안에 드는 멤버들만 추려낸다.
	memset( pListMember, 0, sizeof(pListMember) );
	for( int i = 0; i < pParty->m_nSizeofMember; i++ )
	{
		pMember		= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
		if( IsValidObj( (CObj*)pMember ) )
		{
			vDist = pMember->GetPos() - GetPos();
			fDist = D3DXVec3LengthSq( &vDist );		// 아이템 줍는사람과 멤버간의 거리.
			if( fDist < 32.0f * 32.0f )		// xx미터보다 작으면
			{
				pListMember[ nMaxListMember++ ] = pMember;
			}
		}
	}

	if( nMaxListMember == 0 )
	{
		pGetUser	= (CUser*)this;
	}
	else
	{
		switch( pParty->m_nTroupeShareItem )
		{
		case 1 :	// 순서분배

			{
				for( i = 0 ; i < nMaxListMember ; i++ )
				{
					if( pParty->m_nGetItemPlayerId == pListMember[i]->m_idPlayer )
					{
						if( i + 1 >= nMaxListMember )
						{
							pGetUser = pListMember[0];
							break;												
						}
						else
						{
							pGetUser = pListMember[ i+1 ];
							break;
						}
					}
				}
				
				if( pGetUser == NULL )
				{
					pGetUser = pListMember[0];
				}
			}
			break;
		case 2 :	// 수동분배 : 단장에게 다 들어감
			{
				if( pParty->IsLeader( pListMember[0]->m_idPlayer ) )
				{
					pGetUser = pListMember[0];
				}
				else
				{
					pGetUser = (CUser*)this;
				}
			}
			break;
		case 3 :	// 랜덤분배 : 랜덤하게 들어감
			{
				DWORD dwRand = xRandom( nMaxListMember );
				pGetUser = pListMember[dwRand];
			}
			break;
		default:	// 기본 개임분배

			{
				pGetUser = (CUser*)this; 
			}
			break;
		}	// switch
	}	
	pParty->m_nGetItemPlayerId = pGetUser->m_idPlayer;

	bSuccess	= pGetUser->CreateItem( pItemBase );

	for( i = 0; i < nMaxListMember; i++ )
	{
		pMember = pListMember[i];
		if( TRUE == pMember->IsPlayer() && pItemBase->GetProp() != NULL )
		{
			if( bSuccess )
			{
				if( pMember->m_idPlayer != pGetUser->m_idPlayer )
				{
					if( prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN || prj.m_nLanguage == LANG_JAP )
						( (CUser*)pMember )->AddDefinedText( TID_GAME_TROUPEREAPITEM, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
					else
					{
 #ifdef __THAILAND__
						( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM_THA, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
 #else //__THAILAND__
						( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s%s %s\"", pGetUser->GetName(), prj.GetText(TID_GAME_FROM2), pItemBase->GetProp()->szName );
 #endif //__THAILAND__
					}
					//( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s님이 %s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
				}
				else
				{
					( (CUser*)pGetUser )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
				}
			}
			else
			{
				// CreateItem을 실패함 Full
				((CUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
			}
		}									
	}
	
	if( pItemBase->GetProp()->nLog != -1 && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( ( (CItemElem*)pItemBase )->m_nItemNum ) > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = pGetUser->GetName();
					aLogItem.WorldId = pGetUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = pGetUser->GetName();
				aLogItem.WorldId = pGetUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}
#endif // __WORLDSERVER
	return bSuccess;
} // SubLootDropMobParty

//
//
// DoLoot의 하위 - 몹이 떨어트린 아이템 루팅부...
BOOL CMover::SubLootDropMob( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	//	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );	// lock2

	CParty *pParty	= g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer )  )	// 파티가 있으므로 경험치 분배대로 해야함
	{
		// 파티가 있을경우 루팅 처리.
		bSuccess = SubLootDropMobParty( pItem, pParty );
	}
	else // 파티가 없을때~
	{
		bSuccess = SubLootDropMobSingle( pItem );
	}
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
#endif	// __WORLDSERVER

	return bSuccess;
}

// DoLoot의 하위 - 몹이 떨어트린게 아닌! 아이템루팅부...
BOOL CMover::SubLootDropNotMob( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CItemBase* pItemBase = pItem->m_pItemBase;
	
	bSuccess	= CreateItem( pItemBase );
	if( TRUE == IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
			((CUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );						
		else
			((CUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
	if( pItemBase->GetProp()->nLog != -1 && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}
#endif // __WORLDSERVER
	
	return bSuccess;
} // SubLootDropNotMob

//////////////////////////////////////////////////////////////////////////
//
// 바닥에 떨어진 pItem을 갖는다.
//
//////////////////////////////////////////////////////////////////////////
BOOL CMover::DoLoot( CItem *pItem )
{
	if( IsPlayer() == FALSE )	return FALSE;

	CItemBase* pItemBase = pItem->m_pItemBase;
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	if( pItem->GetPosAttr() < 0 )
		return FALSE;

#else	// __WORLDSERVER
	PLAYSND( SND_INF_GROUNDPICKUP );
#endif	// __WORLDSERVER
	
	ItemProp *pItemProp = pItem->GetProp();
	if( pItemProp->dwItemKind1 == IK1_GOLD ) 
	{
		PickupGold( ((CItemElem*)pItemBase)->GetGold(), pItem->m_bDropMob );
	}
	else 
	{
		// <아이템 로그 - 획득>
#ifdef __WORLDSERVER
		if( pItem->m_bDropMob == TRUE ) // 몬스터가 떨어뜨린것만
		{
			// 몹이 떨어트린 아이템에 대한 루팅 처리
			bSuccess = SubLootDropMob( pItem );
		}
		else	// 어떤유저가 떨어뜨린것은 줏은넘이 임자~
		{
			// 몹이 떨어트린게 아닌 아이템의 루팅에 대한 처리
			bSuccess = SubLootDropNotMob( pItem );

		}
#else	// __WORLDSERVER
 #ifndef __ONLINE
		bSuccess	= AddItem( pItemBase );
 #endif	// __ONLINE
#endif	// not __WORLDSERVER
	}

	if( TRUE == bSuccess )
	{
		pItem->Delete();
#ifdef __WORLDSERVER
//		g_UserMng.AddMotion( this, OBJMSG_PICKUP );
		RemoveInvisible();		// 아이템 줏으면 투명 해제.
#endif	// __WORLDSERVER
	}

	return bSuccess;
}

