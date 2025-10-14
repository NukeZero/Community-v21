#include "stdafx.h"
#include "region.h"
#include "defineWorld.h"
#include "WorldDoc.h"
#include "worldview.h"


#ifdef _DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static	char	THIS_FILE[] = __FILE__;
#endif

//#ifdef __NEWPROCESS
CObj* CWorld::m_aobjCull[ MAX_DISPLAYOBJ ];
CObj* CWorld::m_asfxCull[ MAX_DISPLAYSFX ];
int CWorld::m_nObjCullSize = 0;
int CWorld::m_nSfxCullSize = 0;
//#endif


CWorld::CWorld()
{
	m_nLandWidth	= 0;
	m_nLandHeight	= 0;
	m_nDeleteObjs	= 0;
	m_szFileName[0]	= '\0';
	m_fMaxHeight = 270.0f;
	m_fMinHeight = 85.0f;
	m_bFly = TRUE;
	m_bIsIndoor  = FALSE;
	m_dwDiffuse  = 0;
	m_v3LightDir = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	m_dwIdMusic  = 0;
	
	m_bIsContinent = FALSE;
	m_bIsUsing24Light = FALSE;
	
#ifdef __N_TRIGGER_AREASET_050203
	m_nPKMode = 0;
	m_szWorldName[0] = '\0';
#endif // __N_TRIGGER_AREASET_050203	

	//g_pIB	= NULL;
	//g_pExIB	= NULL;
	m_fElapsedTime	=0;
	m_nVisibilityLand	= (int)( m_fFarPlane / ( MAP_SIZE * MPU ) );
	m_fFogStartValue	= 70;//m_fFarPlane - 384;	// kentauji edited 5/28
	m_fFogEndValue	= 400;//m_fFarPlane + 300;/// 2;	//1000.0f;
	m_fFogDensity	= 0.0f;
	m_pd3dDevice	= NULL;
	//m_pCloudVertexBuffer	= NULL;
	//m_pWaterVertexBuffer	= NULL;
	m_pBoundBoxVertexBuffer = NULL;
	m_nCharLandPosX = -1;
	m_nCharLandPosZ = -1;
	
	//m_nWaterVertexNum    	= 0;
	//m_nCloudVertexNum	    = 0;

	m_bViewGrid	= FALSE;
	m_bViewGridPatch = FALSE;
	m_bViewGridLandscape = FALSE;
	m_bViewWireframe = FALSE;
	m_bViewSkybox = TRUE;
	m_bViewTerrain = TRUE;
	m_bViewAllObjects = TRUE;
	m_bViewFog = TRUE;
	m_bViewWater = TRUE;
	m_bViewName	= TRUE;
	m_bViewLight = TRUE;
	m_bViewWeather = TRUE;//FALSE;
	m_bViewBoundBox	= FALSE;
	m_bCullObj = TRUE;
	m_bProcess = TRUE;
	m_bViewObj = TRUE;
	m_bViewMover = TRUE;
	m_bViewItem	= TRUE;
	m_bViewRegion = TRUE;
	m_bViewSpawn = TRUE;
#ifdef __CSC_RENDER_AXIS
	m_bViewAxis = FALSE;
#endif //__CSC_RENDER_AXIS
//#ifdef __BEAST
//	m_bFixedHour = TRUE;
//#else
//	m_bFixedHour = FALSE;
//#endif
//	m_nFixedHour = 12;
	
	m_bViewHeightAttribute = TRUE;
	m_bViewLODObj = TRUE;
	m_bViewIdState = FALSE;
	// 디버그 관련 플랙 끝 

	m_dwAmbient	= D3DCOLOR_ARGB( 255,128,128,128);
	m_pObjFocus		= NULL;
	m_idObjFocusOld	= NULL_ID;
	m_dwLightIndex	= 0;
	m_pCamera	= NULL;
	m_apLand	= NULL;

	// 디버그 관련 플랙  
#ifdef __BEAST
	//m_pGridVertexBuffer	    = NULL;
	//m_nGridVertexNum	    = 0   ;
	m_pBoundBoxVertexBuffer	= NULL;
	m_nBoundBoxVertexNum	= 0   ;
	//m_pCircleVertexBuffer	= NULL;
	//m_nCircleVertexNum	    = 0   ;
	m_pAxisVertexBuffer     = NULL;
	m_nAxisVertexNum        = 0   ;
	m_nLightType            = 0   ;
	m_nLightHour            = 12  ;
	m_lightDirect.r1        = 0.5f;
	m_lightDirect.g1        = 0.5f;
	m_lightDirect.b1        = 0.5f;
	m_lightDirect.r2        = 0.5f;
	m_lightDirect.g2        = 0.5f;
	m_lightDirect.b2        = 0.5f;
#endif	// __BEAST

	m_dwWorldID	= NULL_ID;
	m_dwIdWorldRevival = WI_WORLD_NONE; 
	ZeroMemory( m_szKeyRevival, sizeof( m_szKeyRevival ) );	
	m_apLandAttr = NULL;

#ifndef __ONLINE
	m_bLoadScriptFlag = FALSE;
#endif
#ifdef  __V050322_CACHEQUEST
	m_timerScript.Set( MIN( 1 ) );
#endif // __VGLOVALSCRIPT_20050319

#ifdef __USE_STR_TBL0407
	m_nString	= 0;
#endif	// __USE_STR_TBL0407

	m_bIsWorkingEV = FALSE;
	m_dwAddedTime = 0;
	m_dwOldTime = 0;

	for( int i=0; i<3; ++i )
	{
		m_strSkyTex[i] = DEFAULT_FNAME;
		m_strCloudTex[i] = DEFAULT_FNAME;
	}

	m_strSunTex = DEFAULT_FNAME;
	m_strMoonTex = DEFAULT_FNAME;
	m_strSeaCloud = DEFAULT_FNAME;

	m_pTextureSeaCloud = NULL;

	m_bFixedLight = FALSE;
}

CWorld::~CWorld()
{
	Free();
}

