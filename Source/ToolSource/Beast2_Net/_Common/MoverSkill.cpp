
#include "stdafx.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "defineText.h"
#include "ModelObject.h"
#include "CreateObj.h"
#include "defineobj.h"

#ifdef __XPET
#include "..\_aiinterface\AIPet.h"
#endif

#ifdef __GUILDCOMBAT
#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;
#endif //__GUILDCOMBAT

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
#include "party.h"
extern	CPartyMng	g_PartyMng;
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __BEAST		// 비스트에선 이 모듈이 사용되지 않는다.

#ifdef __V050322_CACHEQUEST
extern void __SetQuest( DWORD dwIdMover, int nQuest );
#endif  // __V050322_CACHEQUEST	

//
// 매직공격 데미지량 계산

//
int		CMover::EstimateMagicHit( CMover* pAttacker )
{
	return 1;
}

#ifdef __WORLDSERVER
// 스킬큐에 들어 있는 마법들의 총 캐스팅 시간을 계산.
int		CMover::GetQueueCastingTime( void )
{
	if( IsNPC() )	return 0;
	CUser *pUser = (CUser *)this;
	int		i;
	SHORTCUT *pSlotQueue = pUser->m_playTaskBar.m_aSlotQueue;
	int		nTotalTime = 0;

	for( i = 0; i < MAX_SLOT_QUEUE; i ++ )
	{
		if( pSlotQueue[i].m_dwShortcut )
		{
			LPSKILL pSkill = GetSkill( 0, pSlotQueue[i].m_dwId );
			
			ItemProp *pSkillProp;
			if( !( pSkillProp = pSkill->GetProp() ) )					// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
			{
				Error( "CMover::GetQueueCastingTime : %s. 스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill );
				return FALSE;	// property not found
			}
			
			AddSkillProp* pAddSkillProp;
			
			if( !( pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel ) ) )					// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
			{
				Error( "CMover::GetQueueCastingTime : %s. 애드스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill );
				return FALSE;	// property not found
			}

#ifdef _DEBUG
			if( (int)pAddSkillProp->dwSkillReady <= 0 )
				Message( "GetQueueCastingTime : %s의 addProp dwSkillReady가 %d", pSkillProp->szName, pAddSkillProp->dwSkillReady );
#endif
			nTotalTime += (int)pAddSkillProp->dwSkillReady;
		}
	}

	nTotalTime = (int)(nTotalTime * 0.7f);		// 70프로 적용한후.
	nTotalTime = (int)( (nTotalTime / 1000.0f) * SEC1 );	// 캐스팅단위 시간으로 변환.

#ifdef __NEWWPN0901
	nTotalTime	= GetCastingTime( nTotalTime );
#endif	// __NEWWPN0901

	return nTotalTime;
}

#endif // worldserver

/*--------------------------------------------------------------------------------------------------------


											Do Use Skill


----------------------------------------------------------------------------------------------------------*/

#ifdef __XSKILL0425
// 최초의 스킬 사용 명령이 내리짐

// pSkill
//		Use Skill
//	Mover가 사용가능한 스킬배열(m_aJobSkill[3+14])에서 nIdx에 해당하는 스킬을 꺼내 그것을 실행한다.
// sutType : 스킬사용시 단축키/스킬큐의 여부를 클라로부터 받아서 처리.  캐스팅타임 계산이 다르다.
//
// 플레이어가 사용하는 버전(신버전)
#ifdef __CLIENT
BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime, DWORD dwSkill, DWORD dwLevel )
#else // __CLIENT
BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType )
#endif // __CLIENT
{
	if( IsNPC() )	return FALSE;		// 엔피씨는 이쪽을 타면 안된다.

#ifdef __YADV_RESKILLPOINT
	if( IsActiveMover() && m_dwSkillPoint )
	{
		return FALSE;
	}
#endif //__YADV_RESKILLPOINT

#ifdef __WORLDSERVER
	DWORD dwSkill = 0, dwLevel;
#endif	// __WORLDSERVER
	
	if( nIdx >= 0 )
	{
		LPSKILL pSkill	= GetSkill( nType, nIdx );
		if( pSkill == NULL )
		{
			Error( "CMover::DoUseSkill : %s는 nIdx에 스킬을 가지고 있지 않다. %d", m_szName, nIdx );
			return FALSE;
		}
		
		dwSkill	= pSkill->dwSkill;
		dwLevel	= pSkill->dwLevel;
	}

#ifdef __WORLDSERVER
	int nCastingTime = 0;
#endif

	ItemProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "DoUseSkill_P" );

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;
	
	if( pSkillProp->dwSkillType == KT_MAGIC )
	{
		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬. - 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		{
	#ifdef __WORLDSERVER
			nCastingTime = GetQueueCastingTime();
	#endif
			dwMsg = OBJMSG_MAGICCASTING;
		} else
		if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬. - 캐스팅 동작없이 시전.
		{
	#ifdef __WORLDSERVER
			nCastingTime = 0;
	#endif
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} else
		{								// 일반적인 마법 스킬 사용 - 캐스팅 동작 있음.
	#ifdef __WORLDSERVER
			nCastingTime = (int)((pAddSkillProp->dwSkillReady / 1000.0f) * SEC1);
#ifdef __NEWWPN0901
			nCastingTime	= GetCastingTime( nCastingTime );
#endif	// __NEWWPN0901
	#endif
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
		

	BOOL bSuccess = DoUseSkill( dwSkill, dwLevel, idFocusObj, sutType, nCastingTime );

	return  bSuccess;
}

//
// 2005.4.25 신 인터페이스
// dwSkill,nLevel만 있으면 어디서든지 사용가능한 버전	
// 몬스터까지 사용가능
BOOL CMover::DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime )
{
	if( IsDie() )	return FALSE;
	if( m_pActMover->IsFly() )	return FALSE;
	if( m_dwFlag & MVRF_NOATTACK )	
		return FALSE;		// 공격금지 상태에선 스킬 못씀.
	
#ifdef __GUILDCOMBAT
	if( GetWorld()->GetID() == WI_WORLD_GUILDWAR
		&& g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )	// 길드전에서 스킬사용중지

	{
		if( g_GuildCombatMng.m_nGuildCombatState == CGuildCombat::WAR_WAR_STATE && m_nGuildCombatState == 0 )
			return FALSE;
	}
#endif // __GUILDCOMBAT


#ifdef __YADV_RESKILLPOINT
	if( IsActiveMover() && m_dwSkillPoint )
	{
		return FALSE;
	}
#endif //__YADV_RESKILLPOINT	
	
#ifdef __SCHOOL0701
	if( IsMode( ATTACK_MODE ) ) return FALSE;
#endif // __SCHOOL0701
	
	ItemProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoUseSkill" );

	CMover* pTarget = prj.GetMover( idFocusObj );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj( pTarget ) )	
		return FALSE;

	if( pSkillProp->dwID == SI_ASS_HEAL_HEALING )	// 힐링 상대가 죽었으면 취소
	{
		if( pTarget->IsDie() )
			return FALSE;
	}
#ifdef __WORLDSERVER
	if( m_SkillState.HasSkill( BUFF_SKILL, SI_BLD_SUP_BERSERK ) )		// 버서크가 걸려있는 상태에선 스킬사용 금지.
	{
		//((CUser*)this)->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );		// 지금은 스킬을 사용할수 없어요

		return FALSE;
	}
#endif
	
#if defined(__WORLDSERVER)
	if( pSkillProp->dwID == SI_ACR_YOYO_SNITCH || pSkillProp->dwID == SI_JST_YOYO_VATALSTAB )	// 강탈, 혼신의 일격스킬이면 다이일루젼상태인지 검사한다.
	{
		if( !m_SkillState.HasSkill( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) )		// 다크일루젼 상태가 아니면 사용못함
		{
			((CUser*)this)->AddDefinedText( TID_GAME_REQ_DARK, "" );		// 다크일루젼상태에서 가능합니다.
			return FALSE;
		}
	}
/*
	if( pSkillProp->dwID == SI_ACR_SUP_DARKILLUSION )
	{
		float fSec = 15.0f;
		if( m_nAtkCnt && m_nAtkCnt < (int)(SEC1 * fSec) )		// 공격한지 15초가 지나야만 다크일루젼을 쓸수있다
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_NEVERKILLSTOP, "" );	// 잠시만 기다려주세요.
			return FALSE;
		}
	}	
*/
#endif //defined(__WORLDSERVER) && defined(__ACROBAT_0504)

	MoverProp *pMoverProp = pTarget->GetProp();
	if( pMoverProp == NULL )
		Error( "DoUseSkill : %s , 타겟 %s 의 프로퍼티가 없다", m_szName, pTarget->m_szName );
	
	if( pTarget != this )	// 타인에게 쓰는 종류일때만 아래검사를 함.
	{
#ifdef __WORLDSERVER
		if( pTarget->IsNPC() && pTarget->IsPeaceful() )	
			return FALSE;	// NPC지만 친한?NPC에겐 스킬 사용 금지

		
		if( !g_eLocal.GetState( EVE_STEAL ) )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜마법을 쓰려할때
				if( IsSteal( pTarget ) )	// pTarget를 스틸 하려하면 
					return FALSE;
		}
		
		if( pSkillProp->nEvildoing > 0 )	// 좋은 마법을
		{
			if( GetHitType2( pTarget, TRUE ) != HITTYPE_FAIL )  	// 적대적인 대상에게 쓰려할때
			{
				if( IsPlayer() )
					((CUser*)this)->AddDefinedText( TID_GAME_NEVERKILLSTOP, "" );		// 스킬을 사용할 수 없습니다.
				return FALSE;		// 스킬 취소
			}
		}
				
