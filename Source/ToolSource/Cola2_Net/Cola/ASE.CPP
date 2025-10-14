#include "stdafx.h"

#include <stdio.h>
#include "ase.h"
#include "scanner.h"

// GEOMOBJECT를 만나면 Read_GEOMOBJECT()로 들어가서.
// 이름이 Bip로 시작하면 CBone노드를 생성하고
// 그렇지 않다면 CASEMesh를 생성.
// 일반 계층적 키프레임 애니메이션을 위해서 CASEMesh도 트리구조가 되어야 한다
// 

float	g_fScale = 0.1f;

//extern char	g_strError[];
// Our custom FVF, which describes our custom vertex structure

int			g_nMaxID = 0;


//////////////////////////////////////////////////////////////////////////////
CAse :: CAse()
{
	m_pd3dDevice = NULL;
	Init();
}

CAse :: ~CAse()
{
	Destroy();
}
			


								
void	CAse :: Init( void )
{

	m_pGroup = &m_Group[0];		// 0번 그룹이 디폴트가 된다.
	m_nGroupCount = 0;
	memset( m_Group, 0, sizeof(GROUP) * MAX_GROUP );
	m_pCollObject = NULL;
	m_pPath = NULL;
	m_pAttr = NULL;
	m_nMaxMaterial = 0;
	m_pMaterialList = NULL;
	memset( m_szFullPath, 0, sizeof(m_szFullPath) );

//	m_pTempTop = NULL;
	m_nFramePerSample = 1;
	m_nTicksPerFrame = 160;
//	m_nFrameMax = 0;	// (57 / m_nFramePerSample) + 1;
//	m_nCurrFrame = 0;
//	m_fFrameCurrent = 0;
//	m_bEndFrame = FALSE;
	m_nCount = 0;
//	m_pSelectGeo = NULL;
//	m_dKps = (double)1 / 30;
//	m_lnPrev = m_lnCurr = 0;
	QueryPerformanceFrequency( &m_lnFreq );
//	m_pd3dDevice = NULL;
	m_bBiped = FALSE;
	m_bSendVS = FALSE;
	m_bScale = FALSE;
	m_bLOD = FALSE;
//	m_fScrlU = m_fScrlV = 0.0f;
	m_vMin.x = m_vMin.y = m_vMin.z = 65535.0;
	m_vMax.x = m_vMax.y = m_vMax.z = -65535.0;
	m_nMaxEvent = 0;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	memset( m_pEvent, 0, sizeof(m_pEvent) );

	D3DXMatrixIdentity( &m_mLocalRH );
	D3DXMatrixIdentity( &m_mLocalLH );
	D3DXMatrixIdentity( &m_mLocalShield );
	D3DXMatrixIdentity( &m_mLocalKnuckle );
	
	m_nMaxBoneNode = 0;
}

void	CAse :: Destroy( void )
{
	int		i, j;

	DeleteDeviceObjects();

	for( i = 0; i < MAX_GROUP; i ++ )
		for( j = 0; j < MAX_GEOMOBJECT; j ++ )
			SAFE_DELETE( m_Group[i].m_pGeoMObject[j] );
//	for( i = 0; i < MAX_GEOMOBJECT; i ++ )
//		SAFE_DELETE( m_pGeoMObject[i] );
//	for( i = 0; i < MAX_GEOMOBJECT; i ++ )
//		SAFE_DELETE( m_pObjectLOD2[i] );
//	for( i = 0; i < MAX_GEOMOBJECT; i ++ )
//		SAFE_DELETE( m_pObjectLOD3[i] );
	
	SAFE_DELETE( m_pCollObject );
	SAFE_DELETE( m_pPath );
	SAFE_DELETE_ARRAY( m_pAttr );
	for( i = 0; i < m_nMaxMaterial; i ++ )
		SAFE_RELEASE( m_pMaterialList[i].m_pTexture );
	
	SAFE_DELETE_ARRAY( m_pMaterialList );


	Init();
}

// m_nMaxGeoMObject를 읽으면 메시가 없는 뼈대도 오브젝트로 치기 땜에 메시를 포함한것만 계산.
int		CAse :: GetMaxObject( void )
{
	int		i, nMax = 0;
	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;
	
	for( i = 0; i < nMaxGeoMObject; i ++ )
	{
		if( pGeoMObject[i]->m_pMesh )
			nMax ++;
	}
	return nMax;
}

int		CAse :: GetMaxVertex( void )
{
	int		i, nMax = 0;
	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;

	for( i = 0; i < nMaxGeoMObject; i ++ )
	{
		if( pGeoMObject[i]->m_pMesh )
			nMax += pGeoMObject[i]->m_pMesh->m_nMaxVertexList;
	}
	return nMax;
}

int		CAse :: GetMaxFace( void )
{
	int		i, nMax = 0;
	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;

	for( i = 0; i < nMaxGeoMObject; i ++ )
	{
		if( pGeoMObject[i]->m_pMesh )
			nMax += pGeoMObject[i]->m_pMesh->m_nMaxFaceList;
	}
	return nMax;
}

int		CAse :: GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;

	for( i = 0; i < nMaxGeoMObject; i ++ )
	{
		if( pGeoMObject[i]->m_pMesh )
			nMax += pGeoMObject[i]->m_pMesh->m_nMaxVertexBufferBlock;
	}
	return nMax;
}

int		CAse :: GetMaxBone( void )
{
	CGeoMObject *pBiped = GetBiped();
	if( pBiped == NULL )	return 0;

//	int nNum = pBiped->GetLength();
	return m_nMaxBoneNode;
}


HRESULT CAse::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i, j;
//	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
//	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;
	
	m_pd3dDevice = pd3dDevice;

	for( j = 0; j < MAX_GROUP; j ++ )
		for( i = 0; i < m_Group[j].m_nMaxGeoMObject; i ++ )
			m_Group[j].m_pGeoMObject[i]->InitDeviceObjects( pd3dDevice );

	if( m_pCollObject )
		m_pCollObject->InitDeviceObjects( pd3dDevice );
		

	return S_OK;
}

HRESULT CAse::RestoreDeviceObjects( void )
{
	int		i, j;
//	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
//	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;
	
	for( j = 0; j < MAX_GROUP; j ++ )
		for( i = 0; i < m_Group[j].m_nMaxGeoMObject; i ++ )
			m_Group[j].m_pGeoMObject[i]->RestoreDeviceObjects();

	if( m_pCollObject )
		m_pCollObject->RestoreDeviceObjects();

	return S_OK;
}

HRESULT CAse::InvalidateDeviceObjects( void )
{
	return DeleteDeviceObjects();
}

// 디바이스오브젝트만을 삭제.0
HRESULT CAse::DeleteDeviceObjects( void )
{
	int		i, j;
//	int		nMaxGeoMObject = m_pGroup->m_nMaxGeoMObject;
//	CGeoMObject **pGeoMObject = m_pGroup->m_pGeoMObject;

	for( j = 0; j < MAX_GROUP; j ++ )
		for( i = 0; i < m_Group[j].m_nMaxGeoMObject; i ++ )
			m_Group[j].m_pGeoMObject[i]->DeleteDeviceObjects();

	if( m_pCollObject )
		m_pCollObject->DeleteDeviceObjects();
		
	return S_OK;
}


/*
VOID Cleanup()
{
	if( g_pVB != NULL )
		g_pVB->Release();
	if( g_pVertexList )
		delete []g_pVertexList;

	GeoMObject_TreeRelease( g_pBone );

	if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	if( g_pD3D != NULL )
        g_pD3D->Release();
*/





//////////////////////////////////////////////////////////////////////////////////////////
// *SUBMATERIAL ? {}   읽음.
int		Read_SUBMATERIAL( CScanner *scan, SUB_MATERIAL *pSub )
{
	int		nNumBlock = 0;

	pSub->m_pTexture = NULL;
	pSub->m_strBitmapFileName[0] = 0;
	pSub->m_dwEffect = 0;
	pSub->m_fTilingU = pSub->m_fTilingV = 1.0f;
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MAP_DIFFUSE" ) == 0 )
		{
			scan->GetToken();		// {
			while(1)
			{
				scan->GetToken();
				if( scan->token[0] == '}' )		break;
				if( strcmp( scan->token, "BITMAP" ) == 0 )			// 텍스쳐 비트맵 파일명 읽음
				{
					scan->GetToken();					// 파일명 읽음
					strcpy( pSub->m_strBitmapFileName, scan->token );		// 완성된 파일명을 기록
				} else
				if( strcmp( scan->token, "UVW_U_TILING" ) == 0 )
				{
					pSub->m_fTilingU = scan->GetFloat();
				} else
				if( strcmp( scan->token, "UVW_V_TILING" ) == 0 )
				{
					pSub->m_fTilingV = scan->GetFloat();
				}
			}
		}
		if( strcmp( scan->token, "MAP_REFLECT" ) == 0 )
		{
			pSub->m_dwEffect |= XE_REFLECT;
		}
		if( strcmp( scan->token, "MAP_OPACITY" ) == 0 )
		{
			pSub->m_dwEffect |= XE_OPACITY;		// Opacity맵이 있으면 있다는 플랙만 저장한다.
			scan->GetToken();		// {
			while(1)
			{
				scan->GetToken();
				if( scan->token[0] == '}' )		break;

				if( strcmp( scan->token, "MAP_AMOUNT" ) == 0 )			// 텍스쳐 비트맵 파일명 읽음
					pSub->m_nAmount = (int)(scan->GetFloat() * 255.0f);
			}
		}
		if( strcmp( scan->token, "MAP_SELFILLUM" ) == 0 )
		{
			pSub->m_dwEffect |= XE_SELF_ILLUMINATE;
		}
		if( strcmp( scan->token, "MAP_SPECULAR" ) == 0 )
		{
			pSub->m_dwEffect |= XE_SPECULAR;
		}
		if( strcmp( scan->token, "MAP_BUMP" ) == 0 )
		{
			pSub->m_dwEffect |= XE_BUMP;
		}
		
		
		if( strcmp( scan->token, "MATERIAL_TWOSIDED" ) == 0 )
		{
			pSub->m_dwEffect |= XE_2SIDE;
		}


		if( strcmp( scan->token, "MATERIAL_AMBIENT" ) == 0 )
		{
			pSub->m_Material.Ambient.r = scan->GetFloat();
			pSub->m_Material.Ambient.g = scan->GetFloat();
			pSub->m_Material.Ambient.b = scan->GetFloat();
			pSub->m_Material.Ambient.a = 0.0;
		}
		if( strcmp( scan->token, "MATERIAL_DIFFUSE" ) == 0 )
		{
			pSub->m_Material.Diffuse.r = scan->GetFloat();
			pSub->m_Material.Diffuse.g = scan->GetFloat();
			pSub->m_Material.Diffuse.b = scan->GetFloat();
			pSub->m_Material.Diffuse.a = 0.0;
		}
		if( strcmp( scan->token, "MATERIAL_SPECULAR" ) == 0 )
		{
			pSub->m_Material.Specular.r = scan->GetFloat();
			pSub->m_Material.Specular.g = scan->GetFloat();
			pSub->m_Material.Specular.b = scan->GetFloat();
			pSub->m_Material.Specular.a = 0.0;
		}
	}
	pSub->m_Material.Emissive.r = 0.0;
	pSub->m_Material.Emissive.g = 0.0;
	pSub->m_Material.Emissive.b = 0.0;
	pSub->m_Material.Emissive.a = 0.0;

	pSub->m_Material.Power = 0.0;
	return 1;
}


