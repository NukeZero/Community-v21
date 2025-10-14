// ObjPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "ObjPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjPanel dialog


CObjPanel::CObjPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CObjPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc=NULL;
	m_pView=NULL;
}


void CObjPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjPanel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjPanel, CDialog)
	//{{AFX_MSG_MAP(CObjPanel)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjPanel message handlers

void CObjPanel::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_OBJLIST);
	if(pList) {
		RECT rect;
		GetClientRect(&rect);
		pList->MoveWindow(rect.left+5,rect.top+5,rect.right-10,rect.bottom-10);
	}
}

BOOL CObjPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_OBJLIST);
	if(pList) {
		pList->InsertColumn(0,"object",LVCFMT_LEFT,300);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
