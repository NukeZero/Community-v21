#include "StdAfx.h"

#include "FLBehaviorFunction.h"
#include "AIInterface.h"
#include "DialogMsg.h"


#include "FLPartyAggroFSM.h"

#ifdef __WORLDSERVER
#include "../WorldServer/User.h"
#include "../worldserver/FLMapGrid.h"
#include "../worldserver/LinkMap.h"
#endif 



#include "party.h"
extern	CPartyMng			g_PartyMng;

#undef max

//-------------------------------------------------------------------------------------------------------------------------------------------------//

FLPartyAggroLeaderBehavior::FLPartyAggroLeaderBehavior(/* const DWORD dwMoverIndex ) : m_dwMoverIndex( dwMoverIndex )*/)
{

}

FLPartyAggroLeaderBehavior::~FLPartyAggroLeaderBehavior()
{

}

CMover * FLPartyAggroLeaderBehavior::TryScanTargetByNear_Party( const FLAIOptionalFlags & kOptionalFlags )
{
	for( CalledMonsterMap::iterator it = m_mapCalledMonster.begin(); it != m_mapCalledMonster.end(); ++it )
	{
		const OBJID objid	= it->second;
		CMover * pMover		= prj.GetMover( objid );
		if( IsValidObj( pMover ) && pMover->IsLive() && pMover->m_bActiveAttack == TRUE )
		{
			return FLScanTarget::ByNear( *pMover, *pMover, pMover->GetProp()->m_nAttackFirstRange, kOptionalFlags );
		}
	}

	return NULL;
}

void FLPartyAggroLeaderBehavior::TryDoSummon( CMover & Leader, CWorld * pWorld/*, const MoverProp & Prop*/ )
{
	typedef				MoverProp::SummonMonstersMap		Map;

	if( IsValidObj( &Leader ) == FALSE )
		return;

	if( Leader.IsDie() == TRUE )
		return;
	
	if( pWorld == NULL )
		return;


	MoverProp * pProp	= prj.GetMoverProp( Leader.m_dwIndex );
	if( pProp == NULL )
		return;

	//const float fBaseAngle	= 180.f / Prop.m_mapSubMonsters.size();
	//const float fDist		= 1.0f;

	// 리스트 추려서 죽거나 없는 것만 소환
	int i = 0;
	for( Map::const_iterator it = pProp->m_mapSummonMonsters.begin(); it != pProp->m_mapSummonMonsters.end(); ++it, ++i )
	{
		MoverProp::FLMonsterKey monsterKey		= it->first;
		//const DWORD	dwMoverIndex	= it->first;
		//const DWORD dwSequence		= it->second;

		CalledMonsterMap::const_iterator subIt = m_mapCalledMonster.find( monsterKey );
		if( subIt != m_mapCalledMonster.end() )
		{
			const OBJID objid		= subIt->second;
			CMover * pSub			= prj.GetMover( objid );
			if( pSub != NULL )
				continue;
			//if( IsValidObj( pSub ) == TRUE && pSub->IsLive() )
			//	continue;
		}

		const D3DXVECTOR3 vPos	= GetSubMonsterPosition( Leader.GetPos(), Leader.GetAngle(), i, pProp->m_mapSummonMonsters.size(), SUB_MONSTER_DIST );
		CMover * pMover		= CreateMover( pWorld, monsterKey.dwMoverID, vPos, 0 );
		if( pMover == NULL )
		{
			DEBUG_BREAK;
			continue;
		}	

		FLPartyAggroSubMonsterFSM * pFSM = static_cast< FLPartyAggroSubMonsterFSM * >( pMover->m_pFSM );
		if( pMover->GetProp()->dwAI != AII_PARTY_AGGRO_SUB || pMover->m_pFSM == NULL )
		{
			DEBUG_BREAK;
			delete pMover;
		}

		pFSM->SetPartyLeader( Leader.GetId() );

		m_mapCalledMonster.insert( CalledMonsterMap::value_type( monsterKey, pMover->GetId() ) );
	}


	Update_CleanKilledSubMonster();
}

void FLPartyAggroLeaderBehavior::Update_CleanKilledSubMonster()
{
	for( CalledMonsterMap::iterator it = m_mapCalledMonster.begin(); it != m_mapCalledMonster.end(); )
	{
		const OBJID objid	= it->second;
		CMover * pMover		= prj.GetMover( objid );
		if( IsValidObj( pMover ) && pMover->IsLive() )
			++it;
		else
			it				= m_mapCalledMonster.erase( it );			//@@@@@@@@ 대입 해야하나?
	}
}


void FLPartyAggroLeaderBehavior::ClearAllSubMonster()
{
	for( CalledMonsterMap::iterator it = m_mapCalledMonster.begin(); it != m_mapCalledMonster.end(); ++it )
	{
		const OBJID objid	= it->second;
		CMover * pMover	= prj.GetMover( objid );
		if( pMover == NULL )
			continue;

		pMover->Delete();
	}
}

void FLPartyAggroLeaderBehavior::KillSubMonsterAll( CMover * pAttacker )
{
	for( CalledMonsterMap::iterator it = m_mapCalledMonster.begin(); it != m_mapCalledMonster.end(); ++it )
	{
		const OBJID objid	= it->second;
		CMover * pTarget	= prj.GetMover( objid );
		if( pTarget == NULL )
			continue;

		//강제로 몬스터 죽임.
		if( pAttacker != NULL )
		{
			pAttacker->SubExperience( pTarget );		// pTarget를 죽이고 난후의 m_pAttacker 경험치 처리.
			pAttacker->m_nAtkCnt	= 0;				// 타겟을 죽였으면 공격자의 어택카운트 클리어
		}

		switch( _GetContentState( CT_DROP_ITEM_REFORM  ) )
		{
		case CS_VER1:
			{
				pTarget->OnDie( *pAttacker );
			}
			break;
		default:
			pTarget->DropItemByDied( pAttacker );				// 몬스터였다면 아이템 드랍.
			break;
		}

		pTarget->DoDie( pAttacker );				// pTarget 죽어라. 
		pTarget->m_nHitPoint	= 0;
	}	
}


void FLPartyAggroLeaderBehavior::SetSubMonsterPos( CMover & Leader )
{
	int i = 0;
	for( CalledMonsterMap::iterator it = m_mapCalledMonster.begin(); it != m_mapCalledMonster.end(); ++it, ++i )
	{
		const OBJID objid	= it->second;
		CMover * pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) == FALSE )
			continue;

		if( pMover->IsDie() == TRUE )
			continue;

		pMover->SetStop();
		const D3DXVECTOR3 vDest = GetSubMonsterPosition( Leader.GetDestPos(), Leader.GetAngle(), i, m_mapCalledMonster.size(), SUB_MONSTER_DIST );
		//pMover->SetPos(  );
		SetMoveToDest( *pMover, vDest );
	}
}

