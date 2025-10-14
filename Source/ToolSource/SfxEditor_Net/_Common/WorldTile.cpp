#include "stdafx.h"

#define TILEIMGSIZE (32*32*2)
#define TILEFILENUM 500
#define TILEEXT     0x7ef4

typedef struct tagTileOrg
{
	WORD nSum;	
	BYTE data[TILEIMGSIZE];

} TILEORG,* LPTILEORG;

struct Tile
{
	BYTE bBlock   : 1;
	BYTE bReserve : 1;
	BYTE nTable      ;
	union	{
		DWORD nIndex;
		LPTILEORG pTileOrg;
	};
};

#ifdef __VAMPYRE_TILE

CPtrArray  CWorld::m_tileAry           ; 
LPDWORD    CWorld::m_pOffsetTile = NULL;
DWORD      CWorld::m_nSizeTile   = 0   ;
DWORD      CWorld::m_nSizeExt    = 0   ;

CTile* CWorld::GetTile(D3DXVECTOR3 vector)
{
	CTile* pTile = &m_fieldArray[ ( ((int)vector.z / 4) * 63) + ((int)vector.x / 4) ];
	return pTile;
}
CTile* CWorld::GetTile(CPoint pt)
{
	CTile* pTile = &m_fieldArray[  pt.y * 63 + pt.x ];
	return pTile;
}
CTile* CWorld::GetTile(int x,int y)
{
	CTile* pTile = &m_fieldArray[ y * 63 + x ];
	return pTile;
}
BOOL CWorld::LoadField(LPCTSTR lpszFileName)
{
	struct HEIGHTTILE
	{
		BYTE byHeight;
		WORD wTile;
	};
	CTile* lpTilecell;
	HEIGHTTILE heightTile[63*63]; 
	CFile file;
	if(file.Open( MakePath( DIR_WORLD, lpszFileName ) ,CFile::modeRead))
	{
		file.Read(heightTile,sizeof(heightTile));
		for(int y = 0; y < 63; y++)
		{
			for(int x = 0; x < 63; x++)
			{
				lpTilecell = &m_fieldArray[y*63+x];
				lpTilecell->m_byHeight = heightTile[y*63+x].byHeight;
				lpTilecell->m_wTileNum = heightTile[y*63+x].wTile;
			}
		}
	}
	else
	{
		for(int y = 0; y < 63; y++)
		{
			for(int x = 0; x < 63; x++)
			{
				lpTilecell = &m_fieldArray[y*63+x];
				lpTilecell->m_byHeight = 0;//rand() % 10;
				lpTilecell->m_wTileNum = rand() % (16*16);
			}
		}
	}
	//lpTilecell = &m_fieldArray[10*63+10];
	//lpTilecell->m_byHeight = 15;
	//lpTilecell = &m_fieldArray[11*63+10];
	//lpTilecell->m_byHeight = 20;
	return LoadTile("sample.tle",m_fieldArray);
}
BOOL CWorld::SaveField(LPCTSTR lpszFileName)
{
	struct HEIGHTTILE
	{
		BYTE byHeight;
		WORD wTile;
	};
	CTile* lpTilecell;
	HEIGHTTILE heightTile[63*63]; 
	CFile file;
	if(file.Open( MakePath( DIR_WORLD, lpszFileName ) ,CFile::modeWrite | CFile::modeCreate))
	{
		for(int y = 0; y < 63; y++)
		{
			for(int x = 0; x < 63; x++)
			{
				lpTilecell = &m_fieldArray[y*63+x];
				heightTile[y*63+x].byHeight = lpTilecell->m_byHeight;
				heightTile[y*63+x].wTile = lpTilecell->m_wTileNum;
			}
		}
		file.Write(heightTile,sizeof(heightTile));
	}
	//lpTilecell = &m_fieldArray[10*63+10];
	//lpTilecell->m_byHeight = 15;
	//lpTilecell = &m_fieldArray[11*63+10];
	//lpTilecell->m_byHeight = 20;
	return TRUE;
}
HRESULT CWorld::BuildField(LPDIRECT3DDEVICE8 pd3dDevice)
{
	int dwNumringments = 63;
  HRESULT hr;

	// 각 타일 셀의 기본적인 텍스춰 좌표와 버텍스를 만든다.
	CTile* lpTilecell;
	CString strTexture;
	TCHAR* pChar;
	int nMaxTile = 0;
	ZeroMemory(m_byTileUsed,65000*sizeof(DWORD));
	int xxxx,yyyy;
	for(yyyy = 0; yyyy < dwNumringments; yyyy++ )
	{
		for(xxxx=0; xxxx < dwNumringments; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			m_byTileUsed[lpTilecell->m_wTileNum] = TRUE;
			if(lpTilecell->m_wTileNum > nMaxTile) 
				nMaxTile = lpTilecell->m_wTileNum;
		}
	}
	// 사용한 타일 상태를 최적화 타일 순번으로 교체한다.
	// 맵핑 소스도 만든다.
	for(int i = 0, nTileCount = 1; i < nMaxTile; i++)
	{
		if(m_byTileUsed[i])
		{
			m_byTileUsed[i] = nTileCount++;
		}
	}
	// 최적화 타일 값을 넣는다.
	for(yyyy = 0; yyyy < dwNumringments; yyyy++ )
	{
		for(xxxx=0; xxxx < dwNumringments; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			lpTilecell->m_wTileOpt = m_byTileUsed[lpTilecell->m_wTileNum] - 1;
		}
	}
	m_nFieldVBMax = nTileCount / (16 * 16);
	if(nTileCount % (16 * 16)) m_nFieldVBMax++;
	// 텍스춰 생성하기 
	D3DLOCKED_RECT lockedRect[MAX_FIELDVERTEX];
	for(i = 0; i < m_nFieldVBMax; i++)
	{
		D3DXCreateTexture(pd3dDevice, 512, 512,D3DX_DEFAULT,0,D3DFMT_X1R5G5B5,D3DPOOL_MANAGED,&m_fieldTexture[i]);
		LPDIRECT3DTEXTURE8 pTexture = m_fieldTexture[i];
		HRESULT a = pTexture->LockRect(0,&lockedRect[i],0,0);//,D3DLOCK_DISCARD);
	}
	for(i = 0; i < nMaxTile; i++)
	{
		if(m_byTileUsed[i])
		{
			int nIndex = ((m_byTileUsed[i] - 1) % 256);
			int nTexture = (m_byTileUsed[i] - 1) / 256;//lpTilecell->GetTextureIndex();
			LPWORD lpData = (LPWORD)lockedRect[ nTexture ].pBits;
			int nPitch = lockedRect[ nTexture ].Pitch / 2;
			Tile* pTile = (Tile*)m_tileAry.GetAt(i);
			if(pTile == NULL) continue;
			int x = nIndex % 16;
			int y = nIndex / 16;
			if(y == 15)
			{
				int a;
				a++;
			}
			LPWORD lpDst = (LPWORD)&lpData[(y*32)*nPitch+(x*32)];
			for(int yy = 0; yy < 32; yy++)
			{
				memcpy(&lpDst[yy*nPitch],&pTile->pTileOrg->data[yy*32*2],32*2);
			}

		}
	}
	// 텍스춰 Unlock하기 
	for(i = 0; i < m_nFieldVBMax; i++)
		m_fieldTexture[i]->UnlockRect(0);

	for(yyyy = 0; yyyy < dwNumringments; yyyy++ )
	{
		for(xxxx=0; xxxx < dwNumringments; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			lpTilecell->Compute(xxxx,yyyy);
			m_byTileUsed[lpTilecell->m_wTileNum] = TRUE;
			if(lpTilecell->m_wTileNum > nMaxTile) 
				nMaxTile = lpTilecell->m_wTileNum;
		}
	}
	BuildVertex(pd3dDevice);
	BuildGridVertex(pd3dDevice);

	return S_OK;
}
HRESULT CWorld::BuildVertex(LPDIRECT3DDEVICE8 pd3dDevice)
{
	HRESULT hr;
	CTile* lpTilecell;
	// 만들어진 버텍스를 높이에 따라 조절한다.
	DWORD dwNumringments = 63;
	for(DWORD yyyy = 0; yyyy < dwNumringments; yyyy++ )
  {
    for( DWORD xxxx=0; xxxx < dwNumringments; xxxx++ )
    {
			UpdateHeight(xxxx,yyyy);
    }
  }
	// 버틱스 갯수 카운트 
	ZeroMemory(m_fieldVertexNum,sizeof(DWORD)*MAX_FIELDVERTEX);
	for(yyyy = 0; yyyy < dwNumringments; yyyy++ )
	{
		for(DWORD xxxx=0; xxxx < dwNumringments; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			m_fieldVertexNum[lpTilecell->GetTextureIndex()] += 6;
		}
	}
	ZeroMemory(m_fieldVertexBuffer,sizeof(LPDIRECT3DVERTEXBUFFER8)*MAX_FIELDVERTEX);
	UpdateVertex(pd3dDevice);
	return hr;
}
void CWorld::UpdateHeight(int x,int y,BOOL bComputeTile)
{
	CTile* lpTilecell,* pTileTemp;

	lpTilecell = &m_fieldArray[y*63+x];
	lpTilecell->m_bUpdate = TRUE;
	if(bComputeTile)
		lpTilecell->Compute(x,y);

	bComputeTile = FALSE;
	//pTileArray[(*pnTileNum)++] = lpTilecell;

	if(y > 0) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x+0)];
		if(bComputeTile) pTileTemp->Compute(x+0,y-1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
		  lpTilecell->m_vec0 = pTileTemp->m_vec1;
			lpTilecell->m_vec2 = pTileTemp->m_vec3;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y > 0 && x > 0) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x-1)];
		if(bComputeTile) pTileTemp->Compute(x-1,y-1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec0 = pTileTemp->m_vec3;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y > 0 && x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x+1)];
		if(bComputeTile) pTileTemp->Compute(x+1,y-1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec2 = pTileTemp->m_vec1;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(x > 0) 
	{
		pTileTemp = &m_fieldArray[(y-0)*63+(x-1)];
		if(bComputeTile) pTileTemp->Compute(x-1,y-0);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec0 = pTileTemp->m_vec2;
			lpTilecell->m_vec1 = pTileTemp->m_vec3;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y-0)*63+(x+1)];
		if(bComputeTile) pTileTemp->Compute(x+1,y-0);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec2 = pTileTemp->m_vec0;
			lpTilecell->m_vec3 = pTileTemp->m_vec1;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x+0)];
		if(bComputeTile) pTileTemp->Compute(x+0,y+1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec1 = pTileTemp->m_vec0;
			lpTilecell->m_vec3 = pTileTemp->m_vec2;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1 && x > 0) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x-1)];
		if(bComputeTile) pTileTemp->Compute(x-1,y+1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec1 = pTileTemp->m_vec2;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1 && x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x+1)];
		if(bComputeTile) pTileTemp->Compute(x+1,y+1);
		if(pTileTemp->m_byHeight > lpTilecell->m_byHeight)
		{
			lpTilecell->m_vec3 = pTileTemp->m_vec0;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	/////////////////////////////////////////
	if(y > 0) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x+0)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
		{
			pTileTemp->m_vec1 = lpTilecell->m_vec0;
			pTileTemp->m_vec3 = lpTilecell->m_vec2;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y > 0 && x > 0) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x-1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
			pTileTemp->m_vec3 = lpTilecell->m_vec0;
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y > 0 && x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y-1)*63+(x+1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
			pTileTemp->m_vec1 = lpTilecell->m_vec2;
		pTileTemp->m_bUpdate = TRUE;
	}
	if(x > 0) 
	{
		pTileTemp = &m_fieldArray[(y-0)*63+(x-1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
		{
			pTileTemp->m_vec2 = lpTilecell->m_vec0;
			pTileTemp->m_vec3 = lpTilecell->m_vec1;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y-0)*63+(x+1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
		{
			pTileTemp->m_vec0 = lpTilecell->m_vec2;
			pTileTemp->m_vec1 = lpTilecell->m_vec3;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x+0)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
		{
			pTileTemp->m_vec0 = lpTilecell->m_vec1;
			pTileTemp->m_vec2 = lpTilecell->m_vec3;
		}
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1 && x > 0) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x-1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
			pTileTemp->m_vec2 = lpTilecell->m_vec1;
		pTileTemp->m_bUpdate = TRUE;
	}
	if(y < 63 - 1 && x < 63 - 1) 
	{
		pTileTemp = &m_fieldArray[(y+1)*63+(x+1)];
		if(pTileTemp->m_byHeight < lpTilecell->m_byHeight)
			pTileTemp->m_vec0 = lpTilecell->m_vec3;
		pTileTemp->m_bUpdate = TRUE;
	}
}
// 수정된 타일을 찾아 버틱스를 업데이트 한다.
HRESULT CWorld::UpdateVertex(LPDIRECT3DDEVICE8 pd3dDevice) //,CTile* pTileArray[],int nTileNum)
{
	CTile* lpTilecell;
	HRESULT hr;
	// 새로운 버택스 버퍼 만들기 
	FOGVERTEX* pVerticesArray[MAX_FIELDVERTEX];
	ZeroMemory(pVerticesArray,sizeof(DWORD)*MAX_FIELDVERTEX);

	for(DWORD yyyy = 0; yyyy < 63; yyyy++ )
	{
		for(DWORD xxxx=0; xxxx < 63; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			if(lpTilecell->m_bUpdate)
			{
				int nTextureIndex = lpTilecell->GetTextureIndex();
				if(m_fieldVertexBuffer[nTextureIndex])
				{
					SAFE_RELEASE(m_fieldVertexBuffer[nTextureIndex]);
					m_fieldVertexBuffer[nTextureIndex] = NULL;
				}
				lpTilecell->m_bUpdate = FALSE;
			}
		}
	}
	/*
	for(int i = 0; i < nTileNum; i++ )
	{
		int nTextureIndex = pTileArray[i]->GetTextureIndex();
		SAFE_RELEASE(m_fieldVertexBuffer[nTextureIndex]);
		m_fieldVertexBuffer[nTextureIndex] = NULL;
	}
	*/
	for(int i = 0; i < m_nFieldVBMax; i++ )
	{
		if(m_fieldVertexBuffer[i] == NULL) 
		{
			hr = pd3dDevice->CreateVertexBuffer( m_fieldVertexNum[i]*sizeof(FOGVERTEX),
																					 D3DUSAGE_WRITEONLY, D3DFVF_FOGVERTEX,
																					 D3DPOOL_MANAGED, &m_fieldVertexBuffer[i] );
			hr = m_fieldVertexBuffer[i]->Lock( 0, m_fieldVertexNum[i]*sizeof(FOGVERTEX),(BYTE**)&pVerticesArray[i], 0 );
		}
	}
	// 버텍스 생성 
	FOGVERTEX* pVertices;
	for(yyyy = 0; yyyy < 63; yyyy++ )
	{
		for(DWORD xxxx=0; xxxx < 63; xxxx++ )
		{
			lpTilecell = &m_fieldArray[yyyy*63+xxxx];
			pVertices = pVerticesArray[lpTilecell->GetTextureIndex()];
			if(pVertices == NULL) continue;

			pVertices->p   = lpTilecell->m_vec0; // 0
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu0;
			pVertices->tv  = lpTilecell->m_tv0;
		//	pVertices->color =0xffffffff;
			pVertices++;

			pVertices->p   = lpTilecell->m_vec1; // 1
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu1;
			pVertices->tv  = lpTilecell->m_tv1;
		//	pVertices->color= 0xffffffff;
			pVertices++;

			pVertices->p   = lpTilecell->m_vec2; // 2
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu2;
			pVertices->tv  = lpTilecell->m_tv2;
		//	pVertices->color =0xffffffff;
			pVertices++;

			/////////////////////////////////////////////
			// Add two vertices to the strip at each step
			pVertices->p   = lpTilecell->m_vec1; // 1
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu1;
			pVertices->tv  = lpTilecell->m_tv1;
		//	pVertices->color =0xffffffff;
			pVertices++;

			pVertices->p   = lpTilecell->m_vec2; // 2
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu2;
			pVertices->tv  = lpTilecell->m_tv2;
		//	pVertices->color =0xffffffff;
			pVertices++;

			pVertices->p   = lpTilecell->m_vec3; // 3
			pVertices->n.x = 0;
			pVertices->n.y = 1;
			pVertices->n.z = 0;
			pVertices->tu  = lpTilecell->m_tu3;
			pVertices->tv  = lpTilecell->m_tv3;
		//			pVertices->color =0xffffffff;
			pVertices++;
			pVerticesArray[lpTilecell->GetTextureIndex()] = pVertices;
			lpTilecell->m_bUpdate = FALSE;
		}
	}
	for(i = 0; i < m_nFieldVBMax; i++ )
	{
		if(pVerticesArray[i]) 
		{
			m_fieldVertexBuffer[i]->Unlock();
		}
	}
	return hr;
}
int CompareField_(const void *arg1,const void *arg2)
{
	if(*(LPWORD)arg1 > *(LPWORD)arg2)
		return 1;
  if(*(LPWORD)arg1 < *(LPWORD)arg2)
		return -1;
	return 0;
}
BOOL CWorld::LoadTile(LPCTSTR lpszFileName,CTile* pTileArray)
{
	// 타일 해더 파일을 읽기 
	CFile file;
	if(!file.Open(MakePath( DIR_WORLDTILE, lpszFileName ),CFile::modeRead))
		return FALSE;
	CArchive ar(&file,CArchive::load);
	Tile* pTile;
	int nSize;
	ar >> m_nSizeTile >> m_nSizeExt;
	if(m_nSizeExt)
	{
		nSize = TILEEXT + m_nSizeExt;
		m_pOffsetTile = new DWORD[nSize];
		ar.Read(m_pOffsetTile,m_nSizeTile*sizeof(DWORD));
		ar.Read(m_pOffsetTile+TILEEXT,m_nSizeExt*sizeof(DWORD));
	}
	else
	{
		nSize = m_nSizeTile;
		m_pOffsetTile = new DWORD[nSize];
		ar.Read(m_pOffsetTile,nSize*sizeof(DWORD));
	}
	m_tileAry.SetSize(nSize);
	file.Close();

	// 실제 타일 읽을 준비 하기 
	Tile* pTile2;
	WORD nTile;
	//int nSize;
	if(m_nSizeExt)
		nSize = TILEEXT + m_nSizeExt;
	else  
		nSize = m_nSizeTile;
	CString strPath = MakePath( DIR_WORLDTILE, "sample.tle" );
	CString strNum;
	CString strFileName;
	strPath = strPath.Left(strPath.GetLength()-4);

	// 기본 타일 읽기 
	CFile file1;
	int nOldFileNum = -1;
	for(int i = 0; i < 2; i++)
	{
		if(m_tileAry.GetAt(i))
			continue;
		if(nOldFileNum != i/TILEFILENUM)
		{
			strNum.Format("%02d",i/TILEFILENUM);
			strFileName = strPath + strNum + ".tmg";		
			if(nOldFileNum != -1) file1.Close();
			if(!file1.Open(strFileName,CFile::modeRead)) 
				return FALSE;
			nOldFileNum = i/TILEFILENUM;
		}
		pTile = new Tile;
		file1.Read(pTile,sizeof(Tile));
		pTile->pTileOrg = new TILEORG;
		file1.Read(pTile->pTileOrg->data,TILEIMGSIZE);
		pTile->pTileOrg->nSum = 1;
		pTile->bBlock = TRUE;
		m_tileAry.SetAt(i,pTile);
	}
	if(nOldFileNum != -1)
		file1.Close();

	// 필드 타일 읽기 
	nSize = 63*63;//pWordArray->GetSize();
	LPWORD lpSortArray = new WORD[nSize];
	for(i = 0; i < nSize; i++)
		lpSortArray[i] = pTileArray[i].m_wTileNum;
	qsort((void *)lpSortArray,(size_t)nSize,sizeof(WORD),CompareField_);
	nOldFileNum = -1;
	for(i = 0; i < nSize; i++)
	{
		nTile = lpSortArray[i];
		if(m_tileAry.GetAt(nTile))
			continue;
		if(nOldFileNum != nTile/TILEFILENUM)
		{
			strNum.Format("%02d",nTile/TILEFILENUM);
			strFileName = strPath + strNum + ".tmg";		
			if(nOldFileNum != -1) file1.Close();
			if(!file1.Open(strFileName,CFile::modeRead)) 
			{
				delete lpSortArray;	return FALSE;
			}
			nOldFileNum = nTile/TILEFILENUM;
		}
		file1.Seek(m_pOffsetTile[nTile],CFile::begin);
		pTile = new Tile;
		file1.Read(pTile,sizeof(Tile));
		// 오리지날인가?
		if(pTile->nIndex == 0xffffffff)
		{
			// 오리지날이다.
			pTile->pTileOrg = new TILEORG;
			file1.Read(pTile->pTileOrg->data,TILEIMGSIZE);
			pTile->pTileOrg->nSum = 1;
		}
		else
		{
			// 오리지날이 아니라면, 오리지날의 색인 값을 이용해 포인터를 읽어본다.
			// 존재하는가?
			int nIndex = pTile->nIndex;
			if(nTile >= TILEEXT)
				nIndex += TILEEXT;
			if(m_tileAry[nIndex])
				// 존재한다. 오리지날이 로드되어 있다. 
				pTile->pTileOrg = ((Tile*)m_tileAry[nIndex])->pTileOrg;
			else
			{
				// 오리지날이 로드되어 있지 않으므로 로드한다.
				CFile file2,* pCurFile;
				if(nTile/TILEFILENUM == nIndex/TILEFILENUM)
					pCurFile = &file1;
				else
				{
					pCurFile = &file2;
					strNum.Format("%02d",nIndex/TILEFILENUM);
					strFileName = strPath + strNum + ".tmg";
					if(!file2.Open(strFileName,CFile::modeRead)) 
					{
						delete lpSortArray;	return FALSE;
					}
				}
				pCurFile->Seek(m_pOffsetTile[nIndex],CFile::begin);
				pTile2 = new Tile;
				pCurFile->Read(pTile2,sizeof(Tile));
				pTile2->pTileOrg = new TILEORG;
				pCurFile->Read(pTile2->pTileOrg->data,TILEIMGSIZE);
				pTile2->pTileOrg->nSum = 1;
				m_tileAry.SetAt(nIndex,pTile2);
				pTile->pTileOrg = pTile2->pTileOrg;
			}
			pTile->pTileOrg->nSum++;
		}
		m_tileAry.SetAt(nTile,pTile);
	}
	if(nOldFileNum != -1)
		file1.Close();
	delete lpSortArray;
	return TRUE;
}
void CWorld::FreeTile()
{
	delete m_pOffsetTile;
	for(DWORD i = 0; i < m_tileAry.GetSize(); i++)
	{
		if(m_tileAry.GetAt(i))
		{
			Tile* pTile = (Tile*)m_tileAry.GetAt(i);
			LPTILEORG lpTileOrg = pTile->pTileOrg;
			if(lpTileOrg->nSum == 1)
				delete lpTileOrg;
			else
				lpTileOrg->nSum--;
			delete pTile;
		}
	}
}
// 
// GetFieldHight(D3DXVECTOR vecPos)
// vecPos의 해당하는 필드의 높이를 정교하게 계산해서 돌려 준다. 
// return 값은 y 좌표에 해당된다.
//
FLOAT CWorld::GetFieldHight(D3DXVECTOR3 vecPos)
{
	// 이 버전은 빠른 처리를 위해 편법을 사용했다.

	CTile* pTile =GetTile(vecPos);

	FLOAT fU = vecPos.x - ((int)(vecPos.x / 4.0f) * 4.0f);
	FLOAT fV = vecPos.z - ((int)(vecPos.z / 4.0f) * 4.0f);

	// 1 : intersection.U = TILECELLWIDTH : x
	FLOAT fy1;
	FLOAT fy2;
	FLOAT fHeight;

	if(fU + fV < 4.0f)
	{
		fy1 = fV * (pTile->m_vec1.y - pTile->m_vec0.y) / 4.0f; // 1 : intersection.U = (m_vec1.y - m_vec0.y) : y;
		fy2 = fU * (pTile->m_vec2.y - pTile->m_vec0.y) / 4.0f; // 1 : intersection.V = (m_vec2.y - m_vec0.z) : y;
		fy1 += pTile->m_vec0.y;
		fy2 += pTile->m_vec0.y;
		fHeight = fy1 + (fy2 - pTile->m_vec0.y);
	}
	else
	{
		fy1 = (4.0f - fU) * (pTile->m_vec1.y - pTile->m_vec3.y) / 4.0f; // 1 : intersection.U = (m_vec1.y - m_vec0.y) : y;
		fy2 = (4.0f - fV) * (pTile->m_vec2.y - pTile->m_vec3.y) / 4.0f; // 1 : intersection.V = (m_vec2.y - m_vec0.z) : y;
		fy1 += pTile->m_vec3.y;
		fy2 += pTile->m_vec3.y;
		fHeight = fy1 + (fy2 - pTile->m_vec3.y);
	}
	return fHeight;
/*
  // 이 버전은 내적을 구해서 높이를 구한다. 정석이지만 속도가 느리다.

	D3DXVECTOR3 edge1;
	D3DXVECTOR3 edge2;
	D3DXVECTOR3 orig;
	D3DXVECTOR3 normal, L1;
	D3DXVECTOR3 v0;

	orig = vecPos;
	orig.y = 0;

	if(fU + fV < 4.0f)
	{
		edge1 = pTile->m_vec1 - pTile->m_vec0;
		edge2 = pTile->m_vec2 - pTile->m_vec0;
		v0 = pTile->m_vec0;
	}
	else
	{
		edge1 = pTile->m_vec1 - pTile->m_vec3;
		edge2 = pTile->m_vec2 - pTile->m_vec3;
		v0 = pTile->m_vec3;
	}

	D3DXVECTOR3 pvec;
	D3DXVECTOR3 dir(0,-1,0);
	D3DXVec3Cross( &pvec, &dir, &edge2 );
	FLOAT det = D3DXVec3Dot( &edge1, &pvec );
	D3DXVECTOR3 tvec;
	if( det > 0 )
		tvec = orig - v0;
	else
	{
		tvec = v0 - orig;
		det = -det;
	}
	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross( &qvec, &tvec, &edge1 );

	// Calculate t, scale parameters, ray intersects triangle
	return -(D3DXVec3Dot( &edge2, &qvec ) * (1.0f / det));
*/
	return 0.0f;
}
//
// 필드의 클릭한 지점을 얻기
// [in] point는 클라이언트 화면의 좌표 
// [out] pVector는 목표 좌표 
//
BOOL CWorld::ClientPointToVector(RECT rect,POINT point,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pVector)
{
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;

	GetPickRay(rect,point,pmatProj,pmatView,&vPickRayOrig,&vPickRayDir);

	D3DXINTERSECTINFO intersection;

	POINT pt;
	FLOAT fU;                        // Barycentric Hit Coordinates    
	FLOAT fV;                        // Barycentric Hit Coordinates
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = FAR_PLANE;                    

	D3DXVECTOR3 vecIntersect;

	CTile* lpTile;
	CTile* lpTileSelect = NULL;

	BOOL bTriangle1 = FALSE;

	// Collect all intersections
	for(int y = 0; y < 63; y++)
	{
		for(int x = 0; x < 63; x++)
		{
			lpTile = &m_fieldArray[y*63+x];
			if(IntersectTriangle(lpTile->m_vec0,lpTile->m_vec2,lpTile->m_vec1, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
			{
				if(fDist < fNearDist)
				{
					fNearDist = fDist;
					*pVector = vecIntersect;
					lpTileSelect = lpTile;
				}
			}
			else
			if(IntersectTriangle(lpTile->m_vec3,lpTile->m_vec1,lpTile->m_vec2, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
			{
				if(fDist < fNearDist)
				{
					fNearDist = fDist;
					*pVector = vecIntersect;
					lpTileSelect = lpTile;
				}
			}
		}
	}
	if(lpTileSelect)
	{
		/*
		// 1 : intersection.U = TILECELLWIDTH : x
		pVector->x = pt.x * TILECELLWIDTH  + intersection.U * TILECELLWIDTH ; // / 1; 나누기 1은 할 필요 없음 
		pVector->z = pt.y * TILECELLHEIGHT + intersection.V * TILECELLHEIGHT; // / 1; 나누기 1은 할 필요 없음 
		FLOAT fy1;
		FLOAT fy2;
		if(bTriangle1)
		{
			fy1 = intersection.V * (lpTileSelect->m_vec1.y - lpTileSelect->m_vec0.y); // 1 : intersection.U = (m_vec1.y - m_vec0.y) : y;
			fy2 = intersection.U * (lpTileSelect->m_vec2.y - lpTileSelect->m_vec0.y); // 1 : intersection.V = (m_vec2.y - m_vec0.z) : y;
			fy1 += lpTileSelect->m_vec0.y;
			fy2 += lpTileSelect->m_vec0.y;
			pVector->y = fy1 + (fy2 - lpTileSelect->m_vec0.y);
		}
		else
		{
			fy1 = (1.0f - intersection.U) * (lpTileSelect->m_vec1.y - lpTileSelect->m_vec3.y); // 1 : intersection.U = (m_vec1.y - m_vec0.y) : y;
			fy2 = (1.0f - intersection.V) * (lpTileSelect->m_vec2.y - lpTileSelect->m_vec3.y); // 1 : intersection.V = (m_vec2.y - m_vec0.z) : y;
			fy1 += lpTileSelect->m_vec3.y;
			fy2 += lpTileSelect->m_vec3.y;
			pVector->y = fy1 + (fy2 - lpTileSelect->m_vec3.y);
			TRACE("%f\n",pVector->y);
		}
		*/
	//	TRACE("Intersect %f, %f, %f\n",pVector->x,pVector->y,pVector->z);
		return TRUE;

		//pVector->y = pt.y * TILECELLHEIGHT
		//pVector->x = 
		//pVector->y
		//pVector->z

	}
	return FALSE;
}
#endif
