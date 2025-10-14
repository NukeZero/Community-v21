#include "stdafx.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"

#ifdef __CLIENT
#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif
#endif

#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
extern	CUserMng	g_UserMng;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER

#include "party.h"
extern	CPartyMng	g_PartyMng;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 현재 상태를 무시하고 새로운 동작으로 강제 세팅.
// 서버와 동기화 할때외엔 사용하지 말것.
void	CActionMover::ForcedSet( D3DXVECTOR3 &vDelta, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{
#ifdef __CLIENT
#ifdef _DEBUG
	if( m_pMover != g_pPlayer )
	{
		if( (GetState() & OBJSTA_DMG_FLY) )
		{
			int a = 0;
		}
	}
#endif	// CLIENT
#endif	// DEBUG

	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
	{
//		WriteLog( "ForcedSet(): 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
	{
//		WriteLog( "ForcedSet(): 1 & 0" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_SIT ) == 0 && ( dwStateFlag & OBJSTAF_SIT ) )
	{
//		WriteLog( "ForcedSet()OBJSTAF_SIT: 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_SIT ) && ( dwStateFlag & OBJSTAF_SIT ) == 0 )
	{
//		WriteLog( "ForcedSet()OBJSTAF_SIT: 1 & 0" ); 
		return;
	}

	m_vDelta = vDelta;
	__ForceSetState( dwState );	// 여기 이외엔 이함수 사용하지 말것.
			
	ClearStateFlag();
	AddStateFlag( dwStateFlag );

	if( (GetStateFlag() & OBJSTAF_SIT) && ((dwState & OBJSTA_MOVE_ALL) != OBJSTA_SIT) )	// IsSit()인데
	{
		Message( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	} 
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && ((dwState & OBJSTA_MOVE_ALL) == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
	{
		Message( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	}

	m_nMotionEx = nMotionEx;
	DefaultSet();
}

void	CActionMover::ForcedSet2( D3DXVECTOR3 &vDelta, float fAccPower, float fTurnAngle, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{
	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
	{
//		WriteLog( "ForcedSet2(): 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
	{
//		WriteLog( "ForcedSet2(): 1 & 0" ); 
		return;
	}

	m_vDelta = vDelta;
	m_fAccPower	= fAccPower;
	m_fTurnAngle	= fTurnAngle;
	__ForceSetState( dwState );
	ClearStateFlag();
	AddStateFlag( dwStateFlag );
	m_nMotionEx = nMotionEx;
	DefaultSet();
	if( (GetStateFlag() & OBJSTAF_SIT) && (dwState != OBJSTA_SIT))	// IsSit()인데
	{
		Message( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	} 
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && (dwState == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
	{
		Message( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	}
}

#ifdef __J0518
void CActionMover::HalfForcedSet( D3DXVECTOR3 & vd, float fAccPower, float fTurnAngle )
{
	m_vDelta	= vd;
	m_fAccPower		= fAccPower;
	m_fTurnAngle	= fTurnAngle;
	DefaultSet();
}
#endif	// __J0518

void	CActionMover::DefaultSet( void )
{
//	m_pMover->ResetDestParam( DST_SPEED, 0, FALSE );
	m_pMover->m_fCrrSpd		= 0.0f;
}

//
//	Action Message Process
//	액션 메시지를 받아 처리한다.
//	어떤 행위가 발생하는 시점에 대한 처리를 담당.
//	최적화를 위해서 이미 설정되어 있는 상태면 중복 처리 하지 않음
//
int		CActionMover::ProcessActMsg1( CMover* pMover,  OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 )
{
#ifndef __BEAST	
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;
	//DWORD m_dwState = m_dwState;
	//DWORD m_dwStateFlag = m_dwStateFlag;
	//D3DXVECTOR3 m_vDelta = m_vDelta;

	switch( dwMsg )
	{
	// 평화모드 제자리에 서있어라!
	case OBJMSG_STAND:
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
		{
			ResetState( OBJSTA_MOVE_ALL );		
			m_vDelta.x = m_vDelta.z = 0;
			if( pMover->IsFlyingNPC() )
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}
		if( GetMoveState() == OBJSTA_STAND )	return 0;
		if( GetMoveState() == OBJSTA_PICKUP )	return 0;
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( IsActDamage() )		return -3;
		if( IsDie() )	return -4;
		if( IsAction() )	return 0;
		SetMoveState( OBJSTA_STAND );
		if( GetStateFlag() & OBJSTAF_COMBAT ) {	// 전투모드에 따라서 다르게
			pMover->SetMotion( MTI_STAND );
		}
		else {	// 평화모드 서있기
			pMover->SetMotion( MTI_STAND );	// 평화모드 서있기
		}
		RemoveStateFlag( OBJSTAF_ETC );
		break;
	case OBJMSG_STOP:		
	case OBJMSG_ASTOP:
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
		{
			ResetState( OBJSTA_MOVE_ALL );		
			m_vDelta.x = m_vDelta.z = 0;
			if( pMover->IsFlyingNPC() )
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}
		pMover->ClearDest();
		return 0;	// ControlGround에서 키입력없을때 STOP을 부르는데 거기서 리턴값을 맞추기 위해 이렇게 했음. 
		break;
//	case OBJMSG_SETANGLE:
//		pMover->SetAngle( nParam1 );
//		break;
	case OBJMSG_SITDOWN:
		if( GetStateFlag() & OBJSTAF_SIT )	return 0;		// 이미 앉은모드면 리턴 - 플래그로 검사하지 말고 state로 직접검사하자.

		if( nParam3 == 0 )
		{
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( IsAction() )	return 0;
#ifdef __CLIENT
			if( pMover->IsActiveMover() && (pMover->m_dwReqFlag & REQ_USESKILL) )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
#endif	// __CLIENT
		}
		AddStateFlag( OBJSTAF_SIT );		// 
		SendActMsg( OBJMSG_STOP );
		pMover->SetMotion( MTI_SIT, ANILOOP_CONT );		// 앉기 모션시작
		SetMoveState( OBJSTA_SIT );
		break;
	case OBJMSG_STANDUP:
		if( (GetStateFlag() & OBJSTAF_SIT) == 0 )	return 0;	// 이미 선상태면 리턴

		if( nParam3 == 0 )
		{
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( IsAction() )	return 0;
#ifdef __CLIENT
			if( pMover->IsActiveMover() && (pMover->m_dwReqFlag & REQ_USESKILL) )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
#endif	// __CLIENT
		}
		SetMoveState( OBJSTA_SIT );
		AddStateFlag( OBJSTAF_SIT );		// 
		pMover->SetMotion( MTI_GETUP, ANILOOP_CONT );
		break;
	case OBJMSG_PICKUP:
		if( IsSit() )	return 0;
		if( IsAction() )	return 0;
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
#ifdef __CLIENT
		if( pMover->IsActiveMover() )
		{
			if( pMover->m_dwReqFlag & REQ_USESKILL )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
			if( pMover->m_dwReqFlag & REQ_MOTION )		return 0;	// 서버로부터 모션 응답이 오기전까지 액션 금지.
		}
#endif	// __CLIENT
		if( GetMoveState() == OBJSTA_PICKUP )	return 0;	// 이미 집고 있으면 추가로 실행되지 않게 .
		SetMoveState( OBJSTA_PICKUP );
		pMover->SetMotion( MTI_PICKUP, ANILOOP_1PLAY, MOP_FIXED );
		break;

	case OBJMSG_COLLECT:
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
		SetState( OBJSTA_ACTION_ALL, OBJSTA_COLLECT );
		pMover->SetMotion( MTI_COLLECT, ANILOOP_LOOP, MOP_FIXED );
		pMover->RemoveInvisible();
		break;

	case OBJMSG_STUN:
		SetState( OBJSTA_ACTION_ALL, OBJSTA_STUN );		// 스턴은 어떤 상태에서도 걸릴 수 있으니 상태는 무조것 세팅하자.
		pMover->m_dwFlag |= MVRF_NOACTION;				// 액션 금지 상태로 전환.
		pMover->m_wStunCnt = (short)nParam1;			// 얼마동안이나 스턴상태가 되느냐.

		if( GetState() == OBJSTA_STAND )
			pMover->SetMotion( MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED );		// 아무것도 안하는 스탠딩 상태면 스턴 모션 내자.
		break;
		
	// 전진해라!
	case OBJMSG_TURNMOVE:
		if( GetMoveState() == OBJSTA_FMOVE && (int)pMover->GetAngle() == nParam1 )	return 0;	// 전진 상태이며 각도가 같으면 메시지 무효 
		if( pMover->m_dwFlag & MVRF_NOMOVE )	
			return 0;		// 이동금지 상태면 걍 리턴.
		
//		if( IsActJump() )	return -1;	// 점프거나 공격 상태면 메시지 무효 처리
		if( IsActAttack() )		return -2;
//		if( IsAction() )	return 0;
//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		pMover->SetAngle( (float)nParam1 );
		if( pMover->IsFlyingNPC() )
			pMover->SetAngleX( (float)nParam2 );
		SendActMsg( OBJMSG_STOP_TURN );
		SendActMsg( OBJMSG_FORWARD );
		break;
	case OBJMSG_TURNMOVE2:
		if( GetMoveState() == OBJSTA_FMOVE && (int)pMover->GetAngle() == nParam1 )	return 0;	// 전진 상태이며 각도가 같으면 메시지 무효 
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		if( IsAction() )	return 0;
		if( IsActJump() )	return -1;	// 점프거나 공격 상태면 메시지 무효 처리
		if( IsActAttack() )		return -2;
//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		pMover->SetAngle( (float)nParam1 );
		if( pMover->IsFlyingNPC() )
			pMover->SetAngleX( (float)nParam2 );
		SendActMsg( OBJMSG_STOP_TURN );
		SendActMsg( OBJMSG_BACKWARD );
		break;
	case OBJMSG_FORWARD:
#ifdef __CLIENT
		{
//			int *a = NULL;
//			*a = 1;

		}
#endif
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetMoveState() == OBJSTA_FMOVE )	return 0;	// 이미 전진상태면 추가로 처리 하지 않음
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
//		if( IsAction() )	return 0;
		SetMoveState( OBJSTA_FMOVE );
		if( IsStateFlag( OBJSTAF_WALK ) )	// 걷는모드
		{
			if( IsStateFlag( OBJSTAF_COMBAT ) )		// 전투모드
				pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
			else
				pMover->SetMotion( MTI_WALK );	// 걍 걷기
		} 
		else
		{
			if( pMover->SetMotion( MTI_RUN ) )
				pModel->SetMotionBlending( TRUE );
		}
		pMover->OnActFMove();
		break;
	// 후진해라!
	case OBJMSG_BACKWARD:
		if( IsActJump() )	return -1;	// if jump, x
		if( IsActAttack() )		return -2;	// 공격동작중이면 취소됨.
//		if( IsActDamage() )	ResetState( OBJSTA_DMG_ALL );
		if( IsActDamage() )		return -3;
		if( GetMoveState() == OBJSTA_BMOVE )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		SetMoveState( OBJSTA_BMOVE );
		if( IsStateFlag( OBJSTAF_COMBAT ) )		
			pMover->SetMotion( MTI_WALK );	// 전투모드로 걷기
		else
			pMover->SetMotion( MTI_WALK );		// 걍 걷기
		break;
	// 좌/우 턴 해라!
	case OBJMSG_LTURN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetTurnState() == OBJSTA_LTURN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		SetTurnState( OBJSTA_LTURN );
		break;
	case OBJMSG_RTURN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetTurnState() == OBJSTA_RTURN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		SetTurnState( OBJSTA_RTURN );
		break;
	case OBJMSG_STOP_TURN:
		if( GetTurnState() == 0 )		return 0;		// 이미 턴 상태가 없다면 처리 안함
		SetTurnState( 0 );		// 턴을 중지
		break;
	// 피치를 들어올림. 비행몹의 경우만 사용.
	case OBJMSG_LOOKUP:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
		if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetTurnState() == OBJSTA_LOOKUP )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		SetLookState( OBJSTA_LOOKUP );
		break;
	case OBJMSG_LOOKDOWN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		//		if( IsActDamage() )		return -3;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
		if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetTurnState() == OBJSTA_LOOKDOWN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
		SetLookState( OBJSTA_LOOKDOWN );
		break;
	case OBJMSG_STOP_LOOK:
		if( GetLookState() == 0 )		return 0;
		SetLookState( 0 );		// 룩 중지
		break;
		// 점프
	case OBJMSG_JUMP:
		if( IsActJump() )	return 0;
		if( IsActAttack() )		return -2;
		if( IsActDamage() )		return -3;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#ifdef __CLIENT
		if( pMover->IsActiveMover() )
		{
			if( pMover->m_dwReqFlag & REQ_USESKILL )	return 0;	// 서버로부터 useskill응답이 오기전까진 점프해선 안됨.
			if( pMover->m_dwReqFlag & REQ_MOTION )		return 0;	// 서버로부터 모션 응답이 오기전까지 점프 금지.
		}
#endif	// __CLIENT
		SendActMsg( OBJMSG_STOP_TURN );
		SetJumpState( OBJSTA_SJUMP1 );
		pMover->SetMotion( MTI_JUMP1, ANILOOP_1PLAY );
		pModel->SetMotionBlending( FALSE );
		break;
	// 완드공격
	case OBJMSG_ATK_MAGIC1:	// 공격 1
		{
			if( IsAction() )	return 0;
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetState() & OBJSTA_ATK_ALL )	return 0;		// 이미 공격동작을 하고 있으면 취소.
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -1;		// 공격금지 상태면 걍 리턴.
			
#ifdef __SCHOOL0701
			if( pMover->IsMode( ATTACK_MODE ) ) return -1;
#endif // __SCHOOL0701
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsInvalidObj( pHitObj ) )	return -1;
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -1;
			}
#endif
			if( IsActDamage() )
				ResetState( OBJSTA_DMG_ALL );
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
		
			SendActMsg( OBJMSG_STOP_TURN );
			SendActMsg( OBJMSG_STAND );
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGIC1 );
			//pMover->SetMotion( MTI_ATK1, ANILOOP_1PLAY );		// 완드동작이 없으므로 공격동작으로 대신함.
			pMover->SetMotion( MTI_ATK1, ANILOOP_1PLAY );		// 완드동작이 없으므로 공격동작으로 대신함.
			pHitObj->SetJJim( pMover );
			pMover->m_nAtkCnt = 1;		// 카운트 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
		}
		break;
		// 레인지공격
	case OBJMSG_ATK_RANGE1:	// 공격 1
		{
			if( IsAction() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -1;		// 공격금지 상태면 걍 리턴.
#ifdef __SCHOOL0701
			if( pMover->IsMode( ATTACK_MODE ) ) return -1;
#endif // __SCHOOL0701
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetState() & OBJSTA_ATK_ALL )	return 0;		// 이미 공격동작을 하고 있으면 취소.
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )	
				return -1;

#ifdef __ACROBAT_0504
			m_idTarget = (DWORD)nParam1;		// 공격 타겟.
#endif 

#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -1;
			}
#endif
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -1;
//			if( IsActDamage() )
//				ResetState( OBJSTA_DMG_ALL );
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			
			int		nUseMotion = nParam2;
			SendActMsg( OBJMSG_STOP_TURN );
			SendActMsg( OBJMSG_STAND );
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_RANGE1 );
			pMover->SetMotion( nUseMotion, ANILOOP_1PLAY );		// 완드동작이 없으므로 공격동작으로 대신함.
			pHitObj->SetJJim( pMover );			// 공격이 시작되면 타겟에다가 내가 찜했다는걸 표시.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			
		}
		break;
		// 공격 - 리턴값 -2는 클라로 부터 받은 명령을 완전 무시한다.
	case OBJMSG_ATK1:	// 공격 1
	case OBJMSG_ATK2:	// 공격 2
	case OBJMSG_ATK3:	// 공격 3	
	case OBJMSG_ATK4:	// 공격 4
		{
			if( IsAction() )
				return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )
			{
				return -2;		// 공격금지 상태면 걍 리턴.
			}

#ifdef __SCHOOL0701
			if( pMover->IsMode( ATTACK_MODE ) )
			{
				return -2;
			}
#endif // __SCHOOL0701

#ifdef __CLIENT
			if( pMover->IsActiveMover() )
#endif
			{
				// 기본은 오른손 검사( 완드용 )
				CItemElem* pItemElem = pMover->GetWeaponItem();
/*
#ifdef __ACROBAT_0504
				// 오른손무기가 없으면 왼손 검사( 보우용 )
				if( pItemElem == NULL )
					pItemElem = pMover->GetWeaponItem(PARTS_LWEAPON);
#endif //__ACROBAT_0504
*/
				
#ifdef __ACROBAT_0504
				if( pItemElem && (pItemElem->GetProp()->dwItemKind3 == IK3_WAND || pItemElem->GetProp()->dwItemKind3 == IK3_BOW ) )
#else //__ACROBAT_0504					
				if( pItemElem && (pItemElem->GetProp()->dwItemKind3 == IK3_WAND ) )
#endif //__ACROBAT_0504
				{
					return -2;
				}
			}
				
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )
				return( -1 );

#ifdef __XUZHU
			if( pMover->IsPlayer() )
			{
				int a = 0;
			}
#endif // xuzhu
			m_idTarget = (DWORD)nParam1;		// 공격 타겟.
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -2;
			}
#endif
			if( pHitObj->m_pActMover->IsDie() )	return( -1 );
			if( GetState() & OBJSTA_ATK_ALL )	return( 0 );	// 이미 공격중이면 리턴
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );
			

			DWORD dwOption = MOP_SWDFORCE | MOP_NO_TRANS;
			
#ifdef __XSLOW1018
			int nMin, nTemp;
			pMover->GetHitMinMax( &nMin, &nTemp );
			if( dwMsg == OBJMSG_ATK3 || dwMsg == OBJMSG_ATK4 )
				if( pMover->GetLevel() >= 12 && pHitObj->GetHitPoint() < (int)(nMin * 1.5f) )
					dwOption |= MOP_HITSLOW;
#endif // xSlow1018
#ifndef __BEAST
			if( pMover->IsPlayer() )
				pMover->m_fAniSpeed = pMover->GetAttackSpeed();
#endif // not Beast
			
			switch( dwMsg )
			{
				case OBJMSG_ATK1:
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK1 );
					pMover->SetMotion( MTI_ATK1, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK2:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK2 );
					pMover->SetMotion( MTI_ATK2, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK3:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK3 );
					pMover->SetMotion( MTI_ATK3, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK4:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK4 );
					pMover->SetMotion( MTI_ATK4, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
			}
			pMover->SetAngle( GetDegree( pHitObj->GetPos(), m_pMover->GetPos() ) );
			pHitObj->SetJJim( pMover );

			pMover->m_nAtkCnt = 1;		// 카운트 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			m_objidHit	= (OBJID)nParam1;	// 타겟의 아이디.	

			m_dwAtkFlags	= (DWORD)LOWORD( (DWORD)nParam3 );
			if( m_dwAtkFlags == 0 ) 
			{
				if( pMover->GetAttackResult( pHitObj, dwOption ) )		// 공격 성공률을 얻는다.
					m_dwAtkFlags = AF_GENERIC;
				else
					m_dwAtkFlags = AF_MISS;
			}
			return( (int)m_dwAtkFlags );
			break;
		}	
	// 몬스터들의 특수공격.
	case OBJMSG_SP_ATK1:
	case OBJMSG_SP_ATK2:
		{
			CMover* pHitObj		= prj.GetMover( nParam1 );
			//nParam2;		// 특수공격에 사용할 무기. 보통은 몬스터이므로 가상아이템이 들어온다.
			int		nUseMotion	= nParam3;		// 사용할 모션.
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )	return( -1 );
			if( pHitObj->m_pActMover->IsDie() )	return( -1 );
			if( GetState() & OBJSTA_ATK_ALL )	return( 0 );	// 이미 공격중이면 리턴
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return 0;		// 공격금지 상태면 걍 리턴.
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );

			DWORD dwOption = MOP_SWDFORCE /*| MOP_NO_TRANS*/;	// 몬스터니까 공격동작도 모션트랜지션 되게 해봤다.
			SetState( OBJSTA_ATK_ALL, OBJSTA_SP_ATK1 );
			pMover->SetMotion( nUseMotion, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
			pMover->SetAngle( GetDegree( pHitObj->GetPos(), m_pMover->GetPos() ) );
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			
			return 1;
			
		}
		
		break;

	// 전사들의 근접 전투스킬류
	case OBJMSG_MELEESKILL:
		{
#ifdef __XSYNC0711
			if( IsAction() )	return -2;	
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -2;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -2;
 #ifdef __WORLDSERVER
//			if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
//				return -2;
			if( GetState() & OBJSTA_ATK_ALL )	return -2;	// 이미 공격중이면 리턴. 서버에선 씹히지 않게 하자.
			pMover->m_dwMotion = -1;	// 나머지 경우는 간단하게 생각하기 위해 모두 클리어 시키고 스킬만 실행하자.
			ClearState();				// 상태 클리어하고 다시 맞춤,.
 #else
//			if( pMover->IsActiveMover() )
			if( 0 )
			{
				if( GetState() & OBJSTA_ATK_ALL )	return 0;	// 이미 공격중이면 리턴
				if( IsSit() )		// 앉아있는상태였으면 해제
					ResetState( OBJSTA_MOVE_ALL );
				if( GetDmgState() == OBJSTA_DMG )	// 피격 중 스킬명령이면 피격 상태를 클리어 하고 공격을 시킨다.
					ResetState( OBJSTA_DMG_ALL );
				else
				if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 공격 되지 않음.
					return -3;
				if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )	// 전투모드가 아니면 전투모드로 자동 변환.
					SendActMsg( OBJMSG_MODE_COMBAT );
				SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
				SendActMsg( OBJMSG_STAND );
			} else
			{	
				// 이리저리 해보니 액티브 무버건 아니건 서버에서 신호오면 바로 시작하는게 좋을거 같애서 이렇게 함.
				pMover->m_dwMotion = -1;	
				ClearState();				// 상태 클리어하고 다시 맞춤,.
			}
 #endif