// *MATERIAL ? { }		매터리얼 정보 읽음
int		Read_MATERIAL( CScanner *scan, ASE_MATERIAL *pMaterial )
{
	int		nNumBlock = 0;
	int		nNumSubMtrl;

	pMaterial->m_pTexture = NULL;
	pMaterial->m_pSubMaterial = NULL;
	pMaterial->m_nMaxSubMaterial = 0;
	pMaterial->m_strBitmapFileName[0] = 0;
	pMaterial->m_fTilingU = pMaterial->m_fTilingV = 1.0f;	// 디폴트.
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MAP_DIFFUSE" ) == 0 )
		{
			scan->GetToken();		// {
			while(1)
			{
				scan->GetToken();
				if( scan->token[0] == '}' )		break;

				if( strcmp( scan->token, "BITMAP" ) == 0 )			// 텍스쳐 비트맵 파일명 읽음
				{
					scan->GetToken();					// 파일명 읽음
					strcpy( pMaterial->m_strBitmapFileName, scan->token );		// 풀패스를 그대로 읽음
				} else
				if( strcmp( scan->token, "UVW_U_TILING" ) == 0 )
				{
					pMaterial->m_fTilingU = scan->GetFloat();
				} else
				if( strcmp( scan->token, "UVW_V_TILING" ) == 0 )
				{
					pMaterial->m_fTilingV = scan->GetFloat();
				}
			}
		}
		if( strcmp( scan->token, "MAP_REFLECT" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_REFLECT;
		}
		if( strcmp( scan->token, "MAP_OPACITY" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_OPACITY;		// Opacity맵이 있으면 있다는 플랙만 저장한다.
			scan->GetToken();		// {
			while(1)
			{
				scan->GetToken();
				if( scan->token[0] == '}' )		break;

				if( strcmp( scan->token, "MAP_AMOUNT" ) == 0 )			// 텍스쳐 비트맵 파일명 읽음
					pMaterial->m_nAmount = (int)(scan->GetFloat() * 255.0f);
			}
		}
		if( strcmp( scan->token, "MAP_SELFILLUM" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_SELF_ILLUMINATE;
		}
		if( strcmp( scan->token, "MAP_SPECULAR" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_SPECULAR;
		}
		if( strcmp( scan->token, "MAP_BUMP" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_BUMP;
		}
		
		if( strcmp( scan->token, "MATERIAL_TWOSIDED" ) == 0 )
		{
			pMaterial->m_dwEffect |= XE_2SIDE;
		}

		if( strcmp( scan->token, "MATERIAL_AMBIENT" ) == 0 )
		{
//			pMaterial->m_Material.Ambient.r = scan->GetFloat();
//			pMaterial->m_Material.Ambient.g = scan->GetFloat();
//			pMaterial->m_Material.Ambient.b = scan->GetFloat();
			pMaterial->m_Material.Ambient.r = 1.0f;
			pMaterial->m_Material.Ambient.g = 1.0f;
			pMaterial->m_Material.Ambient.b = 1.0f;
			pMaterial->m_Material.Ambient.a = 0.0;
		}
		if( strcmp( scan->token, "MATERIAL_DIFFUSE" ) == 0 )
		{
			pMaterial->m_Material.Diffuse.r = 1.0f;
			pMaterial->m_Material.Diffuse.g = 1.0f;
			pMaterial->m_Material.Diffuse.b = 1.0f;
//			pMaterial->m_Material.Diffuse.r = scan->GetFloat();
//			pMaterial->m_Material.Diffuse.g = scan->GetFloat();
//			pMaterial->m_Material.Diffuse.b = scan->GetFloat();
			pMaterial->m_Material.Diffuse.a = 0.0;
		}
		if( strcmp( scan->token, "MATERIAL_SPECULAR" ) == 0 )
		{
			pMaterial->m_Material.Specular.r = scan->GetFloat();
			pMaterial->m_Material.Specular.g = scan->GetFloat();
			pMaterial->m_Material.Specular.b = scan->GetFloat();
			pMaterial->m_Material.Specular.a = 0.0;
		}
		// 서브 매트리얼 갯수
		if( strcmp( scan->token, "NUMSUBMTLS" ) == 0 )
		{
			pMaterial->m_nMaxSubMaterial = scan->GetNumber();
			if( pMaterial->m_nMaxSubMaterial > 16 )
				Message( "경고:서브 매터리얼이 무쟈게 많군요. 정상인가요? %d", pMaterial->m_nMaxSubMaterial );
			pMaterial->m_pSubMaterial = new SUB_MATERIAL[ pMaterial->m_nMaxSubMaterial ];
			memset( pMaterial->m_pSubMaterial, 0, sizeof(SUB_MATERIAL) * pMaterial->m_nMaxSubMaterial );	// zero clear
		}
		if ( strcmp( scan->token, "SUBMATERIAL" ) == 0 )
		{
			nNumSubMtrl = scan->GetNumber();		// 서브 매터리얼 번호
			Read_SUBMATERIAL( scan, &pMaterial->m_pSubMaterial[ nNumSubMtrl ] );
		}
	}
	pMaterial->m_Material.Emissive.r = 0.0;
	pMaterial->m_Material.Emissive.g = 0.0;
	pMaterial->m_Material.Emissive.b = 0.0;
	pMaterial->m_Material.Emissive.a = 0.0;

	pMaterial->m_Material.Power = 0.0;

	return 1;
}

// *MATERIAL_LIST { }		매터리얼 정보 읽음
int		CAse :: Read_MATERIAL_LIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nNumMaterial = 0;
	int		nMaterial = 0;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MATERIAL_COUNT" ) == 0 )
		{
			nNumMaterial = scan->GetNumber();		// 매터리얼 갯수 읽음
			if( nNumMaterial )
			{
				m_nMaxMaterial = nNumMaterial;
				m_pMaterialList = new ASE_MATERIAL[ nNumMaterial ];
				memset( m_pMaterialList, 0, sizeof( ASE_MATERIAL ) * nNumMaterial );
//				memset( &m_pMaterialList->m_Material, 0, sizeof( D3DMATERIAL9 ) );
			}
		}
		if( strcmp( scan->token, "MATERIAL" ) == 0 )
		{
			nMaterial = scan->GetNumber();		// Material Index
			Read_MATERIAL( scan, &m_pMaterialList[ nMaterial ] );
		}
	}

	return 1;
}

int		CAse :: Read_SCENE( CScanner *scan )
{
	int		nNumBlock = 0;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "SCENE_FIRSTFRAME" ) == 0 )
		{
			int		nNum = scan->GetNumber();
			if( nNum < 0 )	
				Error( "첫번째 프레임이 %d이다.", nNum );
		}
		if( strcmp( scan->token, "SCENE_LASTFRAME" ) == 0 )
		{
			m_nFrameMax = (scan->GetNumber() / m_nFramePerSample) + 1;
		}
		if( strcmp( scan->token, "SCENE_FRAMESPEED" ) == 0 )
		{
			m_nFrameSpeed = scan->GetNumber();
			m_fPerSlerp = (float)1 / m_nFrameSpeed;
		}
		if( strcmp( scan->token, "SCENE_TICKSPERFRAME" ) == 0 )
		{
			m_nTicksPerFrame = scan->GetNumber();
		}
		
	}

	return 1;
}


// *SKIN_DATA {}
void	CAse :: Read_SKIN_DATA( CScanner *scan )
{
	int		nNumBlock = 0;
	char	szName[256];
#ifdef __MOD_LOD

	CGeoMObject *pGeo[3];
	
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "NODE_NAME" ) == 0 )
		{
			scan->GetToken();
			strcpy( szName, scan->token );

			// 세 그룹에서 다 찾아둔다 
			for(int nCount = 0; nCount < MAX_GROUP; ++nCount)
			{
				pGeo[nCount] = FindGeoM( GetGroup(nCount), szName );		
			}
		}
		if( strcmp( scan->token, "SKIN_VERTEX_DATA" ) == 0 )
		{
			// *SKIN_VERTEX_DATA   1151 {  <- 버텍스 개수를 파싱한다
			int		nNumVert = scan->GetNumber();		

			for(int nCount = 0; nCount < MAX_GROUP; ++nCount)
			{
				if( pGeo[nCount] )
				{
					// 아... 숫자 자체도 여기서 읽으면 안될거같으네.. 리드스킨 밖에서 읽도록 바꿀까나
					if( pGeo[nCount]->m_pMesh )
						pGeo[nCount]->m_pMesh->Read_SKIN(scan, nNumVert);
					else
						Message( "경고 : %s 메쉬가 없는 오브젝트인데 스킨데이타가 있다", pGeo[nCount]->m_strName );
				} else
					// 해당 그룹에 메쉬가 있는데도 없다고 나올경우 
					if(m_Group[nCount].m_nMaxGeoMObject > 0) Error( "%s pGeo == NULL", scan->token );
			}
		}
	}

#else

	CGeoMObject *pGeo = NULL;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "NODE_NAME" ) == 0 )
		{
			scan->GetToken();
			strcpy( szName, scan->token );
			pGeo = FindGeoM( m_pGroup, szName );		// NODE_NAME "Dragon"과 같은 GEOMOBJECT를 찾음 - 현재그룹에서...
		}
		if( strcmp( scan->token, "SKIN_VERTEX_DATA" ) == 0 )
		{
			if( pGeo )
			{
				if( pGeo->m_pMesh )
					pGeo->m_pMesh->Read_SKIN( scan );
				else
					Message( "경고 : %s 메쉬가 없는 오브젝트인데 스킨데이타가 있다", pGeo->m_strName );
			} else
				Error( "%s pGeo == NULL", scan->token );
		}
	}
#endif
}

//
//
//
void	CAse :: ProcessSetType( GROUP *pGroup )
{
	int		i;
	CGeoMObject *pGeoMObject;
	CASEMesh	*pMesh;
	
	for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
	{
		pGeoMObject = pGroup->m_pGeoMObject[i];
		pMesh = pGeoMObject->m_pMesh;
		
		if( pMesh )
		{
			if( pMesh->m_pPhysiqueVertex )	// 피지크를 가진 메쉬
				pGeoMObject->SetType( GMT_SKIN );		// 피지크가 있는건 스킨메쉬 타입
			else	// 피지크가 없는 메쉬
				pGeoMObject->SetType( GMT_NORMAL );	// 피지크가 없는건 일반오브젝트.
		} else
		{
			// 메쉬가 없는것중에 Bip나 Bone으로 시작하는건 뼈대.
			if( _strnicmp( pGeoMObject->m_strName, "bip", 3 ) == 0 || _strnicmp( pGeoMObject->m_strName, "bone", 4 ) == 0 )		// Biped가 하나라도 있는지 체크
			{
				m_bBiped = TRUE;
				pGeoMObject->SetType_Recu( GMT_BONE );	// 하위 노두도 모두 바이페드로 지정해야한다
			}
		}
	}
}

