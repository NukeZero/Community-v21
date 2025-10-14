#include "stdafx.h"
#include "Region.h"
#include "ModelGlobal.h"
#include "..\_Common\ParticleMng.h"
#include "..\_Common\TailEffectMng.h"

#include "defineskill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPDIRECT3DINDEXBUFFER9 g_pIB;
LPDIRECT3DINDEXBUFFER9 g_pExIB; 
LPDIRECT3DINDEXBUFFER9 g_pGridIB;
//WORD g_anPrimitive[4]={128,32,8,2};
WORD g_anPrimitive[3]={32,8,2};
WORD g_anPrimitiveA[3]={12,4,1};
WORD g_anStartIndex[4]={0,(128+48)*3,(128+48+32+16)*3,(128+48+32+16+8+4)*3};

CCamera*   CWorld::m_pCamera = NULL;
BOOL       CWorld::m_bViewLODTerrain = TRUE;
BOOL       CWorld::m_bMiniMapRender = FALSE;
CWeather   CWorld::m_weather;
D3DCOLOR   CWorld::m_dwBgColor = D3DCOLOR_XRGB(0xe0,0xe0,0xff);;//D3DCOLOR_XRGB(50,50,100);//D3DCOLOR_XRGB(250,200,100);;//
D3DXMATRIX CWorld::m_matProj;   
D3DLIGHT9  CWorld::m_light;//Color;
D3DLIGHT9  CWorld::m_lightFogSky;

#ifdef __BEAST
FLOAT CWorld::m_fFarPlane  = 1024.0f;
FLOAT CWorld::m_fNearPlane = 0.1f;
#else
FLOAT CWorld::m_fFarPlane  = 512.0f;//768.0f; 512가 미니멈 
FLOAT CWorld::m_fNearPlane = 0.5f;
#endif



CULLSTATE CullObject( CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, D3DXPLANE* pPlaneBounds );
BOOL EdgeIntersectsFace( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, D3DXPLANE* pPlane );

float fDiv = 4.0f;

// 오래전부터 써오던 게임내 시간단위 Light color values, 이 녀석을 default로 쓴다.
static const LIGHTCOLOR gs_kLight[24] = // diffuse, ambient
{
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 0 12
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 1
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 2
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 3
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 4
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 5

	0.5f,  0.5f,  0.6f,    0.4f, 0.4f, 0.4f, // 6
	0.7f,  0.7f,  0.7f,    0.5f, 0.5f, 0.5f, // 7
	0.8f,  0.8f,  0.8f,    0.5f, 0.5f, 0.5f, // 8
	0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 9
	1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 10
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 11
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 12
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 13 1
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 14 2
	1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 15 3
	0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 16 5
	0.9f,  0.6f,  0.2f,    0.5f, 0.5f, 0.4f, // 17 6
	0.6f,  0.6f,  0.4f,    0.4f, 0.4f, 0.4f, // 18 7
	0.5f,  0.5f,  0.4f,    0.4f, 0.4f, 0.4f, // 19 8
	0.45f, 0.45f, 0.4f,    0.35f, 0.35f, 0.35f, // 20 8
	0.43f, 0.43f, 0.5f,    0.33f, 0.33f, 0.3f, // 21 9
	0.41f, 0.41f, 0.5f,    0.31f, 0.31f, 0.3f, // 22 10
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f  // 23 11
};

void CWorld::Projection( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight )
{ 
	// Frame Window와 관려된 Projection
	FLOAT fAspect = (FLOAT)nWidth / (FLOAT)nHeight;
	float fFov = D3DX_PI / fDiv;

}

#if 1
// 지형과 오브젝트만 렌더링.
void CWorld::RenderBase( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont )
{

}
#endif // 0

void CWorld::Render( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont )
{
	{
		// 기본 랜더 세팅 
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
		
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		
		// 기본 매트릭스 
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		// 출력을 와이어 프레임으로. 
		if( m_bViewWireframe)
		{
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			m_pd3dDevice->SetTexture( 0, NULL );
		}
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		// 카메라 트랜스폼 ( View 트랜스폼 ) 
		m_pCamera->Transform( pd3dDevice, this );
		// 밉맵 세팅 
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		float fBias=-0.0f;
		//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 2);
		//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS , FtoDW(fBias));
		//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS , 0 );
		
		//suntodo:
		SetLight( TRUE ); // 지형은 반드시 조명을 받게 한다.
		SetFogEnable( m_pd3dDevice, m_bViewFog );
		

		// 지형 랜더링  
		CHECK1(); 
		//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 255,0,0,0) ); 
	 #ifdef __BEAST
		RenderTerrain();
	 #else
		if( g_Option.m_nShadow < 2 ) SetStateShadowMap( m_pd3dDevice, 2, m_pCamera->m_matView );
		RenderTerrain();
		if( g_Option.m_nShadow < 2 ) ResetStateShadowMap( m_pd3dDevice, 2 );
	 #endif
		CHECK2( "  Render Terrain" );
		
		SetLight( m_bViewLight );
		
		// 그리드 랜더링
		RenderGrid();

#ifdef __CSC_RENDER_AXIS
		RenderAxis();
#endif //__CSC_RENDER_AXIS
		
		// 오브젝트 랜더링 
		RenderObject( pFont );
	}


	//RenderWater();

	// 바운드 박스 랜더링 
	RenderBoundBox();

	// 기즈모 랜더링 
//	RenderGizmo();
	RenderAxis();
	
	// 랜더 스테이트 복구 
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	SetFogEnable( pd3dDevice, FALSE );

}
//-----------------------------------------------------------------------------
// Name: ObjSortNearToFar()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------

// 가까운데서 먼거리로 소팅 (이게 빠르데요)
int ObjSortNearToFar( const VOID* arg1, const VOID* arg2 )
{
    D3DXVECTOR3 vPos1 = (*(CObj**)arg1)->GetPos();
    D3DXVECTOR3 vPos2 = (*(CObj**)arg2)->GetPos();
	D3DXVECTOR3 vDir = (*(CObj**)arg1)->GetWorld()->m_pCamera->GetPos();//m_vPos2;
    
    //FLOAT d1 = vPos1.x * vDir.x + vPos1.z * vDir.z;
    //FLOAT d2 = vPos2.x * vDir.x + vPos2.z * vDir.z;

//	FLOAT d1 =  sqrt( (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z ) );
//	FLOAT d2 =  sqrt( (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z ) );
	FLOAT d1 =  (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z );
	FLOAT d2 =  (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z );

    if (d1 > d2)
        return +1;

    return -1;
}
// 먼데서 가까운 순으로 소팅 
int ObjSortFarToNear( const VOID* arg1, const VOID* arg2 )
{
    D3DXVECTOR3 vPos1 = (*(CObj**)arg1)->GetPos();
    D3DXVECTOR3 vPos2 = (*(CObj**)arg2)->GetPos();
	D3DXVECTOR3 vDir = (*(CObj**)arg1)->GetWorld()->m_pCamera->GetPos();//m_vPos2;
    
    //FLOAT d1 = vPos1.x * vDir.x + vPos1.z * vDir.z;
    //FLOAT d2 = vPos2.x * vDir.x + vPos2.z * vDir.z;

//	FLOAT d1 =  sqrt( (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z ) );
//	FLOAT d2 =  sqrt( (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z ) );
	// 실제 거리가 필요한게 아니므로 sqrt를 씌울필요는 없을거 같습니다.
	// sqrt가 무지 느린함수거든요.
	// 문제생기면 얘기해주세요. xuzhu.
	FLOAT d1 =  (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z );
	FLOAT d2 =  (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z );

    if (d1 < d2)
        return +1;

    return -1;
}

// 먼데서 가까운 순으로 소팅 - 미리 계산해둔 거리값으로 비교하는 버전.
int ObjSortFarToNear2( const VOID* arg1, const VOID* arg2 )
{
	float	fDist1 = (*(CObj**)arg1)->m_fDistCamera;
	float	fDist2 = (*(CObj**)arg2)->m_fDistCamera;

    if (fDist1 < fDist2)
        return 1;
	
    return -1;
}

void CWorld::RenderTerrain()
{
	//m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );//m_bViewLight );

	int x, z, i, j;
	int px,pz;
	WorldPosToLand( m_pCamera->m_vPos, x, z );
	
	for( i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
		{
			int nOffset = i * m_nLandWidth + j;
			if( LandInWorld( (int)j, (int)i ) && m_apLand[ nOffset ] )
			{
				m_apLand[ nOffset ]->UpdateCull();
			}
		}
	}
	
	if( m_bViewTerrain )
	{
		if( CWorld::m_bViewLODTerrain ) 
		{
			for( i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
			{
				for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
				{
					int nOffset = i * m_nLandWidth + j;
					if( LandInWorld( (int)j, (int)i ) && m_apLand[ nOffset ] )
					{
						if(m_apLand[ nOffset ]->isVisibile())
						{
							if(j>0 && LandInWorld( (int)j-1, (int)i ) && m_apLand[ nOffset-1 ])
								for(pz=0;pz<NUM_PATCHES_PER_SIDE;pz++)
									if(m_apLand[nOffset]->m_aPatches[pz][0].isVisibile())
										if(m_apLand[nOffset]->m_aPatches[pz][0].m_nLevel<m_apLand[nOffset-1]->m_aPatches[pz][NUM_PATCHES_PER_SIDE-1].m_nLevel)
											m_apLand[nOffset]->m_aPatches[pz][0].m_nRightLevel=m_apLand[nOffset-1]->m_aPatches[pz][NUM_PATCHES_PER_SIDE-1].m_nLevel;
							if(i>0 && LandInWorld( (int)j, (int)i-1 ) && m_apLand[ nOffset-m_nLandWidth ])
								for(px=0;px<NUM_PATCHES_PER_SIDE;px++)
									if(m_apLand[nOffset]->m_aPatches[0][px].isVisibile())
										if(m_apLand[nOffset]->m_aPatches[0][px].m_nLevel<m_apLand[nOffset-m_nLandWidth]->m_aPatches[NUM_PATCHES_PER_SIDE-1][px].m_nLevel)
											m_apLand[nOffset]->m_aPatches[0][px].m_nTopLevel=m_apLand[nOffset-m_nLandWidth]->m_aPatches[NUM_PATCHES_PER_SIDE-1][px].m_nLevel;
							if(j<m_nLandWidth-1 && LandInWorld( (int)j+1, (int)i ) && m_apLand[ nOffset+1 ])
								for(pz=0;pz<NUM_PATCHES_PER_SIDE;pz++)
									if(m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE-1].isVisibile())
										if(m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE-1].m_nLevel<m_apLand[nOffset+1]->m_aPatches[pz][0].m_nLevel)
											m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE-1].m_nLeftLevel=m_apLand[nOffset+1]->m_aPatches[pz][0].m_nLevel;
							if(i<m_nLandWidth-1 && LandInWorld( (int)j, (int)i+1 ) && m_apLand[ nOffset+m_nLandWidth ])
								for(px=0;px<NUM_PATCHES_PER_SIDE;px++)
									if(m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE-1][px].isVisibile())
										if(m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE-1][px].m_nLevel<m_apLand[nOffset+m_nLandWidth]->m_aPatches[0][px].m_nLevel)
											m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE-1][px].m_nBottomLevel=m_apLand[nOffset+m_nLandWidth]->m_aPatches[0][px].m_nLevel;
						}
					}
				}
			}
		}

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//////////////

		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

		
		float fBias=-.7f;

		for( i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
		{
			for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
			{
				int nOffset = i * m_nLandWidth + j;
				if( LandInWorld( (int)j, (int)i ) && m_apLand[ nOffset ] )
				{
					if(m_apLand[ nOffset ]->isVisibile())
						m_apLand[ nOffset ]->Render( m_pd3dDevice );
				}
			}
		}
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE );
	}
}
void CWorld::RenderWater()
{
	SetLight( m_bViewLight );
	SetFogEnable( m_pd3dDevice, m_bViewFog );

	{
		int x, z, i, j;
//		int px,pz;
		WorldPosToLand( m_pCamera->m_vPos, x, z );

		for( i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
		{
			for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
			{
				int nOffset = i * m_nLandWidth + j;
				if( LandInWorld( (int)j, (int)i ) && m_apLand[ nOffset ] )
				{
					if(m_apLand[ nOffset ]->isVisibile())
					{
						if( m_bViewWater )
						{
							m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bViewLight );
							m_apLand[ nOffset ]->RenderWater( m_pd3dDevice);
						}
						if( m_bViewHeightAttribute  )
						{
							m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
							m_apLand[ nOffset ]->RenderHeightAttribute( m_pd3dDevice );
						}
					}						
				}
			}
		}
	}
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bViewLight );
}


