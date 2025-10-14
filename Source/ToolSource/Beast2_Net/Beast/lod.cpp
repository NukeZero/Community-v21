#include "stdafx.h"
#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
#include "CommandBeast.h"
#endif //__CSC_UNDO_REDO
#endif //__BEAST
#include "Sfx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
//#define __FULLWORLD // 이것을 넣으면 물,구름,높이속성,위치정보의 버텍스를 만들지 않는다.
/*
100 0 "water_riverAni0039.dds" 0 ""
    101 0 "water_riverAni0040.dds" 0 ""
    102 0 "water_riverAni0041.dds" 0 ""
    103 0 "water_riverAni0042.dds" 0 ""
    104 0 "water_riverAni0043.dds" 0 ""
    105 0 "water_riverAni0044.dds" 0 ""
    106 0 "water_riverAni0045.dds" 0 ""
    107 0 "water_riverAni0046.dds" 0 ""
    108 0 "water_riverAni0047.dds" 0 ""
    109 0 "water_riverAni0048.dds" 0 ""
    110 0 "water_riverAni0049.dds" 0 ""
    111 0 "water_riverAni0050.dds" 0 ""
    112 0 "water_riverAni0051.dds" 0 ""
    113 0 "water_riverAni0052.dds" 0 ""
    114 0 "water_riverAni0053.dds" 0 ""
    115 0 "water_riverAni0054.dds" 0 ""
*/
#define D3DCOLOR_ARGB16( a, r, g, b ) ( ( a >> 4 ) << 12 ) | ( ( r >> 4 ) << 8 ) | ( ( g >> 4 ) << 4 ) | ( b >> 4 )

FLOAT CLandscape::m_fCloud_u1 = 0.0f;
FLOAT CLandscape::m_fCloud_v1 = 0.0f;
FLOAT CLandscape::m_fCloud_u2 = 0.0f;
FLOAT CLandscape::m_fCloud_v2 = 0.0f;
#ifndef __WATER_EXT
FLOAT CLandscape::m_fWaterFrame = 0.0f;
#endif //__WATER_EXT

DWORD LODLEVELRANGE1 = (32*MPU);
DWORD LODLEVELRANGE2 = (64*MPU);
DWORD LODLEVELRANGE3 = (128*MPU);

void SetLODDetail( int nDetail )
{
	switch( nDetail ) 
	{
	case 0:
		LODLEVELRANGE1 = (64*MPU);
		LODLEVELRANGE2 = (128*MPU);
		LODLEVELRANGE3 = (256*MPU);
		break;
	case 1:
		LODLEVELRANGE1 = (32*MPU);
		LODLEVELRANGE2 = (64*MPU);
		LODLEVELRANGE3 = (128*MPU);
		break;
	case 2:
		LODLEVELRANGE1 = (16*MPU);
		LODLEVELRANGE2 = (32*MPU);
		LODLEVELRANGE3 = (64*MPU);
		break;
	}
}
// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

CPatch::~CPatch()
{
}

void CPatch::Init( int heightX, int heightY, int worldX, int worldY, FLOAT *hMap )
{
	m_nWorldX = worldX;
	m_nWorldY = worldY;

	m_pHeightMap = &hMap[heightY * (MAP_SIZE+1) + heightX];

	m_bVisible=FALSE;
	m_bDirty=TRUE;
	CalculateBound();
}

void CPatch::Render(LPDIRECT3DDEVICE9 pd3dDevice,int X,int Y)
{
#ifndef __WORLDSERVER
	if( pd3dDevice ) 
	{
		DWORD nBaseVertexIndex = ((PATCH_SIZE+1)*(PATCH_SIZE+1)) * (Y*NUM_PATCHES_PER_SIDE+X);
		pd3dDevice->SetIndices( g_pIB );
		if( CWorld::m_bViewLODTerrain ) 
		{
			if(m_nLevel==3) 
			{
				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel], 2 );
			}
			else 
			{
				if(m_nLevel!=m_nTopLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*0)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel], g_anPrimitive[m_nLevel] );
					//pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*0)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nLeftLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*1)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*1)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nRightLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*2)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*2)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nBottomLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*3)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*3)*3, g_anPrimitive[m_nLevel] );
			}
		}
		else 
		{
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1),0, 128 );
		}
	}
#endif // WORLDSERVER
}

void CPatch::CalculateBound()
{
	float maxy=-9999999.0f;
	float miny=9999999.0f;
	for(int i=0;i<=PATCH_SIZE;i++)
		for(int j=0;j<=PATCH_SIZE;j++)
		{
			float y = (float)m_pHeightMap[(i *(MAP_SIZE+1))+j ];
			// extra height
			if( y >= HGT_NOWALK )
			{
				if( y >= HGT_DIE )
					y -= HGT_DIE;
				else
				if( y >= HGT_NOMOVE )
					y -= HGT_NOMOVE;
				else
				if( y >= HGT_NOFLY )
					y -= HGT_NOFLY;
				else
					y -= HGT_NOWALK;
			}
			if(y>maxy) maxy=y;
			if(y<miny) miny=y;
		}
	m_avBounds[0] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY) * MPU ); // xyz
	m_avBounds[1] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, miny, (float)(m_nWorldY) * MPU ); // Xyz
	m_avBounds[2] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY) * MPU ); // xYz
	m_avBounds[3] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, maxy, (float)(m_nWorldY) * MPU ); // XYz
	m_avBounds[4] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // xyZ
	m_avBounds[5] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, miny, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // XyZ
	m_avBounds[6] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // xYZ
	m_avBounds[7] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, maxy, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // XYZ

	m_vCenter=(m_avBounds[0]+m_avBounds[1]+m_avBounds[2]+m_avBounds[3]+m_avBounds[4]+m_avBounds[5]+m_avBounds[6]+m_avBounds[7])/8;

	D3DXPlaneFromPoints( &m_aplaneBounds[0], &m_avBounds[0], 
		&m_avBounds[1], &m_avBounds[2] ); // Near
	D3DXPlaneFromPoints( &m_aplaneBounds[1], &m_avBounds[6], 
		&m_avBounds[7], &m_avBounds[5] ); // Far
	D3DXPlaneFromPoints( &m_aplaneBounds[2], &m_avBounds[2], 
		&m_avBounds[6], &m_avBounds[4] ); // Left
	D3DXPlaneFromPoints( &m_aplaneBounds[3], &m_avBounds[7], 
		&m_avBounds[3], &m_avBounds[5] ); // Right
	D3DXPlaneFromPoints( &m_aplaneBounds[4], &m_avBounds[2], 
		&m_avBounds[3], &m_avBounds[6] ); // Top
	D3DXPlaneFromPoints( &m_aplaneBounds[5], &m_avBounds[1], 
		&m_avBounds[0], &m_avBounds[4] ); // Bottom
}

void CPatch::Cull()
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( g_cullinfo.planeFrustum[iPlane].a * m_avBounds[iPoint].x +
                g_cullinfo.planeFrustum[iPlane].b * m_avBounds[iPoint].y +
                g_cullinfo.planeFrustum[iPlane].c * m_avBounds[iPoint].z +
                g_cullinfo.planeFrustum[iPlane].d < 0)
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        if( bOutside[iPoint] == 0 )
		{
			m_bVisible=TRUE;
			CalculateLevel();
            return ;
		}
    }
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
		m_bVisible=FALSE;
        return ;
    }
	m_bVisible=TRUE;
	if(CWorld::m_bViewLODTerrain) CalculateLevel();
}

void CPatch::CalculateLevel()
{
	float dist=D3DXVec3Length(&(CWorld::m_pCamera->m_vPos-m_vCenter));
	if(dist>LODLEVELRANGE3) m_nLevel=3;
	else if(dist>LODLEVELRANGE2) m_nLevel=2;
	else if(dist>LODLEVELRANGE1) m_nLevel=1;
	else m_nLevel=0;
}



// -------------------------------------------------------------------------------------------------
//	CLandscape CLASS
// -------------------------------------------------------------------------------------------------
/*
  width unit
	128  1   = 0  1 ~ 1.9
	64   2   = 1  2 ~ 3 
	32   4   = 2  4 ~ 7
	16   8   = 3  8 ~ 15  
	8    16  = 4
	4    32  = 5
	2    64  = 6
	1    128 = 7 128 ~ 255
*/
CLandscape::CLandscape()
{
	m_dwVersion = 0;
	m_pd3dDevice = NULL;
	m_pHeightMap = NULL;
	//m_aWaterHeight = NULL;
	m_bModified = TRUE;
	m_pVB=NULL;
	m_pWorld = NULL;
	m_pCloudVertexBuffer = NULL;
	m_pWaterVertexBuffer = NULL;
	m_pHgtAttrVertexBuffer = NULL;
	m_pLandAttrVertexBuffer = NULL;
	m_nCloudVertexNum = 0;
	m_nWaterVertexNum = 0;
	m_nHgtAttrVertexNum = 0;
	m_nLandAttrVertexNum = 0;

#ifdef __N_WATERFRAME_050202
	//	버텍스버퍼를 그림의 종류마다 만들어준다. WATERVERTEXBUFFER, * LPWATERVB new를 하라는 말이다.
	m_pWaterVB = new WATERVERTEXBUFFER[ prj.m_terrainMng.m_nWaterFrame ];
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
		ZeroMemory( &m_pWaterVB[ loop ], sizeof( WATERVERTEXBUFFER ) );
#endif//__N_WATERFRAME_050202
	
	//ZeroMemory( m_aObjLinkMap, sizeof(CObj*) * MAP_SIZE * MAP_SIZE );

	// 128 -> 32
	// 32  -> 16
	// 8   -> 4
	// 2   -> 0 -> 1
	int nWidth	= MAP_SIZE * 2;
	for( int i = 0; i < MAX_LINKLEVEL; i++ )
	{
		nWidth /= 2;
		if( nWidth == 0 ) nWidth = 1;
		// 128	// 64	// 32	// 16 // 8 // 4  -	// 4	// 8	// 16	// 32 // 64 // 128
		for( int j = 0; j < MAX_LINKTYPE; j++ )
		{
			m_apObjLink[j][i]	= new CObj*[nWidth*nWidth];
			memset( m_apObjLink[j][i], 0, sizeof(CObj*) * nWidth*nWidth );
		}
		m_nWidthLinkMap[i] = nWidth;
	}
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		m_apObject[ i ] = new CObj * [ 5000 ];
		m_adwObjNum[ i ] = 0;
	}

	//for(i=0;i<NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE;i++) m_aLandMark[i]=FALSE;

	m_pLightMapLayer = NULL;
}
CLandscape::~CLandscape()
{
	FreeTerrain();
#ifdef __N_WATERFRAME_050202
	//	버텍스버퍼를 그림의 종류마다 만들어준다. WATERVERTEXBUFFER, * LPWATERVB new를 하라는 말이다.
//	SAFE_DELETE_ARRAY( m_pWaterVB );
#endif//__N_WATERFRAME_050202
	
 	for( int i = 0; i < MAX_LINKLEVEL; i++ ) {
		for( int j = 0; j < MAX_LINKTYPE; j++ ) {
			SAFE_DELETE_ARRAY( m_apObjLink[j][i] );
		}
	}

	DeleteDeviceObjects( );
	InvalidateDeviceObjects( );

	//gmpbigsun : stop the leak!	
	//m_pWaterVB는 사용자 구조체로 이안에 LPDIRECT3DVERTEXBUFFER9만 릴리즈 하고있었다. 
	//m_pWaterVB가 new로 할당된 배열이기 때문에 릴리즈 이후 제거하도록 했다.
	SAFE_DELETE_ARRAY( m_pWaterVB );		
	
	for( int i = 0; i < m_aLayer.GetSize(); i++) {
		delete (CLandLayer*)m_aLayer.GetAt(i);
	}

	// 오브젝트 파괴 
	for( int i = 0; i < MAX_OBJARRAY; i++)
 	{
 		CObj** apObject = m_apObject[ i ];
 		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
 		{
 			if( m_pWorld->m_pObjFocus == apObject[ j ] )
 				m_pWorld->SetObjFocus( NULL );
 			if( CObj::m_pObjHighlight == apObject[ j ] )
 				CObj::m_pObjHighlight = NULL;
 			SAFE_DELETE_ARRAY( apObject[ j ] );
 		}
 		SAFE_DELETE_ARRAY( apObject );
 	}
}

//
// 디바이스 관련
//
HRESULT CLandscape::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
	m_pd3dDevice = pd3dDevice;
	m_pWorld = pWorld;

	FreeTerrain();
	m_pHeightMap = (FLOAT *)malloc( (MAP_SIZE+1) * (MAP_SIZE+1) * sizeof(FLOAT) );
	//m_aWaterHeight = new WATERHEIGHT[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];

#ifndef __NewWater
	ZeroMemory(m_aWaterHeight, MAP_SIZE * MAP_SIZE * sizeof(WATERHEIGHT));

#else
	ZeroMemory( m_aWaterHeight, NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE * sizeof( WATERHEIGHT) );
#endif
	
//	NewLandscape(INIT_TEX);

	return S_OK;
}

HRESULT CLandscape::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( m_pVB ) 
		return S_OK;

//#ifdef __BEAST
	if(pd3dDevice) 
		//pd3dDevice->CreateVertexBuffer( (PATCH_SIZE+1) * (PATCH_SIZE+1) * (NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE) *sizeof(D3DLANDSCAPEVERTEX),
		//								  D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_SYSTEMMEM, &m_pVB , NULL);
		pd3dDevice->CreateVertexBuffer( (PATCH_SIZE+1) * (PATCH_SIZE+1) * (NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE) *sizeof(D3DLANDSCAPEVERTEX),
										  D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB , NULL);
		
//#else
//	if(pd3dDevice) 
//		pd3dDevice->CreateVertexBuffer( (PATCH_SIZE+1) * (PATCH_SIZE+1) * (NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE) *sizeof(D3DLANDSCAPEVERTEX),
//										  D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM, //D3DPOOL_DEFAULT ,
//									  &m_pVB);
//#endif
	
	CPatch *patch;
	m_bDirty=TRUE;
	for ( int Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
	{
		for ( int X=0; X < NUM_PATCHES_PER_SIDE; X++ )
		{
			patch = &(m_aPatches[Y][X]);
			patch->Init( X*PATCH_SIZE, Y*PATCH_SIZE, m_nWorldX+X*PATCH_SIZE, m_nWorldY+Y*PATCH_SIZE, m_pHeightMap );
		}
	}
	CalculateBound();
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->RestoreDeviceObjects();
		}
	}
	// 물 버텍스 버퍼 만들기 
	MakeWaterVertexBuffer();
	MakeHgtAttrVertexBuffer();
	MakeLandAttrVertexBuffer();

	return S_OK;
}
HRESULT CLandscape::MakeWaterVertexBuffer()
{
#ifdef __FULLWORLD
	return S_OK;
#endif
	SAFE_RELEASE( m_pWaterVertexBuffer );
	SAFE_RELEASE( m_pCloudVertexBuffer );
	// 물 버텍스 버퍼 만들기 
#ifndef __N_WATERFRAME_050202
	WATERVERTEX* pVerticesWater;
#endif
	WATERVERTEX* pVerticesCloud;
	m_nWaterVertexNum = 0;
	m_nCloudVertexNum = 0;
	m_nLandAttrVertexNum = 0;
	
#ifdef __N_WATERFRAME_050202
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		SAFE_RELEASE( m_pWaterVB[ loop ].pVB );
		m_pWaterVB[ loop ].WaterVertexNum = 0;
	}
	WATERVERTEX**	pVAWater;
	byte byWaterTexture;
	pVAWater = new WATERVERTEX*[ prj.m_terrainMng.m_nWaterFrame ];
