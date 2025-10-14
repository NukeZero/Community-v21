// TreeHelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lang.h"
#include "ScriptConvert.h"
#include "TreeHelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeHelpDlg dialog


CTreeHelpDlg::CTreeHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeHelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeHelpDlg)
	m_bSkip = FALSE;
	m_SrcPath = _T("");
	m_DestPath = _T("");
	//}}AFX_DATA_INIT
}


void CTreeHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeHelpDlg)
	DDX_Control(pDX, IDC_COMBO1, m_LangKor);
	DDX_Control(pDX, IDC_COMBO2, m_Lang);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeHelpDlg)
	ON_BN_CLICKED(IDC_MERGE, OnMerge)
	ON_BN_CLICKED(IDC_TREEHELP_BUTTON1, OnTreehelpLoad1)
	ON_BN_CLICKED(IDC_TREEHELP_BUTTON3, OnTreehelpLoad2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeHelpDlg message handlers

void CTreeHelpDlg::OnMerge() 
{
	// TODO: Add your control notification handler code here
	if( m_Lang.GetCurSel() == -1 || m_LangKor.GetCurSel() == -1 )
	{
		AfxMessageBox( "Please Select Nation" );
		return;
	}
	
	CFileDialog dlg(   
        FALSE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Help Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		CString FileName = dlg.GetFileTitle();
		FileName+=".inc";
		
		Merge( m_vecSrc, m_vecDest );
		
		if( Save( FileName ) )
			AfxMessageBox( "Save OK!" );
		else
			AfxMessageBox( "Save Error!" );
		
	}
	
}

void CTreeHelpDlg::OnTreehelpLoad1() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Help Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		m_vecSrc.clear();
		
		if( Load( dlg.GetFileName(), m_vecSrc ) )
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

void CTreeHelpDlg::OnTreehelpLoad2() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Help Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		m_vecDest.clear();
		
		if( Load( dlg.GetFileName(), m_vecDest ) )
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

void CTreeHelpDlg::OnCheck1() 
{
	m_bSkip = !m_bSkip;	
}

BOOL CTreeHelpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bSkip = TRUE;
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTreeHelpDlg::Save( LPCTSTR lpszFileName )
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
	
	file.PutWideString( "\r\n" );
	
	CString strTab = "";
	LANG_STR_EX help_inc;
	
	file.PutWideString( "\r\n" );
	file.PutWideString( "\r\n" );
	
	for( int i=0; i<m_vecSrc.size(); i++ )
	{
		help_inc = m_vecSrc[i];

		BOOL bSkip = TRUE;

		for( int j = 0; j < LANG_MAX; j++ )
		{
			if( wcslen( help_inc.awszMessage[j] ) != 0 )
			{
				bSkip = FALSE;
				break;
			}
		}

		if( bSkip == FALSE )
		{
			for( int j = 0; j < LANG_MAX; j++ )
			{
				string.Format( _T( strTab+"%s " ), g_szLanguage[j] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.Write( help_inc.awszMessage[j], wcslen(help_inc.awszMessage[j] ) * 2 );
				
				file.PutWideString( "\r\n" );
			}
		}
		else
		{
			AfxMessageBox( "경고! 입력이 안된곳이 있습니다. 모든언어 공백" );
			//return FALSE;
		}
		
		switch( help_inc.m_nBraces )
		{
			case 0:
				break;
			case 1: // {
				file.PutWideString( strTab+"{\r\n" );
				strTab.Insert( 0, "                ");
				break;
			case 2: // }
				strTab.Delete( 0, 16 );
				file.PutWideString( strTab+"}\r\n" );
				break;
		}
	}
	
	return TRUE;
}
BOOL CTreeHelpDlg::Load(LPCTSTR lpszFileName, VEC_LANG_STR_EX& vec_help)
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	
	CScript script;
	if( script.Load( "lang.h" ) == TRUE )
		script.PreScan( FALSE );

	vec_help.clear();
	

	int nErrCount1 = 0;
	int nErrCount2 = 0;
	LANG_STR_EX lang_str;
	
	scanner.GetToken();
	
	do 
	{
		nErrCount1++;

		for( int i = 0; i < LANG_MAX; i++ )
			lang_str.awszMessage[ i ][0] = 0;

		lang_str.m_bSkip = FALSE;

		DWORD dwNation = 0;

		for( int i=0; i<LANG_MAX; i++ )
		{
			if( dwNation != i )
				break;
			
			scanner.GetLastFull();// 스트링 

			if( dwNation >= 0 && dwNation < LANG_MAX )
			{
				wcscpy( lang_str.awszMessage[ dwNation ], scanner.m_wszToken );		
				//lang_str.m_str[dwNation] = scanner.Token;
			}
			dwNation = scanner.GetNumber(); // 국가 코드 
		}

		int nOpenCount = 0;
		int nCloseCount = 0;
		int nCount = 0;

		if( *scanner.token == '{' )
		{
			nCount = 1;
			lang_str.m_nBraces = 1;
			lang_str.m_bIsHeader = TRUE;
			vec_help.push_back(lang_str);

			scanner.GetToken();

			do 
			{
				nErrCount2++;

				for( int i = 0; i < LANG_MAX; i++ )
					lang_str.awszMessage[ i ][0] = 0;

				for( int i=0; i<LANG_MAX; i++ )
				{
					if( *scanner.token == '{' || *scanner.token == '}' )
						break;
						
					if( dwNation != i )
						break;
					
					scanner.GetLastFull();// 스트링 
					
					if( dwNation >= 0 && dwNation < LANG_MAX )
					{
						wcscpy( lang_str.awszMessage[ dwNation ], scanner.m_wszToken );		
						//lang_str.m_str[dwNation] = scanner.Token;
					}
					
					dwNation = scanner.GetNumber(); // 국가 코드 
				}

				if( *scanner.token == '{' )
				{
					nCount++;
					lang_str.m_nBraces = 1;
					scanner.GetToken();

					if( *scanner.token == '{' )
						lang_str.m_bSkip = TRUE;					
				}
				else
				if( *scanner.token == '}' )
				{
					nCount--;
					lang_str.m_nBraces = 2;
					scanner.GetToken();

					if( *scanner.token == '}' )
						lang_str.m_bSkip = TRUE;					
				}
				else
				{
					lang_str.m_bSkip = FALSE;
					lang_str.m_nBraces = 0;
				}
				
				lang_str.m_bIsHeader = FALSE;
				vec_help.push_back(lang_str);
				
				if( nErrCount2 >= 1000000 )
				{
					AfxMessageBox( "문법오류발견" );
					return FALSE;
				}

			} while( nCount != 0 );
		}
		else
		{
			nCount   = 0;
			lang_str.m_nBraces = 0;
			lang_str.m_bIsHeader = FALSE;
			vec_help.push_back(lang_str);
		}
		
		if( nErrCount1 >= 1000000 )
		{
			AfxMessageBox( "문법오류발견" );
			return FALSE;
		}
		
	} while( scanner.tok != FINISHED );

	return TRUE;
}
BOOL CTreeHelpDlg::Merge(VEC_LANG_STR_EX& vecSrc, const VEC_LANG_STR_EX& vecDest)
{
	int nLang = m_Lang.GetCurSel();
	int nLangKor = m_LangKor.GetCurSel();

	for( int i=0; i<vecDest.size(); i++ )
	{
		for( int j=0; j<vecSrc.size(); j++ )
		{
			LANG_STR_EX lang_str1 = vecDest[i];
			LANG_STR_EX lang_str2 = vecSrc[j];
			
			// 같은 디파인을 찾았다..
			if( _wcsicmp( vecSrc[j].awszMessage[0], vecDest[i].awszMessage[0] ) == 0 &&
				wcslen(vecSrc[j].awszMessage[0]) == wcslen(vecDest[i].awszMessage[0]) )
			{
				if ( m_bSkip )
				{
					*vecSrc[j].awszMessage[nLang] = 0;
					//wcscpy( vecSrc[j].awszMessage[nLang], 0 );
				}
				else
					wcscpy( vecSrc[j].awszMessage[nLang], vecDest[i].awszMessage[nLangKor] );
			}
		}
	}
				
	return TRUE;
}
