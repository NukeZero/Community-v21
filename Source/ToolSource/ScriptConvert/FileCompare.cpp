// FileCompare.cpp : implementation file
//

#include "stdafx.h"
#include "scriptconvert.h"
#include "FileCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileCompare dialog


CFileCompare::CFileCompare(CWnd* pParent /*=NULL*/)
	: CDialog(CFileCompare::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileCompare)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFileCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCompare)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileCompare, CDialog)
	//{{AFX_MSG_MAP(CFileCompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileCompare message handlers

void CFileCompare::OnFilecompare() 
{
	// TODO: Add your control notification handler code here
	
}
