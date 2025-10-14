#include "stdafx.h"

#include "ASE.h"

extern	float	g_fScale;
extern  char	g_strError[];

//#define		__DIP

CASEMesh :: CASEMesh()
{
	Init();
}

CASEMesh :: ~CASEMesh()
{
	Destroy();
}

void	CASEMesh :: Init( void )
{
	m_nMaxVertexBuffer = 0;
	m_nMaxVertexList = 0;
	m_nMaxFaceList = 0;

	m_pPhysiqueVertex = NULL;

	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pVertexList = NULL;
	m_pFaceList = NULL;

	m_ptempIVB = NULL;
	m_ptempIIB = NULL;
	m_pIVB = NULL;
	m_pIB = NULL;
	m_pIIB = NULL;
	m_nMaxIVB = 0;
	m_nMaxIB = 0;
	
	m_nMaxVertexBufferBlock = 0;
	m_pVertexBufferBlock = NULL;
	m_pMtrlBlkTexture = NULL;

	m_pVB = NULL;
	m_pIVertexBuffer = NULL;
	m_pIIndexBuffer = NULL;

	m_nMaxTVertices = 0;
	m_pTVertices = NULL;
	m_nMaterialRef = 0;

	m_vMin.x = m_vMin.y = m_vMin.z = 0;
	m_vMax.x = m_vMax.y = m_vMax.z = 0;

	m_nWidth = m_nHeight = m_nDepth = 0;

	m_bOpacity = FALSE;
	m_bRigid = FALSE;
	m_bBump = FALSE;
	m_pd3dDevice = NULL;
	m_nMaxUseBone = 0;
	memset( m_UseBone, 0, sizeof(int) * MAX_VS_BONE );
}

void	CASEMesh :: Destroy( void  )
{
	SAFE_DELETE_ARRAY( m_pPhysiqueVertex );
	SAFE_DELETE_ARRAY( m_pVertexBuffer );
	SAFE_DELETE_ARRAY( m_pIndexBuffer );
	SAFE_DELETE_ARRAY( m_pVertexList );
	SAFE_DELETE_ARRAY( m_pVertexBufferBlock );
	SAFE_DELETE_ARRAY( m_pFaceList );
	SAFE_DELETE_ARRAY( m_pTVertices );
	SAFE_DELETE_ARRAY( m_pIVB );
	SAFE_DELETE_ARRAY( m_pIB );
	SAFE_DELETE_ARRAY( m_pIIB );
	SAFE_DELETE_ARRAY( m_ptempIVB );
	SAFE_DELETE_ARRAY( m_ptempIIB );

	DeleteDeviceObjects();

	Init();
}

HRESULT CASEMesh :: RestoreDeviceObjects()
{
	if( m_pd3dDevice == NULL )	return E_FAIL;
	if( m_pVB )		return S_OK;	// 이미 버텍스버퍼가 할당되어 있으면 걍 리턴해야 한다

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	CreateVB( m_pd3dDevice );
	SendVertexBuffer( TRUE );		// AGP버텍스 버퍼로 전송

	return S_OK;
}

HRESULT CASEMesh :: InvalidateDeviceObjects()
{
	return DeleteDeviceObjects();
}

HRESULT CASEMesh :: DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIIndexBuffer );
	SAFE_RELEASE( m_pIVertexBuffer );
	return S_OK;
}


// Texture Vertex list를 읽음
int		CASEMesh :: Read_MESH_TVERTLIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nTempMax = 0;
	float	u, v, w;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_TVERT" ) == 0 )
		{
			scan->GetToken();		// t vertex number skip
			u = scan->GetFloat();	// u
			v = scan->GetFloat();	// v
			w = scan->GetFloat();	// w
			m_pTVertices[ nTempMax ].u = u;
			m_pTVertices[ nTempMax ].v = 1.0f - v;
			m_pTVertices[ nTempMax ].w = w;

			nTempMax ++;
		}
	}

	return 1;
}

// 텍스쳐 페이스 읽음.
int		CASEMesh :: Read_MESH_TFACELIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nIdxA, nIdxB, nIdxC;
	int		nVertex = 0;
	int		nNumFace = 0;

	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();
	
	int		nMaxMtrl = nMaxMaterial;
	int		nMaxSubMtrl = 0;
	ASE_MATERIAL	*pMtrl = NULL;
	SUB_MATERIAL	*pSubMtrl = NULL;
	
	if( pMaterialList )
	{
		pMtrl = &pMaterialList[ m_nMaterialRef ];
		nMaxSubMtrl = pMtrl->m_nMaxSubMaterial;
		// 서브 매터리얼이 있으면 그것을 max로 쓴다.  없으면 메인매터리얼갯수를 max로 쓴다
		if( nMaxSubMtrl )
			nMaxMtrl = nMaxSubMtrl;
	}
	
	FLOAT fTileU, fTileV;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_TFACE" ) == 0 )
		{
			MESH_FACE	*pFace;
			nNumFace = scan->GetNumber();	// T face number:
			nIdxA = scan->GetNumber();
			nIdxB = scan->GetNumber();
			nIdxC = scan->GetNumber();
			pFace = &m_pFaceList[ nNumFace ];
//			pFace->ta = nIdxA;
//			pFace->tb = nIdxC;
//			pFace->tc = nIdxB;

			fTileU = fTileV = 1.0f;		// 디폴트

			if( pMtrl )
			{
				if( pMtrl->m_nMaxSubMaterial )
				{
					fTileU = pMtrl->m_pSubMaterial[ pFace->m_nMaterialID ].m_fTilingU;
					fTileV = pMtrl->m_pSubMaterial[ pFace->m_nMaterialID ].m_fTilingV;
				} else
				{
					fTileU = pMtrl->m_fTilingU;
					fTileU = pMtrl->m_fTilingV;
				}
			}

			m_pVertexBuffer[ pFace->a ].tu = m_pTVertices[ nIdxA ].u * fTileU;
			m_pVertexBuffer[ pFace->a ].tv = m_pTVertices[ nIdxA ].v * fTileV;
			m_pVertexBuffer[ pFace->b ].tu = m_pTVertices[ nIdxC ].u * fTileU;
			m_pVertexBuffer[ pFace->b ].tv = m_pTVertices[ nIdxC ].v * fTileV;
			m_pVertexBuffer[ pFace->c ].tu = m_pTVertices[ nIdxB ].u * fTileU;
			m_pVertexBuffer[ pFace->c ].tv = m_pTVertices[ nIdxB ].v * fTileV;
		}
	}

	return 1;
}

// 매핑채널읽음.
int		CASEMesh :: Read_MESH_MAPPING_CHANNEL( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nVertex = 0;
	int		nNumFace = 0;
	
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
	}
	
	return 1;
}


// color Vertex list를 읽음
int		CASEMesh :: Read_MESH_CVERTLIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nTempMax = 0;
	int		r, g, b;
	DWORD	dwColor;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_VERTCOL" ) == 0 )
		{
			scan->GetToken();		// c vertex number skip
			r = (int)(scan->GetFloat() * 255);	// r
			g = (int)(scan->GetFloat() * 255);	// g
			b = (int)(scan->GetFloat() * 255);	// b
			dwColor = (r << 16) | (g << 8) | b;
			m_pCVertices[ nTempMax ] = dwColor;

			nTempMax ++;
		}
	}

	return 1;
}

// color 페이스 읽음.
int		CASEMesh :: Read_MESH_CFACELIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nIdxA, nIdxB, nIdxC;
	int		nVertex = 0;
	int		nNumFace = 0;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_CFACE" ) == 0 )
		{
			nNumFace = scan->GetNumber();	// C face number:
			nIdxA = scan->GetNumber();
			nIdxB = scan->GetNumber();
			nIdxC = scan->GetNumber();
//#ifndef	__XVS
/*			m_pVertexBuffer[ m_pFaceList[ nNumFace ].a ].color = m_pCVertices[ nIdxA ];
			m_pVertexBuffer[ m_pFaceList[ nNumFace ].a ].color = m_pCVertices[ nIdxA ];
			m_pVertexBuffer[ m_pFaceList[ nNumFace ].b ].color = m_pCVertices[ nIdxC ];
			m_pVertexBuffer[ m_pFaceList[ nNumFace ].b ].color = m_pCVertices[ nIdxC ];
			m_pVertexBuffer[ m_pFaceList[ nNumFace ].c ].color = m_pCVertices[ nIdxB ];
			m_pVertexBuffer[ m_pFaceList[ nNumFace ].c ].color = m_pCVertices[ nIdxB ];*/
//#endif
		}
	}

	return 1;
}



// Vertex List 읽음
int		CASEMesh :: Read_MESH_VERTEX_LIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nMax = 0;
	float	x, y, z;
	D3DXVECTOR3		vLocal;
	D3DXMATRIX		vInverse;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_VERTEX" ) == 0 )
		{
			scan->GetToken();		// vertex number skip
			x = scan->GetFloat();	// x
			y = scan->GetFloat();	// y
			z = scan->GetFloat();	// z]

			x *= g_fScale;
			y *= g_fScale;
			z *= g_fScale;
			// 
			m_pVertexList[ nMax ].m_vWorldPos.x = x;
			m_pVertexList[ nMax ].m_vWorldPos.y = z;
			m_pVertexList[ nMax ].m_vWorldPos.z = y;
			m_pVertexList[ nMax ].m_vLocalPos.x = x;
			m_pVertexList[ nMax ].m_vLocalPos.y = z;
			m_pVertexList[ nMax ].m_vLocalPos.z = y;

			// 월드좌표계로 되어 있는 각 버텍스값을 현재오브젝트의 피봇 중심으로 바꾼다.
			// 월드버텍스 좌표에서 피봇좌표를 빼서 피봇중심의 로컬좌표로 변환 했다.
			// local = WorldVertex * inv(NodeTM);
//			D3DXVec3TransformCoord( &vLocal, &g_pVertexList[ nMax ].m_vWorldPos, &m_matInverseTM );
//			g_pVertexList[ nMax ].m_vLocalPos = vLocal;
			nMax ++;
		}
	}
	m_nMaxVertexList = nMax;

	return 1;
}

int		CASEMesh :: Read_MESH_FACE_LIST( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nIdxA, nIdxB, nIdxC;
	int		nVertex = 0;
	int		nFace = 0;
	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_FACE" ) == 0 )
		{
			scan->GetToken();	scan->GetToken();	// face number:
			scan->GetToken();	scan->GetToken();	// A:
			nIdxA = scan->GetNumber();
			scan->GetToken();	scan->GetToken();	// B:
			nIdxB = scan->GetNumber();
			scan->GetToken();	scan->GetToken();	// C:
			nIdxC = scan->GetNumber();
			m_pFaceList[ nFace ].a = nVertex;	m_pIndexBuffer[ nVertex ].nIdx = nIdxA;		m_pVertexBuffer[ nVertex++ ].position = m_pVertexList[ nIdxA ].m_vWorldPos;
			m_pFaceList[ nFace ].b = nVertex;	m_pIndexBuffer[ nVertex ].nIdx = nIdxC;		m_pVertexBuffer[ nVertex++ ].position = m_pVertexList[ nIdxC ].m_vWorldPos;
			m_pFaceList[ nFace ].c = nVertex;	m_pIndexBuffer[ nVertex ].nIdx = nIdxB;		m_pVertexBuffer[ nVertex++ ].position = m_pVertexList[ nIdxB ].m_vWorldPos;
			m_pFaceList[ nFace ].v1 = nIdxA;
			m_pFaceList[ nFace ].v2 = nIdxC;
			m_pFaceList[ nFace ].v3 = nIdxB;
//			m_pFaceList[ nFace ].a = nIdxA;
//			m_pFaceList[ nFace ].b = nIdxC;
//			m_pFaceList[ nFace ].c = nIdxB;
		}
		// 이것까지 읽어야 한줄이 끝난다.
		if( strcmp( scan->token, "MESH_MTLID" ) == 0 )
		{
//			int		nMaxMaterial = nMaxMaterial;
			if( nMaxMaterial == 0 )		nMaxMaterial = 1;
//			m_pFaceList[ nFace ].m_nMaterialID = scan->GetNumber() % nMaxMaterial;		// sub material id 읽음
			m_pFaceList[ nFace ].m_nMaterialID = scan->GetNumber();			// sub material id 읽음
			nFace ++;
		}
	}
	m_nMaxVertexBuffer = nVertex;		// face * 3 된 버텍스리스트의 최종 갯수를 넣음.
	m_nMaxFaceList = nFace;

	return 1;
}


#define		MAX_NORMAL	(1024 * 16)

static D3DXVECTOR3	s_vNormal[ MAX_NORMAL ];
static int			s_nNormalCnt[ MAX_NORMAL ];	

