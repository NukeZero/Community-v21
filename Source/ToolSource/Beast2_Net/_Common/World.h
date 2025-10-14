#ifndef __WORLD_2002_1_22
#define __WORLD_2002_1_22

#include <D3D9.h>
#include <list>
#include <basetsd.h> 
#include "Respawn.h"

#ifdef __WORLDSERVER
	#include "ServerDesc.h"
	#include "..\WorldServer\LinkMap.h"
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////
// define 
///////////////////////////////////////////////////////////////////////////////////////////

#define	MAX_ADDOBJS				10240
#define	MAX_DYNAMICOBJ			81920
#define	MAX_BKGND				20480
#define	MAX_MODIFYLINK			4096
#define	MAX_REPLACEOBJ			1024
#define	CLOSEWORKER				(DWORD)-1
#define D3DFVF_BOUNDBOXVERTEX	(D3DFVF_XYZ|D3DFVF_DIFFUSE) 
#define D3DFVF_AXISVERTEX		(D3DFVF_XYZ|D3DFVF_DIFFUSE) 
#define WORLD_WIDTH				( MAP_SIZE * m_nLandWidth )
#define WORLD_HEIGHT			( MAP_SIZE * m_nLandHeight )
#define WORLD_SIZE				( WORLD_WIDTH * WORLD_HEIGHT )
#define MAX_DISPLAYOBJ			5000
#define MAX_DISPLAYSFX			500
#define MINIMAP_SIZE			256
#define WLD_EXTRA_WIDTH			100.0f
#define	MAX_DELETEOBJS			4096

#define MAX_MOVERSELECT			5

////////////////////////////////////////////////////////////////////////////////////////////
// extern 
///////////////////////////////////////////////////////////////////////////////////////////
extern LPDIRECT3DINDEXBUFFER9	g_pIB;
extern LPDIRECT3DINDEXBUFFER9	g_pExIB;
extern LPDIRECT3DINDEXBUFFER9	g_pGridIB;
extern WORD						g_anPrimitive[3];
extern WORD						g_anPrimitiveA[3];
extern WORD						g_anStartIndex[4];

////////////////////////////////////////////////////////////////////////////////////////////
// struct 선언 
///////////////////////////////////////////////////////////////////////////////////////////
struct AXISVERTEX 
{ 
	D3DXVECTOR3 p; 
	DWORD		color; 
};

struct BOUNDBOXVERTEX 
{ 
	D3DXVECTOR3 p; 
	DWORD		color; 
};

typedef struct tagLANDATTR
{
	DWORD dwColor;
	DWORD dwGenRegionAttr;
	DWORD dwPayRegionAttr;
	DWORD dwMusic;
	DWORD dwRepeat;
	DWORD dwCityName;
	DWORD dwLandName;
	DWORD dwPayID;
	CHAR  dwKey[ 64 ];
} LANDATTR,* LPLANDATTR; 

typedef struct tagLIGHTCOLOR 
{ 
	FLOAT r1,g1,b1,r2,g2,b2; 
} LIGHTCOLOR,* LPLIGHTCOLOR;

typedef struct tagREPLACEOBJ
{
	CMover*		pObj;
	DWORD		dwWorldID;
	D3DXVECTOR3	vPos;
	DWORD		dpid;
#ifdef __MSVR
	u_long		uIdofMulti;
#endif	// __MSVR
} REPLACEOBJ, *LPREPLACEOBJ; 


////////////////////////////////////////////////////////////////////////////////////////////
// class foward선언 
////////////////////////////////////////////////////////////////////////////////////////////
class CObj;
class CAnim;
class CItem;
class CSfx;
class CCtrl;
class CMover;
class CLandscape;

#ifdef __BEAST
class	CWorldView;
#endif

class CWorld
{
	friend CLandscape;
	friend CObj;
	friend CMover;
#ifdef __BEAST
	friend CWorldView;
#endif

public:
	CWorld();
	~CWorld();


///////////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
///////////////////////////////////////////////////////////////////////////////
public:
	CLinkMap		m_linkMap;

	CCtrl*			m_apObject[MAX_DYNAMICOBJ];	// dynamic
	u_long			m_dwObjNum;
	CObj*			m_apBkgnd[MAX_BKGND];	// static
	u_long			m_dwBkgndNum;
	float*			m_apHeightMap;	// height

	CRIT_SEC		m_csModifyLink;
	int				m_cbModifyLink;
	CObj*			m_apModifyLink[MAX_MODIFYLINK];

