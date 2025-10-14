#include "stdafx.h"
#include "Ship.h"
#include "ModelObject.h"
#include "CreateObj.h"
//#include "ModelGlobal.h"


#ifdef __CLIENT
	#include "AppDefine.h"
	#include "defineSound.h"
	#include "Sfx.h"
	#include "../Neuz/DPClient.h"
	#include "..\_Common\ParticleMng.h"
#endif	// __CLIENT
#ifdef	__WORLDSERVER
	#include "../WorldServer/User.h"
	#include "WorldMng.h"
	#include "..\_AIInterface\AIInterface.h"
	#include "Party.h"
#endif	// __WORLDSERVER

#ifdef __EVENT_MONSTER
	#include "EventMonster.h"
#endif // __EVENT_MONSTER

#include "FLSkillSystem.h"


#ifdef __CLIENT
	extern	CDPClient			g_DPlay;
#else	// __CLIENT
	extern	CWorldMng			g_WorldMng;
	extern	CPartyMng			g_PartyMng;
#endif	// not __CLIENT

BOOL IsLootSendText( PT_ITEM_SPEC pItemProp )
{
	if( pItemProp == NULL )
		return FALSE;

	if( pItemProp->dwItemKind1 == IK1_GOLD    ||
		pItemProp->dwItemKind1 == IK1_GENERAL ||
		pItemProp->dwItemKind1 == IK1_SYSTEM  ||
		pItemProp->dwItemKind1 == IK1_RIDE        )
		return FALSE;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------------------


											Action Event Handler
										   ----------------------
						CActionMover내에서 동작에 관련된 이벤트가 발생하면 호출되는 함수군


----------------------------------------------------------------------------------------------------------*/


// RT_TIME을 쓰는 스킬들의 스킬 시간을 계산.
void	CMover::SubReferTime( DWORD *pTime1, DWORD *pTime2, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp )
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
		
		dwTime1 = (DWORD)( ((pSkillProp->dwReferValue1/10.0f)*dwTime1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime1/50.0f)) );
	}
	
	if( pSkillProp->dwReferTarget2 == RT_TIME )
	{
		switch( pSkillProp->dwReferStat2 )
		{
		case DST_STA:	dwTime2 = GetSta();		break;
		case DST_DEX:	dwTime2 = GetDex();		break;
		case DST_INT:	dwTime2 = GetInt();		break;
		}
		
		dwTime2 = (DWORD)( ((pSkillProp->dwReferValue2/10.0f)*dwTime2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime2/50.0f)) );
	}

	*pTime1 = dwTime1;
	*pTime2 = dwTime2;
}

BOOL	CMover::GetSkillProp( SkillProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	char szStr[256] = {0, };
	FLSPrintf( szStr, _countof( szStr ), "%s-%s", m_szName, szErr );

	return CCtrl::GetSkillProp( ppSkillProp, ppAddSkillProp, nSkill, dwLevel, szStr );
}


// 쿨타임 시작!
//sun: 9, 대인용 AddSkillProp 컬럼 추가 및 적용
void	CMover::SetCoolTime( AddSkillProp* pAddSkillProp, LPCTSTR /*szCall*/ )
{
	int nIdx	= GetSkillIdx( pAddSkillProp->dwSkillPropID );		// 스킬리스트 인덱스를 찾음.
	
	if( nIdx < 0 || nIdx >= MAX_SKILL_JOB )
		FLERROR_LOG( PROGRAM_NAME, _T( "szCall SetCoolTime : %d %d스킬을 찾을 수 없음 %s" ), nIdx, pAddSkillProp->dwSkillPropID, GetName() );
	else
		m_tmReUseDelay[ nIdx ] = pAddSkillProp->dwCooldown + timeGetTime();		// 1/1000단위
}


//
//	장거리무기가 발사되는 시점에 발생하는 이벤트
//
BOOL	CMover::OnAttackRange()
{
//  	if( GetAct() != OBJACT_RANGE_ATTACK ) {
//  		return FALSE;
//  	}

	int		idTarget = GetActParam( 0 );
	DWORD	dwItemID = GetActParam( 1 );		// 사용할 아이템 아이디 (player이면 충전레벨)
	int		idSfxHit = GetActParam( 2 );		// 몬스터가 사용한것이면 여기에 서버로부터 받은값이 들어있다.
	UNREFERENCED_PARAMETER( idSfxHit );

	BOOL	bControl = FALSE;					//sun: 8, // __S8_PK

	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj(pHit) )	
		return FALSE;							// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.
	
	PT_ITEM_SPEC pItemProp = NULL;
	if( IsPlayer() )
		pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else 
	{
		if( dwItemID == NULL_ID || dwItemID == 0 )	
			pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
		else
			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	}

	if( pItemProp == NULL )
		return FALSE;

#ifdef __WORLDSERVER
//sun: 12, // __MONSTER_SKILL
	if( IsNPC() )
		CMonsterSkill::GetInstance()->ApplySkill( this, pHit, ATK_RANGE );
#endif // __WORLDSERVER

	DWORD dwSfxObj = pItemProp->dwSfxObj2;
	
	D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
	D3DXVECTOR3 vPosSrc  = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

#ifdef __CLIENT		
	CSfx* pSfx = NULL;
#endif
	// 아크로뱃은 이펙트 출력이 항상 왼손에...
	if( IsPlayer() )		
	{
		PT_ITEM_SPEC pItemElem = GetActiveHandItemProp( PARTS_BULLET );  // 화살아이템을 참조로 화살이펙트 생성
		if( pItemElem && pItemElem->dwSfxObj2 != NULL_ID )
		{
		#ifdef __CLIENT
			if( IsActiveMover() )
				pSfx = CreateSfxArrow( pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
		#endif //__CLIENT

		#ifdef __WORLDSERVER
			g_xWSUserManager->AddCreateSfxAllow( this, pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
		#endif //__WORLDSERVER
		}
	}
	else
	{
		if( dwSfxObj == NULL_ID )	
			return FALSE;
	#ifdef __CLIENT		
		pSfx = CreateSfx( D3DDEVICE, dwSfxObj, vPosSrc,GetId(), vPosDest ,idTarget );
	#endif
	}

	DWORD dwAtkFlags = GetAct() == OBJACT_RANGE_ATTACK ? GetActParam( 3 ) : AF_MISS;
	if( IsPlayer() )
		dwAtkFlags |= AF_RANGE;

#ifdef __WORLDSERVER
	OnAttackSFX( pHit->GetId(), ( IsPlayer()? dwItemID: 0 ), NULL_ID, 0, 0, 0, dwAtkFlags, bControl );

#endif	// __WORLDSERVER

#ifdef __CLIENT
	if( pSfx )
	{
		if( IsActiveMover() )		
		{
			idSfxHit = ++m_idSfxHit; 	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, dwAtkFlags );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = IsPlayer() ? dwItemID : 0;		// 충전레벨도 넣어준다.
		}
		// 쏜놈이 g_pPlayer거나 / 쏜놈은 몹이고 맞는놈이 g_pPlayer면
		if( IsActiveMover() || (IsNPC() && pHit->IsActiveMover()) )
			pSfx->m_idSfxHit	= idSfxHit;		// id를 등록함
	}
#endif //__CLIENT
	return TRUE;
}


