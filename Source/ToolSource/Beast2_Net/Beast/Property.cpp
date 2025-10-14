// Property.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "Property.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPagePropControl property page

IMPLEMENT_DYNCREATE(CPagePropControl, CPropertyPage)

CPagePropControl::CPagePropControl() : CPropertyPage(CPagePropControl::IDD)
{
	//{{AFX_DATA_INIT(CPagePropControl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPagePropControl::~CPagePropControl()
{
}

void CPagePropControl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropControl)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropControl, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropControl)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropControl message handlers