	CRIT_SEC		m_csReplaceObj;
	int				m_cbReplaceObj;
	REPLACEOBJ		m_aReplaceObj[MAX_REPLACEOBJ];

	CRIT_SEC		m_AddRemoveLock;
	int				m_cbAddObjs;
	CObj*			m_apAddObjs[MAX_ADDOBJS];
	BOOL			m_bAddItToGlobalId[MAX_ADDOBJS];

	CDWordStack		m_ObjStack;
	TCHAR			m_lpszWorld[64];
	u_long			m_cbUser;
	HANDLE			m_hIOCP;
	HANDLE			m_hWait;
	volatile long	m_lWork;
	u_long			m_uWorker;
	int				m_nThread;
	list<HANDLE>	m_listthread;
	LPWATERHEIGHT	m_apWaterHeight;		// 사용안함 
	LPBYTE			m_apLocation;			// 사용안함 
	CWorld*			nextptr;	// next world ptr
	BOOL			m_bSrc;
	list<CCtrlRegion*>		m_lspCtrlRegion;

public:
//	Operations
	HANDLE			GetIOCP();
	void			CreateWorker( u_long uWorker );
	void			CloseWorker();
	void			Process();
	void			ModifyView( CCtrl* pCtrl );
	BOOL			ReadWorld( const CRect & rcLandscapce ); 
	short			GetPosAttr( const D3DXVECTOR3 & vPos );
	BOOL			GetRect( CPoint point, CRect & rRect );
	BOOL			PreremoveObj( OBJID objid );
	CObj*			PregetObj( OBJID objid );

private:
	int				IsOverlapped( int c, int p, int r, int w );
	void			AddItToView( CCtrl* pCtrl );
	void			_add();
	void			_process();
	void			_modifylink();
	void			_replace();
	void			_delete();

///////////////////////////////////////////////////////////////////////////////
#else // end __WORLDSERVER
///////////////////////////////////////////////////////////////////////////////

public:
	static D3DLIGHT9	m_light;
	static D3DLIGHT9	m_lightFogSky;
	static CObj*		m_aobjCull[ MAX_DISPLAYOBJ ];
	static CObj*		m_asfxCull[ MAX_DISPLAYSFX ];
	static int			m_nObjCullSize;
	static int			m_nSfxCullSize;
	static CWeather		m_weather;
	static CCamera*		m_pCamera;
	static D3DXMATRIX	m_matProj;
	static D3DCOLOR		m_dwBgColor;
	static FLOAT		m_fFarPlane;
	static FLOAT		m_fNearPlane;
#ifdef __CLIENT
	static BOOL			m_bZoomView;
	static int			m_nZoomLevel;
	static CSkyBox		m_skyBox;
#ifdef __GUILDCOMBAT
	static CMover*		m_amvrSelect[ MAX_MOVERSELECT ];
#endif //__GUILDCOMBAT
#endif
	
	CLandscape**	m_apLand;
	FLOAT			m_fElapsedTime;
	CObjArray       m_aryObjBuffer;
	CObj*           m_pObjFocus;
	OBJID			m_idObjFocusOld;
	DWORD           m_dwAmbient;
	CMapStringToPtr m_mapLight;
	DWORD           m_dwLightIndex;
	int             m_nVisibilityLand;
	FLOAT           m_fFogStartValue;
	FLOAT           m_fFogEndValue;
	FLOAT           m_fFogDensity;
	int             m_nCharLandPosX;
	int             m_nCharLandPosZ;

	int                     m_nBoundBoxVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pBoundBoxVertexBuffer;
    LPDIRECT3DDEVICE9       m_pd3dDevice;        
	D3DMATERIAL9			m_baseMaterial;

	// 디버그 정보 관련 
	static BOOL		m_bViewLODTerrain;
	static BOOL		m_bMiniMapRender;
	BOOL			m_bViewGrid;
	BOOL			m_bViewGridPatch;
	BOOL			m_bViewGridLandscape;
	BOOL			m_bViewWireframe;
	BOOL			m_bViewSkybox;
	BOOL			m_bViewTerrain;
	BOOL			m_bViewAllObjects;
	BOOL			m_bViewFog;
	BOOL			m_bViewWater;
	BOOL			m_bViewName;
	BOOL			m_bViewLight;
	BOOL			m_bViewWeather;
	BOOL			m_bViewBoundBox;
	BOOL			m_bViewLODObj;
	BOOL			m_bCullObj;
	BOOL			m_bProcess;
	BOOL			m_bViewObj;
	BOOL			m_bViewMover;
	BOOL			m_bViewItem;
	BOOL			m_bViewRegion;
	BOOL			m_bViewSpawn;
	BOOL			m_bViewHeightAttribute;
	BOOL			m_bViewIdState;
	//BOOL			m_bFixedHour;
	//int			m_nFixedHour;
	// 디버그 정보 관련 끝 