//
//	완드(매직)어택이 발사되는 시점에 발생하는 이벤트.
//	
BOOL	CMover::OnAttackMagic()
{
//  	if( GetAct() != OBJACT_MAGIC_ATTACK ) {
//  		return FALSE;
//  	}

	int		idTarget = GetActParam( 0 );
	int		nMagicPower = GetActParam( 1 );

	BOOL	bControl = GetActParam( 4 );		//sun: 8, // __S8_PK

	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.

	if( IsInvalidObj(pHit) )	
		return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.

#ifdef __WORLDSERVER
	const DWORD attackFlag = GetAct() == OBJACT_MAGIC_ATTACK ? GetActParam( 3 ) : AF_MISS;
	OnAttackSFX( pHit->GetId(), nMagicPower, NULL_ID, 0, 0, 0, attackFlag, bControl );

#else	// __WORLDSERVER
	DWORD dwWeaponId;

	if( IsActiveMover() ) 
	{
		FLItemElem* pWeapon = GetWeaponItem();
		if( pWeapon == NULL )	return FALSE;
		dwWeaponId	= pWeapon->m_dwItemId;
	}
	else 
	{
		dwWeaponId	= m_aEquipInfo[PARTS_RWEAPON].dwItemID;
		if( dwWeaponId == 0 )	return FALSE;
	}

	D3DXVECTOR3 vPosDest = pHit->GetPos(); 
	// 발사 목표지점은 상대의 중앙.
	float fHeight = fabs(pHit->m_pModel->m_vMax.y) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
	fHeight *= pHit->GetScale().x;
	vPosDest.y += fHeight;	

	D3DXVECTOR3 vPosSrc = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

	PT_ITEM_SPEC pHandItemProp = GetActiveHandItemProp( PARTS_RWEAPON );
	CSfx* pSfx = NULL;
	if( m_pActMover->IsFly() )
	{
		pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 28, XI_FIR_WAND02 ), vPosSrc, GetId(), vPosDest, idTarget );
		if( pSfx )
			pSfx->ShootSfx( GetAngle(), -GetAngleX(), 0.75f );		 
	} 
	else
	{
		DWORD dwSfxObj2 = XI_INDEX( 27, XI_FIR_WAND01 );
		if( pHandItemProp )
			dwSfxObj2 = pHandItemProp->dwSfxObj2;

		pSfx = CreateSfx( D3DDEVICE, dwSfxObj2, vPosSrc,GetId(), vPosDest ,idTarget );
		if( pSfx )
		{
			switch(nMagicPower) 
			{
			case 1:		pSfx->SetScale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );	break;
			case 2:		pSfx->SetScale( D3DXVECTOR3( 1.7f, 1.7f, 1.7f ) );	break;
			case 3:
			case 4:		pSfx->SetScale( D3DXVECTOR3( 2.5f, 2.5f, 2.5f ) );	break;
			}
			((CSfxItemWandAtk1 *)pSfx)->m_fCenter = fHeight;		
		}
	}

	if( pSfx )
	{
		if( IsActiveMover() )		// g_pPlayer만 아래짓을 하게 한다.
		{
			int idSfxHit = ++m_idSfxHit;			// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, AF_MAGIC );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = nMagicPower;		// 충전레벨도 넣어준다.
		}
	}


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
#endif // __WORLDSERVER
	return TRUE;
}

void	CMover::OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
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

			OnDamage( nItemID, dwState, NULL, &vPos );		// 이펙트가 발생하는 위치에 데미지를 주고
#ifdef __CLIENT
			GetWorld()->m_pCamera->SetQuake( 15, 0.5f );
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1615, XI_NPCSP1DIRFIRESP ), vPos );	// 이펙트를 발생 - 서버에선 생성 안됨.
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
#ifdef __CLIENT
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1613, XI_NPCSP1DIRCANNON ), vPos, GetId() );	// 발사이펙트.
			if( pSfx )
			{
				pSfx->SetAngle( -GetAngle() );
				pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
			}
#endif
			CMover *pTarget = pHitObj;
			if( IsValidObj(pTarget) )
			{
				OnDamage( nItemID, dwState, NULL, &pTarget->GetPos() );		// 타겟의 좌표를 중심으로 데미지를 줌.
#ifdef __CLIENT
				pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1616, XI_NPCSP1RANBALL ), vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1 );		// 포탄
				if( pSfx )
				{
					pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
					{
						FLOAT fAngXZ, fAngH;
						D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
						xGetDegree( &fAngXZ, &fAngH, vDist );		// 목표와의 각도구함.
						((CSfxAtkStraight *)pSfx)->ShootSfx( fAngXZ, -fAngH, 2.0f, XI_INDEX( 1617, XI_NPCSP1RANBOOM ) );		// 날아가는 방향 설정.
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

			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1614, XI_NPCSP1DIRCIRCLE ), vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
			pModel->GetEventPos( &vPos, 4 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

			pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1614, XI_NPCSP1DIRCIRCLE ), vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
#endif
		}
		break;
	}
}

