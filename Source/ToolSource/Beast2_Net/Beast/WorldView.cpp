// WorldView.cpp : implementation of the CWorldView class
//

#include "stdafx.h"
#include "Beast.h"
#include "DlgSetTime.h"
#include "DlgSetGrid.h"
#include "DlgRespawnDefault.h"

#include "WorldDoc.h"
#include "WorldView.h"
#include <process.h>
#include "ObjProperty.h"
#include "ModelView.h"
#include "DlgRespawn.h"
#include "DlgBmpSaveOption.h"
#include "DlgRevival.h"
#include "DlgRandomDegreeScale.h"
#include "DlgSaveoption.h"
#include "DlgWorldLight.h"
#include "DlgWorldProp.h"
#include "PagePropPath.h"
#include "DlgContinent.h"
#include "DlgInputAngle.h"

//@@추가 KYT
#include "ObjectTextureView.h"

#ifdef __CSC_WORLDOBJ_LIST
#include "WorldTreeView.h"
#endif //__CSC_WORLDOBJ_LIST

#ifdef __CSC_UNDO_REDO
#include "CommandBeast.h"
#include ".\worldview.h"
#endif //__CSC_UNDO_REDO

#include "DialogProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSfxTexture      g_SfxTex;
extern CSfxMng          g_SfxMng;
extern CSfxObjMng       g_SfxObjMng;
extern CSfxMeshMng      g_SfxMeshMng;

int CWorldView::m_nOpenViewCnt;

BOOL CWorldView::m_bRandomDegreeScale = FALSE;
BOOL CWorldView::m_bRandomDegree = FALSE;
BOOL CWorldView::m_bRandomScale = FALSE;
int  CWorldView::m_nRandomDegree1 = -180;
int  CWorldView::m_nRandomDegree2 = 180;
FLOAT  CWorldView::m_fRandomScale1 = -0.2f;
FLOAT  CWorldView::m_fRandomScale2 =  0.2f;
FLOAT  CWorldView::m_fGridX = 1.0f;
FLOAT  CWorldView::m_fGridY = 1.0f;
FLOAT  CWorldView::m_fGridZ = 1.0f;
int  CWorldView::m_nRespawnNum = 10;
int  CWorldView::m_nRespawnSec = 60;

C3DRectTracker::C3DRectTracker()
{
	m_pRect = NULL;
	m_pWorldView = NULL;
	m_pd3dDevice = NULL;
	m_pWorld = NULL;
	m_nPickRectPoint = -1;
}
BOOL C3DRectTracker::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nPickRectPoint = -1;
	CObj* pObjFocus = m_pWorld->GetObjFocus();
	if( pObjFocus )
	{
		m_pRect = NULL;
		if( pObjFocus->IsType( OT_MOVER ) && ((CMover*)pObjFocus)->IsRespawn() )
		{
			m_pRect = &((CMover*)pObjFocus)->m_respawnInfo.m_rect; 
			m_dwColor = 0xffff0000;
		}
		if( pObjFocus->IsType( OT_ITEM ) && ((CItem*)pObjFocus)->IsRespawn() )
		{
			m_pRect = &((CItem*)pObjFocus)->m_respawnInfo.m_rect; 
			m_dwColor = 0xffff7f7f;
		}
		if( pObjFocus->IsType( OT_CTRL ) && ((CCommonCtrl*)pObjFocus)->IsRespawn() )
		{
			m_pRect = &((CCommonCtrl*)pObjFocus)->m_respawnInfo.m_rect; 
			m_dwColor = 0xff00ffff;
		}
		if( pObjFocus->IsType( OT_REGION ) )
		{
			m_pRect = &((CRegion*)pObjFocus)->m_region.m_rect;
			if( pObjFocus->GetIndex() == RI_TRIGGER )
				m_dwColor = 0xff0000ff; 
			else
			if( pObjFocus->GetIndex() == RI_ATTRIBUTE )
				m_dwColor = 0xff00ff00;
			else
			{
				//m_pRect = NULL;
				//return FALSE;
			}
		}
		if( m_pRect )
		{
			CRect rect = *m_pRect;
			CPoint pt = CPoint( (int)( pObjFocus->GetPos().x ), (int)( pObjFocus->GetPos().z ) );
			pt.x = pt.x / MPU * MPU;
			pt.y = pt.y / MPU * MPU;
			rect += pt;
			m_nPickRectPoint = PickRectTracker( rect, point );
			if( m_nPickRectPoint >= 0 )
				return TRUE;
		}
	}
	return FALSE;
}
BOOL C3DRectTracker::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if( m_nPickRectPoint >= 0 )
	{
		D3DXVECTOR3 vector;
		CRect rectClient;
		m_pWorldView->GetClientRect( &rectClient );
		if( m_pWorld->ClientPointToVector( NULL, rectClient, point, &m_pWorld->m_matProj, &m_pWorld->m_pCamera->m_matView, &vector ) )
		{
			CObj* pObjFocus = m_pWorld->GetObjFocus();
			//CRect rect = *m_pRect;
			D3DXVECTOR3 vPos = pObjFocus->GetPos();
			vector.x = (float)( int( vector.x / MPU * MPU ) );
			vector.z = (float)( int( vector.z / MPU * MPU ) );
			vPos.x = (float)( int( vPos.x / MPU * MPU ) );
			vPos.z = (float)( int( vPos.z / MPU * MPU ) );

			vector.x -= vPos.x;
			vector.z -= vPos.z;
			vector.x += MPU;
			vector.z += MPU;
			switch( m_nPickRectPoint )
			{
			case 0:
				m_pRect->left = (LONG)( vector.x );
				m_pRect->top = (LONG)( vector.z );
				break;
			case 1:
				m_pRect->right = (LONG)( vector.x );
				m_pRect->top = (LONG)( vector.z );
				break;
			case 2:
				m_pRect->left = (LONG)( vector.x );
				m_pRect->bottom = (LONG)( vector.z );
				break;
			case 3:
				m_pRect->right = (LONG)( vector.x );
				m_pRect->bottom = (LONG)( vector.z );
				break;
			}
			if( m_pRect->left > 0 )  m_pRect->left = 0;
			if( m_pRect->top > 0 )  m_pRect->top = 0;
			if( m_pRect->right < 0 )  m_pRect->right = 0;
			if( m_pRect->bottom < 0 )  m_pRect->bottom = 0;
		}
		return TRUE;
	}
	return FALSE;
}
int C3DRectTracker::PickRectTracker( CRect rect, CPoint ptClient )
{
	D3DXVECTOR3 vOut;
    D3DVIEWPORT9 vp;

	rect.left   = rect.left  / MPU * MPU;
	rect.top    = rect.top   / MPU * MPU;
	rect.right  = rect.right / MPU * MPU;
	rect.bottom = rect.bottom / MPU * MPU;

	POINT point[ 4 ] =
	{
		{ rect.left,  rect.top    },
		{ rect.right, rect.top    },
		{ rect.left,  rect.bottom },
		{ rect.right, rect.bottom }
	};
    m_pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matWorld;

	for( int i = 0; i < 4; i++ )
	{
		FLOAT fY = m_pWorld->GetLandHeight( (FLOAT)point[ i ].x, (FLOAT)point[ i ].y );
		D3DXMatrixTranslation( &matWorld, (FLOAT)point[ i ].x, fY, (FLOAT)point[ i ].y );

		D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_pWorld->m_matProj,
			&m_pWorld->m_pCamera->m_matView, &matWorld );

		CRect rectPoint( (int)( vOut.x - 4 ), (int)( vOut.y - 4 ), (int)( vOut.x + 4 ), (int)( vOut.y + 4 ) );
		if( rectPoint.PtInRect( ptClient ) )
		{
			D3DXVECTOR3 vIntersect;
			CRect rectClient;//( 0, 0, vp.Width, vp.Height );
			m_pWorldView->GetClientRect( &rectClient );
			if( m_pWorld->ClientPointToVector( NULL, rectClient, ptClient, &m_pWorld->m_matProj, &m_pWorld->m_pCamera->m_matView, &vIntersect) )
			{
				return i;
			}
		}
	}
	return -1;
}
void C3DRectTracker::RenderRectTracker(  )
{
	CObj* pObjFocus = m_pWorld->GetObjFocus();
	if( pObjFocus == NULL || m_pRect == NULL )
		return;
	D3DXVECTOR3 vPos = pObjFocus->GetPos();
	CRect rect = *m_pRect;
	CPoint pointTemp = CPoint( (int)( vPos.x ), (int)( vPos.z ) );
	pointTemp.x = pointTemp.x / MPU * MPU;
	pointTemp.y = pointTemp.y / MPU * MPU;
	rect += pointTemp;

	D3DXVECTOR3 vOut;
    D3DVIEWPORT9 vp;

	rect.left   = rect.left  / MPU * MPU;
	rect.top    = rect.top   / MPU * MPU;
	rect.right  = rect.right / MPU * MPU;
	rect.bottom = rect.bottom / MPU * MPU;

	POINT point[ 4 ] =
	{
		{ rect.left,  rect.top    },
		{ rect.right, rect.top    },
		{ rect.left,  rect.bottom },
		{ rect.right, rect.bottom }
	};
    m_pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matWorld;

	for( int i = 0; i < 4; i++ )
	{
		FLOAT fY = m_pWorld->GetLandHeight( (FLOAT)point[ i ].x, (FLOAT)point[ i ].y );
		D3DXMatrixTranslation( &matWorld, (FLOAT)point[ i ].x, fY, (FLOAT)point[ i ].y );

		D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_pWorld->m_matProj,
			&m_pWorld->m_pCamera->m_matView, &matWorld );

		rect.SetRect( (int)( vOut.x - 1 ), (int)( vOut.y - 1 ), (int)( vOut.x + 3 ), (int)( vOut.y + 3 ) ); 
		g_2DRender.RenderFillRect( rect, 0xffffffff );
	}
}

/*
#define LANDTEX_NORMAL   0
#define LANDTEX_DETAIL   1
#define LANDTEX_BLANK    2
#define LANDTEX_LANDMARK 3 

CDlgWorkLandscape::m_nLandTexture;
*/
#ifndef DEGREETORADIAN
	#define DEGREETORADIAN(x) ((2*D3DX_PI*x)/(360))
#endif 

//D3DXToRadian

//D3DXMATRIX g_matProj;
//D3DXMATRIX g_matView;

//LPDIRECT3DINDEXBUFFER9  m_pIB;

//CMover* g_pMover;

DWORD CWorldView::m_dwBrushType    = BRUSHTYPE_TERRAIN;
DWORD CWorldView::m_dwTerrainBrush = 0;
CObj* CWorldView::m_pObjBrush      = NULL;

D3DXVECTOR3 CWorldView::m_vBrushPos     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 CWorldView::m_vBrushScale   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 CWorldView::m_vBrushRotaion = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

int CWorldView::m_nTool = -1;
//int	CWorldView::m_nTool = ID_VIEW_SCROLL;

/*
CObjProperty* CWorldView::m_pObjProperty = NULL;
CPagePropObj*     CWorldView::m_pPageObj = NULL;
CPagePropCtrl*    CWorldView::m_pPageCtrl = NULL; 
CPagePropItem*    CWorldView::m_pPageItem = NULL; 
CPagePropMover*   CWorldView::m_pPageMover = NULL;
CPagePropLight*   CWorldView::m_pPageLight = NULL;
CPagePropRegion*  CWorldView::m_pPageRegion = NULL;
CPagePropTrigger*  CWorldView::m_pPageTrigger = NULL;
CPagePropAttribute*  CWorldView::m_pPageAttribute = NULL;
*/
BOOL                 CWorldView::m_bDlgRespawn = FALSE;
//CDlgRespawn          CWorldView::m_dlgRespawn = 0;
CDlgWorldLight       CWorldView::m_dlgWorldLight;//0;

//int CWorldView::m_nSelect = 0;
int CWorldView::m_nRestrict = ID_RESTRICT_XZ;

/////////////////////////////////////////////////////////////////////////////
// CWorldView

IMPLEMENT_DYNCREATE(CWorldView, CView)

BEGIN_MESSAGE_MAP(CWorldView, CView)
	//{{AFX_MSG_MAP(CWorldView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILTER_SHADING, OnFilterShading)
	ON_COMMAND(ID_FILTER_TEXTURE, OnFilterTexture)
	ON_COMMAND(ID_FILTER_WIRE_FRAME, OnFilterWireFrame)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHADING, OnUpdateFilterShading)
	ON_UPDATE_COMMAND_UI(ID_FILTER_TEXTURE, OnUpdateFilterTexture)
	ON_UPDATE_COMMAND_UI(ID_FILTER_WIRE_FRAME, OnUpdateFilterWireFrame)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, OnUpdateViewZoom)
	ON_COMMAND(ID_VIEW_SCROLL, OnViewScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCROLL, OnUpdateViewScroll)
	ON_COMMAND(ID_VIEW_ROTATION, OnViewRotation)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATION, OnUpdateViewRotation)
	ON_COMMAND(ID_VIEW_FRAMEMOVE, OnViewFramemove)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FRAMEMOVE, OnUpdateViewFramemove)
	ON_COMMAND(ID_EDIT_DRAW, OnEditDraw)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAW, OnUpdateEditDraw)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VIEW_ZOOM_EXTENTS, OnViewZoomExtents)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_EXTENTS, OnUpdateViewZoomExtents)
	ON_COMMAND(ID_VIEW_FOG, OnViewFog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOG, OnUpdateViewFog)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_COMMAND(ID_VIEW_SKYBOX, OnViewSkybox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SKYBOX, OnUpdateViewSkybox)
	ON_COMMAND(ID_VIEW_LOD, OnViewLod)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOD, OnUpdateViewLod)
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
#ifdef __CSC_RENDER_AXIS
	ON_COMMAND(ID_VIEW_AXIS, OnViewAxis)
#endif //__CSC_RENDER_AXIS
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
	ON_COMMAND(ID_VIEW_TERRAIN, OnViewTerrain)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TERRAIN, OnUpdateViewTerrain)
	ON_COMMAND(ID_VIEW_OBJECT, OnViewAllObjects)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECT, OnUpdateViewObject)
	ON_COMMAND(ID_FILE_SAVE_WORLD, OnFileSaveWorld)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_WORLD, OnUpdateFileSaveWorld)
	ON_COMMAND(ID_VIEW_WATER, OnViewWater)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATER, OnUpdateViewWater)
	ON_COMMAND(ID_EXEC_GAME, OnExecGame)
	ON_UPDATE_COMMAND_UI(ID_EXEC_GAME, OnUpdateExecGame)
	ON_COMMAND(ID_FILE_SAVE_AS_WORLD, OnFileSaveAsWorld)
	ON_COMMAND(ID_SELECT, OnSelect)
	ON_UPDATE_COMMAND_UI(ID_SELECT, OnUpdateSelect)
	ON_COMMAND(ID_SELECT_MOVE, OnSelectMove)
	ON_UPDATE_COMMAND_UI(ID_SELECT_MOVE, OnUpdateSelectMove)
	ON_COMMAND(ID_SELECT_ROTATION, OnSelectRotation)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ROTATION, OnUpdateSelectRotation)
	ON_COMMAND(ID_SELECT_SCALE, OnSelectScale)
	ON_UPDATE_COMMAND_UI(ID_SELECT_SCALE, OnUpdateSelectScale)
	ON_WM_KEYUP()
	ON_COMMAND(ID_RESTRICT_X, OnRestrictX)
	ON_UPDATE_COMMAND_UI(ID_RESTRICT_X, OnUpdateRestrictX)
	ON_COMMAND(ID_RESTRICT_Y, OnRestrictY)
	ON_UPDATE_COMMAND_UI(ID_RESTRICT_Y, OnUpdateRestrictY)
	ON_COMMAND(ID_RESTRICT_Z, OnRestrictZ)
	ON_UPDATE_COMMAND_UI(ID_RESTRICT_Z, OnUpdateRestrictZ)
	ON_COMMAND(ID_RESTRICT_XZ, OnRestrictXz)
	ON_UPDATE_COMMAND_UI(ID_RESTRICT_XZ, OnUpdateRestrictXz)
	ON_COMMAND(ID_SELECT_PUT, OnSelectPut)
	ON_UPDATE_COMMAND_UI(ID_SELECT_PUT, OnUpdateSelectPut)
	ON_COMMAND(ID_RESPAWN, OnRespawn)
	ON_UPDATE_COMMAND_UI(ID_RESPAWN, OnUpdateRespawn)
	ON_COMMAND(ID_VIEW_GRID_PATCH, OnViewGridPatch)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID_PATCH, OnUpdateViewGridPatch)
	ON_COMMAND(ID_VIEW_GRID_LANDSCAPE, OnViewGridLandscape)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID_LANDSCAPE, OnUpdateViewGridLandscape)
	ON_COMMAND(ID_FILE_SAVE_AS_BITMAP, OnFileSaveAsBitmap)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_BITMAP, OnUpdateFileSaveAsBitmap)
	ON_COMMAND(ID_VIEW_LIGHT, OnViewLight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHT, OnUpdateViewLight)
	ON_COMMAND(ID_TERRAIN_TEXTURE, OnTerrainTexture)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_TEXTURE, OnUpdateTerrainTexture)
	ON_COMMAND(ID_TERRAIN_COLOR, OnTerrainColor)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_COLOR, OnUpdateTerrainColor)
	ON_COMMAND(ID_TERRAIN_HEIGHT, OnTerrainHeight)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_HEIGHT, OnUpdateTerrainHeight)
	ON_COMMAND(ID_VIEW_SPAWN, OnViewSpawn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SPAWN, OnUpdateViewSpawn)
	ON_COMMAND(ID_VIEW_OBJ, OnViewObj)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJ, OnUpdateViewObj)
	ON_COMMAND(ID_VIEW_MOVER, OnViewMover)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVER, OnUpdateViewMover)
	ON_COMMAND(ID_EDIT_ONLAND, OnEditOnland)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ONLAND, OnUpdateEditOnland)
	ON_COMMAND(ID_VIEW_REGION, OnViewRegion)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REGION, OnUpdateViewRegion)
	ON_COMMAND(ID_VIEW_NAME, OnViewName)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAME, OnUpdateViewName)
	ON_COMMAND(ID_EDIT_GRID, OnEditGrid)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GRID, OnUpdateEditGrid)
	ON_COMMAND(ID_EDIT_LOCK, OnEditLock)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOCK, OnUpdateEditLock)
	ON_COMMAND(ID_VIEW_TOP, OnViewTop)
	ON_COMMAND(ID_TERRAIN_WATER, OnTerrainWater)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_WATER, OnUpdateTerrainWater)
	ON_COMMAND(ID_VIEW_FRONT, OnViewFront)
	ON_COMMAND(ID_VIEW_HEIGHT_ATTRIBUTE, OnViewHeightAttribute)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HEIGHT_ATTRIBUTE, OnUpdateViewHeightAttribute)
	ON_COMMAND(ID_VIEW_OBJLOD, OnViewObjlod)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJLOD, OnUpdateViewObjlod)
	ON_COMMAND(ID_WORLD_REVIVAL, OnWorldRevival)
	ON_UPDATE_COMMAND_UI(ID_WORLD_REVIVAL, OnUpdateWorldRevival)
	ON_COMMAND(ID_EDIT_RANDOM_DEGREE_SCALE, OnEditRandomDegreeScale)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RANDOM_DEGREE_SCALE, OnUpdateEditRandomDegreeScale)
	ON_COMMAND(ID_VIEW_SET_TIME, OnViewSetTime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SET_TIME, OnUpdateViewSetTime)
	ON_COMMAND(ID_EDIT_SETGRID, OnEditSetgrid)
	ON_COMMAND(ID_EDIT_RESPAWN_DEFAULT, OnEditRespawnDefault)
	ON_COMMAND(ID_EDIT_RESPAWN_ENABLE, OnEditRespawnEnable)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESPAWN_ENABLE, OnUpdateEditRespawnEnable)
	ON_COMMAND(ID_EDIT_HEAPOBJECT, OnEditHeapobject)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HEAPOBJECT, OnUpdateEditHeapobject)
	ON_COMMAND(ID_WORLD_LIGHT, OnWorldLight)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_BUTTON_ICON_CONTINENT, OnSetupContinent)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ICON_CONTINENT, OnUpdateSetupContinent)
	ON_COMMAND(ID_VIEW_CONTINENT, OnViewContinent)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTINENT, OnUpdateViewContinent)
	ON_COMMAND_EX(ID_POPUP_WORLD_REMOVE  , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_PROPERTY, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_INVISIBLE    , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_INVISIBLE_UPSTAIRS   , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_SHOW_ALL_OBJECTS   , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_GET_OBJECT    , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_INVISIBLE_COOLSION    , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_ROTATE    , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_TRANSLATION    , OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_WORLD_SCALE    , OnPopupCommand)

	ON_COMMAND(ID_WORLD_PROPERTIES, OnWorldProperties)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COOR_X,   OnUpdateCoorX)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COOR_Y,   OnUpdateCoorY)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COOR_Z,   OnUpdateCoorZ)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LANDNAME,   OnUpdateLandName)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LAYER,   OnUpdateLayer)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECTNAME,   OnUpdateObjectName)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TERRAINNAME,   OnUpdateTerrainName)

	ON_MESSAGE(WM_CLOSE_OBJ_PROP_SHEET ,OnCloseObjProperty)
	ON_COMMAND(ID_WINDOW_CALC_WORLDMAP, OnWindowCalcWorldmap)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CALC_WORLDMAP, OnUpdateWindowCalcWorldmap)
//	ON_COMMAND( ID_BUTTON_ICON_LIGHTMAP, OnLightMap )
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldView construction/destruction


CWorldView::CWorldView()
{
	m_nFilterType = FT_TEXTURE;

	//g_AppFormView          = this;
	//m_bHiResTerrain        = FALSE;
	//m_bHiResTerrainOld     = FALSE;
	//m_dwFogColor           = BACKGROUNDCOLOR;//0x00b5b5ff;
	//m_dwFogMode            = D3DFOG_LINEAR;
	//m_bCanDoTableFog       = FALSE;
	//m_bCanDoVertexFog      = FALSE;
	//m_bCanDoWFog           = FALSE;
	m_bDeviceUsesWFog      = FALSE;
	m_bRangeBasedFog       = FALSE;
	m_bUsingTableFog       = FALSE;
	//m_fFogStartSlider      = 0.0f;
	//m_fFogEndSlider        = 0.3f;
	//m_fFogStartValue       = 0.0f;
	//m_fFogEndValue         = 1.0f;
	//m_fFogDensity          = 0.0f;
	m_hwndRenderWindow     = NULL;
	m_hwndRenderFullScreen = NULL;
	m_dwNumTerrainVertices = 0L;
	m_dwNumColumnVertices  = 0L;
	//m_bUseDepthBuffer      = TRUE;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bMButtonDown = FALSE;
	m_bLBDownMouseMove = FALSE;
	m_bScrollPossible = FALSE; 
	m_bBeastViewReady = FALSE;
	m_bRespawnEnable = FALSE;
	m_nOpenViewCnt++;

	//m_camera.m_fYaw = 0;
	//m_fRoll = 0;
	//m_camera.m_fPitch = 0;//360-45;
	//FLOAT fYaw   = DEGREETORADIAN( m_nYaw );
	//FLOAT fPitch = DEGREETORADIAN( m_nPitch );
	//FLOAT fRoll  = DEGREETORADIAN( m_nRoll );
	//m_vecPos = D3DXVECTOR3((FLOAT)(63/2)*4,200.0f,0.0f);
	//m_vecPos = D3DXVECTOR3(100.0f,100.0f,100.0f);
	///m_vecLookAt = D3DXVECTOR3(m_vecPos.x,0.0f,m_vecPos.z);
	//m_vecLookAt = D3DXVECTOR3(m_vecPos.x+sinf(fYaw),m_vecLookAt.y,m_vecPos.z+cosf(fYaw));

	//m_camera.m_fYaw      = 0;//359;
	//m_camera.m_fPitch    = 0;//320;
	//m_fRoll     = 0;
	//m_vecPos    = D3DXVECTOR3( 63.0f, 157.0f, -57.0f );
	//m_vecLookAt = D3DXVECTOR3( 62.0f,   0.0f,  50.0f );

	m_vIntersect = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bCursorInWorld = FALSE;
	m_bSelectDrag = FALSE;
	//m_pObjCurrent = NULL;
	m_nPickRectPoint = -1;
	m_nPickCollection = 0;

	//GetRayEnd(D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd)
	//m_vecPos = D3DXVECTOR3((FLOAT)(63/2)*4,200.0f,(FLOAT)(63/2)*4);
	//m_vecLookAt = D3DXVECTOR3( m_vecPos.x+0.0001f, 5.0f,m_vecPos.z);
	//vecPos    = D3DXVECTOR3((FLOAT)(63/2)*4,30,(FLOAT)(63/2)*4);
	//vecLookAt = D3DXVECTOR3(m_vecPos.x,m_vecPos.y,(FLOAT)(63/2)*4+1.0f);
	//m_vecPos    = D3DXVECTOR3((FLOAT)(63/2)*4,150.0f,(FLOAT)(63/2)*4);
	//m_vecLookAt = D3DXVECTOR3(m_vecPos.x,0.0f,(FLOAT)(63/2)*4);
	//m_vecAirPlane = D3DXVECTOR3(50.0f,30.0f,50.0f);
	//FLOAT fTheta = (2*D3DX_PI*m_nYaw)/(360-1);//m_nYaw
	//m_vecLookAt = D3DXVECTOR3( m_vecPos.x + sinf(fTheta)*40, 5.0f,m_vecPos.z + cosf(fTheta)*40 );
	//m_vecLookAt = D3DXVECTOR3(m_vecPos.x,100.0f,m_vecPos.z);
	m_vBrushPos     = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vBrushScale   = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vBrushRotaion = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_bShowCursorWhenFullscreen = TRUE;

	m_bPickUpTile = FALSE;
	m_ptField.x = 0;
	m_ptField.y = 0;
	m_bFrameMoving = FALSE;
				
	m_pFont = new CD3DFont( _T("gulim"), 9 );//, D3DFONT_BOLD );

	m_bViewFog = FALSE;
	g_pWorldView = this;

	m_bEditOnLand = FALSE;
	m_bEditHeapObject = FALSE;
	m_bOnGrid = FALSE;

	m_camera.m_vPos = D3DXVECTOR3( -56.0f, 200.0f, -56.0f );
	m_camera.m_fYaw = 0.8f;
	m_camera.m_fPitch = -6.0f;

	m_bLock = FALSE;
	m_VBforLine = NULL;
	m_bSetupContinent = FALSE;

	//g_kDlgContinent.Init( &m_world );
}
CWorldView::~CWorldView()
{
	m_nOpenViewCnt--;
	SAFE_DELETE( m_pFont );
	//g_pMainFrame->m_dlgNavigator.Update( NULL, NULL );	//기존 네비 생략
	g_pMainFrame->m_dlgNavigatorMapView.Update( NULL, NULL );
	m_dlgWorldLight.DestroyWindow();
	m_dlgWorldLight.m_pWorld = NULL;
	m_dlgWorldLight.m_pWorldView = NULL;
	SAFE_DELETE( m_pObjBrush );	
	
	SAFE_RELEASE( m_VBforLine );
}
BOOL CWorldView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.cx = 800;
	cs.cy = 600;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldView drawing