//////////////////////////////////////////////////////////////////
// 오브젝트 랜더링 
//
// 출력 순서
//
// 1.오브젝트 출력 
// 2.물 출력 (오브젝트가 먼저 있어야 오브젝트가 물속에 비치게 할 수 있다.
// 3.이펙트 출력 ( 이펙트를 맨 마지막에 출력해야, 오브젝트와 물이 배경으로 보일 수 있다. )
// 4.반투명하게 사라지는 오브젝트를 맨 마지막에 출력한다.(반투명 오브젝트는 물 이후에 출력하기 때문에 물 속에 비치게 할 수 없다.)
//
void CWorld::RenderObject( CD3DFont* pFont )
{
	int i = 0, nObjRegion = 0;
	static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
	
//#ifdef __NEWPROCESS
	// 화면 출력용 포인트 삭제 
	for( i = 0; i < m_nObjCullSize; i++ )
		if( m_aobjCull[ i ] )
			m_aobjCull[ i ]->m_ppViewPtr = NULL;
	for( i = 0; i < m_nSfxCullSize; i++ )
		if( m_asfxCull[ i ] )
			m_asfxCull[ i ]->m_ppViewPtr = NULL;	
//#endif
	m_nObjCullSize = 0;
	m_nSfxCullSize = 0;

	

	if( m_bViewAllObjects )
	{
		CHECK1();
		// 컬링을 하자. 화면에 보이는 놈들만 추려내기
		// 추려내는 것과 동시에 컬링 플렉도 세팅하자.
		CObj* pObj;
		CLandscape* pLand;// m_nVisibilityLand
		// 정적 오브젝트 컬링 및 컬렉션 
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bViewLight );
		int _nCount = 0, _nCount2 = 0;

		//FOR_LINKMAP( m_pCamera->m_vPos, pObj, MAP_SIZE  / 4  )
		FOR_LAND( this, pLand, m_nVisibilityLand, FALSE )
		{
			FOR_OBJARRAY( pLand, pObj )
			{
#ifdef __BEAST
				//sun here
				if( m_bViewObj == FALSE && pObj->GetType() == OT_OBJ ) continue;
				if( m_bViewMover == FALSE && pObj->GetType() == OT_MOVER ) continue;
				if( m_bViewItem == FALSE && pObj->GetType() == OT_ITEM ) continue;
				if( m_bViewRegion == FALSE && pObj->GetType() == OT_REGION ) continue;
				if( m_bViewSpawn == FALSE && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex ) continue;
				if( !IsVecInVisibleLand( pObj->GetPos(), m_pCamera->m_vPos, m_nVisibilityLand ) ) //&& pObj->GetIndex() != 8 )
					continue;
#endif
				// 거리를 계산하고, 거리에 따라 출력할 것과 안할 것을 구분한다.
				D3DXVECTOR3 *pv, *pvCamera;
				float xDist, yDist, zDist;
				pvCamera = &(m_pCamera->GetPos());//;m_vPos2);	// 카메라 좌표
				pv = &(pObj->GetPos());	// CObj좌표
				xDist = (pv->x - pvCamera->x);
				yDist = (pv->y - pvCamera->y);
				zDist = (pv->z - pvCamera->z);
#ifdef _DEBUG
				if( pObj->m_dwIndex == 88 )
				{
					int a = 0;
				}
#endif

	#ifdef __YRENDER_FLAG
				LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( pObj->m_dwType, pObj->m_dwIndex );
				
				if( lpModelElem && g_Option.m_bSFXRenderOff && lpModelElem->m_bRenderFlag != 1 )
					continue;
	#endif //__YRENDER_FLAG		
				
				
				pObj->m_fDistCamera = sqrt( xDist * xDist + yDist * yDist + zDist * zDist );
				static FLOAT fDistant[3][4] = { 
					{ 400, 200, 70, 400 },
					{ 250, 150, 60, 400 },
					{ 150, 100, 40, 400 } };
				if( m_bViewLODObj )
				{
#ifdef __BEAST
 
					int nDistant = pObj->GetModel()->m_pModelElem->m_dwDistant;

					if( ( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->IsRespawn() )
						|| ( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsRespawn() ) )
						nDistant = 0;
					if( pObj->m_fDistCamera > fDistant[ g_Option.m_nObjectDistant ][ nDistant ] ) 
					{
						continue;
					}
#else
					if( m_bMiniMapRender == FALSE && pObj->m_fDistCamera > fDistant[ g_Option.m_nObjectDistant ][ pObj->GetModel()->m_pModelElem->m_dwDistant ] ) 
					{
#ifdef _DEBUG
						if( pObj->m_dwIndex == 88 )
						{
							int a = 0;
						}
#endif
//						pObj->m_wBlendFactor = 0;
						continue;
					}
#endif
				}
				else
					pObj->m_fDistCamera = 0;
				//pObj->m_fDistCamera = sqrt( xDist * xDist + yDist * yDist + zDist * zDist ) + 200;

//				vCam2Obj = pObj->GetPos() - m_pCamera->m_vPos;	// 카메라에서 오브젝트쪽의 벡터
//				D3DXVec3Normalize( &vCam2Obj, &vCam2Obj );
//				FLOAT fDot = D3DXVec3Dot( &vCamDir, &vCam2Obj );
//				if( fDot <= 0 )	// 카메라시선과 오브젝쪽의 벡터의 각도차가 90도보다 적을때만 컬링 테스트
//		  			continue;
					
//				_nCount2 ++;
				if( pLand->isVisibile() )
				//if( IsVecInVisibleLand( pObj->GetPos(), m_pCamera->m_vPos, m_nVisibilityLand ) && pObj->GetIndex() > 8 )
//				if( pObj->GetType() != OT_OBJ || ( pObj->GetType() == OT_OBJ && pObj->GetIndex() > 15 ) )
//#endif
				{
					if( !( m_bMiniMapRender == TRUE && pObj->GetType() != OT_OBJ ) )
					{
						// 행렬과 바운드 박스 업데이트 
						if( pObj->IsUpdateMatrix() )
							pObj->UpdateMatrix();
						// cull 상태 초기화 
						pObj->SetCullState( CS_UNKNOWN );

						if( m_nObjCullSize < MAX_DISPLAYOBJ )
						{
							{
								if( pObj->m_pModel!=NULL && pObj->m_pModel->GetModelType() != MODELTYPE_SFX ) // kentauji edited
//								if( pObj->m_pModel->GetModelType() != MODELTYPE_ANIMATED_BILLBOARD )
								{
									if( m_bCullObj )
									{
										_nCount ++;
										pObj->m_cullstate 
											= CullObject( &g_cullinfo, pObj->m_vecBoundsWorld, pObj->m_planeBoundsWorld );

										if( pObj->IsCull() == FALSE )
											m_aobjCull[ m_nObjCullSize++ ] = pObj;
									}
									else
										m_aobjCull[ m_nObjCullSize++ ] = pObj;
								}
								else
								{
									if( m_bCullObj )
									{
										_nCount ++;
										pObj->m_cullstate 
											= CullObject( &g_cullinfo, pObj->m_vecBoundsWorld, pObj->m_planeBoundsWorld );

										if( pObj->IsCull() == FALSE )
										{
											m_asfxCull[ m_nSfxCullSize++ ] = pObj;
										}
									}
									else
									{
										m_asfxCull[ m_nSfxCullSize++ ] = pObj;
									}
								}
							} // dot
						}
						else
							TRACE("한 화면에 출력 오브젝트가 %d를 넘었다. 위험!!!! \n", m_nObjCullSize);
					}
				}
			}
			END_OBJARRAY
		}
		END_LAND
		//END_LINKMAP
		CHECK2( "Assert Obj" );

		// 모두 추려냈으면 추려낸 것을 소팅하자. 
		CHECK1();
	    qsort( m_aobjCull, m_nObjCullSize, sizeof(CObj*), ObjSortFarToNear2 ); 
//#ifdef __NEWPROCESS
		for( i = 0; i < m_nObjCullSize; i++ )
			m_aobjCull[ i ]->m_ppViewPtr = &m_aobjCull[ i ];	
//#endif
		CHECK2("Sort");
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		SetLight( m_bViewLight );
		::SetLight( m_bViewLight );
		::SetFog( m_bViewFog );

		// 이제 오브젝트 출력 
		CHECK1();
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		{
			//static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
			//SetFogEnable( m_pd3dDevice, FALSE );
			::SetTransformView( m_pCamera->m_matView );		// CModelObject::Render()를 부르기전에 이것을 호출해주자. 내부에서 pd3dDevice->GetTransform()을 안하기 위해서다.
			::SetTransformProj( m_matProj );

			SetDiffuse( m_light.Diffuse.r, m_light.Diffuse.g, m_light.Diffuse.b );
			SetAmbient( m_light.Ambient.r+0.4f, m_light.Ambient.g+0.4f, m_light.Ambient.b+0.4f );

			SetLightVec( m_light.Direction );
			if( m_bViewFog )
			{
				m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_bViewFog );
				m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
				m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
				m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
			}				
			g_nMaxTri = 0;
			// OT_OBJ인것과 아닌것을 구분해서 찍자.
			// OT_OBJ가 아닌것들.
			for( i = 0; i < m_nObjCullSize; i++)
			{
				pObj = m_aobjCull[ i ];

				if( m_bViewFog )
				{
					//m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_bViewFog );
					//m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
					//m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
					//m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
					if( pObj->m_pModel->m_bSkin )	// 스키닝인것
					{
//						m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
						
						ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
						if( m_bIsContinent && pInfo && pInfo->_bUseEnvir )		//gmpbigsun : 대륙정보 우선
							vConst.w = ( pInfo->_fFogEnd - pObj->m_fDistCamera) / ( pInfo->_fFogEnd - pInfo->_fFogStart );
						else
							vConst.w = (m_fFogEndValue - pObj->m_fDistCamera) / (m_fFogEndValue - m_fFogStartValue);
						m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
						if( 0 )//pObj->GetType() == OT_MOVER )	// 스키닝 캐릭터
						{
							SetDiffuse( 0, 0, 0 );
							SetAmbient( m_light.Ambient.r * 2.0f, m_light.Ambient.g * 2.0f, m_light.Ambient.b * 2.0f );
//							SetAmbient( 1.0f, 1.0f, 1.0f );
						} else
						{	// 스키닝되는 배경
							SetLightVec( m_light.Direction );
						}
					} else
					{	// 스키닝이 아닌것
//						m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_bViewFog );
//						m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
//						m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
//						m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
						SetAmbient( m_light.Ambient.r+0.3f, m_light.Ambient.g+0.3f, m_light.Ambient.b+0.3f );						
					}
				}
				
				{
					pObj->Render( m_pd3dDevice );
				}

				// 오브젝트 이름은 마지막에 찍는쪽으로 옮김.
				if( m_bViewBoundBox )
				{
					SetBoundBoxVertex( pObj );
					RenderBoundBoxVertex( pObj );
				}
			}
			// OT_OBJ인것

		}
#if 0
		{
			D3DXVECTOR3 pTri[54];
			int nMax = GetLandTris( g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pTri ) / 3;
			D3DXMATRIX m;
			D3DXMatrixIdentity( &m );
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
			for( i = 0; i < nMax; i ++ )
			{
				g_Grp3D.Render3DTri( &pTri[i*3], COL_RED, TRUE );
			}
		}
#endif
		CHECK2( "  Render Obj" );
	}
	CHECK1();	// rener water
	RenderWater();
	CHECK2( "  Render Water" );

	
	// 수면 막 처리 (카메라가 물 밑으로 들어가면 화면 전면에 물텍스춰를 그려 물속에 들어간 것 처럼 보이게 하기)
	D3DXMATRIX matWorld;
	D3DXMATRIX mat, matView;
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	if( m_pCamera ) 
	{
		LPWATERHEIGHT lpWaterHeight = GetWaterHeight( (int)( m_pCamera->m_vPos.x ), (int)( m_pCamera->m_vPos.z ) );
		if( lpWaterHeight && m_pCamera->m_vPos.y < lpWaterHeight->byWaterHeight && 
	#ifdef __N_WATERFRAME_050202
			( lpWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER )
	#else
			lpWaterHeight->byWaterTexture == WTYPE_WATER )
	#endif//__N_WATERFRAME_050202
		{
			D3DXMatrixLookAtLH( &mat, &D3DXVECTOR3(0.0f,0.0f,-1.0f), &D3DXVECTOR3(0.0f,0.0f,1.0f), &D3DXVECTOR3(0.0f,1.0f,0.0f) );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &mat );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			//m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x7fffffff );
			m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 20 )->m_pTexture );
			m_pd3dDevice->SetFVF ( D3DFVF_D3DSFXVERTEX );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			D3DSFXVERTEX pVertices[4];
