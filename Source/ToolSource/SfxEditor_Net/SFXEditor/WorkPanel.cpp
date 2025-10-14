// WorkPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "FramePanel.h"
#include "WorkPanel.h"
#include "childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkPanel dialog


CWorkPanel::CWorkPanel(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CWorkPanel)
	//}}AFX_DATA_INIT
	m_pDoc=NULL;
	m_pView=NULL;
}

void CWorkPanel::Init(void)
{
/*
	CListCtrl* plist=(CListCtrl*)GetDlgItem(IDC_LIST1);

	plist->SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	plist->InsertColumn(0,"object",LVCFMT_LEFT,200);
	plist->InsertColumn(1,"aaa",LVCFMT_LEFT,200);
	plist->InsertItem(0,"111");
	plist->SetItem(0,1,LVIF_TEXT,"222",0,0,0,0);
	plist->InsertItem(1,"333");
	plist->SetItem(1,1,LVIF_TEXT,"444",0,0,0,0);
*/

	CTabCtrl* pTab=(CTabCtrl*)GetDlgItem(IDC_WORKTAB);

	TCITEM tabCtrlItem;
	CRect rectTab;

	tabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
	tabCtrlItem.pszText = "Texture";
	tabCtrlItem.lParam = (LPARAM)(&m_TexPanel);
	pTab->InsertItem(0,&tabCtrlItem);

	tabCtrlItem.pszText = "Particle";
	tabCtrlItem.lParam = (LPARAM)(&m_ObjPanel);
	pTab->InsertItem(1,&tabCtrlItem);

	m_TexPanel.Create(IDD_1TEX,pTab->GetParent());
	m_TexPanel.ShowWindow(TRUE);
	m_ObjPanel.Create(IDD_1OBJ,pTab->GetParent());
	m_ObjPanel.ShowWindow(FALSE);
}

void CWorkPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkPanel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkPanel, CDialogBar)
	//{{AFX_MSG_MAP(CWorkPanel)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_WORKTAB, OnSelchangeTab)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkPanel message handlers

void CWorkPanel::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	CTabCtrl* pTab=(CTabCtrl*)GetDlgItem(IDC_WORKTAB);
	if(pTab) {
		RECT rect;
		GetClientRect(&rect);
		pTab->MoveWindow(rect.left+5,rect.top+5,rect.right-10,rect.bottom-10);

		pTab->AdjustRect(FALSE,&rect);
		rect.bottom-=rect.top;
		rect.right-=rect.left;

		if(m_TexPanel.m_hWnd)
			m_TexPanel.MoveWindow(rect.left+5,rect.top+5,rect.right-10,rect.bottom-10);
		if(m_ObjPanel.m_hWnd)
			m_ObjPanel.MoveWindow(rect.left+5,rect.top+5,rect.right-10,rect.bottom-10);
	}
}

int CWorkPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}
/*
void CWorkPanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListCtrl* plist=(CListCtrl*)GetDlgItem(IDC_LIST1);
	if(plist) {
		POSITION pos;
		pos=plist->GetFirstSelectedItemPosition();
		if(pos) {
			int nItem=plist->GetNextSelectedItem(pos);
			CChildFrame* pParent=(CChildFrame*)GetParent();
			pParent->m_FramePanel.m_nTest=nItem;
			pParent->m_FramePanel.Invalidate();
		}
		else {
			CChildFrame* pParent=(CChildFrame*)GetParent();
			pParent->m_FramePanel.m_nTest=-1;
			pParent->m_FramePanel.Invalidate();
		}
	}

	CDialogBar::OnLButtonDown(nFlags, point);
}
*/

void CWorkPanel::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabCtrl* pTab=(CTabCtrl*)GetDlgItem(IDC_WORKTAB);
//	CFramePanel* pFramePanel=&(((CChildFrame*)GetParent())->m_FramePanel);

	switch(pTab->GetCurSel())
	{
		case 0: 
			m_TexPanel.ShowWindow(TRUE);
			m_ObjPanel.ShowWindow(FALSE);
			//m_TexPanel->SetFocus();
//			sprintf(pFramePanel->m_strTest,"aigo");
//			pFramePanel->Invalidate();
			break;
		case 1: 
			m_TexPanel.ShowWindow(FALSE);
			m_ObjPanel.ShowWindow(TRUE);
			m_TexPanel.SetFocus();
//			sprintf(pFramePanel->m_strTest,"manse");
//			pFramePanel->Invalidate();
			break;
	}

	*pResult = 0;
}
