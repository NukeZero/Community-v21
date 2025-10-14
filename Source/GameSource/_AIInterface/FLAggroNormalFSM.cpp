#include "StdAfx.h"

#if defined __WORLDSERVER && defined __AGGRO16

#include "FLAggroNormalFSM.h"

#include "dialogmsg.h"

#include "AIInterface.h"
#include "FLBehaviorFunction.h"
#include "InstanceDungeonBase.h"

#include "../WorldServer/User.h"

#undef max


const static float			RANGE_RETURN_TO_BEGIN	= 120.0f;
const static float			RANGE_MOVE				= 15.0f;



FLAggroNormalFSM::FLAggroNormalFSM( CMover & Owner, const DWORD dwAggroID ) 
: FLFSM< FLAggroNormalFSM >( Owner ) 
, m_pAggro( new FLAggro( dwAggroID ) )
{
	ClearAll();
	ChangeState( m_IdleState );
}

FLAggroNormalFSM::~FLAggroNormalFSM()
{
	delete m_pAggro;
}

void					
FLAggroNormalFSM::SetPartyLeader( const DWORD dwLeaderObjid )
{
	m_dwPartyOwnerOBJID		= dwLeaderObjid;
}

void
FLAggroNormalFSM::InitProp()
{
	ClearAll();
	
	m_vPosBegin				= GetMover().GetPos();//vPosBegin;
	m_fPatrolRange			= RANGE_MOVE;

	m_tmRageTimeOutInterval = RAGE_TIME_OVER_MAX;
}

void
FLAggroNormalFSM::ClearAll()
{
	m_dwPartyOwnerOBJID		= NULL_ID;

	m_vPosBegin				= D3DXVECTOR3( 0, 0, 0 );
	m_fPatrolRange			= 0;

	m_tmRageTimeOutInterval = 0;
	m_OwnerObjid			= NULL_ID;

	ClearWithoutProp();
}

void 
FLAggroNormalFSM::ClearWithoutProp()
{
	m_RageState.Clear();


	if( GetAggroMng() != NULL )
		GetAggroMng()->Clear();

	m_Patrol.Clear();
	m_Attack.Clear();
	m_TrySummonMonster.Clear();
	m_vOldPos				= D3DXVECTOR3( 0, 0, 0 );
}

FLAggro *		
FLAggroNormalFSM::GetAggroMng()
{
	return m_pAggro;
}

void			
FLAggroNormalFSM::sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 )
{
	///aggro 관련 행동 처리. STATE_ 와의 관련은 없다.
	switch( dwMessage )
	{
	case AIMSG_REQUEST_HELP:
		{
			const OBJID	objid	= dwParam1;
			if( GetAggroMng() != NULL )
				GetAggroMng()->SumAggro( GetMover(), objid, 1 );		//@@@@@@@@@@@@@@@@@@
		}
		break;
	case AIMSG_INIT:
		{
		}
		break;
	case AIMSG_INIT_TARGETCLEAR:		
	case AIMSG_DSTDIE:	
		{
			const OBJID objid	= ( OBJID )dwParam1;
			if( GetAggroMng() == NULL )
				break;

			GetAggroMng()->RemoveMover( objid );
			if( GetAggroMng()->Size() == 0 )
			{
				GetAggroMng()->Clear();		
			}
			else 
			{
				CMover * pTarget = FLScanTarget::ByAggro( GetMover(), m_OptinalFlags );
				if( pTarget != NULL )
				{
					GetAggroMng()->SumNextTagerAggro( GetMover(), *pTarget );
				}
			}
		}
		break;
	case AIMSG_DSTDAMAGE:
		{
			__noop;
		}
		break;
	case AIMSG_DAMAGE:
		{
			const OBJID objid = ( OBJID )dwParam1;
			const int nDamage		= ( int )dwParam2;
			if( GetAggroMng() != NULL )
			{
				if(  GetAggroMng()->SumFirstAttackerAggro( GetMover(), objid ) == FALSE )
					GetAggroMng()->SumAttackAggro( GetMover(), objid, nDamage );
			}
		}
		break;

	
	case AIMSG_EXIT:
		{
			if( GetAggroMng() != NULL )
				GetAggroMng()->Clear();

			m_LeaderBehavior.ClearAllSubMonster();
		}
		break;
	case AIMSG_DIE:	
		{
			if( GetAggroMng() != NULL )
				GetAggroMng()->Clear();

			m_LeaderBehavior.KillSubMonsterAll( ( CMover * )dwParam1 );
		}
		break;

	case AIMSG_TIMEOUT:
		{
			if( GetAggroMng() != NULL )
				GetAggroMng()->Clear();
		}
		break;
	default:
		int i = 0;		//breakpoint 용
		UNREFERENCED_PARAMETER( i );
		__noop;
		break;
	}

	FLFSM< FLAggroNormalFSM >::sendAIMsg( dwMessage, dwParam1, dwParam2 );
}

