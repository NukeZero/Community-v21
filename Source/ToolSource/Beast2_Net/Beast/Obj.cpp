// Obj.cpp: implementation of the CObj class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CreateObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

//////////////////////////////////////////////////////////////////////
// class static member 
//////////////////////////////////////////////////////////////////////

BOOL	CObj::m_bCollision		= TRUE;
BOOL	CObj::m_bAnimate		= TRUE;
CObj*	CObj::m_pObjActive		= NULL;
CObj*	CObj::m_pObjHighlight	= NULL; 
int		CObj::m_nMethod			= METHOD_NONE;

	CRIT_SEC	CObj::m_csMethod;


//////////////////////////////////////////////////////////////////////
// CObj
//////////////////////////////////////////////////////////////////////

CObj::CObj()
{
	m_dwFlags       = 0;

	m_dwObjAryIdx	= 0xffffffff;
	m_dwType		= OT_OBJ;
	m_pModel		= NULL;
	m_fAngle		= 0.0f;
	m_vPos			= D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vAxis			= D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vScale		= D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_pPrev			= NULL; // 이전 노드 
	m_pNext			= NULL; // 다음 노드 
	m_pWorld		= NULL;
	m_pAIInterface	= NULL;
	
	SetUpdateMatrix( TRUE );
	SetPosChanged( TRUE );

	m_dwIndex		= 0;
	m_dwMotion		= m_dwMotionOld = -1;
	m_dwMotionOption = 0;
	m_dwLinkLevel	= 0;
	m_fAngX			= 0.0f;
#ifdef __Y_ROTXZ
	m_fAngZ			= 0.0f;
#endif //__Y_ROTXZ	
	m_dwAIInterface = 0;
#ifdef __YAIMONSTER_EX
	m_dwAIInterfaceState = 2;
#endif //__YAIMONSTER_EX
	SetDelete( FALSE );
	m_ppViewPtr		= NULL;

	D3DXMatrixScaling( &m_matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fAngle ) );
	D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);


	m_nFadeoutCnt	= 255;
	m_wBlendFactor	= 255;
	m_pCamera		= NULL;
	m_pLight		= NULL;
	m_fDistCamera	= 0.0f;
	m_dy			= 0.0f;
	m_nEvent		= 0;
	SetVisible( TRUE );
    m_cullstate		= CS_UNKNOWN;
	
	m_bSnaped = FALSE;
	

}

CObj::~CObj()
{
	// 애니메이션이 가능한 것만 여기서 파괴가 가능하다.
	// 애니메이션이 불가능한 것은 CModelMng에서 관리하고 파괴한다.
	if( m_pModel && m_pModel->IsAniable() )
		SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pAIInterface );
#ifndef __WORLDSERVER
	SAFE_DELETE( m_pCamera );
	SAFE_DELETE( m_pLight );
#endif
}
// 이 함수를 실행하기 전에 인덱스와 타입이 먼저 세팅되어 있어야한다.
void CObj::InitProp()
{
	ObjProp* pProp = GetProp();
	if( pProp )
	{
#ifndef __ONLINE
		//_tcscpy( m_szName, pProp->szName );
		SetAIInterface( pProp->dwAI );
#endif	// __ONLINE
	}
	else
	{  
		//TRACE( _T( "CMover::InitProp - MoverProp가 없음. \n" ) );
		//ASSERT( 0 );
	}
}

ObjProp* CObj::GetProp() 
{ 
	return prj.GetProp( m_dwType, m_dwIndex ); 
}

void CObj::UpdateLocalMatrix(void)
{
	D3DXMatrixScaling( &m_matScale, m_vScale.x, m_vScale.y, m_vScale.z );
#ifdef __Y_ROTXZ
	D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fAngle ), D3DXToRadian( -m_fAngX ), D3DXToRadian( m_fAngZ ) );
#else //__Y_ROTXZ
	D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fAngle ) );
#endif //__Y_ROTXZ	
	D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
	UpdateMatrix();
}

void CObj::UpdateMatrix( BOOL bUpdateBoundBox )
{
    // Recompute m_mat, m_vecBoundsWorld, and m_planeBoundsWorld 
    // when the thing's position, orientation, or bounding box has changed

	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matScale );
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matRotation );
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matTrans );

	if( bUpdateBoundBox )
	{
		// Transform bounding box coords from local space to world space
		for( int i = 0; i < 8; i++ )
			D3DXVec3TransformCoord( &m_vecBoundsWorld[i], &m_vecBoundsLocal[i], &m_matWorld );

		// Determine planes of the bounding box
		D3DXPlaneFromPoints( &m_planeBoundsWorld[0], &m_vecBoundsWorld[0], 
			&m_vecBoundsWorld[1], &m_vecBoundsWorld[2] ); // Near
		D3DXPlaneFromPoints( &m_planeBoundsWorld[1], &m_vecBoundsWorld[6], 
			&m_vecBoundsWorld[7], &m_vecBoundsWorld[5] ); // Far
		D3DXPlaneFromPoints( &m_planeBoundsWorld[2], &m_vecBoundsWorld[2], 
			&m_vecBoundsWorld[6], &m_vecBoundsWorld[4] ); // Left
		D3DXPlaneFromPoints( &m_planeBoundsWorld[3], &m_vecBoundsWorld[7], 
			&m_vecBoundsWorld[3], &m_vecBoundsWorld[5] ); // Right
		D3DXPlaneFromPoints( &m_planeBoundsWorld[4], &m_vecBoundsWorld[2], 
			&m_vecBoundsWorld[3], &m_vecBoundsWorld[6] ); // Top
		D3DXPlaneFromPoints( &m_planeBoundsWorld[5], &m_vecBoundsWorld[1], 
			&m_vecBoundsWorld[0], &m_vecBoundsWorld[4] ); // Bottom
	}
	SetUpdateMatrix( FALSE );
}

// Parameters 
//   pObj : 추가할 오브젝트 포인터
//  
// Remarks  
//   현재 this 이전에 오브젝트를 추가한다.
//
void CObj::InsPrevNode(CObj* pObj)
{
	pObj->m_pNext = this;
	pObj->m_pPrev = m_pPrev;
	if(m_pPrev)
		m_pPrev->m_pNext = pObj;
	m_pPrev = pObj;
}
//
// Parameters 
//   pObj : 추가할 오브젝트 포인터
//  
// Remarks  
//   현재 this 다음에 오브젝트를 추가한다.
//
void CObj::InsNextNode(CObj* pObj)
{
	if(pObj->m_pNext || pObj->m_pPrev)
	{
		;//TRACE1("CObj::InsNextNode에서 노드가 꼬였음1. Type = %d\n", pObj->GetType1());
	}
	pObj->m_pNext = m_pNext;
	pObj->m_pPrev = this;
	if(m_pNext)
		m_pNext->m_pPrev = pObj;
	m_pNext = pObj;
	if(pObj->m_pNext == pObj)
	{
	;//	TRACE("CObj::InsNextNode에서 노드가 꼬였음2. Type = %d\n", pObj->GetType1());
	}
}
//
// Remarks   
//   오브젝트를 노드 연결에서 삭제한다. 
//
void CObj::DelNode()
{
	if(m_pPrev)
		m_pPrev->m_pNext = m_pNext;
	if(m_pNext)
		m_pNext->m_pPrev = m_pPrev;
	m_pNext = m_pPrev = NULL;
}