#endif//__N_WATERFRAME_050202

#ifndef __NewWater
	if (usingNewWater)
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
#ifndef __N_WATERFRAME_050202
				if (m_aWaterHeight[j + i * MAP_SIZE].byWaterTexture == WTYPE_WATER)
					m_nWaterVertexNum += 6;
				if (m_aWaterHeight[j + i * MAP_SIZE].byWaterTexture == WTYPE_CLOUD)
					m_nCloudVertexNum += 6;
#else
				byWaterTexture = m_aWaterHeight[j + i * MAP_SIZE].byWaterTexture;
				if ((byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
				{
					m_nWaterVertexNum += 6;
					m_pWaterVB[(byWaterTexture >> 2)].WaterVertexNum += 6;
				}
				if (byWaterTexture == WTYPE_CLOUD)
					m_nCloudVertexNum += 6;
#endif// not __N_WATERFRAME_050202
			}
		}
	}
	else
#endif
	for( int i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
#ifndef __N_WATERFRAME_050202
			if( m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ].byWaterTexture == WTYPE_WATER )
				m_nWaterVertexNum += 6;
			if( m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ].byWaterTexture == WTYPE_CLOUD )
				m_nCloudVertexNum += 6;
#else
			byWaterTexture = m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ].byWaterTexture;
			if( ( byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER )
			{
				m_nWaterVertexNum += 6;
				m_pWaterVB[ (byWaterTexture >> 2) ].WaterVertexNum += 6;
			}
			if( byWaterTexture == WTYPE_CLOUD )
				m_nCloudVertexNum += 6;
#endif// not __N_WATERFRAME_050202
		}
	}
	HRESULT hr;

#ifndef __N_WATERFRAME_050202
	if( m_nWaterVertexNum )
	{
		hr = m_pd3dDevice->CreateVertexBuffer( m_nWaterVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, &m_pWaterVertexBuffer, NULL );
		if( SUCCEEDED( hr ) ) 
			hr = m_pWaterVertexBuffer->Lock( 0, m_nWaterVertexNum * sizeof( WATERVERTEX ) , (void**)&pVerticesWater, 0 );
		if( FAILED( hr) ) 
			return hr;
	}
#else
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		if ( !m_pWaterVB[ loop ].WaterVertexNum )
			continue;
		hr = m_pd3dDevice->CreateVertexBuffer( m_pWaterVB[ loop ].WaterVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, 
			&m_pWaterVB[ loop ].pVB, NULL );
		if ( SUCCEEDED( hr ) )
		{
			hr = m_pWaterVB[ loop ].pVB->Lock( 0, m_pWaterVB[ loop ].WaterVertexNum * sizeof( WATERVERTEX ),
				(void**)&pVAWater[ loop ], 0 );
		}
		if ( FAILED( hr ) )
			return hr;
	}
#endif//__N_WATERFRAME_050202
	if( m_nCloudVertexNum )
	{
		hr = m_pd3dDevice->CreateVertexBuffer( m_nCloudVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, &m_pCloudVertexBuffer, NULL );
		if( SUCCEEDED( hr ) ) 
			hr = m_pCloudVertexBuffer->Lock( 0, m_nCloudVertexNum * sizeof( WATERVERTEX ) , (void**)&pVerticesCloud, 0 );
		if( FAILED( hr ) ) 
			return hr;
	}
	int nWorldX = m_nWorldX * MPU;
	int nWorldZ = m_nWorldY * MPU;

#ifndef __NewWater
	if (usingNewWater)
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				LPWATERHEIGHT lpWaterHeight = &m_aWaterHeight[j + i * MAP_SIZE];
#ifdef __N_WATERFRAME_050202
				if ((lpWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
#else
				if (lpWaterHeight->byWaterTexture == WTYPE_WATER && m_nWaterVertexNum)
#endif
				{
#ifndef __N_WATERFRAME_050202
					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU));
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 0.0f;
					pVerticesWater->v = 0.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;

					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU));
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 0.0f;
					pVerticesWater->v = 3.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;

					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));;
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 3.0f;
					pVerticesWater->v = 0.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;

					///////////////////////////////////////////

					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU));
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 0.0f;
					pVerticesWater->v = 3.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;

					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));;
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 3.0f;
					pVerticesWater->v = 0.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;

					pVerticesWater->p.x = (float)(nWorldX + (j * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));;
					pVerticesWater->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVerticesWater->p.z = (float)(nWorldZ + (i * PATCH_SIZE * MPU) + (PATCH_SIZE * MPU));
					pVerticesWater->n.x = 0;
					pVerticesWater->n.y = 1.0;
					pVerticesWater->n.z = 0;
					pVerticesWater->u = 3.0f;
					pVerticesWater->v = 3.0f;
					pVerticesWater->color = 0xaaaaaaaa;
					pVerticesWater++;
#else
#ifdef __WATER_EXT
					int loop = lpWaterHeight->byWaterTexture >> 2;

					if (loop >= 0 && loop < prj.m_terrainMng.m_nWaterFrame && m_pWaterVB[loop].WaterVertexNum > 0)
					{
#else //__WATER_EXT
					for (loop = 0; loop < prj.m_terrainMng.m_nWaterFrame; loop++)
					{
						if (!m_pWaterVB[loop].WaterVertexNum)
							continue;
#endif //__WATER_EXT
						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU));
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 0.0f;
						pVAWater[loop]->v = 0.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;

						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU));
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU) + ( 1 * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 0.0f;
						pVAWater[loop]->v = 3.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;

						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU) + (1 * MPU));
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 3.0f;
						pVAWater[loop]->v = 0.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;

						///////////////////////////////////////////

						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU));
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU) + (1 * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 0.0f;
						pVAWater[loop]->v = 3.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;

						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU) + (1 * MPU));;
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 3.0f;
						pVAWater[loop]->v = 0.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;

						pVAWater[loop]->p.x = (float)(nWorldX + (j * MPU) + (1 * MPU));;
						pVAWater[loop]->p.y = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
						pVAWater[loop]->p.z = (float)(nWorldZ + (i * MPU) + (1 * MPU));
						pVAWater[loop]->n.x = 0;
						pVAWater[loop]->n.y = 1.0;
						pVAWater[loop]->n.z = 0;
						pVAWater[loop]->u = 3.0f;
						pVAWater[loop]->v = 3.0f;
						pVAWater[loop]->color = 0xaaaaaaaa;
						pVAWater[loop]++;
					}
#endif
				}
				else if (lpWaterHeight->byWaterTexture == WTYPE_CLOUD && m_nCloudVertexNum)
				{
					// 1 3
					// 0 2
					pVerticesCloud->p.x = (float)(nWorldX + (j * MPU));
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 0.0f;
					pVerticesCloud->v = 0.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;

					pVerticesCloud->p.x = (float)(nWorldX + (j * MPU));
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU) + (1 * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 0.0f;
					pVerticesCloud->v = 1.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;

					pVerticesCloud->p.x = (float)(nWorldX + (j *  MPU) + (1 * MPU));;
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 1.0f;
					pVerticesCloud->v = 0.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;

					//////////////////////////////////

					pVerticesCloud->p.x = (float)(nWorldX + (j * MPU));
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU) + (1 * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 0.0f;
					pVerticesCloud->v = 1.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;

					pVerticesCloud->p.x = (float)(nWorldX + (j * MPU) + (1 * MPU));;
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 1.0f;
					pVerticesCloud->v = 0.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;

					pVerticesCloud->p.x = (float)(nWorldX + (j * MPU) + (1 * MPU));;
					pVerticesCloud->p.y = 0.0f;
					pVerticesCloud->p.z = (float)(nWorldZ + (i * MPU) + (1 * MPU));
					pVerticesCloud->n.x = 0;
					pVerticesCloud->n.y = 1.0;
					pVerticesCloud->n.z = 0;
					pVerticesCloud->u = 1.0f;
					pVerticesCloud->v = 1.0f;
					pVerticesCloud->color = 0xaaaaaaaa;
					pVerticesCloud++;
				}
			}
		}
	}
	else
#endif
	for( int i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			LPWATERHEIGHT lpWaterHeight = &m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ];
#ifdef __N_WATERFRAME_050202
			if( ( lpWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME) ) == WTYPE_WATER )
#else
			if( lpWaterHeight->byWaterTexture == WTYPE_WATER && m_nWaterVertexNum )
#endif//__N_WATERFRAME_050202
				
			{
				// 1 3
				// 0 2
				/*
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 1.0f / 4.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 1.0f / 4.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				///////////////////////////////////////////
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 1.0f / 4.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 1.0f / 4.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = 96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 1.0f / 4.0f;
				pVerticesWater->v     = 1.0f / 4.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
*/
	#ifndef __N_WATERFRAME_050202
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;

				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 3.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 3.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				///////////////////////////////////////////
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 0.0f;
				pVerticesWater->v     = 3.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 3.0f;
				pVerticesWater->v     = 0.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
				
				pVerticesWater->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesWater->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesWater->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesWater->n.x   = 0;
				pVerticesWater->n.y   = 1.0;
				pVerticesWater->n.z   = 0;
				pVerticesWater->u     = 3.0f;
				pVerticesWater->v     = 3.0f;
				pVerticesWater->color  = 0xaaaaaaaa;
				pVerticesWater++;
	#else
#ifdef __WATER_EXT
				int loop = lpWaterHeight->byWaterTexture >> 2;

				if ( loop >= 0 && loop < prj.m_terrainMng.m_nWaterFrame && m_pWaterVB[ loop ].WaterVertexNum > 0)
				{
#else //__WATER_EXT
				for ( loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
				{
					if ( !m_pWaterVB[ loop ].WaterVertexNum )
						continue;
#endif //__WATER_EXT
					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;

					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					///////////////////////////////////////////
					
					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
					pVAWater[ loop ]->p.y   = (FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
					pVAWater[ loop ]->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
				}
	#endif//__N_WATERFRAME_050202
			}
			else
			if( lpWaterHeight->byWaterTexture == WTYPE_CLOUD && m_nCloudVertexNum )
			{
				// 1 3
				// 0 2
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				//////////////////////////////////
				
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
			}
		}
	}
#ifndef __N_WATERFRAME_050202
	if( m_nWaterVertexNum )
		m_pWaterVertexBuffer->Unlock();
#else
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		if ( !m_pWaterVB[ loop ].WaterVertexNum )
			continue;
		m_pWaterVB[ loop ].pVB->Unlock();
	}
#endif//__N_WATERFRAME_050202
	if( m_nCloudVertexNum )
		m_pCloudVertexBuffer->Unlock();
#ifdef __N_WATERFRAME_050202
	SAFE_DELETE_ARRAY( pVAWater );
#endif//__N_WATERFRAME_050202
	return S_OK;
}
HRESULT CLandscape::MakeLandAttrVertexBuffer()
{
#ifdef __BEAST
#ifdef __FULLWORLD
	return S_OK;
#endif	
	//return 0;
	SAFE_RELEASE( m_pLandAttrVertexBuffer );
	// 물 버텍스 버퍼 만들기 
	WATERVERTEX* pVerticesLandAttr;
	m_nLandAttrVertexNum = 0;
	
	for( int i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			if( m_aLandAttr[ j + i * NUM_PATCHES_PER_SIDE ] )
				m_nLandAttrVertexNum += 6;
		}
	}
	HRESULT hr;
	if( m_nLandAttrVertexNum )
	{
		hr = m_pd3dDevice->CreateVertexBuffer( m_nLandAttrVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, &m_pLandAttrVertexBuffer, NULL );
		if( SUCCEEDED( hr ) ) 
			hr = m_pLandAttrVertexBuffer->Lock( 0, m_nLandAttrVertexNum * sizeof( WATERVERTEX ) , (void**)&pVerticesLandAttr, 0 );
		if( FAILED( hr ) ) 
			return hr;
	}
	int nWorldX = m_nWorldX * MPU;
	int nWorldZ = m_nWorldY * MPU;
	for( int i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			BYTE bByte = m_aLandAttr[ j + i * NUM_PATCHES_PER_SIDE ];
			if( bByte )
			{
				// 1 3
				// 0 2
				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 0.0f;
				pVerticesLandAttr->v     = 0.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;

				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 0.0f;
				pVerticesLandAttr->v     = 3.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;
				
				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 3.0f;
				pVerticesLandAttr->v     = 0.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;
				
				///////////////////////////////////////////
				
				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 0.0f;
				pVerticesLandAttr->v     = 3.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;
				
				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 3.0f;
				pVerticesLandAttr->v     = 0.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;
				
				pVerticesLandAttr->p.x   = (float)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );;
				pVerticesLandAttr->p.y   = 0;//(FLOAT)lpWaterHeight->byWaterHeight;////96.0f;
				pVerticesLandAttr->p.z   = (float)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesLandAttr->n.x   = 0;
				pVerticesLandAttr->n.y   = 1.0;
				pVerticesLandAttr->n.z   = 0;
				pVerticesLandAttr->u     = 3.0f;
				pVerticesLandAttr->v     = 3.0f;
				pVerticesLandAttr->color  = 0xaaaaaaaa;
				pVerticesLandAttr++;
			}
		}
	}
	if( m_nLandAttrVertexNum )
		m_pLandAttrVertexBuffer->Unlock();
#endif
	return S_OK;
}

HRESULT CLandscape::MakeHgtAttrVertexBuffer()
{
#ifdef _DEBUG
	//return S_OK;		//gmpbigsun : 왜 디버그에선? 이걸 만들어야 못가는지역등의 정보를 볼수있다.
#endif

#ifdef __BEAST
#ifdef __FULLWORLD
	return S_OK;
#endif
	SAFE_RELEASE( m_pHgtAttrVertexBuffer );
	
	// 물 버텍스 버퍼 만들기 
	HGTATTRVERTEX* pVertices;
	m_nHgtAttrVertexNum = 0;
	
	for( int i = 0; i < MAP_SIZE; i++ )
	{
		for( int j = 0; j < MAP_SIZE; j++ )
		{
			if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_NOWALK )
				m_nHgtAttrVertexNum += 6;
		}
	}
	if( m_nHgtAttrVertexNum == 0 )
		return S_OK;
	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( m_nHgtAttrVertexNum * sizeof( HGTATTRVERTEX ),
		D3DUSAGE_WRITEONLY, D3DFVF_HGTATTRVERTEX, D3DPOOL_MANAGED, &m_pHgtAttrVertexBuffer, NULL );
	if( SUCCEEDED( hr) ) 
		hr = m_pHgtAttrVertexBuffer->Lock( 0, m_nHgtAttrVertexNum * sizeof( HGTATTRVERTEX ) , (void**)&pVertices, 0 );

	int nWorldX = m_nWorldX * MPU;
	int nWorldZ = m_nWorldY * MPU;
	for( int i = 0; i < MAP_SIZE; i++ )
	{
		for( int j = 0; j < MAP_SIZE; j++ )
		{
			if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_NOWALK )
			{
				DWORD dwColor = 0x80ffffff;
				if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_DIE )
					dwColor = 0x80ff0000;
				else
				if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_NOMOVE )
					dwColor = 0x80ffffff;
				else
				if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_NOFLY )
					dwColor = 0x800000ff;
				else
				if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] >= HGT_NOWALK )
					dwColor = 0x8000ff00;
				
				FLOAT fHeight;
				// 1 2 1 3 
				// 0 3 0 2 
				if( j % 2 == i % 2)
				{
					fHeight = GetHeightMap( j + i * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;

					fHeight = GetHeightMap( j + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;

					////////////////////////////////////////////////////////////

					fHeight = GetHeightMap( j + i * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + ( i + 0 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );// + ( MPU );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
				}
				else
				{
					fHeight = GetHeightMap( j + i * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( j + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + i * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );// + ( MPU );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;

					fHeight = GetHeightMap( j + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + i * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );// + ( MPU );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
					
					fHeight = GetHeightMap( ( j + 1 ) + ( i + 1 ) * ( MAP_SIZE + 1 ) );
					pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
					pVertices->p.y   = fHeight;
					pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
					pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
					pVertices->color  = dwColor;
					pVertices++;
				}
				
				///////////////////////////////////////////
				/*
				fHeight = GetHeightMap( j + (i + 1) * ( MAP_SIZE + 1 ) );
				pVertices->p.x   = (float)( nWorldX + ( j * MPU ) );
				pVertices->p.y   = fHeight;
				pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );
				pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				pVertices->color  = dwColor;
				pVertices++;
				
				fHeight = GetHeightMap( (j + 1) + i * ( MAP_SIZE + 1 ) );
				pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
				pVertices->p.y   = fHeight;
				pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) + ( MPU ) );
				pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				pVertices->color  = dwColor;
				pVertices++;
				
				fHeight = GetHeightMap( (j + 1) + (i + 1) * ( MAP_SIZE + 1 ) );
				pVertices->p.x   = (float)( nWorldX + ( j * MPU ) + ( MPU ) );
				pVertices->p.y   = fHeight;
				pVertices->p.z   = (float)( nWorldZ + ( i * MPU ) );
				pVertices->n     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				pVertices->color  = dwColor;
				pVertices++;
				*/
			}
		}
	}
	if( m_pHgtAttrVertexBuffer )
		m_pHgtAttrVertexBuffer->Unlock();
#endif
	return S_OK;
}