D3DXVECTOR3				FLPartyAggroLeaderBehavior::GetSubMonsterPosition( const D3DXVECTOR3 vCenterPos, const float fAngle, const DWORD nSequenceIndex, const DWORD nSequenceMax, const float fDist )
{
	const float fBaseAngle	= 180.f / ( nSequenceMax + 1 );

	const FLOAT fSubAngle		= fAngle + 90.0f + ( fBaseAngle * nSequenceIndex );
	D3DXVECTOR3 vSub;
	AngleToVectorXZ( &vSub, fSubAngle, fDist );						

	const D3DXVECTOR3 vPos	= vCenterPos + vSub;
	//const D3DXVECTOR3 vPos	= Leader.GetDestPos() + vSub;
	return vPos;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------//

FLPatrolBehavior::FLPatrolBehavior()
{
	Clear();
}


FLPatrolBehavior::~FLPatrolBehavior()
{

}

void FLPatrolBehavior::Clear()
{
	m_tmStopMoverTimeOut	= 0;
}

BOOL FLPatrolBehavior::PatrolUpdate( CMover & Src, const D3DXVECTOR3 & vPosBegin, const float fPatrolRange )
{
	if( Src.GetMovePattern() == 0 )			// 패턴이 없다면 ( AI로 가져와야함 )
	{
		const DWORD tmTick	= ::GetTickCount();
		if( m_tmStopMoverTimeOut < tmTick && Src.m_pActMover->IsMove() == FALSE )//== FALSE )
		{
			SetRandomPatrolPos( Src, vPosBegin, fPatrolRange );
			return TRUE;
		}	
	}
	else if( Src.GetMoveEvent() == 99 )		//@@@@@@  8자비행 ( 하드코딩 되어 있음 )
	{
		Src.SetMovePattern( 0 );				//일반 패턴 전환
		return TRUE;
	}
	return FALSE;
}

void FLPatrolBehavior::SetRandomPatrolPos( CMover & Src, const D3DXVECTOR3 & vPosBegin, const float fPatrolRange )
{
	D3DXVECTOR3 vDest		= Src.GetPos();
	vDest.x					+= ( int )( xRand() % 21 ) - 10;		//@@@@ 기존 소스가 하드 코딩 되어있음 ㅋ
	vDest.z					+= ( int )( xRand() % 21 ) - 10;

	const float fRange		= Src.IsFlyingNPC() ? fPatrolRange * 3.0f : fPatrolRange;
	D3DXVECTOR3 vDist		= vDest - vPosBegin;
	vDist.y = 0.0f;
	if( IsInRange( vDist, fRange ) == TRUE )					// 15미터 반경을 벗어나지 못함 
	{
		SetMoveToDest( Src, vDest );
		return;
	}

	SetMoveToDest( Src, vPosBegin );
	return;
}

void FLPatrolBehavior::SetStopMoverTimeOut( CMover & Src, const DWORD tmStopInterval )
{
	m_tmStopMoverTimeOut	= ::GetTickCount() + tmStopInterval;
	Src.SetStop();
}


//----------------------------------------------------------------------------------------------------//



FLRunawayNSelfHealBehavior::FLRunawayNSelfHealBehavior()
{
	Clear();
}

FLRunawayNSelfHealBehavior::~FLRunawayNSelfHealBehavior()
{

}

void FLRunawayNSelfHealBehavior::Clear() /*AIMSG_INIT */
{
	m_bFirstRunaway			= FALSE;
	m_tmStopMoverTimeOut	= 0;
}

void FLRunawayNSelfHealBehavior::StartInit( CMover & Src, CMover & Target )
{
	// 현재의 반대 방향으로 도주 시작 
	// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
	Src.AddAngle( (float)( 180 + ( 20 - xRandom( 40 ) ) ) );

	const DWORD dwAttackRange	= Target.GetActiveHandItemProp()->dwAttackRange;
	const FLOAT fRange			= Target.GetAttackRange( dwAttackRange ) * 1.7f;
	const FLOAT fAngle			= Src.GetAngle();
	const D3DXVECTOR3 vDst		= Src.GetPos() + VelocityToVec( fAngle, fRange );
	SetMoveToDest( Src, vDst );
	m_bFirstRunaway				= TRUE; // 처음 도주인가?
}

void FLRunawayNSelfHealBehavior::RunawayUpdate( CMover & Src, CMover & Target ) /*, const D3DXVECTOR3 & vPosBegin, const float fPatrolRange ) //MoveProcessRunaway */
{
	//if( m_tmStopMoverTimeOut < tmTick && Src.m_pActMover->IsMove() == FALSE )
	if( m_tmStopMoverTimeOut > ::GetTickCount() || Src.m_pActMover->IsMove() == TRUE )
		return;

	const DWORD dwAttackRange	= Target.GetActiveHandItemProp()->dwAttackRange;
	const FLOAT fRange			= Target.GetAttackRange( dwAttackRange ) * 1.7f;

	if( IsInRange( Target.GetPos() - Src.GetPos(), fRange ) == TRUE )
	{
		// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
		Src.AddAngle( (float)( 20 - xRandom( 40 ) ) );
		// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
		const FLOAT fAngle	= Src.GetAngle();
		const D3DXVECTOR3 vDst	= Src.GetPos() + VelocityToVec( fAngle, fRange );
		SetMoveToDest( Src, vDst );
	}

	// 여기서 휴식 일정시간마다 HP를 채워준다.
	if( ::GetTickCount() > Src.m_dwTickRecovery )	// .
	{
		Src.m_dwTickRecovery= ::GetTickCount() + NEXT_TICK_RECOVERY;
		Src.IncHitPoint( Src.GetHPRecovery() );
	}
}

void FLRunawayNSelfHealBehavior::SetStopMoverTimeOut( CMover & Src, const DWORD tmStopInterval )
{
	//m_tmStopMoverTimeOut = tmStopInterval;
	m_tmStopMoverTimeOut	= ::GetTickCount() + tmStopInterval;
	Src.SetStop();
}

BOOL FLRunawayNSelfHealBehavior::IsFirstRunaway() const
{
	return m_bFirstRunaway;
}

//----------------------------------------------------------------------------------------------------//


FLAttackBehavior::FLAttackBehavior()
{
	Clear();
}

FLAttackBehavior::~FLAttackBehavior()
{

}

void FLAttackBehavior::Clear()
{
	m_tmAttackPoint			= 0;
	m_tmRageTimeOut			= 0;
}

void FLAttackBehavior::SetAttackPointTime( const DWORD tmAttackPoint )
{
	m_tmAttackPoint			= tmAttackPoint;
}

DWORD FLAttackBehavior::GetAttackPointTime() const
{
	return m_tmAttackPoint;
}

void FLAttackBehavior::SetRageTimeOutTime( const DWORD tmRangTimeOutInterval ) 
{
	m_tmRageTimeOut			= std::max( ::GetTickCount() + tmRangTimeOutInterval, tmRangTimeOutInterval );		// 공격후 추격 
	//m_tmRageTimeOut			= tmRageTimeOut;
}

DWORD FLAttackBehavior::GetRageTimeOutTime() const
{
	return m_tmRageTimeOut;
}

BOOL FLAttackBehavior::AutoChoiceRangeAttack( CMover & Src, CMover & Target )
{
	MoverProp * pProp	= Src.GetProp();


	int	nTargetJob		= 0;
	if( Target.IsPlayer() == TRUE )		nTargetJob = Target.GetJob();				// 타겟의 직업.
	const BOOL bRange	= ( pProp->m_bRangeAttack[ nTargetJob ] ) ? TRUE : FALSE;	// AI에 거리가 있으면 원거리 AI있음.
	const int nRange	= pProp->m_bRangeAttack[ nTargetJob ] & 0x7F;

	const BOOL bMelee	= ( BOOL )pProp->m_bMeleeAttack;							// AI에 근거리공격도 있는지.
	if( bRange && bMelee )															// 원거리/근거리 둘다 가지고 있음
	{
		const D3DXVECTOR3 vDist	= Target.GetPos() - Src.GetPos();					// 타겟과의 거리.
		const float fDistSq		= D3DXVec3LengthSq( &vDist );

		const int nHalfRange	= nRange / 2;										// 사거리의 반.
		if( fDistSq < ( nHalfRange * nHalfRange ) )									// 너무 가까이 붙어있어 거리유지가 히들면.
		{
			return FALSE;								// 근접공격으로 하자.
		}
		else
		{
			return TRUE;								// 거리에 여유가 있으므로 원거리 공격.
		}
	} 
	else if( bRange == TRUE && bMelee == FALSE )		// 원거리만 가지고 있음
	{
		return TRUE;
	}
	else if( bRange == FALSE && bMelee == FALSE )		
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ATTACK TYPE : None RangeAttack & None MeeleeAttack : MonsterID(%d) ]" ), Src.GetIndex() );
	}

	// 근거리만 가지고 있음
	return FALSE;
}

