// CharacterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vutil.h"
#include "lang.h"
#include "cclass.h"
#include "ScriptConvert.h"
#include "CharacterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharacterDlg dialog

extern BOOL GetLangScript( CScanner& scanner, wchar_t wszToken[] );

CCharacterDlg::CCharacterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCharacterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacterDlg)
	m_SrcPath = _T("");
	m_DestPath = _T("");
	m_bSkip = FALSE;
	//}}AFX_DATA_INIT
}


void CCharacterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterDlg)
	DDX_Control(pDX, IDC_COMBO2, m_LangKor);
	DDX_Control(pDX, IDC_COMBO1, m_Lang);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharacterDlg, CDialog)
	//{{AFX_MSG_MAP(CCharacterDlg)
	ON_BN_CLICKED(IDC_CHAR_BUTTON1, OnCharButton1)
	ON_BN_CLICKED(IDC_CHAR_BUTTON2, OnCharButton2)
	ON_BN_CLICKED(IDC_CHAR_MSTART, OnCharMstart)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterDlg message handlers

void CCharacterDlg::OnCharButton1() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Character Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

void CCharacterDlg::OnCharButton2() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Character Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

BOOL CCharacterDlg::Load(LPCTSTR lpszFileName, VEC_CHARACTER_INC& vec_char)
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	
	CScript script;
	if( script.Load( "lang.h" ) == TRUE )
		script.PreScan( FALSE );
	
	CHARACTER_INC	character_inc;
	
	scanner.GetToken(); 
	
	character_inc.m_strNPCName = scanner.Token;
	
	do 
	{
		character_inc.m_strString.clear();
		character_inc.m_vecAddVendorSlot.clear();

		scanner.GetToken(); // {
		scanner.GetToken(); 
		while(*scanner.token != '}')
		{
			if(scanner.Token == ";") 
			{ 
				scanner.GetToken(); 
				continue; 
			}
			else
			if(scanner.Token == "SetName")
			{ 
				character_inc.m_strSetName.m_strTitle = scanner.Token;

				scanner.GetToken(); // (

				for( int i=0; i<LANG_MAX; i++ )
				{
					GetLangScript( scanner,  character_inc.m_strSetName.awszMessage[i] );
					
					if( scanner.Token == ')' )
					{
						// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
						if( i+1 != LANG_MAX )
						{
							for( int kk=i+1; kk<LANG_MAX; kk++ )
							{
								CString str;
								str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
									character_inc.m_strSetName.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
								//AfxMessageBox( str );
								character_inc.m_strSetName.awszMessage[ kk ][0] = '\0';
							}
						}
						
						break;
					}
				}
				
				scanner.GetToken(); // ;
			}
			else
			if(scanner.Token == "AddVendorSlot" )
			{
				NATION_STR_EX nation_str_ex;
				
				nation_str_ex.m_strTitle = scanner.Token;

				scanner.GetToken(); // (
				scanner.GetToken(); // index
				nation_str_ex.m_strIndex = scanner.Token;
				scanner.GetToken(); // ,

				for( int i=0; i<LANG_MAX; i++ )
				{
					GetLangScript( scanner,  nation_str_ex.awszMessage[i] );
					
					if( scanner.Token == ')' )
					{
						// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
						if( i+1 != LANG_MAX )
						{
							for( int kk=i+1; kk<LANG_MAX; kk++ )
							{
								CString str;
								str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
									nation_str_ex.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
								//AfxMessageBox( str );
								nation_str_ex.awszMessage[ kk ][0] = '\0';
							}
						}
						
						break;
					}
				}
				
				scanner.GetToken(); // ;
				
				character_inc.m_vecAddVendorSlot.push_back( nation_str_ex );
			}
			else
			if(scanner.Token == "setting" || scanner.Token == "Setting" )
			{
				WCSTR wcstr;
				wcscpy( wcstr.m_wcstr, scanner.m_wszToken );
				character_inc.m_strString.push_back( wcstr );
				
				scanner.GetToken(); // {
				
				scanner.GetToken2();
				do 
				{
					wcscpy( wcstr.m_wcstr, scanner.m_wszToken );
					scanner.GetLastFull();
					wcscat( wcstr.m_wcstr, scanner.m_wszToken );
					
					character_inc.m_strString.push_back( wcstr );
					
					scanner.GetToken2();
					
				} while(scanner.Token != '}' );
			}
			else
			{/*
				CString strError;
				strError.Format( "모르는 명령어 발견 : %s", scanner.Token );
				AfxMessageBox( strError, MB_OK );
				*/
			}

			scanner.GetToken();
		}

		scanner.GetToken(); // Mover name

		vec_char.push_back(character_inc);

		character_inc.m_strNPCName = scanner.Token;

	} while(scanner.tok != FINISHED);

	return TRUE;
}