void CObj::Process( FLOAT fElapsedTime )
{
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );

	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	if( lpModelElem->m_dwModelType == MODELTYPE_SFX )
	{
		if( ((CSfxModel*)m_pModel)->Process() )
			((CSfxModel*)m_pModel)->m_nCurFrame = 0;
	}
	else
	{
#ifndef __WORLDSERVER
		D3DXVECTOR3 vPos = GetPos();
		m_pModel->FrameMove( &vPos );		// 월드서버에선 CObj::m_pModel의 FrameMove를 돌릴필요 없다.
#endif // not WorldServer
	}

#ifndef __WORLDSERVER
#ifndef __CLIENT
	if( m_pLight )
		m_pLight->Appear( m_pWorld->m_pd3dDevice );
#endif
#endif


	ProcessAI( 0 );
}

void CObj::ProcessAI( long lIdx )
{

}

void CObj::Execute()
{
}

void CObj::SetOnLand()
{  
	//if( m_dwIndex == 23 || m_dwIndex == 24 )
	//	return;

	//if( m_dwIndex >= 80 && m_dwIndex < 90 )
	//	return;

	//if( m_dwType == OT_MOVER && m_dwIndex == 5 )
	//	return;
	if( GetModel()->m_pModelElem->m_bFly == FALSE )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y = m_pWorld->GetLandHeight( GetPos() );
		SetPos( vPos );
	}
}

#ifndef __WORLDSERVER
BOOL CObj::IsShowSmallSize( LPDIRECT3DDEVICE9 pd3dDevice, int nSmallSize )
{
	D3DVIEWPORT9 vp;
    pd3dDevice->GetViewport( &vp );

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	D3DXVECTOR3 vOut[ 8 ];
	for( int i = 0; i < 8; i++ )
		D3DXVec3Project( &vOut[ i ], &pBB->m_vPos[ i ], &vp, &m_pWorld->m_matProj, &m_pWorld->m_pCamera->m_matView, &m_matWorld );	
	CRect rectClient = CRect( 10000, 10000, 0, 0 );
	for( int i = 0; i < 8; i++ )
	{
		D3DXVECTOR3 vPos = vOut[ i ];
		if( vPos.x < rectClient.left )
			rectClient.left = (LONG)( vPos.x );
		if( vPos.x > rectClient.right )
			rectClient.right = (LONG)( vPos.x );
		if( vPos.y < rectClient.top )
			rectClient.top = (LONG)( vPos.y );
		if( vPos.y > rectClient.bottom )
			rectClient.bottom = (LONG)( vPos.y );
	}
	if( rectClient.Width() < nSmallSize && rectClient.Height() < nSmallSize )
	{
		m_nFadeoutCnt -= 20;
		if( m_nFadeoutCnt < 0 )
		{
			m_nFadeoutCnt = 0;
			return FALSE;
		}
	}
	else
	{
		m_nFadeoutCnt += 20;
		if( m_nFadeoutCnt >= 255 )
			 m_nFadeoutCnt = 255;
	}
	if( m_nFadeoutCnt == 0 )
		return FALSE;
	return TRUE;
}
#endif	// __WORLDSERVER

void CObj::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifndef __WORLDSERVER
#ifdef _DEBUG	
	if( GetType() == OT_OBJ && m_dwIndex == 298 )
	{
		int a = 0;
	}
#endif

	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
	{
		return;
	}

	if( IsUpdateMatrix() )
		UpdateMatrix();

	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );

	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	if( lpModelElem->m_dwModelType == MODELTYPE_SFX )
	{
		((CSfxModel*)m_pModel)->m_vPos = GetPos();
		((CSfxModel*)m_pModel)->m_vPos.y += 0.2f;
		((CSfxModel*)m_pModel)->m_vRotate.y = GetAngle();
		((CSfxModel*)m_pModel)->m_vScale = GetScale();
		((CSfxModel*)m_pModel)->m_matScale = m_matScale;
		((CSfxModel*)m_pModel)->Render( pd3dDevice, NULL );
		return;
	}
	// 지원하지 않음 
	//if( lpModelElem->m_pTexture )
	//	((CModelObject *)m_pModel)->GetParts(0)->m_pObject3D->SetTexture( lpModelElem->m_pTexture );	// 이런식으로 쓰는건 pModel의 오브젝트가 하나라는 가정하에 쓰는 것이다.

	CModel* pModel = m_pModel;
	DWORD dwModelType = pModel->GetModelType();

	//gmpbigsun
	pModel->m_bRenderCollsion = IsInvisibleCollision( );
	
	
	// 사이즈가 스크린 좌표에서 20 픽셀보다 작으면 출력하지 않는다.
	if( GetType() == OT_ITEM )
	{
	//	if( IsShowSmallSize( pd3dDevice, 60 ) == FALSE )
	//		return;
	}
	if( GetActiveObj() != NULL )
	{
		//m_pWorld->m_pCamera->m_vPos
		//vPosObj = GetPos() - m_pWorld->m_pCamera->m_vPos;
		//vPosAct = GetActiveObj()->GetPos() - m_pWorld->m_pCamera->m_vPos;
		//nLengthObj = D3DXVec3Length( &vPosObj );
		//nLengthAct = D3DXVec3Length( &vPosAct );
		//if( nLength > CWorld::m_fFarPlane ) 
		//	nLength = CWorld::m_fFarPlane;
		// 미니맵을 랜더링 하는 것이라면 반투명 효과나 거리 체크를 무시한다.
		if( m_pWorld->m_bMiniMapRender == FALSE )
		{
			//if( m_nFadeoutCnt != 255 )
			//	return;
				//pModel->SetBlendFactor( m_nFadeoutCnt );//255 - nBlendFactor );
			//else
			/*
			if( nLength > 250 )
			{
			//	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
				//	return;
				nLength -= 200;
				if( nLength > 255 ) nLength = 255;
				// far : length = 255 : 
				int nBlendFactor = nLength * 255 / 255;//CWorld::m_fFarPlane;
				pModel->SetBlendFactor( 255 - nBlendFactor );
				if( nBlendFactor == 255 )
					return;
				return;
			}
			else
				pModel->SetBlendFactor( 255 );
				*/
		}
	}
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	if( m_wBlendFactor < 255 )
	{
		int a = 0;
	}
	int wBlendFactor = m_wBlendFactor;
	if( m_pModel->m_nNoEffect )
		wBlendFactor = 255;

	m_pModel->SetBlendFactor( wBlendFactor );
	
	if( pModel->m_nNoEffect == 0 )
	{
		if( m_pWorld->m_bIsIndoor == 1 )
			pModel->SetGroup( 0 );
		else
		{
			int fDist = (int)( 50.0f );
			switch( g_Option.m_nObjectDetail )
			{
			case 0 :	fDist = (int)( 50.0f );	break;
			case 1 :	fDist = (int)( 30.0f );	break;
			case 2 :	fDist = (int)( 15.0f );	break;
			}
			int nLevel = (int)( m_fDistCamera / fDist );
			if( nLevel >= 2 )	nLevel = 2;
			if( nLevel < 0 )	
			{
				Message( "CObj::Render : %d, lod lv=%d %f", m_dwIndex, nLevel, m_fDistCamera );
				nLevel = 0;
			}
			
			pModel->SetGroup( nLevel );
		}
	} else
		pModel->SetGroup( 2 );		// 그림자 찍을땐 가장 낮은 단계로 찍는다.
	pModel->Render( pd3dDevice, &m_matWorld ); 
