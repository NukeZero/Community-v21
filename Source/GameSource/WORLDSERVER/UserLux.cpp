#include "stdafx.h"
#include "User.h"
#include <WorldMng.h>

#include "UserMacro.h"

extern	CWorldMng	g_WorldMng;


#include "FLMapGrid.h"
#include "LinkMap.h"

/*  
//
// 자원 채집 시작
//
BOOL FLWSUser::DoCollect( CMover *pTarget )
{
	BOOL bAble = TRUE;

	if( pTarget->m_idCollecter != NULL_ID )	// pTarget이 주인이 있는 몹이고 
	{
		CMover *pCollecter = prj.GetMover( pTarget->m_idCollecter );
		if( IsValidObj(pCollecter) )
		{
			if( m_idparty )		// 채집하려는 사람이 파티가 있는가?
			{
				if( m_idparty != pCollecter->m_idparty )		// 몹 주인과 같은 파티가 아니다.
					if( pTarget->m_nCollectOwnCnt > 0 )			// 아직 시간이 덜풀렸는가?
						bAble = FALSE;		// 못먹음.
			} else
			// 파티가 없을때
			{
				if( pTarget->m_idCollecter != GetId() )			// 몹 주인이 내가 아니냐?
					if( pTarget->m_nCollectOwnCnt > 0 )			// 아직 시간이 덜풀렸는가?
						bAble = FALSE;		// 못먹음.
			}
		}
//		else
			// 주인이 사라졌으면 걍 먹을 수 있음.
//
//		if( pTarget->m_idCollecter != GetId() )	// 그 주인이 내가 아니고
//		{
//			CMover *pCollecter = prj.GetMover( pTarget->m_idCollecter );
//			if( IsValidObj( pCollecter ) )
//			{
//				if( m_idparty && m_idparty != pCollecter->m_idparty )	// 같은 파티원도 아니고
//				{
//					if( pTarget->m_nCollectOwnCnt > 0 ) // 아직 시간이 덜 풀렸으면
//					{
//						AddDefinedText( TID_COLLECT_ERROROTHER );
//						return FALSE;				// pTarget은 this가 못 먹는다.
//					}
//				}
//			}
//		}

	}

	// 채집이 허용되지 않는 상황이면
	if( bAble == FALSE )
	{
		AddDefinedText( TID_COLLECT_ERROROTHER );		// 에러메시지 출력하고
		return FALSE;				// pTarget은 this가 못 먹는다.
	}
	

	SkillProp *pHandProp = GetActiveHandItemProp();
	if( pHandProp->dwItemKind3 != IK3_COLLECTER )
		return FALSE;
				
	if( CMover::DoCollect( pTarget ) == TRUE )
	{
		m_idTargetCollect = pTarget->GetId();	// pTarget을 채집중이라는것을 표시함.
//		pTarget->m_idCollecter = GetId();		// 타겟에다 내꺼라는 표시함.
		m_nCollect = 0;
		m_tmCollect = g_tmCurrent;
	} else
		return FALSE;
	
	return TRUE;
}
*/

/*
//
//
// this가 pTarget을 채집한다.
int		FLWSUser::OnActCollecting( void )
{
	CMover *pTarget = prj.GetMover( m_idTargetCollect );		// 채집대상의 포인터 꺼냄.
	if( IsInvalidObj( pTarget ) )	// 채집중 시간이 다되서 오브젝트가 사라지거나 거시기한 오브젝트면 걍 리턴.
		return TRUE;		// 채집 동작 해제하라.

	// 채집 모션이 진행되는동안 계속 들어온다.
	if( g_tmCurrent > m_tmCollect + 1000.0f )
	{
		m_tmCollect = g_tmCurrent;
		SkillProp *pHandProp = GetActiveHandItemProp();
		if( pHandProp == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "pHandProp==NULL" ) );
			return TRUE;
		}
		if( pHandProp->dwItemKind3 != IK3_COLLECTER )		// 채집중 도구를 없애면 채집 취소.
			return TRUE;
		
		m_nCollect += (int)pHandProp->dwLoadingTime;				// 아이템의 채집능력에 따라 채집량을 누적.
		pTarget->m_nResource -= (int)pHandProp->dwLoadingTime;		// 채집한 만큼 타겟의 리소스에서 뺌.
		if( pTarget->m_nResource < 0 )
			pTarget->m_nResource = 0;
		if( m_nCollect >= pHandProp->nShellQuantity )	// 채집양이 맥스치를 넘어가면.
		{
			//보조석 1개 생성;
			BYTE bID;
			FLItemElem itemElem;
			itemElem.m_dwItemId	= pTarget->GetProp()->dwSourceMaterial;		// 자원 생성.
			itemElem.m_nItemNum		= 1;
			if( itemElem.m_dwItemId != NULL_ID )
			{
				BOOL bRet = CreateItem( &itemElem, &bID );
				if( bRet == TRUE )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "C";
					aLogItem.SendName = pTarget->GetName();
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = GetGold();
					aLogItem.Gold2 = GetGold();
					aLogItem.ItemNo = itemElem.GetSerialNumber();
					aLogItem.ItemName = itemElem.GetProp()->szName;
					aLogItem.itemNumber = 1;
					g_DPSrvr.OnLogItem( aLogItem );
				}
			} else
				FLERROR_LOG( PROGRAM_NAME, _T( "%s가 dwSourceMaterial이 -1" ), pTarget->GetName() );

			FLOAT fResRatio = (float)pTarget->m_nResource / pTarget->GetProp()->dwMaterialAmount;		// 몇퍼센트 남았는지 ...
			FLOAT fScale = pTarget->m_pModel->m_pModelElem->m_fScale;		// mdlDyna에 있던 scale값이 100%
			if( fResRatio > 1.0f )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%f %d %d" ), fResRatio, pTarget->m_nResource, (int)pTarget->GetProp()->dwMaterialAmount );
				fResRatio = 1.0f;
			}
			fScale *= fResRatio;
			D3DXVECTOR3 vScale = D3DXVECTOR3( fScale, fScale, fScale );
			pTarget->SetScale( vScale );
			if( fScale > 10.0f )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s %f %f %d %d" ), GetName(), fScale, 
														pTarget->m_pModel->m_pModelElem->m_fScale, pTarget->m_nResource, pTarget->GetProp()->dwMaterialAmount );
			
			g_xWSUserManager->AddSetScale( pTarget, fResRatio );
			

			m_nCollect -= pHandProp->nShellQuantity;	// 오버된것 재조정.
		}
		if( pTarget->m_nResource <= 0 )							// 타겟의 리소스가 다떨어졌으면.
		{
			// 채집끝
			pTarget->m_nResource = 0;							// 0으로만 해두고 타겟이 없어지는건 그쪽에서 알아서 한다.
			AddDefinedText( TID_COLLECT_EMPTY );	// 채집이 끝났습니다
			return TRUE;	// 채집동작 해제하라.
		}			
	}
	return 0;
}
*/

