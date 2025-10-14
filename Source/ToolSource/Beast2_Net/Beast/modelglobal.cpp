#include "stdafx.h"
#include <dxerr9.h>

#include "defineObj.h"

#include "path.h"
#include "ModelGlobal.h"

//#include "..\_UnhandledException\ExceptionHandler.h"
#if !defined( __COLA ) && !defined(__SFXEDITOR)
#include "Project.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LARGE_INTEGER		g_lnFreq;
LARGE_INTEGER		g_lnPrev, g_lnCurr;

double		g_dUps;		// update per sec - 화면 갱신 시간
//double		g_dKps;		// key per sec - 한 애니메이션 키가 넘어가는데 기다리는 시간
//DWORD	g_tmCurrent = 0;		// 전역으로 사용할 현재 시간.

//int		g_nRenderCnt = 0;	// 렌더화면때마다 하나씩 카운트 되는 변수
//int		g_nProcessCnt = 0;	// 프로세스마다 하나씩 카운트 되는 변수.

void	InitUPS( void )
{
//	g_dKps = (double)1 / 60;	 								// 한개의 애니메이션 키가 넘어가는데 걸리는 시간.
	QueryPerformanceFrequency( &g_lnFreq );			// 초당 주파수 계산

	QueryPerformanceCounter( &g_lnCurr );				// 카운터를 똑같이 맞춘다.
	g_lnPrev = g_lnCurr;
}
// 한 화면 갱신에 걸리는 시간 계산
void	CalcUPS( void )
{
	g_lnPrev = g_lnCurr;
	QueryPerformanceCounter( &g_lnCurr );
	// 이전 화면에서 지금 화면까지 걸린 시간
	g_dUps = (double)(g_lnCurr.QuadPart - g_lnPrev.QuadPart) / g_lnFreq.QuadPart;		
}

float g_fHairLight  = 1.5f;
float s_fDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float s_fAmbient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
D3DXVECTOR4 s_vLight( 0.0f, 0.0f, 1.0f, 0.0f );		// 라이트 벡터
D3DXVECTOR4 s_vLightPos( 0.0f, 5000.0f, 0.0f, 0.0f );		// 라이트 포지션
D3DXMATRIX s_mViewProj;		// 
D3DXMATRIX s_mView;		// 
D3DXMATRIX s_mProj;		// 
BOOL	s_bLight;
BOOL	s_bNight;
BOOL	s_bFog;

float s_fFogStart = 0;
float s_fFogEnd   = 100;


void	SetDiffuse( float r, float g, float b )
{
	s_fDiffuse[0] = r;	s_fDiffuse[1] = g;	s_fDiffuse[2] = b;
}

void	SetAmbient( float r, float g, float b )
{
	s_fAmbient[0] = r;	s_fAmbient[1] = g;	s_fAmbient[2] = b;
}

void	SetLightVec( const D3DVECTOR &vLight )
{
	s_vLight.x = vLight.x;
	s_vLight.y = vLight.y;
	s_vLight.z = vLight.z;
}

void	SetLightPos( const D3DVECTOR &vLight )
{
	s_vLightPos.x = vLight.x;
	s_vLightPos.y = vLight.y;
	s_vLightPos.z = vLight.z;
}

void	SetTransformViewProj( const D3DXMATRIX &mViewProj )
{
	s_mViewProj = mViewProj;
}

void	SetTransformView( const D3DXMATRIX &mView )
{
	s_mView = mView;
}

void	SetTransformProj( const D3DXMATRIX &mProj )
{
	s_mProj = mProj;
}

// 렌더스테이트 라이트가 켜져있는가 아닌가.
void	SetLight( BOOL bLight )
{
	s_bLight = bLight;
}

// 밤이면 트루.
void	SetNight( BOOL bNight )
{
	s_bNight = bNight;
}

void	SetFog( BOOL bFog )
{
	s_bFog = bFog;
}

CModelGlobal g_ModelGlobal;

