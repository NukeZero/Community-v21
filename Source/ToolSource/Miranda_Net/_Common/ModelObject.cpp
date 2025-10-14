//
//
//
//
//
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <stdio.h>
#ifndef __SFXEDITOR
#include "defineObj.h"
#include "defineNeuz.h"
#endif
#include "Material.h"
#include "ModelGlobal.h"
#include "ModelObject.h"
//#include "..\_UnhandledException\ExceptionHandler.h"

//#include "..\_DirectX\dxutil.h"
//#include "..\_DirectX\d3dutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef	_DEBUG
extern LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
extern LPDIRECT3DVERTEXSHADER9	g_pSkiningVS;
#endif

BOOL	g_bUsableVS = TRUE;		// 하드웨어 버텍스 쉐이더(1.1)를 쓸수 있는가.
DWORD	g_dwUsablePS_Level = 0;		

extern float g_fHairLight;
extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;		// 라이트 벡터
extern D3DXVECTOR4 s_vLightPos;		// 라이트 포지션
extern D3DXMATRIX s_mViewProj;		// 
extern D3DXMATRIX s_mView;		// 
extern D3DXMATRIX s_mProj;		// 

extern BOOL s_bLight;


char   g_szLastFile[512];

/////////////////////////////////////////////////////////////////////////////////////
////////////
////////////
////////////
////////////
////////////
/////////////////////////////////////////////////////////////////////////////////////
CModelObject::CModelObject()
{
	Init();
}

CModelObject::~CModelObject()
{
	Destroy();
}

void CModelObject::Init( void )
{
	int		i;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Init( i );
	}

	m_pBone = NULL;
	m_pForce = m_pForce2 = NULL;
	m_pPartsEffect = m_pPartsEffect2 = NULL;
	m_pMotion = m_pMotionOld = NULL;
	m_mUpdateBone = NULL;
//	D3DXMatrixTranslation( &m_matPos, 0, 0, 0 );
//	m_fYRadian = 0;
	m_nModelType = MODELTYPE_NONE;//BIPED_MESH;
	m_bMotionBlending = FALSE;
	m_fBlendWeight = 0;
//	m_nMaxBone = 0;
//	m_bSendVS = 0;
	m_bCollObj = 0;
//	m_pBaseBone = 
	m_pBaseBoneInv = NULL;

	m_vMin.x = m_vMin.y = m_vMin.z = 65535.0;
	m_vMax.x = m_vMax.y = m_vMax.z = -65535.0;
	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;

	m_pCloakTexture = NULL;
//	D3DXMatrixIdentity( &m_mWorldRH );
/*
	m_bLocal = FALSE;
	m_vLastKeyPos = D3DXVECTOR3( 65535.0f, 65535.0f, 65535.0f );

	m_vLastPath.x = m_vLastPath.y = m_vLastPath.z = 0;
*/
	
}

void CModelObject::Init( int nIdx )
{
	if( nIdx < 0 )	return;
	memset( &m_Element[nIdx], 0, sizeof(O3D_ELEMENT) );
	m_Element[nIdx].m_nParentIdx = -1;
#ifdef __YPARTS_EX
	m_Element[nIdx].m_nPartsIdx = -1;
#endif
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalRH );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalLH );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalShield );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalKnuckle );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalTM );
}

//
void CModelObject::Destroy( void )
{
	int		i;

	// 메쉬 삭제
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Destroy( i );
	}
	SAFE_DELETE_ARRAY( m_mUpdateBone );

	SAFE_DELETE( m_pForce );
	SAFE_DELETE( m_pForce2 );
	SAFE_DELETE( m_pPartsEffect );
	SAFE_DELETE( m_pPartsEffect2 );
	Init();
	// m_pBone를 직접 삭제하면 안된다.
	// m_pMotion을 직접 삭제하면 안된다.
	// CObject3D::m_pObject3D를 직접 삭제하면 안된다.  CObject3DMng에서 삭제해 준다.
}

//
BOOL CModelObject::Destroy( int nIdx )
{
	if( nIdx < 0 )	return FALSE;
	O3D_ELEMENT	*pElem = &m_Element[ nIdx ];
	int		i;

	if( pElem->m_pObject3D )
	{
		if( pElem->m_ppd3d_VB )
		{
			pElem->m_pObject3D->SetLOD(0);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )		// 
				SAFE_RELEASE( pElem->m_ppd3d_VB1[i] );
			pElem->m_pObject3D->SetLOD(1);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				SAFE_RELEASE( pElem->m_ppd3d_VB2[i] );
			pElem->m_pObject3D->SetLOD(2);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				SAFE_RELEASE( pElem->m_ppd3d_VB3[i] );
			pElem->m_pObject3D->SetLOD(0);
			
			SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );		// 전체 메모리 풀을 한꺼번에 날림.
		}
		g_Object3DMng.DeleteObject3D( pElem->m_pObject3D );	// pObject3D가 공유된게 아니면 여기서 날려 버림.
		Init( nIdx );
		return TRUE;
	}
	Init( nIdx );
	return FALSE;
}

// 검광클래스 생성
void	CModelObject::CreateForce( int nParts )
{
#ifdef __CLIENT
	if( nParts == PARTS_RWEAPON )
	{
		if( m_pForce )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce = new CSwordForce;
	} else
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pForce2 )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce2 = new CSwordForce;
	} else
	{
		ASSERT(0);
	}
#endif
}


