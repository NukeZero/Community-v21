// FileCounterMng.cpp : implementation file
//

#include "stdafx.h"
#include "scriptconvert.h"
#include "FileCounterMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileCounterMng dialog

char *p1 = "Script(\\..)";
char *p2 = "TXT(\\Script)";
char *p3 = "XLS(\\EXCEL)";

char pszPathname[2048] = {0};
char pszOFPathname[2048] = {0};

int g_nOKDiffXLS = 0;
int g_nOKDiffTXT = 0;


int g_nOFDiffXLS = 0;
int g_nOFDiffTXT = 0;



#define __ADVPROCESS

CFileCounterMng::CFileCounterMng(CWnd* pParent /*=NULL*/)
	: CDialog(CFileCounterMng::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileCounterMng)
	m_OK_FullPath = _T("");
	m_11 = _T("");
	m_12 = _T("");
	m_13 = _T("");
	m_OF_FullPath = _T("");
	m_21 = _T("");
	m_22 = _T("");
	m_23 = _T("");
	//}}AFX_DATA_INIT
}


void CFileCounterMng::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCounterMng)
	DDX_Control(pDX, IDC_LIST5, m_OF_XLS);
	DDX_Control(pDX, IDC_LIST6, m_OF_TXT);
	DDX_Control(pDX, IDC_LIST4, m_OF_Script);
	DDX_Control(pDX, IDC_LIST3, m_OK_TXT);
	DDX_Control(pDX, IDC_LIST2, m_OK_XLS);
	DDX_Control(pDX, IDC_LIST1, m_OK_Script);
	DDX_Text(pDX, IDC_EDIT1, m_OK_FullPath);
	DDX_Text(pDX, IDC_EDIT2, m_11);
	DDX_Text(pDX, IDC_EDIT3, m_12);
	DDX_Text(pDX, IDC_EDIT4, m_13);
	DDX_Text(pDX, IDC_EDIT5, m_OF_FullPath);
	DDX_Text(pDX, IDC_EDIT6, m_21);
	DDX_Text(pDX, IDC_EDIT7, m_22);
	DDX_Text(pDX, IDC_EDIT8, m_23);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileCounterMng, CDialog)
	//{{AFX_MSG_MAP(CFileCounterMng)
	ON_BN_CLICKED(IDC_BUTTON1, OnOldKorFileOpen)
	ON_BN_CLICKED(IDC_BUTTON2, OnOKXLS_DifferentButton)
	ON_BN_CLICKED(IDC_BUTTON4, OnOKTXT_DifferentButton)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkList2)
	ON_LBN_DBLCLK(IDC_LIST3, OnDblclkList3)
	ON_BN_CLICKED(IDC_BUTTON5, OnOldFFileOpen)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_LBN_DBLCLK(IDC_LIST6, OnDblclkList6)
	ON_LBN_DBLCLK(IDC_LIST5, OnDblclkList5)
	ON_LBN_DBLCLK(IDC_LIST4, OnDblclkList4)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileCounterMng message handlers