#endif
}


CModel* CObj::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex ) 
{
	return prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex );
}

void CObj::ResetScale()
{
	m_vScale.x = m_pModel->m_pModelElem->m_fScale;
	m_vScale.y = m_pModel->m_pModelElem->m_fScale;
	m_vScale.z = m_pModel->m_pModelElem->m_fScale;
}

BOOL CObj::SetTypeIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex, BOOL bInitProp )
{
	m_dwIndex = dwIndex;
//	if( pd3dDevice )
	{ 
		m_pModel = LoadModel( pd3dDevice, dwType, dwIndex );
		if(m_pModel) 
		{ 
			if( bInitProp )
			{
				m_vScale.x = m_pModel->m_pModelElem->m_fScale;
				m_vScale.y = m_pModel->m_pModelElem->m_fScale;
				m_vScale.z = m_pModel->m_pModelElem->m_fScale;
			}
			UpdateBoundBox();
			if( bInitProp )
				InitProp();
			return TRUE; 
		} else
		{
			LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( dwType, dwIndex );
			if( lpModelElem == NULL )
				Error( "CObj::SetTypeIndex : LoadModel실패 %d %d", dwType, dwIndex );
			if( lpModelElem->m_dwModelType != MODELTYPE_SFX )	// sfx는 안읽으므로 sfx가 아닌것만 에러내자 .
				Error( "CObj::SetTypeIndex : LoadModel실패 %d %d", dwType, dwIndex );
		}
	}
	return FALSE;
}

BOOL CObj::UpdateBoundBox()
{
	if( m_pModel == NULL ) 
		return FALSE;

	if( m_pModel )
	{
		// OBB의 중점과 축을 재계산한다.
		m_OBB.UpdateMartix( m_pModel->m_vMin, m_pModel->m_vMax, m_matScale, m_matRotation, m_matWorld );
	}

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	m_pModel->m_pObj = this;
	if( pBB )
	{
		//     - z
		//   3 | 2
		// - --+-- + x
		//   0 | 1
		//
		//   7 | 6
		// - --+-- + x
		//   4 | 5
		m_vecBoundsLocal[0] = D3DXVECTOR3( m_pModel->m_vMax.x, m_pModel->m_vMax.y, m_pModel->m_vMax.z );
		m_vecBoundsLocal[1] = D3DXVECTOR3( m_pModel->m_vMax.x, m_pModel->m_vMax.y, m_pModel->m_vMin.z );
		m_vecBoundsLocal[2] = D3DXVECTOR3( m_pModel->m_vMax.x, m_pModel->m_vMin.y, m_pModel->m_vMax.z );
		m_vecBoundsLocal[3] = D3DXVECTOR3( m_pModel->m_vMax.x, m_pModel->m_vMin.y, m_pModel->m_vMin.z );
		m_vecBoundsLocal[4] = D3DXVECTOR3( m_pModel->m_vMin.x, m_pModel->m_vMax.y, m_pModel->m_vMax.z );
		m_vecBoundsLocal[5] = D3DXVECTOR3( m_pModel->m_vMin.x, m_pModel->m_vMax.y, m_pModel->m_vMin.z );
		m_vecBoundsLocal[6] = D3DXVECTOR3( m_pModel->m_vMax.x, m_pModel->m_vMin.y, m_pModel->m_vMin.z );
		m_vecBoundsLocal[7] = D3DXVECTOR3( m_pModel->m_vMin.x, m_pModel->m_vMin.y, m_pModel->m_vMax.z );

		UpdateLocalMatrix();
	}

#if !defined(__WORLDSERVER)
	CalcLinkLevel();
#endif	// !defined(__WORLDSERVER)
	return TRUE;
}

//gmpbigsun
const enum FD_OBJ_ATTR	 // data의 저장 읽기에서 관여하는 속성들
{
	FOA_Invisible_Collision = 0x00010000,	//실제게임에서 그리지 않는 충돌전용 오브젝트 
	FOA_Invisible = 0x00020000,				//그리지도 않고 충돌도 않는 오브젝트 
};