void CWorld::Free()
{
	// Light 파괴 
	POSITION pos 
		= m_mapLight.GetStartPosition();
	CLight* pLight;
	CString string;
	while( pos )
	{
		m_mapLight.GetNextAssoc( pos, string, (void*&)pLight );
		SAFE_DELETE( pLight );
	}
	m_mapLight.RemoveAll();
	if( m_apLand != NULL )
	{
		for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
			SAFE_DELETE( m_apLand[ i ] );
		SAFE_DELETE_ARRAY( m_apLand );
	}
	/*
	// 나머지 파괴 
	#ifdef __BEAST
	//SAFE_RELEASE( m_pGridVertexBuffer );
	SAFE_RELEASE( m_pBoundBoxVertexBuffer );
	SAFE_RELEASE( m_pCircleVertexBuffer );
	#endif
	*/
	m_pObjFocus	= NULL;
	m_nDeleteObjs	= 0;

	for( ObjItor k = m_aryObjBuffer.begin(); k != m_aryObjBuffer.end(); ++k )
	{
		CObj* pObj = *k;
		delete pObj;
 	}
	m_aryObjBuffer.clear();


	SAFE_DELETE_ARRAY( m_apLandAttr );
}
void CWorld::LoadAllMoverDialog()
{


}

#ifdef __BEAST
void CWorld::SetObjFocus( CObj* pObj ) 
{ 
	m_pObjFocus = pObj; 
}
#endif

// removed /2003/10/09

void CWorld::AddLight( CLight* pLight )
{
	pLight->m_dwIndex = m_dwLightIndex++;
	m_mapLight.SetAt( pLight->m_strKey, pLight );
	//m_pd3dDevice->SetLight( pLight->m_dwIndex, pLight );
	//m_pd3dDevice->LightEnable( pLight->m_dwIndex, TRUE );
}

CLight* CWorld::GetLight( LPCTSTR lpszKey )
{
	CLight* pLight = NULL;
	m_mapLight.Lookup( lpszKey, (void*&)pLight );
	return pLight;
}


BOOL CWorld::AddObj( CObj* pObj, BOOL bAddItToGlobalId )
{
	if( pObj->GetWorld() )
	{
		WriteError("CWorld::AddObj obj's world is NOT NULL." );
		return FALSE;
	}

	D3DXVECTOR3 v = pObj->GetPos();
	if( VecInWorld( v ) == FALSE )
	{
		WriteError("CWorld::AddObj world id:%d x:%f y:%f z:%f", GetID(), v.x, v.y, v.z );
		return FALSE;
	}

	if( pObj->IsDynamicObj() )
	{

		// 이 코드는 데드락을 유발하므로(내부적 prj lock), _add에서 실행한다.
		if( bAddItToGlobalId ) 
			( (CCtrl*)pObj )->AddItToGlobalId();

		FLOAT fHeight	= GetLandHeight( v );
		v.y		= ( v.y < fHeight ? fHeight : v.y );
		
		// 예외 처리...Door컨트롤은 땅에 묻혀두 됨
		if( pObj->m_dwType == OT_CTRL )
		{
			CCommonCtrl* pCCtrl = (CCommonCtrl*)pObj;
			CtrlProp* pProp = pCCtrl->GetProp();
			if( pProp )
			{
				if( pProp->dwCtrlKind1 == CK1_DOOR )
					v.y = pObj->GetPos().y;
			}
		}

		pObj->SetPos( v );
	}
	pObj->UpdateLocalMatrix();
	pObj->SetWorld( this );
	if( pObj->m_pAIInterface ) 
		pObj->m_pAIInterface->InitAI();

	InsertObjLink( pObj );
	AddObjArray( pObj );


	return TRUE;
}

void CWorld::RemoveObj( CObj* pObj )
{
	if( pObj->GetWorld() == NULL ) 
		return;
	pObj->SetWorld( NULL );

	RemoveObjLink( pObj );
	RemoveObjArray( pObj );

}

// 모든 루프가 끝나고 메모리에서 delete 된다.
void CWorld::DeleteObj( CObj* pObj )
{
	if( !pObj->IsDelete() )
	{
		pObj->SetDelete( TRUE );
		m_apDeleteObjs[ m_nDeleteObjs++ ] = pObj;
	}
}

BOOL CWorld::AddObjArray( CObj* pObj )
{
	CLandscape* pLandscape	= GetLandscape( pObj );
	if( NULL != pLandscape ) 
	{
		pLandscape->AddObjArray( pObj );
	}
	else 
	{
	}

	return TRUE;
}

void CWorld::RemoveObjArray( CObj* pObj )
{
	CLandscape* pLandscape	= GetLandscape( pObj );

	if( NULL != pLandscape ) 
	{
		pLandscape->RemoveObjArray( pObj );
	}
	else 
	{
	}

}

BOOL CWorld::InsertObjLink( CObj* pObj )
{
	D3DXVECTOR3 vPos	= pObj->GetLinkPos();
	if( VecInWorld( vPos ) == FALSE )
		return FALSE;


	FLOAT rX	= vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ	= vPos.z / MPU;	// - m_nWorldY;
	int mX	= int( rX / MAP_SIZE );
	int mZ	= int( rZ / MAP_SIZE );

	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) 
	{
		return pLandscape->InsertObjLink( pObj );
	}
	else 
	{
	}
	return FALSE;

}

BOOL CWorld::RemoveObjLink2( CObj* pObj )
{
	return TRUE;
}

BOOL CWorld::RemoveObjLink( CObj* pObj )
{
	D3DXVECTOR3	vPos	= pObj->GetLinkPos();
	if( VecInWorld( vPos ) == FALSE )
	{
		WriteError( "ROL//INVALID POS//%d, %d, %f, %f, %f, %f, %f, %f", 
			pObj->GetType(), pObj->GetIndex(), pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z, vPos.x, vPos.y, vPos.z );
		return FALSE;
	}

	FLOAT rX = vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;	// - m_nWorldY;
	int   mX = int( rX / MAP_SIZE );
	int   mZ = int( rZ / MAP_SIZE );

	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->RemoveObjLink( pObj );
	}
	else {
	}
	return FALSE;

}

