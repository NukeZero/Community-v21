// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SFXEditor.h"

#include "MainFrm.h"

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
	ON_EN_CHANGE(IDC_MODELROTX, OnChangeModelrotx)
	ON_EN_CHANGE(IDC_MODELROTY, OnChangeModelroty)
	ON_EN_CHANGE(IDC_MODELROTZ, OnChangeModelrotz)
	ON_EN_CHANGE(IDC_BGCOLOREDIT, OnChangeBgcoloredit)
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
	m_vRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_dwColor = 0x00808080;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
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

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	
	{
		// Initialize dialog bar m_wndInfoDlgBar
		if (!m_wndModelTrans.Create(this, IDD_MODELDIALOGBAR,
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE, NULL))
		{
			TRACE0("Failed to create dialog bar m_wndInfoDlgBar\n");
			return -1;		// fail to create
		}
		
		m_wndModelTrans.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndModelTrans);
		
		m_wndModelTrans.SetWindowText("¸ðµ¨ È¸Àü");
		
		m_wndModelTrans.GetDlgItem(IDC_BGCOLOREDIT)->SetWindowText("128 128 128");
	}
	
	DockControlBarLeftOf( &m_wndModelTrans, &m_wndToolBar );
	
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

void CMainFrame::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
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


void CMainFrame::OnChangeModelrotx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMDIFrameWnd::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	m_wndModelTrans.GetDlgItemText( IDC_MODELROTX, str );
	m_vRot.x = (float)( atof(str.GetBuffer(0)) );
}
void CMainFrame::OnChangeModelroty() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMDIFrameWnd::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	m_wndModelTrans.GetDlgItemText( IDC_MODELROTY, str );
	m_vRot.y = (float)( atof(str.GetBuffer(0)) );
}
void CMainFrame::OnChangeModelrotz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMDIFrameWnd::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	m_wndModelTrans.GetDlgItemText( IDC_MODELROTZ, str );
	m_vRot.z = (float)( atof(str.GetBuffer(0)) );
}

void CMainFrame::OnChangeBgcoloredit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMDIFrameWnd::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	m_wndModelTrans.GetDlgItemText( IDC_BGCOLOREDIT, str );

	int nr, ng, nb;
	sscanf( str, "%d %d %d", &nr, &ng, &nb );

	m_dwColor = D3DCOLOR_XRGB( nr, ng, nb );
}