BOOL CObj::Read( CFileIO* pFile )
{
	pFile->Read( &m_fAngle  , sizeof( m_fAngle   ) );
	pFile->Read( &m_vAxis   , sizeof( m_vAxis    ) );
	pFile->Read( &m_vPos    , sizeof( m_vPos     ) );
	pFile->Read( &m_vScale  , sizeof( m_vScale   ) );
	pFile->Read( &m_dwType   , sizeof( m_dwType    ) ); 
	pFile->Read( &m_dwIndex  , sizeof( m_dwIndex   ) );

#ifdef __BS_INVISIBLE_COLLISION 
	//!! warning : m_dwType 는 하위 2바이트만 이용해야 합니다 
	// 맵툴에서 임의로 데이터를 추가하면 문제가 발생할수 있으므로 (전버젼) 상위 2이트를 이용합니다.	( 15차부터 )
	if( m_dwType & FOA_Invisible_Collision )
	{	
		SetInvisibleCollision( TRUE );
		m_dwType &= ~FOA_Invisible_Collision;
	}

	if( m_dwType & FOA_Invisible )
	{
		SetInvisibleNoCollision( TRUE );
		m_dwType &= ~FOA_Invisible;
	}
#endif

#ifdef __Y_ROTXZ
	m_fAngX = m_vAxis.x;
	m_fAngZ = m_vAxis.z;
//	m_fAngle = m_vAxis.y;
#endif //__Y_ROTXZ	

	// 기본 정보(type, index)만 읽고 InitProp을 한다. 저걸 읽지 않으면 Property를 얻을 수 없어 초기화 안됨.
	InitProp();

	pFile->Read( &m_dwMotion, sizeof( m_dwMotion ) );

	DWORD dwAIIntreface;
	pFile->Read( &dwAIIntreface, sizeof( dwAIIntreface ) );
	DWORD dwReserved = 0;

#ifdef __YAIMONSTER_EX
	pFile->Read( &m_dwAIInterfaceState, sizeof( m_dwAIInterfaceState ) );

	if( m_dwAIInterfaceState == 0 )
		m_dwAIInterfaceState = 2;

	SetAIInterface( dwAIIntreface, m_dwAIInterfaceState );	
#else //__YAIMONSTER_EX
	SetAIInterface( dwAIIntreface );	
	pFile->Read( &dwReserved, sizeof( dwReserved ) );	
#endif //__YAIMONSTER_EX


//	pFile->Read( &m_dwAIInterfaceState, sizeof( m_dwAIInterfaceState ) );
	
	// 비율 적용을 위해서 읽을 때 복원한다. 이는 MPU를 바꿔을 때 오브젝트들이
	// 바뀐 맵이 좌표 비율이 재조정되게 하기 위한 것이다. 오즉 디스크 저장시에만 필요하다.

	m_vPos.x	*= FLOAT( OLD_MPU );
	m_vPos.z	*= FLOAT( OLD_MPU );

	return TRUE;
}
BOOL CObj::Write( CFileIO* pFile )
{
	D3DXVECTOR3 vPos	= m_vPos;
	//gmpbigsun: 비율 적용을 위해서 저장시에는 unit이 1미터가 되도록 저장하고 읽을 때 복원한다.
	vPos.x /= FLOAT( OLD_MPU );
	vPos.z /= FLOAT( OLD_MPU );

	pFile->Write( &m_fAngle  , sizeof( m_fAngle   ) );
#ifdef __Y_ROTXZ
	m_vAxis.x = m_fAngX;
	m_vAxis.z = m_fAngZ;
//	m_vAxis.y = m_fAngle;
#endif //__Y_ROTXZ
	pFile->Write( &m_vAxis   , sizeof( m_vAxis    ) );
	pFile->Write( &vPos    , sizeof( m_vPos     ) );
	pFile->Write( &m_vScale  , sizeof( m_vScale   ) );
//	if( m_dwType >= OT_CTRL )
//		m_dwType++;
	
	//gmpbigsun[090813] : 현재 m_dwType의 MAX값은 MAX_OBJTYPE(9) 이다. 이값은 절대 65535를 넘을일이 없다고 보고 상위 2바이트를 이용.
	DWORD saveType = m_dwType;		

#ifdef __BS_INVISIBLE_COLLISION	
	//!! warning : m_dwType 는 하위 2바이트만 이용해야 합니다 
	// 맵툴에서 임의로 데이터를 추가하면 문제가 발생할수 있으므로 (전버젼) 상위 2바이트를 이용합니다.
	
	if( IsInvisibleCollision( ) ) 
		saveType |= FOA_Invisible_Collision;
#endif

	pFile->Write( &saveType   , sizeof( saveType    ) );
//	if( m_dwType >= OT_CTRL )
//		m_dwType--;

//581 MODELTYPE_SFX "" 0 0 0 1.000000 0 0 0 
  //    "fire01" 579 
	
	/*
if( m_dwIndex == 781 || m_dwIndex == 779 && m_dwType == 0 )
	{
		m_dwIndex -= 200;
		pFile->Write( &m_dwIndex  , sizeof( m_dwIndex   ) );
		m_dwIndex += 200;
	}
	else
	*/

	pFile->Write( &m_dwIndex  , sizeof( m_dwIndex   ) );
	pFile->Write( &m_dwMotion, sizeof( m_dwMotion ) );
	pFile->Write( &m_dwAIInterface, sizeof( m_dwAIInterface ) );
	DWORD dwReserved = 0;

#ifdef __YAIMONSTER_EX
	pFile->Write( &m_dwAIInterfaceState, sizeof( m_dwAIInterfaceState ) );	
#else //__YAIMONSTER_EX
	pFile->Write( &dwReserved, sizeof( dwReserved ) );	
#endif //__YAIMONSTER_EX
	
	return TRUE;
}


void CObj::SetPos( const D3DXVECTOR3& vPos )      
{ 
	if( m_vPos == vPos )
		return;

	SetPosChanged( TRUE );		// 좌표가 변경되었음을 세팅 

	CWorld* pWorld = GetWorld();

	if( pWorld )
	{
		if( pWorld->VecInWorld( vPos ) == FALSE )
			return;

#	ifdef __BEAST
		// 좌표가 이동되었다. CLandscape::SetModified 호출해서 맵이 수정되었음을 기록한다.
		pWorld->GetLandscape( m_vPos )->SetModified( TRUE ); // 오브젝트가 이동되면 현재 랜드에서 사라질 수 있으므로 일단 수정 기록
		pWorld->GetLandscape( vPos )->SetModified( TRUE ); // 새 랜드에 있을 수 있으므로 수정 기록 
#	endif
		D3DXVECTOR3	vOld	= GetPos() / (float)( MPU );
		D3DXVECTOR3 vCur	= vPos / (float)( MPU );
		if( (int)vOld.x != (int)vCur.x || (int)vOld.z != (int)vCur.z )
		{
			int nOldX = int( vOld.x / MAP_SIZE );
			int nOldZ = int( vOld.z / MAP_SIZE );
			int nCurX = int( vCur.x / MAP_SIZE );
			int nCurZ = int( vCur.z / MAP_SIZE );   
			if( nOldX != nCurX || nOldZ != nCurZ )
			{
				CLandscape* pOldLandscape	= pWorld->m_apLand[ nOldX + nOldZ * pWorld->m_nLandWidth ];
				if( NULL != pOldLandscape )
				{
					pOldLandscape->RemoveObjLink( this );
					pOldLandscape->RemoveObjArray( this );
				}
				m_vPos = vPos;
				CLandscape* pCurLandscape	= pWorld->m_apLand[ nCurX + nCurZ * pWorld->m_nLandWidth ];
				if( NULL != pCurLandscape )
				{
					pCurLandscape->InsertObjLink( this );
					pCurLandscape->AddObjArray( this );
				}
			}
			else
			{
				// 같은 Landscape 안에서의 이동 
				// kentauji edited
				int nWidth;

				if( m_pModel )
				{
					nWidth	= (int)( m_pModel->GetMaxWidth() );
				}
				else
				{
					nWidth=0;
				}

				// kentauji edited
				DWORD dwLinkType	= GetLinkType();
				if( pWorld->GetObjInLinkMap( m_vPos, dwLinkType, m_dwLinkLevel ) == this )
					pWorld->SetObjInLinkMap( m_vPos, dwLinkType, m_dwLinkLevel, m_pNext );
				DelNode();
				CObj* pObj	= pWorld->GetObjInLinkMap( vPos, dwLinkType, m_dwLinkLevel );
				if( pObj )
					pObj->InsNextNode( this );
				else
					pWorld->SetObjInLinkMap( vPos, dwLinkType, m_dwLinkLevel, this );
				m_vPos	= vPos; 
			}
		}
		else 
		{
			m_vPos	= vPos;
		}
	}
	else 
	{
		m_vPos	= vPos;
	}

#ifndef __ONLINE
	D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	SetUpdateMatrix( TRUE );

#else	// __ONLINE
	if( GetType() != OT_MOVER )
	{
		D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
		SetUpdateMatrix( TRUE );
	}
#endif	// __ONLINE

	return;
}