void	CModelObject::TakeOffParts( int nParts )
{
	if( nParts < 0 )
		Error( "CModelObject::TakeOffParts : nParts = %d", nParts );

#ifndef __YPARTS_EX
	if( nParts >= MAX_ELEMENT )
		return;
#endif //__YPARTS_EX

	// 파츠를 제거했는데, 아래 부분을 안해주면 old 파츠에서 매모리 릭 발생 - vampyre
	O3D_ELEMENT	*pElem;
#ifdef __YPARTS_EX
	int nIdx = GetPartsIdx(nParts);
	pElem = GetParts(nParts);
#else //__YPARTS_EX
	pElem = &m_Element[ nParts ];
#endif //__YPARTS_EX
	if( pElem && pElem->m_ppd3d_VB )
	{
		pElem->m_pObject3D->SetLOD(0);
		for( int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
			SAFE_RELEASE( pElem->m_ppd3d_VB1[j] );
		pElem->m_pObject3D->SetLOD(1);
		for( int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			SAFE_RELEASE( pElem->m_ppd3d_VB2[j] );
		pElem->m_pObject3D->SetLOD(2);
		for( int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			SAFE_RELEASE( pElem->m_ppd3d_VB3[j] );
		pElem->m_pObject3D->SetLOD(0);
		SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );		// 전체 메모리 풀을 한꺼번에 날림.
	}

#ifdef __YPARTS_EX
	if(nIdx != -1)
		Init( nIdx );
#else //__YPARTS_EX
	Init( nParts );
#endif //__YPARTS_EX

}

int		CModelObject::GetMaxObject( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxObject();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxVertex( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxVertex();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxFace( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxFace();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxMtrlBlk();
		}
	}

	return nMax;
}


//
//
// m_fFrameCurrent의 Path Delta값을 보간하여 리턴
/*
void	CModelObject::GetPathDelta( D3DXVECTOR3 *pOut )
{
	int nCurFrm, nNextFrm;
	float fSlerp;
	float fNumFrm = m_fFrameCurrent;
	D3DXVECTOR3 vSlerp;

	if( m_pMotion->m_pPath == NULL )
		Error( "CModelObject::GetPathPos : m_pPath가 NULL" );

	nCurFrm = (int)fNumFrm;
	nNextFrm = nCurFrm + 1;
	if( nNextFrm >= m_nFrameMax )	nNextFrm = m_nFrameMax - 1;
	fSlerp = fNumFrm - (float)nCurFrm;

	D3DXVECTOR3 *pPath = m_pMotion->m_pPath;
	D3DXVec3Lerp( &vSlerp, &pPath[nCurFrm], &pPath[nNextFrm], fSlerp );		// 보간된 Pos

	*pOut = vSlerp - m_vLastPath;		// 마지막 Path[ m_fFrameCurrent ]에서 이전 PathPos를 빼서 Delta로 만든다.
	m_vLastPath = vSlerp;
	
}
*/

/*
// 바운딩박스의 8귀퉁이 좌표를 리턴
const BOUND_BOX		*CModelObject::GetBBVector( void )
{
	D3DXVECTOR3		*v = m_BB.m_vPos;

	return	&m_BB;
}
*/
//
//		LoadBone
//
int		CModelObject::LoadBone( LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY
	if( m_pBone )
		Message( "CModelObject::LoadBone ; 이미 본이 로딩되어 있다. 0x%08x", (int)m_pBone );

	// Bone 관리자를 통해 본을 로딩한후 그 포인터를 받아온다.
	m_pBone = g_BonesMng.LoadBone( szFileName );

	// 본을 못읽었다.
	if( m_pBone == NULL )
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", szFileName );
		ADDERRORMSG( szStr );
		return FAIL;
	}

	m_bSkin = TRUE;		// 스키닝 오브젝트라는 플래그
#ifdef _DEBUG
	if( m_mUpdateBone )
		Message( "CModelObject::LoadBone : %s 이미 읽었는데 또 읽은것 같다.", szFileName );
#endif

	// 오브젝트의 계층구조가 애니메이션되면서 실시간으로 변환되는 매트릭스 배열
//	if( m_mUpdateBone == NULL )		// 모션을 재로딩을 할 수 있으므로 이미 할당 받았으면 다시 받지 않음.
	m_mUpdateBone = new D3DXMATRIX[ m_pBone->m_nMaxBone * 2 ];	// Inv랑 같이 쓰려고 * 2로 잡는다,.
	m_pBaseBoneInv = m_mUpdateBone + m_pBone->m_nMaxBone;
	
	int		i;
	for( i = 0; i < m_pBone->m_nMaxBone; i ++ )
	{
		m_mUpdateBone[i] = m_pBone->m_pBones[i].m_mTM;		// 기본셋은 미리 카피해둠.
		m_pBaseBoneInv[i] = m_pBone->m_pBones[i].m_mInverseTM;	// Inv도 미리 받아둠.
	}
#endif // not viewer
	return SUCCESS;
}


//
//	LoadMotion
// 오브젝트를 로딩하지않고도 독립적으로 동작해야한다.
// 
int		CModelObject::LoadMotion( LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY
	int		i;
	// 본이 아직 할당 되어 있지 않으면 애니를 읽을수 없다.
	if( m_pBone == NULL )		
	{
		Error( "%s : 본을 로딩하지 않고 모션을 읽음", szFileName );
		return FAIL;
	}
	m_pMotionOld = m_pMotion;		// 예전 모션받아둠.
	m_nFrameOld = (int)m_fFrameCurrent;
	// Motion관리자를 통해 동작을 로딩한후 그 포인터를 받아온다.
	m_pMotion = g_MotionMng.LoadMotion( szFileName );
	if( m_pMotion == NULL )		// 모션을 못읽었다.
	{
		return FAIL;
	}
	if( m_pMotionOld == NULL )
		m_pMotionOld = m_pMotion;		// 이전프레임이 없으면 현재 프레임을 Old로 쓴다.  NULL이 되는상황이 없게 할것.

	InitAnimate();

	m_nFrameMax = m_pMotion->GetMaxFrame();
	m_fPerSlerp = (float)((int)(m_pMotion->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.
//	m_vLastKeyPos = D3DXVECTOR3( 65535.0f, 65535.0f, 65535.0f );
//	m_vLastPath.x = m_vLastPath.y = m_vLastPath.z = 0;

	if( m_pMotionOld->m_nMaxBone != m_pMotion->m_nMaxBone )		// 이전에 읽은 모션의 본개수와 다르면 에러냄.
	{
		LPCTSTR szErr = Error( "CModelObject::LoadElement - %s bone count(%d) - old count(%d)", szFileName, m_pMotion->m_nMaxBone, m_pMotionOld->m_nMaxBone );
		ADDERRORMSG( szErr );
	}

//	if( m_mUpdateBone == NULL )		// 모션을 재로딩을 할 수 있으므로 이미 할당 받았으면 다시 받지 않음.
//		m_mUpdateBone = new D3DXMATRIX[ m_pMotion->m_nMaxBone ];
	
	// 이미 스킨 오브젝트가 로딩되어 있는 상태면 m_mUpdateBone을 그 오브젝트에 등록시킴.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
			m_Element[i].m_pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 함수내부에서 스킨인넘만 등록시킨다.
	}
#endif // not viewer
	return SUCCESS;
}

// 파츠를 옮김.
// 왼손에 든걸 오른손에 옮기는등...
int	CModelObject::MovePart( int nDstPart, int nSrcPart )
{
#ifdef __YPARTS_EX
	O3D_ELEMENT* pDestPart = GetParts( nDstPart );
	O3D_ELEMENT* pSrcPart = GetParts( nSrcPart );

	if( !pDestPart && pSrcPart )
	{
		LoadElement( pSrcPart->m_pObject3D->m_szFileName, nDstPart );
		memset( pSrcPart, 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.	
		return 1;
	}

	if( !pDestPart || !pSrcPart )
		return 0;

	if( pDestPart->m_pObject3D )
		Message( "CModelObject::MovePart : m_Element[%d].m_pObject3D == 0x%08x", nDstPart, (int)pDestPart->m_pObject3D );
	
	memcpy( pDestPart, pSrcPart, sizeof(O3D_ELEMENT) );	// 카피하고
	memset( pSrcPart, 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.	
#else //__YPARTS_EX
	if( m_Element[ nDstPart ].m_pObject3D )
		Message( "CModelObject::MovePart : m_Element[%d].m_pObject3D == 0x%08x", nDstPart, (int)m_Element[ nDstPart ].m_pObject3D );
	memcpy( &m_Element[ nDstPart ], &m_Element[ nSrcPart ], sizeof(O3D_ELEMENT) );	// 카피하고
	memset( &m_Element[ nSrcPart ], 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.
#endif //__YPARTS_EX
	return 1;
}

//
//	오브젝트 로딩.  모션을 읽지 않은 상태에서도 독립적으로 동작한다.
//
int		CModelObject::LoadElement( LPCTSTR szFullName, int nParts )
{
	LPCTSTR szFileName = GetFileName( szFullName );

#ifndef __VIEWER_ONLY
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;

	if( nParts < 0 )
	{
		Error( "CModelObject::LoadElement : nParts 에러 - nParts = %d, %s", nParts, szFileName );
		return FAIL;
	}

#ifndef __YPARTS_EX
	if( nParts >= MAX_ELEMENT )
		return FAIL;
#endif //__YPARTS_EX
	

#ifndef __COLA
	if( nParts == PARTS_RIDE )
		return FAIL;
#endif

#ifdef __YPARTS_EX
	if(  nParts >= PARTS_LOWER2 && nParts <= PARTS_BULLET )
		return FAIL;

	int nIdx = GetPartsIdx(nParts);
	Destroy( nIdx );
#else //__YPARTS_EX
	Destroy( nParts );
#endif //__YPARTS_EX

#ifdef __CLIENT
	HRESULT hr;
	// Object3D관리자를 통해 메쉬를 로딩한 후 그 포인터를 받아온다.
	if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// 디바이스가 허접하면 에러남김.
	{
		LPCTSTR szErr = Error( "%s 디바이스실패 %08x", szFileName, (int)hr );
		ADDERRORMSG( szErr );
	}
#endif

	pObject3D = g_Object3DMng.LoadObject3D( m_pd3dDevice, szFileName );
	//TRACE( "new Object 3D%p\n", pObject3D);
	
	if( pObject3D == NULL )
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", MakePath( DIR_MODEL, szFileName ) );
		ADDERRORMSG( szStr );
		return FAIL;
	}
	pObject3D->InitDeviceObjects( m_pd3dDevice );

	// 스킨파츠의 경우 외부본을 읽었는지 검사.
	if( pObject3D->IsUseExternBone() )		// 외장본을 사용하는 오브젝이다.
	{
		//!!gmpbigsun : 외장본을 사용하는 오브젝트인 경우 자동으로 로드한다.
		if( NULL == m_pBone  )
		{
			CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
			pView->LoadBone_Auto( szFullName );
		}

		if( m_pBone == NULL )			// 본을 아직 안읽은 상태다...
		{
			
			Error( "%s : bone not loaded.", szFileName );		// 본을 로딩하지 않고 메쉬를 읽음
			return FAIL;
		}
	}
	
#ifdef __YPARTS_EX
	for( int i=0; i<MAX_ELEMENT; i++ )
	{
		if( m_Element[ i ].m_nPartsIdx == -1 )
			break;
	}

	if( i == MAX_ELEMENT )
		return FAIL;

	pElem = &m_Element[ i ];
	memset( pElem, 0, sizeof(O3D_ELEMENT) );
	pElem->m_nPartsIdx = nParts;
	pElem->m_nParentIdx = -1;
	D3DXMatrixIdentity( &(pElem->m_mLocalRH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalLH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalShield) );
	D3DXMatrixIdentity( &(pElem->m_mLocalKnuckle) );

#else //__YPARTS_EX

	pElem = &m_Element[ nParts ];

#endif //__YPARTS_EX	

	pElem->m_pObject3D = pObject3D;
	
	memset( &g_szLastFile, 0, sizeof(char)*512 );
	strcpy( g_szLastFile, szFileName );

	if( pObject3D->GetGMOBJECT() == NULL )
	{
		LPCTSTR szStr = Error( "CModelObject::LoadElement에서 튕김 : 읽은 파일명 %s", MakePath( DIR_MODEL, szFileName ) );
		ADDERRORMSG( szStr );
		return FAIL;
	}
	
	if( pObject3D->GetGMOBJECT()->m_bOpacity )
		m_dwRenderEffect = REFF_ALPHA;
//	if( pObject3D->GetGMOBJECT()->m_bReflection )
//		m_dwRenderEffect = REFF_REFLECT;
	m_bCollObj = pObject3D->IsHaveCollObj();
	if( pObject3D->m_nHavePhysique )	m_bSkin = TRUE;

	if( m_nFrameMax > 0 )	// 이미 모션이나 오브젝을 읽어서 maxframe이 있을때 이번에 읽은 오브젝틔 maxframe하고 비교. 틀리면 에러.
	{
		if( pObject3D->GetMaxFrame() )
			if( m_nFrameMax != pObject3D->GetMaxFrame() )
			{
				LPCTSTR szErr = Error( "CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax );
				ADDERRORMSG( szErr );
			}
	}
	if( m_nFrameMax == 0 )	// maxframe이 세팅된적이 없을때만 최초 세팅함.
		m_nFrameMax = pObject3D->GetMaxFrame();

	// 애니메이션 속도
	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.


	// 이미 본이 로딩되어 있는 상태면 m_pUpdateBone을 오브젝트의 외부뼈대로 등록함.
	if( m_pBone )
		pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );


	// 새로 읽은 Mesh의  BB를 기준으로 CModelObject의 BB를 갱신
	// 이부분계산은 현재 정확하지 않다.  Min,Max모두 Local기준인데다가
	// 사람이 창을 쥐고 있을경우 창까지 바운딩 박스에 포함되고 있다.
	if( pObject3D->m_vBBMin.x < m_vMin.x )	m_vMin.x = pObject3D->m_vBBMin.x;
	if( pObject3D->m_vBBMin.y < m_vMin.y )	m_vMin.y = pObject3D->m_vBBMin.y;
	if( pObject3D->m_vBBMin.z < m_vMin.z )	m_vMin.z = pObject3D->m_vBBMin.z;
	if( pObject3D->m_vBBMax.x > m_vMax.x )	m_vMax.x = pObject3D->m_vBBMax.x;
	if( pObject3D->m_vBBMax.y > m_vMax.y )	m_vMax.y = pObject3D->m_vBBMax.y;
	if( pObject3D->m_vBBMax.z > m_vMax.z )	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB( m_BB.m_vPos, m_vMin, m_vMax );


	// 읽어온 엘리먼트는 this의 뼈대에 부모지정이 안되어 있게 한다.
	// 만약 칼을 읽어왔다면 수동으로 SetParent()하도록 한다.
	pElem->m_nParentIdx = -1;
/*	
	// 읽은 오브젝트가 부모인덱스를 가진다면 그것을 지정함.
	// 일반적으로 오른손이 부모로 되어 있다.
	int nParentIdx = pObject3D->GetGMOBJECT()->m_nParentIdx;
	if( nParentIdx != -1 )
		SetParent( nParts, nParentIdx );
	else
		pElem->m_nParentIdx = -1;
*/
	if( pObject3D->m_nHavePhysique )	// 피지크를 가지고 있다면 스킨은 버텍스버퍼를 따로 가짐
	{
		if( pElem->m_ppd3d_VB )
			Message( "CModelObject::LoadElement: %s pElem->m_ppd3d_VB is not NULL", szFileName );

		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);
		
		if( nTotal <= 0 )
			Error( "%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal );

		pElem->m_ppd3d_VB = new LPDIRECT3DVERTEXBUFFER9[ nTotal ];
		memset( pElem->m_ppd3d_VB, 0, sizeof(LPDIRECT3DVERTEXBUFFER9) * nTotal );
		LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB = pElem->m_ppd3d_VB;
		pObject3D->SetLOD(0);		pElem->m_ppd3d_VB1 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_ppd3d_VB2 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_ppd3d_VB3 = ppd3d_VB;		
		pObject3D->SetLOD(0);
		pElem->m_ppd3d_VBSel = pElem->m_ppd3d_VB1;
	}
	
	// 버텍스 버퍼 생성
//	if( pObject3D->GetGMOBJECT()->m_Type == GMT_SKIN )
//		pObject3D->RestoreDeviceObjects( D3DPOOL_SYSTEMMEM );		// 이걸 여기서 하면 안되는 이유가 있어서 안한건데 그 이유가 기억이 안난다 -_-;;;
//	else
		pObject3D->RestoreDeviceObjects( pElem->m_ppd3d_VB );		// 이걸 여기서 하면 안되는 이유가 있어서 안한건데 그 이유가 기억이 안난다 -_-;;;
#endif // not viewer		
	return SUCCESS;
}

// SetParent( PARTS_LWEAPON, BONE_RHAND );
// nParts의 오브젝트의 부모를 nBoneIdx로 세팅
// 부모를 바꿀때 쓴다.
void	CModelObject::SetParent( int nParts, int nBoneIdx )
{
#ifdef __YPARTS_EX
	O3D_ELEMENT	*pElem = GetParts( nParts );

	if( !pElem )
		return;

#else //__YPARTS_EX
	O3D_ELEMENT	*pElem = &m_Element[ nParts ];
#endif //__YPARTS_EX

	pElem->m_nParentIdx = nBoneIdx;
	pElem->m_mLocalTM = pElem->m_pObject3D->GetGMOBJECT()->m_mLocalTM;

	// 만약 본의 개수가 바뀌었다면 BONE_RHAND의 인덱스도 바껴야 한다. 인간형모델은 본 개수를 통일 시켜야 한다.
	// 본 인덱스를 쉽게 알수 있는 방법은 CModelObject->m_pMotion->m_pBoneInfo[]에 0~32까지를 입력시켜보면서 Bip01 R Hand를 찾는다,
#ifndef __COLA
	if( nBoneIdx == GetRHandIdx() )	
		pElem->m_mLocalRH = m_pBone->m_mLocalRH;

	if( nBoneIdx == GetLHandIdx() )	
		pElem->m_mLocalLH = m_pBone->m_mLocalLH;
	
	if( nBoneIdx == GetLArmIdx() )	
		pElem->m_mLocalShield = m_pBone->m_mLocalShield;

	if( nBoneIdx == GetRArmIdx() )
		pElem->m_mLocalKnuckle = m_pBone->m_mLocalKnuckle;
#endif
}

//
// nParts에 텍스쳐파일 szBitmap을 멀티텍스쳐로 등록
// 기존 텍스쳐를 파괴할 필요는 없다.
// 반드시 LoadMesh()이후에 되어야 한다.
//
void	CModelObject::SetTextureMulti( LPCTSTR szBitmap, int nParts )
{
	D3DMATERIAL9	d3dmtrl;
	MATERIAL		*mtrl;

	mtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &d3dmtrl, szBitmap );
#ifdef __YPARTS_EX
	O3D_ELEMENT* pParts = GetParts(nParts);

	if( pParts )
		pParts->m_pTextureEx = mtrl->m_pTexture;
#else //__YPARTS_EX
	m_Element[ nParts ].m_pTextureEx = mtrl->m_pTexture;		// m_pTextureEx에 포인터가 있으면 렌더링할때 멀티로 셋팅
#endif //__YPARTS_EX
}

//
void	CModelObject::ClearTextureEx( int nParts )
{
#ifdef __YPARTS_EX
	O3D_ELEMENT* pParts = GetParts(nParts);
	
	if( pParts )
		pParts->m_pTextureEx = NULL;
#else //__YPARTS_EX
	m_Element[ nParts ].m_pTextureEx = NULL;
#endif //__YPARTS_EX
}

//
void	CModelObject::ChangeTexture( int nParts, LPCTSTR szSrc, LPCTSTR szDest )
{
#ifdef __WORLDSERVER
	return;
#endif

#ifdef __YPARTS_EX
	O3D_ELEMENT* pParts = GetParts(nParts);
	
	if( pParts )
		pParts->m_pObject3D->ChangeTexture( szSrc, szDest );
#else //__YPARTS_EX	
	m_Element[ nParts ].m_pObject3D->ChangeTexture( szSrc, szDest );
#endif //__YPARTS_EX
}

#ifndef __WORLDSERVER
//---------------------------------------------------------------------------------------------
//
// Render()를 부를때 내부에서 SetTransform()을 해주고 외부에선 매트릭스만 넘기는것을 원칙으로 하자.
// 넘겨주는 매트릭스는 오브젝트의 원점을 넘겨준다.
// 가령 인간의 경우는 센터부분이다.
//
FLOAT   g_fDiffuse[4];
FLOAT   g_fAmbient[4];
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
만약에 Render했는데 화면에 안나온다!!! 그러면!
static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
SetTransformView( matView );
SetTransformProj( matProj );
SetDiffuse( 1.0, 1.0, 1.0 );
SetAmbient( 1.0, 1.0, 1.0 );

이걸 렌더 하기전에 불러줬는지 확인해라!!!!!!!!!!!!!!!!!!!!! 크아아악!
좆도 고생했네!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
 */
int		CModelObject::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
//	return 1;
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;
	int		i;
	int		nNextFrame;
	D3DXMATRIX	m1;

#ifndef __NEUZ	// 게임클라이언트가 아닐때...
	if( IsEmptyElement() == TRUE )		// 모델이 로드가 안되어 있으면 걍 리턴
		return 1;
#endif

#ifdef	_DEBUG
	if( m_mUpdateBone && g_pSkiningVS == NULL )
		Error( "CMyD3DApplication::RestoreDeviceObjects()에 CreateSkiningVS()를 호출하시오.\r\nCMyD3DApplication::InvalidateDeviceObjects()에는 DeleteSkiningVS()를 호출하시오.\r\n혹은 *.vsh파일을 읽지 못했다." );
	if( m_nLoop == 0 )
		Error( "경고 : %s : CModelObject::m_nLoop가 지정되지 않았다.", m_pMotion->m_szName );
#endif

#ifdef	__WORLDSERVER
	if( pd3dDevice == NULL )	return 1;
#endif

	if( m_pMotion )		// m_pMotion이 널이라면 m_mUpdateBone도 널이다.
		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
//		AnimateBone( NULL );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴

	if( m_pBone )		// m_pBone이 있다면 뼈대가 있다는 얘기. VS를 써야 한다.
	{
		D3DXMATRIX *pmBones;
		D3DXMATRIX mWorldTranspose;
		D3DXMATRIX *pmBonesInv = m_pBaseBoneInv ;
		BONE	*pBoneList = m_pBone->m_pBones;
		pmBones = m_mUpdateBone;

		if( m_pBone->m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
		{
			int		nMaxBone = m_pBone->m_nMaxBone;

			if( nMaxBone > MAX_VS_BONE )	
				Error( "CModelObject::Render : overflow bone count - %d", nMaxBone );

			for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE개 이하	
			{
				mWorldTranspose = pmBonesInv[i] * pmBones[i];
				
//#ifdef	__YENV		
//				CString str;
//				str.Format( "mBoneMatrix[%d]", i );
//				HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &mWorldTranspose );
//#else //__YENV
				D3DXMatrixTranspose( &mWorldTranspose, &mWorldTranspose );		// 매트릭스를 돌린다음.
				m_pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&mWorldTranspose, 3 );		// 상수레지스터에 집어넣음.
//#endif //__YENV
				
			}
		}
		D3DXMATRIX	mView, mProj;
		D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;

		D3DXVECTOR4 vLight = s_vLight;
		D3DXVECTOR4 vLightPos = s_vLightPos;

		mViewProj = *mWorld * s_mView * s_mProj;
		
		D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
		D3DXMatrixTranspose( &mWorldTranspose, mWorld );

		D3DXMatrixInverse( &mInvWorld, NULL, mWorld );
		D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
		D3DXVec4Normalize( &vLight, &vLight );
		D3DXVec4Transform( &vLightPos, &vLightPos, &mInvWorld );
//		D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );

#ifdef __YENV
		/*
		// 투영 설정...
		g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &mViewProjTranspose );
		
		// 라이트 위치 설정
		D3DXVECTOR4 v;
		D3DXVECTOR4 vLight_Pos = s_vLight;
		D3DXMATRIX mLocal;
		D3DXMatrixInverse( &mLocal, NULL, &mViewProjTranspose );						
		D3DXVec4Transform( &v, &vLight_Pos, &mLocal );						// 로컬좌표로 변환
		D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
		v.w = -0.6f;														// 환경광의 밝기(Ambint) Def : -0.3f
		
		// 라이트 방향 설정
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
		
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
		*/
#else //__YENV
		m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );		// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );	// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );	// specular use
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&vLightPos, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
//		m_pd3dDevice->SetVertexShaderConstant( 95, &vConst, 1 );
#endif //__YENV
	}
/*
*/
	if( m_nNoEffect == 0 )
	{
		// 기본 설정 
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
	}
	DWORD dwBlendFactor = m_dwColor | ( m_dwBlendFactor << 24 );
	//DWORD dwBlendFactor = 0xffff0000 ;//| ( m_dwBlendFactor << 24 );
	// 오브젝트의 반투명 효과 세팅 
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( m_dwBlendFactor, m_dwColor & , 0, 0) );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwBlendFactor );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	}

	nNextFrame = GetNextFrame();
	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vec3LightBackup = D3DXVECTOR3( s_vLight[0], s_vLight[1], s_vLight[2] );
	D3DXVECTOR4 vec4Diffuse = D3DXVECTOR4( s_fDiffuse[0], s_fDiffuse[1], s_fDiffuse[2], s_fDiffuse[3] );;
	
	// 엘리먼트엔 스킨,일반,모핑 심지어는 파티클까지도 포함될수있다.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];		

		pObject3D = pElem->m_pObject3D;
		if( pObject3D == NULL )		
			continue;