#endif
		SetAngle( GetDegree(pTarget->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
	}
	
#ifdef __GUILDCOMBAT
	// 길드컴뱃중에는 부활스킬 금지

	if( GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
			return FALSE;

		if( g_GuildCombatMng.m_nState != CGuildCombat::WAR_STATE )
		{
			if( pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
				return FALSE;
		}
	}
#endif // __GUILDCOMBAT	

#ifdef __WORLDSERVER
#ifdef __SCHOOL0722
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
		if( pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
			return FALSE;
#ifdef __SCHOOL0802
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
			return FALSE;
#endif	// __SCHOOL0802
	}
#endif	// __SCHOOL0722

	// 스킬을 사용할때 적합한 무기를 들고 있는가 검사

	ItemProp* pItemProp = NULL;
	DWORD	dwItemKind3;

	if( pSkillProp->dwLinkKind == IK3_SHIELD )	// 필요아아이템이 방패일때
	{
		CItemElem *pItemElem = GetEquipItem( PARTS_SHIELD );
		if( pItemElem )
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		} else
			dwItemKind3 = NULL_ID;
	}
	else	// 필요아이템이 방패가 아닐때는 액티브 아이템을...
	{
		pItemProp = GetActiveHandItemProp();
		dwItemKind3 = pItemProp->dwItemKind3;
	}
	
	switch( pSkillProp->dwLinkKind )	// 스킬이 필요로하는 무기.
	{
	case IK3_ENCHANTWEAPON:
		{
			if( IsPlayer() )
			{
				BOOL bFail = FALSE;

				CItemElem *pItemElemR = GetWeaponItem();
				CItemElem *pItemElemL = GetLWeaponItem();

				if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
				{	// 양손다 속성이 없을땐
					if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		
						(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
						bFail = TRUE;		// 사용금지

				} else
				if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
				{
					if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
						bFail = TRUE;		// 사용금지

				} else
				if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
				{
					bFail = TRUE;		// 이때는 무조건 사용금지

				}
//				else
//					Message( "CSkillInfluence::Process : 뭐냐 이상황은? %s", pMover->GetName() );

				if( bFail )
				{
					( (CUser*)this )->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );	// 속성무기가 없어 사용못함.
					return FALSE;
				}
		}
		}
		break;
	case IK3_MAGICBOTH:
		if( dwItemKind3 != IK3_WAND  && dwItemKind3 != IK3_STAFF )	// 손에든 무기가 완드도 스태프도 아니면

		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
  #ifdef __ACROBAT_0504
	case IK3_YOBO:
		if( dwItemKind3 != IK3_YOYO && dwItemKind3 != IK3_BOW )	
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
  #endif //__ACROBAT_0504
	default:
		if( pSkillProp->dwLinkKind != NULL_ID && pSkillProp->dwLinkKind != dwItemKind3 )	// 그외는 IK3를 비교해서 틀리면 에러.
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
	}

	// 투핸드용이냐 원핸드용이냐의 조건검사.
	if( pSkillProp->dwHanded != NULL_ID )
	{
		if( pItemProp )
		{
			if( IsPlayer() )
			{
				if( pSkillProp->dwHanded == HD_DUAL )		// 듀얼용 스킬인데
				{
					if( IsDualWeapon() == FALSE )			// 듀얼로 안차고 있다.
					{
						((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
						return FALSE;
					}
				} else
				if( pItemProp->dwHanded != pSkillProp->dwHanded )	// 스킬과 쥐고 있는 아이템의 Handed 종류가 다르면 에러
				{
					((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
					return FALSE;
				}
			}
		}
	}

	// 필요총알종류가 지정되어 있다면

	if( IsBullet( pSkillProp ) == FALSE )
	{
		return FALSE;
	}

	if( TRUE == IsPlayer() )
	{
//		ASSERT( pSkillProp->dwItemJob >= JOB_VAGRANT );
#ifdef __XSKILL0425
		if( pSkillProp->dwReqDisLV != NULL_ID && GetLevel() < pSkillProp->dwReqDisLV )
			return FALSE;
#else
		if( GetLevel() < pSkillProp->dwReqDisLV )
			return FALSE;
#endif
		
		if( pSkillProp->dwReSkill1 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill1 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
				{
					( (CUser*)this )->AddText( "SkillLevel1 " );
					return FALSE;
				}
			}
			else
			{
				int nIdx = GetSkillIdx( dwSkill );
				Message( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}

		if( pSkillProp->dwReSkill2 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill2 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
				{
					( (CUser*)this )->AddText( "SkillLevel2 " );
					return FALSE;
				}				
			}
			else
			{
				int nIdx = GetSkillIdx( dwSkill );
				Message( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}
	}

	if( pSkillProp->dwID == SI_BIL_PST_ASALRAALAIKUM )	// 앗살라의 경우 예외처리.
		if( GetFatiguePoint() <= 0 )		// FP가 하나도 없으면 사용안됨.
			return FALSE;

#endif	// __WORLDSERVER


 #ifdef __WORLDSERVER
		// 쿨타임 검사.
		{
			int nSkillIdx = GetSkillIdx( dwSkill );		// 스킬리스트 인덱스를 찾음.
			
			if( nSkillIdx >= 0 )
			{
				if( m_tmReUseDelay[ nSkillIdx ] )		// 쓰려고 하는 스킬 쿨타임이 남아있으면 실행안됨.
				{
					if( IsPlayer() )
						((CUser*)this)->AddDefinedText( TID_GAME_SKILLWAITTIME, "" );	// 아직 사용할 수 없습니다.
					return FALSE;
				}
			}
		}
 #endif // __WORLDSERVER
		

	SendActMsg( OBJMSG_STOP );
	ClearDest();
//	ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
//	ClearDestPos();		// 스킬을 쓰기 직전엔 목표 추적을 멈춤.

//#ifndef __CLIENT
//	int nCastingTime = 0;
//#endif // not client

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;
	
	//--- 마나가 충분한지 검사...
	if( pSkillProp->dwSkillType == KT_SKILL )
	{
#ifdef __WORLDSERVER
		if( GetFatiguePoint() - GetReqFp( pAddSkillProp->nReqFp ) < 0 )	// FP가 모자르므로 취소
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
			return FALSE;
		}

		// KT_SKILL중에 MP가 소모되는 값이있으면 FP와 같이 소모시켜준다.
		if( (int)pAddSkillProp->nReqMp > 0 )
		{
			if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
				return FALSE;
			}
		}
#endif
		dwMsg = OBJMSG_MELEESKILL;	// 클라에서도 실행해야함
	} else
	if( pSkillProp->dwSkillType == KT_MAGIC )
	{
#ifdef __WORLDSERVER
 #ifdef __ACROBAT_0504
		if( GetAdjParam( DST_CHRSTATE ) & CHS_SILENT )		// 침묵상태에선 매직스킬 쓸 수 없음.
		{
//			if( TRUE == IsPlayer() )
//				( (CUser*)this )->AddDefinedText( TID_NOTUSESKILL, "" );		// 스킬을 사용할 수 없습니다.
			return FALSE;
		}
 #endif // acrobat_0504
		if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
			return FALSE;
		}
#endif // worldserver
		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬. - 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		{
			if( IsPlayer() )
				dwMsg = OBJMSG_MAGICCASTING;
		} else
		if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬. - 캐스팅 동작없이 시전.
		{
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} else
		{								// 일반적인 마법 스킬 사용 - 캐스팅 동작 있음.
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else
		Error( "DoUseSkill : %s dwSkillType = %d", m_szName, pSkillProp->dwSkillType );	// 프로퍼티값이 이상함.

#ifndef __XSYNC0711	
	// 명령수행중 필요한 파라메터세팅.
	SetActParam( OBJACT_USESKILL, dwSkill, idFocusObj/*, nIdx*/ );	
	m_nAParam[3]	= nLevel;
#endif
	

	//--- 동작 시작 -----------------------------------------------------------------
	int nRet = m_pActMover->SendActMsg( dwMsg, nMotion, idFocusObj, nCastingTime );
	if( nRet == 1 )	// 스킬 동작 시작.
	{
		// 명령수행중 필요한 파라메터세팅.
		SetActParam( OBJACT_USESKILL, dwSkill, idFocusObj/*, nIdx*/ );	
		m_nAParam[3]	= nLevel;

#ifdef __WORLDSERVER
		if( pSkillProp->dwSkillType == KT_SKILL )
		{
			// 근접스킬은 사용하는 즉시 fp감소.
			if( (int)pAddSkillProp->nReqFp > 0 )
				IncFatiguePoint( -GetReqFp( pAddSkillProp->nReqFp ) );			// FP감소

			int nReqMp	= GetReqMp( pAddSkillProp->nReqMp );
			if( nReqMp > 0 )
				IncManaPoint( -nReqMp );
			
			if( IsPlayer() )
			{
				int nIdx = GetSkillIdx( dwSkill );
				if( nIdx >= 0 )
				{
 #ifdef __XPXP0303
					DecPXP( pAddSkillProp->dwExp, nIdx );		// pxp깎음
 #endif
					IncSkillExp( 0, nIdx );			// skill exp	일반스킬은 스킬시작때 경치 올림.
				}
			}
		}
		
		if( pSkillProp->dwLinkKindBullet != NULL_ID )	// 총알(?)을 필요로 하는거면

		{
			CItemElem *pItemElem = GetEquipItem( PARTS_BULLET );
			if( pItemElem )
				ArrowDown( 1 );
		}

		if( pSkillProp->nEvildoing < 0 )		// 나쁜마법으로만 몬스터나 플레이어를 찜할수있다.
		{
			pTarget->SetJJim( this );			// pTarget를 찜했다는걸 표시.
			m_nAtkCnt = 1;						// 빗자루 딜레이를 위한 공격 카운터 시작.
		}

		// DST param이 적용되는 스킬은 여기서 적용함.

		g_UserMng.AddUseSkill( this, dwSkill, nLevel, idFocusObj, sutType, nCastingTime );
#endif	// __WORLDSERVER
	} 
	else
	if( nRet == -2 )	// 0:큐에 쌓는다 -1:모름  -2:완전실패하여 취소시킴   -3:모름
	{
		return FALSE;
	}
	
	m_dwFlag |= MVRF_SKILL;		// 스킬사용 명령이 시작됐다.
#ifdef __CLIENT
	if( IsActiveMover() )		// g_pPlayer일경우 
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar->m_nExecute == 1 )		// 스킬바 사용 대기중상태였으면

			pTaskBar->m_nExecute = 2;		// 실제 스킬바 사용중으로 전환.
	}
#endif
	TRACE( "DoUseSkill:%s, ", pSkillProp->szName );
	
	// 3이후부터는 수동으로 입력
//	m_nAParam[3] = nCastingTime;
	
//	ClearActParam();	// 행동 파라메터 클리어.

#ifdef __CLIENT
//	if( GetJob() == JOB_BLADE )		// 블레이드는 스킬쓸때
	{
		DWORD dwLinkSfxDouble = NULL_ID;
		switch( dwSkill )
		{	// 여기다 XI_ 추가하면 CreateSfx의 CSfxPartsLink()에도 추가시켜야 함/
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01;	break;
		case SI_BLD_DOUBLE_SONICBLADE:		dwLinkSfxDouble = XI_SKILL_BLD_DOUBLE_SONICBLADE01;	break;
		case SI_KNT_TWOSW_CHARGE:			dwLinkSfxDouble = XI_SKILL_KNT_TWOSW_CHARGE01;	break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLESW_BLADEDANCE01;      break;

	#ifdef __ACROBAT_0504  // 제스터 양손 이펙트 : sfx프레임이 끝이면 소멸함
		case SI_JST_YOYO_VATALSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_VATALSTAB01;	break;
		case SI_JST_YOYO_HITOFPENYA:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_HITOFPENYA01;	break;

		case SI_JST_YOYO_CRITICALSWING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_CRITICALSWING01;	break;
		case SI_ACR_SUP_SLOWSTEP:	dwLinkSfxDouble = XI_SKILL_ACR_YOYO_SLOWSTEP01;	break;
		case SI_JST_SUP_POISON:		dwLinkSfxDouble = XI_SKILL_JST_SUP_POISON01;	break;
		case SI_JST_SUP_BLEEDING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_BLEEDING01;	break;
		case SI_JST_YOYO_BACKSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		case SI_JST_SUP_ABSORB:	dwLinkSfxDouble = XI_SKILL_JST_SUP_ABSORB01;	break;
	#endif //__ACROBAT_0504
			break;
		}
		if( dwLinkSfxDouble != NULL_ID )
		{
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
			if( pSfx )
			{
				pSfx->SetPartLink( 0 );		// 오른손에 링크.
				pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId() );  // 링크이펙 하나더 생성.
				if( pSfx )
					pSfx->SetPartLink( 1 );		// 손에 링크.
			}
		}
	}

#ifdef __ACROBAT_0504
	// 왼손 링크 이펙

	DWORD dwLinkSfxLeft = NULL_ID;
	switch( dwSkill )
	{	// 아크로벳 스킬이면 활 시전 이펙트 왼손에 링크한다
	case SI_RAG_BOW_ICEARROW:	dwLinkSfxLeft = XI_SKILL_RAG_BOW_ICEARROW01;	break;
	case SI_ACR_BOW_JUNKBOW:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_JUNKBOW01;	break;
	case SI_ACR_BOW_AIMEDSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AIMEDSHOT01;	break;
	case SI_ACR_BOW_SILENTSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_SILENTSHOT01;	break;
	case SI_ACR_BOW_ARROWRAIN:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_ARROWRAIN01;      break;
	case SI_ACR_BOW_AUTOSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AUTOSHOT01;      break;
		
	case SI_RAG_SUP_FASTATTACK:	  dwLinkSfxLeft = XI_SKILL_RAG_SUP_FASTATTACK01;break;
	case SI_RAG_BOW_FLAMEARROW:	  dwLinkSfxLeft = XI_SKILL_RAG_BOW_FLAMEARROW01;break;
	case SI_RAG_BOW_PIRCINGARROW: dwLinkSfxLeft = XI_SKILL_RAG_BOW_PIRCINGARROW01;break;
	case SI_RAG_BOW_POISONARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_POISONARROW01;break;
	case SI_RAG_BOW_SILENTARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_SILENTARROW01;break;
	case SI_RAG_BOW_TRIPLESHOT:   dwLinkSfxLeft = XI_SKILL_RAG_BOW_TRIPLESHOT01;break;
		
		break;
	}
	
	if( dwLinkSfxLeft != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxLeft, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 1 );		// 왼손에 링크.
	}
////////////////////////////////////////////////////////////////////////////////////////////////////
	// 오른손 링크 이펙

	DWORD dwLinkSfxRight = NULL_ID;
	switch( dwSkill )
	{
#ifdef __YADV_SKILL_SFX
	case SI_MER_ONE_SPLMASH: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT04; break;
	case SI_MER_ONE_GUILOTINE: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT02; break;
	case SI_MER_ONE_REFLEXHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT01; break;
	case SI_MER_ONE_SPECIALHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT03; break;
	case SI_ASS_KNU_POWERFIST:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT03;	break;
	case SI_ASS_KNU_TAMPINGHOLE:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT02;	break;
	case SI_ASS_KNU_BURSTCRACK:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT01;	break;
#endif //__YADV_SKILL_SFX
	case SI_GEN_ATK_COUNTER:	dwLinkSfxRight = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		break;
	}
	
	if( dwLinkSfxRight != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxRight, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 0 );		// 오른손에 링크.
	}
	
#endif // __ACROBAT_0504
		

	// 마법 혹은 전투스킬의 발동시작 이펙트발생
	// 자기id GetId()
	// 타겟id idFocusObj
	if( pSkillProp->dwSfxObj != NULL_ID )
	{
		CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		
 #ifdef __S0602_1
		// 공격스킬과 타겟이 몬스터 이면 화면에 출력을 안함
  #ifdef __ACROBAT_0504
		if( pSkillProp->dwExeTarget != EXT_MELEEATK && 
  #else
		if( pSkillProp->dwExeTarget != EXT_SPECIALATK && 
  #endif
			pSkillProp->dwExeTarget != EXT_MAGICATK && 
			pSkillProp->dwExeTarget != EXT_AROUNDATK &&  
			pSkillProp->dwExeTarget != EXT_MAGICATKSHOT && 
			pTarget->IsPlayer() )
		{
			CString str;
			if( this == g_pPlayer ) // 내가 쓴것
			{
				str.Format( prj.GetText( TID_GAME_SKILL_SELF ), pSkillProp->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_SELF ) );	
			}
			else
			if( pTarget == g_pPlayer && ( this != pTarget || pSkillProp->dwID == SI_ASS_CHEER_CIRCLEHEALING ) ) // 내가 쓴것이 아니라면

			{
				str.Format( prj.GetText( TID_GAME_SKILL_MATE ), GetName(), pSkillProp->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_MATE ) );	
			}
		}
 #endif // __S0602_1
	}
#endif	// __CLIENT
#ifdef __J0315
#ifdef __WORLDSERVER
	g_dpDBClient.AddSkillFrequencyLog( dwSkill );
#endif	// __WORLDSERVER
#endif	// __J0315

#ifdef __CLIENT	
	if( prj.m_nLanguage == LANG_JAP )
	{
		if( g_pPlayer )//&& g_pPlayer->m_dwAuthorization >= AUTH_GAMEMASTER )
		{
			if( g_Option.m_bVoice )
			{
				if( IsActiveMover() )
				{
					int nindex = 0;
					
					if( pSkillProp->dwID != -1 )
					{
						if( GetSex() == 0 )
							nindex = pSkillProp->dwID+3000;
						else
							nindex = pSkillProp->dwID+3500;
						
						g_SoundMng.m_nOldSoundVolume = g_SoundMng.m_nSoundVolume;
						g_SoundMng.m_nSoundVolume = 0;
						PLAYSND( nindex );

						g_SoundMng.m_nSoundVolume = g_SoundMng.m_nOldSoundVolume;
					}
				}
			}
		}
	}
#endif	// __CLIENT

	return TRUE;
}
#else // ACROBAT_0504
//{{AFX
// 최초의 스킬 사용 명령이 내리짐

// pSkill
//		Use Skill
//	Mover가 사용가능한 스킬배열(m_aJobSkill[3+14])에서 nIdx에 해당하는 스킬을 꺼내 그것을 실행한다.
//
// 구버전

int g_nOldSkill = 0;
#ifdef __CLIENT
BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime, DWORD dwSkill, DWORD dwLevel )
#else // __CLIENT
BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType )
#endif // __CLIENT
{
	if( IsDie() )	return FALSE;
	if( m_pActMover->IsFly() )	return FALSE;
	if( m_dwFlag & MVRF_NOATTACK )	
		return FALSE;		// 공격금지 상태에선 스킬 못씀.
	
#ifdef __SCHOOL0701
	if( IsMode( ATTACK_MODE ) ) return FALSE;
#endif // __SCHOOL0701

	ItemProp *pSkillProp;

#if defined(__WORLDSERVER) 
	DWORD dwSkill = 0, dwLevel;
#endif	// __WORLDSERVER

	if( nIdx < 0 )
	{
		pSkillProp	= prj.GetSkillProp( dwSkill );
	}
	else
	{
		LPSKILL pSkill	= GetSkill( nType, nIdx );
		if( pSkill == NULL )
		{
			Error( "CMover::DoUseSkill : %s는 nIdx에 스킬을 가지고 있지 않다. %d %d", m_szName, nType, nIdx );
			return FALSE;
		}

		dwSkill	= pSkill->dwSkill;
		dwLevel	= pSkill->dwLevel;

		pSkillProp	= pSkill->GetProp();
	}

	if( !pSkillProp )
	{
		Error( "CMover::DoUseSkill : %s. 스킬(%d)의 프로퍼티가 없다.", m_szName, dwSkill );
		return FALSE;
	}

	if( pSkillProp->nLog == 1 )
	{
		return FALSE;
	}
	
	AddSkillProp* pAddSkillProp;
	
	if( !( pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel ) ) )	// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
	{
		Error( "CMover::DoUseSkill : %s. 애드스킬(%d)의 프로퍼티가 없다.", m_szName, dwSkill );
		return FALSE;
	}

	CMover* pTarget = prj.GetMover( idFocusObj );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj( pTarget ) )	
		return FALSE;

	if( pSkillProp->dwID == SI_ASS_HEAL_HEALING )	// 힐링 상대가 죽었으면 취소
	{
		if( pTarget->IsDie() )
			return FALSE;
	}
