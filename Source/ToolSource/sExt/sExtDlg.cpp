// sExtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sExt.h"
#include "sExtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSExtDlg dialog

CSExtDlg::CSExtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSExtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSExtDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSExtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSExtDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSExtDlg, CDialog)
	//{{AFX_MSG_MAP(CSExtDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSExtDlg message handlers

BOOL CSExtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CComboBox* pCombo	= (CComboBox*)GetDlgItem( IDC_COMBO_LANG );
	pCombo->SetCurSel( 0 );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSExtDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSExtDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CSExtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSExtDlg::OnButtonRun() 
{
// #1
	FromtextClient( _T( "textClient.inc" ) );
	FromtextClient( _T( "resData.inc" ) );
	FromtextClient( _T( "World//WdMadrigal//wdMadrigal.rgn" ) );
	FromtextClient( _T( "Client//faq.inc" ) );
	FromtextClient( _T( "Client//GameGuard.inc" ) );
	FromtextClient( _T( "Client//Guide.inc" ) );
	FromtextClient( _T( "Client//help.inc" ) );
	FromtextClient( _T( "Client//InstantHelp.inc" ) );
	FromtextClient( _T( "Client//tip.inc" ) );
	FromtextClient( _T( "Client//treeHelp.inc" ) );
//FromtextClient( _T( "Client//PatchClient.inc" ) );
// #2
	FrompropItemEtc( _T( "etc.inc" ) );
	FrompropItemEtc( _T( "propItemEtc.inc" ) );
	FrompropItemEtc( _T( "textEmotion.txt" ) );
// #3
	FrompropQuest( _T( "propQuest.inc" ) );
	FrompropQuest( _T( "propQuest-Scenario.inc" ) );
	FrompropQuest( _T( "propQuest-DungeonandPK.inc" ) );
	FrompropQuest( _T( "propQuest-RequestBox.inc" ) );
	FrompropQuest( _T( "character.inc" ) );
	FrompropQuest( _T( "character-etc.inc" ) );
	FrompropQuest( _T( "character-school.inc" ) );
// #4
	FrompropItem(  _T( "propKarma.txt" ) );
	FrompropItem(  _T( "propCtrl.txt" ) );
	FrompropItem(  _T( "propMover.txt" ) );
	FrompropItem(  _T( "propMotion.txt" ), 1 );
	FrompropItem(  _T( "propItem.txt" ), 2 );
	FrompropItem(  _T( "propSkill.txt" ), 2 );
	FrompropItem(  _T( "propTroupeSkill.txt" ), 2 );
// #?
//propGuildQuest.inc
//propPartyQuest.inc
}