HRESULT CLandscape::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pCloudVertexBuffer );// = NULL;
	SAFE_RELEASE( m_pWaterVertexBuffer );// = NULL;
	SAFE_RELEASE( m_pHgtAttrVertexBuffer );// = NULL;
#ifdef __N_WATERFRAME_050202
	for ( int loop =0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
		SAFE_RELEASE( m_pWaterVB[ loop ].pVB );
#endif//__N_WATERFRAME_050202

#ifdef __LOCATION
	SAFE_RELEASE( m_pLandAttrVertexBuffer );
#endif
	//m_nCloudVertexNum = 0;
	//m_nWaterVertexNum = 0;
	
	// 매모리풀이 default이므로 비디오메모리에 잡힌다. 따라서 
	// InvalidateDeviceObjects에서 파괴해야한다.
	//SAFE_RELEASE( m_texMiniMap.m_pTexture );
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->InvalidateDeviceObjects();
		}
	}

	return S_OK;
}
HRESULT CLandscape::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_texMiniMap.m_pTexture );
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->DeleteDeviceObjects();
		}
	}
	for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		CLandLayer* pLandLayer = (CLandLayer*)m_aLayer.GetAt( i );
		SAFE_RELEASE( pLandLayer->m_pLightMap );
	}
	return S_OK;
}

//
// 높이 맵 초기화 등

//
void CLandscape::FreeTerrain()
{
//	SAFE_DELETE(m_pHeightMap);		// <- malloc으로 한건데 왜 delete로 지우냥.
	if( m_pHeightMap )
	{
		free( m_pHeightMap );
		m_pHeightMap = NULL;
	}


	//SAFE_DELETE(m_aWaterHeight);
	//m_pHeightMap=NULL;
}
void CLandscape::ResetTerrain(DWORD dwInitHeight, BYTE* pHeightMap)
{
	m_dwHeightDefault = dwInitHeight;

	int i,j;
	if(pHeightMap) 
	{
		for( i = 0; i < (MAP_SIZE+1) * (MAP_SIZE+1); i++ )
		{
			m_pHeightMap[ i ] = (FLOAT)(pHeightMap[i]);
		}
	}
	else 
	{
		for( i = 0; i < (MAP_SIZE+1) * (MAP_SIZE+1); i++ )
			m_pHeightMap[ i ] = (FLOAT)dwInitHeight;
	}

	for(i=0;i<NUM_PATCHES_PER_SIDE;i++)
	{
		for(j=0;j<NUM_PATCHES_PER_SIDE;j++)
			m_aPatches[i][j].SetDirty(TRUE);
	}
	m_bDirty = TRUE;

}
void CLandscape::NewLandscape( DWORD dwTextureId )
{
	NewLayer( (WORD)( dwTextureId ) );
	/*
	for( i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			if( m_pHeightMap[ ( j * PATCH_SIZE ) + ( i * PATCH_SIZE ) * ( MAP_SIZE + 1 ) ] == 0 )
			{
				m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ] = WTYPE_CLOUD;
			}
		}
	}
	*/
#ifdef __BEAST
	for( int i = 0; i < MAP_SIZE; i++ )
	{
		for( int j = 0; j < MAP_SIZE; j++ )
		{
			if( m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] == 0 )
			{
				m_pHeightMap[ j + i * ( MAP_SIZE + 1 ) ] = (FLOAT)HGT_NOMOVE;//(pHeightMap[i]);
#ifndef __NewWater
				if (usingNewWater)
					m_aWaterHeight[j + i * MAP_SIZE].byWaterTexture = WTYPE_CLOUD;
				else
#endif				
				m_aWaterHeight[ ( j / PATCH_SIZE ) + ( i / PATCH_SIZE ) * NUM_PATCHES_PER_SIDE ].byWaterTexture = WTYPE_CLOUD;
				SetTex( j, i, 9, 255, FALSE );
			}
		}
	}
#endif	
//void CLandscape::SetTex(int x, int z, WORD nTex, BYTE nOpacity,BOOL bLandMark)	
/*
	CPatch *patch;
	int X, Y;
	for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
		for ( X=0; X < NUM_PATCHES_PER_SIDE; X++ )
		{
			patch = &(m_aPatches[Y][X]);
			patch->SetMainTexture((WORD)dwTextureId);
			patch->SetSubTexture((WORD)dwTextureId);
			prj.m_terrainMng.LoadTexture( m_pd3dDevice, patch->GetMainTexture() );
		}
*/
}

//
// 렌더 관련
//
void CLandscape::SetVertices()
{
	CPatch *patch = NULL;
	D3DLANDSCAPEVERTEX* pVB = NULL;
	if(m_pVB) 
	{
		m_pVB->Lock( 0, 0, (void**)&pVB, 0 );
		int nV=0;
		FLOAT fTemp=1.0f/MAP_SIZE;		//0.0078125
		FLOAT fTemp2=fTemp*(PATCH_SIZE-1)*NUM_PATCHES_PER_SIDE/MAP_SIZE;
		D3DXVECTOR3 v1,v2,v3,v4,vTemp,vTempPos,vTempNormal;
		for ( int Y = 0; Y < NUM_PATCHES_PER_SIDE; Y++)
		{
			for ( int X = 0; X < NUM_PATCHES_PER_SIDE; X++ )
			{
				patch = &(m_aPatches[ Y ][ X ]);
				if( patch->isDirty() ) {
					patch->SetDirty( FALSE );
					for( int i = 0; i < PATCH_SIZE + 1; i++ )
					{
						for( int j = 0; j < PATCH_SIZE + 1; j++ )
						{
							float fHeight  = GetHeightMap( ( ( i + Y * PATCH_SIZE ) * ( MAP_SIZE + 1 ) ) + ( j + X * PATCH_SIZE ) );
							vTempPos=pVB[nV].p=D3DXVECTOR3((float) ( X*PATCH_SIZE+j+m_nWorldX ) * MPU,fHeight,(float) ( Y * PATCH_SIZE + i + m_nWorldY ) * MPU );
							//if() // ( i % 8 ) && !( j % 8 ) )
							{
								vTempNormal=D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
								if((j-1+X*PATCH_SIZE)>0)
									v1=D3DXVECTOR3((float) (X*PATCH_SIZE+j-1+m_nWorldX) * MPU,(float)GetHeightMap(((i+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j-1+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+m_nWorldY) * MPU);
								if((i+1+Y*PATCH_SIZE)<MAP_SIZE+1)
									v2=D3DXVECTOR3((float) (X*PATCH_SIZE+j+m_nWorldX) * MPU,(float)GetHeightMap(((i+1+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+1+m_nWorldY) * MPU);
								if((j+1+X*PATCH_SIZE)<MAP_SIZE+1)
									v3=D3DXVECTOR3((float) (X*PATCH_SIZE+j+1+m_nWorldX) * MPU,(float)GetHeightMap(((i+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+1+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+m_nWorldY) * MPU);
								if((i-1+Y*PATCH_SIZE)>0)
									v4=D3DXVECTOR3((float) (X*PATCH_SIZE+j+m_nWorldX) * MPU,(float)GetHeightMap(((i-1+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i-1+m_nWorldY) * MPU);

								if( (j-1+X*PATCH_SIZE)>0 && (i+1+Y*PATCH_SIZE)<MAP_SIZE+1 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v1),&(v1-v2));
									vTempNormal+=vTemp;
								}
								if( (i+1+Y*PATCH_SIZE)<MAP_SIZE+1 && (j+1+X*PATCH_SIZE)<MAP_SIZE+1 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v2),&(v2-v3));
									vTempNormal+=vTemp;
								}
								if( (j+1+X*PATCH_SIZE)<MAP_SIZE+1 && (i-1+Y*PATCH_SIZE)>0 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v3),&(v3-v4));
									vTempNormal+=vTemp;
								}
								if( (i-1+Y*PATCH_SIZE)>0 && (j-1+X*PATCH_SIZE)>0 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v4),&(v4-v1));
									vTempNormal+=vTemp; 
								}
								D3DXVec3Normalize(&(vTempNormal),&(vTempNormal));
								pVB[nV].n=vTempNormal;
							}
		/*
							float alpha=(float)(j+m_nWorldX)/PATCH_SIZE;
							float alpha1;
							alpha1= ((int)alpha)%2 + (alpha-(int)alpha);
							alpha=(alpha1<1.0f)?alpha1:2.0f-alpha1;
							alpha=1.0f;
		*/
							/*
							if( GetLandMark(X,Y)) {
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) );
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) );
								pVB[nV].tu2=(float)( (float)(j*fTemp2) + ((float)X*PATCH_SIZE/MAP_SIZE) + (fTemp/2) ) * 1;
								pVB[nV].tv2=(float)( (float)(i*fTemp2) + ((float)Y*PATCH_SIZE/MAP_SIZE) + (fTemp/2) ) * 1;
							}
							else
							*/
							{
							//	float u1 = (float)( (float)((FLOAT)j)/(PATCH_SIZE) ) * 3;
							//	float v1 = (float)( (float)((FLOAT)i)/(PATCH_SIZE) ) * 3;
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) ) * 3; //MPU;
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) ) * 3; //MPU;

							//	float u2 = (float) (fTemp2*(j+X*(PATCH_SIZE)) + (fTemp/2) ) * 1.0f;
							//	float v2 = (float) (fTemp2*(i+Y*(PATCH_SIZE)) + (fTemp/2) ) * 1.0f;
								pVB[nV].tu2=(float) (fTemp2*(j+X*(PATCH_SIZE)) + (fTemp/2) ) * 1.0f;
								pVB[nV].tv2=(float) (fTemp2*(i+Y*(PATCH_SIZE)) + (fTemp/2) ) * 1.0f;

								/*
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) ) * MPU;
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) ) * MPU;
								pVB[nV].tu2=(float) (fTemp2*(j+X*(PATCH_SIZE)) + (fTemp/2) ) * 1;
								pVB[nV].tv2=(float) (fTemp2*(i+Y*(PATCH_SIZE)) + (fTemp/2) ) * 1;
								*/
							}
							nV++;
						}
					}
				}
				else nV+=(PATCH_SIZE+1)*(PATCH_SIZE+1);
#ifdef __BEAST
				CheckAllLayer(X,Y);	//sun
#endif
			}
		}
		m_pVB->Unlock();
	}
#ifdef __BEAST
	UpdateAllLayer();
#endif
	m_bDirty=FALSE;
}
void CLandscape::RenderPatches()
{
	int X, Y;
	CPatch* patch;
	CLandLayer* pLayer;
	int nBlendStatus=0;
	
	for(int i = 0; i < NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE; i++ )
		m_abPatchRendered[ i ] = FALSE;

	for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*)(m_aLayer[i]);

		if( pLayer->m_bVisible == FALSE )
			continue;

		LPTERRAIN pTerrain = prj.m_terrainMng.GetTerrain( pLayer->m_nTex );
		if( pTerrain && pTerrain->m_pTexture == NULL )
			prj.m_terrainMng.LoadTexture( pLayer->m_nTex );

		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
			continue;

		m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture );
		prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture->GetLevelCount();
		m_pd3dDevice->SetTexture( 1, pLayer->m_pLightMap );	

		m_pd3dDevice->SetIndices( g_pIB );

		for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
		{
			for ( X=0; X < NUM_PATCHES_PER_SIDE; X++ )
			{
				patch = &(m_aPatches[Y][X]);
				if( patch->isVisibile() ) 
				{
					//DWORD dwColor = pLayer->m_pTempLightMap[ X+Y*NUM_PATCHES_PER_SIDE ];
					//if( (dwColor >> 24) < 2 )
					//	continue;

					if( pLayer->m_aPatchEnable[X+Y*NUM_PATCHES_PER_SIDE] ) 
					{

						if( m_abPatchRendered[X+Y*NUM_PATCHES_PER_SIDE]) 
						{
							if( nBlendStatus == 0 )  
							{
								m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);		
								nBlendStatus=1;
							}

						}
						else 
						{
							m_abPatchRendered[X+Y*NUM_PATCHES_PER_SIDE]=TRUE;
							if( nBlendStatus == 1 ) 
							{
								m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
								nBlendStatus=0;
							}
						}

						patch->Render(m_pd3dDevice,X,Y);
					}

				}
			}
		}
	}
}
HRESULT CLandscape::Render(LPDIRECT3DDEVICE9 pd3dDevice,BOOL bLod)
{
	/*
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
//	pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_WIREFRAME );
*/
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

#if 1
	//기존 방식
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

//	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
//	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
#else
	//gmpbigsun: 스플래팅을 하려면 아래처럼-----------------------------------
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

//	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA    );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//gmpbigsun end-----------------------------------------------------------
#endif

	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );
	//  
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

//	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
//	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetVertexDeclaration( NULL );
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_D3DLANDSCAPEVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( D3DLANDSCAPEVERTEX ) );

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );


//	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 2);
	//float fBias=-.7f;
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
	
/*
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
*/
//	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	RenderPatches();

