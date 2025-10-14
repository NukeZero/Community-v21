// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Beast.h"

#include "MainFrm.h"
#include "ModelView.h"
#include "WorldTreeView.h"
//#include "PreviewBar.h"
#include "TerrainEdit.h"
////#include "WorldDoc.h"
//#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_WORKSPACE, OnViewWorkspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	ON_COMMAND(ID_VIEW_PREVIEWER, OnViewPreviewer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIEWER, OnUpdateViewPreviewer)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT, OnUpdateFileSaveProject)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_NAVIGATOR, &CMainFrame::OnViewNavigator)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAVIGATOR, &CMainFrame::OnUpdateViewNavigator)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COOR_X,
	ID_INDICATOR_COOR_Y,
	ID_INDICATOR_COOR_Z,
	ID_INDICATOR_LANDNAME,
	ID_INDICATOR_LAYER,  
	ID_INDICATOR_OBJECTNAME,  
	ID_INDICATOR_TERRAINNAME,  
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_terrainSheet( "fufk" ) // , m_dlgNavigator( this )
{
	// TODO: add member initialization code here
	g_pMainFrame = this;
	//m_pObjBrush = NULL;
	//m_dwTerrainTexture = 0;
	m_pObjView = NULL;
#ifdef __CSC_WORLDOBJ_LIST
	m_pWorldObjList = NULL;
#endif //__CSC_WORLDOBJ_LIST

#ifdef __CSC_TERRAIN_OPACITY
	m_nTerrainTexOpacity = 255;
#endif //__CSC_TERRAIN_OPACITY
	m_nTerrainTool = 0;
}

CMainFrame::~CMainFrame()
{
}