CObj* CWorld::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel )
{
	if( VecInWorld( vPos ) == FALSE )
		return NULL;

	FLOAT rX = vPos.x / MPU;// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;// ;- m_nWorldY;
	int	mX	= int( rX / MAP_SIZE );
	int	mZ	= int( rZ / MAP_SIZE );
//	int	mX_	= int( int(rX) % MAP_SIZE );
//	int	mZ_	= int( int(rZ) % MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE];
	}
	else {
		return NULL;
	}

}

BOOL CWorld::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
{
	if( VecInWorld( vPos ) == FALSE )
		return FALSE;

	FLOAT rX = vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;	// - m_nWorldY;
	int mX	= int( rX / MAP_SIZE );
	int mZ	= int( rZ / MAP_SIZE );
//	int mX_	= int( int(rX) % MAP_SIZE );
//	int mZ_	= int( int(rZ) % MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		pLandscape->SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE]	= pObj;
	}
	else {
	}

	return TRUE;
} 


void CWorld::Process( FLOAT fElapsedTime )
{

	// 처리 프로세스 
	CHECK1();
	int i, j, k, l, x, y;
	WorldPosToLand( m_pCamera->m_vPos, x, y );
	CLandscape* pLand;
//	CObj* m_apObjProcess[ 5000 ];
	DWORD dwObjProcessNum = 0;
	D3DXVECTOR3 vPos, *pvCameraPos = &m_pCamera->m_vPos;
	float	fLengthSq;
	float	fFarSq = CWorld::m_fFarPlane / 2;
	fFarSq *= fFarSq;
	int		nNumObj;
	CObj* pObj;
	
#ifdef _DEBUG
	int _nCnt = 0;
#endif


	// OT_SHIP이 OT_ITEM이나 OT_MOVER보다 먼저 프로세스 되어야 해서 어쩔수 없이 이 방법을 씀.
	static int idx[ MAX_OBJARRAY ] = { 0, 1, 2, 7, 3, 4, 5, 6 };	
	for( i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++ )
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
#ifdef __BEAST
				for( k = 0; k < MAX_OBJARRAY; k++ )
#else				
				for( k = OT_ANI; k < MAX_OBJARRAY; k++ )
#endif
				{
					CObj** apObject = pLand->m_apObject[ idx[k] ];
					nNumObj = pLand->m_adwObjNum[ idx[k] ];
					for( l = 0; l < nNumObj; l++ )
					{
						if( *apObject && /*(*apObject)->m_pModel->IsAniable() &&*/ (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL )
						{
							vPos = (*apObject)->GetPos() - *pvCameraPos;
							fLengthSq = D3DXVec3LengthSq( &vPos );
							if( fLengthSq < fFarSq ) 
							{
#ifdef _DEBUG
								_nCnt ++;
#endif								
								(*apObject)->Process( 0 );
							}
								//m_apObjProcess[ dwObjProcessNum++ ] = *apObject;
						}
						apObject ++;
					}
				}
			}
		}
	}
	CHECK2("    Visibi");
	
	// 자, 여기서 랜드에 속한 배열에서 바로 프로세싱하지 않고  apObjTemp 배열에 추가한 후에
	// Process한 것에 주목해야한다. Process 과정에서 오브제트가 삭제되거나 추가될 수 있는데
	// 랜드에 속한 배열을 루핑하면서 동일 배열에 추가되거나 삭제되게 되면  버그의  가능성이
	// 높아진다. 따라서 약간의 오버해드를 감수하면서 apObjTemp를 활용한다.
	CHECK1();
	//for( i = 0; i < dwObjProcessNum; i++ )
	//{
	//	m_apObjProcess[ i ]->Process( 0 );
	//}
	
	CHECK2("    prc");
	//	m_weather.Process( this );
	// 오브젝트 Delete ( DeleteObj가 호출된 오브젝트들)

	for( i = 0; i < m_nDeleteObjs; i++ )
	{
		pObj = m_apDeleteObjs[ i ];
		//#ifdef __CLIENT
		if( m_pObjFocus == pObj )
			SetObjFocus( NULL );
		if( CObj::m_pObjHighlight == pObj )
			CObj::m_pObjHighlight = NULL;
//#ifdef __NEWPROCESS
		// 화면에 출력되고 있는 오브젝트인가.
		if( pObj->m_ppViewPtr )
		{
			// 그렇다면 화면 출력 배열에서 자신을 삭제 
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}
//#endif
		//#endif
		RemoveObjLink( pObj );
		RemoveObjArray( pObj );
		SAFE_DELETE( pObj );
	}
	m_nDeleteObjs = 0;

}

// 
// GetHeight(D3DXVECTOR vecPos)
// vecPos의 해당하는 필드의 높이를 정교하게 계산해서 돌려 준다. 
// return 값은 y 좌표에 해당된다.
//
FLOAT CWorld::GetLandHeight( float x, float z )
{
	float _x = x, _z = z;

	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE )
		return 0;
	x /= MPU; 
	z /= MPU;
	float rX = (float)( x );
	float rZ = (float)( z );
	int mX = int( rX / MAP_SIZE );
	int mZ = int( rZ / MAP_SIZE );
	CLandscape* pLand = m_apLand[ mX + mZ * m_nLandWidth ];
	if( pLand == NULL ) 
		return 0;

	return pLand->GetHeight( x - ( mX * MAP_SIZE ), z - ( mZ * MAP_SIZE ) );
}

FLOAT CWorld::GetLandHeight( D3DXVECTOR3 vPos )
{
	return GetLandHeight( vPos.x, vPos.z );
}

