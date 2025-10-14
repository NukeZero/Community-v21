// DlgAddDelObject.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgAddDelObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddObject dialog


CDlgAddObject::CDlgAddObject(  CStrArray* pStrArray, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddObject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pStrArray = pStrArray;
}


void CDlgAddObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddObject)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddObject, CDialog)
	//{{AFX_MSG_MAP(CDlgAddObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddObject message handlers

BOOL CDlgAddObject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton* pButton = (CButton*)GetDlgItem( IDOK);
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST ); 

	for( int i = 0; i < (int)( m_pStrArray->size() ); i++ )
	{
		string strName = m_pStrArray->at( i );
		pListBox->AddString( strName.data() );
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgAddObject::OnOK() 
{
	// TODO: Add extra validation here
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST ); 
	int nCount = pListBox->GetSelCount();
	LPINT pSelect = new int[ nCount ];
	pListBox->GetSelItems( nCount, pSelect );
	m_pStrArray->clear();
	CString strGet;
	string strName;
	for( int i = 0; i < nCount; i++ )
	{
		pListBox->GetText( pSelect[ i ], strGet );
		strName = strGet;
		m_pStrArray->push_back( strName.data() );
	}
	delete pSelect;
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDelObject dialog


CDlgDelObject::CDlgDelObject( CStrArray* pStrArray, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDelObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDelObject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pStrArray = pStrArray;
}


void CDlgDelObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDelObject)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDelObject, CDialog)
	//{{AFX_MSG_MAP(CDlgDelObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDelObject message handlers



BOOL CDlgDelObject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST ); 

	for( int i = 0; i < (int)( m_pStrArray->size() ); i++ )
	{
		string strName = m_pStrArray->at( i );
		pListBox->AddString( strName.data() );
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDelObject::OnOK() 
{
	// TODO: Add extra validation here
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST ); 
	int nCount = pListBox->GetSelCount();
	LPINT pSelect = new int[ nCount ];
	pListBox->GetSelItems( nCount, pSelect );
	m_pStrArray->clear();
	CString strGet;
	string strName;
	for( int i = 0; i < nCount; i++ )
	{
		pListBox->GetText( pSelect[ i ], strGet );
		strName = strGet;
		m_pStrArray->push_back( strName.data() );
	}
	delete pSelect;
	CDialog::OnOK();
}
