#ifndef LANDSCAPE_H
#define LANDSCAPE_H

//#define MAP_64
#ifndef __MAP_SIZE

//#include "CMclCritSec.h"
//#ifdef MAP_256
// ------- 512x512 MAP -------
//#define MAP_SIZE (256)
//#define NUM_PATCHES_PER_SIDE (64)
//#else	// MAP_256
//#ifdef MAP_128
// ------- 128x128 MAP -------
//#define MAP_SIZE (128)
//#define NUM_PATCHES_PER_SIDE (16)
//#else	// MAP_128
// ------- 64x64 MAP -------
#define MAP_SIZE (128)
#define NUM_PATCHES_PER_SIDE (16) // 128(MAP_SIZE)에 16(NUM_PATCHES_PER_SIDE)개의 패치가 있음 
//#endif	// MAP_128
//#endif	// MAP_256

#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE) // 패치의 한 변의 크기
#define LIGHTMAP_SIZE ((PATCH_SIZE-1)*NUM_PATCHES_PER_SIDE) // 랜드스케이프 하나에 사용될 라이트맵의 한 변의 크기

#define D3DFVF_D3DLANDSCAPEVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2)
#define D3DFVF_WATERVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1) 
#define D3DFVF_HGTATTRVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE) 

#define LANDREALSCALE ( MAP_SIZE * g_MPU )

#define INIT_HEIGHT 0
#define INIT_TEX 0

//#define MAX_LINKLEVEL 5		// 5 : 오브젝트크기 64미터까지 커버됨. 더큰오브젝트를 원할땐 숫자를 늘이시오. Jeff에게 협의바람.
//#define MAX_LINKLEVEL 6		// 5 : 오브젝트크기 64미터까지 커버됨. 더큰오브젝트를 원할땐 숫자를 늘이시오. Jeff에게 협의바람.

#define MAX_LAYER 20

#define HGT_NOWALK 1000.0f // 걷기, 점프 금지 
#define HGT_NOFLY  2000.0f // 비행 금지 
#define HGT_NOMOVE 3000.0f // 걷기, 비행 금지 
#define HGT_DIE    4000.0f // 죽음 

#define HATTR_NONE   0 
#define HATTR_NOWALK 1 // 걷기, 점프 금지 
#define HATTR_NOFLY  2 // 비행 금지 
#define HATTR_NOMOVE 3 // 걷기, 비행 금지 
#define HATTR_DIE    4 // 죽음 

#define _WATERHEIGHT 96.0f // 물 높이 
#define _CLOUDHEIGHT 80.0f // 구름 높이 

struct WATERVERTEX { D3DXVECTOR3 p; D3DXVECTOR3 n; DWORD color; FLOAT u, v; };
struct HGTATTRVERTEX { D3DXVECTOR3 p; D3DXVECTOR3 n; DWORD color; };

#ifdef __VLANDATTR
#define LAND_VERSION 3
#else
#define LAND_VERSION 2
#endif

#ifdef __BS_OVERLAP_LAYER
#undef LAND_VERSION
#define LAND_VERSION 4
#endif 

struct D3DLANDSCAPEVERTEX
{
	D3DXVECTOR3 p; 
	D3DXVECTOR3 n;
	FLOAT       tu1, tv1;
	FLOAT       tu2, tv2;
};

class CObj;
class CWorld;

// 랜드스케이프에서 사용하는 레이어 클래스
// 하나당 한 종류의 텍스쳐가 어떻게 깔려있는지의 정보를 가진다
struct CLandLayer
{
public:
	BOOL m_bVisible;
	WORD m_nTex; // 이 레이어가 담당하는 텍스쳐 ID
	BOOL m_aPatchEnable[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE]; // 패치의 표시 상태를 저장하는 배열
	LPDIRECT3DTEXTURE9 m_pLightMap; // 라이트맵 포인터
#ifdef __BEAST
#	ifdef __16BITLIGHT
	WORD* m_pTempLightMap; // 에디터용 임시 라이트 맵 저장소
#	else
	DWORD* m_pTempLightMap;
#endif
#endif

