#include "stdafx.h"
#include "ActionMover.h"

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
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT
#include "defineitem.h"
#include "DefineObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//
//
BOOL	CActionMover::ProcessCollisionFly( D3DXVECTOR3 *vPos )
{
	CMover* pMover = m_pMover;
	
	D3DXVECTOR3 vStart = *vPos;
	D3DXVECTOR3 vDelta = m_vDeltaAccu;

	pMover->SetIAObjLink( NULL );
	vStart.y += 0.5f;	// vPos는 지면의 좌표.  빗자루 높이로 충돌해야하므로 대략 0.5더함
	// vStart-m_vDelta로 이어지는 진행방향에 충돌검사를 하고 충돌했다면 반사된 벡터를 vDelta에 넣어준다.

	BOOL fResult = FALSE;

#ifdef __EH
	try
	{
#endif	// __EH

	if( CObj::m_bCollision ) 
	{
#ifdef __WORLDSERVER
		if( FALSE == pMover->IsPlayer() || TRUE == pMover->IsEmptyDest() )
#else	// __WORLDSERVER
		if( TRUE == pMover->IsActiveMover() || TRUE == pMover->IsEmptyDest() )
#endif	// __WORLDSERVER
			fResult	= pMover->GetWorld()->ProcessCollisionReflection( &vDelta, vStart, m_vDeltaAccu );
	}

	if( fResult == TRUE )	// 뭔가 충돌했다.
	{
		vDelta *= 0.7f;
		m_vDelta = vDelta;
	}
	
//		m_delta -= (m_delta * 0.1);
	FLOAT h = pMover->GetWorld()->GetFullHeight(D3DXVECTOR3( vPos->x, vPos->y + 1.0f, vPos->z ));//0.5f;
	m_fCurrentHeight = h;
	
//	*vPos += vDelta;
	vPos->y += vDelta.y;

	if( vPos->y < h )		// 지상에서의 최대 낮게 날수 있는 고도 2.0미터
	{
		vPos->y		= h;
		fResult = TRUE;
	}
	LPWATERHEIGHT pWaterHeight = pMover->GetWorld()->GetWaterHeight( pMover->GetPos() );
	
	if ( vPos->y > pMover->GetWorld()->m_fMaxHeight )
		vPos->y = pMover->GetWorld()->m_fMaxHeight;
	if( pWaterHeight && pWaterHeight->byWaterTexture == WTYPE_CLOUD )	// 구름층일땐 최저고도를 유지한다.
		if( vPos->y < pMover->GetWorld()->m_fMinHeight )
			vPos->y = pMover->GetWorld()->m_fMinHeight;
	CWorld *pWorld = pMover->GetWorld();

	// x축 더함
	vPos->x += vDelta.x;
#if defined( __ONLINE ) || defined( __WORLDSERVER )
	if( vPos->x <= WLD_EXTRA_WIDTH ) vPos->x = WLD_EXTRA_WIDTH;
	if( vPos->x >= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH ) )	vPos->x	= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH );

	int nAttr = pWorld->GetHeightAttribute( vPos->x, vPos->z );		// 이동할 위치의 속성 읽음.
	switch( nAttr )
	{
	case HATTR_NOMOVE:	// 이동 금지.
		vPos->x -= vDelta.x;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
		break;
	}
#endif // online || worldserver

	// z축 더함
	vPos->z += vDelta.z;
#if defined( __ONLINE ) || defined( __WORLDSERVER )
	if( vPos->z <= WLD_EXTRA_WIDTH ) vPos->z	= WLD_EXTRA_WIDTH;
	if( vPos->z >= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH ) )	vPos->z	= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH );

	if( pWorld->m_bFly )
		nAttr = pWorld->GetHeightAttribute( vPos->x, vPos->z );		
	else
		nAttr = HATTR_NOFLY;

	switch( nAttr )
	{
	case HATTR_NOMOVE:	// 이동 금지.
		vPos->z -= vDelta.z;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
		break;
	case HATTR_DIE:		// 이영역에 들어가면 죽음.
		pMover->DoDie( NULL );
		break;
	case HATTR_NOFLY:	// 비행 금지 - 자동으로 비행모드 풀림.
		pMover->UnequipRide();
		break;
	}
#endif // __ONLINE || __WORLDSERVER
	
#ifdef __EH
	}
	catch( ... )
	{
		WriteError( "%s %d", __FILE__, __LINE__ );
	}
#endif	// __EH

	return fResult;
}

