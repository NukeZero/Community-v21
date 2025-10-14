
#include "stdafx.h"
#include "FLSkillSystem.h"

//////////////////////////////////////////////////////////////////////////
#include "Mover.h"
#include "RainbowRace.h"
#include "eveschool.h"

#ifdef	__WORLDSERVER
#include "party.h"
#include "worldmng.h"
#include "CtrlObjs.h"
#include "../worldserver/FLMapGrid.h"
#include "../worldserver/LinkMap.h"
#endif

#ifdef	__CLIENT
#include <defineText.h>
#include <ResData.h>
#include "../Neuz/DPClient.h"
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
extern	CGuildCombat	g_GuildCombatMng;

#ifdef	__WORLDSERVER
extern	CPartyMng		g_PartyMng;
extern	CWorldMng		g_WorldMng;
#endif

#ifdef	__CLIENT
extern	CDPClient		g_DPlay;
#endif
//////////////////////////////////////////////////////////////////////////


FLSkillSystem*	FLSkillSystem::GetInstance()
{
	static FLSkillSystem cSkillSystem;
	return &cSkillSystem;
}

FLSkillSystem::FLSkillSystem()
{
}

FLSkillSystem::~FLSkillSystem()
{
}

bool	FLSkillSystem::CanUseSkill( CMover* pUseMover, const DWORD dwSkillID, const DWORD dwSkillLevel, const OBJID idFocusObj, const SKILLUSETYPE sutType )
{
	//////////////////////////////////////////////////////////////////////////
	// 스펙 찾기
	SkillProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	if( pUseMover->GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkillID, dwSkillLevel, "DoUseSkill" ) == FALSE )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	// imParkth - 타인 대상 스킬인데 자기 자신이 대상이 되면 실패
	if( NULL == pSkillProp || EXT_ANOTHER == pSkillProp->dwExeTarget && pUseMover->GetId() == idFocusObj )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 상태 확인
	if( pUseMover->IsDie() || pUseMover->m_pActMover->IsFly() )
	{
		return false;
	}

	// 공격금지 상태에선 스킬 못씀.
	if( pUseMover->m_dwFlag & MVRF_NOATTACK )
	{
#ifdef __WORLDSERVER	
		((FLWSUser*) pUseMover)->AddText( prj.GetText( TID_GAME_STILLNOTUSE ) );	
#endif
		return false;
	}
	if( pUseMover->IsMode( NO_ATTACK_MODE ) )
	{
		return false;
	}

	// 액션슬롯으로 사용한 스킬일때 액션슬롯 등록가능여부 체크
	if( _GetContentState( CT_NEWUI_19 ) != CS_VER1 && ( sutType == SUT_QUEUESTART || sutType == SUT_QUEUEING ) )
	{
		if( pSkillProp->bCanUseActionSlot == false )
		{
			return false;
		}
	}

#ifdef __WORLDSERVER
	// 버서크가 걸려있는 상태에선 스킬사용 금지.
	if( pUseMover->HasBuff( BUFF_SKILL, SKILL_INDEX( 143, SI_BLD_SUP_BERSERK ) ) )
	{
		return false;
	}

	// 레인보우 레이스
	if( pSkillProp->dwID == SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) && pUseMover->IsPlayer() &&
		CRainbowRaceMng::GetInstance()->IsEntry( pUseMover->m_idPlayer ) )
	{
		((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
		return false;
	}
#endif
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 장소(World) 확인
	CWorld* pWorld	= pUseMover->GetWorld();
	if( pWorld == NULL )
	{
		return false;
	}

	DWORD dwWorldID = pWorld->GetID();
	if( dwWorldID == WI_WORLD_QUIZ )
	{
		return false;
	}
	// 길드컴뱃중에는 부활스킬 금지
	if( dwWorldID == WI_WORLD_GUILDWAR )
	{
		// 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwID == SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ) || pSkillProp->dwID == SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) )
		{
			return false;
		}

		if( g_GuildCombatMng.m_nState != CGuildCombat::WAR_STATE )
		{
			if( pSkillProp->dwID == SKILL_INDEX( 45, SI_ASS_HEAL_RESURRECTION ) )
			{
				return false;
			}
		}
	}
#ifdef __WORLDSERVER	
	// 길드대전맵에서 선수만 사용
	if( dwWorldID == WI_WORLD_GUILDWAR )
	{
		// 관중석에서는 스킬을 사용할수 없습니다
		if( pUseMover->m_nGuildCombatState == 0 )
		{
			if( pUseMover->IsPlayer() )
			{
				((FLWSUser*) pUseMover)->AddText( prj.GetText( TID_GAME_GUILDCOMBAT_STANDS_NOTUSESKILL ) );
			}

			return false;
		}
	}
#endif //__WORLDSERVER

	// 일대일 길드대전 시 블링크풀 사용 금지 // 서버에서는 왜 체크 안하지?
	if( WI_WORLD_GUILDWAR1TO1_0 <= dwWorldID && dwWorldID <= WI_WORLD_GUILDWAR1TO1_L )
	{
		if( pSkillProp->dwID == SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ) )
		{
#ifdef __WORLDSERVER
			((FLWSUser*) pUseMover)->AddText( prj.GetText( TID_GAME_NEVERKILLSTOP ) );
#else
			g_WndMng.PutString( prj.GetText( TID_GAME_NEVERKILLSTOP ), NULL, prj.GetTextColor( TID_GAME_NEVERKILLSTOP ) );
#endif // __WORLDSERVER
			
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 타겟 확인
	CMover* pTarget = prj.GetMover( idFocusObj );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj( pTarget ) )
	{
		return false;
	}

	if( pSkillProp->dwID == SKILL_INDEX( 44, SI_ASS_HEAL_HEALING ) )	// 힐링 상대가 죽었으면 취소
	{
		if( pTarget->IsDie() )
		{
			return false;
		}
	}

	if( pSkillProp->dwID == SKILL_INDEX( 412, SI_WIN_YOYO_BACKSTEP ) )
	{
		if( pTarget->GetWorld() == pWorld )
		{
			D3DXVECTOR3 vStart = pUseMover->GetPos();
			vStart.y += 1.0f;
			D3DXVECTOR3 vEnd   = pTarget->GetPos();
			if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) == TRUE )
			{
#ifdef __CLIENT
				g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
#endif // __CLIENT
				return false;
			}
		}
		else
		{
			return false;
		}
	}

#ifdef __WORLDSERVER
	if( pSkillProp->IsDebuff() == TRUE )	//PK 금지구역에서 악행 종류의 스킬은 PVP 상대에게만 사용하도록..
	{
		int nAttackerPK = 0, nDefenderPK = 0;
		if( pUseMover->IsPlayer() && pTarget->IsPlayer() && (pUseMover != pTarget) ) //(this!=pTarget) - 타겟을 안찍는 스킬은 타겟이 시전자이다.
		{
			DWORD dwRegionAttr	= pUseMover->GetPKPVPRegionAttr();
			nAttackerPK = pUseMover->IsPKPVPInspectionBase( dwRegionAttr, FALSE );
			nDefenderPK = pTarget->IsPKPVPInspectionBase( dwRegionAttr, FALSE );
			if( (nAttackerPK == 1 || nDefenderPK == 1) &&
				!(	pUseMover->IsPVPTarget( pTarget )
				|| pUseMover->IsWarTarget( pTarget )
				|| pUseMover->IsGuildCombatTarget( pTarget ) 
				|| pUseMover->IsArenaTarget( pTarget )
				|| pUseMover->IsEventArenaTarget( pTarget )
				) )
			{
				((FLWSUser*)pUseMover)->AddDefinedText( TID_GMAE_PK_NOT_AREA, "" );
				return false;
			}
		}
	}

	// 흠... 
	if( ( pSkillProp->dwID == SKILL_INDEX( 114, SI_ASS_CHEER_QUICKSTEP )	&& pTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 317, SI_GEN_EVE_QUICKSTEP ) ) ) ||
		( pSkillProp->dwID == SKILL_INDEX( 20, SI_ASS_CHEER_HASTE )			&& pTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 318, SI_GEN_EVE_HASTE ) ) ) ||
		( pSkillProp->dwID == SKILL_INDEX( 49, SI_ASS_CHEER_HEAPUP )		&& pTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 319, SI_GEN_EVE_HEAPUP ) ) ) ||
		( pSkillProp->dwID == SKILL_INDEX( 116, SI_ASS_CHEER_ACCURACY )		&& pTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 320, SI_GEN_EVE_ACCURACY ) ) ) )
	{
		if( pTarget->IsPlayer() )
		{
			((FLWSUser*)pTarget)->AddDefinedText( TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName );
		}

		return false;
	}
	// 강탈, 혼신의 일격스킬이면 다이일루젼상태인지 검사한다.
	if( pSkillProp->dwID == SKILL_INDEX( 203, SI_ACR_YOYO_SNITCH ) || pSkillProp->dwID == SKILL_INDEX( 214, SI_JST_YOYO_VATALSTAB ) )
	{
		// 다크일루젼 상태가 아니면 사용못함
		if( !pUseMover->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) )
		{
			if( pUseMover->IsPlayer() )
			{
				((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_REQ_DARK, "" );		// 다크일루젼상태에서 가능합니다.
			}

			return false;
		}
	}

	// 쿨타임 검사.
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1
		|| ( _GetContentState( CT_NEWUI_19 ) != CS_VER1 && sutType == SUT_NORMAL ) )
	{
		int nSkillIdx = pUseMover->GetSkillIdx( dwSkillID );		// 스킬리스트 인덱스를 찾음.
		if( nSkillIdx >= 0 )
		{
			if( pUseMover->GetReuseDelay( nSkillIdx ) )		// 쓰려고 하는 스킬 쿨타임이 남아있으면 실행안됨.
			{
				if( pUseMover->IsPlayer() )
				{
					((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_SKILLWAITTIME, "" );	// 아직 사용할 수 없습니다.
				}
				return false;
			}
		}
	}

	if( pUseMover->CheckAttackDistance( pTarget, pSkillProp->dwAttackRange ) == false )
	{
		return false;
	}
#endif //__WORLDSERVER

	return true;
}

bool	FLSkillSystem::IsRemainSkillDelayTime( CMover* pUseMover, const DWORD dwSkillID ) const
{
	// 쿨타임 검사.
#ifdef __CLIENT
	/*
	클라에서는 다른유저가 사용한 스킬에 대해서도 해당 로직을 그대로 탄다.
	다른 유저의 스킬 정보는 가지고 있지 않으므로 스킬인덱스를 가져오지 못한다.
	고로 체크 불가능.. 다른 유저일땐 그냥 쿨타임 없다고 본다..
	*/
	if( pUseMover->IsActiveMover() == FALSE )
	{
		return false;
	}
#endif // __CLIENT

	const int nSkillIdx = pUseMover->GetSkillIdx( dwSkillID );		// 스킬리스트 인덱스를 찾음.
	if( nSkillIdx >= 0 && nSkillIdx <= MAX_SKILL_JOB )
	{
		if( pUseMover->GetReuseDelay( nSkillIdx ) <= 0 )		// 쿨타임이 없거나 지났으므로 사용 가능..
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		/************************************************************************/
		/*
		스킬 시전자의 스킬 테이블에 있는 스킬이 아닌 하드코딩된 스킬들이 남아있다.( ex. 아크로뱃의 역습 )
		해당 스킬들은 유저의 잡스킬 인덱스를 뽑아올 수 없으므로 그냥 딜레이가 없다고 판단한다.
		*/
		/************************************************************************/

		return false;
	}
}