#ifdef __WORLDSERVER
	if( m_SkillState.Find( SI_BLD_SUP_BERSERK ) )		// 버서크가 걸려있는 상태에선 스킬사용 금지.
	{
		//((CUser*)this)->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );		// 지금은 스킬을 사용할수 없어요

		return FALSE;
	}
#endif
	
#if defined(__WORLDSERVER) && defined(__ACROBAT_0504)
	if( pSkillProp->dwID == SI_ACR_YOYO_SNITCH )	// 강탈스킬이면 다이일루젼상태인지 검사한다.
	{
		if( !m_SkillState.Find( SI_ACR_SUP_DARKILLUSION ) )		// 다크일루젼 상태가 아니면 사용못함
		{
			((CUser*)this)->AddDefinedText( TID_GAME_REQ_DARK, "" );		// 다크일루젼상태에서 가능합니다.
			return FALSE;
		}
	}
	
/*
	if( m_nAtkCnt && pSkillProp->dwID == SI_ACR_SUP_DARKILLUSION )
	{
		float fSec = 15.0f;
		if( m_nAtkCnt < (int)(SEC1 * fSec) )		// 공격한지 15초가 지나야만 다크일루젼을 쓸수있다
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_NEVERKILLSTOP, "" );	// 잠시만 기다려주세요.
			return FALSE;
		}
	}
*/	
#endif //defined(__WORLDSERVER) && defined(__ACROBAT_0504)

	MoverProp *pMoverProp = pTarget->GetProp();
	if( pMoverProp == NULL )
		Error( "DoUseSkill : %s , 타겟 %s 의 프로퍼티가 없다", m_szName, pTarget->m_szName );
	
	if( pTarget != this )	// 타인에게 쓰는 종류일때만 아래검사를 함.
	{
#ifdef __WORLDSERVER
		if( pTarget->IsNPC() && pTarget->IsPeaceful() )	
			return FALSE;	// NPC지만 친한?NPC에겐 스킬 사용 금지

		
		if( !g_eLocal.GetState( EVE_STEAL ) )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜마법을 쓰려할때
				if( IsSteal( pTarget ) )	// pTarget를 스틸 하려하면 
					return FALSE;
		}
#endif
		SetAngle( GetDegree(pTarget->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
	}

#ifdef __WORLDSERVER
#ifdef __SCHOOL0722
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
		if( pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
			return FALSE;
#ifdef __SCHOOL0802
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
			return FALSE;
#endif	// __SCHOOL0802
	}
#endif	// __SCHOOL0722

	// 스킬을 사용할때 적합한 무기를 들고 있는가 검사

	ItemProp* pItemProp = NULL;
	DWORD	dwItemKind3;
#ifdef __ACROBAT_0504	
/*
	if( pSkillProp->dwLinkKind == IK3_BOW )	// 필요아아이템이 보우일때 왼쪽을 검사한다.
	{
		pItemProp = GetActiveHandItemProp(PARTS_LWEAPON);
		dwItemKind3 = pItemProp->dwItemKind3;
	}
	else	
*/
#endif //__ACROBAT_0504
	if( pSkillProp->dwLinkKind == IK3_SHIELD )	// 필요아아이템이 방패일때
	{
		CItemElem *pItemElem = GetEquipItem( PARTS_SHIELD );
		if( pItemElem )
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		} else
			dwItemKind3 = NULL_ID;
	}
	else	// 필요아이템이 방패가 아닐때는 액티브 아이템을...
	{
		pItemProp = GetActiveHandItemProp();
		dwItemKind3 = pItemProp->dwItemKind3;
	}
	
	switch( pSkillProp->dwLinkKind )	// 스킬이 필요로하는 무기.
	{
	case IK3_ENCHANTWEAPON:
		{
			if( IsPlayer() )
			{
				BOOL bFail = FALSE;

				CItemElem *pItemElemR = GetWeaponItem();
				CItemElem *pItemElemL = GetLWeaponItem();

				if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
				{	// 양손다 속성이 없을땐
					if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		
						(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
						bFail = TRUE;		// 사용금지

				} else
				if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
				{
					if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
						bFail = TRUE;		// 사용금지

				} else
				if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
				{
					bFail = TRUE;		// 이때는 무조건 사용금지

				}
//				else
//					Message( "CSkillInfluence::Process : 뭐냐 이상황은? %s", pMover->GetName() );

				if( bFail )
				{
					( (CUser*)this )->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );	// 속성무기가 없어 사용못함.
					return FALSE;
				}
		}
		}
		break;
	case IK3_MAGICBOTH:
		if( dwItemKind3 != IK3_WAND  && dwItemKind3 != IK3_STAFF )	// 손에든 무기가 완드도 스태프도 아니면

		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
	default:
		if( pSkillProp->dwLinkKind != NULL_ID && pSkillProp->dwLinkKind != dwItemKind3 )	// 그외는 IK3를 비교해서 틀리면 에러.
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
	}

	// 투핸드용이냐 원핸드용이냐의 조건검사.
	if( pSkillProp->dwHanded != NULL_ID )
	{
		if( pItemProp )
		{
			if( IsPlayer() )
			{
				if( pSkillProp->dwHanded == HD_DUAL )		// 듀얼용 스킬인데
				{
					if( IsDualWeapon() == FALSE )			// 듀얼로 안차고 있다.
					{
						((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
						return FALSE;
					}
				} else
				if( pItemProp->dwHanded != pSkillProp->dwHanded )	// 스킬과 쥐고 있는 아이템의 Handed 종류가 다르면 에러
				{
					((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
					return FALSE;
				}
			}
		}
	}

	// 필요총알종류가 지정되어 있다면

	if( IsBullet( pSkillProp ) == FALSE )
	{
		return FALSE;
	}

	if( TRUE == IsPlayer() )
	{
//		ASSERT( pSkillProp->dwItemJob >= JOB_VAGRANT );
		if( GetLevel() < pSkillProp->dwReqDisLV )
			return FALSE;
		
		if( pSkillProp->dwReSkill1 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill1 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
				{
					( (CUser*)this )->AddText( "SkillLevel1 " );
					return FALSE;
				}
			}
			else
			{
				Message( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}

		if( pSkillProp->dwReSkill2 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill2 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
				{
					( (CUser*)this )->AddText( "SkillLevel2 " );
					return FALSE;
				}				
			}
			else
			{
				Message( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}
	}

	if( pSkillProp->dwID == SI_BIL_PST_ASALRAALAIKUM )	// 앗살라의 경우 예외처리.
		if( GetFatiguePoint() <= 0 )		// FP가 하나도 없으면 사용안됨.
			return FALSE;

#endif	// __WORLDSERVER

 #ifdef __WORLDSERVER
		// 쿨타임 검사.
		{
			int nSkillIdx = nIdx;
			if( nIdx < 0 )
				nSkillIdx = GetSkillIdx( dwSkill );		// 스킬리스트 인덱스를 찾음.

			if( nSkillIdx >= 0 )
			{
				if( m_tmReUseDelay[ nSkillIdx ] )		// 쓰려고 하는 스킬 쿨타임이 남아있으면 실행안됨.
				{
					if( IsPlayer() )
						((CUser*)this)->AddDefinedText( TID_GAME_SKILLWAITTIME, "" );	// 아직 사용할 수 없습니다.
					return FALSE;
				}
			} else
				Message( "DoUseSkill : ReUseDelay[ %d ]", nSkillIdx );
		}
 #endif // WorldServer

			

	SendActMsg( OBJMSG_STOP );
	ClearDest();
//	ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
//	ClearDestPos();		// 스킬을 쓰기 직전엔 목표 추적을 멈춤.

#ifndef __CLIENT
	int nCastingTime = 0;
#endif // not client

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;
	
	//--- 마나가 충분한지 검사...
	if( pSkillProp->dwSkillType == KT_SKILL )
	{
#ifdef __WORLDSERVER
		if( GetFatiguePoint() - (int)pAddSkillProp->nReqFp < 0 )	// FP가 모자르므로 취소
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
			return FALSE;
		}

		// KT_SKILL중에 MP가 소모되는 값이있으면 FP와 같이 소모시켜준다.
		if( (int)pAddSkillProp->nReqMp > 0 )
		{
			if( GetManaPoint() - (int)pAddSkillProp->nReqMp < 0 )		// 마나가 모자르므로 취소
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
				return FALSE;
			}
		}
#endif
		dwMsg = OBJMSG_MELEESKILL;	// 클라에서도 실행해야함
	} else
	if( pSkillProp->dwSkillType == KT_MAGIC )
	{
#ifdef __WORLDSERVER
		if( GetAdjParam( DST_CHRSTATE ) & CHS_SILENT )		// 침묵상태에선 매직스킬 쓸 수 없음.
		{
//			if( TRUE == IsPlayer() )
//				( (CUser*)this )->AddDefinedText( TID_NOTUSESKILL, "" );		// 스킬을 사용할 수 없습니다.
			return FALSE;
		}
		if( GetManaPoint() - (int)pAddSkillProp->nReqMp < 0 )		// 마나가 모자르므로 취소
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
			return FALSE;
		}
#endif // worldserver
		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬. - 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		{
			if( IsPlayer() )
			{
#ifdef __WORLDSERVER
				nCastingTime = GetQueueCastingTime();
#endif
				dwMsg = OBJMSG_MAGICCASTING;
			}
		} else
		if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬. - 캐스팅 동작없이 시전.
		{
#ifdef __WORLDSERVER
			nCastingTime = 0;
#endif
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} else
		{								// 일반적인 마법 스킬 사용 - 캐스팅 동작 있음.
#ifdef __WORLDSERVER
			nCastingTime = (int)((pAddSkillProp->dwSkillReady / 1000.0f) * SEC1);
#endif
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else
		Error( "DoUseSkill : %s dwSkillType = %d", m_szName, pSkillProp->dwSkillType );	// 프로퍼티값이 이상함.

	// 명령수행중 필요한 파라메터세팅.
	SetActParam( OBJACT_USESKILL, dwSkill, idFocusObj, nIdx );	
	m_nAParam[3]	= dwLevel;
	

	//--- 동작 시작 -----------------------------------------------------------------
	int nRet = m_pActMover->SendActMsg( dwMsg, nMotion, idFocusObj, nCastingTime );
	if( nRet == 1 )	// 스킬 동작 시작.
	{
#ifdef __WORLDSERVER
		if( pSkillProp->dwSkillType == KT_SKILL )
		{
			// 근접스킬은 사용하는 즉시 fp감소.
			if( (int)pAddSkillProp->nReqFp > 0 )			
				IncFatiguePoint( -pAddSkillProp->nReqFp );			// FP감소

			if( (int)pAddSkillProp->nReqMp > 0 )
				IncManaPoint( -pAddSkillProp->nReqMp );
 #ifdef __XPXP0303
			DecPXP( pAddSkillProp->dwExp, nIdx );		// pxp깎음
 #endif
			IncSkillExp( nType, nIdx );			// skill exp	일반스킬은 스킬시작때 경치 올림.
		}
		
		if( pSkillProp->dwLinkKindBullet != NULL_ID )	// 총알(?)을 필요로 하는거면

		{
			CItemElem *pItemElem = GetEquipItem( PARTS_BULLET );
			if( pItemElem )
			{
				pItemElem->UseItem();		// 총알 하나 소모

				UpdateItem( (CHAR)ITYPE_ITEM, pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum );
			}
		}

		if( pSkillProp->nEvildoing < 0 )		// 나쁜마법으로만 몬스터나 플레이어를 찜할수있다.
		{
			pTarget->SetJJim( this );			// pTarget를 찜했다는걸 표시.
			m_nAtkCnt = 1;						// 빗자루 딜레이를 위한 공격 카운터 시작.
		}

		// DST param이 적용되는 스킬은 여기서 적용함.

		g_UserMng.AddUseSkill( this, dwSkill, dwLevel, idFocusObj, sutType, nCastingTime );
#endif	// __WORLDSERVER
		
	} 
	else
	if( nRet == -2 )	//
	{
		return FALSE;
	}
	
	m_dwFlag |= MVRF_SKILL;		// 스킬사용 명령이 시작됐다.
#ifdef __CLIENT
	if( IsActiveMover() )		// g_pPlayer일경우 
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar->m_nExecute == 1 )		// 스킬바 사용 대기중상태였으면

			pTaskBar->m_nExecute = 2;		// 실제 스킬바 사용중으로 전환.
		TRACE( "DoUseSkill-%d", pTaskBar->m_nExecute );
	}
#endif

	// 3이후부터는 수동으로 입력
//	m_nAParam[3] = nCastingTime;
	
//	ClearActParam();	// 행동 파라메터 클리어.

#ifdef __CLIENT
	{
		DWORD dwLinkSfx = NULL_ID;
		switch( dwSkill )
		{	// 여기다 XI_ 추가하면 CreateSfx의 CSfxPartsLink()에도 추가시켜야 함
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	dwLinkSfx = XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01;	break;
		case SI_BLD_DOUBLE_SONICBLADE:		dwLinkSfx = XI_SKILL_BLD_DOUBLE_SONICBLADE01;	break;
		case SI_KNT_TWOSW_CHARGE:			dwLinkSfx = XI_SKILL_KNT_TWOSW_CHARGE01;	break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	dwLinkSfx = XI_SKILL_BLD_DOUBLESW_BLADEDANCE01;      break;
	#ifdef __ACROBAT_0504
		case SI_JST_YOYO_VATALSTAB:	dwLinkSfx = XI_SKILL_JST_YOYO_VATALSTAB01;	break;
		case SI_JST_YOYO_HITOFPENYA:	dwLinkSfx = XI_SKILL_JST_YOYO_HITOFPENYA01;	break;

		case SI_JST_YOYO_CRITICALSWING:	dwLinkSfx = XI_SKILL_JST_SUP_CRITICALSWING02;	break;
		case SI_ACR_SUP_SLOWSTEP:	dwLinkSfx = XI_SKILL_ACR_YOYO_SLOWSTEP01;	break;
		case SI_JST_SUP_POISON:	dwLinkSfx = XI_SKILL_JST_SUP_POISON02;	break;
		case SI_JST_SUP_BLEEDING:	dwLinkSfx = XI_SKILL_JST_SUP_BLEEDING02;	break;
		case SI_JST_YOYO_BACKSTAB:	dwLinkSfx = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		case SI_JST_SUP_ABSORB:	dwLinkSfx = XI_SKILL_JST_SUP_ABSORB02;	break;
	#endif //__ACROBAT_0504
			break;
		}
		if( dwLinkSfx != NULL_ID )
		{
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfx, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
			if( pSfx )
			{
				pSfx->SetPartLink( 0 );		// 오른손에 링크.
				pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfx, GetPos(), GetId() );  // 링크이펙 하나더 생성.
				if( pSfx )
					pSfx->SetPartLink( 1 );		// 손에 링크.
			}
		}
	}

	// 마법 혹은 전투스킬의 발동시작 이펙트발생
	// 자기id GetId()
	// 타겟id idFocusObj
	if( pSkillProp->dwSfxObj != NULL_ID )
	{
#ifdef __ACROBAT_0504
		DWORD dwLinkSfx = NULL_ID;
		switch( dwSkill )
		{	// 아크로벳 스킬이면 활 시전 이펙트 왼손에 링크한다
		case SI_ACR_BOW_JUNKBOW:	dwLinkSfx = XI_SKILL_ACR_BOW_JUNKBOW01;	break;
		case SI_ACR_BOW_AIMEDSHOT:	dwLinkSfx = XI_SKILL_ACR_BOW_AIMEDSHOT01;	break;
		case SI_ACR_BOW_SILENTSHOT:	dwLinkSfx = XI_SKILL_ACR_BOW_SILENTSHOT01;	break;
		case SI_ACR_BOW_ARROWRAIN:	dwLinkSfx = XI_SKILL_ACR_BOW_ARROWRAIN01;      break;
		case SI_ACR_BOW_AUTOSHOT:	dwLinkSfx = XI_SKILL_ACR_BOW_AUTOSHOT01;      break;
			
		case SI_RAG_SUP_FASTATTACK:	  dwLinkSfx = XI_SKILL_RAG_SUP_FASTATTACK01;break;
		case SI_RAG_BOW_FLAMEARROW:	  dwLinkSfx = XI_SKILL_RAG_BOW_FLAMEARROW01;break;
		case SI_RAG_BOW_PIRCINGARROW: dwLinkSfx = XI_SKILL_RAG_BOW_PIRCINGARROW01;break;
		case SI_RAG_BOW_POISONARROW:  dwLinkSfx = XI_SKILL_RAG_BOW_POISONARROW01;break;
		case SI_RAG_BOW_SILENTARROW:  dwLinkSfx = XI_SKILL_RAG_BOW_SILENTARROW01;break;
		case SI_RAG_BOW_TRIPLESHOT:   dwLinkSfx = XI_SKILL_RAG_BOW_TRIPLESHOT01;break;
			
			break;
		}
		
		if( dwLinkSfx != NULL_ID )
		{
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfx, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
			if( pSfx )
				pSfx->SetPartLink( 1 );		// 왼손에 링크.
		}
		else
		{
			CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		}
#else // __ACROBAT_0504
		CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
#endif //__ACROBAT_0504
		
 #ifdef __S0602_1
		// 공격스킬과 타겟이 몬스터 이면 화면에 출력을 안함
  #ifdef __ACROBAT_0504
		if( pSkillProp->dwExeTarget != EXT_MELEEATK && 
  #else
		if( pSkillProp->dwExeTarget != EXT_SPECIALATK && 
  #endif
			pSkillProp->dwExeTarget != EXT_MAGICATK && 
			pSkillProp->dwExeTarget != EXT_AROUNDATK &&  
			pSkillProp->dwExeTarget != EXT_MAGICATKSHOT && 
			pTarget->IsPlayer() )
		{
			CString str;
			if( this == g_pPlayer ) // 내가 쓴것
			{
				str.Format( prj.GetText( TID_GAME_SKILL_SELF ), pSkillProp->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_SELF ) );	
			}
			else
			if( pTarget == g_pPlayer && ( this != pTarget || pSkillProp->dwID == SI_ASS_CHEER_CIRCLEHEALING ) ) // 내가 쓴것이 아니라면

			{
				str.Format( prj.GetText( TID_GAME_SKILL_MATE ), GetName(), pSkillProp->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_MATE ) );	
			}
		}
 #endif // __S0602_1
	}
#endif	// __CLIENT
#ifdef __J0315
#ifdef __WORLDSERVER
	g_dpDBClient.AddSkillFrequencyLog( dwSkill );
#endif	// __WORLDSERVER
#endif	// __J0315

#ifdef __CLIENT	
	if( prj.m_nLanguage == LANG_JAP )
	{
		if( g_pPlayer )//&& g_pPlayer->m_dwAuthorization >= AUTH_GAMEMASTER )
		{
			if( g_Option.m_bVoice )
			{
				if( IsActiveMover() )
				{
					int nindex = 0;
					
					if( pSkillProp->dwID != -1 )
					{
						if( GetSex() == 0 )
							nindex = pSkillProp->dwID+3000;
						else
							nindex = pSkillProp->dwID+3500;
						
						g_SoundMng.m_nOldSoundVolume = g_SoundMng.m_nSoundVolume;
						g_SoundMng.m_nSoundVolume = 0;
						PLAYSND( nindex );

						g_SoundMng.m_nSoundVolume = g_SoundMng.m_nOldSoundVolume;
					}
				}
			}
		}
	}
#endif	// __CLIENT

	return TRUE;
}
//}}AFX
#endif // not ACROBAT_0504