void FLAttackBehavior::SetMoveForRangeAttacking( CMover & Src, CMover & Target )
{
	int nTargetJob		= std::max( Target.GetJob(), 0 );
	if( nTargetJob >= MAX_JOB )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범 %d" ), nTargetJob );
		nTargetJob = 0;
	}

	MoverProp * pProp	= Src.GetProp();
	const FLOAT	fRange  = ( FLOAT )( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F );		// 사거리
	const BOOL bKeepRange = ( pProp->m_bRangeAttack[ nTargetJob ] & 0x80) ? TRUE : FALSE;	// 상위 1비트가 온? //거리유지 flag

	const D3DXVECTOR3 vTargetTo = Target.GetPos() - Src.GetPos();
	const FLOAT fDist	= ::D3DXVec3Length( &vTargetTo );		// 적과의 거리.
	if( bKeepRange )
	{
		if( ::fabs( fDist - fRange ) < 1.0f )	// 적과의 거리와 사거리의 차이가 1미터 이내면 더이상 움직일 필요 없음.
		{
			if( Src.IsEmptyDest() == FALSE )	// 이동중임.
			{
				SetMoveToDest( Src, Src.GetPos() );	// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
			}
		} 
		else
		{
			// fRange의 거리를 유지하도록 ...
			if( fDist < fRange )	
			{
				const FLOAT fAngle	= GetDegreeX( Src.GetPos(), Target.GetPos() );	// dest, src의 파라메터 위치를 바꾸면 반대방향이 나온다.
				D3DXVECTOR3 vDst	= Src.GetPos() + VelocityToVec( fAngle, 10 );
				SetMoveToDest( Src, vDst );  // 반대방향으로 달려서 거리를 벌림.
			} 
			else
			{
				SetMoveToDest( Src, Target.GetId() );	// 목표와의 거리가 사거리보다 더 머니까 근접함.
			}
		}
	} 
	else	// not keepRange
	{
		if( fDist < fRange )		// 사거리안에 있으면 제자리에 있음.
		{
			if( Src.IsEmptyDest() == FALSE )	// 이동중임.
			{
				SetMoveToDest( Src, Src.GetPos() );		// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
			}
		} 
		else
		{
			SetMoveToDest( Src, Target.GetId() );	// 사거리 밖이면 목표쪽으로 접근.
		}
	}
}

BOOL FLAttackBehavior::TryAttack( CMover & Src, CMover & Target, const BOOL bRangeAttack, const DWORD tmRageTimeoutInterval )
{
	//CMover & Src			= AI.GetMover();
	MoverProp *pProp		= Src.GetProp();
	if( pProp == NULL )
		return FALSE;

	int	nTargetJob			= 0;
	if( Target.IsPlayer() )
		nTargetJob			= std::max( Target.GetJob(), 0 );

	if( nTargetJob >= MAX_JOB )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범 %d" ), nTargetJob );
		nTargetJob = 0;
	}

	//const BOOL bRangeAttack = ( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F ) ? TRUE : FALSE;	// 장거리 공격 AI가 있냐.

	//FLAggro::SumFirstTargetAggro( Src, Target.GetId() );

	const DWORD dwAtkMethod		= ( bRangeAttack ) ? GetAtkMethod_Far( Src ) : GetAtkMethod_Near( Src );
	const DWORD dwAtkRange		= GetAtkRange( Src, dwAtkMethod );
	// 장거리 공격 선택이라면?
	if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
	{
		// 유효 거리 판단
		const FLOAT fRange		= ( float )( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F ) + 1.0f;		// 사거리 +1미터 정도는 걍 쏜다.

		const D3DXVECTOR3 vTargetTo = Src.GetPos() - Target.GetPos();	
		if( IsInRange( vTargetTo, fRange ) == TRUE )		// AI에서 지정한 공격거리까지 다가갔는가?
		{
			DoAttack( Src, Target, dwAtkMethod, tmRageTimeoutInterval );	//@@@@@@@@@@@@@@@@@@@@@@@@ 공격		
			m_tmAttackPoint		= ::GetTickCount() + SEC( 3 );		// 장거리 상태이다. 3초 공격 딜레이 설정.
		}
	} 
	else
	{
		const FLOAT fRange	= Src.GetAttackRange( dwAtkRange );
		if( Src.IsRangeObj( &Target, fRange ) )			// 3D 거리 체크?
		{
			DoAttack( Src, Target, dwAtkMethod, tmRageTimeoutInterval );	//@@@@@@@@@@@@@@@@@@@@@@@@ 공격	
			//			m_bGoTarget = TRUE;  // <- 이건 뭐하는거? 
			// 공격을 했으면 어택 딜레이 리셋.
			const int nDelay	= (int)pProp->dwReAttackDelay - SEC( 1 ) + xRandom( SEC( 2 ) );
			m_tmAttackPoint		= ::GetTickCount() + nDelay;
			FLTRACE_LOG( PROGRAM_NAME, _T( "AtkDelay:%d" ), (int)nDelay );
		}
	}
	// 근접공격은 ARRIVAL에서 처리한다. 여기서 처리해도 좋지 않을까?

	return TRUE;
}