// dwSkill,nLevel만 있으면 어디서든지 사용가능한 버전	
// 몬스터까지 사용가능
DWORD	FLSkillSystem::DoUseSkill( CMover* pUseMover, const DWORD dwSkill, const int nLevel, const OBJID idFocusObj, const SKILLUSETYPE sutType, const BOOL bControl, const int nCastingTime )
{
	if( CanUseSkill( pUseMover, dwSkill, nLevel, idFocusObj, sutType ) == false )
	{
		return FSC_SKILLUSE_FAIL;
	}

	if( IsRemainSkillDelayTime( pUseMover, dwSkill ) == true )
	{
		return FSC_SKILLUSE_SKIP;
	}

	SkillProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	if( pUseMover->GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoUseSkill" ) == FALSE )
	{
		return FSC_SKILLUSE_FAIL;
	}

	CMover* pTarget = prj.GetMover( idFocusObj );	// 타겟의 포인터를 얻어냄.
	if( IsValidObj( pTarget ) == FALSE )
	{
		return FSC_SKILLUSE_FAIL;
	}

	CWorld* pWorld	= pUseMover->GetWorld();
	if( pWorld == NULL )
	{
		return FSC_SKILLUSE_FAIL;
	}

	if( pTarget != pUseMover )	// 타인에게 쓰는 종류일때만 아래검사를 함.
	{
#ifdef __WORLDSERVER
		// 타인에게 쓰는 스킬은 즉시시전이 없나?
		// (즉시사용 : 타겟으로 지정되어 있는 대상에게 사용한다. 타겟팅과 상관없이 자기자신에게 쓰는 방식.)
		// 설명이 두개다 어느게 맞는거냐.
		// 자신에게 사용하는 스킬 걸러내기.
		if( pSkillProp->dwUseChance == WUI_NOW )	// 康 - 리소스 해킹
			return FSC_SKILLUSE_FAIL;
		if( pSkillProp->dwExeTarget == EXT_SELFCHGPARAMET )
			return FSC_SKILLUSE_FAIL;

		if( pTarget->IsNPC() && pTarget->IsPeaceful() )
		{
			return FSC_SKILLUSE_FAIL;	// NPC지만 친한?NPC에겐 스킬 사용 금지
		}

		if( !g_eLocal.GetState( EVE_STEAL ) )
		{
			if( pSkillProp->IsDebuff() == TRUE )	// 나쁜마법을 쓰려할때
			{
				if( pUseMover->IsSteal( pTarget ) )		// pTarget를 스틸 하려하면
				{
					return FSC_SKILLUSE_FAIL;
				}
			}
		}
		if( pWorld->IsArena() == FALSE )		// 아레나가 아니면,
		{
			if( pSkillProp->IsDebuff() == FALSE )	// 좋은 마법을
			{
				HITTYPE hy = HITTYPE_FAIL;
				if( (hy = pUseMover->GetHitType2( pTarget, TRUE, TRUE )) != HITTYPE_FAIL )  	// 적대적인 대상에게 쓰려할때
				{
					if( pUseMover->IsPlayer() )
					{
						((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_NEVERKILLSTOP, "" );		// 스킬을 사용할 수 없습니다.
					}

					return FSC_SKILLUSE_FAIL;		// 스킬 취소
				}
			}
		}
		
#endif
		pUseMover->SetAngle( GetDegree( pTarget->GetPos(), pUseMover->GetPos() ) );		// 목표쪽으로 몸을 돌림.
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 아이템 검사
#ifdef __WORLDSERVER

	// 스킬을 사용할때 적합한 무기를 들고 있는가 검사
	PT_ITEM_SPEC pItemProp = NULL;
	DWORD	dwItemKind3 = 0;

	if( pSkillProp->dwNeedItemKind3 == IK3_SHIELD )	// 필요아아이템이 방패일때
	{
		FLItemElem *pItemElem = pUseMover->GetEquipItem( PARTS_SHIELD );
		if( pItemElem )
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		}
		else
		{
			dwItemKind3 = NULL_ID;
		}
	}
	else if( pSkillProp->dwNeedItemKind3 == IK3_ZEMBARUNA || pSkillProp->dwNeedItemKind3 == IK3_MAGICBARUNA || pSkillProp->dwNeedItemKind3 == IK3_SHILDBARUNA )
	{
		FLItemElem *pItemElem = pUseMover->GetEquipItem( PARTS_SHIELD );
		if( pItemElem )
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		}
		else
		{
			dwItemKind3 = NULL_ID;
		}
	}
	else	// 필요아이템이 방패가 아닐때는 액티브 아이템을...
	{
		pItemProp = pUseMover->GetActiveHandItemProp();
		dwItemKind3 = pItemProp->dwItemKind3;
	}
	
	switch( pSkillProp->dwNeedItemKind3 )	// 스킬이 필요로하는 무기.
	{
	case IK3_ENCHANTWEAPON:
		{
			if( pUseMover->IsPlayer() )
			{
				bool bSuccess = true;

				FLItemElem *pItemElemR = pUseMover->GetWeaponItem();
				FLItemElem *pItemElemL = pUseMover->GetLWeaponItem();

				if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
				{
					// 양손다 속성이 없을땐
					if( (pItemElemR && pItemElemR->m_byItemResist == SAI79::NO_PROP) &&
						(pItemElemL && pItemElemL->m_byItemResist == SAI79::NO_PROP) )
						bSuccess = false;		// 사용금지
				} 
				else if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
				{
					if( pItemElemR && pItemElemR->m_byItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
						bSuccess = false;		// 사용금지
				} 
				else if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
				{
					bSuccess = false;		// 이때는 무조건 사용금지
				}

				if( bSuccess == false )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );	// 속성무기가 없어 사용못함.
					return FSC_SKILLUSE_FAIL;
				}
			}
		}
		break;
	case IK3_MAGICBOTH:
		{
			if( dwItemKind3 != IK3_WAND && dwItemKind3 != IK3_STAFF )	// 손에든 무기가 완드도 스태프도 아니면
			{
				if( pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_WRONGITEM, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
		break;
	case IK3_YOBO:
		{
			if( dwItemKind3 != IK3_YOYO && dwItemKind3 != IK3_BOW )	
			{
				if( pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_WRONGITEM, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
		break;
	default:
		{
			if( pSkillProp->dwNeedItemKind3 != NULL_ID && pSkillProp->dwNeedItemKind3 != dwItemKind3 )	// 그외는 IK3를 비교해서 틀리면 에러.
			{
				if( pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_WRONGITEM, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
		break;
	}

	// sun 8차 버프창 관련 적용, 타겟의 버프중 이미 걸린 버프가 레벨이 높을 경우 걸지 못하게 함.
	if( pSkillProp->dwSkillType == BUFF_SKILL )
	{
		IBuff* pBuff	= pTarget->m_buffs.GetBuff( BUFF_SKILL, (WORD)( pSkillProp->dwID ) );
		if( pBuff )
		{
			if( pBuff->GetLevel() > (DWORD)( nLevel ) )
			{
				if( pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_DONOTUSEBUFF, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
	}

	// 투핸드용이냐 원핸드용이냐의 조건검사.
	if( pSkillProp->dwNeedItemHanded != NULL_ID )
	{
		// pItemProp가 NULL이면?
		if( pItemProp )
		{
			if( pUseMover->IsPlayer() )
			{
				if( pSkillProp->dwNeedItemHanded == HD_DUAL )		// 듀얼용 스킬인데
				{
					if( pUseMover->IsDualWeapon() == FALSE )			// 듀얼로 안차고 있다.
					{
						((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_WRONGITEM, "" );
						return FSC_SKILLUSE_FAIL;
					}
				}
				else if( pItemProp->dwHanded != pSkillProp->dwNeedItemHanded )	// 스킬과 쥐고 있는 아이템의 Handed 종류가 다르면 에러
				{
					((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_WRONGITEM, "" );
					return FSC_SKILLUSE_FAIL;
				}
			}
		}
	}

	// 필요총알종류가 지정되어 있다면
	if( pUseMover->IsBullet( pSkillProp ) == FALSE )
	{
		return FSC_SKILLUSE_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 스킬 검사?
	if( pUseMover->IsPlayer() )
	{
//sun: 10차 전승시스템	Neuz, World, Trans

		//////////////////////////////////////////////////////////////////////////
		// mirchang_100908 극단 스킬일 경우에는 극단인지 먼저 체크...
		if( pSkillProp->dwExeTarget == EXT_TROUPE )
		{
			CParty* pParty = g_PartyMng.GetParty( pUseMover->m_idparty );
			if( pParty == NULL )
			{
				((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_NEVERKILLSTOP );
				return FSC_SKILLUSE_FAIL;
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	if( pSkillProp->dwID == SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ) )	// 앗살라의 경우 예외처리.
	{
		if( pUseMover->IsPlayer() && pUseMover->GetFatiguePoint() < pUseMover->GetReqFp( pAddSkillProp->nReqFp )  )	// chipi_090917 필요 FP 검사
		{
			((FLWSUser*)pUseMover)->AddDefinedText( TID_GAME_REQFP ); // chipi_00917
			return FSC_SKILLUSE_FAIL;
		}
	}

	
#endif // __WORLDSERVER
	//////////////////////////////////////////////////////////////////////////

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;

	//////////////////////////////////////////////////////////////////////////
	//--- 마나가 충분한지 검사 하고 메시지 타입(?) 설정하고 모션 설정
	if( pSkillProp->dwSkillType == KT_SKILL )
	{
#ifdef __WORLDSERVER
			// 전승시스템	Neuz, World, Trans
			if( pUseMover->GetReqFp( pAddSkillProp->nReqFp ) == 9999999 )
			{
				if( pUseMover->GetManaPoint() != pUseMover->GetMaxManaPoint() )
				{
					if( TRUE == pUseMover->IsPlayer() )
					{
						( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQMP, "" );
					}
					return FSC_SKILLUSE_FAIL;
				}
				if( pUseMover->GetFatiguePoint() != pUseMover->GetMaxFatiguePoint() )
				{
					if( TRUE == pUseMover->IsPlayer() )
					{
						( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQFP, "" );
					}
					return FSC_SKILLUSE_FAIL;
				}
			}
			else
			{
				if( pUseMover->GetFatiguePoint() - pUseMover->GetReqFp( pAddSkillProp->nReqFp ) < 0 )	// FP가 모자르므로 취소
				{
					if( TRUE == pUseMover->IsPlayer() )
					{
						( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQFP, "" );
					}
					return FSC_SKILLUSE_FAIL;
				}

				// KT_SKILL중에 MP가 소모되는 값이있으면 FP와 같이 소모시켜준다.
				if( (int)pAddSkillProp->nReqMp > 0 )
				{
					if( pUseMover->GetManaPoint() - pUseMover->GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
					{
						if( TRUE == pUseMover->IsPlayer() )
						{
							( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQMP, "" );
						}
						return FSC_SKILLUSE_FAIL;
					}
				}
			}

#endif
		dwMsg = OBJMSG_MELEESKILL;	// 클라에서도 실행해야함
	} 
	else if( pSkillProp->dwSkillType == KT_MAGIC )
	{
#ifdef __WORLDSERVER
		if( pUseMover->GetAdjParam( DST_CHRSTATE ) & CHS_SILENT )		// 침묵상태에선 매직스킬 쓸 수 없음.
		{
			((FLWSUser*) pUseMover)->AddText( prj.GetText( TID_GAME_NEVERKILLSTOP ) );	
			return FSC_SKILLUSE_FAIL;
		}

		//sun: 10차 전승시스템	Neuz, World, Trans
		if( pUseMover->GetReqMp( pAddSkillProp->nReqMp ) == 9999999 )
		{
			if( pUseMover->GetManaPoint() != pUseMover->GetMaxManaPoint() )
			{
				if( TRUE == pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQMP, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
			if( pUseMover->GetFatiguePoint() != pUseMover->GetMaxFatiguePoint() )
			{
				if( TRUE == pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQFP, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
		else
		{
			if( pUseMover->GetManaPoint() - pUseMover->GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
			{
				if( TRUE == pUseMover->IsPlayer() )
				{
					( (FLWSUser*)pUseMover )->AddDefinedText( TID_GAME_REQMP, "" );
				}
				return FSC_SKILLUSE_FAIL;
			}
		}
#endif // worldserver

		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬. - 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		{
			if( pUseMover->IsPlayer() )
			{
				dwMsg = OBJMSG_MAGICCASTING;
			}
		} 
		else if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬. - 캐스팅 동작없이 시전.
		{
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} 
		else
		{									// 일반적인 마법 스킬 사용 - 캐스팅 동작 있음.
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s dwSkillType = %d" ), pUseMover->GetName(), pSkillProp->dwSkillType );	// 프로퍼티값이 이상함.
	}

	//////////////////////////////////////////////////////////////////////////
	// ????????????
	if( sutType != SUT_QUEUEING && nCastingTime == 0 && pSkillProp->dwID != SKILL_INDEX( 223, SI_GEN_ATK_COUNTER ) )
		nMotion += 2;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 스킬 적용 준비
	pUseMover->SendActMsg( OBJMSG_STOP );
	pUseMover->ClearDest();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//--- 동작 시작 -----------------------------------------------------------------
	// 전승시스템	Neuz, World, Trans
	int nRet = pUseMover->m_pActMover->SendActMsg( dwMsg, nMotion, idFocusObj, nCastingTime, dwSkill );
	if( nRet == 1 )	// 스킬 동작 시작.
	{
		// 명령수행중 필요한 파라메터세팅.
		pUseMover->SetActParam( OBJACT_USESKILL, dwSkill, idFocusObj, 0, nLevel, bControl );

#ifdef __WORLDSERVER
		if( pSkillProp->dwSkillType == KT_SKILL )
		{
			// 근접스킬은 사용하는 즉시 fp감소.
			if( (int)pAddSkillProp->nReqFp > 0 )
			{
				pUseMover->IncFatiguePoint( -(pUseMover->GetReqFp( pAddSkillProp->nReqFp )) );	// FP감소
			}

			int nReqMp	= pUseMover->GetReqMp( pAddSkillProp->nReqMp );
			if( nReqMp > 0 )
			{
				pUseMover->IncManaPoint( -nReqMp );
			}
		}
		// 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwSkillType == KT_MAGIC )
		{
			if( pUseMover->GetReqMp( pAddSkillProp->nReqMp ) == 9999999 )
			{
				pUseMover->IncFatiguePoint( -(pUseMover->GetFatiguePoint()) );	// FP감소
				pUseMover->IncManaPoint( -(pUseMover->GetManaPoint()) );
			}
		}

		if( pSkillProp->dwNeedItemBulletKind2 != NULL_ID )	// 총알(?)을 필요로 하는거면
		{
			FLItemElem *pItemElem = pUseMover->GetEquipItem( PARTS_BULLET );
			if( pItemElem )
			{
				pUseMover->ArrowDown( 1 );
			}
		}

		if( pSkillProp->IsDebuff() == TRUE )		// 나쁜마법으로만 몬스터나 플레이어를 찜할수있다.
		{
			pTarget->SetJJim( pUseMover );			// pTarget를 찜했다는걸 표시.
			pUseMover->m_nAtkCnt = 1;						// 빗자루 딜레이를 위한 공격 카운터 시작.
		}

		// DST param이 적용되는 스킬은 여기서 적용함.
		g_xWSUserManager->AddUseSkill( pUseMover, dwSkill, nLevel, idFocusObj, sutType, nCastingTime );
#endif	// __WORLDSERVER
	} 
	else if( nRet == -2 )	// 0:큐에 쌓는다 -1:모름  -2:완전실패하여 취소시킴   -3:모름
	{
		return FSC_SKILLUSE_FAIL;
	}
	
	pUseMover->m_dwFlag |= MVRF_SKILL;		// 스킬사용 명령이 시작됐다.


#if defined __WORLDSERVER && defined __AGGRO16
	if( pSkillProp->dwID == SKILL_INDEX( 403, SI_LOD_SUP_ANGER ) )
	{
		if( pTarget->GetAggroMng() != NULL )
			pTarget->GetAggroMng()->SumLodSupAnger( *pTarget, *pUseMover );
	}

	if( pSkillProp->dwID == SKILL_INDEX( 400, SI_LOD_SUP_PULLING ) )
	{
		if( pTarget->GetAggroMng() != NULL )
			pTarget->GetAggroMng()->SumLodSupPulling( *pTarget, *pUseMover );
	}
#endif //__AGGRO16


#ifdef __WORLDSERVER
	if( g_eLocal.GetState( EVE_PK ) )
	{
		if( !pUseMover->IsChaotic() && ( pTarget->IsPKPink() || ( pTarget->IsChaotic() && pSkillProp->IsDebuff() == FALSE ) ) )
		{
			pUseMover->SetPKPink( GetTickCount() + SEC(prj.m_PKSetting.nReadyAttack) );
			g_xWSUserManager->AddPKPink( pUseMover, 1 );	
		}
	}
#endif // __WORLDSERVER


#ifdef __CLIENT
	if( pUseMover->IsActiveMover() )		// g_pPlayer일경우
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar->m_nExecute == 1 )		// 스킬바 사용 대기중상태였으면
			pTaskBar->m_nExecute = 2;		// 실제 스킬바 사용중으로 전환.
	}

	{
		DWORD dwLinkSfxDouble = NULL_ID;
		switch( dwSkill )
		{	// 여기다 XI_ 추가하면 CreateSfx의 CSfxPartsLink()에도 추가시켜야 함/
		case SKILL_INDEX( 140, SI_BLD_DOUBLEAX_SPRINGATTACK ):	dwLinkSfxDouble = XI_INDEX( 333, XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01 );	break;
		case SKILL_INDEX( 142, SI_BLD_DOUBLE_SONICBLADE ):		dwLinkSfxDouble = XI_INDEX( 336, XI_SKILL_BLD_DOUBLE_SONICBLADE01 );	break;
		case SKILL_INDEX( 133, SI_KNT_TWOSW_CHARGE ):			dwLinkSfxDouble = XI_INDEX( 315, XI_SKILL_KNT_TWOSW_CHARGE01 );	break;
		case SKILL_INDEX( 139, SI_BLD_DOUBLESW_BLADEDANCE ):	dwLinkSfxDouble = XI_INDEX( 334, XI_SKILL_BLD_DOUBLESW_BLADEDANCE01 );      break;

		// 제스터 양손 이펙트 : sfx프레임이 끝이면 소멸함
		case SKILL_INDEX( 214, SI_JST_YOYO_VATALSTAB ):	dwLinkSfxDouble = XI_INDEX( 527, XI_SKILL_JST_YOYO_VATALSTAB01 );	break;
		case SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ):	dwLinkSfxDouble = XI_INDEX( 525, XI_SKILL_JST_YOYO_HITOFPENYA01 );	break;

		case SKILL_INDEX( 207, SI_JST_YOYO_CRITICALSWING ):	dwLinkSfxDouble = XI_INDEX( 520, XI_SKILL_JST_SUP_CRITICALSWING01 );	break;
		case SKILL_INDEX( 197, SI_ACR_SUP_SLOWSTEP ):	dwLinkSfxDouble = XI_INDEX( 510, XI_SKILL_ACR_YOYO_SLOWSTEP01 );	break;
		case SKILL_INDEX( 208, SI_JST_SUP_POISON ):		dwLinkSfxDouble = XI_INDEX( 521, XI_SKILL_JST_SUP_POISON01 );	break;
		case SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ):	dwLinkSfxDouble = XI_INDEX( 522, XI_SKILL_JST_SUP_BLEEDING01 );	break;
		case SKILL_INDEX( 211, SI_JST_YOYO_BACKSTAB ):	dwLinkSfxDouble = XI_INDEX( 524, XI_SKILL_JST_YOYO_BACKSTAB01 );	break;
		case SKILL_INDEX( 210, SI_JST_SUP_ABSORB ):	dwLinkSfxDouble = XI_INDEX( 523, XI_SKILL_JST_SUP_ABSORB01 );	break;
			break;
		}
		if( dwLinkSfxDouble != NULL_ID )
		{
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, pUseMover->GetPos(), pUseMover->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
			if( pSfx )
			{
				pSfx->SetPartLink( 0 );		// 오른손에 링크.
				pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, pUseMover->GetPos(), pUseMover->GetId() );  // 링크이펙 하나더 생성.
				if( pSfx )
					pSfx->SetPartLink( 1 );		// 손에 링크.
			}
		}
	}

	// 왼손 링크 이펙
	DWORD dwLinkSfxLeft = NULL_ID;
	switch( dwSkill )
	{	// 아크로벳 스킬이면 활 시전 이펙트 왼손에 링크한다
	case SKILL_INDEX( 216, SI_RAG_BOW_ICEARROW ):	dwLinkSfxLeft = XI_INDEX( 529, XI_SKILL_RAG_BOW_ICEARROW01 );	break;
	case SKILL_INDEX( 194, SI_ACR_BOW_JUNKBOW ):	dwLinkSfxLeft = XI_INDEX( 507, XI_SKILL_ACR_BOW_JUNKBOW01 );	break;
	case SKILL_INDEX( 196, SI_ACR_BOW_AIMEDSHOT ):	dwLinkSfxLeft = XI_INDEX( 509, XI_SKILL_ACR_BOW_AIMEDSHOT01 );	break;
	case SKILL_INDEX( 198, SI_ACR_BOW_SILENTSHOT ):	dwLinkSfxLeft = XI_INDEX( 511, XI_SKILL_ACR_BOW_SILENTSHOT01 );	break;
	case SKILL_INDEX( 200, SI_ACR_BOW_ARROWRAIN ):	dwLinkSfxLeft = XI_INDEX( 513, XI_SKILL_ACR_BOW_ARROWRAIN01 );      break;
	case SKILL_INDEX( 202, SI_ACR_BOW_AUTOSHOT ):	dwLinkSfxLeft = XI_INDEX( 515, XI_SKILL_ACR_BOW_AUTOSHOT01 );      break;
		
	case SKILL_INDEX( 215, SI_RAG_SUP_FASTATTACK ):	  dwLinkSfxLeft = XI_INDEX( 528, XI_SKILL_RAG_SUP_FASTATTACK01 );break;
	case SKILL_INDEX( 217, SI_RAG_BOW_FLAMEARROW ):	  dwLinkSfxLeft = XI_INDEX( 530, XI_SKILL_RAG_BOW_FLAMEARROW01 );break;
	case SKILL_INDEX( 218, SI_RAG_BOW_PIRCINGARROW ): dwLinkSfxLeft = XI_INDEX( 531, XI_SKILL_RAG_BOW_PIRCINGARROW01 );break;
	case SKILL_INDEX( 219, SI_RAG_BOW_POISONARROW ):  dwLinkSfxLeft = XI_INDEX( 532, XI_SKILL_RAG_BOW_POISONARROW01 );break;
	case SKILL_INDEX( 220, SI_RAG_BOW_SILENTARROW ):  dwLinkSfxLeft = XI_INDEX( 533, XI_SKILL_RAG_BOW_SILENTARROW01 );break;
	case SKILL_INDEX( 222, SI_RAG_BOW_TRIPLESHOT ):   dwLinkSfxLeft = XI_INDEX( 535, XI_SKILL_RAG_BOW_TRIPLESHOT01 );break;
		
		break;
	}
	
	if( dwLinkSfxLeft != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxLeft, pUseMover->GetPos(), pUseMover->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 1 );		// 왼손에 링크.
	}

	// 오른손 링크 이펙
	DWORD dwLinkSfxRight = NULL_ID;
	switch( dwSkill )
	{
	case SKILL_INDEX( 4, SI_MER_ONE_SPLMASH ): dwLinkSfxRight = XI_INDEX( 1731, XI_SKILL_MER_ONE_SUPPORT04 ); break;
	case SKILL_INDEX( 12, SI_MER_ONE_GUILOTINE ): dwLinkSfxRight = XI_INDEX( 1729, XI_SKILL_MER_ONE_SUPPORT02 ); break;
	case SKILL_INDEX( 14, SI_MER_ONE_REFLEXHIT ): dwLinkSfxRight = XI_INDEX( 1728, XI_SKILL_MER_ONE_SUPPORT01 ); break;
	case SKILL_INDEX( 11, SI_MER_ONE_SPECIALHIT ): dwLinkSfxRight = XI_INDEX( 1730, XI_SKILL_MER_ONE_SUPPORT03 ); break;
	case SKILL_INDEX( 117, SI_ASS_KNU_POWERFIST ):	dwLinkSfxRight = XI_INDEX( 1734, XI_SKILL_ASS_KNU_SUPPORT03 );	break;
	case SKILL_INDEX( 104, SI_ASS_KNU_TAMPINGHOLE ):	dwLinkSfxRight = XI_INDEX( 1733, XI_SKILL_ASS_KNU_SUPPORT02 );	break;
	case SKILL_INDEX( 105, SI_ASS_KNU_BURSTCRACK ):	dwLinkSfxRight = XI_INDEX( 1732, XI_SKILL_ASS_KNU_SUPPORT01 );	break;
	case SKILL_INDEX( 223, SI_GEN_ATK_COUNTER ):	dwLinkSfxRight = XI_INDEX( 524, XI_SKILL_JST_YOYO_BACKSTAB01 );	break;
		break;
	}
	
	if( dwLinkSfxRight != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxRight, pUseMover->GetPos(), pUseMover->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 0 );		// 오른손에 링크.
	}

	// 마법 혹은 전투스킬의 발동시작 이펙트발생
	// 자기id GetId()
	// 타겟id idFocusObj
	if( pSkillProp->dwSfxObj != NULL_ID )
	{
		CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, pUseMover->GetPos(), pUseMover->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
	}	
	// 공격스킬과 타겟이 몬스터 이면 화면에 출력을 안함
	if( pSkillProp->dwExeTarget != EXT_MELEEATK && 
		pSkillProp->dwExeTarget != EXT_MAGICATK && 
		pSkillProp->dwExeTarget != EXT_AROUNDATK &&  
		pSkillProp->dwExeTarget != EXT_MAGICATKSHOT && 
		pTarget->IsPlayer() )
	{
		CString str;
		if( pUseMover == g_pPlayer ) // 내가 쓴것
		{
			str.Format( prj.GetText( TID_GAME_SKILL_SELF ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_SELF ) );	
		}
		else if( pTarget == g_pPlayer && ( pUseMover != pTarget || pSkillProp->dwID == SKILL_INDEX( 51, SI_ASS_CHEER_CIRCLEHEALING ) ) ) // 내가 쓴것이 아니라면
		{
			str.Format( prj.GetText( TID_GAME_SKILL_MATE ), pUseMover->GetName(), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_MATE ) );	
		}
	}

	if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
	{
		if( g_pPlayer )
		{
			if( g_Option.m_bVoice )
			{
				if( pUseMover->IsActiveMover() )
				{
					int nindex = 0;
					
					if( NULL == pSkillProp )
						return TRUE;

					if( pSkillProp->dwID != NULL_ID )
					{
						if( pUseMover->GetSex() == 0 )
							nindex = pSkillProp->dwID+3000;
						else
							nindex = pSkillProp->dwID+3500;

						PLAYSND( nindex );
					}
				}
			}
		}
	}
#endif	// __CLIENT

	return FSC_SKILLUSE_SUCCESS;
}

// nLevel의 dwSkill이 pTarget을 향하여 발동됨.  어태커는 this
// bIgnoreProp이 TRUE이면 내부에서 확률계산을 무시하고 100% 실행되게 한다.
void	FLSkillSystem::DoActiveSkill( CCtrl* pUseCtrl, DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProp, BOOL bControl )
{
	SkillProp *pSkillProp;
	AddSkillProp *pAddSkillProp;
	if( pUseCtrl->GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoActiveSkill" ) == FALSE )
	{
		return;
	}

	if( pSkillProp == NULL || pAddSkillProp == NULL )
	{
		if( pUseCtrl->GetType() == OT_MOVER && ( static_cast<CMover*>(pUseCtrl) )->IsPlayer() == TRUE )
		{
			PT_ITEM_SPEC pItemProp = ( static_cast<CMover*>(pUseCtrl) )->GetActiveHandItemProp();
			if( pItemProp != NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "GetActiveHandItemProp : ItemId:[%d]" ), pItemProp->dwID );
			}

			FLERROR_LOG( PROGRAM_NAME, _T( "ActiveSkill Invalid. PlayerId:[%07d], SkillId:[%d]" ), ( static_cast<CMover*>(pUseCtrl) )->m_idPlayer, dwSkill );
		}

		return;
	}

	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT )		// 쏘는 형태면 sfx만 발생
	{
		ShootSkill( pUseCtrl, pTarget, pSkillProp, pAddSkillProp );
	}
	else
	{
		DoApplySkill( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, bIgnoreProp, 0, FALSE, bControl );

#ifdef __WORLDSERVER		
		g_xWSUserManager->AddDoApplySkill( pUseCtrl, pTarget->GetId(), dwSkill, nLevel );
#endif
	}
}

//
// 스킬 모션이 작동하다 타격 프레임이 되면 이 함수를 호출함 
// 근접전 스킬.    
//
BOOL	FLSkillSystem::OnMeleeSkill( CMover* pUseMover, int /*nType*/, int nCount )
{
	if( pUseMover->GetAct() != OBJACT_USESKILL )	
		return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.

	int		nSkill = pUseMover->GetActParam( 0 );
	OBJID	idTarget = pUseMover->GetActParam( 1 );
	DWORD	dwLevel	= pUseMover->GetActParam( 3 );
	BOOL	bControl = pUseMover->GetActParam( 4 );

	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	// 스킬프로퍼티 가져옴.
	if( pUseMover->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "OnMeleeSkill" ) == FALSE )
	{
		return FALSE;
	}

	// .타겟에게 스킬효과(데미지같은..)를 준다.
	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )	
		return FALSE;

#if defined(__CLIENT)
	CModelObject* pModel =(CModelObject*)pUseMover->GetModel();
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;

		D3DXVECTOR3 vDir;
		if( pFocusObj == pUseMover )
			vDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		else
			vDir = pUseMover->GetPos() - pFocusObj->GetPos();

		D3DXVec3Normalize( &pModel->m_SparkInfo.m_v3SparkDir, &vDir );
	}
#endif //defined(__CLIENT) 

	int nDamage = 1;

	// 쏘는 형태만 sfx발생.
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT	)
	{
#ifdef __WORLDSERVER
		int nMaxDmgCnt	= 1;
		if( pSkillProp->tmContinuousPain != NULL_ID )
			nMaxDmgCnt	= ( pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime ) + 1;

		float fDmgPower = 0;
		if( pSkillProp->dwDmgShift != NULL_ID )
			fDmgPower	= (int)pSkillProp->dwDmgShift * 0.2f;

		pUseMover->OnAttackSFX( pFocusObj->GetId(),
								pAddSkillProp->dwSkillLvl,
								pSkillProp->dwID,
								0,
								pUseMover->GetAngle(),
								fDmgPower,
								( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ),
								bControl );

#endif	// __WORLDSERVER
		ShootSkill( pUseMover, pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	}
	else
	{
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
		nDamage = DoApplySkill( pUseMover, pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, bControl );

	}
	FLTRACE_LOG( PROGRAM_NAME, _T( "OnMeleeSkill%s, " ), pSkillProp->szName );

	if( nDamage && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
	{
		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
#ifdef __CLIENT
		if( pUseMover->IsActiveMover() )
#endif
		{
			//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
			if( pAddSkillProp->dwCooldown != NULL_ID )	// 쿨타임이 있는 스킬의 경우
			{
				pUseMover->SetCoolTime( pAddSkillProp, "OnMeleeSkill" );
			}

		}
	}

	return TRUE;
}


//
//	마법 스킬의 타점때 호출.
//  nCount 워터볼의 경우 여러번 호출이 되기때문에 nCount==0 의 경우만 경험치가 올라간다.
//
BOOL	FLSkillSystem::OnMagicSkill( CMover* pUseMover, int /*nType*/, int nCount )
{
#ifdef __WORLDSERVER
	UNREFERENCED_PARAMETER( nCount );
#endif	//__WORLDSERVER


	if( pUseMover->GetAct() != OBJACT_USESKILL )	
		return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.

	int		nSkill = pUseMover->GetActParam( 0 );
	OBJID	idTarget = pUseMover->GetActParam( 1 );
	DWORD	dwLevel = pUseMover->GetActParam( 3 );
	BOOL	bControl = pUseMover->GetActParam( 4 );

	SkillProp*	pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
//	LPSKILL		pSkill	= NULL;

	BOOL bRet = pUseMover->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "FLSkillSystem::OnMagicSkill" );
	if( bRet == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "스킬프롭읽기 실패함. 현재진행중인 Motion=%d %d" ), pUseMover->m_dwMotion, dwLevel );
		return FALSE;
	}

	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )		
		return FALSE;

	if( pSkillProp->dwID == SKILL_INDEX( 44, SI_ASS_HEAL_HEALING ) )	// 힐링 상대가 죽었으면 취소
	{
		if( pFocusObj->IsDie() )
			return FALSE;
	}

	BOOL bSuccess = TRUE;
	// 쏘는 형태만 sfx발생.
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT	)
	{
#ifdef __WORLDSERVER
		int nMaxDmgCnt	= 1;
		if( pSkillProp->tmContinuousPain != NULL_ID )
			nMaxDmgCnt	= ( pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime ) + 1;

		float fDmgPower = 0;
		if( pSkillProp->dwDmgShift != NULL_ID )
			fDmgPower	= (int)pSkillProp->dwDmgShift * 0.2f;

		pUseMover->OnAttackSFX( pFocusObj->GetId(),
								pAddSkillProp->dwSkillLvl,
								pSkillProp->dwID,
								0,
								pUseMover->GetAngle(),
								fDmgPower,
								( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ),
								bControl );

#endif	// __WORLDSERVER
		ShootSkill( pUseMover, pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	}
	else
	{
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
		int nDamage = DoApplySkill( pUseMover, pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, bControl );
		if( nDamage == 0 )
			return FALSE;

#ifdef __CLIENT	
		if( pSkillProp->dwID == SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ) )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			if( pUseMover->IsActiveMover() )
			{
				D3DXVECTOR3 vStart;
				D3DXVECTOR3 vEnd;

				vStart = pUseMover->GetPos();		vStart.y += 1.0f;
				vEnd = pWndWorld->m_vTelePos;

				CWorld *pWorld = pUseMover->GetWorld();

				if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
					g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
					return FALSE;
				}

				g_DPlay.SendTeleSkill( pUseMover->GetId(), pWndWorld->m_vTelePos );
			}
		}
#endif // Client		
	}

	if( bSuccess )	// 스킬사용 실패하면 마나안깎임. / 경험치 안오름.
	{
#ifdef __WORLDSERVER
		// 발사한 순간에 마나등을 깍는다.
		int nReqMp = pUseMover->GetReqMp( pAddSkillProp->nReqMp );

		//sun: 10차 전승시스템	Neuz, World, Trans
		if( nReqMp != 9999999 )
		{
			if( pAddSkillProp->nSkillCount > 1 )
				nReqMp /= pAddSkillProp->nSkillCount;

			pUseMover->IncManaPoint( -nReqMp );
			if( pAddSkillProp->nReqFp != NULL_ID )
				pUseMover->IncFatiguePoint( -(pUseMover->GetReqFp( pAddSkillProp->nReqFp )) );
		}

		//sun: 10차 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwID == SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) )
		{
			REPLACE_TYPE type = REPLACE_NORMAL;

			PRegionElem pRgnElem	= NULL;
			CWorld* pWorld	= pUseMover->GetWorld();
			if( !pWorld )
				return FALSE;

			if( pUseMover->IsChaotic() )
			{
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				{
					pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				}
				if( NULL == pRgnElem )	// Find near revival pos
				{
					pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUseMover->GetPos() );		
				}
			}
			else
			{
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				{
					pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				}
				if( NULL == pRgnElem )	// Find near revival pos
				{
					pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUseMover->GetPos() );
				}
			}

			if( NULL != pRgnElem )
			{
				pUseMover->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, type, nRevivalLayer );
			}
			else
			{
				pUseMover->REPLACE( g_uIdofMulti, pWorld->GetID(), pUseMover->GetPos(), type, nDefaultLayer );
			}
		}
		else if( pSkillProp->dwID == SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ) )
		{
			D3DXVECTOR3 vSrc	= pUseMover->GetPos();
			FLOAT		fAngle	= GetDegree( pFocusObj->GetPos(), pUseMover->GetPos() );	// 시전자와 타겟의 각도를 구함
			//vSrc.x -= 1.0f;
			//vSrc.z -= 1.0f;

			CWorld* pFocusObjWorld = pFocusObj->GetWorld();
			if( NULL == pFocusObjWorld )
				return FALSE;

			CWorld* pUseMoverWorld = pUseMover->GetWorld();
			if( NULL == pUseMoverWorld )
				return FALSE;

			if( pFocusObjWorld->GetID() != pUseMoverWorld->GetID() || pFocusObj->GetLayer() != pUseMover->GetLayer() )
			{
				FLWARNING_LOG( PROGRAM_NAME, _T("SI_KNT_HERO_DRAWING Focus[%d][%d] UseMover[%d][%d]"), 
					pFocusObjWorld->GetID(), pFocusObj->GetLayer(), pUseMoverWorld->GetID(), pUseMover->GetLayer() );
				return FALSE;
			}

// 			pFocusObj->SetPos(vSrc);
// 			g_xWSUserManager->AddSetPos( pFocusObj, vSrc );
// 			g_xWSUserManager->AddPushPower( pFocusObj, pFocusObj->GetPos(), pFocusObj->GetAngle(), fAngle, 0.2f );
// 			pFocusObj->ClearDestObj();

			D3DXVECTOR3 pos = pFocusObj->GetPos() - pUseMover->GetPos();
			const float distance = ::sqrt( D3DXVec3LengthSq( &pos ) ) * -0.1f;
			if( !pFocusObj->pushedByAnotherPower( fAngle, distance ) ) {
				return FALSE;
			}
		}

#endif // WORLDSERVER

		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
#ifdef __CLIENT
		if( pUseMover->IsActiveMover() && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
#endif // __CLIENT
			//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
			if( pAddSkillProp->dwCooldown != NULL_ID )
				pUseMover->SetCoolTime( pAddSkillProp, "OnMagicSkill" );

	} // bSuccess

#if defined(__CLIENT)
	CModelObject* pModel = (CModelObject*)pUseMover->GetModel();
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;

		AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, pUseMover->GetAngle(), -1.0f );
	}
#endif //defined(__CLIENT)

	return TRUE;
}

//
//	지속성을 갖는 스킬이 시간이 다되 끝났을때 호출됨.
//
void	FLSkillSystem::OnEndSkillState( CMover* pTarget, DWORD dwSkill, DWORD dwLevel )
{
	SkillProp* pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	if( pTarget->GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "OnEndSkillState" ) == FALSE )
	{
		return;
	}

	if( dwSkill == SKILL_INDEX( 204, SI_ACR_YOYO_COUNTER ) )	// 카운터 어택스킬의 경우
	{
		pTarget->m_pActMover->ResetState( OBJSTA_ATK_ALL );		// 공격동작 중단시킴.  (이런 패턴의 스킬은 앞으로 일반화 시키자)
	}

	//#if defined( __CLIENT ) 
	//
	//	switch( dwSkill )	// 091022 mirchang - 프로텍션, 펜바리어 스킬 버프 해제 시 sfx 삭제
	//	{
	//		case SKILL_INDEX( 65, SI_MAG_FIRE_HOTAIR ):
	//			SetRemoveSfxObj( pSkillProp->dwSfxObj2 );
	//			break;
	//		case SKILL_INDEX( 242, SI_PSY_HERO_STONE ):
	//		case SKILL_INDEX( 10, SI_MER_SHIELD_PANBARRIER ):
	//		case SKILL_INDEX( 9, SI_MER_SHIELD_PROTECTION ):
	//			SetRemoveSfxObj( pSkillProp->dwSfxObj4 );
	//			break;
	//
	//			//gmpbigsun( 20100823 ) 음 .. 자동으로 제거하게 바꺼야 한다. 일단 하고...
	//		default:
	//			SetRemoveSfxObj( pSkillProp->dwSfxObj4 );
	//			break;
	//	}
	//  #endif	// 

#ifdef __WORLDSERVER
	// 서버일경우 유저들에게 보내줌.
	for( size_t Nth = 0; Nth < MAX_SKILL_PARAM; ++Nth )
	{
		pTarget->ResetDestParam( pAddSkillProp->dwDestParam[ Nth ], pAddSkillProp->dwAdjParamVal[ Nth ], TRUE );
	}
// 	pTarget->ResetDestParam( pAddSkillProp->dwDestParam1, pAddSkillProp->dwAdjParamVal[0], TRUE );
// 	pTarget->ResetDestParam( pAddSkillProp->dwDestParam2, pAddSkillProp->dwAdjParamVal[1], TRUE );
#endif
}

// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
#ifdef __CLIENT
void	FLSkillSystem::ShootSkill( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp* pSkillProp, AddSkillProp* pAddSkillProp )
{
	DWORD dwShootObj = pSkillProp->dwSfxObj2;
	if( dwShootObj != NULL_ID )
	{
		D3DXVECTOR3 vPos = pUseCtrl->GetPos();

		if( ((CMover*)pUseCtrl)->IsPlayer() )
		{
			// 보우에서 나가는 화살은 몇가지 셋팅을 해주자
			if( pSkillProp->dwNeedItemKind3 == IK3_BOW )
			{
				// 화살은 왼손에 연결...
				CModelObject *pModel = (CModelObject *)pUseCtrl->GetModel();
				pModel->GetHandPos( &vPos, PARTS_LWEAPON, pUseCtrl->GetMatrixWorld() );		// 주먹 월드좌표 구함		
				vPos.y -=	1.0f;
			}

#ifdef __ADDWEAPON_CROSSBOW16
			if( pSkillProp->dwNeedItemKind3 == IK3_CROSSBOW )
			{
				// 화살은 왼손에 연결...
				CModelObject *pModel = (CModelObject *)pUseCtrl->GetModel();
				pModel->GetHandPos( &vPos, PARTS_LWEAPON, pUseCtrl->GetMatrixWorld() );		// 주먹 월드좌표 구함		
				vPos.y -=	1.0f;
			}
#endif //__ADDWEAPON_CROSSBOW16
		}

		CSfxShoot *pShootSfx = CreateShootSfx( D3DDEVICE, dwShootObj, vPos, pUseCtrl->GetId(), D3DXVECTOR3(0,0,0), pTarget->GetId() );
		if( pShootSfx )
		{		
			D3DXVECTOR3 vPos = pUseCtrl->GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.

			if( pUseCtrl->GetType() == OT_MOVER )
			{
				if( ((CMover*)pUseCtrl)->IsActiveMover() )		// g_pPlayer가 쏠때만 생성해서 전송.
				{
					int idSfxHit = ++(((CMover*)pUseCtrl)->m_idSfxHit);	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
					int		nMaxDmgCnt = 1;
					if( pSkillProp->tmContinuousPain != NULL_ID )		// 지속스킬이 있는 데미지 형태면
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.

					if( pSkillProp->dwSkillType == KT_SKILL )
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MELEESKILL, NULL_ID, nMaxDmgCnt );
					else
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt );
					pShootSfx->m_idSfxHit	= idSfxHit;
				}
			}

			pShootSfx->SetSkill( pSkillProp->dwID );
			pShootSfx->m_nMagicPower	= pAddSkillProp->dwSkillLvl;	// 스킬레벨은 MagicPower변수로 같이써도 될거 같다.
			pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;	// 폭발 이펙트 등록.

		}
		// 지속시간동안 이펙트도 유지되어야 하는것은 CSkillInfluence::Process()에서 처리한다.
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s dwSfxObj2가 지정되지 않음" ), pSkillProp->szName );
	}
}
#else
void	FLSkillSystem::ShootSkill( CCtrl* /*pUseCtrl*/, CCtrl* /*pTarget*/, SkillProp* /*pSkillProp*/, AddSkillProp* /*pAddSkillProp*/ )	{}
#endif // CLIENT

