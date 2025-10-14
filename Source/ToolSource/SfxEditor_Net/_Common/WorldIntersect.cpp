#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	vPos지점의 지형 높이를 계산.  지형, 배경오브젝트 모두 포함이다.
//
FLOAT CWorld::GetFullHeight( D3DXVECTOR3 vPos, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, -2000.0f, 0.0f );	// 위로 1000 아래로 1000 최대 거리...
	D3DXVECTOR3 vIntersect, vEnd;
	FLOAT fDist, fDistMin = 4194304.0f;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	//return GetLandHeight( vPos.x, vPos.z );
	vEnd = vPos + vDir;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
#ifdef	__WORLDSERVER
#else	// __WORLDSERVER
		if( pObj->GetType() == OT_OBJ ) //&& pObj->GetIndex() >= 15 && pObj->GetIndex() != 27 && pObj->GetIndex() != 86 && pObj->GetIndex() != 87 )
#endif	// __WORLDSERVER
		{
			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				vIntersect -= vPos;		// vPos로부터의 상대벡터
				fDist = D3DXVec3LengthSq( &vIntersect );	// 상대적인 거리
				if( fDist < fDistMin )	// 더 가까운놈이 있을지 모르니 가장 가까운 오브젝트를 개려낸다.
				{
					fDistMin = fDist;
					pMinObj = pObj;
				}
			}
		}
	}
	END_LINKMAP
	
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가	
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
		D3DXVECTOR3 *pTri = pModel->IntersectRayTri( vPos, vDir, pMinObj->GetMatrixWorld(), &vIntersect, &fDist );
		if( pTri )
			return vIntersect.y;
	}
	return GetLandHeight( vPos.x, vPos.z );
}

//
//	vPos바로 아래쪽의 높이 계산.  바로 아래쪽에 걸리는게 없다면 무조건 땅바닥 높이로 계산.
//
FLOAT CWorld::GetUnderHeight( D3DXVECTOR3 &vPos, float fUnder, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, fUnder, 0.0f );	// 키높이(1.5) + 여유분(1.0)
	D3DXVECTOR3 vIntersect, vEnd;
	FLOAT fDist;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	//return GetLandHeight( vPos.x, vPos.z );
	vEnd = vPos + vDir;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
#ifdef	__WORLDSERVER
#else	// __WORLDSERVER
		if( pObj->GetType() == OT_OBJ ) //&& pObj->GetIndex() >= 15 && pObj->GetIndex() != 27 && pObj->GetIndex() != 86 && pObj->GetIndex() != 87 )
#endif	// __WORLDSERVER
		{
			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				pMinObj = pObj;		// 찾았으면 바로 튀어나옴 - 속도땜에 이랬다...
				goto LP1;
			}
		}
	}
	END_LINKMAP
LP1:
	
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가	
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
//		D3DXVECTOR3 *pTri = pModel->IntersectRayTri( vPos, vDir, pMinObj->GetMatrixWorld(), &vIntersect, &fDist );
//		if( pTri )
//			return vIntersect.y;
		D3DXVECTOR3	*pTri[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTri )
		{
			vIntersect = vPos + fDist * vDir;
			return vIntersect.y;
		}
	}
	return GetLandHeight( vPos.x, vPos.z );
}

//
//	vPos바로 위쪽의 높이 계산.  
//
FLOAT CWorld::GetOverHeight( D3DXVECTOR3 &vPos, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );	// 
	D3DXVECTOR3 vIntersect, vEnd;
	FLOAT fDist;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	//return GetLandHeight( vPos.x, vPos.z );
	vEnd = vPos + vDir;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
#ifdef	__WORLDSERVER
#else	// __WORLDSERVER
		if( pObj->GetType() == OT_OBJ )// && pObj->GetIndex() >= 15 && pObj->GetIndex() != 27 && pObj->GetIndex() != 86 && pObj->GetIndex() != 87 )
#endif	// __WORLDSERVER
		{
			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				pMinObj = pObj;		// 찾았으면 바로 튀어나옴
				goto LP1;
			}
		}
	}
	END_LINKMAP