BOOL FLAttackBehavior::DoAttack( CMover & Src, CMover & Target, const DWORD dwAttackMethod, const DWORD tmRageTimeoutInterval )
{
	//CMover & Src			= AI.GetMover();
	if( IsInvalidObj( &Src ) || dwAttackMethod == 0 )	
		return FALSE;

	const DWORD dwAtkItemID = GetAtkItemId( Src, dwAttackMethod );
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwAtkItemID );

	if( pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s %d %d" ), Src.GetName(), dwAttackMethod, dwAtkItemID );
		return FALSE;
	}

	if( pItemProp->dwAttackRange == AR_RANGE || pItemProp->dwAttackRange == AR_WAND )
	{
		// 장거리 공격은, 선택된 공격 방식(dwAtkMethod)의 해당 공격 아이템 속성에서 속성을 얻어 장거리 공격이라고 판단함.
		// 서버에서 idSfx를 만들어서 자기자신에게 등록시켜둠. 나중에 클라에서 맞아서 서버로 정보가 넘어오면 그것과 비교해서 데미지를 넘겨줌.
		const int idSfxHit	= Src.m_sfxHitArray.Add( 0, Target.GetId(), AF_GENERIC, 0 );	// 이부분은 DoAttackRange안으로 들어가야 할거 같다.
		if( dwAtkItemID == NULL_ID )
			FLERROR_LOG( PROGRAM_NAME, _T( "경고 : %s의 dwAtk2가 지정되지 않았다" ), Src.GetName() );

		if( Src.DoAttackRange( &Target, dwAtkItemID, idSfxHit ) != 1 )
			Src.m_sfxHitArray.RemoveSfxHit( idSfxHit, FALSE );	// sfxHitArray.Add넣었던것을 다시뺌. 
	}
	else
	{
		// 필살 공격은, 프로퍼티에서 정보를 얻어올 수 없으므로 공격 방식(dwAtkMethod)으로 결정.
		// 필살 공격 루틴은 내부에서 단거리 공격을 할 수도 있다.
		if( dwAtkItemID == NULL_ID )
			FLERROR_LOG( PROGRAM_NAME, _T( "경고 : %s의 공격%d가 지정되지 않았다" ), Src.GetName(), dwAttackMethod );
		if( dwAttackMethod == 3 )
			Src.DoAttackSP( &Target, dwAtkItemID );
		else
		{
			Src.DoAttackMelee( &Target, OBJMSG_ATK1 );
 			//int nError = Src.SendActMsg( OBJMSG_ATK1, Target.GetId(), 0, 0 );
 			//if( nError > 0 ) 
 			//	g_xWSUserManager->AddMeleeAttack( &Src, OBJMSG_ATK1, Target.GetId(), 0, MAKELONG( 0, (WORD)nError ) );
		}
	}

	//m_tmRageTimeOut			= ::GetTickCount() + tmRageTimeoutInterval;		// 공격 후 s_tmAttack초간 추적해옴.
	m_tmRageTimeOut			= std::max( ::GetTickCount() + tmRageTimeoutInterval, tmRageTimeoutInterval );		// 공격후 추격 
	return TRUE;
}

DWORD FLAttackBehavior::GetAtkRange( CMover & Src, const DWORD dwAtkMethod )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( GetAtkItemId( Src, dwAtkMethod ) );
#ifdef _DEBUG
	if( pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ItemProp->dwAtk%d이 NULL이다. (%s)" ), dwAtkMethod, Src.GetName() );
	}
#endif
	return pItemProp->dwAttackRange;
}

DWORD FLAttackBehavior::GetAtkMethod_Near( CMover & Src )
{
//	MoverProp* pMoverProp = Src.GetProp();
	const DWORD dwRandom = xRandom( 100 );
	const int nDeathblow = (int)( Src.GetHitPointPercent() / 4.5f );
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}

	switch( Src.m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 60 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 80 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	}
	return 1;
}

DWORD FLAttackBehavior::GetAtkMethod_Far( CMover & Src )
{
//	MoverProp* pMoverProp = Src.GetProp();
	const DWORD dwRandom = xRandom( 100 );

	// 필살 판정 성공!
	const int nDeathblow = (int)( Src.GetHitPointPercent() / 4.5f );
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}

	switch( Src.m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 30 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 20 )
			return 1;
		return 2;
	}
	return 1;
}

DWORD FLAttackBehavior::GetAtkItemId( CMover & Src, const DWORD dwAtkMethod )
{
	switch( dwAtkMethod )
	{ 
	case 1:
		return Src.GetProp()->dwAtk1;
	case 2:
		return Src.GetProp()->dwAtk2;
	case 3:
		return Src.GetProp()->dwAtk3;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------//

FLSummonMonsterBehavior::FLSummonMonsterBehavior()
{
	Clear();
}

FLSummonMonsterBehavior::~FLSummonMonsterBehavior()
{
}

void FLSummonMonsterBehavior::Clear()
{
	m_tmSummonPoint			= ::GetTickCount() + SEC( 20 );
}

void FLSummonMonsterBehavior::TrySummonUpdate( CMover & Src )
{
	if( ::GetTickCount() < m_tmSummonPoint )
		return;

	MoverProp * pProp		= Src.GetProp();
	if( pProp == NULL )
		return;

	m_tmSummonPoint			= ::GetTickCount() + SEC( 20 );

	for( SummonVector::iterator it = m_vecCalledObjID.begin(); it != m_vecCalledObjID.end(); )		//m_vecIdSummon
	{
		CMover * pZako		= prj.GetMover( *it );
		if( IsValidObj( pZako ) && pZako->IsLive() )
		{
			++it;
		}
		else
		{
			it				= m_vecCalledObjID.erase( it );			//@@@@@@@@ 대입 해야하나?
		}
	}

	if( pProp->m_nSummNum <= ( int )m_vecCalledObjID.size() )
		return;

	if( (int)( xRandom(100) ) < pProp->m_nSummProb )	// 소환 시도.
	{
		CObj * pObj			= CreateObj( D3DDEVICE, OT_MOVER, (DWORD)pProp->m_nSummID );
		if( NULL == pObj )	
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "소환 실패 %d" ), pProp->m_nSummID );
			return;
		}

		D3DXVECTOR3 vLocal;
		vLocal.x			= 1.5f - xRandomF( 3.0f );		// 보스 주위에 랜덤으로 생성.
		vLocal.y			= 0;
		vLocal.z			= 1.5f - xRandomF( 3.0f );

		D3DXVECTOR3 vPos	= Src.GetPos();
		vPos				+= vLocal;

		pObj->SetPos( vPos );
		pObj->InitMotion( MTI_STAND );
		pObj->UpdateLocalMatrix();
		( ( CMover * )pObj )->m_bActiveAttack = TRUE;	// 부하는 선공 형으로 소환하자.
		( ( CMover * )pObj )->AddItToGlobalId();
		if( Src.GetWorld()->ADDOBJ( pObj, FALSE, Src.GetLayer() ) == TRUE )
		{
			m_vecCalledObjID.push_back( ( ( CMover * )pObj )->GetId() );
			g_xWSUserManager->AddDlgEmoticon( &Src, DLGEMOT_SUMMON );
		}
		else
		{
			SAFE_DELETE( pObj );
		}
	}
}

//----------------------------------------------------------------------------------------------------//




FLCallHelperBehavior::FLCallHelperBehavior()
{
	Clear();
}

FLCallHelperBehavior::~FLCallHelperBehavior()
{

}

void FLCallHelperBehavior::Clear()
{
	m_tmHelpPoint		= 0;
	m_bStopCallHelper	= FALSE;
}