	CLandLayer(LPDIRECT3DDEVICE9 pd3dDevice,WORD nTex);
	~CLandLayer();
	void SetPatchEnable(int x, int z,BOOL bEnable); // 이 부분의 패치를 표시할지 말지를 설정
	BOOL GetPatchEnable(int x, int z) { return m_aPatchEnable[x+z*NUM_PATCHES_PER_SIDE]; } // 이 부분의 패치의 표시 상태를 리턴
};
#define WTYPE_NONE  0x00
#define WTYPE_CLOUD 0x01
#define WTYPE_WATER 0x02

#define LOCA_MASK 0x0f
#define LOCA_A    0x10
#define LOCA_B    0x20
#define LOCA_C    0x40
#define LOCA_D    0x80

typedef struct
{
	BYTE byWaterHeight;
	BYTE byWaterTexture;

} WATERHEIGHT,* LPWATERHEIGHT;

#ifdef __N_WATERFRAME_050202
typedef struct
{
	DWORD					WaterVertexNum;
	LPDIRECT3DVERTEXBUFFER9	pVB;
}WATERVERTEXBUFFER, * LPWATERVB;
#endif//__N_WATERFRAME_050202

#define DEFAULT_WATER	0

#define MASK_WATERFRAME	0xfc

// 랜드스케이프 클래스
// 패치의 집합으로서 월드의 일부를 구성
class CLandscape
{
	LPDIRECT3DDEVICE9 m_pd3dDevice; // d3d 디바이스
	BOOL m_bModified; // 이 랜드스케이프의 내용이 바뀌면 세팅됨.
	CWorld* m_pWorld; // 월드의 포인터

protected:
static FLOAT m_fCloud_u1, m_fCloud_v1 ;
static FLOAT m_fCloud_u2, m_fCloud_v2 ;
#ifndef __WATER_EXT
static FLOAT m_fWaterFrame;
#endif //__WATER_EXT
	
	FLOAT* m_pHeightMap;											// 높이 맵 (실제 할당 주소)
	//FLOAT* m_pWaterMap;	  											// 물, 구름 
	//BYTE*  m_pWaterHeight;
	//
#ifndef __NewWater
	bool usingNewWater = false;
	WATERHEIGHT m_aWaterHeight[MAP_SIZE * MAP_SIZE];
#else
	WATERHEIGHT m_aWaterHeight[NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE];
#endif
	BYTE m_aLandAttr[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];

	//FLOAT* m_pWaterHeightMap;

	FLOAT GetHeightMap( int nOffset ) { 
		FLOAT fHeight = m_pHeightMap[ nOffset ];
		if( fHeight >= HGT_NOWALK ) {
			if( fHeight >= HGT_DIE )
				return m_pHeightMap[ nOffset ] - HGT_DIE; 
			if( fHeight >= HGT_NOMOVE )
				return m_pHeightMap[ nOffset ] - HGT_NOMOVE; 
			if( fHeight >= HGT_NOFLY )
				return m_pHeightMap[ nOffset ] - HGT_NOFLY; 
			return m_pHeightMap[ nOffset ] - HGT_NOWALK; 
		}
		return fHeight;	
	}
	void SetHeightAttribute( int x, int z, DWORD dwAttr );
	int GetHeightAttribute( int x, int z );
	LPWATERHEIGHT GetWaterHeight(int x, int z );
	
	HRESULT MakeWaterVertexBuffer();
	HRESULT MakeLandAttrVertexBuffer();
	HRESULT MakeHgtAttrVertexBuffer();
	
	BOOL m_bDirty;													// 버텍스 버퍼를 수정할 필요가 있을 경우 TRUE로 세팅

	BOOL m_bVisible;				 								// 컬링된 결과
	D3DXVECTOR3  m_avBounds[8];										// 컬링에 사용할 바운드 박스 벡터
	D3DXPLANE    m_aplaneBounds[6];									// 컬링에 사용할 바운드 박스 평면