BOOL CSExtDlg::FromtextClient( LPCTSTR sFile )
{
	UpdateData( TRUE );
	CComboBox* pCombo	= (CComboBox*)GetDlgItem( IDC_COMBO_LANG );
	int nLang	= pCombo->GetCurSel();
	if( nLang < 0 )
		return FALSE;
	CFile f;
	if( f.Open( sFile, CFile::modeRead ) )
	{
		DWORD dwCount	= f.GetLength();
		TCHAR* lpBuffer	= new TCHAR[dwCount/sizeof(TCHAR)+1];
		f.Read( lpBuffer, dwCount );
		memset( &lpBuffer[dwCount/sizeof(TCHAR)], 0, sizeof(TCHAR) );
		CString str	= lpBuffer;
		delete[] lpBuffer;

		CString strFile		= sFile;

		int i1	= str.Find( _T( "LANG_KOR" ) );
		int nCount	= 0;
		CString strTable	= str.Left( 1 );	// unicode text
		while( i1 >= 0 && i1 < str.GetLength() )
		{
			CString s;
			int i2	= str.Find( _T( "LANG_ENG" ), i1 );
			if( nLang == 0 )
				s	= str.Mid( i1+8, i2-(i1+8) );
			int i3	= str.Find( _T( "LANG_JAP" ), i2 );
			if( nLang == 1 )
				s	= str.Mid( i2+8, i3-(i2+8) );
			int i4	= str.Find( _T( "LANG_CHI" ), i3 );
			if( nLang == 2 )
				s	= str.Mid( i3+8, i4-(i3+8) );
			int i5	= str.Find( _T( "LANG_THA" ), i4 );
			if( nLang == 3 )
				s	= str.Mid( i4+8, i5-(i4+8) );
			int i6	= str.Find( _T( "LANG_TWN" ), i5 );
			if( nLang == 4 )
				s	= str.Mid( i5+8, i6-(i5+8) );
			int i7	= str.Find( _T( "\r\n" ), i6 );
			if( i7 == -1 )
				i7	= str.GetLength();
			if( nLang == 5 )
				s	= str.Mid( i6+8, i7-(i6+8) );
//			TRACE( _T( "%s\n" ), s );
			str.Delete( i1, i7 - i1 );
			CString strKey;
			strKey.Format( _T( "IDS_%s_%s_%06d" ), strFile.Left( strFile.GetLength() - 4 ), strFile.Right( 3 ), nCount++ );
			strKey.MakeUpper();
			strKey.Replace( _T( "//" ), _T( "_" ) );
			str.Insert( i1, (LPCTSTR)strKey );

			strTable	+= strKey;
			strTable	+= _T( "\t" );
			s.TrimLeft();
			s.TrimRight();
			strTable	+= s;
			strTable	+= _T( "\r\n" );
			i1	= str.Find( _T( "LANG_KOR" ), i1 );
		}
		f.Close();
		CFile f1;
		CString strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_cod" ) );
		if( f1.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f1.Write( (LPCTSTR)str, str.GetLength()*sizeof(TCHAR) );	
		}
		strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_str" ) );

		CFile f2;
		if( f2.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f2.Write( (LPCTSTR)strTable, strTable.GetLength()*sizeof(TCHAR) );
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSExtDlg::FrompropItemEtc( LPCTSTR sFile )
{
	UpdateData( TRUE );
	CComboBox* pCombo	= (CComboBox*)GetDlgItem( IDC_COMBO_LANG );
	int nLang	= pCombo->GetCurSel();
	if( nLang < 0 )
		return FALSE;
	CFile f;
	if( f.Open( sFile, CFile::modeRead ) )
	{
		DWORD dwCount	= f.GetLength();
		TCHAR* lpBuffer	= new TCHAR[dwCount/sizeof(TCHAR)+1];
		f.Read( lpBuffer, dwCount );
		memset( &lpBuffer[dwCount/sizeof(TCHAR)], 0, sizeof(TCHAR) );
		CString str	= lpBuffer;
		delete[] lpBuffer;

		CString strFile		= sFile;

		int i1	= str.Find( _T( "\"" ) );
		int nCount	= 0;
		CString strTable	= str.Left( 1 );
		while( i1 >= 0 )
		{
			CString s;
			int i2	= str.Find( _T( "\"" ), i1+1 );
			if( nLang == 0 )
				s	= str.Mid( i1+1, i2 - ( i1+1 ) );
			int i3	= str.Find( _T( "\"" ), i2+1 );
			int i4	= str.Find( _T( "\"" ), i3+1 );
			if( nLang == 1 )
				s	= str.Mid( i3+1, i4 - (i3+1) );
			int i5	= str.Find( _T( "\"" ), i4+1 );
			int i6	= str.Find( _T( "\"" ), i5+1 );
			if( nLang == 2 )
				s	= str.Mid( i5+1, i6 - (i5+1) );
			int i7	= str.Find( _T( "\"" ), i6+1 );
			int i8	= str.Find( _T( "\"" ), i7+1 );
			if( nLang == 3 )
				s	= str.Mid( i7+1, i8-(i7+1) );
			int i9	= str.Find( _T( "\"" ), i8+1 );
			int i10	= str.Find( _T( "\"" ), i9+1 );
			if( nLang == 4 )
				s	= str.Mid( i9+1, i10-(i9+1) );
			int i11	= str.Find( _T( "\"" ), i10+1 );
			int i12	= str.Find( _T( "\"" ), i11+1 );
			if( nLang == 5 )
				s	= str.Mid( i11+1, i12-(i11+1) );
			TRACE( _T( "%s\n" ), s );
			str.Delete( i1, i12 - i1 + 1 );
			CString strKey;
			strKey.Format( _T( "IDS_%s_%s_%06d" ), strFile.Left( strFile.GetLength() - 4 ), strFile.Right( 3 ), nCount++ );
			strKey.MakeUpper();
			strKey.Replace( _T( "//" ), _T( "_" ) );
			str.Insert( i1, (LPCTSTR)strKey );

			strTable	+= strKey;
			strTable	+= _T( "\t" );
			s.TrimLeft();
			s.TrimRight();
			strTable	+= s;
			strTable	+= _T( "\r\n" );

			i1	= str.Find( _T( "\"" ), i1 );
		}
		f.Close();
		CFile f1;
		CString strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_cod" ) );
		if( f1.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f1.Write( (LPCTSTR)str, str.GetLength()*sizeof(TCHAR) );	
		}
		strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_str" ) );

		CFile f2;
		if( f2.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f2.Write( (LPCTSTR)strTable, strTable.GetLength()*sizeof(TCHAR) );
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSExtDlg::FrompropQuest( LPCTSTR sFile )
{
	UpdateData( TRUE );
	CComboBox* pCombo	= (CComboBox*)GetDlgItem( IDC_COMBO_LANG );
	int nLang	= pCombo->GetCurSel();
	if( nLang < 0 )
		return FALSE;
	CFile f;
	if( f.Open( sFile, CFile::modeRead ) )
	{
		DWORD dwCount	= f.GetLength();
		TCHAR* lpBuffer	= new TCHAR[dwCount/sizeof(TCHAR)+1];
		f.Read( lpBuffer, dwCount );
		memset( &lpBuffer[dwCount/sizeof(TCHAR)], 0, sizeof(TCHAR) );
		CString str	= lpBuffer;
		delete[] lpBuffer;

		CString strFile		= sFile;

		CString strTable	= str.Left( 1 );	// unicode text
		int nCount	= 0;

		int i1	= str.Find( _T( "LANG_KOR" ) );
		
		while( i1 >= 0 && i1 < str.GetLength() )
		{
			CString s;
			int i2	= str.Find( _T( "\"" ), i1 );
			int i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 0 )
				s	= str.Mid( i2+1, i3-(i2+1) );
			i2	= str.Find( _T( "\"" ), i3+1 );
			i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 1 )
				s	= str.Mid( i2+1, i3-(i2+1) );
			i2	= str.Find( _T( "\"" ), i3+1 );
			i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 2 )
				s	= str.Mid( i2+1, i3-(i2+1) );
			i2	= str.Find( _T( "\"" ), i3+1 );
			i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 3 )
				s	= str.Mid( i2+1, i3-(i2+1) );
			i2	= str.Find( _T( "\"" ), i3+1 );
			i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 4 )
				s	= str.Mid( i2+1, i3-(i2+1) );
			
			i2	= str.Find( _T( "\"" ), i3+1 );
			int e	= str.Find( _T( "LANG" ), i3+1 );
			if( i2 < e && e != -1 )
			{
				CString strErr	= _T( "error: " ) + strFile + _T( " " ) + s;
				AfxMessageBox( strErr );
			}

			i3	= str.Find( _T( "\"" ), i2+1 );
			if( nLang == 5 )
				s	= str.Mid( i2+1, i3-(i2+1) );
//			TRACE( _T( "%s\n" ), s );
			i2	= str.Find( _T( "\r\n" ), i3+1 );
			if( i2 == -1 )
				i2	= str.GetLength();
			str.Delete( i1, i2 - i1 );
			CString strKey;
			strKey.Format( _T( "IDS_%s_%s_%06d" ), strFile.Left( strFile.GetLength() - 4 ), strFile.Right( 3 ), nCount++ );
			strKey.MakeUpper();
			strKey.Replace( _T( "//" ), _T( "_" ) );
			str.Insert( i1, (LPCTSTR)strKey );

			strTable	+= strKey;
			strTable	+= _T( "\t" );
			s.TrimLeft();
			s.TrimRight();
			strTable	+= s;
			strTable	+= _T( "\r\n" );
			i1	= str.Find( _T( "LANG_KOR" ), i1 );
		}
		f.Close();
		CFile f1;
		CString strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_cod" ) );
		if( f1.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f1.Write( (LPCTSTR)str, str.GetLength()*sizeof(TCHAR) );	
		}
		strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_str" ) );

		CFile f2;
		if( f2.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f2.Write( (LPCTSTR)strTable, strTable.GetLength()*sizeof(TCHAR) );
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSExtDlg::FrompropItem( LPCTSTR sFile, int nType )
{
	UpdateData( TRUE );
	CComboBox* pCombo	= (CComboBox*)GetDlgItem( IDC_COMBO_LANG );
	int nLang	= pCombo->GetCurSel();
	if( nLang < 0 )
		return FALSE;
	CFile f;
	if( f.Open( sFile, CFile::modeRead ) )
	{
		DWORD dwCount	= f.GetLength();
		TCHAR* lpBuffer	= new TCHAR[dwCount/sizeof(TCHAR)+1];
		f.Read( lpBuffer, dwCount );
		memset( &lpBuffer[dwCount/sizeof(TCHAR)], 0, sizeof(TCHAR) );
		CString str	= lpBuffer;
		delete[] lpBuffer;

		CString strFile		= sFile;

		int i1	= str.Find( _T( "\"\"\"" ) );

		int iComment	= 0;
		while( 1 )
		{
			iComment	= str.Find( _T( "//" ), iComment+1 );
			if( iComment != -1 && iComment < i1 )
			{
				int nCrLf	= str.Find( _T( "\r\n" ), iComment );
				i1	= str.Find( _T( "\"\"\"" ), nCrLf );
			}
			else
				break;
		}
		iComment	= 0;
		while( 1 )
		{
			iComment	= str.Find( _T( "/*" ), iComment+1 );
			if( iComment != -1 && iComment < i1 )
			{
				int nClose	= str.Find( _T( "*/" ), iComment );
				i1	= str.Find( _T( "\"\"\"" ), nClose );
			}
			else
				break;
		}

		if( nType == 1 )
		{
			for( int i = 0; i < 2; i++ )
				i1	= str.Find( _T( "\"\"\"" ), i1+3 );
		}

		int nCount	= 0;
		CString strTable	= str.Left( 1 );
		while( i1 >= 0 )
		{
			CString s;
			int i2	= str.Find( _T( "\"\"\"" ), i1+3 );
			if( nLang == 0 )
				s	= str.Mid( i1+3, i2 - ( i1+3 ) );
			int i3	= str.Find( _T( "\"\"\"" ), i2+3 );
			int i4	= str.Find( _T( "\"\"\"" ), i3+3 );
			if( nLang == 1 )
				s	= str.Mid( i3+3, i4 - (i3+3) );
			int i5	= str.Find( _T( "\"\"\"" ), i4+3 );
			int i6	= str.Find( _T( "\"\"\"" ), i5+3 );
			if( nLang == 2 )
				s	= str.Mid( i5+3, i6 - (i5+3) );
			int i7	= str.Find( _T( "\"\"\"" ), i6+3 );
			int i8	= str.Find( _T( "\"\"\"" ), i7+3 );
			if( nLang == 3 )
				s	= str.Mid( i7+3, i8-(i7+3) );
			int i9	= str.Find( _T( "\"\"\"" ), i8+3 );
			int i10	= str.Find( _T( "\"\"\"" ), i9+3 );
			if( nLang == 4 )
				s	= str.Mid( i9+3, i10-(i9+3) );
			int i11	= str.Find( _T( "\"\"\"" ), i10+3 );
			int i12	= str.Find( _T( "\"\"\"" ), i11+3 );
			if( nLang == 5 )
				s	= str.Mid( i11+3, i12-(i11+3) );
			TRACE( _T( "%s\n" ), s );
			str.Delete( i1, i12 - i1 + 3 );
			CString strKey;
			strKey.Format( _T( "IDS_%s_%s_%06d" ), strFile.Left( strFile.GetLength() - 4 ), strFile.Right( 3 ), nCount++ );
			strKey.MakeUpper();
			strKey.Replace( _T( "//" ), _T( "_" ) );
			str.Insert( i1, (LPCTSTR)strKey );

			strTable	+= strKey;
			strTable	+= _T( "\t" );
			s.TrimLeft();
			s.TrimRight();
			strTable	+= s;
			strTable	+= _T( "\r\n" );

			int n	= i1;
			i1	= str.Find( _T( "\"\"\"" ), i1 );

			// skip comments
			iComment	= n-1;
			while( 1 )
			{
				iComment	= str.Find( _T( "//" ), iComment+1 );
				if( iComment != -1 && iComment < i1 )
				{
					int nCrLf	= str.Find( _T( "\r\n" ), iComment );
					i1	= str.Find( _T( "\"\"\"" ), nCrLf );
				}
				else
					break;
			}
			iComment	= n-1;
			while( 1 )
			{
				iComment	= str.Find( _T( "/*" ), iComment+1 );
				if( iComment != -1 && iComment < i1 )
				{
					int nClose	= str.Find( _T( "*/" ), iComment );
					i1	= str.Find( _T( "\"\"\"" ), nClose );
				}
				else
					break;
			}

			if( nType == 1 && ( nCount % 2 ) == 0 )
			{
				for( int i = 0; i < 2; i++ )
				{
					if( i1 >= 0 )
						i1	= str.Find( _T( "\"\"\"" ), i1+3 );
				}
			}
			else if( nType == 2 && ( nCount % 2 ) == 1 )
			{
				for( int i = 0; i < 4; i++ )
					i1	= str.Find( _T( "\"\"\"" ), i1+3 );
			}
		}
		f.Close();
		CFile f1;
		CString strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_cod" ) );
		if( f1.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f1.Write( (LPCTSTR)str, str.GetLength()*sizeof(TCHAR) );	
		}
		strResult	= strFile;
		strResult.Insert( strFile.GetLength() - 4, _T( "_str" ) );

		CFile f2;
		if( f2.Open( strResult, CFile::modeCreate | CFile::modeWrite ) )
		{
			f2.Write( (LPCTSTR)strTable, strTable.GetLength()*sizeof(TCHAR) );
		}
		return TRUE;
	}
	return FALSE;
}