#else
			if( IsAction() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return 0;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -1;
#ifdef __WORLDSERVER
//			if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
//				return -2;
			pMover->m_dwMotion = -1;	// 서버에선 모션 다시 읽음.  클라에서 실행하라고 명령한것기땜에 서버에서 씹히면 안된다.
			ClearState();				// 상태 클리어하고 다시 맞춤,.
 #else
			if( GetState() & OBJSTA_ATK_ALL )	return 0;	// 이미 공격중이면 리턴
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 스킬명령이면 피격 상태를 클리어 하고 공격을 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 공격 되지 않음.
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )	// 전투모드가 아니면 전투모드로 자동 변환.
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );
				
 #endif
#endif // not xSync0711
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MELEESKILL );			// 근접전투스킬상태 설정.
			DWORD dwMotion = (DWORD)nParam1;
			int	nLoop = ANILOOP_1PLAY;

			int	nSkill = pMover->GetActParam( 0 );

		#ifdef __ACROBAT_0504
			if( nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE || nSkill == SI_ACR_YOYO_COUNTER )
		#else //__ACROBAT_0504
			if( nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE )
		#endif //__ACROBAT_0504
			{
				nLoop = ANILOOP_CONT;
			}

#ifdef __GM_COMMAND_ITEM
#ifdef __CLIENT			
			if( pMover->m_SkillState.HasLikeItemBuf( IK3_TEXT_DISGUISE ) )
			{
				dwMotion = MTI_ATK1;
			}