void CWorldView::OnDraw(CDC* pDC)
{
	CWorldDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
  // Update and render a frame
//  if(IsReady())
  {
			CheckForLostFullscreen();
			RenderScene();
			m_bBeastViewReady = TRUE;
    //if (lstrcmp(strStatsPrev, g_AppFormView->PstrFrameStats()) != 0)
      // g_AppFormView->GetDlgItem(IDC_FPS_TEXT)->SetWindowText(g_AppFormView->PstrFrameStats());
  }
}

/////////////////////////////////////////////////////////////////////////////
// CWorldView printing

BOOL CWorldView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWorldView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWorldView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWorldView diagnostics

#ifdef _DEBUG
void CWorldView::AssertValid() const
{
	CView::AssertValid();
}

void CWorldView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWorldDoc* CWorldView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWorldDoc)));
	return (CWorldDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldView message handlers






BOOL CWorldView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
extern CD3DApplication* g_pD3DApp;
BOOL CWorldView::CreateDevice() 
{
	m_hwndRenderWindow = CView::GetSafeHwnd();
	SetApp();

	// Note that for the MFC samples, the device window and focus window
	// are not the same.
	CD3DApplication::m_hWnd = m_hwndRenderWindow;
	CD3DApplication::m_hWndFocus = NULL;//m_hwndRenderFullScreen;
	CD3DApplication::Create( AfxGetInstanceHandle() );
	
	/*
	D3DCAPS9* pCaps;
	D3DFORMAT backBufferFormat;
	DWORD dwBehavior; 
	D3DFORMAT adapterFormat;
	//*/

	//g_pD3DApp->ConfirmDevice( pCaps, dwBehavior, adapterFormat, backBufferFormat );

	// Update UI, and device's fog parameters
	//OnVertexFog();
	//UpdateUIForDeviceCapabilites();
	//SetFogParameters();

	if( m_bFrameMoving )
	{
		DXUtil_Timer( TIMER_RESET );
		SetTimer( 1, 1, NULL );
	}
	// 자동 세이브 타이머 설정 5분 
	//SetTimer(2,1000*60*5,NULL);
	SetTimer( 2, 1000 * 60 * 1, NULL );

	m_3DRectTracker.m_pWorldView = this;
	m_3DRectTracker.m_pd3dDevice = m_pd3dDevice;
	m_3DRectTracker.m_pWorld = &m_world;

	//g_pMainFrame->m_dlgNavigator.Update( &m_world, this );	//기존 네비 생략
	g_pMainFrame->m_dlgNavigatorMapView.Update( &m_world, this );

	return TRUE;
}




LRESULT CWorldView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//	LRESULT result = CD3DApplication::MsgProc(GetSafeHwnd(),message, wParam, lParam);
	m_ArcBall.HandleMouseMessages( CView::GetSafeHwnd(), message, wParam, lParam );

	LRESULT result = CView::WindowProc(message, wParam, lParam);
	//if(g_pWndMgr)
		//g_pWndMgr->WindowProc(message,wParam,lParam);
	return result;
	// TODO: Add your specialized code here and/or call the base class
	//return CWnd::WindowProc(message, wParam, lParam);
}

void CWorldView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	if(m_bBeastViewReady == FALSE)
		return;

	HRESULT hr;
//	if( m_bActive && m_bWindowed)
	{
		RECT rcClientOld;
		rcClientOld = m_rcWindowClient;

		// Update window properties
		GetWindowRect(&m_rcWindowBounds );
		GetClientRect(&m_rcWindowClient );

		if( rcClientOld.right - rcClientOld.left !=
				m_rcWindowClient.right - m_rcWindowClient.left ||
				rcClientOld.bottom - rcClientOld.top !=
				m_rcWindowClient.bottom - m_rcWindowClient.top)
		{
			// A new window size will require a new backbuffer
			// size, so the 3D structures must be changed accordingly.
//			m_bReady = FALSE;

			m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

			// Resize the 3D environment
			if( FAILED( hr = Reset3DEnvironment() ) )
			{
				//DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
				return;
			}
		//	m_bReady = TRUE;
		}
	}	


	
}
/////////////


//-----------------------------------------------------------------------------
// Name: CheckForLostFullscreen()
// Desc: If fullscreen and device was lost (probably due to alt-tab), 
//       automatically switch to windowed mode
//-----------------------------------------------------------------------------
HRESULT CWorldView::CheckForLostFullscreen()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CWorldView::FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CWorldView::FrameMove()
{
	m_world.Process( m_fElapsedTime );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CWorldView::Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendeyyyy. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CWorldView::Render()
{
	// Clear the viewport
//	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 255, 90, 146, 222 ), 1.0f, 0 );////CWorld::m_dwBgColor, 1.0f, 0);
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 255,50, 50, 50 ), 1.0f, 0 );////CWorld::m_dwBgColor, 1.0f, 0);		//!!
//	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x808080, 1.0f, 0);

//	m_world.Finishing();

	// Begin the scene
	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		// m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		//D3DXMATRIX matView, mat;
		//D3DXMatrixLookAtLH( &matView, &m_vecPos, &m_vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		//m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		//m_camera.m_vPos = m_vecPos;
		//m_camera.m_vLookAt = m_vecLookAt;
		//m_camera.m_vPos2= m_camera.m_vPos;

		//	UpdateCullInfo(&g_matView,&g_matProj);
		// 필드 출력 

		m_world.SetCamera( &m_camera );
		m_world.ReadWorld( m_camera.m_vPos );
		m_world.Render( m_pd3dDevice, m_pFont );

#ifdef __Y_PATROL
		g_pMainFrame->m_dlgPatrol.RenderAllPath();

//		CObj* pObj = m_world.GetObjFocus();
#endif //__Y_PATROL

//		m_world.RenderGrids(CPoint(m_vIntersect.x,m_vIntersect.z),CPoint(m_vIntersect.x,m_vIntersect.z),1,0xffff0000);

		/////////////////////////////////////////////////////////////////
		// 브러쉬 출력 
		//
		if( GetTool() == ID_SELECT_PUT && m_pObjBrush )
		{
			POINT ptCursor;
			GetCursorPos( &ptCursor );
			ScreenToClient( &ptCursor );
			D3DXVECTOR3 vector;
			CRect rect;
			GetClientRect(&rect);
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			//if( m_world.ClientPointToVector( rect, ptCursor, &m_world.m_matProj, &m_camera.m_matView, &vector, TRUE ) )
			if( m_bCursorInWorld )
			{
				vector = m_vIntersect;
				if( m_bOnGrid )
				{
					vector.x = vector.x / m_fGridX;//) / 4 ) * 4;
					vector.z = vector.z / m_fGridZ;/// 4 ) * 4;
					vector.x = int( vector.x ) * m_fGridX;
					vector.z = int( vector.z ) * m_fGridZ;/// 4 ) * 4;
				}
				m_pObjBrush->SetWorld( NULL );
				m_pObjBrush->SetPos( m_vBrushPos + vector );
				m_pObjBrush->SetScale( m_vBrushScale );// + D3DXVECTOR3(1.0f,1.0f,1.0f) );
				m_pObjBrush->SetWorld( &m_world );
				m_pObjBrush->UpdateLocalMatrix();
				/*
				if( m_pObjBrush->GetType() == OT_REGION )
				{
					m_pd3dDevice->SetTexture( 0, NULL );
					m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_BLENDDIFFUSEALPHA);
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE);
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE);
					m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
					m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
					m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
					m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				}
				*/

				//m_camera.Transform( m_pd3dDevice, &m_world );
				//::SetTransformView( m_camera.m_matView );		// CModelObject::Render()를 부르기전에 이것을 호출해주자. 내부에서 pd3dDevice->GetTransform()을 안하기 위해서다.
				//::SetTransformProj( m_world.m_matProj );

				if( m_pObjBrush->m_pModel->m_bSkin )	// 스키닝인것
				{
//						m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
					static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );

					if( m_world.m_bIsContinent )		//gmpbigsun : 포그 거리는 대륙정보 우선
					{
						ContinentInfo* pInfo = g_kDlgContinent._pCurContinent;
						if( pInfo )
							vConst.w = ( pInfo->_fFogEnd - m_pObjBrush->m_fDistCamera) / ( pInfo->_fFogEnd - pInfo->_fFogStart );
					}
					else
						vConst.w = (m_world.m_fFogEndValue - m_pObjBrush->m_fDistCamera) / (m_world.m_fFogEndValue - m_world.m_fFogStartValue);

					m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
					SetLightVec( m_world.m_light.Direction );
				}
				m_pObjBrush->Render(m_pd3dDevice);
			}
		}
		if( GetTool() == ID_TERRAIN_HEIGHT ) //&& m_bPickUpTile)
		{
			CPoint point(0,0);
			//point.x = m_vIntersect.x / 1.0f;
			//point.y = m_vIntersect.z / 1.0f;

			if( m_bLButtonDown == FALSE )
			{
				m_ptField.x = (LONG)( m_vIntersect.x / 1.0f + (MPU / 2) ); // 커서 포인트가 중심으로 이동하기위해 좌표 보정 
				m_ptField.y = (LONG)( m_vIntersect.z / 1.0f + (MPU / 2) ); // 커서 포인트가 중심으로 이동하기위해 좌표 보정 
			}
			
			if( CPageTerrainHeight::m_nHeightStyle == 2 )
				EditHeightSoft( point, TRUE );
			else
			if( CPageTerrainHeight::m_nHeightStyle == 1 )
				EditHeightFlat( point, TRUE );
			else
			if( CPageTerrainHeight::m_nHeightStyle == 0 )
				EditHeightMountain( point, TRUE );
			else
			if( CPageTerrainHeight::m_nHeightStyle == 3 ) // random
				EditHeightRandom( point, TRUE );
		}
		if( GetTool() == ID_TERRAIN_TEXTURE )
		{
			//if(m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
			if( m_bCursorInWorld )
			{
				SetTexture( m_vIntersect.x, m_vIntersect.z, (WORD)( m_dwTerrainBrush ),
						(FLOAT)( CPageTerrainTexture::m_nRadiusTexture ), (FLOAT)( CPageTerrainTexture::m_nHardnessTexture ), TRUE ); 
			}
		}

		m_3DRectTracker.RenderRectTracker();
		/*
		CObj* pObj = m_world.GetObjFocus();
		if( pObj && pObj->GetType() == OT_MOVER )
		{
			if( ((CMover*)pObj)->IsRespawn() )
			{
				CRespawnInfo* pRespawn = &((CMover*)pObj)->m_respawnInfo; 
				D3DXVECTOR3 vPos = pObj->GetPos();
				CRect rect = pRespawn->rect;
				CPoint point = CPoint( vPos.x, vPos.z );
				point.x = point.x / MPU * MPU;
				point.y = point.y / MPU * MPU;
				rect += point;
				m_3DRectTracker.RenderRectTracker( rect, 0, 0x808080ff );
			}
		}
		*/
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    FALSE );
		if( m_bSelectDrag == TRUE )
		{
			m_rectDraw.NormalizeRect();
			g_2DRender.RenderRect( &m_rectDraw, 0xffffff00 );
		}
		g_2DRender.SetFont( m_pFont );
		CString strCamera;
		strCamera.Format( "Camera x = %f, y = %f, z= %f", m_camera.m_vPos.x, m_camera.m_vPos.y, m_camera.m_vPos.z );
		g_2DRender.TextOut( 1, 1, strCamera, 0xff000000 );
		g_2DRender.TextOut( 0, 0, strCamera, 0xffffffff );

		//gmpbigsun : 대륙정보 라인 
		if( g_kDlgContinent.IsWindowVisible( ) && g_kDlgContinent._pCurContinent )
			RenderLines( );

		// End the scene.
		m_pd3dDevice->EndScene();
	}

//	g_pMainFrame->m_dlgNavigator.UpdatePos();		//기존 네비 생략
	g_pMainFrame->m_dlgNavigatorMapView.UpdatePos();

	return S_OK;
}
/*
int CWorldView::PickRectTracker( CRect rect, CPoint ptClient )
{
	D3DXVECTOR3 vOut;
    D3DVIEWPORT8 vp;

	rect.left   = rect.left  / MPU * MPU;
	rect.top    = rect.top   / MPU * MPU;
	rect.right  = rect.right / MPU * MPU;
	rect.bottom = rect.bottom / MPU * MPU;

	POINT point[ 4 ] =
	{
		{ rect.left,  rect.top    },
		{ rect.right, rect.top    },
		{ rect.left,  rect.bottom },
		{ rect.right, rect.bottom }
	};
    m_pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matWorld;

	for( int i = 0; i < 4; i++ )
	{
		FLOAT fY = m_world.GetLandHeight( (FLOAT)point[ i ].x, (FLOAT)point[ i ].y );
		D3DXMatrixTranslation( &matWorld, (FLOAT)point[ i ].x, fY, (FLOAT)point[ i ].y );

		D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
			&m_world.m_pCamera->m_matView, &matWorld );

		CRect rectPoint( vOut.x - 4, vOut.y - 4, vOut.x + 4, vOut.y + 4); 
		if( rectPoint.PtInRect( ptClient ) )
		{
			D3DXVECTOR3 vIntersect;
			CRect rectClient;
			GetClientRect(&rectClient);
			if( m_world.ClientPointToVector( rectClient, ptClient, &m_world.m_matProj, &m_camera.m_matView, &vIntersect) )
			{
				return i;
			}
		}
	}
	return -1;
}
void CWorldView::MoveRectTracker( CRect rect, CPoint ptClient )
{
	D3DXVECTOR3 vOut;
    D3DVIEWPORT8 vp;

	rect.left   = rect.left  / MPU * MPU;
	rect.top    = rect.top   / MPU * MPU;
	rect.right  = rect.right / MPU * MPU;
	rect.bottom = rect.bottom / MPU * MPU;

	POINT point[ 4 ] =
	{
		{ rect.left,  rect.top    },
		{ rect.right, rect.top    },
		{ rect.left,  rect.bottom },
		{ rect.right, rect.bottom }
	};
    m_pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matWorld;

	for( int i = 0; i < 4; i++ )
	{
		FLOAT fY = m_world.GetLandHeight( (FLOAT)point[ i ].x, (FLOAT)point[ i ].y );
		D3DXMatrixTranslation( &matWorld, (FLOAT)point[ i ].x, fY, (FLOAT)point[ i ].y );

		D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
			&m_world.m_pCamera->m_matView, &matWorld );

		rect.SetRect( vOut.x - 1, vOut.y - 1, vOut.x + 3, vOut.y + 3); 
		if( rect.PtInRect( ptClient ) )
		{
			D3DXVECTOR3 vIntersect;
			CRect rectClient;
			GetClientRect(&rectClient);
			if( m_world.ClientPointToVector( rectClient, ptClient, &m_world.m_matProj, &m_camera.m_matView, &vIntersect) )
			{
				point[ i ].x = vIntersect.x;
				point[ i ].y = vIntersect.z;
				break;
			}
		}
	}
}
*/
/*
void CWorldView::RenderRectTracker( CRect rect, WORD dx, DWORD color )
{
	m_world.RenderGrids( rect, dx, color );

	D3DXVECTOR3 vOut;
    D3DVIEWPORT8 vp;

	rect.left   = rect.left  / MPU * MPU;
	rect.top    = rect.top   / MPU * MPU;
	rect.right  = rect.right / MPU * MPU;
	rect.bottom = rect.bottom / MPU * MPU;

	POINT point[ 4 ] =
	{
		{ rect.left,  rect.top    },
		{ rect.right, rect.top    },
		{ rect.left,  rect.bottom },
		{ rect.right, rect.bottom }
	};
    m_pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matWorld;

	for( int i = 0; i < 4; i++ )
	{
		FLOAT fY = m_world.GetLandHeight( (FLOAT)point[ i ].x, (FLOAT)point[ i ].y );
		D3DXMatrixTranslation( &matWorld, (FLOAT)point[ i ].x, fY, (FLOAT)point[ i ].y );

		D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
			&m_world.m_pCamera->m_matView, &matWorld );

		rect.SetRect( vOut.x - 1, vOut.y - 1, vOut.x + 3, vOut.y + 3); 
		g_2DRender.RenderFillRect( rect, 0xffffffff );
	}
}
*/
void CWorldView::SetTool( int nTool )
{
	//m_nSelect = nEditType;
	m_nTool = nTool;
//	m_nTool = -1;
}
int CWorldView::GetTool()
{
	return m_nTool;
/*
	if( CWorldView::m_dwBrushType == BRUSHTYPE_OBJECT )
		return EDITTYPE_OBJECT;
	if( CDlgWorkLandscape::m_nLandscapeTool == 0 )
		return ID_TERRAIN_HEIGHT ;
	if( CDlgWorkLandscape::m_nLandscapeTool == 1 )
		return ID_TERRAIN_TEXTURE ;
	if( CDlgWorkLandscape::m_nLandscapeTool == 2 )
		return ID_TERRAIN_COLOR ;
	//if( CDlgWorkLandscape::m_nLandscapeTool == 2 )
	//	return ID_TERRAIN_COLOR ;
*/
	return -1 ;
}
void CWorldView::RenderDX()
{
	/*
	if(m_nFilterType == FT_WIRE_FRAME)
	{
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetTexture( 0, NULL );
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	}
	*/
//	m_world.m_skyBox.Render( &m_world, m_pd3dDevice );
//	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//D3DXMATRIX matView, mat;
	//D3DXMatrixLookAtLH( &matView, &m_vecPos, &m_vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	//m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	//g_matView = matView;

  // 필드 출력 
	//m_world.Render(m_pd3dDevice);
/*
  m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DXMATRIX  matWorld;
	D3DXMatrixIdentity(&matWorld); 
	D3DXMatrixScaling(&mat,50,50,50);
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
	//FLOAT fTheta = (2*D3DX_PI*m_nAngle)/(360-1);//m_nAngle
	//D3DXMatrixRotationZ( &mat, fTheta );
	//D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
	D3DXMatrixTranslation(&mat,63*4/2,10,63*4/2);//m_vecPos.x,m_vecPos.y,m_vecPos.z);
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
*/

/*
	// 브러쉬 출력 
	if( GetTool() == ID_SELECT_PUT && m_pObjBrush )
	{
		POINT ptCursor;
		GetCursorPos( &ptCursor );
		ScreenToClient( &ptCursor );
		D3DXVECTOR3 vector;
		CRect rect;
		GetClientRect( &rect );
		if( m_world.ClientPointToVector( rect, ptCursor, &g_matProj,&g_matView,&vector ) )
		{
			m_pObjBrush->SetPos( m_vBrushPos + vector );
			m_pObjBrush->SetScale( m_vBrushScale + D3DXVECTOR3(1.0f,1.0f,1.0f) );
			m_pObjBrush->Render(m_pd3dDevice);
		}
		*/
		/*
		D3DXVECTOR3 vPickRayOrig;
		D3DXVECTOR3 vPickRayDir ;
		// 화면의 중심에서 마우스 포인터가 닿는 곳까지 광선을 쏴 마우스를 포인팅할 지점을 얻는다.
		GetPickRay(ptCursor,&g_matProj,&g_matView,&vPickRayOrig,&vPickRayDir);
		GetRayEnd(&vPickRayOrig,&vPickRayDir,&vPickRayDir);

		g_pMainFrame->m_pObjBrush->m_vecPos = vPickRayDir;
		//g_pMainFrame->m_pObjBrush->m_vecPos.y = 10;//2m_world.GetTile(vPickRayDir)->m_byHeight;
		g_pMainFrame->m_pObjBrush->m_vecScale = D3DXVECTOR3(0.4f,0.4f,0.4f);
		g_pMainFrame->m_pObjBrush->Render(m_pd3dDevice);
		*/
//	}
/*
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
	*/
  //m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
  //m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

/*
  // Show frame rate
  m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
  m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
	CHAR string[64];
	sprintf(string,"Position = x,y,z(%f,%f,%f)",m_vecPos.x,m_vecPos.y,m_vecPos.z);
  m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), string );
	sprintf(string,"LookAt = x,y,z(%f,%f,%f)",m_vecLookAt.x,m_vecLookAt.y,m_vecLookAt.z);
  m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), string );
	sprintf(string,"Yaw = %d",m_nYaw);
  m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,0), string );
	sprintf(string,"Pitch = %d",m_nPitch);
  m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,0), string );

	// 라이팅 
  D3DLIGHT8 light;
	D3DUtil_InitLight( light, D3DLIGHT_POINT, m_vecPos.x, m_vecPos.y, m_vecPos.z );
  light.Attenuation0 =  0.1f;
  light.Range        = 200.0f;
  m_pd3dDevice->SetLight( 0, &light );
  m_pd3dDevice->LightEnable( 0, TRUE );
  */
  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    FALSE );

}				
void CWorldView::RenderDC()
{
	return;
	/*
	IDirect3DSurface8* pDestSurface;
	m_pd3dDevice->GetRenderTarget(&pDestSurface);
	//if(m_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface) == D3D_OK)
	{
		D3DLOCKED_RECT lockedRect;
		CRect rectIn (0,0,63,63);
		if(pDestSurface->LockRect(&lockedRect,rectIn,0) == D3D_OK)// Lock
		{
			g_pMemDC->SelectSurface(lockedRect.pBits,lockedRect.Pitch/2);
			g_pWndMgr->Paint(g_pMemDC);
			pDestSurface->UnlockRect();

		}
	}
	pDestSurface->Release();
	*/
}				
//-----------------------------------------------------------------------------
// Name: CWorldView::InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CWorldView::InitDeviceObjects()
{
	// Get the device caps
	//m_pFont = new CD3DFont( _T("gulim"), 9 );//, D3DFONT_BOLD );

	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps );
	CWorld::StaticInitDeviceObjects( m_pd3dDevice );
	prj.m_terrainMng.InitDeviceObjects( m_pd3dDevice );
	m_world.InitDeviceObjects(m_pd3dDevice);
//	g_TextureMng.SetD3DDevice( m_pd3dDevice );
	m_pFont->InitDeviceObjects( m_pd3dDevice );
	g_2DRender.InitDeviceObjects( m_pd3dDevice );
	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);
	
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWorldView::RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CWorldView::RestoreDeviceObjects()
{
    // Setup the arcball parameters
    m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0.85f );
    m_ArcBall.SetRadius( 1.0f );
	
   m_pFont->RestoreDeviceObjects();
	CWorld::StaticRestoreDeviceObjects( m_pd3dDevice );

    // Set up the object material
#ifdef	__XVS
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
#ifdef	__X2LINK
		CreateSkinningVS( m_pd3dDevice, MakePath( _T( "" ), _T( "skining2.vsh" ) ) );
#else
		CreateSkinningVS( m_pd3dDevice, MakePath( _T( "" ), _T( "skining.vsh" ) ) );
#endif
    }
#endif
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );

    // Set up a texture
    //m_pd3dDevice->SetTexture( 0, m_pFloorTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    //m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );

    // Set the transform matrices
    D3DXMATRIX matWorld, matProj;
    D3DXMatrixIdentity(&matWorld);
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	FLOAT fAspect = ( FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane, CWorld::m_fFarPlane );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_world.m_matProj = matProj;

    // Set up the light
    D3DLIGHT9 light;
    //D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 50.0f, 0.0f );
	D3DUtil_InitLight( light, D3DLIGHT_POINT, m_camera.m_vPos.x, m_camera.m_vPos.y, m_camera.m_vPos.z );
    light.Attenuation0 =  0.1f;
    light.Range        = 200.0f;
    //m_pd3dDevice->SetLight( 0, &light );
    //m_pd3dDevice->LightEnable( 0, TRUE );
    //m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,            TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,       TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,     FALSE );

		m_pd3dDevice->SetRenderState(    D3DRS_SHADEMODE  ,     D3DSHADE_GOURAUD  );

 //   m_pAirplane->RestoreDeviceObjects( m_pd3dDevice );

//		m_skinMesh.RestoreDeviceObjects();
	//	m_pSkyBox->RestoreDeviceObjects( m_pd3dDevice );
		m_world.RestoreDeviceObjects(m_pd3dDevice);
		prj.m_modelMng.RestoreDeviceObjects(m_pd3dDevice);

        //D3DLIGHT8 light;
		
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_DIRECTIONAL;

    light.Diffuse.r = 1.0;
    light.Diffuse.g = 1.0;
    light.Diffuse.b = 1.0;
    light.Specular.r = 0;
    light.Specular.g = 0;
    light.Specular.b = 0;
    light.Ambient.r = 0.25;
    light.Ambient.g = 0.25;
    light.Ambient.b = 0.25;

    light.Direction = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);

	//HRESULT hr;
    //hr = m_pd3dDevice->SetLight(0, &light );
    //if (FAILED(hr))
      //  return E_FAIL;

    //hr = m_pd3dDevice->LightEnable(0, TRUE);
    //if (FAILED(hr))
      //  return E_FAIL;

    // Set Light for vertex shader
    D3DXVECTOR4 vLightDir( 0.0f, 0.0f, 1.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstantF( 1, (float*)&vLightDir, 1);
	g_2DRender.RestoreDeviceObjects( &m_d3dsdBackBuffer );// .RestoreDeviceObjects( m_pd3dDevice );
	g_SfxMng.RestoreDeviceObjects();
	
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWorldView::InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CWorldView::InvalidateDeviceObjects()
{
#ifdef	__XVS
	DeleteVertexShader( m_pd3dDevice );
#endif
	CWorld::StaticInvalidateDeviceObjects();
	m_pFont->InvalidateDeviceObjects();
	m_world.InvalidateDeviceObjects();
	prj.m_modelMng.InvalidateDeviceObjects();
	prj.m_terrainMng.InvalidateDeviceObjects();
	g_2DRender.InvalidateDeviceObjects();
	g_SfxMng.InvalidateDeviceObjects();
	g_SfxMeshMng.InvalidateDeviceObjects();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CWorldView::FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CWorldView::FinalCleanup()
{
	SAFE_DELETE(m_pFont);
	//SAFE_DELETE(m_pObjBrush);
	// SAFE_DELETE(m_pSkyBox);
   return S_OK;
}



//-----------------------------------------------------------------------------
// Name: CWorldView::DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CWorldView::DeleteDeviceObjects()
{
	//SAFE_RELEASE( m_pColumnVB );
	//SAFE_RELEASE( m_pTerrainVB );
	//SAFE_RELEASE( m_pFloorTexture );
	CWorld::StaticDeleteDeviceObjects();
	m_pFont->DeleteDeviceObjects();
	m_world.DeleteDeviceObjects();
	m_world.Free();
	g_2DRender.DeleteDeviceObjects();
	prj.m_modelMng.DeleteDeviceObjects();
	prj.m_terrainMng.DeleteDeviceObjects();
	g_TextureMng.DeleteDeviceObjects();
	g_SfxMng.DeleteDeviceObjects();
	g_SfxTex.DeleteAll();
	g_SfxObjMng.RemoveAll();
	g_SfxMeshMng.DeleteAll();
	g_Object3DMng.Destroy();	
	
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: CWorldView::ConfirmDevice()
// Desc: Called duyyyy device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CWorldView::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
                                 D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
//HRESULT CWorldView::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
//		D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	HRESULT hr;
	hr = 0;
	// 스키닝 코드 
	// This sample wants mixed vertex processing rather than hardware
	// vertex processing so it can fallback to sw processing if the 
	// device supports fewer than three matrices when skinning.
#ifdef	__XVS
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1) )
            return S_OK;	
		else
		{
			Error( "그래픽카드가 버텍스쉐이더 1.1을 지원하지 않아 실행할 수 없습니다.\r\n\r\nGeForce2 혹은 Ati Radeon 7xxx급이 이에 해당하며 곧 지원 예정입니다." );
			exit(1);
//			SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
		}
    }
