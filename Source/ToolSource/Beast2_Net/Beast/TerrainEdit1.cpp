// TerrainEdit1.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "TerrainEdit1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TerrainEdit property page

IMPLEMENT_DYNCREATE(TerrainEdit, CPropertyPage)

TerrainEdit::TerrainEdit() : CPropertyPage(TerrainEdit::IDD)
{
	//{{AFX_DATA_INIT(TerrainEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

TerrainEdit::~TerrainEdit()
{
}

void TerrainEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TerrainEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TerrainEdit, CPropertyPage)
	//{{AFX_MSG_MAP(TerrainEdit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TerrainEdit message handlers