int		CASEMesh :: CalcNormal( void )
{
	int		nNumBlock = 0;
	D3DXVECTOR3	normal;
	int		nIdxNormal = 0;
	int		nVertex = 0;
	int		nFace = 0;
//	int		nNumVertex;
	int		i;
	D3DXVECTOR3	v1, v2, v3, vA, vB;
	int		nIdxA, nIdxB, nIdxC;

	memset( s_vNormal, 0, MAX_NORMAL * sizeof(D3DXVECTOR3) );
	memset( s_nNormalCnt, 0, MAX_NORMAL * sizeof(int) );

	for( i = 0; i < m_nMaxFaceList; i ++ )
	{
		// face를 구성하는 버텍스 3개 꺼냄
		nIdxA = m_pFaceList[i].v1;
		nIdxB = m_pFaceList[i].v2;
		nIdxC = m_pFaceList[i].v3;
		v1 = m_pVertexList[ nIdxA ].m_vLocalPos;
		v2 = m_pVertexList[ nIdxB ].m_vLocalPos;
		v3 = m_pVertexList[ nIdxC ].m_vLocalPos;
//		D3DXVec3TransformCoord( &v1, &v1, &m_pGeoMObject->m_matLocalTM );
//		D3DXVec3TransformCoord( &v2, &v2, &m_pGeoMObject->m_matLocalTM );
//		D3DXVec3TransformCoord( &v3, &v3, &m_pGeoMObject->m_matLocalTM );
		
		// 노말 구함
		vA = v2 - v1;
		vB = v3 - v1;
		D3DXVec3Cross( &normal, &vA, &vB );
		D3DXVec3Normalize( &normal, &normal );		// 최종 버텍스 노말 완성
		
		m_pFaceList[i].n = normal;		// face list에 face normal 넣음

		if( nIdxA >= MAX_NORMAL || nIdxB >= MAX_NORMAL || nIdxC >= MAX_NORMAL )
			Error( "nIdxABC(%d %d %d)가 MAX_NORMAL의 범위를 넘어섰다.", nIdxA, nIdxB, nIdxC );

		// this face에서 사용하는 버텍스 a, b, c
		// 이 버텍스노말에 자기가 속한 face의 normal을 계속 누적시킨다.
		s_vNormal[ nIdxA ] += normal;
		s_vNormal[ nIdxB ] += normal;
		s_vNormal[ nIdxC ] += normal;

		s_nNormalCnt[ nIdxA ] ++;
		s_nNormalCnt[ nIdxB ] ++;
		s_nNormalCnt[ nIdxC ] ++;
	}

	// 누적된 버텍스 노말을 소속된 면갯수로 나눠서 노말라이즈 한다.
	for( i = 0; i < m_nMaxVertexList; i ++ )
	{
		normal = s_vNormal[i] / (float)s_nNormalCnt[i];
//		D3DXVec3TransformCoord( &normal, &normal, &m_pGeoMObject->m_matLocalTM );
		D3DXVec3Normalize( &s_vNormal[i], &normal );		// 최종 버텍스 노말 완성
		m_pVertexList[i].m_vNormal = s_vNormal[i];
		m_pVertexList[i].m_vWorldNormal = s_vNormal[i];
	}

	// 다시 버텍스 버퍼에 풀어서 옮겨담음
	for( i = 0; i < m_nMaxFaceList; i ++ )
	{
		nIdxA = m_pFaceList[i].v1;
		nIdxB = m_pFaceList[i].v2;
		nIdxC = m_pFaceList[i].v3;
		m_pIndexBuffer[i*3+0].normal = s_vNormal[ nIdxA ];
		m_pIndexBuffer[i*3+1].normal = s_vNormal[ nIdxB ];
		m_pIndexBuffer[i*3+2].normal = s_vNormal[ nIdxC ];
		m_pVertexBuffer[i*3+0].normal = s_vNormal[ nIdxA ];
		m_pVertexBuffer[i*3+1].normal = s_vNormal[ nIdxB ];
		m_pVertexBuffer[i*3+2].normal = s_vNormal[ nIdxC ];
	}

	
/*
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_VERTEXNORMAL" ) == 0 )
		{
			nNumVertex = scan->GetToken();	// vertex number
			normal[nIdxNormal].x = scan->GetFloat();		// normal x
			normal[nIdxNormal].y = scan->GetFloat();		// normal z
			normal[nIdxNormal].z = scan->GetFloat();		// normal y
			nIdxNormal ++;
		}
		// ASE의 Face Normal값은 순서대로 표기되어 있으므로 버텍스노말값도 순서대로 넣으면 된다.
		if( nIdxNormal == 3 )		// 각 버텍스들의 노말값 세팅.
		{
			m_pIndexBuffer[ nVertex++ ].normal = D3DXVECTOR3( normal[0] );
			m_pIndexBuffer[ nVertex++ ].normal = D3DXVECTOR3( normal[2] );
			m_pIndexBuffer[ nVertex++ ].normal = D3DXVECTOR3( normal[1] );
			m_pVertexBuffer[ nVertex-3 ].normal = D3DXVECTOR3( normal[0] );
			m_pVertexBuffer[ nVertex-2 ].normal = D3DXVECTOR3( normal[2] );
			m_pVertexBuffer[ nVertex-1 ].normal = D3DXVECTOR3( normal[1] );
			nIdxNormal = 0;
			nFace ++;
		}
	}
*/
	return 1;
}


// *MESH{} 읽음
int		CASEMesh :: Read_MESH( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nNumVertex = 0;
	int		nNumFaces = 0;
	int		nNumTVertex = 0;
	int		nNumTVFaces = 0;
	int		nNumCVertex = 0;
	int		nNumCVFaces = 0;
	
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "MESH_NUMVERTEX" ) == 0 )
		{
			nNumVertex = scan->GetNumber();			// 버텍스 갯수 읽음
			if( nNumVertex == 0 )	return FAIL;

			m_pVertexList = new SKIN_VERTEX[ nNumVertex ];	// 버텍스 리스트
			memset( m_pVertexList, 0, sizeof(SKIN_VERTEX) * nNumVertex );
		}
		// 페이스 갯수 읽음
		if( strcmp( scan->token, "MESH_NUMFACES" ) == 0 )		
		{
			nNumFaces = scan->GetNumber();
			if( nNumFaces == 0 )	return FAIL;
			// DrawPrimitive를 사용하기 위해 NumFace * 3크기만큼의 버텍스버퍼를 만든다.
			{
				m_pVertexBuffer = new FVF_ASEMESH[ nNumFaces * 3 ];	// numFace * 3의 크기 할당
				memset( m_pVertexBuffer, 0, sizeof(FVF_ASEMESH) * (nNumFaces * 3) );
#ifndef	__XVS
				for( int i = 0; i < nNumFaces * 3; i ++ )
					m_pVertexBuffer[i].color = 0xffffffff;				// 버텍스 칼라 흰색으로 초기화
#endif
				m_nMaxFaceList = nNumFaces;
				m_pFaceList = new MESH_FACE[ nNumFaces ];		// face list할당
				m_pIndexBuffer = new INDEX_BUFFER[ nNumFaces * 3 ];
			}
		}
		// 텍스쳐 버텍스 갯수 읽음
		if( strcmp( scan->token, "MESH_NUMTVERTEX" ) == 0 )		
		{
			nNumTVertex = scan->GetNumber();
			if( nNumTVertex )	
			{
				m_pTVertices = new TEXTURE_COORDI[ nNumTVertex ];
				m_nMaxTVertices = nNumTVertex;		// 최종 텍스쳐 버텍스 갯수
			}
		}
		if( strcmp( scan->token, "MESH_NUMTVFACES" ) == 0 )		
		{
			nNumTVFaces = scan->GetNumber();
		}
		// 컬러 버텍스 갯수 읽음
		if( strcmp( scan->token, "MESH_NUMCVERTEX" ) == 0 )		
		{
			nNumCVertex = scan->GetNumber();
			if( nNumCVertex )
			{
				m_pCVertices = new DWORD[ nNumCVertex ];
				m_nMaxCVertices = nNumCVertex;		// 최종 컬러 버텍스 갯수
			}
		}
		if( strcmp( scan->token, "MESH_NUMCVFACES" ) == 0 )		
		{
			nNumCVFaces = scan->GetNumber();
		}
		// 버텍스 리스트 읽음
		if( strcmp( scan->token, "MESH_VERTEX_LIST" ) == 0 && nNumVertex > 0 )	Read_MESH_VERTEX_LIST( scan );
		if( strcmp( scan->token, "MESH_FACE_LIST" ) == 0 && nNumFaces > 0 )		Read_MESH_FACE_LIST( scan );
		if( strcmp( scan->token, "MESH_TVERTLIST" ) == 0 && nNumTVertex > 0 )	Read_MESH_TVERTLIST( scan );
		if( strcmp( scan->token, "MESH_TFACELIST" ) == 0 && nNumTVFaces > 0 )	Read_MESH_TFACELIST( scan );
		if( strcmp( scan->token, "MESH_CVERTLIST" ) == 0 && nNumCVertex > 0 )	Read_MESH_CVERTLIST( scan );
		if( strcmp( scan->token, "MESH_CFACELIST" ) == 0 && nNumCVFaces > 0 )	Read_MESH_CFACELIST( scan );
		if( strcmp( scan->token, "MESH_MAPPINGCHANNEL" ) == 0 )
		{
//			Message( "매핑 채널이 설정되어 있어 매핑이 제대로 출력되지 않을 수 있습니다. 매핑채널을 삭제해주세요 %s", m_pGeoMObject->m_strName );
			Read_MESH_MAPPING_CHANNEL( scan );
		}
//		if( strcmp( scan->token, "MESH_NORMALS" ) == 0 && nNumVertex > 0 )		Read_MESH_NORMALS( scan );		// 노말은 따로 계산함,.
	}

//	if( m_pTempVertices )
//	{
//		delete []m_pTempVertices;		// 더이상 필요치 않다.
//		m_pTempVertices = NULL;
//	}
	return 1;
}