//void FLAggroNormalFSM::SetManualTarget( const OBJID objid )
//{
//	//if( GetAggroMng() == NULL )
//	//	return;
//
//	//m_OptinalFlags.bStalkTarget			= TRUE;
//	//GetAggroMng()->Clear();
//	//GetAggroMng()->SumAggro( GetMover(), objid, std::numeric_limits< UINT64 >::max() );
//
//	//ChangeState( m_RageState );
//}
//--------------------------------------------------------------------------------------------------------//

void FLAggroNormalFSM::FLIdleState::OnEnter( FLAggroNormalFSM & FSM )
{
	FSM.ClearWithoutProp();
	FSM.GetMover().SetSpeedFactor( 1.0F );
}

void FLAggroNormalFSM::FLIdleState::UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg )
{
	CMover & Owner			= FSM.GetMover();

	if( AIMSG_DIE != msg.dwMessage )
		FSM.m_LeaderBehavior.TryDoSummon( FSM.GetMover(), FSM.GetMover().GetWorld() );

	switch( msg.dwMessage )
	{
	case AIMSG_REQUEST_HELP:
		{
			const OBJID objid	= ( OBJID )msg.dwParam1;
			UNREFERENCED_PARAMETER( objid );
			FSM.ChangeState( FSM.m_RageState );
		}
		break;

	case AIMSG_NONE:
	case AIMSG_PROCESS:
		{
			//파티 몬스터리더 일경우 부하 시야로부터 타겟을 찾는다.
			CMover * pTarget	= FSM.m_LeaderBehavior.TryScanTargetByNear_Party( FSM.m_OptinalFlags );
			if( pTarget != NULL )
			{
				FSM.ChangeState( FSM.m_RageState );
				break;
			}

			// 다른 곳에서 어그로를 받은....
			pTarget				= FLScanTarget::ByAggro( FSM.GetMover(), FSM.m_OptinalFlags );
			if( pTarget != NULL )
			{
				FSM.ChangeState( FSM.m_RageState );
				break;
			}

			if( FSM.GetMover().m_bActiveAttack == TRUE )
			{
				CMover * pTarget = FLScanTarget::ByNear( Owner, Owner, Owner.GetProp()->m_nAttackFirstRange, FSM.m_OptinalFlags );
				if( pTarget != NULL )
				{
					if( FSM.GetAggroMng() != NULL )
						FSM.GetAggroMng()->SumFirstTargetAggro( Owner, pTarget->GetId() );

					FSM.ChangeState( FSM.m_RageState );
					break;
				}
			}

			//리더가 죽었거나 없다면.
			CMover * pLeader = prj.GetMover( FSM.m_dwPartyOwnerOBJID );
			if( IsValidObj( pLeader ) == TRUE && pLeader->IsLive() )		
			{
				//리더에게 지시를 받는다.
				//FSM.m_LeaderBehavior.SetSubMonsterPos( FSM.GetMover() );	//부하 조정
			}
			else	//자체 patrol
			{
				if( FSM.m_Patrol.PatrolUpdate( Owner, FSM.m_vPosBegin, FSM.m_fPatrolRange ) == TRUE )
					FSM.m_LeaderBehavior.SetSubMonsterPos( FSM.GetMover() );	//부하 조정
			}
		}
		break;
	case AIMSG_ARRIVAL:
		{
			CMover * pLeader = prj.GetMover( FSM.m_dwPartyOwnerOBJID );
			if( IsValidObj( pLeader ) == TRUE && pLeader->IsLive() )		//리더가 아니면
			{
				FSM.GetMover().SetAngle( pLeader->GetAngle() );
				FSM.GetMover().SetStop();
			}
			else	// 리더이거나 리더가 없으면
			{
				FSM.m_Patrol.SetStopMoverTimeOut( FSM.GetMover(), SEC( 5 ) + xRandom( SEC( 1 ) )  );
			}
		}
		break;

	case AIMSG_DAMAGE:
		{
			FSM.ChangeState( FSM.m_RageState );
		}
		break;
	
	case AIMSG_COLLISION: 
		{
			if( FSM.m_OptinalFlags.bIgnoreCollision == FALSE )
				FSM.m_Patrol.SetRandomPatrolPos( Owner, FSM.m_vPosBegin, FSM.m_fPatrolRange );
		}
		break;

	default:
		__noop;
		break;
	}
}

void FLAggroNormalFSM::FLIdleState::OnLeave( FLAggroNormalFSM & /*FSM*/ )
{
	__noop;
}

//-------------------------------------------------------------------------------------------//

FLAggroNormalFSM::FLRageState::FLRageState()
{
	Clear();
}

FLAggroNormalFSM::FLRageState::~FLRageState()
{

}