//	else	
//	{
//		// Software vertex processing always supports vertex shaders
//		return S_OK;
//	}
    // Check that the device can blend vertices with at least two matrices
    // (Software can always do up to 4 blend matrices)
    if( pCaps->MaxVertexBlendMatrices < 2 )
        return E_FAIL;

    // If this is a TnL device, make sure it supports directional lights
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
            return E_FAIL;
    }
#else
	if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;
#endif

	if( pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		return S_OK;

	return S_OK;/*
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE
	// 스키닝 코드 
    // This sample wants mixed vertex processing rather than hardware
    // vertex processing so it can fallback to sw processing if the 
    // device supports fewer than three matrices when skinning.
    if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        return E_FAIL;

    if( pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
        return S_OK;

    return E_FAIL;
	*/
}

//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Adjusts the window properties for windowed or fullscreen mode
//-----------------------------------------------------------------------------
HRESULT CWorldView::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }
    return S_OK;
}







//-----------------------------------------------------------------------------
// Name: CAppForm::OneTimeSceneInit()
// Desc: Called duyyyy initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CWorldView::OneTimeSceneInit()
{
	/*
#ifdef _WIN64
    SetClassLongPtr( GetSafeHwnd(), GCLP_HCURSOR, (LONG_PTR)LoadCursor( NULL, IDC_SIZEALL ) );
#else
    SetClassLong( GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor( NULL, IDC_SIZEALL ) );
#endif
		*/
    return S_OK;
}

void CWorldView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

/*	switch( nChar )
	{
	case VK_DELETE:
		if( m_nTool == ID_BUTTON_ICON_CONTINENT )
			SubLineData_Back( );

		break;
	} */

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CWorldView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	//D3DXVECTOR3 vPos = g_pMover->GetPos();

	//gmpbigsun : LCONTROL + R ( 수치 입력받아서 회전 )
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
	{
		if(nChar == 'R' )
		{
			if( !m_world.m_arySelect.empty( ) )
				CreateModalRotateObjects( &m_world.m_arySelect );
		}
		else if( nChar == 'T' )
		{
			CObj* pObj = m_world.GetObjFocus( );
			if( pObj )
				CreateModalTranslateObject( pObj );
		}
	}


	switch(nChar)
	{
		case 'Q':
			//g_pMover->SetMotion(0);
			break;
		case 'W':
			//g_pMover->SetMotion(1);
			break;
		case VK_LEFT:
			RotationObjects( D3DXToRadian( -45 ) );
			Invalidate( FALSE );
			break;
		case VK_UP:
			//vPos += D3DXVECTOR3(0,0,0.5f);			
			break;
		case VK_RIGHT:
			RotationObjects( D3DXToRadian( 45 ) );
			Invalidate( FALSE );
			break;
		case VK_DOWN:
			//g_pMover->m_vecPos -= D3DXVECTOR3(0,0,0.5f);						
			break;
		case VK_DELETE:
			m_world.DeleteSelectObj();
			m_aPickCollection.RemoveAll();
			m_nPickCollection = 0;
			Invalidate( FALSE );
			break;
	}

/*
	int nAngle;
	FLOAT fTheta;
	switch(nChar)
	{
		case 'Q':
			g_pMover->SetMotion(0);
			break;
		case 'W':
			g_pMover->SetMotion(1);
			break;
	//D3DXMATRIX* WINAPI D3DXMatrixRotationYawPitchRoll( D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll );
		case 'R': // up
			nAngle = m_nYaw;
			if(nAngle < 0) nAngle = 360 + nAngle;
			fTheta = (2*D3DX_PI*nAngle)/(360-0);//m_nYaw
		  vecPos = D3DXVECTOR3(sinf(fTheta), 0,cosf(fTheta));
			m_vecPos += vecPos;
			//m_//vecLookAt += vecPos;
			break;
		case 'F': // down
			nAngle = m_nYaw - 180;
			if(nAngle < 0) nAngle = 360 + nAngle;
			fTheta = (2*D3DX_PI*nAngle)/(360-0);//m_nYaw
		  vecPos = D3DXVECTOR3(sinf(fTheta), 0,cosf(fTheta));
			m_vecPos += vecPos;
			//m_vecLookAt += vecPos;
			break;
		case 'D': // left
			nAngle = m_nYaw - 90;
			if(nAngle < 0) nAngle = 360 + nAngle;
			fTheta = (2*D3DX_PI*nAngle)/(360-1);//m_nYaw
		  vecPos = D3DXVECTOR3(sinf(fTheta), 0,cosf(fTheta));
			m_vecPos += vecPos;
			//m_vecLookAt += vecPos;
			break;
		case 'G': // right
			nAngle = m_nYaw + 90;
			if(nAngle >= 360) nAngle -= 360;
			fTheta = (2*D3DX_PI*nAngle)/(360-1);//m_nYaw
		  vecPos = D3DXVECTOR3(sinf(fTheta), 0,cosf(fTheta));
			m_vecPos += vecPos;
			//_vecLookAt += vecPos;
			break;
		case 'A':// m_fTheta
			m_vecPos.y++;
			break;
		case 'Z':
			{
			m_vecPos.y--;
			//if(m_vecPos.y < 0) m_vecPos.y = 0;
			}
			break;
		case 'S':// m_fTheta
			m_vecLookAt.y++;
			break;
		case 'X':
			{
			m_vecLookAt.y--;
			//if(m_vecLookAt.y < 0) m_vecLookAt.y = 0;
			}
			break;

		case VK_LEFT:// m_fTheta
			m_nYaw--;
			if(m_nYaw < 0 ) m_nYaw = 359;
			break;
		case VK_UP:
			{
				//m_vecLookAt.y += 0.1f;
			m_nPitch--;
			if(m_nPitch < 0 ) m_nPitch = 359;
			break;

			////fTheta = (2*D3DX_PI*m_nYaw)/(360-0);//m_nYaw
			//m_vecPos = D3DXVECTOR3( m_vecPos.x + sinf(fTheta)*1, m_vecPos.y,m_vecPos.z + cosf(fTheta)*1 );
			//TRACE("x = %f,y = %f,z = %f\n",m_vecPos.x,m_vecPos.y,m_vecPos.z);
			}
			break;
		case VK_RIGHT:
			m_nYaw++;
			if(m_nYaw >= 360) m_nYaw = 0;
			break;
		case VK_DOWN:
			{
			//	m_vecLookAt.y -= 0.1f;
			m_nPitch++;
			if(m_nPitch >= 360) m_nPitch = 0;
			break;

			//int nDegree = m_nYaw + 180;
			//if(nDegree >= 360) nDegree = nDegree - 360;
			//FLOAT fTheta = (2*D3DX_PI*nDegree)/(360-0);//m_nYaw
			//m_vecPos = D3DXVECTOR3( m_vecPos.x + sinf(fTheta)*1, m_vecPos.y,m_vecPos.z + cosf(fTheta)*1 );
			//TRACE("x = %f,y = %f,z = %f\n",m_vecPos.x,m_vecPos.y,m_vecPos.z);
			}
			break;
	}
	*/
	//FLOAT fYaw   = (2*D3DX_PI*m_nYaw)/(360-1);
	//FLOAT fPitch = (2*D3DX_PI*m_nPitch)/(360-1);
	//FLOAT fRoll  = (2*D3DX_PI*m_nRoll)/(360-1);
	/*
	D3DXMATRIX mat;
	D3DXVECTOR3 vec;
	D3DXMatrixRotationYawPitchRoll(&mat,fYaw,fPitch,fRoll);

	vec = D3DXVECTOR3(0,0,1.0f);
	m_vecLookAt.x	=	vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13  ;
	m_vecLookAt.y =	vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23  ;
	m_vecLookAt.z	=	vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33  ;

	m_vecLookAt += m_vecPos;
	*/
  //m_vecLookAt = D3DXVECTOR3(m_vecPos.x+sinf(fYaw),m_vecPos.y+sinf(fPitch)*m_vecPos.y,m_vecPos.z+cosf(fYaw));
	//		m_vecLookAt = D3DXVECTOR3(m_vecPos.x+sinf(fYaw),m_vecLookAt.y,m_vecPos.z+cosf(fYaw));
/*
	FLOAT a = cosf(fPitch);
	TRACE("%f,\n",a);
	D3DXMATRIX mat;
	D3DXVECTOR3 vec;
	D3DXMatrixRotationYawPitchRoll(&mat,fYaw,fPitch,fRoll);
	D3DXMatrixRotationY(&mat,fYaw);

	vec = D3DXVECTOR3(0,0,1.0f);
	m_vecLookAt.x	=	vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13  ;
	m_vecLookAt.y =	vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23  ;
	m_vecLookAt.z	=	vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33  ;

	m_vecLookAt += m_vecPos;
*/
// x = 1 * -sin(t);
// y = 1 * 0
// z = 1 * 
//	m_vecLookAt.x	=	m_vecLookAt.x * mat._11 + m_vecLookAt.y * mat._21 + m_vecLookAt.z * mat._31 + 1 * mat._41  ;
//	m_vecLookAt.y =	m_vecLookAt.x * mat._12 + m_vecLookAt.y * mat._22 + m_vecLookAt.z * mat._32 + 1 * mat._42  ;
//	m_vecLookAt.z	=	m_vecLookAt.x * mat._13 + m_vecLookAt.y * mat._23 + m_vecLookAt.z * mat._33 + 1 * mat._43  ;

	//D3DXMatrixTranslation(&mat,1,0,0);
	//D3DXVec3TransformCoord(&vec,&D3DXVECTOR3(1,1,1),&mat);
  //m_vecLookAt = D3DXVECTOR3(sinf(fYaw), 0,cosf(fYaw));

		//Vector

/*
	D3DXMATRIX mat;

	//D3DXMatrixRotationYawPitchRoll(&mat,fYaw,fPitch,fRoll);
	D3DXMatrixRotationY(&mat,fYaw);
	FLOAT x = 1, y = 1, z = 1;

	m_vecLookAt.x	=	x * mat._11 + y * mat._21 + z * mat._31 + 1 * mat._41  ;
	m_vecLookAt.y =	x * mat._12 + y * mat._22 + z * mat._32 + 1 * mat._42  ;
	m_vecLookAt.z	=	x * mat._13 + y * mat._23 + z * mat._33 + 1 * mat._43  ;
	//m_vecLookAt.x		1 = x*_14+y*_24+z*_34+1*_44  

	/*
			(x,y,z,1 ) (mat._11 = ;mat._12 = ;mat._13 = ;mat._14 = ;)
			           (mat._21 = ;mat._22 = ;mat._23 = ;mat._24 = ;)
			           (mat._31 = ;mat._32 = ;mat._33 = ;mat._34 = ;)
			           (mat._41 = ;mat._42 = ;mat._43 = ;mat._44 = ;)
			x * _11 + y * 

	*/
	//m_vecLookAt.x = ;
	//m_vecLookAt.y = 0;
	//m_vecLookAt.z = cosf(fYaw);
/*

	D3DXMatrixIdentity(mat);
	mat._11 = ;mat._12 = ;mat._13 = ;
	mat._21 = ;mat._22 = ;mat._23 = ;
	mat._31 = ;mat._32 = ;mat._33 = ;

	m_vecLookAt.x = sinf(fYaw);
	m_vecLookAt.y = 0;
	m_vecLookAt.z = cosf(fYaw);
		
			
		= D3DXVECTOR3(sinf(fYaw), 0,cosf(fYaw));
	//D3DXVECTOR3 f = D3DXVECTOR3(0,sinf(fPitch),cosf(fPitch));
	//m_vecLookAt += f;//D3DXVECTOR3(0,sinf(fPitch),cosf(fPitch));
q.x = sin(theta/2) * axis.x
q.y = sin(theta/2) * axis.y
q.z = sin(theta/2) * axis.z
q.w = cos(theta/2)

                  fYaw    fPitch   fRoll
	m_vecLookAt.x	=	sin(10) 0        sin;
	m_vecLookAt.y =	0       sin(10)  cos;
	m_vecLookAt.z	=	cos(10) cos(10)  0  ;

	x = x * sin(10) + y * 0       + z * cos(10);
	y = x * 0       + y * sin(10) + z * cos(10);
	z = x * sin(10) + y * cos(10) + z * 0      ;

                  fYaw    fPitch   fRoll
	m_vecLookAt.x	=	sin(10) 0        0  ;
	m_vecLookAt.y =	0       0        0  ;
	m_vecLookAt.z	=	cos(10) 0        0  ;


	/////////////////////////////////////////////
	        x       y       z
	yaw   = sin(10) 0       cos(10)
	pitch = 0       sin(10) cos(10)  *  (x,y,z)
	roll  = sin(10) cos(20) 0  

	yaw
	x = x * sin(10) + y * 0       + z * sin(10);
	y = x * 0       + y * 0       + z * 0      ;
	z = x * 0       + y * 0       + z * 0  
		
	pitch
	x = x * 0       + y * 0       + z * 0      ;
	y = x * 0       + y * sin(10) + z * 0      ;
	z = x * cos(10) + y * 0       + z * 0  



	x 축
	x = 0
	y = cos + -sin
	z = sin + cos

	y 축
	x = cos + sin
	y = -sin + cos
	z = 0;

	z 축 
	x = cos + -sin
	y = sin + cos
	z = 0


  //m_vecLookAt = D3DXVECTOR3(sinf(fYaw), 0,cosf(fYaw));
	//D3DXVECTOR3 f = D3DXVECTOR3(0,sinf(fPitch),cosf(fPitch));
	//m_vecLookAt += f;//D3DXVECTOR3(0,sinf(fPitch),cosf(fPitch));
	D3DXMatrixRotationY(&mat,fYaw);
	m_vecLookAt.x = 

			(x,y,z,1 ) (mat._11 = ;mat._12 = ;mat._13 = ;mat._14 = ;)
			           (mat._21 = ;mat._22 = ;mat._23 = ;mat._24 = ;)
			           (mat._31 = ;mat._32 = ;mat._33 = ;mat._34 = ;)
			           (mat._41 = ;mat._42 = ;mat._43 = ;mat._44 = ;)
			x = x*_11+y*_21+z*_31+1*_41  
			y = x*_12+y*_22+z*_32+1*_42  
			z = x*_13+y*_23+z*_33+1*_43  
			1 = x*_14+y*_24+z*_34+1*_44  
	*/

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CWorldView::OpenWorld( LPCTSTR lpszFileName )
{
	m_world.SetCamera( &m_camera );
	CString strFileName = lpszFileName;
	strFileName.MakeLower();
	if( strFileName.Find( _T( ".bmp" ) ) != -1 )
	{
		if( m_world.OpenBmpWorld( strFileName ) )
		{
			//m_vecPos    = m_camera.m_vPos   ;
			//m_vecLookAt = m_camera.m_vLookAt;
			InvalidateDeviceObjects();
			RestoreDeviceObjects( );// pView->m_pd3dDevice );
			return TRUE;
		}
	}
	else
	if( m_world.OpenWorld( strFileName ) )
	{
		//m_vecPos    = m_camera.m_vPos   ;
		//m_vecLookAt = m_camera.m_vLookAt;
		InvalidateDeviceObjects();
		RestoreDeviceObjects( );// pView->m_pd3dDevice );

		//네비게이션의 맵텍스쳐 로딩
		g_pMainFrame->m_dlgNavigatorMapView.LoadNaviMapTexture( );

		return TRUE;
	}
	return FALSE;
}
void CWorldView::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 1 )
	{
//		if(IsReady())
		{
			CheckForLostFullscreen();
			RenderScene();
		}
	}
	else
	if( nIDEvent == 2 )
	{
		if( m_world.m_szFileName[0] )
		{
			;//	m_world.SaveWorld( m_world.m_szFileName, TRUE );
		}
	}
	CView::OnTimer(nIDEvent);
}

void CWorldView::OnDestroy() 
{
	CView::OnDestroy();
	// TODO: Add your message handler code here
	Cleanup3DEnvironment();
	KillTimer(1);
}

void CWorldView::OnFilterShading() 
{
	m_nFilterType = FT_SHADING;
	Invalidate(FALSE);
}

void CWorldView::OnFilterTexture() 
{
	m_nFilterType = FT_TEXTURE;
	Invalidate(FALSE);
}

void CWorldView::OnFilterWireFrame() 
{
	m_nFilterType = FT_WIRE_FRAME;
	Invalidate(FALSE);
}	

void CWorldView::OnUpdateFilterShading(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nFilterType == FT_SHADING);
}

void CWorldView::OnUpdateFilterTexture(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nFilterType == FT_TEXTURE);
}

void CWorldView::OnUpdateFilterWireFrame(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nFilterType == FT_WIRE_FRAME);
}

D3DXVECTOR3 m_vPickRayDir;