	#ifdef __BEAST 
		LIGHTCOLOR   m_lightDirect; 
		int          m_nLightHour;		// 조명을 위한 지정 시간 
		int          m_nLightType;		// 0 = direct, 1 = hour
		D3DXVECTOR3  m_vCenter;			// 오브제트를 이동, 회전 시킬 때 중심축 
		CObjArray    m_arySelect;
		CModelObject m_meshGizmo;
		CModelObject m_meshSportlight;
		CModelObject m_meshPointlight;
		int          m_nAxisVertexNum;
		LPDIRECT3DVERTEXBUFFER9		m_pAxisVertexBuffer;
	#endif	// __BEAST

	#ifdef __VSWATER
		LPDIRECT3DVERTEXBUFFER9 m_pWaterVB;
		LPDIRECT3DINDEXBUFFER9  m_pWaterIB;
		DWORD       m_dwNumWaterVertices;
		DWORD       m_dwNumWaterIndices;
		DWORD       m_dwWaterShader;
		DWORD       m_dwWaterSize;
	#endif

	void			Process(FLOAT fElapsedTime);
	static DWORD	GetDiffuseColor() { return D3DCOLOR_ARGB( 255,(int)(m_lightFogSky.Diffuse.r * 255),(int)(m_lightFogSky.Diffuse.g * 255),(int)(m_lightFogSky.Diffuse.b * 255) ); }
	static DWORD	GetAmbientColor() { return D3DCOLOR_ARGB( 255,(int)(m_lightFogSky.Ambient.r * 255),(int)(m_lightFogSky.Ambient.g * 255),(int)(m_lightFogSky.Ambient.b * 255) ); }
	inline void		WorldPosToLand( D3DXVECTOR3 vPos, int& x, int& z ); 
	inline void		WorldPosToLandPos( D3DXVECTOR3 vPos, int& x, int& z ); 
	inline BOOL		LandInWorld( int x, int z );
	inline BOOL		IsVecInVisibleLand( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, int nVisibilityLand );
	inline BOOL		IsVecInRange( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, FLOAT fRadius );
#ifdef __BEAST
	void			SetObjFocus( CObj* pObj );
#else //__BEAST
	void			SetObjFocus( CObj* pObj, BOOL bSend = TRUE );
#endif //__BEAST
	inline CObj*	GetObjFocus() { return m_pObjFocus; }
	inline CLandscape*	GetLandscape( CObj* pObj );
	inline CLandscape*	GetLandscape( D3DXVECTOR3 vPos );
	inline CLandscape*	GetLandscape( int x, int z );
	inline CPatch*		GetPatch( D3DXVECTOR3 vPos );

	void	ProcessAllSfx( void );

///////////////////////////////////////////////////////////////////////////////
#endif	//end !__WORLDSERVER
///////////////////////////////////////////////////////////////////////////////
public:
#ifdef  __V050322_CACHEQUEST
	CMapWordToPtr   m_mapCreateChar;
	CTimer          m_timerScript;
#ifndef __BEAST
	CScriptDialog	m_WorldScript;
#endif // __BEAST
#endif // __V050322_CACHEQUEST

	LPLANDATTR		m_apLandAttr;
	CObj*			m_apDeleteObjs[MAX_DELETEOBJS];
	int				m_nDeleteObjs;
	TCHAR			m_szFilePath[ 128 ];
	TCHAR			m_szFileName[ 64  ];
	int				m_nLandWidth ;
	int				m_nLandHeight;
	DWORD			m_dwIdWorldRevival; 
	TCHAR			m_szKeyRevival[ 32 ];
	FLOAT			m_fMaxHeight;
	FLOAT			m_fMinHeight;
	DWORD			m_dwWorldID;
	CRegionElemArray	m_aBeginPos;
	CRegionElemArray	m_aRegion;
	CRegionElemArray	m_aStructure;
	BOOL			m_bFly;
	BOOL			m_bIsIndoor;
	DWORD			m_dwDiffuse;
	D3DXVECTOR3		m_v3LightDir;
	DWORD           m_dwIdMusic;
	
#ifdef __N_TRIGGER_AREASET_050203
	int				m_nPKMode;
	TCHAR			m_szWorldName[ 128 ];
#endif // __N_TRIGGER_AREASET_050203

#ifndef __ONLINE
	BOOL			m_bLoadScriptFlag;
//	CScript			m_script; // 스크립트 
#endif	//!__ONLINE

#if !defined(__BEAST) && !defined(__ONLINE)
	CRespawner		m_respawner;
	u_long			Respawn()	{	return m_respawner.Spawn( this );	}
#endif	



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// World.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	inline BOOL		VecInWorld( const D3DXVECTOR3 &vec );
	inline BOOL		VecInWorld( FLOAT x, FLOAT z );