#endif
#endif // __GM_COMMAND_ITEM

			pMover->SetMotion( dwMotion, nLoop, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 해당 동작애니 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			
		#ifdef __YMOTIONHITCOUNT
			m_nMotionHitCount = 0;
		#endif //__YHITCOUNT
			
			break;
		}
	// 마법사들의 마법스킬류.
	case OBJMSG_MAGICSKILL:
	case OBJMSG_MAGICCASTING:
		{
#ifdef __XSYNC0711
			if( IsAction() )	return -2;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -2;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -2;
 #ifdef __WORLDSERVER
			if( GetState() & OBJSTA_ATK_ALL )	return -2;	// 이미 공격중이면 리턴
			pMover->m_dwMotion = -1;	// 서버에선 모션 다시 읽음.  클라에서 실행하라고 명령한것기땜에 서버에서 씹히면 안된다.
			ClearState();				// 상태 클리어하고 다시 맞춤.
 #else // worldserver
//			if( pMover->IsActiveMover() )
			if( 0 )
			{
				if( GetState() & OBJSTA_ATK_ALL )	return 0;	// 이미 공격중이면 리턴
				if( IsSit() )		// 앉아있는상태였으면 해제
					ResetState( OBJSTA_MOVE_ALL );
				if( GetDmgState() == OBJSTA_DMG )	// 피격 중 스킬명령이면 피격 상태를 클리어 하고 공격을 시킨다.
					ResetState( OBJSTA_DMG_ALL );
				else
				if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 공격 되지 않음.
					return -3;
				if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )	// 전투모드가 아니면 전투모드로 자동 변환.
					SendActMsg( OBJMSG_MODE_COMBAT );
				SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
				SendActMsg( OBJMSG_STAND );
			} else
			{
				// 이리저리 해보니 액티브 무버건 아니건 서버에서 신호오면 바로 시작하는게 좋을거 같애서 이렇게 함.
				pMover->m_dwMotion = -1;	
				ClearState();				// 상태 클리어하고 다시 맞춤.
			}
 #endif  // not worldserver