void	CMover::OnAttackMelee_BigMuscle( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
	case OBJSTA_ATK2:
		{
			if( IsValidObj(pHitObj) )
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
		}
		break;
	case OBJSTA_ATK3:		// 땅치기 스킬 - 두손
		{
			D3DXMATRIX mMatrix;
			
			const D3DXMATRIX mWorld1 = GetMatrixWorld();
			D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(12)), &mWorld1/*&(GetMatrixWorld())*/ );
			
			D3DXVECTOR3 v2;
			v2.x = mMatrix._41;
			v2.y = mMatrix._42;
			v2.z = mMatrix._43;
			v2.y -= 5.0f;
			
		#ifdef __CLIENT
			// 땅치고 끝날때 이펙트 왼손
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1743, XI_SKILL_CIRCLE_DUST ), v2 );			
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );

			PLAYSND( SND_PC_SKILLD_HITOFPENYA );//, &v2 );
		#endif //__CLIENT 			
			
			const D3DXMATRIX mWorld2 = GetMatrixWorld();
			D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(16)), &mWorld2/*&(GetMatrixWorld())*/ );
			D3DXVECTOR3 v3;
			v3.x = mMatrix._41;
			v3.y = mMatrix._42;
			v3.z = mMatrix._43;						
			v3.y -= 5.0f;
			
		#ifdef __CLIENT
			// 땅치고 끝날때 이펙트 오른손
			pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1743, XI_INDEX( 1743, XI_SKILL_CIRCLE_DUST ) ), v3 );			
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );
		#endif //__CLIENT 			
		
			if( m_pActMover->m_nMotionHitCount == 1 )
			{				
	#ifdef __WORLDSERVER
				g_xApplyItemEffect->SendDamageAroundPos( &GetPos(), (AF_GENERIC | AF_CRITICAL) , this, OBJTYPE_PLAYER, ITEM_INDEX( 9262, II_WEA_MOB_MONSTER4_ATK2 ), 16.0f );					

				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 228, SI_GEN_BLEEDING ), 10, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							
	#endif //__WORLDSERVER
				
			}
			else
			{
	#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
	#endif //__CLIENT
	#ifdef __WORLDSERVER				
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 300, SI_BIGMUSCLE_STUNGROUP ), 1, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							
	#endif //__WORLDSERVER
			}
		}
		break;
	case OBJSTA_ATK4:   // 땅치기 스킬 - 한손
		{
			static D3DXVECTOR3 v3OldPos;

			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				v3OldPos = pHitObj->GetPos();			
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )
			{
				D3DXMATRIX mMatrix;
				const D3DXMATRIX mWorld = GetMatrixWorld();
				
				D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(12)), &mWorld/*&(GetMatrixWorld())*/ );
				
				D3DXVECTOR3 v2;
				v2.x = mMatrix._41;
				v2.y = mMatrix._42;
				v2.z = mMatrix._43;
				v2.y -= 5.0f;
				
			#ifdef __CLIENT
				// 땅치고 끝날때 이펙트 왼손
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1743, XI_SKILL_CIRCLE_DUST ), v2 );
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 9.0f, 9.0f, 9.0f ) );
				
				pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 246, XI_SKILL_MAG_EARTH_ROCKCRASH01 ), v3OldPos );
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
				
				PLAYSND( SND_PC_SKILLD_HITOFPENYA );
			#endif //__CLIENT

			#ifdef __WORLDSERVER
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 228, SI_GEN_BLEEDING ), 10, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					g_cSkillSystem->DoApplySkill( this, pHitObj, pSkillProp, pAddSkillProp );	
				
				g_xApplyItemEffect->SendDamageAroundPos( &(v3OldPos), AF_GENERIC | AF_CRITICAL , this, OBJTYPE_PLAYER, ITEM_INDEX( 9262, II_WEA_MOB_MONSTER4_ATK2 ), 4.0f );	
			#endif //__WORLDSERVER
				
					
			}
		}
		
		break;
	}
}

void	CMover::OnAttackMelee_Krrr( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
		{
	#ifdef __WORLDSERVER			
			OnDamage( nItemID, dwState, pHitObj );		
	#endif //__WORLDSERVER			
		}
		break;
	case OBJSTA_ATK2:	
		{
	#ifdef __WORLDSERVER
			int nRandom = xRandom(100);
			if( nRandom <= 50 )
			{
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 301, SI_KRRR_POISON ), 1, "OnSfxHit" );
				if( bSuccess )
				{
					g_cSkillSystem->ApplySkill( this, pHitObj, pSkillProp, pAddSkillProp );
				}
			}
			else
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK3:		// 두손으로 도끼 내려치기
		{
		#ifdef __CLIENT
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1615, XI_NPCSP1DIRFIRESP ), pHitObj->GetPos() );	// 이펙트를 발생 - 서버에선 생성 안됨.
			if( pSfx )
				pSfx->SetAngle( -GetAngle() );

			PLAYSND( SND_PC_BOSS_CRASH, &pHitObj->GetPos() );
		#endif //__CLIENT

		#ifdef __WORLDSERVER
			SkillProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 224, SI_GEN_KNOCK_BACK ), 4, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							

			g_xApplyItemEffect->SendDamageAroundPos( &(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, ITEM_INDEX( 9261, II_WEA_MOB_MONSTER4_ATK1 ), 7.0f );	
		#endif //__WORLDSERVER
		}
		break;
	case OBJSTA_ATK4:		// 주문을 외우고 플레이어들 한곳에 모인후 광역데미지 줌
		{
			static D3DXVECTOR3	  vLocal;
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
		#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_SUMMONS );
		#endif //__CLIENT
				
				AngleToVectorXZ( &vLocal, GetAngle(), 5.0f );	// 때리는방향 5미터앞.
				vLocal += GetPos();

				FLOAT fHeight = GetWorld()->GetLandHeight( vLocal.x, vLocal.z );
				vLocal.y = fHeight;
				
		#ifdef __WORLDSERVER
				CPartyQuestProcessor* pProc	= CPartyQuestProcessor::GetInstance();
				CWorld* pWorld	= GetWorld();
				if( pWorld )
				{
					int nId	= pProc->PtInQuestRect( pWorld->GetID(), GetPos() );
					if( nId > -1 )
					{
						PPARTYQUESTELEM pElem	= pProc->GetPartyQuest( nId );
						PPARTYQUESTRECT pRect	= pProc->GetPartyQuestRect( nId );
						if( pElem && pRect )
#ifdef __LAYER_1015
							g_xWSUserManager->ReplaceWorldArea( pElem->idParty, pRect->dwWorldId, pRect->dwWorldId, vLocal.x, vLocal.z, 0.5f, TRUE, GetLayer() );
#else	// __LAYER_1015
							g_xWSUserManager->ReplaceWorldArea( pElem->idParty, pRect->dwWorldId, pRect->dwWorldId, vLocal.x, vLocal.z, 0.5f, TRUE );
#endif	// __LAYER_1015
					}
				}
		#endif //__WORLDSERVER
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )		// 약 5초후에 스턴
			{	
		#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
		#endif //__CLIENT
#ifdef __WORLDSERVER
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 302, SI_KRRR_STUNGROUP ), 1, "OnAttackMelee_Krrr" );
				if( bSuccess )
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );	
#endif //__WORLDSERVER
				
			}
			else
			if( m_pActMover->m_nMotionHitCount == 2 )
			{	
#ifdef __CLIENT
				D3DXVECTOR3 v3Pos = vLocal;
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 434, XI_SKILL_ELE_MULTI_METEOSHOWER02 ), v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
				v3Pos.x += ( xRandomF( 12.0f ) + -6.0f );
				pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 434, XI_SKILL_ELE_MULTI_METEOSHOWER02 ), v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
				v3Pos = vLocal;
				v3Pos.z += ( xRandomF( 12.0f ) + -6.0f );
				pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 434, XI_SKILL_ELE_MULTI_METEOSHOWER02 ), v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