void FLCallHelperBehavior::TryCallHelper( CMover & Src, CMover & Target )
{
	if( IsValidObj( &Src ) == FALSE || Src.IsDie() )
		return;
	if( IsValidObj( &Target ) == FALSE || Target.IsDie() )
		return;

	MoverProp * pProp	= Src.GetProp();
	if( pProp == NULL )
		return;

	if( m_tmHelpPoint > ::GetTickCount() + pProp->m_tmUnitHelp )
		return;

	if( m_bStopCallHelper == TRUE )
		return;

	// 범위 계산		
	int nRange				= 20;
	switch( pProp->m_nHelpRangeMul )
	{
	case 0:		nRange		= pProp->m_nAttackFirstRange / 2;	break;
	default:	nRange		= pProp->m_nAttackFirstRange * pProp->m_nHelpRangeMul;	break;	// 시야범위의 배수.
	}


	// 도와달라고 할 몬스터 수집.
	CObj* pObj;

	const size_t MAX_ARRAY			= 40;
	CMover* apMonster[ MAX_ARRAY ][ 5 ];
	int anCountMonster[ 5 ]			= { 0 };
	const D3DXVECTOR3 vCenterPos	= Src.GetPos();

	FOR_LINKMAP( Src.GetWorld(), vCenterPos, pObj, nRange / MPU, CObj::linkDynamic, Src.GetLayer() )
	{
		if( pObj != static_cast< CObj * >( &Src ) && pObj->GetType() == OT_MOVER )
		{
			// FOR_LINKMAP 자체가 느리기 때문에 이 루프를 안쪽에 넣었다.
			for( int i = 0; i < pProp->m_nCallHelperMax; i++ )
			{
				//int nCallHelperIdx = Src.GetProp()->m_nCallHelperIdx[ i ];
				const int nCallHelperNum = Src.GetProp()->m_nCallHelperNum[ i ];
				// 아무나 부르는것이거나 || 같은 종족만 부르는 옵션일때 같은 인덱스인가?
				// 그리고.............
				if( ( pProp->m_bHelpWho == 1 || (pProp->m_bHelpWho == 2 && pObj->GetIndex() == Src.GetIndex() ) ) 
					&& ( anCountMonster[ i ] < nCallHelperNum || nCallHelperNum == 0 )	)	
				{
					if( pObj->m_pFSM != NULL )
					{
						apMonster[ anCountMonster[ i ] ][ i ] = (CMover*)pObj;
						anCountMonster[ i ]++;
						if( anCountMonster[ i ] >= MAX_ARRAY ) 
							goto NEXT;
					}
				}
			}
		}
	}
	END_LINKMAP

NEXT:
	// 파티원 정보 수집.
	int nCountPlayer				= 0;
	CMover* apPlayer[ MAX_ARRAY ]	= { 0 };
	BOOL bParty						= FALSE;

	CParty * pParty					= NULL;
	// 몬스터 파티 공격 ? 가능 여부 
	for( int i = 0; i < pProp->m_nCallHelperMax; i++ )
		bParty |= pProp->m_bCallHelperParty[ i ] ? TRUE : FALSE;

	if( bParty && IsValidObj( &Target ) )
	{
		pParty = g_PartyMng.GetParty( Target.m_idparty );
		// 파티가 확실하다면 파티원을 apPlayer에 수집한다.
		if( pParty )
		{
			FOR_LINKMAP( Src.GetWorld(), vCenterPos, pObj, 20 / MPU, CObj::linkPlayer, Src.GetLayer() )
			{
				if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && pParty->IsMember( ((CMover*)pObj)->m_idPlayer ) )
				{
					apPlayer[ nCountPlayer++ ] = (CMover*)pObj;
					if( nCountPlayer >= MAX_ARRAY )
						goto NEXT2;
				}
			}
			END_LINKMAP
		}
	}

NEXT2:
	for( int i = 0; i < pProp->m_nCallHelperMax; i++ )
	{
		if( pParty )
		{
			if( nCountPlayer )
			{
				for( int j = 0; j < anCountMonster[ i ]; j++ )
				{
					// 각각의 몬스터에게 타겟을 랜덤으로 배정한다. 
					CMover* pNewTarget = (CMover*)apPlayer[ rand() % nCountPlayer ];
					if( pNewTarget )
						apMonster[ j ][ i ]->PostAIMsg( AIMSG_REQUEST_HELP, pNewTarget->GetId(), 0 );		//RAGE 상태로 만듬
				}
			}
		}
		else
		{ 
			for( int j = 0; j < anCountMonster[ i ]; j++ )
				apMonster[ j ][ i ]->PostAIMsg( AIMSG_REQUEST_HELP, Target.GetId(), 0 );					//RAGE 상태로 만듬
		}
	}


	g_xWSUserManager->AddDlgEmoticon( &Src, DLGEMOT_HELPER );

	m_tmHelpPoint		= ::GetTickCount() + pProp->m_tmUnitHelp;
	if( pProp->m_tmUnitHelp == 0 )		// 0이면 한번만 부르고 안부름.
		m_bStopCallHelper = TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------------//


void FLSetRandomPos( CMover & Src )
{
	D3DXVECTOR3 vDest		= Src.GetPos();
	vDest.x					+= ( int )( xRand() % 21 ) - 10;		//@@@@ 기존 소스가 하드 코딩 되어있음 ㅋ
	vDest.z					+= ( int )( xRand() % 21 ) - 10;

	SetMoveToDest( Src, vDest );
}


//-------------------------------------------------------------------------------------------------//

BOOL				
IsInRange( const D3DXVECTOR3 & vDist, const float fRange )
{
	D3DXVECTOR3 dist	= vDist;
	dist.y				= 0.0f;
	const float fDist	= ::D3DXVec3LengthSq( & dist );
	return ( fDist > fRange * fRange ) ? FALSE : TRUE;
}

void				
SetMoveToDest( CMover & Src, const D3DXVECTOR3 & vDest )		//TRUE : 도착
{
	//CMover::ProcessMover()에서 처리됨 
	Src.SetDestPos( vDest );
	g_xWSUserManager->AddSetDestPos( &Src, vDest, 1 );
}

void				
SetMoveToDest( CMover & Src, const OBJID dwOBJID )
{
	if( Src.GetDestId() == dwOBJID )
		return;

	Src.SetSpeedFactor( 2.0F );

	//CMover * pTarget	= prj.GetMover( dwOBJID );
	//if( IsValidObj( pTarget ) == TRUE )
	//	SetTaget( pTarget->GetId(), pTarget->m_idparty );


	Src.SetDestObj( dwOBJID, 0.0f, TRUE );

//	g_xWSUserManager->AddMoverSetDestObj( &Src, dwOBJID );
}

void				
SetReturnToBegin( CMover & Src, const D3DXVECTOR3 & vPosBegin )
{
	//Src.SetTarget( NULL_ID, 0 );
	//Src.AIInit();

	Src.SetSpeedFactor( 2.66F );
	SetMoveToDest( Src, vPosBegin );
}

void				
SetMaxHP( CMover & Src )
{
	Src.SetPointParam( DST_HP, Src.GetMaxHitPoint() );
	//Src.RemoveAllEnemies(); //자기 때린 리스트
}










//CMover * TryScanValidTargetByAggroFSM( FLAggro & AggroMng )
//{
//	//	if( AggroMng.GetOwnerObjID() == NULL_ID )
//	//		return NULL;
//
//	CMover * pAggroOwner		= prj.GetMover( AggroMng.GetOwnerObjID() );
//	if( IsValidObj( pAggroOwner ) == FALSE )
//		return NULL;
//
//	OBJID aggroOBJID;
//	if( AggroMng.GetMaxAggro( aggroOBJID ) == TRUE )
//	{
//		CMover * pAggroTarget	= prj.GetMover( aggroOBJID );
//		if( IsValidObj( (CObj*)pAggroTarget ) == FALSE ||
//			pAggroTarget->GetWorld()->GetID() != pAggroOwner->GetWorld()->GetID() ||
//			pAggroTarget->IsDie() == TRUE ||
//			pAggroTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) == TRUE ||										// 변신 중이라면 
//			pAggroTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) == TRUE ||		// 은신 중이라면
//			pAggroTarget->IsRegionAttr( RA_SAFETY ) == TRUE ||												// 안전 영역이라면
//			pAggroTarget->IsFly() != pAggroOwner->IsFlyingNPC()												// 한쪽이 날고 있다면.
//			)		
//		{
//			AggroMng.RemoveMover( /**pAggroOwner,*/ aggroOBJID );
//		}
//		else
//		{
//			return pAggroTarget;
//		}
//	}
//
//	return NULL;
//}