LP1:
	
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가	
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
//		D3DXVECTOR3 *pTri = pModel->IntersectRayTri( vPos, vDir, pMinObj->GetMatrixWorld(), &vIntersect, &fDist );
//		if( pTri )
//			return vIntersect.y;
		D3DXVECTOR3	*pTri[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTri )
		{
			vIntersect = vPos + fDist * vDir;
			return vIntersect.y;
		}
	}
	return 65535.0f;	// 걸리는게 없으면 최대치로 설정
}

DWORD	_dwA, _dwB, _dwC;


static int	_nSlideCnt2 = 0;

//
//	vPos-vDir의 이동벡터를 받아 배경과의 충돌을 검사하고
//	그 후처리를 계산하여 pOut으로 넘겨준다.
//
BOOL	CWorld::ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
{
	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vEnd, vIntersect, vSlide;
	
	vEnd = vPos + vDir;		// 레이를 선분으로 바꾼다.
	
	*pOut = vDir;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic )
	{
		if( pObj->GetType() == OT_CTRL ) 
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect, FALSE ) == TRUE )
			{
				if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld(), 0 ) == TRUE )
				{
					// 진행방향으로 뭔가 충돌했으면 한번더 돈다. 리커시브로.
					D3DXVECTOR3 vE = vIntersect + vSlide;		// 미끄러진 끝점.
					*pOut = vSlide;
//					*pOut = vE - vPos;		// 시작점에서 미끄러진끝점으로의 벡터를 이동벡터로 취함.
//					*pOut = vIntersect - vPos;
					if( ++_nSlideCnt2 <= 1 )
					{
//						D3DXVec3Normalize( &vE, &vSlide );
//						vE *= 0.5f;
						int r = ProcessCollision( pOut, vPos, vSlide );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
						if( r )
						{
							pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
						}
					}
					_nSlideCnt2 = 0;
					return 1;
				}
			}
		}
	}
	END_LINKMAP
		
		
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
#ifdef	__WORLDSERVER
#else	// __WORLDSERVER
		if( pObj->GetType() == OT_OBJ ) 
#endif	// __WORLDSERVER
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				// 선분과 삼각형의 교차 검사.  교차한다면 slide vector를 구하고
				// 교차하지 않는다면 걍 FALSE를 리턴한다.
				if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld() ) == TRUE )
				{
					// 진행방향으로 뭔가 충돌했으면 한번더 돈다. 리커시브로.
//					D3DXVECTOR3 vE = vPos + *pOut;		// 미끌어진곳의 끝지점
//					D3DXVECTOR3 vDir2 = vE - vIntersect;		// 충돌한지점에서 미끄러진지점까지의 방향벡터.
					D3DXVECTOR3 vE = vIntersect + vSlide;		// 미끄러진 끝점.
					*pOut = vSlide;
//					*pOut = vE - vPos;		// 시작점에서 미끄러진끝점으로의 벡터를 이동벡터로 취함.
//					*pOut = vIntersect - vPos;
					if( ++_nSlideCnt2 <= 1 )
					{
//						D3DXVec3Normalize( &vE, &vSlide );
//						vE *= 0.5f;
						int r = ProcessCollision( pOut, vPos, vSlide );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
//						int r = ProcessCollision( pOut, vPos, *pOut );		// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
						if( r )	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
						{
							pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
						}
					}
					_nSlideCnt2 = 0;
					return 2;
				}
			}
		}
	}
	END_LINKMAP
		
	_nSlideCnt2 = 0;
		
	return FALSE;
}