//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );		
	return S_OK;
	
}
//////////////////////////////////////////////////////////////
HRESULT CLandscape::RenderWater( LPDIRECT3DDEVICE9 pd3dDevice )
{
//	return S_OK;
	D3DXMATRIX mat,matView,matProj;
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DXMatrixIdentity( &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
/*
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
*/
	// 랜더 워터 
/*
	int nDivWater1 = m_nWaterFrame / 4;
	int nDivWater2 = m_nWaterFrame % 4;
	mat._31 = ( 1.0f / 4.0f ) * (FLOAT)nDivWater2;  //m_fCloud_u1;
	mat._32 = ( 1.0f / 4.0f ) * (FLOAT)nDivWater1;  //m_fCloud_v1;
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
	m_nWaterFrame++;
	if( m_nWaterFrame >= 16 )
	m_nWaterFrame = 0;
	m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 11 )->m_pTexture );
	*/
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 200, 0, 0, 0) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF ( D3DFVF_WATERVERTEX ); 
	//	이 버텍스 버퍼를 여러개를 만들어서 뿌릴수 있도록 만들어 주어야 한다.
#ifndef __N_WATERFRAME_050202
	m_pd3dDevice->SetStreamSource( 0, m_pWaterVertexBuffer, 0, sizeof(WATERVERTEX) );

	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );


	if( m_nWaterVertexNum )
	{
		//int nTemp = ( m_fWaterFrame - (int)m_fWaterFrame ) * 10;
		// 10 : nTemp = 255 : x
		//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255 - ( nTemp * 255 / 10 ), 0, 0, 0 ) );
		//	현재는 버텍스 버퍼를 한 개 가지고 모든 물을 뿌려주고 있기때문에 
		//	이런 형태로 찍어줘도 상관 없지만 늘어나게 되면 프레임도 물마다 달라지게 될것이므로 
		//	m_fWaterFrame로 돌아가는 것들도 2개로 나누어야 할것이다.
		m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 20 + m_fWaterFrame )->m_pTexture );

		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nWaterVertexNum/3);
	}

#else
	for ( int i = 0 ; i < prj.m_terrainMng.m_nWaterFrame ; i++)
    {
		if ( !m_pWaterVB[ i ].WaterVertexNum )
			continue;

		m_pd3dDevice->SetStreamSource( 0, m_pWaterVB[ i ].pVB, 0, sizeof(WATERVERTEX) );
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

#ifdef __WATER_EXT
		if(prj.m_terrainMng.m_fWaterFrame[i] >= static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt))
			prj.m_terrainMng.m_fWaterFrame[i] = static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt - 1);

		//!!gmpbigsun : 이건 아닌듯 인덱스로 -1를 뽑는데..-_-  일단 문제는 없게 수정함090810 
		int idx = static_cast<int>(prj.m_terrainMng.m_fWaterFrame[i]);
		if( idx > -1 )
		{
			DWORD dwID = prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ idx ];
			LPTERRAIN pTerrain = prj.m_terrainMng.GetTerrain( dwID );

			//gmpbigsun: 텍스쳐 로딩을 어디서도 하지 않아서, 일단 여기서 하는걸로...
			if( NULL == pTerrain->m_pTexture )
				prj.m_terrainMng.LoadTexture( dwID );
			LPDIRECT3DTEXTURE9 pTexture = prj.m_terrainMng.GetTerrain( dwID )->m_pTexture;
			m_pd3dDevice->SetTexture(0, pTexture );
//			prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ static_cast<int>(prj.m_terrainMng.m_fWaterFrame[i]) ] )->m_pTexture);
		}
#else //__WATER_EXT
		if ( m_fWaterFrame > 40)
			m_fWaterFrame = m_fWaterFrame;
		LPDIRECT3DTEXTURE9 Texture = prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ (int)m_fWaterFrame ] )->m_pTexture;
		m_pd3dDevice->SetTexture(0, prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ (int)m_fWaterFrame ] )->m_pTexture);
#endif //__WATER_EXT
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_pWaterVB[ i ].WaterVertexNum / 3);
    }
#endif//__N_WATERFRAME_050202

	//3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nTemp * 255 / 10, 0, 0, 0 ) );
	//nt nWaterFrame = m_fWaterFrame + 1;
	//if( nWaterFrame >= 16 ) nWaterFrame = 0;
	//m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 20 + nWaterFrame )->m_pTexture );
	//m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nWaterVertexNum/3);
	
/*
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	// 시간에 따라 하늘 텍스쳐를 자연스럽게 바꿔치기한다.
	if(g_GameTimer.m_nHour<6) {
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture3);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
	}
	else if(g_GameTimer.m_nHour==6) {
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255-(g_GameTimer.m_nMin*255/59), 0, 0, 0 ) );
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture3);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( g_GameTimer.m_nMin*255/59, 0, 0, 0 ) );
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
*/
#ifdef __CLOUD_ANIMATION_BUG
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
#endif // __CLOUD_ANIMATION_BUG
	if( m_nCloudVertexNum )
	{
		// 랜더 구름 
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		// TODO : 
		// 10번이 구름텍스쳐로 지정되어있는데 "Terrain.inc" 이건뭐 어쩌라는거임..
		// 이 텍스쳐를 바꾸고 싶다고 하는데
		// 일단 툴에서 구름텍스쳐 이름을 지정하게 하고
		// 지정되어 있는경우 강제로 해당 텍스쳐로 바꿔주자.
		// 작업은 4월 중순까지 하자 

		if( m_pWorld->m_pTextureSeaCloud )
			m_pd3dDevice->SetTexture( 0, m_pWorld->m_pTextureSeaCloud );
		else		
			m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 10 )->m_pTexture );

		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetFVF ( D3DFVF_WATERVERTEX );
		m_pd3dDevice->SetStreamSource( 0, m_pCloudVertexBuffer, 0, sizeof( WATERVERTEX ) );

		mat._31 = m_fCloud_u1;
		mat._32 = m_fCloud_v1;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		D3DXMatrixTranslation( &mat, 0.0f, 40.0f, 0.0f );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nCloudVertexNum / 3 );

		mat._31 = m_fCloud_u2;
		mat._32 = m_fCloud_v2;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		D3DXMatrixTranslation( &mat, 0.0f, 80.0f, 0.0f );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nCloudVertexNum / 3 );

		m_fCloud_u1 += 0.001f;
		m_fCloud_v1 += 0.001f;
		m_fCloud_u2 += 0.0015f;
		m_fCloud_v2 += 0.0015f;
	}
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	// 매번 갱신하면 엄청난 프레임 저하...
//	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
//	pBeastView->Invalidate( FALSE );
	
	return S_OK;
}
HRESULT CLandscape::RenderHeightAttribute( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __BEAST
	if( m_nHgtAttrVertexNum == 0 )
		return S_OK;

	D3DXMATRIX mat,matView,matProj;
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DXMatrixIdentity( &mat );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_DIFFUSE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	
	D3DXMatrixTranslation( &mat, 0.0f, 0.0f, 0.0f );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	{
		// Render Height Attribute
		m_pd3dDevice->SetTexture( 0, NULL );
		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetFVF ( D3DFVF_HGTATTRVERTEX );

		m_pd3dDevice->SetStreamSource( 0, m_pHgtAttrVertexBuffer, 0, sizeof( HGTATTRVERTEX ) );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nHgtAttrVertexNum/3);
	//return S_OK;

	}

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
#endif	
	return S_OK;
}

//
// 컬링 관련
//
void CLandscape::CalculateBound()
{
	if( m_bDirty ) 
	{
		int i,j;
		SetVertices();
		float maxy=-9999999.0f;
		float miny=9999999.0f;

		for(i=0;i<=MAP_SIZE;i++)
			for(j=0;j<=MAP_SIZE;j++)
			{
				float y = (float)GetHeightMap((i *(MAP_SIZE+1))+j );
				if(y>maxy) maxy=y;
				if(y<miny) miny=y;
			}
		m_avBounds[0] = D3DXVECTOR3( (float)(m_nWorldX)*MPU, miny, (float)m_nWorldY * MPU ); // xyz
		m_avBounds[1] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, miny, (float)m_nWorldY * MPU ); // Xyz
		m_avBounds[2] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)m_nWorldY * MPU ); // xYz
		m_avBounds[3] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, maxy, (float)m_nWorldY * MPU ); // XYz
		m_avBounds[4] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY+MAP_SIZE) * MPU); // xyZ
		m_avBounds[5] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, miny, (float)(m_nWorldY+MAP_SIZE) * MPU); // XyZ
		m_avBounds[6] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY+MAP_SIZE) * MPU ); // xYZ
		m_avBounds[7] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, maxy, (float)(m_nWorldY+MAP_SIZE) * MPU); // XYZ

		D3DXPlaneFromPoints( &m_aplaneBounds[0], &m_avBounds[0], 
			&m_avBounds[1], &m_avBounds[2] ); // Near
		D3DXPlaneFromPoints( &m_aplaneBounds[1], &m_avBounds[6], 
			&m_avBounds[7], &m_avBounds[5] ); // Far
		D3DXPlaneFromPoints( &m_aplaneBounds[2], &m_avBounds[2], 
			&m_avBounds[6], &m_avBounds[4] ); // Left
		D3DXPlaneFromPoints( &m_aplaneBounds[3], &m_avBounds[7], 
			&m_avBounds[3], &m_avBounds[5] ); // Right
		D3DXPlaneFromPoints( &m_aplaneBounds[4], &m_avBounds[2], 
			&m_avBounds[3], &m_avBounds[6] ); // Top
		D3DXPlaneFromPoints( &m_aplaneBounds[5], &m_avBounds[1], 
			&m_avBounds[0], &m_avBounds[4] ); // Bottom

		for(i=0;i<NUM_PATCHES_PER_SIDE;i++)
			for(j=0;j<NUM_PATCHES_PER_SIDE;j++)
				m_aPatches[j][i].CalculateBound();
	}
}

void CLandscape::UpdateCull()
{
	Cull();
	if(m_bVisible)
		for(int i=0;i<NUM_PATCHES_PER_SIDE;i++)
			for(int j=0;j<NUM_PATCHES_PER_SIDE;j++)
				m_aPatches[j][i].Cull();
	for(int j=0;j<NUM_PATCHES_PER_SIDE;j++)
		for(int i=0;i<NUM_PATCHES_PER_SIDE;i++)
		{
			if(m_aPatches[j][i].isVisibile()) {
				if(j>0) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j-1][i].m_nLevel)
						m_aPatches[j][i].m_nTopLevel=m_aPatches[j-1][i].m_nLevel;
					else m_aPatches[j][i].m_nTopLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nTopLevel=m_aPatches[j][i].m_nLevel;

				if(i<NUM_PATCHES_PER_SIDE-1) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j][i+1].m_nLevel)
						m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i+1].m_nLevel;
					else
						m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i].m_nLevel;

				if(i>0) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j][i-1].m_nLevel)
						m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i-1].m_nLevel;
					else
						m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i].m_nLevel;

				if(j<NUM_PATCHES_PER_SIDE-1) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j+1][i].m_nLevel)
						m_aPatches[j][i].m_nBottomLevel=m_aPatches[j+1][i].m_nLevel;
					else
						m_aPatches[j][i].m_nBottomLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nBottomLevel=m_aPatches[j][i].m_nLevel;
			}
		}
}

void CLandscape::Cull()
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( g_cullinfo.planeFrustum[iPlane].a * m_avBounds[iPoint].x +
                g_cullinfo.planeFrustum[iPlane].b * m_avBounds[iPoint].y +
                g_cullinfo.planeFrustum[iPlane].c * m_avBounds[iPoint].z +
                g_cullinfo.planeFrustum[iPlane].d < 0)
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        // If any point is inside all 6 frustum planes, it is inside
        // the frustum, so the object must be rendered.
        if( bOutside[iPoint] == 0 )
		{
			m_bVisible=TRUE;
            return ;
		}
    }
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
		m_bVisible=FALSE;
        return ;
    }
	m_bVisible=TRUE;
}

//
// 지형 에디트용 함수

//
void CLandscape::SetDirtyPatch(DWORD x,DWORD z)
{
	if(x<0 || x>=MAP_SIZE || z<0 || z>=MAP_SIZE) return;
	int px=x/PATCH_SIZE;
	int pz=z/PATCH_SIZE;
	m_aPatches[pz][px].SetDirty(TRUE);
	m_bDirty=TRUE;
}

FLOAT CLandscape::GetHeight(FLOAT x,FLOAT z)
{
	if(x<0 || x>=MAP_SIZE || z<0 || z>=MAP_SIZE) return .0f;
	int px,pz;
	FLOAT dx,dz;
	FLOAT dy1,dy2,dy3,dy4;
	px=(int)x;
	pz=(int)z;
	dx=x-px;
	dz=z-pz;

	FLOAT y1=GetHeightMap(px+pz*(MAP_SIZE+1));
	FLOAT y2=GetHeightMap(px+1+pz*(MAP_SIZE+1));
	FLOAT y3=GetHeightMap(px+(pz+1)*(MAP_SIZE+1));
	FLOAT y4=GetHeightMap(px+1+(pz+1)*(MAP_SIZE+1));
	dy1=y1*(1-dx)*(1-dz);
	dy2=y2*dx*(1-dz);
	dy3=y3*(1-dx)*dz;
	dy4=y4*dx*dz;
	return dy1+dy2+dy3+dy4;
}

FLOAT CLandscape::GetHeight(DWORD x,DWORD z)
{
	if(x<0 || x>MAP_SIZE || z<0 || z>MAP_SIZE) return .0f;
	return GetHeightMap(x+z*(MAP_SIZE+1));
}

FLOAT CLandscape::GetHeight(POINT pos)
{
	return GetHeight((DWORD)pos.x,(DWORD)pos.y);
}

void CLandscape::SetHeight(DWORD x,DWORD z,FLOAT factor)
{
	if(x<0 || x>MAP_SIZE || z<0 || z>MAP_SIZE) return;
	SetModified( TRUE );
	if( m_pHeightMap[x+z*(MAP_SIZE+1)] >= HGT_NOWALK )
	{
		if( m_pHeightMap[x+z*(MAP_SIZE+1)] >= HGT_DIE )
			m_pHeightMap[x+z*(MAP_SIZE+1)] = (FLOAT)factor + HGT_DIE;
		else
		if( m_pHeightMap[x+z*(MAP_SIZE+1)] >= HGT_NOMOVE )
			m_pHeightMap[x+z*(MAP_SIZE+1)] = (FLOAT)factor + HGT_NOMOVE;
		else
		if( m_pHeightMap[x+z*(MAP_SIZE+1)] >= HGT_NOFLY )
			m_pHeightMap[x+z*(MAP_SIZE+1)] = (FLOAT)factor + HGT_NOFLY;
		else
			m_pHeightMap[x+z*(MAP_SIZE+1)] = (FLOAT)factor + HGT_NOWALK;
	}
	else
		m_pHeightMap[x+z*(MAP_SIZE+1)] = (FLOAT)factor;
	SetDirtyPatch(x,z);

	if(x%PATCH_SIZE==0) {
		SetDirtyPatch(x-1,z);
		if(z%PATCH_SIZE==0) {
			SetDirtyPatch(x-1,z-1);
			SetDirtyPatch(x,z-1);
		}
		if(z%PATCH_SIZE==PATCH_SIZE-1) {
			SetDirtyPatch(x-1,z+1);
			SetDirtyPatch(x,z+1);
		}
	}
	else if(x%PATCH_SIZE==PATCH_SIZE-1) {
		SetDirtyPatch(x+1,z);
		if(z%PATCH_SIZE==0) {
			SetDirtyPatch(x+1,z-1);
			SetDirtyPatch(x,z-1);
		}
		if(z%PATCH_SIZE==PATCH_SIZE-1) {
			SetDirtyPatch(x+1,z+1);
			SetDirtyPatch(x,z+1);
		}
	}
	else {
		if(z%PATCH_SIZE==0) {
			SetDirtyPatch(x,z-1);
		}
		if(z%PATCH_SIZE==PATCH_SIZE-1) {
			SetDirtyPatch(x,z+1);
		}
	}
}

