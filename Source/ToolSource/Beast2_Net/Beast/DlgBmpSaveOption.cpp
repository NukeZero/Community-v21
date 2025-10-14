// DlgBmpSaveOption.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgBmpSaveOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBmpSaveOption dialog


CDlgBmpSaveOption::CDlgBmpSaveOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBmpSaveOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBmpSaveOption)
	m_bViewRegion = FALSE;
	m_bViewName = FALSE;
	m_bViewSpawn = FALSE;
	m_bHeightAttrb = FALSE;
	m_bWater = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgBmpSaveOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBmpSaveOption)
	DDX_Check(pDX, IDC_VIEW_REGION, m_bViewRegion);
	DDX_Check(pDX, IDC_VIEW_NAME, m_bViewName);
	DDX_Check(pDX, IDC_VIEW_SPAWN, m_bViewSpawn);
	DDX_Check(pDX, IDC_HEIGHT_ATTRB, m_bHeightAttrb);
	DDX_Check(pDX, IDC_WATER, m_bWater);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBmpSaveOption, CDialog)
	//{{AFX_MSG_MAP(CDlgBmpSaveOption)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBmpSaveOption message handlers
