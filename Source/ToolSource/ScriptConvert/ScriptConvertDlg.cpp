// ScriptConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptConvert.h"
#include "ScriptConvertDlg.h"
#include "ResdataConvert.h"
#include "TextClientDlg.h"
#include "QuestDlg.h"
#include "CharacterDlg.h"
#include "HelpConvert.h"
#include "TreeHelpDlg.h"
#include "FileCounterMng.h"
//#include <atlcom.h>
#include <atldbcli.h>
#include <atlconv.h>
#include <atldbsch.h>
#include <OLEDBERR.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define		MAX_WCHAR		100000

//#define __SRP_VERSION

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


#define RsITEM(rs,x) rs->Fields->Item[_variant_t(x)]->Value
#define UC (char *)

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
// CScriptConvertDlg dialog

CScriptConvertDlg::CScriptConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptConvertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptConvertDlg)
	m_Count1 = _T("");
	m_Count2 = _T("");
	m_Directory1 = _T("");
	m_Directory2 = _T("");
	m_Directory3 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_vecScript.clear();
}

void CScriptConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptConvertDlg)
	DDX_Control(pDX, IDC_COMBO1, m_ComboControl);
	DDX_Control(pDX, IDC_CONVERT_DONE, m_ConvertList);
	DDX_Control(pDX, IDC_SCRIPT_LIST1, m_ScriptList1);
	DDX_Text(pDX, IDC_STATIC1, m_Count1);
	DDX_Text(pDX, IDC_STATIC2, m_Count2);
	DDX_Text(pDX, IDC_EDIT1, m_Directory1);
	DDX_Text(pDX, IDC_EDIT2, m_Directory2);
	DDX_Text(pDX, IDC_EDIT3, m_Directory3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptConvertDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptConvertDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SCRIPT_LOAD, OnScriptLoad)
	ON_LBN_SELCHANGE(IDC_SCRIPT_LIST1, OnSelchangeScriptList1)
	ON_BN_CLICKED(IDC_TEXT_LOAD, OnTextLoad)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, OnVersionLoad1)
	ON_BN_CLICKED(IDC_BUTTON2, OnVersionLoad2)
	ON_BN_CLICKED(IDC_BUTTON3, OnVersionLoad3)
	ON_BN_CLICKED(IDC_BUTTON4, OnVersionConvert)
	ON_BN_CLICKED(IDC_RES_LOAD1, OnResLoad1)
	ON_BN_CLICKED(IDC_TEXTCLIENT_BUTTON, OnTextclientButton)
	ON_BN_CLICKED(IDC_PROPQUEST_BUTTON2, OnPropquestButton2)
	ON_BN_CLICKED(IDC_CHARACTER_BUTTON, OnCharacterButton)
	ON_BN_CLICKED(IDC_HELP_BUTTON3, OnHelpButton3)
	ON_BN_CLICKED(IDC_TREEHELP_BUTTON2, OnTreehelpButton2)
	ON_BN_CLICKED(IDC_FILECOMPARE, OnFilecompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptConvertDlg message handlers

BOOL CScriptConvertDlg::OnInitDialog()
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_ScriptList1.ResetContent();
	m_ConvertList.ResetContent();

	m_ComboControl.SetCurSel( -1 );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScriptConvertDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScriptConvertDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScriptConvertDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CScriptConvertDlg::OnScriptLoad() 
{
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}

	// TODO: Add your control notification handler code here
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT |OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Script Files (*.txt)|*.txt|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());

	char buf[10000] = {NULL,};
	
    dlg.m_ofn.lpstrFile = buf;
    dlg.m_ofn.nMaxFile  = 10000;
	
	if ( dlg.DoModal() == IDOK )
    {
		m_ScriptList1.ResetContent();
		
		CString str;
		CString FileName;
		CString FullFileName;
		CString DirectoryName;
		CString CountStr;
		
        for( POSITION pos = dlg.GetStartPosition() ; pos != NULL ; )
        {
			m_vecScript.clear();

            FullFileName  = dlg.GetNextPathName(pos).GetBuffer(0);
			GetFileName( FullFileName, &DirectoryName, &FileName );
			
			CScanner s;

			int nString = 0;
			int TextCount = 0;
			int KeyCount = 0;
			int nOldInsert = -1;
			
			vector<CString> String_1;
			vector<CString> String_2;
			
			BOOL bSkip = FALSE;
			int  bSkipCount = 1;
			
			if( s.Load( FullFileName.GetBuffer(0), TRUE ) )
			{
				str = s.m_pBuf;
				s.GetToken();
				
				// #auto, #init without convert
				while( s.tok != FINISHED )
				{
					if( s.Token == "{" )
						nString++;
					
					if( s.Token == "}" )
						nString--;

					if( s.Token == "script" )
					{
						s.GetToken();
						continue;
					}
				
					if( s.Token == "#auto" || s.Token == "#init" )
					{
						bSkip = TRUE;
						s.GetToken();
						continue;
					}
						
					if( bSkip && nString >=2 )
					{
						s.GetToken();
						continue;
					}
					else
						bSkip = FALSE;

						
					// Key
					if( s.tokenType == STRING && nString == 1 )
					{
						CountStr.Format( "__KEY%03d", KeyCount++ );

						String_1.push_back(CountStr);
						String_2.push_back(s.Token);
						
						int npos1 = str.Find( s.m_pProg, ( nOldInsert == -1 ) ? 0 : nOldInsert );
						int npos2 = npos1 - s.Token.GetLength()-1;
						
						str.Delete( npos2, s.Token.GetLength() );
						str.Insert( npos2, CountStr );
						nOldInsert = npos2;
					}
					else
					// String
					if( s.tokenType == STRING )
					{
						CountStr.Format( "__TEXT%03d", TextCount++ );
					
						// Script Change...
						int npos1 = str.Find( s.m_pProg, ( nOldInsert == -1 ) ? 0 : nOldInsert );
						int npos2 = npos1 - s.Token.GetLength()-1;
						
						str.Delete( npos2, s.Token.GetLength() );
						str.Insert( npos2, CountStr );
						nOldInsert = npos2;

						if( s.Token.Find( "'" ) != -1 )
						{
							CString Error;
							Error.Format( "%s \nInsert CHAR '     ", FileName.GetBuffer(0) );
							AfxMessageBox( Error );
							//break;
						}

						String_1.push_back(CountStr);
						String_2.push_back(s.token);
					}
					else
					{
					
					}

					s.GetToken();
				}

				if( nString != 0 )
				{
					CString stst;
					stst.Format( "{의 개수가 틀림\n%s", FileName );
					AfxMessageBox( stst );
				}
				// #auto, #init with convert
				////////////////////////////////////////////////////////////////////
				s.m_pProg = s.m_pBuf;// = str.GetBuffer(0);
				s.GetToken();
				nOldInsert = -1;
				
				while( s.tok != FINISHED )
				{
					if( s.Token == "{" )
						nString++;
					
					if( s.Token == "}" )
						nString--;
					
					if( s.Token == "script" )
					{
						s.GetToken();
						continue;
					}
					
					if( s.Token == "#auto" || s.Token == "#init" )
					{
						String_1.push_back(" ");
						String_2.push_back(" ");

						bSkip = TRUE;
						s.GetToken();
						continue;
					}
					
					if( bSkip && nString >=2 )
					{
						// String
						if( s.tokenType == STRING )
						{
							CountStr.Format( "__TEXT%03d", TextCount++ );
							
							// Script Change...
//							int npos1 = str.Find( s.m_pProg, ( nOldInsert == -1 ) ? 0 : nOldInsert );
        					int npos1 = str.Find( s.Token, ( nOldInsert == -1 ) ? 0 : nOldInsert );
							int npos2 = npos1;// - s.Token.GetLength()-1;
							
							str.Delete( npos2, s.Token.GetLength() );
							str.Insert( npos2, CountStr );
							nOldInsert = npos2;
							
							String_1.push_back(CountStr);
							String_2.push_back(s.Token);
						}
					}
					else
						bSkip = FALSE;

					s.GetToken();
				}					
				////////////////////////////////////////////////////////////////////


				// Save Data Edit..
				for( int i=0; i<String_1.size(); i++ )
				{
					if( String_1[i].Find( "KEY", 0 ) != -1 )
					{
						CString SrcStr = String_2[i];

						for( int j=0; j<String_2.size(); j++ )
						{
							if( i==j )
								continue;
							
							int n = String_2[j].Find(SrcStr);
							if( n != -1 )
							{
								if( String_2[j].GetLength() == SrcStr.GetLength() )
								{
									String_2[j].Delete( n, SrcStr.GetLength() );
									String_2[j].Insert( n, String_1[i] );
								}
								else
								if( String_2[j].Mid(n-1, 1) == "[" )
								{
									if( String_2[j].Mid( n + SrcStr.GetLength(), 1 ) == "]" )
									{
										String_2[j].Delete( n, SrcStr.GetLength() );
										String_2[j].Insert( n, String_1[i] );
									}
								}
							}
						}
					}
				}
				/////////////////////////////////////////////////////////////////////////
				

				
				// Script Save...
				/////////////////////////////////////////////////////////////////////////
				FILE* fp;
				
				CString Name, NameList;
				NameList = Name = "__SCRIPT__"+FileName;
				
				if( MakeDirectory( "Script" ) )
				{
					Name = DirectoryName+"\\Script\\"+Name;
#ifdef __SRP_VERSION
					Name.Replace( ".txt", ".srp" );
#endif //__SRP_VERSION					
					fp = fopen( Name.GetBuffer(0), "wt" );
					fwrite( str.GetBuffer(0), sizeof(TCHAR), str.GetLength(), fp );
					
					fclose(fp);
				}
				/////////////////////////////////////////////////////////////////////////

				
				
				// Excel Save...
				/////////////////////////////////////////////////////////////////////////
				FileName.Replace( ".txt", ".xls" );
				NameList = Name = "__TEXT__"+FileName;
				
				if( MakeDirectory("Excel") )
				{
					m_ScriptList1.AddString( Name );

					Name = DirectoryName+"\\Excel\\"+Name;
					
					CExcelCtrl SS( Name, "sheet1" );
					
					CStringArray	Array;
					
					SS.BeginTransaction();
					
					Array.RemoveAll();
					
					Array.Add( "__DEFINE" );
					Array.Add( "__TEXT" );
					
					SS.AddHeaders( Array );

					CString strTempTemp;

					for( int k = 0; k < String_1.size(); k++ )
					{
						char* p = String_2[k].GetBuffer(0);

						while( *p == ' ' )
							p++;
						
						if( String_1[k].Find( "KEY", 0 ) != -1 )
						{
							Array.RemoveAll();
							Array.Add( " " );
							SS.AddRow(Array);
						}

						Array.RemoveAll();
						Array.Add( String_1[k] );

						strTempTemp.Format( "%s", p );
						Array.Add( strTempTemp );
						
						SS.AddRow(Array);
					}

					SS.Commit();
				}
				/////////////////////////////////////////////////////////////////////////

								
			}
        }	
	}	
	
	m_ScriptList1.UpdateData();
	
	UpdateData(FALSE);
}