// PHYSIQUE데이타 읽음.
int		CASEMesh :: Read_PHYSIQUE( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nNumVertex = 0;
	char	*szBone, szBuff[256];
	PHYSIQUE_BLEND	vBlend;
	
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		// Number of rigid vertex
		if( strcmp( scan->token, "PHYSIQUE_NUMVERTEXASSIGNMENT" ) == 0 )
		{
			int		nMaxPhysique;
			nMaxPhysique = scan->GetNumber();			// 피지크 버텍스 갯수 읽음.
			if( nMaxPhysique != m_nMaxVertexList )
				Error( "%s오브젝트의 버텍스개수(%d)와 피지크개수(%d)가 다르다.", m_pGeoMObject->m_strName, m_nMaxVertexList, nMaxPhysique );
//				MessageBox( NULL, "버텍스갯수와 피지크갯수가 다르다", NULL, MB_OK );
			m_pPhysiqueVertex = new	PHYSIQUE[ m_nMaxVertexList ];
			memset( m_pPhysiqueVertex, 0, sizeof(PHYSIQUE) * m_nMaxVertexList );
		}

		// Blend vertex
		if( strcmp( scan->token, "PHYSIQUE_BLENDED_RIGIDTYPE_LIST" ) == 0 )
		{
			int		numNode = 0, nNumReal = 0;
			float	fWeight;
			int		i, j;

			scan->GetToken();		// 버텍스 번호 스킵
			scan->GetToken();		// {
			scan->GetToken();		// *
			scan->GetToken();		// PHYSIQUE_VERTEXASSIGNMENT_NUMNODE
			numNode = scan->GetNumber();	// Blend node 갯수
			scan->GetToken();		// *
			scan->GetToken();		// PHYSIQUE_VERTEXASSIGNMENT_NODELIST
			scan->GetToken();		// {
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex = new PHYSIQUE_BLEND[ numNode ];		// 실제 쓰일땐 numNode보다 작을수도 있다.
			for( i = 0; i < numNode; i++ )
			{
				scan->GetToken();		// *
				scan->GetToken();		// PHYSIQUE_VERTEXASSIGNMENT_NODE
				scan->GetToken();		// 노드 번호 스킵
				m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[nNumReal].m_nWeight = fWeight = scan->GetFloat();
				scan->GetToken();		// "Bip01"

				// 발가락 손가락 뼈대의 이름을 바꿈.
				szBone = m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[nNumReal].m_strBoneName;
				memset( szBone, 0, 32 );
				strcpy( szBone, scan->token );	// 일단 Bip01 L(R) Finger를 전부 카피
				memset( szBuff, 0, 256 );
				strcpy( szBuff, szBone );
				_strlwr( szBuff );
					
				if( strncmp( szBuff+8, "finger", 6 ) == 0 )	// 손가락뼈에 연결된 버텍스가 나오면 손뼈로 바꿈
				{
					strcpy( szBone+8, "Hand" );		// Bip01 L(R) Hand로 변환
					szBone[8+strlen("Hand")] = 0;	// 끝에 NULL
				}
				if( strncmp( szBuff+8, "toe", 3 ) == 0 )
				{
					strcpy( szBone+8, "Foot" );		// Bip01 L(R) Hand로 변환
					szBone[8+strlen("Foot")] = 0;	// 끝에 NULL
				}

				m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[nNumReal].m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.
				// 이미 나온건지 검사.
				for( j = 0; j < nNumReal; j ++ )
				{	// 이미 한번 나왔던 본이면...
					if( strcmp( scan->token, m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[j].m_strBoneName ) == 0 )
					{
						m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[j].m_nWeight += fWeight;	// 그본에다가 weight를 더하고 
						goto LP1;	// 리스트를 추가하진 않고 다음으로...
					}
				}
				nNumReal ++;
LP1:	
				int a; a= 0;
			}
#ifdef	__X2LINK
//			if( nNumReal > 2 )		// 3링크 이상이면 플래그 설정하고 non-blend로 처리
//				m_bRigid = TRUE;
			if( nNumReal == 1 )		
			{
				nNumReal = 2;		// 최하 2link
				m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[0].m_nWeight = 1.0f;	// 0.7 0.3으로 나뉘었는데 합쳐졌으니 0.7이 아니라 1.0이되야 하는게 맞다.
				m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_nWeight = 0.0f;	// [0]의 내용을 1에 복사.
				m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_pBone   = NULL;
				strcpy( m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_strBoneName, 
					    m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[0].m_strBoneName );
			}
#endif
			PHYSIQUE	*p = &m_pPhysiqueVertex[ nNumVertex ];
			// 가장 영향이 큰 본을 사용하도록 소트한다.
			for ( i = 0; i < nNumReal-1; i++ )
			{
				for( j = i+1; j < nNumReal; j ++ )
				{
					if( p->m_pBlendVertex[i].m_nWeight < p->m_pBlendVertex[j].m_nWeight )
					{
						vBlend = p->m_pBlendVertex[j];	// 바꿈.
						p->m_pBlendVertex[j] = p->m_pBlendVertex[i];
						p->m_pBlendVertex[i] = vBlend;
					}
				}
			}
			if( nNumReal > 2 )
			{
				if( p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight < 1.0f )
				{
					float fSub = 1.0f - (p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight);
					p->m_pBlendVertex[0].m_nWeight += (fSub / 2);
					p->m_pBlendVertex[1].m_nWeight += (fSub / 2);

				}
			}
			if( nNumReal >= 2 )
			{
				BOOL	bZeroAll = TRUE;
				for( i = 0; i < nNumReal; i ++ )
				{
					if( p->m_pBlendVertex[i].m_nWeight > 0.0f )
						bZeroAll = FALSE;
				}
				if( bZeroAll == TRUE )
				{
					Message( "경고 Read_PHYSIQUE : 오브젝트%s.  %s, %s에 링크된 버텍스의 Weight가 둘다 0 이다. ", 
								m_pGeoMObject->m_strName, p->m_pBlendVertex[0].m_strBoneName, p->m_pBlendVertex[1].m_strBoneName );
					for( i = 0; i < nNumReal; i ++ )
					{
						p->m_pBlendVertex[i].m_nWeight = 0.5f;		// 강제로 0.5로 다시 맞춰줌.
					}
				}
			}
		
			m_pPhysiqueVertex[ nNumVertex ].m_nNumBone = nNumReal;		// 실제 세어본 개수

			scan->GetToken();		// }
			scan->GetToken();		// }
			nNumVertex ++;
		}

		// non-blend vertex
		if( strcmp( scan->token, "PHYSIQUE_NONBLENDED_RIGIDTYPE" ) == 0 )
		{
			scan->GetToken();		// 버텍스 번호 스킵
			scan->GetToken();		// Bone 이름 읽음.
#ifdef	__X2LINK
			m_pPhysiqueVertex[ nNumVertex ].m_nNumBone = 2;
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex = new PHYSIQUE_BLEND[2];
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[0].m_nWeight = 1.0f;
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_nWeight = 0;	

			szBone = m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[0].m_strBoneName;
			memset( szBone, 0, 32 );
			strcpy( szBone, scan->token );	// 일단 Bip01 L(R) Finger를 전부 카피
			memset( szBuff, 0, 256 );
			strcpy( szBuff, szBone );
			_strlwr( szBuff );
			if( strncmp( szBuff+8, "finger", 6 ) == 0 )	// 손가락뼈에 연결된 버텍스가 나오면 손뼈로 바꿈
				strcpy( szBone+8, "Hand" );		// Bip01 L(R) Hand로 변환
			if( strncmp( szBuff+8, "toe", 3 ) == 0 )
				strcpy( szBone+8, "Foot" );		// Bip01 L(R) Hand로 변환
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[0].m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.

			szBone = m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_strBoneName;
			memset( szBone, 0, 32 );
			strcpy( szBone, scan->token );	// 일단 Bip01 L(R) Finger를 전부 카피
			memset( szBuff, 0, 256 );
			strcpy( szBuff, szBone );
			_strlwr( szBuff );
			
			if( strncmp( szBuff+8, "finger", 6 ) == 0 )	// 손가락뼈에 연결된 버텍스가 나오면 손뼈로 바꿈
				strcpy( szBone+8, "Hand" );		// Bip01 L(R) Hand로 변환
			if( strncmp( szBuff+8, "toe", 3 ) == 0 )
				strcpy( szBone+8, "Foot" );		// Bip01 L(R) Hand로 변환
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex[1].m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.
#else
			m_pPhysiqueVertex[ nNumVertex ].m_nNumBone = 1;
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex = new PHYSIQUE_BLEND;
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex->m_nWeight = 1.0;

			szBone = m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex->m_strBoneName;
			memset( szBone, 0, 32 );
			strcpy( szBone, scan->token );	// 일단 Bip01 L(R) Finger를 전부 카피
			memset( szBuff, 0, 256 );
			strcpy( szBuff, szBone );
			strlwr( szBuff );
			if( strncmp( szBuff+8, "finger", 6 ) == 0 )	// 손가락뼈에 연결된 버텍스가 나오면 손뼈로 바꿈
				strcpy( szBone+8, "Hand" );		// Bip01 L(R) Hand로 변환
			if( strncmp( szBuff+8, "toe", 3 ) == 0 )
				strcpy( szBone+8, "Foot" );		// Bip01 L(R) Hand로 변환
			m_pPhysiqueVertex[ nNumVertex ].m_pBlendVertex->m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.
#endif
			nNumVertex ++;
		}
	}

	return 1;
}



// SKIN데이타 읽음.
int		CASEMesh :: Read_SKIN( CScanner *scan, int nVertNum)
{

	int		nNumBlock = 0;
	int		nNumVertex = 0;
	PHYSIQUE_BLEND	vBlend;
	int nNum = nVertNum;


	if( nNum != m_nMaxVertexList || nNum == 0)
	{
		
		return 0;
	}

	m_pPhysiqueVertex = new	PHYSIQUE[ m_nMaxVertexList ];
	memset( m_pPhysiqueVertex, 0, sizeof(PHYSIQUE) * m_nMaxVertexList );

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' || scan->token[0] == '\0')	
		{
			if( --nNumBlock <= 0 )	break;
		}

		// skin vertex
		if( strcmp( scan->token, "VERTEX" ) == 0 )
		{
			int		numNode, nRealNum;
			int		i, j;
			PHYSIQUE	*p = &m_pPhysiqueVertex[ nNumVertex ];

			scan->GetToken();		// 버텍스 번호 스킵
			numNode = nRealNum = scan->GetNumber();	// Blend node 갯수
			if( numNode == 0 ) 
			{
				nNumVertex ++;
				Error( "어떤본에도 연결되어 있지 않은 버텍스가 존재. 모든 버텍스는 어떤 본에라도 연결되어야 함." );
				exit(1);
				continue;
			}
			scan->GetToken();		// {
			if( numNode == 1 )	numNode = 2;		// 최소 두개가 되도록 한다.
			p->m_nNumBone = numNode;		// bone 노드 갯수 셋
			p->m_pBlendVertex = new PHYSIQUE_BLEND[ numNode ];
			for( i = 0; i < nRealNum; i ++ )
			{
				scan->GetToken();		// *
				scan->GetToken();		// BONE
				scan->GetToken();		// bone name
				strcpy( p->m_pBlendVertex[i].m_strBoneName, scan->token );
				p->m_pBlendVertex[i].m_nWeight = scan->GetFloat();
				p->m_pBlendVertex[i].m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.
			}
//			if( nRealNum > 1 )
//			{
//				for( i = 0; i < nRealNum; i ++ )
//					p->m_pBlendVertex[i].m_nWeight = 0.5f;
//			}
			if( numNode != nRealNum )	// 2개로 조정된것이라면 [0]을 [1]에도 똑같이 만들어 준다.
			{
				p->m_pBlendVertex[1] = p->m_pBlendVertex[0];
				p->m_pBlendVertex[1].m_nWeight = 0;
			}
			// 가장 영향이 큰 본을 사용하도록 소트한다.
			for ( i = 0; i < numNode-1; i++ )
			{
				for( j = i+1; j < numNode; j ++ )
				{
					if( p->m_pBlendVertex[i].m_nWeight < p->m_pBlendVertex[j].m_nWeight )
					{
						vBlend = p->m_pBlendVertex[j];	// 바꿈.
						p->m_pBlendVertex[j] = p->m_pBlendVertex[i];
						p->m_pBlendVertex[i] = vBlend;
					}
				}
			}
			if( p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight < 1.0f )
			{
				float fSub = 1.0f - (p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight);
				p->m_pBlendVertex[0].m_nWeight += (fSub / 2);
				p->m_pBlendVertex[1].m_nWeight += (fSub / 2);

			}
			

			scan->GetToken();		// }
			nNumVertex ++;
		}
	}

	//m_bRigid = TRUE;

	return 1;
}



// SKIN데이타 읽음.
int		CASEMesh :: Read_SKIN( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nNumVertex = 0;
	PHYSIQUE_BLEND	vBlend;

	int		nNum = scan->GetNumber();		// *SKIN_VERTEX_DATA   1151 {  <- 개수 부분
	if( nNum != m_nMaxVertexList )
	{
		if( m_pGeoMObject )
			Message( "%s 버텍스개수와 스킨버텍스개수가 다르다.", m_pGeoMObject->m_strName );
		else
			Message( "버텍스개수와 스킨버텍스개수가 다르다." );
		return 0;
	}
	if( nNum == 0 )
	{
		if( m_pGeoMObject )
			Message( "%s 스킨버텍스 개수가 0이다", m_pGeoMObject->m_strName );
		else
			Message( "스킨버텍스 개수가 0이다" );
		return 0;
	}

	m_pPhysiqueVertex = new	PHYSIQUE[ m_nMaxVertexList ];
	memset( m_pPhysiqueVertex, 0, sizeof(PHYSIQUE) * m_nMaxVertexList );

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}

		// skin vertex
		if( strcmp( scan->token, "VERTEX" ) == 0 )
		{
			int		numNode, nRealNum;
			int		i, j;
			PHYSIQUE	*p = &m_pPhysiqueVertex[ nNumVertex ];

			scan->GetToken();		// 버텍스 번호 스킵
			numNode = nRealNum = scan->GetNumber();	// Blend node 갯수
			if( numNode == 0 ) 
			{
				nNumVertex ++;
				Error( "어떤본에도 연결되어 있지 않은 버텍스가 존재. 모든 버텍스는 어떤 본에라도 연결되어야 함." );
				exit(1);
				continue;
			}
			scan->GetToken();		// {
			if( numNode == 1 )	numNode = 2;		// 최소 두개가 되도록 한다.
			p->m_nNumBone = numNode;		// bone 노드 갯수 셋
			p->m_pBlendVertex = new PHYSIQUE_BLEND[ numNode ];
			for( i = 0; i < nRealNum; i ++ )
			{
				scan->GetToken();		// *
				scan->GetToken();		// BONE
				scan->GetToken();		// bone name
				strcpy( p->m_pBlendVertex[i].m_strBoneName, scan->token );
				p->m_pBlendVertex[i].m_nWeight = scan->GetFloat();
				p->m_pBlendVertex[i].m_pBone = NULL;		// 이건 지금 알수 없으니 ASE를 다 읽고 난 후 셋팅함.
			}
//			if( nRealNum > 1 )
//			{
//				for( i = 0; i < nRealNum; i ++ )
//					p->m_pBlendVertex[i].m_nWeight = 0.5f;
//			}
			if( numNode != nRealNum )	// 2개로 조정된것이라면 [0]을 [1]에도 똑같이 만들어 준다.
			{
				p->m_pBlendVertex[1] = p->m_pBlendVertex[0];
				p->m_pBlendVertex[1].m_nWeight = 0;
			}
			// 가장 영향이 큰 본을 사용하도록 소트한다.
			for ( i = 0; i < numNode-1; i++ )
			{
				for( j = i+1; j < numNode; j ++ )
				{
					if( p->m_pBlendVertex[i].m_nWeight < p->m_pBlendVertex[j].m_nWeight )
					{
						vBlend = p->m_pBlendVertex[j];	// 바꿈.
						p->m_pBlendVertex[j] = p->m_pBlendVertex[i];
						p->m_pBlendVertex[i] = vBlend;
					}
				}
			}
			if( p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight < 1.0f )
			{
				float fSub = 1.0f - (p->m_pBlendVertex[0].m_nWeight + p->m_pBlendVertex[1].m_nWeight);
				p->m_pBlendVertex[0].m_nWeight += (fSub / 2);
				p->m_pBlendVertex[1].m_nWeight += (fSub / 2);

			}
			

			scan->GetToken();		// }
			nNumVertex ++;
		}
	}

	//m_bRigid = TRUE;

	return 1;
}