void	CModelGlobal::SetCamera( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDest )
{
	m_vCameraPos = vPos;
	m_vCameraDest = vDest;
	m_vCameraForward = vDest - vPos;
	D3DXVec3Normalize( &m_vCameraForward, &m_vCameraForward );
}

extern CPartsLaser g_Laser;

HRESULT CModelGlobal::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __XEFFECT0530
	for( int i=0; i<MAX_ELETEXTURE; i++ )
	{
		SAFE_RELEASE(m_pElecTexture[i]);
	}	
#else //__XEFFECT0530
	SAFE_RELEASE( m_pElecTexture[0] );
	SAFE_RELEASE( m_pElecTexture[1] );
	SAFE_RELEASE( m_pElecTexture[2] );
#endif //__XEFFECT0530


	return S_OK;
}

HRESULT CModelGlobal::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __COLA
	D3DXCreateTextureFromFile( pd3dDevice, MakeExePath( "", "etc_elec01.tga" ), &m_pElecTexture[0] );
//		Error( "etc_elec01.tga 텍스쳐 못읽음" );	// 편의상 에러는 표시하지 말자.
	D3DXCreateTextureFromFile( pd3dDevice, MakeExePath( "", "etc_elec02.tga" ), &m_pElecTexture[1] );
//		Error( "etc_elec02.tga 텍스쳐 못읽음" );
	D3DXCreateTextureFromFile( pd3dDevice, MakeExePath( "", "etc_elec03.tga" ), &m_pElecTexture[2] );
//		Error( "etc_elec03.tga 텍스쳐 못읽음" );
#else
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec01.tga" ), &m_pElecTexture[0] );
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec02.tga" ), &m_pElecTexture[1] );
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec03.tga" ), &m_pElecTexture[2] );
 #ifdef __XEFFECT0530

	char buffer[64] = {0};
	for( int i=3; i< MAX_ELETEXTURE; i++ )
	{
		sprintf( buffer, "etc_elec%.2d.tga", i+1 );
		LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, buffer ), &m_pElecTexture[i] );
	}
#endif // xEffect0530

#endif // not Cola
	return S_OK;
}

//----------------------------------------------------------------------
//	 shadow
//----------------------------------------------------------------------
LPD3DXRENDERTOSURFACE   g_pRenderToSurface = NULL;
LPDIRECT3DSURFACE9      g_pShadowSurface   = NULL;
LPDIRECT3DTEXTURE9      g_pShadowTexture   = NULL; 

D3DXMATRIX g_mViewLight;		// 빛으로부터 플레이어쪽으로 보는 뷰 매트릭스.
D3DXMATRIX g_mShadowProj;		// 쉐도우 프로젝션
BOOL	g_bShadow = TRUE;