int CWorld::GetLandTris( float x, float z, D3DXVECTOR3* pTris )
{
	int offset=0;
	if(GetLandTri2(x,		z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,		z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z+MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,		z+MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z+MPU,	pTris+offset)) offset+=6;
	return offset;
}
BOOL CWorld::GetLandTri2( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE ) return FALSE;

	x /= MPU; 
	z /= MPU;

	int lx=(int)(x/MAP_SIZE);
	int lz=(int)(z/MAP_SIZE);
	DWORD gx=(int)x-lx*MAP_SIZE;
	DWORD gz=(int)z-lz*MAP_SIZE;
	float glx=x-(int)x;
	float glz=z-(int)z;

	CLandscape* pLand = m_apLand[ lx + lz * m_nLandWidth ];
	if( pLand == NULL ) return FALSE;

	//	pLand->GetHeight();;
	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );

			pTri[3]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );
			pTri[4]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );
			pTri[5]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );

			pTri[3]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );
			pTri[4]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU) );
			pTri[5]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU) );
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));

			pTri[3]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[4]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[5]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));

			pTri[3]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[4]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[5]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
		}
	}

	return TRUE;
}

void CWorld::GetLandTri( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE ) return;

	x /= MPU; 
	z /= MPU;

	int lx=(int)(x/MAP_SIZE);
	int lz=(int)(z/MAP_SIZE);
	DWORD gx=(int)x-lx*MAP_SIZE;
	DWORD gz=(int)z-lz*MAP_SIZE;
	float glx=x-(int)x;
	float glz=z-(int)z;

	CLandscape* pLand = m_apLand[ lx + lz * m_nLandWidth ];
	if( pLand == NULL ) return;

	//	pLand->GetHeight();;
	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz  ),(FLOAT)( (gz  +lz*MAP_SIZE)*MPU));
			pTri[1]=D3DXVECTOR3((FLOAT)( (gx  +lx*MAP_SIZE)*MPU),pLand->GetHeight(gx  ,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
			pTri[2]=D3DXVECTOR3((FLOAT)( (gx+1+lx*MAP_SIZE)*MPU),pLand->GetHeight(gx+1,gz+1),(FLOAT)( (gz+1+lz*MAP_SIZE)*MPU));
		}
	}


}
#if 0
/*
FLOAT CWorld::GetFullHeight( D3DXVECTOR3 vPos, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, -1.0f, 0.0f ); 
	D3DXVECTOR3 vIntersect;
	FLOAT fDist;              

	CObj* pObj;
	int nRange = 1;
	//return GetLandHeight( vPos.x, vPos.z );
	FOR_LINKMAP( this, vPos, pObj, nRange )
	{
		if( pObj->GetType() == OT_OBJ && pObj->GetIndex() >= 15 && pObj->GetIndex() != 27 && pObj->GetIndex() != 86 && pObj->GetIndex() != 87 )
		{
			D3DXMATRIX mWorld = pObj->GetMatrixWorld();
			CModel* pModel = pObj->m_pModel;
			if( pModel->IntersectBB( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
				//return vIntersect.y;
				if( pModel->Intersect( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
					return vIntersect.y;
		}
	}
	END_LINKMAP

	return GetLandHeight( vPos.x, vPos.z );
}
*/
#endif //0
/*
int GetMeshheight( CObj* pObj )
{
	if( pObj->GetType() == OT_OBJ )
	{
		// 오브젝트의 높이를 얻는다. 
		// 구하는 방법은 캐릭터의 키 높이에서 수직으로 지형을 향해 광선을 쏘아 충돌하는 오브젝트의 좌표를 얻는 것이다.
		// 내 키를 넘는 오브젝트는 높이를 얻어내지 못한다. 이러한 오브젝트는 기본적으로 진행 자체를 할 수 없으므로
		// 다른 충돌 알고리즘으로 충돌 여부를 확인해야한다.

		D3DXVECTOR3 vPickRayOrig = vPos;// - D3DXVECTOR3( 0.0f, 2.0f, 0.0f );
		D3DXVECTOR3 vPickRayDir( 0.0f, -1.0f, 0.0f ); ;
		D3DXVECTOR3 vIntersect;
		FLOAT fDist;              
		FLOAT fNearDist = m_fFarPlane;                    

		D3DXMATRIX mWorld = pObj->GetMatrixWorld();
		CModel* pModel = pObj->m_pModel;

		if( pModel->IntersectBB( vPickRayOrig, vPickRayDir, mWorld, &vIntersect, &fDist ) == TRUE )
		{
			if( pModel->Intersect( vPickRayOrig, vPickRayDir, mWorld, &vIntersect, &fDist ) == TRUE )
				return vIntersect.y;
		}
	}
	return -1;
}
GetAround( GetPos(), 1, GetModelHeight );

void CWorld::GetAround( D3DXVECTOR3 vPos, int nRange, BOOL (*pFunc)() )
{
	float rX = vPos.x / MPU;
	float rZ = vPos.z / MPU;
	for( int i = -nRange; i <= nRange; i++ )
	{
		for( int j = -nRange; j <= nRange; j++ )
		{
			int mX = int( ( rX + ( j * MPU ) ) / MAP_SIZE );
			int mZ = int( ( rZ + ( i * MPU ) ) / MAP_SIZE );
			CLandscape* pLand = m_apLand[ mX + mZ * m_nLandWidth ];
			if( pLand ) 
			{
				int nMaxWidth = MAP_SIZE;
				int nDiv = 1;
				for( int k = 0; k < 8; k++ )
				{
					D3DXVECTOR3 vPosTemp = vPos;
					vPosTemp.x /= MPU; vPosTemp.x -= pLand->m_nWorldX;
					vPosTemp.z /= MPU; vPosTemp.z -= pLand->m_nWorldY;
					vPosTemp.x /= nDiv;
					vPosTemp.z /= nDiv;
					int nPos = ( int( vPosTemp.z ) + i ) * nMaxWidth + ( int( vPosTemp.x ) + j );
					if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
					{
						CObj** pObjs = pLand->m_aObjLinkMap2[ k ];
						CObj* pObj = pObjs[ nPos ];
						while( pObj )
						{
							if( (* pFunc->m_pFunc )( pObj ) == TRUE )
								return TRUE;
							pObj = pObj->GetNextNode();
						}
					}
					nDiv *= 2;
					nMaxWidth /= 2;
				}
			}
		}
	}
	return FALSE;
}
*/