#ifdef __YPARTS_EX	
#ifndef __FINITEITEM0705
		// 외투의상을 입었다면 주방어구 의상은 숨긴다...			
		int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
		int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
			
		O3D_ELEMENT *pElem2 = NULL;
			
		for( int i=0; i<5; i++ )
		{
			if( pElem->m_nPartsIdx == nArryEquip1[i] )
			{
				pElem2 = GetParts( nArryEquip2[i] );

				if( pElem2 )
				{
					if( pElem )
					{
						pElem->m_nEffect |= XE_HIDE;
						break;
					}
				}
			}			
		}
#endif //__FINITEITEM0705
#endif //__YPARTS_EX

	  
#ifdef __ACROBAT_0504
		if( pElem->m_nEffect & XE_HIDE )	
			continue;
#endif

		pObject3D->m_nNoTexture = m_nNoTexture;
		pObject3D->m_nNoEffect = m_nNoEffect;
		// 뼈대에 링크되어 있다.
		if( pElem->m_nParentIdx != -1 )		// 본의 인덱스를 뜻한다.
		{
			// 만약 본의 개수가 바뀌었다면 ParentIdx도 틀려졌기 때문에 바꿔줘야 한다.
			m1 = m_mUpdateBone[ pElem->m_nParentIdx ] * *mWorld;		// 뼈대에 링크되어 있다면 뼈대위치가 센터가 된다.
			if( pElem->m_nParentIdx == GetRHandIdx() )
				m1 = pElem->m_mLocalRH * m1;
			else if( pElem->m_nParentIdx == GetLHandIdx() )
				m1 = pElem->m_mLocalLH * m1;
			else if( pElem->m_nParentIdx == GetLArmIdx() )
				m1 = pElem->m_mLocalShield * m1;
			else if( pElem->m_nParentIdx == GetRArmIdx() )
				m1 = pElem->m_mLocalKnuckle * m1;
		}
		else
			m1 = *mWorld;
		//*