// 쉐도우맵 텍스쳐 생성.
#if !defined( __BEAST ) && !defined( __SFXEDITOR ) && !defined(__XTOOL)
BOOL CreateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat )
{  
#ifndef __WORLDSERVER
	if ( g_pRenderToSurface )	return TRUE;	// 이미 있으면 생성안함.

	TRACE( "%d", (int)backBufferFormat );
	HRESULT hr;
	hr = D3DXCreateRenderToSurface( pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, backBufferFormat, FALSE, D3DFMT_UNKNOWN, &g_pRenderToSurface);
	hr = D3DXCreateTexture( pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET, backBufferFormat,  D3DPOOL_DEFAULT, &g_pShadowTexture );
	if( hr == E_OUTOFMEMORY )
	{
		Error( "쉐도우 맵 생성중 메모리 부족 %d, %d", g_Option.m_nShadow, g_Option.m_nTextureQuality );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if ( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		Error( "쉐도우 맵 생성중 비디오 메모리 부족 %d, %d", g_Option.m_nShadow, g_Option.m_nTextureQuality );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if( FAILED( hr ) )
	{
		LPCTSTR szErr = Error( "쉐도우 맵 생성 에러 %s, %d, %d", DXGetErrorString9(hr), g_Option.m_nShadow, g_Option.m_nTextureQuality );
		ADDERRORMSG( szErr );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if( g_pShadowTexture )
	{
		g_pShadowTexture->GetSurfaceLevel( 0, &g_pShadowSurface );
	}
	else
	{
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}

	g_bShadow = TRUE;
#endif // not WorldServer
	return TRUE;
}
#endif

void DeleteShadowMap( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( g_pShadowSurface );
	SAFE_RELEASE( g_pShadowTexture );
	SAFE_RELEASE( g_pRenderToSurface );
	g_bShadow = FALSE;
}
// 쉐도우맵용 렌더타겟에 쉐도우맵 렌더링
void RenderShadowMap( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

// 디버깅용 쉐도우맵창을 그린다.
void RenderShadowMapInfo( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	float vertex2D[4][4+2] = 
	{
		{   0,  0,0,1, 0,0 },
		{ 128,  0,0,1, 1,0 },
		{ 128,128,0,1, 1,1 },
		{   0,128,0,1, 0,1 },
	};
	pd3dDevice->SetTexture( 0, g_pShadowTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertex2D, sizeof(float)*(4+2) );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
}


//
// 그림자 리시버들을 렌더링 하기전에 이것을 호출한다.
//
void SetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage, const D3DXMATRIX &mView )
{
	if( g_bShadow == FALSE )	return;
	D3DXMATRIX mShadowUV; 

	D3DXMATRIX mCameraToWorld;
	D3DXMatrixInverse( &mCameraToWorld, NULL, &mView );

	D3DXMATRIX scale,offset;
	D3DXMatrixScaling( &scale, 0.5f, -0.5f, 1.0f );
	D3DXMatrixTranslation( &offset, 0.5f, 0.5f, 0.0f );

	D3DXMatrixMultiply( &mShadowUV, &mCameraToWorld, &g_mViewLight );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &g_mShadowProj );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &scale );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &offset );

	pd3dDevice->SetTexture( 0, NULL );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTexture( 1, NULL );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	HRESULT hr;
	hr = pd3dDevice->SetTexture( nShadowStage, g_pShadowTexture );
	
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );
	hr = pd3dDevice->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + nShadowStage), &mShadowUV );
	// 여기까지 괜찮음.
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_CURRENT );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
//	return;
//	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_CURRENT );
	//	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
}

void ResetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage )
{
	if( g_bShadow == FALSE )	return;
//	D3DXMATRIX m;
//	D3DXMatrixIdentity( &m );
//	pd3dDevice->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + nShadowStage), &m );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTexture( nShadowStage, NULL );
	return;
}

#if !defined( __COLA ) && !defined(__SFXEDITOR)
////////////////////////////////////////////////////////////////////////////
#ifdef __XEFFECT1006
void	CPartsFire::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx )
#else
void	CPartsFire::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos )
#endif
{
	int		i;
	D3DXMATRIX	m_mScale;
	
	D3DXMatrixIdentity( &m_mScale );
	
	for( i = 0; i < MAX_PARTS_FIRE; i ++ )
	{
		if( m_pList[i] )	continue;
#ifdef __XEFFECT1006
		m_pList[i] = (CSfxModel *)prj.m_modelMng.LoadModel( pd3dDevice, OT_SFX, dwSfx );
#else
		m_pList[i] = (CSfxModel *)prj.m_modelMng.LoadModel( pd3dDevice, OT_SFX, XI_NAT_FIRE01 );
#endif
		m_pList[i]->m_matScale = m_mScale;
		m_vList[i] = vPos;
		break;
	}
	
}

void	CPartsFire::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	int		i;
	CSfxModel *pSfxModel = NULL;
	
//	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	
	for( i = 0; i < MAX_PARTS_FIRE; i ++ )
	{
		pSfxModel = m_pList[i];
		if( pSfxModel == NULL )		continue;
		
		if( pSfxModel->Process() )	// 1play 끝남.
		{
			m_pList[i] = NULL;
			continue;
		}
		D3DXVec3TransformCoord( &pSfxModel->m_vPos, &m_vList[i], mWorld );
#ifndef __XEFFECT1006
		pSfxModel->m_vPos.y += 0.11f;
#endif // not xItem0914
		
//		pSfxModel->m_vPos = m_vList[i];
//		pSfxModel->m_vPos.y += 0.11f;
//		pSfxModel->m_vRotate.y = GetAngle();
//		pSfxModel->m_vScale = GetScale();
//		pSfxModel->m_matScale = m_mScale;
		pSfxModel->m_vScale.x = pSfxModel->m_vScale.y = pSfxModel->m_vScale.z = pSfxModel->m_pModelElem->m_fScale;
		pSfxModel->Render( pd3dDevice, NULL );
		
	}
}
#endif // not cola