//
//	지속성을 갖는 스킬이 시간이 다되 끝났을때 호출됨.
//
void	CMover::OnEndSkillState( DWORD dwSkill, DWORD dwLevel )
{
	if( dwSkill < 0 || dwSkill > prj.m_aPropSkill.GetSize() )
	{
		Error( "OnEndSkillState : 스킬의 범위를 넘어섬. %d", dwSkill );
		return;
	}
	ItemProp* pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "OnEndSkillState" );

	if( dwSkill == SI_ACR_YOYO_COUNTER )	// 카운터 어택스킬의 경우

	{
		m_pActMover->ResetState( OBJSTA_ATK_ALL );		// 공격동작 중단시킴.  (이런 패턴의 스킬은 앞으로 일반화 시키자)
	}

	BOOL	bSend = FALSE;
#ifdef __WORLDSERVER
	bSend = TRUE;			// 서버일경우 유저들에게 보내줌.
	ResetDestParam( pAddSkillProp->dwDestParam1, pAddSkillProp->nAdjParamVal1, bSend );
	ResetDestParam( pAddSkillProp->dwDestParam2, pAddSkillProp->nAdjParamVal2, bSend );
#endif


}

// 일반 공격 동작이 끝났을때 호출.
void	CMover::OnEndMeleeAttack( DWORD dwState )
{
#ifdef __WORLDSERVER
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_END_MELEEATTACK );
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_END_MELEEATTACK );
#endif	// __POSTAIMSG1008
#endif // WorldServer
}

/*--------------------------------------------------------------------------------------------------------


											Do Use Item


----------------------------------------------------------------------------------------------------------*/
//
//  Use Item
//
#if defined( __CLIENT ) && !defined( __ONLINE )
// 싱글 클라이언트 버젼 (디버깅 및 데모용)
BOOL CMover::DoUseItem( DWORD dwItemId, DWORD dwFocusId, int nPart  )
{
	if( IsDie() )
		return FALSE;

//	if( m_dwFlag & MVRF_SKILL )		// 스킬사용중엔 아이템 사용 안된다.
//		return FALSE;

	WORD dwType = LOWORD( dwItemId );
	WORD dwId = HIWORD( dwItemId );

	CItemBase* pItemBase = GetItemId( dwId ); 
	if( pItemBase == NULL )
		return( FALSE );

	ItemProp* pItemProp = pItemBase->GetProp();
	DWORD dwItemKind1 = pItemProp->dwItemKind1;
	DWORD dwItemKind2 = pItemProp->dwItemKind2;
	DWORD dwItemKind3 = pItemProp->dwItemKind3;
//	DWORD dwItemKind4 = pItemProp->dwItemKind4;

	{
		CItemElem* pItemElem = (CItemElem*)pItemBase;

//		ItemProp* pProp = pItemBase->GetProp();

		PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
		//if( pItemProp->szSndAttack1[ 0 ] != '=' ) {
		//	PLAYSND( pItemProp->szSndAttack1 );
		//}
/*----------------------------------------*/
		// armor, weapon
		if( pItemProp->dwParts != NULL_ID )
		{
			BOOL f	= !m_Inventory.IsEquip( dwId );	// equip
			if( TRUE == DoEquip( pItemElem, f ) )
			{
			}
		}
/*----------------------------------------*/
		// 일반적인 아이템 사용 
		else
		{
			switch( pItemProp->dwItemKind2 )
			{
				case IK2_REFRESHER:
				case IK2_FOOD:
					{
						int nPoint, nVal1, nVal2, nValue;
						if( pItemProp->dwDestParam1 != (DWORD)-1 )
						{
							nPoint	= GetPointParam( pItemProp->dwDestParam1 );
							nVal1	= pItemProp->nAdjParamVal1;
							if( nPoint + nVal1 > pItemProp->dwAbilityMin ) {
								nVal2	= nPoint + nVal1 - pItemProp->dwAbilityMin;
								if( nVal2 > nVal1 )		nVal2	= nVal1;
								nVal1	-= nVal2;
							}
							else {
								nVal2	= 0;
							}
							SetPointParam( pItemProp->dwDestParam1, nPoint + nVal1 + ( nVal2 * 30 / 100 ) );
						}
						
						if( pItemProp->dwDestParam2 != (DWORD)-1 )
						{
							nPoint	= GetPointParam( pItemProp->dwDestParam2 );
							nVal1	= pItemProp->nAdjParamVal2;
							if( nPoint + nVal1 > pItemProp->dwAbilityMax ) {
								nVal2	= nPoint + nVal1 - pItemProp->dwAbilityMax;
								if( nVal2 > nVal1 )		nVal2	= nVal1;
								nVal1	-= nVal2;
							}
							else {
								nVal2	= 0;
							}

							SetPointParam( pItemProp->dwDestParam1, ( nPoint + nVal1 + ( nVal2 * 30 / 100 ) ) );
						}
						break;
					}

				default:
					{
						if( pItemProp->dwDestParam1 != -1 )
							SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, 1 );
						if( pItemProp->dwDestParam2 != -1 )
							SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, 1 );
						break;
					}
			}

			// 갯수가 0이면  아이템 삭제 
			pItemElem->UseItem();
			// remove & transfer
			UpdateItem( dwId, UI_NUM, pItemElem->m_nItemNum );
		}
	}
	return TRUE;
}
#else // defined( __CLIENT ) && !defined( __ONLINE )


// __ONLINE, __WORLDSERVER
// 아이템 ID로 아이템 생성(인벤토리와 상관없이 사용)
BOOL CMover::DoUseItemVirtual( DWORD dwItemId, BOOL bEffectSkip )
{
#ifdef __WORLDSERVER
	//인벤없는 아템 사용시는 죽음과 관계없이 실행가능하게한다.

	ItemProp* pItemProp = prj.GetItemProp( dwItemId );

	if( pItemProp )
	{
#ifdef __S0504_FLY
		if( IsItemRedyTime( pItemProp, pItemProp->dwID, FALSE ) == FALSE )
			return FALSE;
#endif // __S0504_FLY

		D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
#ifdef __NPP_050308
		DWORD	dwSfxID = pItemProp->dwSfxObj3;
#else
		DWORD	dwSfxID = pItemProp->dwSfxObj;
#endif

		if( !bEffectSkip )
		{
			PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
		}

#ifdef __SM_ITEM_2ND
		if( dwItemId == II_SYS_SYS_SCR_RETURN )
		{
			// 돌아가기 
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
				m_SkillState.RemoveSkillInfluence( BUFF_ITEM, II_SYS_SYS_SCR_RETURN );

			DWORD dwWorldID;
			D3DXVECTOR3 vPos = ((CUser*)this)->GetReturnPos( &dwWorldID );
		#ifdef __MSVR
			((CUser*)this)->Replace( g_uIdofMulti, dwWorldID, vPos );
		#else	// __MSVR
			((CUser*)this)->Replace( dwWorldID, vPos );
		#endif
			return TRUE;
		}
#endif

		if( dwItemId == II_CHR_SYS_SCR_ESCAPEBLINKWING )
		{
#ifdef __PIERCING_SM_TIME
			if( pItemProp->dwCircleTime != (DWORD)-1 )
			{
#ifdef __YESCAPE_MOTION
				if( !IsSMMode( SM_ESCAPE ) )
				{
					SetSMMode( SM_ESCAPE, pItemProp->dwCircleTime );
				}
				else
				{
					return FALSE;
				}
#endif	// __YESCAPE_MOTION
			}
			return DoUseItemBlinkWing( pItemProp, NULL, FALSE );
#else	// __PIERCING_SM_TIME
			if( DoUseItemSM( pItemProp, NULL, &sPos ) == FALSE )
				return FALSE;
#endif	// __PIERCING_SM_TIME
		}

		if( dwSfxID != NULL_ID && !bEffectSkip )		// 아이템 사용시에 생성될 이펙트
		{
			g_UserMng.AddCreateSfxObj( this, dwSfxID, sPos.x, sPos.y, sPos.z );	// 절대좌표로 하자.
		}	
	}
#endif //__WORLDSERVER

	return TRUE;
}


