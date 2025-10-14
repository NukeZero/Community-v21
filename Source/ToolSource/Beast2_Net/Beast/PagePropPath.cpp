// PagePropPath.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "PagePropPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPagePropPath property page

IMPLEMENT_DYNCREATE(CPagePropPath, CPropertyPage)

CPagePropPath::CPagePropPath() : CPropertyPage(CPagePropPath::IDD)
{
	//{{AFX_DATA_INIT(CPagePropPath)
	m_Delay = _T("");
	//}}AFX_DATA_INIT
}

CPagePropPath::~CPagePropPath()
{
}

void CPagePropPath::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropPath)
	DDX_Control(pDX, IDC_MOTION, m_Motion);
	DDX_Text(pDX, IDC_DELAY, m_Delay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropPath, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropPath)
	ON_BN_CLICKED(IDC_BUTTON1, OnOK)
	ON_BN_CLICKED(IDC_BUTTON2, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropPath message handlers

void CPagePropPath::Init(CObj *pObj)
{
	m_pObj = pObj;
}


BOOL CPagePropPath::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if( m_pObj == NULL )
		return 0;

	m_Motion.SetItemData( m_Motion.AddString("MTI_WALK"), MTI_WALK );
	m_Motion.SetItemData( m_Motion.AddString("MTI_RUN"), MTI_RUN );
	
	CPathObj* pPathObj = (CPathObj*)m_pObj;
	
	m_Delay.Format( "%d", pPathObj->m_dwDelay );

	SetItemData( pPathObj->m_dwMotionState );

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePropPath::OnOK() 
{
	UpdateData( TRUE );

	CPathObj* pPathObj = (CPathObj*)m_pObj;
	pPathObj->m_dwDelay = atoi( m_Delay );
	pPathObj->m_dwMotionState = m_Motion.GetItemData( m_Motion.GetCurSel() );
	
}

void CPagePropPath::OnCancel() 
{
}

void CPagePropPath::SetItemData(DWORD dwItemData)
{
	DWORD dwData;
	for( int i=0; i<m_Motion.GetCount(); i++ )
	{
		dwData = m_Motion.GetItemData( i );

		if( dwData == dwItemData )
		{
			m_Motion.SetCurSel( i );
			return;
		}
	}

	m_Motion.SetCurSel( 0 );

}
