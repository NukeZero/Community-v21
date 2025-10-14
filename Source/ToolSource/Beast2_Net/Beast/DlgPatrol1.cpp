// DlgPatrol1.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgPatrol1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol dialog


CDlgPatrol::CDlgPatrol(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPatrol::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPatrol)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPatrol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPatrol)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPatrol, CDialog)
	//{{AFX_MSG_MAP(CDlgPatrol)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol message handlers