	void			Free();
	DWORD			GetID()	{ return m_dwWorldID; }
	LPREGIONELEM	GetBeginPos( int i = 0 );
	LPREGIONELEM	GetRandomBeginPos();
	void			LoadAllMoverDialog();

	BOOL			AddObj( CObj* pObj, BOOL bAddItToGlobalId = FALSE );
	void			RemoveObj( CObj* pObj );
	void			DeleteObj( CObj* pObj );
	BOOL			DoNotAdd( CObj* pObj );
	BOOL			AddObjArray( CObj* pObj );
	void			RemoveObjArray( CObj* pObj );
	BOOL			InsertObjLink( CObj* pObj );
	BOOL			RemoveObjLink( CObj* pObj );
	BOOL			RemoveObjLink2( CObj* pObj );
	void			AddMoveLandObj( CObj* pObj );
	CObj*			GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel );
	BOOL			SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj );

	FLOAT			GetFullHeight( D3DXVECTOR3 vPos, CObj* pExceptionObj = NULL );
	FLOAT			GetUnderHeight( D3DXVECTOR3 &vPos, float fUnder = -1000.0f, CObj* pExceptionObj = NULL );
	FLOAT			GetOverHeight( D3DXVECTOR3 &vPos, CObj* pExceptionObj = NULL );
	FLOAT			GetLandHeight( float x, float z );
	FLOAT			GetLandHeight( D3DXVECTOR3 vecPos );
#ifdef __CLIENT
	FLOAT			GetLandHeight_Fast( float x, float z );
#endif

	void			GetLandTri( float x, float z, D3DXVECTOR3* pTri );
	BOOL			GetLandTri2( float x, float z, D3DXVECTOR3* pTri2 );
	int				GetLandTris( float x, float z, D3DXVECTOR3* pTris );
	LPWATERHEIGHT	GetWaterHeight( D3DXVECTOR3& vPos )	{ return GetWaterHeight( (int)vPos.x, (int)vPos.z ); }
	LPWATERHEIGHT	GetWaterHeight( int x, int z );
	int				GetHeightAttribute( int x, int z );
	
	CMover*			FindTouchObj( CObj* pActor, D3DXVECTOR3 vMin, D3DXVECTOR3 vMax );		// vMin,vMax와 충돌하는 무버를 찾음
	BOOL			ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	BOOL			ProcessCollisionReflection( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	FLOAT			ProcessUnderCollision( D3DXVECTOR3 *pOut, CObj **pObjColl, D3DXVECTOR3 &vPos, CObj* pExceptionObj = NULL );
	// bSkipTrans:반투명되는 오브젝트는 스킵함.  bWithTerrain:지형까지 포함해서 검사함.
	BOOL			IntersectObjLine( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans = FALSE, BOOL bWithTerrain = FALSE, BOOL bWithObject = TRUE );	
	BOOL			IntersectObjLine2( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans = FALSE, BOOL bWithTerrain = FALSE, BOOL bWithObject = TRUE );	
	FLOAT			IntersectRayTerrain2( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir );
	void			SendDamageAround( const D3DXVECTOR3 *pvPos, int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange );
		


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// World3D.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WORLDSERVER
public:
	// Render
	void			Projection( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight );
	void			Render( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont = NULL );
	void			RenderBase( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont );
		
	// Light, Camera and etc...
	void			AddLight( CLight* pLight );
	CLight*			GetLight( LPCTSTR lpszKey );
	void			SetCamera( CCamera* pCamera ) { m_pCamera = pCamera; }
	CCamera*		GetCamera() { return m_pCamera; }
	void			SetLight( BOOL bLight );
	void			SetFogEnable( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bEnable );

	// Culling
	void			UpdateCullInfo( D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );
	void			CalculateBound();

	// Pick and Intersect
	BOOL			ClientPointToVector( D3DXVECTOR3 *pOut, RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, D3DXVECTOR3* pVector, BOOL bObject = FALSE );
	BOOL			IsPickTerrain(RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView );
	FLOAT			IntersectRayTerrain( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir );
	CObj*			PickObject( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter = 0xffffffff, CObj* pExceptionObj = NULL, D3DXVECTOR3* pVector = NULL, BOOL bOnlyTopPick = FALSE, BOOL bOnlyNPC = FALSE  );
	CObj*			PickObject_Fast( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter = 0xffffffff, CObj* pExceptionObj = NULL, BOOL bBoundBox = FALSE, BOOL bOnlyNPC = FALSE );
	BOOL			CheckBound( D3DXVECTOR3* vPos, D3DXVECTOR3* vDest, D3DXVECTOR3* vOut, FLOAT* fLength );
	#ifdef __BEAST
		int			GetPickCollection( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, CPtrArray* pPtrArray );
	#endif
		
	void			RenderGrid();
	void			RenderGrids( CRect rect, WORD dx, DWORD color );
	void			RenderWorldGrids(int wx,int wy,CPoint ptLT,CPoint ptRB,WORD dx,DWORD color);
	
	// Direct3D 관련 오브젝트 초기화및 제거, 재설정 관련 
	HRESULT			InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT			RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT			DeleteDeviceObjects();
	HRESULT			InvalidateDeviceObjects();
	void			RenderTerrain();
	static HRESULT	StaticInitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	static HRESULT	StaticRestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	static HRESULT	StaticDeleteDeviceObjects();
	static HRESULT	StaticInvalidateDeviceObjects();

private:
	void			RenderWater();
	void			RenderObject( CD3DFont* pFont = NULL );
	void			RenderBoundBoxVertex( CObj* pObj );
	void			SetBoundBoxVertex( CObj* pObj );
#endif // !__WORLDSERVER

	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorldFile.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	BOOL			OpenBmpWorld( LPCTSTR lpszFileName, BOOL bDir = FALSE );
	BOOL			OpenWorld( LPCTSTR lpszFileName, BOOL bDir = FALSE );
	BOOL			OpenWorld( OBJID idWorld, BOOL bDir = FALSE );	
	BOOL			LoadObject();
	BOOL			ReadRegion( CScript& s );
	BOOL			ReadRespawn( CScript& s );
	BOOL			LoadRegion();
	BOOL			LoadLandAttr();

#ifdef __CLIENT
	BOOL			ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand = TRUE );
	BOOL			MakeMiniMapTexture( int xx, int yy, CTexture* pTexture, LPBYTE pBitmap );