////////////////////////////////////////////////////////////////////////////
void CPartsBeam :: Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize, int nLevel )
{
	if( m_nMax >= MAX_BEAM )	m_nMax = 0;
	m_vBeam[m_nMax * 2 + 0] = v1;
	m_vBeam[m_nMax * 2 + 1] = v2;

	D3DXVECTOR3 v = v2 - v1;
	v /= 2.0f;

	m_vCenter[m_nMax] = v1 + v;
//	m_vBeam[m_nMax * 2 + 0] -= m_vCenter[m_nMax];
//	m_vBeam[m_nMax * 2 + 1] -= m_vCenter[m_nMax];
	m_fSize[ m_nMax ] = fSize;
	m_nLevel[ m_nMax ] = nLevel;
	m_vLast = v2;
	
	m_bActive = TRUE;
	m_nMax ++;
}

// 연속으로 빔생성.
// 최초 발생지점은 원점.
void CPartsBeam :: Create( const D3DXVECTOR3 &v )
{
	if( m_nMax >= MAX_BEAM )	m_nMax = 0;
	if( m_bActive == FALSE )	// 최초 시작.
	{
		m_vBeam[ 0 ] = v;	// 최초 시작위치
		m_bActive = TRUE;
	} else
	{
		m_vBeam[ m_nMax * 2 + 0 ] = m_vLast;		// 시작좌표는 바로 이전 빔의 마지막 좌표로 한다.
	}

	m_vBeam[ m_nMax * 2 + 1 ] = v;

	D3DXVECTOR3 vh = m_vBeam[ m_nMax * 2 + 1 ] - m_vBeam[ m_nMax * 2 + 0 ];
	vh /= 2.0f;

	m_vCenter[m_nMax] = m_vBeam[ m_nMax * 2 + 0 ] + vh;
	m_vBeam[ m_nMax * 2 + 0 ] -= m_vCenter[m_nMax];
	m_vBeam[ m_nMax * 2 + 1 ] -= m_vCenter[m_nMax];
//	m_fScale[ m_nMax ] = 1.0f;
	m_fSize[ m_nMax ] = 0.05f;

	m_vLast = v;
	
	m_nMax ++;
}