//
// 피지크오브젝트는 뼈대기준으로 좌표변환시키고 일반오브젝트는 피봇기준으로 변환시킨다.
//
void	CAse :: ProcessLocalTransform( GROUP *pGroup )
{
	int		l;
	CGeoMObject *pGeoMObject;
	CASEMesh	*pMesh;
	
	for( l = 0; l < pGroup->m_nMaxGeoMObject; l ++ )
	{
		pGeoMObject = pGroup->m_pGeoMObject[l];
		pMesh = pGeoMObject->m_pMesh;
		
		// pMesh가 없다는것은 일단 바이페드이므로 본 인덱스를 붙여준다.
		//		if( pMesh == NULL )
		//			pGeoMObject->SetIdx( 0 );			// 하위 노드들을 순서대로 훑으며 인덱스를 매김 
		// Biped는 Mesh를 가지고 있지 않다.
		if( pMesh == NULL )		continue;
		
		// 피지크를 가지고 있으면 뼈대기준으로 좌표변환
		if( pMesh->m_pPhysiqueVertex )
		{
			int			i, j, k;
			PHYSIQUE	*p;
			D3DXMATRIX	matInvRotate;
			D3DXVECTOR3	vNormal;
			
			for( i = 0; i < pMesh->m_nMaxVertexList; i ++ )		// 모든 버텍스 피지크 리스트를 루프.
			{
				p = &(pMesh->m_pPhysiqueVertex[i]);
				for( j = 0; j < p->m_nNumBone; j ++ )		// 소속된 bone의 수만큼 루프
				{
					// 소속 bone의 이름을 가진 본 노드를 찾아서 세팅.  본은 그룹0에서만 찾는다.
					for( k = 0; k < m_Group[0].m_nMaxGeoMObject; k ++ )
					{
						p->m_pBlendVertex[j].m_pBone = m_Group[0].m_pGeoMObject[k]->TreeFindNode( p->m_pBlendVertex[j].m_strBoneName );
						if( p->m_pBlendVertex[j].m_pBone )		break;
					}
					if( p->m_pBlendVertex[j].m_pBone == NULL )		
					{
						Error( "%s 에 피지크가 걸려있는 버텍스가 있는데 이 본을 찾을 수가 없다.", p->m_pBlendVertex[j].m_strBoneName );
					}
				}
				if( pMesh->m_bRigid )	// 3link 이상이라 non-blend로 처리
				{
					// 자기 뼈대 기준 로컬좌표로 변환
					D3DXVec3TransformCoord( &(pMesh->m_pVertexList[i].m_vLocalPos), &(pMesh->m_pVertexList[i].m_vWorldPos),    &(p->m_pBlendVertex[0].m_pBone->m_matInverseTM) );
					D3DXVec3TransformCoord( &(pMesh->m_pVertexList[i].m_vNormal),   &(pMesh->m_pVertexList[i].m_vWorldNormal), &(p->m_pBlendVertex[0].m_pBone->m_matInverseTM) );
				} else
				{
					// 로컬좌표로 변환하지 않음.
					pMesh->m_pVertexList[i].m_vLocalPos = pMesh->m_pVertexList[i].m_vWorldPos;
					pMesh->m_pVertexList[i].m_vNormal   = pMesh->m_pVertexList[i].m_vWorldNormal;
				}
			}
			// 노말을 원점으로 되돌려놓는다.
			PHYSIQUE *pPhy;
			for( i = 0; i < pMesh->m_nMaxVertexBuffer; i ++ )
			{
				pPhy = &(pMesh->m_pPhysiqueVertex[ pMesh->m_pIndexBuffer[i].nIdx ]);
				if( pPhy->m_pBlendVertex == NULL ) continue;
				matInvRotate = pPhy->m_pBlendVertex[0].m_pBone->m_matInverseTM;
				matInvRotate._41 = 0;
				matInvRotate._42 = 0;
				matInvRotate._43 = 0;		// 회전값만을 필요로 하므로.
				D3DXVec3TransformCoord( &vNormal, 
					&(pMesh->m_pIndexBuffer[i].normal), 
					&matInvRotate
					);
				pMesh->m_pIndexBuffer[i].normal = vNormal;
			}
			pMesh->CalcNormal();		// 메쉬를 다 읽고난후 노말 게산
		} else
		// 피지크가 아닌 오브젝트는 피봇중심으로 버텍스좌표 변환
		{
			// 일반오브젝트는 피봇중심 좌표로 모두 변환 되므로 월드상의 좌표는 TM을 참조해야 한다.
			pMesh->TransformToLocal();
			pMesh->CalcNormal();		// 메쉬를 다 읽고난후 노말 게산
		}
		
		pMesh->SetBoneIdx();	// m_pVertexBuffer상에서 matIdx를 모두 세팅시킴 - Reindexing하기전 디폴트값으로 사용된다.
	}
}

void	CAse :: CreateMaterialBlock( GROUP *pGroup )
{
	int		i;
	CGeoMObject *pGeoMObject;

	for( i = 0; i < pGroup->m_nMaxGeoMObject; i++ )
	{
		pGeoMObject = pGroup->m_pGeoMObject[i];
		if( pGeoMObject->m_pMesh )
			pGeoMObject->m_pMesh->CreateVertexBufferBlock();
	}
}


////////////////////////////////////////////////////////////////////
//
//		ASE Loader
//
////////////////////////////////////////////////////////////////////
int		CAse :: LoadModel( LPCTSTR szFileName )
{
	CScanner	scan;
	int			i, j;

	if( scan.Load( szFileName ) == 0 )	
	{
		Error( "%s file not found", szFileName );
		return 0;
	}

	strcpy( m_szFullPath, GetFilePath( szFileName ) );	// c:\test\ 패스만 떼서 저장함.

	g_fScale = (float)g_Option.m_fScale / 100;		// 옵션에 설정된 값에 따라 얼마나 축소시킬건가
	g_nMaxID = 0;		// GEOMOBJECT를 읽을때마다 ID번호를 하나씩 부여한다.
	BOOL		bAnimate = FALSE;	// 애니메이션이 있는가 없는가.

	while( 1 )
	{
		scan.GetToken();
		if( scan.tok == FINISHED )	break;
		if( scan.token[0] == '*' )
		{
			scan.GetToken();
			if( strcmp( scan.token, "SCENE" ) == 0 )
			{
				Read_SCENE( &scan );
			}
			if( strcmp( scan.token, "MATERIAL_LIST" ) == 0 )
			{
				Read_MATERIAL_LIST( &scan );
			}

			if( strcmp( scan.token, "GROUP" ) == 0 )
			{
				scan.GetToken();	// Group name
				if( _stricmp( scan.token, "#lod1" ) == 0 )		// LOD1 데이타부 발견...
					m_pGroup = &m_Group[0];
				if( _stricmp( scan.token, "#lod2" ) == 0 )		// LOD2 데이타부 발견...
					m_pGroup = &m_Group[1];
				if( _stricmp( scan.token, "#lod3" ) == 0 )		// LOD3 데이타부 발견...
					m_pGroup = &m_Group[2];
				
			}

			if( strcmp( scan.token, "GEOMOBJECT" ) == 0 || strcmp( scan.token, "HELPEROBJECT" ) == 0 )
			{
				CGeoMObject	*pNode = new CGeoMObject( this );		// 빈 노드를 하나 생성.
				pNode->InitDeviceObjects( m_pd3dDevice );
				if( strcmp( scan.token, "HELPEROBJECT" ) == 0 )		
					pNode->m_bHelper = 1;
//				if( g_pBone == NULL )		g_pBone = pNode;	// 루트로 지정
				if( FAIL == pNode->Read_GEOMOBJECT( &scan ) )
				{
					SAFE_DELETE( pNode );
				}
				else
				{
					char szName[256];
					strcpy( szName, pNode->m_strName );
					_strlwr( szName );
					if( szName[0] == '#' )
					{
						if( _strnicmp( szName+1, "event", 5 ) == 0 )	// 이벤트 좌표
						{
							char szNum[8];
							memset( szNum, 0, sizeof(szNum) );
							szNum[0] = szName[6];
							int nNum = atoi( szNum );
							if( nNum >= 0 && nNum <= MAX_MDL_EVENT - 1 )
							{
								m_vEvent[nNum].x = pNode->m_matLocalTM._41;		// 부모노드로부터의 상대좌표.
								m_vEvent[nNum].y = pNode->m_matLocalTM._42;
								m_vEvent[nNum].z = pNode->m_matLocalTM._43;

								m_pEvent[nNum] = pNode->m_pParent;		// 링크된 부모노드.
								if( nNum + 1 > m_nMaxEvent )
									m_nMaxEvent = nNum + 1;
							}
							SAFE_DELETE( pNode );
						} else
						if( strncmp( szName+1, "coll", 4 ) == 0 )	// 충돌용 메시
						{
							m_pCollObject = pNode;
						} else
						if( strncmp( szName+1, "path", 4 ) == 0 )	// 이동경로 헬퍼
						{
							if( pNode->m_pFrame )		// 프레임이 있는지 확인. 없으면 오브젝트가 있더라도 path로 인정하지 않음.
								m_pPath = pNode;
						} else
						if( _strnicmp( szName+1, "sod", 3 ) == 0 )	// 오른손 무기
						{
							m_mLocalRH = pNode->m_matLocalTM;   // LocalTM을 저장함
							SAFE_DELETE( pNode );
						} else
						if( _strnicmp( szName+1, "lsod", 3 ) == 0 )	// 오른손 무기
						{
							m_mLocalLH = pNode->m_matLocalTM;   // LocalTM을 저장함
							SAFE_DELETE( pNode );
						} else
						if( _strnicmp( szName+1, "shd", 3 ) == 0 )	// 방패
						{
							m_mLocalShield = pNode->m_matLocalTM;
							SAFE_DELETE( pNode );
						} else
						if( _strnicmp( szName+1, "knu", 3 ) == 0 )	// 너클
						{
							m_mLocalKnuckle = pNode->m_matLocalTM;
							SAFE_DELETE( pNode );
						}
								
					} else
					{
						pNode->m_nID = g_nMaxID++;		// 고유번호 부여
						// 바이페드 뼈대만 아니면 다 리스트에 차례로 들어간다.
						if( (strncmp( pNode->m_strName, "Bip", 3 ) != 0 && 
							strncmp( pNode->m_strName, "Bone", 4 ) != 0 ) || 
							pNode->m_pParent == NULL )
						{
							m_pGroup->m_pGeoMObject[ m_pGroup->m_nMaxGeoMObject++ ] = pNode;	// Root오브젝트들을 등록.
						}
					}
				}
				if( pNode && pNode->m_pFrame )		
					bAnimate = TRUE;	// 애니메이션이 하나라도 있으면 true
			}

			// bone + skin data
			// *SKIN_DATA
			if( strcmp( scan.token, "SKIN_DATA" ) == 0 )
			{
				Read_SKIN_DATA( &scan );
			}
		}
	}
	
	// LOD그룹 누락 체크..
	m_bLOD = TRUE;
	if( m_Group[1].m_nMaxGeoMObject == 0 || m_Group[2].m_nMaxGeoMObject == 0 )	// LOD둘중 하나가 없는데...
	{
		if( m_Group[1].m_nMaxGeoMObject > 0 )		// 그중 하나라도 있다. 즉, lod가 1개만 만들어졌다...
			Message( "경고 : #LOD3 가 없다" );
		if( m_Group[2].m_nMaxGeoMObject > 0 )		// 그중 하나라도 있다. 즉, lod가 1개만 만들어졌다...
			Message( "경고 : #LOD2 가 없다" );
		m_bLOD = FALSE;		// 그룹 1, 2중 하나라도 없으면 LOD모델이 없는걸로 함.
	}


	m_pGroup = &m_Group[0];		// 그룹1로 스위칭

	// 오브젝트에 애니메이션이 하나도 없다면 MaxFrame은 0으로 설정
	if( bAnimate == FALSE )		m_nFrameMax = 0;

	CGeoMObject *pGeoMObject;
	CASEMesh	*pMesh;

	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];		
		if( pGroup->m_nMaxGeoMObject == 0 )		continue;	// 오브젝트가 없으면 루프를 돌필요 없다.

		ProcessSetType( pGroup );		// 각 오브젝트들의 타입을 부여.

		// 계층구조의 맨밑에 연결된 의미없는 헬퍼는 삭제시킨다.
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			pGeoMObject->TreeDeleteHelper();
		}

		if( pGroup->m_nMaxGeoMObject >= 2 )
		{
			int k;
			for( i = 0; i < pGroup->m_nMaxGeoMObject-1; i ++ )
			{
				for( k = 1; k < pGroup->m_nMaxGeoMObject; k ++ )
				{
					CGeoMObject *pTemp;
					if( i < k && _strcmpi(pGroup->m_pGeoMObject[i]->m_strName, "light") == 0 &&	// light가 light아닌것보다 앞에 나오면 순서를 바꿈
						_strcmpi(pGroup->m_pGeoMObject[k]->m_strName, "light") != 0 )
					{
						pTemp = pGroup->m_pGeoMObject[i];
						pGroup->m_pGeoMObject[i] = pGroup->m_pGeoMObject[k];
						pGroup->m_pGeoMObject[k] = pTemp;
					}

				}
			}
		}

		// 본 계층의 노드에 인덱스를 붙임. - LOD모델의 바이페드는 오리지날 바이페드와 같게해서 따로 처리 하지 않는다.
		int nBones = 0;
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			pMesh = pGeoMObject->m_pMesh;
			if( pMesh == NULL && !pGeoMObject->m_bHelper)		// 본을 먼저 찾아서 인덱스를 다 붙임.  max bone을 사용한것은 본보다 스킨이 먼저 나오기때문이다.
			{
				nBones ++;
				if( nBones >= 2 )		// 뼈대루트가 두개이상이면 경고
					Message( "뼈대의 루트가 여러개입니다. 루트는 하나만 사용해야 합니다. %s 계층은 무시합니다.", pGeoMObject->m_strName );
				pGeoMObject->SetIdx(0);		// 하위 노드들을 순서대로 훑으며 인덱스	를 매김 

			}
		}
		
		// 매터리얼 블럭 소트
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			pMesh = pGeoMObject->m_pMesh;
			if( pMesh )
				pMesh->SortMaterialBlock();			// 같은 매터리얼끼리 소트시켜 블럭화 시킴
		}

		ProcessLocalTransform( pGroup );	// 피지크오브젝트는 뼈대기준으로 좌표변환시키고 일반오브젝트는 피봇기준으로 변환시킨다.
	}
	
	
	if( m_pCollObject )
		m_pCollObject->m_pMesh->SortMaterialBlock();