// tmMaxEscape : 탈출의 쿨타임
void FLWSUser::AddEscape( DWORD tmMaxEscape )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_DO_ESCAPE;
	m_Snapshot.ar << tmMaxEscape;
}

// 액션포인트 갱신.
void FLWSUser::AddSetActionPoint( int nAP )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETACTIONPOINT;
	m_Snapshot.ar << nAP;
}

void FLWSUser::AddSetTarget( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETTARGET;
	m_Snapshot.ar << idTarget;
}

// ----------------------------------------------------------------------------------
//
//   FLWSUserManager Lux
//
// ----------------------------------------------------------------------------------
/*
void	FLWSUserManager::AddDoCollect( FLWSUser* pUser, OBJID idTarget )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_DO_COLLECT;
	ar << idTarget;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}
*/

void	FLWSUserManager::AddCreateSkillEffect( CMover *pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	if( pAttacker == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pAttacker ) << SNAPSHOTTYPE_ACTIVESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pAttacker->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pAttacker )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pAttacker )
		if( pAttacker->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pAttacker )
}

// pMover가 스턴 된것을 주위 유저클라에 알림.
void	FLWSUserManager::AddSetStun( CMover *pMover, int nMaxTime )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSTUN;
	ar << nMaxTime;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void	FLWSUserManager::AddDoApplySkill( CCtrl *pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_DOAPPLYUSESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

// SendActMsg를 주위 유저들에게 보냄 .
void	FLWSUserManager::AddSendActMsg( CMover *pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SENDACTMSG;
	ar << (int)dwMsg;
	ar << pMover->IsFly();
	ar << nParam1 << nParam2 << nParam3;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

// Mover를 어느방향으로 밀리게 하다.
// pMover : 밀리는 무버
// vPos : 밀리는 시점에서의 vPos - 정확한 동기화때문에 좌표와 각도까지 필요하다.
// fAngle : 밀리는 시점에서의 Angle - 
// fPushAngle : 미는 각도.
// fPower : 미는 힘.
void	FLWSUserManager::AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fAngle, FLOAT fPushAngle, FLOAT fPower )
{
	if( pMover == NULL )
	{
		return;
	}

	if( (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) || pMover->m_pActMover->GetState() & OBJSTA_STUN ) // 데미지 플라이중이면 리턴
		return;

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PUSHPOWER;
	ar << vPos;
	ar << fAngle;
	ar << fPushAngle << fPower;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void	FLWSUserManager::AddRemoveSkillInfluence( CMover *pMover, WORD wType, WORD wID )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	ar << wType;
	ar << wID;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetPosAngle( CCtrl* pCtrl, const D3DXVECTOR3 &vPos, FLOAT fAngle )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_SETPOSANGLE;
	ar << vPos << fAngle;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

/*
void FLWSUser::AddRemoveSkillInfluence( WORD wType, WORD wID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.Lock();
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	m_Snapshot.ar << wType;
	m_Snapshot.ar << wID;
	
	m_Snapshot.Unlock();
}
*/

void	FLWSUserManager::AddSetSleeping( CMover *pMover, DWORD dwDuration )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSLEEPING;
	ar << dwDuration;
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void	FLWSUserManager::AddSeasonEffect()
{
	for( std::map<DWORD, FLWSUser*>::iterator pos = m_users.begin(); pos != m_users.end(); ++pos ) {
		if( IsValidObj( static_cast<CObj*>( pos->second ) ) == TRUE ) {
			pos->second->AddSeasonEffect();
		}
	}
}
