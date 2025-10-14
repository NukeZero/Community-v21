// WorldView.h : interface of the CWorldView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDVIEW_H__7BA89B6E_E94B_4B1E_800E_44BFA667D440__INCLUDED_)
#define AFX_WORLDVIEW_H__7BA89B6E_E94B_4B1E_800E_44BFA667D440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgRespawn.h"
#include "DlgWorldLight.h"
#include "WorldDoc.h"

#define FT_TEXTURE    0
#define FT_SHADING    1
#define FT_WIRE_FRAME 2

#define MAX_FIELDVERTEX  200

/*
#define VIEW_ZOOM     0
#define VIEW_ROTATION 1
#define VIEW_SCROLL   2
#define VIEW_DRAW     3
#define VIEW_ERASE    4
#define VIEW_HEIGHT   5
*/
#define BRUSHTYPE_OBJECT  0
#define BRUSHTYPE_TERRAIN 1 

//#define EDITTYPE_HEIGHT                    0
//#define EDITTYPE_TEXTURE                   1
//#define EDITTYPE_COLOR                     2
//#define EDITTYPE_OBJ_EDIT_PUT_UP           3
/*
#define EDITTYPE_OBJ_EDIT_PUT              4
#define EDITTYPE_OBJ_EDIT_SELECT           5
#define EDITTYPE_OBJ_EDIT_SELECT_MOVE      6
#define EDITTYPE_OBJ_EDIT_SELECT_ROTATION  7
#define EDITTYPE_OBJ_EDIT_SELECT_SCALE     8
*/
//#define EDITTYPE_OBJ_EDIT_DELETE           9 
//#define EDITTYPE_OBJ_EDIT_WIREFRAME        10
//#define EDITTYPE_OBJ_EDIT_UNVISIBLE        11
//#define EDITTYPE_OBJ_EDIT_ALL_VISIBLE      12

class CObjProperty;
class CPagePropObj;
class CPagePropCtrl;
class CPagePropItem;
class CPagePropMover;
class CPagePropLight;
class CPagePropRegion;
class CPagePropTrigger;
class CPagePropAttribute;

class C3DRectTracker
{
public:
	DWORD m_dwColor;
	CRect* m_pRect;
	CWorldView* m_pWorldView;
    LPDIRECT3DDEVICE9 m_pd3dDevice; 
	CWorld* m_pWorld;
	int m_nPickRectPoint;
	C3DRectTracker();
	~C3DRectTracker() { }

	BOOL OnLButtonDown(UINT nFlags, CPoint point) ;
	BOOL OnMouseMove( UINT nFlags, CPoint point );
	int  PickRectTracker( CRect rect, CPoint ptClient );
	void RenderRectTracker();
};


class CWorldView : public CView, public CD3DApplication
{
	C3DRectTracker m_3DRectTracker;
	//CObj*     m_pObjCurrent;
	CD3DFont* m_pFont;
	int       m_nFilterType;
	BOOL      m_bPickUpTile;
	BOOL      m_bViewFog;
	CPoint    m_ptField;
	FLOAT     m_fWldHeight;
	D3DXVECTOR3 m_vOrigin;
	BOOL      m_bSelectDrag;
	CRect     m_rectDraw;
	CPoint    m_ptOrg;
	CPoint    m_ptButtonDown;
	BOOL      m_bLock;
	BOOL      m_bBeastViewReady;
	BOOL	  m_bSetupContinent;
	
//static	int       m_nSelect;
static  int       m_nRestrict;
	void RenderHeightPoint( D3DXVECTOR3 vPos, int nCurRadius, int nRadius, DWORD dwColor );
	void RenderVertexPoint( D3DXVECTOR3 vPos, DWORD dwColor, int fScale = 1 );
	void SetLightColor( CPoint point );
	void EditHeightMountain( CPoint point, BOOL bRender = FALSE );
	void EditHeightFlat( CPoint point, BOOL bRender = FALSE );
	void EditHeightSoft( CPoint point, BOOL bRender = FALSE );
	void EditHeightRandom( CPoint point, BOOL bRender = FALSE );
	void EditHeightAttribute( CPoint point, int nAttr );
	void SetTexture( FLOAT x, FLOAT z, WORD nTex, FLOAT fRadius = 4.0f, FLOAT fHardness = 2.0f, BOOL bRender = FALSE );
	int m_nDegree;
	D3DXVECTOR3 m_vCenter;
	D3DXVECTOR3 GetSelectCenter();

	void FlatPatch(CPatch* pPatch);
	
	D3DXVECTOR3 ScreenToVector( int sx, int sy );
	void UpdateCamera( CPoint ptAdd ) ;
private:

	DWORD   m_dwFogColor;
	DWORD   m_dwFogMode;
	BOOL    m_bCanDoTableFog;
	BOOL    m_bCanDoVertexFog;
	BOOL    m_bCanDoWFog;
	BOOL    m_bDeviceUsesWFog;
	BOOL    m_bRangeBasedFog;
	BOOL    m_bUsingTableFog;
	BOOL    m_bEditOnLand;
	BOOL    m_bEditHeapObject;
	BOOL    m_bEditRandomDegreeScale;
	BOOL    m_bOnGrid;
	HWND    m_hwndRenderWindow;
	HWND    m_hwndRenderFullScreen;


	DWORD   m_dwNumTerrainVertices;
	DWORD   m_dwNumColumnVertices;

	D3DXVECTOR3 m_vIntersect;
	BOOL        m_bCursorInWorld;
	BOOL        m_bRespawnEnable;

static CDlgWorldLight  m_dlgWorldLight;
//static CDlgRespawn* m_pDlgRespawn;
static BOOL m_bDlgRespawn;
static BOOL m_bRandomDegreeScale;
static BOOL m_bRandomDegree;
static BOOL m_bRandomScale;
static int m_nRandomDegree1, m_nRandomDegree2;
static FLOAT m_fRandomScale1, m_fRandomScale2;
static int   m_nRespawnNum;
static int   m_nRespawnSec;

static FLOAT m_fGridX;
static FLOAT m_fGridY;
static FLOAT m_fGridZ;

public: 
	CPtrArray m_aPickCollection;
	int       m_nPickCollection;
	
	//	Undo, Redo
	int	Undo();
	int	Redo();

	CToolCamera m_camera;

	CD3DArcBall         m_ArcBall;          // mouse rotation utility
static int m_nOpenViewCnt;
	/*
static CObjProperty*   m_pObjProperty;
static CPagePropObj*       m_pPageObj;
static CPagePropCtrl*      m_pPageCtrl; 
static CPagePropItem*      m_pPageItem; 
static CPagePropMover*     m_pPageMover;
static CPagePropLight*     m_pPageLight;
static CPagePropRegion*    m_pPageRegion;
static CPagePropTrigger*   m_pPageTrigger;
static CPagePropAttribute* m_pPageAttribute;
*/
	BOOL OpenWorld( LPCTSTR lpszFileName );
	//HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	//virtual HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT);// { return S_OK; }
	//HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
//virtual HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT);// { return S_OK; }
    HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );

	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	virtual HRESULT AdjustWindowForChange();

	CObj* PickCollection( CPoint point );
//	VOID UpdateUIForDeviceCapabilites();
//  VOID SetFogParameters();

	void RenderDX(); 
	void RenderDC();
	//void RenderRectTracker( CRect rect, WORD dx, DWORD color );
	void MoveRectTracker( CRect rect, CPoint point );
	int  PickRectTracker( CRect rect, CPoint point );

	int  m_nPickRectPoint;

//	BOOL IsReady() { return m_bReady; }
	TCHAR* PstrFrameStats() { return m_strFrameStats; }
	VOID RenderScene() { Render3DEnvironment(); }
	HRESULT CheckForLostFullscreen();

	//gmpbigsun: 경계대륙 ---------------------------------------------------------------
	IDirect3DVertexBuffer9* m_VBforLine;
	
	void AddLineData( const D3DXVECTOR3& kPos );
	void SubLineData_Back( ); 
	void UpdateLineVertex( );
	void RenderLines( );
	void RenderContinentPoints( );
	//----------------------------------------------------------------------------------

	//FLOAT m_fYaw  ; // 좌우 
	//FLOAT m_fPitch; // 상하 
	//FLOAT m_fRoll ; // 좌우 회전 (뱅글 뱅글 --;)

//	int m_nDegree;
	int m_nAirplaneDegree;
	//D3DXVECTOR3 m_vecPos;
	//D3DXVECTOR3 m_vecLookAt;
	D3DXVECTOR3 m_vecAirPlane;
	BOOL m_bLButtonDown;
	BOOL m_bRButtonDown;
	BOOL m_bMButtonDown;
	BOOL m_bLBDownMouseMove;
	BOOL m_bScrollPossible;

	BOOL ContourLineUp(int x,int y,int r,FLOAT fHeight);
	BOOL ContourLineDown(int x,int y,int r,FLOAT fHeight);
//	CSkinMesh m_skinMesh;

	//BOOL ClientPointToVector(POINT point,D3DXVECTOR3* pVector);
	//void GetPickRay(POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrg,D3DXVECTOR3* pvPickRayDir);
	//void GetRayEnd(D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd);
//	CD3DArcBall  m_ArcBall;          // mouse rotation utility

protected: // create from serialization only
	CWorldView();
	DECLARE_DYNCREATE(CWorldView)