#endif //__CLIENT
				
			}
			else
			if( m_pActMover->m_nMotionHitCount == 3 )
			{	
		#ifdef __WORLDSERVER
				g_xApplyItemEffect->SendDamageAroundPos( &(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, ITEM_INDEX( 9262, II_WEA_MOB_MONSTER4_ATK2 ), 5.0f );	
		#endif //__WORLDSERVER
				
			}
		}
		break;
	}
}

//sun: 메테오니커 파이어 발사!!!
void	CMover::OnAttackMelee_Meteonyker( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:
		{
			if( IsValidObj(pHitObj) )
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
		}
		break;
	case OBJSTA_ATK3:		// 파이어볼 발사
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				CMover *pTarget = pHitObj;

				D3DXVECTOR3 vPos;
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetEventPos( &vPos, 0 );	// 용의 입 좌표.
				D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

				if( IsValidObj(pTarget) )
				{
					OnDamage( nItemID, dwState, pTarget, &pTarget->GetPos() );		// 타겟의 좌표를 중심으로 데미지를 줌.
					SkillProp *pSkillProp;
					AddSkillProp *pAddSkillProp;
					BOOL bFind	= GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 189, SI_GEN_STUNSINGLE ), 5, "SI_GEN_STUNSINGLE" );
					if( bFind )
					{
						if( xRand() % 2 )
							g_cSkillSystem->DoApplySkill( this, pTarget, pSkillProp, pAddSkillProp , false );
					}

	#ifdef __CLIENT
					CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1616, XI_NPCSP1RANBALL ), vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1 );		// 포탄
					if( pSfx )
					{
						pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
						{
							FLOAT fAngXZ, fAngH;
							D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
							xGetDegree( &fAngXZ, &fAngH, vDist );		// 목표와의 각도구함.
							((CSfxAtkStraight *)pSfx)->ShootSfx( fAngXZ, -fAngH, 2.0f, XI_INDEX( 1617, XI_NPCSP1RANBOOM ) );		// 날아가는 방향 설정.
						}
					}
	#endif	// __CLIENT
				}
			}
		}
		break;
	case OBJSTA_ATK4:   // 날개 접었다가 펼치기
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				// 광역 효과
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 308, SI_GEN_BURN ), 1, "OnAttackMelee_Meteonyker" );
				if( bSuccess )
				{
					D3DXVECTOR3 vPos	= GetPos();
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false, FALSE, 10.0F );
					MoverProp* pProp	= GetProp();
					OnDamage( pProp->dwAtk4, OBJSTA_ATK4, NULL, &vPos );
#ifdef __CLIENT
					CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1863, XI_NPCMETEONYKER ), vPos );
#endif	// __CLIENT
				}
			}
		}
		break;
	}
}

void	CMover::OnAttackMelee_Bear( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
		{
	#ifdef __WORLDSERVER
			SkillProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 234, SI_GEN_ATKSPELL_RATEGROUP ), 5, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );			
			
			OnDamage( nItemID, dwState, pHitObj );					
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK2:	
		{
	#ifdef __WORLDSERVER
			OnDamage( nItemID, dwState, pHitObj );					
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK3:		
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{				
	#ifdef __CLIENT
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1743, XI_SKILL_CIRCLE_DUST ), GetPos() );			
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );
				
				PLAYSND( SND_PC_SKILLD_HITOFPENYA );//, &v2 );
	#endif //__CLIENT 			
				
	#ifdef __WORLDSERVER
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 224, SI_GEN_KNOCK_BACK ), 4, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );
	#endif //__WORLDSERVER			
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )
			{
	#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
	#endif //__CLIENT
				
	#ifdef __WORLDSERVER
				SkillProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 190, SI_GEN_STUNGROUP ), 5, "OnAttackMelee_Bear" );
				if( bSuccess )
					g_cSkillSystem->ApplySkillAround( this, this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );				
	#endif //__WORLDSERVER			
			}
		}
		break;
	case OBJSTA_ATK4:	
		{	
#ifdef __WORLDSERVER
			SkillProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess;
	
			// 자신 힐링
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 44, SI_ASS_HEAL_HEALING ), 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			//버서크
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 303, SI_MUSHMOOT_BERSERK ), 1, "OnAttackMelee_Bear" );
			if( bSuccess )
				g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			// 우든스킨 
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 304, SI_MUSHMOOT_WOODENSKIN ), 1, "OnAttackMelee_Bear" );
			if( bSuccess )
				g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );
			
			//// 힘업
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 49, SI_ASS_CHEER_HEAPUP ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			////캐논볼
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 50, SI_ASS_CHEER_CANNONBALL ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			////멘탈사인
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 52, SI_ASS_CHEER_MENTALSIGN ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			////비프업
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 53, SI_ASS_CHEER_BEEFUP ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );
			//
			//
			////캐츠플렉스
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 115, SI_ASS_CHEER_CATSREFLEX ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			////어큐러시
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 116, SI_ASS_CHEER_ACCURACY ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );

			////해이스트
			//bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SKILL_INDEX( 20, SI_ASS_CHEER_HASTE ), 1, "OnAttackMelee_BigMuscle" );
			//if( bSuccess )
			//	g_cSkillSystem->DoApplySkill( this, this, pSkillProp, pAddSkillProp , false );
			

			// 보스몬스터가 점점 커진다.
			D3DXVECTOR3 vScale = GetScale();

			vScale *= 1.3f;

			if( vScale.x < 10.0f && m_dwIndex != OBJECT_INDEX( 1565, MI_GIANTTREEK1 ))
			{
				SetScale( vScale );
				g_xWSUserManager->AddSetScale( this, (vScale.x / m_pModel->m_pModelElem->m_fScale)  );
			}
			