void FLAggroNormalFSM::FLRageState::Clear()
{
	m_tmAttackPoint			= 0;
}

void FLAggroNormalFSM::FLRageState::OnEnter( FLAggroNormalFSM & FSM )
{
	FSM.m_Attack.SetRageTimeOutTime( /*::GetTickCount() + */FSM.m_tmRageTimeOutInterval ); 

	FSM.GetMover().SetSpeedFactor( 2.0F );
}

void FLAggroNormalFSM::FLRageState::UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg )
{	
	CMover & Owner			= FSM.GetMover();

	switch( msg.dwMessage )
	{
	case AIMSG_INVALID_TARGET:
	case AIMSG_DSTDIE:
	case AIMSG_NONE:
	case AIMSG_PROCESS:
		{
			// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
			if( Owner.IsDie() || ( Owner.m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL ) )
				return;

			// 시작 지점과 거리가 멀거나 || 시간 초과이면 RETURN TO HOME
			if( FSM.m_OptinalFlags.bIgnoreTraceRange == FALSE )
			{
				const D3DXVECTOR3 vDist = Owner.GetPos() - FSM.m_vPosBegin;
				const float fRange		= Owner.IsFlyingNPC() ? ( RANGE_MOVE * 3.0f ) : RANGE_MOVE;		
				if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( Owner.GetWorld()->GetID() ) == FALSE
					&& IsInRange( vDist, RANGE_RETURN_TO_BEGIN + fRange ) == FALSE )	
				{
					FSM.ChangeState( FSM.m_ReturnToBeginState );
					break;
				}
			}

			// 타겟 검색.
			CMover * pTarget	= FLScanTarget::ByAggro( FSM.GetMover(), FSM.m_OptinalFlags );
			if( pTarget == NULL )
			{
				FSM.ChangeState( FSM.m_ReturnToBeginState );
				break;
			}

			//neodeath : 분석 결과 공격적인 상태에서 일정 범위 안에서 일정시간동안 공격을 한번도 못했을 경우 
			//			갖혀있다고 판단하여 워프 시킴 ( 결론은 땜빵 코드 )
			if( ::GetTickCount() > FSM.m_Attack.GetRageTimeOutTime() )			// 추적 설정 시간이 만료 되었으면.
			{
				//오류 비켜가기 땜빵코드인듯	
				FSM.m_Attack.SetRageTimeOutTime( /*::GetTickCount() + */FSM.m_tmRageTimeOutInterval );
				// 5초전 좌표와 지금 좌표의 거리를 계산.
				D3DXVECTOR3 vDist	= Owner.GetPos() - FSM.m_vOldPos;
				vDist.y				= 0;
				if( IsInRange( vDist, 2.0f ) )			//@@@@@ 2미터를 못벗어났다면 어딘가에 갖혔다는 뜻.
				{
					if( Owner.GetSpeed( Owner.m_pActMover->m_fSpeed ) > 0 )	//@@@@@@  마법적으로 움직이지 못하게 된상태에선 워프하면 안됨.
					{
						Owner.SetPos( pTarget->GetPos() );
						g_xWSUserManager->AddSetPos( &Owner, Owner.GetPos() );
					}
				}
				FSM.m_vOldPos			= Owner.GetPos();
			}

			// 자기힐 시도
			const BOOL bRecvCondition = RecvConditionUpdate( Owner );
			if( bRecvCondition == TRUE )
			{
				FSM.m_Attack.SetAttackPointTime( ::GetTickCount() + SEC( 3 ) );
			}

			// 공격 거리 선택
			const BOOL bRangeAttack	= FSM.m_Attack.AutoChoiceRangeAttack( Owner, *pTarget );
			SetMoveToDest( Owner, pTarget->GetId() );		// 근거리는 무조건 목표쪽으로 접근.

			// 공격 찬스이면 실제 공격
			if( ::GetTickCount() > FSM.m_Attack.GetAttackPointTime() )
			{
				if( FSM.m_Attack.TryAttack( Owner, *pTarget, bRangeAttack, FSM.m_tmRageTimeOutInterval ) == FALSE )
					break;
			}

			// 자식 몬스터 소환 시도		
			FSM.m_TrySummonMonster.TrySummonUpdate( Owner );	
		}
		break;

	case AIMSG_DAMAGE:
		{
			// 타겟 검색.			
			CMover * pTarget = FLScanTarget::ByAggro( FSM.GetMover(), FSM.m_OptinalFlags );
			if( pTarget == NULL )
			{
				if( FSM.m_RunawayNSelfHeal.IsFirstRunaway() == FALSE )
					FSM.ChangeState( FSM.m_ReturnToBeginState );
				break;
			}

			// 도주가 ON이라면
			if( Owner.GetHitPointPercent( 100 ) < Owner.GetProp()->m_nRunawayHP )
			{
				FSM.ChangeState( FSM.m_RunawayState );
			}

			// 주변 동료를 부를 수 있다면.
			if( Owner.GetProp()->m_bHelpWho )	// 헬프AI가 있을경우.
			{
				FSM.m_CallHelper.TryCallHelper( FSM.GetMover(), *pTarget );
			}
		}
		break;

	case AIMSG_COLLISION: 
		{
			if( FSM.m_OptinalFlags.bIgnoreCollision == FALSE )
				FLSetRandomPos( FSM.GetMover() );
		}
		break;
	}
}