#if !defined(__BEAST) && !defined(__ONLINE)
static REGIONELEM g_regionElem;

LPREGIONELEM CWorld::GetBeginPos( int i ) 
{ 
	LPREGIONELEM lpRegionElem;
	lpRegionElem = m_aBeginPos.GetAt( i ); 
	if( lpRegionElem ) return lpRegionElem;
	return &g_regionElem;
}
LPREGIONELEM CWorld::GetRandomBeginPos()
{
	LPREGIONELEM apRegionElem[ 100 ];
	int nSize = 0;
	for( int i = 0; i < m_aBeginPos.GetSize(); i++ )
	{
		if( m_aBeginPos.GetAt( i ) )
			apRegionElem[ nSize++ ] = m_aBeginPos.GetAt( i );
	}
	if( nSize )
		return apRegionElem[ xRandom( nSize ) ];
	g_regionElem.m_vPos.x = 50.0f;
	g_regionElem.m_vPos.y = 50.0f;
	g_regionElem.m_vPos.z = 50.0f;
	return &g_regionElem;
}
#endif

LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z )
{
	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE )
		return NULL;
	x /= MPU;
	z /= MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return 0;
	float rX = (float)( x );// - m_nWorldX;
	float rZ = (float)( z );// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);
	
	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return 0;


	int tx, tz;
	if (x >= MAP_SIZE)
		tx = x % MAP_SIZE;
	else
		tx = x;

	if (z >= MAP_SIZE)
		tz = z % MAP_SIZE;
	else
		tz = z;
	//int tx = ( x % MAP_SIZE ) / PATCH_SIZE;
	//int tz = ( z % MAP_SIZE ) / PATCH_SIZE;
	
	return m_apLand[ mX + mZ * m_nLandWidth ]->GetWaterHeight( tx, tz );
}
int CWorld::GetHeightAttribute(int x, int z )
{
	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE )
		return -1;
	x /= MPU;
	z /= MPU;
	int rX = x;
	int rZ = z;
	int mX=int( rX / MAP_SIZE);
	int mZ=int( rZ / MAP_SIZE);
	if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
		return -1;
	return m_apLand[ mX + mZ * m_nLandWidth ]->GetHeightAttribute( x-(mX*MAP_SIZE), z-(mZ*MAP_SIZE) );
}




// pvPos의 좌표를 중심으로 범위 데미지를 준다.
// ------------------------------------------
// nDmgType : 데미지 타입 ex) AF_MAGICSKILL
// pAttacker : 공격자(시전자) - 앞으로 CCtrl*로 바꿔야 한다
// nApplyType : 적용대상. ex) OBJTYPE_PLAYER | OBJTYPE_MONSTER
// nSkill : 사용한 스킬종류
// fRange : 타겟을 중심으로한 데미지 범위(미터)
// (&vPos, AF_MAGICSKILL, this, nSkill, 5.0f, 0.0, 1.0f )
void	CWorld::SendDamageAround( const D3DXVECTOR3 *pvPos, int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange )
{

} // SendDamageAround()


#ifdef __USE_STR_TBL0407
BOOL CWorld::LoadString( void )
{
	m_mapString.clear();

#ifdef _DEBUG
	//return FALSE;		//gmpbigsun
#endif

	CScript	s;
	if( s.Load( (LPCTSTR)m_strStrFile, FALSE ) == FALSE )
	{
		Error( "LoadString: file not found: %s", m_strStrFile );
		return FALSE;
	}

	s.GetToken_NoDef();
	while( s.tok != FINISHED )
	{
		CString str	= s.Token;
		if( str.Find( "IDS", 0 ) != 0 )
		{
			Error( "LoadString: %s", m_strStrFile );
			s.GetToken();
			continue;
		}
		s.GetLastFull();
		bool bResult	= m_mapString.insert( map<string, string>::value_type( (LPCTSTR)str, (LPCTSTR)s.Token ) ).second;
//		TRACE( _T( "%s, %s" ), str, s.Token );

		if( !bResult )
		{
			Error( "string error: %s", str );
		}
		else
		{
			CString strKey	= str.Right( 6 );
			int nString		= atoi( (LPCSTR)strKey ) + 1;
			if( nString > m_nString )
				m_nString	= nString;
#ifdef _DEBUG
			char lpOutputString[4096];
			if( s.Token.GetLength() < 4096 )
			{
				sprintf( lpOutputString, "string %s\t%s\n", str, s.Token );
				OutputDebugString( lpOutputString );
			}
#endif	// _DEBUG
		}
		s.GetToken_NoDef();
	}
	return TRUE;
}

BOOL CWorld::WriteString( void )
{
	CFileIO	f;
	if( f.Open( m_strStrFile, _T( "wb" ) ) == FALSE )
		return FALSE;
	f.PutC( (char)0xff );
	f.PutC( (char)0xfe );
	int j = 0;
	for( map<string, string>::iterator i = m_mapString.begin(); i != m_mapString.end(); ++i )
	{
//		TRACE( _T( "%s\t%s\n" ), i->first.data(), i->second.data() );
		f.PutWideString( i->first.data() );
		f.PutWideString( _T( "\t" ) );
		CString str	= i->second.data();
		str.Replace( "\r\n", "\\n" );
		f.PutWideString( (LPCTSTR)str );
		f.PutWideString( _T( "\r\n" ) );
	}
	return TRUE;
}
#endif	// __USE_STR_TBL0407



#ifdef __Y_PATROL

CPatrolPath::CPatrolPath()
{
	Clear();
}

CPatrolPath::~CPatrolPath()
{
}

void CPatrolPath::Clear()
{
	m_mapPatrolPath.clear();
	m_dwIndex.clear();
	m_bReturn.clear();
}

CPatrolPath*	CPatrolPath::GetInstance( void )
{
	static CPatrolPath	sMailBox;
	return &sMailBox;
}

