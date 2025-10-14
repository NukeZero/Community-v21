#include "stdafx.h"
#include "DefineObj.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"


#include "party.h"
extern	CPartyMng	g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CMeleeAtkMsgq::Process( CAction* pActMover )
{
#ifndef __WORLDSERVER
	if( pActMover->GetMover()->IsActiveMover() )	return( FALSE );
#endif	// __WORLDSERVER
	LPACTMSG lpActMsg	= GetHead();
	int nError;
	while( lpActMsg )
	{
		nError	= pActMover->SendActMsg( (OBJMSG)lpActMsg->dwMsg, lpActMsg->nParam1, lpActMsg->nParam2, lpActMsg->nParam3 );
		if( nError > 0 ) {
			RemoveHead();
			return( TRUE );
		}
		else if( nError == 0 ) {
			return( FALSE );
		}
		else if( nError < 0 ) {
			RemoveHead();
			lpActMsg	= GetHead();
		}
	}
	return( FALSE );
}

BOOL CMagicAtkMsgq::Process( CMover* pMover )
{

	return( FALSE );
}

void CAtkMsgq2::Process( CAction* pActMover, u_short* puRHit )
{
#ifndef __WORLDSERVER
	if( pActMover->GetMover()->IsActiveMover() )	return;
#endif	// __WORLDSERVER
	LPACTMSG2 lpActMsg2	= GetHead();
	if( lpActMsg2 ) {
		pActMover->SendActMsg( (OBJMSG)lpActMsg2->dwMsg, lpActMsg2->nParam1, lpActMsg2->nParam2, lpActMsg2->nParam3 );
		RemoveHead();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CActionMoverPool*	CActionMover::m_pPool	= new CActionMoverPool( 256 );

CActionMover::CActionMover() 
{
	Init();
}
CActionMover::CActionMover( CMover* pMover ) : CAction( pMover )
{
	Init();
}

CActionMover::~CActionMover()
{
	Destroy();
}

void	CActionMover::Init( void )
{
	m_nCount		= 0;
	m_idTarget		= NULL_ID;
	m_objidHit		= NULL_ID;
	m_fAccAng		= 0;
	m_fTurnAngle	= 0.0f;
	m_fAccPower		= 0;
	m_fSpeed		= 0;		// 이것은 뛸때 this의 스피드를 말한다.  공중에선 적용되지 않음. 몬스터도 기본 뛰는게 된다,.
	m_vDelta.x		= m_vDelta.y = m_vDelta.z = 0;
	m_vDeltaE.x		= m_vDeltaE.y = m_vDeltaE.z = 0;
	m_nMotionEx		= -1;
	m_bGround		= 0;
	m_bMove			= 0;
	m_dwAtkFlags	= 0;
	m_fCurrentHeight = 0;
	m_nCastingTime	= 0;
	m_fDistance		= 0.0f;
	m_nShootCnt		= 0;
	

	m_vDeltaAccu	= m_vPosLast	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

void	CActionMover::Destroy( void )
{
	return;
}

BOOL	CActionMover::ProcessCollision( D3DXVECTOR3 *vPos )
{
	BOOL f	= FALSE;
	BOOL bFly = FALSE;
	
	return( f );
}

//
// ProcessAction()처리전 한번 들르는 곳, ProcessAction()이 네번 돌더라도 이곳은 한번만 들른다
//
void	CActionMover :: ProcessPreAction( void )
{
	m_vDeltaAccu.x	= m_vDeltaAccu.y	= m_vDeltaAccu.z	= 0.0f;
}

//
//		무버 액션 처리
//		현재 지정된 동작상태에 따라 적절한 모션을 플레이 해줌
//	vPos : pMover->m_vPos.  
//
int		CActionMover :: ProcessAction( D3DXVECTOR3 *vPos, int nIdx )
{
//	{
//		CWorld *pWorld = m_pMover->GetWorld();
//		D3DXVECTOR3 v = m_pMover->GetPos();
//		v.y += 1.5;
//		FLOAT f = pWorld->GetUnderHeight( v );
//	}

	if( IsFly() == FALSE )
	{
		m_vDelta.x	= m_vDelta.z	= 0.0f;		
	}
	
	CMover* pMover = m_pMover;

	BOOL	bFly = FALSE;

#ifdef _DEBUG
	if( pMover->IsPlayer() )
	{
		int a	= 0;
	}
	else
	{
		int a	= 0;
	}
#endif	// _DEBUG

	if( pMover->IsDelete() )	return FALSE;
	
	if( pMover->IsFlyingNPC() )		// 비행형 무버인가.
		bFly = TRUE;

	// 아무것도 아닌상태
	if( GetState() == 0 ) 
	{
		SendActMsg( OBJMSG_STAND );

	}
	//--------- 상태 처리 ----------------------------
	// 기본적으로 여러가지 상태가 섞일 수 있다.
	// 대기상태와 / 이동중상태를 처리

	if( GetState() & OBJSTA_MOVE_ALL )
		ProcessState( GetState() & OBJSTA_MOVE_ALL );

	if( GetState() & OBJSTA_TURN_ALL )
		ProcessState( GetState() & OBJSTA_TURN_ALL );

	if( GetState() & OBJSTA_LOOK_ALL )
		ProcessState( GetState() & OBJSTA_LOOK_ALL );

	if( GetState() & OBJSTA_JUMP_ALL )
		ProcessState( GetState() & OBJSTA_JUMP_ALL );

	if( GetState() &	OBJSTA_ATK_ALL )
		ProcessState( GetState() & OBJSTA_ATK_ALL );

	if( GetState() &	OBJSTA_DMG_ALL )
		ProcessState( GetState() & OBJSTA_DMG_ALL );
	
	if( GetState() & OBJSTA_ACTION_ALL )
		ProcessState( GetState() & OBJSTA_ACTION_ALL );
	
	m_bMove = 1;
	if( m_vDelta.x == 0 && m_vDelta.z == 0 )	// x,z좌표 이동이 없다. 움직이지 않았음
		if( m_vDeltaE.x == 0 && m_vDeltaE.y == 0 && m_vDeltaE.z == 0 )
			m_bMove = 0;

	if( m_vDelta.y != 0 || m_bMove )	// delta x, y, z값에 변경이 있었다면 "서있다" 해제
	{
		m_bGround = 0;	// 
	}

	if( fabs(vPos->y - m_vPosLast.y) > 0.001f )
		m_bGround = 0;
	
	// 지상모드
	if( IsFly() == FALSE )	
	{
		// 중력벡터 더함
		if( !bFly && GetDmgState() != OBJSTA_DISAPPEAR )		// 죽어 사라져가는 중엔 중력처리 안함
		{
			if( m_bGround != 1 )		// 땅에 서있는 경우가 아닐때만 중력 내림
			{
				// 중력 처리,
				m_vDelta.y -= 0.005f;
				if( m_vDelta.y < -0.50f )		m_vDelta.y = -0.50f;
			}
		}
	} else
	{
		ProcessFlyMove();
	}

	if( IsDie() )	
		m_vDeltaE.x = m_vDeltaE.y = m_vDeltaE.z = 0;

	m_vDelta += m_vDeltaE;		// 외부힘을 더함.
	m_vDeltaAccu += m_vDelta;

	if( (GetState() & OBJSTA_DMG_FLY) == 0 )
		m_vDeltaE *= 0.9f;			// 외부힘은 마찰때문에 계속 감소.

	m_nCount++;
	
	return 1;
}


int CActionMover::SendActMsg( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 )
{	
	if( IsFly() )
		return ProcessActMsg2( m_pMover, dwMsg, nParam1, nParam2, nParam3, nParam4 );
	return ProcessActMsg1( m_pMover, dwMsg, nParam1, nParam2, nParam3, nParam4 );
}

void CActionMover::ProcessState( DWORD dwState, int nParam ) 
{ 
	if( IsFly() )
	{
		ProcessState2( m_pMover, dwState, nParam );
	}
	else
	{
		ProcessState1( m_pMover,dwState, nParam );
	}
}

BOOL CActionMover::Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld ) 
{
	return TRUE; 
}


//
// XZ평면 각도와 Y(높이쪽)각도 그리고 힘을 넘겨받아 벡터를 생성
// 6시방향 0도 시계반대로 돌아가며 3시방향이 90도
void	CActionMover::DoDamageFly( float fAngleXZ, float fAngleY, float fPower )
{
	float fTheta = D3DXToRadian( fAngleXZ );
	float fTheta2 = D3DXToRadian( fAngleY );
	m_vDelta.y += -cosf( fTheta2 ) * fPower;
	float fDist = sinf( fTheta2 ) * fPower;
	m_vDeltaE.x += sinf( fTheta ) * fDist;
	m_vDeltaE.z += -cosf( fTheta ) * fDist;
}