BOOL CWorldView::ContourLineUp(int x,int y,int r,FLOAT fHeight)
{
	CPoint pt(0,r), ptTemp;
	int d = 2 * (1 - r);
	BOOL aaaa = FALSE;
	FLOAT fWldHeight;
	while(pt.y >= 0) 
	{
		ptTemp = CPoint(x+pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE1;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y++;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE1:
		ptTemp = CPoint(x+pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE2;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y--;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			//lpTile->m_byHeight = fHeight;
		//	lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE2:
		ptTemp = CPoint(x-pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE3;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			//lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y++;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE3:
		ptTemp = CPoint(x-pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE4;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			//lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y--;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight < fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE4:
		if(d + pt.y > 0) 
		{
			pt.y--;
			d -= (2 * pt.y) - 1;
		}
		if(pt.x >= (d-1)) 
		{
			pt.x++;
			d += (2 * pt.x) + 1;
		}
	}
	return aaaa;

/*
	CPoint pt(0,r), ptTemp;
	int d = 2 * (1 - r);
	CTile* lpTile;
	BOOL aaaa = FALSE;
	while(pt.y >= 0) 
	{
		ptTemp = CPoint(x+pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 63 || ptTemp.y < 0 || ptTemp.y >= 63)
			goto COMPUTE1;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
			lpTile->m_byHeight = fHeight;
			lpTile->Compute(ptTemp.x,ptTemp.y);
			m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y++;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE1:
		ptTemp = CPoint(x+pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 63 || ptTemp.y < 0 || ptTemp.y >= 63)
			goto COMPUTE2;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
			lpTile->m_byHeight = fHeight;
			lpTile->Compute(ptTemp.x,ptTemp.y);
			m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y--;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
			//lpTile->m_byHeight = fHeight;
		//	lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE2:
		ptTemp = CPoint(x-pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 63 || ptTemp.y < 0 || ptTemp.y >= 63)
			goto COMPUTE3;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
			lpTile->m_byHeight = fHeight;
			lpTile->Compute(ptTemp.x,ptTemp.y);
			m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y++;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE3:
		ptTemp = CPoint(x-pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 63 || ptTemp.y < 0 || ptTemp.y >= 63)
			goto COMPUTE4;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
			lpTile->m_byHeight = fHeight;
			lpTile->Compute(ptTemp.x,ptTemp.y);
			m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
		ptTemp.y--;
		lpTile = &m_world.m_fieldArray[ptTemp.y*63+ptTemp.x];
		if(lpTile->m_byHeight < fHeight)
		{
		//	lpTile->m_byHeight = fHeight;
			//lpTile->Compute(ptTemp.x,ptTemp.y);
			//m_world.UpdateHeight(ptTemp.x,ptTemp.y);
			aaaa = TRUE;
		}
COMPUTE4:
		if(d + pt.y > 0) 
		{
			pt.y--;
			d -= (2 * pt.y) - 1;
		}
		if(pt.x >= (d-1)) 
		{
			pt.x++;
			d += (2 * pt.x) + 1;
		}
	}
	return aaaa;
*/
	return 1;
}
BOOL CWorldView::ContourLineDown(int x,int y,int r,FLOAT fHeight)
{
	CPoint pt(0,r), ptTemp;
	int d = 2 * (1 - r);
	BOOL aaaa = FALSE;
	FLOAT fWldHeight;
	while(pt.y >= 0) 
	{
		ptTemp = CPoint(x+pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE1;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
		ptTemp.y++;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
COMPUTE1:
		ptTemp = CPoint(x+pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE2;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
		ptTemp.y--;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
COMPUTE2:
		ptTemp = CPoint(x-pt.x,y+pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE3;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
		ptTemp.y++;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
COMPUTE3:
		ptTemp = CPoint(x-pt.x,y-pt.y);
		if(ptTemp.x < 0 || ptTemp.x >= 1024 || ptTemp.y < 0 || ptTemp.y >= 1024)
			goto COMPUTE4;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
		ptTemp.y--;
		fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( ptTemp.x ), (float)( ptTemp.y ) );
		if( fWldHeight > fHeight )
		{
			m_world.SetHeight( ptTemp, fHeight );
			aaaa = TRUE;
		}
COMPUTE4:
		if(d + pt.y > 0) 
		{
			pt.y--;
			d -= (2 * pt.y) - 1;
		}
		if(pt.x >= (d-1)) 
		{
			pt.x++;
			d += (2 * pt.x) + 1;
		}
	}
	return aaaa;
	return 1;
}
BOOL m_bCopyObj;
D3DXVECTOR3 m_vScreenToVector;

D3DXVECTOR3 CWorldView::ScreenToVector( int sx, int sy )
{
    // Scale to screen
	FLOAT m_fRadius = 0.1f;
	CRect rect;
	this->CView::GetClientRect( &rect );
	int m_iWidth = rect.Width();
	int m_iHeight = rect.Height();
    FLOAT x   = -(sx - m_iWidth/2)  / (m_fRadius*m_iWidth/2);
    FLOAT y   =  (sy - m_iHeight/2) / (m_fRadius*m_iHeight/2);

//    x   = -sx / (m_fRadius*m_iWidth);
    //y   =  sy / (m_fRadius*m_iHeight);
	
	/*
    if( m_bRightHanded )
    {
        x = -x;
        y = -y;
    }
	*/
    FLOAT z   = 0.0f;
    FLOAT mag = x*x + y*y;
	
    if( mag > 1.0f )
    {
        FLOAT scale = 1.0f/sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
		//z = mag;
        z = sqrtf( 1.0f - mag );
	
    // Return vector
    return D3DXVECTOR3( x, y, z );
}
void CWorldView::UpdateCamera( CPoint ptAdd ) 
{
	CRect rect;
	CView::GetClientRect(&rect);
	FLOAT m_fRadius = 0.1f;
	FLOAT x = -ptAdd.x / (m_fRadius*rect.Width());
	FLOAT y =  ptAdd.y / (m_fRadius*rect.Height());
	m_camera.m_fPitch += y;// * 10;
	m_camera.m_fYaw   -= x;// * 10;
	/*
	if( m_camera.m_fPitch < 0 ) 
		m_camera.m_fPitch = ( 2 * D3DX_PI ) + m_camera.m_fPitch;
	if( m_camera.m_fPitch >= ( 2 * D3DX_PI) ) 
		m_camera.m_fPitch = 2 * D3DX_PI;
		*/
	if( m_camera.m_fYaw < 0 ) 
		m_camera.m_fYaw = ( 2 * D3DX_PI ) + m_camera.m_fYaw;
	if( m_camera.m_fYaw >= ( 2 * D3DX_PI) ) 
		m_camera.m_fYaw -= ( 2 * D3DX_PI );
}
void CWorldView::OnMouseMove( UINT nFlags, CPoint point ) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( &ptCursor );
	D3DXVECTOR3 vector;
	CRect rect;
	CView::GetClientRect(&rect);
	BOOL bRender = FALSE;
			
	m_camera.Transform( m_pd3dDevice, &m_world );
	if( m_bEditHeapObject && m_world.PickObject( rect, ptCursor, &m_world.m_matProj, &m_camera.m_matView, 0xffffffff, NULL, &m_vIntersect, FALSE, FALSE  ) )
		m_bCursorInWorld = TRUE;
	else
	if( m_world.ClientPointToVector( NULL, rect, ptCursor, &m_world.m_matProj, &m_camera.m_matView, &m_vIntersect, FALSE ) )
		m_bCursorInWorld = TRUE;
	else
		m_bCursorInWorld = FALSE;

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->m_pageTerrainLayer.UpdateLayer( m_vIntersect, &m_world );
	
	//if(m_nTool == VIEW_DRAW)
	//{
	//	Invalidate(FALSE);
	//}
	// 브러쉬를 들고 있으면 랜더하라.
	if( GetTool() == ID_SELECT_PUT && m_pObjBrush )
		bRender = TRUE;
	// 미들 버튼을 눌렀을 땨 (주로 화면 스크롤및 화면 회전)
	if(m_bMButtonDown == TRUE)
	{
		if( GetAsyncKeyState(VK_LMENU) & 0x8000 )
		{
			CPoint pt = point - m_ptOrg;
			UpdateCamera( pt ) ;
			bRender = TRUE;
		}
		else
		if( m_bScrollPossible == TRUE )
		{
			D3DXVECTOR3 vPickRayOrig;
			D3DXVECTOR3 vPickRayDir ;
			CRect rect;
			CView::GetClientRect(&rect);
			GetPickRay(rect,point,&m_world.m_matProj,&m_camera.m_matView,&vPickRayOrig,&vPickRayDir);
			GetRayEnd(&vPickRayOrig,&vPickRayDir,&vPickRayDir);
			//TRACE("OnLButtonUp %f %f %f\n",vPickRayDir.x,vPickRayDir.y,vPickRayDir.z);
			D3DXVECTOR3 vNew = vPickRayDir - m_vPickRayDir;
			if(vNew.x == 0 && vNew.z == 0)
				return;
			vNew.y = 0;
			m_camera.m_vPos -= vNew;
			m_camera.m_vLookAt -= vNew;
			bRender = TRUE;
		}
	}
	else
	{
		if( m_ptOrg == point )
			return;

		if( m_bLButtonDown == TRUE )
		{
			bRender = TRUE;
			if(m_nTool == ID_VIEW_SCROLL)
			{
				if( m_bScrollPossible == TRUE )
				{
					D3DXVECTOR3 vPickRayOrig;
					D3DXVECTOR3 vPickRayDir ;
					CRect rect;
					CView::GetClientRect(&rect);
					GetPickRay(rect,point,&m_world.m_matProj,&m_camera.m_matView,&vPickRayOrig,&vPickRayDir);
					GetRayEnd(&vPickRayOrig,&vPickRayDir,&vPickRayDir);
					TRACE("OnLButtonUp %f %f %f\n",vPickRayDir.x,vPickRayDir.y,vPickRayDir.z);
					D3DXVECTOR3 vNew = vPickRayDir - m_vPickRayDir;
					if(vNew.x == 0 && vNew.z == 0)
						return;
					vNew.y = 0;
					m_camera.m_vPos -= vNew;
					m_camera.m_vLookAt -= vNew;
				}
			}
			else
			if(m_nTool == ID_VIEW_ZOOM)
			{
				CPoint pt = m_ptOrg - point;
				D3DXVECTOR3 vBackPos = m_camera.m_vPos;
				D3DXVECTOR3 vAdd = m_vPickRayDir * ((FLOAT)pt.y / 10);
				m_camera.m_vPos += vAdd;
				if(m_camera.m_vPos.y < 10)
					m_camera.m_vPos = vBackPos;
				m_camera.m_vLookAt = m_camera.m_vPos + m_vPickRayDir ;//+= vecAdd;
				//m_camera.m_vLookAt += vAdd;
				if(m_camera.m_vPos.y < 10)
					m_camera.m_vPos = vBackPos;
			}
			else
			if(m_nTool == ID_VIEW_ROTATION)
			{
				CPoint pt = point - m_ptOrg;
				UpdateCamera( pt ) ;
				bRender = TRUE;
			}
			else
			{
				
				D3DXVECTOR3 vPickRayOrig;
				D3DXVECTOR3 vPickRayDir ;
				D3DXVECTOR3 vector;
				CRect rect;
				CView::GetClientRect(&rect);

				if( GetTool() == ID_TERRAIN_TEXTURE )
				{
					//if(m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
					if( m_bCursorInWorld )
					{
						vector = m_vIntersect;
						switch(CPageTerrainTexture::m_nLandTexture) {
						case LANDTEX_NORMAL:
							SetTexture( vector.x, vector.z, (WORD)( m_dwTerrainBrush ),
								(FLOAT)( CPageTerrainTexture::m_nRadiusTexture ), (FLOAT)( CPageTerrainTexture::m_nHardnessTexture ) ); 
							break;
/*
						case LANDTEX_DETAIL:
							m_world.SetDetailPatch( vector.x, vector.z,m_dwTerrainBrush); 
							break;
*/
						case LANDTEX_BLANK:
							m_world.SetBlankPatch( (int)( vector.x ), (int)( vector.z ) ); 
							break;
						case LANDTEX_LANDMARK:
//							m_world.SetTexture( vector.x, vector.z, m_dwTerrainBrush,
//								CPageTerrainTexture::m_nRadiusTexture, CPageTerrainTexture::m_nHardnessTexture); 
							m_world.SetLandMarkPatch( (int)( vector.x ), (int)( vector.z ), (WORD)( m_dwTerrainBrush ) );
							break;
						}
					}
				}
				else
				if( GetTool() == ID_TERRAIN_WATER )
				{
					//if(m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
					if( m_bCursorInWorld )
					{
						vector = m_vIntersect;
					#ifdef __N_WATERFRAME_050202
						m_world.SetWaterHeight( (int)( vector.x ), (int)( vector.z ), CPageTerrainWater::m_nWaterHeight, CPageTerrainWater::m_nWaterTexture | CPageTerrainWater::m_nWaterFrame ); 
					#else
						m_world.SetWaterHeight( vector.x, vector.z, CPageTerrainWater::m_nWaterHeight, CPageTerrainWater::m_nWaterTexture ); 
					#endif//__N_WATERFRAME_050202
					}
				}
				else
				if( GetTool() == ID_TERRAIN_HEIGHT && m_bPickUpTile)
				{
					if( CPageTerrainHeight::m_nHeightStyle == 2 )
					{
						if( m_ptOrg != point )
							EditHeightSoft( point );
					}
					else
					if( CPageTerrainHeight::m_nHeightStyle == 1 )
					{
						EditHeightFlat( point );
					}
					else
					if( CPageTerrainHeight::m_nHeightStyle == 0 )
					{
						EditHeightMountain( point );
					}
					else
					if( CPageTerrainHeight::m_nHeightStyle == 3 ) // random
						EditHeightRandom( point );
					else
					if( CPageTerrainHeight::m_nHeightStyle == 4 ) // attribute
						EditHeightAttribute( point, CPageTerrainHeight::m_nHeightAttribute );
								
				}
				else
				if( GetTool() == ID_TERRAIN_COLOR )
				{
					//if(m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
					if( m_bCursorInWorld )
					{
						vector = m_vIntersect;
						// 민감하게 칠해지는 것을 방지하기 위해서 좌표를 둔하게 하기.
						D3DXVECTOR3 vTempOrg = m_vOrigin;
						D3DXVECTOR3 vTempCur = vector;
						vTempOrg.x = (float)( int( vTempOrg.x / MPU ) );
						vTempOrg.z = (float)( int( vTempOrg.z / MPU ) );
						vTempCur.x = (float)( int( vTempCur.x / MPU ) );
						vTempCur.z = (float)( int( vTempCur.z / MPU ) );
						vTempCur.y = vTempOrg.y;
						if( vTempOrg != vTempCur )
						{
							m_world.SetColor( vector.x, vector.z, CPageTerrainColor::m_dwVertexColor, (FLOAT)( CPageTerrainColor::m_nRadiusVColor ), (FLOAT)( CPageTerrainColor::m_nHardnessVColor ) );
							m_vOrigin = vector;
						}
					}
				}
				else
				if( GetTool() == ID_SELECT || GetTool() == ID_SELECT_MOVE || GetTool() == ID_SELECT_ROTATION || GetTool() == ID_SELECT_SCALE )
				{
					if( m_bPickUpTile == TRUE || m_bLock )
					{
						//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
						if( m_bCursorInWorld )
						{
							vector = m_vIntersect;
							// 왼쪽 버튼 누르고 오브젝트를 처음 이동하는 것인가?
							if( m_bLBDownMouseMove == FALSE )
							{
								// 컨트롤 키를 눌렀나? 그렇다면 복사다.
								if( (GetAsyncKeyState(VK_LSHIFT) & 0x8000) )
								{
									m_world.CopySelectObj();
									m_bCopyObj = TRUE;
								}
							}
							D3DXVECTOR3 vec = vector - m_vOrigin;
							if( ( GetAsyncKeyState( VK_LMENU ) & 0x8000 ) || GetTool() == ID_SELECT_ROTATION  )
							{
#ifdef __CSC_UNDO_REDO

								if(CCommandMgrBeast::GetInstance()->GetCurCommandType() == -1)
								{
									CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_ROTATE);

									for( int i=0; i<(int)( m_world.m_arySelect.size() ); i++)
									{
										CObj* pObj = (CObj*)m_world.m_arySelect.at(i);
										
										if(pObj)
										{
											D3DXVECTOR3 vecAngle;
											vecAngle.x = pObj->GetAngleX();
											vecAngle.y = pObj->GetAngle();
											vecAngle.z = pObj->GetAngleZ();

											CCommandMgrBeast::GetInstance()->m_mapOldObjRotate.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, vecAngle));
										}
									}
								}
#endif //__CSC_UNDO_REDO
								//for( int i = 0; i < m_world.m_arySelect.size(); i++)
								{
									CPoint pt = point - m_ptOrg;
									if( pt.y )
									{
										/*
										CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
										pMainFrame->m_pObjView->setmodi m_bModifiedMdlObj = TRUE;
										*/
										m_world.GetLandscape( vector )->SetModified( TRUE ); // 오브젝트가 이동되면 현재 랜드에서 사라질 수 있으므로 일단 수정 기록										
										
										// 마우스 이동에 따른 가변폭 구하기.
										FLOAT b = (FLOAT) ( pt.y < 0 ? -pt.y : -pt.y ) / 100.f;
										// 가변폭을 라디안에 더하기.
										if( m_nRestrict == ID_RESTRICT_X )
											RotationObjectsX( b );
										else
										if( m_nRestrict == ID_RESTRICT_Z )
											RotationObjectsZ( b );
										else
											RotationObjects( b );
										
										GetSelectCenter() ;
									}
								}
							}
							else
							if(  GetTool() == ID_SELECT_MOVE )
							{
#ifdef __CSC_UNDO_REDO
								if(CCommandMgrBeast::GetInstance()->GetCurCommandType() == -1)
								{
									CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_MOVE);
									for( int i=0; i<(int)( m_world.m_arySelect.size() ); i++)
									{
										CObj* pObj = (CObj*)m_world.m_arySelect.at(i);
										
										if(pObj)
										{
											D3DXVECTOR3 vecObjPos = pObj->GetPos();
											CCommandMgrBeast::GetInstance()->m_mapOldObjMove.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, vecObjPos));
										}
									}
								}
#endif //__CSC_UNDO_REDO
								D3DXVECTOR3 vecTemp = vec;
								CPoint pt = point - m_ptOrg;
								for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
								{
									vec = vecTemp;
									CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
									D3DXVECTOR3 vPos = pObj->m_vPosGrid;//GetPos();
									// 오브젝트 이동 
									if( m_nRestrict == ID_RESTRICT_Y )
									{
										vec.y = (float)-pt.y / 10.0f;
										vec.x = 0;
										vec.z = 0;
										pObj->m_vPosGrid = vPos + vec;
										vec = pObj->m_vPosGrid;

										if( m_bOnGrid )
										{
											vector.y = vector.y / m_fGridY;//) / 4 ) * 4;
											vector.y = int( vector.y ) * m_fGridY;
										}
										pObj->SetPos( vec );
										
										//pObj->SetPos( vPos + vec );
									}
									else
									if( m_nRestrict == ID_RESTRICT_XZ )
									{
										vec.y = 0;
										//vec.x += pt.x / 10.0f
										//vec.z += pt.y / 10.0f
										pObj->m_vPosGrid = vPos + vec;
										vec = pObj->m_vPosGrid;
										if( m_bOnGrid )
										{
											vec.x = vec.x / m_fGridX;//) / 4 ) * 4;
											vec.z = vec.z / m_fGridZ;/// 4 ) * 4;
											vec.x = int( vec.x ) * m_fGridX;
											vec.z = int( vec.z ) * m_fGridZ;/// 4 ) * 4;

											//vec.x = int( vec.x ) ;/// 4 ) * 4;
											//vec.z = int( vec.z );/// 4 ) * 4;
										}
										pObj->SetPos( vec );
										/*
										if( pObj->IsType( OT_MOVER ) && ((CMover*)pObj)->IsRespawn() )
										{
											CMover* pMover = (CMover*)pObj;
											pMover->m_respawnInfo.rect += CPoint( vec.x, vec.z);
										}
										else
										if( pObj->IsType( OT_ITEM ) && ((CItem*)pObj)->IsRespawn() )
										{

										}
										*/
										
									}
									else
									if( m_nRestrict == ID_RESTRICT_Z )
									{
										//gmpbigsun( 100224 ) : 오브젝트 이동시 진행방향, 진행방향의 수직인방향으로만 움직이는 기능추가 
										//1. 현재 지점에서 움직인 좌표로 vector를 만들고 
										D3DXVECTOR3 kPickDir = m_vIntersect - m_vOrigin;

										//2. 그 길이가 곧 움직여야 할 거리다.
										float fLength = D3DXVec3Length( &kPickDir );

										//3. 그 길이만큼의 벡터를 생성한다.
										float fAngle = pObj->GetAngle( );
										
										float fMoreY = (float)( abs( pt.y ) - abs( pt.x ) );
										if( fMoreY < 0 )
											fLength = 0.0f;
									
										D3DXVECTOR3 kDir;
										AngleToVector( &kDir, fAngle, 0.0f, fLength );	

										// 카메라와 오브젝트의 방향에 따라 움직이는 방향을 결정 
										D3DXVECTOR3 kCamDir = pObj->GetPos() - m_camera.m_vPos;
										D3DXVECTOR3 kObjDir = kDir;

										kCamDir.y = 0.0f;
										kObjDir.y = 0.0f;
										float fDot = D3DXVec3Dot( &kCamDir, &kObjDir );
										if( fDot > 0 )
										{
											//예각 
											if( pt.y > 0 )
												kDir = -kDir;
										}
										else
										{
											if( pt.y < 0 )
												kDir = -kDir;
										}
																			
										vec = kDir;
										pObj->m_vPosGrid = vPos + vec;
										vec = pObj->m_vPosGrid;
										if( m_bOnGrid )
										{
											vec.x = vec.x / m_fGridX;//) / 4 ) * 4;
											vec.z = vec.z / m_fGridZ;/// 4 ) * 4;
											vec.x = int( vec.x ) * m_fGridX;
											vec.z = int( vec.z ) * m_fGridZ;/// 4 ) * 4;

											//vec.x = int( vec.x ) ;/// 4 ) * 4;
											//vec.z = int( vec.z );/// 4 ) * 4;
										}
										pObj->SetPos( vec );

										if( m_bEditOnLand )
											pObj->SetOnLand();		
											
									}
									else
									if( m_nRestrict == ID_RESTRICT_X )
									{
										//1. 현재 지점에서 마우스 포인터 좌표로 vector를 만들고 
										D3DXVECTOR3 kPickDir = m_vIntersect - m_vOrigin;

										//2. 그 길이가 곧 움직여야 할 거리다.
										float fLength = D3DXVec3Length( &kPickDir );

										//3. 그 길이만큼의 벡터를 생성한다.
										float fAngle = pObj->GetAngle( );
																				
										float fMoreX = (float)( abs( pt.x ) - abs( pt.y ) );
										if( fMoreX < 0 )
											fLength = 0.0f;
									
										//4. 캠방향과 오브젝트 방향 관계를 확인한후 
										D3DXVECTOR3 kDir;
										AngleToVector( &kDir, fAngle, 0.0f, 1.0f );	

										D3DXVECTOR3 kCamDir = pObj->GetPos() - m_camera.m_vPos;
										D3DXVECTOR3 kObjDir = kDir;

										kCamDir.y = 0.0f;
										kObjDir.y = 0.0f;

										float fDot = D3DXVec3Dot( &kCamDir, &kObjDir );

										//5. 4번이 끝났으므로 실제로 움직일 방향 설정 
										fAngle += 90;	//! 진행방향과 직각인축 으로 
										AngleToVector( &kDir, fAngle, 0.0f, fLength );

										if( fDot > 0 )
										{
											//예각 
											if( pt.x > 0 )
												kDir = -kDir;
										}
										else
										{
											if( pt.x < 0 )
												kDir = -kDir;
										}

										vec = kDir;
										pObj->m_vPosGrid = vPos + vec;
										vec = pObj->m_vPosGrid;
										if( m_bOnGrid )
										{
											vec.x = vec.x / m_fGridX;//) / 4 ) * 4;
											vec.z = vec.z / m_fGridZ;/// 4 ) * 4;
											vec.x = int( vec.x ) * m_fGridX;
											vec.z = int( vec.z ) * m_fGridZ;/// 4 ) * 4;

											//vec.x = int( vec.x ) ;/// 4 ) * 4;
											//vec.z = int( vec.z );/// 4 ) * 4;
										}
										pObj->SetPos( vec );

										if( m_bEditOnLand )
											pObj->SetOnLand();		
											
									}

									GetSelectCenter();
								}
							}
							else
							if(  GetTool() == ID_SELECT_SCALE )
							{
#ifdef __CSC_UNDO_REDO
								if(CCommandMgrBeast::GetInstance()->GetCurCommandType() == -1)
								{
									CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_SCALE);
									for( int i=0; i<(int)( m_world.m_arySelect.size() ); i++)
									{
										CObj* pObj = (CObj*)m_world.m_arySelect.at(i);
										
										if(pObj)
										{
											D3DXVECTOR3 vecObjScale = pObj->GetScale();
											CCommandMgrBeast::GetInstance()->m_mapOldObjScale.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, vecObjScale));
										}
									}
								}
#endif //__CSC_UNDO_REDO
								for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
								{
									CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
									CPoint pt = m_ptOrg - point;
#ifdef __CSC_SCALE_CTRL
									FLOAT fScale = (FLOAT)pt.y / 100;
									D3DXVECTOR3 vImp(0.0f, 0.0f, 0.0f);
									switch(m_nRestrict)
									{
										case ID_RESTRICT_X:
											vImp.x = fScale;
											break;
										case ID_RESTRICT_Y:
											vImp.y = fScale;
											break;
										case ID_RESTRICT_Z:
											vImp.z = fScale;
											break;
										case ID_RESTRICT_XZ:
											vImp.x = fScale;
											vImp.y = fScale;
											vImp.z = fScale;
											break;
									}
									D3DXVECTOR3 vScale = pObj->GetScale();
									pObj->SetScale( vScale + vImp );
#else //__CSC_SCALE_CTRL
									FLOAT fScale = (FLOAT)pt.y / 100;
									D3DXVECTOR3 vScale = pObj->GetScale();
									pObj->SetScale( vScale + D3DXVECTOR3( fScale, fScale, fScale ) );
#endif //__CSC_SCALE_CTRL
									pObj->UpdateLocalMatrix();
								}
							}
							m_vOrigin = vector;
						}
					}
					else
					{
						if( m_3DRectTracker.OnMouseMove( nFlags, point ) == FALSE )
						{
							m_bSelectDrag = TRUE;
							m_rectDraw.SetRect( m_ptButtonDown.x, m_ptButtonDown.y, point.x, point.y );
						}
						/*
						if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
						{
							CObj* pObjFocus = m_world.GetObjFocus();
							CRect* pRectRespawn = &((CMover*)pObjFocus)->m_respawnInfo.rect;
							D3DXVECTOR3 vPos = pObjFocus->GetPos();
							vector.x = int( vector.x / MPU * MPU );
							vector.z = int( vector.z / MPU * MPU );
							vPos.x = int( vPos.x / MPU * MPU );
							vPos.z = int( vPos.z / MPU * MPU );

							vector.x -= vPos.x;
							vector.z -= vPos.z;
							vector.x += MPU;
							vector.z += MPU;
							switch( m_nPickRectPoint )
							{
							case 0:
								pRectRespawn->left = vector.x;
								pRectRespawn->top = vector.z;
								break;
							case 1:
								pRectRespawn->right = vector.x;
								pRectRespawn->top = vector.z;
								break;
							case 2:
								pRectRespawn->left = vector.x;
								pRectRespawn->bottom = vector.z;
								break;
							case 3:
								pRectRespawn->right = vector.x;
								pRectRespawn->bottom = vector.z;
								break;
							}
							if( pRectRespawn->left > 0 )  pRectRespawn->left = 0;
							if( pRectRespawn->top > 0 )  pRectRespawn->top = 0;
							if( pRectRespawn->right < 0 )  pRectRespawn->right = 0;
							if( pRectRespawn->bottom < 0 )  pRectRespawn->bottom = 0;
						}
						*/
					}
				}
				//CMainFrame* pMainFrame = (CWorldView*) AfxGetMainWnd();
				//pMainFrame->m_workSpace.m_pDlgWorkWorld->Invalidate(FALSE);
			}
		}
	}

	//D3DXMatrixLookAtLH( &g_matView, &m_vecPos, &m_vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	//m_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );
//	UpdateCullInfo(&g_matView,&g_matProj);
	m_ptOrg = point;
	//D3DXVECTOR3 vPickRayOrig;
	//GetPickRay(point,&g_matProj,&g_matView,&vPickRayOrig,&m_vPickRayDir);
//	GetRayEnd(&vPickRayOrig,&m_vPickRayDir,&m_vPickRayDir);
	//Invalidate(FALSE);
	if( m_bLButtonDown )
		m_bLBDownMouseMove = TRUE;

	if( bRender ) 
		RenderScene();
	else
		Invalidate(FALSE);
	
	CView::OnMouseMove(nFlags, point);
}

// lenajang:현재 맵툴에서의 오브젝트배치된 것들의 y축을 기준으로 한 회전을 위한 함수이다.
// 한축만을 위한 회전이기 때문에 변위의 크기정도의 값을 가지고 회전을 하게 만들어져 있다. 
// 궁금한 점은 'directX에 의존하지 않기위해서 이렇게 만들어 진것인가'이다. 
// 아니면 로컬좌표로의 전환이라던가 추가적인 연산을 피하기 위해서 이런식으로 계산이 된것인가.
void CWorldView::RotationObjects( FLOAT fAddRadian )
{
	for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
	{
		CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
		//CPoint pt = point - m_ptOrg;
		D3DXVECTOR3 vPos = pObj->GetPos();
		// 오브젝트 회전 
		// 객체와 중심 사이의 길이. 반지름.
		FLOAT fRadian = sqrt( 
			fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
			fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
			);
		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVECTOR3 Line2 = vPos;
		Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < m_world.m_vCenter.x )
			a = ( D3DX_PI * 2.0f ) - a;
		// 마우스가 위아래로 움직였으면 돌기 시작함.(pt.y를 검사하면 알 수 있음)
		//if( pt.y )
		{
			// 마우스 이동에 따른 가변폭 구하기.
			FLOAT b = fAddRadian;//(FLOAT) ( pt.y < 0 ? -pt.y : -pt.y ) / 50.f;
			// 가변폭을 라디안에 더하기.
			a += b;
			// 오브젝트가 이동이 없고 원점에서 돌기만 하는 경우라면 vPos를 변경할 필요가 없다.
			// lenajang:오브젝트는 아래의 if블럭아래에서 회전을 하지만 위치지정은 if블럭에서
			// 해준다.
			if( vlength )
			{
				TRACE( "a %f\n", a );
				vPos.x = m_world.m_vCenter.x + sin( a ) * fRadian;
				vPos.z = m_world.m_vCenter.z + cos( a ) * fRadian;
				pObj->SetPos( vPos );
			}
			// lenajang:이 오브젝트는 실제로 그냥 회전만 하게 된다.
			pObj->AddAngle( -(360.0f * b / ( D3DX_PI * 2.0f ) ));
			//pObj->SetOnLand();								
		}
	} 
}

void CWorldView::RotationObjectsX( FLOAT fAddRadian )
{
#ifdef __Y_ROTXZ	
	for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
	{
		CObj* pObj = (CObj*)m_world.m_arySelect.at( i );

		//CPoint pt = point - m_ptOrg;
		D3DXVECTOR3 vPos = pObj->GetPos();
		// 오브젝트 회전 
		// 객체와 중심 사이의 길이. 반지름.
		FLOAT fRadian = sqrt( 
			fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
			fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
			);
		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVECTOR3 Line2 = vPos;
		Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < m_world.m_vCenter.x )
			a = ( D3DX_PI * 2.0f ) - a;
		// 마우스가 위아래로 움직였으면 돌기 시작함.(pt.y를 검사하면 알 수 있음)
		//if( pt.y )
		{
			// 마우스 이동에 따른 가변폭 구하기.
			FLOAT b = fAddRadian;//(FLOAT) ( pt.y < 0 ? -pt.y : -pt.y ) / 50.f;
			// 가변폭을 라디안에 더하기.
			a += b;
			// 오브젝트가 이동이 없고 원점에서 돌기만 하는 경우라면 vPos를 변경할 필요가 없다.
			// lenajang:오브젝트는 아래의 if블럭아래에서 회전을 하지만 위치지정은 if블럭에서
			// 해준다.
			if( vlength )
			{
				TRACE( "a %f\n", a );
				vPos.x = m_world.m_vCenter.x + sin( a ) * fRadian;
				vPos.z = m_world.m_vCenter.z + cos( a ) * fRadian;
				pObj->SetPos( vPos );
			}
			// lenajang:이 오브젝트는 실제로 그냥 회전만 하게 된다.
			FLOAT f = pObj->GetAngleX();
			f += fAddRadian * 25.0f;
			pObj->SetAngleX( f );
			//pObj->SetOnLand();								
		}
	} 
#endif //__Y_ROTXZ
}

void CWorldView::RotationObjectsZ( FLOAT fAddRadian )
{
#ifdef __Y_ROTXZ	
	for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
	{
		CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
		//CPoint pt = point - m_ptOrg;
		
		D3DXVECTOR3 vPos = pObj->GetPos();
		// 오브젝트 회전 
		// 객체와 중심 사이의 길이. 반지름.
		FLOAT fRadian = sqrt( 
			fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
			fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
			);
		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVECTOR3 Line2 = vPos;
		Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < m_world.m_vCenter.x )
			a = ( D3DX_PI * 2.0f ) - a;
		// 마우스가 위아래로 움직였으면 돌기 시작함.(pt.y를 검사하면 알 수 있음)
		//if( pt.y )
		{
			// 마우스 이동에 따른 가변폭 구하기.
			FLOAT b = fAddRadian;//(FLOAT) ( pt.y < 0 ? -pt.y : -pt.y ) / 50.f;
			// 가변폭을 라디안에 더하기.
			a += b;
			// 오브젝트가 이동이 없고 원점에서 돌기만 하는 경우라면 vPos를 변경할 필요가 없다.
			// lenajang:오브젝트는 아래의 if블럭아래에서 회전을 하지만 위치지정은 if블럭에서
			// 해준다.
			if( vlength )
			{
				TRACE( "a %f\n", a );
				vPos.x = m_world.m_vCenter.x + sin( a ) * fRadian;
				vPos.z = m_world.m_vCenter.z + cos( a ) * fRadian;
				pObj->SetPos( vPos );
			}
			// lenajang:이 오브젝트는 실제로 그냥 회전만 하게 된다.
			FLOAT f = pObj->GetAngleZ();
			f += fAddRadian * 25.0f;
			pObj->SetAngleZ( f );
			//pObj->SetOnLand();								
		}
	} 
#endif //__Y_ROTXZ
}

//	lenajang;아직 사용하면 안됨... 완성된버젼이 아님
void CWorldView::RotationObjects(const D3DXVECTOR3& vtRadian)
{
	for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++)
	{
		CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
		//CPoint pt = point - m_ptOrg;
		D3DXVECTOR3 vPos = pObj->GetPos();
		// 오브젝트 회전 
		// 객체와 중심 사이의 길이. 반지름.
		//	각도를 구하는 순서 ; Z(Roll), X(Pitch), Y(Yaw) 
		//					=> 이 순서는 실제 회전하는 순서의 반대로 구해준다.
		//					=> 이유는 회전을 하는것이 
		FLOAT fRadianRoll = sqrt( 
			fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
			fabs ( vPos.y - m_world.m_vCenter.y ) * fabs ( vPos.y - m_world.m_vCenter.y ) 
			);
		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		D3DXVECTOR3 Line2 = vPos;
		Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.y * Line2.y ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < m_world.m_vCenter.x )
			a = ( D3DX_PI * 2.0f ) - a;


		FLOAT fRadianPitch = sqrt( 
			fabs ( vPos.y - m_world.m_vCenter.y ) * fabs ( vPos.y - m_world.m_vCenter.y ) +
			fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
			);

		FLOAT fRadianYaw = sqrt( 
			fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
			fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
			);
		

/*		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVECTOR3 Line2 = vPos;
		Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < m_world.m_vCenter.x )
			a = ( D3DX_PI * 2.0f ) - a;*/
	}
}