void CPatrolPath::SetReturn( DWORD dwIndex, BOOL bReturn )
{
	map< DWORD, DWORD >::iterator it = m_dwIndex.find( dwIndex );
	
	if( it != m_dwIndex.end() )
	{
		it->second = bReturn;
	}
}

// 좌표를 추가한다.
void CPatrolPath::AddPatrolPath( DWORD dwIndex, CPathObj* vecInfo )
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );

	vector<CPathObj*> pTemp;

	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		// 처음엔 시작좌표 넣는다...
		it->second.push_back( vecInfo );
	}
	else
	// 새롭게 넣는다
	{
		pTemp.push_back( vecInfo );
		m_mapPatrolPath.insert( map< DWORD, vector<CPathObj*> >::value_type(dwIndex, pTemp) );
	}
}

// 해당 인덱스의 좌표를 얻는다.
CPathObj* CPatrolPath::GetPatrolPath( DWORD dwIndex )
{
	map< DWORD, vector<CPathObj*> >::iterator it1 = m_mapPatrolPath.find( dwIndex );
	map< DWORD, DWORD >::iterator            it2 = m_dwIndex.find( dwIndex );
	
	if( it1 == m_mapPatrolPath.end() || it2 == m_dwIndex.end() )
		return NULL;
	
	vector<CPathObj*> _vecinfo = it1->second;

	return _vecinfo[it2->second];
}

// 현재 좌표가 도착했는지 검사한다.
BOOL CPatrolPath::IsArrPos( DWORD dwIndex, D3DXVECTOR3& vPos )
{
	map< DWORD, vector<CPathObj*> >::iterator it1 = m_mapPatrolPath.find( dwIndex );
	map< DWORD, DWORD >::iterator            it2 = m_dwIndex.find( dwIndex );
	
	if( it1 == m_mapPatrolPath.end() || it2 == m_dwIndex.end() )
		return FALSE;
	
	vector<CPathObj*> _vecinfo = it1->second;
	
	_vecinfo[it2->second]->GetPos();

	D3DXVECTOR3 v3Length;
	D3DXVec3Subtract( &v3Length, &vPos, &(_vecinfo[it2->second]->GetPos()) );

	FLOAT fLength = D3DXVec3Length( &v3Length );

	if( fLength < 0.5f )
		return TRUE;
	
	return FALSE;
}

// 다음 
void CPatrolPath::GetNextPos( DWORD dwIndex, D3DXVECTOR3& vPos )
{
	map< DWORD, vector<CPathObj*> >::iterator it1 = m_mapPatrolPath.find( dwIndex );
	map< DWORD, DWORD >::iterator            it2 = m_dwIndex.find( dwIndex );
	
	if( it1 == m_mapPatrolPath.end() || it2 == m_dwIndex.end() )
		return;

	it2->second++;

	{
		if( it2->second >= it1->second.size() )
		{
			it2->second = 0;
		}
	}

	vector<CPathObj*> _vecinfo = it1->second;
	
	vPos = _vecinfo[it2->second]->GetPos();
}

void CPatrolPath::AddPatrolIndex(DWORD dwIndex)
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		return;
	}
	else
	// 새롭게 넣는다
	{
		vector<CPathObj*> pTemp;
		pTemp.clear();
		m_mapPatrolPath.insert( map< DWORD, vector<CPathObj*> >::value_type(dwIndex, pTemp) );

		int ncount = m_mapPatrolPath.size();
	}	
}

BOOL CPatrolPath::IsFirstPath( DWORD dwIndex )
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		if( it->second.size() == 0 )
			return TRUE;
	}

	return FALSE;
}

void CPatrolPath::DeletePatrolIndex(DWORD dwIndex)
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		m_mapPatrolPath.erase( it );
	}
}