#endif //__WORLDSERVER			
		}
		break;
	}
}
// 데미지를 적용하는 부분. - 공격방식에 따라서 타점한번에 어러번 호출될 수도 있다.
// pHitObj : 데미지를 먹을 대상. NULL이면 vDamagePos의 좌표를 중심으로 데미지를 준다.
BOOL	CMover::OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos, DWORD dwAtkFlag )
{
	int nParam = 0x01;		//sun

	if( IsPlayer() && IsDualWeapon() )		// 쌍칼모드냐?
	{
		if( dwAtkFlag & AF_GENERIC )	// 평타면
		{
			switch( dwState )
			{
			case OBJSTA_ATK1:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK2:	nParam |= 0x02;		break; // 왼손
			case OBJSTA_ATK3:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK4:	nParam |= 0x03;		break; // 양손
			case OBJSTA_ATK5:	nParam |= 0x03;		break; // 양손
			}
		}
	}


#if defined(__CLIENT)
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
#endif //defined(__CLIENT) 
	
	int	nDamage = 0;
	if( IsPlayer() )
	{
		// 플레이어
		nDamage = pHitObj->m_pActMover->SendDamage( dwAtkFlag, GetId(), nParam );
	}
	else
	{
		// 몬스터
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItemID );
		if( pItemProp )
		{
			if( pItemProp->dwSpellRegion == SRO_AROUND )
			{
				int nType = OBJTYPE_PLAYER;		// 플레이어들만 데미지.
				if( pHitObj )
				{
					if( dwAtkFlag == AF_MAGICSKILL )
					{
						pHitObj->SendDamageAround( dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange, 1.0f, 1.0f );
					}
					else
					{
#ifdef __WORLDSERVER
						g_xApplyItemEffect->SendDamageAroundTarget( dwAtkFlag, this, pHitObj, nType, nItemID, (float)pItemProp->_dwSkillRange, 1.0f );
#endif // __WORLDSERVER
					}
				}
				else
				{
					if( dwAtkFlag == AF_MAGICSKILL )
					{
						GetWorld()->SendDamageAround( pvDamagePos, dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange );
					}
					else
					{
#ifdef __WORLDSERVER
						g_xApplyItemEffect->SendDamageAroundPos( pvDamagePos, dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange );
#endif // __WORLDSERVER
					}
				}
			} else
			{
				if( pHitObj )
					nDamage = pHitObj->m_pActMover->SendDamage( dwAtkFlag, GetId(), 1 );
			}
		} else
		{
			if( pHitObj )
				nDamage = pHitObj->m_pActMover->SendDamage( dwAtkFlag, GetId(), 1 );
		}
	}


 #ifdef __WORLDSERVER
	if( nDamage > 0 )		// THIS가 공격했는데 데미지 먹이는데 성공했다
	{
		if( pHitObj->m_pActMover->GetState() & OBJSTA_ATK_MELEESKILL )	// 맞는놈이 스킬 모션중이었다.
		{
			DWORD dwSkill = pHitObj->GetActParam(0);		// 현재 사용하고 있는 스킬. dwSkill
			if( dwSkill == SKILL_INDEX( 14, SI_MER_ONE_REFLEXHIT ) )			// 리플렉히트 스킬을 쓰고 있는 중에 맞았다.
			{
				pHitObj->m_nReflexDmg += nDamage;			// 리플렉데미지는 쌓임.
			}
		}

		////< 어그로 추가 >
		////몬스터가 PC를 처음 때릴경우
		//if( IsMonster() == TRUE && m_nFirstHitObjID == NULL_ID )	
		//{
		//	FLAggro::SumFirstTargetAggro( *this, pHitObj->GetId() );	
		//	m_nFirstHitObjID			= pHitObj->GetId();			
		//}
		////PC가 Monster 때릴 경우
		//else if( IsPlayer() == TRUE && pHitObj->IsMonster() )					
		//{
		//	if( m_nFirstAttackerObjID == NULL_ID )						//PC가 Monster를 최초로 때린 경우
		//	{
		//		FLAggro::SumFirstAttackerAggro( *pHitObj, GetId() );
		//		m_nFirstHitObjID		= pHitObj->GetId();					
		//	}
		//	else														
		//	{
		//		FLAggro::SumAttackAggro( *pHitObj, this->GetId(), nDamage );
		//	}
		//}
	}
 #endif
	return nDamage > 0;
}