BOOL	FLScanTarget::IsValidTarget( CMover & AIMover, CMover & Target, const FLAIOptionalFlags & kOptionalFlags )
{
	if( IsValidObj( &Target ) == FALSE )
		return FALSE;

	if( Target.GetWorld()->GetID() != AIMover.GetWorld()->GetID() )
		return FALSE;

	if( Target.IsDie() == TRUE )
		return FALSE;

	// 무적
	//if( kOptionalFlags.bIgnoreMatchless == FALSE )
	//{
	//	if( Target.m_dwMode & ( MATCHLESS_MODE | MATCHLESS2_MODE ) )
	//		return FALSE;
	//}

	// 은신
	if( kOptionalFlags.bIgnoreTransparent == FALSE )
	{
		if( Target.HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) == TRUE )
			return FALSE;

		if( Target.m_dwMode & TRANSPARENT_MODE )
			return FALSE;
	}

	if( Target.IsFly() == TRUE )
		return FALSE;

	if( Target.HasBuffByIk3( IK3_TEXT_DISGUISE ) == TRUE )										// 변신 중이라면
		return FALSE;

	if(	Target.IsRegionAttr( RA_SAFETY ) == TRUE )											// 안전 영역이라면
		return FALSE;

	return TRUE;
}

CMover * FLScanTarget::Stalking( CMover & AIMover, const OBJID objid, const FLAIOptionalFlags & kOptionalFlags )
{
	CMover * pTarget		= prj.GetMover( objid );
	if( IsValidObj( pTarget ) == FALSE )
		return NULL;

	if( IsValidTarget( AIMover, *pTarget, kOptionalFlags ) == FALSE )
		return NULL;

	return pTarget;
}

//CMover *				
//ScanTargetByNear( CMover & Behavior, CObj & CenterObj, const int nRangeMeter/*, const E_AI_TARGET_TYPE eTargetType*/ )
//{
//	const D3DXVECTOR3 vCenter	= CenterObj.GetPos();
//
//	CObj * pObj;
//	CMover * pTarget		= NULL;
//	const float fRadius		= ( float )( nRangeMeter * nRangeMeter );
//	D3DXVECTOR3 vDist;
//
//	FOR_LINKMAP( CenterObj.GetWorld(), vCenter, pObj, nRangeMeter, CObj::linkPlayer, Behavior.GetLayer() )
//	{
//		FLASSERT( pObj->GetType() == OT_MOVER && static_cast< CMover * >( pObj )->IsPlayer() );
//		pTarget				= static_cast< CMover * >( pObj );
//		vDist			= pTarget->GetPos() - vCenter;
//		if( D3DXVec3LengthSq( & vDist ) < fRadius )
//		{
//			if( IsValidObj( (CObj*)pTarget ) == TRUE &&
//				pTarget->IsDie() == FALSE &&
//				pTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) == FALSE &&										// 변신 중이라면 
//				pTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) == FALSE &&		// 은신 중이라면
//				pTarget->IsRegionAttr( RA_SAFETY ) == FALSE &&// 안전 영역이라면
//				pTarget->IsFly() == Behavior.IsFlyingNPC() &&
//				pTarget->IsMode( TRANSPARENT_MODE ) == FALSE )	//투명화 상태?
//			{
//				return pTarget;
//			}
//		}
//	}
//	END_LINKMAP
//		return NULL;
//}

CMover * FLScanTarget::ByNear( CMover & Behavior, CObj & CenterObj, const int nRangeMeter, const FLAIOptionalFlags & kOptionalFlags  )
{
	const D3DXVECTOR3 vCenter	= CenterObj.GetPos();

	CObj * pObj;
	CMover * pTarget		= NULL;
	const float fRadius		= ( float )( nRangeMeter * nRangeMeter );
	D3DXVECTOR3 vDist;

	FOR_LINKMAP( CenterObj.GetWorld(), vCenter, pObj, nRangeMeter, CObj::linkPlayer, Behavior.GetLayer() )
	{
		FLASSERT( pObj->GetType() == OT_MOVER && static_cast< CMover * >( pObj )->IsPlayer() );
		pTarget				= static_cast< CMover * >( pObj );
		vDist			= pTarget->GetPos() - vCenter;
		if( D3DXVec3LengthSq( & vDist ) < fRadius )
		{
			if( IsValidObj( pTarget ) == TRUE 
				|| IsValidTarget( Behavior, *pTarget, kOptionalFlags ) == TRUE )
			{
				return pTarget;
			}
		}
	}
	END_LINKMAP
		return NULL;
}






CMover * FLScanTarget::ByAggro( CMover & AIMover, const FLAIOptionalFlags & kOptionalFlags )
{
	if( AIMover.GetAggroMng() == NULL )
		return NULL;

	OBJID aggroOBJID;
	if( AIMover.GetAggroMng()->GetMaxAggro( aggroOBJID ) == TRUE )
	{
		CMover * pAggroTarget	= prj.GetMover( aggroOBJID );
		if( IsValidObj( (CObj*)pAggroTarget ) == FALSE 
			|| IsValidTarget( AIMover, *pAggroTarget, kOptionalFlags ) == FALSE )
		{
			AIMover.GetAggroMng()->RemoveMover( aggroOBJID );
		}
		else
		{
			return pAggroTarget;
		}
	}

	return NULL;
}

//CMover * TryScanValidTargetByAggro( CMover & AggroOwner )
//{
//	//어그로 객체를 찾고 
//	if( AggroOwner.m_pAIInterface == NULL || AggroOwner.m_pAIInterface->GetAggroMng() == NULL )
//		return NULL;
//
//	FLAggro & AggroMng		= * AggroOwner.m_pAIInterface->GetAggroMng(); 
//
//
//	OBJID aggroOBJID;
//	if( AggroMng.GetMaxAggro( aggroOBJID ) == TRUE )
//	{
//		CMover * pAggroTarget	= prj.GetMover( aggroOBJID );
//		if( IsValidObj( (CObj*)pAggroTarget ) == FALSE ||
//			pAggroTarget->GetWorld()->GetID() != AggroOwner.GetWorld()->GetID() ||
//			pAggroTarget->IsDie() == TRUE ||
//			pAggroTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) == TRUE ||										// 변신 중이라면 
//			pAggroTarget->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) == TRUE ||		// 은신 중이라면
//			pAggroTarget->IsRegionAttr( RA_SAFETY ) == TRUE ||												// 안전 영역이라면
//			pAggroTarget->IsFly() != AggroOwner.IsFlyingNPC()													// 한쪽이 날고 있다면.
//			)		
//		{
//			AggroMng.RemoveMover( /*AggroOwner,*/ aggroOBJID );
//		}
//		else
//		{
//			return pAggroTarget;
//		}
//	}
//
//	return NULL;
//}


//CMover * TryScanValidTargetByAggroFSM( const FLAIOptionalFlags & kOptionalFlags, FLAggro & AggroMng )
//{
//	CMover * pAggroOwner	= prj.GetMover( AggroMng.GetOwnerObjID() );
//	if( IsValidObj( pAggroOwner ) == FALSE )
//		return NULL;
//
//	OBJID aggroOBJID;
//	if( AggroMng.GetMaxAggro( aggroOBJID ) == TRUE )
//	{
//		CMover * pAggroTarget	= prj.GetMover( aggroOBJID );
//		if( IsValidObj( (CObj*)pAggroTarget ) == FALSE 
//			|| IsValidTarget( kOptionalFlags, *pAggroOwner, *pAggroTarget ) == FALSE )
//		{
//			AggroMng.RemoveMover( /**pAggroOwner,*/ aggroOBJID );
//		}
//		else
//		{
//			return pAggroTarget;
//		}
//	}
//
//	return NULL;
//}