void CObj::SetAngle( FLOAT fAngle )
{ 
	if( fAngle > 360.0f ) 
		fAngle = fmod( fAngle, 360.0f );
	else if( fAngle < 0.0f )
	{
		fAngle = fmod( fAngle, 360.0f );
		fAngle += 360.0f;
	}

	m_fAngle	= fAngle;
//	D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -fAngle ) );
//	D3DXMATRIX	matRotX;
//	D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fAngX ) );
//	m_matRotation	*= matRotX;

	FLOAT fAngZ = 0.0f;
	
#ifdef __Y_ROTXZ
	fAngZ = -m_fAngZ;
#endif //__Y_ROTXZ
	
	
	if( GetType() == OT_MOVER )
	{
		CMover *pMover = (CMover *)this;
		if( (pMover->IsNPC() && pMover->IsFlyingNPC()) || (pMover->IsPlayer() && pMover->IsFly()) )
			D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fAngle ), D3DXToRadian( -m_fAngX ), D3DXToRadian( fAngZ ) );
		else
			D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fAngle ) );
	}
	else
	{
#ifdef __Y_ROTXZ
		D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fAngle ), D3DXToRadian( -m_fAngX ), D3DXToRadian( fAngZ ) );
#else //__Y_ROTXZ
		D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fAngle ) );
#endif //__Y_ROTXZ
	}
	SetUpdateMatrix( TRUE );
}


void CObj::AddAngleX( FLOAT fAngle )
{
	FLOAT fAng	= m_fAngX + fAngle;
	m_fAngX		= fAng;
	SetAngle( m_fAngle );
}

void CObj::SetAngleX( FLOAT fAngX )
{ 
	SetAngle( m_fAngle );

	m_fAngX		= fAngX;

	FLOAT fAngZ = 0.0f;
	
#ifdef __Y_ROTXZ
	fAngZ = -m_fAngZ;
#endif //__Y_ROTXZ
	
	D3DXMATRIX matRotX;
	D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fAngle ), D3DXToRadian( -m_fAngX ), D3DXToRadian( fAngZ ) );
//	D3DXMatrixRotationX( &matRotX, D3DXToRadian( fAngX ) );
//	m_matRotation	*= matRotX;
	SetUpdateMatrix( TRUE );
//	m_bUpdateRotation = TRUE;
}

#ifdef __Y_ROTXZ
void CObj::SetAngleZ( FLOAT fAngZ )
{ 
	SetAngle( m_fAngle );
	
	m_fAngZ		= fAngZ;
	D3DXMATRIX matRotX;
	D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fAngle ), D3DXToRadian( -m_fAngX ), D3DXToRadian( fAngZ ) );
	//	D3DXMatrixRotationX( &matRotX, D3DXToRadian( fAngX ) );
	//	m_matRotation	*= matRotX;
	SetUpdateMatrix( TRUE );
	//	m_bUpdateRotation = TRUE;
}
#endif //__Y_ROTXZ

