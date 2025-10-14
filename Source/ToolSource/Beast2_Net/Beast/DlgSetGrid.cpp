// DlgSetGrid.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgSetGrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGrid dialog


CDlgSetGrid::CDlgSetGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetGrid)
	m_fGridX = 1.0f;
	m_fGridY = 1.0f;
	m_fGridZ = 1.0f;
	//}}AFX_DATA_INIT
}


void CDlgSetGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetGrid)
	DDX_Text(pDX, IDC_EDIT_GRID_X, m_fGridX);
	DDX_Text(pDX, IDC_EDIT_GRID_Y, m_fGridY);
	DDX_Text(pDX, IDC_EDIT_GRID_Z, m_fGridZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetGrid, CDialog)
	//{{AFX_MSG_MAP(CDlgSetGrid)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGrid message handlers
