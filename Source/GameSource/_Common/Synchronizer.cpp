#include "StdAfx.h"
#include "Synchronizer.h"
#include "Mover.h"


AutoSynchronizer::AutoSynchronizer( CMover* pMover )
:	m_pMover( pMover )
,	m_vSyncPos( 0.0f, 0.0f, 0.0f )
,	m_fSyncAngle( 0.0f )
,	m_fSyncAngleX( 0.0f )
,	null_vec3( 0.0f, 0.0f, 0.0f )
,	m_collisionSynchronizeTime( 0 )
{
}

AutoSynchronizer::~AutoSynchronizer()
{
}

void AutoSynchronizer::Initialize()
{
	m_vSyncPos		= m_pMover->GetPos();
	m_fSyncAngle	= m_pMover->GetAngle();
	m_fSyncAngleX	= m_pMover->GetAngleX();
#ifdef __CLIENT
	UpdateGhost();
#endif __CLIENT
}

void AutoSynchronizer::Serialize( CAr & ar )
{
	if( ar.IsStoring() )	{
		ar << m_pMover->GetPos() << m_pMover->GetAngle() << m_pMover->GetAngleX() << m_pMover->m_pActMover->m_vDelta;
		ar << m_pMover->m_dwMotion << m_pMover->m_dwMotionOption << m_pMover->m_pActMover->m_nMotionEx << m_pMover->m_pModel->m_nLoop;
		ar << m_pMover->m_pActMover->GetState() << m_pMover->m_pActMover->GetStateFlag();
		ar << static_cast<bool>( m_pMover->IsEmptyDest() == TRUE );
	}
	else	{
		ar >> vSyncPos >> fAngle >> fAngleX >> vDelta;
		ar >> dwMotion >> dwMotionOption >> nMotionEx >> nLoop;
		ar >> dwState >> dwStateFlag;
		ar >> bEmptyDest;
		Synchronize();
	}
}

#ifdef __WORLDSERVER
AutoSynchronizer::Check_Result AutoSynchronizer::CheckValidSynchronize()
{
	if( ( m_pMover->IsFly() == TRUE ) != ( ( dwStateFlag & OBJSTAF_FLY ) == OBJSTAF_FLY ) )
		return invalid;
	
	const D3DXVECTOR3 vDistance = m_pMover->GetPos() - vSyncPos;
	const float fDistance = ::D3DXVec3LengthSq( &vDistance );
	const float fAllowedFrame = 60.0f;	// 1 sec
	const float fLimitDistance = m_pMover->IsFly() == TRUE	?	::pow( ( 500000.0f / ( 60.0f* 60.0f * 60.0f ) ) * fAllowedFrame, 2 )	/*시속500km일때*/
															:	::pow( m_pMover->GetSpeed( m_pMover->m_pActMover->m_fSpeed ) * fAllowedFrame, 2 ); // 동기 제한 거리
	return fDistance <= fLimitDistance ? valid : replace;
}
#endif // __WORLDSERVER

void AutoSynchronizer::SendSynchronize()
{
#ifdef __WORLDSERVER
	g_xWSUserManager->AddMoverBehavior( m_pMover );
#endif // __WORLDSERVER
#ifdef __CLIENT
	g_DPlay.SendPlayerBehavior();
#endif // __CLIENT
}

void AutoSynchronizer::CollisionState( BOOL bCollision )
{
#ifdef __WORLDSERVER
	if( m_pMover->IsPlayer() == TRUE )
		return;
#endif // __WORLDSERVER
#ifdef __CLIENT
	if( m_pMover->IsActiveMover() == FALSE )
		return;
#endif // __CLIENT
	if( bCollision )	{
		const DWORD	collisionSynchronizeInterval = SEC( 1 );
		if( m_collisionSynchronizeTime <= GetTickCount() - collisionSynchronizeInterval )	{
			SendSynchronize();
			m_collisionSynchronizeTime = GetTickCount();
		}
	}
	else	{
		if( m_collisionSynchronizeTime != 0 )
			SendSynchronize();
		m_collisionSynchronizeTime = 0;
	}
}

void AutoSynchronizer::Synchronize()
{
	if( ::IsValidObj( m_pMover ) == FALSE || m_pMover->m_pActMover == NULL )
		return;

#ifdef __WORLDSERVER
	switch( CheckValidSynchronize() )	{
		case valid:
#endif // __WORLDSERVER
			m_pMover->SetPos( vSyncPos );
			m_pMover->SetAngle( fAngle );
			m_pMover->SetAngleX( fAngleX );
			m_pMover->m_pActMover->m_vDelta = vDelta;
			
			m_pMover->m_pActMover->__ForceSetState( dwState );	// 여기 이외엔 이함수 사용하지 말것.
			m_pMover->m_pActMover->ClearStateFlag();
			m_pMover->m_pActMover->AddStateFlag( dwStateFlag );
			m_pMover->m_pActMover->m_nMotionEx = nMotionEx;
			m_pMover->SetMotion( dwMotion, nLoop, dwMotionOption );
			if( bEmptyDest )	{
#ifdef __WORLDSERVER
				if( m_pMover->GetCmd() == OBJACT_USESKILL )
					m_pMover->ProcessMoveArrivalBySynchronizer();									
#endif // __WORLDSERVER
				m_pMover->ClearDest();
			}
#ifdef __WORLDSERVER
			SendSynchronize();
			break;
		case replace:
			m_pMover->REPLACE( g_uIdofMulti, m_pMover->GetWorld()->GetID(), m_pMover->GetPos(), REPLACE_FORCE, m_pMover->GetLayer() );
			break;
		case invalid:
		default:
			break;
	}
#endif // __WORLDSERVER
}