//			D3DSFXVERTEX* pVertices = new D3DSFXVERTEX[ 4 ];
			pVertices[ 0 ].p = D3DXVECTOR3(-2.0f,-2.0f,1.0f);
			pVertices[ 0 ].tu1 = .0f;
			pVertices[ 0 ].tv1 = .0f;
			pVertices[ 1 ].p = D3DXVECTOR3(-2.0f,2.0f,1.0f);
			pVertices[ 1 ].tu1 = .0f; 
			pVertices[ 1 ].tv1 = .0f;
			pVertices[ 2 ].p = D3DXVECTOR3(2.0f,2.0f,1.0f);
			pVertices[ 2 ].tu1 = .0f; 
			pVertices[ 2 ].tv1 = .0f;
			pVertices[ 3 ].p = D3DXVECTOR3(2.0f,-2.0f,1.0f);
			pVertices[ 3 ].tu1 = .0f; 
			pVertices[ 2 ].tv1 = .0f;
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, pVertices, sizeof( D3DSFXVERTEX ) );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
//			delete pVertices;
		}
	}
	CHECK1();
	qsort( m_asfxCull, m_nSfxCullSize, sizeof(CObj*), ObjSortFarToNear );
//#ifdef __NEWPROCESS
	for( i = 0; i < m_nSfxCullSize; i++ )
		m_asfxCull[ i ]->m_ppViewPtr = &m_asfxCull[ i ];	
//#endif
	//SetFogEnable( m_pd3dDevice, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );////_bViewLight );
	CObj* pObj;
	//
	// 3.이펙트 출력 
	// 
	for( i = 0; i < m_nSfxCullSize; i++)
	{
		pObj = m_asfxCull[ i ]; 
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );//m_bViewLight );
		pObj->Render( m_pd3dDevice );
	}

//	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );////_bViewLight );
	//
	// 5. 이름은 맨 마지막에 찍음 
	// 
	for( i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[ i ];
		if( pObj && m_bViewName && pFont )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) pObj;


#ifdef __BEAST
				pMover->RenderName( m_pd3dDevice, pFont );
#else
				DWORD dwColor = 0xffffffff;
#ifdef __SCHOOL0722
				if( pObj->m_fDistCamera < 100 ) // 이벤섭에선 더 멀리 보인다.
#else
				if( pObj->m_fDistCamera < 40 ) // && /*((CMover*)pObj)->IsPlayer() && pObj != pObj->GetActiveObj() ) */|| CObj::m_pObjHighlight == pObj )
#endif
				{
					CMover* pMover = (CMover*) pObj;
					if( pMover->IsMode( TRANSPARENT_MODE ) == 0 )		// 투명상태가 아닐때만 렌더.
					{
						// 나와 상대가 게임 마스터이거나 그보다 높은 신분이라면?
						if( pMover->IsAuthHigher( AUTH_GAMEMASTER ) && CMover::GetActiveMover()->IsAuthHigher( AUTH_GAMEMASTER ) ) //> IsAuthHigher( AUTH_GAMEMASTER ) )
						{
							// 높은 레벨의 신분은 낮은 레벨의 신분을 구별할 수 있지만 낮은 신분은 높은 신분을 볼 수 없다.
							if( pMover->m_dwAuthorization <= CMover::GetActiveMover()->m_dwAuthorization )
							{
								if( pMover->IsAuthorization( AUTH_GAMEMASTER    ) ) dwColor = 0xffffff90; else
									if( pMover->IsAuthorization( AUTH_OPERATOR      ) ) dwColor = 0xff90ff90; else
										if( pMover->IsAuthorization( AUTH_ADMINISTRATOR ) ) dwColor = 0xff9090ff;						
							}
						}
						else
							// 일반 유저는 게임 마스터일 경우만 노란색으로.
							if( pMover->IsAuthorization( AUTH_GAMEMASTER ) ) //> IsAuthHigher( AUTH_GAMEMASTER ) )
								dwColor = 0xffffff00; // 노란색 
							pMover->RenderName( m_pd3dDevice, pFont, dwColor );
							
					}
				}
#endif // else BEAST
			}
		}
	}
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_bViewFog );
	CHECK2("  Render Effect");

/*
	{
		D3DXMatrixLookAtLH( &mat, &D3DXVECTOR3(0.0f,0.0f,-1.0f), &D3DXVECTOR3(0.0f,0.0f,1.0f), &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &mat );
		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		//m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x7fffffff );
		//m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 20 )->m_pTexture );
		m_pd3dDevice->SetFVF ( D3DFVF_3DVERTEX );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		
		FVF_3DVERTEX* pVertices = new FVF_3DVERTEX[ 4 ];
		pVertices[ 0 ].vPos = D3DXVECTOR3(-2.0f,-2.0f,1.0f);
		pVertices[ 0 ].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
		pVertices[ 1 ].vPos = D3DXVECTOR3(-2.0f,2.0f,1.0f);
		pVertices[ 1 ].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
		pVertices[ 2 ].vPos = D3DXVECTOR3(2.0f,2.0f,1.0f);
		pVertices[ 2 ].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
		pVertices[ 3 ].vPos = D3DXVECTOR3(2.0f,-2.0f,1.0f);
		pVertices[ 3 ].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, pVertices, sizeof( FVF_3DVERTEX ) );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		delete pVertices;
	}
*/	
	
}
void	_DrawRect( LPDIRECT3DDEVICE9 pd3dDevice, int x, int y, int w, int h, DWORD dwColor )
{
	#define D3DFVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
	
	struct FVF_2DVERTEX
	{
		D3DXVECTOR4 vPos;	// The 3D position for the vertex
		DWORD		dwColor;		// The vertex color
	};
	FVF_2DVERTEX	m_aVertex[ 8 ];

	for( int i = 0; i < 8; i ++ )
	{
		m_aVertex[i].vPos    = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
		m_aVertex[i].dwColor = 0xffffffff;
	}
	

	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = (float)x;
	aVertex[0].vPos.y = (float)y;
	aVertex[0].vPos.z = 0.0f;
	aVertex[0].vPos.w = 1.0f;
	aVertex[0].dwColor = dwColor;
	
	aVertex[1].vPos.x = (float)(x + (w - 1));
	aVertex[1].vPos.y = (float)y;
	aVertex[1].vPos.z = 0.0f;
	aVertex[1].vPos.w = 1.0f;
	aVertex[1].dwColor = dwColor;
	
	aVertex[2].vPos.x = (float)(x + (w - 1));
	aVertex[2].vPos.y = (float)(y + (h - 1));
	aVertex[2].vPos.z = 0.0f;
	aVertex[2].vPos.w = 1.0f;
	aVertex[2].dwColor = dwColor;
	
	aVertex[3].vPos.x = (float)x;
	aVertex[3].vPos.y = (float)(y + (h - 1));
	aVertex[3].vPos.z = 0.0f;
	aVertex[3].vPos.w = 1.0f;
	aVertex[3].dwColor = dwColor;
	
	aVertex[4].vPos.x = (float)x;
	aVertex[4].vPos.y = (float)y;
	aVertex[4].vPos.z = 0.0f;
	aVertex[4].vPos.w = 1.0f;
	aVertex[4].dwColor = dwColor;
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, aVertex, sizeof(FVF_2DVERTEX) );
	
	
}