// 충돌메시는 LocalTM기준으로 변환시키지 말자...
//	if( m_pCollObject )		// 충돌 메시.
//		m_pCollObject->m_pMesh->TransformToLocal();		

	LoadTexture();				// ASE에서 읽은 Texture정보로 텍스쳐 파일을 읽음.
	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];		
		CreateMaterialBlock( pGroup );		// Draw루틴의 최적화를 위해서 Material들의 집합으로 묶어준다.
	}

	if( m_pCollObject )
		m_pCollObject->m_pMesh->CreateVertexBufferBlock();
	

	// 본 계층의 노드 개수 셈.
	CGeoMObject *pBone = GetBiped();
	if( pBone )
		m_nMaxBoneNode = pBone->GetLength();
	
	SetupMatrices();		// 바운딩 박스 계산에 앞서 스키닝이 한번 이뤄져야 한다.

	// 바운딩 박스 계산.
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			pMesh = pGeoMObject->m_pMesh;
			if( pMesh )
			{
				// 버텍스 리스트를 검색해서 MAX,MIN바운드 영역을 가려낸다
				pMesh->CalcBoundBox();
			
				// 대표바운딩 박스 계산.
				D3DXVECTOR3	vWMin, vWMax;		// 각 오브젝트들의 바운딩박스를 월드로 트랜스폼시킨후의 매트릭스
				if( pMesh->m_pPhysiqueVertex == NULL )
				{
					D3DXVec3TransformCoord( &vWMin, &pMesh->m_vMin, &pGeoMObject->m_matWorld );
					D3DXVec3TransformCoord( &vWMax, &pMesh->m_vMax, &pGeoMObject->m_matWorld );
				} else
				{
					vWMin = pMesh->m_vMin;
					vWMax = pMesh->m_vMax;
				}
				if( vWMin.x < m_vMin.x )	m_vMin.x = vWMin.x;
				if( vWMin.y < m_vMin.y )	m_vMin.y = vWMin.y;
				if( vWMin.z < m_vMin.z )	m_vMin.z = vWMin.z;
				if( vWMax.x > m_vMax.x )	m_vMax.x = vWMax.x;
				if( vWMax.y > m_vMax.y )	m_vMax.y = vWMax.y;
				if( vWMax.z > m_vMax.z )	m_vMax.z = vWMax.z;

			}
		}
	}
	// 리인덱싱.
#ifdef	__XVS
	if( pBone )
	{
//		int nMaxBone = pBone->GetLength();
		if( m_nMaxBoneNode <= MAX_VS_BONE )		// 모델의 뼈대개수가 28개 이하면
			m_bSendVS = TRUE;		// 모델렌더전에 뼈대를 모두 전송하는 방식을 선택.
		else
//			아니면 각 오브젝트는 자기가 쓰는 뼈대만 렌더링 전에 전송하는 방식.  
//			그에따라 각 버텍스는 참조하는 뼈대인덱스를 재정렬 해놔야 함. 
			ReindexingBoneIdx();
	}
#endif

	// indexed vertexbuffer 생성.  Reindexing작업이 끝난 후 이뤄져야 한다. 리인덱싱중에 버텍스가 또 쪼개질수 있기 때문이다.
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			if( pGeoMObject->m_pMesh )
				pGeoMObject->m_pMesh->MakeIndexed();				// facelist와 vertexbuffer를 가지고 indexed data를 생성
		}
	}
	if( m_pCollObject )
		m_pCollObject->m_pMesh->MakeIndexed();

	// 익스포트 할때 매핑 코디네이트 좌표를 출력안하는 실수를 감지 하기 위한것.
	BOOL bMappingCoodi = FALSE;
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];
		for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
		{
			pGeoMObject = pGroup->m_pGeoMObject[i];
			if( pGeoMObject->m_pMesh )
				if( pGeoMObject->m_pMesh->m_pTVertices )		// m_pTVertices가 전부 NULL이면 매핑코디네이트가 출력안됬다는 뜻.
					bMappingCoodi = TRUE;
		}
	}
	if( bMappingCoodi == FALSE )
		Message( "Max에서 Export할때 Mash Options->Mapping 옵션을 켜지 않았습니다.\n이 옵션을 켜지 않으면 텍스쳐 매핑이 보이지 않습니다" );
	

	m_nModelType = MODELTYPE_ASE;
	m_fPerSlerp = 0.5f;		// 따로 초기화

	// 프레임이 있으면 프레임속성할당.
	if( m_nFrameMax > 0 )	
	{
		m_pAttr = new MOTION_ATTR[ m_nFrameMax ];
		memset( m_pAttr, 0, sizeof(MOTION_ATTR) * m_nFrameMax );
	}

	// R Hand가 아버지인 오브젝트(칼같은) 검색. - Group0 에서만 검사한다.
/*	
	BOOL	bLocalRH = FALSE;
	for( i = 0; i < m_Group[0].m_nMaxGeoMObject; i++ )
	{
		pGeoMObject = m_Group[0].m_pGeoMObject[i];
		if( pGeoMObject->m_pParent )
		{
			if( strcmpi(pGeoMObject->m_pParent->m_strName + 6, "r hand") == 0 )	// 부모이름이 R Hand면...
			{
				m_mLocalRH = pGeoMObject->m_matLocalTM;   // LocalTM을 저장함
				bLocalRH = TRUE;
				break;
			} else
			if( strcmpi(pGeoMObject->m_pParent->m_strName + 6, "l hand") == 0 )	// 부모이름이 L Hand면...
			{
				m_mLocalLH = pGeoMObject->m_matLocalTM;   // LocalTM을 저장함
				break;
			}
		}
	}
	// L ForeArm가 아버지인 오브젝트(방패) 검색.
	for( i = 0; i < m_Group[0].m_nMaxGeoMObject; i++ )
	{
		pGeoMObject = m_Group[0].m_pGeoMObject[i];
		if( pGeoMObject->m_pParent )
		{
			if( strcmpi(pGeoMObject->m_pParent->m_strName + 6, "l forearm") == 0 )	// 부모이름이 L ForeArm면...
			{
				m_mLocalShield = pGeoMObject->m_matLocalTM;   // LocalTM을 저장함
				break;
			}
		}
	}
*/	
	CString	str = GetFileName( szFileName );
	str.MakeLower();
	if( str.Left(6) == "parts_" )
	{
		if( m_nFrameMax > 0 )
			Message( "경고 : 말머리 parts_인 ASE파일에는 애니메이션이 없어야 한다." );
//		if( bLocalRH == FALSE )
//			Message( "경고 : parts 에는 오른손에 쥐고 있는 무기가 있어야 합니다. \r\n만약 무기가 있는데도 이 메시지가 나타난다면 무기가 오른손 본(BipXX R Hand)에 링크 되어 있는지 확인해 주시오." );

	}

	
	// 가장 마지막에 수행하는것
	{
		CString	strTitle, strPath, strName;
		strPath  = GetFilePath( szFileName );
		strTitle = GetFileTitle( szFileName );		// test.ase -> test
		strName = strPath + strTitle + ".inf";
		LoadModelEx( strName );	// ASE 확장정보 읽음.
	}

	return 1;
}


// 피지크 정보가 있는 스킨메쉬의 오브젝트를 리턴
//CGeoMObject *CGeoMObject :: GetSkinMesh( void )
//{
//}


//char	pName[256];

HRESULT	LoadTextureFileEx( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, LPDIRECT3DTEXTURE9 *ppTexture )
{
	HRESULT hr = D3DXCreateTextureFromFileEx( pd3dDevice, szFileName, 
						D3DX_DEFAULT, D3DX_DEFAULT, /*D3DX_DEFAULT*/4, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
						D3DPOOL_MANAGED,  D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR              , 
						D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0x00000000, NULL, NULL, ppTexture );
	return hr;
}