BOOL CCharacterDlg::Merge(VEC_CHARACTER_INC& vecSrc, const VEC_CHARACTER_INC& vecDest)
{
	int nLang = m_Lang.GetCurSel();
	int nLangKor = m_LangKor.GetCurSel();
	
	for( int i=0; i<vecDest.size(); i++ )
	{	
		for( int j=0; j<vecSrc.size(); j++ )
		{
			if( vecDest[i].m_strNPCName.Find( vecSrc[j].m_strNPCName ) != -1 && vecDest[i].m_strNPCName.GetLength() == vecSrc[j].m_strNPCName.GetLength() )
			{
				// 내용이 있으면 복사, 없으면 갱신된 내용임
				if ( m_bSkip )
				{
					// 한글 내용이 같으면 복사
					if( _wcsicmp( vecSrc[j].m_strSetName.awszMessage[0], vecDest[i].m_strSetName.awszMessage[0] ) == 0 )
						wcscpy( vecSrc[j].m_strSetName.awszMessage[nLangKor], vecDest[i].m_strSetName.awszMessage[nLang] );
					else //틀리면 해당언어 삭제
						*vecSrc[j].m_strSetName.awszMessage[nLangKor] = 0;
				}
				else
					wcscpy( vecSrc[j].m_strSetName.awszMessage[nLangKor], vecDest[i].m_strSetName.awszMessage[nLang] );

				for( int nVendorCount = 0; nVendorCount < vecDest[i].m_vecAddVendorSlot.size(); nVendorCount++ )
				{
					for( int nVendorCount2 = 0; nVendorCount2 < vecSrc[j].m_vecAddVendorSlot.size(); nVendorCount2++ )
					{
						int nNum1 = atoi(vecDest[i].m_vecAddVendorSlot[nVendorCount].m_strIndex);
						int nNum2 = atoi(vecSrc[j].m_vecAddVendorSlot[nVendorCount2].m_strIndex);

						if( nNum1 == nNum2 )
						{
							if ( m_bSkip )
							{
								// 한글 내용이 같으면 복사
								if( _wcsicmp( vecSrc[j].m_vecAddVendorSlot[nVendorCount2].awszMessage[0], vecDest[i].m_vecAddVendorSlot[nVendorCount].awszMessage[0] ) == 0 )
									wcscpy( vecSrc[j].m_vecAddVendorSlot[nVendorCount2].awszMessage[nLangKor], vecDest[i].m_vecAddVendorSlot[nVendorCount].awszMessage[nLang] );
								else
								{
									*vecSrc[j].m_vecAddVendorSlot[nVendorCount2].awszMessage[nLangKor]	= 0;
								}
							}
							else
								wcscpy( vecSrc[j].m_vecAddVendorSlot[nVendorCount2].awszMessage[nLangKor], vecDest[i].m_vecAddVendorSlot[nVendorCount].awszMessage[nLang] );
							
							break;
						}
					}
				}
			}
		}
	}
					
	return TRUE;
}

BOOL CCharacterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bSkip = TRUE;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharacterDlg::OnCharMstart() 
{
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
        "Character Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

BOOL CCharacterDlg::Save( LPCTSTR lpszFileName )
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
	file.PutWideString( "\r\n" );
	file.PutWideString( "\r\n" );
	
	CHARACTER_INC character_inc;
	
	for( int i=0; i<m_vecSrc.size(); i++ )
	{
		character_inc = m_vecSrc[i];
		
		string.Format( _T("%s\r\n"), character_inc.m_strNPCName );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		
		file.PutWideString( "{\r\n" );

		// save setting
		if( character_inc.m_strString.size() )
		{
			file.PutWideString( "\t" );
			WCSTR wcstr = character_inc.m_strString[0];
			file.Write( wcstr.m_wcstr, wcslen(wcstr.m_wcstr) * 2 );
			file.PutWideString( "\r\n" );
		}

		file.PutWideString( "\t{\r\n" );
		for( int k = 1; k < character_inc.m_strString.size(); k++ )
		{
			WCSTR wcstr = character_inc.m_strString[k];
			file.PutWideString( "\t\t" );
			file.Write( wcstr.m_wcstr, wcslen(wcstr.m_wcstr) * 2 );
			file.PutWideString( "\r\n" );
		}
		file.PutWideString( "\t}\r\n" );
		
		file.PutWideString( "\r\n" );
		
		// save SetName
		string.Format( _T("\t%s\r\n"), character_inc.m_strSetName.m_strTitle );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );

		file.PutWideString( "\t(\r\n" );

		for( int nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
		{
			string.Format( _T( "\t%s, " ), g_szLanguage[nLangCount] );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			CString pl = '"';
			file.PutWideString( _T(pl) );
			
			file.Write( character_inc.m_strSetName.awszMessage[nLangCount], wcslen(character_inc.m_strSetName.awszMessage[nLangCount] ) * 2 );
			
			file.PutWideString( _T(pl) );
			
			if( nLangCount == LANG_MAX-1 )
				file.PutWideString( "\r\n" );
			else
				file.PutWideString( ",\r\n" );
		}
		
		file.PutWideString( "\t);\r\n" );
		
		// save AddVendorSlot
		for( int nVendorCount=0; nVendorCount<character_inc.m_vecAddVendorSlot.size(); nVendorCount++ )
		{
			NATION_STR_EX nation_str_ex;
			nation_str_ex = character_inc.m_vecAddVendorSlot[nVendorCount];

			file.PutWideString( "\r\n" );

			string.Format( _T("\t%s( %s,\r\n"), nation_str_ex.m_strTitle, nation_str_ex.m_strIndex );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			for( int nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t%s, " ), g_szLanguage[nLangCount] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
				CString pl = '"';
				file.PutWideString( _T(pl) );
				
				file.Write( nation_str_ex.awszMessage[nLangCount], wcslen(nation_str_ex.awszMessage[nLangCount] ) * 2 );
				
				file.PutWideString( _T(pl) );
				
				if( nLangCount == LANG_MAX-1 )
					file.PutWideString( "\r\n" );
				else
					file.PutWideString( ",\r\n" );
			}
			
			file.PutWideString( "\t);\r\n" );
		}	

		file.PutWideString( "}\r\n" );
		file.PutWideString( "\r\n" );
	}

	return TRUE;
}



void CCharacterDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	m_bSkip = !m_bSkip;
}