void CWorldView::RenderHeightPoint( D3DXVECTOR3 vPos, int nCurRadius, int nRadius, DWORD dwColor )
{
	if( nRadius >= 20 * MPU )
	{
		if( nCurRadius >= nRadius - MPU )
			RenderVertexPoint( vPos, dwColor );
	}
	else
		RenderVertexPoint( vPos, dwColor );
	
	//vPos.y = m_world.GetLandHeight( vPos.x, vPos.z );
	//RenderVertexPoint( vPos, dwColor );
}
void CWorldView::RenderVertexPoint( D3DXVECTOR3 vPos, DWORD dwColor, int iScale )
{
	D3DXVECTOR3 vOut;
	D3DVIEWPORT9 vp;
	m_pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matWorld;
	CRect rect;
	D3DXMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
	
	D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
		&m_world.m_pCamera->m_matView, &matWorld );
	
	rect.SetRect( (int)( vOut.x - iScale ), (int)( vOut.y - iScale ), (int)( vOut.x + iScale ), (int)( vOut.y + iScale ) );
	g_2DRender.RenderFillRect( rect, dwColor );
}
void CWorldView::SetTexture( FLOAT x,FLOAT z,WORD nTex, FLOAT fRadius, FLOAT fHardness, BOOL bRender )
{
	if( m_world.VecInWorld( x, z ) == FALSE )
		return;

	//if( bRender )
	//	return;
	int xx = (int)( x );
	int yy = (int)( z );

	x /= MPU;
	z /= MPU;
	x += .5f;
	z += .5f;
	x = x * (float)LIGHTMAP_SIZE / MAP_SIZE;		//LIGHTMAP_SIZE / MAP_SIZE = 0.875
	z = z * (float)LIGHTMAP_SIZE / MAP_SIZE;

	FLOAT insideradius  = fHardness;
	FLOAT outsideradius = fRadius - fHardness;

	fRadius--;
	fHardness--;
	for(int i = (int)( -fRadius ); i <= fRadius ; i++ )  
	{
		for(int j = (int)( -fRadius ); j <= fRadius; j++ ) 
		{
			FLOAT distance = sqrt( float(( i * i ) + ( j * j )) );
			if( distance <= fRadius ) 
			{
				if( bRender )
				{
					if( (int)distance * MPU >= (int)fRadius * MPU  - MPU )
					{
						int nXRadius = j * MPU;
						int nYRadius = i * MPU;
						FLOAT fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( xx + nXRadius ), (float)( yy + nYRadius ) );// / 1000.0f;
						RenderVertexPoint( D3DXVECTOR3( (float)( xx + nXRadius ), fWldHeight, (float)( yy + nYRadius ) ), 0xa0ffffff );
					}
				}
				else
				{
#ifdef __CSC_TERRAIN_OPACITY
					CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
					int nOpacity;
					if(pMainFrame)
						nOpacity = pMainFrame->m_nTerrainTexOpacity;
					else
						nOpacity = 255;

					if( distance <= insideradius ) 
					{
						m_world.SetTex( (int)( x + j ), (int)( z + i ), nTex, nOpacity, FALSE );
					}
					else 
					{
						FLOAT tempdistance = distance - insideradius;
						int alpha = 255 - int( 255.0f * tempdistance / outsideradius ) - ( 255 - nOpacity);
						if(alpha < 0)
							alpha = 0;

						m_world.SetTex( (int)( x + j ), (int)( z + i ), nTex, alpha, FALSE);
					}
#else //__CSC_TERRAIN_OPACITY
					if( distance <= insideradius ) 
					{
						m_world.SetTex( x + j, z + i, nTex, 255, FALSE );
					}
					else 
					{
						FLOAT tempdistance = distance - insideradius;
						int alpha = 255 - int( 255.0f * tempdistance / outsideradius );
						m_world.SetTex( x + j, z + i, nTex, alpha, FALSE);
					}
#endif //__CSC_TERRAIN_OPACITY
				}
			}
		}
	}

	if( bRender == FALSE )
		m_world.CalculateBound();
}

void CWorldView::EditHeightSoft( CPoint point, BOOL bRender )
{
	/*
	D3DXVECTOR3 vOut;
	D3DVIEWPORT9 vp;
	D3DXMATRIX matWorld;
	DRAWVERTEX* pVertex = NULL;
	DRAWVERTEX* pVertices = NULL; 
	
	if( bRender )
	{
		m_pd3dDevice->GetViewport( &vp );
		pVertex = new DRAWVERTEX[ 64000 * 4 ];
		pVertices = pVertex; 
	}
	*/
	int x = m_ptField.x / MPU * MPU;
	int y = m_ptField.y / MPU * MPU;

	CPoint pt = point - m_ptOrg;
	FLOAT fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( x ), (float)( y ) );
	FLOAT fWldStatic = fWldHeight - (FLOAT)pt.y;

	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	int nRadius = CPageTerrainHeight::m_nRadiusHeight * MPU;

	if( m_bCursorInWorld )
	{
		vector = m_vIntersect;
		x = int( vector.x + ( MPU / 2 ) ) / MPU * MPU;
		y = int( vector.z + ( MPU / 2) ) / MPU * MPU;


		for( int yc = -nRadius; yc < nRadius; yc += MPU ) 
		{
			for( int xc = -nRadius; xc < nRadius; xc += MPU )
			{
				FLOAT r = fabs( sqrt( float(xc * xc + yc * yc) ) );

				if( r < nRadius && !( xc % MPU) && !(yc % MPU ))
				{
					int xx = x + xc;
					int yy = y + yc;
					//if( xx < 0 || xx >= WORLD_SIZE || yy < 0 || yy >= WORLD_SIZE )
					//	continue;
					fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy ) );// / 1000.0f;
#ifdef __CSC_UNDO_REDO
					if(!bRender)
					{
						TERRAINCOORD terraincoord;
						terraincoord.m_Point.x = xx;
						terraincoord.m_Point.y = yy;
						map<TERRAINCOORD, FLOAT>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.find(terraincoord);
						
						if(iter == CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.end())
						{
							CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_HEIGHT_CHG);
							CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.insert(map<TERRAINCOORD, FLOAT>::value_type(terraincoord, fWldHeight));
						}
						else
						{
							FLOAT fOld = iter->second;
							if(fWldHeight != fOld)
							{
								CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_HEIGHT_CHG);
								CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.insert(map<TERRAINCOORD, FLOAT>::value_type(terraincoord, fWldHeight));
							}
						}
					}
#endif //__CSC_UNDO_REDO
					FLOAT afWldHeight[8];
					afWldHeight[0] = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy-MPU ) );
					afWldHeight[1] = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy+MPU ) );
					afWldHeight[2] = (FLOAT) m_world.GetLandHeight( (float)( xx-MPU ), (float)( yy ) );
					afWldHeight[3] = (FLOAT) m_world.GetLandHeight( (float)( xx+MPU ), (float)( yy ) );
					afWldHeight[4] = (FLOAT) m_world.GetLandHeight( (float)( xx-MPU ), (float)( yy-MPU ) );
					afWldHeight[5] = (FLOAT) m_world.GetLandHeight( (float)( xx-MPU ), (float)( yy+MPU ) );
					afWldHeight[6] = (FLOAT) m_world.GetLandHeight( (float)( xx+MPU ), (float)( yy-MPU ) );
					afWldHeight[7] = (FLOAT) m_world.GetLandHeight( (float)( xx+MPU ), (float)( yy+MPU ) );
					FLOAT fWldHeightTotal = fWldHeight;

					for( int i = 0; i < 8; i++)
						fWldHeightTotal += afWldHeight[ i ];

					fWldHeightTotal /= 9;
					if( fWldHeight < fWldHeightTotal )
						fWldHeight += 0.001f;
					else
					if( fWldHeight > fWldHeightTotal )
						fWldHeight -= 0.001f;
					if( bRender )
					{
						/*
						D3DXMatrixTranslation( &matWorld, (FLOAT)xx, fWldHeightTotal, (FLOAT)yy );
							D3DXVec3Project( &vOut, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &vp, &m_world.m_matProj,
							&m_world.m_pCamera->m_matView, &matWorld );
						
						SetDrawVertex( pVertices, vOut.x + 0, vOut.y - 0, 0xffffffff );
						pVertices++;
						SetDrawVertex( pVertices, vOut.x + 0, vOut.y + 1, 0xffffffff );
						pVertices++;
						SetDrawVertex( pVertices, vOut.x + 1, vOut.y - 0, 0xffffffff );
						pVertices++;
						SetDrawVertex( pVertices, vOut.x + 1, vOut.y + 1, 0xffffffff );
						pVertices++;
						*/
						RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeightTotal, (float)( yy ) ), (int)( r ), nRadius, 0xffffffff );
					}
					else
					{
						m_world.SetHeight( CPoint( xx, yy ), fWldHeightTotal );//fWldHeightTotal );
					}	
				}
			}
		}
	}

	m_world.CalculateBound();
	/*
	if( bRender )
	{
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTexture( 0, NULL );
		m_pd3dDevice->SetFVF( D3DFVF_DRAWVERTEX );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)( (UINT)pVertices - (UINT)pVertex ) / 4, pVertex, sizeof( DRAWVERTEX ) );
		delete pVertex;
	}
	*/
}
//FLOAT fx = xCenter + sin( DEGREETORADIAN( degree360 ) ) * radius;
//FLOAT fy = yCenter + cos( DEGREETORADIAN( degree360 ) ) * radius;

void CWorldView::EditHeightFlat( CPoint point, BOOL bRender )
{
	int x = m_ptField.x / MPU * MPU;
	int y = m_ptField.y / MPU * MPU;

	CPoint pt = point - m_ptOrg;
	FLOAT fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( x ), (float)( y ) );	
	CRect rect;
	CView::GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	//int nRadius = CDlgWorkLandscape::m_nRadiusHeight + 4;
	//int nRadiusOut = ( CPageTerrainHeight::m_nRadiusHeight * MPU ) + ( CPageTerrainHeight::m_nRadiusHardness * MPU );
	//int nRadiusIn = CPageTerrainHeight::m_nRadiusHeight * MPU;

	int nRadiusOut = CPageTerrainHeight::m_nRadiusHeight * MPU;
	int nRadiusIn = ( CPageTerrainHeight::m_nHardnessHeight * MPU );
	int nInverval = nRadiusOut - nRadiusIn;

	//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	if( m_bCursorInWorld )
	{
		vector = m_vIntersect;
		
		// 중심 좌표 
		x = int( vector.x  + (MPU / 2) ) / MPU * MPU;
		y = int( vector.z  + (MPU / 2) ) / MPU * MPU;

		for( int yc = -nRadiusOut; yc < nRadiusOut; yc += MPU ) 
		{
			for( int xc = -nRadiusOut; xc < nRadiusOut; xc += MPU )
			{
				FLOAT r = fabs( sqrt( float(xc * xc + yc * yc) ) );

				if( r < nRadiusOut && !( xc % MPU) && !(yc % MPU ))
				{
					// 현재 좌표 
					int xx = x + xc;
					int yy = y + yc;

					// 현재 높이 
					fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy ) );// / 1000.0f;
#ifdef __CSC_UNDO_REDO
					if(!bRender)
					{
						TERRAINCOORD terraincoord;
						terraincoord.m_Point.x = xx;
						terraincoord.m_Point.y = yy;
						map<TERRAINCOORD, FLOAT>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.find(terraincoord);
						FLOAT fOld = iter->second;
						if(iter == CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.end() || fWldHeight != fOld)
						{
							CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_HEIGHT_CHG);
							CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.insert(map<TERRAINCOORD, FLOAT>::value_type(terraincoord, fWldHeight));
						}
					}
#endif //__CSC_UNDO_REDO
					if( r <= nRadiusIn )
					{
						if( bRender )
							RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadiusOut, 0xffffffff );
						else					
							m_world.SetHeight( xx, yy, m_fWldHeight );
					}
					else
					{
						FLOAT fx, fy;
						if( xc == 0 )
						{
							fy = ( nRadiusOut + MPU ) * yc / r; // r : nRadiusOut = yc : fy
							fx = fy * xc / yc; // yc : fy = xc : fx
						}
						else
						{
							fx = ( nRadiusOut + MPU ) * xc / r; // r : nRadiusOut = xc : fx
							fy = fx * yc / xc; // xc : fx = yc : fy
						}
						// 가장 낮은, 혹은 가장 높은 높이 구함 
						FLOAT fMaxHeight = m_world.GetLandHeight( x + fx, y + fy );

						// 지금 높이보다 클 경우 
						if( fMaxHeight < m_fWldHeight )
						{
							if( bRender )
								RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadiusOut, 0xffffff00 );
							else
							{								
								FLOAT fHeightItv = ( m_fWldHeight - fMaxHeight ) / nInverval;//( 4 * MPU);
								// 끝과 지금의 거리차와 높이차를 구하면 현재의 높이!
								fWldHeight = fMaxHeight + ( ( nRadiusOut - r ) * fHeightItv );
								if( fWldHeight > m_world.GetLandHeight( (float)( xx ), (float)( yy ) ) )
									m_world.SetHeight( xx, yy, fWldHeight );
							}
						}
						else
						// 지금 높이보다 낮을 경우 
						if( fMaxHeight > m_fWldHeight )
						{
							if( bRender )
								RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadiusOut, 0xffffff00 );
							else
							{
								FLOAT fHeightItv = ( m_fWldHeight - fMaxHeight ) / nInverval;//( 4 * MPU);
								// 끝과 지금의 거리차와 높이차를 구하면 현재의 높이!
								fWldHeight = fMaxHeight + ( ( nRadiusOut - r ) * fHeightItv );
								if( fWldHeight < m_world.GetLandHeight( (float)( xx ), (float)( yy ) ) ) 
									m_world.SetHeight( xx, yy, fWldHeight );
							}
						}
						// 높이가 같을 경우 
						else
						{
							if( bRender )
								RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadiusOut, 0xffffff00 );
							else
							{
								fWldHeight = m_fWldHeight;
								m_world.SetHeight( CPoint( xx, yy ), fWldHeight );
							}
						}
					}
				}
			}
		}
	}

	m_world.CalculateBound();
}
void CWorldView::EditHeightMountain( CPoint point, BOOL bRender )
{
	FLOAT x = (float)( m_ptField.x / MPU * MPU );
	FLOAT y = (float)( m_ptField.y / MPU * MPU );
	CPoint pt = point - m_ptOrg;
	
	if( pt.y == 0 && bRender == FALSE )
		return;

	FLOAT fWldHeight = (FLOAT) m_world.GetLandHeight( x, y );
	FLOAT fWldStatic = fWldHeight;// - ( (FLOAT)pt.y / 0.0f );
	FLOAT fHeightItv = (FLOAT) abs( pt.y ) / 10.0f;

	CRect rect;
	CView::GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	int nRadius = CPageTerrainHeight::m_nRadiusHeight * MPU;

	for( int yc = -nRadius; yc < nRadius; yc += MPU) 
	{
		for( int xc = -nRadius; xc < nRadius; xc += MPU)
		{
			FLOAT r = fabs( sqrt( float(xc * xc + yc * yc ) ) );

			if( r < nRadius && !( xc % MPU) && !(yc % MPU ))
			{
				FLOAT xx = x + xc;
				FLOAT yy = y + yc;
				//if(xx < 0 || xx >= WORLD_SIZE || yy < 0 || yy >= WORLD_SIZE)
				//	continue;
				fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy ) );// / 1000.0f;
#ifdef __CSC_UNDO_REDO
				if(!bRender)
				{
					TERRAINCOORD terraincoord;
					terraincoord.m_Point.x = (LONG)( xx );
					terraincoord.m_Point.y = (LONG)( yy );
					map<TERRAINCOORD, FLOAT>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.find(terraincoord);
					FLOAT fOld = iter->second;
					if(iter == CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.end() || fWldHeight != fOld)
					{
						CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_HEIGHT_CHG);
						CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.insert(map<TERRAINCOORD, FLOAT>::value_type(terraincoord, fWldHeight));
					}
				}
#endif //__CSC_UNDO_REDO
				if( bRender )
					RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadius, 0xffffffff );
				else
				if( pt.y < 0 )
				{
					//if( fWldHeight < fWldStatic )
					{
						fWldHeight += ( cos( ( D3DX_PI / 2.0f ) * r / nRadius) ) * fHeightItv;// / 100.0f );// * pt.y;
						if( fWldHeight >= 65000 ) fWldHeight = 65000;
						m_world.SetHeight( CPoint( (int)( xx ), (int)( yy ) ), fWldHeight );
					}
				}
				else
				{
					//if( fWldHeight > fWldStatic )
					{
						fWldHeight -= ( cos( ( D3DX_PI / 2.0f ) * r / nRadius ) ) * fHeightItv;// * pt.y;
						if( fWldHeight < 0 ) fWldHeight = 0;
						m_world.SetHeight( CPoint( (int)( xx ), (int)( yy ) ), fWldHeight );
					}
				}
			}
		}
	}

	m_world.CalculateBound();
}

//	마우스로 지정된 위치를 랜덤으로 높이를 설정한다.
void CWorldView::EditHeightRandom( CPoint point, BOOL bRender )
{
	int x = m_ptField.x / MPU * MPU;
	int y = m_ptField.y / MPU * MPU;

	CPoint pt = point - m_ptOrg;
	FLOAT fWldHeight;// = (FLOAT) m_world.GetLandHeight( x, y );
	//FLOAT fWldStatic;// = fWldHeight - (FLOAT)pt.y;
	//EditHeightFlat( point );
	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	int nRadius = CPageTerrainHeight::m_nRadiusHeight * MPU;

	//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	if( m_bCursorInWorld)
	{
		vector = m_vIntersect;
		x = int( vector.x + ( MPU / 2 ) ) / MPU * MPU;
		y = int( vector.z + ( MPU / 2 ) ) / MPU * MPU;


		for( int yc = -nRadius; yc < nRadius; yc += MPU ) 
		{
			for( int xc = -nRadius; xc < nRadius; xc += MPU )
			{
				FLOAT r = fabs( sqrt( float(xc * xc + yc * yc) ) );

				if( r < nRadius && !( xc % MPU) && !(yc % MPU ))
				{
					int xx = x + xc;
					int yy = y + yc;
					//if( xx < 0 || xx >= WORLD_SIZE || yy < 0 || yy >= WORLD_SIZE )
					//	continue;
					fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( xx ), (float)( yy ) );// / 1000.0f;
#ifdef __CSC_UNDO_REDO
					if(!bRender)
					{
						TERRAINCOORD terraincoord;
						terraincoord.m_Point.x = xx;
						terraincoord.m_Point.y = yy;
						map<TERRAINCOORD, FLOAT>::iterator iter = CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.find(terraincoord);
						FLOAT fOld = iter->second;
						if(iter == CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.end() || fWldHeight != fOld)
						{
							CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::TERRAIN_HEIGHT_CHG);
							CCommandMgrBeast::GetInstance()->m_mapOldTerrainHeigth.insert(map<TERRAINCOORD, FLOAT>::value_type(terraincoord, fWldHeight));
						}
					}
#endif //__CSC_UNDO_REDO
					if( bRender )
						RenderHeightPoint( D3DXVECTOR3( (float)( xx ), fWldHeight, (float)( yy ) ), (int)( r ), nRadius, 0xffffffff );
					else
					{
						fWldHeight += ( float( random( 20 ) ) / 100.0f ) - 0.1f;
						m_world.SetHeight( CPoint( xx, yy ), fWldHeight );//fWldHeightTotal );
					}
				}
			}
		}
	}

	m_world.CalculateBound();
}
void CWorldView::EditHeightAttribute( CPoint point, int nAttr )
{
	//HRESULT hr;
	//int x;// = m_ptField.x / MPU * MPU;
	//int y;// = m_ptField.y / MPU * MPU;
	
	CPoint pt = point - m_ptOrg;
	//FLOAT fWldHeight;// = (FLOAT) m_world.GetLandHeight( x, y );
	//FLOAT fWldStatic;// = fWldHeight - (FLOAT)pt.y;
	
	//EditHeightFlat( point );
	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	int nRadius = CPageTerrainHeight::m_nRadiusHeight * MPU;

	//m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector );
	vector = m_vIntersect;
	m_world.SetHeightAttribute( (int)( vector.x ), (int)( vector.z ), nAttr, (float)( nRadius ) );
	/*
	if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	{
		x = int( vector.x ) / MPU * MPU;
		y = int( vector.z ) / MPU * MPU;
		
		for( int yc = -nRadius; yc < nRadius; yc += MPU ) 
		{
			for( int xc = -nRadius; xc < nRadius; xc += MPU )
			{
				FLOAT r = fabs( sqrt( xc * xc + yc * yc ) );
				
				if( r < nRadius && !( xc % MPU) && !(yc % MPU ))
				{
					int xx = x + xc;
					int yy = y + yc;
					m_world.SetHeightAttribute( xx, yy, nAttr );
				}
			}
		}
		
	}
	m_world.CalculateBound();
	*/
}
void CWorldView::FlatPatch(CPatch* pPatch)
{
	D3DXVECTOR3 vLT,vRT,vLB,vRB;//,vTemp;
	int xx,yy;
	float y1,y2,y3,y4;
	float dx,dy;
	vLT=D3DXVECTOR3((float)( pPatch->m_nWorldX*MPU ),0,(float)( pPatch->m_nWorldY*MPU) );
	vRT=D3DXVECTOR3((float)( pPatch->m_nWorldX*MPU+MPU*PATCH_SIZE ),0,(float)( pPatch->m_nWorldY*MPU) );
	vLB=D3DXVECTOR3((float)( pPatch->m_nWorldX*MPU ),0,(float)( pPatch->m_nWorldY*MPU+MPU*PATCH_SIZE) );
	vRB=D3DXVECTOR3((float)( pPatch->m_nWorldX*MPU+MPU*PATCH_SIZE ),0,(float)( pPatch->m_nWorldY*MPU+MPU*PATCH_SIZE) );
	vLT.y=m_world.GetLandHeight(vLT);
	vRT.y=m_world.GetLandHeight(vRT);
	vLB.y=m_world.GetLandHeight(vLB);
	vRB.y=m_world.GetLandHeight(vRB);
//	vTemp=(vLT+vRT+vLB+vRB)/4;
	for(int i=0;i<PATCH_SIZE+1;i++) {
		yy=(i+pPatch->m_nWorldY)*MPU;
		dy=(float)i/PATCH_SIZE;
		for(int j=0;j<PATCH_SIZE+1;j++) {
			xx=(j+pPatch->m_nWorldX)*MPU;
			dx=(float)j/PATCH_SIZE;
			y1=vLT.y*(1-dx)*(1-dy);
			y2=vRT.y*(dx)*(1-dy);
			y3=vLB.y*(1-dx)*(dy);
			y4=vRB.y*(dx)*(dy);
			m_world.SetHeight(CPoint(xx,yy),y1+y2+y3+y4);
		}
	}
}

void CWorldView::SetLightColor( CPoint point )
{
/*
	FLOAT xx, x = point.x;
	FLOAT yy, y = point.y;
	FLOAT fRadius;
	FLOAT r1,g1,b1;
	FLOAT r2,g2,b2;
	FLOAT r,g,b;
	D3DCOLOR xColor;

	for( int yc = -CPageTerrainColor::m_nRadiusVColor; yc < CPageTerrainColor::m_nRadiusVColor; yc++) 
	{
		for( int xc = -CPageTerrainColor::m_nRadiusVColor; xc < CPageTerrainColor::m_nRadiusVColor; xc++)
		{
			fRadius = fabs( sqrt( xc * xc + yc * yc ) );
			if( fRadius < CPageTerrainColor::m_nRadiusVColor )
			{
				xx = x + xc;
				yy = y + yc;
				if( ((int)xx % MAP_SIZE > 0 && (int)xx % PATCH_SIZE == 0) || (int)xx % MAP_SIZE > MAP_SIZE-2  ) continue;
				if( ((int)yy % MAP_SIZE > 0 && (int)yy % PATCH_SIZE == 0) || (int)yy % MAP_SIZE > MAP_SIZE-2  ) continue;

				r1 = ( CPageTerrainColor::m_dwVertexColor & 0x00ff0000 ) >> 16;
				g1 = ( CPageTerrainColor::m_dwVertexColor & 0x0000ff00 ) >> 8;
				b1 = ( CPageTerrainColor::m_dwVertexColor & 0x000000ff );

				xColor = m_world.GetColor( xx, yy );

				r2 = ( xColor & 0x00ff0000 ) >> 16;
				g2 = ( xColor & 0x0000ff00 ) >> 8;
				b2 = ( xColor & 0x000000ff );

				if( r1 >= 127 &&  g1 >= 127 && b1 >= 127 )
				{
					r1 -= r2; g1 -= g2; b1 -= b2;

					// m_nRadiusVColor : r = fRadius : x

					r1 = (FLOAT)r1 * ( (FLOAT)CPageTerrainColor::m_nRadiusVColor - fRadius ) / (FLOAT)CPageTerrainColor::m_nRadiusVColor;
					g1 = (FLOAT)g1 * ( (FLOAT)CPageTerrainColor::m_nRadiusVColor - fRadius ) / (FLOAT)CPageTerrainColor::m_nRadiusVColor;
					b1 = (FLOAT)b1 * ( (FLOAT)CPageTerrainColor::m_nRadiusVColor - fRadius ) / (FLOAT)CPageTerrainColor::m_nRadiusVColor;

					r1 += r2; g1 += g2; b1 += b2;

					r = r1 > r2 ? r1 : r2;
					g = g1 > g2 ? g1 : g2;
					b = b1 > b2 ? b1 : b2;

					xColor = D3DCOLOR_ARGB( 0, (DWORD)r, (DWORD)g, (DWORD)b );
				}
				else
				{
					// m_nRadiusVColor : r = fRadius : x
					// 10 : 128 = 10 : 128
					// 10 : ( 128 - 100 ) = 5 : x
					// 10 : 28 = 5 : 14
					// 128 - 14 = 114

					r1 = ( r2 - r1 ) * (  CPageTerrainColor::m_nRadiusVColor - fRadius ) / CPageTerrainColor::m_nRadiusVColor;
					g1 = ( g2 - g1 ) * (  CPageTerrainColor::m_nRadiusVColor - fRadius ) / CPageTerrainColor::m_nRadiusVColor;
					b1 = ( b2 - b1 ) * (  CPageTerrainColor::m_nRadiusVColor - fRadius ) / CPageTerrainColor::m_nRadiusVColor;

					r1 = r2 - r1;
					g1 = g2 - g1;
					b1 = b2 - b1;

					r = r1 < r2 ? r1 : r2;
					g = g1 < g2 ? g1 : g2;
					b = b1 < b2 ? b1 : b2;

					xColor = D3DCOLOR_ARGB( 0, (DWORD)r, (DWORD)g, (DWORD)b );
				}
				m_world.SetColor( xx, yy, xColor );
			}
		}
	}
*/
}