// ASE에서 읽은 Texture정보로 텍스쳐 파일을 읽음.
void	CAse :: LoadTexture( void )
{
	int		i, j;
	char	szFileName[1024];
	CString		strTexPath;
	CString		strFullPath = m_szFullPath;

	if( m_pMaterialList == NULL )	return;

	for( i = 0; i < m_nMaxMaterial; i ++ )
	{
		if( m_pMaterialList[i].m_strBitmapFileName[0] )
		{
			// Use D3DX to create a texture from a file based image
			// ASE에 기록된 텍스쳐파일 패스로 찾아본다.
			// - ASE기록된 텍스쳐패스는 사용하지 않는다. 무조건 ase가 있는 폴더부터 찾는다.
//			if( FAILED( LoadTextureFileEx( m_pd3dDevice, m_pMaterialList[i].m_strBitmapFileName, &m_pMaterialList[i].m_pTexture ) ) )
			{
				strcpy( szFileName, GetFileName(m_pMaterialList[i].m_strBitmapFileName) );	// 텍스쳐의 파일명만 떼넴 test.tga
				strTexPath = strFullPath + szFileName;		// ASE풀패스 + szFileName
				// ASE풀패스 + 텍스쳐파일로 찾아본다.
				if( FAILED( LoadTextureFileEx( m_pd3dDevice, strTexPath, &m_pMaterialList[i].m_pTexture ) ) )
				{
					strTexPath = g_strPathTexture + szFileName;
//					Message( "%s", strTexPath );
					// 텍스쳐 폴더에서 찾아본다.
					if( FAILED( LoadTextureFileEx( m_pd3dDevice, strTexPath, &m_pMaterialList[i].m_pTexture ) ) )
	//				if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, strTexPath, &m_pMaterialList[i].m_pTexture ) ) )
					{
						if( m_pd3dDevice )
							Error( "%s 텍스쳐 비트맵 못읽음", strTexPath );
//						Error( "%s 텍스쳐 비트맵 못읽음", m_pMaterialList[i].m_strBitmapFileName );
					}
				}
			}
		}
		// 서브매트리얼이 있으면 그것도 텍스쳐 읽는다
		if( m_pMaterialList[i].m_pSubMaterial )
		{
			for( j = 0; j < m_pMaterialList[i].m_nMaxSubMaterial; j ++ )
			{
				if( m_pMaterialList[i].m_pSubMaterial[j].m_strBitmapFileName[0] )
				{
					// Use D3DX to create a texture from a file based image
//					if( FAILED( LoadTextureFileEx( m_pd3dDevice, m_pMaterialList[i].m_pSubMaterial[j].m_strBitmapFileName,  &m_pMaterialList[i].m_pSubMaterial[j].m_pTexture ) ) )
					{
						strcpy( szFileName, GetFileName(m_pMaterialList[i].m_pSubMaterial[j].m_strBitmapFileName) );	// 텍스쳐의 파일명만 떼넴 test.tga
						strTexPath = strFullPath + szFileName;		// ASE풀패스 + szFileName
						if( FAILED( LoadTextureFileEx( m_pd3dDevice, strTexPath,  &m_pMaterialList[i].m_pSubMaterial[j].m_pTexture ) ) )
						{
							strTexPath = g_strPathTexture + szFileName;
	//						if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, strTexPath, &m_pMaterialList[i].m_pSubMaterial[j].m_pTexture ) ) )
							if( FAILED( LoadTextureFileEx( m_pd3dDevice, strTexPath,  &m_pMaterialList[i].m_pSubMaterial[j].m_pTexture ) ) )
							{
								if( m_pd3dDevice )
									Error( "%s 서브 텍스쳐 비트맵 못읽음", strTexPath );
//								Error( "%s 서브 텍스쳐 비트맵 못읽음", m_pMaterialList[i].m_pSubMaterial[j].m_strBitmapFileName );
							}
						}
					}
				}
			}
		}
	}
}

extern CScript g_DefineSound;

void	CAse :: FrameMove( D3DXVECTOR3 *pvSndPos, float fSpeed )
{
	if( g_DefineSound.m_pBuf )	// 스크립트 안읽었으면 들어가지 말것.
	{
		MOTION_ATTR *pAttr = IsAttrSound();
		if( pAttr && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
		{
			if( PLAYSND( pAttr->m_nSndID, pvSndPos ) == FALSE )
			{
//				Message( "PlaySound : 실패 %d", pAttr->m_nSndID );
//				m_nPause = 1;
			}
		}
	}

	CModel::FrameMove();

	
	
}

//
//float	slp = 0.0;
void		CAse :: SetupMatrices( void ) 
{ 
	int		i;
	CGeoMObject	*pObj;
	CASEMesh		*pMesh;
	float		fSlp;

	m_lnPrev = m_lnCurr;
	QueryPerformanceCounter( &m_lnCurr );
	m_dUps = (double)(m_lnCurr.QuadPart - m_lnPrev.QuadPart) / m_lnFreq.QuadPart;

//	double dPerSlerp = m_dUps / m_dKps;
//	double dPerSlerp = 0.5;
	int		nCurrFrm = (int)m_fFrameCurrent;		// 정수형 프레임번호로 바꿈
	fSlp = m_fFrameCurrent - (float)nCurrFrm;		// 소숫점 부분만 가져옴

	if( m_pPath )
	{
		m_pPath->SetupQuat( fSlp, NULL );
	}

//	if( m_bBiped )	SetGroup(0);	// 본이 있는 오브젝트는 그룹0의 본을 사용.
//	m_pGroup = &m_Group[0];		// 그룹0로 스위칭

	for( i = 0; i < m_pGroup->m_nMaxGeoMObject; i ++ )
	{
		pObj = m_pGroup->m_pGeoMObject[i];
		pMesh = pObj->m_pMesh;
		if( m_pPath && pObj->GetType() == GMT_BONE )
		{
			pObj->SetupQuat( fSlp, &m_pPath->m_matWorld );
		} else
			pObj->SetupQuat( fSlp );
//		pObj->SetupMatrices( 0 );		// 계층별로 TM들을 계산하여 월드 트랜스폼 한다.

		// 메쉬가 있고 
		if( pMesh )
		{
			// 피지크가 있는것이라면 스킨피즈크를 재계산
			if( pMesh->m_pPhysiqueVertex )
			{
				pMesh->TransformSkinVertex();		// 스킨 버텍스들을 계산된 본에 따라 재 계산.
				pMesh->SetVertexBuffer();			// 계산된 스킨 버텍스들을 Face list에 따라 버텍스 버퍼에 다시 채움.
			}
		}
	}

//	m_nCurrFrame = (int)m_fFrameCurrent;		// 소숫점 떼냄

//	slp += 0.001;
//	if( slp > 1.0 )		slp -= 1.0;
}


//
//
//
void		CAse :: SetupQuat( void ) 
{ /*
	int		i;
	CGeoMObject	*pObj;
	CASEMesh		*pMesh;

	for( i = 0; i < m_nMaxGeoMObject; i ++ )
	{
		pObj = m_pGeoMObject[i];
		pMesh = pObj->m_pMesh;
		pObj->SetupQuat( 0 );		// 계층별로 TM들을 계산하여 월드 트랜스폼 한다.

		// 피지크가 있는것이라면
		if( pMesh->m_pPhysiqueVertex )
		{
			pMesh->TransformSkinVertex();		// 스킨 버텍스들을 계산된 본에 따라 재 계산.
			pMesh->SetVertexBuffer();			// 계산된 스킨 버텍스들을 Face list에 따라 버텍스 버퍼에 다시 채움.
		}
	}*/
}

// Biped는 메쉬가 없으므로 렌더링 안된다.
// Skin은 지정된 월드로 맞추고 렌더링 한다.
// 기타 일반 오브젝트
// 부모가 있는경우 : SetupMatrices()에서 부모와 곱해진 월드TM이 나왔으므로 그것을 월드로 맞추고 렌더
// 부모가 없는경우 : SetupMatrices()에서 부모가 없는 경우는 자기 TM이 곧 LocalTM이고 LocalTM으로 
//                   월드가 맞춰 졌기때문에 그대로 사용하면 됨
// 따라서...m_matWorld로 월드를 맞추면 된다는 야그...


BOOL	CAse :: Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	D3DXMATRIX	mat;
	CGeoMObject	*pObj;
	int			i;

	SetupMatrices();

//	pd3dDevice->SetTransform( D3DTS_WORLD, pmWorld );		// Set World Transform 
//	g_Grp3D.RenderAABB( m_vMin, m_vMax, 0xffffffff );

//	SetGroup(2);
	// 
	for( i = 0; i < m_pGroup->m_nMaxGeoMObject; i++ )
	{
		pObj = m_pGroup->m_pGeoMObject[i];
		pObj->Render( pmWorld );
	}

	if( g_nCollObject )
		if( m_pCollObject )
			m_pCollObject->Render( pmWorld );
	return S_OK;
}

BOOL	CAse::RenderBB( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	CGeoMObject	*pObj;
	int			i;

	pd3dDevice->SetTransform( D3DTS_WORLD, pmWorld );		// Set World Transform 
	g_Grp3D.RenderAABB( m_vMin, m_vMax, 0xff0000ff );

	SetGroup(0);
	// 
	for( i = 0; i < m_pGroup->m_nMaxGeoMObject; i++ )
	{
		pObj = m_pGroup->m_pGeoMObject[i];
		pObj->RenderBB( pmWorld );
	}

	return TRUE;
}


//
// 스킨을 저장할땐 뼈대의 기본셋도 같이 저장하자.
//
int		CAse :: SaveModel( LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY	
	if( IsBiped() )
		return SaveSkinEach( szFileName );
	else
		return SaveNormMesh( szFileName );
#endif
	
	return SUCCESS;
}

//
// ase파일명 szFileName을 이용해 모션이나 o3d를 자동으로 인식해서 저장해준다
// Convert시에 이것을 사용한다.
//
void	CAse :: SaveAuto( LPCTSTR szASEName )
{
#ifndef __VIEWER_ONLY
	
	CString	strASE, strHead, strName, strTitle, strSimple;
	int		nType = 0;

	strASE = szASEName;
	strTitle = GetFileTitle( strASE );
	strSimple = GetFileName( strASE );
	strHead = strTitle.Left(strTitle.Find( '_' )+1);		// 헤더부분만 떼냄.  mvr_  part_
	strHead.MakeLower();

	strTitle = strASE.Left( strASE.GetLength() - 4 );	// 패스 + 타이틀만 남겨두고 확장자는 떼냄.

//	ase.LoadModel( strASE );					// ase 읽음
	CGeoMObject	*pBiped = GetBiped();	// 바이페드포인터

	// 본이 포함되어 있는것(스킨형)과 없는것 따로 처리.
	// 스킨형은 o3d, ani로 뽑아냄
	if( pBiped )		// 스킨형
	{
		int		n = 0, nNum = 0;
		while(1)		// 이름내 _갯수를 셈
		{
			n = strSimple.Find( '_', n );
			if( n == -1 )	break;
			n++;
			nNum ++;
		}
		// 바이페드ase 중에서....
		if( strHead == "mvr_" )	// mvr은 스킨과 애니메이션 두가지 형태
		{
			switch( nNum )
			{
			case 1:		// mvr_heroine.ase 밑줄하나형태. o3d하나에 오브젝트모두를 저장하고 스킨만 저장.
					nType = 3;		break;		
			case 2:		
			default:		// mvr_heroine_run.ase mvr_female_basic.ase 밑줄두개나 그이상. 애니메이션만 뽑음. 혹은 본만 뽑음
				{
					CString		str = strTitle;
					str.MakeLower();		// 소문자로 변환
					if( str.Right(5) == "basic" )		// 마지막 5자가 "basic"인지 검사.
					{
						// 본만 뽑음
						nType = 4;	
					} else
					{
						// 애니메이션만 뽑음
						nType = 2;	
					}
					break;		
				}
			}
		} else
		if( strHead == "parts_" ) // parts는 모두 스킨.
		{
			nType = 1;		// 스킨만 뽑아냄. GeoMObject별로 o3d가 생성.
		} else
#ifdef __ITEM_ANIMATION_SAVE
		if( strHead == "item_" ) // item도 바이페드를 포함하고 있는 경우 무버와 같은 방법으로 저장
		{
			switch( nNum )
			{
			case 1:
				{
					nType = 3;						// item_test.ase 형태. chr과 o3d를 함께 뽑아냄
					break;
				}
			case 2:
			default:
				{
					CString str = strTitle;
					str.MakeLower();				// 소문자로 변환
					if( str.Right(5) == "basic" )	// 마지막 5자가 "basic"인지 검사
					{
						nType = 4;					// 본만 뽑음
					}
					else
					{
						nType = 2;					// 애니메이션만 뽑음
					}
				}
			}
		} else
#endif // __ITEM_ANIMATION_SAVE
		{
			nType = 0;		// ase파일명으로 o3d생성.  모든오브젝이 o3d안에 다 들어가고 애니메이션까지 들어간다.
		}

		switch( nType )
		{
		case 0:		// obj_풍선.ase, test.ase  기타말머리거나 파일명에 밑줄이 없는경우. 본을 사용한 배경오브젝트에 사용.
			strName = strTitle + ".o3d";			// test -> test.o3d
			SaveSkinOne( strName, TRUE );				// 파일 하나짜리로 저장.
			// mes
			break;
		case 1:		// parts_머서너리.ase	// 파일명에 밑줄이 하나 있는 경우 오브젝트별로 스킨을 뽑아냄
			// 
			{
			strName = strTitle + ".o3d";			// test -> test.o3d
			SaveSkinEach( strName );				// save skin
			}
			break;
		case 3:		// mvr_buff.ase  -> mvr_buff.chr   mvr_buff.o3d  두개 생성
			strName = strTitle + ".o3d";			// test -> test.o3d
			SaveSkinOne( strName, FALSE );			// 파일 하나짜리로 저장. 애니메이션은 저장하지 않음.
			strName = strTitle + ".chr";
			SaveCHR( strName );			// chr을 같이 저장.
			// mes
			break;
		case 4:		// mvr_female_basic.ase   -> mvr_female.chr  본만 뽑아냄
			{
				CString	str;
				str = strTitle.Left( strTitle.GetLength() - 6 );	// mvr_female_basic.ase에서 _basic을 뺌
				strName = str + ".chr";
				SaveCHR( strName );			// chr을 같이 저장.
			}
			break;
		case 2:		// mvr_heroine_attack.ase	파일명에 밑줄이 2개이상 있는 경우  ANI만 뽑아냄
		default:	// mvr_heroine_attack_2.ase
			// ani
			strName = strTitle + ".ani";			// test -> test.ani
			SaveANI( strName );				// save ani
			break;
		}
		// save 완료
	} else
	// 바이페드가 아닌것들은 일반오브젝트로 뽑아냄
	{
		// mes로 변환
		strName = strTitle + ".o3d";			// test -> test.o3d
		SaveNormMesh( strName );			// save normal mesh
	}
#endif // not viewer only
}