void CWorld::RenderBoundBoxVertex( CObj* pObj )
{
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vPos = pObj->GetPos();
	D3DXVECTOR3 vScale = pObj->GetScale();

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixScaling( &mat, vScale.x, vScale.y, vScale.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	FLOAT fTheta = D3DXToRadian( -pObj->GetAngle() );
	D3DXMatrixRotationY( &mat, fTheta );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	D3DXMatrixTranslation( &mat, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetTexture( 0, NULL);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF ( D3DFVF_BOUNDBOXVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pBoundBoxVertexBuffer, 0, sizeof(BOUNDBOXVERTEX) );
	m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_nBoundBoxVertexNum / 2 );
}
void CWorld::SetBoundBoxVertex( CObj* pObj )
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	const BOUND_BOX* pBB;
	CModel* pModel = pObj->m_pModel;
	pBB = pModel->GetBBVector();
	/*
	switch( pModel->GetModelType() )
	{
		case MODELTYPE_BILLBOARD:
			return;
		case MODELTYPE_STATIC_MESH: 
			pBB = ((CModelObject*)pModel)->GetBBVector(); 
			break;
		case MODELTYPE_BIPED_MESH: 
			pBB = ((CModelObject*)pModel)->GetBBVector(); 
			break;
	}
	*/
	D3DXVECTOR3 vBoundBox;
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5

	//gmpbigsun
	DWORD dwColor = 0xffffffff;
	if( pObj->m_bSnaped )
		dwColor = 0xffff0000;

	BOUNDBOXVERTEX* pVertices;
	m_pBoundBoxVertexBuffer->Lock( 0, m_nBoundBoxVertexNum * sizeof(BOUNDBOXVERTEX), (void**)&pVertices, 0 );

	// 0
	vBoundBox = pBB->m_vPos[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 1 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 1
	vBoundBox = pBB->m_vPos[ 1 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 2 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 2
	vBoundBox = pBB->m_vPos[ 2 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 3 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 3
	vBoundBox = pBB->m_vPos[ 3 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 0 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;

	// 4
	vBoundBox = pBB->m_vPos[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 4 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 5
	vBoundBox = pBB->m_vPos[ 1 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 5 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 6
	vBoundBox = pBB->m_vPos[ 3 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 7 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 7
	vBoundBox = pBB->m_vPos[ 2 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 6 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;

	// 8
	vBoundBox = pBB->m_vPos[ 4 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 5 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 9
	vBoundBox = pBB->m_vPos[ 5 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 6 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 10
	vBoundBox = pBB->m_vPos[ 6 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 7 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	// 11
	vBoundBox = pBB->m_vPos[ 7 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;
	vBoundBox = pBB->m_vPos[ 4 ];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox                                  ; pVertices->color = dwColor;	pVertices++;

	m_pBoundBoxVertexBuffer->Unlock();
}
BOOL CWorld::CheckBound(D3DXVECTOR3* vPos,D3DXVECTOR3* vDest,D3DXVECTOR3* vOut, FLOAT* fLength )
{
	D3DXVECTOR3 tempVec=(*(vPos)-*(vDest));// /10.0f;
	float length=D3DXVec3Length(&tempVec);
	D3DXVec3Normalize(&tempVec,&tempVec);
	tempVec/=10;
	D3DXVECTOR3 tempVec2=*(vDest);
	D3DXVECTOR3 vDist1, vDist2;
	float tempheight;
	BOOL	b1, b2;
	b1 = FALSE;	b2 = FALSE;
	tempVec2 += (tempVec * 10);		// 카메라가 자꾸 지형에 껴들어가서 좀 뺐다.
	
#ifdef __YCAMERA_EX
	BOOL  bWaterChkDn = FALSE;
	BOOL  bWaterChkUp = FALSE;
	FLOAT fWaterHeight = 0.0f;
	LPWATERHEIGHT pWaterHeight = GetWaterHeight( *vDest );
	if( pWaterHeight && 
#ifdef __N_WATERFRAME_050202
		( pWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER )
#else
		pWaterHeight->byWaterTexture == WTYPE_WATER )
#endif//__N_WATERFRAME_050202
	{
		fWaterHeight = (FLOAT)pWaterHeight->byWaterHeight;
		//vDest->y += 0.5f;
		
		if( fWaterHeight < vDest->y )
		{
			bWaterChkDn = TRUE;
		}
		else
		{
			bWaterChkUp = TRUE;
		}
	}					
#endif //__YCAMERA_EX


	for( int i = 0; i < int( length * 10 ); i++ ) 
	{
		tempVec2+=tempVec;
		tempheight=GetLandHeight( tempVec2.x, tempVec2.z );
		if(tempVec2.y<tempheight+0.3f)
		{
			b1 = TRUE;
			vDist1 = tempVec2 - *vDest;		// 사람에서 교차점까지의 방향벡터
//			*(vOut)=tempVec2;
//			vOut->y+=1.1f;
//			return TRUE;
			break;
		}
#ifdef __YCAMERA_EX
		// 케릭터가 물위에 있고, 잠기지 않았다면 수면 충돌체크하여 카메라가 물속으로 안들어가게 처리함.
		D3DXVECTOR3 vWaterVec = tempVec2;
		//vWaterVec.y+=0.5f;
		if( bWaterChkDn )
		{
			if(vWaterVec.y<fWaterHeight+0.3f)
			{
				b1 = TRUE;
				vDist1 = vWaterVec - *vDest;	
				break;
			}
		}
		else
		// 케릭터가 물위에 있고, 잠겼다면 수면 충돌체크하여 카메라가 물밖으로 못나가게 처리함
		if( bWaterChkUp )
		{
			if(vWaterVec.y>fWaterHeight-0.3f)
			{
				b1 = TRUE;
				vDist1 = vWaterVec - *vDest;	
				break;
			}
		}
#endif //__YCAMERA_EX

	}
	D3DXVECTOR3 vIntersect;
	BOOL bRet = IntersectObjLine2( &vIntersect, *vDest, *vPos, TRUE );
	if( bRet )
	{
		b2 = TRUE;
		vDist2 = vIntersect - *vDest;		// 사람에서 교차점까지의 방향벡터
//		*vOut = vIntersect;
	}
	FLOAT	fDist1, fDist2;
	if( b1 == TRUE && b2 == TRUE )	// 지형이랑 오브젝트 모두 교차했다
	{
		fDist1 = D3DXVec3LengthSq( &vDist1 );
		fDist2 = D3DXVec3LengthSq( &vDist2 );
		if( fDist1 < fDist2 )		// 그중 가까운놈으로 씀
		{
			*fLength = D3DXVec3Length( &vDist1 );
			*vOut = tempVec2;
		}
		else
		{
			*fLength = D3DXVec3Length( &vDist2 );
			*vOut = vIntersect;
		}
		return TRUE;
	} else
	if( b1 )	// 지형에만 부딪혔다.
	{
		*fLength = D3DXVec3Length( &vDist1 );		
		*vOut = tempVec2;
		return TRUE;
	} else
	if( b2 )	// 오브젝트에만 부딪혔다
	{
		*fLength = D3DXVec3Length( &vDist2 );		
		*vOut = vIntersect;
		return TRUE;
	}

	// 아무데도 부딪히지 않았다.
	*fLength = length;
	return FALSE;
}
void CWorld::SetLight( BOOL bLight )
{
	//ACE("SetLight %d \n", bLight);
	CLight* pLight;

	D3DXVECTOR3 vPos = ( CObj::GetActiveObj() != NULL ? CObj::GetActiveObj()->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );

	pLight = GetLight( "direction" );

	//gmpbigsun: 대륙안에 들어갔을때는 대륙정보를 이용한다.
	BOOL bUseContEnvir = FALSE;
	if( m_bIsContinent )
	{
		ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
		if( pInfo )
			bUseContEnvir = pInfo->_bUseEnvir;
	}

	if( bUseContEnvir )
	{
		if( pLight )
		{
			// 만약 처음 진입이라면 Lerp process를 탄다.

			// 대륙정보 대로!!
			ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
			if( pInfo )
			{
				pLight->Ambient.r = pInfo->_fMulAmbient[ 0 ];
				pLight->Ambient.g = pInfo->_fMulAmbient[ 1 ];
				pLight->Ambient.b = pInfo->_fMulAmbient[ 2 ];

				pLight->Specular.r = 1.0f;
				pLight->Specular.g = 1.0f;
				pLight->Specular.b = 1.0f;

				pLight->Diffuse.r = pInfo->_fMulDiffuse[ 0 ];
				pLight->Diffuse.g = pInfo->_fMulDiffuse[ 1 ];
				pLight->Diffuse.b = pInfo->_fMulDiffuse[ 2 ];

				UpdateLerpEV( pLight );
				memcpy( &m_lightFogSky, pLight, sizeof( m_lightFogSky ) );
			}

			pLight->Ambient.r  += 0.0f;
			pLight->Ambient.g  += 0.0f;
			pLight->Ambient.b  += 0.0f;
			// 변화 없음 
			pLight->Specular.r = 2.0f;
			pLight->Specular.g = 2.0f;
			pLight->Specular.b = 2.0f;
			// 주변 
			pLight->Diffuse.r  += 0.1f;
			pLight->Diffuse.g  += 0.1f;
			pLight->Diffuse.b  += 0.1f;
			
			//gmpbigsun : 최종 Light 
			UpdateLerpEV( pLight );

			memcpy( &m_light, pLight, sizeof( m_light ) );
			
			D3DXVECTOR3 vecSun = D3DXVECTOR3( 0.0f, 0.5f,0.5f);
			D3DXVec3Normalize(&(vecSun),&(vecSun));
			pLight->SetDir( -vecSun.x, -vecSun.y, -vecSun.z ); 
			pLight->Appear( m_pd3dDevice, TRUE );
		}
	}
	else
	if( m_bIsIndoor )
	{
		if( pLight )
		{		
			// 음영 변화 
			pLight->Diffuse.r = ((m_dwDiffuse>>16) & 0xff) / 255.f;
			pLight->Diffuse.g = ((m_dwDiffuse>>8) & 0xff)  / 255.f;
			pLight->Diffuse.b = ((m_dwDiffuse) & 0xff)     / 255.f;

			// 변화 없음 
			pLight->Specular.r = 1.0f;
			pLight->Specular.g = 1.0f;
			pLight->Specular.b = 1.0f;
			// 전체 변화 
			pLight->Ambient.r  = ((m_dwAmbient>>16) & 0xff) / 255.f;
			pLight->Ambient.g  = ((m_dwAmbient>>8) & 0xff)  / 255.f;
			pLight->Ambient.b  = ((m_dwAmbient) & 0xff)     / 255.f;

			UpdateLerpEV( pLight );

			memcpy( &m_lightFogSky, pLight, sizeof( m_lightFogSky ) );

			pLight->Diffuse.r  += 0.1f;
			pLight->Diffuse.g  += 0.1f;
			pLight->Diffuse.b  += 0.1f;
			// 변화 없음 
			pLight->Specular.r = 2.0f;
			pLight->Specular.g = 2.0f;
			pLight->Specular.b = 2.0f;
			// 주변 
			pLight->Ambient.r  += 0.0f;
			pLight->Ambient.g  += 0.0f;
			pLight->Ambient.b  += 0.0f;
			
			//gmpbigsun : 최종 Light 
			UpdateLerpEV( pLight );

			memcpy( &m_light, pLight, sizeof( m_light ) );
			
			D3DXVECTOR3 vecSun = D3DXVECTOR3( 0.0f, 0.5f,0.5f);
			D3DXVec3Normalize(&(vecSun),&(vecSun));
			pLight->SetDir( -vecSun.x, -vecSun.y, -vecSun.z ); 
			pLight->Appear( m_pd3dDevice, TRUE );
		}
	}
	else
	{
		if( pLight )
		{
			int nHour = 8, nMin = 0;
			// 비스트는 시간을 m_nLightHour에서 가져온다.
			if( m_nLightType == 1 )
				nHour = m_nLightHour;

			nHour--;
			if( nHour < 0 ) nHour = 0;
			if( nHour > 23 ) nHour = 23;

			//if( m_bFixedHour )
			//	nHour = m_nFixedHour, nMin = 0;
			LIGHTCOLOR lightColorPrv = gs_kLight[ ( nHour - 1 == -1 ) ? 23 : nHour - 1 ];
			LIGHTCOLOR lightColor = gs_kLight[ nHour ];

			//m_lightColor = lightColorPrv;
			lightColorPrv.r1 += ( lightColor.r1 - lightColorPrv.r1) * nMin / 60;
			lightColorPrv.g1 += ( lightColor.g1 - lightColorPrv.g1) * nMin / 60;
			lightColorPrv.b1 += ( lightColor.b1 - lightColorPrv.b1) * nMin / 60;
			lightColorPrv.r2 += ( lightColor.r2 - lightColorPrv.r2) * nMin / 60;
			lightColorPrv.g2 += ( lightColor.g2 - lightColorPrv.g2) * nMin / 60;
			lightColorPrv.b2 += ( lightColor.b2 - lightColorPrv.b2) * nMin / 60;
			// 60(minMax) : 15(curMin) = 0.5(colorDistant) : x(curCol)

			// 음영 변화 
			pLight->Diffuse.r  = lightColorPrv.r1;
			pLight->Diffuse.g  = lightColorPrv.g1;
			pLight->Diffuse.b  = lightColorPrv.b1;
			// 변화 없음 
			pLight->Specular.r = 1.0f;
			pLight->Specular.g = 1.0f;
			pLight->Specular.b = 1.0f;
			// 전체 변화 
			pLight->Ambient.r  = lightColorPrv.r2;
			pLight->Ambient.g  = lightColorPrv.g2;
			pLight->Ambient.b  = lightColorPrv.b2;

			memcpy( &m_lightFogSky, pLight, sizeof( m_lightFogSky ) );

			pLight->Diffuse.r  += 0.1f;
			pLight->Diffuse.g  += 0.1f;
			pLight->Diffuse.b  += 0.1f;
			// 변화 없음 
			pLight->Specular.r = 2.0f;
			pLight->Specular.g = 2.0f;
			pLight->Specular.b = 2.0f;
			// 주변 
			pLight->Ambient.r  += 0.0f;
			pLight->Ambient.g  += 0.0f;
			pLight->Ambient.b  += 0.0f;

			memcpy( &m_light, pLight, sizeof( m_light ) );
			
	#ifdef __BEAST
			if( m_nLightType == 0 ) // direct 
			{
				pLight->Diffuse.r = m_lightDirect.r1;
				pLight->Diffuse.g = m_lightDirect.g1;
				pLight->Diffuse.b = m_lightDirect.b1;
				pLight->Ambient.r = m_lightDirect.r2;
				pLight->Ambient.g = m_lightDirect.g2;
				pLight->Ambient.b = m_lightDirect.b2;
			
				memcpy( &m_light, pLight, sizeof( m_light ) );
			}


			D3DXVECTOR3 vecSun = D3DXVECTOR3( 0.0f, 0.5f, 0.5f);
			D3DXVec3Normalize(&(vecSun),&(vecSun));
			pLight->SetDir( -vecSun.x, -vecSun.y, -vecSun.z ); 
			pLight->Appear( m_pd3dDevice, TRUE );
	#else
			D3DXVECTOR3 vecSun=D3DXVECTOR3( 0.0f, 0.0f,1.0f);
			D3DXMATRIX  matTemp;
			D3DXMatrixRotationX( &matTemp,(m_skyBox.m_fSunAngle +180)*3.1415926f/180.0f);
			D3DXVec3TransformCoord(&vecSun,&vecSun,&matTemp);
			pLight->SetDir( vecSun.x, vecSun.y, vecSun.z ); 

			vecSun = D3DXVECTOR3( 0.0f, 0.5f,0.5f);
			D3DXVec3Normalize(&(vecSun),&(vecSun));
			pLight->SetDir( -vecSun.x, -vecSun.y, -vecSun.z ); 
			pLight->Appear( m_pd3dDevice, TRUE );
	#endif
		}
	}

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, bLight );//m_bViewLight );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0,0,0,0) ); //m_dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );
	::SetLight( bLight );

	// 기본 패터리얼 정의 
	m_pd3dDevice->SetMaterial( &m_baseMaterial );


}
HRESULT CWorld::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
//	int i,j;
	m_pd3dDevice = pd3dDevice;
//	m_skyBox.InitDeviceObjects( pd3dDevice );
#ifdef __BEAST
	m_meshGizmo.InitDeviceObjects( pd3dDevice );
	m_meshGizmo.LoadModel( "gizmo.o3d" );
	m_meshGizmo.RestoreDeviceObjects();
#endif

	if( prj.m_terrainMng.GetTerrain( 10 )->m_pTexture == NULL )
		prj.m_terrainMng.LoadTexture( 10 );
//	if( prj.m_terrainMng.GetTerrain( 11 )->m_pTexture == NULL )
//		prj.m_terrainMng.LoadTexture( 11 );
 
//#ifdef __YWATER_ANI_EX
//	for( int i = 0; i < prj.m_terrainMng.m_nWaterFrame ; i++ )
//	{
//		for (int j = 0; j < prj.m_terrainMng.m_pWaterIndexList[i].ListCnt; j++)
//		{
//			BOOL retval =  prj.m_terrainMng.LoadTexture(prj.m_terrainMng.m_pWaterIndexList[i].pList[j]);
//#define __ERRORTXT
//			Error("tex id %d, retval %d %s", prj.m_terrainMng.m_pWaterIndexList[i].pList[j], retval, prj.m_terrainMng.m_pWaterIndexList[i].pList[j]);
//		}
//	}
//#else //__YWATER_ANI_EX
	for( int i = 20; i < 54; i++ )
		prj.m_terrainMng.LoadTexture( i );
//#endif //__YWATER_ANI_EX			
		

	CLight* pLight;
/*
	CLight* pLight = new CLight;
	pLight->SetLight( "player", D3DLIGHT_POINT, 0, 0, 0 );
	pLight->Attenuation0 = 0.0f;
	pLight->Attenuation1 = 0.4f;
	pLight->Attenuation2 = 0.0f;
	pLight->Range        = 40.0f;
	pLight->Diffuse.r    = 1.0f;
	pLight->Diffuse.g    = 1.0f;
	pLight->Diffuse.b    = 1.0f;
	pLight->Specular.r   = 1.0f;
	pLight->Specular.g   = 1.0f;
	pLight->Specular.b   = 1.0f;
	pLight->Ambient.r    = 0.2f;
	pLight->Ambient.g    = 0.2f;
	pLight->Ambient.b    = 0.2f;
	AddLight( pLight );
*/
	pLight = new CLight;
	pLight->SetLight( "direction", D3DLIGHT_DIRECTIONAL, 0, 150, 0 );
	pLight->Attenuation0 =  0.0f;
	pLight->Range        = 40;//FLT_MAX;//127.0f;
	pLight->Diffuse.r    = 1.0f;
	pLight->Diffuse.g    = 1.0f;
	pLight->Diffuse.b    = 1.0f;
	pLight->Specular.r   = 1.0f;
	pLight->Specular.g   = 1.0f;
	pLight->Specular.b   = 1.0f;
	pLight->Ambient.r    = 0.0f;
	pLight->Ambient.g    = 0.0f;
	pLight->Ambient.b    = 0.0f;
	pLight->Direction    = D3DXVECTOR3( 0.0f, 0.5f, 0.5f);
	AddLight( pLight );

	pLight = new CLight;
	pLight->SetLight( "direction2", D3DLIGHT_DIRECTIONAL, 0, 150, 0 );
	pLight->Attenuation0 =  0.0f;
	pLight->Range        = 40;//FLT_MAX;//127.0f;
	pLight->Diffuse.r    = 0.1f;
	pLight->Diffuse.g    = 0.1f;
	pLight->Diffuse.b    = 0.1f;
	pLight->Specular.r   = 1.0f;
	pLight->Specular.g   = 1.0f;
	pLight->Specular.b   = 1.0f;
	pLight->Ambient.r    = 1.0f;
	pLight->Ambient.g    = 1.0f;
	pLight->Ambient.b    = 1.0f;
	pLight->Direction    = D3DXVECTOR3( 0.0f, 0.5f, 0.5f);
	AddLight( pLight );
	

	ZeroMemory( &m_baseMaterial, sizeof(m_baseMaterial) );
	m_baseMaterial.Diffuse.r = 1.0f;
	m_baseMaterial.Diffuse.g = 1.0f;
	m_baseMaterial.Diffuse.b = 1.0f;
	m_baseMaterial.Diffuse.a = 1.0f;
	m_baseMaterial.Ambient.r  = 1.0f;
	m_baseMaterial.Ambient.g  = 1.0f;
	m_baseMaterial.Ambient.b  = 1.0f;
	m_baseMaterial.Ambient.a  = 1.0f;
	m_baseMaterial.Specular.r  = 1.0f;
	m_baseMaterial.Specular.g  = 1.0f;
	m_baseMaterial.Specular.b  = 1.0f;
	m_baseMaterial.Specular.a  = 1.0f;
	m_baseMaterial.Power = 50.0f;   

	if( m_pTextureSeaCloud )
		SAFE_RELEASE( m_pTextureSeaCloud );

	if( m_strSeaCloud != "default" )
		LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, m_strSeaCloud ), &m_pTextureSeaCloud );

	return hr;
}
HRESULT CWorld::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_pBoundBoxVertexBuffer ) 
		return S_OK;
	HRESULT hr;
	SetFogEnable( pd3dDevice, m_bViewFog );
	for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++ ) 
	{
		if( m_apLand[ i ] )
			m_apLand[ i ]->RestoreDeviceObjects( pd3dDevice );
	}
	// 바운드 박스 버텍스 버퍼 만들기 
	m_nBoundBoxVertexNum = 12 * 2; // 라인수 * 점 ( 하나의 라인은 점 두쌍 )
	hr = m_pd3dDevice->CreateVertexBuffer( 
		m_nBoundBoxVertexNum * sizeof( BOUNDBOXVERTEX ),
		D3DUSAGE_WRITEONLY, D3DFVF_BOUNDBOXVERTEX,
		D3DPOOL_MANAGED, &m_pBoundBoxVertexBuffer, NULL );
	/*
	hr = m_pd3dDevice->CreateVertexBuffer( 
		m_nBoundBoxVertexNum * sizeof( BOUNDBOXVERTEX ),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_BOUNDBOXVERTEX,
		D3DPOOL_SYSTEMMEM, &m_pBoundBoxVertexBuffer, NULL );
		*/
#ifdef __BEAST
	m_meshGizmo.RestoreDeviceObjects();
	//if( m_nLandWidth && m_nLandHeight )
	//	BuildGridVertex( m_pd3dDevice );

	// Axis 버텍스 버퍼 
	AXISVERTEX* pAxisVertices;
	m_nAxisVertexNum = 6;
	// 버텍스 버퍼 만들기 
	hr = pd3dDevice->CreateVertexBuffer( m_nAxisVertexNum * sizeof(AXISVERTEX),
		 D3DUSAGE_WRITEONLY, D3DFVF_AXISVERTEX, D3DPOOL_MANAGED, &m_pAxisVertexBuffer, NULL );
	if( FAILED(hr) ) return hr;

	hr = m_pAxisVertexBuffer->Lock( 0, m_nAxisVertexNum * sizeof(AXISVERTEX), (void**)&pAxisVertices, 0 );
	if( FAILED(hr) ) return hr;

#ifdef __CSC_RENDER_AXIS
	// x axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	pAxisVertices++;
	pAxisVertices->p.x     = -0.4f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	pAxisVertices++;

	// y axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	pAxisVertices++;
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = -0.4f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	pAxisVertices++;

	// z axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 255, 0 );
	pAxisVertices++;
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.4f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 255, 0 );
	pAxisVertices++;
#else //__CSC_RENDER_AXIS
	// x axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	pAxisVertices++;
	pAxisVertices->p.x     = 10.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	pAxisVertices++;

	// z axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 255, 0 );
	pAxisVertices++;
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 10.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 255, 0 );
	pAxisVertices++;

	// y axis
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 0.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	pAxisVertices++;
	pAxisVertices->p.x     = 0.0f;
	pAxisVertices->p.y     = 10.0f;
	pAxisVertices->p.z     = 0.0f;
	pAxisVertices->color   = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	pAxisVertices++;
#endif //__CSC_RENDER_AXIS
	m_pAxisVertexBuffer->Unlock();
#endif
	return hr;
}

HRESULT CWorld::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;
	for(int i = 0; i < m_nLandWidth * m_nLandHeight; i++) 
	{
		if( m_apLand[ i ] )
			m_apLand[ i ]->InvalidateDeviceObjects();
	}
#ifdef __BEAST
	m_meshGizmo.InvalidateDeviceObjects();
	SAFE_RELEASE( m_pAxisVertexBuffer );
	//SAFE_RELEASE( m_pGridVertexBuffer );
	//SAFE_RELEASE( m_pRegionVertexBuffer );
//	SAFE_RELEASE( m_pCircleVertexBuffer );
#endif
	SAFE_RELEASE( m_pBoundBoxVertexBuffer );
	//SAFE_RELEASE( m_pCloudVertexBuffer );	
	//SAFE_RELEASE( m_pWaterVertexBuffer );	

#ifdef __VSWATER
    SAFE_RELEASE( m_pWaterIB );
    SAFE_RELEASE( m_pWaterVB );
#endif
	return hr;
}
HRESULT CWorld::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	for(int i = 0; i < m_nLandWidth * m_nLandHeight; i++) 
	{
		if( m_apLand[ i ] )
		{
			m_apLand[ i ]->DeleteDeviceObjects();
		}
	}
	if( m_apLand != NULL )
	{
		for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
		{
			SAFE_DELETE( m_apLand[ i ] );
		}
		SAFE_DELETE_ARRAY( m_apLand );
	}
#ifdef __BEAST
	m_meshGizmo.DeleteDeviceObjects();
#endif
#ifdef __VSWATER
    m_pd3dDevice->DeleteVertexShader( m_dwWaterShader );
#endif
	return hr;
}
HRESULT CWorld::StaticInitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CWorld::StaticRestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( g_pExIB == NULL ) 
	{
		pd3dDevice->CreateIndexBuffer( (PATCH_SIZE*PATCH_SIZE)* 6 * sizeof( WORD ),
										  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED , &g_pExIB, NULL);
		WORD* pIB;
		int count=0;
		g_pExIB->Lock( 0, 0, (void**)&pIB, 0 );
		for(int py=0;py<PATCH_SIZE;py++) {
			for(int px=0;px<PATCH_SIZE;px++) {
				if((px+py)%2==0) {
					pIB[count++]=px+py*(PATCH_SIZE+1);
					pIB[count++]=(px+1)+(py+1)*(PATCH_SIZE+1);
					pIB[count++]=px+1+py*(PATCH_SIZE+1);

					pIB[count++]=px+py*(PATCH_SIZE+1);
					pIB[count++]=px+(py+1)*(PATCH_SIZE+1);
					pIB[count++]=(px+1)+(py+1)*(PATCH_SIZE+1);
				}
				else {
					pIB[count++]=px+py*(PATCH_SIZE+1);
					pIB[count++]=px+(py+1)*(PATCH_SIZE+1);
					pIB[count++]=(px+1)+py*(PATCH_SIZE+1);

					pIB[count++]=px+(py+1)*(PATCH_SIZE+1);
					pIB[count++]=(px+1)+(py+1)*(PATCH_SIZE+1);
					pIB[count++]=(px+1)+py*(PATCH_SIZE+1);
				}
			}
		}
	}
	if( g_pGridIB == NULL ) 
	{
		pd3dDevice->CreateIndexBuffer( NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE*(PATCH_SIZE+1)*(PATCH_SIZE+1)*2*2 * sizeof( WORD ),
										  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED , &g_pGridIB, NULL);
	}
	if( g_pIB == NULL ) 
	{
		pd3dDevice->CreateIndexBuffer( (128+48+32+16+8+4+2)*3 * sizeof( WORD ),
										  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED , &g_pIB, NULL );
		WORD* pIB;
		g_pIB->Lock( 0, 0, (void**)&pIB, 0 );

		WORD tempTriList[]={
			// LOD level 0 아래쪽 삼각형 32개의 index
			0,10,1, 1,10,2, 2,10,11, 2,11,12, 2,12,3, 3,12,4, 4,12,13, 4,13,14, 4,14,5, 5,14,6, 6,14,15, 6,15,16, 6,16,7, 7,16,8,
			10,20,11, 11,20,12, 12,20,21, 12,21,22, 12,22,13, 13,22,14, 14,22,23, 14,23,24, 14,24,15, 15,24,16,
			20,30,21, 21,30,22, 22,30,31, 22,31,32, 22,32,23, 23,32,24, 30,40,31, 31,40,32,
			// LOD level 0 왼쪽 삼각형 32개의 index
			8,16,17, 16,24,25, 16,25,26, 16,26,17, 24,32,33, 24,33,34, 24,34,25, 25,34,26, 26,34,35,
			32,40,41, 32,41,42, 32,42,33, 33,42,34, 34,42,43, 34,43,44, 34,44,35, 40,50,41, 41,50,42, 42,50,51, 42,51,52, 42,52,43, 43,52,44, 44,52,53,
			50,60,51, 51,60,52, 52,60,61, 52,61,62, 52,62,53, 60,70,61, 61,70,62, 62,70,71, 70,80,71,
			// LOD level 0 오른쪽 삼각형 32개의 index
			0,9,10, 9,18,10, 10,18,19, 10,19,20, 18,27,28, 18,28,19, 19,28,20, 20,28,29, 20,29,30,
			27,36,28, 28,36,37, 28,37,38, 28,38,29, 29,38,30, 30,38,39, 30,39,40, 36,45,46, 36,46,37, 37,46,38, 38,46,47, 38,47,48, 38,48,39, 39,48,40,
			45,54,46, 46,54,55, 46,55,56, 46,56,47, 47,56,48, 54,63,64, 54,64,55, 55,64,56, 63,72,64,
			// LOD level 0 윗쪽 삼각형 32개의 index
			48,49,40, 40,49,50, 48,56,57, 48,57,58, 48,58,49, 49,58,50, 50,58,59, 50,59,60,
			56,64,65, 56,65,66, 56,66,57, 57,66,58, 58,66,67, 58,67,68, 58,68,59, 59,68,60, 60,68,69, 60,69,70,
			64,72,73, 64,73,74, 64,74,65, 65,74,66, 66,74,75, 66,75,76, 66,76,67, 67,76,68, 68,76,77, 68,77,78, 68,78,69, 69,78,70, 70,78,79, 70,79,80,
			// LOD level 0에서 LOD level 1에 연결시키는 아래쪽 삼각형 12개의 index
			0,10,2, 2,10,20, 2,20,4, 4,20,22, 4,22,24, 4,24,6, 6,24,16, 6,16,8,
			20,30,22, 22,30,40, 22,40,32, 22,32,24,
			// LOD level 0에서 LOD level 1에 연결시키는 왼쪽 삼각형 12개의 index
			8,16,26, 16,24,26, 32,40,42, 32,42,24, 24,42,44, 24,44,26,
			40,50,42, 42,50,60, 42,60,44, 44,60,62, 60,70,62, 62,70,80,
			// LOD level 0에서 LOD level 1에 연결시키는 오른쪽 삼각형 12개의 index
			0,18,10, 10,18,20, 20,18,36, 20,36,38, 20,38,30, 30,38,40,
			36,54,56, 36,56,38, 38,56,48, 38,48,40, 54,72,64, 54,64,56,
			// LOD level 0에서 LOD level 1에 연결시키는 윗쪽 삼각형 12개의 index
			48,56,58, 48,58,40, 50,40,58, 50,58,60,
			64,72,74, 64,74,56, 56,74,76, 58,56,76, 58,76,60, 60,76,78, 60,78,70, 70,78,80,

			// LOD level 1 아래쪽 삼각형 8개의 index
			0,20,2, 2,20,4, 4,20,22, 4,22,24, 4,24,6, 6,24,8, 20,40,22, 22,40,24,
			// LOD level 1 왼쪽 삼각형 8개의 index
			8,24,26, 24,40,42, 24,42,44, 24,44,26, 40,60,42, 42,60,44, 44,60,62, 60,80,62,
			// LOD level 1 오른쪽 삼각형 8개의 index
			0,18,20, 18,36,20, 20,36,38, 20,38,40, 36,54,56, 36,56,38, 38,56,40, 54,72,56,
			// LOD level 1 윗쪽 삼각형 8개의 index
			40,56,58, 40,58,60, 56,72,74, 56,74,76, 56,76,58, 58,76,60, 60,76,78, 60,78,80,
			// LOD level 1에서 LOD level 2에 연결시키는 아래쪽 삼각형 4개의 index
			4,0,20, 4,20,40, 4,40,24, 4,24,8,
			// LOD level 1에서 LOD level 2에 연결시키는 왼쪽 삼각형 4개의 index
			44,8,24, 44,24,40, 44,40,60, 44,60,80,
			// LOD level 1에서 LOD level 2에 연결시키는 오른쪽 삼각형 4개의 index
			36,72,56, 36,56,40, 36,40,20, 36,20,0,
			// LOD level 1에서 LOD level 2에 연결시키는 윗쪽 삼각형 4개의 index
			76,80,60, 76,60,40, 76,40,56, 76,56,72,

			// LOD level 2 아래쪽 삼각형 2개의 index
			4,0,40, 4,40,8,
			// LOD level 2 왼쪽 삼각형 2개의 index
			44,8,40, 44,40,80,
			// LOD level 2 오른쪽 삼각형 2개의 index
			36,72,40, 36,40,0,
			// LOD level 2 윗쪽 삼각형 2개의 index
			76,80,40, 76,40,72,
			// LOD level 2에서 LOD level 3에 연결시키는 아래쪽 삼각형 1개의 index
			0,40,8,
			// LOD level 2에서 LOD level 3에 연결시키는 왼쪽 삼각형 1개의 index
			8,40,80,
			// LOD level 2에서 LOD level 3에 연결시키는 오른쪽 삼각형 1개의 index
			0,72,40,
			// LOD level 2에서 LOD level 3에 연결시키는 윗쪽 삼각형 1개의 index
			40,72,80,

			// LOD level 3의 삼각형 2개의 index
			0,72,8, 8,72,80
		};
		memcpy( pIB, tempTriList, (128+48+32+16+8+4+2)*3 * sizeof( WORD ) );
		g_pIB->Unlock();
	}
	return S_OK;
}
HRESULT CWorld::StaticInvalidateDeviceObjects()
{
	SAFE_RELEASE( g_pIB );
	SAFE_RELEASE( g_pExIB );
	SAFE_RELEASE( g_pGridIB );
	return S_OK;
}
HRESULT CWorld::StaticDeleteDeviceObjects()
{

	return S_OK;
}