// Attributes
public:
static  int   m_nTool;	
//static  int   m_nView;
static  DWORD m_dwBrushType;
static	DWORD m_dwTerrainBrush;
static	CObj* m_pObjBrush;
static	D3DXVECTOR3 m_vBrushPos;
static	D3DXVECTOR3 m_vBrushScale;
static	D3DXVECTOR3 m_vBrushRotaion;

	CWorldDoc* GetDocument();


	void RotationObjects( FLOAT fRadian );
	void RotationObjectsX( FLOAT fRadian );
	void RotationObjectsZ( FLOAT fRadian );
	void RotationObjects( const D3DXVECTOR3& vtRadian);

	//	RotationObjects속의 한 각도에 대한 계산을 함수로 빼내어서 
	//	x,y,z각각 계산을 해줄때 함수 호출로 간단하게 불러 낼수 있도록 수정하다.
	//	우선 함수만 추가하고 나중에 실제적으로 렌더링 하는 부분에 이 내용이 들어
	//	갈때 호출하는 부분까지 넣어 주는 것이 좋을 듯하다.(렌더링 하는 부분은 common에 있기 때문이지...)
	
	void DoProperty();
	
	CWorld m_world;

	BOOL CreateDevice() ;
static void SetTool( int nTool );
static int GetTool();