#endif

#ifdef __BEAST
	BOOL			NewWorld( int nWidth, int nHeight, int nTexture, FLOAT fHeight = 10.0f );
	BOOL			NewWorld( LPCTSTR lpszFileName,int nTexture, FLOAT fMultipleHeight = 1.0f );
	BOOL			WriteRegionSub( CFileIO* pFile, CObj* pObj );
	BOOL			SaveRegion( LPCTSTR lpszFileName );
	BOOL			SaveObject( LPCTSTR lpszFileName );
	BOOL			SaveWorld( LPCTSTR lpszFileName, BOOL bSaveLND = TRUE, BOOL bSaveDYO = TRUE, BOOL bSaveRGN = TRUE, BOOL bSaveDDS = FALSE, BOOL bFullSave = FALSE );
	BOOL			SaveBitmap( LPCTSTR lpszFileName, CD3DFont* pFont = NULL, BOOL bViewRegion = FALSE, BOOL bViewName = FALSE, BOOL bViewSpawn = FALSE, BOOL bViewWater = TRUE, BOOL bViewHeightAttrb = FALSE );	
	BOOL			SaveLandBitmap( LPCTSTR lpszFileName, int xx, int yy );
	BOOL			ReadWorld();
	BOOL			ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand = FALSE );
	BOOL			MakeMiniMapTexture( int xx, int yy, CTexture* pTexture, LPBYTE pBitmap );
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorldEdit.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __BEAST
public:
	// Undo & Redo
	BOOL			Undo();
	BOOL			Redo();

	// Select
	D3DXVECTOR3		GetSelectCenter();
	BOOL			IsValidSelectObj( CObj* pObj );
	void			SetSelectGridPos();
	BOOL			IsSelectObj( CObj* pObj );
	void			SetSelectObj( CObj* pObj, BOOL bSelect );
	void			SetSelectObj( CRect rect );
	void			MoveSelectObj( D3DXVECTOR3 vAdd );
	void			InvisibleSelectObj();
	void			InvisibleUpstairsObj( FLOAT fY );
	void			ShowAllObjects();
	void			DeleteSelectObj();
	void			CopySelectObj();
	void			UnselectObj();
	void			RenderCircleVertex( CObj* pObj );
	void			SetCircleVertex( CObj* pObj );
	void			SetHeight(int x,int z,FLOAT factor);
	void			SetHeight(POINT pos,FLOAT factor);
	void			SetAllObjOnLand();

	void			SetWorldCol( int lx,int lz,int gx,int gz,DWORD color);
	void			SetCol( int x,int z,DWORD color);
	void			SetColor(FLOAT x,FLOAT z,DWORD color, FLOAT fRadius, FLOAT fHardness);
	DWORD			GetCol( int x,int z );
	int 			GetTexture( int x, int y );
	void			SetWorldTex( int lx,int lz,int gx,int gz,WORD nTex, BYTE nOpacity=255,BOOL bLandMark=FALSE);
	void			SetTex( int x,int z,WORD nTex, BYTE nOpacity=255,BOOL bLandMark=FALSE);
	void			SetBlankPatch(int x,int z);
	void			SetLandMarkPatch(int x,int z,WORD nTex);
	void			SetDetailPatch(int x,int z,WORD nTex);
	void			SetWaterHeight(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture );
	void			SetHeightAttribute( int x, int z, DWORD dwAttr, FLOAT fRadius );
		