BOOL CPatrolPath::DeletePatrolPath( CPathObj* pPathObj )
{
	for( map< DWORD, vector<CPathObj*> >::iterator it1 = m_mapPatrolPath.begin(); it1 != m_mapPatrolPath.end() ; ++it1 )
	{
		vector<CPathObj*>::iterator i;
		for( i = it1->second.begin(); i != it1->second.end(); ++i )
		{
			if( (*i) == pPathObj )
			{
				it1->second.erase( i-- );
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CPatrolPath::UpdatePatrolData(DWORD dwIndex)
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		vector<CPathObj*> _vecInfo = it->second;
		
		for( int i=1; i< (int)( _vecInfo.size() ); i++ )
		{
			int nIndex = i - 1;
			
			D3DXVECTOR3 v3Pos1 = _vecInfo[nIndex]->GetPos();
			D3DXVECTOR3 v3Pos2 = _vecInfo[i]->GetPos();
			D3DXVECTOR3 v3Result;
			
			D3DXVec3Subtract( &v3Result, &v3Pos2, &v3Pos1 );
			D3DXVec3Normalize( &_vecInfo[i]->m_vDir, &v3Result );		
			_vecInfo[i]->m_fLength = D3DXVec3Length( &v3Result );			
		}
	}
}

BOOL CPatrolPath::Save( LPCTSTR szFileName )
{
	FILE *fp = fopen( szFileName, _T( "wt" ) );

	if(fp == NULL) 
		return FALSE;

	if( m_mapPatrolPath.size() == 0 )
		return FALSE;

	for( map< DWORD, vector<CPathObj*> >::iterator it1 = m_mapPatrolPath.begin(); it1 != m_mapPatrolPath.end() ; ++it1 )
	{
		_ftprintf(fp, _T( "INDEX %d\n" ), it1->first );
		_ftprintf(fp, _T( "{\n" ) );
		
		vector<CPathObj*> _vecInfo = it1->second;

		for( int i=0; i < (int)( _vecInfo.size() ); i++ )
		{
			_ftprintf(fp, _T( "\t %4d    %f %f %f\n" ), i, _vecInfo[i]->GetPos().x, _vecInfo[i]->GetPos().y, _vecInfo[i]->GetPos().z );
			
			if( i != 0 )
			{
				int nIndex = i - 1;
			
				D3DXVECTOR3 v3Pos1 = _vecInfo[nIndex]->GetPos();
				D3DXVECTOR3 v3Pos2 = _vecInfo[i]->GetPos();
				D3DXVECTOR3 v3Result;
				
				D3DXVec3Subtract( &v3Result, &v3Pos2, &v3Pos1 );
				D3DXVec3Normalize( &_vecInfo[i]->m_vDir, &v3Result );		
				_vecInfo[i]->m_fLength = D3DXVec3Length( &v3Result );

				_ftprintf(fp, _T( "\t\t %f %f %f %d %d %f\n" ), _vecInfo[i]->m_vDir.x, _vecInfo[i]->m_vDir.y, _vecInfo[i]->m_vDir.z, _vecInfo[i]->m_dwMotionState, _vecInfo[i]->m_dwDelay, _vecInfo[i]->m_fLength );
			}
/*
			if( i == _vecInfo.size()-1 )
			{
				_ftprintf(fp, _T( "\t %4d    %f %f %f\n" ), i+1, _vecInfo[0]->GetPos().x, _vecInfo[0]->GetPos().y, _vecInfo[0]->GetPos().z );
				
				if( i != 0 )
				{
					int nIndex = i - 1;
					
					D3DXVECTOR3 v3Pos1 = _vecInfo[i]->GetPos();
					D3DXVECTOR3 v3Pos2 = _vecInfo[0]->GetPos();
					D3DXVECTOR3 v3Result;
					D3DXVECTOR3 v3DirTemp;
					FLOAT fLength;
					
					D3DXVec3Subtract( &v3Result, &v3Pos2, &v3Pos1 );
					D3DXVec3Normalize( &v3DirTemp, &v3Result );		
					fLength = D3DXVec3Length( &v3Result );
					
					_ftprintf(fp, _T( "\t\t %f %f %f %d %d %f\n" ), v3DirTemp.x, v3DirTemp.y, v3DirTemp.z, _vecInfo[0]->m_dwMotionState, _vecInfo[0]->m_dwDelay, fLength );
				}
			}
*/			
		}

		_ftprintf(fp, _T( "}\n" ) );		
		_ftprintf(fp, _T( "\n" ) );		
	}

	fclose(fp);	

	return TRUE;
}

BOOL CPatrolPath::Load( LPCTSTR szFileName )
{
#ifdef __BEAST
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if( pBeastView == NULL )
		return FALSE;
#endif //__BEAST
	
	CScript s;
	if( s.Load( szFileName, FALSE ) )
	{
		Clear();

		DWORD nIndex = 0;
		D3DXVECTOR3 v3Pos;

		s.GetToken();

		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "INDEX" ) )
			{
				nIndex = s.GetNumber();
				AddPatrolIndex( nIndex );
			}

			s.GetToken();

			int nCount = s.GetNumber();

			while( s.Token != _T( "}" ) )
			{
				v3Pos.x = s.GetFloat();
				v3Pos.y = s.GetFloat();
				v3Pos.z = s.GetFloat();

				CObj* pObj = CreateObj( g_2DRender.m_pd3dDevice, 8, 12 );
				pObj->SetPos( v3Pos );
				pObj->SetAngle( 0 );
				pObj->UpdateLocalMatrix();
		#ifdef __BEAST
				pBeastView->m_world.AddObj( pObj );
				pBeastView->m_aPickCollection.RemoveAll();
				pBeastView->m_aPickCollection.Add( pObj );
		#endif //__BEAST
				
				if( nCount != 0 )
				{
					((CPathObj*)pObj)->m_vDir.x = s.GetFloat();
					((CPathObj*)pObj)->m_vDir.y = s.GetFloat();
					((CPathObj*)pObj)->m_vDir.z = s.GetFloat();
					((CPathObj*)pObj)->m_dwMotionState = s.GetNumber();
					((CPathObj*)pObj)->m_dwDelay = s.GetNumber();
					((CPathObj*)pObj)->m_fLength = s.GetFloat();					
				}				

				AddPatrolPath( nIndex, (CPathObj*)pObj );
				nCount = s.GetNumber();
			}
			
			s.GetToken();			
		}
	}	
	else
	{
		return FALSE;
	}

	return TRUE;	
}

void CPatrolPath::RenderPath(DWORD dwIndex, CWorld* pWorld )
{
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		D3DXMATRIX matWorld;
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vOut;
		D3DVIEWPORT9 vp;
		CPoint point1;
		CPoint point2;
		CRect rect;
		D3DXVECTOR3 vPosOld;
		
		g_2DRender.m_pd3dDevice->GetViewport( &vp );
		
		vector<CPathObj*> _vecInfo = it->second;

		for( int i=0; i < (int)( _vecInfo.size() ); i++ )
		{
			vPos = _vecInfo[i]->GetPos();
			
			D3DXMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
			
			D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &pWorld->m_matProj,
				&pWorld->m_pCamera->m_matView, &matWorld );
			
			rect.SetRect( (int)( vOut.x - 1 ), (int)( vOut.y - 1 ), (int)( vOut.x + 2 ), (int)( vOut.y + 2 ));
			g_2DRender.RenderFillRect( rect, 0xffff00ff );
			g_2DRender.TextOut( (int)( vOut.x ), (int)( vOut.y ), i, 0xffff00ff );

			if( i>=1 )
			{
				point1 = CPoint( (int)( vPosOld.x ), (int)( vPosOld.y ) );
				point2 = CPoint( (int)( vOut.x ), (int)( vOut.y ) );
				
				g_2DRender.RenderLine( point1, point2, 0xffffff00 );
			}

			vPosOld = vOut;
			
		}
	}
}