//
//
BOOL CMover::DoUseItem( DWORD dwData, DWORD dwFocusId, int nPart  )
{
#ifdef __WORLDSERVER
	if( IsDie() )
		return FALSE;

	WORD dwType = LOWORD( dwData );
	WORD dwId   = HIWORD( dwData );

	CItemBase* pItemBase = GetItemId( dwId ); 
	if( IsUsableItem( pItemBase ) == FALSE )
		return FALSE;

	CItemElem* pItemElem = (CItemElem*)pItemBase;
	ItemProp* pItemProp  = pItemBase->GetProp();
	DWORD dwItemKind1    = pItemProp->dwItemKind1;
	DWORD dwItemKind2    = pItemProp->dwItemKind2;
	DWORD dwItemKind3    = pItemProp->dwItemKind3;

	PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
	
	if( pItemProp->dwParts != NULL_ID )
	{
		// armor, weapon
		if( m_pActMover->IsActAttack() == FALSE )	// 공격중엔 장비 못바꿈.
		{
			DoUseEquipmentItem( pItemElem, dwId, nPart );
		}
	}
	else
	{
		// 일반적인 아이템 사용 

	#ifdef __S0504_FLY
		if( IsItemRedyTime( pItemProp, pItemElem->m_dwObjId, TRUE ) == FALSE )
			return FALSE;
	#endif // __S0504_FLY

		DWORD dwGroup = 0;

		if( IsPlayer() )
		{
			CUser* pUser = (CUser*)this;

			dwGroup = pUser->m_cooltimeMgr.GetGroup( pItemProp );
			if( dwGroup )
			{
				if( pUser->m_cooltimeMgr.CanUse( dwGroup ) == FALSE )
				{
				#if !defined(__ITEMCOOLTIME_VER2_050308)
					pUser->AddDefinedText( TID_GAME_ATTENTIONCOOLTIME, "" );	
				#endif
					return FALSE;
				}
			}
		}

		D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	#ifdef __NPP_050308
		DWORD	dwSfxID = pItemProp->dwSfxObj3;
	#else
		DWORD	dwSfxID = pItemProp->dwSfxObj;
	#endif

	#ifndef __PIERCING_SM_TIME
		if( pItemProp->dwItemKind1 == IK1_CHARGED )
		{
		#ifdef __CHARGEDITEM0620
			if( DoUseItemSM( pItemProp, pItemElem, &sPos, nPart ) == FALSE )
		#else	// __CHARGEDITEM0620
			if( DoUseItemSM( pItemProp, pItemElem, &sPos ) == FALSE )
		#endif	// __CHARGEDITEM0620
			{
				return FALSE;
			}
		}
		else
	#endif	// __PIERCING_SM_TIME
		{
			// 비 상용화 아이템
			switch( pItemProp->dwItemKind2 )
			{
			#ifdef __ITEMBUFF0929
				case IK2_BUFF:
					{
						DoApplySkill( this, pItemProp, NULL );
						break;
					}
			#endif	// __ITEMBUFF0929
		#ifdef __V050322_CACHEQUEST
				case IK2_TEXT: //텍스트 문서 처리 
					// 퀘스트가 있으면 퀘스트 시작 
					if( pItemProp->dwQuestId && pItemElem->m_bItemResist == FALSE )
					{
						::__SetQuest( GetId(), pItemProp->dwQuestId );
						pItemElem->m_bItemResist = TRUE;
						UpdateItem( pItemElem->m_dwObjId, UI_IR, pItemElem->m_bItemResist );	
					}
					break;
		#endif // __V050322_CACHEQUEST
		#ifdef __PIERCING_SM_TIME
				case IK2_SYSTEM:
					{
						int nResult = 0;
					#ifdef __CHARGEDITEM0620
						nResult = DoUseItemSystem( pItemProp, pItemElem, nPart );
					#else	// __CHARGEDITEM0620
						if( DoUseItemSystem( pItemProp, pItemElem ) > 0 )
					#endif	// __CHARGEDITEM0620
						{
							if( 0 < nResult )
							{
								if( nResult == 2 )
								{
									((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
								}
						#ifdef __SM_ITEM_2ND
								else if( nResult == 3 )
								{
									((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
								}
						#endif // __SM_ITEM_2ND
								return FALSE;
							}
							else if( pItemProp->bCharged )
							{
								// 상용화 아이템 사용 로그 삽입
								g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
								if( pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT || pItemProp->dwID == II_SYS_SYS_SCR_SMELTING )
								{
									LogItemInfo aLogItem;
									aLogItem.SendName = ((CUser*)this)->GetName();
									aLogItem.RecvName = "UPGRADEITEM";
									aLogItem.WorldId = ((CUser*)this)->GetWorld()->GetID();
									aLogItem.Gold = ((CUser*)this)->GetGold();
									aLogItem.Gold2 = ((CUser*)this)->GetGold();
									aLogItem.Action = "H";
									g_DPSrvr.OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
								}
							}
						}
					}
					break;
		#endif	// __PIERCING_SM_TIME
				case IK2_BLINKWING:
					return DoUseItemBlinkWing( pItemProp, pItemElem );

				case IK2_REFRESHER:
					{
		#ifdef __PIERCING_SM_TIME
						if( pItemProp->dwCircleTime != (DWORD)-1 )
						{
							if( IsSMMode( SM_MAINTAIN_MP ) == FALSE && SetSMMode( SM_MAINTAIN_MP, pItemProp->dwCircleTime ) )
							{
								SetPointParam( DST_MP, GetMaxManaPoint() );
							}
							else
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); //   
								return FALSE;
							}
							g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
						}
						else
		#endif	// __PIERCING_SM_TIME
						{
							if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
								return FALSE;
						}
						break;
					}
				case IK2_POTION:
		#ifdef __PIERCING_SM_TIME
					if( pItemProp->dwCircleTime != (DWORD)-1 )	// 상용아이템은 dwCircleTime을 썼더라. 나중에 dwSkillTime으로 통합예정.
					{
						if( IsSMMode( SM_MAINTAIN_FP ) == FALSE && SetSMMode( SM_MAINTAIN_FP, pItemProp->dwCircleTime ) )
						{
							SetPointParam( DST_FP, GetMaxFatiguePoint() );
						}
						else
						{
							((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); //   
							return FALSE;
						}
						g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
					}
					else
		#endif	// __PIERCING_SM_TIME
					{
						if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
							return FALSE;
					}
					break;
				case IK2_FOOD:
					if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
						return FALSE;
					break;
				case IK2_AIRFUEL:	// 비행연료류

					{
						ItemProp *pRideProp = prj.GetItemProp( m_dwRideItemIdx );	// 빗자루 프로퍼티
						if( pRideProp && TRUE == IsPlayer() && m_pActMover->IsFly() )
						{
							if( pItemProp->dwItemKind3 == IK3_ACCEL )	// 가속 연료

							{
								m_tmAccFuel = pItemProp->dwAFuelReMax * 1000;;	// 몇초간 가속할수 있냐.
							}
							else
							{	// 비행연료

								if( pRideProp->dwItemKind3 == IK3_BOARD && pItemProp->dwItemKind3 == IK3_CFLIGHT )	// 타고있는건 보드 / 사용하려는건 빗자루용

								{
									((CUser*)this)->AddDefinedText( TID_GAME_COMBFUEL, "" );	// 이건 빗자루용이여~
									return FALSE;
								}
								else
								if( pRideProp->dwItemKind3 == IK3_STICK && pItemProp->dwItemKind3 == IK3_BFLIGHT )	// 타고있는건 빗자루 / 사용하려는건 보드용

								{
									((CUser*)this)->AddDefinedText( TID_GAME_BOARDFUEL, "" );	// 이건 보드용이여~
									return FALSE;
								}
									
								m_nFuel += pItemProp->dwFuelRe;		// 충전량.
								if( m_nFuel >= pRideProp->dwFFuelReMax )	// 빗자루 연료통보다 크면 끝.
									m_nFuel = pRideProp->dwFFuelReMax;
							}
							((CUser *)this)->AddSetFuel( m_nFuel, m_tmAccFuel );		// 변화된 량을 전송. - SetPointParam에다 합쳐도 괜찮을듯?
						} else
							return FALSE;
					}
					break;
		#ifdef __GM_COMMAND_ITEM
				case IK2_GMTEXT:
					{
						BOOL bGMTime = TRUE;
						if( pItemProp->dwSkillTime != NULL_ID )		// 지속시간이 있는 아이템
						{
							if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )	// 변신 아이템은 비행중에는 사용할수 없음.
							{
								if( IsFly() )
								{
									((CUser*)this)->AddDefinedText( TID_PK_FLIGHT_NOUSE, "" ); // 비행 중에는 사용할 수 없습니다.
									bGMTime = FALSE;
								}
							}
							if( bGMTime )
							{
								if( !m_SkillState.HasLikeItemBuf( pItemProp->dwItemKind3 ) )
								{
									DoApplySkill( this, pItemProp, NULL );		// 대상에게 아이템효과를 적용함. 
								}
								else
								{
									((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
									bGMTime = FALSE;
								}
							}								
						}

						if(  pItemProp->dwID == II_SYS_SYS_GM_NODISGUISE )
						{
							if( m_SkillState.HasLikeItemBuf( IK3_TEXT_DISGUISE ) )
								m_SkillState.RemoveLikeItemBuf( IK3_TEXT_DISGUISE );
							else
								bGMTime = FALSE;
						}

						if( bGMTime )
						{
							char szGMCommand[64] = {0,};
							CString szGMText = pItemProp->szTextFileName;
							szGMText.Replace( '(', '\"' );
							szGMText.Replace( ')', '\"' ); 
							int nGMCount = szGMText.Find( "/", 1 );
							if( nGMCount != -1 )
								strncpy( szGMCommand, szGMText, nGMCount );
							else
								strcpy( szGMCommand, szGMText );
							ParsingCommand( szGMCommand, (CMover*)this, TRUE );
						}
						else
							return FALSE;
					}
					break;
			#endif // __GM_COMMAND_ITEM
				default:
					{
					#ifdef __XITEMBUFF	
						if( pItemProp->dwSkillTime != NULL_ID )		// 지속시간이 있는 아이템
						{
							DoApplySkill( this, pItemProp, NULL );		// 대상에게 아이템효과를 적용함. 
						}
					#else
						if( pItemProp->dwDestParam1 != -1 )
							SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, 1 );
						if( pItemProp->dwDestParam2 != -1 )
							SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, 1 );
						break;
					#endif
					}
			} // switch ik2

			switch( pItemProp->dwItemKind3 )
			{
			case IK3_MAGICTRICK:	// 이펙트 아이템류.
				{
					float	fTheta = D3DXToRadian( GetAngle() );
					switch( pItemProp->dwID )
					{
					case II_GEN_MAG_TRI_FIRESHOWER:
				#if defined(__TAIWAN__)
					case II_GEN_MAG_TRI_HWFIREWORKS:
				#endif // Taiwan || Thailand
					case II_CHR_MAG_TRI_ROCKETBOMB:
					case II_CHR_MAG_TRI_HEARTBOMB:
					case II_CHR_MAG_TRI_TWISTERBOMB:
						sPos.x = GetPos().x + sinf(fTheta) * 3.0f;			// 바라보는 방향 1미터 앞에다 발생시킴.
						sPos.z = GetPos().z + -cosf(fTheta) * 3.0f;
						sPos.y = GetPos().y + 1.5f;
						sPos.y = GetWorld()->GetUnderHeight( D3DXVECTOR3(sPos.x,sPos.y,sPos.z) );
						break;
					case II_GEN_MAG_TRI_NEWYEARBOMB:
					case II_GEN_MAG_TRI_SULNALBOMB:
					case II_GEN_MAG_TRI_GOODBYEBOMB:
						sPos.x = GetPos().x;
						sPos.z = GetPos().z;
						sPos.y = GetPos().y + 3.0f;
						break;
					}
				}
				break;
#ifdef __XPET
			case IK3_PET:
				{
				#ifdef __GUILDCOMBAT
					// 길드컴뱃 맵에서는 팻 소환 금지

					if( GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						return FALSE;
					}
				#endif //__GUILDCOMBAT
						
					if( m_idPet == NULL_ID )	// 아직 소환 안한상태.
					{
						DWORD dwIndex = pItemProp->dwLinkKind;
						if( dwIndex != NULL_ID )
						{
							CMover *pMover = CreateMover( GetWorld(), dwIndex, GetPos() );
							if( pMover )
							{
								if( pMover->m_pAIInterface )
								{
									CAIPet *pAIPet = (CAIPet *)pMover->m_pAIInterface;
									if( pAIPet )
									{
										pAIPet->SetOwner( GetId() );
										m_idPet = pMover->GetId();		// 자기펫의 아이디를 가지고 있음.
									}
								}
							}
						} else
							Error( "아이템 %s 의 dwLinkKind가 지정되지 않음", pItemProp->szName );
					} 
					else
					{		// 펫을 이미 소환해놓은 상태.
						CMover *pPet = prj.GetMover( m_idPet );
						if( IsValidObj( pPet ) )
							pPet->Delete();			// 펫을 없앰.

						m_idPet = NULL_ID;		// 어떤이유로 인해 펫을 못찾는 상태라면 걍 없는걸로 함.
					}
				}
				break;
#endif // xPet
			} // switch
		}

		if( dwSfxID != NULL_ID )		// 아이템 사용시에 생성될 이펙트
			g_UserMng.AddCreateSfxObj( this, dwSfxID, sPos.x, sPos.y, sPos.z );	// 절대좌표로 하자.
		

		// 갯수가 0이면  아이템 삭제 
		pItemElem->UseItem();

		CHAR cUIParam = UI_NUM;
		if( dwGroup )	// 쿨타임 아이템이면 사용시각을 기록한다.
		{
			CUser* pUser = (CUser*)this;
			pUser->m_cooltimeMgr.SetTime( dwGroup, pItemProp->GetCoolTime() );	
			cUIParam = UI_COOLTIME;
		}

		// remove & transfer
		UpdateItem( dwId, cUIParam, pItemElem->m_nItemNum );
	}
	
#endif // WORLDSERVER

	return TRUE;
} // DoUseItem




#endif // // defined( __CLIENT ) && !defined( __ONLINE )

// IK2_BLINKWING의 처리.
BOOL CMover::DoUseItemBlinkWing( ItemProp *pItemProp, CItemElem* pItemElem, BOOL bUse )
{
#ifdef __WORLDSERVER
	if( m_pActMover->IsSit() )
		return FALSE;

	#ifdef __GUILDCOMBAT
		// 길드대전중에는 블링크윙 사용 금지

		CWorld* pWorld	= GetWorld();

		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			((CUser*) this)->AddDefinedText( TID_GAME_LIMITZONE_USE );
			return 0;
		}
	#endif //__GUILDCOMBAT

	#ifndef __NPP_050308
		if( m_nDuel )
		{
			// 듀얼중 블링크윙 사용 금지

			if( TRUE == IsPlayer() )
				((CUser*)this)->AddDefinedText( TID_GAME_PPVP_USEBRINKITEM, "" );
			return FALSE;
		}
	#endif // __NPP_050308

#ifdef __S0504_FLY
	if( IsStateMode( STATE_BASEMOTION_MODE ) ) 
		return FALSE;
#endif // __S0504_FLY

#ifdef __NPP_050308
	if( bUse )
	{
		if( GetWorld()->m_dwWorldID == WI_WORLD_KEBARAS )
			return FALSE;
	}
#endif // __NPP_050308

	// 제한레벨이 걸려있을때.
	if( pItemProp->dwLimitLevel1 != NULL_ID && pItemProp->dwLimitLevel1 > GetLevel() )
	{
		if( TRUE == IsPlayer() )
			( (CUser*)this )->AddDefinedText( TID_GAME_USINGNOTLEVEL, "\"%s\"", pItemProp->szName );
		return FALSE;
	}

	if( pItemProp->dwItemKind3 == IK3_TOWNBLINKWING )	// IK3_TOWNBLINKWING
	{
		// lodestar
		if( bUse )
		{
			if( IsUsableItem( pItemElem ) == FALSE )
				return FALSE;

			pItemElem->UseItem();
			UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum );
		}
		
		PRegionElem pRgnElem	= NULL;
		CWorld* pWorld	= GetWorld();
		if( !pWorld )
			return FALSE;
		
#ifdef __NPP_050308
		if( IsChaotic() )
			pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );
		else
#endif // __NPP_050308
		{
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), GetPos() );
			
		}
		if( NULL != pRgnElem )
		{
#ifdef __MSVR
			Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#else	// __MSVR
			Replace( pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#endif	// __MSVR
		}
		else
		{
#ifdef __MSVR
			Replace( g_uIdofMulti, pWorld->GetID(), GetPos() );
#else	// __MSVR
			Replace( pWorld->GetID(), GetPos() );
#endif	// __MSVR
		}
	}
	else
	{
		//		x: dwItemAtkOrder1
		//		y: dwItemAtkOrder2
		//		z: dwItemAtkOrder3
		//		angle: dwItemAtkOrder4
		//		dwWeaponType: worldid
		if( pItemProp->dwWeaponType == WI_WORLD_NONE || pItemProp->dwWeaponType == 0xffffffff )
			return FALSE;
		D3DXVECTOR3 vPos( pItemProp->dwItemAtkOrder1, pItemProp->dwItemAtkOrder2, pItemProp->dwItemAtkOrder3 );
		SetAngle( (float)pItemProp->dwItemAtkOrder4 );
		// remove & transfer
		if( bUse )
		{
			if( IsUsableItem( pItemElem ) == FALSE )
				return FALSE;

			pItemElem->UseItem();
			UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum );
		}
		Replace( g_uIdofMulti, pItemProp->dwWeaponType, vPos );
	}
#endif // worldserver
	return TRUE;
}