#else
			if( IsAction() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return 0;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -1;
#ifdef __WORLDSERVER
			pMover->m_dwMotion = -1;	// 서버에선 모션 다시 읽음.  클라에서 실행하라고 명령한것기땜에 서버에서 씹히면 안된다.
			ClearState();				// 상태 클리어하고 다시 맞춤.
 #else // worldserver
			if( GetState() & OBJSTA_ATK_ALL )	return 0;	// 이미 공격중이면 리턴
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 스킬명령이면 피격 상태를 클리어 하고 공격을 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 공격 되지 않음.
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )	// 전투모드가 아니면 전투모드로 자동 변환.
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );
				
 #endif  // not worldserver
#endif // not xSync0711
			m_nCastingTime = nParam3;
			if( m_nCastingTime > 0 )	// 캐스팅 타임이 있을때만...
			{
				SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_CASTING1 );			// 캐스팅부터 시작 -> 발사모션.
			}
			else
			{
				SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGICSKILL );			// 발사모션만.
//				pMover->m_nAtkCnt = 1;		// 카운트 시작.
			}
#ifdef __GM_COMMAND_ITEM
#ifdef __CLIENT
			if( dwMsg == OBJMSG_MAGICSKILL && pMover->m_SkillState.HasLikeItemBuf( IK3_TEXT_DISGUISE ) )
			{
				nParam1 = MTI_ATK1;
			}
