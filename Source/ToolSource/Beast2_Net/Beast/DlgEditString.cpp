// DlgEditString.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgEditString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditString dialog


CDlgEditString::CDlgEditString(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditString::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditString)
	m_string = _T("");
	//}}AFX_DATA_INIT
}


void CDlgEditString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditString)
	DDX_Text(pDX, IDC_EDIT1, m_string);
	DDV_MaxChars(pDX, m_string, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditString, CDialog)
	//{{AFX_MSG_MAP(CDlgEditString)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditString message handlers