#ifdef __CHARGEDITEM0620
int	CMover::DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem, int nPart )
#else	// __CHARGEDITEM0620
int	CMover::DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem )
#endif	// __CHARGEDITEM0620
{
	int nResult = 0;

#ifdef __WORLDSERVER
	switch( pItemProp->dwID )
	{
	case II_CHR_SYS_SCR_RESTATE:
		{
			ReState();
		}
		break;
	case II_CHR_SYS_SCR_TRANSY:
		{
#ifdef __CHARGEDITEM0620
			if( DoUseItemSexChange( nPart ) == FALSE )
#else	// __CHARGEDITEM0620
			if( DoUseItemSexChange() == FALSE )
#endif	// __CHARGEDITEM0620
				nResult = 1;
		}
		break;
	case II_CHR_SYS_SCR_SHOUTFULL15:
		{
			if( !( IsSMMode( SM_SHOUT15 ) || IsSMMode( SM_SHOUT30 ) ) )
				SetSMMode( SM_SHOUT15, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_SHOUTFULL30:
		{
			if( !( IsSMMode( SM_SHOUT15 ) || IsSMMode( SM_SHOUT30 ) ) )
				SetSMMode( SM_SHOUT30, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_PSKILLFULL15:
		{
			if( !( IsSMMode( SM_PARTYSKILL15 ) || IsSMMode( SM_PARTYSKILL30 ) ) )
				SetSMMode( SM_PARTYSKILL15, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_PSKILLFULL30:
		{

			if( !( IsSMMode( SM_PARTYSKILL15 ) || IsSMMode( SM_PARTYSKILL30 ) ) )
				SetSMMode( SM_PARTYSKILL30, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_COMMBANK15:
		{
			if( !( IsSMMode( SM_BANK15 ) || IsSMMode( SM_BANK30 ) ) )
				SetSMMode( SM_BANK15, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_COMMBANK30:
		{
			if( !( IsSMMode( SM_BANK15 ) || IsSMMode( SM_BANK30 ) ) )
				SetSMMode( SM_BANK30, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_ACTIVITION:
		{
			if( !( IsSMMode( SM_ACTPOINT ) ) )
				SetSMMode( SM_ACTPOINT, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_UPCUTSTONE:
		{
			if( !( IsSMMode( SM_ATTACK_UP ) ) )
			{
				SetSMMode( SM_ATTACK_UP, pItemProp->dwCircleTime );
			}
			else
				nResult = 2;
		}
		break;
#ifdef __NPP_050308
	case II_SYS_SYS_VIR_PENALTYSTR:
		{
			//if( !( IsSMMode( SM_STR_DOWN ) ) )
			{
				KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nRecoverChao_Slaughter);

				if( pKarmaProp )
				{
					// 초단위 -> 시간다위 변경

					SetSMMode( SM_STR_DOWN, pKarmaProp->dwStatLimitTime);//*3600 );
				}
			}
			//else
			//	nResult = 2;
		}
		break;
	case II_SYS_SYS_VIR_PENALTYSTA:
		{
			//if( !( IsSMMode( SM_STA_DOWN ) ) )
			{
				KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nRecoverChao_Slaughter);
				if( pKarmaProp )
				{
					// 초단위 -> 시간다위 변경

					SetSMMode( SM_STA_DOWN, pKarmaProp->dwStatLimitTime);//*3600 );
				}
			}
			//else
			//	nResult = 2;
		}
		break;
	case II_SYS_SYS_VIR_PENALTYINT:
		{
			//if( !( IsSMMode( SM_INT_DOWN ) ) )
			{
				KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nRecoverChao_Slaughter);
				if( pKarmaProp )
				{
					// 초단위 -> 시간다위 변경

					SetSMMode( SM_INT_DOWN, pKarmaProp->dwStatLimitTime);//*3600 );
				}
			}
			//else
			//	nResult = 2;
		}
		break;
	case II_SYS_SYS_VIR_PENALTYDEX:
		{
			//if( !( IsSMMode( SM_DEX_DOWN ) ) )
			{
				KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nRecoverChao_Slaughter);
				if( pKarmaProp )
				{
					// 초단위 -> 시간다위 변경

					SetSMMode( SM_DEX_DOWN, pKarmaProp->dwStatLimitTime);//*3600 );
				}
			}
			//else
			//	nResult = 2;
		}
		break;
#endif //__NPP_050308	
#ifdef __S050524_SM_ITEM
	case II_SYS_SYS_SCR_FORGIVE:
		if( IsPlayer() )
		{
			if( IsChaotic() )
			{
				UpgradeKarma();
				((CUser*)this)->AddDefinedText( TID_GAME_GETKARMA );	// 카르마가 1단계 회복되었습니다						
			}
			else
			{
				// 카오틱 플레이어의 경우만 사용 할 수 있습니다.
				((CUser*)this)->AddDefinedText( TID_GAME_NOTCHOAS );
				nResult = 1;		// 1 - 아이템이 사용안되게 한다.
			}
		}
		break;

	case II_SYS_SYS_SCR_BLESSING:
		{
			if( !( IsSMMode( SM_REVIVAL ) ) )
				SetSMMode( SM_REVIVAL, pItemProp->dwCircleTime );
			else
				nResult = 2;					
		}
		break;
	case II_SYS_SYS_SCR_SELPAR:
		{
			CParty* pParty;
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
			pParty = g_PartyMng.GetParty( GetPartyId() );
			if( pParty )
			{
				if( pParty->m_nLevel < MAX_PARTYLEVEL )	// 단막극단일때만 사용가능
					pParty->SetPartyLevel( ((CUser*)this), 10, 180, 0 );
				else
					nResult = 1;
			}
			else
			{
				nResult = 1;
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1					

			if( nResult == 1 )
				((CUser*)this)->AddDefinedText( TID_GAME_NTROUPEO10, "" );
		}
		break;
	case II_SYS_SYS_SCR_HOLY:
		{
			m_SkillState.RemoveAllSkillDebuff();
		}
		break;
	case II_SYS_SYS_SCR_VELOCIJUMP:
		{
			if( !( IsSMMode( SM_VELOCIJUMP ) ) )
			{
				if( pItemProp->dwDestParam1 != -1 )
					SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->nAdjParamVal1 );
				if( pItemProp->dwDestParam2 != -1 )
					SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->nAdjParamVal2 );
				SetSMMode( SM_VELOCIJUMP, pItemProp->dwCircleTime );
			}
			else
			{
				nResult = 2;	
			}
		}
		break;
	case II_SYS_SYS_SCR_RECCURENCE:
		{
			if( InitSkillExp() )
				((CUser*)this)->AddInitSkillExp();
			else
				nResult = 1;
		}
		break;
#endif // __S050524_SM_ITEM


#ifdef __SM_ITEM_2ND
	case II_SYS_SYS_SCR_AMPES:	// "ES증폭의 두루마리"		
		{
			// ES, EM은 동일안 아이템.. I는 같이 사용할수 업습니다.
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ))
				nResult = 2;
			else if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	case II_SYS_SYS_SCR_AMPEM:	// "EM증폭의 두루마리"		
		{
			// ES, EM은 동일안 아이템.. I는 같이 사용할수 업습니다.
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ))
				nResult = 2;
			else if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	case II_SYS_SYS_SCR_AMPI:	// "I증폭의 두루마리"		
		{
/*			ItemProp *pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_AMPEM ); // ES증폭의 두루마리
			if( pItemProp == NULL )
				return 1;
			
			// ES, EM은 동일안 아이템.. I는 같이 사용할수 업습니다.
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ))
				nResult = 2;
			else if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
*/
		}
		break;
	case II_SYS_SYS_SCR_GLVCHEER:	// 응원의 흰 장갑
		{
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_GLVCHEER ) )
				nResult = 2;
			if( pItemProp )
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	// TODO 
	case II_SYS_SYS_SCR_SMELPROT:	// "보호의 두루마리"		
		{
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
#ifdef __SM_ITEM_2ND_EX
	case II_SYS_SYS_SCR_SMELPROT2:
		{
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
#endif	// __SM_ITEM_2ND_EX
	case II_SYS_SYS_SCR_SMELTING:	// "제련의 두루마리"	
		{
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	case II_SYS_SYS_SCR_SKLINSTRUCT:	// "기술의 전수자"		
		{
			if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_SKLINSTRUCT ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	case II_SYS_SYS_SCR_RETURN:		// "귀환의 두루마리"		
		{
			if( IsPlayer() )
			{
				if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
					nResult = 2;
				else
				{
					CWorld* pWorld	= GetWorld();
					if( !pWorld )
						return 0;

					D3DXVECTOR3 vPos = GetPos();
					//클락워크 돔/ 케바라스에서는 이 아이템을 사용할 수 없으며 만약 사용하려 시도 시 '이 지역은 제한 지역이므로 아이템을 사용할 수 없습니다.'라는 메시지를 출력한다
			#ifdef __GUILDCOMBAT
					if( pWorld->GetID() == WI_WORLD_GUILDWAR || pWorld->GetID() == WI_WORLD_KEBARAS || prj.IsGuildQuestRegion( vPos ) )
			#else //__GUILDCOMBAT
					if( pWorld->GetID() == WI_WORLD_KEBARAS || prj.IsGuildQuestRegion( vPos ) )
			#endif //__GUILDCOMBAT
					{			
						((CUser*) this)->AddDefinedText( TID_GAME_LIMITZONE_USE );
						return 0;
					}

					((CUser*) this)->SetReturnPos( vPos );

					//1.귀환모드로 설정 
					DoApplySkill( (CCtrl*)this, pItemProp, NULL );
					//2. 선택된 마을로 이동 
					LPCTSTR lpszKey = ((CUser*)this)->m_lpszVillage;
					if( lpszKey )
					{
						PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( GetWorld()->GetID(), lpszKey );
						if( pRgnElem )
						{
						#ifdef __MSVR
							Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
						#else	// __MSVR
							Replace( pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
						#endif	// __MSVR
						}
					}
					else
					{
						Error( "II_SYS_SYS_SCR_RETURN selected village is NULL\n" );
					}
				}
			}
		}
		break;
#endif // __SM_ITEM_2ND
#ifdef __INSTANT_BANK0831
	case II_SYS_SYS_SCR_CUSTODY2:
		{
			CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_INFO_PENG );
			lstrcpy( pMover->m_szCharacterKey, "MaFl_InstantBank" );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( GetPos() );
			pMover->InitMotion( pMover->m_dwMotion );
			pMover->UpdateLocalMatrix();
			pMover->m_dwTickCreated		= GetTickCount();
			GetWorld()->AddObj( pMover, TRUE );
			break;
		}
#endif	// __INSTANT_BANK0831
	default:
			nResult = 1;
		break;
	}
#endif // __WORLDSERVER
	return nResult;
}

// 소스보기 헷갈리니까 유료아이템만 따로 뺏다.
BOOL	CMover::DoUseItemFood_SM( ItemProp *pItemProp, CItemElem* pItemElem )
{
#ifdef __WORLDSERVER
	if( !( IsSMMode( SM_MAX_HP ) || IsSMMode( SM_MAX_HP50 ) ) )		// MAXHP 버프가 없을때만 사용됨.
	{
		DWORD nAdjParamVal;
		int nType;
		if( pItemProp->dwID == II_CHR_FOO_COO_BULLHAMS )
		{
			nAdjParamVal = pItemProp->nAdjParamVal2;
			nType = SM_MAX_HP;
		}
		else
		if( pItemProp->dwID == II_CHR_FOO_COO_GRILLEDEEL )
		{
			m_nPlusMaxHitPoint = nAdjParamVal = int( GetMaxOriginHitPoint() / ( 100 / pItemProp->nAdjParamVal1 ) );
			nType = SM_MAX_HP50;
		}
		else
			return FALSE;
			
		SetDestParam( pItemProp->dwDestParam1, nAdjParamVal, nAdjParamVal );
		SetSMMode( nType, pItemProp->dwCircleTime );
	}
	else
	{
		((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" );	// 사용중인 상용 아이템
		return FALSE;
	}
#endif
	return TRUE;
}

// IK2_FOOD / IK2_REFRESHER
BOOL	CMover::DoUseItemFood( ItemProp *pItemProp, CItemElem* pItemElem )
{
#ifdef __WORLDSERVER
	if( pItemProp->dwID == II_GEN_FOO_INS_SUPERHOTDOG )
	{
		SetPointParam( DST_HP, GetMaxHitPoint() );
		SetPointParam( DST_MP, GetMaxManaPoint() );
		SetPointParam( DST_FP, GetMaxFatiguePoint() );
		return TRUE;
	}

#ifdef __XITEM0722
	int nPoint;
	int nMax, nHP, nHPMax;
	for( int i = 0; i < 2; i ++ )
	{
		if( pItemProp->dwDestParam[i] != (DWORD)-1 )
		{
			nHP	   = GetPointParam( pItemProp->dwDestParam[i] );
			nPoint = pItemProp->nAdjParamVal[i];
			nHPMax = GetMaxPoint( pItemProp->dwDestParam[i]);
			nMax   = pItemProp->dwAbilityMin;
			
			if( pItemProp->dwCircleTime != -1)	// 유료아이템만 dwCircleTime으로 다 썼더라....
			{
				BOOL bRet = DoUseItemFood_SM( pItemProp, pItemElem );
				if( bRet == FALSE )	return FALSE;
			}
			else
			{
				if( pItemProp->dwDestParam[i] == DST_HP || pItemProp->dwDestParam[i] == DST_MP || pItemProp->dwDestParam[i] == DST_FP )
				{
					if( nPoint != -1 )
					{
						if( nHP >= nMax )
						{
							if( nHP + (nPoint * 0.3f) >= nHPMax )
								nHP = nHPMax;
							else
							{
								if( TRUE == IsPlayer() )
								{
									DWORD dwParam = 0;
									switch( pItemProp->dwDestParam[i] )
									{
									case DST_HP:	dwParam = TID_GAME_LIMITHP;		break;
									case DST_MP:	dwParam = TID_GAME_LIMITMP;		break;
									case DST_FP:	dwParam = TID_GAME_LIMITFP;		break;
									}
									
									((CUser*)this)->AddDefinedText( dwParam, "" );
								}
								nPoint = nPoint * 0.3f;
								nHP = nHP + nPoint;
							}
						}
						else
						{
							if( nHP + nPoint >= nHPMax )	// 포인트 더했을때 오바되는걸 처리.
								nHP = nHPMax;
							else
								nHP = nHP + nPoint;
						}
					}
					
					SetPointParam( pItemProp->dwDestParam[i], nHP );
				} // DST_HP, MP, FP
 #ifdef __XITEMBUFF
				else
				{
  #ifndef __XITEM0722
					if( pItemProp->dwSkillTime != NULL_ID )	// 지속시간이 있는 아이템을 쓸대

  #endif // not xItem0722
						DoApplySkill( this, pItemProp, NULL );
				}
 #endif // xItemBuff
			}
		}
	} // for
#else // xItem0722
//{{AFX
	int nPoint;
	int nMax, nHP, nHPMax;
	if( pItemProp->dwDestParam1 != (DWORD)-1 )
	{
		nHP	   = GetPointParam( pItemProp->dwDestParam1 );
		nPoint = pItemProp->nAdjParamVal1;
		nHPMax = GetMaxPoint( pItemProp->dwDestParam1);
		nMax   = pItemProp->dwAbilityMin;

		if( pItemProp->dwCircleTime != -1)	// 유료아이템만 dwCircleTime으로 다 썼더라....
		{
			BOOL bRet = DoUseItemFood_SM( pItemProp, pItemElem );
			if( bRet == FALSE )	return FALSE;
		}
		else
		{
			if( pItemProp->dwDestParam1 == DST_HP || pItemProp->dwDestParam1 == DST_MP || pItemProp->dwDestParam1 == DST_FP )
			{
				if( nPoint != -1 )
				{
					if( nHP >= nMax )
					{
						if( nHP + (nPoint*0.3f) >= nHPMax )
							nHP = nHPMax;
						else
						{
							if( TRUE == IsPlayer() )
							{
								DWORD dwParam = 0;
								switch( pItemProp->dwDestParam1 )
								{
								case DST_HP:	dwParam = TID_GAME_LIMITHP;		break;
								case DST_MP:	dwParam = TID_GAME_LIMITMP;		break;
								case DST_FP:	dwParam = TID_GAME_LIMITFP;		break;
								}
								
								( (CUser*)this )->AddDefinedText( dwParam, "" );
							}

							nPoint = nPoint*0.3f;
							nHP = nHP + nPoint;
						}
					}
					else
					{
						if( nHP + nPoint >= nHPMax )	// 포인트 더했을때 오바되는걸 처리.
							nHP = nHPMax;
						else
							nHP = nHP + nPoint;
					}
				}
				
	//			if( nPoint == -1 )
	//				SetPointParam( pItemProp->dwDestParam1, nPoint );
	//			else
					SetPointParam( pItemProp->dwDestParam1, nHP );
			} // DST_HP, MP, FP
#ifdef __XITEMBUFF
			else
			{
				if( pItemProp->dwSkillTime != NULL_ID )	// 지속시간이 있는 아이템을 쓸대

					DoApplySkill( this, pItemProp, NULL );
			}
#endif // xItemBuff
		}
	}
	
	if( pItemProp->dwDestParam2 != (DWORD)-1 )
	{
		nHP	   = GetPointParam( pItemProp->dwDestParam2 );
		nPoint = pItemProp->nAdjParamVal2;
		nHPMax = GetMaxPoint( pItemProp->dwDestParam2);
		nMax   = pItemProp->dwAbilityMin;
		
		if( nPoint != -1 )
		{
			if( nHP >= nMax )
			{
				if( nHP + (nPoint*30 / 100) >= nHPMax )
				{
					nHP = nHPMax;
				}
				else
				{
					if( TRUE == IsPlayer() )
					{
						DWORD dwParam = 0;
						switch( pItemProp->dwDestParam1 )
						{
						case DST_HP:
							dwParam = TID_GAME_LIMITHP;
							break;
						case DST_MP:
							dwParam = TID_GAME_LIMITMP;
							break;
						case DST_FP:
							dwParam = TID_GAME_LIMITFP;
							break;
						}
						
						( (CUser*)this )->AddDefinedText( dwParam, "" );
					}
					
					nPoint = nPoint*30 / 100;
					nHP = nHP + nPoint;
				}
			}
			else
			{
				if( nHP + nPoint >= nHPMax )
				{
					nHP = nHPMax;
				}
				else
				{
					nHP = nHP + nPoint;
				}
			}
		}
		
		SetPointParam( pItemProp->dwDestParam2, nHP );
	}
//}}AFX
#endif // not xItem0722
#endif // worldserver
	
	return TRUE;
}

#ifdef __WORLDSERVER
#ifdef __CHARGEDITEM0620
BOOL CMover::DoUseItemSM( ItemProp *pItemProp, CItemElem* pItemElem, D3DXVECTOR3* pFos, int nPart )
#else	// __CHARGEDITEM0620
BOOL CMover::DoUseItemSM( ItemProp *pItemProp, CItemElem* pItemElem, D3DXVECTOR3* pFos )
#endif	// __CHARGEDITEM0620
{
	int nResult = 0;
	switch( pItemProp->dwItemKind2 )
	{
	case IK2_FOOD:
		if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
			nResult = 1;
		break;
	case IK2_SYSTEM:
		{
			nResult = DoUseItemSystem( pItemProp, pItemElem );
		}
		break;
	case IK2_REFRESHER:
		{
			if( SetSMMode( SM_MAINTAIN_MP, pItemProp->dwCircleTime ) )
				SetPointParam( DST_MP, GetMaxManaPoint() );
			else
				nResult = 2;
		}
		break;
	case IK2_POTION:
		{
			if( SetSMMode( SM_MAINTAIN_FP, pItemProp->dwCircleTime ) )
				SetPointParam( DST_FP, GetMaxFatiguePoint() );
			else
				nResult = 2;
		}
		break;
	case IK2_BLINKWING:
		{
#ifdef __YESCAPE_MOTION
			if( !IsSMMode( SM_ESCAPE ) )
			{
				DoUseItemBlinkWing( pItemProp, NULL, FALSE );
				SetSMMode( SM_ESCAPE, pItemProp->dwCircleTime );
			}
			else
			{
				nResult = 2;
			}		
#endif // __YESCAPE_MOTION
		}
		break;
	default:
		break;
	} // ik2
	
	switch( pItemProp->dwItemKind3 )
	{
	case IK3_MAGICTRICK:	// 이펙트 아이템류.
		{
			float	fTheta = D3DXToRadian( GetAngle() );
			switch( pItemProp->dwID )
			{
			case II_CHR_MAG_TRI_ROCKETBOMB:
			case II_CHR_MAG_TRI_HEARTBOMB:
			case II_CHR_MAG_TRI_TWISTERBOMB:
				pFos->x = GetPos().x + sinf(fTheta) * 3.0f;			// 바라보는 방향 1미터 앞에다 발생시킴.
				pFos->z = GetPos().z + -cosf(fTheta) * 3.0f;
				pFos->y = GetPos().y + 1.5f;
				pFos->y = GetWorld()->GetUnderHeight( D3DXVECTOR3(pFos->x,pFos->y,pFos->z) );
				break;
			}
		}
		break;
	} // ik3
	
	if( 0 < nResult )
	{
		if( nResult == 2 )
		{
			((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
		}
#ifdef __SM_ITEM_2ND
		else if( nResult == 3 )
		{
			((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
		}
#endif // __SM_ITEM_2ND
		return FALSE;
	}
	else
	{
		// 상용화 아이템 사용 로그 삽입
		g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
	}
	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
#ifdef __CHARGEDITEM0620
BOOL CMover::DoUseItemSexChange( int nFace )
#else	// __CHARGEDITEM0620
BOOL CMover::DoUseItemSexChange( )
#endif	// __CHARGEDITEM0620
{
	// 1.
	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
	{
		if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_LWEAPON
			|| dwParts == PARTS_RWEAPON || dwParts == PARTS_SHIELD || dwParts == PARTS_RIDE
			|| ( dwParts >= PARTS_NECKLACE1 && dwParts <= PARTS_BULLET ) )
			continue;
		
		CItemElem* pArmor	= m_Inventory.GetEquip( dwParts );
		if( pArmor )
		{
		#ifdef __S050524_SM_ITEM
			((CUser*)this)->AddDefinedText( TID_GAME_CHECK_EQUIP, "" );
				return FALSE;
		#else // __S050524_SM_ITEM
			if( DoEquip( pArmor, FALSE ) == TRUE )
			{
				g_UserMng.AddDoEquip( this, -1, pArmor, FALSE );
			}
			else
			{
				//
			}
		#endif // __S050524_SM_ITEM
		}
	}

	// 2.
	DWORD dwIndex;
	if( GetSex() == SEX_MALE )
	{
		dwIndex		= MI_FEMALE;
		SetSex( SEX_FEMALE );
	}
	else
	{
		dwIndex	= MI_MALE;
		SetSex( SEX_MALE );
	}

	SetTypeIndex( D3DDEVICE, OT_MOVER, dwIndex );
	ResetScale();
	SetMotion( MTI_WALK );
	UpdateLocalMatrix();

	// 3.
	g_UserMng.AddSexChange( this );

	// 4.
#ifdef __CHARGEDITEM0620
	m_dwHeadMesh	= (DWORD)nFace;
	g_UserMng.AddChangeFace( m_idPlayer, (DWORD)nFace );
#endif // __CHARGEDITEM0620

	return TRUE;
}
#endif	// __WORLDSERVER

/*--------------------------------------------------------------------------------------------------------


											Do Wand Attack


----------------------------------------------------------------------------------------------------------*/
//
// 완드어택을 하라!
//
int	 CMover::DoAttackMagic( CObj *pTargetObj, int nMagicPower, int idSfxHit )
{
	if( IsFly() )
		return( -1 );

	if( IsInvalidObj(pTargetObj) )		// 타겟이 거시기 하면 리턴
		return( -1 );

	ItemProp* pItemProp;
#ifndef __ONLINE
	CItemElem* pItemElem = GetWeaponItem();	// 손에든 무기가 없으면 리턴
	if( pItemElem == NULL )
		return( - 1 ) ;
	pItemProp	= pItemElem->GetProp();
#else	// __ONLINE
	if( IsActiveMover() ) {
		CItemElem* pItemElem = GetWeaponItem();
		if( pItemElem == NULL )
			return( - 1 ) ;
		pItemProp	= pItemElem->GetProp();
	}
	else {
#ifdef __FINITEITEM0705
		DWORD dwItemId	= m_aEquipInfo[PARTS_RWEAPON].dwId;
#else	// __FINITEITEM0705
		DWORD dwItemId	= m_adwEquipment[PARTS_RWEAPON];
#endif	// __FINITEITEM0705
		if( dwItemId == NULL_ID )
			return( -1 );
		pItemProp	= prj.GetItemProp( dwItemId );
	}
#endif	// __ONLINE

	if( pItemProp->dwItemKind3 != IK3_WAND /* && pItemProp->dwItemKind3 != IK3_STAFF */)		// 들고있는 무기가 완드가 아니면 에러.
		return( -1 );

#ifdef __CLIENT
	// 현재 지점과 목표지점까지 막히는게 있으면 메시지 출력하고 리턴
	CWorld *pWorld = GetWorld();
	D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
	D3DXVECTOR3 vEnd   = pTargetObj->GetPos();	vEnd.y += 0.5f;

	if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
	{
		g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
		
		return -1;
	}
#endif  // CLIENT

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_ATK_MAGIC1, idTarget ) ) == 1 )	// 완드공격 동작 시작.
	{
		if( m_pActMover->IsFly() == FALSE )		
			SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );		// 실행중 사용할 파라메터 등록
		
#ifdef __CLIENT	
		if( nMagicPower >= 1 )	// 레벨 2이상을 쓰면 액션포인트 하나 올라감.
		{
			if( g_WndMng.m_pWndTaskBar->m_nActionPoint < 100 )
				g_WndMng.m_pWndTaskBar->m_nActionPoint ++;
		}
#endif
		
#ifdef __WORLDSERVER
		if( IsPlayer() )
		{
			if( nMagicPower >= 1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100 )	// 2단계충전이상 사용하면 액션포인트 올라감.
				((CUser*)this)->m_playTaskBar.m_nActionPoint ++;
		}
		if( nMagicPower == 3 )		// 4단계짜리를 쓰면 마나 5포인트 감소
			IncManaPoint( -GetReqMp( pItemProp->dwReqMp ) );	
		g_UserMng.AddMagicAttack( this, OBJMSG_ATK_MAGIC1, idTarget, 0, 0, nMagicPower, idSfxHit );
#else	// __WORLDSERVER
#ifdef __ONLINE
		if( IsActiveMover() ) {
			SetActParam( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );	// 효과 식별자 등록
			g_DPlay.SendMagicAttack( OBJMSG_ATK_MAGIC1, ( (CCtrl*)pTargetObj )->GetId(), 0, 0, nMagicPower, m_idSfxHit );
		}
#endif	// __ONLINE
#endif	// __WORLDSERVER
	}
	return( nError );
}

/*--------------------------------------------------------------------------------------------------------


											Do Range Attack


----------------------------------------------------------------------------------------------------------*/
//
// 레인지어택을 하라!
// dwItemID : 무기아이템 아이디

//
#ifdef	__ACROBAT_0504
int	 CMover::DoAttackRange( CObj *pTargetObj, DWORD dwItemID, int idSfxHit )
{
	ItemProp* pItemProp = NULL;
	int nPower = 0;

	if( IsInvalidObj(pTargetObj) )
		return -1;
	

#ifdef __CLIENT
	if( IsActiveMover() )
	{
		// 현재 지점과 목표지점까지 막히는게 있으면 메시지 출력하고 리턴
		CWorld *pWorld = GetWorld();
		D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
		D3DXVECTOR3 vEnd   = pTargetObj->GetPos();	vEnd.y += 0.5f;

		if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
			
			return -1;
		}
	}
#endif  // CLIENT
	

	if( IsPlayer() )
	{
		pItemProp = GetActiveHandItemProp();			
		nPower = dwItemID;

#ifdef __WORLDSERVER
		CItemElem* pItemElem = m_Inventory.GetEquip( PARTS_BULLET );	// 화살이 있는지 체크
		if( pItemElem == NULL || pItemElem->GetProp()->dwItemKind3 != IK3_ARROW )
		{
			return -1;
		}
#endif // __WORLDSERVER
	}
	else
	{
		if( dwItemID == NULL_ID || dwItemID == 0 )	
			pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
		else
			pItemProp = prj.GetItemProp( dwItemID );
	}
	
	if( pItemProp == NULL )
	{
		Error( "DoAttackRange : %s ActiveHandItemProp = NULL", m_szName );
		return -1;
	}

	if( pItemProp->dwWeaponType != WT_RANGE && pItemProp->dwWeaponType != WT_RANGE_BOW )		// 들고있는 무기가 RANGE가 아니면 리턴
	{
		return -1;
	}
	
	DWORD dwUseMotion = pItemProp->dwUseMotion;	
	if( dwUseMotion == NULL_ID )
	{
		if( IsPlayer() )
		{
			dwUseMotion = MTI_ATK_13 - MTI_STAND_13;//23;
		}
		else
		{
			Message( "DoAttackRange : %s [%d]'s dwUseMotion = NULL_ID", m_szName, pItemProp->dwID );
			dwUseMotion = MTI_ATK1;		// 프로퍼티에 장거리 공격 모션이 지정되지 않았으면 기본 모션으로 한다.
		}
	}

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_ATK_RANGE1, idTarget, (int)dwUseMotion ) ) == 1 )	// 장거리공격.
	{
		SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );			// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_RANGE_ATTACK, idTarget, dwItemID, idSfxHit );	// 효과 식별자 등록
		
#ifdef __WORLDSERVER
		g_UserMng.AddRangeAttack( this, OBJMSG_ATK_RANGE1, idTarget, dwItemID, 0, idSfxHit );
		ArrowDown( 1 );		// 화살 1개 없앰
#else	// __WORLDSERVER
	#ifdef __ONLINE
		if( IsActiveMover() )	// 클라에서 플레이어가 사용한거라면 서버로 정보 보냄.
			g_DPlay.SendRangeAttack( OBJMSG_ATK_RANGE1, ( (CCtrl*)pTargetObj )->GetId(), dwItemID, m_idSfxHit ); 
	#endif	// __ONLINE
#endif	// __WORLDSERVER
	}
	return nError;
}
	
#else	// !__ACROBAT_0504
int	 CMover::DoAttackRange( CObj *pTargetObj, DWORD dwItemID, int idSfxHit )
{
	if( IsInvalidObj(pTargetObj) )
		return 0;

	ItemProp* pItemProp;
	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	if( pItemProp == NULL )
	{
		Error( "DoAttackRange : %s 의 ActiveHandItemProp이 NULL", m_szName );
		return 0;
	}

	if( pItemProp->dwWeaponType != WT_RANGE )		// 들고있는 무기가 RANGE가 아니면 리턴
	{
		return	0;
	}

	DWORD dwUseMotion = pItemProp->dwUseMotion;
	if( dwUseMotion == NULL_ID )
	{
		Message( "DoAttackRange : %s의 공격%d의 dwUseMotion이 지정되지 않았다", m_szName, pItemProp->dwID );
		dwUseMotion = MTI_ATK1;		// 프로퍼티에 장거리 공격 모션이 지정되지 않았으면 기본 모션으로 한다.
	}

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_ATK_RANGE1, idTarget, (int)dwUseMotion ) ) == 1 )	// 장거리공격.
	{
		SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_RANGE_ATTACK, idTarget, dwItemID, idSfxHit );	// 효과 식별자 등록
		
#ifdef __WORLDSERVER
		g_UserMng.AddRangeAttack( this, OBJMSG_ATK_RANGE1, idTarget, dwItemID, 0, idSfxHit );
#else	// __WORLDSERVER
#ifdef __ONLINE
//		if( IsActiveMover() )	// 클라에서 플레이어가 사용한거라면 서버로 정보 보냄.
//			g_DPlay.SendRangeAttack( OBJMSG_ATK_RANGE1, ( (CCtrl*)pTargetObj )->GetId(), 0, 0, m_idSfxHit );
#endif	// __ONLINE
#endif	// __WORLDSERVER
	}
	return nError;
}
#endif // !__ACROBAT_0504


// 몬스터 특수공격
int		CMover::DoAttackSP( CObj *pTargetObj, DWORD dwItemID )
{
	if( IsInvalidObj(pTargetObj) )
		return( -1 );
	
	ItemProp* pItemProp;
	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	if( pItemProp == NULL )
	{
		Error( "DoAttackSP : %s 의 ItemProp이 NULL %d", m_szName, (int)dwItemID );
		return FALSE;
	}

	DWORD dwUseMotion = pItemProp->dwUseMotion;
	if( dwUseMotion == NULL_ID )
	{
		Message( "DoAttackSP : %s의 공격%d의 dwUseMotion이 지정되지 않았다", m_szName, pItemProp->dwID );
		dwUseMotion = MTI_ATK1;		// 프로퍼티에 장거리 공격 모션이 지정되지 않았으면 기본 모션으로 한다.
	}
	
	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();
	
	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_SP_ATK1, idTarget, (int)dwItemID, (int)dwUseMotion ) ) == 1 )	// 장거리공격.
	{
		SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_SP_ATTACK, idTarget, (int)dwItemID );	// 효과 식별자 등록
		
#ifdef __WORLDSERVER
		g_UserMng.AddAttackSP( this, OBJMSG_SP_ATK1, idTarget, dwItemID, 0 );
#else	// __WORLDSERVER
#ifdef __ONLINE
#endif	// __ONLINE

#ifdef __NPP_050308
		DWORD dwSfxObj = pItemProp->dwSfxObj3;		// 특수공격에 발동이펙트가 있다면 3번사용.
#else
		DWORD dwSfxObj = pItemProp->dwSfxObj;
#endif
		if( dwSfxObj != NULL_ID )
			CreateSfx( D3DDEVICE, dwSfxObj, GetPos(), GetId(), D3DXVECTOR3(0,0,0), idTarget );

		
#endif	// __WORLDSERVER
	}
	return( nError );
	
	
}

