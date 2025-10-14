#include "Command.h"

#ifdef __CSC_UNDO_REDO

// Terrain Info Structs
typedef struct TerrainCoord
{
	CPoint m_Point;

	TerrainCoord() : m_Point(0, 0) {}
	inline bool operator < (const TerrainCoord& terraincoord) const { return memcmp(this, &terraincoord, sizeof(TerrainCoord)) < 0; }
} TERRAINCOORD, * LPTERRAINCOORD;

typedef struct TexLandLayer
{
	int m_nLandScape;
	int m_nTex;
	int m_nLayerTemp;
	int m_nX;
	int m_nZ;

	TexLandLayer() : m_nLandScape(-1), m_nTex(-1), m_nLayerTemp(-1), m_nX(0), m_nZ(0) {} 
	inline bool operator < (const TexLandLayer& texlandlayer) const { return memcmp(this, &texlandlayer, sizeof(TexLandLayer)) < 0; }
} TEXLANDLAYER, * LPTEXLANDLAYER;

typedef struct WaterLayer
{
	int m_nTex;
	DWORD m_dwHeight;

	WaterLayer() : m_nTex(0), m_dwHeight(0) {}
	inline bool operator < (const WaterLayer& waterlayer) const { return memcmp(this, &waterlayer, sizeof(WaterLayer)) < 0; }
} WATERLAYER, * LPWATERLAYER;


// 비스트 커맨드 매니저
class CCommandMgrBeast : public CCommandMgr
{
public:
	enum {TERRAIN_HEIGHT_CHG=1, TERRAIN_TEX_CHG, TERRAIN_WATER_CHG, TERRAIN_COLOR_CHG, OBJ_NEW, OBJ_REMOVE, OBJ_MOVE, OBJ_SCALE, OBJ_ROTATE};
	int m_nLandScape;

	map<TERRAINCOORD, FLOAT> m_mapOldTerrainHeigth;
	map<TEXLANDLAYER, int> m_mapOldTexLandLayer;
	map<TEXLANDLAYER, WATERLAYER> m_mapOldWaterTexLayer;
	map<TEXLANDLAYER, DWORD> m_mapOldTexColorLayer;
	map<CObj*, D3DXVECTOR3> m_mapOldObjMove;
	map<CObj*, D3DXVECTOR3> m_mapOldObjScale;
	map<CObj*, D3DXVECTOR3> m_mapOldObjRotate;

public:
	CCommandMgrBeast();
	~CCommandMgrBeast();

public:
	static CCommandMgrBeast* GetInstance();
	void EndCommand();
};

#endif //__CSC_UNDO_REDO