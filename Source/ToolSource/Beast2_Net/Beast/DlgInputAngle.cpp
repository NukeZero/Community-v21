// DlgInputAngle.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgInputAngle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputAngle dialog


CDlgInputAngle::CDlgInputAngle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputAngle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputAngle)
	m_fAngle = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgInputAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputAngle)
	DDX_Text(pDX, IDC_EDIT1, m_fAngle);
	DDV_MinMaxFloat(pDX, m_fAngle, -360.f, 360.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputAngle, CDialog)
	//{{AFX_MSG_MAP(CDlgInputAngle)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputAngle message handlers
/////////////////////////////////////////////////////////////////////////////
// CDlgInputPosition dialog


CDlgInputPosition::CDlgInputPosition(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputPosition::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputPosition)
	m_fPositionX = 0.0f;
	m_fPositionY = 0.0f;
	m_fPositionZ = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgInputPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputPosition)
	DDX_Text(pDX, IDC_EDIT1, m_fPositionX);
	DDX_Text(pDX, IDC_EDIT2, m_fPositionZ);
	DDX_Text(pDX, IDC_EDIT3, m_fPositionY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputPosition, CDialog)
	//{{AFX_MSG_MAP(CDlgInputPosition)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputPosition message handlers
// DlgInputAngle.cpp : 구현 파일입니다.
//

// CDlgInputScale 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputScale, CDialog)
CDlgInputScale::CDlgInputScale(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputScale::IDD, pParent)
	, m_fEditScale(0)
	, m_fEditOldScale(0)
{
}

CDlgInputScale::~CDlgInputScale()
{
}

void CDlgInputScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fEditScale);
	DDX_Text(pDX, IDC_EDIT2, m_fEditOldScale);
}


BEGIN_MESSAGE_MAP(CDlgInputScale, CDialog)
END_MESSAGE_MAP()


// CDlgInputScale 메시지 처리기입니다.