	LPDIRECT3DVERTEXBUFFER9 m_pVB;									// 버텍스 버퍼

public:
	int m_nWorldX,m_nWorldY;										// 이 랜드스케이프의 좌하단 월드좌표

protected:
	LPDIRECT3DVERTEXBUFFER9 m_pCloudVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_pWaterVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_pHgtAttrVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_pLandAttrVertexBuffer;
	DWORD m_nCloudVertexNum;
	DWORD m_nWaterVertexNum;
#ifdef __N_WATERFRAME_050202
	WATERVERTEXBUFFER		*m_pWaterVB;
#endif//__N_WATERFRAME_050202
	DWORD m_nHgtAttrVertexNum;
	DWORD m_nLandAttrVertexNum;
	
	void DeleteLayer( WORD nTex ); // 레이어 삭제
	void OptimizeLayer();
	//void OptimizeLayer( int x, int y );
	CLandLayer* NewLayer( WORD nTex, BOOL bOverlap = FALSE );  // 지정한 레이어의 포인터를 리턴

#ifdef __BEAST // 이하는 에디터에서만 사용
	BOOL CheckLayer(CLandLayer* pLayer,int x,int z); // 지정 패치에 지정한 레이어의 텍스쳐가 표시되는지를 체크
	void CheckAllLayer(int x,int z); // 지정 패치의 모든 레이어의 표시 여부를 체크
	void UpdateAllLayer(void); // 모든 패치의 모든 레이어의 표시 여부를 체크

	int	 GetTex(int x, int z);
	void SetTex(int x, int z, WORD nTex, BYTE nOpacity,BOOL bLandMark=FALSE); // 특정 위치에 지정된 텍스쳐를 그린다.
	void SetCol(int x, int z, DWORD color); // 특정 위치에 지정된 색을 덧칠한다.
	DWORD GetCol(int x, int z); // 특정 위치의 색을 리턴
	void SetColAllLayer(int x, int z, DWORD color);
	void SetPatchAlphaLayer(CLandLayer* pLayer, int x, int z, BYTE nAlpha);
	void SetBlankPatch(int x, int z);
	void SetLandMark(int x, int z,WORD nTex,BOOL bEnable);
	void SetWaterHeight(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture );
#ifdef __CSC_UNDO_REDO
public:
	CLandLayer* AddLayerBeast(WORD nTex);
	inline void SetLandMarkBeast(int x, int z,WORD nTex,BOOL bEnable) {SetLandMark(x, z, nTex, bEnable);};
	inline void OptimizeLayerBeast() {OptimizeLayer();};

	inline DWORD GetWaterHeightBeast(int x, int z) { return usingNewWater ? m_aWaterHeight[x + z * MAP_SIZE].byWaterHeight : m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterHeight; };
	inline DWORD GetWaterTexBeast(int x, int z) { return usingNewWater ? m_aWaterHeight[x + z * MAP_SIZE].byWaterTexture : m_aWaterHeight[x + z * NUM_PATCHES_PER_SIDE].byWaterTexture; };
	void SetWaterHeightBeast(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture);

	//by gmpbigsun
	DWORD m_dwHeightDefault;
#endif //__CSC_UNDO_REDO
#endif // 이상은 에디터에서만 사용
//	BOOL GetLandMark(int x, int z) { return m_aLandMark[x+z*NUM_PATCHES_PER_SIDE]; }

	//BOOL m_aLandMark[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];

public:
	DWORD m_dwVersion;
	CPtrArray m_aLayer; // 이 랜드스케이프에 사용될 레이어들의 배열