// *MESH_MTLID 서브 메터리얼 번호에 따라 페이스를 소팅시킴.
// 이곳은 아직 텍스쳐파일을 읽진 않은 상태다.
void	CASEMesh :: SortMaterialBlock( void )
{
	int		i, j;
	FVF_ASEMESH	tempCV;
	INDEX_BUFFER	tempIB;
	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();


	{
		int		nMaxMtrl = nMaxMaterial;
		int		nMaxSubMtrl = 0;
		ASE_MATERIAL		*pMtrl = NULL;
		SUB_MATERIAL	*pSubMtrl = NULL;
		
		if( pMaterialList )
		{
			pMtrl = &pMaterialList[ m_nMaterialRef ];
			nMaxSubMtrl = pMtrl->m_nMaxSubMaterial;
			// 서브 매터리얼이 있으면 그것을 max로 쓴다.  없으면 메인매터리얼갯수를 max로 쓴다
			if( nMaxSubMtrl )
				nMaxMtrl = nMaxSubMtrl;
		}

		if( nMaxMtrl == 0 )		nMaxMtrl = 1;
		// 매터리얼 갯수로 %를 해서 ID값을 정확하게 맞춘다
		for( i = 0; i < m_nMaxFaceList; i ++ )
		{
			m_pFaceList[i].m_nMaterialID %= nMaxMtrl;
			if( pMtrl )
			{
				if( m_bOpacity == FALSE )	m_bOpacity	= (pMtrl->m_dwEffect & XE_OPACITY) ? TRUE : FALSE;
				if( m_bBump == FALSE )		m_bBump		= (pMtrl->m_dwEffect & XE_BUMP) ? TRUE : FALSE;
			}

			// Opacity사용여부를 알려줌
			if( nMaxSubMtrl )
			{
				pSubMtrl = &(pMtrl->m_pSubMaterial[ m_pFaceList[i].m_nMaterialID ]);	// sub mtrl 포인터를 받아서.
				if( m_bOpacity == FALSE )		// 이미 TRUE가 됐는데 다시 없애버릴걸 우려
					m_bOpacity = (pSubMtrl->m_dwEffect) ? TRUE : FALSE;		// 오파시티값을 사용하고 있다면 플래그에 자동 세팅 될것이다.
				if( m_bBump == FALSE )		// 이미 TRUE가 됐는데 다시 없애버릴걸 우려
					m_bBump = (pSubMtrl->m_dwEffect) ? TRUE : FALSE;		// 범프값을 사용하고 있다면 플래그에 자동 세팅 될것이다.
			}
		}
	}

	MESH_FACE	tempFace;
	int		nVBIdx;

	for( i = 0; i < m_nMaxFaceList - 1; i ++ )
	{
		for( j = i + 1; j < m_nMaxFaceList; j ++ )
		{
			if( m_pFaceList[j].m_nMaterialID < m_pFaceList[i].m_nMaterialID )
			{
				// sort해야할 상황
				// MtrlID만 바꿀게 아니라 face를 통째로 바꿔야 하는게 맞다.
				tempFace = m_pFaceList[i];
				m_pFaceList[i] = m_pFaceList[j];
				m_pFaceList[j] = tempFace;

				nVBIdx = m_pFaceList[i].a;		// 버텍스버퍼도 실제로 바꼈기땜에 a,b,c까지 바뀌면 안된다.
				m_pFaceList[i].a = m_pFaceList[j].a;
				m_pFaceList[j].a = nVBIdx;
				nVBIdx = m_pFaceList[i].b;
				m_pFaceList[i].b = m_pFaceList[j].b;
				m_pFaceList[j].b = nVBIdx;
				nVBIdx = m_pFaceList[i].c;
				m_pFaceList[i].c = m_pFaceList[j].c;
				m_pFaceList[j].c = nVBIdx;

				tempCV = m_pVertexBuffer[i*3];
				m_pVertexBuffer[i*3] = m_pVertexBuffer[j*3];
				m_pVertexBuffer[j*3] = tempCV;

				tempCV = m_pVertexBuffer[i*3+1];
				m_pVertexBuffer[i*3+1] = m_pVertexBuffer[j*3+1];
				m_pVertexBuffer[j*3+1] = tempCV;

				tempCV = m_pVertexBuffer[i*3+2];
				m_pVertexBuffer[i*3+2] = m_pVertexBuffer[j*3+2];
				m_pVertexBuffer[j*3+2] = tempCV;

				tempIB = m_pIndexBuffer[i*3];
				m_pIndexBuffer[i*3] = m_pIndexBuffer[j*3];
				m_pIndexBuffer[j*3] = tempIB;
				
				tempIB = m_pIndexBuffer[i*3+1];
				m_pIndexBuffer[i*3+1] = m_pIndexBuffer[j*3+1];
				m_pIndexBuffer[j*3+1] = tempIB;
				
				tempIB = m_pIndexBuffer[i*3+2];
				m_pIndexBuffer[i*3+2] = m_pIndexBuffer[j*3+2];
				m_pIndexBuffer[j*3+2] = tempIB;
			}
		}
	}

//	for( i = 0; i < m_nMaxFaceList; i ++ )
//	{
//		if( m_pFaceList[i].m_nMaterialID > m_nMaxVertexBufferBlock )		
//			m_nMaxVertexBufferBlock = m_pFaceList[i].m_nMaterialID;		// 가장 큰수의 ID를 찾음
//	}

}

#define		MAX_MTRLBLK		256

//----------------------------------------------------------------------
// Draw루틴의 최적화를 위해서 Material들의 집합으로 묶어준다.
//
//-----------------------------------------------------------------------
void	CASEMesh :: CreateVertexBufferBlock( void )
{
	int		i;
	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();

	// 최소 1개의 버텍스버퍼블럭은 존재 해야한다.
	if( pMaterialList == NULL )
	{
		if( nMaxMaterial )
			Message( "CASEMesh::CreateVertexBufferBlock : ASE->m_nMaxMaterial에 값이 있다. %d", nMaxMaterial );
		m_nMaxVertexBufferBlock = 1;
		m_pVertexBufferBlock = new MATERIAL_BLOCK[ 1 ];
		m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[ 1 ];
		memset( m_pVertexBufferBlock, 0, sizeof(MATERIAL_BLOCK) );
		memset( m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) );
		m_pVertexBufferBlock->m_nPrimitiveCount = m_nMaxFaceList;
//		m_pVertexBufferBlock->m_nStartVertex = 0;
//		m_pVertexBufferBlock->m_nTextureID = 0;
//		m_pVertexBufferBlock->m_pTexture = NULL;
		return;
	}

//	m_nMaxVertexBufferBlock ++;		// ID는 0부터 시작하므로 ID+1의 크기를 가져야 한다. 
	// -1이 아니라는것은 매터리얼 참조를 읽었다는 것이다.
	if( m_nMaterialRef != -1 )
	{
		int		nNumBlock = 0;
		int		nTempID = -1;
		int		nNumID = 0;
		int		nMaxSubMtrl = pMaterialList[ m_nMaterialRef ].m_nMaxSubMaterial;
		int		nCnt = 0;

		if( nMaxSubMtrl == 0 )	nMaxSubMtrl = 1;
		m_pVertexBufferBlock = new MATERIAL_BLOCK[ MAX_MTRLBLK /*m_nMaxVertexBufferBlock*/ ];	// 미리 크게 잡아놓는식으로 하자.
		m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[ MAX_MTRLBLK /*m_nMaxVertexBufferBlock*/ ];
		memset( m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * MAX_MTRLBLK /*m_nMaxVertexBufferBlock*/ );
		memset( m_pVertexBufferBlock, 0, sizeof(MATERIAL_BLOCK) * MAX_MTRLBLK /*m_nMaxVertexBufferBlock*/ );
		for( i = 0; i < m_nMaxFaceList; i ++ )
		{
			if( nTempID != (m_pFaceList[i].m_nMaterialID % nMaxSubMtrl) )
			{
				if( nNumBlock >= MAX_MTRLBLK )
					Error( "CreateVertexBufferBlock : 블럭수가 너무 많다. %d\r\nMaterial개수가 너무 많다. Max의 Material Editor를 확인해 볼 것!", nNumBlock );
				nTempID = (m_pFaceList[i].m_nMaterialID % nMaxSubMtrl);
				m_pVertexBufferBlock[ nNumBlock ].m_nStartVertex = i * 3;		// 새로운 ID가 시작하는 곳의 인덱스 기록.
				m_pVertexBufferBlock[ nNumBlock ].m_dwEffect = 0;		// m_nReflect에다가 모든 이펙트의 옵션을 다 때려넣는방식으로 바꿈.
				
				if( pMaterialList[ m_nMaterialRef ].m_pSubMaterial )
				{
					SUB_MATERIAL *pSubMtrl = &(pMaterialList[ m_nMaterialRef ].m_pSubMaterial[ nTempID % nMaxSubMtrl ]);

					m_pMtrlBlkTexture[ nNumBlock ]			     = pSubMtrl->m_pTexture;
//					if( pSubMtrl->m_n2Side )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_2SIDE;
//					if( pSubMtrl->m_bOpacity )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_OPACITY;
//					if( pSubMtrl->m_bReflect )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_REFLECT;
//					if( pSubMtrl->m_nSelfIlluminate )	m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_SELF_ILLUMINATE;
					
//					m_pVertexBufferBlock[ nNumBlock ].m_n2Side   = pSubMtrl->m_n2Side;		// 컨버트 한번하고 나면 이변수는 모두 reserved로 바꾸자
//					m_pVertexBufferBlock[ nNumBlock ].m_nOpacity = pSubMtrl->m_bOpacity;	
//					m_pVertexBufferBlock[ nNumBlock ].m_nReflect = pSubMtrl->m_bReflect;
					m_pVertexBufferBlock[ nNumBlock ].m_dwEffect = pSubMtrl->m_dwEffect;
					
					m_pVertexBufferBlock[ nNumBlock ].m_nAmount  = pSubMtrl->m_nAmount;
				}
				else
				{
					ASE_MATERIAL *pMtrl = &(pMaterialList[ m_nMaterialRef ]);
					m_pMtrlBlkTexture[ nNumBlock ]			     = pMtrl->m_pTexture;
//					if( pMtrl->m_n2Side )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_2SIDE;
//					if( pMtrl->m_bOpacity )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_OPACITY;
//					if( pMtrl->m_bReflect )			m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_REFLECT;
//					if( pMtrl->m_nSelfIlluminate )	m_pVertexBufferBlock[ nNumBlock ].m_nReflect |= XE_SELF_ILLUMINATE;

//					m_pVertexBufferBlock[ nNumBlock ].m_n2Side   = pMtrl->m_n2Side;
//					m_pVertexBufferBlock[ nNumBlock ].m_nOpacity = pMtrl->m_bOpacity;
//					m_pVertexBufferBlock[ nNumBlock ].m_nReflect = pMtrl->m_bReflect;
					m_pVertexBufferBlock[ nNumBlock ].m_dwEffect = pMtrl->m_dwEffect;

					m_pVertexBufferBlock[ nNumBlock ].m_nAmount  = pMtrl->m_nAmount;
				}
				m_pVertexBufferBlock[ nNumBlock ].m_nTextureID = nTempID;// % nMaxSubMtrl;
				
				nNumBlock ++;
			}
		}
		// 프리미티브 갯수 계산
		for( i = 0; i < nNumBlock - 1; i ++ )
		{
			m_pVertexBufferBlock[i].m_nPrimitiveCount = 
				(m_pVertexBufferBlock[ i + 1 ].m_nStartVertex - m_pVertexBufferBlock[i].m_nStartVertex) / 3;
		}
		m_pVertexBufferBlock[i].m_nPrimitiveCount = m_nMaxFaceList - (m_pVertexBufferBlock[i].m_nStartVertex / 3);

		// ID값으로만 측정한 버텍스블럭크기와 실제 세어본 블럭수가 다르면 메모리 할당 재 배치.
//		if( nNumBlock != m_nMaxVertexBufferBlock )		
		{
//			MATERIAL_BLOCK	*pTempBlock;
//			pTempBlock = new MATERIAL_BLOCK[ nNumBlock ];		// 실제 세어본 갯수로 재할당.
//			memcpy( pTempBlock, m_pVertexBufferBlock, sizeof(MATERIAL_BLOCK) * nNumBlock );		// 원래 메모리에서 새 메모리로 옮김
//			delete []m_pVertexBufferBlock;			// 원래 메모리 날리고
//			m_pVertexBufferBlock = pTempBlock;		// 포인터 재 셋팅.
			m_nMaxVertexBufferBlock = nNumBlock;	// 블럭 갯수도 재 셋팅.
		}
	}
}

// NODE_TM을 기준으로 버텍스 좌표들을 로컬로 변환 시킨다
void	CASEMesh :: TransformToLocal( void )
{
//	충돌메시의 피봇이 센터가 아니라도 되게 할것
	int		i;
	int		nMax = m_nMaxVertexBuffer;
	int		nMaxVList = m_nMaxVertexList;
	FVF_ASEMESH	*pBuff = m_pVertexBuffer;
	D3DXMATRIX		*pMatInv = &m_pGeoMObject->m_matInverseTM;
	SKIN_VERTEX		*pVList = m_pVertexList;

	// NODE_TM을 기준으로 버텍스 좌표들을 로컬로 변환.
	for( i = 0; i < nMaxVList; i ++ )		// 모든 버텍스 피지크 리스트를 루프.
	{
		D3DXVec3TransformCoord( &(pVList->m_vLocalPos), &(pVList->m_vWorldPos), pMatInv );
		pVList ++;
	}

	// 버텍스 버퍼에 로컬로 변환된 좌표를 넣는다.
	for( i = 0; i < nMax; i++ )
	{
		pBuff->position = m_pVertexList[ m_pIndexBuffer[i].nIdx ].m_vLocalPos;
//		pBuff->normal	= m_pIndexBuffer[i].normal;
		pBuff ++;
	}
	
}