CObj* CWorldView::PickCollection( CPoint point )
{
	CRect rect;
	GetClientRect(&rect);
	CObj* pObjOld = m_world.GetObjFocus();
	CObj* pObjNew = NULL;
	// 카피해두기 
	CPtrArray ptrArray;
	for( int i = 0; i < m_aPickCollection.GetSize(); i++ )
		ptrArray.Add( m_aPickCollection.GetAt( i ) );
	m_aPickCollection.RemoveAll();
	int nCount = m_world.GetPickCollection( rect, point, &m_world.m_matProj, &m_camera.m_matView, &m_aPickCollection );
	if( nCount )
	{
		// 수집된 갯수가 있으면 비교해서 차례대로 픽킹한다.
		// 이것은 겹쳐진 오브젝트가 있을 경우 피킹시 차례대로 피킹이 가능하게 하기 위한 것임.
		int i = 0;
		if( nCount == ptrArray.GetSize() )
		{
			for( ; i < m_aPickCollection.GetSize(); i++ )
			{
				if( ptrArray.GetAt( i ) != m_aPickCollection.GetAt( i ) )
				{
					m_nPickCollection = 0;
					break;
				}
			}
		}
		else
			m_nPickCollection = 0;
		pObjNew = (CObj*)m_aPickCollection.GetAt( m_nPickCollection );
		if( i == m_aPickCollection.GetSize() )
		{
			m_nPickCollection++;
			if( m_nPickCollection >= m_aPickCollection.GetSize() )
				m_nPickCollection = 0;
		}
	}
	else
	{
		m_nPickCollection = 0;
		m_aPickCollection.SetSize( 0 );
		pObjNew = NULL;
	}

	//if( pObjNew )
	//{
	//	//@@ KYT
	//	CModelObject* pModelObject = static_cast< CModelObject* >( pObjNew->m_pModel );

	//	for( int Ei = 0 ; Ei < MAX_ELEMENT ; ++Ei )
	//	{
	//		CObject3D* pObject3d = pModelObject->GetObject3D( Ei );

	//		if( !pObject3d )
	//			continue;

	//		for( int Gi = 0 ;Gi < pObject3d->GetMaxObject(); ++Gi )
	//		{
	//			GMOBJECT* pGmObj = pObject3d->GetGMOBJECT(  Gi ) ;

	//			if( !pGmObj )
	//				continue;

	//			for( int Mi = 0; Mi < pGmObj->m_nMaxMaterial; ++Mi )
	//			{
	//				MATERIAL* pMaterial =  &pGmObj->m_MaterialAry[ Mi ];
	//				__asm nop
	//			}
	//		}
	//	}
	//}

	return pObjNew;
}
void CWorldView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	SetCapture();
	m_bLButtonDown = TRUE;
	m_ptOrg = point;
	m_ptButtonDown = point;
	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	if( m_nTool == ID_VIEW_SCROLL )
	{
		//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector) )
		if( m_bCursorInWorld )
		{
			vector = m_vIntersect;
			GetPickRay( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vPickRayOrig, &m_vPickRayDir );
			GetRayEnd( &vPickRayOrig, &m_vPickRayDir, &m_vPickRayDir );
			m_bScrollPossible = TRUE;
		}
		else
			m_bScrollPossible = FALSE;
	}
	else
	if( m_nTool == ID_VIEW_ZOOM )
	{
		GetPickRay( rect, CPoint( rect.Width() / 2 , rect.Height() / 2 ) , &m_world.m_matProj, &m_camera.m_matView, &vPickRayOrig, &m_vPickRayDir );
	}
	else
	if( m_nTool == ID_VIEW_ROTATION )
	{
		// 화면의 중심에서 지상 끝까지 광선을 쏴 회전 축을 얻는다.
		GetPickRay( rect, CPoint( rect.Width() / 2, rect.Height() / 2 ), &m_world.m_matProj, &m_camera.m_matView, &vPickRayOrig, &vPickRayDir );
		GetRayEnd( &vPickRayOrig, &vPickRayDir, &m_camera.m_vLookAt);
	}
	else
	{
		//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector, TRUE ) )
		if( m_bCursorInWorld )
		{
			vector = m_vIntersect;
			m_vOrigin = vector;

			//if( !(GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
			//	m_world.UnselectObj();
			//if( GetTool() == ID_SELECT_MOVE )
			//{
				//m_world.UnselectObj();
			//}
			if( GetTool() == ID_SELECT_PUT )
			{				
				if( m_pObjBrush )
				{
					m_world.UnselectObj();
					DWORD dwObjType = pMainFrame->m_pObjView->m_dwObjType;
					DWORD dwObjIndex = pMainFrame->m_pObjView->m_dwObjIndex;

#ifdef __Y_PATROL
					if( g_pMainFrame->m_dlgPatrol.IsEdit() == FALSE && dwObjType == OT_PATH )
					{
						return;
					}
#endif //__Y_PATROL
					CObj* pObj = CreateObj( m_pd3dDevice, dwObjType, dwObjIndex );
					//vector = D3DXVECTOR3( rand() % 63*4, 10, rand() % 63*4);
					if( m_bOnGrid )
					{
						//vector.x = int( vector.x );/// 4 ) * 4;
						//vector.z = int( vector.z );/// 4 ) * 4;
						vector.x = vector.x / m_fGridX;//) / 4 ) * 4;
						vector.z = vector.z / m_fGridZ;/// 4 ) * 4;
						vector.x = int( vector.x ) * m_fGridX;
						vector.z = int( vector.z ) * m_fGridZ;/// 4 ) * 4;
					}
					pObj->SetPos( m_vBrushPos + vector );
					//pObj->m_vecPos.y = 10;//2m_world.GetTile(vPickRayDir)->m_byHeight;
					FLOAT fRandomScale = 0.0f;
					int nDegree = 0;
				
					if( m_bRandomScale )
					{
						fRandomScale = ( rand() % int( ( m_fRandomScale2 - m_fRandomScale1 ) * 1000 ) ) + ( m_fRandomScale1 * 1000 );
						fRandomScale /= 1000;
					}
					if( m_bRandomDegree )
						nDegree =  ( rand() % ( m_nRandomDegree2 - m_nRandomDegree1 ) ) + m_nRandomDegree1;

					pObj->SetScale( m_vBrushScale + D3DXVECTOR3( fRandomScale,fRandomScale,fRandomScale ) );
					pObj->SetAngle( (float)( nDegree ) );
					pObj->UpdateLocalMatrix();
					m_world.AddObj( pObj );
					m_world.SetSelectObj( pObj, TRUE );
					m_aPickCollection.RemoveAll();
					m_aPickCollection.Add( pObj );
					m_nPickCollection = 0;

					if( m_bRespawnEnable )
					{
						CRespawnInfo* pRespawnInfo = NULL;
						if( pObj->GetType() == OT_MOVER )
						{
							CMover* pMover = (CMover*)pObj;
							pRespawnInfo = &pMover->m_respawnInfo;
						}
						if( pObj->GetType() == OT_ITEM )
						{
							CItem* pItem = (CItem*)pObj;
							pRespawnInfo = &pItem->m_respawnInfo;
						}
						if( pObj->GetType() == OT_CTRL )
						{
							CCommonCtrl* pCtrl = (CCommonCtrl*)pObj;
							pRespawnInfo = &pCtrl->m_respawnInfo;
						}
						if( pRespawnInfo )
						{
							D3DXVECTOR3 vPos = pObj->GetPos();
							CRect rect( 0, 0, MPU * 12, MPU * 12);
							if( pRespawnInfo->m_dwIndex == 0 )
							{
								rect.left   = - MPU * 6;
								rect.top    = - MPU * 6;
								rect.right  = + MPU * 6;
								rect.bottom = + MPU * 6;
								pRespawnInfo->m_rect = rect;
							}
							pRespawnInfo->m_dwType  = pObj->GetType();
							pRespawnInfo->m_dwIndex = pObj->GetIndex();
							pRespawnInfo->m_cb = m_nRespawnNum;
							pRespawnInfo->m_uTime = m_nRespawnSec;
						}
					}
#ifdef __Y_PATROL
					if( g_pMainFrame->m_dlgPatrol.IsEdit() && pObj )
					{
						if( pObj->GetType() == OT_PATH )
							g_pMainFrame->m_dlgPatrol.AddPath((CPathObj*)pObj );						
					}
#endif //__Y_PATROL
					
#ifdef __CSC_WORLDOBJ_LIST
					m_world.ResetWorldObjList(m_vIntersect);
#endif //__CSC_WORLDOBJ_LIST
				}
			}
			else
			if( GetTool() == ID_TERRAIN_TEXTURE )
			{
				switch(CPageTerrainTexture::m_nLandTexture) {
				case LANDTEX_NORMAL:
					SetTexture( vector.x, vector.z, (WORD)( m_dwTerrainBrush ),
						(FLOAT)( CPageTerrainTexture::m_nRadiusTexture ), (FLOAT)( CPageTerrainTexture::m_nHardnessTexture ) );
					break;
/*
				case LANDTEX_DETAIL:
					m_world.SetDetailPatch( vector.x, vector.z,m_dwTerrainBrush); 
					break;
*/
				case LANDTEX_BLANK:
					m_world.SetBlankPatch( (int)( vector.x ), (int)( vector.z ) ); 
					break;
				case LANDTEX_LANDMARK:
//					m_world.SetTexture( vector.x, vector.z, m_dwTerrainBrush,
//						CPageTerrainTexture::m_nRadiusTexture, CPageTerrainTexture::m_nHardnessTexture); 
					m_world.SetLandMarkPatch( (int)( vector.x ), (int)( vector.z ), (WORD)( m_dwTerrainBrush ) );
					break;
				}
			}
			else
			if( GetTool() == ID_TERRAIN_HEIGHT )
			{
				D3DXVECTOR3 vector;
				//if(m_world.ClientPointToVector(rect,point,&m_world.m_matProj,&m_camera.m_matView,&vector))
				if( m_bCursorInWorld )
				{
					vector = m_vIntersect;
					m_bPickUpTile = TRUE;
					m_ptField.x = (LONG)( vector.x / 1.0f + (MPU / 2) ); // 커서 포인트가 중심으로 이동하기위해 좌표 보정 
					m_ptField.y = (LONG)( vector.z / 1.0f + (MPU / 2) ); // 커서 포인트가 중심으로 이동하기위해 좌표 보정 
					m_fWldHeight = (FLOAT) m_world.GetLandHeight( (float)( m_ptField.x ), (float)( m_ptField.y ) );

					/*
					void CPageTerrainHeight::OnHeightMountain() 
					{
						m_nHeightStyle = 0;
					}

					void CPageTerrainHeight::OnHeightFlat() 
					{
						m_nHeightStyle = 1;
					}

					void CPageTerrainHeight::OnHeightSoft() 
					{
						m_nHeightStyle = 2;
					}
					void CPageTerrainHeight::OnHeightRandom() 
					{
						m_nHeightStyle = 3;	
					}

					void CPageTerrainHeight::OnHeightAttribute() 
					{
						m_nHeightStyle = 4;
					}*/

					if( CPageTerrainHeight::m_nHeightStyle == 2 )
						EditHeightSoft( point );
					else
					if( CPageTerrainHeight::m_nHeightStyle == 1 )
					{
						if( CPageTerrainHeight::m_bCheckFixedHeight )
							m_fWldHeight = CPageTerrainHeight::m_fFixedHeight;
						EditHeightFlat( point );
					}
					else
					if( CPageTerrainHeight::m_nHeightStyle == 0 )
						EditHeightMountain( point );
					else
					if( CPageTerrainHeight::m_nHeightStyle == 3 ) // random
						EditHeightRandom( point );
					else
					if( CPageTerrainHeight::m_nHeightStyle == 4 ) // attribute
						EditHeightAttribute( point, CPageTerrainHeight::m_nHeightAttribute );
				}
			}
			else
			if( GetTool() == ID_TERRAIN_WATER )
			{
				//if(m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
				if( m_bCursorInWorld )
				{
					#ifdef __N_WATERFRAME_050202
						m_world.SetWaterHeight( (int)( vector.x ), (int)( vector.z ), CPageTerrainWater::m_nWaterHeight, CPageTerrainWater::m_nWaterTexture | CPageTerrainWater::m_nWaterFrame ); 
					#else
						m_world.SetWaterHeight( vector.x, vector.z, CPageTerrainWater::m_nWaterHeight, CPageTerrainWater::m_nWaterTexture ); 
					#endif//__N_WATERFRAME_050202
				}
			}
			else
			if( GetTool() == ID_SELECT || GetTool() == ID_SELECT_MOVE || GetTool() == ID_SELECT_ROTATION || GetTool() == ID_SELECT_SCALE )
			{
				m_world.SetSelectGridPos();
				if( m_bLock == FALSE )
				{											
					CObj* pObjOld = m_world.GetObjFocus();
					CObj* pObjNew = PickCollection( point );

					//CObj* pObjNew = nCount ? m_aPickCollection.GetAt( m_nPickCollection ) : NULL ;//m_world.PickObject( rect, point, &m_world.m_matProj, &m_camera.m_matView );
					//if( pObjNew ) 

					if( pObjNew )
					{
						m_world.SetObjFocus( pObjNew );
					}
					if( m_3DRectTracker.OnLButtonDown( nFlags, point ) == TRUE )
						return;
					m_world.SetObjFocus( pObjNew );
					/*
					m_nPickRectPoint = -1;
					CObj* pObjFocus = m_world.GetObjFocus();
					if( pObjFocus && pObjFocus->GetType() == OT_MOVER && ((CMover*)pObjFocus)->IsRespawn() )
					{
						CRect rectRespawn = ((CMover*)pObjFocus)->m_respawnInfo.rect;
						CPoint pt = CPoint( pObjFocus->GetPos().x, pObjFocus->GetPos().z );
						pt.x = pt.x / MPU * MPU;  
						pt.y = pt.y / MPU * MPU;
						rectRespawn += pt;
			
						m_nPickRectPoint = PickRectTracker(rectRespawn , point );
						if( m_nPickRectPoint >= 0 )
							return;
					}
					*/
					//m_pObjCurrent = NULL;

					g_pMainFrame->m_dlg_Object_Texture_List.BindObjectTexture( pObjNew );

					if( pObjNew )
					{
						m_bPickUpTile = TRUE;
						// LCONTROL을 누르고 있을 때 
						if( (GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
						{
							if( m_world.IsSelectObj( pObjNew ) == FALSE )//->m_bSelect == FALSE )
							{
								m_world.SetSelectObj( pObjNew, TRUE );
								//m_world.SetObjFocus( pObj );
							}
							else
								m_world.SetSelectObj( pObjNew, FALSE );
						}
						else
						{
							if( m_world.IsSelectObj( pObjNew ) == FALSE )//->m_bSelect == FALSE )
								m_world.UnselectObj();
							m_world.SetSelectObj( pObjNew, TRUE );
						//	m_pObjCurrent = pObj;
							//m_world.SetObjFocus( pObj );
						}
						GetSelectCenter();

						

						//g_pMainFrame->m_workSpace.m_pDlgWorkObject->m_fScaleX = pObjNew->GetScale().x;
						//g_pMainFrame->m_workSpace.m_pDlgWorkObject->m_fScaleY = pObjNew->GetScale().y;
						//g_pMainFrame->m_workSpace.m_pDlgWorkObject->m_fScaleZ = pObjNew->GetScale().z;
						//g_pMainFrame->m_workSpace.m_pDlgWorkObject->UpdateData( FALSE );
					}
					else
					{
						m_rectDraw.SetRect( m_ptButtonDown.x, m_ptButtonDown.y, point.x, point.y );
					}
				}

			}
			else
			if( GetTool() == ID_TERRAIN_COLOR )
			{
				m_world.SetColor( (float)( vector.x ), (float)( vector.z ), CPageTerrainColor::m_dwVertexColor ,(float)( CPageTerrainColor::m_nRadiusVColor ), (float)( CPageTerrainColor::m_nHardnessVColor ) );
			}

			RenderScene();
			return;
		}
		/*
		// 화면의 중심에서 마우스 포인터가 닿는 곳까지 광선을 쏴 마우스를 포인팅할 지점을 얻는다.
		GetPickRay(point,&g_matProj,&g_matView,&vPickRayOrig,&vPickRayDir);
		GetRayEnd(&vPickRayOrig,&vPickRayDir,&vPickRayDir);

		DWORD dwObjType  = pMainFrame->m_workSpace.m_pDlgWorkObject->m_dwObjType;
		DWORD dwObjIndex = pMainFrame->m_workSpace.m_pDlgWorkObject->m_dwObjIndex;

		if(g_pMainFrame->m_pObjBrush)
		{
			CObj* pObj = CreateObj(this,dwObjType,dwObjIndex);
			pObj->m_vecPos = vPickRayDir;
			//pObj->m_vecPos.y = 10;//2m_world.GetTile(vPickRayDir)->m_byHeight;
			pObj->m_vecScale = D3DXVECTOR3(0.4f,0.4f,0.4f);
			m_world.AddObj(pObj);
		}
		*/
	}

	Invalidate(FALSE);
	CWnd::OnLButtonDown(nFlags, point);
}
D3DXVECTOR3 CWorldView::GetSelectCenter() 
{
	// 중심축 구하기 
	D3DXVECTOR3 v1 = D3DXVECTOR3( 9999999.0f, 9999999.0f, 9999999.0f);
	D3DXVECTOR3 v2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);
	// 양끝 좌표를 구한다.
	for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++ )
	{
		CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
		//if( pObj && pObj->m_bSelect )
		{
			if( pObj->GetPos().x < v1.x )
				v1.x = pObj->GetPos().x;
			if( pObj->GetPos().y < v1.y )
				v1.y = pObj->GetPos().y;
			if( pObj->GetPos().z < v1.z )
				v1.z = pObj->GetPos().z;
			if( pObj->GetPos().x > v2.x )
				v2.x = pObj->GetPos().x;
			if( pObj->GetPos().y > v2.y )
				v2.y = pObj->GetPos().y;
			if( pObj->GetPos().z > v2.z )
				v2.z = pObj->GetPos().z;
		}
	}
	// 종심을 계산한다.
	FLOAT fx = v2.x - v1.x;
	FLOAT fy = v2.y - v1.y;
	FLOAT fz = v2.z - v1.z;
	m_world.m_vCenter.x = v1.x + ( fx / 2.0f );
	m_world.m_vCenter.y = v1.y + ( fy / 2.0f );
	m_world.m_vCenter.z = v1.z + ( fz / 2.0f );
	return m_world.m_vCenter;
}

void CWorldView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;
	// 셀릭트 드래그중 
	if( m_nTool == ID_BUTTON_ICON_CONTINENT )
	{
		// gmpbigsun TEST:picking point 
		D3DXVECTOR3 kCurPos = m_vIntersect;
		kCurPos.y += 5.0f;
		AddLineData( kCurPos );				//대륙 외곽 
	}
	else
	if( m_bSelectDrag == TRUE )
	{
		if( !(GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
			m_world.UnselectObj();		
		m_world.SetSelectObj( m_rectDraw );
		GetSelectCenter();
		m_aPickCollection.RemoveAll();
		m_nPickCollection = 0;
		m_world.SetObjFocus( NULL );
	}
	else
	if( m_bLock == FALSE && m_bCursorInWorld ) //m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	{
		vector = m_vIntersect;
		if( /*GetTool() == ID_SELECT_MOVE &&*/ m_bLBDownMouseMove == FALSE )
		{
			CObj* pObj = NULL;// = m_world.PickObject( rect, point, &m_world.m_matProj, &m_camera.m_matView );//, 0xffffffff, m_world.GetObjFocus() );
			if( m_aPickCollection.GetSize() )
				pObj = (CObj*)m_aPickCollection.GetAt( m_nPickCollection );
			if( !(GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
			{
				m_world.UnselectObj();
				if( pObj )
					m_world.SetSelectObj( pObj, TRUE );

				if( pObj )
					m_world.SetObjFocus( pObj );
				if( m_3DRectTracker.OnLButtonDown( nFlags, point ) == TRUE )
					return;

			}
			GetSelectCenter();
		}
	}
	if( m_bCopyObj == TRUE )
	{
		if( AfxMessageBox( "Objects copy", MB_YESNO ) == IDNO )
		{
			m_world.DeleteSelectObj();
		}
	}
#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->EndCommand();
#endif //__CSC_UNDO_REDO
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->m_pageTerrainLayer.m_pLandscape = FALSE;
	pMainFrame->m_pageTerrainLayer.UpdateLayer( m_vIntersect, &m_world );
	m_bCopyObj = FALSE;
	m_bLButtonDown = FALSE;
	m_bLBDownMouseMove = FALSE;
	m_bSelectDrag = FALSE;
	m_bPickUpTile = FALSE;
	Invalidate( FALSE );
	CWnd::OnLButtonUp(nFlags, point);
}
BOOL CWorldView::OnPopupCommand(UINT nID)
{
	switch(nID)
	{
		case ID_POPUP_WORLD_REMOVE:
			m_world.DeleteSelectObj();
			m_aPickCollection.RemoveAll();
			m_nPickCollection = 0;
			break;
		case ID_POPUP_WORLD_INVISIBLE:
			m_world.InvisibleSelectObj();
			break;
		case ID_POPUP_WORLD_INVISIBLE_UPSTAIRS:
			{
			CObj* pObj = m_world.GetObjFocus();
			if( pObj) 
				m_world.InvisibleUpstairsObj( pObj->GetPos().y );
			}
			break;
		case ID_POPUP_WORLD_SHOW_ALL_OBJECTS:
			{
			m_world.ShowAllObjects();
			}
			break;
		case ID_POPUP_WORLD_PROPERTY:
			DoProperty();
			break;
		case ID_POPUP_WORLD_GET_OBJECT:
			//ProcessTool(ID_FIELD_GET_HEIGHT,m_pPopupBkg);
			break;

		case ID_POPUP_WORLD_INVISIBLE_COOLSION:
			m_world.InvisibleCollisonSelectObj( );
			break;
			
		case ID_POPUP_WORLD_ROTATE:		
			//gmpbigsun(100222) : 수치입력받아서 회전시켜주기 
			{
				if( !m_world.m_arySelect.empty( ) )
					CreateModalRotateObjects( &m_world.m_arySelect );
			}
			
			break;

		case ID_POPUP_WORLD_TRANSLATION:
			//수치를 입력받아서 이동
			{
				CObj* pObj = m_world.GetObjFocus( );
				if( pObj )
					CreateModalTranslateObject( pObj );
			}
			break;

		case ID_POPUP_WORLD_SCALE:
			//gmpbigsun(20110405) : 수치입력받아서 스케일 조정 
			{
				if( !m_world.m_arySelect.empty( ) )
					CreateModalScaleObjects( &m_world.m_arySelect );
			}
			break;

	}
	Invalidate( FALSE );
//	m_pPopupBkg = NULL;
	return TRUE;
}
void CWorldView::DoProperty() 
{
	if( m_bCursorInWorld )
	{
		D3DXVECTOR3 vector = m_vIntersect;
		CObj* pObj = m_world.GetObjFocus();
			//= m_world.PickObject( rect, point, &m_world.m_matProj, &m_camera.m_matView );
		if( pObj )
		{
			CPropertySheet objProperty( _T( "Object Properties" ), this );
			CPagePropObj pageObj; pageObj.Init( pObj );
			CPagePropLight pageLight;
			CPagePropMover pageMover; pageMover.Init( pObj );
			CPagePropRespawn pageRespawn; pageRespawn.Init( pObj );
			CPagePropAttribute pageAttribute; pageAttribute.Init( (CRegion*)pObj );
			CPagePropTrigger pageTrigger; pageTrigger.Init( (CRegion*)pObj );
			CPagePropCaption pageCaption; pageCaption.Init( (CRegion*)pObj );
		#ifdef __N_TRIGGER_AREASET_050203
			CPagePropAreaSet pageAreaSet; pageAreaSet.Init( (CRegion*)pObj );
		#endif//__N_TRIGGER_AREASET_050203
			CPagePropPos pagePos; pagePos.Init( (CRegion*)pObj );
			CPagePropStructure pageStr; pageStr.Init( (CRegion*)pObj );
			CPagePropControl pageControl; pageControl.Init( (CCommonCtrl*)pObj );
#ifdef __Y_PATROL
			CPagePropPath pagePath;pagePath.Init( pObj );
#endif //__Y_PATROL
			
			switch( pObj->GetType() )
			{
			case OT_OBJ: 
//				return;
				objProperty.AddPage( &pageObj );
//				objProperty.AddPage( &pageLight );
//				pageLight.Init( pObj );
				break;
			case OT_CTRL:
//				if( ((CCommonCtrl*)pObj)->GetProp()->dwCtrlKind1 == CK1_CHEST )
				{
					objProperty.AddPage( &pageRespawn );
					objProperty.AddPage( &pageControl );
				}
				break;
			case OT_ITEM:
				objProperty.AddPage( &pageRespawn );
				break;
			case OT_MOVER:
				//objProperty.AddPage( &pageObj );
				pageMover.m_pWorldView = this;
				objProperty.AddPage( &pageMover );
				objProperty.AddPage( &pageRespawn );
				break;
#ifdef __Y_PATROL
			case OT_PATH:
				//objProperty.AddPage( &pageObj );
				objProperty.AddPage( &pagePath );
				break;				
#endif //__Y_PATROL
			case OT_REGION:
				//objProperty.AddPage( &pageObj );
				//objProperty.AddPage( &pageAttribute );
				//objProperty.AddPage( &pageTrigger );
				//objProperty.AddPage( &pageCaption );
				if( pObj->GetIndex() == RI_TRIGGER  )
				{
					objProperty.AddPage( &pageAttribute );
					objProperty.AddPage( &pageTrigger );
					objProperty.AddPage( &pageCaption );
				#ifdef __N_TRIGGER_AREASET_050203
					objProperty.AddPage( &pageAreaSet );
				#endif//__N_TRIGGER_AREASET_050203
				}
				else
				if( pObj->GetIndex() == RI_ATTRIBUTE )
				{
					objProperty.AddPage( &pageAttribute );
					objProperty.AddPage( &pageTrigger );
					objProperty.AddPage( &pageCaption );
				#ifdef __N_TRIGGER_AREASET_050203
					objProperty.AddPage( &pageAreaSet );
				#endif//__N_TRIGGER_AREASET_050203
				}
				else
				if( pObj->GetIndex() == RI_BEGIN || pObj->GetIndex() == RI_REVIVAL || pObj->GetIndex() == RI_PLACE )
				{
					if( pObj->GetIndex() == RI_BEGIN )
						return;
					objProperty.AddPage( &pagePos );
					//m_pObjProperty->AddPage( &pageAttribute );
					//m_pObjProperty->AddPage( m_pPageRegion );
				}
				else
				if( pObj->GetIndex() == RI_STRUCTURE )// || pObj->GetIndex() == RI_REVIVAL )
				{
					objProperty.AddPage( &pageStr );
				}
				break;
			}
			//m_pObjProperty->Create( g_pMainFrame );
			if( objProperty.DoModal() == IDOK )
			{

			}
		}
	}

}
void CWorldView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( m_nTool == ID_BUTTON_ICON_CONTINENT )
	{
		SubLineData_Back( );
		CView::OnRButtonUp(nFlags, point);
		return;
	}

	m_bRButtonDown = FALSE;
	m_bScrollPossible = FALSE;
	m_bPickUpTile = FALSE;
	ReleaseCapture();

	CRect rect;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector;

	//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	if( m_bCursorInWorld )
	{
		vector = m_vIntersect;
		if( /*GetTool() == ID_SELECT_MOVE &&*/ m_bLBDownMouseMove == FALSE )
		{
			CObj* pObj = m_world.PickObject( rect, point, &m_world.m_matProj, &m_camera.m_matView );
			if( pObj == NULL )
			{
				if( !(GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
					m_world.UnselectObj();
			}
			else
			{
				//m_world.UnselectObj();
				//if( m_aPickCollection.GetSize() )
				//{
				//	CObj* pObjColl = (CObj*)m_aPickCollection.GetAt( m_nPickCollection );
				//	if( pObj != pObjColl )
				//		pObj = pObjColl;
				//}
				//m_world.SetSelectObj( pObj, TRUE );
			}
		}
	}
	Invalidate( FALSE );


/*
 CObjProperty*   m_pObjProperty;
 CPagePropObj*       m_pPageObj;
 CPagePropCtrl*      m_pPageCtrl; 
 CPagePropItem*      m_pPageItem; 
 CPagePropMover*     m_pPageMover;
 CPagePropLight*     m_pPageLight;
 CPagePropRegion*    m_pPageRegion;
 CPagePropTrigger*   m_pPageTrigger;
 CPagePropAttribute* m_pPageAttribute;
*/
	/*
	CPagePropObj       m_pPageObj;
	CPagePropCtrl      m_pPageCtrl; 
	CPagePropItem      m_pPageItem; 
	CPagePropMover     m_pPageMover;
	CPagePropLight     m_pPageLight;
	CPagePropRegion    m_pPageRegion;
	CPagePropTrigger   m_pPageTrigger;
	CPagePropAttribute m_pPageAttribute;
*/
	//if( m_world.ClientPointToVector( rect, point, &m_world.m_matProj, &m_camera.m_matView, &vector ) )
	if( m_bCursorInWorld )
	{
		CMenu menu; 
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_REMOVE       ,"&Remove");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_INVISIBLE    ,"&Invisible");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_INVISIBLE_UPSTAIRS,"&Invisible Upstairs");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_SHOW_ALL_OBJECTS,"&Show All Objects");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_GET_OBJECT   ,"&Get Object");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_PROPERTY     ,"&Properties...");
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_INVISIBLE_COOLSION, "&Invisible collision" );
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_ROTATE, "&Rotate" );
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_TRANSLATION, "&Translate" );
		menu.AppendMenu(MF_STRING, ID_POPUP_WORLD_SCALE, "&Scale" );

		//m_ptBegClient = point;
		ClientToScreen( &point ); 
		if( menu.TrackPopupMenu( TPM_RIGHTBUTTON, point.x, point.y, this ) )
		{
		}

	}


	
	CView::OnRButtonUp(nFlags, point);
}
void CWorldView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bRButtonDown = TRUE;
	SetCapture();

	CObj* pObjOld = m_world.GetObjFocus();
	CObj* pObjNew = PickCollection( point );

	m_world.SetObjFocus( pObjNew );

	if( m_world.IsSelectObj( pObjNew ) == FALSE )//->m_bSelect == FALSE )
		m_world.UnselectObj();

	if( pObjNew )
		m_world.SetSelectObj( pObjNew, TRUE );

	GetSelectCenter();

	Invalidate( FALSE );

}
void CWorldView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	m_bMButtonDown = FALSE;
	ReleaseCapture();