void CObj::SetScale( D3DXVECTOR3 vScale )      
{ 
	m_vScale = vScale;
	D3DXMatrixScaling( &m_matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	SetUpdateMatrix( TRUE );
//	m_bUpdateScale = TRUE;
}
void CObj::MessageFromModel( DWORD dwMessage )
{
	switch( dwMessage )
	{
	case MODELMSG_ENDFRAME:
		Delete();
		break;
	}
}
void CObj::SetAIInterface( DWORD dwAIInterface, DWORD dwState )
{
#ifdef __YAIMONSTER_EX
	if( m_dwAIInterface == dwAIInterface && m_dwAIInterfaceState == dwState )
		return;	
#else //__YAIMONSTER_EX
	if( m_dwAIInterface == dwAIInterface )
		return;
#endif //__YAIMONSTER_EX

	SAFE_DELETE( m_pAIInterface );
	m_pAIInterface	= CreateAIInterface( this, dwAIInterface );
	m_dwAIInterface		= dwAIInterface;
#ifdef __YAIMONSTER_EX
	m_dwAIInterfaceState = dwState;
#endif //__YAIMONSTER_EX
}

BOOL CObj::TurnAngle( FLOAT fDestAngle, FLOAT fAddAngle )
{
	FLOAT fAngle = fDestAngle - m_fAngle;

	if( fAngle < 0.0f )
	{
		if( fAngle > -180.0f ) // 시계 반대 방향
		{
			if( -fAddAngle <= fAngle ) // 같거나 초과 
			{
				SetAngle( fDestAngle );
				return TRUE;
			}
			else
				AddAngle( -fAddAngle );
		}
		else // 시계 방향 
		{
			if( fAddAngle - 360.0f >= fAngle ) // 같거나 초과 
			{
				SetAngle( fDestAngle );
				return TRUE;
			}
			else
				AddAngle( fAddAngle );
		}
	}
	else
	{
		if( fAngle < 180.0f ) // 시계 방향
		{
			if( fAddAngle >= fAngle ) // 같거나 초과 
			{
				SetAngle( fDestAngle );
				return TRUE;
			}
			else
				AddAngle( fAddAngle );
		}
		else // 시계 반대 
		{
			if( 360.0f - fAddAngle <= fAngle ) // 같거나 초과 
			{
				SetAngle( fDestAngle );
				return TRUE;
			}
			else
				AddAngle( -fAddAngle );
		}
	}
	return FALSE;
}	
void CObj::AddAngle( FLOAT fAngle )
{
	FLOAT	fAng = m_fAngle + fAngle;
	if( fAng > 360.0f ) 
		fAng -= 360.0f;
	else if( fAng < 0.0f ) 
		fAng += 360.0f;
	SetAngle( fAng );
}
int CObj::HowDirAngle( FLOAT fSrcAngle,  FLOAT fDestAngle )
{
	FLOAT fAngle = fDestAngle - fSrcAngle;

	if( fAngle < 0.0f )
	{
		if( fAngle > -180.0f ) // 시계 반대 방향
			return 1;
		else // 시계 방향 
			return 0;
	}
	else
	{
		if( fAngle < 180.0f ) // 시계 방향
			return 0;
		else // 시계 반대 
			return 1;
	}
	// 여기까지 올일 없음.
	return 0;
}	
/*
ObjProp* CObj::GetProp()
{
	switch( m_dwType )
	{
	case OT_OBJ  : return prj.GetObjProp( m_dwIndex ); 
	case OT_CTRL : return prj.GetObjProp( m_dwIndex ); 
	case OT_SFX  : return prj.GetObjProp( m_dwIndex ); 
	case OT_ITEM : return prj.GetItemProp( m_dwIndex ); 
	case OT_MOVER: return prj.GetMoverProp( m_dwIndex ); 
	}
	return NULL;
}
*/
// 0 = CW, 1= CCW

void CObj::Delete()
{
	m_pWorld->DeleteObj( this );
}
BOOL CObj::Pick( D3DXVECTOR3* pvPickRayOrig, D3DXVECTOR3* pvPickRayDir, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bOnlyBoundBox, BOOL bColl )
{
//	D3DXMATRIX mWorld = GetMatrixWorld();
//	D3DXVECTOR3 vPos = GetPos();
//	D3DXVECTOR3 vEnd = *pvPickRayOrig + (*pvPickRayDir * 65535.0f);		// 아주 긴 라인을 만든다.
//	D3DXVECTOR3 vIntersect;
	
//	BOOL bRet = m_pModel->IsTouchOBB_Line( *pvPickRayOrig, vEnd, GetMatrixWorld(), pvIntersect, FALSE );
//	if( bRet )
	if( m_pModel->IntersectBB( *pvPickRayOrig, *pvPickRayDir, GetMatrixWorld(), pvIntersect, pfDist ) == TRUE )	
	{
		if( GetType() == OT_REGION )
			return TRUE;

//		vEnd = *pvIntersect - *pvPickRayOrig;
//		*pfDist = D3DXVec3Length( &vEnd );		// 거리를 구함.
		if( bOnlyBoundBox == TRUE )
			return TRUE;
		if( m_pModel->Intersect( *pvPickRayOrig, *pvPickRayDir, GetMatrixWorld(), pvIntersect, pfDist, bColl ) == TRUE )
			return TRUE;
	}
	return FALSE;
}
/*
// 오브젝트의 중심 좌표를 얻는다. center나 원점을 말하는 것이 아니라
// 오브젝트의 실제적인 중심좌표. 이것은 공격 목표의 히트 지점으로 사용된다.
D3DXVECTOR3 CObj::GetBodyPoint()
{
	// 바운드 박스를 오브젝트의 좌표와 스케일, 회전에 의해 트랜스폼한다.
	D3DXMATRIX mWorld1;

	// 회전은 곱하지 않는다. 회전시키면 월드 좌표에서 발지름을 구하는데 엄청난 오차가 생김.
	D3DXMatrixIdentity( &mWorld1 );
	D3DXMatrixMultiply( &mWorld1, &mWorld1, &m_matScale );
	D3DXMatrixMultiply( &mWorld1, &mWorld1, &m_matTrans );

	D3DXVECTOR4 vMin1; D3DXVec3Transform( &vMin1, &m_pModel->m_vMin, &mWorld1 );	
	D3DXVECTOR4 vMax1; D3DXVec3Transform( &vMax1, &m_pModel->m_vMax, &mWorld1 );	
	D3DXVECTOR3 vMin1_ = vMin1;
	D3DXVECTOR3 vMax1_ = vMax1;

	// 중심 좌표를 바운드 박스의 중앙으로 옮긴다.
	D3DXVECTOR3 vPos1;
	D3DXVECTOR3 vPos2 ;
	vPos1.x = vMin1_.x + ( ( vMax1_.x - vMin1_.x ) / 2 );
	vPos1.y = vMin1_.y + ( ( vMax1_.y - vMin1_.y ) / 2 );
	vPos1.z = vMin1_.z + ( ( vMax1_.z - vMin1_.z ) / 2 );

	return vPos1;
}
*/
BOOL CObj::IsTouchObj( D3DXVECTOR3* pvPos  )
{
	/*
	D3DXMATRIX mWorld1 = GetMatrixWorld();
	D3DXMATRIX mWorld2 = pObj->GetMatrixWorld();
	D3DXVECTOR4 vMin1;
	D3DXVECTOR4 vMax1;
	D3DXVECTOR4 vMin2;
	D3DXVECTOR4 vMax2;
	D3DXVec3Transform( &vMin1, &m_pModel->m_vMin, &mWorld1 );	
	D3DXVec3Transform( &vMax1, &m_pModel->m_vMax, &mWorld1 );	
	D3DXVec3Transform( &vMin2, &pObj->m_pModel->m_vMin, &mWorld2 );	
	D3DXVec3Transform( &vMax2, &pObj->m_pModel->m_vMax, &mWorld2 );	
	D3DXVECTOR3 vMin1_ = vMin1;
	D3DXVECTOR3 vMax1_ = vMax1;
	D3DXVECTOR3 vMin2_ = vMin2;
	D3DXVECTOR3 vMax2_ = vMax2;
	return IsTouchAABB( vMin1_, vMax1_, vMin2_, vMax2_ );
	*/
/*	
	D3DXVECTOR3 vDistance1 = GetPos() - *pvPos;
	D3DXVECTOR3 vDistance2 = m_pModel->m_vMax - m_pModel->m_vMin;

	FLOAT fLength1 = D3DXVec3Length( &vDistance1 );
	FLOAT fLength2 = ( vDistance2.x + vDistance2.y + vDistance2.z ) / 3;

	if( fLength1 < fLength2 ) 
		return TRUE;
*/
	D3DXVECTOR3 vDist = m_pModel->m_vMax - m_pModel->m_vMin;	//
	D3DXVECTOR3 vDist2 = GetPos() - *pvPos;
	float fDist = D3DXVec3Length( &vDist2 );			// 두좌표간의 거리
	float fR2 = D3DXVec3Length( &vDist ) / 2.0f;		// this의 반지름
	float fR1 = 0.5f;		// pvPos의 반지름

	if( fR1 + fR2 > fDist )		// 반지름의 합이 두지점의 거리보다 크면 충돌
		return TRUE;

	return FALSE;
}
BOOL CObj::IsTouchObj( CObj* pObj )
{
	D3DXVECTOR3 vMin1, vMax1;	// this
	D3DXVECTOR3 vMin2, vMax2;	// pObj

	vMin1 = m_vPos + m_pModel->m_vMin;	
	vMax1 = m_vPos + m_pModel->m_vMax;
	vMin2 = pObj->m_vPos + pObj->m_pModel->m_vMin;	
	vMax2 = pObj->m_vPos + pObj->m_pModel->m_vMax;

	//축에 정렬시켜야 하는데? 

	return IsTouchAABB( vMin1, vMax1, vMin2, vMax2 );
/*	
	
	// 바운드 박스를 오브젝트의 좌표와 스케일, 회전에 의해 트랜스폼한다.
	D3DXMATRIX mWorld1;
	D3DXMATRIX mWorld2;

	// 회전은 곱하지 않는다. 회전시키면 월드 좌표에서 발지름을 구하는데 엄청난 오차가 생김.
	D3DXMatrixIdentity( &mWorld1 );
	D3DXMatrixMultiply( &mWorld1, &mWorld1, &m_matScale );
	D3DXMatrixMultiply( &mWorld1, &mWorld1, &m_matTrans );
	D3DXMatrixIdentity( &mWorld2 );
	D3DXMatrixMultiply( &mWorld2, &mWorld2, &pObj->m_matScale );
	D3DXMatrixMultiply( &mWorld2, &mWorld2, &pObj->m_matTrans );

	D3DXVECTOR4 vMin1; D3DXVec3Transform( &vMin1, &m_pModel->m_vMin, &mWorld1 );	
	D3DXVECTOR4 vMax1; D3DXVec3Transform( &vMax1, &m_pModel->m_vMax, &mWorld1 );	
	D3DXVECTOR4 vMin2; D3DXVec3Transform( &vMin2, &pObj->m_pModel->m_vMin, &mWorld2 );	
	D3DXVECTOR4 vMax2; D3DXVec3Transform( &vMax2, &pObj->m_pModel->m_vMax, &mWorld2 );	
	D3DXVECTOR3 vMin1_ = vMin1;
	D3DXVECTOR3 vMax1_ = vMax1;
	D3DXVECTOR3 vMin2_ = vMin2;
	D3DXVECTOR3 vMax2_ = vMax2;

	// 중심 좌표를 바운드 박스의 중앙으로 옮긴다.
	D3DXVECTOR3 vPos1;
	D3DXVECTOR3 vPos2 ;
	vPos1.x = vMin1_.x + ( ( vMax1_.x - vMin1_.x ) / 2 );
	vPos1.y = vMin1_.y;// + ( ( vMax1_.y - vMin1_.y ) / 2 );
	vPos1.z = vMin1_.z + ( ( vMax1_.z - vMin1_.z ) / 2 );

	vPos2.x = vMin2_.x + ( ( vMax2_.x - vMin2_.x ) / 2 );
	vPos2.y = vMin2_.y;// + ( ( vMax2_.y - vMin2_.y ) / 2 );
	vPos2.z = vMin2_.z + ( ( vMax2_.z - vMin2_.z ) / 2 );

	// 거리와 반지름을 구해 충돌 체크를 한다.
	D3DXVECTOR3 vDistance1 = vPos1 - vPos2;
	D3DXVECTOR3 vDistance2 = vMax1_ - vMin1_;

	FLOAT fLength1 = D3DXVec3Length( &vDistance1 );
	FLOAT fLength2 = ( ( vDistance2.x + vDistance2.y + vDistance2.z ) / 3 ) * 1.5f; // 평균 지름을 구하고 / 2를 해서 반지름 구하기 

	if( fLength1 < fLength2 ) 
		return TRUE;
	return FALSE;
*/
}

//
// this의 반경 + m_nArrivalRange 내에 pObj의 중심점이 들어와있는가?
//
BOOL	CObj::IsRangeObj( CObj *pSrc, float fRange )
{
	if( m_pModel == NULL )	
		return FALSE;

	if( fRange == 0.0f )
		fRange = 1.0f;		// 일단 임시로...

	// this모델의 가로세로크기의 평균을 반지름으로 택함. (xDist + yDist + zDist) / 3 / 2;
	float	fLenX = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	float	fLenY = fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y);
	float	fLenZ = fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);
	float	fRadius = ((fLenX + fLenY + fLenZ) / 3.0f) / 2.0f;			// this(Dest)모델의 평균(!) 반지름.
	fRadius *= 0.8f;	// 삼면 평균으로만 하니까 넘 커서 조금 줄였다.
	fRadius *= m_vScale.x;		// 스케일 조정.

	fLenX = fabs(pSrc->m_pModel->m_vMax.x - pSrc->m_pModel->m_vMin.x);
	fLenY = fabs(pSrc->m_pModel->m_vMax.y - pSrc->m_pModel->m_vMin.y);
	fLenZ = fabs(pSrc->m_pModel->m_vMax.z - pSrc->m_pModel->m_vMin.z);
	float	fRadiusSrc = ((fLenX + fLenY + fLenZ) / 3.0f) / 2.0f;		// pSrc모델의 평균(!) 반지름.
	fRadiusSrc *= 0.8f;
	fRadiusSrc *= pSrc->m_vScale.x;
	fRadiusSrc += fRange;		// pSrc측반지름에다 m_nArrivalRange를 더함.
	fRadius += fRadiusSrc;

	D3DXVECTOR3 vDist = GetPos() - pSrc->GetPos();	// 두좌표간 벡터
	float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
	
	if( fDistSq < (fRadius * fRadius) )		// 두점간의 거리가 범위보다 작으면 충돌
		return TRUE;
	
	return FALSE;
	
}