private:
	CModelObject	m_meshArrow; // 타겟의 방향을 알려주는 화살표 오브젝트이다.

	void			RenderBoundBox();
	void			RenderGizmo();
	void			RenderAxis();
	// 아래 함수들은 SetColor, GetColor, SetTexture에서만 호출해야한다.
	void			SetMapColor(int x,int z,DWORD color);
public:
	void			RenderRectTracker( CRect rect, WORD dx, DWORD color );
#endif // __BEAST

};

#ifdef __WORLDSERVER
inline HANDLE CWorld::GetIOCP()
{
	return m_hIOCP;
}

inline short CWorld::GetPosAttr( const D3DXVECTOR3 & vPos )
{
	return 0;
}

inline BOOL	CWorld::GetRect( CPoint point, CRect & rRect )
{
	for( list<CCtrlRegion*>::iterator i = m_lspCtrlRegion.begin(); i != m_lspCtrlRegion.end(); ++i ) {
		if( (*i)->GetRect( point, rRect ) )
			return TRUE;
	}
	return FALSE;
}
#else	// __WORLDSERVER
inline BOOL CWorld::LandInWorld( int x, int z )
{
	if( x < 0 || z < 0 || x >= m_nLandWidth || z >= m_nLandHeight )
		return FALSE;
	return TRUE;
}

inline BOOL CWorld::IsVecInVisibleLand( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, int nVisibilityLand )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= WORLD_WIDTH * MPU || (int)vPos.z >= WORLD_HEIGHT * MPU )
		return FALSE;
	int x1 = int( vCenterPos.x ) / ( MAP_SIZE * MPU );
	int z1 = int( vCenterPos.z ) / ( MAP_SIZE * MPU );
	int x2 = int( vPos.x ) / ( MAP_SIZE * MPU );
	int z2 = int( vPos.z ) / ( MAP_SIZE * MPU );
	return x2 >= (int)x1 - nVisibilityLand && x2 <= (int)x1 + nVisibilityLand &&
		   z2 >= (int)z1 - nVisibilityLand && z2 <= (int)z1 + nVisibilityLand ;
}

inline BOOL CWorld::IsVecInRange( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, FLOAT fRadius  )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= WORLD_WIDTH * MPU || (int)vPos.z >= WORLD_HEIGHT * MPU )
		return FALSE;
	if( vPos.x >= ( vCenterPos.x - fRadius ) && vPos.z >= ( vCenterPos.z - fRadius ) 
		&& vPos.x < ( vCenterPos.x + fRadius ) && vPos.z < ( vCenterPos.z + fRadius ) )
		return TRUE;
	return FALSE;
}

inline void CWorld::WorldPosToLand( D3DXVECTOR3 vPos, int& x, int& z ) 
{
	x = int( vPos.x ) / ( MAP_SIZE * MPU );
	z = int( vPos.z ) / ( MAP_SIZE * MPU );
}

inline void CWorld::WorldPosToLandPos( D3DXVECTOR3 vPos, int& x, int& z ) 
{
	x = int( vPos.x ) % ( MAP_SIZE * MPU );
	z = int( vPos.z ) % ( MAP_SIZE * MPU );
}