#endif
#endif // __GM_COMMAND_ITEM
			pMover->SetMotion( nParam1, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 해당 동작애니 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.

		#ifdef __YMOTIONHITCOUNT
			m_nMotionHitCount = 0;
		#endif //__YHITCOUNT
			
			break;
		}
			

	//-------------------- 맞았다.
	case OBJMSG_DAMAGE:
	case OBJMSG_DAMAGE_FORCE:	// 강공격에 맞았다. 날아감.
//		if( IsAction() )	return 0;		// 액션중에도 데미지는 맞을 수 있다.
		{
			if( dwMsg == OBJMSG_DAMAGE_FORCE )	// 강공격으로 들어왔을때
				if( pMover->CanFlyByAttack() == FALSE )	// 날아가지 못하는 상태면.
					dwMsg = OBJMSG_DAMAGE;		// 일반 데미지로 바꿈.
			CMover* pAttacker	= prj.GetMover( (OBJID)nParam2 );
			if( IsValidObj( (CObj*)pAttacker ) ) 
			{
				if( pMover->IsNPC() )	// 맞는넘이 NPC고
				{
					MoverProp *pProp = pMover->GetProp();
					if( pProp )
						if( pProp->bKillable != 1 )		// 죽이기가 불가능 한넘이면 여기서 리턴.
							return 0;
				}
			}
		}

		// 무적이면 여기서 리턴.
		if( pMover->m_dwMode & MATCHLESS_MODE )		
			return 0;