public:
 //   LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWorldView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnFilterShading();
	afx_msg void OnFilterTexture();
	afx_msg void OnFilterWireFrame();
	afx_msg void OnUpdateFilterShading(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterTexture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterWireFrame(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewZoom();
	afx_msg void OnUpdateViewZoom(CCmdUI* pCmdUI);
	afx_msg void OnViewScroll();
	afx_msg void OnUpdateViewScroll(CCmdUI* pCmdUI);
	afx_msg void OnViewRotation();
	afx_msg void OnUpdateViewRotation(CCmdUI* pCmdUI);
	afx_msg void OnViewFramemove();
	afx_msg void OnUpdateViewFramemove(CCmdUI* pCmdUI);
	afx_msg void OnEditDraw();
	afx_msg void OnUpdateEditDraw(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewZoomExtents();
	afx_msg void OnUpdateViewZoomExtents(CCmdUI* pCmdUI);
	afx_msg void OnViewFog();
	afx_msg void OnUpdateViewFog(CCmdUI* pCmdUI);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnViewSkybox();
	afx_msg void OnUpdateViewSkybox(CCmdUI* pCmdUI);
	afx_msg void OnViewLod();
	afx_msg void OnUpdateViewLod(CCmdUI* pCmdUI);
	afx_msg void OnViewGrid();
#ifdef __CSC_RENDER_AXIS
	afx_msg void OnViewAxis();
#endif //__CSC_RENDER_AXIS
	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
	afx_msg void OnViewTerrain();
	afx_msg void OnUpdateViewTerrain(CCmdUI* pCmdUI);
	afx_msg void OnViewAllObjects();
	afx_msg void OnUpdateViewObject(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveWorld();
	afx_msg void OnUpdateFileSaveWorld(CCmdUI* pCmdUI);
	afx_msg void OnViewWater();
	afx_msg void OnUpdateViewWater(CCmdUI* pCmdUI);
	afx_msg void OnExecGame();
	afx_msg void OnUpdateExecGame(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsWorld();
	afx_msg void OnSelect();
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	afx_msg void OnSelectMove();
	afx_msg void OnUpdateSelectMove(CCmdUI* pCmdUI);
	afx_msg void OnSelectRotation();
	afx_msg void OnUpdateSelectRotation(CCmdUI* pCmdUI);
	afx_msg void OnSelectScale();
	afx_msg void OnUpdateSelectScale(CCmdUI* pCmdUI);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRestrictX();
	afx_msg void OnUpdateRestrictX(CCmdUI* pCmdUI);
	afx_msg void OnRestrictY();
	afx_msg void OnUpdateRestrictY(CCmdUI* pCmdUI);
	afx_msg void OnRestrictZ();
	afx_msg void OnUpdateRestrictZ(CCmdUI* pCmdUI);
	afx_msg void OnRestrictXz();
	afx_msg void OnUpdateRestrictXz(CCmdUI* pCmdUI);
	afx_msg void OnSelectPut();
	afx_msg void OnUpdateSelectPut(CCmdUI* pCmdUI);
	afx_msg void OnRespawn();
	afx_msg void OnUpdateRespawn(CCmdUI* pCmdUI);
	afx_msg void OnViewGridPatch();
	afx_msg void OnUpdateViewGridPatch(CCmdUI* pCmdUI);
	afx_msg void OnViewGridLandscape();
	afx_msg void OnUpdateViewGridLandscape(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsBitmap();
	afx_msg void OnUpdateFileSaveAsBitmap(CCmdUI* pCmdUI);
	afx_msg void OnViewLight();
	afx_msg void OnUpdateViewLight(CCmdUI* pCmdUI);
	afx_msg void OnTerrainTexture();
	afx_msg void OnUpdateTerrainTexture(CCmdUI* pCmdUI);
	afx_msg void OnTerrainColor();
	afx_msg void OnUpdateTerrainColor(CCmdUI* pCmdUI);
	afx_msg void OnTerrainHeight();
	afx_msg void OnUpdateTerrainHeight(CCmdUI* pCmdUI);
	afx_msg void OnViewSpawn();
	afx_msg void OnUpdateViewSpawn(CCmdUI* pCmdUI);
	afx_msg void OnViewObj();
	afx_msg void OnUpdateViewObj(CCmdUI* pCmdUI);
	afx_msg void OnViewMover();
	afx_msg void OnUpdateViewMover(CCmdUI* pCmdUI);
	afx_msg void OnEditOnland();
	afx_msg void OnUpdateEditOnland(CCmdUI* pCmdUI);
	afx_msg void OnViewRegion();
	afx_msg void OnUpdateViewRegion(CCmdUI* pCmdUI);
	afx_msg void OnViewName();
	afx_msg void OnUpdateViewName(CCmdUI* pCmdUI);
	afx_msg void OnEditGrid();
	afx_msg void OnUpdateEditGrid(CCmdUI* pCmdUI);
	afx_msg void OnEditLock();
	afx_msg void OnUpdateEditLock(CCmdUI* pCmdUI);
	afx_msg void OnViewTop();
	afx_msg void OnTerrainWater();
	afx_msg void OnUpdateTerrainWater(CCmdUI* pCmdUI);
	afx_msg void OnViewFront();
	afx_msg void OnUpdateViewFront(CCmdUI* pCmdUI);
	afx_msg void OnViewHeightAttribute();
	afx_msg void OnUpdateViewHeightAttribute(CCmdUI* pCmdUI);
	afx_msg void OnViewObjlod();
	afx_msg void OnUpdateViewObjlod(CCmdUI* pCmdUI);
	afx_msg void OnWorldRevival();
	afx_msg void OnUpdateWorldRevival(CCmdUI* pCmdUI);
	afx_msg void OnEditRandomDegreeScale();
	afx_msg void OnUpdateEditRandomDegreeScale(CCmdUI* pCmdUI);
	afx_msg void OnViewSetTime();
	afx_msg void OnUpdateViewSetTime(CCmdUI* pCmdUI);
	afx_msg void OnEditSetgrid();
	afx_msg void OnEditRespawnDefault();
	afx_msg void OnEditRespawnEnable();
	afx_msg void OnUpdateEditRespawnEnable(CCmdUI* pCmdUI);
	afx_msg void OnEditHeapobject();
	afx_msg void OnUpdateEditHeapobject(CCmdUI* pCmdUI);
	afx_msg void OnWorldLight();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg BOOL OnPopupCommand(UINT nID);
	afx_msg void OnWorldProperties();
	afx_msg void OnSetupContinent();
	afx_msg void OnUpdateSetupContinent(CCmdUI* pCmdUI);
	afx_msg void OnViewContinent();
	afx_msg void OnUpdateViewContinent(CCmdUI* pCmdUI);
	//}}AFX_MSG
	void OnUpdateCoorX(CCmdUI* pCmdUI);
	void OnUpdateCoorY(CCmdUI* pCmdUI);
	void OnUpdateCoorZ(CCmdUI* pCmdUI);
	void OnUpdateLandName(CCmdUI* pCmdUI);
	void OnUpdateLayer(CCmdUI* pCmdUI);
	void OnUpdateObjectName(CCmdUI* pCmdUI);
	void OnUpdateTerrainName(CCmdUI* pCmdUI); 
	LONG OnCloseObjProperty(UINT wParam,LONG lParam);
	void OnLightMap();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowCalcWorldmap();
	afx_msg void OnUpdateWindowCalcWorldmap(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in BeastView.cpp
inline CWorldDoc* CWorldView::GetDocument()
   { return (CWorldDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//gmpbigsun : undo, redo Rotate and Translate
void CreateModalTranslateObject( CObj* pObj );
void CreateModalRotateObjects( CObjArray* pObjArray );		//for array
void CreateModalScaleObjects( CObjArray* pObjArray );
void Edit_RotateObject( CObj* pObj, float fAngle );					
BOOL Edit_ScaleObject( CObj* pObj, float fAngle );					

#endif // !defined(AFX_WORLDVIEW_H__7BA89B6E_E94B_4B1E_800E_44BFA667D440__INCLUDED_)
	