//
// this(src) + m_nArrivalRange 의 반경 내에 vDest의 중심점이 들어와있는가?
//
BOOL	CObj::IsRangeObj( const D3DXVECTOR3 &vDest, float fRange )
{
	if( m_pModel == NULL )	return FALSE;
//	if( fRange == 0.0f )
//		fRange = 1.0f;		// 일단 임시로...

	float fLenX, fLenY, fLenZ;
	float	fRadius = 1.0f;
	fRadius *= 0.8f;	// 삼면 평균으로만 하니까 넘 커서 조금 줄였다.

	fLenX = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	fLenY = fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y);
	fLenZ = fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);
	float	fRadiusSrc = ((fLenX + fLenY + fLenZ) / 3.0f) / 2.0f;		// pSrc모델의 평균(!) 반지름.
	fRadiusSrc *= 0.8f;
	fRadiusSrc *= m_vScale.x;
	
	fRadiusSrc += fRange;		// pSrc측반지름에다 m_nArrivalRange를 더함.
	fRadius += fRadiusSrc;
	
	
	D3DXVECTOR3 vDist = vDest - GetPos();			// 두좌표간 벡터
	float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
	
	if( fDistSq < fRadius * fRadius )		// 두점간의 거리가 범위보다 작으면 충돌
		return TRUE;
	
	return FALSE;
	
}