inline CLandscape* CWorld::GetLandscape( CObj* pObj )
{
	FLOAT rX = pObj->GetPos().x;// - m_nWorldX;
	FLOAT rZ = pObj->GetPos().z;// - m_nWorldY;
	int   mX = int( rX / ( MAP_SIZE * MPU ) );
	int   mZ = int( rZ / ( MAP_SIZE * MPU ) );
	int   nOffset = mX + mZ * m_nLandWidth;
	if( nOffset < 0 || nOffset >= m_nLandWidth * m_nLandHeight )
		return NULL;
	return m_apLand[ nOffset ];
}

inline CLandscape* CWorld::GetLandscape( D3DXVECTOR3 vPos )
{
	FLOAT rX = vPos.x;// - m_nWorldX;
	FLOAT rZ = vPos.z ;//- m_nWorldY;
	int   mX = int( rX / ( MAP_SIZE * MPU ) );
	int   mZ = int( rZ / ( MAP_SIZE * MPU ) );
	int   nOffset = mX + mZ * m_nLandWidth;
	if( nOffset < 0 || nOffset >= m_nLandWidth * m_nLandHeight )
		return NULL;
	return m_apLand[ nOffset ];
}

inline CLandscape* CWorld::GetLandscape( int x, int z )
{
	return m_apLand[ x + z * m_nLandWidth ];
}

inline CPatch* CWorld::GetPatch( D3DXVECTOR3 vPos )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= m_nLandWidth * MAP_SIZE * MPU|| (int)vPos.z >= m_nLandHeight * MAP_SIZE * MPU ) return NULL;
	FLOAT rX = vPos.x/MPU;// - m_nWorldX;
	FLOAT rZ = vPos.z/MPU ;//- m_nWorldY;
	int   mX = int( rX / ( MAP_SIZE ) );
	int   mZ = int( rZ / ( MAP_SIZE ) );
	FLOAT rPX = rX - mX*MAP_SIZE ;// - m_nWorldX;
	FLOAT rPZ = rZ - mZ*MAP_SIZE ;//- m_nWorldY;
	int   mPX = int( rPX / ( PATCH_SIZE ) );
	int   mPZ = int( rPZ / ( PATCH_SIZE ) );
	return &(m_apLand[ mX + mZ * m_nLandWidth ]->m_aPatches[mPZ][mPX]);
}
#endif	// !__WORLDSERVER


inline BOOL CWorld::VecInWorld( const D3DXVECTOR3 & vPos )
{
	if( _isnan( (double)vPos.x ) || _isnan( (double)vPos.y ) || _isnan( (double)vPos.z ) )
		return FALSE;

	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= m_nLandWidth * MAP_SIZE * MPU - 4 || (int)vPos.z >= m_nLandHeight * MAP_SIZE * MPU - 4 )
		return FALSE;

	return TRUE;
}
inline BOOL CWorld::VecInWorld( FLOAT x, FLOAT z )
{
	if( _isnan( (double)x ) || _isnan( (double)z ) )
		return FALSE;

	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * ( MAP_SIZE * MPU ) - 4 || (int)z >= m_nLandHeight * ( MAP_SIZE * MPU ) - 4 )
		return FALSE;
	return TRUE;
}

extern CObj *GetLastPickObj( void );


VOID UpdateCullInfo(D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );

#ifdef __WORLDSERVER	// _nRadiusLink

	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType ) { \
			int _nLinkX = _vPos.x / MPU;	\
			int _nLinkZ = _vPos.z / MPU;	\
			int _nWidthLink, _nMaxWidth;	\
			int _nMaxHeight;	\
			int _nUnit;	\
			int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
			int _nX, _nZ;	\
			int _d;	\
			int _nPos;	\
			for( int _i = 0; _i < (_pWorld)->m_linkMap.GetMaxLinkLevel( _dwLinkType ); _i++ ) {	\
				_nWidthLink		= (_pWorld)->m_linkMap.GetLinkWidth( _dwLinkType, _i);	\
				CObj** _pObjs	= (_pWorld)->m_linkMap.GetObj( _dwLinkType, _i );	\
				ASSERT( _pObjs );	\
				_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth ;	\
				_nMaxHeight	= _nWidthLink * (_pWorld)->m_nLandHeight;	\
				_nUnit	=  ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
				_nX		= ( _nLinkX / _nUnit ) * _nUnit * MPU;	\
				_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * MPU;	\
				_d		= _nUnit * MPU / 2;	\
				_nX		= ( _vPos.x - _nX > _d )? 1: 0;	\
				_nZ		= ( _vPos.z - _nZ > _d )? 1: 0;	\
				_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
				_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
				_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
				_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
				for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ )	\
				{	\
					for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
					{	\
						_nPos = _j * _nMaxWidth + _k;	\
						_pObj = _pObjs[ _nPos ];	\
						int __limit = 1000;  \
						while( _pObj && __limit-- ) { 

	#define END_LINKMAP	\
		pObj = pObj->GetNextNode(); } } } } }
		