#ifdef __YPARTS_EX
		if( pElem->m_nPartsIdx == PARTS_HAIR )
#else //__YPARTS_EX
		if( i == PARTS_HAIR )
#endif //__YPARTS_EX
		{
			memcpy( g_fDiffuse, s_fDiffuse, sizeof(FLOAT)*4 );
			memcpy( g_fAmbient, s_fAmbient, sizeof(FLOAT)*4 );

#ifndef __YENV
			SetDiffuse( 0, 0, 0 );
#endif //__YENV
			
#ifdef __CLIENT
			//*
			if( s_bLight )
			{
				float fblack = (CWorld::m_light.Diffuse.r*g_fHairLight) + (CWorld::m_light.Diffuse.g*g_fHairLight) + (CWorld::m_light.Diffuse.b*g_fHairLight);
				fblack = fblack / 3.0f;
				fblack *= 0.6f;
				SetAmbient( min( pObject3D->m_fAmbient[0]*fblack, 1.0f ), min( pObject3D->m_fAmbient[1]*fblack, 1.0f ), min( pObject3D->m_fAmbient[2]*fblack, 1.0f ) );
			}
			else
			/**/
#endif // CLIENT
				SetAmbient( pObject3D->m_fAmbient[0], pObject3D->m_fAmbient[1], pObject3D->m_fAmbient[2] );
			
#ifdef __YENV
			SetDiffuse( pObject3D->m_fAmbient[0], pObject3D->m_fAmbient[1], pObject3D->m_fAmbient[2] );
#endif //__YENV
			
			
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );			
#else //__YENV
			pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
			pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