void CLandscape::SetHeight(POINT pos,FLOAT factor)
{
	SetHeight(pos.x,pos.y,factor);
}

#ifdef __BEAST
BOOL CLandscape::SaveLandscape( LPCTSTR lpszFileName, int xx, int yy )
{
	CFileIO file;
	if( file.Open( lpszFileName, "wb" ) == FALSE )
		return FALSE;
	
#ifndef __NewWater
	if (usingNewWater)
		file.PutDW(4); // new water
#endif
	else
		file.PutDW(LAND_VERSION);

	file.PutDW( xx );
	file.PutDW( yy );
	file.Write( m_pHeightMap, sizeof( FLOAT ), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );
	//file.Write( m_aWaterHeight, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE * sizeof( WATERHEIGHT) );
#ifndef __NewWater
		if (usingNewWater)
			file.Write(m_aWaterHeight, sizeof(WATERHEIGHT), MAP_SIZE * MAP_SIZE);
		else
#endif
	file.Write( m_aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
	file.Write( m_aLandAttr, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );

	OptimizeLayer();
	
	BYTE nLayer = m_aLayer.GetSize();
	file.Write( &nLayer, sizeof(BYTE) , 1 );

	CLandLayer* pLayer;
	for( int i = 0; i < nLayer; i++ ) 
	{
		pLayer = ((CLandLayer*)m_aLayer[ i ] );
		file.Write( &(pLayer->m_nTex), sizeof(WORD), 1 );
		file.Write( pLayer->m_aPatchEnable, sizeof( BOOL ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
		file.Write( pLayer->m_pTempLightMap, sizeof( DWORD ), MAP_SIZE * MAP_SIZE);
	}

	OptimizeObjArray();

	// Write Objects
	//suntodo:
	CObj** apObject = m_apObject[ OT_OBJ ];
	file.Write( &m_adwObjNum[ OT_OBJ ], sizeof( DWORD ), 1 );
	for( int i = 0; i < int( m_adwObjNum[ OT_OBJ ] ); i++ )
	{
		ASSERT( IsValidObj( apObject[ i ] ) );
		apObject[ i ]->m_vPos.x -= xx * LANDREALSCALE;
		apObject[ i ]->m_vPos.z -= yy * LANDREALSCALE;
		::WriteObj( &file, apObject[ i ] );
		apObject[ i ]->m_vPos.x += xx * LANDREALSCALE;
		apObject[ i ]->m_vPos.z += yy * LANDREALSCALE;

	}
	apObject = m_apObject[ OT_SFX ];
	file.Write( &m_adwObjNum[ OT_SFX ], sizeof( DWORD ), 1 );
	for( int i = 0; i < (int)( m_adwObjNum[ OT_SFX ] ); i++ )
	{
		ASSERT( IsValidObj( apObject[ i ] ) );
		apObject[ i ]->m_vPos.x -= xx * LANDREALSCALE;
		apObject[ i ]->m_vPos.z -= yy * LANDREALSCALE;
		::WriteObj( &file, apObject[ i ] );
		apObject[ i ]->m_vPos.x += xx * LANDREALSCALE;
		apObject[ i ]->m_vPos.z += yy * LANDREALSCALE;
	}
	return TRUE;
}
#endif
void CLandscape::SetUsedAllObjects()
{
	for( int i = 0; i < MAX_OBJARRAY; i++ )
	{
		//if( i != OT_ITEM )
		{
			CObj** apObjs = m_apObject[ i ];
			for( int j = 0; j < (int)( m_adwObjNum[ i ] ); j++ )
			{
				if( apObjs[ j ] )
					apObjs[ j ]->m_pModel->m_pModelElem->m_bUsed = TRUE;
			}
		}
	}
}
BOOL CLandscape::LoadLandscape( LPCTSTR lpszFileName, int xx, int yy )
{
#ifdef __BEAST
	CFileIO file;
#else
	CResFile file;
#endif
	if( file.Open( lpszFileName, "rb" ) == FALSE )
		return FALSE;
	ResetTerrain( INIT_HEIGHT );

#ifndef __NewWater
	static bool loadAndConvert = true;
#endif
	//BOOL m_aLandMark[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	//BYTE m_aWaterHeight[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	int xLand, yLand;
	file.Read( &m_dwVersion, sizeof( DWORD) );
	// 1 버젼일 때는 사이즈 읽어야함
	if( m_dwVersion >= 1 )
	{
		file.Read( &xLand, sizeof( xLand ) );
		file.Read( &yLand, sizeof( yLand ) );
	}
	if (m_dwVersion >= 4)
		usingNewWater = true;
		
	file.Read( m_pHeightMap, sizeof( FLOAT ), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );
#ifdef __IVERSION_NODARKON
	// xx가 11 이하면 완전히 갈 수 없도록 하자.
	if( xx <= 11 || ( xx == 12 && yy == 9 ) || ( xx == 12 && yy == 8 ) )
	{
		for( int i = 0; i < ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ); i++ )
		{
			if( m_pHeightMap[ i ] >= HGT_DIE )
				m_pHeightMap[ i ] -= HGT_DIE;
			else
			if( m_pHeightMap[ i ] >= HGT_NOMOVE )
				m_pHeightMap[ i ] -= HGT_NOMOVE;
			else
			if( m_pHeightMap[ i ] >= HGT_NOFLY )
				m_pHeightMap[ i ] -= HGT_NOFLY;
			else
			if( m_pHeightMap[ i ] >= HGT_NOWALK )
				m_pHeightMap[ i ] -= HGT_NOWALK;
			m_pHeightMap[ i ] += HGT_NOMOVE;
		}
	}
	if( xx == 12 && yy == 7 ) // 대각선 
	{
		int x2max = 0;
		for( int y2  = 0; y2 < ( MAP_SIZE + 1 ); y2++ )
		{
			for( int x2  = 0; x2 < x2max; x2++ )
			{
				int i = y2 * ( MAP_SIZE + 1 ) + x2;
				if( m_pHeightMap[ i ] >= HGT_DIE )
					m_pHeightMap[ i ] -= HGT_DIE;
				else
				if( m_pHeightMap[ i ] >= HGT_NOMOVE )
					m_pHeightMap[ i ] -= HGT_NOMOVE;
				else
				if( m_pHeightMap[ i ] >= HGT_NOFLY )
					m_pHeightMap[ i ] -= HGT_NOFLY;
				else
				if( m_pHeightMap[ i ] >= HGT_NOWALK )
					m_pHeightMap[ i ] -= HGT_NOWALK;
				m_pHeightMap[ i ] += HGT_NOMOVE;
			}
			x2max++;
			if( x2max >= ( MAP_SIZE + 1 ) ) 
				x2max = ( MAP_SIZE + 1 );
		}
	}
#endif

#ifndef __NewWater
	if (usingNewWater || loadAndConvert)
	{
		if (loadAndConvert && !usingNewWater)
		{
			WATERHEIGHT waterTemp[NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE];
			file.Read(waterTemp, sizeof(WATERHEIGHT), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE);
			for (int x = 0; x < MAP_SIZE; ++x)
			{
													//	MAP_SIZE	NUM_PATCHES_PER_SIDE	PATCH_SIZE
				for (int y = 0; y < MAP_SIZE; ++y) //	  128			   16					8
				{		
					m_aWaterHeight[x + y * MAP_SIZE] = waterTemp[static_cast<int>(x / PATCH_SIZE) + static_cast<int>(y / PATCH_SIZE) * NUM_PATCHES_PER_SIDE];
				}
			}
			usingNewWater = true;
		}
		else
			file.Read(m_aWaterHeight, sizeof(WATERHEIGHT), MAP_SIZE * MAP_SIZE);
	}
	else
#endif
		file.Read( m_aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
	// 2 버젼일 때는 지역맵 읽어야함
	if( m_dwVersion >= 2 )
	{
		file.Read( m_aLandAttr, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
		if( m_dwVersion == 2 )
			ZeroMemory( m_aLandAttr, sizeof( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ) );
	}
	else
	{
		ZeroMemory( m_aLandAttr, sizeof( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ) );
	}
	BYTE nLayer;
	file.Read( &nLayer, sizeof(BYTE) , 1 );
	CLandLayer* pLayer;
	WORD nTex;

	for(int j = 0; j < nLayer; j++ )  
	{
		file.Read( &( nTex ), sizeof(WORD), 1 );

		//gmpbigsun( 20100910 ) : 버젼4일때는 중복되는 텍스 레이어를 허용한다.
#ifdef __BS_OVERLAP_LAYER
		if( m_dwVersion >= 4 )
			pLayer = NewLayer( nTex, TRUE );
		else 
#endif //__BS_OVERLAP_LAYER
		pLayer = NewLayer( nTex );

		file.Read( pLayer->m_aPatchEnable, sizeof( BOOL ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );

		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect( 0, &rectLock, 0, 0 );
#ifdef __BEAST
#	ifdef __16BITLIGHT
		DWORD dwLightMap[ MAP_SIZE*MAP_SIZE ];
		file.Read( dwLightMap, sizeof( DWORD ), MAP_SIZE * MAP_SIZE );
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
		{
			pLayer->m_pTempLightMap[ i ] = 
			D3DCOLOR_ARGB16
			( 
				( dwLightMap[ i ] & 0xff000000 ) >> ( 24 ), 
				( dwLightMap[ i ] & 0x00ff0000 ) >> ( 16 ),
				( dwLightMap[ i ] & 0x0000ff00 ) >> (  8 ),
				( dwLightMap[ i ] & 0x000000ff )
			);
		}
		//file.Read( pLayer->m_pTempLightMap, sizeof( WORD ), MAP_SIZE * MAP_SIZE );
		memcpy( ((WORD *)rectLock.pBits), pLayer->m_pTempLightMap, MAP_SIZE * MAP_SIZE * sizeof( WORD ) );
#else
		file.Read( pLayer->m_pTempLightMap, sizeof( DWORD ), MAP_SIZE * MAP_SIZE );
		memcpy( ((DWORD *)rectLock.pBits), pLayer->m_pTempLightMap, MAP_SIZE * MAP_SIZE * sizeof( DWORD ) );
#endif
#else
#	ifdef __16BITLIGHT
		DWORD dwLightMap[ MAP_SIZE*MAP_SIZE ];
		file.Read( dwLightMap, sizeof( DWORD ), MAP_SIZE * MAP_SIZE );
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
		{
			((WORD *)rectLock.pBits)[i] = 
			D3DCOLOR_ARGB16
			( 
				( dwLightMap[ i ] & 0xff000000 ) >> ( 24 ), 
				( dwLightMap[ i ] & 0x00ff0000 ) >> ( 16 ),
				( dwLightMap[ i ] & 0x0000ff00 ) >> (  8 ),
				( dwLightMap[ i ] & 0x000000ff )
			);
		}
#	else
		file.Read(((DWORD *)rectLock.pBits),sizeof(DWORD),MAP_SIZE*MAP_SIZE);
#endif
#endif
		pLayer->m_pLightMap->UnlockRect(0);
	}
	// 해당 레이어에 존재하는 텍스춰가 없을 시에는 삭제 
	int nMaxLayer = m_aLayer.GetSize();
	for( int j = 0; j < nMaxLayer; j++ )  
	{
		CLandLayer* pLayer = (CLandLayer*)m_aLayer.GetAt( j );
		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
			prj.m_terrainMng.LoadTexture( pLayer->m_nTex );
		
		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
		{
			delete pLayer;
			m_aLayer.RemoveAt( j );
			nMaxLayer--;
			j--;
		}
	}
	MakeHgtAttrVertexBuffer();
	MakeWaterVertexBuffer();
	MakeLandAttrVertexBuffer();

//	CalculateBound();

	// Read Objects
	CObj** apObject = m_apObject[ OT_OBJ ];
	DWORD dwObjNum;
	file.Read( &dwObjNum, sizeof( DWORD ), 1 );
	TRACE( "Load Object Num = %d\n", dwObjNum );
	for( int i = 0; i < (int)( dwObjNum ); i++ )
	{
		CObj* pObj = ::ReadObj( &file );
		if( m_dwVersion >= 1 )	
		{
			pObj->m_vPos.x += xx * LANDREALSCALE;
			pObj->m_vPos.z += yy * LANDREALSCALE;
		}

		//pObj->m_vPos += D3DXVECTOR3( 1024, 0, 0 );
		if( pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex ) == TRUE)
		{
			{
				pObj->InitMotion( pObj->m_dwMotion );
				if( InsertObjLink( pObj ) )
				{
					AddObjArray( pObj );
					pObj->m_pWorld = m_pWorld;
					pObj->UpdateLocalMatrix();
				}
				else
				{
					delete pObj;
					dwObjNum--;
				}
			}
		}
		else
		{
			delete pObj;
			dwObjNum--;
		}
	}
	size_t size = file.Read( &dwObjNum, sizeof( DWORD ), 1 );
	if( size )
	{
		for( int i = 0; i < (int)( dwObjNum ); i++ )
		{
			CSfx* pObj = (CSfx*)::ReadObj( &file );
			if( m_dwVersion >= 1 )	
			{
				pObj->m_vPos.x += xx  * LANDREALSCALE;
				pObj->m_vPos.z += yy * LANDREALSCALE;
			}
			//pObj->m_vPos += D3DXVECTOR3( 1024, 0, 0 );
			if( pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex ) == TRUE )
			{
				InsertObjLink( pObj );
				AddObjArray( pObj );
				pObj->m_pWorld = m_pWorld;
				pObj->UpdateLocalMatrix();
			}
		}
	}
	SetModified( FALSE );

	//m_texMiniMap.LoadTexture( m_pd3dDevice, strDDSName, 0, FALSE );
	//MakeMiniMapTexture( j, i, &pLand->m_texMiniMap, NULL );
	
//	ASSERT( dwObjNum == m_adwObjNum[ OT_OBJ ] );
	return TRUE;
}
void CLandscape::OptimizeObjArray()
{
	for( int i = 0; i < MAX_OBJARRAY; i++ )
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->IsDelete() )
				RemoveObjArray( apObject[ j ] );

			if( apObject[ j ] == NULL )
			{
				m_adwObjNum[ i ]--;
				for( int k = j; k < int( m_adwObjNum[ i ] ); k++ )
				{
					apObject[ k ] = apObject[ k + 1 ];
					if( apObject[ k ] ) 
						apObject[ k ]->m_dwObjAryIdx = k;
				}
				j--;
			}
		}
		m_aObjStack[ i ].Clear();
	}
}
//
//
//
/*
	1 = 0  1 ~ 1.9
	2 = 1  2 ~ 3 
	4 = 2  4 ~ 7
	8  = 3  8 ~ 15  
	16 = 4
	32 = 5
	64  = 6
	128 = 7 128 ~ 255
*/
BOOL CLandscape::InsertObjLink( CObj* pObj )
{
	if( pObj->m_pModel == NULL )
		return 1;
	D3DXVECTOR3 vPos = pObj->GetPos();
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	/*
	if(  pObj->m_dwType== 4 && pObj->m_dwIndex == 7 )
	{
		int a;
		a++;
	}
	*/
	CObj** aObjLinkMap	= GetObjLink( pObj->GetLinkType(), pObj->GetLinkLevel() );
	int nMaxWidth = m_nWidthLinkMap[ pObj->GetLinkLevel() ];
	int nUnit = MAP_SIZE / nMaxWidth;
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		CObj* pBegObj 
			= aObjLinkMap[ nPos ];
		if( pBegObj )
		{

			if( pObj->m_pNext || pObj->m_pPrev )
			{
				TRACE("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
			pBegObj->InsNextNode(pObj);
		}
		else 
		{
			aObjLinkMap[nPos] = pObj;
			if( pObj->m_pNext || pObj->m_pPrev )
			{
				TRACE("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
		}
	}
	else
	{
		TRACE("헉 InsObjInFld에서 범위를 벗어난게 추가되려고 하네?\n");
		return FALSE;
	}
	return TRUE;
}
BOOL CLandscape::RemoveObjLink( CObj* pObj )
{
	D3DXVECTOR3 vPos = pObj->GetPos();
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( pObj->GetLinkType(), pObj->GetLinkLevel() );
	int nMaxWidth = m_nWidthLinkMap[ pObj->GetLinkLevel() ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );

	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		if( aObjLinkMap[ nPos ] == pObj )
			aObjLinkMap[ nPos ] = pObj->GetNextNode();
		pObj->DelNode();
		return TRUE;
	}
	return FALSE;
}
/*
	1 = 0  1 ~ 1.9
	2 = 1  2 ~ 3 
	4 = 2  4 ~ 7
	8  = 3  8 ~ 15  
	16 = 4
	32 = 5
	64  = 6
	128 = 7
*/
int CLandscape::m_nWidthLinkMap[ MAX_LINKLEVEL ];

CObj* CLandscape::GetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel )
{
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( dwLinkType, nLinkLevel );
	int nMaxWidth = m_nWidthLinkMap[ nLinkLevel ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
		return aObjLinkMap[ nPos ];
	return NULL;
}

BOOL CLandscape::SetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
{
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( dwLinkType, nLinkLevel );
	int nMaxWidth = m_nWidthLinkMap[ nLinkLevel ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		aObjLinkMap[ nPos ] = pObj;
		return TRUE;
	}
	return FALSE;
}
void CLandscape::AddObjArray( CObj* pObj )
{
	int nType = pObj->GetType();
	CObj** apObject = m_apObject[ nType ];
	CObj* pTemp = NULL;
	int nIndex = -1;
	if( m_aObjStack[ nType ].GetCount() )
	{
		nIndex = m_aObjStack[ nType ].Pop();
		if( nIndex == -1 )
		{
			int a	= 0;
		}
		ASSERT( !apObject[ nIndex ] );
		apObject[ nIndex ] = pObj;
	}
	else
	{
		nIndex = m_adwObjNum[ nType ]++;
		if( nIndex == -1 )
		{
			int a	= 0;
		}
		apObject[ nIndex ] = pObj;
	}
	SetModified( TRUE );
	pObj->m_pWorld = m_pWorld;
	pObj->m_dwObjAryIdx = nIndex;
	//InsertObjLink( pObj );
}

void CLandscape::RemoveObjArray( CObj* pObj )
{
	int nType = pObj->GetType();
	CObj** apObject = m_apObject[ nType ];
	if( apObject[ pObj->m_dwObjAryIdx ] == pObj )
	{
		m_aObjStack[ nType ].Push( pObj->m_dwObjAryIdx );
		apObject[ pObj->m_dwObjAryIdx ] = NULL;
		//RemoveObjLink( pObj );
		SetModified( TRUE );
	}
	else
	{
		ASSERT( 0 );
	}
}
/*
void CLandscape::Process()
{
	m_fWaterFrame += 0.05;
	if( m_fWaterFrame >= 16.0f )
		m_fWaterFrame = 0.0f;

	// 처리 프로세스
	for( int i = 0; i < MAX_OBJARRAY; i++ )
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] )
			{
				if( apObject[ j ]->GetWorld() != NULL )
					apObject[ j ]->Process( 0 );
			}
		}
	}
}
*/
int CLandscape::GetObjArraySize()
{
	int nSum = 0;
	for( int i = 0; i < MAX_OBJARRAY; i++ )
		nSum += int( m_adwObjNum[ i ] );
	return nSum;
}
void CLandscape::RemoveLayer_Layer( int nLayer )
{
	SetModified( TRUE );
	CLandLayer* pLayer = (CLandLayer*)m_aLayer.GetAt( nLayer );
	delete pLayer;
	m_aLayer.RemoveAt( nLayer );
}
void CLandscape::DeleteLayer( WORD nTex )
{
	SetModified( TRUE );
	CLandLayer* pLayer;
	for(int i = 0; i < m_aLayer.GetSize(); i++) 
	{
		pLayer = (CLandLayer*)m_aLayer.GetAt( i );
		if( pLayer->m_nTex == nTex ) 
		{
			delete pLayer;
			m_aLayer.RemoveAt( i );
			return;
		}
	}
}

CLandLayer* CLandscape::NewLayer( WORD nTex, BOOL bOverlap )
{
	SetModified( TRUE );
	CLandLayer* pLayer = NULL;

	if( FALSE == bOverlap )	//중복허용이 아닐때만 기존 레이어에 있는지 찾는다
	{
		int nSize = m_aLayer.GetSize();
		for(int i = nSize - 1; i >= 0; --i ) 
		{
			pLayer = (CLandLayer*)(m_aLayer.GetAt(i));
			if( pLayer->m_nTex == nTex ) 
				return pLayer;
		}
	}

	//MAX_LAYER error 체크
	//int nSize = m_aLayer.GetSize();
	//if( nSize > MAX_LAYER )
	//{
	//	CString strTemp;
	//	strTemp.Format( "레이어 갯수가 최대갯수 %d개를 넘었습니다! 레이어를 줄여주세요", MAX_LAYER );
	//	AfxMessageBox( strTemp );

	//	return NULL;\
	//}

	pLayer = new CLandLayer(m_pd3dDevice,nTex);

	if( m_aLayer.GetSize() == 0 ) 
	{
#ifdef __BEAST
#	ifdef __16BITLGHT
		for(int i=0;i<MAP_SIZE*MAP_SIZE;i++)
			pLayer->m_pTempLightMap[i] = D3DCOLOR_ARGB16( 255, 127, 127, 127 );
#	else
		for(int i=0;i<MAP_SIZE*MAP_SIZE;i++)
			pLayer->m_pTempLightMap[i] = D3DCOLOR_ARGB( 255, 127, 127, 127 );
#	endif
#endif
		for(int i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=TRUE;
	}
	else 
	{
		DWORD color = 0;
		CLandLayer* pTempLayer = (CLandLayer*)m_aLayer[0];

		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect( 0 , &rectLock, 0, 0);
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ ) 
		{
#ifdef __BEAST
#	ifdef __16BITLIGHT
			color = pTempLayer->m_pTempLightMap[ i ] & 0x0fff;// 0xffffff;
			pLayer->m_pTempLightMap[i] = color;
#	else
			color = pTempLayer->m_pTempLightMap[i] & 0xffffff;
			pLayer->m_pTempLightMap[i] = color;
#endif
#endif
			((WORD *)rectLock.pBits)[i]=0x0;//color;
		}
		pLayer->m_pLightMap->UnlockRect(0);
		for(int i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=FALSE;
	}
	m_aLayer.Add(pLayer);

	return pLayer;
}

#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
CLandLayer* CLandscape::AddLayerBeast( WORD nTex )
{
	SetModified( TRUE );
	CLandLayer* pLayer = NULL;
	for(int i = 0; i < m_aLayer.GetSize(); i++) 
	{
		pLayer = (CLandLayer*)(m_aLayer.GetAt(i));
		if( pLayer->m_nTex == nTex ) 
			return pLayer;
	}
	pLayer = new CLandLayer(m_pd3dDevice,nTex);

	if( m_aLayer.GetSize() == 0 ) 
	{
#ifdef __BEAST
#	ifdef __16BITLGHT
		for(int i=0;i<MAP_SIZE*MAP_SIZE;i++)
			pLayer->m_pTempLightMap[i] = D3DCOLOR_ARGB16( 255, 127, 127, 127 );
#	else
		for(int i=0;i<MAP_SIZE*MAP_SIZE;i++)
			pLayer->m_pTempLightMap[i] = D3DCOLOR_ARGB( 255, 127, 127, 127 );
#	endif
#endif
		for(int i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=TRUE;
	}
	else 
	{
		DWORD color = 0;
		CLandLayer* pTempLayer = (CLandLayer*)m_aLayer[0];

		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect( 0 , &rectLock, 0, 0);
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ ) 
		{
#ifdef __BEAST
#	ifdef __16BITLIGHT
			color = pTempLayer->m_pTempLightMap[ i ] & 0x0fff;// 0xffffff;
			pLayer->m_pTempLightMap[i] = color;
#	else
			color = pTempLayer->m_pTempLightMap[i] & 0xffffff;
			pLayer->m_pTempLightMap[i] = color;
#endif
#endif
			((WORD *)rectLock.pBits)[i]=0;//color;
		}
		pLayer->m_pLightMap->UnlockRect(0);
		for(int i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=FALSE;
	}
	m_aLayer.Add(pLayer);

	return pLayer;
}

void CLandscape::SetWaterHeightBeast(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture)
{
	SetModified(TRUE);

#ifndef __NewWater
	if (usingNewWater)
	{
		m_aWaterHeight[x + z * MAP_SIZE].byWaterHeight = (BYTE)(dwWaterHeight);
		m_aWaterHeight[x + z * MAP_SIZE].byWaterTexture = (BYTE)(dwWaterTexture);
	}
	else
#endif
	{
		m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterHeight = (BYTE)(dwWaterHeight);
		m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterTexture = (BYTE)(dwWaterTexture);
	}
#ifndef __N_WATERFRAME_050202
	SAFE_RELEASE( m_pWaterVertexBuffer );
#else
	for ( int i = 0 ; i < prj.m_terrainMng.m_nWaterFrame ; i++)
		SAFE_RELEASE( m_pWaterVB[ i ].pVB );
#endif//__N_WATERFRAME_050202
	MakeWaterVertexBuffer();
}
#endif //__CSC_UNDO_REDO
// 해당 레이어에 데이타가 있나 없나 조사하기 
BOOL CLandscape::CheckLayer( CLandLayer* pLayer, int x, int z )
{
	x *= ( PATCH_SIZE - 1 );
	z *= ( PATCH_SIZE - 1 );
	for( int j = 0; j < PATCH_SIZE; j++ ) 
	{
		for( int k = 0; k <= PATCH_SIZE; k++ ) 
		{
#	ifdef __16BITLIGHT
			if( pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] >> 12 > 0 ) 
				return TRUE;
#	else
			DWORD dwAlpha = pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] >> 24;
			//if( pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] >> 24 > 0 ) 

			if( dwAlpha > 0 )
				return TRUE;
#endif
		}
	}
	return FALSE;
}
void CLandscape::CheckAllLayer(int x,int z)
{
	CLandLayer* pLayer;
	for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*)m_aLayer[ i ];
		pLayer->SetPatchEnable( x , z , CheckLayer( pLayer, x, z ) );
	}
}
void CLandscape::UpdateAllLayer(void)
{
	CLandLayer* pLayer = NULL;
	for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*)m_aLayer[i];

		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect(0,&rectLock,0,0);