#ifdef __POSTAIMSG1008
		pMover->PostAIMsg( AIMSG_DAMAGE, nParam2 ); // nParma2는 공격자 
#else	// __POSTAIMSG1008
		pMover->SendAIMsg( AIMSG_DAMAGE, nParam2 ); // nParma2는 공격자 
#endif	// __POSTAIMSG1008
		{
			int nDamage = _ProcessMsgDmg( dwMsg, prj.GetMover( (OBJID)nParam2 ), (DWORD)nParam1, nParam3, nParam4 );	// 실제 가격한 데미지를 리턴함.
			CMover* pAttacker	= prj.GetMover( (OBJID)nParam2 );
			pAttacker->RemoveInvisible();
			return nDamage;
		}
		break;
	// HP가 0이 된 순간 호출
	case OBJMSG_DIE:
		if( IsSit() )		// 앉아있는상태였으면 해제
			ResetState( OBJSTA_MOVE_ALL );
//		if( IsAction() )	return 0;		// 여기에 부활이외 다른동작이 들어간다면 이렇게 하면 안될것 같다.
		
		ClearState();	// 죽을땐 다른 동작 하고 있던거 다 클리어 시켜버리자.
		m_vDelta.x = m_vDelta.z = 0;		// 죽었으면 델타값 클리어.
		pMover->m_tmCrime = 0;				// 만약 범죄자 상태였더라도 죽었으면 클리어 시키자

		pMover->RemoveInvisible();
#ifdef __POSTAIMSG1008
		pMover->PostAIMsg( AIMSG_DIE, nParam2 );
#else	// __POSTAIMSG1008
		pMover->SendAIMsg( AIMSG_DIE, nParam2 );