static int _nRecusCnt = 0;		// 무한 리커전 방지용.
//
//	vPos-vDir의 이동벡터를 받아 배경과의 충돌을 검사하고
//	그 후처리를 계산하여 pOut으로 넘겨준다.
//
BOOL	CWorld::ProcessCollisionReflection( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
{
	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vEnd, vIntersect, vN, v1;
	D3DXMATRIX mTM;
	FLOAT	fDist;
	D3DXVECTOR3	*pTri[3];

	vEnd = vPos + vDir;		// 레이를 선분으로 바꾼다.

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				// 선분과 삼각형의 교차 검사.  pTri에 담겨나온다.
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist );
				if( *pTri )
				{
					// (2*(-D dot N)) * N + D
					v1 = *pTri[1] - *pTri[0];
					vN = *pTri[2] - *pTri[0];
					D3DXVec3Cross( &vN, &v1, &vN );
//					CalcFaceUnitNormal( &vN, *(pTri[0]), *(pTri[1]), *(pTri[2]) );	// 충돌한면의 노말
					mTM = pObj->GetMatrixWorld();
					mTM._41 = mTM._42 = mTM._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mTM );	// mTM에 스케일이 있다면 노말도 같이 커지기땜에 Normalize를 나중에 해야 한다.
					D3DXVec3Normalize( &vN, &vN );
					
					v1.x = -vDir.x;	v1.y = -vDir.y;	v1.z = -vDir.z;	// -D
					FLOAT fLenN = D3DXVec3Dot( &v1, &vN );			// -D dot N
					vN *= (fLenN * 2.0f);							// (-D dot N) * 2  이때 N는 단위벡터 상태여야한다.
					*pOut = vDir + vN;								// 최종 반사벡터.

					if( _nRecusCnt++ < 3 )
						ProcessCollisionReflection( pOut, vPos, *pOut );
					
					_nRecusCnt = 0;

					return TRUE;
				}
			}
		}
	}
	END_LINKMAP

	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkAirShip )
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				// 선분과 삼각형의 교차 검사.  pTri에 담겨나온다.
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist );
				if( *pTri )
				{
					// (2*(-D dot N)) * N + D
					v1 = *pTri[1] - *pTri[0];
					vN = *pTri[2] - *pTri[0];
					D3DXVec3Cross( &vN, &v1, &vN );
	//					CalcFaceUnitNormal( &vN, *(pTri[0]), *(pTri[1]), *(pTri[2]) );	// 충돌한면의 노말
					mTM = pObj->GetMatrixWorld();
					mTM._41 = mTM._42 = mTM._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mTM );	// mTM에 스케일이 있다면 노말도 같이 커지기땜에 Normalize를 나중에 해야 한다.
					D3DXVec3Normalize( &vN, &vN );
					
					v1.x = -vDir.x;	v1.y = -vDir.y;	v1.z = -vDir.z;	// -D
					FLOAT fLenN = D3DXVec3Dot( &v1, &vN );			// -D dot N
					vN *= (fLenN * 2.0f);							// (-D dot N) * 2  이때 N는 단위벡터 상태여야한다.
					*pOut = vDir + vN;								// 최종 반사벡터.

					if( _nRecusCnt++ < 3 )
						ProcessCollisionReflection( pOut, vPos, *pOut );
					
					_nRecusCnt = 0;

					return TRUE;
				}
			}
		}
		END_LINKMAP
	}

	return FALSE;
}