#	ifdef __16BITLIGHT
		memcpy( ( ( WORD*) rectLock.pBits ), pLayer->m_pTempLightMap, MAP_SIZE * MAP_SIZE * sizeof( WORD ) );
#	else
		memcpy( ( ( DWORD*) rectLock.pBits ), pLayer->m_pTempLightMap, MAP_SIZE * MAP_SIZE * sizeof( DWORD ) );
#	endif
		pLayer->m_pLightMap->UnlockRect(0);
	}
}

void CLandscape::SetTex(int x, int z, WORD nTex, BYTE nOpacity,BOOL bLandMark)
{
	//sun!!
	if( x < 0 || x > LIGHTMAP_SIZE || z < 0 || z > LIGHTMAP_SIZE) 
		return;

	SetModified( TRUE );

	//현재 nTex의 레이어가 존재하고
	//해당 Patch에 이 레이어의 LightMap의 알파가 0보다 크면
	//최근작업한 레이어가 nTex가 아니면 똑같은 nTex를 가진 레이어를 생성한다.
    
	BOOL bOverlap = FALSE;
	CLandLayer* pCheckLayer = NULL;
	int nMaxSize = m_aLayer.GetSize();

#ifdef __BS_OVERLAP_LAYER
	//가장 마지막의 레이어, 최근 작업중인 레이어로 판단한다.
	CLandLayer* pLastLayer = (CLandLayer*)m_aLayer[ nMaxSize - 1 ];
	if( pLastLayer )
	{	
		for( int i = 0; i < nMaxSize; ++i )
		{
			pCheckLayer = ( CLandLayer* )m_aLayer[ i ];
			if( pCheckLayer->m_nTex == nTex && pLastLayer->m_nTex != nTex )		//현재 레이어중에 같은 레이어가 있으나 최근작업한 레이어가 아니면
			{
				bOverlap = TRUE;
				break;
			}
		}
	}
#endif 

	CLandLayer* pThisLayer = NewLayer( nTex, bOverlap );
	if( !pThisLayer )
		return;

	int px = x / ( PATCH_SIZE - 1 );
	int pz = z / ( PATCH_SIZE - 1 );

	BOOL bDirt = FALSE;

#ifdef __16BITLIGHT
	nOpacity >>= 4;
	DWORD color = ( pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] & 0x0fff );
	if( pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 12 < nOpacity ) 
	{
		pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color | ( nOpacity << 12 );

		int tempAlphaSum = 0;
		for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
		{
			pLayer = ( CLandLayer* )m_aLayer[ i ];
			if( pLayer->m_nTex != nTex )
				tempAlphaSum += pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 12;
		}
		for( i= 0; i < m_aLayer.GetSize(); i++) 
		{
			pLayer = ( CLandLayer* )m_aLayer[ i ];
			if( pLayer->m_nTex != nTex )

				if( tempAlphaSum == 0 ) 
					pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color;
				else 
					pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color | 
					( ( 15 - nOpacity ) * ( pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 12 ) / tempAlphaSum ) << 12;
		}
#else
	DWORD color = ( pThisLayer->m_pTempLightMap[ x + z * MAP_SIZE ] & 0xffffff );		//RGB
	BYTE dnVal = (BYTE)( pThisLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 24 );		//Alpha
	int dnTex = pThisLayer->m_nTex;

	if( dnVal < nOpacity )		//같은 텍스쳐(레이어)일경우 불투명도가 더 높을경우만 처리 
	{
#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
		TEXLANDLAYER texlandlayer;
		texlandlayer.m_nLandScape = CCommandMgrBeast::GetInstance()->m_nLandScape;
		texlandlayer.m_nTex = pThisLayer->m_nTex;
		texlandlayer.m_nLayerTemp = x + z * MAP_SIZE;
		texlandlayer.m_nX = x;
		texlandlayer.m_nZ = z;
		map<TEXLANDLAYER, int>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldTexLandLayer.find(texlandlayer);
		
		if(iter == CCommandMgrBeast::GetInstance()->m_mapOldTexLandLayer.end())
		{
			CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_TEX_CHG);
			int nOldAlpha = pThisLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 24;
			CCommandMgrBeast::GetInstance()->m_mapOldTexLandLayer.insert(map<TEXLANDLAYER, int>::value_type(texlandlayer, nOldAlpha));
		}
		else
		{
			int nAlpha = iter->second;
			if(nAlpha >> 24 < nOpacity)
			{
				CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_TEX_CHG);
				CCommandMgrBeast::GetInstance()->m_mapOldTexLandLayer.insert(map<TEXLANDLAYER, int>::value_type(texlandlayer, nOpacity));
			}
		}
