// DlgGuidesSetting.cpp : implementation file
//

#include "stdafx.h"
#include "daisy.h"
#include "DlgGuidesSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGuideSetting dialog


CDlgGuideSetting::CDlgGuideSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGuideSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGuideSetting)
	m_nWidth = 4;
	m_nHeight = 4;
	m_nLayout = 0;
	//}}AFX_DATA_INIT
}


void CDlgGuideSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGuideSetting)
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDX_Radio(pDX, IDC_LAYOUT, m_nLayout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGuideSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgGuideSetting)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGuideSetting message handlers