void CWorld::SetFogEnable(LPDIRECT3DDEVICE9 pd3dDevice,BOOL bEnable)
{
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, bEnable );
	if(bEnable)
	{
		DWORD dwColor = CWorld::GetDiffuseColor();
		//DWORD dwRed   = ( dwColor & 0x00ff0000 ) >> 16;
		//DWORD dwGreen = ( dwColor & 0x0000ff00 ) >> 8;
		//DWORD dwBlue  = ( dwColor & 0x000000ff );
		//dwColor = D3DCOLOR_ARGB( 255, dwRed + 30, dwGreen + 30, dwBlue + 30 );
		//pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  0xffc7daff );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );

		ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
		if( m_bIsContinent && pInfo && pInfo->_bUseEnvir )		//gmpbigsun : 포그 거리는 대륙정보 우선
		{
			pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(pInfo->_fFogStart) );
			pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(pInfo->_fFogEnd) );
		}else
		{
			pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(m_fFogStartValue) );
			pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(m_fFogEndValue) );
		}
		pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(m_fFogDensity) );
		/*
    if( m_bUsingTableFog )
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   m_dwFogMode );
    }
    else
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  m_dwFogMode );
        m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, m_bRangeBasedFog );
    }

		*/
		if( 0 )// m_bUsingTableFog ) // pixel(table)
		{
			m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
			m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );//m_dwFogMode );
		}
		else // vertex
		{
			m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
			m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );//m_dwFogMode );
			m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );//m_bRangeBasedFog );
		}
	}
}