/*--------------------------------------------------------------------------------------------------------


											Do Melee Attack


----------------------------------------------------------------------------------------------------------*/
int		CMover::DoAttackMelee( CMover *pTarget, OBJMSG dwMsg, DWORD dwItemID )
{
	SendActMsg( OBJMSG_STOP );		// 제자리 멈춤.

	if( IsInvalidObj(pTarget) )
		return 0;
	
	ItemProp* pItemProp;
	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	if( pItemProp == NULL )
	{
		Error( "DoAttackMelee : %s 의 ItemProp이 NULL %d", GetName(), (int)dwItemID );
		return FALSE;
	}

	// 모션 시작하고.
	int nError	= SendActMsg( dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, 0 ) );
	if( nError > 0 )
	{

		SetActParam( OBJACT_MELEE_ATTACK, pTarget->GetId(), (int)dwItemID );	// OnAttackMelee에서 쓸수 있도록 파라메터 등록.

#ifdef __CLIENT
		g_DPlay.SendMeleeAttack( dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
		if( IsActiveMover() )
		{
#ifdef __V041122_MUSIC
				PlayCombatMusic();
#else
			if( IsRegionAttr( RA_DANGER ) )
				PlayMusic( BGM_BATTLE02 );
			else
				PlayMusic( BGM_BATTLE01 );
#endif
			CItemElem* pWeapon	= GetWeaponItem();
			if( pWeapon )
			{
				ItemProp* pItemProp		= pWeapon->GetProp();
				if( pItemProp )
					if( ( ( pWeapon->m_nHitPoint * 100 ) / pItemProp->dwEndurance ) == 0 )
						g_WndMng.PutString( prj.GetText( TID_GAME_WASTEENDURANCE ), NULL, prj.GetTextColor( TID_GAME_WASTEENDURANCE ) );
			}
		}
#elif defined(__WORLDSERVER)

		g_UserMng.AddMeleeAttack( this, dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );

#endif	// __WORLDSERVER
	}
				

	return TRUE;
	
}	