//
// 일반 공격의 타점때 호출되는 핸들러.
BOOL	CMover::OnAttackMelee( DWORD dwState, CMover *pHitObj )
{
//  	if( GetAct() != OBJACT_MELEE_ATTACK ) {
//  		return FALSE;
//  	}

//	CModelObject	*pModel = (CModelObject *)m_pModel;
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
	PostAIMsg( AIMSG_ATTACK_MELEE );
#endif
	
 	if( m_dwIndex == OBJECT_INDEX( 164, MI_CLOCKWORK1 ) )	// 클락워크의 경우 때릴때 이펙트.
	{
		OnAttackMelee_ClockWorks( dwState, pHitObj );
		return TRUE;
	}
	
	if( m_dwIndex == OBJECT_INDEX( 165, MI_BIGMUSCLE ) )	// 빅머슬의 경우 때릴때 이펙트.
	{
		OnAttackMelee_BigMuscle( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}

	if( m_dwIndex == OBJECT_INDEX( 166, MI_KRRR ) || m_dwIndex == OBJECT_INDEX( 1554, MI_KRRR2 ) )	// 크르릉의 경우 때릴때 이펙트.
	{
		OnAttackMelee_Krrr( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}	

	if( m_dwIndex == OBJECT_INDEX( 167, MI_MUSHMOOT ) || m_dwIndex == OBJECT_INDEX( 1552, MI_MUSHMOOT2 ) || m_dwIndex == OBJECT_INDEX( 1565, MI_GIANTTREEK1 ))	// 머쉬무트의 경우 때릴때 이펙트.
	{
		OnAttackMelee_Bear( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}	

//sun: 메테오니커 파이어 발사!!!
	if( m_dwIndex == OBJECT_INDEX( 715, MI_DU_METEONYKER ) || m_dwIndex == OBJECT_INDEX( 753, MI_DU_METEONYKER2 ) || m_dwIndex == OBJECT_INDEX( 754, MI_DU_METEONYKER3 ) ||
		m_dwIndex == OBJECT_INDEX( 755, MI_DU_METEONYKER4 ) || m_dwIndex == OBJECT_INDEX( 1548, MI_DU_METEONYKER5 ) )
	{
		OnAttackMelee_Meteonyker( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}

#ifdef __WORLDSERVER
//sun: 12, // __MONSTER_SKILL
	if( IsNPC() && CMonsterSkill::GetInstance()->ApplySkill( this, pHitObj, ATK_MELEE ) )
	{
		//m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}
#endif // __WORLDSERVER

	BOOL bSuccess = FALSE;
	BOOL bDamage = TRUE;

#ifdef __WORLDSERVER
	if( IsNPC() && pHitObj->IsPlayer() && pHitObj->IsRangeObj( this, 10.0f ) == FALSE )		// 두번죽는 경우땜에 넣어봤다. 몹->유저 10미터 이상 떨어져있을때는 안맞게.
		bDamage = FALSE;
#endif
	if( bDamage )
	{
		const DWORD attackFlag = GetAct() == OBJACT_MELEE_ATTACK ? GetActParam( 3 ) : AF_MISS;
		bSuccess = OnDamage( nItemID, dwState, pHitObj, NULL, attackFlag );	// 데미지를 준다.
	}

#ifdef __WORLDSERVER
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 && IsPlayer() == TRUE && bSuccess == TRUE )
	{
		if( dwState == OBJSTA_ATK1 && ((FLWSUser*)this)->m_playTaskBar.m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
		{
			((FLWSUser*)this)->m_playTaskBar.m_nActionPoint++;
		}
	}
#endif	// worldserver

#ifdef __CLIENT
	CModelObject	*pModel = (CModelObject *)m_pModel;
	if( bSuccess )		// 타격에 성공했다면 타격음을 플레이.
	{
		if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 && CMover::GetActiveMover() == this )
		{
			if( dwState == OBJSTA_ATK1 && g_WndMng.m_pWndTaskBar->m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
			{
				g_WndMng.m_pWndTaskBar->m_nActionPoint++;
			}
		}
		//  플레이어는 들고 있는 무기에 따라 타격 음이 다르다.
		PT_ITEM_SPEC pItemProp = GetActiveHandItemProp();
		if( pItemProp )
		{
			PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
		}
		
		if( IsPlayer() )
		{
			pModel->m_nPause = 5;	// frame 멈춤
		}
		else
		{
			pModel->m_nPause = 0;	// 몬스터는 멈추지 않음
			pHitObj->m_pModel->m_nPause = 0;
		}

#if defined(__CLIENT)
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
#endif //defined(__CLIENT)
		
		
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

	if( m_dwIndex == OBJECT_INDEX( 164, MI_CLOCKWORK1 ) )	// 클락워크의 경우 때릴때 이펙트.
	{
	#ifdef __WORLDSERVER
		UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
		CModelObject *pModel = (CModelObject *)m_pModel;
		pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
	#endif // WorldServer

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

					#ifdef __CLIENT
						CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1618, XI_NPCSP1RANSPARK ), vOut );
						if( pSfx )
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
					#endif // __CLIENT
					}
						
				}
			}
			break;
		}
	}
//sun: 메테오니커 파이어 발사!!!
	else
	if( m_dwIndex == OBJECT_INDEX( 715, MI_DU_METEONYKER ) || m_dwIndex == OBJECT_INDEX( 753, MI_DU_METEONYKER2 ) || m_dwIndex == OBJECT_INDEX( 754, MI_DU_METEONYKER3 ) ||
		m_dwIndex == OBJECT_INDEX( 755, MI_DU_METEONYKER4 ) || m_dwIndex == OBJECT_INDEX( 1548, MI_DU_METEONYKER5 ) )
	{
	#ifdef __WORLDSERVER
		UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
		CModelObject *pModel = (CModelObject *)m_pModel;
		pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
	#endif // WorldServer

		switch( dwState )
		{
			case OBJSTA_ATK2:
			{
				if( m_pActMover->m_nMotionHitCount == 1 )
				{
					D3DXVECTOR3 v1, v2, v3;
					CModelObject *pModel = (CModelObject *)m_pModel;
					pModel->GetEventPos( &v1, 0 );		// 레이저 나가는 부분.
					pModel->GetEventPos( &v3, 1 );		// 레이저 나가는 부분.
					D3DXVec3TransformCoord( &v1, &v1, GetMatrixWorldPtr() );	// 월드공간으로 변환.
					D3DXVec3TransformCoord( &v3, &v3, GetMatrixWorldPtr() );	// 월드공간으로 변환.

					v2 = D3DXVECTOR3( 0, 0, 40.0f );
					D3DXVec3TransformCoord( &v2, &v2, pModel->GetMatrixBone(6) );	// 6이 머리.  머리에서 v2지점의 위치를 계산.
					D3DXVec3TransformCoord( &v2, &v2, GetMatrixWorldPtr() );	// 월드 공간으로 변환.
					
			#ifdef __CLIENT
					D3DXVECTOR3 vDir = v3 - v1;
					D3DXVec3Normalize( &vDir, &vDir );
					D3DXVECTOR3 vTemp;
					D3DXVECTOR3 vScal = D3DXVECTOR3( 0.3f, 0.3f, 0.3f );
					D3DXVECTOR3 vModelScal = GetScale();

					vScal.x *= vModelScal.x;
					vScal.y *= vModelScal.y;
					vScal.z *= vModelScal.z;

					for( int i = 0; i < 2; i ++ )
					{
						vTemp = vDir * (xRandomF( 0.2f ) + 0.1f );
						vTemp *= 0.8f;
						vTemp.y -= (xRandomF( 0.05f ) + 0.01f );
						
						extern CPartsFireDragon	g_FireDragon;
						g_FireDragon.Create( D3DDEVICE, v1, XI_INDEX( 1753, XI_NAT_FIRE01_ADV ), vScal, vTemp );
					}

			#endif

			#ifdef __CLIENT
					if( (m_pActMover->m_nCount & 1) == 0 )
			#else				
					if( (m_pActMover->m_nCount & 3) == 0 )
			#endif
					{
//						CWorld *pWorld = GetWorld();
						D3DXVECTOR3 vOut	= GetPos();
//						if( pWorld->IntersectObjLine( &vOut, v1, v2, FALSE, TRUE, FALSE ) )
						{
							OnDamage( nItemID, dwState, NULL, &vOut );		// 이펙트가 터지는 위치에 데미지를 줌.
	/*
						#ifdef __CLIENT
							CSfx *pSfx = CreateSfx( D3DDEVICE,  XI_NPCSP1RANSPARK, vOut );
							if( pSfx )
								pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
						#endif // __CLIENT
	*/
						}
							
					}
				}
				break;
			}
		}
	}

	return TRUE;
}

// 몬스터의 특수공격 타격부분에 호출
BOOL	CMover::OnAttackSP()
{
	int		idTarget = GetActParam( 0 );		
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj(pHit) )	return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.

	PT_ITEM_SPEC pItemProp;
	if( nItemID == NULL_ID || nItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = g_xSpecManager->GetSpecItem( nItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)

	if( pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s의 itemProp이 없다. %d" ), m_szName, nItemID );
		return FALSE;
	}

	if( pItemProp->dwSpellRegion == SRO_AROUND )
	{
#ifdef __WORLDSERVER
		g_xApplyItemEffect->SendDamageAroundTarget( AF_GENERIC, this, this, OBJTYPE_PLAYER, nItemID, (float)( pItemProp->_dwSkillRange ), 0.0f );
#endif // __WORLDSERVER
	}
	else
	{
#ifdef __CLIENT
		if( pHit->IsLive() )
			pHit->m_pActMover->SendDamage( AF_MONSTER_SP_CLIENT, GetId(), (nItemID << 16) );	// 09_12_21 몬스터 스페셜 공격
#else
		if( pHit->IsLive() )
			pHit->m_pActMover->SendDamage( AF_GENERIC, GetId(), (nItemID << 16) );	// 직접 공격 스따일.
#endif
	}

	return TRUE;
}                                                         