void FLAggroNormalFSM::FLRageState::OnLeave( FLAggroNormalFSM & /*FSM*/ )
{
	__noop;
}

//------------------------------------------------------------------------------------------------------//



void FLAggroNormalFSM::FLTokenToBeginState::OnEnter( FLAggroNormalFSM & FSM )
{
	FSM.GetMover().m_IsReturnToBegin		= TRUE;
	SetMoveToDest( FSM.GetMover(), FSM.m_vPosBegin );
	FSM.GetMover().SetSpeedFactor( 2.66F );
}

void FLAggroNormalFSM::FLTokenToBeginState::UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg )
{
	switch( msg.dwMessage )
	{
	case AIMSG_NONE:
	case AIMSG_PROCESS:
		{
			if( IsInRange( FSM.m_vPosBegin - FSM.GetMover().GetPos(), 7.0f ) == TRUE )		//neodeath : 7.0f 하드 코딩 되어있었음. 내 잘못이 아님	
			{
				SetMaxHP( FSM.GetMover() );
				FSM.ChangeState( FSM.m_IdleState );		
			}
		}
		break;
	case AIMSG_COLLISION: 
		{
			if( FSM.m_OptinalFlags.bIgnoreCollision == FALSE )
				FLSetRandomPos( FSM.GetMover() );
		}
		break;
	}
}

void FLAggroNormalFSM::FLTokenToBeginState::OnLeave( FLAggroNormalFSM & FSM )
{
	FSM.GetMover().m_IsReturnToBegin		= FALSE;
}


//------------------------------------------------------------------------------------------------------//
void FLAggroNormalFSM::FLRunawayState::OnEnter( FLAggroNormalFSM & FSM )
{
	CMover * pTarget	= FLScanTarget::ByAggro( FSM.GetMover(), FSM.m_OptinalFlags );
	if( pTarget == NULL )
	{
		FSM.ChangeState( FSM.m_ReturnToBeginState );
		return;
	}

	g_xWSUserManager->AddDlgEmoticon( &FSM.GetMover(), DLGEMOT_EVADE );
	FSM.m_RunawayNSelfHeal.StartInit( FSM.GetMover(), *pTarget );
}

void FLAggroNormalFSM::FLRunawayState::UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg )
{
	CMover & Owner			= FSM.GetMover();

	switch( msg.dwMessage )
	{
	case AIMSG_DSTDIE:
	case AIMSG_NONE:
	case AIMSG_PROCESS:
		{
			// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
			if( Owner.IsDie() || ( Owner.m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL ) )
				return;

			// 타겟 검색.
			CMover * pTarget	= FLScanTarget::ByAggro( FSM.GetMover(), FSM.m_OptinalFlags );
			if( pTarget == NULL )
			{
				FSM.ChangeState( FSM.m_ReturnToBeginState );
				break;
			}

			// 체력이 50 % 이상 되면 반격 시도. ( 어디선가 힐을 한다면 )
			if( Owner.GetHitPointPercent( 100 ) >= 50 )
			{
				FSM.ChangeState( FSM.m_RageState );
				break;
			}

			// 도망 업데이트.
			if( pTarget != NULL )
				FSM.m_RunawayNSelfHeal.RunawayUpdate( Owner, *pTarget );
		}
		break;
	case AIMSG_COLLISION:
		{
			// 현재의 반대 방향으로 도주
			// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다. 
			Owner.AddAngle( ( float )( 180 + ( 20 - xRandom( 40 ) ) ) );

			const FLOAT fAngle		= Owner.GetAngle();
			const D3DXVECTOR3 vDst	= Owner.GetPos() + VelocityToVec( fAngle, 5 );
			SetMoveToDest( Owner, vDst );
		}
		break;

	case AIMSG_ARRIVAL:
		{
			if( Owner.GetProp()->m_dwRunawayDelay )
			{
				FSM.m_RunawayNSelfHeal.SetStopMoverTimeOut( Owner, Owner.GetProp()->m_dwRunawayDelay + xRandom( SEC( 1 ) ) );
			}
		}
		break;
	}
}

void FLAggroNormalFSM::FLRunawayState::OnLeave( FLAggroNormalFSM & /*FSM*/ )
{
	__noop;
}



#endif //defined __WORLDSERVER && defined __AGGRO16