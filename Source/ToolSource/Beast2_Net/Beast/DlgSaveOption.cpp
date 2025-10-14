// DlgSaveOption.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgSaveOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveOption dialog


CDlgSaveOption::CDlgSaveOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSaveOption)
	m_bDDS = FALSE;
	m_bDYO = FALSE;
	m_bLND = FALSE;
	m_bRGN = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgSaveOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSaveOption)
	DDX_Check(pDX, IDC_DDS, m_bDDS);
	DDX_Check(pDX, IDC_DYO, m_bDYO);
	DDX_Check(pDX, IDC_LND, m_bLND);
	DDX_Check(pDX, IDC_RGN, m_bRGN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSaveOption, CDialog)
	//{{AFX_MSG_MAP(CDlgSaveOption)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveOption message handlers