// 이거 현재 사용하지 않음.
void	CASEMesh :: SetBlendVertex( void )
{
	int		i;
	int		nMax = m_nMaxFaceList * 3;
	FVF_ASEMESH	*pBuff = m_pVertexBuffer;
	CGeoMObject	*pBone;
	int		nVIdx;

	for( i = 0; i < nMax; i++ )
	{
		nVIdx = m_pIndexBuffer[i].nIdx;
		pBone = m_pPhysiqueVertex[ nVIdx ].m_pBlendVertex[0].m_pBone;
		// 각 버텍스가 소속된 뼈대기준으로 좌표를 변환한다
//		D3DXVec3TransformCoord( &(m_pVertexList[ nVIdx ].m_vLocalPos), &(m_pVertexList[ nVIdx ].m_vWorldPos), &pBone->m_matInverseTM );
		pBuff->position = m_pVertexList[ nVIdx ].m_vLocalPos;
//		pBuff->normal	= m_pIndexBuffer[i].normal;
//		pBuff->weight = 1.0;
//		pBuff->matIdx = pBone->m_nID;
		pBuff ++;
	}

	SendVertexBuffer( FALSE );		// AGP버텍스 버퍼로 전송
}


// 메모리상의 버텍스 버퍼를 AGP메모리로 전송
int		CASEMesh :: SendVertexBuffer( BOOL bSendIB )
{
	if( m_pVB == NULL )	return FAIL;

	VOID*	pVertices;
	int		nMax;

#ifndef	__DIP	// DrawPrimitive
	nMax = m_nMaxVertexBuffer * sizeof(FVF_ASEMESH);
	// m_nMaxVertexBuffer 은 facenum * 3의 크기이다.
	if( FAILED( m_pVB->Lock( 0, nMax, (BYTE**)&pVertices, 0 ) ) )
		return FAIL;
	memcpy( pVertices, m_pVertexBuffer, nMax );	
	m_pVB->Unlock();
#else
	nMax = m_nMaxVertexBuffer * sizeof(FVF_ASEMESH);
	// m_nMaxVertexBuffer 은 facenum * 3의 크기이다.
	if( FAILED( m_pVB->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )
		return FAIL;
	memcpy( pVertices, m_pVertexBuffer, nMax );	
	m_pVB->Unlock();

	// for indexed
	nMax = m_nMaxIVB * sizeof(FVF_ASEMESH);
	if( FAILED( m_pIVertexBuffer->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )
		return FAIL;
	memcpy( pVertices, m_pIVB, nMax );	
	m_pIVertexBuffer->Unlock();

	if( bSendIB )
	{
		nMax = m_nMaxIB * sizeof(WORD);
		if( FAILED( m_pIIndexBuffer->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )
			return FAIL;
		memcpy( pVertices, m_pIB, nMax );	
		m_pIIndexBuffer->Unlock();
	}
#endif

	return SUCCESS;
}

// 본의 변환이 모두 끝난후 실행된다.

// 피지크 버텍스들을 본에 맞춰 다시 계산해서 월드 좌표로 생성
// 월드좌표로 생성된 버텍스들을 버텍스 버퍼에 카피
int		CASEMesh :: SetVertexBuffer( void )
{
	int		i;

	// WorldPos값만 갱신되면 되므로 
	// TransfromSkinVertex()에서 계산된 WorldPos값만 카피시켜 준다.
#ifdef	__DIP
	int		nMax = m_nMaxIVB;
	FVF_ASEMESH *pIVB = m_pIVB;
	SKIN_VERTEX	*pVertexList = m_pVertexList;

	for( i = 0; i < nMax; i ++ )
	{
		pIVB->position = pVertexList[ m_pIIB[i] ].m_vWorldPos;
		pIVB->normal   = pVertexList[ m_pIIB[i] ].m_vWorldNormal;
		pIVB ++;
	}

#else
	int		nMax = m_nMaxFaceList * 3;
	FVF_ASEMESH	*pBuff = m_pVertexBuffer;

	for( i = 0; i < nMax; i++ )
	{
		pBuff->position = m_pVertexList[ m_pIndexBuffer[i].nIdx ].m_vWorldPos;
//		pBuff->normal	= m_pIndexBuffer[i].normal;
		pBuff ++;
	}

#endif
	SendVertexBuffer( FALSE );		// AGP버텍스 버퍼로 전송, 인덱스버퍼는 다시 보낼필요 없다.

	return SUCCESS;
}


void	CASEMesh :: TransformSkinVertex( void )
{
	int		i;
	PHYSIQUE	*p;

	D3DXMATRIX	matRotate, mTM;
	D3DXVECTOR3	v1, v2;

	// 버텍스 갯수만큼 돈다.
	// 스킨의 각 버텍스들은 자기가 소속된 BONE의 최종결과 매트릭스와 자기로컬 좌표를 곱해서
	// 최종 좌표를 생성해낸다.
	if( m_bRigid )
	{	// non-blending skinning
		for( i = 0; i < m_nMaxVertexList; i ++ )
		{
			p = &(m_pPhysiqueVertex[i]);
			D3DXVec3TransformCoord( &(m_pVertexList[i].m_vWorldPos), 
									&(m_pVertexList[i].m_vLocalPos), 
									&(p->m_pBlendVertex[0].m_pBone->m_matWorld)			// 일단은 영향받는 bone을 한개만 쓴다.
								  );

			matRotate = p->m_pBlendVertex[0].m_pBone->m_matWorld;
	//		matRotate._41 = 0;
	//		matRotate._42 = 0;
	//		matRotate._43 = 0;		// 회전값만을 필요로 하므로.
			D3DXVec3TransformCoord( &(m_pVertexList[i].m_vWorldNormal),
									&(m_pVertexList[i].m_vNormal),
									&matRotate
								  );
		}
	} else
	{	
		// 2-link blending skinning
		for( i = 0; i < m_nMaxVertexList; i ++ )
		{
			p = &(m_pPhysiqueVertex[i]);
			// Inv(TM) * AniTM
			if( p->m_pBlendVertex == NULL ) continue;
			mTM = p->m_pBlendVertex[0].m_pBone->m_matInverseTM * p->m_pBlendVertex[0].m_pBone->m_matWorld;
			D3DXVec3TransformCoord( &v1,  &(m_pVertexList[i].m_vLocalPos),  &mTM );		// 변수명은 LocalPos지만 값은 오브젝트계좌표가 들어있다.
			v1 *= p->m_pBlendVertex[0].m_nWeight;

			mTM = p->m_pBlendVertex[1].m_pBone->m_matInverseTM * p->m_pBlendVertex[1].m_pBone->m_matWorld;
			D3DXVec3TransformCoord( &v2,  &(m_pVertexList[i].m_vLocalPos),  &mTM );
			v2 *= p->m_pBlendVertex[1].m_nWeight;

			m_pVertexList[i].m_vWorldPos = v1 + v2;
		}
	}
}

float	u = 0.0f;
float	v = 0.0f;


HRESULT		CASEMesh :: Render( void )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	MATERIAL_BLOCK	*pBlock = m_pVertexBufferBlock;
	LPDIRECT3DTEXTURE9 *pBlkTexture = m_pMtrlBlkTexture;
	int		i;
	CAse	*pASE = m_pGeoMObject->m_pASE;
	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();
	DWORD dwFillMode;
	
	// Render Mesh
	// indexed 
	{
		if( pMaterialList )
		{
#ifdef	__SPECULAR
			pMaterialList[ m_nMaterialRef ].m_Material.Power = 100.0f;
			pMaterialList[ m_nMaterialRef ].m_Material.Ambient.r = 0.5f;
			pMaterialList[ m_nMaterialRef ].m_Material.Ambient.g = 0.5f;
			pMaterialList[ m_nMaterialRef ].m_Material.Ambient.b = 0.5f;
#endif
			pd3dDevice->SetMaterial( &pMaterialList[ m_nMaterialRef ].m_Material );
		}
		pd3dDevice->SetStreamSource( 0, m_pIVertexBuffer, 0, sizeof(FVF_ASEMESH) );
//		pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FVF_ASEMESH) );
		pd3dDevice->SetVertexShader( NULL );
		pd3dDevice->SetVertexDeclaration( NULL );
		pd3dDevice->SetFVF( D3DFVF_ASEVERTEX );
		pd3dDevice->SetIndices( m_pIIndexBuffer );

		if( pASE->m_fScrlU || pASE->m_fScrlV )
		{
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._31 = u;
			mat._32 = v;
			u += pASE->m_fScrlU;
			v += pASE->m_fScrlV;

			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		}

		for( i = 0; i < m_nMaxVertexBufferBlock; i ++ )
		{
#ifdef	__COLA
			pd3dDevice->GetRenderState( D3DRS_FILLMODE, &dwFillMode );
			if( dwFillMode != D3DFILL_SOLID )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
#else
			pd3dDevice->SetTexture( 0, *pBlkTexture );
#endif
			BOOL bLight = FALSE;
			if( _strcmpi( m_pGeoMObject->m_strName, "light" ) == 0 )
			{
				bLight = TRUE;
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
			if( pBlock->m_dwEffect & XE_2SIDE )
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			if( pBlock->m_dwEffect & XE_OPACITY )	// 알파채널
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
//				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// 특정 알파키값 뺌.
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				if( pBlock->m_nAmount < 255 )		// 
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
//					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
					pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(pBlock->m_nAmount, 0, 0, 0) );
					pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
				}

			} else
			if( pBlock->m_dwEffect & XE_REFLECT )
			{
				pd3dDevice->SetTransform( D3DTS_TEXTURE1, &g_mReflect );
				extern LPDIRECT3DTEXTURE9 g_pReflectMap;
				pd3dDevice->SetTexture( 1, g_pReflectMap );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
#ifdef __COLA
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   g_TextureOp[g_nSelectTextureOp] );
#else
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
#endif
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			} else
			if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
			{
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
			}



			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nMaxIVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
//			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 210 );
//			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_nMaxIVB, 0, 210 );
			// 해제
			pd3dDevice->SetTexture( 0, NULL );

			if( pBlock->m_dwEffect & XE_2SIDE )
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			if( pBlock->m_dwEffect & XE_OPACITY )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			} else
			if( pBlock->m_dwEffect & XE_REFLECT )
			{
				pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			} else
			if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
			{
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	//
			}
				
			if( bLight )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
			g_nMaxTri += pBlock->m_nPrimitiveCount;
			pBlock ++;
			pBlkTexture ++;
		}
/*		
		// 해제
		if( g_nEnvMap )
		{
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		}
		if( m_bOpacity )
		{
//			pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		}
*/
	} 

	D3DXVECTOR3	v1, v2, v3, vC, vN, vA, vB;
	// 페이스 노말 그림
	if( m_nShowFNormal )
	{
//		D3DXMATRIX m;
//		D3DXMatrixIdentity( &m );
//		pd3dDevice->SetTransform( D3DTS_WORLD, &m );		// Set World Transform 
		for( i = 0; i < m_nMaxFaceList; i++ )
		{
			v1 = m_pVertexList[ m_pFaceList[i].v1 ].m_vLocalPos;		// face의 버텍스A
			v2 = m_pVertexList[ m_pFaceList[i].v2 ].m_vLocalPos;		// face의 버텍스B
			v3 = m_pVertexList[ m_pFaceList[i].v3 ].m_vLocalPos;		// face의 버텍스C

//			D3DXVec3TransformCoord( &v1, &v1, &m_pGeoMObject->m_matLocalTM );
//			D3DXVec3TransformCoord( &v2, &v2, &m_pGeoMObject->m_matLocalTM );
//			D3DXVec3TransformCoord( &v3, &v3, &m_pGeoMObject->m_matLocalTM );
			
//			vA = v2 - v1;
//			vB = v3 - v1;
//			D3DXVec3Cross( &vN, &vA, &vB );

			vC = (v1 + v2 + v3) / 3.0f;		// 면의 중앙 지점
			vN = m_pFaceList[i].n;
			D3DXVec3Normalize( &vN, &vN );
			vN /= 10.0f;
			vN += vC;
			g_Grp3D.Render3DLine( vC, vN );
		}
	}

	// 버텍스 노말 그림
	if( m_nShowVNormal )
	{
		if( m_pPhysiqueVertex )
		{
			for( i = 0; i < m_nMaxVertexList; i ++ )
			{
				vC = m_pVertexList[i].m_vWorldPos;		// 스키닝오브젝트는 변환된 버텍스를 기준으로 노말을 그리고
				vN = vC + (m_pVertexList[i].m_vNormal / 10);
				g_Grp3D.Render3DLine( vC, vN );
			}
		} else
		{
			for( i = 0; i < m_nMaxVertexList; i ++ )
			{
				vC = m_pVertexList[i].m_vLocalPos;		// 일반오브젝트는 LocalPos + SetTransform(World)로 출력
				vN = vC + (m_pVertexList[i].m_vNormal / 10);
				g_Grp3D.Render3DLine( vC, vN );
			}
		}
	}