int		FLSkillSystem::DoApplySkill( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bControl )
{
	int nResult = DoApplySkillEx( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, bIgnoreProp, nParam, bOnlyDmg, bControl );
	if( nResult == 1 )
	{
#ifdef	__WORLDSERVER
		if( pUseCtrl != pTarget && pUseCtrl->GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER )
		{
			if( pSkillProp->IsDebuff() == TRUE )	// 나쁜마법을 쓰면 공격과 같다고 간주한다.
			{
				((CMover*)pTarget)->OnAttacked( (CMover*)pUseCtrl, 0, TRUE, 0 );	// TRUE는 타겟이다.
			}
		}
#endif 
	}

	return nResult;
}

// 
// 지정된 스킬의 효과를 pTarget에게 적용한다.
// 지정범위에 따라서 pTarget을 중심으로 다른 target에도 적용된다.
// 이것을 사용하는 주체는 사람이 아닐수도 있다
// 여기서는 SpellRegion에 따라 적용대상을 분류만 하고 실제 효과적용은
// ApplySkill에서 처리한다.
// bOnlyDmg : 오직 데미지적용시키는것만 한다.  광역 지속데미지개체에서 사용함.
int		FLSkillSystem::DoApplySkillEx( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bControl )
{
	int		nRet = 1;
	switch( pSkillProp->dwSpellRegion )
	{
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
		{
			if( pSkillProp->dwExeTarget != EXT_TROUPE && pSkillProp->dwExeTarget != EXT_TROUPEWITH )	// 극단대상이 아닐때.
			{
				int nApplyType = 0;
				nApplyType = pUseCtrl->TargetSelecter( pTarget );
				ApplySkillAround( pUseCtrl, pTarget, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProp, bOnlyDmg, 0.0f, bControl );
			}
			else if( pSkillProp->dwExeTarget == EXT_TROUPE || pSkillProp->dwExeTarget == EXT_TROUPEWITH )	// 극단 대상(자기 자신 포함)
			{
				if( pUseCtrl->GetType() == OT_MOVER && ((CMover *)pUseCtrl)->IsPlayer() )
				{
					ApplySkillAroundTroupe( pUseCtrl, ((CMover *)pUseCtrl)->m_idparty, pSkillProp, pAddSkillProp, bIgnoreProp );
				}
			}

#ifdef __WORLDSERVER
			// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
			if( pSkillProp->tmContinuousPain != NULL_ID && !(((CMover*)pTarget)->IsPlayer() && ((CMover*)pTarget)->m_nDead) )	// 지속데미지 형태냐?
			{
				CContDamageCtrl *pDmgCtrl = new CContDamageCtrl;	// 지속데미지를 주는 가상오브젝트 생성 - 메모리풀링 처리 할것
				if( pDmgCtrl )
				{
					pDmgCtrl->m_pSkillProp = pSkillProp;
					pDmgCtrl->m_pAddSkillProp = pAddSkillProp;
					pDmgCtrl->m_idSrc = pUseCtrl->GetId();
					pDmgCtrl->m_idTarget = pTarget->GetId();
					pDmgCtrl->SetPos( pTarget->GetPos() );		// 중심이 되는 타겟의 좌표로...
					pDmgCtrl->m_bControl = bControl;

					// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
					if( ((CMover*)pUseCtrl)->IsPVPTarget( ((CMover*)pTarget) ) )
						pDmgCtrl->m_bDuelTarget = TRUE;

					if( pUseCtrl->GetWorld()->ADDOBJ( pDmgCtrl, FALSE, pUseCtrl->GetLayer() ) == FALSE )
					{
						SAFE_DELETE( pDmgCtrl );
					}
				}
			}
#endif
		}
		break;
	case SRO_LINE:		// this를 중심으로 m_fAngle라인.
		{
			int nApplyType = 0;
			nApplyType = pUseCtrl->TargetSelecter( pTarget );
			ApplySkillLine( pUseCtrl, nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProp, bControl );
		}
		break;
	case SRO_DIRECT:	// 대상 하나에만 적용
	default:
		if( pSkillProp->dwID != SKILL_INDEX( 163, SI_PSY_PSY_PSYCHICWALL ) )		// 사이킥월은 처리 안함.
		{
			ApplySkill( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, bIgnoreProp, nParam );
		}
		break;
	} 

#ifdef __WORLDSERVER		
	// 싸이킥월의 벽을 만들어냄.
	if( pSkillProp->dwID == SKILL_INDEX( 163, SI_PSY_PSY_PSYCHICWALL ) )
	{
		BOOL bCreate = FALSE;
		int i = 0;
		if( pUseCtrl->GetType() == OT_MOVER )
		{
			if( ((CMover *)pUseCtrl)->IsPlayer() )
			{
				FLWSUser *pUser = (FLWSUser *)pUseCtrl;
				for( i = 0; i < 2; i ++ )		// 한번에 2개이상 생성되지 않게함.
				{
					if( pUser->m_pWall[i] == NULL )
					{
						CObj *pObj = CreateObj( D3DDEVICE, OT_CTRL, CTRL_INDEX( 29, CI_PSYCHICWALL ) );		// 벽 컨트롤 생성.
						if( pObj )
						{
							pObj->SetPos( pUseCtrl->GetPos() );
							pObj->SetAngle( -(pUseCtrl->GetAngle()) );
							((CCommonCtrl *)pObj)->m_pSkillProp = pSkillProp;
							((CCommonCtrl *)pObj)->m_pAddSkillProp = pAddSkillProp;
							((CCommonCtrl *)pObj)->m_idAttacker = pUseCtrl->GetId();
							((CCommonCtrl *)pObj)->m_bControl = bControl;

							if( pUseCtrl->GetWorld()->ADDOBJ( pObj, TRUE, pUseCtrl->GetLayer() ) == TRUE )
							{
								pUser->m_pWall[i] = pObj;
								bCreate = TRUE;
							}
							else
							{
								SAFE_DELETE( pObj );
								bCreate = FALSE;
							}
							break;
						}
					}
				}
			}
		}
		if( bCreate == FALSE )	// 생성못했으면
			nRet = 0;			// 실패 리턴

	}


	if( pUseCtrl->GetType() == OT_MOVER )
	{
		// 모든 스킬사용시 다크일루젼상태이면 해제한다!!~
		if( pSkillProp->dwID != SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) )
		{
			if( ((CMover *)pUseCtrl)->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) )
			{
				((CMover *)pUseCtrl)->SetDarkCover( FALSE );
				((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) );		// 해제.
			}
		}

		// 제스터 버프는 특정 버프가 있으면 해제하고 걸어야함
		if( pSkillProp->dwID == SKILL_INDEX( 208, SI_JST_SUP_POISON ) )
		{
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ) );
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 210, SI_JST_SUP_ABSORB ) );
		}
		else if( pSkillProp->dwID == SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ) )
		{
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 208, SI_JST_SUP_POISON ) );
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 210, SI_JST_SUP_ABSORB ) );
		}
		else if( pSkillProp->dwID == SKILL_INDEX( 210, SI_JST_SUP_ABSORB ) )
		{
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ) );
			((CMover *)pUseCtrl)->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 208, SI_JST_SUP_POISON ) );
		}

	}	
