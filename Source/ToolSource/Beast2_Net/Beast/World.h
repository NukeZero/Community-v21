#ifndef __WORLD_2002_1_22
#define __WORLD_2002_1_22

#include <D3D9.h>
#include <list>
//#include <map>
//#include <string>
#include <basetsd.h> 
#include "Respawn.h"

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

#ifdef __Y_PATROL

class CPathObj : public CObj
{
public:
	D3DXVECTOR3		m_vDir;					// 방향벡터
	FLOAT			m_fLength;				// 거리
	DWORD			m_dwMotionState;		// 모션 종류
	DWORD			m_dwDelay;				// 정지시간
	CTimer			m_ctStop;				// 정지시간
	
	CPathObj() : m_vDir(0,0,0), m_fLength(0), m_dwMotionState(MTI_WALK), m_dwDelay(0), m_ctStop(0)
	{
	}
	virtual ~CPathObj() {}
};

typedef struct _VECINFO
{
	D3DXVECTOR3		m_vDir;			// 방향벡터
	FLOAT			m_fLength;		// 거리
	DWORD			m_dwState;		// 모션 종류
	DWORD			m_dwDelay;		// 정지시간
	CTimer			m_ctStop;       // 정지시간
} _VECINFO;

class CPatrolPath
{
	map< DWORD, vector<CPathObj*> > m_mapPatrolPath;	
	map< DWORD, DWORD	 > m_dwIndex;
	map< DWORD, BOOL	 > m_bReturn;

public:
	CPatrolPath();
	~CPatrolPath();

	static	CPatrolPath*				GetInstance( void );
	map< DWORD, vector<CPathObj*> >*     GetMapPatrolPath() { return &m_mapPatrolPath; }
	
	BOOL			Save				( LPCTSTR szFileName );
	BOOL			Load				( LPCTSTR szFileName );
	void			AddPatrolIndex		( DWORD dwIndex );
	void			DeletePatrolIndex	( DWORD dwIndex );
	BOOL			IsFirstPath			( DWORD dwIndex );
	void			Clear				();
	void			SetReturn			( DWORD dwIndex, BOOL bReturn );
	void			AddPatrolPath		( DWORD dwIndex, CPathObj* vecInfo );
		
	CPathObj*		GetPatrolPath		( DWORD dwIndex );
	BOOL			IsArrPos			( DWORD dwIndex, D3DXVECTOR3& vPos );
	void			GetNextPos			( DWORD dwIndex, D3DXVECTOR3& vPos );
	void			RenderPath			( DWORD dwIndex, CWorld* pWorld );
	void			RenderPath			( CObj* pObj, CWorld* pWorld );
	BOOL			DeletePatrolPath	( CPathObj* pPathObj );	
	void			UpdatePatrolData	( DWORD dwIndex );
		
};

#endif //__Y_PATROL

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


struct WORLD_ENVIR_INFO;

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

	CString			m_strSkyTex[ 3 ];
	CString			m_strCloudTex[ 3 ];
	CString			m_strSunTex;
	CString			m_strMoonTex;
	BOOL			m_bFixedLight;

	CString			m_strSeaCloud;		//바다 구름
	LPDIRECT3DTEXTURE9 m_pTextureSeaCloud;

	int                     m_nBoundBoxVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pBoundBoxVertexBuffer;
    LPDIRECT3DDEVICE9       m_pd3dDevice;        
	D3DMATERIAL9			m_baseMaterial;

	LIGHTCOLOR m_kWorldLight[24];		//게임에서 사용할 월드 시간별 Light

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
#ifdef __CSC_RENDER_AXIS
	BOOL			m_bViewAxis;
#endif //__CSC_RENDER_AXIS
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

public:
#ifdef  __V050322_CACHEQUEST
	CMapWordToPtr   m_mapCreateChar;
	CTimer          m_timerScript;
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

	BOOL			m_bIsContinent;
	BOOL			m_bIsUsing24Light;

	LIGHTCOLOR		m_k24Light[24];
	
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

	//gmpbigsun
	FLOAT			IntersectRayTerrain3( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir );
		


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// World3D.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#ifdef __BEAST
	BOOL			NewWorld( int nWidth, int nHeight, int nTexture, FLOAT fHeight = 10.0f );
	BOOL			NewWorld( LPCTSTR lpszFileName,int nTexture, FLOAT fMultipleHeight = 1.0f );
	BOOL			WriteRegionSub( CFileIO* pFile, CObj* pObj );
	BOOL			SaveRegion( LPCTSTR lpszFileName );
	BOOL			SaveObject( LPCTSTR lpszFileName );
	BOOL			SaveWorld( LPCTSTR lpszFileName, BOOL bSaveLND = TRUE, BOOL bSaveDYO = TRUE, BOOL bSaveRGN = TRUE, BOOL bSaveDDS = FALSE, BOOL bFullSave = FALSE );
	BOOL			SaveasWorld( LPCTSTR lpszFileName, BOOL bSaveLND = TRUE, BOOL bSaveDYO = TRUE, BOOL bSaveRGN = TRUE, BOOL bSaveDDS = FALSE, BOOL bFullSave = FALSE );
	BOOL			SaveBitmap( LPCTSTR lpszFileName, CD3DFont* pFont = NULL, BOOL bViewRegion = FALSE, BOOL bViewName = FALSE, BOOL bViewSpawn = FALSE, BOOL bViewWater = TRUE, BOOL bViewHeightAttrb = FALSE );	
	BOOL			SaveLandBitmap( LPCTSTR lpszFileName, int xx, int yy );
	BOOL			ReadWorld();
	BOOL			ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand = FALSE );
	BOOL			MakeMiniMapTexture( int xx, int yy, CTexture* pTexture, LPBYTE pBitmap );

#ifdef __CSC_WORLDOBJ_LIST
	void			ResetWorldObjList(D3DXVECTOR3 vecPos);
#endif //__CSC_WORLDOBJ_LIST
#ifdef __USE_STR_TBL0407
	map<string, string>		m_mapString;
	CString		m_strStrFile;
	int		m_nString;
	BOOL	LoadString( void );
	BOOL	WriteString( void );
#endif	// __USE_STR_TBL0407

protected:
	BOOL			_SaveTheWorld( LPCTSTR lpszFileName, BOOL bSaveLND, BOOL bSaveDYO, BOOL bSaveRGN, BOOL bSaveDDS, BOOL bFullSave );
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

	//gmpbigsun
	void			InvisibleCollisonSelectObj( );
	void			UpdateCheckSnap( );
	void			DoSnap( CObj* pTarget, CObj* pSrc );
	
	void			UpdateLerpEV( CLight* pLight );			// Rst = Vorg * ( 1.0f - t ) + ( Vtar * t ) { t = 0.0 ~ 1.0f; }
	void			StartTEV( );
	
	BOOL			m_bIsWorkingEV;
	DWORD			m_dwOldTime;
	DWORD			m_dwAddedTime;

	//--

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
	BOOL			LoadPatrol();
	void			RenderRectTracker( CRect rect, WORD dx, DWORD color );
	
#endif // __BEAST
};

inline BOOL CWorld::LandInWorld( int x, int z )
{
	if( x < 0 || z < 0 || x >= m_nLandWidth || z >= m_nLandHeight )
		return FALSE;
	return TRUE;
}


extern int g_MPU;
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

	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType ) { \
		int _nLinkX = (int)( _vPos.x / MPU );	\
		int _nLinkZ = (int)( _vPos.z / MPU );	\
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

#endif	// __WORLD_2002_1_22