BOOL CScriptConvertDlg::MakeDirectory(LPCTSTR lpDirName)
{
	DWORD dwAttr;
	
	// 디렉토리나 파일이 존재하지 않음. 따라서 디렉토리를 생성함
	if((dwAttr = GetFileAttributes(lpDirName)) == (DWORD)-1)
		return CreateDirectory(lpDirName, NULL );
	
	// 해당 파일이 디렉토리이므로 별도로 만들지 않고 리턴
	if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;
	
	// 디렉토리가 아닌 파일로서 이미 존재함. 따라서 에러로 리턴
	return FALSE;
}


void CScriptConvertDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CScriptConvertDlg::OnSelchangeScriptList1() 
{
	// TODO: Add your control notification handler code here
}

char* convertUnicode(LPCTSTR str) // 인자로 받을땐 LPCTSTR이 적당하다고 하던데요.. ^^ 
{
    USES_CONVERSION;
    const WCHAR* wStr = T2CW(str);             // str이 Unicode인지 Ansi 인지 따질 필요없게 T2CW로 변환
    int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, wStr, -1, NULL, 0, NULL, NULL); 
	
	// 길이는 -1로 주어 널NULL 문자도 변환되도록
    char *utf8Str = new char[nUTF8codeSize];
    WideCharToMultiByte(CP_UTF8, 0, wStr, -1, utf8Str, nUTF8codeSize, 0, 0);
    return utf8Str;
}