//Wnd::OnMButtonUp(nFlags, point);
}
void CWorldView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	m_bMButtonDown = TRUE;
	SetCapture();

	CRect rect;
	m_ptOrg = point;
	GetClientRect(&rect);
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir ;
	D3DXVECTOR3 vector ;

	if( GetAsyncKeyState(VK_LMENU) & 0x8000 )
	{
		// 화면의 중심에서 지상 끝까지 광선을 쏴 회전 축을 얻는다.
		GetPickRay(rect,CPoint(rect.Width()/2,rect.Height()/2),&m_world.m_matProj,&m_camera.m_matView,&vPickRayOrig,&vPickRayDir);
		CPoint pt = point - m_ptOrg;
		UpdateCamera( pt ) ;
		m_bScrollPossible = FALSE;
	}
	else
	//if(m_world.ClientPointToVector(rect,point,&m_world.m_matProj,&m_camera.m_matView,&vector))
	if( m_bCursorInWorld )
	{
		vector = m_vIntersect;
		GetPickRay(rect,point,&m_world.m_matProj,&m_camera.m_matView,&vPickRayOrig,&m_vPickRayDir);
		GetRayEnd(&vPickRayOrig,&m_vPickRayDir,&m_vPickRayDir);
		m_vOrigin = vector;
		m_bScrollPossible = TRUE;
	}
	else
		m_bScrollPossible = FALSE;

//Wnd::OnMButtonDown(nFlags, point);
}
BOOL CWorldView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( GetAsyncKeyState(VK_LSHIFT) & 0x8000 && m_dwBrushType == BRUSHTYPE_OBJECT && m_pObjBrush )
	{
		m_vBrushPos.y += (FLOAT)zDelta / 100.0f;
	}
	else
	if( GetAsyncKeyState(VK_LCONTROL) & 0x8000 && m_dwBrushType == BRUSHTYPE_OBJECT && m_pObjBrush )
	{
		m_vBrushScale.x += (FLOAT)zDelta / 10000.0f;
		m_vBrushScale.y += (FLOAT)zDelta / 10000.0f;
		m_vBrushScale.z += (FLOAT)zDelta / 10000.0f;
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);
		D3DXVECTOR3 vPickRayOrig;
		D3DXVECTOR3 vPickRayDir ;
		D3DXVECTOR3 vBackPos = m_camera.m_vPos;
		GetPickRay(rect,CPoint(rect.Width()/2,rect.Height()/2),&m_world.m_matProj,&m_camera.m_matView,&vPickRayOrig,&vPickRayDir);
		D3DXVECTOR3 vecAdd = vPickRayDir * (float)( (zDelta / 50) );
		m_camera.m_vPos += vecAdd;
		m_camera.m_vLookAt = m_camera.m_vPos + vPickRayDir ;//+= vecAdd;
	//	if(m_vecPos.y <= 20)
		//	m_vecPos = vBackPos;
	}
	Invalidate(FALSE);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
/*
void CWorldView::GetPickRay(POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir)
{
  HRESULT hr;

	CRect rect;
	GetClientRect(&rect);

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  ( ( ( 2.0f * ptCursor.x ) / rect.Width()  ) - 1 ) / pmatProj->_11;
  v.y = -( ( ( 2.0f * ptCursor.y ) / rect.Height() ) - 1 ) / pmatProj->_22;
  v.z =  1.0f;

  // Get the inverse view matrix
  D3DXMATRIX m;
  //m_pd3dDevice->GetTransform( D3DTS_VIEW, &g_matView );
  D3DXMatrixInverse( &m, NULL, pmatView );

  // Transform the screen space pick ray into 3D space
  pvPickRayDir->x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
  pvPickRayDir->y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
  pvPickRayDir->z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
  pvPickRayOrig->x = m._41;
  pvPickRayOrig->y = m._42;
  pvPickRayOrig->z = m._43;
	//TRACE("RayOrg (%f,%f,%f) RayDir(%f,%f,%f)\n",vPickRayOrig.x,vPickRayOrig.y,vPickRayOrig.z,vPickRayDir.x,vPickRayDir.y,vPickRayDir.z);
}
void CWorldView::GetRayEnd(D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd)
{
	FLOAT fyOrig = pvPickRayOrig->y;
	FLOAT fyDir  = -pvPickRayDir->y;
	pvPickRayDir->x = fyOrig * pvPickRayDir->x / fyDir; // vecY : posY = vecX : posX
	pvPickRayDir->z = fyOrig * pvPickRayDir->z / fyDir; // vecY : posY = vecZ : posZ
	pvPickRayDir->y = -fyOrig; // 아래 *pvPickRayDir + *pvPickRayOrig에서 y가 0이 나오게 하려면 -fyOrig를 넣어야 한다.
	//pvPickRayDir->y = fyOrig * m_vPickRayDir.y / fyDir;
	*pvPickRayEnd = *pvPickRayDir + *pvPickRayOrig;
	//PickRayEnd->y = 0;
}
*/
void CWorldView::OnViewZoom() 
{
	m_nTool = ID_VIEW_ZOOM;
}

void CWorldView::OnUpdateViewZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nTool == ID_VIEW_ZOOM);
}

void CWorldView::OnViewScroll() 
{
	m_nTool = ID_VIEW_SCROLL;
}

void CWorldView::OnUpdateViewScroll(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nTool == ID_VIEW_SCROLL);
}

void CWorldView::OnViewRotation() 
{
	m_nTool = ID_VIEW_ROTATION;
}

void CWorldView::OnUpdateViewRotation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nTool == ID_VIEW_ROTATION);
}

void CWorldView::OnViewFramemove() 
{
	m_bFrameMoving = !m_bFrameMoving;
	if(m_bFrameMoving)
	{
		DXUtil_Timer(TIMER_RESET);
		SetTimer(1,16,0);
	}
	else
		KillTimer(1);
}

void CWorldView::OnUpdateViewFramemove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bFrameMoving);
	
}

void CWorldView::OnEditDraw() 
{
	return;
	//m_nTool = VIEW_DRAW;
Pause( FALSE );
	HRESULT hr;
	if( m_bActive && m_bWindowed)
	{
		RECT rcClientOld;
		rcClientOld = m_rcWindowClient;

		// Update window properties
		GetWindowRect(&m_rcWindowBounds );
		GetClientRect(&m_rcWindowClient );

		//if( rcClientOld.right - rcClientOld.left !=
			//	m_rcWindowClient.right - m_rcWindowClient.left ||
				//rcClientOld.bottom - rcClientOld.top !=
				//m_rcWindowClient.bottom - m_rcWindowClient.top)
		{
			// A new window size will require a new backbuffer
			// size, so the 3D structures must be changed accordingly.
//			m_bReady = FALSE;

			m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

			// Resize the 3D environment
			if( FAILED( hr = Reset3DEnvironment() ) )
			{
				//DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
				return;
			}
		//	m_bReady = TRUE;
		}
	}	
}

void CWorldView::OnUpdateEditDraw(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck(m_nTool == VIEW_DRAW);	
}
/*
void CWorldView::OnEditHeight() 
{
	m_nTool = VIEW_HEIGHT;
}

void CWorldView::OnUpdateEditHeight(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nTool == VIEW_HEIGHT);
}

*/
void CWorldView::OnViewZoomExtents() 
{
	// TODO: Add your command handler code here
	//FLOAT fYaw   = DEGREETORADIAN( m_nYaw );
	//FLOAT fPitch = DEGREETORADIAN( m_nPitch );
	//FLOAT fRoll  = DEGREETORADIAN( m_nRoll );
	//m_camera.m_vPos = D3DXVECTOR3(0,300.0f,0.0f);
	//m_camera.m_vLookAt = D3DXVECTOR3(m_camera.m_vPos.x,0.0f,m_camera.m_vPos.z);
	//m_camera.m_vLookAt = D3DXVECTOR3(m_camera.m_vPos.x+sinf(fYaw),m_camera.m_vLookAt.y,m_camera.m_vPos.z+cosf(fYaw));	
	m_camera.m_vPos.y = 200.0f;
	m_camera.m_fYaw = 0;
	m_camera.m_fPitch = D3DX_PI/2;
	RenderScene();
}

void CWorldView::OnUpdateViewZoomExtents(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CWorldView::OnViewFog() 
{
	// TODO: Add your command handler code here
	m_world.m_bViewFog = !m_world.m_bViewFog;
	m_world.SetFogEnable( m_pd3dDevice, m_world.m_bViewFog );
	RenderScene();
}

void CWorldView::OnUpdateViewFog(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewFog );
}

void CWorldView::OnViewLight() 
{
	m_world.m_bViewLight = !m_world.m_bViewLight;
	RenderScene();
}

void CWorldView::OnUpdateViewLight(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewLight );	
}

void CWorldView::OnViewWireframe() 
{
	// TODO: Add your command handler code here

	m_world.m_bViewWireframe = !m_world.m_bViewWireframe;
	RenderScene();
}

void CWorldView::OnUpdateViewWireframe(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewWireframe );
}

void CWorldView::OnViewSkybox() 
{
	m_world.m_bViewSkybox = !m_world.m_bViewSkybox;
	RenderScene();
}

void CWorldView::OnUpdateViewSkybox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewSkybox );
}

void CWorldView::OnViewLod() 
{
	m_world.m_bViewLODTerrain = !m_world.m_bViewLODTerrain;
	RenderScene();
}

void CWorldView::OnUpdateViewLod(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewLODTerrain );
}

void CWorldView::OnViewGrid() 
{
	m_world.m_bViewGrid = !m_world.m_bViewGrid;
	RenderScene();
}

void CWorldView::OnUpdateViewGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewGrid );
}
void CWorldView::OnViewGridPatch() 
{
	m_world.m_bViewGridPatch = !m_world.m_bViewGridPatch;
	RenderScene();	
}

void CWorldView::OnUpdateViewGridPatch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewGridPatch );	
}

#ifdef __CSC_RENDER_AXIS
void CWorldView::OnViewAxis() 
{
	m_world.m_bViewAxis = !m_world.m_bViewAxis;
	RenderScene();
}
#endif //__CSC_RENDER_AXIS

void CWorldView::OnViewGridLandscape() 
{
	m_world.m_bViewGridLandscape = !m_world.m_bViewGridLandscape;
	RenderScene();	
}

void CWorldView::OnUpdateViewGridLandscape(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewGridLandscape );	
}

void CWorldView::OnViewTerrain() 
{
	m_world.m_bViewTerrain = !m_world.m_bViewTerrain;
	RenderScene();
}

void CWorldView::OnUpdateViewTerrain(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewTerrain );
}

void CWorldView::OnViewAllObjects() 
{
	m_world.m_bViewAllObjects = !m_world.m_bViewAllObjects;
	RenderScene();
}

void CWorldView::OnUpdateViewObject(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewAllObjects );	
}



void CWorldView::OnViewWater() 
{
	m_world.m_bViewWater = !m_world.m_bViewWater;
	RenderScene();
}

void CWorldView::OnUpdateViewWater(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewWater );
	
}

void CWorldView::OnExecGame() 
{
	// TODO: Add your command update UI handler code here
	int nResult;
	nResult = g_pMainFrame->MessageBox("Play this world?","Warning!",MB_ICONEXCLAMATION|MB_YESNO);
	if(nResult == IDYES)
	{
		CWorldDoc* pWorldDoc = (CWorldDoc*)GetDocument(); 
		m_world.SaveWorld( MakePath( DIR_WORLD, "Play\\Play.wld" ), TRUE, TRUE, TRUE, FALSE, TRUE );
		CString strGameExe = MakePath( "", "neuzSingle.exe" );
		SetCurrentDirectory( prj.m_szPrjPath );
		_spawnl( _P_NOWAIT, strGameExe, strGameExe, "", "play", NULL );
		//_spawnl( _P_NOWAIT,strGameExe,strGameExe,prj.m_strPrjFile,pFieldDoc->GetTitle(),"OI_GAWELL",NULL);
	}	
}

void CWorldView::OnUpdateExecGame(CCmdUI* pCmdUI) 
{
}

void CWorldView::OnSetupContinent()
{
	m_nTool = ID_BUTTON_ICON_CONTINENT;
}

void CWorldView::OnUpdateSetupContinent(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_nTool == ID_BUTTON_ICON_CONTINENT );
}

void CWorldView::OnViewContinent()
{
	BOOL bShow = g_kDlgContinent.IsWindowVisible( );

	if( bShow )
	{
		g_kDlgContinent.ShowWindow( SW_HIDE );
		g_kDlgContinent.OutContinent( );
	}
	else 
	{
		g_kDlgContinent.ShowWindow( SW_SHOW );
	}

}

void CWorldView::OnUpdateViewContinent( CCmdUI* pCmdUI )
{
	BOOL bShow = g_kDlgContinent.IsWindowVisible();
	pCmdUI->SetCheck( bShow );
}

void CWorldView::OnFileSaveAsWorld() 
{
	CFileDialog dlg( FALSE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"World file (*.wld)|*.wld|");
	if(dlg.DoModal()!=IDOK) 
		return;
static BOOL bSaveDDS = FALSE;
static BOOL bSaveDYO = TRUE;
static BOOL bSaveRGN = TRUE;
static BOOL bSaveLND = TRUE;
	CDlgSaveOption dlgSaveOption;
	dlgSaveOption.m_bDDS = bSaveDDS;
	dlgSaveOption.m_bDYO = bSaveDYO;
	dlgSaveOption.m_bRGN = bSaveRGN;
	dlgSaveOption.m_bLND = bSaveLND;
	if( dlgSaveOption.DoModal() == IDOK )
	{
		bSaveDDS = dlgSaveOption.m_bDDS;
		bSaveDYO = dlgSaveOption.m_bDYO;
		bSaveRGN = dlgSaveOption.m_bRGN;
		bSaveLND = dlgSaveOption.m_bLND;
	}
	if( m_world.SaveasWorld( dlg.GetFileName(), bSaveLND, bSaveDYO, bSaveRGN, bSaveDDS ) == FALSE )
		AfxMessageBox( _T( "Save Failure! The file might be at 'read only' mode" ) );
	else
	{
		CWorldDoc* pDoc = GetDocument();
		//pDoc->SetTitle( dlg.GetFileName() );
		CString strName = dlg.GetFileName();
		strName.MakeLower(); 
		if( strName.Find( ".wld" ) == -1 )
			strName = strName + ".wld";
			
		pDoc->SetPathName( strName );
	}
}

void CWorldView::OnFileSaveWorld() 
{
	CWorldDoc* pDoc = GetDocument();
	CString strFileName = pDoc->GetPathName();
	if( strFileName.IsEmpty() )
		OnFileSaveAsWorld();
	else
	if( m_world.SaveWorld( strFileName ) == FALSE )
		AfxMessageBox( _T( "Save Failure! The file might be at 'read only' mode" ) );
	else
	{	
		CWorldDoc* pDoc = GetDocument();
		pDoc->SetTitle( strFileName );
	}
}

void CWorldView::OnUpdateFileSaveWorld(CCmdUI* pCmdUI) 
{
}

void CWorldView::OnUpdateCoorX(CCmdUI* pCmdUI) 
{
	CString string = "x =";
	if( m_bCursorInWorld )
		string.Format( "x = %f", m_vIntersect.x );
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateCoorY(CCmdUI* pCmdUI) 
{
	CString string = "y =";
	if( m_bCursorInWorld )
		string.Format( "y = %f", m_vIntersect.y );
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateCoorZ(CCmdUI* pCmdUI) 
{
	CString string = "z =";
	if( m_bCursorInWorld )
		string.Format( "z = %f", m_vIntersect.z );
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateLandName(CCmdUI* pCmdUI) 
{
	CString string = "Land =";
	if( m_bCursorInWorld )
	{
		CString strLandName = m_world.m_szFileName;
		strLandName.Delete( strLandName.GetLength() - 4, 4 );
		string.Format( "Land = %s%02d-%02d.lnd", strLandName, (int)m_vIntersect.x / ( MAP_SIZE * MPU ) , (int)m_vIntersect.z / ( MAP_SIZE * MPU ) );
	}
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateLayer(CCmdUI* pCmdUI) 
{
	CString string = "Layer =";
	if( m_bCursorInWorld )
	{
		CLandscape* pLand = m_world.GetLandscape( m_vIntersect );
		if( pLand )
			string.Format( "Layer = %d", pLand->m_aLayer.GetSize() );
	}
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateObjectName(CCmdUI* pCmdUI) 
{
	CString string = "Obj =";
	if( m_bCursorInWorld )
	{
		if( m_world.m_arySelect.size() == 1)
		{
			CObj* pObj = (CObj*)m_world.m_arySelect.at( 0 ); 
			if( pObj )
			{
				TCHAR szModelName[ 128 ];
				prj.m_modelMng.MakeModelName( szModelName, pObj->GetType(), pObj->GetIndex() );
					string.Format( "Obj = %s", szModelName );
			}
		}
	}
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}
void CWorldView::OnUpdateTerrainName(CCmdUI* pCmdUI) 
{
	CString string = "Tex =";
	if( m_bCursorInWorld )
	{
		int nTexture = m_world.GetTexture( (int)( m_vIntersect.x ), (int)( m_vIntersect.z ) );
		if( nTexture >= 0 )
		{
			LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( nTexture );
			if( lpTerrain )
				string.Format( "Tex = %s", lpTerrain->m_szTextureFileName );
		}
	}
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}

LRESULT CWorldView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_MBUTTONDOWN )
	{
		OnMButtonDown( wParam, CPoint( (int) LOWORD(lParam), (int) HIWORD(lParam) ) );
	}
	else
	if( message == WM_MBUTTONUP )
	{
		OnMButtonUp( wParam, CPoint( (int) LOWORD(lParam), (int) HIWORD(lParam) ) );
	}
	return CView::DefWindowProc(message, wParam, lParam);
}
LONG CWorldView::OnCloseObjProperty(UINT wParam,LONG lParam)
{/*
	if( m_pPageObj )
	{
		m_pPageObj->DestroyWindow();
		delete m_pPageObj;
	}
	if( m_pPageLight )
	{
		m_pPageLight->DestroyWindow();
		delete m_pPageLight;
	}
	if( m_pObjProperty )
	{
		m_pObjProperty->DestroyWindow();
		delete m_pObjProperty;
	}
	m_pObjProperty   = NULL;
	m_pPageObj       = NULL;
	m_pPageLight     = NULL;
	*/
	//PaintField(TRUE);
	Invalidate();
	return TRUE;
}

//void CWorldView::OnLightMap( )
//{
//#ifdef __BS_SUPPORT_LIGHTMAP
//	AfxMessageBox( "Do you want to make a lightmap? or to remove" );
//#else
//	AfxMessageBox( "Not supported lightmap yet" );	
//#endif
//	//gmpbigsun( 20110307 )
//	//각 랜드스케이프 별로 LightMap을 만들고 그 LightMap은 항상 나중에 그려지게 설정한다.
//	//생성순서
//	//1. lightmap전용 layer를 생성한다.
//	//2. 랜드스케이프의 처음 삼각형부터 마지막 삼각형까지 순회하면서 빛과 각도를 계산한다
//	//3. 각도의 크기에 따라 색을 결정한다.
//	//4. 적당한 알파값과 결과색을 조합한후 layer의 색깔을 채운다.
//	//5. lightmap layer는 항상 나중에 그려지도록 한다.
//	//6. 저장할때도 가장 나중에 그려져야함을 기록한다.
//	//7. 게임에서는 lightmap을 위해 특별히 해줄것이 없다. 그냥 렌더링하면 끝.
//
//	
//	CDialogProgress kProgress;
//	kProgress.Create( IDD_DIALOG_PROGRESS );
//	kProgress.CenterWindow();
//	kProgress.ShowWindow( SW_SHOW );
//	kProgress.m_kProgress.SetRange( 0, m_world.m_nLandHeight * m_world.m_nLandWidth - 1 );
//
//	BOOL bOnLightMap = FALSE;
//
//#ifdef __BS_SUPPORT_LIGHTMAP
//	for( int h = 0; h < m_world.m_nLandHeight; ++h )
//	{
//		for( int w = 0; w < m_world.m_nLandWidth; ++w )
//		{
// 			CLandscape* pLandscape	= m_world.m_apLand[ h * m_world.m_nLandWidth + w ];
////			ASSERT( pLandscape );
//
//			//현재 로딩안댄 Landscape는 로딩 -> makelightmap -> 삭제 
//			if( NULL == pLandscape )
//			{
//				pLandscape = new CLandscape;
//				pLandscape->m_nWorldX = w * MAP_SIZE;
//				pLandscape->m_nWorldY = h * MAP_SIZE;
//				pLandscape->InitDeviceObjects( m_pd3dDevice, &m_world );
//				
//				CString strLand = m_world.m_szFileName;
//				strLand.Delete( strLand.GetLength() - 4, 4 );
//
//				CString strLandTemp;
//				strLandTemp.Format( "%s%s%02d-%02d.lnd", m_world.m_szFilePath, strLand, w, h );
//
//				if( pLandscape->LoadLandscape( strLandTemp , w, h ) == FALSE )
//				{
//					delete pLandscape;
//					continue;
//				}
//
//				bOnLightMap = pLandscape->MakeLightMap( );
//				delete pLandscape;
//			}
//			else
//				bOnLightMap = pLandscape->MakeLightMap( );		//이미 로딩된 Landscape는 삭제하면 안대고 할짖만 함
//
//			kProgress.m_kProgress.SetPos( h * m_world.m_nLandWidth + w );
//			Sleep( 50 );
//		}
//	}
//
//	if( bOnLightMap )
//		AfxMessageBox( "OK! All lightmap created" );
//	else 
//		AfxMessageBox( "OK! All lightmap deleted" );
//#endif
//}

void CWorldView::OnSelectPut() 
{
	CWorldView::SetTool( ID_SELECT_PUT );
	//m_nSelect = ID_SELECT_PUT;	
	Invalidate( FALSE );
}

void CWorldView::OnUpdateSelectPut(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_SELECT_PUT );	
}

void CWorldView::OnSelect() 
{
	CWorldView::SetTool( ID_SELECT );
	//m_nTool = ID_SELECT;
	Invalidate( FALSE );
}

void CWorldView::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_SELECT );
}

void CWorldView::OnSelectMove() 
{
	CWorldView::SetTool( ID_SELECT_MOVE );
	// = ID_SELECT_MOVE;	
	Invalidate( FALSE );
}

void CWorldView::OnUpdateSelectMove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_SELECT_MOVE );	
}

void CWorldView::OnSelectRotation() 
{
	CWorldView::SetTool( ID_SELECT_ROTATION );
	//m_nSelect = ID_SELECT_ROTATION;	
	Invalidate( FALSE );
}

void CWorldView::OnUpdateSelectRotation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_SELECT_ROTATION );	
}

void CWorldView::OnSelectScale() 
{
	CWorldView::SetTool( ID_SELECT_SCALE );
	//m_nSelect = ID_SELECT_SCALE;	
	Invalidate( FALSE );
}
void CWorldView::OnUpdateSelectScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_SELECT_SCALE );	
}

void CWorldView::OnRestrictX() 
{
	m_nRestrict = ID_RESTRICT_X;	
}

void CWorldView::OnUpdateRestrictX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nRestrict == ID_RESTRICT_X );	
}

void CWorldView::OnRestrictY() 
{
	m_nRestrict = ID_RESTRICT_Y;	
}

void CWorldView::OnUpdateRestrictY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nRestrict == ID_RESTRICT_Y );		
}

void CWorldView::OnRestrictZ() 
{
	m_nRestrict = ID_RESTRICT_Z;	
}