void CPatrolPath::RenderPath(CObj *pObj, CWorld *pWorld)
{
	DWORD dwIndex;
	CRespawnInfo* pRespawnInfo = NULL;

	if( pObj == NULL )
		return;
	
	if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CMover* pMover = (CMover*) pObj; pRespawnInfo = &pMover->m_respawnInfo;
	}
	if( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CItem* pItem = (CItem*) pObj; pRespawnInfo = &pItem->m_respawnInfo;
	}
	if( pObj->GetType() == OT_CTRL && ((CCommonCtrl*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CCommonCtrl* pCtrl = (CCommonCtrl*) pObj; pRespawnInfo = &pCtrl->m_respawnInfo;
	}
	
	if( pRespawnInfo )
	{
		dwIndex = pRespawnInfo->m_dwPatrolIndex;
	}	
	else
		return;
	
	map< DWORD, vector<CPathObj*> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		D3DXMATRIX matWorld;
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vOut;
		D3DVIEWPORT9 vp;
		CPoint point1;
		CPoint point2;
		CRect rect;
		D3DXVECTOR3 vPosOld;
		
		D3DXVECTOR3 v3Dir;
		D3DXVECTOR3 vPosBack;
		FLOAT		fLength;
		
		g_2DRender.m_pd3dDevice->GetViewport( &vp );
		
		vector<CPathObj*> _vecInfo = it->second;
		
		for( int i=0; i < (int)( _vecInfo.size() ); i++ )
		{
			if( i == 0 )
			{
				vPos = vPosBack = pObj->GetPos();
			}

			if( i != 0 )
			{
				v3Dir = _vecInfo[i]->m_vDir;
				fLength = _vecInfo[i]->m_fLength;

				vPos = vPosBack + (v3Dir * fLength);	
				
				vPosBack = vPos;
			}

			D3DXMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
			
			D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &pWorld->m_matProj,
				&pWorld->m_pCamera->m_matView, &matWorld );
			
			rect.SetRect( (int)( vOut.x - 1 ), (int)( vOut.y - 1 ), (int)( vOut.x + 2 ), (int)( vOut.y + 2 )); 
			g_2DRender.RenderFillRect( rect, 0xffff00ff );
			g_2DRender.TextOut( (int)( vOut.x ), (int)( vOut.y ), i, 0xffff00ff );

			if( i>=1 )
			{
				point1 = CPoint( (int)( vPosOld.x ), (int)( vPosOld.y ) );
				point2 = CPoint( (int)( vOut.x ), (int)( vOut.y ) );
				
				g_2DRender.RenderLine( point1, point2, D3DCOLOR_ARGB( 100, 200, 255, 0 ) );
			}
				
			vPosOld = vOut;
		}
	}	
}


#endif //__Y_PATROL

void CWorld::StartTEV( )
{
	m_dwOldTime = timeGetTime( );
	m_bIsWorkingEV = TRUE;

	//!-_- Invalidate( TRUE )를 호출해줘야 다음 Invalidate(FALSE)를 호출할때 랜더가 된다..;;
	g_pWorldView->Invalidate( TRUE );

	g_kDlgContinent.InContinent();
}

float LerpMul( const float oldVal_, const float newVal_, const float dt_ )
{
	return ( oldVal_ * ( 1.0f - dt_ ) ) + newVal_ * dt_;
}

inline float LerpValueToValue( const float oldVal_, const float newVal_, const float dt_ )
{
	return( oldVal_ - ( oldVal_ - newVal_ ) * dt_ );
}

void CWorld::UpdateLerpEV( CLight* pLight )
{
	// time = 0.0 ~ 1.0

	// ambient
	// diffuse
	// sky
	// fog
	ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
	if( !pInfo )
		return;

	if( !m_bIsWorkingEV )
	{
		pLight->Ambient.r = pInfo->_fMulAmbient[ 0 ];
		pLight->Ambient.g = pInfo->_fMulAmbient[ 1 ];
		pLight->Ambient.b = pInfo->_fMulAmbient[ 2 ];

		pLight->Diffuse.r = pInfo->_fMulDiffuse[ 0 ];
		pLight->Diffuse.g = pInfo->_fMulDiffuse[ 1 ];
		pLight->Diffuse.b = pInfo->_fMulDiffuse[ 2 ];
		
		return;
	}

	DWORD dwCurTime = timeGetTime( );

	DWORD dwDeltaTime = dwCurTime - m_dwOldTime;
	m_dwOldTime = dwCurTime;
	m_dwAddedTime += dwDeltaTime;
	
	static const float fCON = 2.0f;				//몇초 동안?
	static const float fCON_RATE = 1.0f / fCON;
	if( m_dwAddedTime > ( 1000 * fCON ) )
		m_dwAddedTime = (DWORD)( 1000 * fCON ) ;

	float fTime = m_dwAddedTime * 0.001f * fCON_RATE;
	
	// set light, fog, sky here!!!

	float val = 0.0f;
	val = ( (m_dwAmbient>>16) & 0xff ) / 255.0f;
	pLight->Ambient.r = LerpValueToValue( val, pInfo->_fMulAmbient[ 0 ], fTime );

	val = ( (m_dwAmbient>>8) & 0xff ) / 255.0f;
	pLight->Ambient.g = LerpValueToValue( val, pInfo->_fMulAmbient[ 1 ], fTime );

	val = ( m_dwAmbient & 0xff ) / 255.0f;
	pLight->Ambient.b = LerpValueToValue( val, pInfo->_fMulAmbient[ 2 ], fTime );

	val = ( (m_dwDiffuse>>16) & 0xff ) / 255.0f;
	pLight->Diffuse.r = LerpValueToValue( val, pInfo->_fMulDiffuse[ 0 ], fTime );

	val = ( (m_dwDiffuse>>8) & 0xff ) / 255.0f;
	pLight->Diffuse.g = LerpValueToValue( val, pInfo->_fMulDiffuse[ 1 ], fTime );

	val = ( m_dwDiffuse & 0xff ) / 255.0f;
	pLight->Diffuse.b = LerpValueToValue( val, pInfo->_fMulDiffuse[ 2 ], fTime );
	
	g_pWorldView->Invalidate( FALSE );
		
	if( m_dwAddedTime >= 1000 * fCON )
	{
		//ok finished the working
		m_bIsWorkingEV = FALSE;
		m_dwAddedTime = 0;
	}

}