#endif	// __POSTAIMSG1008
		{
			CMover *pAttacker = (CMover *)nParam2;	
			if( pAttacker )
			{
				if( pMover->IsPlayer() && pAttacker->IsPlayer() )	// 죽은놈과 죽인놈이 플레이어고
				{
#ifndef __NPP_050308
					if( pMover->m_nDuel == 1 && pMover->m_idDuelOther == pAttacker->GetId() )	// 둘이 듀얼중이었으면
					{
						pMover->ClearDuel();
						pAttacker->ClearDuel();
					}
#else	// __NPP_050308
#ifdef __WORLDSERVER
					if( pMover->m_nDuel == 1 && pMover->m_idDuelOther == pAttacker->GetId() )	// 둘이 듀얼중이었으면
					{
						pMover->ClearDuel();
						pAttacker->ClearDuel();
						((CUser*)pMover)->AddSetDuel( pMover );
						((CUser*)pAttacker)->AddSetDuel( pAttacker );
					}
					else if( pMover->m_nDuel == 2 && pMover->m_idDuelParty == pAttacker->m_idparty )
					{
						pMover->m_nDuelState	= 300;		//
						g_PartyMng.Lock( theLineFile );
						CMover *pMember;
						CParty* pParty	= g_PartyMng.GetParty( pMover->m_idparty );

						if( pParty )
						{
							for( int i = 0; i < pParty->m_nSizeofMember; i ++ )
							{
								pMember = (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
								if( IsValidObj( pMember ) )
									((CUser*)pMember)->AddSetDuel( pMover );
							}
						}

						CParty* pParty2		= g_PartyMng.GetParty( pMover->m_idDuelParty );
						if( pParty2 )
						{
							for( int i = 0; i < pParty2->m_nSizeofMember; i ++ )
							{
								pMember = (CMover *)g_UserMng.GetUserByPlayerID( pParty2->m_aMember[i].m_uPlayerId );
								if( IsValidObj( pMember ) )
									((CUser*)pMember)->AddSetDuel( pMover );
							}
						}
						g_PartyMng.Unlock( theLineFile );
					}
#endif	// __WORLDSERVER
#endif	// __NPP_050308
				}
				// xuzhu?
				if( pAttacker->IsNPC() )	// 공격자가 몹.
				{	// 날때린몹의 어태커가 나일경우는 죽기전에 클리어시켜준다.
					if( pAttacker->m_idAttacker == pMover->GetId() )	// 공격한몹을 친사람이 나일경우는 스틸체크 해제.
					{
						pAttacker->m_idAttacker = NULL_ID;
						pAttacker->m_idTargeter = NULL_ID;
					}
				}
			}
		}
		//		if( IsActJump() )
//			ResetState( OBJSTA_JUMP_ALL );		// 점프중이면 점프상태 클리어하고 죽는 모션.
//		SendActMsg( OBJMSG_STAND );	// 죽을땐 제자리에 서라
//		ResetState( OBJSTA_TURN_ALL );	// 죽을땐 턴 없애라.
//		nParam1 = OBJMSG_DAMAGE;	// 일단 강공격 날아가기 빼자.
		switch( nParam1 )
		{
		case OBJMSG_DAMAGE_FORCE:	// 강 공격으로 죽었다면 날아가서 죽기
			{
			CMover *pAttacker = (CMover *)nParam2;
			SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_FLY );
			pMover->SetMotion( MTI_DMGFLY, ANILOOP_CONT );
			if( pAttacker )
				DoDamageFly( pAttacker->GetAngle(), 145.0f, 0.25f );	// 어태커가 보는쪽으로 날려보냄.
			}
			break;
		case OBJMSG_DAMAGE:		// 걍 일반 데미지로 죽었다면 그냥 쓰러져 죽기
		default:	
			SetState( OBJSTA_DMG_ALL, OBJSTA_DEAD );
			pMover->SetMotion( MTI_DIE1, ANILOOP_CONT );
			break;
		}
		if( !pMover->IsPlayer() )	// 몬스터일 경우.
		{
 #ifdef __WORLDSERVER			
			m_nDeadCnt	= 60*3;	// 디폴트 - 3초 후에 없어짐.
			MoverProp *pProp = pMover->GetProp();
			if( pProp )
			{
				if( pProp->dwSourceMaterial != NULL_ID || pProp->dwClass == RANK_MATERIAL )	// 자원 몹이면..
				{
					if( pProp->dwHoldingTime == NULL_ID )
						Message( "%s 의 MoverProp::dwHoldingTime 값이 -1", pMover->GetName() );
					m_nDeadCnt = (int)(60.0f * (pProp->dwHoldingTime / 1000.0f));
					if( pProp->dwHoldingTime < 10000 )		// 10초 이하일때
						Message( "dwHoldingTime : %d %s", pProp->dwHoldingTime, pMover->GetName() );
					pMover->m_nCollectOwnCnt = PROCESS_COUNT * 40;
					CMover *pAttacker = (CMover *)nParam2;	
					if( pAttacker )
						pMover->m_idCollecter = pAttacker->GetId();		// 공격자가 소유자가 된다.
					
				}
				{
					if( pProp->dwSourceMaterial == NULL_ID && pProp->dwClass == RANK_MATERIAL )	// 자원몹인데 자원값이 없는경우
						Message( "OBJMSG_DIE : %s %d", pMover->GetName(), pProp->dwSourceMaterial );
					if( pProp->dwSourceMaterial != NULL_ID && pProp->dwClass != RANK_MATERIAL )	// 자원몹이 아닌데 자원값이 있는경우.
						Message( "OBJMSG_DIE 2 : %s %d", pMover->GetName(), pProp->dwSourceMaterial );
				}
			}
 #endif // WorldServer
		}
		break;
	case OBJMSG_APPEAR:
		SetState( OBJSTA_ACTION_ALL, OBJSTA_APPEAR );
		pMover->SetMotion( MTI_APPEAR2, ANILOOP_1PLAY, MOP_FIXED | MOP_NO_TRANS );
		break;
	//---- 모드전환 ------------------------
	case OBJMSG_MODE_COMBAT:
		if( GetStateFlag() & OBJSTAF_COMBAT )	return 0;		// 이미 전투모드면 리턴
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		AddStateFlag( OBJSTAF_COMBAT );	// 전투모드로 서라가 아니고 전투모드로 바꿔라기땜에 서다 명령은 발생시키지 않는다.
		pMover->m_dwMotion = -1;	// 같은 모션을 하라고 한거라 모션이 안바뀌므로 이렇게...
		pMover->SetMotion( MTI_STAND );
		break;
	case OBJMSG_MODE_PEACE:
		if( (GetStateFlag() & OBJSTAF_COMBAT) == 0 )	return 0;
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		RemoveStateFlag( OBJSTAF_COMBAT );
		pMover->m_dwMotion = -1;
		pMover->SetMotion( MTI_STAND );
		break;
	case OBJMSG_MODE_WALK:
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		if( GetStateFlag() & OBJSTAF_WALK )	return 0;		// 이미 걷기모드면 리턴
		AddStateFlag( OBJSTAF_WALK );
		break;
	case OBJMSG_MODE_RUN:
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		if( (GetStateFlag() & OBJSTAF_WALK) == 0 )	return 0;		// 
		RemoveStateFlag( OBJSTAF_WALK );
		break;
	case OBJMSG_MODE_FLY:
		if( nParam3 == 0 )
		{
			if( IsSit() )		return 0;
			if( IsAction() )	return 0;
			if( IsActAttack() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
			{
				CWorld* pWorld	= pMover->GetWorld();
				if( pWorld )
				{
					int nAttr = pMover->GetWorld()->GetHeightAttribute( (int)pMover->GetPos().x, (int)pMover->GetPos().z );		// 이동할 위치의 속성 읽음.
					if( nAttr == HATTR_NOFLY )		// 비행 금지 지역에서 타려고 하면 못탄다.
						return 0;
				}
			}
		}
		AddStateFlag( OBJSTAF_FLY );
		{
			ItemProp *pItemProp = prj.GetItemProp( nParam1 );	// 빗자루 프로퍼티.
			if( pItemProp )
			{
				if( pMover->m_nFuel == -1 )		// 초기값이면
					pMover->m_nFuel = (int)pItemProp->dwFFuelReMax;	// 빗자루 최대 연료량을 세팅.
				// -1일때만 세팅해야지 연료를 반쯤 쓰다가 빗자루를 바꿔도 새로 세팅 되지 않는다.
			}
		}
#ifdef __WORLDSERVER
		if( pMover->IsPlayer() )
		{
			((CUser*)pMover)->SetFlyTimeOn();
		}
#endif // __WORLDsERVER
		ClearState();
#ifdef __CLIENT
		if( m_pMover == CMover::GetActiveMover() )
		{
			m_pMover->GetWorld()->SetObjFocus( NULL );	// 비행직전에 기존타겟팅을 클리어해준다.
			g_Neuz.m_camera.Unlock();
			g_WndMng.m_pWndTaskBar->OnCancelSkill();	// 비행직전엔 스킬바 사용 취소.
		}
#endif
		{
			CModel* pModel = prj.m_modelMng.LoadModel( D3DDEVICE, OT_ITEM, (DWORD) nParam1 );
			m_pMover->SetRide( pModel, (DWORD) nParam1 );
		}
		
		break;
	case OBJMSG_MOTION:		// 단순 모션 플레이
		{
//			if( m_dwStateFlag & OBJSTAF_ETC )	return 0;
			if( IsSit() )		return 0;
			if( IsAction() )	return 0;
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
			AddStateFlag( OBJSTAF_ETC );		// 단순 모션 플레이 상태
			m_nMotionEx = nParam1;				// 모션 인덱스
			int nPlayMode = nParam2;			// 루핑 모드.
			if( m_nMotionEx != -1 )
				pMover->SetMotion( m_nMotionEx, nPlayMode, MOP_FIXED/* | MOP_SWDFORCE*/ );

			pMover->RemoveInvisible();
			
			
		}
		break;
	// 일반 액션.
	case OBJMSG_RESURRECTION:		// 부활.
		ClearState();
		SetState( OBJSTA_DMG_ALL, OBJSTA_RESURRECTION );
		pMover->SetMotion( MTI_ASS_RESURRECTION, ANILOOP_1PLAY, MOP_FIXED );
		break;


#ifdef _DEBUG
#ifdef __CLIENT
	case OBJMSG_TEMP:
		{
			for( int i=0; i < 1; i ++ )
			{

				CMover* pObj = new CMover;
				D3DXVECTOR3 vPos = pMover->GetPos();
				pObj->SetPos( vPos );
				pObj->SetIndex( g_Neuz.m_pd3dDevice, MI_AIBATT1, TRUE );//1 + rand() % 15 );
				pObj->SetMotion( 0 );
				g_WorldMng.Get()->AddObj( pObj, TRUE ); 
			}
		}
		break;
#endif
#endif // DEBUG
		
	case OBJMSG_TEMP2:	// 디버깅용 공격동작만 볼때.
#ifdef __ONLINE
#ifndef __WORLDSERVER
//#ifndef _DEBUG
		if( CMover::GetActiveMover()->IsAuthHigher( AUTH_GAMEMASTER ) )	// 영자 계정일때
//#endif // not debug
		{
			CObj *pObj = pMover->GetWorld()->GetObjFocus();
			g_DPlay.SendCorrReq( pObj );
		}
#endif // not worldserver
#endif
		break;
	
	case OBJMSG_TEMP3:
		{
#ifdef _DEBUG
#ifdef __CLIENT
#ifdef __ONLINE
#ifdef __XUZHU			
			g_DPlay.SendError( 0, 0 );
#endif
#endif // __ONLINE
#endif // client
#endif // xuzhu
		}
		break;
		
	}
#endif
	return 1;
}


/*

  p0 + tD0 = -e0

  t = -( p0 + e0 ) / d0

void	Test( RECT r, Point p, Point.d )
{
	Point t;	// 교차점

	tx = (p0 + 
	
}

*/


