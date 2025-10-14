// HelpConvert.cpp : implementation file
//

#include "stdafx.h"
#include "lang.h"
#include "ScriptConvert.h"
#include "HelpConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpConvert dialog


CHelpConvert::CHelpConvert(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpConvert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpConvert)
	m_SrcPath = _T("");
	m_DestPath = _T("");
	m_bSkip = FALSE;
	//}}AFX_DATA_INIT
}


void CHelpConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpConvert)
	DDX_Control(pDX, IDC_COMBO1, m_Lang);
	DDX_Control(pDX, IDC_COMBO2, m_LangKor);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpConvert, CDialog)
	//{{AFX_MSG_MAP(CHelpConvert)
	ON_BN_CLICKED(IDC_HELP_BUTTON1, OnHelpButton1)
	ON_BN_CLICKED(IDC_HELP_BUTTON2, OnHelpButton2)
	ON_BN_CLICKED(IDC_MERGE, OnMerge)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpConvert message handlers

void CHelpConvert::OnHelpButton1() 
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

void CHelpConvert::OnHelpButton2() 
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

void CHelpConvert::OnMerge() 
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

BOOL CHelpConvert::Save( LPCTSTR lpszFileName )
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
	
	HELP_INC help_inc;
	
	for( int i=0; i<m_vecSrc.size(); i++ )
	{
		help_inc = m_vecSrc[i];
	
		file.PutWideString( "\r\n" );
		file.PutWideString( "\r\n" );
		file.PutWideString( "\r\n" );
		
		for( int j = 0; j < LANG_MAX; j++ )
		{
			string.Format( _T( "%s " ), g_szLanguage[j] );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.Write( help_inc.m_str.awszMessage[j], wcslen(help_inc.m_str.awszMessage[j] ) * 2 );
			
			file.PutWideString( "\r\n" );
		}

		file.PutWideString( "{\r\n" );
		for( int k = 0; k < help_inc.m_vecstr.size(); k++ )
		{
			LANG_STR lang_str = help_inc.m_vecstr[k];

			for( int j = 0; j < LANG_MAX; j++ )
			{
				string.Format( _T( "\t%s " ), g_szLanguage[j] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.Write( lang_str.awszMessage[j], wcslen(lang_str.awszMessage[j] ) * 2 );
				
				file.PutWideString( "\r\n" );
			}			

			file.PutWideString( "\r\n" );
		}
		
		file.PutWideString( "}\r\n" );
	}
	
	return TRUE;
}
BOOL CHelpConvert::Load(LPCTSTR lpszFileName, VEC_HELP_STR& vec_help)
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	
	CScript script;
	if( script.Load( "lang.h" ) == TRUE )
		script.PreScan( FALSE );
	
	HELP_INC help_inc;
	
	for( int i = 0; i < LANG_MAX; i++ )
		help_inc.m_str.awszMessage[ i ][0] = 0;
	
	CString strkor;
	vec_help.clear();
	
	do {
		for( int i = 0; i < LANG_MAX; i++ )
			help_inc.m_str.awszMessage[ i ][0] = 0;

		help_inc.m_vecstr.clear();

		DWORD dwNation;

		for( int i=0; i<LANG_MAX; i++ )
		{
			dwNation = scanner.GetNumber(); // 국가 코드 

			if( scanner.tok == FINISHED )
				return TRUE;

			if( *scanner.token  == '{' )
			{
				if( i != LANG_MAX )
				{
					for( int kk=i; kk<LANG_MAX; kk++ )
					{
						help_inc.m_str.awszMessage[ kk ][0] = '\0';
					}
				}
				break;
			}
			
			scanner.GetLastFull();// 스트링 

			if( dwNation == LANG_KOR )
				strkor = scanner.Token;

			if( dwNation >= 0 && dwNation < LANG_MAX )
				wcscpy( help_inc.m_str.awszMessage[ dwNation ], scanner.m_wszToken );		
		}

		if( strkor.IsEmpty() )
		{
			AfxMessageBox( "한국어가 들어가 있지 않습니다." );
			return FALSE;
		}
		
		LANG_STR lang_str;
		dwNation = scanner.GetNumber(); // 국가 코드 

		scanner.GetNumber();
		int nErrorIndex = 0;
	
		do {
			for( int i=0; i<LANG_MAX; i++ )
			{
				if( dwNation != i )
				{
					if( i != LANG_MAX )
					{
						for( int kk=i; kk<LANG_MAX; kk++ )
						{
							lang_str.awszMessage[ kk ][0] = '\0';
						}
					}
					break;
				}
				
				scanner.GetLastFull();// 스트링 
				
				if( dwNation == LANG_KOR )
					strkor = scanner.Token;
				
				if( dwNation >= 0 && dwNation < LANG_MAX )
					wcscpy( lang_str.awszMessage[ dwNation ], scanner.m_wszToken );		

				dwNation = scanner.GetNumber(); // 국가 코드 
			}

			nErrorIndex++;

			if( nErrorIndex > 100 )
			{
				CString strerror;
				strerror.Format( "잘못된 문법발견 마지막으로 읽은것 %s", strkor );
				AfxMessageBox( strerror );
				break;
			}

			help_inc.m_vecstr.push_back(lang_str);

		} while( *scanner.token != '}' );

		if( help_inc.m_vecstr.size() <= 0 )
		{
			CString strerror;
			strerror.Format( "도움말 설명부분이 없다! 마지막으로 읽은것 %s", strkor );
			AfxMessageBox( strerror) ;
			return FALSE;
		}
	
		vec_help.push_back(help_inc);
		
	} while( scanner.tok != FINISHED );
	
	return TRUE;	
}

BOOL CHelpConvert::Merge(VEC_HELP_STR& vecSrc, const VEC_HELP_STR& vecDest)
{
	int nLang = m_Lang.GetCurSel();
	int nLangKor = m_LangKor.GetCurSel();

	for( int i=0; i<vecDest.size(); i++ )
	{	
		for( int j=0; j<vecSrc.size(); j++ )
		{
			// 내용이 있으면 복사, 없으면 갱신된 내용임
			if( _wcsicmp( vecSrc[j].m_str.awszMessage[0], vecDest[i].m_str.awszMessage[0] ) == 0 )
			{
				if ( m_bSkip )
				{
					*vecSrc[j].m_str.awszMessage[nLangKor] = 0;
					//wcscpy( vecSrc[j].m_str.awszMessage[nLangKor], 0 );
				}
				else
					wcscpy( vecSrc[j].m_str.awszMessage[nLangKor], vecDest[i].m_str.awszMessage[nLang] );
	
				for( int k=0; k<vecDest[i].m_vecstr.size(); k++ )
				{
					for( int l=0; l<vecSrc[j].m_vecstr.size(); l++ )
					{
						if( _wcsicmp( vecSrc[j].m_vecstr[l].awszMessage[0], vecDest[i].m_vecstr[k].awszMessage[0] ) == 0 )
						{
								// 내용이 있으면 복사, 없으면 갱신된 내용임
							if ( m_bSkip )
							{
								*vecSrc[j].m_vecstr[l].awszMessage[nLangKor] = 0;
								//wcscpy( vecSrc[j].m_vecstr[l].awszMessage[nLangKor], 0 );
							}
							else
								wcscpy( vecSrc[j].m_vecstr[l].awszMessage[nLangKor], vecDest[i].m_vecstr[k].awszMessage[nLang] );
						}
					}
				}
			}
		}
	}

	return TRUE;
}

void CHelpConvert::OnCheck1() 
{
	m_bSkip = !m_bSkip;	
}

BOOL CHelpConvert::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bSkip = TRUE;
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
