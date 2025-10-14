// DlgSetTime.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgSetTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetTime dialog


CDlgSetTime::CDlgSetTime(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetTime)
	m_nTime = 0;
	//}}AFX_DATA_INIT
}


void CDlgSetTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetTime)
	DDX_Text(pDX, IDC_EDIT1, m_nTime);
	DDV_MinMaxInt(pDX, m_nTime, 0, 23);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetTime, CDialog)
	//{{AFX_MSG_MAP(CDlgSetTime)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetTime message handlers