void CFileCounterMng::OnOldKorFileOpen() 
{	
	// TODO: Add your control notification handler code here
	ITEMIDLIST        *pidlBrowse;
    char            pszPathname2[2048] = {0};
	CString			strTemp;
	CString			strTempFileName;
	
    BROWSEINFO BrInfo;
    BrInfo.hwndOwner = GetSafeHwnd();
    BrInfo.pidlRoot = NULL;
	
    memset( &BrInfo, 0, sizeof(BrInfo) );
    BrInfo.pszDisplayName = pszPathname;
    BrInfo.lpszTitle = "Please OldXLS Korea Folder Select";
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	
    // 다이얼로그를 띄우기
    pidlBrowse = ::SHBrowseForFolder(&BrInfo);    
	
    if( pidlBrowse != NULL)
    {
		struct _finddata_t c_file;
		long hFile;
		
		// 패스를 얻어옴
        ::SHGetPathFromIDList(pidlBrowse, pszPathname);   

		BOOL bExit1 = FALSE;
		BOOL bExit2 = FALSE;

		////// 폴더 존재여부 /////////////////////////////////////////////////////////////
		strcpy( pszPathname2, pszPathname );
		if( _access(strcat( pszPathname2, "\\EXCEL"), 0 ) == -1 )
		{
			bExit1 = TRUE;
		}
		strcpy( pszPathname2, pszPathname );
		if( _access(strcat( pszPathname2, "\\SCRIPT"), 0 ) == -1 )
		{
			bExit2 = TRUE;
		}	
		
		if( bExit1 || bExit2 )
		{
			CString str;
			if( bExit1 == FALSE )
				str = "'SCRIPT' Folder Not Found";
			else
			if( bExit2 == FALSE )
				str = "'EXCEL' Folder Not Found";
			else
				str = "'EXCEL' Folder Not Found\n'SCRIPT' Folder Not Found";
				
			MessageBox( str );
				
			return;
		}
		/////////////////////////////////////////////////////////////////////////////////





		strcpy( pszPathname2, pszPathname );

		int nError1 = 0;
		int nError2 = 0;


		// script
		if( (hFile = _findfirst( strcat( pszPathname2, "\\*.txt"), &c_file )) == -1L )
		{
			MessageBox( "Path or File Not Found(*.txt)" );
		}
		else
		{
			m_OK_FullPath = pszPathname;

			m_OK_Script.ResetContent();
			m_OK_XLS.ResetContent();
			m_OK_TXT.ResetContent();

			do 
			{
				m_OK_Script.AddString(c_file.name);
				strTempFileName.Format( "__TEXT__%s", c_file.name );
				strTempFileName.Replace( ".txt", ".xls" );
				m_OK_XLS.AddString( strTempFileName );	
				
				strTempFileName.Format( "__SCRIPT__%s", c_file.name );
				m_OK_TXT.AddString( strTempFileName );	
			}while( _findnext( hFile, &c_file ) == 0 );
			
			_findclose( hFile );

			m_11.Format( "%s (%d)", p1, m_OK_Script.GetCount() );
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		CString strFullPath;
		CString strFileName;

		int nFCount = 0;
		
		for( int i=0; i<m_OK_XLS.GetCount(); i++ )
		{
			m_OK_XLS.GetText( i, strFileName );

			strFullPath.Format("%s\\EXCEL\\%s", pszPathname, strFileName );

			// 파일이 있는지 없는지 검사
			if( _access( strFullPath, 0 ) == -1 )
			{
				m_OK_XLS.DeleteString(i);
				m_OK_XLS.InsertString(i, strFileName + "(*)" );
//				strFileName.Replace( "(*)", "" );
				nError1++;
			}
			else
				nFCount++;
		}
		m_13.Format( "%s (%d)  X(%d)", p3, nFCount, nError1 );
		//////////////////////////////////////////////////////////////////////////////////////////////////////
	
		nFCount = 0;
		
		for( int i=0; i<m_OK_TXT.GetCount(); i++ )
		{
			m_OK_TXT.GetText( i, strFileName );
			
			strFullPath.Format("%s\\SCRIPT\\%s", pszPathname, strFileName );
			
			// 파일이 있는지 없는지 검사
			if( _access( strFullPath, 0 ) == -1 )
			{
				m_OK_TXT.DeleteString(i);
				m_OK_TXT.InsertString(i, strFileName + "(*)" );
//				strFileName.Replace( "(*)", "" );
				nError2++;
			}
			else
				nFCount++;			
		}
		m_12.Format( "%s (%d)  X(%d)", p2, nFCount, nError2 );
		
		UpdateData(FALSE);		

		if( nError1 != 0 || nError2 != 0 )
		{
			CString temp;
			temp.Format( "XLS : %d\nTXT : %d", nError1, nError2 );
			MessageBox( temp, "File Not Found!" );
		}
		else
		{
			MessageBox( "Compare Completed!!!" );
		}

		// 틀린거 바로 찾기 초기화
		g_nOKDiffXLS = 0;
		g_nOKDiffTXT = 0;		
    }
}

BOOL CFileCounterMng::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_11 = p1;
	m_12 = p2;
	m_13 = p3;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileCounterMng::OnOKXLS_DifferentButton() 
{
	int i=g_nOKDiffXLS;
	CString str;
	
	if( m_OK_XLS.GetCurSel() == -1 || m_OK_XLS.GetCurSel() >= m_OK_XLS.GetCount()-1 )
		i = 0;
	else
		i = m_OK_XLS.GetCurSel()+1;
	
	for( i; i<m_OK_XLS.GetCount(); i++ )
	{
		m_OK_XLS.GetText( i, str );

		if( str.Find( "(*)" ) != -1 )
		{
			g_nOKDiffXLS = i;
			m_OK_XLS.SetCurSel(i);
			break;
		}
	}
}

void CFileCounterMng::OnOKTXT_DifferentButton() 
{
	int i=g_nOKDiffTXT;
	CString str;
	
	if( m_OK_TXT.GetCurSel() == -1 || m_OK_TXT.GetCurSel() >= m_OK_TXT.GetCount()-1 )
		i = 0;
	else
		i = m_OK_TXT.GetCurSel()+1;
	
	for( i; i<m_OK_TXT.GetCount(); i++ )
	{
		m_OK_TXT.GetText( i, str );
		
		if( str.Find( "(*)" ) != -1 )
		{
			g_nOKDiffTXT = i;
			m_OK_TXT.SetCurSel(i);
			break;
		}
	}	
}

void CFileCounterMng::OnDblclkList1() 
{
	CString str;
	CString strfullpath;
	m_OK_Script.GetText( m_OK_Script.GetCurSel(), str );

	strfullpath.Format( "%s\\%s", pszPathname, str );

	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}

void CFileCounterMng::OnDblclkList2() 
{
	CString str;
	CString strfullpath;
	m_OK_XLS.GetText( m_OK_XLS.GetCurSel(), str );
	
	strfullpath.Format( "%s\\EXCEL\\%s", pszPathname, str );
	
	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}

void CFileCounterMng::OnDblclkList3() 
{
	CString str;
	CString strfullpath;
	m_OK_TXT.GetText( m_OK_TXT.GetCurSel(), str );

	str.Replace( "(*)", "" );
	
	strfullpath.Format( "%s\\SCRIPT\\%s", pszPathname, str );
	
	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}






void CFileCounterMng::OnOldFFileOpen() 
{
	ITEMIDLIST        *pidlBrowse;
    char            pszPathname2[2048] = {0};
	CString			strTemp;
	CString			strTempFileName;
	
    BROWSEINFO BrInfo;
    BrInfo.hwndOwner = GetSafeHwnd();
    BrInfo.pidlRoot = NULL;
	
    memset( &BrInfo, 0, sizeof(BrInfo) );
    BrInfo.pszDisplayName = pszOFPathname;
    BrInfo.lpszTitle = "Please OldXLS Foreign Folder Select";
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	
    // 다이얼로그를 띄우기
    pidlBrowse = ::SHBrowseForFolder(&BrInfo);    
	
    if( pidlBrowse != NULL)
    {
		struct _finddata_t c_file;
		long hFile;
		
		// 패스를 얻어옴
        ::SHGetPathFromIDList(pidlBrowse, pszOFPathname);   

		BOOL bExit1 = FALSE;
		BOOL bExit2 = FALSE;

		////// 폴더 존재여부 /////////////////////////////////////////////////////////////
		strcpy( pszPathname2, pszOFPathname );
		if( _access(strcat( pszPathname2, "\\EXCEL"), 0 ) == -1 )
		{
			bExit1 = TRUE;
		}
		strcpy( pszPathname2, pszOFPathname );
		if( _access(strcat( pszPathname2, "\\SCRIPT"), 0 ) == -1 )
		{
			bExit2 = TRUE;
		}	
		
		if( bExit1 || bExit2 )
		{
			CString str;
			if( bExit1 == FALSE )
				str = "'SCRIPT' Folder Not Found";
			else
			if( bExit2 == FALSE )
				str = "'EXCEL' Folder Not Found";
			else
				str = "'EXCEL' Folder Not Found\n'SCRIPT' Folder Not Found";
				
			MessageBox( str );
				
			return;
		}
		/////////////////////////////////////////////////////////////////////////////////





		strcpy( pszPathname2, pszOFPathname );

		int nError1 = 0;
		int nError2 = 0;


		// script
		if( (hFile = _findfirst( strcat( pszPathname2, "\\*.txt"), &c_file )) == -1L )
		{
			MessageBox( "Path or File Not Found(*.txt)" );
		}
		else
		{
			m_OF_FullPath = pszOFPathname;

			m_OF_Script.ResetContent();
			m_OF_XLS.ResetContent();
			m_OF_TXT.ResetContent();

			do 
			{
				m_OF_Script.AddString(c_file.name);
				strTempFileName.Format( "__TEXT__%s", c_file.name );
				//strTempFileName.Replace( ".txt", ".xls" );
				m_OF_XLS.AddString( strTempFileName );	
				
				strTempFileName.Format( "__SCRIPT__%s", c_file.name );
				m_OF_TXT.AddString( strTempFileName );	
			}while( _findnext( hFile, &c_file ) == 0 );
			
			_findclose( hFile );

			m_21.Format( "%s (%d)", p1, m_OF_Script.GetCount() );
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		CString strFullPath;
		CString strFileName;

		int nFCount = 0;
		
		for( int i=0; i<m_OF_XLS.GetCount(); i++ )
		{
			m_OF_XLS.GetText( i, strFileName );

			strFullPath.Format("%s\\EXCEL\\%s", pszOFPathname, strFileName );

			// 파일이 있는지 없는지 검사
			if( _access( strFullPath, 0 ) == -1 )
			{
				m_OF_XLS.DeleteString(i);
				m_OF_XLS.InsertString(i, strFileName + "(*)" );
				nError1++;
			}
			else
				nFCount++;
		}
		m_22.Format( "%s (%d)  X(%d)", p3, nFCount, nError1 );
		//////////////////////////////////////////////////////////////////////////////////////////////////////
	
		nFCount = 0;
		
		for( int i=0; i<m_OF_TXT.GetCount(); i++ )
		{
			m_OF_TXT.GetText( i, strFileName );
			
			strFullPath.Format("%s\\SCRIPT\\%s", pszOFPathname, strFileName );
			
			// 파일이 있는지 없는지 검사
			if( _access( strFullPath, 0 ) == -1 )
			{
				m_OF_TXT.DeleteString(i);
				m_OF_TXT.InsertString(i, strFileName + "(*)" );
				nError2++;
			}
			else
				nFCount++;			
		}
		m_23.Format( "%s (%d)  X(%d)", p2, nFCount, nError2 );
		
		UpdateData(FALSE);		

		if( nError1 != 0 || nError2 != 0 )
		{
			CString temp;
			temp.Format( "XLS : %d\nTXT : %d", nError1, nError2 );
			MessageBox( temp, "File Not Found!" );
		}
		else
		{
			MessageBox( "Compare Completed!!!" );
		}

		// 틀린거 바로 찾기 초기화
		g_nOFDiffXLS = 0;
		g_nOFDiffTXT = 0;		
    }	
}

void CFileCounterMng::OnButton6() 
{
	int i=g_nOFDiffXLS;
	CString str;

	if( m_OF_XLS.GetCurSel() == -1 || m_OF_XLS.GetCurSel() >= m_OF_XLS.GetCount()-1 )
		i = 0;
	else
		i = m_OF_XLS.GetCurSel()+1;
	
	for( i; i<m_OF_XLS.GetCount(); i++ )
	{
		m_OF_XLS.GetText( i, str );
		
		if( str.Find( "(*)" ) != -1 )
		{
			g_nOFDiffXLS = i;
			m_OF_XLS.SetCurSel(i);
			break;
		}
	}
}

void CFileCounterMng::OnButton7() 
{
	int i=g_nOFDiffTXT;
	CString str;
	
	if( m_OF_TXT.GetCurSel() == -1 || m_OF_TXT.GetCurSel() >= m_OF_TXT.GetCount()-1 )
		i = 0;
	else
		i = m_OF_TXT.GetCurSel()+1;
	
	for( i; i<m_OF_TXT.GetCount(); i++ )
	{
		m_OF_TXT.GetText( i, str );
		
		if( str.Find( "(*)" ) != -1 )
		{
			g_nOFDiffTXT = i;
			m_OF_TXT.SetCurSel(i);
			break;
		}
	}		
}

void CFileCounterMng::OnDblclkList4() 
{
	CString str;
	CString strfullpath;
	m_OF_Script.GetText( m_OF_Script.GetCurSel(), str );
	
	strfullpath.Format( "%s\\%s", pszOFPathname, str );
	
	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}

void CFileCounterMng::OnDblclkList5() 
{
	CString str;
	CString strfullpath;
	m_OF_XLS.GetText( m_OF_XLS.GetCurSel(), str );
	
	strfullpath.Format( "%s\\EXCEL\\%s", pszOFPathname, str );
	
	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}

void CFileCounterMng::OnDblclkList6() 
{
	CString str;
	CString strfullpath;
	m_OF_TXT.GetText( m_OF_TXT.GetCurSel(), str );
	
	str.Replace( "(*)", "" );
	
	strfullpath.Format( "%s\\SCRIPT\\%s", pszOFPathname, str );
	
	if( ShellExecute(GetSafeHwnd(), "open", strfullpath, NULL, NULL, SW_SHOWNORMAL) == NULL )
	{
		MessageBox( "Path or File Not Found!" );
		return;
	}
}

void CFileCounterMng::OnButton8() 
{
	// TODO: Add your control notification handler code here
	if( m_OK_Script.GetCount() <= 0 )
	{
		MessageBox( "Please FileLoad" );
		return;
	}

	if( m_OF_Script.GetCount() <= 0 )
	{
		MessageBox( "Please FileLoad" );
		return;
	}

	int nOhterCount = 0;
	CString str;
	for( int i=0; i<m_OF_Script.GetCount(); i++ )
	{
		m_OF_Script.GetText( i, str );
	
		if( m_OK_Script.FindString( 0, str ) == -1 )
		{
			m_OF_Script.DeleteString(i);
			m_OF_Script.InsertString( i, str+"(*)" );
			nOhterCount++;
		}
	}

	if( m_OF_Script.GetCount() == m_OK_Script.GetCount() && nOhterCount == 0 )
	{
		MessageBox( "The Same" );
	}
	else
	{
		MessageBox( "Different !!!" );
	}
}