static CObj *s_pLastPickObj = NULL;

CObj *GetLastPickObj( void )
{
	return s_pLastPickObj;
}

//
// 필드의 클릭한 지점을 얻기
// [in] point는 클라이언트 화면의 좌표 
// [out] pVector는 목표 좌표 
// pOut : 피킹한곳의 삼각형 버텍스 3개의 시작 포인터
BOOL CWorld::ClientPointToVector( D3DXVECTOR3 *pOut, RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, D3DXVECTOR3* pVector, BOOL bObject )
{
	if( m_pCamera == NULL )
		return FALSE;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;                    
	CObj* pObj = NULL;
	D3DXVECTOR3 vPickObj = D3DXVECTOR3( 0, 0, 0 );	// 오브젝트에 피킹된좌표.


	GetPickRay( rect, point, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir );

	if( bObject )
	{
		pObj = CWorld::PickObject( rect, point, pmatProj, pmatView, 0xffffffff, CMover::GetActiveObj(), pVector, TRUE );
		if( pObj ) 
		{
			vPickObj = *pVector;		// 오브젝트에 피킹되었으면 받아둠.
//			bTriangle1 = TRUE;
//			return TRUE;
		}
	}
	

	vPickRayDir.x *= (FLOAT)MPU * 2;
	vPickRayDir.z *= (FLOAT)MPU * 2;
	vPickRayDir.y *= (FLOAT)MPU * 2;
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;// * MPU;

	FLOAT fFarPlane = m_fFarPlane;

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
						int tempx = (int)( vPickRayCur.x - x );//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
						int tempy = (int)( vPickRayCur.z - z );//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;
						v1=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
						v2=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
						v3=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
						v4=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
						if( IntersectTriangle( v1, v2, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
						{
							if( fDist < fNearDist ) { 
								fNearDist = fDist; *pVector = vecIntersect; bTriangle1 = TRUE; 
							}
						}
						else
						if( IntersectTriangle( v2, v4, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
						{
							if( fDist < fNearDist ) {	
								fNearDist = fDist; *pVector = vecIntersect;	bTriangle1 = TRUE; 
							}
						}
					}
				}
			}
		}
		vLength = vPickRayOrig - vPickRayCur;
	} while(/*PickRayCur.y >= 0.0f && vPickRayCur.y < 100.0f &&*/ D3DXVec3Length( &vLength ) < ( fFarPlane ) );
	
	if( bTriangle1 || pObj )
	{
		// 오브젝트에 피킹되었을 수도 있고 땅에 피킹되었을 수도 있다. 가까운놈을 써야한다.
		if( pObj )
		{
			D3DXVECTOR3 vDist = vPickObj - vPickRayOrig;
			FLOAT fDistSq1 = D3DXVec3LengthSq( &vDist );	// 레이와 오브젝트 피킹 지점까지의 거리
			vDist = *pVector - vPickRayOrig;
			FLOAT fDistSq2 = D3DXVec3LengthSq( &vDist );	// 레이와 지형 피킹지점까지의 거리.
			if( fDistSq1 < fDistSq2 )	// 오브젝트 피킹이 더 가까우면.
			{
				*pVector = vPickObj;	// 바꾼다.
				D3DXVECTOR3 *pTri = ::GetLastPickTri();
				if( pOut )
				{
					D3DXVec3TransformCoord( &pOut[0], &pTri[0], pObj->GetMatrixWorldPtr() );
					D3DXVec3TransformCoord( &pOut[1], &pTri[1], pObj->GetMatrixWorldPtr() );
					D3DXVec3TransformCoord( &pOut[2], &pTri[2], pObj->GetMatrixWorldPtr() );
				}
				s_pLastPickObj = pObj;		// 마지막으로 피킹된 오브젝트.
			} else
			{
				if( pOut )
					GetLandTri( pVector->x, pVector->z, pOut );		// 지형에 피킹됐으면 지형삼각형 꺼냄.
				s_pLastPickObj = NULL;
			}
		} else
		{
			if( pOut )
				GetLandTri( pVector->x, pVector->z, pOut );		// 지형에 피킹됐으면 지형삼각형 꺼냄.
			s_pLastPickObj = NULL;
		}
		return TRUE;
	}

	return FALSE;
}