// 두점 사이의 임의의 점에서 빔을 생성시킴.
void	CPartsBeam :: Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel )
{
/*
	D3DXVECTOR3 v;
	FLOAT		fSlp = xRandomF(1);
	D3DXVec3Lerp( &v, &g_vForce1, &g_vForce2, fSlp );		// 검광 사이의 임의의 점 선택
	
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	
	Create( v );
*/

	D3DXVECTOR3 v, vv;
	float fSize = 0.025f + (xRandomF(1) / 10.0f);

	D3DXVec3Lerp( &v, &v1, &v2, 0.25f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( v1, v, fSize, nLevel );
	vv = v;

	D3DXVec3Lerp( &v, &v1, &v2, 0.5f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( vv, v, fSize, nLevel );
	vv = v;
	
	D3DXVec3Lerp( &v, &v1, &v2, 0.75f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( vv, v, fSize, nLevel );
	vv = v;

	Create( vv, v2, fSize, nLevel );
	
}

void CPartsBeam :: Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward,
					  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel )
{
	Generate( v1, v2, nLevel );		// 번개 이펙트 생성.

	if( m_bActive == FALSE )	return;
	int		i;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_BEAM );
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 128, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR  );
	//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
	//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 
	

	for( i = 0; i < MAX_BEAM; i ++ )
	{
		if( m_vCenter[i].x == 0 )	continue;
		D3DXVECTOR3 vB = m_vBeam[i * 2 + 0] - m_vBeam[i * 2 + 1];		// 빔의 방향벡터.
		D3DXVECTOR3 vE = vEye - m_vBeam[i * 2 + 0];	// 카메라에서 빔의 시작점을 바라보는 벡터
		D3DXVECTOR3 vP;
		D3DXVec3Cross( &vP, &vB, &vE );		// 빔방향벡터와 시선벡터의 외적.

		D3DXVECTOR3 vF = vForward;		// 앞쪽 벡터.
		D3DXVECTOR3 vFxP;
		D3DXVec3Cross( &vFxP, &vF, &vP );
		D3DXVec3Normalize( &vFxP, &vFxP );

		D3DXVECTOR3 vR;
		D3DXVec3Cross( &vR, &vF, &vFxP );		// 오른쪽 벡터

		D3DXMATRIX m1, m2;
		m1._31 = vF.x;	m1._21 = vFxP.x;	m1._11 = vR.x;		m1._41 = m_vBeam[i * 2 + 0].x;
		m1._32 = vF.y;	m1._22 = vFxP.y;	m1._12 = vR.y;		m1._42 = m_vBeam[i * 2 + 0].y;
		m1._33 = vF.z;	m1._23 = vFxP.z;	m1._13 = vR.z;		m1._43 = m_vBeam[i * 2 + 0].z;
		m1._34 = 0;		m1._24 = 0;			m1._14 = 0;			m1._44 = 1;
		m2 = m1;
		m2._41 = m_vBeam[i * 2 + 1].x;
		m2._42 = m_vBeam[i * 2 + 1].y;
		m2._43 = m_vBeam[i * 2 + 1].z;
		m2._44 = 1;

//		float fSize = 0.05f;
		float fSize = m_fSize[i];
		
	//	if( nCnt & 1 )
	//	{
	//		fSize = 0.12f;
	//	}
	//	fSize += (nCnt * 0.01f);
		
		D3DXVec3TransformCoord( &m_vList[i * 4 + 0].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 1].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 2].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m2 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 3].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m2 );

//		m_fScale[i] += 0.01f;
		float f2 = 1.0f; //m_fScale[i];
//		D3DXMATRIX /*mScale,*/ mTrans;
//		D3DXMatrixScaling( &mScale, f2, f2, f2 );
//		D3DXMatrixTranslation( &mTrans, m_vCenter[i].x, m_vCenter[i].y, m_vCenter[i].z );
//		mTrans *= *mWorld;

//		mScale *= mTrans;

//		pd3dDevice->SetTransform( D3DTS_WORLD,	  &mTrans );		// Set World Transform 
		
	//	D3DXVec3TransformCoord( &m_vList[0].pos, &m_vList[0].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[1].pos, &m_vList[1].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[2].pos, &m_vList[2].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[3].pos, &m_vList[3].pos, &mScale );
		
	//  m_vList[0].pos /= 100.0f;
		m_vList[i * 4 + 0].tu = 0.0f;
		m_vList[i * 4 + 0].tv = 0.0f;
		m_vList[i * 4 + 1].tu = 1.0f;
		m_vList[i * 4 + 1].tv = 0.0f;
		m_vList[i * 4 + 2].tu = 0.0f;
		m_vList[i * 4 + 2].tv = 1.0f;
		m_vList[i * 4 + 3].tu = 1.0f;
		m_vList[i * 4 + 3].tv = 1.0f;

#ifdef __XEFFECT0530
		pd3dDevice->SetTexture( 0, g_ModelGlobal.m_pElecTexture[m_nLevel[i] * 3 + xRandom(3)] );
#else
		pd3dDevice->SetTexture( 0, g_ModelGlobal.m_pElecTexture[xRandom(3)] );
#endif

		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &m_vList[i * 4], sizeof(FVF_BEAM) );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   s_bFog );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	
//	nCnt ++;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#if 1
HRESULT CPartsLaser::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( m_pTexture );
	
	return S_OK;
}