// 
//	파일내 오브젝트들을 따로따로 파일에 저장함
//
int		CAse :: SaveSkinEach( LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY
	
	int			i, j;
	CGeoMObject	*pGeo;
	char	buff[1024];
	FILE	*fp;
	int		nTemp = 1;

	SetGroup(0);

	strcpy( buff, szFileName );
	for( i = 0; i < m_pGroup->m_nMaxGeoMObject; i ++ )
	{
		pGeo = m_pGroup->m_pGeoMObject[i];
		if( pGeo->m_pMesh )
		{
			strcpy( buff, GetFilePath( szFileName ) );
			strcat( buff, pGeo->m_strName );		// 오브젝트명 + .o3d = 파일명 생성
			strcat( buff, ".o3d" );

			if( pGeo->m_pMesh->m_pPhysiqueVertex && pGeo->GetType() != GMT_SKIN )
				Error( "SaveSkinEach() : %s 가 피지크가 있는데 MT_SKIN이 아니다.%d", pGeo->m_strName, pGeo->GetType() );

			// 파일로 저장
			fp = fopen( buff, "wb" );
			if( fp == NULL )	
				Error( "%s : 저장실패.  check out했는지 확인할 것.", buff );
			SaveMeshHeader( fp, buff );		// 공통 헤더부 저장

			int nNumNode = 0;
			fwrite( &nNumNode, 4, 1, fp );		// 뼈대 개수.  스킨파츠는 bone이 따로 저장되므로 o3d엔 본을 저장해선 안된다.

			int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD데이타가 없으면 Lvl0만 저장한다.
			nTemp = nMaxGroup * 1;			// 파츠는 lod별로 1개씩이다.
			fwrite( &nTemp, 4, 1, fp );		// lod3개의 오브젝트 개수를 모두 합한 수.  게임에선 메모리풀로 쓰기 위함.

			for( j = 0; j < nMaxGroup; j ++ )
			{
				CGeoMObject *pObj = pGeo;
				if( m_bLOD && j >= 1 )	// lod데이타가 있고....lod2, 3처리할때만 안으로 들어간다.
				{
					pObj = NULL;
					for( int k = 0; k < m_Group[j].m_nMaxGeoMObject; k ++ )	// lod2, 3에서 같은이름의 오브젝트를 찾음.
					{
						if( _stricmp( pGeo->m_strName, m_Group[j].m_pGeoMObject[k]->m_strName ) == 0 )
						{
							pObj = m_Group[j].m_pGeoMObject[k];
							break;
						}
					}
					if( pObj == NULL )	// 같은 이름의 오브젝트가 없다. 에러난 상황
					{
						Message( "경고 CAse::SaveSkinEach : LOD1의 %s와 같은 이름의 오브젝트가 LOD Group%d에 없다.", pGeo->m_strName, j );
						pObj = pGeo;		// 원본으로 대체.
					}
				}

				nTemp = 1;
				fwrite( &nTemp, 4, 1, fp );			// 지오메트리 갯수(무조건 1이다) - 일단은 이렇게 했지만 쇠사슬같은 오브젝트면 개수가 1개 이상이 된다.
				// 피지크가 있는건 스킨, 없는건 일반
				int	type = pObj->GetType();
				if( _strcmpi(pObj->m_strName, "light") == 0 )
					type |= 0x80000000;
				fwrite( &type, 4, 1, fp );		// 오브젝트 타입
	#ifdef	__XVS
				fwrite( &pObj->m_pMesh->m_nMaxUseBone, 4, 1, fp );
				if( pObj->m_pMesh->m_nMaxUseBone > 0 )
					fwrite( pObj->m_pMesh->m_UseBone, sizeof(int) * pObj->m_pMesh->m_nMaxUseBone, 1, fp );	// 오브젝트가 사용하는 본리스트
	#endif
				fwrite( &pObj->m_nID, 4, 1, fp );	// Object ID

				if( pObj->m_pParent )
				{
					fwrite( &pObj->m_pParent->m_nIdx, 4, 1, fp );		// 부모가 있을땐 부모의 인덱스저장, 없으면 -1.
					if( pObj->m_pParent->GetType() == GMT_SKIN )
						Error( "CAse::SaveSkinEach - 오브젝트%s의 부모가 스킨이다.", pObj->m_pParent->m_strName );
					type = pObj->m_pParent->GetType();
					fwrite( &type, 4, 1, fp );		// 부모의 타입
				} else
				{
					int temp = -1;
					fwrite( &temp, 4, 1, fp );		// 그렇지 않은건 -1저장
				}
				fwrite( &(pObj->m_matLocalTM), sizeof(D3DXMATRIX), 1, fp );	// LocalTM저장

			// 실제 메쉬데이타부 저장
				pObj->SaveMesh( fp );
				switch( pObj->GetType() )
				{
				case GMT_BONE:	
					Error( "CAse::SaveSkinEach - 오브젝트%s가 메쉬가 있는데 속성이 GMT_BONE이다", pObj->m_strName );
					break;
				case GMT_SKIN:	
					if( pObj->m_pFrame )
						Error( "CAse::SaveSkinEach - 스킨(%s)에 웬 m_pFrame이 있냐", pObj->m_strName );
					break;  
				default:
					if( m_nFrameMax > 0 )
						pObj->SaveTMAni( fp );	
					break;
				}
			}

			fclose(fp);
#ifdef	__COLA
			{
				CString	strTitle, strPath, strName;
				strPath  = GetFilePath( szFileName );		// path
				strTitle = GetFileTitle( szFileName );		// test.ase -> test
				strName  = strPath + strTitle + ".inf";
				SaveModelEx( strName );		// ASE 확장정보
			}
#endif
			if( m_pd3dDevice )		// 콘솔모드 컨버터로 쓸땐 메시지창이 떠서는 안되기때문이다.
				Message( "%s Save Complete.", buff );
//				Message( "저장완료 : %s", buff );
//			break;		
		}
	}
#endif // not viewer
	return SUCCESS;
}

// 
//	오브젝트들을 o3d하나에 다 저장. 본의 애니메이션까지 저장.
// mvr_buff.ase    obj_풍선    obj_회전관람차 
//
int		CAse :: SaveSkinOne( LPCTSTR szFileName, BOOL bSaveBone )
{
#ifndef __VIEWER_ONLY
	int			i,j;
	CGeoMObject	*pGeo;
	char	buff[256];
	int		nTemp;
	int		nIdx;

	strcpy( buff, szFileName );

	// 저장 파일 오픈
	FILE *fp = fopen( buff, "wb" );
	if( fp == NULL )	
	{
		Error( "%s : 저장실패.  check out했는지 확인할 것.", buff );
		return FAIL;
	}

	SetGroup(0);
	// 공통 헤더부 저장
	SaveMeshHeader( fp, buff );

	// 뼈대의 기본셋 저장
	pGeo = GetBiped();		// 뼈대 트리의 시작 포인터 얻음.
	if( bSaveBone )			// mvr_buff같은건 이게 FALSE다.  obj_풍선같은경우가 TRUE
	{
		fwrite( &m_nMaxBoneNode, 4, 1, fp );		// 뼈대 개수 저장
		if( m_nMaxBoneNode > 0 )
		{
			pGeo->WriteBone( fp );	// 뼈대 트리의 기본셋 TM을 저장.
			if( m_nFrameMax > 0 )		// 본이 있고 애니메이션이 있으면 자체 본애니 저장. obj_풍선에 사용
			{
				// CMotion::ReadTM()으로 읽음
				pGeo->WriteBoneInfo( fp );		// 뼈대구성 정보
				pGeo->SaveANI( fp );
				// ReadTM끝.
			}
#ifdef	__XVS
			fwrite( &m_bSendVS, 4, 1, fp );
#endif
		}
	}
	else
	{
		nTemp = 0;
		fwrite( &nTemp, 4, 1, fp );		// 애니를 저장하지 않을때는 bone개수는 0이다.
	}

	CGeoMObject **pGeoMObject;
	int	nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;
	int	nMaxObj[3] = { 0, 0, 0 };
	nTemp = 0;
	for( j = 0; j < nMaxGroup; j ++ )
	{
		for( i = 0; i < m_Group[j].m_nMaxGeoMObject; i ++ )
			if( m_Group[j].m_pGeoMObject[i]->m_pMesh )	// 본을 제외한 실제 오브젝트의 개수를 셈.
				nMaxObj[j] ++;		// 실제 오브젝트 개수를 세어서 배열에 넣음.
		nTemp += nMaxObj[j];
	}

	fwrite( &nTemp, 4, 1, fp );		// lod3개의 오브젝트 개수를 모두 합한 수.  게임에선 메모리풀로 쓰기 위함.
	
	// 각 오브젝트들의 인덱스 재설정.
	for( j = 0; j < nMaxGroup; j ++ )
	{
		pGeoMObject = m_Group[j].m_pGeoMObject;
		int nMaxGeoMObject = m_Group[j].m_nMaxGeoMObject;
		if( nMaxGeoMObject == 0 )	
		{
			Error( "SaveSkinOne : MaxGeoMObject가 0이다" );
			continue;
		}
		nIdx = 0;
		nTemp = 0;
		for( i = 0; i < nMaxGeoMObject; i ++ )
		{
			pGeo = pGeoMObject[i];
			if( pGeo->m_pMesh )		
				pGeo->m_nIdx = nIdx ++;		// 본은 빼고 나머지 오브젝트들은 파일에 저장되는 순서대로 일련번호를 붙임.
		}
		fwrite( &nMaxObj[j], 4, 1, fp );			// 지오메트리 갯수

		// 실제 기록
		for( i = 0; i < nMaxGeoMObject; i ++ )
		{
			pGeo = pGeoMObject[i];
			if( pGeo->m_pMesh )
			{
				if( pGeo->m_pMesh->m_pPhysiqueVertex && pGeo->GetType() != GMT_SKIN )
					Error( "SaveSkinOne() : %s 가 피지크가 있는데 MT_SKIN이 아니다.%d", pGeo->m_strName, pGeo->GetType() );

				// 피지크가 있는건 스킨, 없는건 일반
				int type = pGeo->GetType();
				if( _strcmpi(pGeo->m_strName, "light") == 0 )
					type |= 0x80000000;
				fwrite( &type, 4, 1, fp );		// 오브젝트 타입
	#ifdef	__XVS
				fwrite( &pGeo->m_pMesh->m_nMaxUseBone, 4, 1, fp );
				if( pGeo->m_pMesh->m_nMaxUseBone > 0 )
					fwrite( pGeo->m_pMesh->m_UseBone, sizeof(int) * pGeo->m_pMesh->m_nMaxUseBone, 1, fp );	// 오브젝트가 사용하는 본리스트
	#endif
				fwrite( &pGeo->m_nID, 4, 1, fp );	// Object ID
				// 바이페드나 스킨메쉬만 아니면 모두 ID를 저장한다.
				
				if( pGeo->m_pParent )
				{
					fwrite( &pGeo->m_pParent->m_nIdx, 4, 1, fp );		// 부모가 있을땐 부모의 인덱스저장, 없으면 -1.
					if( pGeo->m_pParent->GetType() == GMT_SKIN )
						Error( "CAse::SaveSkinOne - 오브젝트%s의 부모가 스킨이다.", pGeo->m_pParent->m_strName );
					type = pGeo->m_pParent->GetType();
					fwrite( &type, 4, 1, fp );		// 부모의 타입
				} else
				{
					int temp = -1;
					fwrite( &temp, 4, 1, fp );		// 그렇지 않은건 -1저장
				}
				fwrite( &(pGeo->m_matLocalTM), sizeof(D3DXMATRIX), 1, fp );	// LocalTM저장

				// 실제 메쉬데이타부 저장
				pGeo->SaveMesh( fp );		
				switch( pGeo->GetType() )
					{
				case GMT_BONE:	
					Error( "CAse::SaveSkinOne - 오브젝트%s가 메쉬가 있는데 속성이 GMT_BONE이다", pGeo->m_strName );
					break;
				case GMT_SKIN:	
					if( pGeo->m_pFrame )
						Error( "CAse::SaveSkinOne - 스킨(%s)에 웬 m_pFrame이 있냐", pGeo->m_strName );
					break;  
				default:
					if( m_nFrameMax > 0 )
						pGeo->SaveTMAni( fp );		// GMT_NORMAL이외는 이쪽으로 아예 오지 않는다는것에 주의
					break;
				}
			}
		}
	} // GROUP LOOP

	// 속성부 저장. - Version 21부터
	if( m_nFrameMax > 0 )
	{
		int nAttr = FALSE;
		for( i = 0; i < m_nFrameMax; i ++ )
		{
			if( m_pAttr[i].m_dwAttr )
			{
				nAttr = TRUE;		// 속성이 하나라도 들어가있으면 속성부 저장함.
				break;
			}
		}
		if( nAttr )
		{
			fwrite( &m_nFrameMax, 4, 1, fp );		// 속성부 저장이 있다는 신호.
			fwrite( m_pAttr, sizeof(MOTION_ATTR) * m_nFrameMax, 1, fp );
		}
	}
	
	fclose(fp);
#ifdef	__COLA
	{
		CString	strTitle, strPath, strName;
		strPath  = GetFilePath( szFileName );		// path
		strTitle = GetFileTitle( szFileName );		// test.ase -> test
		strName  = strPath + strTitle + ".inf";
		SaveModelEx( strName );		// ASE 확장정보
	}
#endif

	if( m_pd3dDevice )		// 콘솔모드 컨버터로 쓸땐 메시지창이 떠서는 안되기때문이다.
		Message( "%s Save Complete.", buff );
//		Message( "저장완료 : %s", buff );
#endif // not viewer
	return SUCCESS;
}