#endif // WorldServer

#ifdef __CLIENT
	// 스킬에 필요한 sfx를 생성.
	CreateSkillSfx( pUseCtrl, pTarget, pSkillProp, pAddSkillProp );
#endif

	// 2차 스킬 발동. ActiveSkill
	if( pSkillProp->dwActiveSkill != NULL_ID )
	{
		if( pSkillProp->dwActiveSkillRate == NULL_ID || xRandom(100) <= pSkillProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
			DoActiveSkill( pUseCtrl, pSkillProp->dwActiveSkill, MIN_SKILL_USE_LEVEL, pTarget, false, bControl );		// dwActiveSkill Lv1이 발동됨.

	}

	return nRet;
}


// pTarget를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// fRangeCustom에 값이 있을경우에는 프로퍼티 nRange값을 무시하고 fRangeCustom으로 바뀐다.
void	FLSkillSystem::ApplySkillAround( CCtrl* pUseCtrl, CCtrl* pTarget, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, BOOL bOnlyDmg, FLOAT fRangeCustom, BOOL bControl )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = pTarget->GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "pSkillProp == NULL D:%d A:%d-%d" ), pTarget->GetIndex(), pUseCtrl->GetType(), pUseCtrl->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
		fRange = (float)pAddSkillProp->dwSkillRange;
	else
		fRange = (float)pSkillProp->dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	if( fRangeCustom != 0.0f )
	{
		fRange = fRangeCustom;
	}

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		FLERROR_LOG( PROGRAM_NAME, _T( "D:%d A:%d-%d %d %f" ), pTarget->GetIndex(), pUseCtrl->GetType(), pUseCtrl->GetIndex(), pSkillProp->dwID, fRange );

	FLOAT fTargetRatio = 0.0f;
	if( pSkillProp->dwSpellRegion == SRO_REGION )	// REGION속성일땐 타겟도 데미지를 먹음.
		fTargetRatio = 1.0f;

	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( pTarget->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, pTarget->GetLayer() )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				FLERROR_LOG( PROGRAM_NAME, _T( "pObj가 MOVER가 아니다 %d" ), pObj->GetType() );

			if( pUseCtrl->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pUseCtrl;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply	= TRUE;
					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						bTarget = (pTarget == pDefender);

						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
						{
							bApply = FALSE;
						}
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
						{
							bApply = FALSE;
						}
					}
				}
				else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			}
			else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( pTarget, fRange ) )	// pTarget(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == pTarget && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					}
					else
					{
						if( pObj != pUseCtrl )		// 어태커는 검색대상에서 제외.
						{
							CMover *pTargetObj = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.
							bTarget = (pTarget == pTargetObj);
							if( IsValidObj( pTargetObj ) && pTargetObj->IsLive() )
								ApplySkill( pUseCtrl, pTargetObj, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}

				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( pTarget->GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, pTarget->GetLayer() )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTargetObj = (CMover *)pObj;
				if( pTargetObj->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;

				//sun: 8, 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)pUseCtrl;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTargetObj->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;

			}
			else
			{
				// 대상이 컨트롤이면.
				if( pObj->GetType() == OT_CTRL )
				{
					if( pUseCtrl->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
						bApply = TRUE;							
				}
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( pTarget, fRange ) )	// this(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == pTarget && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					}
					else
					{
						if( pObj != pUseCtrl )		// 공격자는 검사대상에서 제외.
						{
							if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
							{
								if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
									ApplySkill( pUseCtrl, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							}
							else
							{
								// 타겟이 무버가 아니냐?
								ApplySkill( pUseCtrl, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							}
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

#endif // WORLDSERVER		
}

//
// idParty 극단대상 범위 효과.
//
void	FLSkillSystem::ApplySkillAroundTroupe( CCtrl* pUseCtrl, u_long idParty, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp )
{
#ifdef __WORLDSERVER
	if( pUseCtrl->GetType() != OT_MOVER || ((CMover *)pUseCtrl)->IsPlayer() == FALSE )
	{
		return;
	}

	CParty *pParty	= g_PartyMng.GetParty( idParty );
	if( pParty != NULL )
	{
		D3DXVECTOR3	vDist;
		float		fDist;
		float		fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
		fMaxDistSq *= fMaxDistSq;
		FLOAT fTargetRatio = 0.0f;

		if( pSkillProp->dwSpellRegion == SRO_REGION )
		{
			fTargetRatio = 1.0f;
		}

		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( pMember ) && !pMember->IsDie()
				&& pUseCtrl->GetWorld() && pMember->GetWorld() && pUseCtrl->GetLayer() == pMember->GetLayer() )	// mirchang_100217 범위 검사 오류 수정
			{
				if( pMember == pUseCtrl && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
				{
				}
				else
				{
					vDist = pMember->GetPos() - pUseCtrl->GetPos();
					fDist = D3DXVec3LengthSq( &vDist );			// 파티 멤버와 시전자와의 거리.
					if( fDist < fMaxDistSq )					// xx미터보다 작으면
					{
						ApplySkill( pUseCtrl, pMember, pSkillProp, pAddSkillProp, bIgnoreProp );	// 멤버 각각에게 효과 적용.
					}
				}
			}
		}
	}
	else
	{
		if( pSkillProp->dwExeTarget == EXT_TROUPEWITH )
		{
			ApplySkill( pUseCtrl, pUseCtrl, pSkillProp, pAddSkillProp, bIgnoreProp );	
		}
	}
#endif // WorldServer
} // ApplySkillAroundTroupe

// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
void	FLSkillSystem::ApplySkillLine( CCtrl* pUseCtrl, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProp, BOOL bControl )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = pUseCtrl->GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "pSkillProp == NULL %d" ), pUseCtrl->GetIndex() );

	// 데미지 영역 Local AABB.
	D3DXVECTOR3 vMin1 = D3DXVECTOR3( -fWidth / 2.0f, -fHeight / 2.0f, -fDepth );
	D3DXVECTOR3 vMax1 = D3DXVECTOR3(  fWidth / 2.0f,  fHeight / 2.0f, 0 );

	D3DXMATRIX mInv;		// 공격자측 월드역행렬
	D3DXMATRIX m1, m2, m3;
	D3DXMatrixRotationY( &m1, D3DXToRadian( -(pUseCtrl->GetAngle()) ) );
	D3DXMatrixTranslation( &m2, pUseCtrl->GetPos().x, pUseCtrl->GetPos().y, pUseCtrl->GetPos().z );
	D3DXMatrixMultiply( &m3, &m1, &m2 );	// world = rot * trans

	D3DXMatrixInverse( &mInv, NULL, &m3 );		// 공격자의 역행렬 만듬.

	// 타겟의 좌표를 공격자측 기준으로 역변환 한것.
	D3DXVECTOR3 vDestLocal;

	// 일반적으로 fDepth가 가장 길기때문에 검사 영역은 fDepth로 했다. 
	if( fDepth <= 0 )	// 범위가 0이거나 음수일수는 없다.
		FLERROR_LOG( PROGRAM_NAME, _T( "%d %d" ), pUseCtrl->GetIndex(), fDepth );

	if( fDepth <= 4.0f )		nRange = 4;
	else if( fDepth <= 8.0f )	nRange = 8;
	else if( fDepth <= 16.0f )	nRange = 16;
	else						nRange = 32;

	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;
	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( pUseCtrl->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, pUseCtrl->GetLayer() )
		{
			bApply = FALSE;	// 시작은 FALSE

			if( pObj->GetType() != OT_MOVER )
				FLERROR_LOG( PROGRAM_NAME, _T( "pObj가 MOVER가 아니다 %d" ), pObj->GetType() );

			if( pUseCtrl->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pUseCtrl;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply = TRUE;

					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
						{
							bApply = FALSE;
						}
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
						{
							bApply = FALSE;
						}
					}
				}
				else
				{
					// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			}
			else
			{
				// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply && pObj != pUseCtrl )		// this는 공격대상에서 제외.
			{
				CMover *pTarget = (CMover *)pObj;
				if( IsValidObj( pTarget ) && pTarget->IsLive() )
				{
					// 타겟측 좌표를 역변환.
					D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
					// 타겟측 AABB
					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
					{
						ApplySkill( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, bIgnoreProp, 0, FALSE, bTarget );		// bTarget 타겟이 없으므로 FALSE로 넣음. PK시 선빵을 칠수없음
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( pUseCtrl->GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, pUseCtrl->GetLayer() )
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;

				//sun: 8, 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)pUseCtrl;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTarget->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;

			}
			else
			{
				// 대상이 컨트롤이면
				if( pObj->GetType() == OT_CTRL )
				{
					if( pUseCtrl->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
						bApply = TRUE;							
				}
			}


			if( bApply )
			{
				if( pObj->GetType() == OT_MOVER )
				{
					CMover *pTarget = (CMover *)pObj;

					if( IsValidObj( pTarget ) && pTarget->IsLive() )
					{
						// 타겟측 좌표를 역변환.
						D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
						// 타겟측 AABB
						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
						{
							ApplySkill( pUseCtrl, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp );		// 대상에게 효과를 적용함.
						}
					}
				}
				else
				{
					ApplySkill( pUseCtrl, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp );		// 대상에게 효과를 적용함.
				}
			}
		}
		END_LINKMAP
	}

#endif // WORLDSERVER		
}

// vPos를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// pCenter : 광역 지속스킬의 타겟. NULL일수도 있다
void	FLSkillSystem::ApplySkillRegion( CCtrl* pUseCtrl, const D3DXVECTOR3 &vPos, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, BOOL bOnlyDmg, CCtrl* pCenter, BOOL bControl )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	CCtrl *pSrc = pUseCtrl;
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "pSkillProp == NULL A:%d-%d" ), pSrc->GetType(), pSrc->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
	{
		fRange = (float)pAddSkillProp->dwSkillRange;
		if( pAddSkillProp->dwSkillRange == NULL_ID )
			FLERROR_LOG( PROGRAM_NAME, _T( "스킬 %d의 dwSkillRange가 지정되지 않음" ), pAddSkillProp->dwID );
	}
	else
	{
		fRange = (float)pSkillProp->dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.
		if( pSkillProp->dwSkillRange == NULL_ID )
			FLERROR_LOG( PROGRAM_NAME, _T( "스킬 %d의 dwSkillRange가 지정되지 않음" ), pSkillProp->dwID );
	}

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		FLERROR_LOG( PROGRAM_NAME, _T( "D:%d A:%d-%d %d %f" ), pUseCtrl->GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange );

	BOOL	bApply = FALSE;
	BOOL	bTarget = TRUE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( pUseCtrl->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, pUseCtrl->GetLayer() )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				FLERROR_LOG( PROGRAM_NAME, _T( "pObj가 MOVER가 아니다 %d" ), pObj->GetType() );

			if( pSrc->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply = TRUE;
					//sun: 8, // __S8_PK
					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						if( pCenter )
							bTarget = (pCenter == pDefender);
						else
							bTarget = FALSE;

						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
							bApply = FALSE;
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
							bApply = FALSE;
					}

				}
				else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			}
			else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 어태커는 검색대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						CMover *pTarget = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.

						if( pCenter )
							bTarget = (pCenter == pTarget);
						else
							bTarget = FALSE;

						if( IsValidObj( pTarget ) && pTarget->IsLive() )
						{
							ApplySkill( pSrc, pTarget, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}

				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( pUseCtrl->GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, pUseCtrl->GetLayer() )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
				//sun: 8, 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTarget->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;

			}
			else
			{
				// 대상이 컨트롤이면.
				if( pObj->GetType() == OT_CTRL )
				{
					if( pSrc->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
						bApply = TRUE;							
				}
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 공격자는 검사대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
						{
							if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
								ApplySkill( pSrc, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						}
						else
						{
							// 타겟이 무버가 아니냐?
							ApplySkill( pSrc, (CCtrl*)pObj, pSkillProp, pAddSkillProp, bIgnoreProp, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

#endif // WORLDSERVER		
} // ApplySkillRegion()


// pSkillProp을 this에 적용한다.  데미지가 될수도 있고. 상태변화가 될수도있다.
// pUseCtrl는 시전자(혹은 어태커)
// pAddSkillProp은 NULL일수도 있음을 주의.
void	FLSkillSystem::ApplySkill( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bTarget )
{
	if( pUseCtrl->GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER )
	{
		if( ((CMover *)pUseCtrl)->IsPlayer() && ((CMover *)pTarget)->IsPlayer() && pSkillProp->dwSkillType != NULL_ID )
		{
			DWORD dwPKRegionAttr = ((CMover *)pTarget)->GetPKPVPRegionAttr();
			int nAttackerPK = ((CMover *)pUseCtrl)->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
			int nDefenDerPK = ((CMover *)pTarget)->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
			if( nAttackerPK == 1 || nDefenDerPK == 1 )
			{
				if( pSkillProp->IsDebuff() == TRUE )
				{
					CMover* pAttacker = (CMover*)pUseCtrl;
					CMover* pDefender = (CMover*)pTarget;
					if( !(pAttacker->IsPVPTarget( pDefender ) || pAttacker->IsWarTarget( pDefender ) ||
						pAttacker->IsGuildCombatTarget( pDefender ) || pAttacker->IsArenaTarget( pDefender ) || pAttacker->IsEventArenaTarget( pDefender ))
						)
					{
						return;
					}

					//if( !((CMover *)pSrc)->IsPVPTarget((CMover *)this) )
					//	return;
				}
			}
		}
	}

	// 데미지속성이 있을경우 그것을 처리.
	int nDamage = ApplyDamage( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, nParam, bTarget );
	if( bOnlyDmg == TRUE )		
	{
		return;
	}

	if( CanApplyBuff( static_cast<CMover*>( pUseCtrl ), static_cast<CMover*>( pTarget ), pSkillProp ) == false )
	{
		return;
	}

#if defined(__WORLDSERVER)
	if( pTarget != pUseCtrl && pUseCtrl->GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER )		// 광역 디버프 중 타겟이아니면 skip
	{
		if( ((CMover *)pTarget)->IsDie() == FALSE )
		{
			if( pSkillProp->IsDebuff() == TRUE )													// 나쁜마법은
			{
				if( ((CMover *)pUseCtrl)->GetHitType2(((CMover *)pTarget), bTarget, FALSE ) == HITTYPE_FAIL )  	// 평화적인상대에게
					return;																			// 적용안됨
			}

			CWorld* pWorld	= pUseCtrl->GetWorld();
			if(  pWorld && pWorld->IsArena() == FALSE )		// 아레나가 아니면,
			{
				if( pSkillProp->IsDebuff() == FALSE )													// 좋은 마법은
				{
					if( ((CMover *)pUseCtrl)->GetHitType2(((CMover *)pTarget), bTarget, TRUE ) != HITTYPE_FAIL )  	// 적대적인상대에게
						return;																			// 적용안됨
				}
			}
		}
		else
		{
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )
			{
				if( pSkillProp->dwSfxObj2 != NULL_ID )
					g_xWSUserManager->AddCreateSfxObj( pTarget, pSkillProp->dwSfxObj2 );
			}

			if( pSkillProp->IsDebuff() == TRUE )
				return;
		}
	}
#endif //defined(__WORLDSERVER)

	int nProb	= pSkillProp->nProbability;
	if( pAddSkillProp )
	{
		nProb	= pAddSkillProp->dwProbability;
		//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
		BOOL bPVP	= ( pUseCtrl->GetType() == OT_MOVER && ( (CMover *)pUseCtrl )->IsPlayer() && pTarget->GetType() == OT_MOVER && ( (CMover *)pTarget )->IsPlayer() );
		if( bPVP )
			nProb	= pAddSkillProp->dwProbabilityPVP;
	}

	if( bIgnoreProp )	// 확률무시옵이 있으면 무조건 100%
		nProb = NULL_ID;

	if( nProb == NULL_ID || (int)( xRandom( 100 ) ) < nProb  )		// 적용 확률, =로 되어 있으면 100% 적용
	{
		if( pTarget->GetType() == OT_MOVER )		// 타겟이 무버일때만 지속시간 처리
		{
			DWORD dwSkillTime = 0;
			DWORD dwPainTime = 0;
			if( pAddSkillProp )
			{
				dwSkillTime = pAddSkillProp->dwSkillTime;
				dwPainTime = pAddSkillProp->dwPainTime;
			}
			else
			{
				dwSkillTime = pSkillProp->dwSkillTime;
				dwPainTime = pSkillProp->tmContinuousPain;
			}

			BOOL bNew = TRUE;	// 스킬이 첨 적용됐는가.
			//sun: 팻, 변신 아이템 토글버프 적용... Neuz, World
			if( dwSkillTime != NULL_ID )	// 지속시간이 있는 스킬(프로텍션 등)
			{
				DWORD dwTime1 = 0, dwTime2 = 0;
				if( pSkillProp->dwReferTarget1 == RT_TIME || pSkillProp->dwReferTarget2 == RT_TIME )
				{
					if( pUseCtrl->GetType() == OT_MOVER )		// 시전자가 무버일때만.
						((CMover *)pUseCtrl)->SubReferTime( &dwTime1, &dwTime2, pSkillProp, pAddSkillProp );
				}

				WORD wType;
				if( pSkillProp->dwSkillType == NULL_ID )	
					wType = BUFF_ITEM;
				else										
					wType = BUFF_SKILL;

				// pTarget은 this가 될수도있고 타인이 될수도 있다.
#if defined(__WORLDSERVER)
				int nLevel = 0;
				if( pAddSkillProp )		
					nLevel = pAddSkillProp->dwSkillLvl;
				else					
					nLevel = 0;

				// dwSkillTime이 스킬자체의 지속시간으로도 쓰이고 부가효과의 지속시간으로도 쓰이기때문에 이런구조는 맞지 않다
				// 이대로라면 버닝필드도 버프처럼 걸려버린다.
				if( pSkillProp->dwID == SKILL_INDEX( 170, SI_ELE_FIRE_BURINGFIELD ) )
				{
					bNew = FALSE;	//  버닝필드는 예외처리
				}
				else
				{
					CMover * pThisMover = static_cast<CMover*>( pTarget );
					CMover * pSrcMover = static_cast< CMover * >( pUseCtrl );

					//#if defined __WORLDSERVER && defined __AGGRO16
					//					const BOOL bCanNotUseBuffInAttackMode	= ( pSrcMover->IsAttackMode() == TRUE && ( wType == BUFF_SKILL && pSkillProp->dwID == SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) );		//전투중 쓸수 없는 버프 여기다. 하드코딩해라.
					//					if( bCanNotUseBuffInAttackMode == FALSE )
					//					{
					//#endif		//__AGGRO16
					bNew	= pThisMover->AddBuff( wType, (WORD)( pSkillProp->dwID ), nLevel, dwSkillTime + (int)dwTime1 + (int)dwTime2, dwPainTime, pSkillProp->byBuffTickType, pUseCtrl->GetId() );
#if defined __WORLDSERVER && defined __AGGRO16
					if( bNew == TRUE )
					{
						if(  pSkillProp->IsDebuff() == TRUE )	//나쁜 스킬
						{
							if( pThisMover->GetAggroMng() != NULL )
								pThisMover->GetAggroMng()->SumDebuffAggro( *pSrcMover, *pThisMover );
						}
						else
						{
							FLAggro::SumBuffAggro( *pSrcMover, *pThisMover );
						}
					}
					//}
					//else if( pSrcMover->IsPlayer() == TRUE )
					//{
					//	static_cast< FLWSUser * >( pSrcMover )->AddDefinedText( TID_MMI_GHOUSE_UNSKILL );
					//	bNew					= FALSE;
					//}

#endif //__WORLDSERVER __AGGRO16
				}


#elif defined(__CLIENT)
				bNew = ((CMover *)pTarget)->HasBuff( wType, (WORD)( pSkillProp->dwID ) ) ? FALSE : TRUE;
#endif // Client
			}

			// 최초 적용된 스킬에만
			if( bNew )	// 이미 걸려있는 상태에서 또 쓰면 bNew는 FALSE가 된다.
			{
#ifdef __WORLDSERVER
				// 스킬중 예외처리 해야할것을 처리함.
				if( ApplySkillHardCoding( pUseCtrl, pTarget, pSkillProp, pAddSkillProp ) == FALSE )
					return;

				// 부활스킬셋팅이 되어있다면 아랫것들 무시
				if( ((CMover *)pTarget)->m_Resurrection_Data.bUseing )
					return;

				// 데미지도 주면서 파라메터도 변하는 스킬이 있기땜에 데미지랑 따로 처리.
				switch( pSkillProp->dwExeTarget )
				{
				case EXT_SELFCHGPARAMET:		// 시전자 자신.
					if( pUseCtrl->GetType() == OT_MOVER )	// 타겟(여기선 타겟이 시전자)이 무버일때만...
						ApplyParam( (CMover *)pUseCtrl, pUseCtrl, pSkillProp, pAddSkillProp, TRUE, nDamage );
					break;

				case EXT_OBJCHGPARAMET:
				case EXT_MAGIC:
				case EXT_ANOTHER:
				case EXT_ANOTHERWITH:
				default:
					if( pTarget->GetType() == OT_MOVER )	// 타겟이 무버일때만.
						ApplyParam( (CMover *)pUseCtrl, pTarget, pSkillProp, pAddSkillProp, TRUE , nDamage );
					break;
				}

				if( pTarget->GetType() == OT_MOVER )
				{
					// 밀리는 양이 있는가?
					CMover *pTargetMover = (CMover *)pTarget;
					DWORD dwDmgShift;
					if( pAddSkillProp )
						dwDmgShift = pAddSkillProp->dwDmgShift;
					else
						dwDmgShift = pSkillProp->dwDmgShift;

					if( dwDmgShift != NULL_ID && pTargetMover->GetProp()->dwClass != RANK_SUPER && pTargetMover->GetProp()->dwClass != RANK_MIDBOSS )
					{
						pTargetMover->pushedByAnotherPower( GetDegree( pTargetMover->GetPos(), pUseCtrl->GetPos() ), 0.85f );
					}

					// 상대방을 끌어당긴다
					if( pSkillProp->dwID == SKILL_INDEX( 206, SI_ACR_YOYO_PULLING )	|| pSkillProp->dwID == SKILL_INDEX( 400, SI_LOD_SUP_PULLING ) )
					{
						if( pAddSkillProp && pAddSkillProp->arrAddSkillDestNumData1[2] != NULL_ID )
						{
							// 당겨질 거리
							FLOAT fPullingLen = (FLOAT)pAddSkillProp->arrAddSkillDestNumData1[2] * 0.001f; 

							// 타겟과의 거리
							D3DXVECTOR3 v3Len = pTargetMover->GetPos() - pUseCtrl->GetPos();
							FLOAT fTotalLen = D3DXVec3LengthSq( &v3Len ) * 0.1f;
							{
								if( fTotalLen > pTargetMover->GetAttackRange( AR_HRANGE ) )
									fTotalLen = pTargetMover->GetAttackRange( AR_HRANGE );

								// 거리에 따라 댕겨지는 힘이 다르게 적용됨
								FLOAT fDest = fTotalLen / pTargetMover->GetAttackRange( AR_HRANGE );
								{
									pTargetMover->pushedByAnotherPower( GetDegree( pTargetMover->GetPos(), pUseCtrl->GetPos() ), fPullingLen * -fDest );
								}
							}
						}
					}
				}
#endif // __WORLDSERVER
			} // if( bNew )

#ifdef __WORLDSERVER
			if( pSkillProp->dwID == SKILL_INDEX( 187, SI_GEN_FLASH ) && pTarget->GetType() == OT_MOVER && ((CMover *)pTarget)->IsPlayer() )
			{
				((FLWSUser *)pTarget)->AddCommonSkill( pSkillProp->dwID, pAddSkillProp->dwSkillLvl );
			}
#endif // __WORLDSERVER

		}	// this == OT_MOVER
	}	// if( nProb == NULL_ID || xRandom(100) < nProb  )
#ifdef __WORLDSERVER
	//sun, 11, 확율스킬 효과수정 world,neuz
	else
	{
		if(pSkillProp->dwID == SKILL_INDEX( 242, SI_PSY_HERO_STONE ) )
			g_xWSUserManager->AddRemoveSfxObj( pTarget, pSkillProp->dwSfxObj4 );
	}
#endif	//__WORLDSERVER

#ifdef __WORLDSERVER
	switch( pSkillProp->dwSpellRegion )
	{
	case SRO_AROUND:
	case SRO_REGION:	// 범위공격일땐 여기서 타겟들 각각에 sfxobj3를 뿌려줌.
		if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )
			if( pSkillProp->dwSfxObj3 != NULL_ID )
				g_xWSUserManager->AddCreateSfxObj( pTarget, pSkillProp->dwSfxObj3 );
		break;
	}

	// 예외처리: 강탈스킬 - 페냐를 훔치면서 데미지를 준다
	if( pSkillProp->dwID == SKILL_INDEX( 203, SI_ACR_YOYO_SNITCH ) )
	{
		// pSrc - 공격자 플레이어, this - 피공격자 몬스터 
		// 피공격자 잔액이 0이 아니면 강탈 한다.
		FLWSUser* pUser = (FLWSUser*)pUseCtrl;
		FLWSUser* pDest = (FLWSUser*)pTarget;
		if( pDest->GetType() == OT_MOVER && pDest->GetGold() > 0 )
		{
			int nPlus = (pDest->GetLevel()*20) - (pUser->GetLevel()*10);

			if( pUser->GetLevel() <= 20 )
			{
			}
			else
			{
				if( pUser->GetLevel() <= 30 )
				{
					nPlus	= (int)( nPlus * 0.7f );
				}
				else
				{
					if( pUser->GetLevel() <= 40 )
					{
						nPlus	= (int)( nPlus * 0.5f );
					}
					else
					{
						nPlus	= (int)( nPlus * 0.3f );
					}
				}
			}

			if( nPlus > 0 )
			{				
				nPlus = min( nPlus, pDest->GetGold() );		// 피공격자 돈 보다는 많지 않게 
				pUser->AddGold( nPlus );
				pDest->AddGold( -nPlus );

				pUser->AddGoldText( nPlus );
			}
		}		
	}
#endif	//__WORLDSERVER
}


// ApplySkill 내부에서 데미지 주는부분만 빠짐.
int		FLSkillSystem::ApplyDamage( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) 
{
	int nDamage = 0;
	// 데미지류 스킬의 데미지 전달.
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_MELEEATK:		// 근접스킬데미지공격
	case EXT_MAGICATK:		// 즉시마법데미지공격
	case EXT_MAGICATKSHOT:
		{
			if( pSkillProp->dwID == SKILL_INDEX( 65, SI_MAG_FIRE_HOTAIR ) )	// 핫에어 땜빵처리.
			{
				nDamage = pTarget->SendDamage( AF_MAGICSKILL, pUseCtrl->GetId(), nParam, bTarget );
			}
			else
			{
				int nAtkFlag = 0;
				if( pSkillProp->dwSkillType == KT_MAGIC )
					nAtkFlag = AF_MAGICSKILL;
				else
					nAtkFlag = AF_MELEESKILL;

				if( pSkillProp->dwComboStyle == CT_FINISH )
					nDamage = pTarget->SendDamageForce( nAtkFlag, pUseCtrl->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
				else
					nDamage = pTarget->SendDamage( nAtkFlag, pUseCtrl->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
			}
		}
		break;
	}

	return nDamage;
}

//
// ApplySkill 내부에서 스킬의 하드코딩 처리부.
// this : Target
// pSrc : 시전자.
//
BOOL	FLSkillSystem::ApplySkillHardCoding( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __WORLDSERVER
	switch( pSkillProp->dwID )
	{
	case SKILL_INDEX( 45, SI_ASS_HEAL_RESURRECTION ):		// 부활	
		if( pTarget->GetType() == OT_MOVER )
		{
			CMover *pTargetMover = (CMover *)pTarget;
			if( pTargetMover->IsPlayer() && pTargetMover->IsDie() )	// 플레이어이고 타겟이 죽어있을때
			{
				if( pTargetMover->m_pActMover->GetDmgState() == OBJSTA_RESURRECTION )
				{
					return FALSE;		// 이미 부활중인 사람에게 쓴거라면 취소.
				}
				else
				{
					g_dpDBClient.SendLogLevelUp( pTargetMover, 10 );	// 부활 스킬 로그

					if( ((FLWSUser *)pTarget)->m_Resurrection_Data.bUseing != TRUE )
					{
						((FLWSUser *)pTarget)->AddRevivalBySkillMessage();

						((FLWSUser *)pTarget)->m_Resurrection_Data.bUseing        = TRUE;
						((FLWSUser *)pTarget)->m_Resurrection_Data.dwPlayerID     = ((CMover*)pUseCtrl)->m_idPlayer;
						((FLWSUser *)pTarget)->m_Resurrection_Data.pSkillProp     = pSkillProp;
						((FLWSUser *)pTarget)->m_Resurrection_Data.pAddSkillProp  = pAddSkillProp;
					}
				}
			}
		}
		break;
	}
#endif // WorldServer
	return TRUE;
}

//
// this에게 파라메터를 적용한다.
// SetDestParam을 일괄적으로 하는 부분.
// pSrc는 시전자.
BOOL	FLSkillSystem::ApplyParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend, int nDamage )
{
	if( pUseCtrl == NULL )		return FALSE;

	int		i;
	CMover *pTargetMover = (CMover *)pTarget;

	DWORD	dwDestParam, dwSkillTime, dwPainTime;
	int		nAdjParam, nChgParam;
	int		nDestData[3];

	for( i = 0; i < MAX_SKILL_PARAM; i ++ )
	{
		if( DoActiveParam( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, i, bSend, nDamage ) == TRUE )
		{
			continue;
		}

		if( pAddSkillProp )
		{
			dwDestParam = pAddSkillProp->dwDestParam[i];
			dwSkillTime = pAddSkillProp->dwSkillTime;
			dwPainTime = pAddSkillProp->dwPainTime;

			nAdjParam	= pAddSkillProp->dwAdjParamVal[i];
			nChgParam	= (int)pAddSkillProp->dwChgParamVal[i];
			nDestData[0] = pAddSkillProp->arrAddSkillDestNumData1[0];
			nDestData[1] = pAddSkillProp->arrAddSkillDestNumData1[1];
			nDestData[2] = pAddSkillProp->arrAddSkillDestNumData1[2];
		}
		else
		{	
			dwDestParam = pSkillProp->dwDestParam[i];
			dwSkillTime = pSkillProp->dwSkillTime;
			dwPainTime  = pSkillProp->tmContinuousPain;
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
			if( nAdjParam & CHS_INVISIBILITY )	// 다크일루션상태로...
			{
				if( pTargetMover->SetDarkCover( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				pTargetMover->m_wDarkCover = 0;		
			}
			else if( nAdjParam & CHS_STUN )	// 스턴상태로...
			{
				if( pTargetMover->SetStun( TRUE, dwSkillTime ) == FALSE )
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
				pTargetMover->m_wStunCnt = 0;		
			}
			else if( nAdjParam & CHS_POISON )	// 독상태로..
			{
				if( pTargetMover->SetPoison( TRUE, pUseCtrl->GetId(), dwSkillTime, dwPainTime, static_cast< short >( nDestData[0] ) ) == FALSE )	// nDestData[0] 틱당 들어가는 데미지
					return FALSE;
				pTargetMover->m_wPoisonCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_DARK )	// 암흑상태
			{
				if( pTargetMover->SetDark( TRUE, dwSkillTime, nDestData[1] ) == FALSE )		// nDestData1[1] : 명중률 수정치.
					return FALSE;
				pTargetMover->m_wDarkCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_BLEEDING )	// 출혈상태로..
			{
				if( nDestData[0] < 0 )
					nDestData[0]	= nDamage / 10;
				if( pTargetMover->SetBleeding( TRUE, pUseCtrl->GetId(), dwSkillTime, dwPainTime, static_cast< short >( nDestData[0] ) ) == FALSE )
					return FALSE;
				pTargetMover->m_wBleedingCnt = 0;		// 스턴이랑 같은 이유
			}
			else if( nAdjParam & CHS_SLEEPING )	// 슬리핑상태로...
			{
				if( pTargetMover->SetSleeping( true, dwSkillTime ) == false )
				{
					return FALSE;
				}
				pTargetMover->SetSleepingDuration( 0 );
			}
			else
			{
				pTargetMover->SetDestParam( dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE );
			}
			break;

		case DST_CURECHR:		// 상태 해제
			pTargetMover->RemoveDebuff( nAdjParam );
			break;

		case DST_CLEARBUFF:		// 대상의 버프를 파괴.
			if( nAdjParam == NULL_ID )
			{
				pTargetMover->RemoveBuffAll();
			}
			else
			{
				for( int i = 0; i < nAdjParam; i ++ )
					pTargetMover->RemoveBuffOne();
			}
			break;
		case DST_CHR_CHANCESTUN:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEPOISON:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEDARK:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEBLEEDING:// adj:확률		data1:스킬
		case DST_CHR_CHANCESTEALHP:	// adj:확률		data1:스킬
			pTargetMover->SetDestParam( dwDestParam, nAdjParam, nChgParam, TRUE );
			break;

		//////////////////////////////////////////////////////////////////////////
		case DST_DEBUFF_ALL_CLEAR:
			pTargetMover->RemoveDebufBuffs();
			break;
		//////////////////////////////////////////////////////////////////////////

		default: // 그외 DST_는 일반적인 루틴을 탄다.
			// 일반 DST_처리
			pTargetMover->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
			break;
		}
	}

	return TRUE;
}

BOOL	FLSkillSystem::ApplyActiveParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend, int nDamage )
{
	if( pUseCtrl == NULL )		return FALSE;

	for( int i = 0; i < MAX_SKILL_PARAM; i ++ )
	{
		DoActiveParam( pUseCtrl, pTarget, pSkillProp, pAddSkillProp, i, bSend, nDamage );
	}

	return TRUE;
}

BOOL	FLSkillSystem::DoActiveParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, int nIndex, BOOL /*bSend*/, int nDamage )
{
	CMover *pTargetMover = (CMover *)pTarget;

	DWORD	dwDestParam, dwSkillTime, dwPainTime;
	int		nAdjParam, nChgParam;
	int		nDestData[3];

	if( pAddSkillProp )
	{
		dwDestParam = pAddSkillProp->dwDestParam[nIndex];
		dwSkillTime = pAddSkillProp->dwSkillTime;
		dwPainTime = pAddSkillProp->dwPainTime;

		nAdjParam	= pAddSkillProp->dwAdjParamVal[nIndex];
		nChgParam	= (int)pAddSkillProp->dwChgParamVal[nIndex];
		nDestData[0] = pAddSkillProp->arrAddSkillDestNumData1[0];
		nDestData[1] = pAddSkillProp->arrAddSkillDestNumData1[1];
		nDestData[2] = pAddSkillProp->arrAddSkillDestNumData1[2];
	}
	else
	{	
		dwDestParam = pSkillProp->dwDestParam[nIndex];
		dwSkillTime = pSkillProp->dwSkillTime;
		dwPainTime  = pSkillProp->tmContinuousPain;
		nAdjParam	= pSkillProp->nAdjParamVal[nIndex];
		nChgParam	= (int)pSkillProp->dwChgParamVal[nIndex];
		nDestData[0] = pSkillProp->nDestData1[0];
		nDestData[1] = pSkillProp->nDestData1[1];
		nDestData[2] = pSkillProp->nDestData1[2];
	}
	// destParam 1
	switch( dwDestParam )
	{
	case NULL_ID:
		break;
	case DST_CHR_STEALHP: 			// HP흡수.  흡혈은 DST_로 하기가 좀 안좋다.
		{
			int nStealHP = (int)( nDamage * (nAdjParam / 100.0f) );	// 실제가격한 데미지에서 흡수할양을 정함.
			if( pUseCtrl->GetType() == OT_MOVER )
				((CMover *)pUseCtrl)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );		// 시전자 HP로 흡수.
		}
		break;
	case DST_CHR_STEALHP_IMM: 			// 지정된양의 HP흡수.  
		{
			if( pTargetMover->GetType() == OT_MOVER )
			{
				int nHP = pTargetMover->GetHitPoint();		// 방어자의 hp
				int nStealHP = nAdjParam;	// 흡수해야할 hp
				if( nHP < nStealHP )	// 피를 뽑아와야하는데 남은피가 더 적으면.
					nStealHP = nHP;		// 남은피만큼 뽑아옴
				//SetDestParam( DST_HP, -nStealHP, 0 );		// 피를 뺏김.
				if( pUseCtrl->GetType() == OT_MOVER )
				{
					//   						((CMover*)this)->m_pActMover->SendDamage( AF_FORCE,		// 타겟은 데미지입고
					//   													      GetId(), 
					//  														  nStealHP );
					// chipi_080303 버그 수정 - 몬스터가 자기 자신을 타겟으로 설정하는 문제
					pTargetMover->m_pActMover->SendDamage( AF_FORCE,		// 타겟은 데미지입고
						((CMover*)pUseCtrl)->GetId(), 
						nStealHP );


					((CMover *)pUseCtrl)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM, TRUE  );		// 공격자 HP로 흡수.
				}
			}
		}
		break;

#ifdef __WORLDSERVER
	case DST_HP:	// 힐링.
		if( pAddSkillProp )
		{
			if( pSkillProp->dwReferTarget1 == RT_HEAL || pSkillProp->dwReferTarget2 == RT_HEAL )
			{
				DWORD dwHp1 = 0, dwHp2 = 0;

				if( pUseCtrl->GetType() == OT_MOVER )	// 시전자가 사람일때만 스탯에 의한 영향
				{
					if( pSkillProp->dwReferTarget1 == RT_HEAL )
					{
						switch( pSkillProp->dwReferStat1 )
						{
						case DST_STA:		dwHp1 = ((CMover *)pUseCtrl)->GetSta();			break;
						case DST_DEX:		dwHp1 = ((CMover *)pUseCtrl)->GetDex();			break;
						case DST_INT:		dwHp1 = ((CMover *)pUseCtrl)->GetInt();			break;
						}
						dwHp1 = (DWORD)( ((pSkillProp->dwReferValue1 / 10.0f) * dwHp1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp1 / 50.0f)) );
					}

					if( pSkillProp->dwReferTarget2 == RT_HEAL )
					{
						switch( pSkillProp->dwReferStat2 )
						{
						case DST_STA:		dwHp2 = ((CMover *)pUseCtrl)->GetSta();			break;
						case DST_DEX:		dwHp2 = ((CMover *)pUseCtrl)->GetDex();			break;
						case DST_INT:		dwHp2 = ((CMover *)pUseCtrl)->GetInt();			break;
						}
						dwHp2 = (DWORD)( ((pSkillProp->dwReferValue2 / 10.0f) * dwHp2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp2 / 50.0f)) );
					}
				}

				// 증가될 HP량.
				int nIncHP = pAddSkillProp->dwAdjParamVal[0] + dwHp1 + dwHp2;
				ApplyHeal( (CMover *)pUseCtrl, pTargetMover, nIncHP, pAddSkillProp->dwChgParamVal[0], pSkillProp->dwExeTarget );
			}
		}
		break;

	case DST_FORCE_DAMAGE_MAX_HP_RATE:	// 최대 HP의 x%만큼 데미지를 입힌다. AdjParam 비율(최대10000%)
		{
			int nNeedDamage = static_cast< int >( pTargetMover->GetMaxHitPoint() * (nAdjParam/10000.) );
			if( nNeedDamage > 0 )
			{
				pTargetMover->m_pActMover->SendDamage( AF_FORCE, pUseCtrl->GetId(), nNeedDamage );
			}
		}
		break;

	case DST_FORCE_DAMAGE_SAFE_HP_RATE:	// 최대 HP의 x%의 HP가 남게 데미지를 입힌다. AdjParam 비율(최대10000%)
		{
			int nRemainHP = static_cast< int >( pTargetMover->GetMaxHitPoint() * (nAdjParam/10000.) );
			int nNeedDamage = pTargetMover->GetHitPoint() - nRemainHP;
			if( nNeedDamage > 0 )
			{
				pTargetMover->m_pActMover->SendDamage( AF_FORCE, pUseCtrl->GetId(), nNeedDamage );
			}
		}
		break;

	case DST_FORCE_DAMAGE_VALUE:	// 절대치 데미지 주기 AdjParam(데미지)
		{
			pTargetMover->m_pActMover->SendDamage( AF_FORCE, pUseCtrl->GetId(), nAdjParam );
		}
		break;

	case DST_HEAL_HP_RATE:	// HP 비율 회복 AdjParam(최대 10000%)
		{
			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxHitPoint() > pTargetMover->GetHitPoint() )
				{
					int nIncHP = static_cast< int >( pTargetMover->GetMaxHitPoint() * (nAdjParam/10000.) );
					if( nIncHP <= 0 )
					{
						nIncHP = 1;
					}

					ApplyHeal( pSrcMover, pTargetMover, nIncHP, NULL_CHGPARAM, pSkillProp->dwExeTarget );
				}
			}
		}
		break;

	case DST_HEAL_MP_RATE:	// MP 비율 회복 AdjParam(최대 10000%)
		{
//			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxManaPoint() > pTargetMover->GetManaPoint() )
				{
					int nIncMP = static_cast< int >( pTargetMover->GetMaxManaPoint() * (nAdjParam/10000.) );
					if( nIncMP <= 0 )
					{
						nIncMP = 1;
					}

					pTargetMover->SetDestParam( DST_MP, nIncMP, NULL_CHGPARAM, TRUE );
				}
			}
		}
		break;

	case DST_HEAL_FP_RATE:	// FP 비율 회복 AdjParam(최대 10000%)
		{
//			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxFatiguePoint() > pTargetMover->GetFatiguePoint() )
				{
					int nIncFP = static_cast< int >( pTargetMover->GetMaxFatiguePoint() * (nAdjParam/10000.) );
					if( nIncFP <= 0 )
					{
						nIncFP = 1;
					}

					pTargetMover->SetDestParam( DST_FP, nIncFP, NULL_CHGPARAM, TRUE );
				}
			}
		}
		break;
	case DST_HEAL_HP_VALUE:		// HP 절대치 회복 AdjParam
		{
			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxHitPoint() > pTargetMover->GetHitPoint() )
				{
					ApplyHeal( pSrcMover, pTargetMover, nAdjParam, NULL_CHGPARAM, pSkillProp->dwExeTarget );
				}
			}
		}
		break;
	case DST_HEAL_MP_VALUE:		// MP 절대치 회복 AdjParam
		{
//			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxManaPoint() > pTargetMover->GetManaPoint() )
				{
					pTargetMover->SetDestParam( DST_MP, nAdjParam, NULL_CHGPARAM, TRUE );
				}
			}
		}
		break;
	case DST_HEAL_FP_VALUE:		// FP 절대치 회복 AdjParam
		{
//			CMover* pSrcMover = (CMover*) pUseCtrl;
			if( pAddSkillProp )
			{
				if( pTargetMover->GetMaxFatiguePoint() > pTargetMover->GetFatiguePoint() )
				{
					pTargetMover->SetDestParam( DST_FP, nAdjParam, NULL_CHGPARAM, TRUE );
				}
			}
		}
		break;
