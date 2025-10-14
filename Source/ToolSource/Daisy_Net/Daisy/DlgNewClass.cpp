// DlgNewClass.cpp : implementation file
//

#include "stdafx.h"
#include "daisy.h"
#include "DlgNewClass.h"
#include "BrowseDirectory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewClass dialog


CDlgNewClass::CDlgNewClass(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pWndDialog = NULL;
	m_bEditFileName = FALSE;
}


void CDlgNewClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewClass)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewClass, CDialog)
	//{{AFX_MSG_MAP(CDlgNewClass)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewClass message handlers

void CDlgNewClass::Init( CWndDialog* pWndDialog )
{
	m_pWndDialog = pWndDialog;

	//CComboBox
}

BOOL CDlgNewClass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( m_pWndDialog->GetWndId() );
	CStringArray strArray;
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_WND_ID );
	CScript::GetFindDefToArray( "APP_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		pCombo->AddString( strArray.GetAt( i ) );
	}	
	pCombo->SelectString( 0, lpWndApplet->strDefine );
	CEdit* pEditName = (CEdit*)GetDlgItem( IDC_EDIT_NAME );
	pEditName->SetWindowText( "CWnd" );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewClass::OnChangeEditName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	if( m_bEditFileName == FALSE )
	{
		CEdit* pEditName = (CEdit*)GetDlgItem( IDC_EDIT_NAME );
		CEdit* pEditFileName = (CEdit*)GetDlgItem( IDC_EDIT_FILE_NAME );
		CString string;
		pEditName->GetWindowText( string );
		if( string.IsEmpty() == FALSE )
		{
			if( string[0] == 'C' )
				string.Delete( 0 );
			if( string.IsEmpty() == FALSE )
			{
				string += ".cpp";
				pEditFileName->SetWindowText( string );
			}
			else
				pEditFileName->SetWindowText( "" );
		}
		else
			pEditFileName->SetWindowText( "" );

		pEditName->GetWindowText( m_strClassName );
		pEditFileName->GetWindowText( m_strFileName );
		pEditFileName->GetWindowText( m_strHeaderName );
		if( m_strHeaderName.IsEmpty() == FALSE )
		{
			m_strHeaderName = m_strHeaderName.Left( m_strHeaderName.GetLength() - 4 );
			m_strHeaderName += ".h";
		}
		/*
		CString m_strWndId;
		m_strClassName = ;
		CString m_strPath;
		CString m_strFileName;
		*/
	}

}

void CDlgNewClass::OnOK() 
{
	// TODO: Add extra validation here

	LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( m_pWndDialog->GetWndId() );
	m_strWndId = lpWndApplet->strDefine;
	/*
	CStringArray strArray;
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_WND_ID );
	*/
	CDialog::OnOK();
}
void CDlgNewClass::OnChange() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CDlgChangeFiles dlgChangeFiles;
	dlgChangeFiles.m_strHeader = m_strHeaderName;
	dlgChangeFiles.m_strImplementation = m_strFileName;
	dlgChangeFiles.m_bEditFileName = m_bEditFileName;
	if( dlgChangeFiles.DoModal() == TRUE )
	{
		m_bEditFileName = dlgChangeFiles.m_bEditFileName;
		m_strFileName = dlgChangeFiles.m_strImplementation;
		m_strHeaderName = dlgChangeFiles.m_strHeader;
		UpdateData( FALSE );
		CEdit* pEditFileName = (CEdit*)GetDlgItem( IDC_EDIT_FILE_NAME );
		pEditFileName->SetWindowText( m_strFileName );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFiles dialog


CDlgChangeFiles::CDlgChangeFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChangeFiles)
	m_strDirectory = _T("");
	m_strHeader = _T("");
	m_strImplementation = _T("");
	//}}AFX_DATA_INIT
	m_bEditFileName = FALSE;
}


void CDlgChangeFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeFiles)
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_strDirectory);
	DDX_Text(pDX, IDC_EDIT_HEADER, m_strHeader);
	DDX_Text(pDX, IDC_EDIT_IMPLEMENTATION, m_strImplementation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChangeFiles, CDialog)
	//{{AFX_MSG_MAP(CDlgChangeFiles)
	ON_BN_CLICKED(IDC_DIRECTORY, OnDirectory)
	ON_BN_CLICKED(IDC_HEADER, OnHeader)
	ON_BN_CLICKED(IDC_IMPLEMENTATION, OnImplementation)
	ON_EN_CHANGE(IDC_EDIT_HEADER, OnChangeEditHeader)
	ON_EN_CHANGE(IDC_EDIT_IMPLEMENTATION, OnChangeEditImplementation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFiles message handlers


void CDlgChangeFiles::OnDirectory() 
{
	// TODO: Add your control notification handler code here
	CWizBrowseDirForFiles browseDlg( "*.*" );
	CString strFolder = browseDlg.Do();
	if(strFolder.IsEmpty() == TRUE)
		return;
	m_strDirectory = strFolder;
	UpdateData( FALSE );
	//SetCurrentDirectory(strFolder);

/*
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Windows or os/2 file(*.bmp)|*.bmp|PaintBrush file(*.tga)|*.tga||");
	dlg.SetWindowText( "Browse Files" );
	if( dlg.DoModal() != IDOK ) 
		return;	
		*/
}

void CDlgChangeFiles::OnHeader() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, ".h", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Header file(*.h)|*.h||");
	//dlg.SetWindowText( "Browse Files" );
	if( dlg.DoModal() != IDOK ) 
		return;	
	m_strHeader = dlg.GetFileName();
	UpdateData( FALSE );
}

void CDlgChangeFiles::OnImplementation() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, "*.cpp", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Source file(*.cpp)|*.cpp||");
	//dlg.SetWindowText( "Browse Files" );
	if( dlg.DoModal() != IDOK ) 
		return;	
	m_strImplementation = dlg.GetFileName();
	UpdateData( FALSE );
}

void CDlgChangeFiles::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData( TRUE );

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_strSourcePath = m_strDirectory;
	
	CDialog::OnOK();
}

BOOL CDlgChangeFiles::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_strDirectory = pMainFrame->m_strSourcePath;

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeFiles::OnChangeEditHeader() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_bEditFileName = TRUE;
}

void CDlgChangeFiles::OnChangeEditImplementation() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_bEditFileName = TRUE;	
}
