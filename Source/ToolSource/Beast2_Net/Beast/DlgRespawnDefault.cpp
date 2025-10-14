// DlgRespawnDefault.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgRespawnDefault.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRespawnDefault dialog


CDlgRespawnDefault::CDlgRespawnDefault(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRespawnDefault::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRespawnDefault)
	m_nNum = 0;
	m_nSec = 0;
	//}}AFX_DATA_INIT
}


void CDlgRespawnDefault::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRespawnDefault)
	DDX_Text(pDX, IDC_EDIT_NUM, m_nNum);
	DDX_Text(pDX, IDC_EDIT_SEC, m_nSec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRespawnDefault, CDialog)
	//{{AFX_MSG_MAP(CDlgRespawnDefault)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRespawnDefault message handlers