/*
	else
	{
		pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FVF_ASEMESH) );
		pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nMaxVertexBuffer / 3);
	}
*/

	return S_OK;
}


// Skin Mesh저장
/*
int		CASEMesh :: SaveSKN( LPCTSTR strFileName )
{
	FILE	*fp;
	int		temp;
	int		i, j;
	ASE_MATERIAL	*pMaterial;
	int		nVer = 5;
	int		nSerial = xRand();			// 32비트형 시리얼 번호
	char	reserved[32];

	fp = fopen( strFileName, "wb" );
	if( fp == NULL )	return FAIL;

	fwrite( &nVer, 4, 1, fp );			// 버전.
	fwrite( &nSerial, 4, 1, fp );		// 고유 번호
	fwrite( &m_bOpacity, 4, 1, fp );	
	fwrite( reserved, 32, 1, fp );		// reserved
	fwrite( &m_pGeoMObject->m_nType, 4, 1, fp );		// 오브젝트 타입

	fwrite( &m_nMaxVertexList, 4, 1, fp );			// 버텍스 리스트 갯수
	fwrite( &m_nMaxVertexBuffer, 4, 1, fp );			// 버텍스 버퍼 갯수
	temp = m_nMaxFaceList;
	fwrite( &temp, 4, 1, fp );						// 페이스 갯수

	if( m_pGeoMObject->m_nType == GEOTYPE_LINKMESH )
	{
		fwrite( &(m_pGeoMObject->m_pParent->m_nID), 4, 1, fp );		// 부모의 ID저장
//		fwrite( &(m_pGeoMObject->m_matTM), sizeof(D3DXMATRIX), 1, fp );	// NODE_TM저장
		fwrite( &(m_pGeoMObject->m_matLocalTM), sizeof(D3DXMATRIX), 1, fp );	// NODE_TM 로컬 저장
	} else
	{
		temp = -1;
		fwrite( &temp, 4, 1, fp );		// 그렇지 않은건 -1저장
	}

	// 버텍스 리스트 저장
	for( i = 0; i < m_nMaxVertexList; i ++ )
		fwrite( &m_pVertexList[i].m_vLocalPos, sizeof(D3DXVECTOR3), 1, fp );			// 버텍스 리스트의 LocalPos만 저장
	// 인덱스 버퍼 저장
	fwrite( m_pIndexBuffer, sizeof(INDEX_BUFFER) * (m_nMaxFaceList * 3), 1, fp );		
	// 버텍스 버퍼 저장
	fwrite( m_pVertexBuffer, sizeof(FVF_ASEMESH) * (m_nMaxFaceList * 3), 1, fp );		

	// 피지크정보가 있다면 피지크 버텍스갯수가 저장.  없다면 0이 저장(단일메쉬의 경우)
	if( m_pPhysiqueVertex )		temp = m_nMaxVertexList;
	else						temp = 0;

	fwrite( &temp, 4, 1, fp );
	// 피지크 정보 저장
	if( m_pPhysiqueVertex )
	{
		for( i = 0; i < m_nMaxVertexList; i ++ )
			fwrite( &m_pPhysiqueVertex[i].m_pBlendVertex[0].m_pBone->m_nID, 4, 1, fp );			// 각 버텍스들의 소속bone번호를 저장
	}

	// max값이 있는데 리스트는 없거나
	// max값은 0인데 리스트는 있거나 하는경우 경고
	if( (g_nMaxMaterial && g_pMaterialList == NULL) ||
		(g_nMaxMaterial == 0 && g_pMaterialList ) )
	{
		sprintf( g_strError, "경고 : MaxMaterial = %d : MaterialList = 0x%08X", g_nMaxMaterial, (int)g_pMaterialList );
		MessageBox( NULL, g_strError, NULL, MB_OK );
	}

	fwrite( &g_nMaxMaterial, 4, 1, fp );
	// 매트리얼 정보 저장.
	if( g_pMaterialList )
	{
		pMaterial = &g_pMaterialList[ m_nMaterialRef ];		
		int		nMaxSub = pMaterial->m_nMaxSubMaterial;		// 서브 매트리얼 개수

//		char	pBuff[256];

		fwrite( &nMaxSub, 4, 1, fp );
//		if( pMaterial->m_pSubMaterial )		// 서브 매터리얼이 있을때.
		if( nMaxSub )		// 서브 매터리얼이 있을때.
		{
			for( j = 0; j < nMaxSub; j ++ )
			{
				fwrite( &pMaterial->m_pSubMaterial[j].m_Material, sizeof(D3DMATERIAL9), 1, fp );
				LPCTSTR pBuff = GetFileName( pMaterial->m_pSubMaterial[j].m_strBitmapFileName );
				i = strlen( pBuff );
				fwrite( &i, 4, 1, fp );		// strlen
				fwrite( pBuff, i, 1, fp );	// 저장할때 풀패스였던 비트맵명을 파일명만 추려서 저장.
			}
		} else
		{
			fwrite( &pMaterial->m_Material, sizeof(D3DMATERIAL9), 1, fp );
			LPCTSTR pBuff = GetFileName( pMaterial->m_strBitmapFileName );
			i = strlen( pBuff );
			fwrite( &i, 4, 1, fp );		// strlen
			fwrite( pBuff, i, 1, fp );
		}
		if( nMaxSub < 0 )	MessageBox( NULL, "이런...sub mtrl이 마이너스 값이다.", NULL, MB_OK );
	} else
	{
		// 매터리얼 없으면 그냥 넘어감
	}
	
	// Draw Block 정보 저장
	fwrite( &m_nMaxVertexBufferBlock, 4, 1, fp );
	for( i = 0; i < m_nMaxVertexBufferBlock; i ++ )
	{
		fwrite( &m_pVertexBufferBlock[i].m_nStartVertex,	4, 1, fp );
		fwrite( &m_pVertexBufferBlock[i].m_nPrimitiveCount, 4, 1, fp );
		fwrite( &m_pVertexBufferBlock[i].m_nTextureID,		4, 1, fp );
	}

	// bound box min, max vector
	fwrite( &m_vMin, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &m_vMax, sizeof(D3DXVECTOR3), 1, fp );

	fclose(fp);
	return SUCCESS;
}
*/
//   파일 포인터를 넘겨받아 필요한 부분만 처리한다
int		CASEMesh :: SaveMesh( FILE *fp )
{
	int		temp;
	int		i, j;
	char	reserved[32];

	// bound box min, max vector
	memset( reserved, 0, 32 );

	fwrite( &m_vMin, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &m_vMax, sizeof(D3DXVECTOR3), 1, fp );

	fwrite( &m_bOpacity,	4, 1, fp );	// 오브젝트의 알파채널 포함여부
//	fwrite( &g_nEnvMap,		4, 1, fp );	// 옛날엔 이게 저장됐는데 bump로 바뀜.
	fwrite( &m_bBump,		4, 1, fp );	// bump flag
	fwrite( &m_bRigid,		4, 1, fp );	// 리지드냐 블렌드냐.
	fwrite( reserved,	   28, 1, fp );		// reserved

	fwrite( &m_nMaxVertexList,		4, 1, fp );		// 버텍스 리스트 갯수
	fwrite( &m_nMaxIVB,				4, 1, fp );		// 버텍스 버퍼 개수, indexed용 버텍스 버퍼다.
//	fwrite( &m_nMaxVertexBuffer,	4, 1, fp );		// 버텍스 버퍼 갯수, non-idxed용이다.
	fwrite( &m_nMaxFaceList, 4, 1, fp );			// 페이스 갯수
	fwrite( &m_nMaxIB,				4, 1, fp );

	// 버텍스 리스트 저장
	for( i = 0; i < m_nMaxVertexList; i ++ )
		fwrite( &m_pVertexList[i].m_vLocalPos, sizeof(D3DXVECTOR3), 1, fp );	// 버텍스 리스트의 LocalPos만 저장
	
//	fwrite( m_pIndexBuffer,  sizeof(INDEX_BUFFER) * (m_nMaxFaceList * 3), 1, fp );		// 인덱스 버퍼 저장
//	fwrite( m_pVertexBuffer, sizeof(FVF_ASEMESH) * (m_nMaxFaceList * 3), 1, fp );		// 버텍스 버퍼 저장
#ifdef	__XVS
	if( m_pPhysiqueVertex )
	{
		for( i = 0; i < m_nMaxIVB; i ++ )
		{
			fwrite( &m_pVertexList[ m_pIIB[i] ].m_vLocalPos , sizeof(D3DXVECTOR3), 1, fp );
			fwrite( &m_pIVB[i].w1,	 sizeof(FLOAT), 1, fp );
			fwrite( &m_pIVB[i].w2,	 sizeof(FLOAT), 1, fp );
			fwrite( &m_pIVB[i].matIdx,	 sizeof(DWORD), 1, fp );
			fwrite( &m_pIVB[i].normal,   sizeof(D3DXVECTOR3), 1, fp );
			fwrite( &m_pIVB[i].tu,		 4, 1, fp );
			fwrite( &m_pIVB[i].tv,		 4, 1, fp );
		}
	} else
	{
		for( i = 0; i < m_nMaxIVB; i ++ )	// 스킨오브젝트가 아닌건 matIdx를 저장하지 않음,.
		{
			fwrite( &m_pIVB[i].position, sizeof(D3DXVECTOR3), 1, fp );
			fwrite( &m_pIVB[i].normal,   sizeof(D3DXVECTOR3), 1, fp );
			fwrite( &m_pIVB[i].tu,		 4, 1, fp );
			fwrite( &m_pIVB[i].tv,		 4, 1, fp );
		}
	}
#else
	fwrite( m_pIVB,		sizeof(FVF_ASEMESH) * m_nMaxIVB, 1, fp );	// 인덱스드용 버텍스 버퍼만 저장해야 한다.
#endif
	fwrite( m_pIB,		sizeof(WORD) * m_nMaxIB, 1, fp );
	fwrite( m_pIIB,		sizeof(WORD) * m_nMaxIVB, 1, fp );


	// 피지크정보가 있다면 피지크 버텍스갯수가 저장.  없다면 0이 저장(단일메쉬의 경우)
	if( m_pPhysiqueVertex )		temp = m_nMaxVertexList;
	else						temp = 0;

	fwrite( &temp, 4, 1, fp );
	// 피지크 정보 저장
	if( m_pPhysiqueVertex )
	{
		for( i = 0; i < m_nMaxVertexList; i ++ )
			fwrite( &m_pPhysiqueVertex[i].m_pBlendVertex[0].m_pBone->m_nIdx, 4, 1, fp );			// 각 버텍스들의 소속bone번호를 저장
	}


	ASE_MATERIAL	*pMaterialList = m_pGeoMObject->m_pASE->GetMaterial();
	int				nMaxMaterial = m_pGeoMObject->m_pASE->GetMaxMaterial();
	// Material 저장
	// max값이 있는데 리스트는 없거나
	// max값은 0인데 리스트는 있거나 하는경우 경고
	if( (nMaxMaterial && pMaterialList == NULL) ||
		(nMaxMaterial == 0 && pMaterialList ) )
	{
		Error( "경고 : MaxMaterial = %d : MaterialList = 0x%08X", nMaxMaterial, (int)pMaterialList );
	}

	fwrite( &nMaxMaterial, 4, 1, fp );
	// 매트리얼 정보 저장.
	if( nMaxMaterial )
	{
		ASE_MATERIAL	*pMtrl;
		pMtrl = &pMaterialList[ m_nMaterialRef ];		
		int		nMaxSub = pMtrl->m_nMaxSubMaterial;		// 서브 매트리얼 개수
//		char	pBuff[256];
		int		len;

		fwrite( &nMaxSub, 4, 1, fp );
		if( nMaxSub )		// 서브 매터리얼이 있을때.
		{
			for( j = 0; j < nMaxSub; j ++ )
			{
				fwrite( &pMtrl->m_pSubMaterial[j].m_Material, sizeof(D3DMATERIAL9), 1, fp );
				LPCTSTR pBuff = GetFileName( pMtrl->m_pSubMaterial[j].m_strBitmapFileName );
				len = strlen( pBuff ) + 1;		// 널문자 포함이라서 + 1
				fwrite( &len, 4, 1, fp );		// strlen
				fwrite( pBuff, len, 1, fp );	// 저장할때 풀패스였던 비트맵명을 파일명만 추려서 저장.
			}
		} else
		{
			fwrite( &pMtrl->m_Material, sizeof(D3DMATERIAL9), 1, fp );
			LPCTSTR pBuff = GetFileName( pMtrl->m_strBitmapFileName );
			len = strlen( pBuff ) + 1;
			fwrite( &len, 4, 1, fp );		// strlen
			fwrite( pBuff, len, 1, fp );
		}
		if( nMaxSub < 0 )	MessageBox( NULL, "이런...sub mtrl이 마이너스 값이다.", NULL, MB_OK );
	} else
	{
		// 매터리얼 없으면 그냥 넘어감
	}
	
	// Draw Block 정보 저장
	fwrite( &m_nMaxVertexBufferBlock, 4, 1, fp );
#ifdef	__XVS
	fwrite( m_pVertexBufferBlock, sizeof(MATERIAL_BLOCK) * m_nMaxVertexBufferBlock, 1, fp );	// 한방에 저장함.
#else
	for( i = 0; i < m_nMaxVertexBufferBlock; i ++ )
	{
		fwrite( &m_pVertexBufferBlock[i].m_nStartVertex,	4, 1, fp );
		fwrite( &m_pVertexBufferBlock[i].m_nPrimitiveCount, 4, 1, fp );
		fwrite( &m_pVertexBufferBlock[i].m_nTextureID,		4, 1, fp );		// 서브 메터리얼 번호
	}
#endif

	return SUCCESS;
}