//
//	캐릭터가 높은곳에서 떨어진 시점에 호출
//
void	CMover::OnActDrop()
{
#ifdef __WORLDSERVER
	if( m_pActMover->GetDmgState() & OBJSTA_DMG_FLY_ALL )		// 데미지 플라이땐 이거 안하는게 좋다.
		return;

	if( FALSE == IsPlayer() )
	{
		if( GetSpeedFactor() < 1.9F	//sun: 9,	//__AI_0509
			&& m_dwAIInterface != AII_PET
			)
		{
			ClearDestPos();
			SendActMsg( OBJMSG_STAND );
			g_xWSUserManager->AddMoverBehavior( this );
		}
	}
#else	// __WORLDSERVER
	/*
	if( IsActiveMover() )
	{
		ClearDest();
		SendActMsg( OBJMSG_STOP );
		g_DPlay.SendPlayerBehavior();
	}
	*/
#endif	// not __WORLDSERVER
}

void	CMover::OnActCollision()
{
#ifdef __WORLDSERVER
	if( FALSE == IsPlayer() )
	{
		//sun: 9, //__AI_0509 
		if( GetSpeedFactor() < 1.9F && m_dwAIInterface != AII_PET )
		{
			ClearDest();
			SendActMsg( OBJMSG_STAND );
			g_xWSUserManager->AddMoverBehavior( this );
			PostAIMsg( AIMSG_COLLISION );
		}
	}
#else	// __WORLDSERVER
	if( TRUE == IsActiveMover() )
	{
		//ClearDest();
		//SendActMsg( OBJMSG_STAND );
		CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( pWndWorld )
		{
			pWndWorld->m_bLButtonDown	= FALSE;
		}

		//gmpbigsun( 20120116 ) : 장애물에 걸렸을경우 cancel action slot
		/*
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			if( g_WndMng.m_pWndTaskBar )
				g_WndMng.m_pWndTaskBar->OnCancelSkill();
		}
		*/
	}
#endif	// __WORLDSERVER
}

// 근접스킬 동작 끝났을때.
void	CMover::OnActEndMeleeSkill()
{
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		if( ((FLWSUser *)this)->m_playTaskBar.m_nUsedSkillQueue != SKILL_QUEUE_NONE )
		{
			((FLWSUser *)this)->m_playTaskBar.SetNextSkill( (FLWSUser *)this );
		}
	}
#endif 

}

// 마법스킬 동작 끝났을때.
void	CMover::OnActEndMagicSkill()
{
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
 #ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		if( ((FLWSUser *)this)->m_playTaskBar.m_nUsedSkillQueue != SKILL_QUEUE_NONE )
		{
			((FLWSUser *)this)->m_playTaskBar.SetNextSkill( (FLWSUser *)this );
		}
	}
 #endif 
}

// 착지(JUMP4)플래그가 사라지는 순간.
void	CMover::OnActEndJump4()
{
	if( FALSE == IsEmptyDestPos() )
		SetDestPos( GetDestPos(), m_bForward, FALSE );

	if( m_dwIndex == OBJECT_INDEX( 164, MI_CLOCKWORK1 ) )
	{
		SendActMsg( OBJMSG_APPEAR );
	}
}

// 착지 하는 순간에 발생.
void	CMover::OnActLanding()
{
#ifdef __CLIENT	
	if( m_dwIndex == OBJECT_INDEX( 164, MI_CLOCKWORK1 ) )
	{
		GetWorld()->m_pCamera->SetQuake( 60, 1.0f );		// 보스가 떨어지면 땅이 흔들림.
	}
#endif
}

// 전진 하는 순간에 발생.
void	CMover::OnActFMove()
{
#ifdef __WORLDSERVER
	PostAIMsg( AIMSG_BEGINMOVE );
#endif
}

// Interact 오브젝트에 착지 했을때.
// pIAObj : 어떤 IA오브젝트에 착지했는가.
// vPos : 착지한 this의 좌표.
void	CMover::OnActIALanding( CObj *pIAObj, const D3DXVECTOR3 &/*vPos*/ )
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
			FLTRACE_LOG( PROGRAM_NAME, _T( "SendLocalPosFromIA" ) );
#endif //__CLIENT
		}
	}

}

/*
int		CMover::OnActCollecting() 
{ 
#ifdef __CLIENT
	if( (m_dwFlag & MVRF_COLLECT) == 0 )
	{
		SkillProp *pHandProp = GetActiveHandItemProp();
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
*/

// 공격을 막 시작했을때(일반/스킬/완드 등)
void	CMover::OnAttackStart( CMover * /*pTarget*/, OBJMSG /*dwMsg*/ )
{
}

// this가 투명화 상태 걸려있으면 해제시킴.
void	CMover::RemoveInvisible()
{
#ifdef __WORLDSERVER
	if( GetAdjParam( DST_CHRSTATE ) & CHS_INVISIBILITY )		// 투명화 상태다.
	{
		ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );
	}

	RemoveBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) );		// 해제.
#endif // __WORLDSERVER
}


// pItem이 루팅가능한가를 판별.
#ifdef __WORLDSERVER
BOOL CMover::IsLoot( CItem *pItem, BOOL bPet )
{
	BOOL bTake	= FALSE;
	
	if( pItem->IsDelete() || ( pItem->m_idHolder != 0 && pItem->m_idHolder != m_idPlayer ) )
	{
		return bTake;
	}

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

	DWORD dwTime	= g_tmCurrent;
#ifdef __EVENT_MONSTER
	DWORD dwMonsterId = pItem->m_IdEventMonster;
	if( dwMonsterId != NULL_ID && CEventMonster::GetInstance()->SetEventMonster( dwMonsterId ) )
	{
		DWORD dwLootTime = CEventMonster::GetInstance()->GetLootTime();
		if( (dwTime - pItem->m_dwDropTime) >= (DWORD)( SEC(dwLootTime) ) )
			bTake = TRUE;
		
		if( bPet && !CEventMonster::GetInstance()->IsPetAble() )
			bTake = FALSE;
	}
	else
#endif // __EVENT_MONSTER
//sun: 9차 추가분
	if( (dwTime - pItem->m_dwDropTime) >= SEC(7) )	// 7초가 지난건 아무나 줏을 수 있다.
		bTake = TRUE;

	// 펫은 리스폰된 아이템을 집을수 없음.
	if( bPet && bTake )
	{
		if( 0 < ((CCtrl*)pItem)->m_lRespawn )
			bTake = FALSE;
		// 소지품이 가득 차서 해당 아이템을 획득할 수 없다
		FLItemElem* pItemElem	= (FLItemElem*)pItem->m_pItemBase;
		PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
		if( pItemProp->dwItemKind3 != IK3_GOLD && m_Inventory.IsFull( pItemElem, pItemElem->m_nItemNum ) )
			bTake	= FALSE;
	}

	return bTake;
}
#endif // WorldServer