int __cdecl MyAllocHook(
						int nAllocType,
						void * pvData,
						size_t nSize,
						int nBlockUse,
						long lRequest,
						const unsigned char * szFileName,
						int nLine
						)
{
	if( nAllocType == _HOOK_ALLOC && nSize == 32 )
	{
		int a = 0; 
	}
	return 1;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	_CrtSetAllocHook( MyAllocHook );

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))

	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//if (!m_wndTerrainBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndTerrainBar.LoadToolBar(IDR_EDIT))
	if (!m_wndTerrainBar.Create(this) || !m_wndTerrainBar.LoadToolBar(IDR_EDIT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//if (!m_wndViewBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndViewBar.LoadToolBar(IDR_VIEW))
	if (!m_wndViewBar.Create(this) || !m_wndViewBar.LoadToolBar(IDR_VIEW))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndTerrainBar.SetBarStyle(m_wndTerrainBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndViewBar.SetBarStyle(m_wndViewBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableToolTips( TRUE );
	m_wndTerrainBar.EnableToolTips( TRUE );
	m_wndViewBar.EnableToolTips( TRUE );
	EnableToolTips( TRUE );
		
	
	
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTerrainBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndViewBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndTerrainBar);
	DockControlBar(&m_wndViewBar);
	
	m_wndWorkspaceBar.Create( this, NULL, CString( "Workspace" ), IDD_DIALOG1);
    m_wndWorkspaceBar.SetBarStyle( m_wndWorkspaceBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
    m_wndWorkspaceBar.EnableDocking( CBRS_ALIGN_ANY );
    DockControlBar( &m_wndWorkspaceBar );//, AFX_IDW_DOCKBAR_LEFT);

	//LoadBarState( "BeastBarState" );

	//CSizingControlBar2::GlobalLoadState(_T("BarState"));
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	CRect rectWindow;
	GetDesktopWindow()->GetWindowRect(rectWindow);
	cs.cx = rectWindow.Width();
	cs.cy = rectWindow.Height() - 20;
	cs.x = 0;
	cs.y = 0;
	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CView* CMainFrame::GetActiveView()
{
	CMDIChildWnd *pChild = 
             (CMDIChildWnd *) GetActiveFrame();
	return (CView *) pChild->GetActiveView();
}

void CMainFrame::OpenWorkSpace()
{
	//m_workSpace.Create(this,IDD_WORKSPACE,CBRS_RIGHT,555);  
	//m_workSpace.SetBarStyle(m_workSpace.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_FLYBY | CBRS_ALIGN_RIGHT  );//CBRS_SIZE_DYNAMIC);
	//m_workSpace.OpenWorkSpace();

	m_wndWorkspaceBar.AddView( _T( "Object" ), RUNTIME_CLASS( CModelView ) );
	m_wndWorkspaceBar.AddView( _T( "World" ), RUNTIME_CLASS( CWorldTreeView ) );

//_wndSTCBar2.AddView( _T( "Viwer" ), RUNTIME_CLASS( CPreView ) );

	//m_dlgNavigator.Create( IDD_NAVIGATOR );			//기존 네비 생략
	g_kDlgContinent.Create( IDD_DIALOG_CONTINENT_INFO );
	g_kDlg24Light.Create( IDD_DIALOG_LIGHT24 );
	g_kDlgCalcWorlmap.Create( IDD_DIALOG_CALC_WORLDMAP );
	m_dlgNavigatorMapView.Create( IDD_NAVIGATOR_MAP );
	
#ifdef __Y_PATROL
	m_dlgPatrol.Create( IDD_PATROL );	
#endif //__Y_PATROL

	BOOL bRet = m_dlg_Object_Texture_List.Create( IDD_OBJECT_TEXTURE );

	m_terrainSheet.SetTitle( "Terrain Editor", 0 );

	m_terrainSheet.AddPage( &m_pageTerrainHeight );
	m_terrainSheet.AddPage( &m_pageTerrainTexture );
	m_terrainSheet.AddPage( &m_pageTerrainColor );
	m_terrainSheet.AddPage( &m_pageTerrainWater );
	m_terrainSheet.AddPage( &m_pageTerrainLayer );

	m_terrainSheet.Create( this, WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE );

	CRect rectWindow;
	GetWindowRect( &rectWindow );

	CRect rectSheet, rectPatrol, rectNav, rectObjectTextureList;
	m_terrainSheet.GetWindowRect( &rectSheet );
	rectSheet.OffsetRect( -rectSheet.left, -rectSheet.top );
	rectSheet.OffsetRect( rectWindow.Width() - rectSheet.Width() - 10, 100 );
	m_terrainSheet.MoveWindow( &rectSheet );

#ifdef __Y_PATROL
	m_dlgPatrol.GetWindowRect( &rectPatrol );
	rectPatrol.OffsetRect( -rectPatrol.left, -rectPatrol.top );
	rectPatrol.OffsetRect( rectWindow.Width() - rectPatrol.Width() - 10, rectSheet.Height() + 100 );
	m_dlgPatrol.MoveWindow( &rectPatrol );	
#endif //__Y_PATROL

	m_dlg_Object_Texture_List.GetWindowRect( &rectObjectTextureList );
	rectObjectTextureList.OffsetRect( -rectObjectTextureList.left, -rectObjectTextureList.top );
	rectObjectTextureList.OffsetRect( rectWindow.Width() - rectObjectTextureList.Width() - 10, rectSheet.Height() + 100 );
	m_dlg_Object_Texture_List.MoveWindow( &rectObjectTextureList );	

	//기존 네비 생략
//  	m_dlgNavigator.GetWindowRect( &rectNav );
//  	rectNav.OffsetRect( -rectNav.left, -rectNav.top );
//  	rectNav.OffsetRect( rectWindow.Width() - rectNav.Width() - 10, rectSheet.Height() + rectPatrol.Height() + 100 );
//  	m_dlgNavigator.MoveWindow( &rectNav );

	CRect rectCont;
	g_kDlgContinent.GetWindowRect( &rectCont );
	rectCont.OffsetRect( -rectCont.left, -rectCont.top );
	rectCont.OffsetRect( rectWindow.Width() - rectCont.Width() - 160, rectCont.Height() - 270 );
	g_kDlgContinent.MoveWindow( &rectCont );
	g_kDlgContinent.ShowWindow( SW_HIDE );

	CRect rectNavMap;
	m_dlgNavigatorMapView.GetWindowRect( &rectNavMap );
	rectNavMap.OffsetRect( -rectNavMap.left, -rectNavMap.top );
	rectNavMap.OffsetRect( rectWindow.Width() - rectNavMap.Width() - 100, rectSheet.Height() + rectPatrol.Height() );
	rectNavMap.right = rectNavMap.left + 512;
	rectNavMap.bottom = rectNavMap.top + 512;
	m_dlgNavigatorMapView.MoveWindow( &rectNavMap );

}
void CMainFrame::CloseWorkSpace()
{
	//m_workSpace.DestroyWindow();
}
void CMainFrame::ShowWorkSpace( BOOL bShow )
{
	//m_workSpace.ShowWindow( bShow );
}
/*
void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	//CMDIFrameWnd::RecalcLayout(bNotify);
}
*/
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*
void CMainFrame::SetObjEditTool( int nTool )
{

	m_nObjEditTool = nTool;

	switch( nTool )
	{
	case 0: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_PUT         ); break;
	case 1: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_PUT_UP      ); break;
	case 2: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_SELECT      ); break;
	//case 3: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_DELETE      ); break;
	case 3: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_WIREFRAME   ); break;
	case 4: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_UNVISIBLE   ); break;
	//case 6: CWorldView::SetEditType( EDITTYPE_OBJ_EDIT_ALL_VISIBLE ); break;
	}
}