#ifdef __XUZHU
CMover *_LastMover = NULL;
D3DXVECTOR3 __vOld;
#endif

//
//
//
BOOL	CActionMover::ProcessCollisionGround( D3DXVECTOR3 *vPos )
{
	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	D3DXVECTOR3	vKneePos, vDeltaXZ, vTemp, vIntersect;
	D3DXVECTOR3 vDelta = m_vDeltaAccu;
	
#ifdef __XUZHU
	_LastMover = pMover;
	__vOld = *vPos;
#endif

/*
	// m_pMotion이 널이 아니라고 가정하자
	D3DXVECTOR3 *pPath = pModel->m_pMotion->m_pPath;
	if( pPath )	// path가 있는가?
	{
		D3DXVECTOR3 vPathDelta;
		FLOAT fTheta = D3DXToRadian( -pMover->GetAngle() );
		pModel->GetPathDelta( &vPathDelta );		// 이번 프레임의 이동치를 계산해서 
		D3DXMATRIX mRot;
		D3DXMatrixRotationY( &mRot, fTheta );
		D3DXVec3TransformCoord( &vPathDelta, &vPathDelta, &mRot );
		m_vDelta += vPathDelta;						// m_vDelta에 더함.
		// path가 처리되고 있을때는 중력처리 안함.
	}
*/

	vKneePos = *vPos;	vKneePos.y += 0.4f;		// 무릎높이
	vDeltaXZ = vDelta;	vDeltaXZ.y = 0;		// 이동벡터에서 수평이동 성분만 취함

//#ifndef __X0719
	vDeltaXZ *= 6.5f;	// 원래 5.0이었다.
//#endif // not 0719
	
#ifdef __X15
	vDeltaXZ /= 4.0f;	// 서버가 15프레임으로 돌때는 vDeltaXZ도 4배로 커진다. 그래서 줄여줘야 함.
#endif
	// 현재위치와 이동벡터를 넘겨주면 
	// 배경과의 충돌여부를 검사해서 바뀐 이동벡터를 계산한다.
	BOOL bColl = FALSE;
	BOOL fResult = FALSE;

	bColl	= TRUE;
#ifdef __XPET
	if( pMover->GetIndex() >= MI_PET_LAWOLF && pMover->GetIndex() <= MI_PET_LASTINDEX )
		bColl = FALSE;		// 펫은 오브젝트와 충돌검사 안함.
#endif

	if( CObj::m_bCollision && m_bMove && bColl ) {
#ifdef __X0719
		float fLen = D3DXVec3Length( &vDeltaXZ );		// 진행방향 벡터의 길이를 일단 구함.
		FLOAT fR = 1.0f;
		if( fLen > 0.001f )
		{
			if( fLen < 0.65f )	// 이 길이가 0.65f 보다도 짧으면
			{
				fR = 0.65f / fLen;		// 몇배나 작은지 계산해서
				vDeltaXZ *= fR;		// 벡터를 그만큼 늘여줌.
			}
			fResult		= pMover->GetWorld()->ProcessCollision( &vDeltaXZ, vKneePos, vDeltaXZ );
			vDeltaXZ /= fR;	// 충돌계산이 끝난후엔 다시 벡터 길이 원상복귀.
		}
//		else
//			Message( "X0719 %s %f %f %f - %f", pMover->GetName(), vDeltaXZ.x, vDeltaXZ.y, vDeltaXZ.z, fLen );
		
#else
		fResult		= pMover->GetWorld()->ProcessCollision( &vDeltaXZ, vKneePos, vDeltaXZ );
#endif
	}
#ifdef __X15
	vDeltaXZ *= 4.0f;	// 서버가 15프레임으로 돌때는 vDeltaXZ도 4배로 커진다. 그래서 줄여줘야 함.
#endif
//#ifndef __X0719
	vDeltaXZ /= 6.5f;
//#endif // not 0719
	CWorld* pWorld = GetWorld();

	BOOL bCollision = FALSE;
	// x축 더하기.
	vPos->x += vDeltaXZ.x;
#if defined( __ONLINE ) || defined( __WORLDSERVER )
	if( vPos->x <= WLD_EXTRA_WIDTH ) 
		vPos->x = WLD_EXTRA_WIDTH;
	if( vPos->x >= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH ) )	vPos->x	= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH );

	int nAttr = pWorld->GetHeightAttribute( vPos->x, vPos->z );		// 이동할 위치의 속성 읽음.
	switch( nAttr )
	{
	case HATTR_NOWALK:	// 걷기 금지
	case HATTR_NOMOVE:	// 이동 금지.
		if( pMover->GetIAObjLink() == NULL )	// 배를 타고 있을때는 지상에서의 속성이 무시된다.
		{
			vPos->x -= vDeltaXZ.x;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
			bCollision = TRUE;
		}
		break;
	}
