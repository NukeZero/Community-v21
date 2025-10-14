// DlgRevival.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgRevival.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRevival dialog


CDlgRevival::CDlgRevival(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRevival::IDD, pParent)
{
	m_pWorld;
	//{{AFX_DATA_INIT(CDlgRevival)
	m_strWorld = _T("");
	m_strKey = _T("");
	//}}AFX_DATA_INIT
}


void CDlgRevival::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRevival)
	DDX_CBString(pDX, IDC_COMBO1, m_strWorld);
	DDX_Text(pDX, IDC_EDIT1, m_strKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRevival, CDialog)
	//{{AFX_MSG_MAP(CDlgRevival)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRevival message handlers

BOOL CDlgRevival::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	CComboBox* pComboWorld = (CComboBox*) GetDlgItem( IDC_COMBO1 );
	
	CStringArray strArray;
	CScript::GetFindIdToArray( "WI_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pComboWorld->AddString( strArray.GetAt( i ) );
	
	m_strWorld = CScript::GetFindIdStr( "WI_", m_pWorld->m_dwIdWorldRevival );
	m_strKey = m_pWorld->m_szKeyRevival;
	
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRevival::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );
	
	m_pWorld->m_dwIdWorldRevival = CScript::GetDefineNum( m_strWorld );
	strcpy( m_pWorld->m_szKeyRevival, m_strKey );
	CDialog::OnOK();
}