void CMainFrame::OnObjEditPut() 
{
//	SetObjEditTool( 0 );
}

void CMainFrame::OnObjEditPutUp() 
{
//	SetObjEditTool( 1 );
}

void CMainFrame::OnObjEditSelect() 
{
	SetObjEditTool( 2 );
}

void CMainFrame::OnObjEditMove() 
{
	SetObjEditTool( 3 );
}

void CMainFrame::OnObjEditRotation() 
{
	SetObjEditTool( 4 );
}
void CMainFrame::OnObjEditScale() 
{

	
}
void CMainFrame::OnObjEditDelete() 
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	if( pBeastView )
	{
		pBeastView->m_world.DeleteSelectObj();
		pBeastView->Invalidate( FALSE );
	}
}
#define EDITTYPE_HEIGHT               0
#define EDITTYPE_TEXTURE              1
#define EDITTYPE_COLOR                2
#define EDITTYPE_OBJ_EDIT_PUT         4
#define EDITTYPE_OBJ_EDIT_PUT_UP      5
#define EDITTYPE_OBJ_EDIT_SELECT      6
#define EDITTYPE_OBJ_EDIT_DELETE      7
#define EDITTYPE_OBJ_EDIT_WIREFRAME   8
#define EDITTYPE_OBJ_EDIT_UNVISIBLE   9
#define EDITTYPE_OBJ_EDIT_ALL_VISIBLE 10
*/

void CMainFrame::OnDestroy() 
{
	// TODO: Add your message handler code here
	//CSizingControlBar2::GlobalSaveState(_T("BarState"));

	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CModelView* pModelView = (CModelView*)m_wndWorkspaceBar.GetView( RUNTIME_CLASS( CModelView ) );
	if( pModelView && pModelView->IsModified() )
	{
		if( AfxMessageBox( _T( "프로젝트가 변경되었습니다. 저장하시겠습니까?" ), MB_OKCANCEL ) == IDOK )
		{
			if( pModelView->SaveModelTerrain() == FALSE )
			{
				//if( AfxMessageBox( _T( "프로젝트 저장에 문제가 생겼습니다. 그래도 종료하시겠습니까?" ), MB_OKCANCEL ) == IDCANCEL )
				if( AfxMessageBox( _T( "mdldyna.inc에 저장되어야 할 항목이 있습니다. 수동으로 넣어줘야 합니다!! \r 종료하시겠습니까?" ), MB_OKCANCEL ) == IDCANCEL )
					return;
			}
		}
	}
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnViewWorkspace() 
{
	m_wndWorkspaceBar.ShowWindow( !m_wndWorkspaceBar.IsWindowVisible() );
	RecalcLayout();
}
void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck( m_wndWorkspaceBar.IsWindowVisible() );
}

void CMainFrame::OnViewPreviewer() 
{
	//m_wndPreviewBar.ShowWindow( !m_wndPreviewBar.IsWindowVisible() );
	RecalcLayout();
}

void CMainFrame::OnUpdateViewPreviewer(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck( m_wndPreviewBar.IsWindowVisible() );
	
}

void CMainFrame::OnFileSaveProject() 
{
	AfxMessageBox( "프로젝트 저장은 약간 오래걸릴수있으니 종료메세지가 나올때까지 강종하지 마세요^^" );

	// TODO: Add your command handler code here
	CModelView* pModelView = (CModelView*)m_wndWorkspaceBar.GetView( RUNTIME_CLASS( CModelView ) );
	pModelView->SaveModelTerrain();
	
	AfxMessageBox( "프로젝트 관련파일( mdlobj.inc 또는 terrain.inc )이 저장되었습니다" );

	if( pModelView->m_bModifiedMdlDyna )
		AfxMessageBox( "mdldyna.inc에 저장되어야 할 항목이 감지되었습니다. 수동으로 반드시 넣어줘야 합니다!!" );
}

void CMainFrame::OnUpdateFileSaveProject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

BOOL CMainFrame::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
    SaveBarState(_T("BeastBarState"));
	
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewNavigator()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	BOOL bShow = m_dlgNavigatorMapView.IsWindowVisible( );

	if( bShow )
	{
		m_dlgNavigatorMapView.ShowWindow( SW_HIDE );
	}
	else 
	{
		m_dlgNavigatorMapView.ShowWindow( SW_SHOW );
	}
}

void CMainFrame::OnUpdateViewNavigator(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( m_dlgNavigatorMapView.IsWindowVisible() );
	//RecalcLayout();
}
