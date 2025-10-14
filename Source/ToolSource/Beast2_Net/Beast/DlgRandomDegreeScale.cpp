// DlgRandomDegreeScale.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgRandomDegreeScale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomDegreeScale dialog


CDlgRandomDegreeScale::CDlgRandomDegreeScale(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRandomDegreeScale::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRandomDegreeScale)
	m_nDegree1 = 0;
	m_nDegree2 = 0;
	m_fScale2 = 0.0f;
	m_fScale1 = 0.0f;
	m_bDegree = FALSE;
	m_bScale = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgRandomDegreeScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRandomDegreeScale)
	DDX_Text(pDX, IDC_EDIT_DEGREE1, m_nDegree1);
	DDV_MinMaxInt(pDX, m_nDegree1, -180, 180);
	DDX_Text(pDX, IDC_EDIT_DEGREE2, m_nDegree2);
	DDV_MinMaxInt(pDX, m_nDegree2, -180, 180);
	DDX_Text(pDX, IDC_EDIT_SCALE2, m_fScale2);
	DDV_MinMaxFloat(pDX, m_fScale2, -1.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_SCALE1, m_fScale1);
	DDV_MinMaxFloat(pDX, m_fScale1, -1.f, 1.f);
	DDX_Check(pDX, IDC_CHECK_DEGREE, m_bDegree);
	DDX_Check(pDX, IDC_CHECK_SCALE, m_bScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRandomDegreeScale, CDialog)
	//{{AFX_MSG_MAP(CDlgRandomDegreeScale)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomDegreeScale message handlers