// DoLoot의 하위 - 몹이 떨어트린 아이템을 솔로잉일때 집으려 할때 처리부
BOOL CMover::SubLootDropMobSingle( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER	
	FLItemBase* pItemBase = pItem->m_pItemBase;
	bSuccess	= CreateItem( pItemBase );
	
	if( IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
		{
			((FLWSUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );

			//if( IsLootSendText( pItemBase->GetProp() ) )
			//	((FLWSUser*)this)->AddChatText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );									
			
		}
		else
			((FLWSUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
	if( ( (FLItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (FLItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action = "R";
					//aLogItem.SendName = "GROUND";
					//aLogItem.RecvName = GetName();
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				//aLogItem.Action = "R";
				//aLogItem.SendName = "GROUND";
				//aLogItem.RecvName = GetName();

				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );

				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}

#endif // WorldServer
	return bSuccess;
} // SubLootDropMobSingle

// DoLoot의 하위 - 몹이 떨어트린 아이템을 파티가 있을때 집으려 할때 처리부
BOOL CMover::SubLootDropMobParty( CItem *pItem, CParty *pParty )
{
	BOOL bSuccess = TRUE;;
#ifdef __WORLDSERVER
	
	int	nMaxListMember = 0;
	FLWSUser* pMember	= NULL;
	FLWSUser* pGetUser = NULL;
//	float fDist;
	D3DXVECTOR3	vDist;
	FLItemBase* pItemBase = pItem->m_pItemBase;
	
	// 아이템줍는사람 반경안에 드는 멤버들만 추려낸다.
	//mem_set( pListMember, 0, sizeof(pListMember) );
	FLWSUser *pListMember[ 8 ] = { 0 };
	for( int i = 0; i < pParty->m_nSizeofMember; i++ )
	{
		pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
//		if( IsValidObj( (CObj*)pMember ) )
		// 12-3 康: 아이템 획득은 같은 맵 일정 반경에 속한 사용자로 제한해야한다.
		/*  
		if( IsValidObj( (CObj*)pMember ) && GetWorld() == pMember->GetWorld() )
		{
			vDist = pMember->GetPos() - GetPos();
			fDist = D3DXVec3LengthSq( &vDist );		// 아이템 줍는사람과 멤버간의 거리.
			if( fDist < 32.0f * 32.0f )		// xx미터보다 작으면
			{
				pListMember[ nMaxListMember++ ] = pMember;
			}
		}
		*/ //2009.01.21 // 반경 검사에 레이어 추가
		if( IsValidArea( pMember, 32.0f ) )
			pListMember[ nMaxListMember++ ] = pMember;
	}

	if( nMaxListMember == 0 )
	{
		pGetUser	= (FLWSUser*)this;
	}
	else
	{
		switch( pParty->m_nTroupeShareItem )
		{
		case 1 :	// 순서분배

			{
				for( int i = 0 ; i < nMaxListMember ; i++ )
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
					pGetUser = (FLWSUser*)this;
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
				pGetUser = (FLWSUser*)this; 
			}
			break;
		}	// switch
	}	
	pParty->m_nGetItemPlayerId = pGetUser->m_idPlayer;

	bSuccess	= pGetUser->CreateItem( pItemBase );

	for( int i = 0; i < nMaxListMember; i++ )
	{
		pMember = pListMember[i];
		if( TRUE == pMember->IsPlayer() && pItemBase->GetProp() != NULL )
		{
			if( bSuccess )
			{
				if( pMember->m_idPlayer != pGetUser->m_idPlayer )
				{
					if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_JAP || g_xFlyffConfig->GetMainLanguage() == LANG_HK )
						( (FLWSUser*)pMember )->AddDefinedText( TID_GAME_TROUPEREAPITEM, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
					else
					{
						if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
							( (FLWSUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM_THA, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
						else
							( (FLWSUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s%s %s\"", pGetUser->GetName(), prj.GetText(TID_GAME_FROM2), pItemBase->GetProp()->szName );
					}
				}
				else
				{
					( (FLWSUser*)pGetUser )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
				}
			}
			else
			{
				// CreateItem을 실패함 Full
				if( pGetUser == pMember )
					( (FLWSUser*)pGetUser )->AddDefinedText( TID_GAME_LACKSPACE );
			}
		}									
	}
	
	if( ( (FLItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( ( (FLItemElem*)pItemBase )->m_nItemNum ) > 9 )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action = "R";
					//aLogItem.SendName = "GROUND";
					//aLogItem.RecvName = pGetUser->GetName();
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pGetUser->GetName() );
					aLogItem.WorldId = pGetUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				//aLogItem.Action = "R";
				//aLogItem.SendName = "GROUND";
				//aLogItem.RecvName = pGetUser->GetName();
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), pGetUser->GetName() );
				aLogItem.WorldId = pGetUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
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
#endif	// __WORLDSERVER

	return bSuccess;
}

// DoLoot의 하위 - 몹이 떨어트린게 아닌! 아이템루팅부...
BOOL CMover::SubLootDropNotMob( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	FLItemBase* pItemBase = pItem->m_pItemBase;
	
	bSuccess	= CreateItem( pItemBase );
	if( TRUE == IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
		{
			((FLWSUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );						

			//if( IsLootSendText( pItemBase->GetProp() ) )
			//	((FLWSUser*)this)->AddChatText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );									
		}
		else
			((FLWSUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
	if( ( (FLItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (FLItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action = "R";
					//aLogItem.SendName = "GROUND";
					//aLogItem.RecvName = GetName();
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				//aLogItem.Action = "R";
				//aLogItem.SendName = "GROUND";
				//aLogItem.RecvName = GetName();
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (FLItemElem*)pItemBase ), ( (FLItemElem*)pItemBase )->m_nItemNum );
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

	FLItemBase* pItemBase = pItem->m_pItemBase;
	BOOL bSuccess = TRUE;

#ifdef __CLIENT	
	PLAYSND( SND_INF_GROUNDPICKUP );
#endif
	
	PT_ITEM_SPEC pItemProp = pItem->GetProp();
	if( pItemProp->dwItemKind1 == IK1_GOLD ) 
	{
		PickupGold( ((FLItemElem*)pItemBase)->GetGold(), pItem->m_bDropMob );
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
#endif	// __WORLDSERVER
	}

	if( TRUE == bSuccess )
	{
		pItem->Delete();
#ifdef __WORLDSERVER
//		g_xWSUserManager->AddMotion( this, OBJMSG_PICKUP );
		RemoveInvisible();		// 아이템 줏으면 투명 해제.
#endif	// __WORLDSERVER
	}

	return bSuccess;
}