//BOOL					AutoChoiceRangeAttack( CMover & Src, CMover & Target )			//SelectAttackType
//{
//	MoverProp * pProp	= Src.GetProp();
//	
//
//	int	nTargetJob		= 0;
//	if( Target.IsPlayer() == TRUE )		nTargetJob = Target.GetJob();				// 타겟의 직업.
//	const BOOL bRange	= ( pProp->m_bRangeAttack[ nTargetJob ] ) ? TRUE : FALSE;	// AI에 거리가 있으면 원거리 AI있음.
//	const int nRange	= pProp->m_bRangeAttack[ nTargetJob ] & 0x7F;
//
//	const BOOL bMelee	= ( BOOL )pProp->m_bMeleeAttack;							// AI에 근거리공격도 있는지.
//	if( bRange && bMelee )															// 원거리/근거리 둘다 가지고 있음
//	{
//		const D3DXVECTOR3 vDist	= Target.GetPos() - Src.GetPos();					// 타겟과의 거리.
//		const float fDistSq		= D3DXVec3LengthSq( &vDist );
//
//		const int nHalfRange	= nRange / 2;										// 사거리의 반.
//		if( fDistSq < ( nHalfRange * nHalfRange ) )									// 너무 가까이 붙어있어 거리유지가 히들면.
//		{
//			return FALSE;								// 근접공격으로 하자.
//		}
//		else
//		{
//			return TRUE;								// 거리에 여유가 있으므로 원거리 공격.
//		}
//	} 
//	else if( bRange == TRUE && bMelee == FALSE )		// 원거리만 가지고 있음
//	{
//		return TRUE;
//	}
//	else if( bRange == FALSE && bMelee == FALSE )		// 원거리만 가지고 있음
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ATTACK TYPE : None RangeAttack & None MeeleeAttack : MonsterID(%d) ]" ), Src.GetIndex() );
//	}
//
//	// 근거리만 가지고 있음
//	return FALSE;
//}

//void						SetMoveForRangeAttacking( CMover & Src, CMover & Target )
//{
//	int nTargetJob		= std::max( Target.GetJob(), 0 );
//	if( nTargetJob >= MAX_JOB )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범 %d" ), nTargetJob );
//		nTargetJob = 0;
//	}
//
//	MoverProp * pProp	= Src.GetProp();
//	const FLOAT	fRange  = ( FLOAT )( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F );		// 사거리
//	const BOOL bKeepRange = ( pProp->m_bRangeAttack[ nTargetJob ] & 0x80) ? TRUE : FALSE;	// 상위 1비트가 온? //거리유지 flag
//
//	const D3DXVECTOR3 vTargetTo = Target.GetPos() - Src.GetPos();
//	const FLOAT fDist	= ::D3DXVec3Length( &vTargetTo );		// 적과의 거리.
//	if( bKeepRange )
//	{
//		if( ::fabs( fDist - fRange ) < 1.0f )	// 적과의 거리와 사거리의 차이가 1미터 이내면 더이상 움직일 필요 없음.
//		{
//			if( Src.IsEmptyDest() == FALSE )	// 이동중임.
//				SetMoveToDest( Src, Src.GetPos() );	// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
//		} 
//		else
//		{
//			// fRange의 거리를 유지하도록 ...
//			if( fDist < fRange )	
//			{
//				const FLOAT fAngle	= GetDegreeX( Src.GetPos(), Target.GetPos() );	// dest, src의 파라메터 위치를 바꾸면 반대방향이 나온다.
//				D3DXVECTOR3 vDst	= Src.GetPos() + VelocityToVec( fAngle, 10 );
//				SetMoveToDest( Src, vDst );  // 반대방향으로 달려서 거리를 벌림.
//			} 
//			else
//				SetMoveToDest( Src, Target.GetId() );	// 목표와의 거리가 사거리보다 더 머니까 근접함.
//		}
//	} 
//	else	// not keepRange
//	{
//		if( fDist < fRange )		// 사거리안에 있으면 제자리에 있음.
//		{
//			if( Src.IsEmptyDest() == FALSE )	// 이동중임.
//				SetMoveToDest( Src, Src.GetPos() );		// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
//		} else
//			SetMoveToDest( Src, Target.GetId() );	// 사거리 밖이면 목표쪽으로 접근.
//	}
//}

BOOL					RecvConditionUpdate( CMover & Src )
{
	MoverProp * pProp	= Src.GetProp();
	if( pProp == NULL )
		return FALSE;

	if( pProp->m_bRecvCond == FALSE )
		return FALSE;

	if( Src.GetManaPoint() < 0 )
		return FALSE;


	if( Src.GetHitPointPercent( 100 ) <= pProp->m_nRecvCondMe )	// HP가 m_nRecvCondMe% 이하로 떨어졌는가?
	{
		// m_nHow% 만큼의 HP를 회복
		const int nHP		= ( int )( Src.GetMaxHitPoint() * ( pProp->m_nRecvCondHow / 100.0f ) );	
		Src.IncHitPoint( nHP );

		g_xWSUserManager->AddCreateSfxObj( &Src, XI_INDEX( 62, XI_GEN_CURE01 ), Src.GetPos().x, Src.GetPos().y, Src.GetPos().z );	

		if( pProp->m_nRecvCondMP > 0 )
		{
			const int nMP	= ( pProp->m_nRecvCondMP > 0 ) ? pProp->m_nRecvCondMP * -1 : Src.GetMaxManaPoint();	// 0으로 지정되어 있으면 몽땅 소모.		
			Src.IncManaPoint( nMP );
		}
		return TRUE;
	}
	return FALSE;
}