	BOOL m_abPatchRendered[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	//CObj* m_aObjLinkMap[ MAP_SIZE * MAP_SIZE ];

static	int	m_nWidthLinkMap[ MAX_LINKLEVEL ];
	CObj**	m_apObjLink[MAX_LINKTYPE][MAX_LINKLEVEL];
	CObj***	GetObjLink( DWORD dwLinkType )	{	return( m_apObjLink[dwLinkType] );	}
	CObj**	GetObjLink( DWORD dwLinkType, DWORD dwLinkLevel )	{	return( m_apObjLink[dwLinkType][dwLinkLevel] );		}

#if !defined(__CORESERVER) && !defined(__CACHESERVER)
	CObj**       m_apObject [ MAX_OBJARRAY ];
	DWORD        m_adwObjNum[ MAX_OBJARRAY ];
	CDWordStack  m_aObjStack[ MAX_OBJARRAY ];

	CTexture m_texMiniMap;
	//LPDIRECT3DTEXTURE9 m_pTexMiniMap;

	CPatch m_aPatches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	// 패치 배열
#endif	// !defined(__CORESERVER) && !defined(__CACHESERVER)

	CLandscape();
	~CLandscape();

	void RemoveLayer_Layer( int nLayer );
	
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	BOOL IsModified() { return m_bModified; } // 이 랜드스케이프가 수정되었는지 체크
	void SetModified( BOOL bModified ) { m_bModified = bModified; }

	int  isVisibile( ) { return m_bVisible; } // 컬링된 결과를 리턴

	void FreeTerrain(); // 지형의 메모리 할당을 해제한다.
	void ResetTerrain( DWORD dwInitHeight ,BYTE* pHeight=NULL); // 지형을 초기화한다.
	void NewLandscape( DWORD dwTextureId ); // 지형을 새로 만든다.

	void SetVertices(); // 버텍스 버퍼 재구성
	void RenderPatches(); // 모든 패치를 모든 레이어별로 그린다.
	HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bLod = TRUE );
	HRESULT RenderWater( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RenderHeightAttribute( LPDIRECT3DDEVICE9 pd3dDevice );

	void CalculateBound(); // 컬링용 바운드 박스 재계산
	void UpdateCull(void); // 각 패치별 컬링과 LOD 적용
	void Cull(); // 컬링

	void SetDirtyPatch(DWORD x,DWORD z);

	FLOAT GetHeight( FLOAT x, FLOAT z );
	FLOAT GetHeight( DWORD x, DWORD z ); // 지정 위치의 높이를 돌려준다.
	FLOAT GetHeight( POINT pos );
	void SetHeight( DWORD x, DWORD z, FLOAT factor ); // 지정 위치의 높이를 설정한다
	void SetHeight( POINT pos, FLOAT factor );

	void ReadLandscape(FILE* fp); // 로드
	void WriteLandscape(FILE* fp); // 세이브
	BOOL SaveLandscape( LPCTSTR lpszFileName, int x = 0, int y = 0 );
	BOOL LoadLandscape( LPCTSTR lpszFileName, int x = 0, int y = 0  );
	void OptimizeObjArray();
	void SetUsedAllObjects();

	//void Execute();
	//void Process();

	void AddObjArray( CObj* pObj );
	void RemoveObjArray( CObj* pObj );
	void RemoveObjArray( int nIndex );

	BOOL InsertObjLink( CObj* pObj );
	BOOL RemoveObjLink( CObj* pObj );
	BOOL SetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj );
	CObj* GetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel );

	int GetObjArraySize();

//LightMap=============================================================================================
#ifdef __BS_SUPPORT_LIGHTMAP
	BOOL MakeLightMap( );
	void FillLightColor( D3DXVECTOR3* pLightDir );
	float ComputeShade(DWORD cellRow, DWORD cellCol, D3DXVECTOR3* dirLight );
	void FillLightColor_shadow( int x, int z, DWORD* pTempLightMap, DWORD* pBits  );
#endif

protected:
	CLandLayer* m_pLightMapLayer;	//gmpbigsun(20110307): 가장 나중에 render되어야할 lightmap
//LightMap=============================================================================================

friend CWorld;
};

//gmpbigsun : 이제 MPU는 변한다. 090810
#define MPU  (g_MPU)
//#define g_MPU  4

void SetLODDetail( int nDetail );
//extern DWORD LODLEVELRANGE1;
//extern DWORD LODLEVELRANGE2;
//extern DWORD LODLEVELRANGE3;

#else	// not __MAP_SIZE

#define MAP_SIZE (128)
#define NUM_PATCHES_PER_SIDE (16)
// #define g_MPU 4 // METER PER UNIT		//gmpbigsun:: org 4

#endif	// __MAP_SIZE
#endif	// LANDSCAPE_H