HRESULT CPartsLaser::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __COLA
	D3DXCreateTextureFromFile( pd3dDevice, MakeExePath( "", "etc_Laser01.tga" ), &m_pTexture );
#else
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_Laser01.tga" ), &m_pTexture ) ) )
		Error( "Read not Texture etc_Laser01.tga" );
#endif
	return S_OK;
}


void CPartsLaser::Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize )
{
	m_vBeam[0] = v1;	// 레이저 시작과 끝점.
	m_vBeam[1] = v2;
	m_fSize = fSize;
	
	m_bActive = TRUE;
}

void CPartsLaser::SetPos( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize )
{
	FLOAT fExtent = 0.875f + (sinf( m_fRadian ) / 8.0f);	// 0.875 + -0.125 ~ 0.125(0.25범위)
	fSize *= fExtent;			// 기본폭을 퍼센트로 약간씩 조절함.
	Create( v1, v2, fSize );	// 레이저 이펙트 생성.
}

//
// v1, v2 레이저 시작과 끝. 로컬좌표
// fSize : 레이저 폭, 여기에 내부적으로 커졌다 작아졌다하는 크기가 더해진다.
//
void CPartsLaser :: Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward
					  /*const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, FLOAT fSize*/ )
{
//	Generate( v1, v2 );		// 레이저 이펙트 생성.

	if( m_bActive == FALSE )	return;
	m_fRadian += 0.2f;
	
	//	int		i;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_BEAM );
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 128, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR  );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 
	

	{
//		if( m_vCenter[i].x == 0 )	continue;
		D3DXVECTOR3 vB = m_vBeam[0] - m_vBeam[1];		// 빔의 방향벡터.
		D3DXVECTOR3 vE = vEye - m_vBeam[0];	// 카메라에서 빔의 시작점을 바라보는 벡터
		D3DXVECTOR3 vP;
		D3DXVec3Cross( &vP, &vB, &vE );		// 빔방향벡터와 시선벡터의 외적.

		D3DXVECTOR3 vF = vForward;		// 앞쪽 벡터.
		D3DXVECTOR3 vFxP;
		D3DXVec3Cross( &vFxP, &vF, &vP );
		D3DXVec3Normalize( &vFxP, &vFxP );

		D3DXVECTOR3 vR;
		D3DXVec3Cross( &vR, &vF, &vFxP );		// 오른쪽 벡터

		D3DXMATRIX m1, m2;
		m1._31 = vF.x;	m1._21 = vFxP.x;	m1._11 = vR.x;		m1._41 = m_vBeam[0].x;
		m1._32 = vF.y;	m1._22 = vFxP.y;	m1._12 = vR.y;		m1._42 = m_vBeam[0].y;
		m1._33 = vF.z;	m1._23 = vFxP.z;	m1._13 = vR.z;		m1._43 = m_vBeam[0].z;
		m1._34 = 0;		m1._24 = 0;			m1._14 = 0;			m1._44 = 1;
		m2 = m1;
		m2._41 = m_vBeam[1].x;
		m2._42 = m_vBeam[1].y;
		m2._43 = m_vBeam[1].z;
		m2._44 = 1;

		FLOAT fSize = m_fSize;
		
		D3DXVec3TransformCoord( &m_vList[0].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[1].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[2].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m2 );
		D3DXVec3TransformCoord( &m_vList[3].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m2 );

		float f2 = 1.0f;
		
		m_vList[0].tu = 0.0f;		m_vList[0].tv = 0.0f;
		m_vList[1].tu = 1.0f;		m_vList[1].tv = 0.0f;
		m_vList[2].tu = 0.0f;		m_vList[2].tv = 1.0f;
		m_vList[3].tu = 1.0f;		m_vList[3].tv = 1.0f;

		pd3dDevice->SetTexture( 0, m_pTexture );

		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_vList, sizeof(FVF_BEAM) );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   s_bFog );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

	m_nCount ++;
} // CPartsLaser::Render

#endif // 0
//----------------------------------------------------------------------
//	 shadow
//----------------------------------------------------------------------