//#undef max
//BOOL		TryAttack( CMover & Src, CMover & Target, const BOOL bRangeAttack, const DWORD tmRageTimeout )	//SubAttackChance
//{
//	//CMover & Src			= AI.GetMover();
//	MoverProp *pProp		= Src.GetProp();
//	if( pProp == NULL )
//		return FALSE;
//
//	int	nTargetJob			= 0;
//	if( Target.IsPlayer() )
//		nTargetJob			= std::max( Target.GetJob(), 0 );
//
//	if( nTargetJob >= MAX_JOB )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범 %d" ), nTargetJob );
//		nTargetJob = 0;
//	}
//
//	//const BOOL bRangeAttack = ( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F ) ? TRUE : FALSE;	// 장거리 공격 AI가 있냐.
//
//	FLAggro::SumFirstTargetAggro( Src, Target.GetId() );
//
//	const DWORD dwAtkMethod		= ( bRangeAttack ) ? GetAtkMethod_Far( Src ) : GetAtkMethod_Near( Src );
//	const DWORD dwAtkRange		= GetAtkRange( Src, dwAtkMethod );
//	// 장거리 공격 선택이라면?
//	if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
//	{
//		// 유효 거리 판단
//		const FLOAT fRange		= ( float )( pProp->m_bRangeAttack[ nTargetJob ] & 0x7F ) + 1.0f;		// 사거리 +1미터 정도는 걍 쏜다.
//
//		const D3DXVECTOR3 vTargetTo = Src.GetPos() - Target.GetPos();	
//		if( IsInRange( vTargetTo, fRange ) == TRUE )		// AI에서 지정한 공격거리까지 다가갔는가?
//		{
//			DoAttack( Src, Target, dwAtkMethod, tmRageTimeout );	//@@@@@@@@@@@@@@@@@@@@@@@@ 공격		
//			Src.m_AIMonsterData.tmAttackPoint	= ::GetTickCount() + SEC( 3 );		// 장거리 상태이다. 3초 공격 딜레이 설정.
//		}
//	} 
//	else
//	{
//		const FLOAT fRange	= Src.GetAttackRange( dwAtkRange );
//		if( Src.IsRangeObj( &Target, fRange ) )			// 3D 거리 체크?
//		{
//			DoAttack( Src, Target, dwAtkMethod, tmRageTimeout );	//@@@@@@@@@@@@@@@@@@@@@@@@ 공격	
////			m_bGoTarget = TRUE;  // <- 이건 뭐하는거? 
//			// 공격을 했으면 어택 딜레이 리셋.
//			const int nDelay	= (int)pProp->dwReAttackDelay - SEC( 1 ) + xRandom( SEC( 2 ) );
//			Src.m_AIMonsterData.tmAttackPoint	= ::GetTickCount() + nDelay;
//			FLTRACE_LOG( PROGRAM_NAME, _T( "AtkDelay:%d" ), (int)nDelay );
//		}
//	}
//	// 근접공격은 ARRIVAL에서 처리한다. 여기서 처리해도 좋지 않을까?
//
//	return TRUE;
//}

//BOOL						DoAttack( CMover & Src, CMover & Target, const DWORD dwAttackMethod, const DWORD tmRageTimeout )
//{
//	//CMover & Src			= AI.GetMover();
//	if( IsInvalidObj( &Src ) || dwAttackMethod == 0 )	
//		return FALSE;
//
//	const DWORD dwAtkItemID = GetAtkItemId( Src, dwAttackMethod );
//	ItemProp* pItemProp		= prj.GetItemProp( dwAtkItemID );
//	if( pItemProp == NULL )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s %d %d" ), Src.GetName(), dwAttackMethod, dwAtkItemID );
//		return FALSE;
//	}
//
//	if( pItemProp->dwAttackRange == AR_RANGE || pItemProp->dwAttackRange == AR_WAND )
//	{
//		// 장거리 공격은, 선택된 공격 방식(dwAtkMethod)의 해당 공격 아이템 속성에서 속성을 얻어 장거리 공격이라고 판단함.
//		// 서버에서 idSfx를 만들어서 자기자신에게 등록시켜둠. 나중에 클라에서 맞아서 서버로 정보가 넘어오면 그것과 비교해서 데미지를 넘겨줌.
//		const int idSfxHit	= Src.m_sfxHitArray.Add( 0, Target.GetId(), AF_GENERIC, 0 );	// 이부분은 DoAttackRange안으로 들어가야 할거 같다.
//		if( dwAtkItemID == NULL_ID )
//			FLERROR_LOG( PROGRAM_NAME, _T( "경고 : %s의 dwAtk2가 지정되지 않았다" ), Src.GetName() );
//
//		if( Src.DoAttackRange( &Target, dwAtkItemID, idSfxHit ) != 1 )
//			Src.m_sfxHitArray.RemoveSfxHit( idSfxHit, FALSE );	// sfxHitArray.Add넣었던것을 다시뺌. 
//	}
//	else
//	{
//		// 필살 공격은, 프로퍼티에서 정보를 얻어올 수 없으므로 공격 방식(dwAtkMethod)으로 결정.
//		// 필살 공격 루틴은 내부에서 단거리 공격을 할 수도 있다.
//		if( dwAtkItemID == NULL_ID )
//			FLERROR_LOG( PROGRAM_NAME, _T( "경고 : %s의 공격%d가 지정되지 않았다" ), Src.GetName(), dwAttackMethod );
//		if( dwAttackMethod == 3 )
//			Src.DoAttackSP( &Target, dwAtkItemID );
//		else
//		{
//			int nError = Src.SendActMsg( OBJMSG_ATK1, Target.GetId(), 0, 0 );
//			if( nError > 0 ) 
//				g_xWSUserManager->AddMeleeAttack( &Src, OBJMSG_ATK1, Target.GetId(), 0, MAKELONG( 0, (WORD)nError ) );
//		}
//	}
//
//	Src.m_AIMonsterData.tmRageTimeOut			= ::GetTickCount() + tmRageTimeout;		// 공격 후 s_tmAttack초간 추적해옴.
//	return TRUE;
//}


//DWORD					GetAtkMethod_Near( CMover & Src )
//{
//	MoverProp* pMoverProp = Src.GetProp();
//	const DWORD dwRandom = xRandom( 100 );
//	const int nDeathblow = (int)( Src.GetHitPointPercent() / 4.5f );
//	if( nDeathblow )
//	{
//		if( !xRandom( nDeathblow ) )
//			return 3;
//	}
//
//	switch( Src.m_dwBelligerence )
//	{
//	case BELLI_MELEE2X:
//		if( dwRandom < 60 )
//			return 1;
//		return 2;
//		break;
//	case BELLI_MELEE:
//		if( dwRandom < 80 )
//			return 1;
//		return 2;
//		break;
//	case BELLI_RANGE:
//		if( dwRandom < 50 )
//			return 1;
//		return 2;
//		break;
//	}
//	return 1;
//}


//DWORD				GetAtkMethod_Far( CMover & Src )
//{
//	MoverProp* pMoverProp = Src.GetProp();
//	const DWORD dwRandom = xRandom( 100 );
//
//	// 필살 판정 성공!
//	const int nDeathblow = (int)( Src.GetHitPointPercent() / 4.5f );
//	if( nDeathblow )
//	{
//		if( !xRandom( nDeathblow ) )
//			return 3;
//	}
//
//	switch( Src.m_dwBelligerence )
//	{
//	case BELLI_MELEE2X:
//		if( dwRandom < 50 )
//			return 1;
//		return 2;
//		break;
//	case BELLI_MELEE:
//		if( dwRandom < 30 )
//			return 1;
//		return 2;
//		break;
//	case BELLI_RANGE:
//		if( dwRandom < 20 )
//			return 1;
//		return 2;
//	}
//	return 1;
//}
//
//DWORD					GetAtkRange( CMover & Src, const DWORD dwAtkMethod )
//{
//	//ItemProp* pItemProp = GetAtkProp( dwAtkMethod );
//	ItemProp * pItemProp	= prj.GetItemProp( GetAtkItemId( Src, dwAtkMethod ) );
//#ifdef _DEBUG
//	if( pItemProp == NULL )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "ItemProp->dwAtk%d이 NULL이다. (%s)" ), dwAtkMethod, Src.GetName() );
//	}
//#endif
//	return pItemProp->dwAttackRange;
//}
//
//DWORD					GetAtkItemId( CMover & Src, const DWORD dwAtkMethod )
//{
//	switch( dwAtkMethod )
//	{ 
//	case 1:
//		return Src.GetProp()->dwAtk1;
//	case 2:
//		return Src.GetProp()->dwAtk2;
//	case 3:
//		return Src.GetProp()->dwAtk3;
//	}
//	return 0;
//}

