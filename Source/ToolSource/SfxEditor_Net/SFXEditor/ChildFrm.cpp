// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "SFXEditor.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_MAXIMIZE;
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_WorkPanel.Create(this,IDD_WORKPANEL,CBRS_RIGHT,IDD_WORKPANEL);
	m_WorkPanel.SetBarStyle(m_WorkPanel.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_ALIGN_RIGHT | CBRS_GRIPPER );

	m_WorkPanel.Init();

	m_FramePanel.Create(this,IDD_FRAMEPANEL,CBRS_BOTTOM,IDD_FRAMEPANEL);
	m_FramePanel.SetBarStyle(m_FramePanel.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_ALIGN_BOTTOM | CBRS_GRIPPER );

	m_WorkPanel.m_TexPanel.m_pDoc=m_WorkPanel.m_pDoc=m_FramePanel.m_pDoc=m_pDoc;
	m_WorkPanel.m_TexPanel.m_pView=m_WorkPanel.m_pView=m_FramePanel.m_pView=m_pView;

	return 0;
}

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_SHOWMAXIMIZED;
	
	CMDIChildWnd::ActivateFrame(nCmdShow);
}
