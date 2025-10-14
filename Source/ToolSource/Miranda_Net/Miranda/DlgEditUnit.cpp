// DlgEditUnit.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "DlgEditUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditUnit dialog


CDlgEditUnit::CDlgEditUnit( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditUnit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditUnit)
	m_stcBoneName = _T("");
	m_stcSfxName = _T("");
	m_iSelectedCase = -1;
	m_bLoop = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgEditUnit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditUnit)
	DDX_Control(pDX, IDC_COMBO_CASE, m_cmbCase);
	DDX_Text(pDX, IDC_EDIT_BONENAME, m_stcBoneName);
	DDX_Text(pDX, IDC_EDIT_SFXNAME, m_stcSfxName);
	DDX_CBIndex(pDX, IDC_COMBO_CASE, m_iSelectedCase);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditUnit, CDialog)
	//{{AFX_MSG_MAP(CDlgEditUnit)
	ON_CBN_SELCHANGE(IDC_COMBO_CASE, OnSelchangeComboCase)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditUnit message handlers

LONG CDlgEditUnit::OnInitDialog( UINT wParam, LONG lParam ) 
{
 	CDialog::OnInitDialog();

#ifdef __VS6
	if(!HandleInitDialog(wParam, lParam) || !UpdateData(FALSE))
	{
		return FALSE;
	}
#else
	//gmpbigsun: for VS2003
	HandleInitDialog( wParam, lParam );

	if( !UpdateData( FALSE ) )
	{
		TRACE0("Warning:UpdateData failed during dialog init. \n" );
		return FALSE;
	}
#endif
	
	// TODO: Add extra initialization here
	
	// 이펙트,사운드가 플레이되는 시점 리스트 
	m_cmbCase.InsertString( 0, "create" );
	m_cmbCase.InsertString( 1, "battle" );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgEditUnit::OnSelchangeComboCase() 
{
	// TODO: Add your control notification handler code here
	int iSel = m_cmbCase.GetCurSel( );
	m_cmbCase.GetLBText( iSel, m_strSelectedComboList );
}