//
//
//
BOOL CWorld::IsPickTerrain( RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView )
{
	if( m_pCamera == NULL )
		return FALSE;
	D3DXVECTOR3 vector;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;                    
	FLOAT fFarPlaneSq = m_fFarPlane * m_fFarPlane;

	GetPickRay( rect, point, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir );
	vPickRayDir.x *= (FLOAT)MPU * 2;
	vPickRayDir.z *= (FLOAT)MPU * 2;
	vPickRayDir.y *= (FLOAT)MPU * 2;
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;// * MPU;
	do 
	{
		vPickRayCur += vPickRayAdd;
		if( VecInWorld( vPickRayCur ) )
		{
			CLandscape* pLand = GetLandscape( vPickRayCur );
			if( pLand  && pLand->isVisibile() )
			{
				int tempx = (int)( vPickRayCur.x );//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
				int tempy = (int)( vPickRayCur.z );//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;
				v1=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
				v2=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
				v3=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
				v4=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
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
		vLength = vPickRayOrig - vPickRayCur;
	} while( D3DXVec3LengthSq( &vLength ) < fFarPlaneSq );	// Sq버전으로 바꿈. -xuzhu-
	if( bTriangle1 )
		return TRUE;
	return FALSE;
}

// 주어진 레이와 충돌하는 바닥을 체크.
FLOAT CWorld::IntersectRayTerrain( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir )
{
	if( m_pCamera == NULL )
		return FALSE;
	D3DXVECTOR3 vector;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;                    
	FLOAT fFarPlaneSq = m_fFarPlane * m_fFarPlane;
	
	vPickRayAdd = vPickRayDir;
	vPickRayAdd.x *= (FLOAT)MPU * 2;
	vPickRayAdd.z *= (FLOAT)MPU * 2;
	vPickRayAdd.y *= (FLOAT)MPU * 2;
	vPickRayCur = vPickRayOrig;
//	vPickRayAdd = vPickRayDir;// * MPU;
	do 
	{
		vPickRayCur += vPickRayAdd;
		if( VecInWorld( vPickRayCur ) )
		{
			CLandscape* pLand = GetLandscape( vPickRayCur );
			if( pLand  && pLand->isVisibile() )
			{
				int tempx = (int)( vPickRayCur.x );//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
				int tempy = (int)( vPickRayCur.z );//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;
				v1=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
				v2=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
				v3=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
				v4=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
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
		vLength = vPickRayOrig - vPickRayCur;
	} while( D3DXVec3LengthSq( &vLength ) < fFarPlaneSq );	// Sq버전으로 바꿈. -xuzhu-
	if( bTriangle1 )
		return fNearDist;
	return 0;
}



//
// 필드의 클릭한 지점을 얻기
// [in] point는 클라이언트 화면의 좌표 
// [out] pVector는 목표 좌표 
//
CObj* CWorld::PickObject( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter, CObj* pExceptionObj, D3DXVECTOR3* pVector, BOOL bOnlyTopPick, BOOL bOnlyNPC )
{
	if( m_pCamera == NULL )
		return FALSE;

	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vPickRayDir2;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur ;
	D3DXVECTOR3 vIntersect  ;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;      
	CObj* pSelectObj = NULL;

	GetPickRay( rectClient, ptClient, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir );
	vPickRayDir2 = vPickRayDir;
	vPickRayDir2.y = 0.0f;
	D3DXVec3Normalize( &vPickRayDir2, &vPickRayDir2 );
	int nCount = 0;
	
	CObj* pObj;
	CObj* pNonCullObjs[ 10000 ];
	int nNonCullNum = 0;
	for( int i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj )
		{
			if( bOnlyTopPick && pObj->GetModel()->m_pModelElem->m_bPick == FALSE )
				continue;
			if( pObj->IsCull() == FALSE ) 
			{
				if( pObj != pExceptionObj && ( ObjTypeToObjFilter( pObj->GetType() ) & dwObjectFilter ) ) // &&
//					!( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie() ) )
				{
					if( bOnlyNPC && pObj->GetType() == OT_MOVER )	// bOnlyNPC옵션이 켜져있을때
						if( ((CMover*)pObj)->IsPlayer() )	continue;	// 플레이어는 스킵.

					pNonCullObjs[ nNonCullNum++ ] = pObj;
				}
			}
		}
	}
    //qsort( pNonCullObjs, nNonCullNum, sizeof(CObj*), ObjSortNearToFar ); 

	BOOL bPick = FALSE;
	for( int i = nNonCullNum - 1; i >= 0; i-- )
	{
		CObj* pObj = (CObj*)pNonCullObjs[ i ];
		nCount++;
		if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie() )	// 죽은사람은 바운딩박스로 검사하지 않음.(바운딩박스랑 맞지 않는다).
			bPick = pObj->m_pModel->Intersect( vPickRayOrig, vPickRayDir, pObj->GetMatrixWorld(), &vIntersect, &fDist );
		else
			bPick = pObj->Pick( &vPickRayOrig, &vPickRayDir, &vIntersect, &fDist ); // 그외는 바운딩박스 먼저 검사후, 세밀검사.
		if( bPick )
		{
			if( fDist < fNearDist )
			{
			//	TRACE( "Pick Objec num = %d \n", nCount );
				fNearDist = fDist;
				if( pVector ) 
					*pVector = vIntersect;
				return pObj;
			}
		}
	}
	return NULL;
}
CObj* CWorld::PickObject_Fast( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter, CObj* pExceptionObj, BOOL bBoundBox, BOOL bOnlyNPC )
{
	if( m_pCamera == NULL )
		return NULL;

	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vPickRayDir2;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur ;
	D3DXVECTOR3 vIntersect  ;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;      
	CObj* pSelectObj = NULL;

	GetPickRay( rectClient, ptClient, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir );
	vPickRayDir2 = vPickRayDir;
	vPickRayDir2.y = 0.0f;
	D3DXVec3Normalize( &vPickRayDir2, &vPickRayDir2 );
	int nCount = 0;

	CObj* pObj;
	CObj* pNonCullObjs[ 10000 ];
	int nNonCullNum = 0;
	for( int i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj )
		{
			if( pObj->IsCull() == FALSE /*&& pObj->GetModel()->m_pModelElem->m_dwDistant <= 1*/ ) 
			{
				if( pObj != pExceptionObj && ( ObjTypeToObjFilter( pObj->GetType() ) & dwObjectFilter ) ) //&&
//					!( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie() ) )
				{
					if( bOnlyNPC && pObj->GetType() == OT_MOVER )	// bOnlyNPC옵션이 켜져있을때
						if( ((CMover*)pObj)->IsPlayer() )	continue;	// 플레이어는 스킵.
					pNonCullObjs[ nNonCullNum++ ] = pObj;
				}
			}
		}
	}
	//qsort( pNonCullObjs, nNonCullNum, sizeof(CObj*), ObjSortNearToFar ); 
	BOOL bAABB = bBoundBox;

	for( int i = nNonCullNum - 1; i >= 0; i-- )
	{
		CObj* pObj = (CObj*)pNonCullObjs[ i ];//GetObjInLinkMap( vPickRayCur, i );
		nCount++;
		bAABB = bBoundBox;
		if( pObj->GetType() == OT_CTRL )		// 컨트롤은 바운딩박스로만 체크하면 안됨.
			bAABB = FALSE;
		if( pObj->Pick( &vPickRayOrig, &vPickRayDir, &vIntersect, &fDist, bAABB ) )
		{
			if( fDist < fNearDist )
			{
				fNearDist = fDist;
				return pObj;
			}
		}
	}
	return NULL;
}
#ifdef __BEAST
int CWorld::GetPickCollection( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, CPtrArray* pPtrArray )
{
	if( m_pCamera == NULL )
		return FALSE;
	
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vPickRayDir2;
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur ;
	D3DXVECTOR3 vIntersect  ;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;      
	CObj* pSelectObj = NULL;
	
	GetPickRay( rectClient, ptClient, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir );
	vPickRayDir2 = vPickRayDir;
	vPickRayDir2.y = 0.0f;
	D3DXVec3Normalize( &vPickRayDir2, &vPickRayDir2 );
	int nCount = 0;
	
	CObj* pObj;
	CObj* pNonCullObjs[ 10000 ];
	int nNonCullNum = 0;
	for( int i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj )
		{
			if( pObj->IsCull() == FALSE && pObj->IsVisible() ) 
				pNonCullObjs[ nNonCullNum++ ] = pObj;
		}
	}
    //qsort( pNonCullObjs, nNonCullNum, sizeof(CObj*), ObjSortNearToFar ); 
	
	BOOL bPick = FALSE;
	for( int i = nNonCullNum - 1; i >= 0; i-- )
	{
		CObj* pObj = (CObj*)pNonCullObjs[ i ];
		nCount++;
		bPick = pObj->Pick( &vPickRayOrig, &vPickRayDir, &vIntersect, &fDist ); // 그외는 바운딩박스 먼저 검사후, 세밀검사.
		if( bPick )
			pPtrArray->Add( pObj );
	}
	return pPtrArray->GetSize();
}

#endif
void CWorld::UpdateCullInfo(D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
    D3DXMATRIX mat; 

    D3DXMatrixMultiply( &mat, pMatView, pMatProj );
    D3DXMatrixInverse( &mat, NULL, &mat );

    g_cullinfo.vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    g_cullinfo.vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    g_cullinfo.vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    g_cullinfo.vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    g_cullinfo.vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
    g_cullinfo.vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
    g_cullinfo.vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
    g_cullinfo.vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

    for( INT i = 0; i < 8; i++ )
        D3DXVec3TransformCoord( &g_cullinfo.vecFrustum[i], &g_cullinfo.vecFrustum[i], &mat );

    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[0], &g_cullinfo.vecFrustum[0], 
        &g_cullinfo.vecFrustum[1], &g_cullinfo.vecFrustum[2] ); // Near
    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[1], &g_cullinfo.vecFrustum[6], 
        &g_cullinfo.vecFrustum[7], &g_cullinfo.vecFrustum[5] ); // Far
    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[2], &g_cullinfo.vecFrustum[2], 
        &g_cullinfo.vecFrustum[6], &g_cullinfo.vecFrustum[4] ); // Left
    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[3], &g_cullinfo.vecFrustum[7], 
        &g_cullinfo.vecFrustum[3], &g_cullinfo.vecFrustum[5] ); // Right
    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[4], &g_cullinfo.vecFrustum[2], 
        &g_cullinfo.vecFrustum[3], &g_cullinfo.vecFrustum[6] ); // Top
    D3DXPlaneFromPoints( &g_cullinfo.planeFrustum[5], &g_cullinfo.vecFrustum[1], 
        &g_cullinfo.vecFrustum[0], &g_cullinfo.vecFrustum[4] ); // Bottom
}


//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).  
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane, 
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return 
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the 
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
CULLSTATE CullObject( CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, D3DXPLANE* pPlaneBounds )
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );
    // Check boundary vertices against all 6 frustum planes, 
    // and store result (1 if outside) in a bitfield
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
                pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
                pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
                pCullInfo->planeFrustum[iPlane].d < 0)
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        // If any point is inside all 6 frustum planes, it is inside
        // the frustum, so the object must be rendered.
        if( bOutside[iPoint] == 0 )
            return CS_INSIDE;
    }

    // If all points are outside any single frustum plane, the object is
    // outside the frustum
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
        return CS_OUTSIDE;
    }