// 버텍스 리스트를 검색해서 MAX,MIN바운드 영역을 가려낸다
void	CASEMesh :: CalcBoundBox( void )
{
	D3DXVECTOR3		vMin, vMax;
	int		i;

	vMin.x = vMin.y = vMin.z = 65535.0;
	vMax.x = vMax.y = vMax.z = -65535.0;

	if( m_pPhysiqueVertex )
	{
		for( i = 0; i < m_nMaxVertexList; i ++ )	// 스킨버텍스들의 LocalPos은 뼈대기준으로 변환되어 있기땀시 WorldPos를 써야한다.
		{
			if( m_pVertexList[i].m_vWorldPos.x < vMin.x )	vMin.x = m_pVertexList[i].m_vWorldPos.x;
			if( m_pVertexList[i].m_vWorldPos.y < vMin.y )	vMin.y = m_pVertexList[i].m_vWorldPos.y;
			if( m_pVertexList[i].m_vWorldPos.z < vMin.z )	vMin.z = m_pVertexList[i].m_vWorldPos.z;
			if( m_pVertexList[i].m_vWorldPos.x > vMax.x )	vMax.x = m_pVertexList[i].m_vWorldPos.x;
			if( m_pVertexList[i].m_vWorldPos.y > vMax.y )	vMax.y = m_pVertexList[i].m_vWorldPos.y;
			if( m_pVertexList[i].m_vWorldPos.z > vMax.z )	vMax.z = m_pVertexList[i].m_vWorldPos.z;
		}
	} else
	{
		for( i = 0; i < m_nMaxVertexList; i ++ )
		{
			if( m_pVertexList[i].m_vLocalPos.x < vMin.x )	vMin.x = m_pVertexList[i].m_vLocalPos.x;
			if( m_pVertexList[i].m_vLocalPos.y < vMin.y )	vMin.y = m_pVertexList[i].m_vLocalPos.y;
			if( m_pVertexList[i].m_vLocalPos.z < vMin.z )	vMin.z = m_pVertexList[i].m_vLocalPos.z;
			if( m_pVertexList[i].m_vLocalPos.x > vMax.x )	vMax.x = m_pVertexList[i].m_vLocalPos.x;
			if( m_pVertexList[i].m_vLocalPos.y > vMax.y )	vMax.y = m_pVertexList[i].m_vLocalPos.y;
			if( m_pVertexList[i].m_vLocalPos.z > vMax.z )	vMax.z = m_pVertexList[i].m_vLocalPos.z;
		}
	}

	m_nWidth	= vMax.x - vMin.x;
	m_nHeight	= vMax.y - vMin.y;
	m_nDepth	= vMax.z - vMin.z;

	m_vMin = vMin;	m_vMax = vMax;
}