int		CAse :: SaveMeshHeader( FILE *fp, LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY
	int		nVer = 22;					//VER_MESH
	int		nSerial = xRand();			// 32비트형 시리얼 번호
	D3DXVECTOR3		vMin, vMax, vTemp;
	char	reserved[32];
	int		nTemp = 0;

	char cLen;
	
	strcpy( reserved, GetFileName( szFileName ) );	// 파일명만 떼서 배열에 넣음. c:\test.o3d -> test.o3d
	cLen = (char)strlen( reserved );	// 파일명 길이.
	for( int j = 0; j < cLen; j ++ )
	{
		reserved[j] = reserved[j] ^ (char)0xcd;
	}
		
	if( cLen >= 32 )
		Message( "경고 : %s 파일명이 32바이트를 넘는다. %d", szFileName, (int)cLen );
//	memset( reserved, 0, 32 );
	fwrite( &cLen, 1, 1, fp );		// 파일명 길이. 널포함안함
	fwrite( reserved, cLen, 1, fp );	// 길이만큼만 파일명 저장. 널포함안함
	// 공통 헤더부
	fwrite( &nVer, 4, 1, fp );			// 버전.
	fwrite( &nSerial, 4, 1, fp );		// 고유 번호
	vTemp = D3DXVECTOR3(0,0,0);
	fwrite( &g_vForce1, sizeof(D3DXVECTOR3), 1, fp );	// 검광 위치 1,2의 좌표 저장.
	fwrite( &g_vForce2, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &g_vForce3, sizeof(D3DXVECTOR3), 1, fp );	// 추가 검광 위치 3,4의 좌표 저장.
	fwrite( &g_vForce4, sizeof(D3DXVECTOR3), 1, fp );

	fwrite( &m_fScrlU, sizeof(float), 1, fp );
	fwrite( &m_fScrlV, sizeof(float), 1, fp );
	fwrite( reserved, 16, 1, fp );		// reserved

	fwrite( &m_vMin, sizeof(D3DXVECTOR3), 1, fp );		//
	fwrite( &m_vMax, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &m_fPerSlerp, sizeof(float), 1, fp );		// 
	fwrite( &m_nFrameMax, 4, 1, fp );			// 애니가 없으면 0이되도록 저장할것.

	fwrite( &m_nMaxEvent, 4, 1, fp );	// 이벤트 좌표 저장
	if( m_nMaxEvent > 0 )
		fwrite( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1, fp );

	// 충돌메시 저장
	if( m_pCollObject )		
	{
		nTemp = 1;	fwrite( &nTemp, 4, 1, fp );
		m_pCollObject->SaveMesh( fp );
	}
	else
	{
		nTemp = 0;	fwrite( &nTemp, 4, 1, fp );
	}

	fwrite( &m_bLOD, 4, 1, fp );		// LOD데이타가 있느냐?.
	
#endif  // not viewer only

	return SUCCESS;
}

//
// 확장 정보 저장.
// 타격점/사운드, 검광지점등.
// 

int		CAse :: SaveModelEx( LPCTSTR szFileName )
{
	int		nVer = 3, nTemp;
	char	reserved[32];
	// FILE Open
	FILE *fp = fopen( szFileName, "wb" );
	if( fp == NULL )
	{
		Error( "CAse::SaveModelEx() : %s Save Filed.  Check this file Readonly", szFileName );
		return FAIL;
	}

	memset( reserved, 0, 32 );
	fwrite( &nVer, 4, 1, fp );		// version
	// motion 관련
	if( m_pAttr )
	{
		nTemp = 1;	fwrite( &nTemp, 4, 1, fp );
		fwrite( m_pAttr, sizeof(MOTION_ATTR) * m_nFrameMax, 1, fp );	// motion attr
	}
	else
	{
		nTemp = 0;	fwrite( &nTemp, 4, 1, fp );
	}		

	// model 관련
	fwrite( &g_vForce1, sizeof(D3DXVECTOR3), 1, fp );	// 검광 위치 1,2의 좌표 저장.
	fwrite( &g_vForce2, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &g_vForce3, sizeof(D3DXVECTOR3), 1, fp );	// 추가 검광 위치 3,4의 좌표 저장.
	fwrite( &g_vForce4, sizeof(D3DXVECTOR3), 1, fp );

	fwrite( &m_fScrlU, sizeof(float), 1, fp );
	fwrite( &m_fScrlV, sizeof(float), 1, fp );
	
	fwrite( &m_bNormalClear, sizeof(BOOL), 1, fp );
	// reserved
	fwrite( reserved, 28, 1, fp );		// reserved
	
	fclose(fp);
	return SUCCESS;
}

// 
// ASE확장 정보 읽음
// 확장자 inf
// 반드시 LoadModel을 하고난 후 호출 할것.
int		CAse :: LoadModelEx( LPCTSTR szFileName )
{
	int		nVer, nTemp;
	// FILE Open
	FILE *fp = fopen( szFileName, "rb" );
	if( fp == NULL )
	{
//		Error( "CAse::LoadModelEx() : %s 파일이 없다. 소스세이프에서 다시 받아볼것!", szFileName );
		return FAIL;
	}

	fread( &nVer, 4, 1, fp );		// version
	if( nVer == 1 )
	{
		// 구버전.
		// motion 
		fread( &nTemp, 4, 1, fp );
		if( nTemp )
		{
			for( int i = 0; i < m_nFrameMax; i ++ )
			{
				fread( &m_pAttr[i].m_dwAttr, 4, 1, fp );		// motion attr
				fseek( fp, 32, SEEK_CUR );	// wav filename 32byte skip
				fread( &m_pAttr[i].m_fFrame, 4, 1, fp );		// motion attr
			}
		}
	} 
	else if( nVer == VER_INFO || nVer == 3)
	{
		// motion 
		fread( &nTemp, 4, 1, fp );
		if( nTemp )
			fread( m_pAttr, sizeof(MOTION_ATTR) * m_nFrameMax, 1, fp );		// motion attr
		
	}
	else
	{	// 읽기 불가능한 버전.
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_INFO );
		fclose(fp);
		return FAIL;
	}


	// model 관련
	fread( &g_vForce1, sizeof(D3DXVECTOR3), 1, fp );	// 검광 위치 1,2의 좌표 저장.
	fread( &g_vForce2, sizeof(D3DXVECTOR3), 1, fp );
	if(nVer == 3)
	{
		fread( &g_vForce3, sizeof(D3DXVECTOR3), 1, fp );	//추가 검광 위치 3,4의 좌표 저장.
		fread( &g_vForce4, sizeof(D3DXVECTOR3), 1, fp );		
	}

	fread( &m_fScrlU, sizeof(float), 1, fp );
	fread( &m_fScrlV, sizeof(float), 1, fp );

	fread( &m_bNormalClear, sizeof(BOOL), 1, fp );
	if( m_bNormalClear )
		ClearNormal();

	// reserved
	fseek( fp, 28, SEEK_CUR );		// reserved

	fclose(fp);
	return SUCCESS;
}