#endif // online || worldserver

	// z축 더하기
	vPos->z += vDeltaXZ.z;
#if defined( __ONLINE ) || defined( __WORLDSERVER )
	if( vPos->z <= WLD_EXTRA_WIDTH ) vPos->z	= WLD_EXTRA_WIDTH;
	if( vPos->z >= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH ) )	vPos->z	= ( pWorld->m_nLandWidth * MAP_SIZE * MPU - WLD_EXTRA_WIDTH );
	
	nAttr = pWorld->GetHeightAttribute( vPos->x, vPos->z );		// 이동할 위치의 속성 읽음.
	switch( nAttr )	
	{
	case HATTR_NOWALK:	// 걷기 금지
	case HATTR_NOMOVE:	// 이동 금지.
		if( pMover->GetIAObjLink() == NULL )
		{
			vPos->z -= vDeltaXZ.z;
			bCollision = TRUE;
		}
		break;
	case HATTR_DIE:		// 이영역에 들어가면 죽음. - die는 x, z가 다 더해지고 난후에 검사해야 한다.
		pMover->DoDie( NULL );
		break;
	}
#endif // online || worldserver
	
	if( bCollision )
#ifdef __POSTAIMSG1008
		pMover->PostAIMsg( AIMSG_COLLISION );
#else	// __POSTAIMSG1008
		pMover->SendAIMsg( AIMSG_COLLISION );
#endif	// __POSTAIMSG1008

#ifdef __XUZHU
	if( fabs(vPos->z - __vOld.z) > 10.0f )
	{
		int a=0;
	}
	if( vPos->z > 2000.0f || vPos->z < 800.0f )
	{
		int a = 0;
	}
	