HRESULT	CASEMesh :: CreateVB( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( pd3dDevice == NULL )	return FAIL;
	// 지오메트리 오브젝트의 트리를 돌면서 버텍스 버퍼 생성
	// 및, 버퍼에 데이타 전송
	if( FAILED( pd3dDevice->CreateVertexBuffer( m_nMaxVertexBuffer * sizeof(FVF_ASEMESH),
												  0, D3DFVF_ASEVERTEX,
												  D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
	{
		Error( "버텍스 버퍼 생성 실패 : m_nMaxVertexBuffer = %d\r\n이런 경우는 보통 max파일을 열어보면 좌표만 존재하는 이상한 오브젝트가 있다.  그것을 지울것!", m_nMaxVertexBuffer );
		return E_FAIL;
	}

	// for Indexed Primitive
	if( FAILED( pd3dDevice->CreateVertexBuffer( m_nMaxIVB * sizeof(FVF_ASEMESH),
												  D3DUSAGE_WRITEONLY, D3DFVF_ASEVERTEX,
												  D3DPOOL_MANAGED, &m_pIVertexBuffer, NULL ) ) )
	{
		Error( "Indexed 버텍스 버퍼 생성 실패 : m_nMaxIVB = %d", m_nMaxIVB );
		return E_FAIL;
	}

	if( FAILED( pd3dDevice->CreateIndexBuffer( m_nMaxIB * sizeof(WORD),
                                            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                            D3DPOOL_MANAGED, &m_pIIndexBuffer, NULL ) ) )
	{
		Error( "Indexed 인덱스 버퍼 생성 실패 : m_nMaxIB = %d", m_nMaxIB );
		return E_FAIL;
	}


	return S_OK;
}

void	CASEMesh :: ClearNormal( void )
{
	int		i;
	D3DXVECTOR3		n = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	for( i = 0; i < m_nMaxFaceList; i ++ )
		m_pFaceList[i].n = n;

	for( i = 0; i < m_nMaxVertexList; i ++ )
		m_pVertexList[i].m_vNormal = n;

	for( i = 0; i < m_nMaxVertexBuffer; i ++ )
	{
		m_pIndexBuffer[i].normal = n;
		m_pVertexBuffer[i].normal = n;
	}
	for( i = 0; i < m_nMaxIVB; i ++ )
		m_pIVB[i].normal = n;

	SendVertexBuffer( FALSE );
}

// FVF_ASEMESH 버텍스두개를 비교한다.
BOOL	IsSameVertex( const FVF_ASEMESH &v1, const FVF_ASEMESH &v2 )
{
	if( v1.tu != v2.tu )	return FALSE;
	if( v1.tv != v2.tv )	return FALSE;
	if( v1.position != v2.position )	return FALSE;
	if( v1.normal != v2.normal )	return FALSE;
	if( v1.matIdx != v2.matIdx )	return FALSE;
//#ifndef	__XVS
/*
	if( v1.color != v2.color )		return FALSE;
*/
//#endif
	return TRUE;
}

//
// 버텍스v를 IVB에 추가한다.
// 이미 있으면 새로 추가하지 않고 인덱스만 리턴하고.
// 없으면 추가하고 그 인덱스 리턴
// nIdx : IVB가 VertexList의 어느 버텍스를 가르키고 있는지에 대한 인덱스
//
int		CASEMesh :: AddIVB( const FVF_ASEMESH &v, int nIdx, int nID )
{
	int		i;
	FVF_ASEMESH	*pIVB;
	int		nMax = m_nMaxIVB;
	
	pIVB = m_ptempIVB;

	for( i = 0; i < nMax; i ++ )
	{
		if( IsSameVertex(*pIVB, v) && m_ptempID[i] == nID )		// 찾았으면 인덱스 리턴
			return i;
		pIVB ++;
	}

	if( nMax >= m_nMaxIB )
		Error("에러 : AddIVB(),  m_nMaxIVB(%d)가 m_nMaxIB(%)를 넘어섰다.", nMax, m_nMaxIB );
	m_ptempIVB[ nMax ] = v;
	m_ptempIIB[ nMax ] = (WORD)nIdx;
	m_ptempID[ nMax ] = (BYTE)nID;
	m_nMaxIVB ++;

	return nMax;		// 새로추가된곳의 인덱스 리턴
}


//
//	읽어들인 모델데이타를 인덱스드화 시킨다.
// m_pVertexBuffer의 내용을 바탕으로 인덱스드 시킨다.
//
void	CASEMesh :: MakeIndexed( void )
{
	int		i;

	m_nMaxIB = m_nMaxFaceList * 3;
	m_ptempIVB = new FVF_ASEMESH[ m_nMaxIB ];		// VIB갯수가 IB갯수를 넘어갈순 없을것이다.
	m_ptempIIB = new WORD[ m_nMaxIB ];				// m_pIVB와 갯수가 같다.
	m_ptempID  = new BYTE[ m_nMaxIB ];
	m_pIB  = new WORD[ m_nMaxIB ];			// IB크기는 numFace * 3이다.
	m_nMaxIVB = 0;

	memset( m_ptempIVB, 0, sizeof(FVF_ASEMESH) * m_nMaxIB );
	memset( m_ptempID, 0, sizeof(BYTE) * m_nMaxIB );
	memset( m_pIB,  0, sizeof(WORD) * m_nMaxIB );

	// 1. IVB를 만든다.
	// 2. IVB를 바탕으로 IB를 만든다.
	// 3. Indexed용 MtrlBlk를 만든다.

	MESH_FACE	*pFace = m_pFaceList;
	int		nIdx;
	for( i = 0; i < m_nMaxFaceList; i ++ )		// FaceList loop
	{
		nIdx = AddIVB( m_pVertexBuffer[ pFace->a ], m_pIndexBuffer[ pFace->a ].nIdx, pFace->m_nMaterialID );
		m_pIB[ i*3+0 ] = nIdx;
		nIdx = AddIVB( m_pVertexBuffer[ pFace->b ], m_pIndexBuffer[ pFace->b ].nIdx, pFace->m_nMaterialID );
		m_pIB[ i*3+1 ] = nIdx;
		nIdx = AddIVB( m_pVertexBuffer[ pFace->c ], m_pIndexBuffer[ pFace->c ].nIdx, pFace->m_nMaterialID );
		m_pIB[ i*3+2 ] = nIdx;

		pFace ++;
	}

	// 임시버퍼 내용을 오리지날에 옮김.
	m_pIVB = new FVF_ASEMESH[ m_nMaxIVB ];
	m_pIIB = new WORD[ m_nMaxIVB ];
	memcpy( m_pIVB, m_ptempIVB, sizeof(FVF_ASEMESH) * m_nMaxIVB );
	memcpy( m_pIIB, m_ptempIIB, sizeof(WORD) * m_nMaxIVB );
	SAFE_DELETE_ARRAY( m_ptempIVB );
	SAFE_DELETE_ARRAY( m_ptempIIB );
	SAFE_DELETE_ARRAY( m_ptempID );
/*
	for( i = 0; i < m_nMaxIB; i ++ )
	{
		if( IsSameVertex( m_pVertexBuffer[i], m_pIVB[i] ) == FALSE )
		{
			int a;
			a++;
		}
	}
*/
//	if( m_nMaxIVB != m_nMaxTVertices )
//		Error( "경고 : MakeIndexed() 수행결과 nMaxIVB(%d)와 m_nMaxTVertices(%d)의 개수가 다르다", m_nMaxIVB, m_nMaxTVertices );

}

BOOL	FindBoneIdx( int *pBuff, int nMax, int nBoneIdx )
{
	int		i;
	for( i = 0; i < nMax; i ++ )
	{
		if( pBuff[i] == nBoneIdx )	return TRUE;
	}
	return FALSE;
}

//
// this오브젝트에서 사용된 뼈대의 리스트를 작성하고 개수를 리턴함
// nStart부터 nEnd를 검사대상으로 한다, Primitive인덱스
// 뼈대리스트의 결과는 pOut버퍼에 기록된다.
// pBuff의 최대길이는 256을 넘을수 없다.
//
int		CASEMesh :: CalcUseBone( int nStart, int nCnt, int *pBuff )
{
	int		i, j;
	int		nIdx, nBoneIdx;
	int		nMaxUse = 0;
	int		nMax;
	INDEX_BUFFER *pIndexBuffer;
	
//	if( nCnt > m_nMaxFaceList-1 )
//		Error( "CalcUseBone : nEnd(%d)가 MaxPrimitive(%d)의 범위를 넘어섬.", nEnd, m_nMaxFaceList );

	// nStart ~ nEnd사이의 프리미티브들이 사용하는 뼈대의 인덱스를 모두 골라냄.
	nMax = nCnt * 3;
	pIndexBuffer = m_pIndexBuffer + nStart;
	for( i = 0; i < nMax; i ++ )
	{
		nIdx = pIndexBuffer[ i ].nIdx;
		nBoneIdx = m_pPhysiqueVertex[ nIdx ].m_pBlendVertex[0].m_pBone->m_nIdx;
		// 이미 추가된건지 찾음.
		if( FindBoneIdx( pBuff, nMaxUse, nBoneIdx ) == FALSE )	// 이미 있는거라면 다음 버텍스로 스킵
		{
			if( nMaxUse >= 256 )
				Error( "CalcUseBone : nMaxUse가 256개를 넘어섰다" );
			pBuff[ nMaxUse ] = nBoneIdx;			// 없던거라면 추가. 
			nMaxUse ++;								// 
		}
#ifdef	__X2LINK
		// 두번째
		nBoneIdx = m_pPhysiqueVertex[ nIdx ].m_pBlendVertex[1].m_pBone->m_nIdx;
		// 이미 추가된건지 찾음.
		if( FindBoneIdx( pBuff, nMaxUse, nBoneIdx ) == FALSE )		// 이미 있는거라면 다음 버텍스로 스킵
		{
			if( nMaxUse >= 256 )
				Error( "CalcUseBone : nMaxUse가 256개를 넘어섰다" );
			pBuff[ nMaxUse ] = nBoneIdx;			// 없던거라면 추가. 
			nMaxUse ++;								// 
		}
#endif

	}
	
	// 여기다 웬만하면 소팅하는거 넣자.
	for( i = 0; i < nMaxUse-1; i ++ )
	{
		for( j = i+1; j < nMaxUse; j ++ )
		{
			if( pBuff[i] > pBuff[j] )
			{
				SWAP( pBuff[i], pBuff[j] );
			}
		}
	}

	if( nMaxUse >= 256 )
		Message( "CalcUseBone : nMaxUse(%d)가 지나치게 많다.", nMaxUse );
	return nMaxUse;

}

//
// m_pUseBone[]을 토대로 matIdx를 재설정한다.
// nStart, nEnd : Primitive인덱스
//
void	CASEMesh :: Reindex( int nStart, int nCnt, int nMaxUseBone, int *pUseBone )
{
#ifdef	__XVS
	int		i, j;
	int		nBoneIdx, nIdx;

	int		nMax = nCnt * 3;
	INDEX_BUFFER *pIndexBuffer = m_pIndexBuffer + nStart;
	for( i = 0; i < nMax; i ++ )
	{
		nIdx = pIndexBuffer[ i ].nIdx;
		nBoneIdx = m_pPhysiqueVertex[ nIdx ].m_pBlendVertex[0].m_pBone->m_nIdx;		// 원래 매트릭스 인덱스
		for( j = 0; j < nMaxUseBone; j ++ )
		{
			if( nBoneIdx == pUseBone[j] )
			{
				m_pVertexBuffer[ nStart + i ].matIdx = j * 3;		// Local 본 리스트의 인덱스로 다시 설정
				break;
			}
		}
#ifdef	__X2LINK
		nBoneIdx = m_pPhysiqueVertex[ nIdx ].m_pBlendVertex[1].m_pBone->m_nIdx;		// 원래 매트릭스 인덱스
		for( j = 0; j < nMaxUseBone; j ++ )
		{
			if( nBoneIdx == pUseBone[j] )
			{
				m_pVertexBuffer[ nStart + i ].matIdx |= ((j * 3) << 16);		// Local 본 리스트의 인덱스로 다시 설정
				break;
			}
		}
#endif
	}
#endif
}


/*
void	CASEMesh :: DivideMesh(int nStart, int nCount)
{

	MATERIAL_BLOCK *pMtrlBlk	= &m_pVertexBufferBlock[ m_nMaxVertexBufferBlock ];	// 쪼개진게 새로이 추가될 버퍼.
	*pMtrlBlk					= *pBlk;	// 통째로 전송
	int nOdd					= pBlk->m_nPrimitiveCount / 2;
	int nBoneNum				= 0;


	pBlk->m_nPrimitiveCount		= nCount - nOdd;			// 반으로 쪼개고..
	pMtrlBlk->m_nPrimitiveCount = nOdd;						// 나머지는 끝으로
	pMtrlBlk->m_nStartVertex	= nStart + nCount * 3;

	nBoneNum = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );		// 쪼갠다음 개수셈.
	m_pMtrlBlkTexture[ m_nMaxVertexBufferBlock ] = m_pMtrlBlkTexture[ i ];
	m_nMaxVertexBufferBlock ++;		// 메쉬 개수 하나더 증가

	if( nBoneNum > MAX_VS_BONE )
	{
		DevideMesh(int nStart, int nCount);
	}
	else
	{
		pBlk->m_nMaxUseBone = nBoneNum;
		memcpy( pBlk->m_UseBone, pBuff, sizeof(int) * nMaxUse );
		Reindex( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, nMaxUse, pBuff );
		pBlk ++;
	}
}*/

//
//
//
void	CASEMesh :: ReindexingBoneIdx( int nGroup )
{
#ifdef	__XVS
	int		pBuff[ 256 ];
	int		nMaxUse;
	int		i;

	if( m_pPhysiqueVertex == NULL )		return;			// 피지크가 아닌 일반 메쉬는 들어올필요 없다.
	memset( pBuff, 0, sizeof(int) * 256);//MAX_VS_BONE );

	nMaxUse = CalcUseBone( 0, m_nMaxFaceList, pBuff );
	if( nMaxUse <= MAX_VS_BONE )
	{
		m_nMaxUseBone = nMaxUse;
		memcpy( m_UseBone, pBuff, sizeof(int) * nMaxUse );		// 사용되는 뼈대리스트
		Reindex( 0, m_nMaxFaceList, nMaxUse, pBuff );		// 각 버텍스들의 matIdx 재설정.
	} else
	{/*
#define __FIX_LOD			// 메쉬 LOD 사용시 3번 쪼개 보고 안되면 에러로 처리하는 문제 수정

	MATERIAL_BLOCK *pBlk = m_pVertexBufferBlock;

	for( i = 0; i < m_nMaxVertexBufferBlock; i ++ )
	{
		nMaxUse = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );
		if( nMaxUse > MAX_VS_BONE )
		{

		}
		else
		{

		}
	}

#else */
		MATERIAL_BLOCK *pBlk = m_pVertexBufferBlock;
		for( i = 0; i < m_nMaxVertexBufferBlock; i ++ )
		{
			nMaxUse = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );
			if( nMaxUse > MAX_VS_BONE )
			{
				MATERIAL_BLOCK *pMtrlBlk = &m_pVertexBufferBlock[ m_nMaxVertexBufferBlock ];	// 쪼개진게 새로이 추가될 버퍼.
				*pMtrlBlk = *pBlk;	// 통째로 전송
				int odd = pBlk->m_nPrimitiveCount / 2;
				pBlk->m_nPrimitiveCount = pBlk->m_nPrimitiveCount - odd;	// 반으로 쪼개고..
				pMtrlBlk->m_nPrimitiveCount = odd;							// 나머지는 끝으로
				pMtrlBlk->m_nStartVertex = pBlk->m_nStartVertex + pBlk->m_nPrimitiveCount * 3;
				nMaxUse = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );		// 쪼갠다음 개수셈.
				m_pMtrlBlkTexture[ m_nMaxVertexBufferBlock ] = m_pMtrlBlkTexture[ i ];
				m_nMaxVertexBufferBlock ++;		// 최대 개수 하나더 증가 - 루프를 한번더 돌게 된다.
				if( nMaxUse > MAX_VS_BONE )		// 그래도 많다면 에러내버리자.
				{
					//매터리얼블럭쪼개서 블럭추가;  아직 지원 안함
//					Message( "ReindexingBoneIdx : MtrlBlk내에서 쓰는 본 개수가 너무 많다. %d", nMaxUse );
					Message( "ReindexingBoneIdx : lod%d MtrlBlk내에서 쓰는 본 개수(%d)가 너무 많아서 한번 쪼갭니다.", nGroup+1, nMaxUse );
					// 한번더 쪼개보자.
					pMtrlBlk = &m_pVertexBufferBlock[ m_nMaxVertexBufferBlock ];
					*pMtrlBlk = *pBlk;	// 통째로 전송
					int odd = pBlk->m_nPrimitiveCount / 2;
					pBlk->m_nPrimitiveCount = pBlk->m_nPrimitiveCount - odd;	// 반으로 쪼개고..
					pMtrlBlk->m_nPrimitiveCount = odd;							// 나머지는 끝으로
					pMtrlBlk->m_nStartVertex = pBlk->m_nStartVertex + pBlk->m_nPrimitiveCount * 3;
					nMaxUse = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );		// 쪼갠다음 개수셈.
					m_pMtrlBlkTexture[ m_nMaxVertexBufferBlock ] = m_pMtrlBlkTexture[ i ];
					m_nMaxVertexBufferBlock ++;		// 최대 개수 하나더 증가 - 루프를 한번더 돌게 된다.
					if( nMaxUse > MAX_VS_BONE )		// 그래도 많다면 에러내버리자.
					{
//						Message( "ReindexingBoneIdx : lod%d MtrlBlk내에서 쓰는 본 개수가 너무 많아서 이건 도무지 변환이 안된다오 흑.. %d", nGroup+1, nMaxUse );
//						매터리얼블럭쪼개서 블럭추가;  아직 지원 안함
//						Message( "ReindexingBoneIdx : MtrlBlk내에서 쓰는 본 개수가 너무 많다. %d", nMaxUse );
						Message( "ReindexingBoneIdx : lod%d MtrlBlk내에서 쓰는 본 개수(%d)가 너무 많아서 한번 쪼갭니다.", nGroup+1, nMaxUse );
						// 한번더 쪼개보자.
						pMtrlBlk = &m_pVertexBufferBlock[ m_nMaxVertexBufferBlock ];
						*pMtrlBlk = *pBlk;	// 통째로 전송
						int odd = pBlk->m_nPrimitiveCount / 2;
						pBlk->m_nPrimitiveCount = pBlk->m_nPrimitiveCount - odd;	// 반으로 쪼개고..
						pMtrlBlk->m_nPrimitiveCount = odd;							// 나머지는 끝으로
						pMtrlBlk->m_nStartVertex = pBlk->m_nStartVertex + pBlk->m_nPrimitiveCount * 3;
						nMaxUse = CalcUseBone( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, pBuff );		// 쪼갠다음 개수셈.
						m_pMtrlBlkTexture[ m_nMaxVertexBufferBlock ] = m_pMtrlBlkTexture[ i ];
						m_nMaxVertexBufferBlock ++;		// 최대 개수 하나더 증가 - 루프를 한번더 돌게 된다.
						if( nMaxUse > MAX_VS_BONE )		// 그래도 많다면 에러내버리자.
							Message( "ReindexingBoneIdx : lod%d MtrlBlk내에서 쓰는 본 개수가 너무 많아서 이건 도무지 변환이 안된다오 흑.. %d", nGroup+1, nMaxUse );
						
					}
						
				}
			}

			pBlk->m_nMaxUseBone = nMaxUse;
			memcpy( pBlk->m_UseBone, pBuff, sizeof(int) * nMaxUse );
			Reindex( pBlk->m_nStartVertex, pBlk->m_nPrimitiveCount, nMaxUse, pBuff );
			pBlk ++;
		}
//#endif

	}
#endif
}

//
// m_nIdx에 * 3을 하므로 바이트범위내에서 가능한 본인덱스는 255 / 3 = vb5.
//
void	CASEMesh :: SetBoneIdx( void )
{
#ifdef	__XVS
	int		i, idx;
	if( m_pPhysiqueVertex == NULL )		return;

  #ifdef	__X2LINK
	int		i1, i2;
	PHYSIQUE *pPhy;
	for( i = 0; i < m_nMaxFaceList * 3; i ++ )
	{
		idx = m_pIndexBuffer[i].nIdx;
		pPhy = &(m_pPhysiqueVertex[ idx ]);
		if( pPhy->m_pBlendVertex == NULL ) continue;
		i1 = pPhy->m_pBlendVertex[0].m_pBone->m_nIdx * 3;
		i2 = pPhy->m_pBlendVertex[1].m_pBone->m_nIdx * 3;
		m_pVertexBuffer[i].matIdx = (i2 << 16) | i1;		// 
		m_pVertexBuffer[i].w1 = m_pPhysiqueVertex[ idx ].m_pBlendVertex[0].m_nWeight;
		m_pVertexBuffer[i].w2 = m_pPhysiqueVertex[ idx ].m_pBlendVertex[1].m_nWeight;
	}
  #else
	for( i = 0; i < m_nMaxFaceList * 3; i ++ )
	{
		idx = m_pIndexBuffer[i].nIdx;
		m_pVertexBuffer[i].matIdx = m_pPhysiqueVertex[ idx ].m_pBlendVertex[0].m_pBone->m_nIdx * 3;
	}
  #endif // X2LINK
#endif // XVS
}

//
// 레이와 교차하는 버텍스를 검색
// 레이는 오브젝트 matWorld의 역으로 변환된 상태로 와야 한다.
//
D3DXVECTOR3 *CASEMesh::IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir )
{
	int		j;
	int		nMax;
	SKIN_VERTEX *pVList;
	D3DXVECTOR3	vSize = D3DXVECTOR3(0.025f, 0.025f, 0.025f);

	nMax = m_nMaxVertexList;
	pVList = m_pVertexList;
	if( m_pPhysiqueVertex )
	{
		for( j = 0; j < nMax; j ++ )
		{
			if( IsTouchRayAABB3( vRayOrig, vRayDir, pVList->m_vWorldPos, vSize ) )
			{
				*pOut = pVList->m_vWorldPos;
				return &pVList->m_vWorldPos;
			}
			pVList ++;
		}
	} else
	{
		for( j = 0; j < nMax; j ++ )
		{
			if( IsTouchRayAABB3( vRayOrig, vRayDir, pVList->m_vLocalPos, vSize ) )
			{
				*pOut = pVList->m_vLocalPos;
				return &pVList->m_vLocalPos;
			}
			pVList ++;
		}
	}

	return NULL;
}
