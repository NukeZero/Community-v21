// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Miranda.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND_EX(IDD_CTRL_PANEL, OnBarCheck) 
	ON_UPDATE_COMMAND_UI(IDD_CTRL_PANEL, OnUpdateControlBarMenu) 
	ON_COMMAND_EX(IDD_DIALOG_ANIMATION, OnBarCheck) 

	ON_COMMAND(ID_BUTTON_ICON_WIREFRAME, OnButtonIconWireframe)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ICON_WIREFRAME, OnUpdateWireframe)
	ON_COMMAND(ID_BUTTON_ICON_RENDERBONE, OnButtonIconRenderbone)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ICON_RENDERBONE, OnUpdateRenderbone)

	ON_COMMAND(ID_BUTTON_ICON_MUTIMODE, OnButtonIconMultiMode )
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ICON_MUTIMODE, OnUpdateMultiMode)

	ON_COMMAND(ID_BUTTON_ICON_FINAL, OnButtonIconFinal )

	ON_COMMAND_EX(IDD_DIALOG_BONELIST, OnBarCheck) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_vRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_dwColor = 0x00808080;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//SetIcon(LoadIcon(AfxGetInstanceHandle(), (LPCSTR)IDI_ICON5), TRUE);
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | /*WS_VISIBLE |*/ CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this, WS_VISIBLE ) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

// 	// 컨트롤 패널 생성&도킹
  	m_CtrlPanel.Create(this,IDD_CTRL_PANEL, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLYBY | CBRS_GRIPPER, IDD_CTRL_PANEL);
  	m_CtrlPanel.EnableDocking(CBRS_ALIGN_RIGHT);
  	DockControlBar(&m_CtrlPanel);

	//감추기
	SwitchVisibility(IDD_CTRL_PANEL);

	//gmpbigsun : ani panel
	m_kAniPanel.Create( this, IDD_DIALOG_ANIMATION, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_FLYBY | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC, IDD_DIALOG_ANIMATION );
 	m_kAniPanel.EnableDocking(CBRS_ALIGN_LEFT);
 	DockControlBar(&m_kAniPanel);

	//bone
	m_kBoneList.Create( this, IDD_DIALOG_BONELIST, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLYBY | CBRS_GRIPPER, IDD_DIALOG_BONELIST );
 	m_kBoneList.EnableDocking(CBRS_ALIGN_RIGHT);
 	DockControlBar(&m_kBoneList);

//	m_kBoneList.Create( IDD_DIALOG_BONELIST, this );
//	m_kEditUnit.Create( IDD_DIALOG_EDITUNIT, this );
	m_kFinal.Create( IDD_DIALOG_FINAL, this );

 	m_FramePanel.Create(this,IDD_FRAME_PANEL,CBRS_BOTTOM,IDD_FRAME_PANEL);
 	m_FramePanel.SetBarStyle(m_FramePanel.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_ALIGN_BOTTOM | CBRS_GRIPPER );
	m_FramePanel.ShowWindow(SW_HIDE);
	
	SendMessage(WM_COMMAND, ID_VIEW_TOOLBAR, 0);
	SendMessage(WM_COMMAND, ID_VIEW_STATUS_BAR, 0);
	
	SetResourcePath();

	CRect rect;
	GetWindowRect( &rect );
	MoveWindow( 0, 0, rect.Width(), rect.Height() );

	return 0;

}


void CMainFrame::SwitchVisibility(int nID)
{

	SendMessage(WM_COMMAND, nID, 0);

}

void CMainFrame::ShowFrame(int nIsShow)
{
	m_FramePanel.ShowWindow(nIsShow);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	
}


/*
CSize CCtrlPanel::CalcDynamicLayout( int nLength, DWORD dwMode )
{

	return( CControlBar::CalcDynamicLayout(nLength, LM_VERTDOCK));
}*/

void CMainFrame::OnButtonIconWireframe() 
{
	// TODO: Add your command handler code here
	g_bWireframe = !g_bWireframe;
}

void CMainFrame::OnUpdateWireframe( CCmdUI* pCmdUI )
{
	if( g_bWireframe )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnButtonIconRenderbone() 
{
	// TODO: Add your command handler code here
	CMirandaView* pView = (CMirandaView*)GetActiveView();
	pView->m_bRenderBone = !pView->m_bRenderBone;
}

void CMainFrame::OnUpdateRenderbone( CCmdUI* pCmdUI )
{
	CMirandaView* pView = (CMirandaView*)GetActiveView();

	if( pView->m_bRenderBone )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnButtonIconMultiMode() 
{
	// TODO: Add your command handler code here
	CMirandaView* pView = (CMirandaView*)GetActiveView();
	g_bMultiMode = !g_bMultiMode;
}

void CMainFrame::OnUpdateMultiMode( CCmdUI* pCmdUI )
{
	if( g_bMultiMode )
		pCmdUI->SetCheck( 1 );
	else 
	{
		pCmdUI->SetCheck( 0 );

		CMirandaView* pView = (CMirandaView*)GetActiveView();
		pView->ClearSfxUnits( );
	}
}

void CMainFrame::OnButtonIconFinal()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bShow = pFrame->m_kFinal.IsWindowVisible( );
	pFrame->m_kFinal.ShowWindow( !bShow );
	pFrame->SetFocus();
}