//    return CS_OUTSIDE;

    // Now see if any of the frustum edges penetrate any of the faces of
    // the bounding box
    D3DXVECTOR3 edge[12][2] = 
    {
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[1], // front bottom
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], // front top
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], // front left
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], // front right
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], // back bottom
        pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], // back top
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], // back left
        pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], // back right
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], // left bottom
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], // left top
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[5], // right bottom
        pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], // right top
    };
    D3DXVECTOR3 face[6][4] =
    {
        pVecBounds[0], pVecBounds[2], pVecBounds[3], pVecBounds[1], // front
        pVecBounds[4], pVecBounds[5], pVecBounds[7], pVecBounds[6], // back
        pVecBounds[0], pVecBounds[4], pVecBounds[6], pVecBounds[2], // left
        pVecBounds[1], pVecBounds[3], pVecBounds[7], pVecBounds[5], // right
        pVecBounds[2], pVecBounds[6], pVecBounds[7], pVecBounds[3], // top
        pVecBounds[0], pVecBounds[4], pVecBounds[5], pVecBounds[1], // bottom
    };
    D3DXVECTOR3* pEdge;
    D3DXVECTOR3* pFace;
    pEdge = &edge[0][0];
    for( INT iEdge = 0; iEdge < 12; iEdge++ )
    {
        pFace = &face[0][0];
        for( INT iFace = 0; iFace < 6; iFace++ )
        {
            if( EdgeIntersectsFace( pEdge, pFace, &pPlaneBounds[iFace] ) )
            {
                return CS_INSIDE_SLOW;
            }
            pFace += 4;
        }
        pEdge += 2;
    }

    // Now see if any of the bounding box edges penetrate any of the faces of
    // the frustum
    D3DXVECTOR3 edge2[12][2] = 
    {
        pVecBounds[0], pVecBounds[1], // front bottom
        pVecBounds[2], pVecBounds[3], // front top
        pVecBounds[0], pVecBounds[2], // front left
        pVecBounds[1], pVecBounds[3], // front right
        pVecBounds[4], pVecBounds[5], // back bottom
        pVecBounds[6], pVecBounds[7], // back top
        pVecBounds[4], pVecBounds[6], // back left
        pVecBounds[5], pVecBounds[7], // back right
        pVecBounds[0], pVecBounds[4], // left bottom
        pVecBounds[2], pVecBounds[6], // left top
        pVecBounds[1], pVecBounds[5], // right bottom
        pVecBounds[3], pVecBounds[7], // right top
    };
    D3DXVECTOR3 face2[6][4] =
    {
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[1], // front
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[6], // back
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[2], // left
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[5], // right
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[3], // top
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[1], // bottom
    };
    pEdge = &edge2[0][0];
    for( INT iEdge = 0; iEdge < 12; iEdge++ )
    {
        pFace = &face2[0][0];
        for( INT iFace = 0; iFace < 6; iFace++ )
        {
            if( EdgeIntersectsFace( pEdge, pFace, &pCullInfo->planeFrustum[iFace] ) )
            {
                return CS_INSIDE_SLOW;
            }
            pFace += 4;
        }
        pEdge += 2;
    }

    // Now see if frustum is contained in bounding box
    // If any frustum corner point is outside any plane of the bounding box,
    // the frustum is not contained in the bounding box, so the object
    // is outside the frustum
    for( INT iPlane = 0; iPlane < 6; iPlane++ )
    {
        if( pPlaneBounds[iPlane].a * pCullInfo->vecFrustum[0].x +
            pPlaneBounds[iPlane].b * pCullInfo->vecFrustum[0].y +
            pPlaneBounds[iPlane].c * pCullInfo->vecFrustum[0].z +
            pPlaneBounds[iPlane].d  < 0 )
        {
            return CS_OUTSIDE_SLOW;
        }
    }

    // Bounding box must contain the frustum, so render the object
    return CS_INSIDE_SLOW;
}
//-----------------------------------------------------------------------------
// Name: EdgeIntersectsFace()
// Desc: Determine if the edge bounded by the two vectors in pEdges intersects
//       the quadrilateral described by the four vectors in pFacePoints.  
//       Note: pPlane could be derived from pFacePoints using 
//       D3DXPlaneFromPoints, but it is precomputed in advance for greater
//       speed.
//-----------------------------------------------------------------------------
BOOL EdgeIntersectsFace( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, D3DXPLANE* pPlane )
{
    // If both edge points are on the same side of the plane, the edge does
    // not intersect the face
    FLOAT fDist1;
    FLOAT fDist2;
    fDist1 = pPlane->a * pEdges[0].x + pPlane->b * pEdges[0].y +
             pPlane->c * pEdges[0].z + pPlane->d;
    fDist2 = pPlane->a * pEdges[1].x + pPlane->b * pEdges[1].y +
             pPlane->c * pEdges[1].z + pPlane->d;
    if( fDist1 > 0 && fDist2 > 0 ||
        fDist1 < 0 && fDist2 < 0 )
    {
        return FALSE;
    }

    // Find point of intersection between edge and face plane (if they're
    // parallel, edge does not intersect face and D3DXPlaneIntersectLine 
    // returns NULL)
    D3DXVECTOR3 ptIntersection;
    if( NULL == D3DXPlaneIntersectLine( &ptIntersection, pPlane, &pEdges[0], &pEdges[1] ) )
        return FALSE;

    // Project onto a 2D plane to make the pt-in-poly test easier
    FLOAT fAbsA = (pPlane->a > 0 ? pPlane->a : -pPlane->a);
    FLOAT fAbsB = (pPlane->b > 0 ? pPlane->b : -pPlane->b);
    FLOAT fAbsC = (pPlane->c > 0 ? pPlane->c : -pPlane->c);
    D3DXVECTOR2 facePoints[4];
    D3DXVECTOR2 point;
    if( fAbsA > fAbsB && fAbsA > fAbsC )
    {
        // Plane is mainly pointing along X axis, so use Y and Z
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].y;
            facePoints[i].y = pFacePoints[i].z;
        }
        point.x = ptIntersection.y;
        point.y = ptIntersection.z;
    }
    else if( fAbsB > fAbsA && fAbsB > fAbsC )
    {
        // Plane is mainly pointing along Y axis, so use X and Z
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].x;
            facePoints[i].y = pFacePoints[i].z;
        }
        point.x = ptIntersection.x;
        point.y = ptIntersection.z;
    }
    else
    {
        // Plane is mainly pointing along Z axis, so use X and Y
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].x;
            facePoints[i].y = pFacePoints[i].y;
        }
        point.x = ptIntersection.x;
        point.y = ptIntersection.y;
    }

    // If point is on the outside of any of the face edges, it is
    // outside the face.  
    // We can do this by taking the determinant of the following matrix:
    // | x0 y0 1 |
    // | x1 y1 1 |
    // | x2 y2 1 |
    // where (x0,y0) and (x1,y1) are points on the face edge and (x2,y2) 
    // is our test point.  If this value is positive, the test point is
    // "to the left" of the line.  To determine whether a point needs to
    // be "to the right" or "to the left" of the four lines to qualify as
    // inside the face, we need to see if the faces are specified in 
    // clockwise or counter-clockwise order (it could be either, since the
    // edge could be penetrating from either side).  To determine this, we
    // do the same test to see if the third point is "to the right" or 
    // "to the left" of the line formed by the first two points.
    // See http://forum.swarthmore.edu/dr.math/problems/scott5.31.96.html
    FLOAT x0, x1, x2, y0, y1, y2;
    x0 = facePoints[0].x;
    y0 = facePoints[0].y;
    x1 = facePoints[1].x;
    y1 = facePoints[1].y;
    x2 = facePoints[2].x;
    y2 = facePoints[2].y;
    BOOL bClockwise = FALSE;
    if( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 < 0 )
        bClockwise = TRUE;
    x2 = point.x;
    y2 = point.y;
    for( INT i = 0; i < 4; i++ )
    {
        x0 = facePoints[i].x;
        y0 = facePoints[i].y;
        if( i < 3 )
        {
            x1 = facePoints[i+1].x;
            y1 = facePoints[i+1].y;
        }
        else
        {
            x1 = facePoints[0].x;
            y1 = facePoints[0].y;
        }
        if( ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 > 0 ) == bClockwise )
            return FALSE;
    }

    // If we get here, the point is inside all four face edges, 
    // so it's inside the face.
    return TRUE;
}
void CWorld::CalculateBound()
{
	for(int i = 0; i < m_nLandHeight; i++)
	{
		for(int j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ j + i * m_nLandWidth ] )
				m_apLand[ j + i * m_nLandWidth ]->CalculateBound();
		}
	}
}


void CWorld::RenderGrid()
{
	//if( m_bViewGrid || m_bViewGridPatch || m_bViewGridLandscape )
	{ 
		if( m_bViewGrid )
	 		RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , 1, 0xff808080 );
		if( m_bViewGridPatch )
			RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , MAP_SIZE / NUM_PATCHES_PER_SIDE, 0xffffffff );
		if( m_bViewGridLandscape )
			RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , MAP_SIZE, 0xffffff00 );
/*
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetTexture( 0, NULL);
		m_pd3dDevice->SetVertexShader( D3DFVF_GRIDVERTEX );
		m_pd3dDevice->SetStreamSource( 0, m_pGridVertexBuffer, sizeof(GRIDVERTEX) );
		m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_nGridVertexNum/2);
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
*/
	}
}

void CWorld::RenderGrids( CRect rect,WORD dx,DWORD color)
{
	rect.NormalizeRect();
	int startx=rect.left  /MPU;
	int starty=rect.top   /MPU;
	int endx  =rect.right /MPU;
	int endy  =rect.bottom/MPU;

	//if(startx>endx) swap(startx,endx);
	//if(starty>endy) swap(starty,endy);
	if(startx<0) startx=0;
	if(starty<0) starty=0;
	if(endx>m_nLandWidth*MAP_SIZE) endx=m_nLandWidth*MAP_SIZE;
	if(endy>m_nLandHeight*MAP_SIZE) endy=m_nLandHeight*MAP_SIZE;


	int startworldx=startx/MAP_SIZE;
	int startworldy=starty/MAP_SIZE;
	int endworldx  =endx  /MAP_SIZE;
	int endworldy  =endy  /MAP_SIZE;

	if(startworldx<0) startworldx=0;
	if(startworldy<0) startworldy=0;
	if(endworldx>=m_nLandWidth) endworldx=m_nLandWidth-1;
	if(endworldy>=m_nLandHeight) endworldy=m_nLandHeight-1;

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,1);

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	D3DXMATRIX matWorld,matTemp;
//	D3DXMatrixIdentity( &matWorld );
//	matWorld._42=0.05;
//	m_pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp);
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);

	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,  color );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_D3DLANDSCAPEVERTEX );

	for(int wy=startworldy;wy<endworldy+1;wy++) 
	{
		for(int wx=startworldx;wx<endworldx+1;wx++) 
		{
			RenderWorldGrids(wx,wy,CPoint(startx,starty),CPoint(endx,endy),dx,color);
		}
	}
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,0);
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
}

void CWorld::RenderWorldGrids(int wx,int wy,CPoint ptLT,CPoint ptRB,WORD dx,DWORD color)
{
	CLandscape* pLand=GetLandscape(wx,wy);
	if(!pLand) return;
	if(!pLand->isVisibile()) return;
	int startx=ptLT.x-wx*MAP_SIZE;
	int starty=ptLT.y-wy*MAP_SIZE;
	int endx  =ptRB.x-wx*MAP_SIZE;
	int endy  =ptRB.y-wy*MAP_SIZE;

	if(startx<0) startx=0;
	if(starty<0) starty=0;
	if(endx>=MAP_SIZE) endx=MAP_SIZE-1;
	if(endy>=MAP_SIZE) endy=MAP_SIZE-1;

	int startpatchx=startx/PATCH_SIZE;
	int startpatchy=starty/PATCH_SIZE;
	int endpatchx  =endx  /PATCH_SIZE;
	int endpatchy  =endy  /PATCH_SIZE;

	WORD* pIB;
	int nCount=0;
	g_pGridIB->Lock( 0, 0, (void**)&pIB, 0 );
	for(int py=startpatchy;py<=endpatchy;py++) {
		for(int px=startpatchx;px<=endpatchx;px++) {
			if(!pLand->m_aPatches[py][px].isVisibile()) continue;

			int indexoffset= (px+py*NUM_PATCHES_PER_SIDE)*(PATCH_SIZE+1)*(PATCH_SIZE+1);

			int startgridx=startx-px*PATCH_SIZE;
			int startgridy=starty-py*PATCH_SIZE;
			int endgridx  =endx  -px*PATCH_SIZE;
			int endgridy  =endy  -py*PATCH_SIZE;

			if(startgridx<0) startgridx=0;
			if(startgridy<0) startgridy=0;
			if(endgridx>=PATCH_SIZE) endgridx=PATCH_SIZE-1;
			if(endgridy>=PATCH_SIZE) endgridy=PATCH_SIZE-1;

			if(endgridx+px*PATCH_SIZE+wx*MAP_SIZE==ptRB.x) endgridx--;
			if(endgridy+py*PATCH_SIZE+wy*MAP_SIZE==ptRB.y) endgridy--;

			if(dx==0) {
				for(int gy=startgridy;gy<=endgridy+1;gy++) {
					if(gy+py*PATCH_SIZE+wy*MAP_SIZE==ptLT.y || gy+py*PATCH_SIZE+wy*MAP_SIZE==ptRB.y) {
						for(int gx=startgridx;gx<endgridx+1;gx++) {
								pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
								pIB[nCount++]=(gx+1)+(gy*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
				for(int gx=startgridx;gx<=endgridx+1;gx++) {
					if(gx+px*PATCH_SIZE+wx*MAP_SIZE==ptLT.x || gx+px*PATCH_SIZE+wx*MAP_SIZE==ptRB.x) {
						for(int gy=startgridy;gy<endgridy+1;gy++) {
							pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
							pIB[nCount++]=gx+((gy+1)*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
			}
			else {
				for(int gy=startgridy;gy<=endgridy+1;gy++) {
					if((gy+py*PATCH_SIZE+wy*MAP_SIZE-ptLT.y)%dx==0) {
						for(int gx=startgridx;gx<endgridx+1;gx++) {
								pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
								pIB[nCount++]=(gx+1)+(gy*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
				for(int gx=startgridx;gx<=endgridx+1;gx++) {
					if((gx+px*PATCH_SIZE+wx*MAP_SIZE-ptLT.x)%dx==0) {
						for(int gy=startgridy;gy<endgridy+1;gy++) {
							pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
							pIB[nCount++]=gx+((gy+1)*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
			}
		}
	}

	g_pGridIB->Unlock();
	m_pd3dDevice->SetIndices( g_pGridIB );
	m_pd3dDevice->SetStreamSource( 0, pLand->m_pVB, 0, sizeof( D3DLANDSCAPEVERTEX ) );

	if(nCount>0)
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1) * ( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), 0, nCount / 2 );
}