#endif
	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

bool	FLSkillSystem::CanApplyBuff( CMover* pSrcMover, CMover* pTargetMover, SkillProp* pSkillProp )
{
	if( pSrcMover == NULL || pTargetMover == NULL || pSkillProp == NULL )
	{
		return false;
	}

	if( pSrcMover->GetType() != OT_MOVER || pTargetMover->GetType() != OT_MOVER )
	{
		return false;
	}

	MoverProp* pTargetProp	= pTargetMover->GetProp();
	if( pTargetProp == NULL )
	{
		return false;
	}

	if( pSkillProp->IsDebuff() == TRUE )
	{
		// 무적이면 모든 디버프 걸리지 않음.
		if( pTargetMover->m_dwMode & MATCHLESS_MODE )
		{
			return false;
		}

		// 몬스터에게 디버프 걸땐 등급 체크.
		if( pTargetMover->IsPlayer() == FALSE && pTargetProp->dwClass > pSkillProp->dwAdjMonsterGrade )
		{
			return false;
		}
	}
	else
	{
		// 몬스터에게 좋은 버프는 걸리지 않음.
		if( pSrcMover->IsPlayer() == TRUE && pTargetMover->IsPlayer() == FALSE )
		{
			return false;
		}
	}

	return true;
}

#ifdef __WORLDSERVER
void	FLSkillSystem::ApplyHeal( CMover* pHealer, CMover* pTarget, int nHealValue, DWORD dwChgParamVal, DWORD dwExeTarget )
{
	if( nHealValue > 0 )
	{
		if( pHealer->GetType() == OT_MOVER )
		{
			if( pHealer->IsPlayer() )	// 시전자가 플레이어일때
			{
				if( pTarget->GetMaxHitPoint() < pTarget->GetHitPoint() + nHealValue )		// 타겟이 오버힐 됐냐?
					((FLWSUser *)pHealer)->m_nOverHeal = PROCESS_COUNT * 30;				// 시전자는 30초간 오버힐 상태. 
			}
		}
	}

#ifdef __AGGRO16
	FLAggro::SumHealAggro( *static_cast< CMover * >( pHealer ), *pTarget, ( pTarget->GetHitPoint() + nHealValue ) - pTarget->GetMaxHitPoint(), dwExeTarget );
#endif //__AGGRO16

	// 힐시작.
	pTarget->SetDestParam( DST_HP, nHealValue, dwChgParamVal, TRUE );
}
#endif