// 좌표 주위 3x3그리드 영역의 삼각형좌표를 스캔해 vDir과 충돌하는 삼각형을 찾아낸다.
D3DXVECTOR3 *FindTouchLandTri( D3DXVECTOR3 *pTris, int nMaxTri, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, FLOAT *pfDist )
{
	int		i;
	D3DXVECTOR3	*pv0, *pv1, *pv2;
	D3DXVECTOR3	v1, v2;
	FLOAT	fDist;
	BOOL	bRet;
	
	for( i = 0; i < nMaxTri; i ++ )
	{
		pv0 = pTris++;		pv1 = pTris++;		pv2 = pTris++;
		
		bRet = IsTouchRayTri( pv0, pv1, pv2, &vPos, &vDir, &fDist );
		if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
		{
//			if( fDist < 1.0f )	// 레이 길이 안에 있는놈만 검사.
			{
				*pfDist = fDist;
				return pv0;
			}
		}
	}
	
	return NULL;
}
//
// 중력방향의 충돌검사 및 미끄러짐 처리.
// 중력방향의 라인과 충돌하는 삼각형을 찾고 그 지점의 좌표를 얻고 Slide계산후의 좌표도 얻음.
// pType : 충돌한 오브젝트의 타입. OT_OBJ냐 OT_SHIP이냐.
FLOAT CWorld::ProcessUnderCollision( D3DXVECTOR3 *pOut, CObj **pObjColl, D3DXVECTOR3 &vPos, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, -1.5f, 0.0f );	// 키높이(1.5) +	여유분(0.5)
	D3DXVECTOR3 vIntersect, vEnd;
	float	fMaxY = -65535.0f;	// 충돌한 좌표중 가장 높은 위치를 찾기 위한...
	D3DXVECTOR3 vOut( 0, 0, 0 );		// *pOut에 최종적으로 넘겨줄 값.

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	//return GetLandHeight( vPos.x, vPos.z );
	vEnd = vPos + vDir;
	int		nCount = 0;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
		{
			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			nCount ++;
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
				if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
				{
					if( vIntersect.y > fMaxY )
					{
						fMaxY = vIntersect.y;	// // 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
						*pOut = vOut;
						*pObjColl = pObj;	// 충돌한 오브젝트.
					}
				}
			}
		}
	}
	END_LINKMAP

	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkAirShip )
		{
			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			nCount ++;
			pModel = pObj->m_pModel;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
				if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
				{
					if( vIntersect.y > fMaxY )
					{
						fMaxY = vIntersect.y;	// // 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
						*pOut = vOut;
						*pObjColl = pObj;	// 충돌한 오브젝트.
					}
				}
			}
		}
		END_LINKMAP
	}

	D3DXVECTOR3	v1, v2;
	// 현재 위치의 하이트맵 삼각형 읽음
	// 수직벡터로 인터섹트 지점 구하고(높이) 슬라이드 계산.
	// 
	D3DXVECTOR3 pTri[3 * 9 * 2];		// 삼각형 54개
	FLOAT	fDist;
	FLOAT	vx, vz;

	// 바닥 노말 구함
	vx = vPos.x;	vz = vPos.z;
	GetLandTri( vx, vz, pTri );
	v1 = pTri[1] - pTri[0];		// 노말 계산 시작.
	v2 = pTri[2] - pTri[0];
	D3DXVec3Cross( &v1, &v1, &v2 );		// 바닥의 노말 계산.
	D3DXVec3Normalize( &v1, &v1 );		// 단위벡터로 변환

	// 수직벡터와 바닥면의 충돌지점 구함.
	BOOL b = IsTouchRayTri( &pTri[0], &pTri[1], &pTri[2], &vPos, &vDir, &fDist );
	vIntersect.x = vx;
	vIntersect.z = vz;
	vIntersect.y = (vPos.y + fDist * vDir.y); + 0.01f;		// 수직레이와 바닥삼각형의 교차점 - 이것은 높이로도 쓰인다.
	
  #ifdef _XDEBUG
	if( b == FALSE )
	{
//		GetLandTris( vx, vz, pTri );
		Message( "ProcessUnderCollision : Intersect실패. 캐릭터의 좌표가 맵을 벗어났다. %d %d", vPos.x, vPos.z );
	}
  #endif
	
	
	v2.x = v2.z = 0;	v2.y = -1.0f;	// 이건 반드시 단위벡터여야 한다.
	// -0.19080f == cos(101)   -0.37864 == cos(112)
	D3DXVECTOR3 *p = NULL, vTemp9;
	int nMaxTri = 0;
	if( D3DXVec3Dot( &v1, &v2 ) > -0.37864f )	// dot > cos(theta) 일정각 이상 기울어진 면만 슬라이드를 적용시킨다. 두벡터다 노말이어야 한다.
	{
//		v2.y = -1.5f;
		v2.y = -0.2f;
		CalcSlideVec( &vOut, v2, v1 );		// 미끄러짐 벡터 구함
		vTemp9 = vOut;
		nMaxTri = GetLandTris( vx, vz, pTri ) / 3;	// vPos주위의 9개 그리드삼각형 읽음.
		p = FindTouchLandTri( pTri, nMaxTri, vIntersect, vOut, &fDist );	// 교차점에서 slide방향쪽의 삼각형이 충돌하는게 있는지...
		if( p )
		{
//			*pOut *= fDist;		// slide벡터가 다른면에 부딪힌 부분에서 끝나게 한다.
			vOut = vIntersect + vOut * fDist;
		} else
			vOut = vIntersect + vOut;

#ifdef __XUZHU
		if( fabs(vPos.z - vOut.z) > 10.0f )
		{
			int a=0;
		}
#endif
		
	}

	if( vIntersect.y > fMaxY )
	{
		fMaxY = vIntersect.y;
		*pOut = vOut;
	}

	return fMaxY;
}

