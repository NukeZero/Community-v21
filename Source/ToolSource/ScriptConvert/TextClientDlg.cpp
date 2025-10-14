// TextClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lang.h"
#include "ScriptConvert.h"
#include "TextClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextClientDlg dialog


CTextClientDlg::CTextClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextClientDlg)
	m_SrcPath = _T("");
	m_DestPath = _T("");
	m_bSkip = FALSE;
	//}}AFX_DATA_INIT
}


void CTextClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextClientDlg)
	DDX_Control(pDX, IDC_COMBO2, m_SelLangKor);
	DDX_Control(pDX, IDC_COMBO1, m_SelLang);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextClientDlg, CDialog)
	//{{AFX_MSG_MAP(CTextClientDlg)
	ON_BN_CLICKED(IDC_TCLIENT_BUTTON1, OnTclientButton1)
	ON_BN_CLICKED(IDC_TCLIENT_MSTART, OnTclientMstart)
	ON_BN_CLICKED(IDC_TCLIENT_BUTTON2, OnTclientButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextClientDlg message handlers

void CTextClientDlg::OnTclientButton1() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "TextClient Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		m_vecTextSrc.clear();

		if( LoadTextClient( dlg.GetFileName(), 	m_vecTextSrc ) )
		{
			char Buffer[1024] = { 0 };
			GetCurrentDirectory( 1024, Buffer );
			m_SrcPath = Buffer+("\\"+dlg.GetFileName());
			
			UpdateData(FALSE);
		}
		else
			m_SrcPath = "";
	}
}

void CTextClientDlg::OnTclientButton2() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "TextClient Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		m_vecTextDest.clear();
		
		if( LoadTextClient( dlg.GetFileName(), 	m_vecTextDest ) )
		{
			char Buffer[1024] = { 0 };
			GetCurrentDirectory( 1024, Buffer );
			m_DestPath = Buffer+("\\"+dlg.GetFileName());
			
			UpdateData(FALSE);
		}
		else
			m_DestPath = "";
	}
}

BOOL CTextClientDlg::LoadTextClient(LPCTSTR lpszFileName, VEC_TEXTCLIENT& vec_textclient)
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;

	CScript script;
	if( script.Load( "lang.h" ) == TRUE )
		script.PreScan( FALSE );

	TEXTCLIENT textclient;

	for( int i = 0; i < LANG_MAX; i++ )
		textclient.awszMessage[ i ][0] = 0;
	
	scanner.GetToken();
	textclient.m_strDefine = scanner.token;
	
	do {
		scanner.GetToken();
		textclient.m_strColor = "0x";
		textclient.m_strColor+=scanner.token; // color

		scanner.GetToken();

		if( *scanner.token  == '{' )
		{
			DWORD dwNation = scanner.GetNumber();
			while( *scanner.token  != '}' )
			{
				scanner.GetLastFull();// 스트링 

				if( dwNation >= 0 && dwNation < LANG_MAX )
					wcscpy( textclient.awszMessage[ dwNation ], scanner.m_wszToken );
				else
				{
					CString strerror;

					strerror.Format( "%s에 인식못하는 언어선언되어있음 : %d", textclient.m_strDefine, dwNation );
					AfxMessageBox( strerror );
				}

				dwNation = scanner.GetNumber(); // 국가 코드 
			}

			for( int i=0; i<vec_textclient.size(); i++ )
			{
				if( vec_textclient[i].m_strDefine.Find( textclient.m_strDefine ) != -1 && vec_textclient[i].m_strDefine.GetLength() == textclient.m_strDefine.GetLength() )
				{
					CString strerror;
					strerror.Format( "같은 디파인 존재 : %s", textclient.m_strDefine );
					AfxMessageBox( strerror );
					return FALSE;
				}
			}

			vec_textclient.push_back( textclient );

			for( int i = 0; i < LANG_MAX; i++ )
				textclient.awszMessage[ i ][0] = 0;
		}
		scanner.GetToken();
		textclient.m_strDefine = scanner.token;

	} while( scanner.tok != FINISHED );

	return TRUE;
}