#ifdef __CLIENT
void AutoSynchronizer::UpdateGhostPos()
{
	if( m_pMover->IsPlayer() )	{
		if( m_pMover->m_pActMover->IsActAttack() && m_pMover->m_pActMover->m_vDeltaE == null_vec3 )	// 플레이어는 공격중일때 동기하지 않는다.
			return;
	}
	else	{
		if( m_pMover->GetProp() && m_pMover->GetProp()->dwAI == AII_PET && !m_pMover->m_pActMover->IsMove() )	// 펫은 움직일때만 동기
			return;
	}
				
	D3DXVECTOR3 vPos = m_pMover->GetPos();
	vPos.y += 0.07f;	// 구두 굽 높이만큼 올린다.
	
	const float criteria = 0.2f * 0.2f;	// 차이가 0.2 미터 이하면 순간 이동 한다.
	const float distance = ::D3DXVec3LengthSq( &D3DXVECTOR3( vPos - m_vSyncPos ) );
	const bool bGhostUpdate =	(	!m_pMover->IsActiveMover() 
								&&	( m_pMover->m_pActMover->IsMove() || distance > criteria )
								&&	m_vSyncPos != null_vec3
								);
								
	
	if( bGhostUpdate )	{
		const float rate = m_pMover->IsFly() ? 0.05f : 0.2f;		// 동기 가중치 - 비행중일때 더 느리게 한다.
		::D3DXVec3Lerp( &m_vSyncPos, &m_vSyncPos, &vPos, rate );	// 위에서 설정한 가중치로 동기화 한다.
	}
	else	{
		m_vSyncPos	= vPos;
	}

	D3DXMATRIX matTrans;
	::D3DXMatrixTranslation( &matTrans, m_vSyncPos.x, m_vSyncPos.y, m_vSyncPos.z );
	m_pMover->SetMatrixTrans( matTrans );
	m_pMover->SetUpdateMatrix( TRUE );
}

void AutoSynchronizer::UpdateGhostAngle()
{
	const float fAngle = m_pMover->GetAngle();
	const float fAngleX = m_pMover->GetAngleX();
	D3DXMATRIX	matRotation = m_pMover->GetMatrixRotation();
	if( m_pMover->IsActiveMover() )	{
		if( m_pMover->IsFly() )
			m_fSyncAngle = fAngle;
		m_fSyncAngleX = fAngleX;
	}

	const float rate = m_pMover->IsFly() ? 16.0f : 4.0f;
	// Angle
	if( ::fabs( fAngle - m_fSyncAngle ) > 0.001f ) 	{
		float d = fAngle - m_fSyncAngle;
		if( ::fabs( d ) < 4.0f )
			m_fSyncAngle = fAngle;
		if( d < -180.0f )
			d += 360.0f;
		else if( d > 180.0f )
			d -= 360;

		m_fSyncAngle += d / rate;

		if( m_fSyncAngle > 360.0f )
			m_fSyncAngle -= 360.0f;
		else if( m_fSyncAngle < 0.0f )
			m_fSyncAngle += 360.0f;
	}

	// AngleX
	if( fabs( fAngleX - m_fSyncAngleX ) > 0.001f )	{
		float d = fAngleX - m_fSyncAngleX;
		if( fabs( d ) < 4.0f )
			m_fSyncAngleX = fAngleX;
		m_fSyncAngleX += d / rate;

		if( m_fSyncAngleX > 45.0f )
			m_fSyncAngleX = 45.0f;
		else if( m_fSyncAngleX < -45.0f )
			m_fSyncAngleX = -45.0f;
	}

	::D3DXMatrixRotationY( &matRotation, D3DXToRadian( -m_fSyncAngle ) );
	D3DXMATRIX matRotX;
	::D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fSyncAngleX ) );
	matRotation *= matRotX;
	::D3DXMatrixRotationYawPitchRoll( &matRotation, D3DXToRadian( -m_fSyncAngle ), D3DXToRadian( -m_fSyncAngleX ), 0.0f );
	m_pMover->SetMatrixRotation( matRotation );
	m_pMover->SetUpdateMatrix( TRUE );
}

void AutoSynchronizer::UpdateGhost()
{
	UpdateGhostPos();
	UpdateGhostAngle();
}
#endif // __CLIENT