#else	// __WORLDSERVER
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType ) { \
		int _nLinkX = _vPos.x / MPU;	\
		int _nLinkZ = _vPos.z / MPU;	\
		int _nWidthLink;	\
		int _nMaxWidth, _nMaxHeight;	\
		int _nUnit;		\
		int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
		int _nX, _nZ;	\
		int _d;		\
		int _nPos;	\
		for( int _i = 0; _i < MAX_LINKLEVEL; _i++ ) {	\
			_nWidthLink	= CLandscape::m_nWidthLinkMap[ _i ];	\
			_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth;	\
			_nMaxHeight		= _nWidthLink * (_pWorld)->m_nLandHeight;	\
			_nUnit	= ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
			_nX		= ( _nLinkX / _nUnit ) * _nUnit * MPU;	\
			_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * MPU;	\
			_d		= _nUnit * MPU / 2;		\
			_nX		= ( _vPos.x - _nX > _d )? 1: 0;		\
			_nZ		= ( _vPos.z - _nZ > _d )? 1: 0;		\
			_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
			_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
			_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
			_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
			for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ ) \
			{ \
				for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
				{ \
					CLandscape* _pLand \
						= (_pWorld)->m_apLand[ ( _j / _nWidthLink ) * (_pWorld)->m_nLandWidth + ( _k / _nWidthLink ) ]; \
					if( _pLand ) \
					{ \
						_nPos = ( _j % _nWidthLink ) * _nWidthLink + ( _k % _nWidthLink ); \
	 					CObj** _pObjs = _pLand->GetObjLink( _dwLinkType, _i );	\
						ASSERT( _pObjs ); \
						_pObj = _pObjs[ _nPos ]; \
						while( _pObj ) { \
							if( IsValidObj( pObj ) )  

	#define END_LINKMAP pObj = pObj->GetNextNode(); } } } } } }

	#define FOR_LAND( _pWorld, _pLand, _nVisibilityLand, _bVisuble ) { \
		if( (_pWorld)->m_pCamera ) \
		{ \
			int _i, _j, _x, _y; \
			(_pWorld)->WorldPosToLand( (_pWorld)->m_pCamera->m_vPos, _x, _y ); \
			int _nXMin = _x - _nVisibilityLand; if( _nXMin < 0 ) _nXMin = 0; \
			int _nYMin = _y - _nVisibilityLand; if( _nYMin < 0 ) _nYMin = 0; \
			int _nXMax = _x + _nVisibilityLand; if( _nXMax >= (_pWorld)->m_nLandWidth  ) _nXMax = (_pWorld)->m_nLandWidth - 1; \
			int _nYMax = _y + _nVisibilityLand; if( _nYMax >= (_pWorld)->m_nLandHeight ) _nYMax = (_pWorld)->m_nLandHeight - 1; \
			for( _i = _nYMin; _i <= _nYMax; _i++ ) \
			{ \
				for( _j = _nXMin; _j <= _nXMax; _j++ ) \
				{ \
					if( (_pWorld)->m_apLand[ _i * (_pWorld)->m_nLandWidth + _j ] ) \
					{ \
						_pLand = (_pWorld)->m_apLand[ _i * (_pWorld)->m_nLandWidth + _j ]; \
						if( _bVisuble == FALSE || ( _bVisuble == TRUE && _pLand->isVisibile() ) ) 

	#define END_LAND } } } } }

	#define FOR_OBJARRAY( _pLand, _pObj ) { \
		for( int _k = 0; _k < MAX_OBJARRAY; _k++ ) \
		{ \
			CObj** _apObjs = _pLand->m_apObject[ _k ]; \
			for( int _l = 0; _l < int( _pLand->m_adwObjNum[ _k ] ); _l++ ) \
			{ \
				_pObj = _apObjs[ _l ]; \
				if( IsValidObj( _pObj ) )  

	#define END_OBJARRAY } } }

	#define FOR_OBJ( _pLand, _pObj, _nType ) { \
			CObj** _apObjs = _pLand->m_apObject[ _nType ]; \
			for( int _l = 0; _l < int( _pLand->m_adwObjNum[ _nType ] ); _l++ ) \
			{ \
				_pObj = _apObjs[ _l ]; \
				if( IsValidObj( _pObj ) )  

	#define END_OBJ } }  

#endif	// __WORLDSERVER
#endif	// __WORLD_2002_1_22