void CScriptConvertDlg::OnTextLoad() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}
	
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT |OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "XLS Files (*.xls)|*.xls|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());

	char buf[10000] = {NULL,};
	
    dlg.m_ofn.lpstrFile = buf;
    dlg.m_ofn.nMaxFile  = 10000;
	
	if ( dlg.DoModal() == IDOK )
    {
		m_ConvertList.ResetContent();

		CString str;
		CString FileName;
		CString FullFileName;
		CString DirectoryName;

        for( POSITION pos = dlg.GetStartPosition() ; pos != NULL ; )
        {
            FullFileName  = dlg.GetNextPathName(pos).GetBuffer(0);

			GetFileName( FullFileName, &DirectoryName, &FileName );

			if( FileName.Find( "__TEXT__" ) == -1 )
			{
				AfxMessageBox( "텍스트 파일이 아닙니다." );
				return ;
			}

			CScanner s;
			CString CountStr;

			int nString = 0;

			CString strScriptFile;

			int n = DirectoryName.Find( "\\Excel\\" );

			if( n == -1 )
			{
				AfxMessageBox( "원본(스크립트)파일을 찾을수 없습니다.\n경로나 파일명을 확인해주세요." );
				return ;
			}

#ifndef __SRP_VERSION
			FileName.Replace( ".xls", ".txt" );
#else //__SRP_VERSION
			FileName.Replace( ".xls", ".srp" );
#endif //__SRP_VERSION

			strScriptFile = DirectoryName.Left( n+1 );
			strScriptFile += "Script\\";
			strScriptFile += FileName;

			strScriptFile.Replace( "__TEXT__", "__SCRIPT__" );

			vector<CString>    vecScriptList;
			vector<CString>    vecTextList;

			LoadLineString( strScriptFile.GetBuffer(0), vecScriptList );
			
			vector<CString> String_1;
			vector<CString> String_2;

			HRESULT hr = ::CoInitialize(NULL);
			_ConnectionPtr connect;
			hr = connect.CreateInstance(__uuidof(Connection));
			
			try
			{
				CString str1 = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+FullFileName+";Extended Properties=\"Excel 8.0;HDR=YES;\"";
				
				//GetExcelDriver();
				//str1.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s", m_sExcelDriver, FullFileName, FullFileName);
				
				bstr_t strConnectionString = str1;				
				
				connect->Open(strConnectionString, _T(""), _T(""), adOpenUnspecified);
			}
			catch (_com_error&)
			{
			}
			
			_RecordsetPtr set(__uuidof(Recordset));
			
			bstr_t sTableName = "Select * from [sheet1$]"; 
			/**/
			try
			{
				set->PutRefActiveConnection( connect );
				set->Open(sTableName, vtMissing, adOpenForwardOnly, adLockReadOnly, adCmdText);
				
				_variant_t value;


				while ( set->A_EOF == false )
				{
					CString str = _T("");
					CString strValue;
					_variant_t vtFld;
					_variant_t vtIndex;
					
					vtIndex.vt = VT_I2;
					vtIndex.iVal = 0;

					vtFld = set->Fields->GetItem("__DEFINE")->Value;
					switch(vtFld.vt) 
					{
					case VT_R4:
						str = DblToStr(vtFld.fltVal);
						break;
					case VT_R8:
						str = DblToStr(vtFld.dblVal);
						break;
					case VT_BSTR:
						str = vtFld.bstrVal;
						break;
					case VT_I2:
					case VT_UI1:
						str = IntToStr(vtFld.iVal);
						break;
					case VT_INT:
						str = IntToStr(vtFld.intVal);
						break;
					case VT_I4:
						str = LongToStr(vtFld.lVal);
						break;
					case VT_UI4:
						str = ULongToStr(vtFld.ulVal);
						break;
					case VT_DECIMAL:
						{
							//Corrected by Jos?Carlos Mart?ez Gal?
							double val = vtFld.decVal.Lo32;
							val *= (vtFld.decVal.sign == 128)? -1 : 1;
							val /= pow(10., vtFld.decVal.scale); 
							str = DblToStr(val);
						}
						break;
					case VT_DATE:
						{
						}
						break;
					case VT_BOOL:
						str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
						break;
					case VT_EMPTY:
					case VT_NULL:
						str.Empty();
						set->MoveNext();
						continue;
					default:
						str.Empty();
						break;
					}
					strValue = str;
					String_1.push_back(strValue);
					

					vtIndex.iVal = 1;

					vtFld = set->Fields->GetItem("__TEXT")->Value;
					switch(vtFld.vt) 
					{
					case VT_R4:
						str = DblToStr(vtFld.fltVal);
						break;
					case VT_R8:
						str = DblToStr(vtFld.dblVal);
						break;
					case VT_BSTR:
						str = vtFld.bstrVal;
						break;
					case VT_I2:
					case VT_UI1:
						str = IntToStr(vtFld.iVal);
						break;
					case VT_INT:
						str = IntToStr(vtFld.intVal);
						break;
					case VT_I4:
						str = LongToStr(vtFld.lVal);
						break;
					case VT_UI4:
						str = ULongToStr(vtFld.ulVal);
						break;
					case VT_DECIMAL:
						{
							//Corrected by Jos?Carlos Mart?ez Gal?
							double val = vtFld.decVal.Lo32;
							val *= (vtFld.decVal.sign == 128)? -1 : 1;
							val /= pow(10., vtFld.decVal.scale); 
							str = DblToStr(val);
						}
						break;
					case VT_DATE:
						{
						}
						break;
					case VT_BOOL:
						str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
						break;
					case VT_EMPTY:
					case VT_NULL:
						str.Empty();
						
						break;
					default:
						str.Empty();
						break;
					}
					strValue = str;

					String_2.push_back(strValue);
					int bb = strValue.GetLength();

					set->MoveNext();
				}

			}
			catch (_com_error& e)
			{
				_bstr_t bstrSource(e.Source());
				_bstr_t bs =  _bstr_t(" Error: ") + _bstr_t(e.Error()) + _bstr_t(" Msg: ") 
					+ _bstr_t(e.ErrorMessage()) + _bstr_t(" Description: ") 
					+ _bstr_t(e.Description());
			}
			
			
			set->Close();
			connect->Close();
			
			::CoUninitialize();
			
			
			for (int k = 0; k < String_1.size(); k++)
			{
				CString p = String_1[k];
				CString pp = String_2[k];

				if( pp.Find( "'" ) != -1 )
				{
					CString Error;
					Error.Format( "%s \nInsert CHAR ' ", FileName.GetBuffer(0) );
					AfxMessageBox( Error );
					//break;
				}
				
				if( pp.Find( '"' ) != -1 )
				{
					CString Error;
					Error.Format( "%s \nInsert CHAR """""" Break!!!!", FileName.GetBuffer(0) );
					AfxMessageBox( Error );
					break;
				}
				
				for( int j=0; j<vecScriptList.size(); j++ )
				{
					if( String_2[k].Find( "KEY", 0 ) != -1 )
					{
						for( int ll=0; ll<String_1.size(); ll++ )
						{
							/*
							if( String_1[ll] == String_2[k] )
							{
								String_2[k] = String_2[ll];
								break;
							}
							*/
							if( String_1[ll] == " " || String_1[ll] == "" )
								continue;

							if( String_2[k].Find( String_1[ll], 0 ) != -1 )
							{
								String_2[k].Replace(String_1[ll], String_2[ll]);
								//String_2[k] = String_2[ll];
								break;
							}

						}
						
					}
					
					if( String_1[k] != " " )
						vecScriptList[j].Replace( String_1[k], String_2[k] );
				}
			}		
			
			for (int k = 0; k < String_1.size(); k++)
			{
				int nCount = 0;
				char* srcKey = String_1[k].GetBuffer(0);

				for(int h = 0; h < String_1.size(); h++ )
				{
					char* destKey = String_1[h].GetBuffer(0);

					if( _stricmp( srcKey, destKey ) == 0 )
					{
						nCount++;
					}
				}

				if( nCount > 1 )
				{
					CString Error;
					Error.Format( "Duplication Key : %s", srcKey );
					//AfxMessageBox( Error );
					break;
				}
			}				
			
			// 최종 변환된 파일 저장하기....
			n = DirectoryName.Find( "\\Excel\\" );
			
			strScriptFile = DirectoryName.Left( n+1 );
			strScriptFile += FileName;
			strScriptFile.Replace( "__TEXT__", "" );

			FILE* fp;
			fp = fopen( strScriptFile.GetBuffer(0), "wb" );

			BYTE start[] = { 0xFF, 0xFE };
			fwrite(start, 2, 1, fp);
			
			wchar_t wbuf[MAX_WCHAR];
			
			for( int i=0; i<vecScriptList.size(); i++ )
			{
				CString str1 = vecScriptList[i].GetBuffer(0);
				
				if( vecScriptList.size()-1 != i )
				{
					CString strFormat = vecScriptList[i];
					vecScriptList[i].Format( "%s\r\n", strFormat );
				}
				
				ZeroMemory(wbuf, MAX_WCHAR);
				MultiByteToWideChar(GetACP(), 0, (LPCSTR)vecScriptList[i].GetBuffer(0), strlen(vecScriptList[i]), wbuf, MAX_WCHAR);
				fwrite(wbuf, wcslen(wbuf) * 2, 1, fp);

			}


			fclose(fp);

			FileName.Replace( "__TEXT__", "" );

			m_ConvertList.AddString( FileName );
        }	

		CString Message;
		Message.Format( "완료갯수 : %d", m_ConvertList.GetCount() );
		AfxMessageBox( Message );
	}	

	UpdateData();
}

int CScriptConvertDlg::GetFileName( const CString& FullPathName, CString* Path, CString* File )
{
	int nSlash =  FullPathName.ReverseFind( '\\' );

	if( nSlash == -1 )
		return -1;

	*Path = FullPathName.Left( nSlash+1 );
	*File = FullPathName;
	(*File).Delete( 0, nSlash+1 );

	return 1;
}

int CScriptConvertDlg::LoadLineString( char* filename, vector<CString>& vecstring )
{
	int ch;
	FILE* fp;
	LONG  lSeek  = 0;
	DWORD dwText = 0;
	TCHAR* pBuffer = NULL;
	
	fp = fopen( filename, "rt" );
	
	if( fp == NULL )
		return -1;
	
	while( !feof( fp ) )
	{
		ch =_fgettc( fp );
		
		if( ch == '\n' || ch == -1 )
		{
			pBuffer = new TCHAR[dwText+1];
			memset( pBuffer, 0, sizeof(TCHAR) * (dwText+1) );
			
			fseek( fp, lSeek, SEEK_SET );
			fread( pBuffer, sizeof(TCHAR), dwText, fp );
			ch =_fgettc( fp );
			lSeek = ftell( fp );
			dwText = 0;
			
			vecstring.push_back( pBuffer );

			delete [] pBuffer;
		}
		else
		{
			dwText++;
		}
	}
				
    fclose(fp);
	
	return 1;
}




CString CScriptConvertDlg::IntToStr(int nVal)
{
	CString strRet;
	char buff[10];
	
	_itoa(nVal, buff, 10);
	strRet = buff;
	return strRet;
}

CString CScriptConvertDlg::LongToStr(long lVal)
{
	CString strRet;
	char buff[20];
	
	_ltoa(lVal, buff, 10);
	strRet = buff;
	return strRet;
}

CString CScriptConvertDlg::ULongToStr(unsigned long ulVal)
{
	CString strRet;
	char buff[20];
	
	_ultoa(ulVal, buff, 10);
	strRet = buff;
	return strRet;
	
}


CString CScriptConvertDlg::DblToStr(double dblVal, int ndigits)
{
	CString strRet;
	char buff[50];
	
	_gcvt(dblVal, ndigits, buff);
	strRet = buff;
	return strRet;
}

CString CScriptConvertDlg::DblToStr(float fltVal)
{
	CString strRet = _T("");
	char buff[50];
	
	_gcvt(fltVal, 10, buff);
	strRet = buff;
	return strRet;
}

BOOL CScriptConvertDlg::GetFieldValue(_RecordsetPtr* pRecordset, int nIndex, CString& strValue, CString strDateFormat)
{/*
	CString str = _T("");
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
				//Corrected by Jos?Carlos Mart?ez Gal?
				double val = vtFld.decVal.Lo32;
				val *= (vtFld.decVal.sign == 128)? -1 : 1;
				val /= pow(10, vtFld.decVal.scale); 
				str = DblToStr(val);
			}
			break;
		case VT_DATE:
			{
				COleDateTime dt(vtFld);
				
				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);
			}
			break;
		case VT_BOOL:
			str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.Empty();
			break;
		default:
			str.Empty();
			return FALSE;
		}
		strValue = str;
		return TRUE;
	}
	catch(_com_error &e)
	{
	//	dump_com_error(e);
		return FALSE;
	}
//*/
	return TRUE;
}



void CScriptConvertDlg::GetExcelDriver()
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	
	// Get the names of the installed drivers ("odbcinst.h" has to be included )
	if(!SQLGetInstalledDrivers(szBuf,cbBufMax,& cbBufOut))
	{
		m_sExcelDriver = "";
	}
	
	// Search for the driver...
	do
	{
		if( strstr( pszBuf, "Excel" ) != 0 )
		{
			// Found !
			m_sExcelDriver = CString( pszBuf );
			break;
		}
		pszBuf = strchr( pszBuf, '\0' ) + 1;
	}
	while( pszBuf[1] != '\0' );
}

void CScriptConvertDlg::OnSelchangeCombo1() 
{
/*
	{ HANGUL_CHARSET,		949,	"Gulim",		"굴림" },
	{ ANSI_CHARSET,			1252,	"Arial",	"Arial" },
	//	{ ANSI_CHARSET,			1252,	"Arial Black",	"Arial Black" },
	{ SHIFTJIS_CHARSET,		932,	"MS Gothic",	"굃굍 굊긕긘긞긏" },
	{ GB2312_CHARSET,		936,	"SimSun",		"芥竟" },
	{ THAI_CHARSET,			874,	"Microsoft Sans Serif",		"Microsoft Sans Serif" },
	{ CHINESEBIG5_CHARSET,	950,	"MingLiU",		"꾄ⁿ톱" },
*/	
	// TODO: Add your control notification handler code here
	switch( m_ComboControl.GetCurSel() )
	{
	case 0: // kor
		g_codePage = 949;
		break;
	case 1: // eng
		g_codePage = 1252;
		break;
	case 2: // jap
		g_codePage = 932;
		break;
	case 3: // chi
		g_codePage = 936;
		break;
	case 4: // thai
		g_codePage = 874;
		break;
	case 5: // taiwan
		g_codePage = 950;
		break;
	default:
		break;
	}
	
}

BOOL CScriptConvertDlg::OnVersionLoad( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList, BOOL bSpaceSkip )
{
	excel_line.clear();
	vecFileList.clear();

	EXCEL_LINE cexcel_line;
	
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT |OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "XLS Files (*.xls)|*.xls|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	char buf[10000] = {NULL,};
	
    dlg.m_ofn.lpstrFile = buf;
    dlg.m_ofn.nMaxFile  = 10000;
	
	if ( dlg.DoModal() == IDOK )
    {
		int nFileIndex = 0;
		CString FullFileName;
		vector< EXCEL_LINE > emiter;

        for( POSITION pos = dlg.GetStartPosition() ; pos != NULL ; )
        {
            FullFileName  = dlg.GetNextPathName(pos).GetBuffer(0);
			vecFileList.push_back( FullFileName );

			cexcel_line.m_vecsavekey.clear();

			HRESULT hr = ::CoInitialize(NULL);
			
			_ConnectionPtr connect;
			hr = connect.CreateInstance(__uuidof(Connection));
			
			try
			{
				CString str1 = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+FullFileName+";Extended Properties=\"Excel 8.0;HDR=YES;\"";

				GetExcelDriver();
				str1.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s", m_sExcelDriver, FullFileName, FullFileName);
				
				bstr_t strConnectionString = str1;				
				
				connect->Open(strConnectionString, _T(""), _T(""), adOpenUnspecified);
			}
			catch (_com_error&)
			{
			}

			_RecordsetPtr set(__uuidof(Recordset));
			
			bstr_t sTableName = "Select * from [sheet1$]"; 
			try
			{
				set->PutRefActiveConnection( connect );
				set->Open(sTableName, vtMissing, adOpenForwardOnly, adLockReadOnly, adCmdText);
				
				_variant_t value;
				
				excel_line.push_back(emiter);
				
				while ( set->A_EOF == false )
				{
					CString str = _T("");
					CString strValue;
					_variant_t vtFld;
					_variant_t vtIndex;
					
					vtIndex.vt = VT_I2;
					vtIndex.iVal = 0;

					vtFld = set->Fields->GetItem("__DEFINE")->Value;
					switch(vtFld.vt) 
					{
					case VT_R4:
						str = DblToStr(vtFld.fltVal);
						break;
					case VT_R8:
						str = DblToStr(vtFld.dblVal);
						break;
					case VT_BSTR:
						str = vtFld.bstrVal;
						break;
					case VT_I2:
					case VT_UI1:
						str = IntToStr(vtFld.iVal);
						break;
					case VT_INT:
						str = IntToStr(vtFld.intVal);
						break;
					case VT_I4:
						str = LongToStr(vtFld.lVal);
						break;
					case VT_UI4:
						str = ULongToStr(vtFld.ulVal);
						break;
					case VT_DECIMAL:
						{
							//Corrected by Jos?Carlos Mart?ez Gal?
							double val = vtFld.decVal.Lo32;
							val *= (vtFld.decVal.sign == 128)? -1 : 1;
							val /= pow(10., vtFld.decVal.scale); 
							str = DblToStr(val);
						}
						break;
					case VT_DATE:
						{						}
						break;
					case VT_BOOL:
						str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
						break;
					case VT_EMPTY:
					case VT_NULL:
						if( bSpaceSkip )
						{
							str.Empty();
							set->MoveNext();
							continue;
						}
						str = " ";
						break;
					default:
						str.Empty();
						break;
					}
					strValue = str;
					cexcel_line.m_strkey = strValue;
					

					vtIndex.iVal = 1;

					vtFld = set->Fields->GetItem("__TEXT")->Value;
					switch(vtFld.vt) 
					{
					case VT_R4:
						str = DblToStr(vtFld.fltVal);
						break;
					case VT_R8:
						str = DblToStr(vtFld.dblVal);
						break;
					case VT_BSTR:
						str = vtFld.bstrVal;
						break;
					case VT_I2:
					case VT_UI1:
						str = IntToStr(vtFld.iVal);
						break;
					case VT_INT:
						str = IntToStr(vtFld.intVal);
						break;
					case VT_I4:
						str = LongToStr(vtFld.lVal);
						break;
					case VT_UI4:
						str = ULongToStr(vtFld.ulVal);
						break;
					case VT_DECIMAL:
						{
							//Corrected by Jos?Carlos Mart?ez Gal?
							double val = vtFld.decVal.Lo32;
							val *= (vtFld.decVal.sign == 128)? -1 : 1;
							val /= pow(10., vtFld.decVal.scale); 
							str = DblToStr(val);
						}
						break;
					case VT_DATE:
						{
						}
						break;
					case VT_BOOL:
						str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
						break;
					case VT_EMPTY:
					case VT_NULL:
						if( bSpaceSkip && !cexcel_line.m_strkey.GetLength() )
						{
							str.Empty();
							set->MoveNext();
							continue;
						}
						str = " ";
						break;

					default:
						str.Empty();
						break;
					}
					if( bSpaceSkip )
					{
						CString strstr = str;
						strstr.MakeLower();

						if( strstr.Find( ".wav") >= 0 )
						{
							set->MoveNext();
							continue;
						}
					}
					strValue = str;

					cexcel_line.m_nCodePage = 949;
					cexcel_line.m_strstring = strValue;

					set->MoveNext();

					excel_line[nFileIndex].push_back(cexcel_line);
				}
			}
			catch (_com_error& e)
			{
				_bstr_t bstrSource(e.Source());
				_bstr_t bs =  _bstr_t(" Error: ") + _bstr_t(e.Error()) + _bstr_t(" Msg: ") 
					+ _bstr_t(e.ErrorMessage()) + _bstr_t(" Description: ") 
					+ _bstr_t(e.Description());
			}

			int nNum = excel_line[nFileIndex].size();

			nFileIndex++;
			
			set->Close();
			connect->Close();
			
			::CoUninitialize();
		}

		AfxMessageBox( "Load OK!" );

		return TRUE;
	}

	return FALSE;
}

BOOL CScriptConvertDlg::OnVersionLoadTxt( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList )
{
	excel_line.clear();
	vecFileList.clear();

	EXCEL_LINE cexcel_line;
	
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT |OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	char buf[10000] = {NULL,};
	
    dlg.m_ofn.lpstrFile = buf;
    dlg.m_ofn.nMaxFile  = 10000;
	
	if ( dlg.DoModal() == IDOK )
    {
		int nFileIndex = 0;
		CString FullFileName;
		vector< EXCEL_LINE > emiter;

        for( POSITION pos = dlg.GetStartPosition() ; pos != NULL ; )
        {
            FullFileName  = dlg.GetNextPathName(pos).GetBuffer(0);
			vecFileList.push_back( FullFileName );

			cexcel_line.m_vecsavekey.clear();
				
			excel_line.push_back(emiter);

			char p1= '\r';
			char p2= '\n';
			
			CScanner s;
			
			if( s.Load( FullFileName, TRUE ) )
			{
				s.GetToken();
				
				while( s.tok != FINISHED )
				{
					//*
					if( s.Token == "__DEFINE" || s.Token == "__TEXT" )
					{
						/*
						if( *(s.m_pProg+2) == 9 )
						{
							cexcel_line.m_nCodePage = g_codePage;
							cexcel_line.m_strkey = " ";
							cexcel_line.m_strstring = " ";
							
							excel_line[nFileIndex].push_back(cexcel_line);
							s.GetToken();
							continue;
						}
						*/
						s.GetToken();
						continue;
					}
					/**/
					
					
					cexcel_line.m_nCodePage = g_codePage;
					cexcel_line.m_strkey = s.Token;
					s.GetLastFull();

					CString strstr = s.Token;
					strstr.MakeLower();
					if( strstr.Find( ".wav") >= 0 )
					{
						s.GetToken();
						continue;
					}
					cexcel_line.m_strstring = s.Token;

					// 이상하게 유니코드로 바뀐곳에 "가 표함이 되어있다..검사해서 지운다...
					int nFindSp = cexcel_line.m_strstring.Find( '"' );

nLOOP:
					if( nFindSp != -1 )
					{
						cexcel_line.m_strstring.Delete( nFindSp );
						nFindSp = cexcel_line.m_strstring.Find( '"' );

						if( nFindSp != -1 )
							goto nLOOP;
					}

					
					excel_line[nFileIndex].push_back(cexcel_line);

					/*
					if( *(s.m_pProg+2) == 9 )
					{
						cexcel_line.m_nCodePage = g_codePage;
						cexcel_line.m_strkey = " ";
						cexcel_line.m_strstring = " ";
						
						excel_line[nFileIndex].push_back(cexcel_line);
						s.GetToken();
						int nNum = excel_line[nFileIndex].size();
						continue;
					}
					else
						s.GetToken();
					*/
					s.GetToken();
				}
			}

			int nNum = excel_line[nFileIndex].size();
					
			nFileIndex++;
		}

		AfxMessageBox( "Load OK!" );

		return TRUE;
	}

	return FALSE;
}


/*
vector<CString>    m_vecFileList1;
vector<CString>    m_vecFileList2;
vector<CString>    m_vecFileList3;
vector<EXCEL_LINE> m_ExcelLine;
*/
void CScriptConvertDlg::OnVersionLoad1() 
{
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}

	if( OnVersionLoad(m_ExcelLine1, m_ExcelFile1, FALSE ) )
	{		
		char buff[256] = { 0 };
		sprintf( buff, "File Count = %d", m_ExcelFile1.size() );
		m_Directory1 = buff;
		
		UpdateData(FALSE);
	}
}

void CScriptConvertDlg::OnVersionLoad2() 
{
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}

	if( OnVersionLoad(m_ExcelLine2, m_ExcelFile2, TRUE ) )
	{
		char buff[256] = { 0 };
		sprintf( buff, "File Count = %d", m_ExcelFile2.size() );
		m_Directory2 = buff;
		
		UpdateData(FALSE);
	}
}

void CScriptConvertDlg::OnVersionLoad3() 
{
//	OnVersionLoad(m_ExcelLine3, m_ExcelFile3 );
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}
	
	if( OnVersionLoadTxt(m_ExcelLine3, m_ExcelFile3 ) )
	{
		char buff[256] = { 0 };
		sprintf( buff, "File Count = %d", m_ExcelFile3.size() );
		m_Directory3 = buff;
		
		UpdateData(FALSE);
	}
}

void CScriptConvertDlg::OnVersionConvert() 
{
	if( m_ComboControl.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Language" );
		return;
	}
	
	if( m_ExcelLine1.size() <= 0 || m_ExcelLine2.size() <= 0 || m_ExcelLine3.size() <= 0 )
	{
		AfxMessageBox( "File Not Found" );
		return;
	}

	ITEMIDLIST        *pidlBrowse;
    char            pszPathname[1024] = {0};
	
    BROWSEINFO BrInfo;
    BrInfo.hwndOwner = GetSafeHwnd();
    BrInfo.pidlRoot = NULL;
	
    memset( &BrInfo, 0, sizeof(BrInfo) );
    BrInfo.pszDisplayName = pszPathname;
    BrInfo.lpszTitle = "OutPut 디렉토리를 선택하세요";
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	
    // 다이얼로그를 띄우기
    pidlBrowse = ::SHBrowseForFolder(&BrInfo);    
	
    if( pidlBrowse != NULL)
    {
        // 패스를 얻어옴
        ::SHGetPathFromIDList(pidlBrowse, pszPathname);    
    }

	// 모든 키 삭제
	/*
	DeleteKey(m_ExcelLine1, m_ExcelFile1);
	DeleteKey(m_ExcelLine2, m_ExcelFile2);
	DeleteKey(m_ExcelLine3, m_ExcelFile3);
	*/
	DeleteKeyThai(m_ExcelLine1, m_ExcelFile1);
	DeleteKeyThai(m_ExcelLine2, m_ExcelFile2);
	DeleteKeyThai(m_ExcelLine3, m_ExcelFile3);
	//

	CString FullFileNameSrc;
	CString DirectoryNameSrc;
	CString FileNameSrc;

	CString FullFileNameDest;
	CString DirectoryNameDest;
	CString FileNameDest;

	int i, j, k, l;

	for( i=0; i<m_ExcelFile2.size(); i++ )
	{
gPASS:
		if( i>=m_ExcelFile2.size() )
			continue;

		FullFileNameSrc = m_ExcelFile2[i];
		GetFileName( FullFileNameSrc, &DirectoryNameSrc, &FileNameSrc );

		for( j=0; j<m_ExcelFile1.size(); j++ )
		{
			FullFileNameDest = m_ExcelFile1[j];
			GetFileName( FullFileNameDest, &DirectoryNameDest, &FileNameDest );
////////////////////////////////////////////////////////////////////////////////////////////////////
			int pp=0;
			for( ; pp<m_ExcelFile3.size(); pp++ )
			{
				CString FileName1;
				CString FileName2;
				CString FileName3 = m_ExcelFile3[pp];
				GetFileName( FileName3, &FileName1, &FileName2 );

				CString ttemp1 = FileNameDest;
				CString ttemp2 = FileName2;
				ttemp1.Delete(ttemp1.GetLength()-4, 4);
				ttemp2.Delete(ttemp2.GetLength()-4, 4);
				
				if( ttemp1 == ttemp2 )
					break;
			}			
			
			if( pp == m_ExcelFile3.size() )
				continue;
/////////////////////////////////////////////////////////////////////////////////////////////////////
			if( FileNameSrc == FileNameDest )
			{
				EXCEL_LINE* pexcel_line1;
				EXCEL_LINE* pexcel_line2;
				
				for( k=0; k<m_ExcelLine2[i].size(); k++ )
				{
					pexcel_line1 = &(m_ExcelLine2[i][k]);

					if( pexcel_line1->m_strstring == " " || pexcel_line1->m_strstring == "" )
						continue;
						
					EXCEL_LINE  temp;
					// 신버전 엑셀과 비교
					for( l=0; l<m_ExcelLine1[j].size(); l++ )
					{
						pexcel_line2 = &(m_ExcelLine1[j][l]);

						if( pexcel_line2->m_strstring == " " || pexcel_line2->m_strstring == "" )
							continue;

						if( pexcel_line2->m_nCodePage != 949 )
							continue;

						CString temp1, temp2;

						temp1 = ResotreKey( *pexcel_line1, FileNameSrc );
						temp2 = ResotreKey( *pexcel_line2, FileNameSrc );

						if( temp1 == temp2 )
						{
							//temp = m_ExcelLine3[i][k];
							//if( k >= m_ExcelLine3[i].size() )
							//	continue;

							int nLen1 =  m_ExcelLine2.size();
							int nLen2 =  m_ExcelLine3.size();
							
							int nLen3 =  m_ExcelLine2[i].size();
							int nLen4 =  m_ExcelLine3[i].size();

							if( nLen3 != nLen4 )
							{
								CString str;
								str.Format( "구버전 한글엑셀(%d)과 구버전 외국엑셀(%d)의 라인수가 맞지않음 : %s", nLen3, nLen4, FileNameDest );
								AfxMessageBox( str );
								i++;
								goto gPASS;
							}

							pexcel_line2->m_nCodePage = g_codePage;
							pexcel_line2->m_strstring = m_ExcelLine3[i][k].m_strstring;

							m_ExcelLine1[j][l].m_vecsavekey = m_ExcelLine3[i][k].m_vecsavekey;
/*
					#ifdef _DEBUG
							EXCEL_KEY key111;							
							EXCEL_KEY key222;		
					#endif //_DEBUG
							for( int pp=0; pp<m_ExcelLine3[i][k].m_vecsavekey.size(); pp++ )
							{
					#ifdef _DEBUG
								key111 = m_ExcelLine3[i][k].m_vecsavekey[pp];							
								key222 = m_ExcelLine1[j][l].m_vecsavekey[pp];							
								
					#endif //_DEBUG
								m_ExcelLine1[j][l].m_vecsavekey[pp] = m_ExcelLine3[i][k].m_vecsavekey[pp];
							}
*/
							

							/*

							for( int llll = 0; llll<m_ExcelLine3[i].size(); llll++ )
							{
								if( m_ExcelLine3[i][llll].m_strkey == pexcel_line2->m_strkey )
								{
									pexcel_line2->m_nCodePage = g_codePage;
									pexcel_line2->m_strstring = m_ExcelLine3[i][llll].m_strstring;
									break;
								}
							}
							*/
						}
					}
				}

				break;
			}
		}
	}
		
	//키 삽입
	ResotreKeyThai(m_ExcelLine1);
//	ResotreKey(m_ExcelLine1);
		
	SaveConvertdataTXT( m_ExcelLine1, m_ExcelFile1, pszPathname );


	AfxMessageBox( "Process OK!" );
	
}

// Excel Save...
/////////////////////////////////////////////////////////////////////////
void CScriptConvertDlg::SaveConvertdata( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList )
{
	CString FullFileName;
	CString DirectoryName;
	CString FileName;
	
	for( int i=0; i<excel_line.size(); i++ )
	{
		FullFileName = vecFileList[i];

		GetFileName( FullFileName, &DirectoryName, &FileName );
		
//		if( MakeDirectory("Output") )
		{
			//FileName = "1.XLS";
			//FileName = DirectoryName+"\\Output\\"+FileName;
			FileName = "\\"+FileName;
			
			CExcelCtrl SS( FileName, "sheet1" );
			
			CStringArray	Array;
			
			SS.BeginTransaction();
			
			Array.RemoveAll();
			
			Array.Add( "__DEFINE" );
			Array.Add( "__TEXT" );
			
			SS.AddHeaders( Array );
			
			for( int j=0; j<excel_line[i].size(); j++ )
			{
				char* strstr = excel_line[i][j].m_strstring.GetBuffer(0);
				char* strkey = excel_line[i][j].m_strkey.GetBuffer(0);
				
				Array.RemoveAll();

				Array.Add( strkey );
				Array.Add( strstr );
				
				SS.AddRow(Array);
			}
			SS.Commit();
		}
	}
}

void CScriptConvertDlg::SaveConvertdataTXT( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList, CString Path )
{
	CString FullFileName;
	CString DirectoryName;
	CString FileName;
	
	for( int i=0; i<excel_line.size(); i++ )
	{
		FullFileName = vecFileList[i];
		
		GetFileName( FullFileName, &DirectoryName, &FileName );

		FileName = Path+"\\"+FileName;

		FILE* fp;
		
		fp = fopen( FileName, "wb" );
		
		BYTE start[] = { 0xFF, 0xFE };
		fwrite(start, 2, 1, fp);
		
		wchar_t wbuf[MAX_WCHAR];

		CString     strstr;
		strstr.Format( "%s\t%s\r\n", "__DEFINE", "__TEXT" );
		ZeroMemory(wbuf, MAX_WCHAR);
		MultiByteToWideChar(949, 0, (LPCSTR)strstr.GetBuffer(0), strlen(strstr), wbuf, MAX_WCHAR);
		fwrite(wbuf, wcslen(wbuf) * 2, 1, fp);
		
		for( int j=0; j<excel_line[i].size(); j++ )
		{
			EXCEL_LINE  temp = excel_line[i][j];
			
			strstr.Format( "%s\t%s\r\n", temp.m_strkey, temp.m_strstring );
			
			ZeroMemory(wbuf, MAX_WCHAR);
			
			MultiByteToWideChar(temp.m_nCodePage, 0, (LPCSTR)strstr.GetBuffer(0), strlen(strstr), wbuf, MAX_WCHAR);
			
			fwrite(wbuf, wcslen(wbuf) * 2, 1, fp);
		}

		fclose(fp);
		
	}
}


CString CScriptConvertDlg::ResotreKey( EXCEL_LINE excel_line, CString filename )
{
	string test1;
	EXCEL_KEY key;

	int i=0;
	for( ; i<excel_line.m_vecsavekey.size(); i++ )
	{
		key = excel_line.m_vecsavekey[i];
				
		CString* temp = &(excel_line.m_strstring);
				
		test1 = temp->GetBuffer(0);
				
		int nIndex = 0;

		nIndex = test1.find( "ABB" );
		
		if( nIndex == -1 )
		{
			CString errorstr;
			errorstr.Format( "nIndex == -1\nFileName : %s", filename );
			AfxMessageBox( errorstr );
			continue;
		}
		
		test1.erase( test1.begin()+nIndex, test1.begin()+nIndex+3 );
		test1.insert( nIndex, key.m_strkey );    
		
		excel_line.m_strstring = test1.c_str();
	}

	return  (i == 0) ? excel_line.m_strstring : test1.c_str();
}

void CScriptConvertDlg::ResotreKeyThai( vector< vector< EXCEL_LINE > >& excel_line )
{
	EXCEL_KEY key;
	
	for( int i=0; i<excel_line.size(); i++ )
	{
		for( int j=0; j<excel_line[i].size(); j++ )
		{
			EXCEL_LINE exstring = excel_line[i][j];

			for( int k=0; k<excel_line[i][j].m_vecsavekey.size(); k++ )
			{
				key = excel_line[i][j].m_vecsavekey[k];
				
				CString* temp = &(excel_line[i][j].m_strstring);

				string test1 = temp->GetBuffer(0);

				int nIndex = 0;
				nIndex = test1.find( "ABB" );

				if( nIndex == -1 )
				{
					CString Error;
					Error.Format( "CScriptConvertDlg::ResotreKeyThai()ABB Not Found : %s\nString : %s", m_ExcelFile1[i], exstring.m_strstring );
					//이상하다
					//AfxMessageBox( Error );
					continue;
				}

				test1.erase( test1.begin()+nIndex, test1.begin()+nIndex+3 );
				test1.insert( nIndex, key.m_strkey );      
				
//				excel_line[i][j].m_strstring.Delete( nIndex, 3 );
//				excel_line[i][j].m_strstring.Insert( nIndex, key.m_strkey );

				excel_line[i][j].m_strstring = test1.c_str();
				
			}
		}
	}
}

void CScriptConvertDlg::DeleteKey( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList )
{
	EXCEL_KEY key;

	int nOffset  = 0;
	int nOffsetS = 0;
	int nOffsetE = 0;
	
	for( int i=0; i<excel_line.size(); i++ )
	{
		for( int j=0; j<excel_line[i].size(); j++ )
		{
			CString* temp = &(excel_line[i][j].m_strstring);

			nOffset = excel_line[i][j].m_strstring.Find( "[", 0 );

			if( nOffset == -1 )
			{
				excel_line[i][j].m_vecsavekey.clear();
			}
			else
			{
				do
				{
					nOffsetS = excel_line[i][j].m_strstring.Find( "[", nOffset );
					nOffsetE = excel_line[i][j].m_strstring.Find( "]", nOffsetS );

					if( nOffsetS != -1 )
					{
						if( nOffsetE == -1 )
						{
							CString str;
							str.Format( "FILE:%s\n%s\nDeleteKey Error", vecFileList[i], excel_line[i][j].m_strstring );
							//AfxMessageBox( str );
							//return;
						}
						else
						{
							key.m_nOffset = nOffsetS;
							key.m_strkey  = excel_line[i][j].m_strstring.Mid( nOffsetS, (nOffsetE+1) - nOffsetS );
							
							excel_line[i][j].m_strstring.Delete( nOffsetS, (nOffsetE+1) - nOffsetS );
							excel_line[i][j].m_strstring.Insert( nOffsetS, "ABB" );
							//excel_line[i][j].m_strstring.Replace( key.m_strkey, "ABB" );

							excel_line[i][j].m_vecsavekey.push_back(key);
						}

						nOffset = nOffsetS;
					}
				}while( nOffsetS != -1 );
			}
		}
	}
}

void CScriptConvertDlg::DeleteKeyThai( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList )
{
	EXCEL_KEY key;
	
	int nOffset  = 0;
	int nOffsetS = 0;
	int nOffsetE = 0;
	
	for( int i=0; i<excel_line.size(); i++ )
	{
		for( int j=0; j<excel_line[i].size(); j++ )
		{
			CString* temp = &(excel_line[i][j].m_strstring);

			/*
			string test1 = temp->GetBuffer(0);
			string test2 = "[";
			string::iterator get = find_first_of(test1.begin(), test1.end(), test2.begin(), test2.end());

			TRACE (get);
			*/

			string test1 = temp->GetBuffer(0);
			string test2 = "[";
			
			nOffset = test1.find( '[' );

			if( nOffset == -1 )
			{
				excel_line[i][j].m_vecsavekey.clear();
			}
			else
			{
				do
				{
					nOffsetS = test1.find( '[', nOffset );
					nOffsetE = test1.find( ']', nOffsetS );
//					nOffsetS = excel_line[i][j].m_strstring.Find( "[", nOffset );
//					nOffsetE = excel_line[i][j].m_strstring.Find( "]", nOffsetS );
					
					if( nOffsetS != -1 )
					{
						if( nOffsetE == -1 )
						{
							CString str;
							str.Format( "FILE:%s\n%s\nDeleteKey Error -]-가 없다", vecFileList[i], excel_line[i][j].m_strstring );
							//AfxMessageBox( str );
							return;
						}
						else
						{
							if( nOffsetS < 0 )
								int a = 0;

							key.m_nOffset = nOffsetS;
							
							test2 = test1.substr( nOffsetS, (nOffsetE+1) - nOffsetS );

							key.m_strkey = test2.c_str();

//							key.m_strkey  = excel_line[i][j].m_strstring.Mid( nOffsetS, (nOffsetE+1) - nOffsetS );
//							key.m_strkey  = excel_line[i][j].m_strstring.Mid( nOffsetS, (nOffsetE+1) - nOffsetS );
							
							test1.erase( test1.begin()+nOffsetS, test1.begin()+nOffsetE+1 );
							test1.insert( nOffsetS, "ABB" );      
							//excel_line[i][j].m_strstring.Delete( nOffsetS, (nOffsetE+1) - nOffsetS );
							//excel_line[i][j].m_strstring.Insert( nOffsetS, "ABB" );
							excel_line[i][j].m_strstring = test1.c_str();

							excel_line[i][j].m_vecsavekey.push_back(key);
						}
						
						nOffset = nOffsetS;
					}

					
				}while( nOffsetS != -1 );
			}
		}
	}
}

void CScriptConvertDlg::OnResLoad1() 
{
	// TODO: Add your control notification handler code here
	CResdataConvert cDlg(this);

	cDlg.DoModal();
}

void CScriptConvertDlg::OnTextclientButton() 
{
	// TODO: Add your control notification handler code here
	CTextClientDlg cDlg(this);
	
	cDlg.DoModal();
	
}

void CScriptConvertDlg::OnPropquestButton2() 
{
	// TODO: Add your control notification handler code here
	CQuestDlg cDlg(this);
	
	cDlg.DoModal();
	
}

void CScriptConvertDlg::OnCharacterButton() 
{
	// TODO: Add your control notification handler code here
	CCharacterDlg cDlg(this);
	
	cDlg.DoModal();
	
}

void CScriptConvertDlg::OnHelpButton3() 
{
	// TODO: Add your control notification handler code here
	CHelpConvert cDlg(this);
	
	cDlg.DoModal();
	
}

void CScriptConvertDlg::OnTreehelpButton2() 
{
	// TODO: Add your control notification handler code here
	CTreeHelpDlg cDlg(this);
	
	cDlg.DoModal();
	
}

void CScriptConvertDlg::OnFilecompare() 
{
	CFileCounterMng cDlg(this);
	
	cDlg.DoModal();
}
