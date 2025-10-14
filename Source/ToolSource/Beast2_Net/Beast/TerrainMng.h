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
#ifdef __WATER_EXT
	FLOAT fWaterFrame;
#endif //__WATER_EXT
	tagWaterTexList()
	{
		ListCnt = 0;
		pList = NULL;
#ifdef __WATER_EXT
		fWaterFrame = 0.1f;
#endif //__WATER_EXT
	}
}WaterTexList, *LPWATERTEXLIST;
#endif//__N_WATERFRAME_050202

#define MAX_WATER	8		//	юс╫ц╥н 

#define MAX_TERRAIN 512

class CTerrainMng
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
public:
	std::map<unsigned long, TERRAIN> m_aTerrain;
#ifdef __N_WATERFRAME_050202
	int				m_nWaterFrame;
	WaterTexList	*m_pWaterIndexList;
#endif//__N_WATERFRAME_050202
#ifdef __WATER_EXT
	FLOAT			m_fWaterFrame[MAX_WATER];
#endif //__WATER_EXT
	
	CTerrainMng();
	~CTerrainMng();

	std::map<unsigned long, TERRAIN>::iterator begin() { return m_aTerrain.begin(); }
	std::map<unsigned long, TERRAIN>::iterator end() { return m_aTerrain.end(); }


	BOOL LoadTexture( DWORD dwId );
#ifdef __BEAST
	DWORD AddTerrain( LPCTSTR lpszFileName ); 
	BOOL RemoveTerrain( DWORD dwId ); 
#endif
	LPTERRAIN GetTerrain(const unsigned long dwId )
	{
		std::map<unsigned long, TERRAIN>::iterator iter = m_aTerrain.find(dwId);
		if (iter != m_aTerrain.end())
			return &iter->second;
		return nullptr;
	}
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	
};

#endif