#endif
	

	// 좌표 더해준후 보정(후처리)
	if( GetDmgState() != OBJSTA_DISAPPEAR )		// 죽어 사라지는 중엔 지형충돌검사 안함.
	{
		{
			// 좌표가 내려갔음
			if( vDelta.y < 0 )
			{
				if( m_bGround != 1 )	// 바닥에 그냥 서있는 상태(m_bGround == 1)는 추가적으로 충돌검사 할필요 없다.
				{
#ifdef	_DEBUG
#ifdef	__CLIENT
					if( g_pPlayer == pMover )
					{
						int a = 0;
					}
#else
					if( pMover->IsPlayer() )
					{
						int a = 0;
					}
#endif
#endif
					
					D3DXVECTOR3 vSlide;
					// 미끄러진후 바뀐 vDelta
					// vPos.xz 지점의 h값.
					vSlide.x = vSlide.z = vSlide.y = 0;	// slide벡터.
					FLOAT h;
					CObj *pObjColl = NULL;
					if( 1 || bColl )	// 현재는 무조건 이것은 TRUE다
						h = pMover->GetWorld()->ProcessUnderCollision( &vSlide, &pObjColl, D3DXVECTOR3( vPos->x, vPos->y + 1.0f, vPos->z ) );
					else
						h = pMover->GetWorld()->GetLandHeight( vPos->x, vPos->z );
					m_fCurrentHeight = h;
					// 캐릭터 머리에서 아래로 라인을 내리고 걸리는게 있으면 적당한 값으로 슬라이드 계산후 적용.
					// 걸리는게 없다면 공중에 떠있는것이므로 vPos->y += delta.y만 해주고 h값으로 계산.
					// 단 슬라이드 계산할때는 면이 일정각이상으로 기울어져 있을때만 적용. 안그러면 모든 경사면은 다 미끄러져 못올라간다.
					// 슬라이드를 더할때는 실제로 바닥에 닿았을때만 더해준다.
					// 슬라이드 적용후 한번더 높이 계산해서 보정해줘야 할거 같다.
					// 운동량을 더해줌
					D3DXVECTOR3 tempV = *vPos;
					vPos->y += vDelta.y;
					
					if( GetJumpState() == OBJSTA_SJUMP3 )	// 하강동작 상태였을때
					{
 						if( vPos->y < h )	// 바닥에 닿으면
						{
//							fResult = TRUE;		// 수직축으로는 충돌플랙을 리턴해선 안된다.
							if( vSlide.y )
							{
								*vPos = vSlide;
								m_bGround = 2;	// 땅에 닿긴 했는데 미끄러 지고 있다.
#if defined( __ONLINE ) || defined( __WORLDSERVER )
#ifdef __POSTAIMSG1008
								pMover->PostAIMsg( AIMSG_COLLISION );
#else	// __POSTAIMSG1008
								pMover->SendAIMsg( AIMSG_COLLISION );
#endif	// __POSTAIMSG1008
#endif // online || worldserver
							} else
							{
								vPos->y = h;
								m_bGround = 1;	// 미끄러 지지 않고 있다.
							}

							if( pObjColl )
							{
								if( pObjColl->GetType()	== OT_SHIP )	// 착지한 오브젝트가 SHIP이면
									pMover->OnActIALanding( pObjColl, *vPos );
								else
									pMover->SetIAObjLink( NULL );
							} else
								pMover->SetIAObjLink( NULL );	// 착지 했는데 오브젝트가 아니면 IA에서 내린걸로 간주.
							
//							else
//								Message( "CActionMover::ProcessCollisionGround - 착지는 했는데 오브젝트가 없다. %f %f %f", pMover->GetPos().x, pMover->GetPos().y, pMover->GetPos().z );
							m_vDelta.y = 0;
						}
//						else
//							pMover->SetIAObjLink( NULL );	// 발이 떨어지면 링크를 풀어줌 - 배에서 제자리 점프 한경우엔 링크를 끊지 말자. 확실히 다른곳에 착지했을곳만 링크를 끊자
					} else	// 그냥 좌표만 내려가고 있던상태( 비탈길을 내려간다거나.  걸어다가 빠졌다거나)
					{
						if( vPos->y - 0.25f > h )	// 발바닥 아래 xx지점을 더 내려서 검사했는데도 바닥보다 높다면 
						{
//							pMover->SetIAObjLink( NULL );	// 확실하게 다른곳에 착지했을때만 링크를 끊게 바꿔서 이건 삭제
							if( vDelta.y < -0.1f )		// 떨어지는 속도가 0.x보다 클때만 하강동작.  살짝 떨어지는건 걍 좌표만 내려감.
							{
								SetJumpState( OBJSTA_SJUMP3 );		// 하강 상태로 전환
								pMover->OnActDrop();				// 이벤트 호출
							}
						} else		// 발아래 0.5지점내에 바닥이 있다면 그냥 발바닥 붙여줌
						{
//							fResult = TRUE;
							m_vDelta.y = 0;
							if( vSlide.y )
							{
								*vPos = vSlide;
								m_bGround = 2;
#if defined( __ONLINE ) || defined( __WORLDSERVER )
#ifdef __POSTAIMSG1008
								pMover->PostAIMsg( AIMSG_COLLISION );
#else	// __POSTAIMSG1008
								pMover->SendAIMsg( AIMSG_COLLISION );
#endif	// __POSTAIMSG1008
#endif // online || worldserver
							} else
							{
								vPos->y = h;
								m_bGround = 1;
							}
							
							if( pObjColl )
							{
								if( pObjColl->GetType()	== OT_SHIP )	// 착지한 오브젝트가 SHIP이면
									pMover->OnActIALanding( pObjColl, *vPos );
								else
									pMover->SetIAObjLink( NULL );
							} else
								pMover->SetIAObjLink( NULL );	// 착지 했는데 오브젝트가 아니면 IA에서 내린걸로 간주.
						}
					}
				} // m_bGround
			} else
			// 좌표가 올라갔음
			if( vDelta.y > 0 )
			{
//				pMover->SetIAObjLink( NULL );	
				// x, y위치의 천장 높이.
				FLOAT h	= pMover->GetWorld()->GetOverHeight( D3DXVECTOR3( vPos->x, vPos->y + 1.5f, vPos->z ) );
				
				// 운동량을 더해줌
				vPos->y += vDelta.y;
				if( vPos->y + 1.5f > h )	// 머리 부분이 천장에 닿았으면 
				{
					fResult = TRUE;			// 위로 올라가는건 체크해야한다,.
					vPos->y = h - 1.5f ;	// y좌표 보정
					m_vDelta.y = -0.01f;			// 올라가던거 멈춤
				}
			}
		}
	}

//	if( fResult )
//	{
//		int a=0;
//	}

#ifdef __XUZHU
if( fabs(vPos->z - __vOld.z) > 10.0f )
{
	int a=0;
}
if( vPos->z > 2000.0f || vPos->z < 800.0f )
{
	int a = 0;
}
#endif

	
	return fResult;
}

