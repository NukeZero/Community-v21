// ModelTrans.cpp : implementation file
//

#include "stdafx.h"
#include "sfxeditor.h"
#include "ModelTrans.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelTrans dialog


CModelTrans::CModelTrans(CWnd* pParent /*=NULL*/)
	: CDialog(CModelTrans::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelTrans)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModelTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelTrans)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelTrans, CDialog)
	//{{AFX_MSG_MAP(CModelTrans)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelTrans message handlers