BOOL	CObj::IsRangeObj_Plane( CObj *pObj, float fRange )
{
	if( fRange == 0.0f )
		fRange = 1.0f;		// 일단 임시로...
	// this모델의 가로세로크기의 평균을 반지름으로 택함. (xDist + yDist + zDist) / 3 / 2;
	float	fLenX = m_pModel->m_vMax.x - m_pModel->m_vMin.x;
	float	fLenY = m_pModel->m_vMax.y - m_pModel->m_vMin.y;
	float	fLenZ = m_pModel->m_vMax.z - m_pModel->m_vMin.z;

	float	fRadius = (fabs(fLenX + fLenY + fLenZ) / 3.0f) / 2.0f;		// 모델의 평균(!) 반지름.

	fRadius += fRange;		// 반지름에다 m_nArrivalRange를 더함.

	D3DXVECTOR3 vDist = GetPos() - pObj->GetPos();	// 두좌표간 벡터
	vDist.y = 0.0f;
	float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
	
	if( fDistSq < fRadius * fRadius )		// 두점간의 거리가 범위보다 작으면 충돌
		return TRUE;
	
	return FALSE;
	
}
#ifndef __WORLDSERVER
CPoint CObj::GetClientPoint( LPDIRECT3DDEVICE9 pd3dDevice, int nBoundBoxIndex )
{
	D3DXVECTOR3 vOut, vPos;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	vPos = pBB->m_vPos[ nBoundBoxIndex ];
	pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y , m_vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixScale() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixRotation() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	D3DXVec3Project( &vOut, &vPos, &vp, &m_pWorld->m_matProj, &m_pWorld->m_pCamera->m_matView, &matWorld );	
	return CPoint( (int)( vOut.x ), (int)( vOut.y ) );
}
CPoint CObj::GetClientPoint( LPDIRECT3DDEVICE9 pd3dDevice )
{
	D3DXVECTOR3 vOut, vPos = GetPos();
	D3DVIEWPORT9 vp;
	pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixScale() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixRotation() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixTrans() );
	D3DXVec3Project( &vOut, &D3DXVECTOR3(0,0.0f,0), &vp, &m_pWorld->m_matProj, &m_pWorld->m_pCamera->m_matView, &matWorld );
	return CPoint( (int)( vOut.x ), (int)( vOut.y ) );
}
#endif

int		g_ShipExp = FALSE;
D3DXVECTOR3	g_vPosShip;
//
//	비행선
//
void	CObj::ProcessAirShip( void )
{
	return;
}


DWORD CObj::GetLinkType( void )
{
	switch( m_dwType )
	{
#ifdef __Y_PATROL		
		case OT_PATH:
#endif //__Y_PATROL
		case OT_OBJ:
		case OT_REGION:
			return CObj::linkStatic;
		case OT_SHIP:
			return CObj::linkAirShip;
		case OT_MOVER:
			if( ( (CMover*)this )->IsPlayer() == TRUE )
				return CObj::linkPlayer;
		case OT_CTRL:
		case OT_SFX:
		case OT_ITEM:
			return CObj::linkDynamic;
	}
	ASSERT( 0 );
	return( -1 );
}

void CObj::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
	/*
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
    pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &m_pWorld->m_matProj,
		&m_pWorld->m_pCamera->m_matView, &matWorld);
	vOut.x -= pFont->GetTextExtent( m_szName ).cx / 2;
	pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, m_szName );
	pFont->DrawText( vOut.x, vOut.y, dwColor, m_szName );
	*/
	return;
}

// this의 반지름을 계산함.
FLOAT	CObj::GetRadius( void )
{
	float	fLenX = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	float	fLenY = fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y);
	float	fLenZ = fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);
	
	float	fRadius = ((fLenX + fLenY + fLenZ) / 3.0f) / 2.0f;		// this(Dest)모델의 평균(!) 반지름.
	fRadius *= m_vScale.x;		// 스케일 조정.

	return fRadius;
}

// this의 반지름을 계산함 - XZ평면의 크기만.
FLOAT	CObj::GetRadiusXZ( void )
{
	float	fLenX = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	float	fLenZ = fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);
	
	float	fRadius = ((fLenX + fLenZ) / 2.0f) / 2.0f;		// this(Dest)모델의 평균(!) 반지름.
	fRadius *= m_vScale.x;		// 스케일 조정.
	
	return fRadius;
}

void CObj::SetWorld( CWorld* pWorld )	
{ 
	m_pWorld = pWorld; 
}

// 링크레벨을 설정한다.
void CObj::CalcLinkLevel()
{
	if( m_pModel == NULL )		// 스킬에서 쓰는 가상오브젝트때문에 모델이 없는 오브젝트도 생겼다.
	{
		m_dwLinkLevel = 0;
		return;
	}

	float fThisWidth = 0.0f;
	if( m_pModel->IsNullBoundBox() == FALSE )
	{
		D3DXMATRIX mWorld = GetMatrixWorld();
		D3DXVECTOR4 vMin, vMax;
		D3DXVec3Transform( &vMin, &m_pModel->m_vMin, &mWorld );	
		D3DXVec3Transform( &vMax, &m_pModel->m_vMax, &mWorld );	
		fThisWidth =( fabs(vMax.x - vMin.x) > fabs(vMax.z - vMin.z) ) ? vMax.x - vMin.x : vMax.z - vMin.z; 
		fThisWidth = fabs( fThisWidth );
	}


	int nWidthLink, nWidthPerGrid;
	nWidthLink	= MAP_SIZE * 2;
	int i = 0;
	for( ; i < MAX_LINKLEVEL; i++ )
	{
		// 16	// 8	// 4	// 2	// 1	// 1	-	// 32	// 64	// 128	// 256	// 512	// 1024
		// 128	// 64	// 32	// 16	-	// 4	// 8	// 16	// 32
		nWidthLink	/= 2;
		if( nWidthLink == 0 )	
			nWidthLink	= 1;

		nWidthPerGrid	= MAP_SIZE * MPU / nWidthLink;
		if( fThisWidth < (float)nWidthPerGrid )
			break;
	}

	if( i == MAX_LINKLEVEL )
		i = MAX_LINKLEVEL - 1;

	m_dwLinkLevel	= i;
}

#ifdef __BEAST
void CObj::RenderRect( LPDIRECT3DDEVICE9 pd3dDevice, CRect rect, DWORD dwColor )
{
	D3DXVECTOR3 vPos = GetPos();
	//m_pWorld->RenderRectTracker( CRect( vPos.x- 10, vPos.z - 10,  vPos.x + 10, vPos.z + 10 ), 0, 0x808080ff );
	CPoint point = CPoint( (int)( vPos.x ), (int)( vPos.z ) );
	point.x = point.x / MPU * MPU;
	point.y = point.y / MPU * MPU;
	rect += point;
	// RenderGrids는 원점의 트랜스폼에서 시작하므로 트랜스폼을 리셋해야한다.
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld ); 
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
	m_pWorld->RenderGrids( rect, 0, dwColor );//0x80ffff00 );
//	RenderName( pd3dDevice );
}
#endif