#endif //__YENV
		} else
#ifdef __YPARTS_EX
		if( pElem->m_nPartsIdx == PARTS_CLOAK )
#else //__YPARTS_EX
		if( i == PARTS_CLOAK )
#endif //__YPARTS_EX 
		{
			g_ModelGlobal.SetTexture( m_pCloakTexture );		// 외부지정 망토텍스쳐가 있다면.
		}
		/**/
		if( m_pBone )
			pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
		
#if defined(__YSPARK) && defined(__CLIENT)
		if( m_SparkInfo.m_bUsed )
		{
			SetLightVec( m_SparkInfo.m_v3SparkDir );
			
			D3DXVECTOR3 vec3Diffuse;
			D3DXVec3Lerp( &vec3Diffuse, &D3DXVECTOR3(0.0f,0.0f,0.0f), &m_SparkInfo.m_v3Color, m_SparkInfo.m_fLerp );
			SetDiffuse( vec3Diffuse.x, vec3Diffuse.y, vec3Diffuse.z );
		}
#endif //defined(__YSPARK) && defined(__CLIENT)
		
		pObject3D->Render( pd3dDevice, pElem->m_ppd3d_VBSel, m_fFrameCurrent, nNextFrame, &m1, pElem->m_nEffect, dwBlendFactor );

		pObject3D->m_nNoEffect = 0;

#ifdef __YPARTS_EX
		if( pElem->m_nPartsIdx == PARTS_HAIR )
#else //__YPARTS_EX
		if( i == PARTS_HAIR )
#endif //__YPARTS_EX 
		{
			memcpy( s_fDiffuse, g_fDiffuse, sizeof(FLOAT)*4 );
			memcpy( s_fAmbient, g_fAmbient, sizeof(FLOAT)*4 );
			
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );			
#else //__YENV		
			pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
			pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
#endif //__YENV
		} else
#ifdef __YPARTS_EX
		if( pElem->m_nPartsIdx == PARTS_CLOAK )
#else //__YPARTS_EX
		if( i == PARTS_CLOAK )
#endif //__YPARTS_EX 
			g_ModelGlobal.SetTexture( NULL );
	}
	
	// 상태 해제
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		if( m_nNoEffect == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}
	}

	
#if defined(__YSPARK) && defined(__CLIENT)
	{
		SetLightVec( vec3LightBackup );
		SetDiffuse( vec4Diffuse.x, vec4Diffuse.y, vec4Diffuse.z );
	}
#endif //defined(__YSPARK) && defined(__CLIENT)
	
	return 1;
}

/*C2DRender g_2DRender;
void RenderVertexPoint( LPDIRECT3DDEVICE9 pD3DDevice, D3DXVECTOR3 vPos, DWORD dwColor, int iScale )
{
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	D3DXVECTOR3 vOut;
	D3DVIEWPORT9 vp;
	pD3DDevice->GetViewport( &vp );
	D3DXMATRIX matWorld;
	CRect rect;
	D3DXMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
	
	D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &pView->m_matProj,
		 &pView->m_Camera.m_matView, &matWorld );
	
	rect.SetRect( (int)( vOut.x - iScale ), (int)( vOut.y - iScale ), (int)( vOut.x + iScale ), (int)( vOut.y + iScale ) );
	g_2DRender.RenderFillRect( rect, dwColor );
}*/