#endif //__CSC_UNDO_REDO
#endif //__BEAST

		//gmpbigsun: A - B - A 인경우 A가 중복이지만 레이어를 새로 생성해서 자연스럽게 혼합되도록 수정함
		//기존 알고리즘은 알파조정에 있어 가장 하단부 에 있는 텍스쳐는 무조건 불투명이라는 조건때문에 그 위에 올라오는 레이어의 알파부분에
		//하단부 색이 보이는 현상이 버그로 지적되었다( 사실 버그라고 하긴 뭐하고, 원래 이방식이 이렇다. 근데 작업자들은 버그라고 인식한다. )

		DWORD dwOpa = nOpacity << 24;
		pThisLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color | dwOpa ;		//같은레이어에 대해 컬러 알파값 업데이트하고...

#if 0
		CLandLayer* pLayer = NULL;
		DWORD dwColor = 0;
		DWORD dwAlpha = 0;
		for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
		{
			//pass the base texture
			if( 0 == i )
				continue; 

			pLayer = (CLandLayer*)m_aLayer[ i ];
			dwColor = pLayer->m_pTempLightMap[ x + z * MAP_SIZE ];
			if( ( dwColor >> 24 ) == 0 )
				continue;
			
			if( pLayer->m_nTex != nTex )
			{
				dwAlpha = 255 - nOpacity;
				pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color | (dwAlpha << 24);
			}
		}
#else

#ifndef __BS_OVERLAP_LAYER 
//제 0번 Layer영역은 절대로 Blink처리를 하면 안된다 
// Blink처리가 될경우 색이 올라오는등의 문제가 발생한다. 작업자는 이점을 꼭 숙지하여야 한다. -_-이것땜시 겁나 고생함.
// 만일 중복레이어를 ( 쓸일 없겠지만 , 너무 비효율적임 )써야 하는경우라면 아래코드는 필요없다.

//gmpbigsun( 20100831 ) : BEGIN old codes==================================================================================================
		int tempAlphaSum = 0;
		CLandLayer* pLayer = NULL;
		for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
		{
			pLayer = (CLandLayer*)m_aLayer[ i ];
		
			// 현재 칠하려는 텍스쳐와 다른 레이어들의 해당자리의 알파값을 전부 더한다.
			if( pLayer->m_nTex != nTex )
			{
				DWORD dwAlpha = (pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 24);
				tempAlphaSum += dwAlpha;
			}
			
			//tempAlphaSum += (pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] >> 24);
			//tempAlphaSum = 255;
		}

		int nMaxSize = m_aLayer.GetSize();

		for( int i = 0; i < nMaxSize; i++) 
		{
			pLayer = (CLandLayer*)m_aLayer[ i ];
		
			DWORD dwOrgColor = pLayer->m_pTempLightMap[ x+z*MAP_SIZE ];
			DWORD dwOrgAlpha = dwOrgColor >> 24;

 			if( pLayer->m_nTex != nTex )
			{
				if( tempAlphaSum == 0 )
					pLayer->m_pTempLightMap[x+z*MAP_SIZE] = color;
				else 
				{
					DWORD dwAlpha = ( (255 - nOpacity) * dwOrgAlpha/tempAlphaSum )<<24;
					DWORD dwRst = color | dwAlpha;
				//	pLayer->m_pTempLightMap[x+z*MAP_SIZE] = color | ( (255 - nOpacity)*(pLayer->m_pTempLightMap[x+z*MAP_SIZE]>>24)/tempAlphaSum )<<24;
					pLayer->m_pTempLightMap[x+z*MAP_SIZE] = dwRst;
				}
			}

		}
		//END old codes====================================================================================================================
#endif //__BS_OVERLAP_LAYER
#endif
		
#endif //__16BITLIGHT

		SetLandMark( px, pz, nTex, FALSE );				//sun!!
		SetDirtyPatch( px * PATCH_SIZE, pz * PATCH_SIZE );
		if( px * ( PATCH_SIZE - 1 ) == x ) 
		{
			SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, pz * PATCH_SIZE );
			if( pz * ( PATCH_SIZE - 1 ) == z )
				SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
		}
		if( pz * ( PATCH_SIZE - 1 ) == z ) 
		{
			SetDirtyPatch( px * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
		}
	}
	else
	{
		int a = 0;
	}
	OptimizeLayer();
}
int CLandscape::GetTex(int x, int z)
{
	if(x<0 || x>=MAP_SIZE || z<0 || z>=MAP_SIZE) 
		return -1;

	int px = x / PATCH_SIZE;
	int pz = z / PATCH_SIZE;

	CLandLayer* pLayer;
	for( int i = m_aLayer.GetSize() - 1; i >= 0; i-- )
	{
		pLayer = (CLandLayer*)m_aLayer[ i ];
		if( pLayer->m_aPatchEnable[ px + pz * NUM_PATCHES_PER_SIDE ] )
			return pLayer->m_nTex;
	}
	return -1;
}
	/*
void CLandscape::OptimizeLayer( int x, int z )
{
return;// / ( PATCH_SIZE - 1 );
	z = z / ( PATCH_SIZE - 1 );
	// 지워져서 더이상 유용하지 않은 레이어는 삭제한다.
	for( int i = 1; i < m_aLayer.GetSize(); i++ ) 
	{
		CLandLayer* pLayer = (CLandLayer*)m_aLayer[ i ];
		if( CheckLayer( pLayer, x, z ) == FALSE )
		{
			delete pLayer;
			m_aLayer.RemoveAt( i );
			i--;
		}
	}
}
*/

void CLandscape::OptimizeLayer()
{
	BOOL bLayerOk = FALSE;
	for( int i = 1; i < m_aLayer.GetSize(); i++ ) 
	{
		CLandLayer* pLayer = (CLandLayer*)m_aLayer[ i ];
		bLayerOk = FALSE;
		for( int j = 0; j < MAP_SIZE * MAP_SIZE; j++ ) 
		{
#	ifdef __16BITLIGHT
			if( pLayer->m_pTempLightMap[ j ] >> 12 > 0 ) 
			{	
				bLayerOk = TRUE; 
				break;
			}
#	else
			if( pLayer->m_pTempLightMap[ j ] >> 24 > 0 ) 
			{	
				bLayerOk = TRUE; 
				break;
			}
#	endif
		}
		if( bLayerOk == FALSE )
		{
			SetModified( TRUE );
			delete pLayer;
			m_aLayer.RemoveAt( i );
			i--;
		}
	}
}

DWORD CLandscape::GetCol(int x, int z)
{
	if(x<0 || x>LIGHTMAP_SIZE || z<0 || z>LIGHTMAP_SIZE) return 0;

	int px = x / ( PATCH_SIZE - 1 );
	int pz = z / ( PATCH_SIZE - 1 );

	CLandLayer* pLayer;
	for( int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*) m_aLayer[ i ];
#ifdef __16BITLIGHT
		WORD wColor = pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] & 0x0fff;
		return D3DCOLOR_ARGB( 0, ( wColor & 0x0f00 ) >> 4, ( wColor & 0x00f0 ), ( wColor & 0x000f ) << 4 );
		//return (DWORD)( pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] & 0x0fff );
#else
		return (DWORD)( pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] & 0x00ffffff );
#endif
	}
	return 0;
}

void CLandscape::SetCol(int x, int z, DWORD color)
{
	if(x<0 || x>LIGHTMAP_SIZE || z<0 || z>LIGHTMAP_SIZE) return;

	SetModified( TRUE );
	int px=x/(PATCH_SIZE-1);
	int pz=z/(PATCH_SIZE-1);
#ifdef __16BITLIGHT
	color =	D3DCOLOR_ARGB16
			( 
				( color & 0xff000000 ) >> ( 24 ), 
				( color & 0x00ff0000 ) >> ( 16 ),
				( color & 0x0000ff00 ) >> (  8 ),
				( color & 0x000000ff )
			);
#endif
	DWORD tempAlpha;
	CLandLayer* pLayer;
#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
	BOOL bUsed = FALSE;
	for(map<TEXLANDLAYER, DWORD>::iterator iter=CCommandMgrBeast::GetInstance()->m_mapOldTexColorLayer.begin(); iter!=CCommandMgrBeast::GetInstance()->m_mapOldTexColorLayer.end(); ++iter)
	{
		TEXLANDLAYER oldtexcolorlayer = iter->first;
		if(oldtexcolorlayer.m_nX == x && oldtexcolorlayer.m_nZ == z)
		{
			bUsed = TRUE;
			break;
		}
	}
#endif //__CSC_UNDO_REDO
#endif //__BEAST
	for(int i=0;i<m_aLayer.GetSize();i++) 
	{
		pLayer = (CLandLayer*)m_aLayer[i];
#ifdef __16BITLIGHT
		tempAlpha = pLayer->m_pTempLightMap[ x + z * MAP_SIZE ];// >> 24;
		pLayer->m_pTempLightMap[ x + z * MAP_SIZE ] = color | ( tempAlpha & 0xf000 );//<< 24 );
#else
#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
		DWORD dwOldColor = pLayer->m_pTempLightMap[x+z*MAP_SIZE];
		DWORD dwChgColor = color | (dwOldColor & 0xff000000);

		if(!bUsed && dwOldColor != dwChgColor)
		{
			TEXLANDLAYER texcolorlayer;
			texcolorlayer.m_nLandScape = CCommandMgrBeast::GetInstance()->m_nLandScape;
			texcolorlayer.m_nTex = i;
			texcolorlayer.m_nLayerTemp = x + z * MAP_SIZE;
			texcolorlayer.m_nX = x;
			texcolorlayer.m_nZ = z;

			CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_COLOR_CHG);
			CCommandMgrBeast::GetInstance()->m_mapOldTexColorLayer.insert(map<TEXLANDLAYER, DWORD>::value_type(texcolorlayer, dwOldColor));
		}
#endif //__CSC_UNDO_REDO
#endif //__BEAST
		tempAlpha = pLayer->m_pTempLightMap[x+z*MAP_SIZE];// >> 24;
		pLayer->m_pTempLightMap[x+z*MAP_SIZE] = color | (tempAlpha & 0xff000000);//<< 24 );
#endif
	}
	SetDirtyPatch(px*PATCH_SIZE,pz*PATCH_SIZE);
	if(px*(PATCH_SIZE-1)==x) 
	{
		SetDirtyPatch((px-1)*PATCH_SIZE,pz*PATCH_SIZE);
		if(pz*(PATCH_SIZE-1)==z)
			SetDirtyPatch((px-1)*PATCH_SIZE,(pz-1)*PATCH_SIZE);
	}
	if(pz*(PATCH_SIZE-1)==z) 
	{
		SetDirtyPatch(px*PATCH_SIZE,(pz-1)*PATCH_SIZE);
	}
}

void CLandscape::SetPatchAlphaLayer(CLandLayer* pLayer, int x, int z, BYTE nAlpha)
{
	SetModified( TRUE );
	x*=(PATCH_SIZE-1);
	z*=(PATCH_SIZE-1);
	DWORD tempColor;
#ifdef __16BITLIGHT
	nAlpha = ( nAlpha >> ( 24 + 4 ) ) << 12;
#endif
	for(int j=0;j<PATCH_SIZE;j++) 
	{
		for(int k=0;k<=PATCH_SIZE;k++) 
		{
#ifdef __16BITLIGHT
			tempColor = pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] & 0x0fff;
			pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] = tempColor | nAlpha;
#else
			tempColor = pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] & 0xffffff;
			pLayer->m_pTempLightMap[ ( x + k ) + ( z + j ) * MAP_SIZE ] = tempColor | (nAlpha << 24);
#endif
		}
	}
}

void CLandscape::SetBlankPatch(int x, int z)
{
	SetModified( TRUE );
	CLandLayer* pLayer;
	for(int i=0;i<m_aLayer.GetSize();i++) 
	{
		pLayer=(CLandLayer*)m_aLayer[i];
		SetPatchAlphaLayer(pLayer,x,z,0);
	}
	SetDirtyPatch(x*PATCH_SIZE,z*PATCH_SIZE);
}

void CLandscape::SetLandMark(int x, int z,WORD nTex,BOOL bEnable)
{
	SetModified( TRUE );
	return;
	if(bEnable) 
	{
		SetBlankPatch(x,z);
		CLandLayer* pLayer=NewLayer(nTex);
		if( !pLayer )
			return;

		int tx=x*(PATCH_SIZE-1);
		int tz=z*(PATCH_SIZE-1);
#ifdef __16BITLIGHT
		DWORD tempColor = pLayer->m_pTempLightMap[ ( tx + PATCH_SIZE / 2 ) + ( tz + PATCH_SIZE / 2 ) * MAP_SIZE ] & 0x0fff;
		pLayer->m_pTempLightMap[ ( tx + PATCH_SIZE / 2) + ( tz + PATCH_SIZE / 2 ) * MAP_SIZE ] = tempColor | 0xf000;
#else
		DWORD tempColor = pLayer->m_pTempLightMap[ ( tx + PATCH_SIZE / 2 ) + ( tz + PATCH_SIZE / 2 ) * MAP_SIZE ] & 0xffffff;
		pLayer->m_pTempLightMap[ ( tx + PATCH_SIZE / 2) + ( tz + PATCH_SIZE / 2 ) * MAP_SIZE ] = tempColor | 0xff << 24;
#endif
		SetDirtyPatch(x*PATCH_SIZE,z*PATCH_SIZE);
	}
	else 
	{
		//if(m_aLandMark[x+z*NUM_PATCHES_PER_SIDE]) {
		//	SetDirtyPatch(x*PATCH_SIZE,z*PATCH_SIZE);
		//}
	}
	//m_aLandMark[x+z*NUM_PATCHES_PER_SIDE]=bEnable;
}
//
// dwWaterHeight = 0 - none
// dwWaterHeight = 1 - CLOUD
// dwWaterHeight >=2 Water and WaterHeight // meter unit
//
void CLandscape::SetWaterHeight(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture)
{
#ifdef __BEAST
#ifdef __CSC_UNDO_REDO
	BOOL bUsed = FALSE;
	for (map<TEXLANDLAYER, WATERLAYER>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldWaterTexLayer.begin(); iter != CCommandMgrBeast::GetInstance()->m_mapOldWaterTexLayer.end(); ++iter)
	{
		TEXLANDLAYER oldwatertexlayer = iter->first;
		if (oldwatertexlayer.m_nX == x && oldwatertexlayer.m_nZ == z)
		{
			bUsed = TRUE;
			break;
		}
	}

	if (!bUsed)
	{
		TEXLANDLAYER watertexlayer;
		watertexlayer.m_nLandScape = CCommandMgrBeast::GetInstance()->m_nLandScape;
		watertexlayer.m_nTex = 0;
		watertexlayer.m_nLayerTemp = 0;
		watertexlayer.m_nX = x;
		watertexlayer.m_nZ = z;
		WATERLAYER oldwaterlayer;

#ifndef __NewWater
		if (usingNewWater)
		{
			oldwaterlayer.m_dwHeight = m_aWaterHeight[x + z * MAP_SIZE].byWaterHeight;
			oldwaterlayer.m_nTex = m_aWaterHeight[x + z * MAP_SIZE].byWaterTexture;
		}
		else
#endif
		{
			oldwaterlayer.m_dwHeight = m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterHeight;
			oldwaterlayer.m_nTex = m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterTexture;
		}

		CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_WATER_CHG);
		CCommandMgrBeast::GetInstance()->m_mapOldWaterTexLayer.insert(map<TEXLANDLAYER, WATERLAYER>::value_type(watertexlayer, oldwaterlayer));
	}