#endif	// not __BEAST


// 주변에 있는 아이템중 하나를 집는다.
void	CMover::DoPickupItemAround( void )
{
#ifdef __CLIENT

	if( m_pActMover->GetMoveState() == OBJSTA_PICKUP )	// 이미 뭔가를 줍고있다면 취소
		return;
	if( m_pActMover->IsActAttack() )	return;		// 공격중 사용안됨.
	if( IsDie() )		return;						// 죽었을땐 사용안됨.
	
	int nRange	= 4;	// 4m
	float fDistMin = 99999.0f, fDistSq;
	CObj* pObj, *pMinObj = NULL;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;
	FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )
	{
		if( pObj->GetType() == OT_ITEM )
		{
			vDist = pObj->GetPos() - vPos;		// this -> 아이템까지의 벡터
			fDistSq = D3DXVec3LengthSq( &vDist );
			if( fDistSq < 4.0f * 4.0f )		// 아이템과의 거리가 x미터 이내인것을 대상으로.
			{
				if( fDistSq < fDistMin )	// 그중 젤 가까운놈으로 선택
				{
					fDistMin = fDistSq;
					pMinObj = pObj;
				}
			}
		}
	}
	END_LINKMAP

	// 아이템 집을게 있다.
	if( pMinObj )
	{
		CMD_SetUseItem( (CCtrl*)pMinObj );
	}

#endif
}


// this를 타겟으로 주변에 데미지를 준다.
// ------------------------------------------
// nDmgType : 데미지 타입 ex) AF_MAGICSKILL
// pAttacker : 공격자(시전자) - 앞으로 CCtrl*로 바꿔야 한다
// nApplyType : 적용대상. ex) OBJTYPE_PLAYER | OBJTYPE_MONSTER
// nSkill : 사용한 스킬종류

// fRange : 타겟을 중심으로한 데미지 범위(미터)
// fTargetRatio : 타겟에게 적용되는 데미지 비율
// fAroundRatio : 주변에 적용되는 데미지 비율
// (AF_MAGICSKILL, this, nSkill, 5.0f, 0.0, 1.0f )
void	CMover::SendDamageAround( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( fRange <= 4.0f )
		nRange = 4;
	else if( fRange <= 8.0f )
		nRange = 8;
	else if( fRange <= 16.0f )
		nRange = 16;
	else
		nRange = 32;
	
	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Message( "CMover::SendDamageAround : D:%s A:%s %d %f", GetName(), pAttacker->GetName(), nAttackID, fRange );

	ItemProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageAround : %s. 스킬(%d)의 프로퍼티가 업다.", m_szName, nAttackID );
			return;	// property not found
		}
	} else
	{
		pProp = prj.GetItemProp( nAttackID );		// 아이템 프로퍼티꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageAround : %s. 아이템(%d)의 프로퍼티가 업다.", m_szName, nAttackID );
			return;	// property not found
		}
	}
	
//	int nLinkType;
//	if( pAttacker->IsPlayer() )		// 공격자가 플레이어면

//		nLinkType = CObj::linkDynamic;	// 몬스터만 검색

//	else	// 공격자가 플레이어가 아니면

//		nLinkType = CObj::linkPlayer;	// 유저만 검색

	BOOL	bDamage = FALSE;
	BOOL	bTarget = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				if( pAttacker->IsPlayer() )
				{
#ifdef __NPP_050308
					bDamage = TRUE;
#else	
					if( pAttacker->m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == pAttacker->m_idDuelParty )	// 상대 파티원에게만 데미지

							bDamage = TRUE;
					} else
					if( pAttacker->m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
					{
						if( ((CMover*)pObj)->GetId() == pAttacker->m_idDuelOther )	// 듀얼상대에게만
							bDamage = TRUE;
					}
#endif	// __NPP_050308
				} else
				// 공격자가 몬스터
				{
					bDamage = TRUE;
				}

				if( bDamage )
				{
					vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
					fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
					{
						if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
						{
						} else
						{
							if( pObj != pAttacker )		// 어태커는 검색대상에서 제외.
							{
								pTarget = (CMover *)pObj;
								bTarget = ( pTarget == this );
								if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
								{
									if( pProp->dwComboStyle == CT_FINISH )
										pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
									else
										pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
								}
							}
						}
					}
					
					bDamage = FALSE;	// 다음 루프를 위해서 초기화.
				} // bDamage
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pAttacker )		// 공격자는 검사대상에서 제외.
						{
							pTarget = (CMover *)pObj;
							if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
							{
								if( pProp->dwComboStyle == CT_FINISH )
									pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
								else
									pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // SendDamageAround()

BOOL	CMover::DoCollect( CMover *pTarget )
{
	SendActMsg( OBJMSG_STOP );
	BOOL bSuccess = SendActMsg( OBJMSG_COLLECT );
	ClearDest();
		
	return bSuccess;
}


// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
void	CMover::SendDamageLine( int nDmgType, int nApplyType, int nAttackID, 
								float fWidth, float fHeight, float fDepth )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	// 데미지 영역 Local AABB.
	D3DXVECTOR3 vMin1 = D3DXVECTOR3( -fWidth / 2.0f, -fHeight / 2.0f, -fDepth );
	D3DXVECTOR3 vMax1 = D3DXVECTOR3(  fWidth / 2.0f,  fHeight / 2.0f, 0 );

	D3DXMATRIX mInv;		// 공격자측 월드역행렬
	D3DXMATRIX m1, m2, m3;
	D3DXMatrixRotationY( &m1, D3DXToRadian( -GetAngle() ) );
	D3DXMatrixTranslation( &m2, GetPos().x, GetPos().y, GetPos().z );
	D3DXMatrixMultiply( &m3, &m1, &m2 );	// world = rot * trans
	
	D3DXMatrixInverse( &mInv, NULL, &m3 );		// 공격자의 역행렬 만듬.

	// 타겟의 좌표를 공격자측 기준으로 역변환 한것.
	D3DXVECTOR3 vDestLocal;

	// 일반적으로 fDepth가 가장 길기때문에 검사 영역은 fDepth로 했다. 
	if( fDepth <= 4.0f )
		nRange = 4;
	else if( fDepth <= 8.0f )
		nRange = 8;
	else if( fDepth <= 16.0f )
		nRange = 16;
	else
		nRange = 32;
	
	if( fDepth <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Message( "CMover::SendDamageLine : D:%s A:%s %d %f", GetName(), nAttackID, fDepth );

	ItemProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageLine : %s. 스킬(%d)의 프로퍼티가 업ㅂ다.", m_szName, nAttackID );
			return;	// property not found
		}
	} else
	{
		pProp = prj.GetItemProp( nAttackID );		// 아이템 프로퍼티꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageLine : %s. 아이템(%d)의 프로퍼티가 업ㅂ다.", m_szName, nAttackID );
			return;	// property not found
		}
	}
	
//	int nLinkType;
//	if( pAttacker->IsPlayer() )		// 공격자가 플레이어면

//		nLinkType = CObj::linkDynamic;	// 몬스터만 검색

//	else	// 공격자가 플레이어가 아니면

//		nLinkType = CObj::linkPlayer;	// 유저만 검색

	BOOL	bDamage = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				bDamage = FALSE;	// 시작은 FALSE

				if( IsPlayer() )
				{
#ifdef __NPP_050308
					bDamage = TRUE;
#else // __NPP_050308
					if( m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == m_idDuelParty )	// 상대 파티원에게만 데미지

							bDamage = TRUE;
					} else
					if( m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
					{
						if( ((CMover*)pObj)->GetId() == m_idDuelOther )	// 듀얼상대에게만
							bDamage = TRUE;
					}
#endif // __NPP_050308
				} else
				// 공격자가 몬스터
				{
					bDamage = TRUE;
				}

				if( bDamage && pObj != this )		// this는 공격대상에서 제외.
				{
					pTarget = (CMover *)pObj;
					if( IsValidObj( pTarget ) && pTarget->IsLive() )
					{
						// 타겟측 좌표를 역변환.
						D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
						// 타겟측 AABB
						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
						{
							int a = 0;		// 충돌
/*							if( pProp->dwComboStyle == CT_FINISH )
								pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							else
								pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) ); */
						}
					}
				} // bDamage
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				pTarget = (CMover *)pObj;
				if( IsValidObj( pTarget ) && pTarget->IsLive() )
				{
					// 타겟측 좌표를 역변환.
					D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
					// 타겟측 AABB
					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
					{
						if( pProp->dwComboStyle == CT_FINISH )
							pTarget->m_pActMover->SendDamageForce( nDmgType, GetId(), (nAttackID << 16) );
						else
							pTarget->m_pActMover->SendDamage( nDmgType, GetId(), (nAttackID << 16) ); 
					}
				}
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // SendDamageLine()

#ifdef __J0519
#ifdef __WORLDSERVER
void CMover::QuerySetPlayerName( const char* lpszPlayer, DWORD dwData )
{
	g_dpDBClient.SendQuerySetPlayerName( m_idPlayer, lpszPlayer, dwData );
}
#endif	// __WORLDSERVER
#endif	// __J0519