BOOL CModelObject::RenderBone( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* mWorld, CModelObject* pBox )
{
	if( !m_pBone )
		return FALSE;

// 	if( !pBox2 )
// 	{
// 		pBox2 = new CModelObject;
// 		pBox2->InitDeviceObjects( pd3dDevice );
// 		pBox2->LoadElement( "region_begin.o3d", 0 );  
// 
// 		CObject3D* pObj = pBox2->GetObject3D( );
// 		GMOBJECT* pGMObj = pObj->GetGMOBJECT();
// 		pGMObj->m_mLocalTM = pGMObj->m_mLocalTM * kScaleMat;
// 		
// 		pBox2->m_vMin *= 0.1f;
// 		pBox2->m_vMax *= 0.1f;
// 	}

	if( !g_bWireframe )
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	// 각 본을 순회하면서 해당 위치에 박스를 그려준다.
	D3DXMATRIX tempMat;
	D3DXMatrixIdentity( &tempMat );
	for( int i = 0; i < m_pBone->m_nMaxBone ; ++i )
	{
		BONE* pBone = m_pBone->GetBone( i );
		if( pBone )
		{
			D3DXMATRIX kMat = pBone->m_mLocalTM;

		//	D3DXVec3TransformCoord( pOut, &m_pBone->m_vEvent[nIdx], &m_mUpdateBone[ i ])
		//	if( pBone->m_pParent )
		//		kMat = kMat * pBone->m_pParent->m_mLocalTM;

		//	D3DXMatrixMultiply( &tempMat, &kMat, &m_mUpdateBone[i] );
		//	D3DXMatrixMultiply( &tempMat, &tempMat, mWorld );

			// gmpbigsun: m_mUpdateBone은 부모의 Matrix를 써야한다
			if( pBone->m_pParent )
				tempMat = kMat * m_mUpdateBone[pBone->m_nParentIdx] * *mWorld;
			else tempMat = kMat * m_mUpdateBone[i] * *mWorld;
				
			if( 1 == pBone->m_iOverload )			//선택된 본 
			{
			//	pBox->ChangeTexture( 0, "", "red.tga");
				pBox->RenderBB( pd3dDevice, &tempMat );
				
			}
			
			pBox->Render( pd3dDevice, &tempMat );

	//		g_Grp3D.RenderPoint( D3DXVECTOR3( tempMat._41, tempMat._42, tempMat._43 ), 0xffff0000 );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	//gmpbigsun : 본 파일의 버젼이 4이하이면 이벤트좌표는 없다 현재 많은 파일들이 버젼4로 확인댐 
	// 아래코드는 일단 주석처리함 100120
// 	D3DXVECTOR3 kEventPos( 0.0f, 0.0f, 0.0f );
// 	D3DXMATRIX kMat;
// 	for( i = 0; i < MAX_MDL_EVENT; ++i )
// 	{
//		GetEventPos( &kEventPos, i );
// 
// 		if( kEventPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
// 		{
// 			D3DXMatrixTranslation( &kMat, kEventPos.x, kEventPos.y, kEventPos.z );
// 			pBox2->Render( pd3dDevice, &kMat );
// 		}
// 	}

	return TRUE;
}

// void CWorldView::RenderVertexPoint( D3DXVECTOR3 vPos, DWORD dwColor, int iScale )
// {
// 	D3DXVECTOR3 vOut;
// 	D3DVIEWPORT9 vp;
// 	m_pd3dDevice->GetViewport( &vp );
// 	D3DXMATRIX matWorld;
// 	CRect rect;
// 	D3DXMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
// 	
// 	D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
// 		&m_world.m_pCamera->m_matView, &matWorld );
// 	
// 	rect.SetRect( vOut.x - iScale, vOut.y - iScale, vOut.x + iScale, vOut.y + iScale ); 
// 	g_2DRender.RenderFillRect( rect, dwColor );
// }

void	CModelObject::RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	O3D_ELEMENT	*pElem;
	int		i;
	
	if( m_nNoEffect == 0 )
	{
		// 검광 렌더.
		if( m_pForce && m_pForce->m_nMaxSpline > 0 )	m_pForce->Draw( pd3dDevice, mWorld );
		if( m_pForce2 && m_pForce2->m_nMaxSpline > 0 )	m_pForce2->Draw( pd3dDevice, mWorld );
		
#ifdef __CLIENT
		if( CObj::GetActiveObj() )
		{
 #ifdef __XEFFECT1006
			int nEffect, nLevel;
			pElem = GetParts( PARTS_RWEAPON );

			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
			{
				nEffect = pElem->m_nEffect & 0x00FFFFFF;	// 하위 24비트
				nLevel = (pElem->m_nEffect >> 24);			// 상위 8비트
				if( nEffect & XE_ITEM_FIRE )
					CreateParticle( PARTS_RWEAPON, mWorld, PE_FIRE, nLevel );
				else
				if( nEffect & XE_ITEM_ELEC )
					RenderItemElec( PARTS_RWEAPON, mWorld, nLevel );
				else
				if( nEffect & XE_ITEM_WATER )
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WATER, nLevel );
				else
				if( nEffect & XE_ITEM_WIND )
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WIND, nLevel );
				else
				if( nEffect & XE_ITEM_EARTH )
					CreateParticle( PARTS_RWEAPON, mWorld, PE_EARTH, nLevel );
			}

			pElem = GetParts( PARTS_LWEAPON );
			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
			{
				nEffect = pElem->m_nEffect & 0x00FFFFFF;	// 하위 24비트
				nLevel = (pElem->m_nEffect >> 24);			// 상위 8비트
				if( nEffect & XE_ITEM_FIRE )
					CreateParticle( PARTS_LWEAPON, mWorld, PE_FIRE, nLevel );
				else
				if( nEffect & XE_ITEM_ELEC )
					RenderItemElec( PARTS_LWEAPON, mWorld, nLevel );
				else
				if( nEffect & XE_ITEM_WATER )
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WATER, nLevel );
				else
				if( nEffect & XE_ITEM_WIND )
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WIND, nLevel );
				else
				if( nEffect & XE_ITEM_EARTH )
					CreateParticle( PARTS_LWEAPON, mWorld, PE_EARTH, nLevel );
			}
							
 #else	// xEffect1006
			pElem = GetParts( PARTS_RWEAPON );
			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
			{
				if( pElem->m_nEffect & XE_ITEM_FIRE )
				CreateParticle( PARTS_RWEAPON, mWorld );
				else
				if( pElem->m_nEffect & XE_ITEM_ELEC )
					RenderItemElec( PARTS_RWEAPON, mWorld, 0 );
			}
			pElem = GetParts( PARTS_LWEAPON );
			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
			{
				if( pElem->m_nEffect & XE_ITEM_FIRE )
					CreateParticle( PARTS_LWEAPON, mWorld );
				else
				if( pElem->m_nEffect & XE_ITEM_ELEC )
					RenderItemElec( PARTS_LWEAPON, mWorld, 0 );
			}
 #endif // not xEffect1006
		}
#endif
		
		for( i = 0; i < MAX_ELEMENT; i ++ )
		{
			pElem = &m_Element[i];
			pElem->m_nEffect = 0;		// 쓰고나면 초기화 해주자
		}
	}
}


BOOL	CModelObject :: RenderBB( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 

	g_Grp3D.RenderAABB( m_vMin, m_vMax, 0xffff0000 );
	return 1;
}

#endif // WORLDSERVER





//
HRESULT CModelObject::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{ 
	m_pd3dDevice = pd3dDevice;  
	return S_OK; 
}

HRESULT CModelObject::RestoreDeviceObjects()
{
	int		i;
	O3D_ELEMENT	*pElem;
	CObject3D	*pObject3D;

	SetGroup(0);
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			// pElem이 스킨일땐 m_ppd3d_VB[]에 버텍스버퍼가 담겨서 돌아온다.
			pObject3D->RestoreDeviceObjects( pElem->m_ppd3d_VB );
		}
	}
	
	if( m_pPartsEffect )
		m_pPartsEffect->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2 )
		m_pPartsEffect2->RestoreDeviceObjects( m_pd3dDevice );
	
	return S_OK;
}	
HRESULT CModelObject::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();

	if( m_pPartsEffect )
		m_pPartsEffect->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2 )
		m_pPartsEffect2->InvalidateDeviceObjects( m_pd3dDevice );
	return  S_OK;
}	
HRESULT CModelObject::DeleteDeviceObjects()
{
	int		i, j;
	O3D_ELEMENT	*pElem;
	CObject3D				*pObject3D;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			if( pElem->m_ppd3d_VB )
			{
//				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
//					SAFE_RELEASE( pElem->m_ppd3d_VB[j] );
				pElem->m_pObject3D->SetLOD(0);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
					SAFE_RELEASE( pElem->m_ppd3d_VB1[j] );
				pElem->m_pObject3D->SetLOD(1);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					SAFE_RELEASE( pElem->m_ppd3d_VB2[j] );
				pElem->m_pObject3D->SetLOD(2);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					SAFE_RELEASE( pElem->m_ppd3d_VB3[j] );
				pElem->m_pObject3D->SetLOD(0);
	//				SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );	// 여기서 이걸 삭제하면 안된다. device 자원만 삭제.
			}
//			pObject3D->DeleteDeviceObjects();
		}
	}

	return  S_OK;
}	

void	CModelObject::FrameMove( D3DXVECTOR3 *pvSndPos, float fSpeed )
{
#ifdef __CLIENT
	if( m_pMotion )		// CModel::FrameMove에서 프레임이 증가되기전에 검사해봐야 한다.
	{
		MOTION_ATTR *pAttr = IsAttrSound();
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );
		
		BOOL bQuake = IsAttrQuake();
		if( bQuake && m_nPause == 0 )
		{
			CMover *pMover = CMover::GetActiveMover();
			if( pMover )
			{
			#ifdef __ACROBAT_0504
				ItemProp* pItemProp = pMover->GetActiveHandItemProp();
				
				if( pItemProp && pItemProp->dwItemKind3 != IK3_YOYO && !pMover->IsActiveMover() )
					pMover->GetWorld()->m_pCamera->SetQuake( 15 );				

			#else //__ACROBAT_0504
					pMover->GetWorld()->m_pCamera->SetQuake( 15 );
			#endif //__ACROBAT_0504
			}
		}
	} else
	{
		CObject3D *pObject = GetObject3D();
		MOTION_ATTR *pAttr = pObject->IsAttrSound( m_fFrameCurrent );
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );
	}
#endif
	
	CModel::FrameMove( NULL, fSpeed );

	if( m_pForce )
	{
		if( /*!m_bEndFrame &&*/ m_pForce->m_nMaxSpline && !m_nPause )
		{
			m_pForce->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce->m_nMaxDraw > m_pForce->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce->m_nMaxDraw = 0;
				else
					m_pForce->m_nMaxDraw = m_pForce->m_nMaxSpline;
			}
				
		}

		m_pForce->Process();
	}

	if( m_pForce2 )
	{
		if( m_pForce2->m_nMaxSpline && !m_nPause )
		{
			m_pForce2->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce2->m_nMaxDraw > m_pForce2->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce2->m_nMaxDraw = 0;
				else
					m_pForce2->m_nMaxDraw = m_pForce2->m_nMaxSpline;
			}
			
		}
		
		m_pForce2->Process();
	}
	
	if( m_bMotionBlending )
	{
		m_fBlendWeight += 0.05f;
//		if( fabs(m_fBlendWeight - 1.0f) < 0.001f )			
		if( m_fBlendWeight >= 1.0f )		// 블렌딩이 1.0까지 다 진행됐으면 블렌딩 해제
			SetMotionBlending( FALSE );
	}

}