#ifdef __CLIENT
//
// 스킬을 썼을때 클라에서 생성해야할 sfx 처리
//
void	FLSkillSystem::CreateSkillSfx( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	//sun skill here
	{
		// 발사체 형태가 아니다.
		CSfx *pSfx = NULL;
		if( (int)pAddSkillProp->dwSkillTime > 0 )	// 지속시간이 있는 스킬은
		{
			if( pSkillProp->dwSfxObj4 == NULL_ID )		// 이펙이 유지되는 스킬이 아닐경우에만.
			{
				if( pSkillProp->dwExeTarget == EXT_MELEEATK )
				{
					D3DXVECTOR3 vLocal;

					if( pSkillProp->dwNeedItemKind3 == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ( &vLocal, pUseCtrl->GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += pUseCtrl->GetPos();
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, pUseCtrl->GetPos(), pUseCtrl->GetId(), vLocal, pTarget->GetId(), 0 );

					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, pUseCtrl->GetPos(), pUseCtrl->GetId(), pUseCtrl->GetPos(), pUseCtrl->GetId(), 0 );  // 5번 이펙트를 출력함.
				} else
				{
					if( pSkillProp->dwSfxObj3 != NULL_ID )		// 타겟에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, pUseCtrl->GetPos(), pUseCtrl->GetId(), pTarget->GetPos(), pTarget->GetId(), 0 );  // 3번 이펙트를 출력함.
					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, pUseCtrl->GetPos(), pUseCtrl->GetId(), pUseCtrl->GetPos(), pUseCtrl->GetId(), 0 );  // 5번 이펙트를 출력함.
				}

				if( pSkillProp->dwNeedItemKind3 == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
					CreateYoyoSkill( pUseCtrl, pSfx, pTarget, pSkillProp, pAddSkillProp );

			} else
			{
				if( pUseCtrl->GetType() == OT_MOVER )
				{
					//gmpbigsun( 20100824 ) : CreateSfx의 Return값을 받아서 사운드 출력이 가능하게 함.
					if( pSkillProp->dwSkillType == NULL_ID )	// NULL_ID면 아이템들
					{
						if( ((CMover *)this)->HasBuff( BUFF_ITEM, (WORD)( pSkillProp->dwID ) ) == FALSE )	// 이미 실행되고 있지 않을때만 이펙 출력
							pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, pUseCtrl->GetPos(), pUseCtrl->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
					} else
					{
						// 이미 실행되고 있지 않을때만 이펙 출력
						//						BOOL bRst = ((CMover *)this)->HasBuff( BUFF_SKILL, (WORD)( pSkillProp->dwID ) );

						//gmpbigsun(20100826) : 버프형태는 따로 IBuff::Process에서 sfx를 생성하므로 여기서 sfx를 생성할 필요가 없다.
						//if( ((CMover *)this)->HasBuff( BUFF_SKILL, (WORD)( pSkillProp->dwID ) ) == FALSE
						//	&& pSkillProp->dwID != SI_MER_SHIELD_PROTECTION && pSkillProp->dwID != SI_MER_SHIELD_PANBARRIER )	// 091022 mirchang - 여기서 먼저 생성 후 IBuff.Process에서 sfx를 다시 생성하는 문제 발생. 땜빵.
						//	pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );

						//gmpbigsun(20100920, #0010102) : 일부스킬 유지이펙트 안나온다는 현상
						//아니 이게 버픈지 그냥 유지인지 알수있는 방법이 없나??
						//일단 hardly
						if( pSkillProp->dwID == 170 || pSkillProp->dwID == 151 || pSkillProp->dwID == 225 )
							pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, pUseCtrl->GetPos(), pUseCtrl->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );

						D3DXVECTOR3 vPos = pUseCtrl->GetPos();
						PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
					}
				}
			}
		} else
		{	// 지속시간 없는 스킬
			if( pSkillProp->dwExeTarget == EXT_MELEEATK )	// 근접공격 스킬의 타격 이펙트.
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )	
				{
					D3DXVECTOR3 vLocal;

					if( pSkillProp->dwNeedItemKind3 == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ( &vLocal, pUseCtrl->GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += pUseCtrl->GetPos();		// GetPos()를 this에서 pTarget으로 바꿨다. 2006/6/20 xuzhu.
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, vLocal, pUseCtrl->GetId(), vLocal, pTarget->GetId(), 0 );	// 2006/6/20 xuzhu 가 바꿈.
				}
			} else
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )
				{
					D3DXVECTOR3 vPos = pTarget->GetPos();
					vPos.y += 1.0f;
					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, pUseCtrl->GetPos(), pUseCtrl->GetId(), vPos, pTarget->GetId(), 0 );
				}
			}

			if( pSkillProp->dwNeedItemKind3 == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
			{
				CreateYoyoSkill( pUseCtrl, pSfx, pTarget, pSkillProp, pAddSkillProp );
			}
			else
				if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, pUseCtrl->GetPos(), pUseCtrl->GetId(), pUseCtrl->GetPos(), pUseCtrl->GetId(), 0 );  // 5번 이펙트는 시전자 자신에게 출력
		}
		if( pSfx )
		{
			D3DXVECTOR3 vPos = pUseCtrl->GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
		}

		//dwSkillRange
		if( pSkillProp->dwID == SKILL_INDEX( 187, SI_GEN_FLASH ) && pSfx )
		{
			FLOAT fRange = 1.5f;
			pSfx->SetScale(  D3DXVECTOR3(fRange, fRange, fRange) );
		}
	}
} // CreateSkillSfx