//
// 매터리얼, 메쉬, 애니메이션까지 모두 저장한다.
// 일반 모델이나, 키프레임 모델은 이걸로 저장
// 
int		CAse :: SaveNormMesh( LPCTSTR szFileName )
{
	CGeoMObject	*pGeo;
	int		i, j;
	int		nIdx = 0, temp = 0;	

	// FILE Open
	FILE *fp = fopen( szFileName, "wb" );
	if( fp == NULL )	
	{
		Error( "CAse::SaveNormMesh() : %s 세이브 실패.  Readonly를 확인할것", szFileName );
		return FAIL;
	}

	SaveMeshHeader( fp, szFileName );		// 공통 헤더부 저장

	int nMaxBone = 0;
	fwrite( &nMaxBone, 4, 1, fp );		// 일반형 메쉬는 본이 없다.

	int		nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD데이타가 있을때만 3개다 저장한다.
	temp = 0;
	for( j = 0; j < nMaxGroup; j ++ )
		temp += m_Group[j].m_nMaxGeoMObject;

	fwrite( &temp, 4, 1, fp );		// lod3개의 오브젝트 개수를 모두 합한 수.  게임에선 메모리풀로 쓰기 위함.

	for( j = 0; j < nMaxGroup; j ++ )
	{
		CGeoMObject **pGeoMObject = m_Group[j].m_pGeoMObject;
		int		nMaxGeoMObject = m_Group[j].m_nMaxGeoMObject;
		if( nMaxGeoMObject == 0 )
		{
			Error( "SaveNormMesh : MaxGeoMObject가 0이다" );
			continue;
		}
		fwrite( &nMaxGeoMObject, 4, 1, fp );		// 지오메트리 갯수

		// 세이브 하기 직전에 본은 빼고 차례대로 일련번호를 붙인다. 파일에 저장되는 순서다.
		nIdx = 0;
		for( i = 0; i < nMaxGeoMObject; i ++ )
		{
			pGeo = pGeoMObject[i];
			// 메쉬가 있는놈만 저장(바이페드는 없다)
			if( pGeo->m_pMesh )		
				pGeo->m_nIdx = nIdx ++;
		}

		// 지오메트리 개수 만큼 기록한다.
		for( i = 0; i < nMaxGeoMObject; i ++ )
		{
			pGeo = pGeoMObject[i];
			// 메쉬가 있는놈만 저장(바이페드는 없다)
			if( pGeo->m_pMesh )		
			{
				int type = pGeo->GetType();
				if( _strcmpi(pGeo->m_strName, "light") == 0 )
					type |= 0x80000000;
				fwrite( &type, 4, 1, fp );			// 오브젝트 타입
	#ifdef	__XVS
				temp = 0;	fwrite( &temp, 4, 1, fp );	// max use bone
	#endif
				fwrite( &pGeo->m_nID, 4, 1, fp );	// Object ID
				// 부모 인덱스 저장.
				if( pGeo->m_pParent )
				{
					// 부모가 본이면 본계층의 인덱스가 저장되고 
					// 부모가 일반이면 일반오브젝트의 인덱스가 저장된다.
					if( pGeo->m_pParent->m_nIdx == -1 )
						Error( "CAse::SaveNormMesh - 부모가 있는데 인덱스는 -1이다." );
					fwrite( &pGeo->m_pParent->m_nIdx, 4, 1, fp );		// 부모가 있을땐 부모의 Idx저장, 없으면 -1.
					type = pGeo->m_pParent->GetType();
					fwrite( &type, 4, 1, fp );		// 부모의 타입
				} else
				{
					int temp = -1;
					fwrite( &temp, 4, 1, fp );		// 부모가 없는건 -1저장
				}
				fwrite( &(pGeo->m_matLocalTM), sizeof(D3DXMATRIX), 1, fp );	// NODE_TM저장

				pGeo->SaveMesh( fp );		// 메쉬부 저장
				if( m_nFrameMax > 0 )	
					pGeo->SaveTMAni( fp );		// 에니메이션부 저장, maxframe이 0이면 아예 들어가지 않는다.
			}
		}
	} // GROUP

	// 속성부 저장. - Version 21부터
	if( m_nFrameMax > 0 )
	{
		int nAttr = FALSE;
		for( i = 0; i < m_nFrameMax; i ++ )
		{
			if( m_pAttr[i].m_dwAttr )
			{
				nAttr = TRUE;		// 속성이 하나라도 들어가있으면 속성부 저장함.
				break;
			}
		}
		if( nAttr )
		{
			fwrite( &m_nFrameMax, 4, 1, fp );		// 속성부 저장이 있다는 신호.
			fwrite( m_pAttr, sizeof(MOTION_ATTR) * m_nFrameMax, 1, fp );
		}
	}
	
	fclose(fp);

#ifdef	__COLA
	{
		CString	strTitle, strPath, strName;
		strPath  = GetFilePath( szFileName );		// path
		strTitle = GetFileTitle( szFileName );		// test.ase -> test
		strName  = strPath + strTitle + ".inf";
		SaveModelEx( strName );		// ASE 확장정보
	}
#endif
	
	if( m_pd3dDevice )
		Message( "%s Save Complete.", szFileName );

	return SUCCESS;
}

//
//	바운드박스 vMin,vMax를 기준으로 반지름을 돌려줌
//
//float	CAse::GetRadius( void )
//{
//	D3DXVECTOR3 vLen = m_vMax - m_vMin;
//	float	fLen = D3DXVec3Length( &vLen );
//	return fLen/2;
//}


//
//	CAse가 가진 모든 GEOMOBJECT중에서 이름으로 찾음
//
CGeoMObject*	CAse :: FindGeoM( GROUP *pGroup, LPCTSTR szName )
{
	CGeoMObject	*pGEO, *pObj;
	int		i;

	if( m_pPath )
	{
		if( _strcmpi( m_pPath->m_strName, szName ) == 0 )		// GeomObject검사하깆ㄴ에 Path오브젝트를 먼저 검사.
			return m_pPath;
	}

	for( i = 0; i < pGroup->m_nMaxGeoMObject; i ++ )
	{
		pGEO = pGroup->m_pGeoMObject[i];
		pObj = pGEO->TreeFindNode( szName );
		if( pObj )
			return pObj;
	}

	return NULL;
}

// 바이페드 트리 오브젝트의 포인터를 리턴
// 본을 찾을땐 Group[0]에서만 찾는다.
CGeoMObject *CAse :: GetBiped( void )
{
	int		i;
	CGeoMObject *pObj = NULL;
	for( i = 0; i < m_Group[0].m_nMaxGeoMObject; i ++ )
	{
		pObj = m_Group[0].m_pGeoMObject[i];
		if( pObj->GetType() == GMT_BONE )	
			return pObj;
	}

	return NULL;
}


//
//  .chr (bone + 기타정보) 파일 생성
//
int		CAse :: SaveCHR( LPCTSTR szFileName )
{
#ifndef __VIEWER_ONLY
	FILE *fp = fopen( szFileName, "wb" );
	if( fp == NULL )
	{
		Error( "%s : 저장실패.  Check Out을 했는지 확인해 볼 것!", szFileName );
		return FAIL;
	}

	CGeoMObject	*pBiped = GetBiped();
	pBiped->WriteBON( fp );				// 헤더 및 bone의 계층 구조 write

	fwrite( &m_bSendVS, 4, 1, fp );		// 본개수가 MAX_VS_BONE보다 작아서 한번에 VS로 전송할수 있는가
	
	D3DXMATRIX	mReserved;
	fwrite( &m_mLocalRH,	sizeof(D3DXMATRIX), 1, fp );
	fwrite( &m_mLocalShield,sizeof(D3DXMATRIX), 1, fp );
	fwrite( &m_mLocalKnuckle,	sizeof(D3DXMATRIX), 1, fp );	// 

	fwrite( m_vEvent, sizeof(D3DXVECTOR3) * MAX_MDL_EVENT, 1, fp );
	int i;
	int nTemp = -1;
	for( i = 0; i < MAX_MDL_EVENT; i ++ )
	{
		if( m_pEvent[i] )
			fwrite( &m_pEvent[i]->m_nIdx, 4, 1, fp );
		else
			fwrite( &nTemp, 4, 1, fp );

	}

	// version 7 
	fwrite( &m_mLocalLH,	sizeof(D3DXMATRIX), 1, fp );
	

	fclose(fp);

	if( m_pd3dDevice )
		Message( "%s\r\n뼈대 %s를 저장함. 뼈대개수:%d", szFileName, pBiped->m_strName, m_nMaxBoneNode );
#endif // not viewer	
	return SUCCESS;
}

int		CAse :: SaveANI( LPCTSTR szFileName )
{
	char	reserved[32];
	int		nVer = 10;					// VER_MOTION
	int		nSerial = xRand();			// 32비트형 시리얼 번호
	CGeoMObject *pBone = GetBiped();
	
	if( pBone == NULL )	
	{
		Error( "CAse::SaveANI : 본이 없다. %s", szFileName );
		return FAIL;
	}

	FILE *fp = fopen( szFileName, "wb" );
	if( fp == NULL )	
	{
		Error( "%s : 저장실패.  Check Out을 했는지 확인해 볼것!.", szFileName );
		return FAIL;
	}

//	int		nNumNode = pBone->GetLength();
	int		nNumNode = m_nMaxBoneNode;
	
	// header
	memset( reserved, 0, 32 );
	fwrite( &nVer, 4, 1, fp );
	fwrite( &nSerial, 4, 1, fp );
	fwrite( &m_fPerSlerp, sizeof(float), 1, fp );
	fwrite( reserved, 32, 1, fp );		// 여유공간
	fwrite( &nNumNode, sizeof(int), 1, fp );
	fwrite( &m_nFrameMax, sizeof(int), 1, fp );
	
	int		nPath;
	if( m_pPath )	// path가 있다면 그것을 먼저 저장.
	{
		nPath = 1;
		fwrite( &nPath, 4, 1, fp );		// 패스가 있다는것을 알림
		m_pPath->SavePathRecv( fp );	// path의 m_vPos를 저장.
	} else
	{
		nPath = 0;
		fwrite( &nPath, 4, 1, fp );		// 
	}

	pBone->WriteBoneInfo( fp );		// 뼈대구성 정보
	
	pBone->SaveANI( fp );
	
	// 속성부 저장.
	fwrite( m_pAttr, sizeof(MOTION_ATTR) * m_nFrameMax, 1, fp );

	fwrite( &m_nMaxEvent, 4, 1, fp );	// 이벤트 좌표 저장
	if( m_nMaxEvent > 0 )
		fwrite( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1, fp );

	fclose(fp);
	if( m_pd3dDevice )
		Message( "%s\r\n뼈대 %s의 애니메이션을 %d frame 저장함. 뼈대개수:%d", szFileName, pBone->m_strName, m_nFrameMax, m_nMaxBoneNode );
#ifdef	__COLA
	{
		CString	strTitle, strPath, strName;
		strPath  = GetFilePath( szFileName );		// path
		strTitle = GetFileTitle( szFileName );		// test.ase -> test
		strName  = strPath + strTitle + ".inf";
		SaveModelEx( strName );		// ASE 확장정보
	}
#endif

	return SUCCESS;
}


void	CAse :: ClearNormal( void )
{
	int		i;

	for( int j = 0; j < MAX_GROUP; j ++ )
	{
		for( i = 0; i < m_Group[j].m_nMaxGeoMObject; i++ )
		{
			CGeoMObject	*pObj = m_Group[j].m_pGeoMObject[i];

			pObj->ClearNormal();
		}
	}
}

// Group0에서만 검사한다. - 좀더 빠른 검색을 위해 LOD2에서 검사하는것도 괜찮을듯.
D3DXVECTOR3 *CAse::IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir )
{
	int		i;
	D3DXVECTOR3 *v;
	for( i = 0; i < m_Group[0].m_nMaxGeoMObject; i ++ )
	{
		CGeoMObject *pObj = m_Group[0].m_pGeoMObject[i];
		if( v = pObj->IntersectRayVertex( pOut, vRayOrig, vRayDir ) )
			return v;
	}

	return NULL;
}


//
// 각 오브젝트별(or 매터리얼블럭별)로 사용하는 최소의본리스트만 추려내고 그에따라 참조본인덱스를
// 재 인덱싱 한다.
//
void	CAse :: ReindexingBoneIdx( void )
{
	int		i, j;
	CASEMesh	*pMesh;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GROUP *pGroup = &m_Group[j];
		CGeoMObject **ppGeo = pGroup->m_pGeoMObject;
		for( i = 0; i < pGroup->m_nMaxGeoMObject;  i++ )
		{
			pMesh = (*ppGeo)->m_pMesh;
			ppGeo++;
			if( pMesh == NULL )		continue;
			pMesh->ReindexingBoneIdx(j);
		}
	}
}