//
//	검광을 생성
//	m_pMotion의 첫프레임~마지막프레임까지의 검광을 생성한다.
void	CModelObject::MakeSWDForce( int nParts, DWORD dwItemKind3, BOOL bSlow, DWORD dwColor, float fSpeed )
{
#ifndef __CLIENT
	return;
#endif
	D3DXVECTOR3		v1, v2;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
//	int		n1, n2;
	D3DXMATRIX	m1;
	if( !pElem ) 
		return;

	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

	// FrameCurrent를 0으로
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	int nLoop = m_nLoop;
	m_nLoop = ANILOOP_1PLAY;
	
	CreateForce( nParts );	// 검광 오브젝트 생성
	CSwordForce *pForce;
	if( nParts == PARTS_RWEAPON )
		pForce = m_pForce;
	else
		pForce = m_pForce2;

	pForce->Clear();
	pForce->m_dwColor = dwColor;

	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	

	while(1)
	{
		if( m_pMotion )
			m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
//		AnimateBone( FALSE );		// 뼈대를 애니메이션 시킴. 스키닝은 하지 않음

		// 무기 WorldTM = 무기LocalTM X 무기부모WorldTM  
		D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
		
		// 칼끝 버텍스의 월드 좌표 계산
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
		D3DXVec3TransformCoord( &v2, &(pElem->m_pObject3D->m_vForce2), &m1 );
		pForce->Add( v1, v2 );

		CModel::FrameMove( NULL, fSpeed );
		if( bSlow && IsAttrHit( m_fFrameCurrent ) )
			m_bSlow = TRUE;
		if( m_bEndFrame )
			break;
	}

	pForce->MakeSpline();		// 스플라인 생성
	

	// rewind
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	m_nLoop = nLoop;
	m_bSlow = FALSE;
}

#ifdef __XEFFECT1006
void	CModelObject::CreateParticle( int nParts, const D3DXMATRIX *pmWorld, int nType, int nLevel )
#else
void	CModelObject::CreateParticle( int nParts, const D3DXMATRIX *pmWorld )
#endif
{
#ifdef __CLIENT
 #ifdef __XEFFECT1006	
	if( nLevel < 0 || nLevel > 5 )
		return;
 #endif
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

	D3DXMATRIX	m1, m2;
	D3DXVECTOR3		v1, v2, v3;
	
	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;

//	D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m2 );
//	D3DXVec3TransformCoord( &v2, &(pElem->m_pObject3D->m_vForce2), &m2 );
	v1 = pElem->m_pObject3D->m_vForce1;
	v2 = pElem->m_pObject3D->m_vForce2;
	
	float fSlp = xRandomF(1);
	D3DXVec3Lerp( &v3, &v1, &v2, fSlp );

//	v3.x += ((0.5f - xRandomF(1)) / 5.0f);
//	v3.y += ((0.5f - xRandomF(1)) / 5.0f);
//	v3.z += ((0.5f - xRandomF(1)) / 5.0f);
	CPartsFire *pFire = NULL;
	
#ifdef __XEFFECT1006
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pPartsEffect2 && m_pPartsEffect2->m_nType != nType )	// 이펙트가 이미 지정되어 있고  같은게 아니라면
			SAFE_DELETE( m_pPartsEffect2 );							// 이펙트 삭제하고 다시 할당.
		if( m_pPartsEffect2 == NULL )
			m_pPartsEffect2 = new CPartsFire( nType );
		
		pFire = (CPartsFire *)m_pPartsEffect2;
	} else
	{
		if( m_pPartsEffect && m_pPartsEffect->m_nType != nType )	// 이펙트가 이미 지정되어 있고  같은게 아니라면
			SAFE_DELETE( m_pPartsEffect );							// 이펙트 삭제하고 다시 할당.
		if( m_pPartsEffect == NULL )
			m_pPartsEffect = new CPartsFire( nType );
		
		pFire = (CPartsFire *)m_pPartsEffect;
	}
	if( (g_nRenderCnt & 3) == 0 )
	{
		DWORD dwSfx = 0;
		switch( nType )
		{
		case PE_FIRE : dwSfx = XI_NAT_FIRE01;	break;
		case PE_ELEC : dwSfx = XI_NAT_FIRE01;	break;	// 이경우는 발생하지 않음.
		case PE_WATER: dwSfx = XI_NAT_WATER01;	break;
		case PE_WIND : dwSfx = XI_NAT_WIND01;	break;
		case PE_EARTH: dwSfx = XI_NAT_EARTH01;	break;
		default: dwSfx = XI_NAT_FIRE01;
		}
		dwSfx += nLevel;

		pFire->Create( m_pd3dDevice, v3, dwSfx );	// 해당 sfx로 파티클 생성시킴.
	}
#else // xEffect1006
	if( m_pPartsEffect && m_pPartsEffect->m_nType != PE_FIRE )	// 이펙트가 이미 지정되어 있고 불이 아니면
		SAFE_DELETE( m_pPartsEffect );							// 이펙트 삭제하고 다시 할당.
	if( m_pPartsEffect == NULL )
	{
		m_pPartsEffect = new CPartsFire;
	}
	
	pFire = (CPartsFire *)m_pPartsEffect;
	if( (g_nRenderCnt & 3) == 0 )
	{
		pFire->Create( m_pd3dDevice, v3 );
	}
#endif // not xEffect1006

	pFire->Render( m_pd3dDevice, &m2 );

#endif // client
}

void	CModelObject::RenderItemElec( int nParts, const D3DXMATRIX *pmWorld, int nLevel )
{
#ifdef __CLIENT
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	D3DXMATRIX	m1, m2;
	D3DXVECTOR3		v1, v2, v3;
	
	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;
	
//	D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
//	D3DXVec3TransformCoord( &v2, &(pElem->m_pObject3D->m_vForce2), &m1 );

	v1 = pElem->m_pObject3D->m_vForce1;		// 로컬로 건네주고
	v2 = pElem->m_pObject3D->m_vForce2;

	CPartsBeam *pBeam;
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pPartsEffect2 && m_pPartsEffect2->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect2 );
		}

		if( m_pPartsEffect2 == NULL )
			m_pPartsEffect2 = new CPartsBeam;

		pBeam = (CPartsBeam *)m_pPartsEffect2;
	} else
	{
		if( m_pPartsEffect && m_pPartsEffect->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect );
		}
		
		if( m_pPartsEffect == NULL )
			m_pPartsEffect = new CPartsBeam;
		
		pBeam = (CPartsBeam *)m_pPartsEffect;
	}
	
	pBeam->Render( m_pd3dDevice, &m2, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward, v1, v2, nLevel );
#endif // CLIENT
}

//
//
//
void	CModelObject::SetMotionBlending( BOOL bFlag )
{ 
	if( bFlag == m_bMotionBlending )	return;
	m_bMotionBlending = bFlag;
	m_fBlendWeight = 0.0f;
}

//
// 교차/충돌테스트 관련
//
//
// 레이에 교차하는 버텍스를 찾아서 리턴.
//
D3DXVECTOR3 *CModelObject::IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir )
{
	int		i;
	O3D_ELEMENT		*pElem = m_Element;
	D3DXVECTOR3	*pV;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( pElem->m_pObject3D )
		{
			if( pV = pElem->m_pObject3D->IntersecRayVertex( pOut, vRayOrig, vRayDir ) )
				return pV;		// 찾았으면 *pOut엔 좌표, 리턴포인터는 실제버텍스.
		}
		pElem ++;
	}

	return NULL;
}

//
// 레이에 교차하는 버텍스를 찾아서 리턴.
//
/*
BOOL	CModelObject::IsTouchOBB_Line( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									   const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect )
{
	int		i;
	O3D_ELEMENT		*pElem = m_Element;
	D3DXVECTOR3	*pVertex;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pVertex = pElem->m_pObject3D->FindTouchVertex( vRayOrig, vRayDir );
		if( pVertex )
			return pVertex;
		pElem ++;
	}

	return NULL;
}
*/

//BOOL CModelObject::IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist )
//{
//
//}
//BOOL	CModelObject::IntersectBB( 