#endif //__CSC_UNDO_REDO
#endif //__BEAST
	SetModified(TRUE);
#ifndef __NewWater
	if (usingNewWater)
	{

		m_aWaterHeight[x + z * MAP_SIZE].byWaterHeight = (BYTE)(dwWaterHeight);
		m_aWaterHeight[x + z * MAP_SIZE].byWaterTexture = (BYTE)(dwWaterTexture);
	}
	else
#endif
	{
		m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterHeight = (BYTE)(dwWaterHeight);
		m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterTexture = (BYTE)(dwWaterTexture);
	}
#ifndef __N_WATERFRAME_050202
	SAFE_RELEASE( m_pWaterVertexBuffer );
#else
	for ( int i = 0 ; i < prj.m_terrainMng.m_nWaterFrame ; i++)
		SAFE_RELEASE( m_pWaterVB[ i ].pVB );
#endif//__N_WATERFRAME_050202
	MakeWaterVertexBuffer();
	//SetModified( TRUE );
}
void CLandscape::SetHeightAttribute( int x, int z, DWORD dwAttr )
{
	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE ) 
		return;
	SetModified( TRUE );
	FLOAT fHeight = m_pHeightMap[x+z*(MAP_SIZE+1)];
	// recover height
	if( fHeight >= HGT_NOWALK )
	{
		if( fHeight >= HGT_DIE )
			fHeight = fHeight - HGT_DIE;
		else
		if( fHeight >= HGT_NOMOVE )
			fHeight = fHeight - HGT_NOMOVE;
		else
		if( fHeight >= HGT_NOFLY )
			fHeight = fHeight - HGT_NOFLY;
		else
			fHeight = fHeight - HGT_NOWALK;
	}
	if( dwAttr == HATTR_NOWALK )
		fHeight += HGT_NOWALK;
	if( dwAttr == HATTR_NOFLY )
		fHeight += HGT_NOFLY;
	if( dwAttr == HATTR_NOMOVE )
		fHeight += HGT_NOMOVE;
	if( dwAttr == HATTR_DIE )
		fHeight += HGT_DIE;
	m_pHeightMap[x+z*(MAP_SIZE+1)] = fHeight; 
	//MakeHgtAttrVertexBuffer();
	//SetModified( TRUE );
}
#endif
int CLandscape::GetHeightAttribute( int x, int z )
{
	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE ) 
		return -1;
	FLOAT fHeight = m_pHeightMap[ x + z * ( MAP_SIZE + 1 ) ];
	if( fHeight >= HGT_NOWALK )
	{
		if( fHeight >= HGT_DIE )
			return HATTR_DIE;
		if( fHeight >= HGT_NOMOVE )
			return HATTR_NOMOVE;
		if( fHeight >= HGT_NOFLY )
			return HATTR_NOFLY;
		if( fHeight >= HGT_NOWALK )
			return HATTR_NOWALK;
		return HATTR_NONE;
	}
	return HATTR_NONE;
}


LPWATERHEIGHT CLandscape::GetWaterHeight( int x, int z )
{
#ifndef __NewWater
	return usingNewWater ? &m_aWaterHeight[x + z * MAP_SIZE] : &m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE];

#else
	return &m_aWaterHeight[ x + z * NUM_PATCHES_PER_SIDE ];
#endif
}

#ifdef __BS_SUPPORT_LIGHTMAP
void CLandscape::FillLightColor( D3DXVECTOR3* pLightDir )
{
	const int TEST_SIZE = 1;

	assert( m_pLightMapLayer );

	LPDIRECT3DTEXTURE9 pLightMap = m_pLightMapLayer->m_pLightMap;
	DWORD* pTempLightMap = m_pLightMapLayer->m_pTempLightMap;

	//시작부터 끝까지 삼각형을 뽑아내서 빛과의 각도를 계산하고 음영의 강도를 설정한 값을
	//텍스쳐에 기록한다.
	
	DWORD color = 0;
	DWORD alpha = 0;
	DWORD alpha2 = 0;

	DWORD cellRow = 0;
	DWORD cellCol = 0;


	D3DLOCKED_RECT rectLock;
	pLightMap->LockRect( 0 , &rectLock, 0, 0);

#if 0 
	//step1 : 자체 언덕이 아닌 그림자가 드리워져야할곳 계산
	for( int i = 0; i < MAP_SIZE; ++i ) 
	{
		for( int j = 0; j < MAP_SIZE; ++j )
		{
			cellRow = j+(1+j/8);
			cellCol = i+(1+i/8);

			alpha2 = (DWORD)( 255.0f * ComputeShade( cellRow,cellCol, pLightDir ) );	//음 보정값은 패치 offset인데 왜이래야하는지는..정확히 못밝힘

			if( alpha2 > 255 )
				alpha2 = 255;

			if( 0 < alpha2 )
			{
				//만일 default height보다 큰 지점을 솓구친 지점이라고 보면
				//만일 default height보다 큰 지점이라면 광선을 default height의 평면으로 쏴서 충돌지점에 그림자를 그려준다.
				FillLightColor_shadow( i, j, pTempLightMap, (DWORD*)rectLock.pBits);
			}

		}	
	}
#endif 

	//step2: 자체 그림자 
	for( int i = 0; i < MAP_SIZE; ++i ) 
	{
		for( int j = 0; j < MAP_SIZE; ++j )
		{
			int index = i * MAP_SIZE + j;
			color = pTempLightMap[index] & 0xffffff;
			alpha = pTempLightMap[index] >> 24;

			cellRow = j+(1+j/8);
			cellCol = i+(1+i/8);
			alpha2 = (DWORD)( 255.0f * ComputeShade( cellRow,cellCol, pLightDir ) );	//음 보정값은 패치 offset인데 왜이래야하는지는..정확히 못밝힘

			if( alpha2 > 255 )
				alpha2 = 255;

			if( alpha < alpha2 )
			{
				alpha = alpha2;
				SetDirtyPatch( i, j );
			}

			alpha = alpha << 24;
			pTempLightMap[index] = (color|alpha);
			((DWORD *)rectLock.pBits)[index] = (color|alpha);
		}	
	}

	pLightMap->UnlockRect(0);
}

void CLandscape::FillLightColor_shadow( int x, int z, DWORD* pTempLightMap, DWORD* pBits )
{
	int xr = 1 + (x / 8 );
	int zr = 1 + (z / 8 );

	float fHeight = GetHeight( (DWORD)(z + zr), (DWORD)(x + xr ) );
	
	if( fHeight > ( m_dwHeightDefault ) )
	{
		D3DXVECTOR3 kDir( -10.0f, -6.0f, 10.0f );
		D3DXVec3Normalize( &kDir, &kDir );
		D3DXVECTOR3 kLight( (float)x , (float)fHeight, (float)z);

		float fDisHeight = (fHeight - m_dwHeightDefault);

		fDisHeight = (fDisHeight / 2.5f);

#if 1 
//VECTOR 버젼
		D3DXVECTOR3 kLight2;
		for( float f = 1.0f; f < (fDisHeight); f += 0.4f )
		{
			kLight2 = kLight + kDir * f;
		
			DWORD shadowX = (DWORD)(kLight2.x );
			DWORD shadowZ = (DWORD)(kLight2.z );
			DWORD dwColor = 0x77303030;

			int index = shadowX * MAP_SIZE + shadowZ;

			if( index >= 0 && index < MAP_SIZE * MAP_SIZE )
			{
				pTempLightMap[index] = dwColor;
				SetDirtyPatch( shadowX, shadowZ );
			}
		}

#else
//WORLD 충돌지점 버젼

		kLight.x *= MPU;
		kLight.z *= MPU;


		float fLen = m_pWorld->IntersectRayTerrain3( kLight, kDir );
		if( fLen < 100.0f && fLen > 2.0f )
		{
			D3DXVECTOR3 kLight2;

			for( float f = 4.0f; f < (fLen); f += 4.0f )
			{
				kLight2 = kLight + kDir * f;
				kLight2.x /= MPU;
				kLight2.z /= MPU;
			
				DWORD shadowX = (DWORD)(kLight2.x );
				DWORD shadowZ = (DWORD)(kLight2.z );
				DWORD dwColor = 0x55303030;

				int index = shadowX * MAP_SIZE + shadowZ;
				if( index >= 0 && index < MAP_SIZE * MAP_SIZE )
				{
					pTempLightMap[index] = dwColor;
					SetDirtyPatch( shadowX, shadowZ );
				}
			}
		}
#endif 

	
	//	kLight = kLight + kDir * (fDisHeight); 

	//	int shadowX = (int)(kLight.x );
	//	int shadowZ = (int)(kLight.z );
	//	int index = 0;
	//	DWORD dwColor = 0x77303030;

	//	index = shadowX * MAP_SIZE + shadowZ;
	//	pTempLightMap[ index ] = dwColor;
	//	SetDirtyPatch( shadowX, shadowZ );

		//int x2 = x, z2 = z;
		//for( ; x2 >= shadowX; --x2 )
		//{
		//	for( ; z2 <= shadowZ; ++z2 )
		//	{
		//		int x3 = x2 ;
		//		int z3 = z2 ;

		//		index = x3 * MAP_SIZE + z3;
		//		pTempLightMap[ index ] = dwColor;
		//		SetDirtyPatch( x3, z3 );

		//	}
		//}

		//for( ; z2 <= shadowZ; ++z2 )
		//	for( ; x2 >= shadowX; --x2 )
		//	{
		//		int x3 = z2;
		//		int z3 = x2;

		//		index = x3 * MAP_SIZE + z3;
		//		pTempLightMap[ index ] = dwColor;
		//		SetDirtyPatch( x3, z3 );
		//	}


	//	int index = shadowZ * MAP_SIZE + shadowX;
	//	pTempLightMap[index] = dwColor;
		//pBits[ index ] = 0x77303030;

	}
}

BOOL CLandscape::MakeLightMap( )
{
	SetModified( TRUE );

	if( m_pLightMapLayer )
	{
		DeleteLayer( m_pLightMapLayer->m_nTex );
		m_pLightMapLayer = NULL;
		return FALSE;		//test toggle
	}

	m_pLightMapLayer = NewLayer( 144 );
	assert( m_pLightMapLayer );

	for(int i=0;i<MAP_SIZE*MAP_SIZE;i++)
		m_pLightMapLayer->m_pTempLightMap[i] = D3DCOLOR_ARGB( 0, 30, 30, 30 );

	// 아침 낯 저녁의 세방향 빛을 모조리 함계 표현해보자.
//	D3DXVECTOR3 vLightDir1( 4.0f, 1.0f, 10.0f );		// 빛 방향.

	D3DXVECTOR3 vLightDir2( 10.0f, 1.0f, 10.0f );		// 빛 방향.
//	D3DXVECTOR3 vLightDir3( 4.0f, 1.0f, -10.0f );		// 빛 방향.
	
//	D3DXVec3Normalize( &vLightDir1, &vLightDir1 );
	D3DXVec3Normalize( &vLightDir2, &vLightDir2 );
//	D3DXVec3Normalize( &vLightDir3, &vLightDir3 );
	
//	FillLightColor( &vLightDir1 );
	FillLightColor( &vLightDir2 );
//	FillLightColor( &vLightDir3 );

	return TRUE;
}

float CLandscape::ComputeShade(DWORD cellRow, DWORD cellCol, D3DXVECTOR3* dirLight )
{
	// 삼각형의 세점을 얻어서
	float heightA = GetHeight(cellRow,   cellCol);
	float heightB = GetHeight(cellRow, cellCol+1);
	float heightC = GetHeight(cellRow+1, cellCol);

	if( 0.0f == heightA || 0.0f == heightB || 0.0f == heightC )
		return 0.0f;

	// 두벡터를 만들고
	D3DXVECTOR3 u((float)MPU, (float)(heightB - heightA), 0.0f);
	D3DXVECTOR3 v(0.0f,       (float)(heightC - heightA), -(float)MPU);

	// 법선벡터를 만들어서
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	//지면이 평면이라면 패스
//	D3DXVECTOR3 kUp( 0.0f, 1.0f, 0.0f );
//	float fDotUp = D3DXVec3Dot( &n, &kUp );
//	if( fDotUp > 0.95f )
//		return 0.0f;

	// 내적을 취해 각도를 계산해 낸다

	float cosine = D3DXVec3Dot(&n, dirLight);

//	if( cosine < 0.0f )
//		return fabs(cosine * 0.1f);	// 원래는 

//	if(cosine < 0.0f)			//빛과 90도이상으로 벌어지게 되면 빛이 닿지 않는다고 판단할수있겟다.
//		cosine = 0.0f;	

	if( cosine > 0.0f )
		return 0.0f;

	return fabs(cosine);
}
#endif //__BS_SUPPORT_LIGHTMAP

CLandLayer::CLandLayer(LPDIRECT3DDEVICE9 pd3dDevice,WORD nTex) 
{
	m_bVisible = TRUE;
	m_nTex = nTex;
	if( pd3dDevice ) 
	{
#ifdef __16BITLIGHT
		pd3dDevice->CreateTexture( MAP_SIZE, MAP_SIZE, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED,
										   &m_pLightMap, NULL );
#else
		pd3dDevice->CreateTexture( MAP_SIZE, MAP_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
										   &m_pLightMap, NULL );
#endif
		D3DLOCKED_RECT rectLock;
		m_pLightMap->LockRect( 0, &rectLock, 0, 0 );
#ifdef __16BITLIGHT
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
			((WORD *)rectLock.pBits)[i] = D3DCOLOR_ARGB16( 0, 127, 127, 127 );
#else
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
			((DWORD *)rectLock.pBits)[i] = D3DCOLOR_ARGB16( 0, 127, 127, 127 );
#endif
		m_pLightMap->UnlockRect( 0 );

		for( int i = 0; i < NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE; i++ ) 
		{
			m_aPatchEnable[ i ]=FALSE;
		}
	}
#ifdef __BEAST
#ifdef __16BITLIGHT
	m_pTempLightMap = new WORD[ MAP_SIZE * MAP_SIZE ];
	for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ ) 
		m_pTempLightMap[ i ] = D3DCOLOR_ARGB16( 0, 127, 127, 127 );
#else
	m_pTempLightMap = new DWORD[ MAP_SIZE * MAP_SIZE ];
	for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ ) 
		m_pTempLightMap[ i ] = D3DCOLOR_ARGB( 0, 127, 127, 127 );
#endif
#endif
}
CLandLayer::~CLandLayer() 
{
	SAFE_RELEASE( m_pLightMap );
#ifdef __BEAST
	SAFE_DELETE_ARRAY( m_pTempLightMap );
#endif
}

void CLandLayer::SetPatchEnable(int x, int z,BOOL bEnable)
{
	if( x == 0 && z == 0 )
	{
		int b = 0;
	}

	if(!(x<0 || x>=NUM_PATCHES_PER_SIDE || z<0 || z>=NUM_PATCHES_PER_SIDE ))
		m_aPatchEnable[x+z*NUM_PATCHES_PER_SIDE]=bEnable;
}


//if( tempAlphaSum != 0 && pLayer->m_nTex != nTex )
//{
//	DWORD newColor = pLayer->m_pTempLightMap[x+z*MAP_SIZE];
//	//원래 색상과의 알파 혼합만을 계산한다.
//	pLayer->m_pTempLightMap[x+z*MAP_SIZE] = newColor | ( (255-nOpacity)*(pLayer->m_pTempLightMap[x+z*MAP_SIZE]>>24)/tempAlphaSum )<<24;









