//
//	라인과 주변오브젝트와의 충돌을 검사해서, 교차점을 계산한다.
// 주의 : 검색대상 오브젝트는 시야절두체에 들어오는 오브젝트만 대상으로 한다.
// 카메라 충돌은 이걸로 하면 안된다. 카메라가 vPos라서 절두체에 속하는 오브젝트가 계속 바뀐다.
//
BOOL	CWorld::IntersectObjLine( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans, BOOL bWithTerrain, BOOL bWithObject )
{
	D3DXVECTOR3	vDir;
#ifdef __CLIENT
	if( bWithObject )
	{
		CObj* pObj;
		CObj* pNonCullObjs[ 10000 ];
		CObj **pList = m_aobjCull;
		int nNonCullNum = 0;
		for( int i = 0; i < m_nObjCullSize; i++ )
		{
			pObj = *pList++;
			if( pObj )
			{
				if( pObj->GetType() == OT_OBJ || pObj->GetType() == OT_CTRL )
				{
					if( pObj->IsCull() == FALSE /*&& pObj->GetModel()->m_pModelElem->m_dwDistant <= 1*/ ) 
					{
						if( nNonCullNum >= 10000 )
						{
							Error( "CWorld::IntersectObjLine : 범위를 넘어섬." );
							break;
						}
						if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
							continue;
						pNonCullObjs[ nNonCullNum++ ] = pObj;
					}
				}
			}
		}
		
		int ObjSortNearToFar( const VOID* arg1, const VOID* arg2 );
		qsort( pNonCullObjs, nNonCullNum, sizeof(CObj*), ObjSortNearToFar ); 

		CModel	*pModel = NULL;
		D3DXVECTOR3	vIntersect;
		FLOAT fDist;
		
		for( i = nNonCullNum - 1; i >= 0; i-- )
		{
			pObj = (CObj*)pNonCullObjs[ i ];//GetObjInLinkMap( vPickRayCur, i );
			pModel = pObj->m_pModel;
			BOOL bCollObj = TRUE;
			if( pObj->GetType() == OT_CTRL )	bCollObj = FALSE;
			if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect, bCollObj ) == TRUE )
			{
				D3DXVECTOR3	*pTri[3];
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist, bCollObj );
				if( *pTri )
				{
					if( pOut )
					{
						vDir = vEnd - vPos;
						*pOut = vPos + fDist * vDir;
					}
					return TRUE;
				}
			}
		}
	} // bWithObject
#endif // CLIENT

	BOOL bRet = FALSE;
	
	if( bWithTerrain )		// 지형까지 검사함.
	{
		vDir = vEnd - vPos;		// 레이를 얻음.
		D3DXVec3Normalize( &vDir, &vDir );
		FLOAT fDist = IntersectRayTerrain2( vPos, vDir );
		if( fDist )
		{
			*pOut = vPos + (vDir * fDist);
			bRet = TRUE;
		}
	}

	return bRet;
}