void	CModelObject::GetForcePos( D3DXVECTOR3 *vOut, int nIdx, int nParts, const D3DXMATRIX &mWorld )
{
	D3DXMATRIX *pmLocal;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else
	{
		D3DXMatrixIdentity( &m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
		
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	if( nIdx == 0 )
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
	else
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce2), &m1 );
	
	D3DXVec3TransformCoord( &v1, &v1, &mWorld );
	*vOut = v1;
	
}

// 주먹 중앙의 위치를 계산할때.
void	CModelObject::GetHandPos( D3DXVECTOR3 *vOut, int nParts, const D3DXMATRIX &mWorld )
{
	D3DXMATRIX *pmLocal;
//	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

//	if( pElem->m_pObject3D == NULL )		return;
	
//	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
//	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	int nParentIdx = 0;
	if( m_pBone == NULL )	return;
	if( nParts == PARTS_RWEAPON )
	{
		nParentIdx = GetRHandIdx();
		pmLocal = &m_pBone->m_mLocalRH;
	}
	else if( nParts == PARTS_LWEAPON  )
	{
		nParentIdx = GetLHandIdx();
		pmLocal = &m_pBone->m_mLocalLH;
	}
	else
	{
		D3DXMatrixIdentity( &m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
	
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	// 애니메이션끝난 본의 매트릭스와 LocalR/LH를 곱해서 최종 매트릭스 구함.
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ nParentIdx ] );
	v1.x = m1._41;		v1.y = m1._42;		v1.z = m1._43;		// 좌표만 필요함.
	
	D3DXVec3TransformCoord( &v1, &v1, &mWorld );
	*vOut = v1;
	
}

BOOL CModelObject::GetPosBone( D3DXVECTOR3* pOut, const char* bonename )
{
	//gmpbigsun : 본이름으로 본좌표 추출 
	if( !m_pBone )
		return FALSE;

	for( int i =0; i < m_pBone->m_nMaxBone; ++i )
	{
		BONE* pUnitBone = m_pBone->GetBone( i );
		if( !pUnitBone )
		{
			assert( 0 );
			continue;
		}

		if( strcmp( bonename, pUnitBone->m_szName ) == 0 )
		{
			D3DXMATRIX matTemp;
			
			if( pUnitBone->m_pParent )
				D3DXMatrixMultiply( &matTemp, &pUnitBone->m_mLocalTM, &m_mUpdateBone[ pUnitBone->m_nParentIdx ] );		//부모의 matrix
			else 
				D3DXMatrixMultiply( &matTemp, &pUnitBone->m_mLocalTM, &m_mUpdateBone[ i ] );		
			pOut->x = matTemp._41;
			pOut->y = matTemp._42;
			pOut->z = matTemp._43;

			return TRUE;
		}
	}

	return FALSE;
	
}

BOOL CModelObject::GetPosBone( D3DXVECTOR3* pOUt, const int idx )
{
	D3DXMATRIX mat;
	BONE* pBone = m_pBone->GetBone( idx );
	if( !pBone )
		return FALSE;

	if( pBone->m_pParent )
		D3DXMatrixMultiply( &mat, &pBone->m_mLocalTM, &m_mUpdateBone[ pBone->m_nParentIdx ] );
	else 
		D3DXMatrixMultiply( &mat, &pBone->m_mLocalTM, &m_mUpdateBone[ idx ] );

	pOUt->x = mat._41;
	pOUt->y = mat._42;
	pOUt->z = mat._43;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//
//
//
D3DXVECTOR3		SplineSlerp( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3, D3DXVECTOR3 *v4, float fSlerp )
{
	D3DXVECTOR3		vResult;

	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;

	D3DXVECTOR3		m0, m1;
	const float alpha = 0.0f;

	m0 = ((1 - alpha) / 2.0f) * 
		 ((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		 ((*v3 - *v2) + *v4 - *v3);

	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
			   ((t3 - (2 * t2) + t) * m0) + 
			   ((t3 - t2) * m1) +
			   (((-2 * t3) + (3 * t2)) *
			   *v3 );

	return vResult;
}

void	CSwordForce::Add( D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	if( m_nMaxVertex >= MAX_SF_SWDFORCE )
	{
		LPCTSTR szErr = Message( "SWDForceAdd : 범위 초과 %d", m_nMaxVertex );
		ADDERRORMSG( szErr );
		return;
	}
	FVF_SWDFORCE*	pList = &m_aList[ m_nMaxVertex ];

	pList->position = v1;
	pList->color = 0xffffffff;

	pList = &m_aList2[ m_nMaxVertex ];
	pList->position = v2;
	pList->color = 0xffffffff;

	m_nMaxVertex ++;
}

// 등록된 키리스트를 스플라인 보간된 리스트로 바꾼다.
// 이렇게 만들어진 리스트를 최종 그린다.
void	CSwordForce::MakeSpline( void )
{
	FVF_SWDFORCE *pKey;
	int		i, j;
	D3DXVECTOR3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = m_nMaxVertex;

	m_nMaxSpline = 0;
	for( i = 0; i < nMaxVertex-1; i ++ )
	{
		// i ~ i+1사이를 보간한다.  
		a = i - 1;		if( a < 0 )	a = 0;
		b = i;
		c = i+1;
		d = i+2;		if( d >= nMaxVertex )	d = nMaxVertex - 1;
		for( j = 0; j < MAX_SF_SLERP+1; j ++ )
		{
			if( m_nMaxSpline >= MAX_SF_SPLINE )
			{
				LPCTSTR szErr = Message( "SWDForceAdd Spline : 범위 초과 %d", m_nMaxSpline );
				ADDERRORMSG( szErr );
				break;
			}
			pKey = m_aList;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
			pKey = m_aList2;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
		}
	}
}
/*
void	CSwordForce::Process( void )
{
	FVF_SWDFORCE *pList = m_aSpline;
	int		i;
	int		nRed = 100;

	if( m_nMaxSpline == 0 )		return;


	for( i = m_nMaxDraw+1; i >= 0; i -- )
	{
		pList[i].color = D3DCOLOR_ARGB(255,nRed,0, 0); 

		nRed -= 4; if( nRed < 0 )	nRed = 0;
	}c

}
*/
void	CSwordForce::Process( void )
{
	FVF_SWDFORCE *pList = m_aSpline;
	int		i;
	int		nRed1 =  0;
	int		nGreen1 =  0;
	int		nBlue1 =   0;
	int		nRed2 = (m_dwColor >> 16) & 0xff;
	int		nGreen2 = (m_dwColor >> 8) & 0xff;
	int		nBlue2 = m_dwColor & 0xff;
//	int		nRed2 = 120;
//	int		nGreen2 = 120;
//	int		nBlue2 = 230;
	
	if( m_nMaxSpline == 0 )		return;

	for( i = m_nMaxDraw - 1; i >= 0; i -- )
	{
		if( i % 2 )
			pList[i].color = D3DCOLOR_ARGB( 255, nRed1, nGreen1, nBlue1 ); 
		else
			pList[i].color = D3DCOLOR_ARGB( 255, nRed2, nGreen2, nBlue2 ); 
		
		nRed1   -= 3; if( nRed1   < 0 )	nRed1   = 0;
		nGreen1 -= 3; if( nGreen1 < 0 ) nGreen1 = 0;
		nBlue1  -= 3; if( nBlue1  < 0 )	nBlue1  = 0;

		nRed2   -= 3; if( nRed2   < 0 )	nRed2   = 0;
		nGreen2 -= 3; if( nGreen2 < 0 ) nGreen2 = 0;
		nBlue2  -= 3; if( nBlue2  < 0 )	nBlue2  = 0;
	}

}

//
//
//
void	CSwordForce::Draw( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{

//	if( m_nMaxSpline == 0 )	return;
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_SWDFORCE );

    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	{
		int		nShare, nRest;
		int		i;
		FVF_SWDFORCE *p = m_aSpline;
		int		nMaxDraw = m_nMaxDraw;

		nMaxDraw -= 2;
		nShare = nMaxDraw / 64;
		nRest  = nMaxDraw % 64;
		for( i = 0; i < nShare; i ++ )			// 프리미티브 갯수에 제한이 있어서 이렇게 했다.
		{
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 64, p, sizeof(FVF_SWDFORCE) );
			p += 64;
		}

		if( nRest > 0 )
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, nRest, p, sizeof(FVF_SWDFORCE) );

	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

}

/*

  SetMotionTrans(...);		// 이전에 실행하던 동작에서 새로운 동작으로 전이가 되도록 한다.



*/