void	FLSkillSystem::CreateYoyoSkill( CCtrl* pUseCtrl, CSfx* pSfx, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	PT_ITEM_SPEC pItemProp = ((CMover *)pUseCtrl)->GetActiveHandItemProp();

	if(pItemProp)
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vLocal;
		D3DXVECTOR3 vPosSrc   = pUseCtrl->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
		D3DXVECTOR3 vPosDest  = pTarget->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 목표 지점을 임의로 올려준다. 땜빵 

		CModelObject *pModel = (CModelObject *)pUseCtrl->GetModel();

		int nSkill = pSkillProp->dwID;

		switch( nSkill )
		{
		case SKILL_INDEX( 197, SI_ACR_SUP_SLOWSTEP ):
		case SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ):
		case SKILL_INDEX( 214, SI_JST_YOYO_VATALSTAB ):
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, pUseCtrl->GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, pUseCtrl->GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);
			}
			break;
		case SKILL_INDEX( 205, SI_ACR_YOYO_DEADLYSWING ):
		case SKILL_INDEX( 201, SI_ACR_YOYO_CROSSLINE ):
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, pUseCtrl->GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, pUseCtrl->GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);

				pModel->GetHandPos( &vPos, PARTS_LWEAPON, pUseCtrl->GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, pUseCtrl->GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
			}
			break;							
		}
	}

}

#endif // Client