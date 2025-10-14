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
// CDlgGuidesSetting dialog


CDlgGuidesSetting::CDlgGuidesSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGuidesSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGuidesSetting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgGuidesSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGuidesSetting)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGuidesSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgGuidesSetting)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGuidesSetting message handlers
