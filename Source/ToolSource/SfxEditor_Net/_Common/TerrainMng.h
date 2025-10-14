#ifndef		__TERRAINMNG_H__
#define		__TERRAINMNG_H__

typedef struct tagTERRAIN
{
	LPDIRECT3DTEXTURE9 m_pTexture;
	TCHAR m_szTextureFileName[128];
	TCHAR m_szSoundFileName  [128];
	BOOL  m_bBlock;
	DWORD m_dwId;

	BOOL IsValid() { return m_szTextureFileName[0] ? TRUE : FALSE; }

} TERRAIN,* LPTERRAIN;

#ifdef __N_WATERFRAME_050202
typedef struct tagWaterTexList
{
	int  ListCnt;
	int* pList;
	tagWaterTexList()
	{
		ListCnt = 0;
		pList = NULL;
	}
}WaterTexList, *LPWATERTEXLIST;
#endif//__N_WATERFRAME_050202

#define MAX_WATER	8		//	юс╫ц╥н 

#define MAX_TERRAIN 256

class CTerrainMng
{
	int m_nSize;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
public:
	TERRAIN m_aTerrain[MAX_TERRAIN];
#ifdef __N_WATERFRAME_050202
	int				m_nWaterFrame;
	WaterTexList	*m_pWaterIndexList;
#endif//__N_WATERFRAME_050202

	CTerrainMng();
	~CTerrainMng();

	int GetSize() { return m_nSize; }

	BOOL LoadTexture( DWORD dwId );
#ifdef __BEAST
	DWORD AddTerrain( LPCTSTR lpszFileName ); 
	BOOL RemoveTerrain( DWORD dwId ); 
#endif
	LPTERRAIN	GetTerrain( DWORD dwId ) { return &m_aTerrain[ dwId ]; }
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	
};

#endif