BOOL CTextClientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_vecTextSrc.clear();
	m_vecTextDest.clear();

	m_bSkip = TRUE;

	m_nLang = -1;
	m_nLangKor = -1;

	UpdateData( FALSE );
	
	return TRUE;  
}

void CTextClientDlg::OnSelchangeCombo1() 
{
	m_nLang = m_SelLang.GetCurSel();
}

void CTextClientDlg::OnSelchangeCombo2() 
{
	m_nLangKor = m_SelLangKor.GetCurSel();
}


void CTextClientDlg::OnTclientMstart() 
{
	if( m_nLang == -1 || m_nLangKor == -1 )
	{
		AfxMessageBox( "Please Select Nation" );
		return;
	}

	CFileDialog dlg(   
        FALSE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Textclient Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		CString FileName = dlg.GetFileTitle();
		FileName+=".inc";

		for( int i=0; i<m_vecTextDest.size(); i++ )
		{
			for( int j=0; j<m_vecTextSrc.size(); j++ )
			{
				// 같은 디파인을 찾았다..
				if( m_vecTextSrc[j].m_strDefine.Find( m_vecTextDest[i].m_strDefine ) != -1 && m_vecTextSrc[j].m_strDefine.GetLength() == m_vecTextDest[i].m_strDefine.GetLength() )
				{
					// 내용이 있으면 복사, 없으면 갱신된 내용임
					if ( m_bSkip )
					{
						if( _wcsicmp( m_vecTextSrc[j].awszMessage[0], m_vecTextDest[i].awszMessage[0] ) == 0 )
							wcscpy( m_vecTextSrc[j].awszMessage[m_nLangKor], m_vecTextDest[i].awszMessage[m_nLang] );
						else
						{
							*m_vecTextSrc[j].awszMessage[m_nLangKor] = 0;
							//wcscpy( m_vecTextSrc[j].awszMessage[m_nLangKor], 0 );
						}
					}
					else
						wcscpy( m_vecTextSrc[j].awszMessage[m_nLangKor], m_vecTextDest[i].awszMessage[m_nLang] );

					break;
				}
			}
		}

		if( Save( FileName ) )
			AfxMessageBox( "Save OK!" );
		else
			AfxMessageBox( "Save Error!" );
	}
}

BOOL CTextClientDlg::Save( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
		return FALSE; 
	
	CString string;
	wchar_t wszBuff[ 2048 ];

	file.PutC( 0xff );
	file.PutC( 0xfe );
	
	// 무식하다..ㅡㅡ;
	string = "//이 파일 수정 시에 절대 EditPlus를 사용하시지 마십시요.\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "//유니코드 부분에 충돌이 있습니다.\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "//메모장을 사용하여 주십시요.\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "// textServer.inc : Multi language text\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "// textClient.inc : Multi language text\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "// 이야기식 변수 표현 : %s\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );
	string = "// 수치 변수 표현 : %d\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff) * 2 );

	file.PutWideString( "\r\n" );
	file.PutWideString( "\r\n" );
	file.PutWideString( "\r\n" );
	
	TEXTCLIENT textclient;

	for( int i=0; i<m_vecTextSrc.size(); i++ )
	{
		textclient = m_vecTextSrc[i];

		string.Format( _T("%s %s"), textclient.m_strDefine, textclient.m_strColor );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );

		file.PutWideString( "\r\n" );
		file.PutWideString( "{\r\n" );

		for( int j = 0; j < LANG_MAX; j++ )
		{
			string.Format( _T( "\t%s " ), g_szLanguage[j] );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.Write( textclient.awszMessage[j], wcslen(textclient.awszMessage[j] ) * 2 );

			file.PutWideString( "\r\n" );
		}

		file.PutWideString( "}\r\n" );
		file.PutWideString( "\r\n" );
	}
	
	return TRUE;
}


void CTextClientDlg::OnCheck1() 
{
	m_bSkip = !m_bSkip;	
}

