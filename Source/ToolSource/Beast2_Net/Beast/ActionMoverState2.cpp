#include "stdafx.h"
#include "defineObj.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		FRIC_AIR	0.011f		// 비행중 마찰계수


void	CActionMover::ProcessFlyMove( void )
{
	CMover* pMover = m_pMover;
	
	// 비행모드
	D3DXVECTOR3		vAcc;
	// 관성 처리
	FLOAT	fAngX = D3DXToRadian( pMover->GetAngleX() );		// X축 각도
	FLOAT	fAccPwr = m_fAccPower;

	// 자기 직업에 맞지 않는 탈것을 타면 탈것의 능력에 페널티가 생긴다.
		
	FLOAT	fDist = cosf(fAngX) * fAccPwr;		// fAng, fAngX, fAccPower에 의한 힘벡터 생성
	FLOAT	fAng = D3DXToRadian( pMover->GetAngle() );
	
	vAcc.x = sinf(fAng) * fDist;
	vAcc.z = -cosf(fAng) * fDist;
	vAcc.y = -sinf(fAngX) * fAccPwr;

#define		COS45	0.7071f

	float fLenSq = D3DXVec3LengthSq( &m_vDelta );
 	
	extern int g_nDrift;
	// 드리프트 처리.
	D3DXVECTOR3 vDeltaNorm, vAccNorm;
	D3DXVec3Normalize( &vDeltaNorm, &m_vDelta );
	D3DXVec3Normalize( &vAccNorm, &vAcc );
	float fDot = D3DXVec3Dot( &vDeltaNorm, &vAccNorm );	// 관성방향과 추진방향의 내적
	
//	TRACE( "ProcessFlyMove 1-> %0.3f, %0.3f - %0.3f, %0.3f, %3.3f, %3.3f\n", m_vDelta.x, m_vDelta.z, vAcc.x, vAcc.z, fAng, pMover->GetAngle() );
	if( fAccPwr > 0 )
	{
		if( fLenSq > 0.01f )
		{
//			if( fDot < COS45 )		// 관성방향과 추진방향의 각도가 cos45보다 크면(내적부호는 반대).
			if( fDot < cosf(70.0f) )	// 이거 라디안으로 변환해야한다.
			{
				vAcc *= 2.0f;		// 급추진을 얻는다.
				m_vDelta *= 0.985f;
			}
		}
	}
		
//	TRACE( "ProcessFlyMove 2-> %0.3f, %0.3f - %0.3f, %0.3f\n", m_vDelta.x, m_vDelta.z, vAcc.x, vAcc.z );
	fLenSq = D3DXVec3LengthSq( &m_vDelta );		// 1/60 sec 속도
	float	fMaxSpeed = 0.3f;
	if( GetStateFlag() & OBJSTAF_TURBO )		// 터보모드에선 가속력 x배속.
	{
		fMaxSpeed *= 1.1f;
	}
		
	if( fLenSq < (fMaxSpeed * fMaxSpeed) )		// 일정이상 속도를 넘지 않게 하자.
	{
		m_vDelta += vAcc;	// 기존 관성(속도)에 새로운 힘벡터를 더해서 새로운 관성을 생성
	}
	m_vDelta *= (1.0f - FRIC_AIR);
//	TRACE( "ProcessFlyMove 3-> %0.3f, %0.3f - %0.3f, %0.3f\n", m_vDelta.x, m_vDelta.z, vAcc.x, vAcc.z );

}

//
//	State process
//	가급적 이곳에선 pMover->SetMotion을 하지 말것
//	빗자루 비행모드
//
void	CActionMover::ProcessState2( CMover* pMover,  DWORD dwState, int nParam )
{


}

//
//		비행중 공격 액션 처리
//
void	CActionMover::_ProcessStateAttack2( DWORD dwState, int nParam )
{

}