void CWorldView::OnUpdateRestrictZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nRestrict == ID_RESTRICT_Z );	
}

void CWorldView::OnRestrictXz() 
{
	m_nRestrict = ID_RESTRICT_XZ;
}

void CWorldView::OnUpdateRestrictXz(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nRestrict == ID_RESTRICT_XZ );
}


void CWorldView::OnRespawn() 
{
	// TODO: Add your command handler code here

	//m_dlgRespawn.m_pWorldView = this;
	//m_dlgRespawn.m_pWorld = &m_world;
	//m_dlgRespawn.Create( CDlgRespawn::IDD );

	
}

void CWorldView::OnUpdateRespawn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bDlgRespawn );
}


void CWorldView::OnFileSaveAsBitmap() 
{
	CFileDlgWorld dlg( FALSE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Bitmap file (*.bmp)|*.bmp|");
	if( dlg.DoModal() != IDOK ) 
		return;
	BOOL bViewRegion = FALSE;
	BOOL bViewName = FALSE;
	BOOL bViewSpawn = FALSE;
	BOOL bViewHeightAttrb = FALSE;
	BOOL bViewWater = TRUE;

	CDlgBmpSaveOption dlgBmpSaveOption;
	if( dlgBmpSaveOption.DoModal() == IDOK )
	{
		bViewRegion = dlgBmpSaveOption.m_bViewRegion;
		bViewName = dlgBmpSaveOption.m_bViewName;
		bViewSpawn = dlgBmpSaveOption.m_bViewSpawn;
		bViewHeightAttrb = dlgBmpSaveOption.m_bHeightAttrb;
		bViewWater = dlgBmpSaveOption.m_bWater;
	}

	//gmpbigsun: 흠 비트맵 저장시 MPU가 4가 아닐경우 이상현상이 발생하는데 이유를 찾아보자...일단4로 바꿔서 찍는다.
	// 이렇게 하면 가끔 하얀 이미지가 저장되는경우가 있는데
	// g_MPU = backMPU; 이부분을 주석화해서 MPU 4로 한프레임 돌게 해줘야 정상작동된다.. 찍고난후 프로그램 종료해야한다. 강제로 MPU를 변경했기때문에

	//sun here
	int backMPU = g_MPU;
//	if( g_MPU != OLD_MPU )
//	{
//		g_MPU = OLD_MPU;
//		Reset3DEnvironment( );		//MPU가 바뀌면 이것을 꼭 해줘야 제대로 먹는다.
//	}

	m_world.SaveBitmap( dlg.GetFileName(), m_pFont, bViewRegion, bViewName, bViewSpawn, bViewWater, bViewHeightAttrb );	

//	if( backMPU != g_MPU )
//	{
//		g_MPU = backMPU;
//		Reset3DEnvironment( );
//	}

	CString str;
	str.Format( "Bitmap을 %s로 저장했습니다", dlg.GetFileName() );
	AfxMessageBox( str.GetString() );
}

void CWorldView::OnUpdateFileSaveAsBitmap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CWorldView::OnTerrainTexture() 
{
	m_nTool = ID_TERRAIN_TEXTURE;
}

void CWorldView::OnUpdateTerrainTexture(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_TERRAIN_TEXTURE );
}

void CWorldView::OnTerrainColor() 
{
	m_nTool = ID_TERRAIN_COLOR;
}

void CWorldView::OnUpdateTerrainColor(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_TERRAIN_COLOR );
}

void CWorldView::OnTerrainHeight() 
{
	m_nTool = ID_TERRAIN_HEIGHT;
}

void CWorldView::OnUpdateTerrainHeight(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_TERRAIN_HEIGHT );
}

void CWorldView::OnEditOnland() 
{
	m_bEditOnLand = !m_bEditOnLand;
	if( m_bEditOnLand )
	{
		for( int i = 0; i < (int)( m_world.m_arySelect.size() ); i++ )
		{
			CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
			pObj->SetOnLand();
		}
		RenderScene();
	}
}

void CWorldView::OnUpdateEditOnland(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bEditOnLand );
}

void CWorldView::OnViewSpawn() 
{
	m_world.m_bViewSpawn = !m_world.m_bViewSpawn;
	RenderScene();
}

void CWorldView::OnUpdateViewSpawn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewSpawn );	
}

void CWorldView::OnViewObj() 
{
	m_world.m_bViewObj = !m_world.m_bViewObj;
	RenderScene();
}

void CWorldView::OnUpdateViewObj(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewObj );	
}

void CWorldView::OnViewMover() 
{
	m_world.m_bViewMover = !m_world.m_bViewMover;
	RenderScene();
}

void CWorldView::OnUpdateViewMover(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewMover );	
}
void CWorldView::OnViewHeightAttribute() 
{
	m_world.m_bViewHeightAttribute = !m_world.m_bViewHeightAttribute;
	RenderScene();
}

void CWorldView::OnUpdateViewHeightAttribute(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewHeightAttribute );
}
void CWorldView::OnViewRegion() 
{
	m_world.m_bViewRegion = !m_world.m_bViewRegion;
	RenderScene();
}

void CWorldView::OnUpdateViewRegion(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewName );	
}
void CWorldView::OnViewObjlod() 
{
	m_world.m_bViewLODObj = !m_world.m_bViewLODObj;		
	RenderScene();
}

void CWorldView::OnUpdateViewObjlod(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewLODObj );		
	
}

void CWorldView::OnViewName() 
{
	m_world.m_bViewName = !m_world.m_bViewName;
	RenderScene();
}

void CWorldView::OnUpdateViewName(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_world.m_bViewName );	
}

void CWorldView::OnEditGrid() 
{
	// TODO: Add your command handler code here
	m_bOnGrid = !m_bOnGrid;
	
}

void CWorldView::OnUpdateEditGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bOnGrid );	
}

void CWorldView::OnEditLock() 
{
	m_bLock = !m_bLock;	
}

void CWorldView::OnUpdateEditLock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bLock );	
}

void CWorldView::OnViewTop() 
{
	m_camera.m_fYaw = 0;
	m_camera.m_fPitch = D3DX_PI/2;
	RenderScene();
	
}
void CWorldView::OnViewFront() 
{
	m_camera.m_fYaw = 0;
	m_camera.m_fPitch = 0;//3.14f/2;
	RenderScene();
}

void CWorldView::OnTerrainWater() 
{
	m_nTool = ID_TERRAIN_WATER;
}

void CWorldView::OnUpdateTerrainWater(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nTool == ID_TERRAIN_WATER );
}

void CWorldView::OnWorldRevival() 
{
	CDlgRevival dlgRevival;
	dlgRevival.m_pWorld = &m_world;
	if( dlgRevival.DoModal() == IDOK )
	{


	}
}

void CWorldView::OnUpdateWorldRevival(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck( TRUE );
}

void CWorldView::OnEditRandomDegreeScale() 
{
	//m_bRandomDegreeScale = !m_bRandomDegreeScale;
	CDlgRandomDegreeScale dlgRandomDegreeScale;

	dlgRandomDegreeScale.m_bDegree = m_bRandomDegree;
	dlgRandomDegreeScale.m_bScale  = m_bRandomScale;
	dlgRandomDegreeScale.m_nDegree1 = m_nRandomDegree1;
	dlgRandomDegreeScale.m_nDegree2 = m_nRandomDegree2;
	dlgRandomDegreeScale.m_fScale1 = m_fRandomScale1;
	dlgRandomDegreeScale.m_fScale2 = m_fRandomScale2;

	if( dlgRandomDegreeScale.DoModal() )
	{
		m_bRandomDegree = dlgRandomDegreeScale.m_bDegree;
		m_bRandomScale  = dlgRandomDegreeScale.m_bScale;
		m_nRandomDegree1 = dlgRandomDegreeScale.m_nDegree1; 
		m_nRandomDegree2 = dlgRandomDegreeScale.m_nDegree2;
		m_fRandomScale1  = dlgRandomDegreeScale.m_fScale1;
		m_fRandomScale2  = dlgRandomDegreeScale.m_fScale2;
	}
}

void CWorldView::OnUpdateEditRandomDegreeScale(CCmdUI* pCmdUI) 
{
	//mdUI->SetCheck( m_bRandomDegreeScale );
}

void CWorldView::OnViewSetTime() 
{
	CDlgSetTime dlgSetTime;
	//dlgSetTime.m_nTime = m_world.m_nFixedHour;
	if( dlgSetTime.DoModal() == IDOK )
	{
	//	m_world.m_nFixedHour = dlgSetTime.m_nTime;
		RenderScene();
	}
}

void CWorldView::OnUpdateViewSetTime(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CWorldView::OnEditSetgrid() 
{
	CDlgSetGrid dlgSetGrid;
	dlgSetGrid.m_fGridX = m_fGridX;
	dlgSetGrid.m_fGridY = m_fGridY;
	dlgSetGrid.m_fGridZ = m_fGridZ;
	if( dlgSetGrid.DoModal() == IDOK )
	{
		m_fGridX = dlgSetGrid.m_fGridX;
		m_fGridY = dlgSetGrid.m_fGridY;
		m_fGridZ = dlgSetGrid.m_fGridZ;
		RenderScene();
	}
}

void CWorldView::OnEditRespawnDefault() 
{
	CDlgRespawnDefault dlgRespawn;
	dlgRespawn.m_nNum = m_nRespawnNum;
	dlgRespawn.m_nSec = m_nRespawnSec;
	if( dlgRespawn.DoModal() == IDOK )
	{
		m_nRespawnNum = dlgRespawn.m_nNum;
		m_nRespawnSec = dlgRespawn.m_nSec;
		RenderScene();
	}
}

void CWorldView::OnEditRespawnEnable() 
{
	m_bRespawnEnable = !m_bRespawnEnable;
}

void CWorldView::OnUpdateEditRespawnEnable(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bRespawnEnable );
}

void CWorldView::OnEditHeapobject() 
{
	// TODO: Add your command handler code here
	m_bEditHeapObject = !m_bEditHeapObject;
}

void CWorldView::OnUpdateEditHeapobject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck( m_bEditHeapObject );
	
}

void CWorldView::OnWorldLight() 
{
	// TODO: Add your command handler code here
	m_dlgWorldLight.ShowWindow( TRUE );
	m_dlgWorldLight.m_nLightType = m_world.m_nLightType    ;
	m_dlgWorldLight.m_nHour      = m_world.m_nLightHour    ;
	m_dlgWorldLight.m_fDiffuseR  = m_world.m_lightDirect.r1;
	m_dlgWorldLight.m_fDiffuseG  = m_world.m_lightDirect.g1;
	m_dlgWorldLight.m_fDiffuseB  = m_world.m_lightDirect.b1;
	m_dlgWorldLight.m_fAmbientR  = m_world.m_lightDirect.r2;
	m_dlgWorldLight.m_fAmbientG  = m_world.m_lightDirect.g2;
	m_dlgWorldLight.m_fAmbientB  = m_world.m_lightDirect.b2;
	m_dlgWorldLight.UpdateData( FALSE );
	m_dlgWorldLight.m_pWorld = &m_world;
	m_dlgWorldLight.m_pWorldView = this;
}

void CWorldView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	m_dlgWorldLight.Create( CDlgWorldLight::IDD );
	
	// TODO: Add your specialized code here and/or call the base class
	
}

void CWorldView::OnEditUndo() 
{
#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->UndoCommand();
	m_world.CalculateBound();
	OnPaint();
#endif //__CSC_UNDO_REDO
}

void CWorldView::OnEditRedo() 
{
#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->RedoCommand();
	m_world.CalculateBound();
	OnPaint();
#endif //__CSC_UNDO_REDO
}

void CWorldView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
}

void CWorldView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
}

void CWorldView::OnWorldProperties() 
{
	// TODO: Add your command handler code here
	CDlgWorldProp dlgWorldProp, dlgWorldPropBackup;
	dlgWorldProp.m_bFly           = m_world.m_bFly          ;
	dlgWorldProp.m_bIndoor        = m_world.m_bIsIndoor     ;
	dlgWorldProp.m_fFogDensity    = m_world.m_fFogDensity   ;
	dlgWorldProp.m_fFogStart      = m_world.m_fFogStartValue;
	dlgWorldProp.m_fFogEnd        = m_world.m_fFogEndValue  ;
	dlgWorldProp.m_fLightDirX     = m_world.m_v3LightDir.x  ;
	dlgWorldProp.m_fLightDirY     = m_world.m_v3LightDir.y  ;
	dlgWorldProp.m_fLightDirZ     = m_world.m_v3LightDir.z  ;
	dlgWorldProp.m_dwDiffuseRed   = ( m_world.m_dwDiffuse & 0x00ff0000 ) >> 16;
	dlgWorldProp.m_dwDiffuseGreen = ( m_world.m_dwDiffuse & 0x0000ff00 ) >> 8;
	dlgWorldProp.m_dwDiffuseBlue  = ( m_world.m_dwDiffuse & 0x000000ff );
	dlgWorldProp.m_dwAmbientRed   = ( m_world.m_dwAmbient & 0x00ff0000 ) >> 16;
	dlgWorldProp.m_dwAmbientGreen = ( m_world.m_dwAmbient & 0x0000ff00 ) >> 8;
	dlgWorldProp.m_dwAmbientBlue  = ( m_world.m_dwAmbient & 0x000000ff );
	dlgWorldProp.m_bUse24 = m_world.m_bIsUsing24Light;

	dlgWorldPropBackup.m_bFly            = dlgWorldProp.m_bFly           ;
	dlgWorldPropBackup.m_bIndoor         = dlgWorldProp.m_bIndoor        ;
	dlgWorldPropBackup.m_fFogDensity     = dlgWorldProp.m_fFogDensity    ;
	dlgWorldPropBackup.m_fFogStart       = dlgWorldProp.m_fFogStart      ;
	dlgWorldPropBackup.m_fFogEnd         = dlgWorldProp.m_fFogEnd        ;
	dlgWorldPropBackup.m_fLightDirX      = dlgWorldProp.m_fLightDirX     ;
	dlgWorldPropBackup.m_fLightDirY      = dlgWorldProp.m_fLightDirY     ;
	dlgWorldPropBackup.m_fLightDirZ      = dlgWorldProp.m_fLightDirZ     ;
	dlgWorldPropBackup.m_dwDiffuseRed    = dlgWorldProp.m_dwDiffuseRed   ;
	dlgWorldPropBackup.m_dwDiffuseGreen  = dlgWorldProp.m_dwDiffuseGreen ;
	dlgWorldPropBackup.m_dwDiffuseBlue   = dlgWorldProp.m_dwDiffuseBlue  ;
	dlgWorldPropBackup.m_dwAmbientRed    = dlgWorldProp.m_dwAmbientRed   ;
	dlgWorldPropBackup.m_dwAmbientGreen  = dlgWorldProp.m_dwAmbientGreen ;
	dlgWorldPropBackup.m_dwAmbientBlue   = dlgWorldProp.m_dwAmbientBlue  ;
	dlgWorldPropBackup.m_bUse24 = dlgWorldProp.m_bUse24;

	dlgWorldProp.m_pWorldView = this;
	dlgWorldProp.m_pWorld = &m_world;

	UpdateData( FALSE );		//gmpbigsun

	if( dlgWorldProp.DoModal() == IDOK )
	{
		m_world.m_bFly           = dlgWorldProp.m_bFly        ; 
		m_world.m_bIsIndoor      = dlgWorldProp.m_bIndoor     ; 
		m_world.m_fFogDensity    = dlgWorldProp.m_fFogDensity ; 
		m_world.m_fFogStartValue = dlgWorldProp.m_fFogStart   ; 
		m_world.m_fFogEndValue   = dlgWorldProp.m_fFogEnd     ; 
		m_world.m_v3LightDir.x   = dlgWorldProp.m_fLightDirX  ; 
		m_world.m_v3LightDir.y   = dlgWorldProp.m_fLightDirY  ; 
		m_world.m_v3LightDir.z   = dlgWorldProp.m_fLightDirZ  ; 
		m_world.m_dwDiffuse  = ( dlgWorldProp.m_dwDiffuseRed   << 16) | 0xff000000;
		m_world.m_dwDiffuse |= ( dlgWorldProp.m_dwDiffuseGreen << 8 );
		m_world.m_dwDiffuse |= ( dlgWorldProp.m_dwDiffuseBlue       );
		m_world.m_dwAmbient  = ( dlgWorldProp.m_dwAmbientRed   << 16) | 0xff000000;
		m_world.m_dwAmbient |= ( dlgWorldProp.m_dwAmbientGreen << 8 );
		m_world.m_dwAmbient |= ( dlgWorldProp.m_dwAmbientBlue       );
#ifdef __N_TRIGGER_AREASET_050203
		m_world.m_nPKMode = dlgWorldProp.m_nPKMode;
#endif // __N_TRIGGER_AREASET_050203

		m_world.m_bIsUsing24Light = dlgWorldProp.m_bUse24;
	}
	else
	{
		dlgWorldProp.m_bFly            = dlgWorldPropBackup.m_bFly           ;
		dlgWorldProp.m_bIndoor         = dlgWorldPropBackup.m_bIndoor        ;
		dlgWorldProp.m_fFogDensity     = dlgWorldPropBackup.m_fFogDensity    ;
		dlgWorldProp.m_fFogStart       = dlgWorldPropBackup.m_fFogStart      ;
		dlgWorldProp.m_fFogEnd         = dlgWorldPropBackup.m_fFogEnd        ;
		dlgWorldProp.m_fLightDirX      = dlgWorldPropBackup.m_fLightDirX     ;
		dlgWorldProp.m_fLightDirY      = dlgWorldPropBackup.m_fLightDirY     ;
		dlgWorldProp.m_fLightDirZ      = dlgWorldPropBackup.m_fLightDirZ     ;
		dlgWorldProp.m_dwDiffuseRed    = dlgWorldPropBackup.m_dwDiffuseRed   ;
		dlgWorldProp.m_dwDiffuseGreen  = dlgWorldPropBackup.m_dwDiffuseGreen ;
		dlgWorldProp.m_dwDiffuseBlue   = dlgWorldPropBackup.m_dwDiffuseBlue  ;
		dlgWorldProp.m_dwAmbientRed    = dlgWorldPropBackup.m_dwAmbientRed   ;
		dlgWorldProp.m_dwAmbientGreen  = dlgWorldPropBackup.m_dwAmbientGreen ;
		dlgWorldProp.m_dwAmbientBlue   = dlgWorldPropBackup.m_dwAmbientBlue  ;
		dlgWorldProp.m_bUse24		   = dlgWorldPropBackup.m_bUse24;
	}

	//gmpbigsun
	
	UpdateData( FALSE );
	Invalidate( FALSE );
}

struct ColorVertex
{
	ColorVertex( float x, float y, float z, D3DCOLOR color ) : _x( x ), _y( y ), _z( z ), _color( color ) { }

	float _x, _y, _z;
	D3DCOLOR _color;

	static const DWORD FVF;
};
const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE ;

void CWorldView::AddLineData( const D3DXVECTOR3& kPos )
{
	if( !g_kDlgContinent._pCurContinent )
		return;

	if( g_kDlgContinent._pCurContinent->_cVertices.size() > 20 )
	{
		AfxMessageBox( "continent verts cannot go beyond 20 -> addlinedata" );
		return;
	}

	g_kDlgContinent.AddExternalVertex( kPos );

	UpdateLineVertex( );
}

void CWorldView::SubLineData_Back( )
{
	g_kDlgContinent.SubExternalVertex_Back( );

	UpdateLineVertex( );
}

void CWorldView::UpdateLineVertex( )
{
	Vec3D_Container cVectors = g_kDlgContinent.GetCurVertices( );	// 레퍼런스로 받으면 안댐 복사본이 필요함
	size_t size = cVectors.size();

	if( size > 1 )
	{
		// connect the end point and start point
		cVectors.push_back( cVectors[ 0 ] );			// linked start point with end point
		++size;

		if( m_VBforLine )
			m_VBforLine->Release( );

		m_pd3dDevice->CreateVertexBuffer( (size) * sizeof( ColorVertex ), D3DUSAGE_WRITEONLY, ColorVertex::FVF, D3DPOOL_MANAGED, &m_VBforLine, 0 );

		ColorVertex* pOtVertices = 0;
		m_VBforLine->Lock( 0, 0, (void**)&pOtVertices, 0 );

		D3DXVECTOR3 kPos;
		for( int idx = 0; idx < (int)( size ); ++idx )
		{
			kPos = cVectors[ idx ];

			int c = (int)( ( (float)idx / (float)( size - 1 ) ) * 255 );
			pOtVertices[ idx ] = ColorVertex( kPos.x, kPos.y, kPos.z, D3DCOLOR_XRGB( c, c, c ) );
			
		}

		m_VBforLine->Unlock( );

		Invalidate( FALSE );

	}
}

void CWorldView::RenderLines( )
{
	Vec3D_Container cVectors = g_kDlgContinent.GetCurVertices( );		// 레퍼런스로 받으면 안댐 복사본이 필요함 

	if( cVectors.empty() )
		return;

	size_t size = cVectors.size();
	if( size > 1 )
	{
		// connect the end point and start point
		cVectors.push_back( cVectors[ 0 ] );
		++size;
	}

	for( Vec3D_Iter iter = cVectors.begin(); iter != cVectors.end(); ++iter )
	{
		RenderVertexPoint( *iter, 0xffff0000, 4 );
	}
	
	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
	m_pd3dDevice->SetStreamSource(0, m_VBforLine, 0, sizeof(ColorVertex));
	m_pd3dDevice->SetFVF(ColorVertex::FVF);
	m_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, size-1 );

	//!!
	//Invalidate( FALSE );
}

void CreateModalRotateObjects( CObjArray* pObjArray )
{
	// if one size
	int maxsize = pObjArray->size( );

	if( maxsize == 1 )
	{
		CDlgInputAngle kDlg;

		CObj* pObj = (CObj*)pObjArray->at( 0 );
		if( pObj )
			kDlg.m_fAngle = pObj->GetAngle( );

		if( IDOK == kDlg.DoModal( ) )
			Edit_RotateObject( pObj, kDlg.m_fAngle );
	}
	else if( maxsize > 1 )
	{
		CDlgInputAngle kDlg;
		CObj* pObj = (CObj*)pObjArray->at( 0 );
		if( pObj )
			kDlg.m_fAngle = pObj->GetAngle( );

		if( IDOK == kDlg.DoModal() )
		{
			for( int i = 0; i < (int)( pObjArray->size() ); i++)
			{
				pObj = (CObj*)pObjArray->at( i );
				if( pObj )
					Edit_RotateObject( pObj, kDlg.m_fAngle );
			}
		}
	}
	else
	{
		assert( 0 );
		AfxMessageBox( "error!! not selected objects" );
	}
	
}

void CreateModalScaleObjects( CObjArray* pObjArray )
{
	// if one size
	int maxsize = pObjArray->size( );
	D3DXVECTOR3 kScale;

	if( maxsize == 1 )
	{
		CDlgInputScale kDlg;

		CObj* pObj = (CObj*)pObjArray->at( 0 );
		if( pObj )
		{
			kScale = pObj->GetScale( );
			kDlg.m_fEditOldScale = kScale.x * 100.0f;		//귀찮으니 x값으로 
		}

		if( IDOK == kDlg.DoModal( ) )
			Edit_ScaleObject( pObj, kDlg.m_fEditScale * 0.01f );
	}
	else if( maxsize > 1 )
	{
		CDlgInputScale kDlg;
		CObj* pObj = (CObj*)pObjArray->at( 0 );
		if( pObj )
		{
			kScale = pObj->GetScale( );
			kDlg.m_fEditOldScale = kScale.x * 100.0f;		//귀찮으니 x값으로 
		}

		if( IDOK == kDlg.DoModal() )
		{
			for( int i = 0; i < (int)( pObjArray->size() ); i++)
			{
				pObj = (CObj*)pObjArray->at( i );
				if( pObj )
				{
					BOOL bOK = Edit_ScaleObject( pObj, kDlg.m_fEditScale * 0.01f );
					if( !bOK )
						break;
				}
			}
		}
	}
	else
	{
		assert( 0 );
		AfxMessageBox( "error!! not selected objects" );
	}
}

void Edit_RotateObject( CObj* pObj, float fAngle )
{
	// 현재 포커싱 오브젝트에 대해 회전각을 입력받아 회전시켜줌 
	D3DXVECTOR3 vecAngle;
	vecAngle.x = pObj->GetAngleX();
	vecAngle.y = pObj->GetAngle();
	vecAngle.z = pObj->GetAngleZ();
#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_ROTATE);
	CCommandMgrBeast::GetInstance()->m_mapOldObjRotate.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, vecAngle));
#endif

	pObj->SetAngle( fAngle );			//설정된 값으로 

#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->EndCommand();
#endif //__CSC_UNDO_REDO

}

BOOL Edit_ScaleObject( CObj* pObj, float fScale )
{

	// 현재 포커싱 오브젝트에 대해 회전각을 입력받아 회전시켜줌 
	D3DXVECTOR3 vecScale;
	vecScale = pObj->GetScale();

#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_SCALE);
	CCommandMgrBeast::GetInstance()->m_mapOldObjScale.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, vecScale));
#endif

	//기본 크기에서 설정값을 곱해야 한다.
	vecScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	vecScale *= fScale;
	pObj->SetScale( vecScale );			//설정된 값으로 

#ifdef __CSC_UNDO_REDO
	CCommandMgrBeast::GetInstance()->EndCommand();
#endif //__CSC_UNDO_REDO

	return TRUE;
}

void CreateModalTranslateObject( CObj* pObj )
{
	D3DXVECTOR3 kPos = pObj->GetPos( );

	CDlgInputPosition kDlg;
	kDlg.m_fPositionX = kPos.x;
	kDlg.m_fPositionY = kPos.y;
	kDlg.m_fPositionZ = kPos.z;

	if( kDlg.DoModal( ) == IDOK )
	{
#ifdef __CSC_UNDO_REDO
		CCommandMgrBeast::GetInstance()->SetCurCommandType(CCommandMgrBeast::OBJ_MOVE);
		CCommandMgrBeast::GetInstance()->m_mapOldObjMove.insert(map<CObj*, D3DXVECTOR3>::value_type(pObj, kPos));
#endif

		kPos.x = kDlg.m_fPositionX;
		kPos.y = kDlg.m_fPositionY;
		kPos.z = kDlg.m_fPositionZ;

		pObj->SetPos( kPos );

#ifdef __CSC_UNDO_REDO
		CCommandMgrBeast::GetInstance()->EndCommand();
#endif //__CSC_UNDO_REDO
	}
}

//월드맵 좌표 계산기
void CWorldView::OnWindowCalcWorldmap()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	BOOL bShow = g_kDlgCalcWorlmap.IsWindowVisible( );

	if( bShow )
		g_kDlgCalcWorlmap.ShowWindow( SW_HIDE );
	else 
		g_kDlgCalcWorlmap.ShowWindow( SW_SHOW );

}


void CWorldView::OnUpdateWindowCalcWorldmap(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	BOOL bShow = g_kDlgCalcWorlmap.IsWindowVisible();
	pCmdUI->SetCheck( bShow );
}