//
// vPos를 중심으로 일정한 반경내의 오브젝트를 대상으로 검색
// 카메라 충돌은 이걸로 해야한다.
// CLIENT 전용
BOOL	CWorld::IntersectObjLine2( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans, BOOL bWithTerrain, BOOL bWithObject )
{
	BOOL bRet = FALSE;
#ifdef __CLIENT
	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vDir, vIntersect;
	FLOAT fDist;
	BOOL bAble = TRUE;
	
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic )
	{
		if( pObj->GetType() == OT_OBJ ) 
		{
			bAble = TRUE;
			if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
				bAble = FALSE;
			
			if( bAble )
			{
				// 선분과 오브젝트OBB의 검사.
				pModel = pObj->m_pModel;
				if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
				{
					D3DXVECTOR3	*pTri[3];
					((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist );
					if( *pTri )
					{
						if( pOut )
						{
							vDir = vEnd - vPos;
							*pOut = vPos + fDist * vDir;
						}
						return TRUE;
					}
				}
			}
		}
	}
	END_LINKMAP
	
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic )
	{
		if( pObj->GetType() == OT_CTRL ) 
		{
			bAble = TRUE;
			if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
				bAble = FALSE;
			
			if( bAble )
			{
				// 선분과 오브젝트OBB의 검사.
				pModel = pObj->m_pModel;
				if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect, FALSE  ) == TRUE )
				{
					D3DXVECTOR3	*pTri[3];		// 컨트롤 오브젝트는 충돌메시 안씀.
					((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist, FALSE );
					if( *pTri )
					{
						if( pOut )
						{
							vDir = vEnd - vPos;
							*pOut = vPos + fDist * vDir;
						}
						return TRUE;
					}
				}
			}
		}
	}
	END_LINKMAP
	
	if( bWithTerrain )		// 지형까지 검사함.
	{
		vDir = vEnd - vPos;		// 레이를 얻음.
		D3DXVec3Normalize( &vDir, &vDir );
		FLOAT fDist = IntersectRayTerrain2( vPos, vDir );
		if( fDist )
		{
			*pOut = vPos + (vDir * fDist);
			bRet = TRUE;
		}
	}
#endif	
	return bRet;
}

// 주어진 레이와 충돌하는 바닥을 체크. - 레이저 공격에 쓰임.
FLOAT CWorld::IntersectRayTerrain2( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir )
{
#ifdef __CLIENT	
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;                    
	
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;// * MPU;
	vPickRayAdd.x *= (FLOAT)MPU * 2;
	vPickRayAdd.z *= (FLOAT)MPU * 2;
	vPickRayAdd.y *= (FLOAT)MPU * 2;
	
	FLOAT fFarPlane = m_fFarPlane;
	fFarPlane /= 2;
	do 
	{
		vPickRayCur += vPickRayAdd;
		if( VecInWorld( vPickRayCur ) )
		{
			CLandscape* pLand = GetLandscape( vPickRayCur );
			if( pLand  && pLand->isVisibile() )
			{
				for( int z = -MPU; z <= MPU; z += MPU )
				{
					for( int x = -MPU; x <= MPU; x += MPU )
					{
						int tempx = vPickRayCur.x - x;//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
						int tempy = vPickRayCur.z - z;//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;
						v1=D3DXVECTOR3( ( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy       ), ( tempy       ) );
						v2=D3DXVECTOR3( ( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy       ), ( tempy       ) );
						v3=D3DXVECTOR3( ( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy + MPU ), ( tempy + MPU ) );
						v4=D3DXVECTOR3( ( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy + MPU ), ( tempy + MPU ) );
						if( IntersectTriangle( v1, v2, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
						{
							if( fDist < fNearDist ) { fNearDist = fDist; bTriangle1 = TRUE; }
						}
						else
							if( IntersectTriangle( v2, v4, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
							{
								if( fDist < fNearDist ) {	fNearDist = fDist; bTriangle1 = TRUE; }
							}
					}
				}
			}
		}
		vLength = vPickRayOrig - vPickRayCur;
	} while( D3DXVec3LengthSq( &vLength ) < (fFarPlane * fFarPlane) );
	if( bTriangle1 )
		return fNearDist;
#else // CLIENT
	D3DXVECTOR3 vTri[6];
	FLOAT	x, z;
	int		i, j;
	FLOAT	fDist = 0, fMinDist = 65535.0f;
	BOOL	bRet = FALSE; 

	x = vPickRayOrig.x;		z = vPickRayOrig.z;

	for( i = -10; i < 10; i ++ )
	{
		for( j = -10; j < 10; j ++ )
		{
			if( GetLandTri2( x + (MPU * j), z + (MPU * i), vTri ) )
			{
				bRet = IsTouchRayTri( &vTri[0], &vTri[1], &vTri[2], &vPickRayOrig, &vPickRayDir, &fDist );
				if( bRet && fDist >= 0 )
				{
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
					}
				}
				bRet = IsTouchRayTri( &vTri[3], &vTri[4], &vTri[5], &vPickRayOrig, &vPickRayDir, &fDist );
				if( bRet && fDist >= 0 )
				{
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
					}
				}
			}
		}
	}

	if( fDist )
		return fMinDist;

#endif // not CLIENT

	return 0;
}



