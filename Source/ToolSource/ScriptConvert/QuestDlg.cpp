// QuestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vutil.h"
#include "lang.h"
#include "cclass.h"
#include "ScriptConvert.h"
#include "QuestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuestDlg dialog

extern BOOL GetLangScript( CScanner& scanner, wchar_t wszToken[] );


CQuestDlg::CQuestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuestDlg)
	m_SrcPath = _T("");
	m_DestPath = _T("");
	m_bSkip = FALSE;
	//}}AFX_DATA_INIT
}


void CQuestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestDlg)
	DDX_Control(pDX, IDC_COMBO2, m_LangKor);
	DDX_Control(pDX, IDC_COMBO1, m_Lang);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestDlg, CDialog)
	//{{AFX_MSG_MAP(CQuestDlg)
	ON_BN_CLICKED(IDC_QUEST_BUTTON1, OnQuestButton1)
	ON_BN_CLICKED(IDC_QUEST_MSTART, OnQuestMstart)
	ON_BN_CLICKED(IDC_TCLIENT_BUTTON2, OnTclientButton2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuestDlg message handlers

void CQuestDlg::OnQuestButton1() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "PropQuest Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

void CQuestDlg::OnTclientButton2() 
{
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "PropQuest Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

BOOL CQuestDlg::Load(LPCTSTR lpszFileName, VEC_PROPQUEST& vec_quest)
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	
	CScript script;
	if( script.Load( "lang.h" ) == TRUE )
		script.PreScan( FALSE );
	
	PROPQUEST	propquest;
	
	scanner.GetToken(); 

	while( scanner.tok != FINISHED )
	{
		//*
		propquest.m_NpcName.m_strTitle.Empty();
		propquest.m_Title.m_strTitle.Empty();
		propquest.m_vecSetDialog.clear();
		propquest.m_vecSetting.clear();
		propquest.m_vecState.clear();
		propquest.m_PatrolZoneName.m_strTitle.Empty();
		/**/
		
		propquest.m_strNumber = scanner.token;

		if( propquest.m_strNumber == "2037" )
		{
			int a = 0;
		}
		
		scanner.GetToken();
		
		while( scanner.Token[0] != '}' )
		{
			if( scanner.Token == "{" )
			{
				scanner.GetToken();
				continue;
			}
			else
			if( scanner.Token == "SetNPCName" )
			{
				propquest.m_NpcName.m_strTitle = scanner.Token;
				
				scanner.GetToken(); // (
				
				for( int i=0; i<LANG_MAX; i++ )
				{
					if( GetLangScript( scanner,  propquest.m_NpcName.awszMessage[i] ) == FALSE )
					{
						CString szError;
						szError.Format( "%s에 콤바가 없으요(%s)", propquest.m_NpcName.m_strTitle, propquest.m_strNumber );
						AfxMessageBox( szError );	
						return FALSE;
					}
					
					if( scanner.Token == ')' )
					{
						// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
						if( i+1 != LANG_MAX )
						{
							for( int kk=i+1; kk<LANG_MAX; kk++ )
							{
								CString str;
								str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
									propquest.m_NpcName.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
								//AfxMessageBox( str );
								propquest.m_NpcName.awszMessage[ kk ][0] = '\0';
							}
						}
						
						break;
					}
				}
				
				scanner.GetToken(); // ;
			}
			else
			if( scanner.Token == "SetTitle" )
			{
				propquest.m_Title.m_strTitle = scanner.Token;

				scanner.GetToken(); // (
				
				for( int i=0; i<LANG_MAX; i++ )
				{
					if( GetLangScript( scanner,  propquest.m_Title.awszMessage[i] ) == FALSE )
					{
						CString szError;
						szError.Format( "%s에 콤바가 없으요(%s)", propquest.m_Title.m_strTitle, propquest.m_strNumber );
						AfxMessageBox( szError );								
						return FALSE;
					}
					

					if( scanner.Token == ')' )
					{
						// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
						if( i+1 != LANG_MAX )
						{
							for( int kk=i+1; kk<LANG_MAX; kk++ )
							{
								CString str;
								str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
									propquest.m_Title.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
								//AfxMessageBox( str );
								propquest.m_Title.awszMessage[ kk ][0] = '\0';
							}
						}

						break;
					}
				}

				scanner.GetToken(); // ;
			}
			else
			if( scanner.Token == "setting" || scanner.Token == "Setting" )
			{
				WCSTR wcstr;
				wcscpy( wcstr.m_wcstr, scanner.m_wszToken );
				propquest.m_vecSetting.push_back( wcstr );

				scanner.GetToken(); // {

				scanner.GetToken2();
				do 
				{
					wcscpy( wcstr.m_wcstr, scanner.m_wszToken );
					scanner.GetLastFull();
					wcscat( wcstr.m_wcstr, scanner.m_wszToken );

					propquest.m_vecSetting.push_back( wcstr );

					scanner.GetToken2();

				} while(scanner.Token != '}' );
			}
			else
			if( scanner.Token == "SetPatrolZoneName" )
			{
				propquest.m_PatrolZoneName.m_strTitle = scanner.Token;
				
				scanner.GetToken(); // (
				
				for( int i=0; i<LANG_MAX; i++ )
				{
					if( GetLangScript( scanner,  propquest.m_PatrolZoneName.awszMessage[i] ) == FALSE )
					{
						CString szError;
						szError.Format( "%s에 콤바가 없으요(%s)", propquest.m_PatrolZoneName.m_strTitle, propquest.m_strNumber );
						AfxMessageBox( szError );								
						return FALSE;
					}
					
					if( scanner.Token == ')' )
					{
						// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
						if( i+1 != LANG_MAX )
						{
							for( int kk=i+1; kk<LANG_MAX; kk++ )
							{
								CString str;
								str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
									propquest.m_PatrolZoneName.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
								//AfxMessageBox( str );
								propquest.m_PatrolZoneName.awszMessage[ kk ][0] = '\0';
							}
						}
						
						break;
					}
				}
				
				scanner.GetToken(); // ;
			}
			else
			if( scanner.Token == "SetDialog" )
			{
				NATION_STR_EX nation_str_ex;

				nation_str_ex.m_strTitle = scanner.Token;

				scanner.GetToken(); // (
				scanner.GetToken(); // index
				
				nation_str_ex.m_strIndex = scanner.Token;

				scanner.GetToken(); // ,
				
				for( int i=0; i<LANG_MAX; i++ )
				{
					if( GetLangScript( scanner,  nation_str_ex.awszMessage[i] ) == FALSE )
					{
						CString szError;
						szError.Format( "%s에 콤바가 없으요(%s)", nation_str_ex.m_strIndex, propquest.m_strNumber );
						AfxMessageBox( szError );
						return FALSE;
					}
					
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
				
				propquest.m_vecSetDialog.push_back( nation_str_ex );
			}
			else
			if( scanner.Token == "state" )
			{
				NATION_STATE_EX nation_state;
				nation_state.m_vecStateSet.clear();
				
				scanner.GetToken(); // index

				nation_state.m_strIndex = scanner.token;

				scanner.GetToken(); // {
				scanner.GetToken();

				while( scanner.Token[0] != '}' )
				{
					if( scanner.Token == "SetDesc" || scanner.Token == "SetCond" || scanner.Token == "SetStatus" )
					{
						NATION_STR nation_str;
						nation_str.m_strTitle = scanner.Token;
						
						scanner.GetToken(); // (
						
						for( int i=0; i<LANG_MAX; i++ )
						{
							if( GetLangScript( scanner,  nation_str.awszMessage[i] ) == FALSE )
							{
								CString szError;
								szError.Format( "%s에 콤바가 없으요(%s)", nation_str.m_strTitle, propquest.m_strNumber );
								AfxMessageBox( szError );								
								return FALSE;
							}
							
							if( scanner.Token == ')' )
							{
								// LANG_MAX 보다 작게 읽었다...(언어추가하기 전의 파일이다)
								if( i+1 != LANG_MAX )
								{
									for( int kk=i+1; kk<LANG_MAX; kk++ )
									{
										CString str;
										str.Format( "%s 부분의 언어수가 LANG_MAX(%d)보다 작습니다.\n자동으로 나머지 언어를 추가합니다.\n\n파일명 : %s\n추가언어 : %s", 
											nation_str.m_strTitle, LANG_MAX-1, lpszFileName, g_szLanguage[kk] );
										//AfxMessageBox( str );
										nation_str.awszMessage[ kk ][0] = '\0';
									}
								}
								
								break;
							}
						}
						
						scanner.GetToken(); // ;

						nation_state.m_vecStateSet.push_back(nation_str);
						//nation_state.m_strDesc = nation_str;
					}
					else
					if( scanner.Token == "QuestItem" )
					{
						WCSTR wcstr;
						wcscpy( wcstr.m_wcstr, scanner.m_wszToken );
						scanner.GetLastFull();
						wcscat( wcstr.m_wcstr, scanner.m_wszToken );
						
						nation_state.m_vecQuestItem.push_back( wcstr );
					}

					scanner.GetToken(); // }
				}

				propquest.m_vecState.push_back( nation_state );
			}
			else
			{
				//AfxMessageBox( "알수없는 명령어" );
			}
			
			scanner.GetToken(); // ;
		} 
		
		vec_quest.push_back(propquest);
		scanner.GetToken();
	}
	
	
	return TRUE;	
}

BOOL CQuestDlg::Save( LPCTSTR lpszFileName )
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
	
	PROPQUEST propquest;
	
	for( int i=0; i<m_vecSrc.size(); i++ )
	{
		propquest = m_vecSrc[i];
		
		string.Format( _T("%s\r\n"), propquest.m_strNumber );
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		file.Write( wszBuff, wcslen( wszBuff) * 2 );
		
		file.PutWideString( "{\r\n" );

		// save SetTitle
		int nLangCount = 0;

		if( !propquest.m_Title.m_strTitle.IsEmpty() )
		{
			string.Format( _T("\t%s\r\n"), propquest.m_Title.m_strTitle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.PutWideString( "\t(\r\n" );
			
			for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t\t%s, " ), g_szLanguage[nLangCount] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );

				CString pl = '"';
				file.PutWideString( _T(pl) );
				
				file.Write( propquest.m_Title.awszMessage[nLangCount], wcslen(propquest.m_Title.awszMessage[nLangCount] ) * 2 );
				
				file.PutWideString( _T(pl) );
				
				if( nLangCount == LANG_MAX-1 )
					file.PutWideString( "\r\n" );
				else
					file.PutWideString( ",\r\n" );
			}
			
			file.PutWideString( "\t);\r\n" );
			file.PutWideString( "\r\n" );
		}

		// save SetNPCName
		if( !propquest.m_NpcName.m_strTitle.IsEmpty() )
		{
			string.Format( _T("\t%s\r\n"), propquest.m_NpcName.m_strTitle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.PutWideString( "\t(\r\n" );
			
			for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t%s, " ), g_szLanguage[nLangCount] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
				CString pl = '"';
				file.PutWideString( _T(pl) );
				
				file.Write( propquest.m_NpcName.awszMessage[nLangCount], wcslen(propquest.m_NpcName.awszMessage[nLangCount] ) * 2 );
				
				file.PutWideString( _T(pl) );
				
				if( nLangCount == LANG_MAX-1 )
					file.PutWideString( "\r\n" );
				else
					file.PutWideString( ",\r\n" );
			}
			
			file.PutWideString( "\t);\r\n" );
			file.PutWideString( "\r\n" );
		}
		
		// save SetPatrolZoneName
		if( !propquest.m_PatrolZoneName.m_strTitle.IsEmpty() )
		{
			string.Format( _T("\t%s\r\n"), propquest.m_PatrolZoneName.m_strTitle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.PutWideString( "\t(\r\n" );
			
			for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t\t%s, " ), g_szLanguage[nLangCount] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
				CString pl = '"';
				file.PutWideString( _T(pl) );
				
				file.Write( propquest.m_PatrolZoneName.awszMessage[nLangCount], wcslen(propquest.m_PatrolZoneName.awszMessage[nLangCount] ) * 2 );
				
				file.PutWideString( _T(pl) );
				
				if( nLangCount == LANG_MAX-1 )
					file.PutWideString( "\r\n" );
				else
					file.PutWideString( ",\r\n" );
			}
			
			file.PutWideString( "\t);\r\n" );
			file.PutWideString( "\r\n" );
		}

		// save setting
		if( propquest.m_vecSetting.size() )
		{
			file.PutWideString( "\t" );
			WCSTR wcstr = propquest.m_vecSetting[0];
			file.Write( wcstr.m_wcstr, wcslen(wcstr.m_wcstr) * 2 );
			file.PutWideString( "\r\n" );

			file.PutWideString( "\t{\r\n" );
			for( int k = 1; k < propquest.m_vecSetting.size(); k++ )
			{
				WCSTR wcstr = propquest.m_vecSetting[k];
				file.PutWideString( "\t\t" );
				file.Write( wcstr.m_wcstr, wcslen(wcstr.m_wcstr) * 2 );
				file.PutWideString( "\r\n" );
			}
			file.PutWideString( "\t}\r\n" );
			
			file.PutWideString( "\r\n" );
		}
		
		// save SetDialog
		for( int nDlgCount=0; nDlgCount<propquest.m_vecSetDialog.size(); nDlgCount++ )
		{
			NATION_STR_EX nation_str_ex;

			nation_str_ex = propquest.m_vecSetDialog[nDlgCount];

			string.Format( _T("\t%s\r\n"), nation_str_ex.m_strTitle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );

			file.PutWideString( "\t(\r\n" );

			string.Format( _T("\t\t%s,\r\n"), nation_str_ex.m_strIndex );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			
			for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t\t%s, " ), g_szLanguage[nLangCount] );
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

		file.PutWideString( "\r\n" );

		// save state
		for( int nStateCount=0; nStateCount<propquest.m_vecState.size(); nStateCount++ )
		{
			NATION_STATE_EX vecState;
			
			vecState = propquest.m_vecState[nStateCount];
			
			string.Format( _T( "\tstate %s\r\n" ), vecState.m_strIndex );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.PutWideString( "\t{\r\n" );

			
			// save QuestItem
			for( int nQuestItem = 0; nQuestItem < vecState.m_vecQuestItem.size(); nQuestItem++ )
			{
				WCSTR wcstr = vecState.m_vecQuestItem[nQuestItem];
				file.PutWideString( "\t\t" );
				file.Write( wcstr.m_wcstr, wcslen(wcstr.m_wcstr) * 2 );
				file.PutWideString( "\r\n" );
			}
			

			file.PutWideString( "\r\n" );
			
			// save SetDesc, SetStatus, SetCond

			for( int nnn = 0; nnn < vecState.m_vecStateSet.size(); nnn++ )
			{
				NATION_STR nation_str;
				nation_str = vecState.m_vecStateSet[nnn];
				
				string.Format( _T( "\t\t%s\r\n" ), nation_str.m_strTitle );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.PutWideString( "\t\t(\r\n" );
				
				for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
				{
					string.Format( _T( "\t\t\t%s, " ), g_szLanguage[nLangCount] );
					_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
					file.Write( wszBuff, wcslen( wszBuff) * 2 );
					
					CString pl = '"';
					file.PutWideString( _T(pl) );
					
					file.Write( nation_str.awszMessage[nLangCount], wcslen(nation_str.awszMessage[nLangCount] ) * 2 );
					
					file.PutWideString( _T(pl) );
					
					if( nLangCount == LANG_MAX-1 )
						file.PutWideString( "\r\n" );
					else
						file.PutWideString( ",\r\n" );
				}
				
				file.PutWideString( "\t\t);\r\n" );
			}
					
			/*
			NATION_STR nation_str;

			nation_str = vecState.m_strDesc;
			
			string.Format( _T( "\t\t%s\r\n" ), nation_str.m_strTitle );
			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
			file.Write( wszBuff, wcslen( wszBuff) * 2 );
			file.PutWideString( "\t\t(\r\n" );
			
			for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
			{
				string.Format( _T( "\t\t\t%s, " ), g_szLanguage[nLangCount] );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				
				CString pl = '"';
				file.PutWideString( _T(pl) );
				
				file.Write( nation_str.awszMessage[nLangCount], wcslen(nation_str.awszMessage[nLangCount] ) * 2 );
				
				file.PutWideString( _T(pl) );
				
				if( nLangCount == LANG_MAX-1 )
					file.PutWideString( "\r\n" );
				else
					file.PutWideString( ",\r\n" );
			}
			
			file.PutWideString( "\t\t);\r\n" );
			*/
			

			/*
			for( int nSetDesc = 0; nSetDesc<vecState.m_vecDesc.size(); nSetDesc++ )
			{
				NATION_STR nation_str;

				nation_str = vecState.m_vecDesc[nSetDesc];

				string.Format( _T( "\t\tSetDesc\r\n" ), nation_str.m_strTitle );
				_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
				file.Write( wszBuff, wcslen( wszBuff) * 2 );
				file.PutWideString( "\t\t(\r\n" );
				
				for( nLangCount = 0; nLangCount < LANG_MAX; nLangCount++ )
				{
					string.Format( _T( "\t\t\t%s, " ), g_szLanguage[nLangCount] );
					_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
					file.Write( wszBuff, wcslen( wszBuff) * 2 );
					
					CString pl = '"';
					file.PutWideString( _T(pl) );
					
					file.Write( nation_str.awszMessage[nLangCount], wcslen(nation_str.awszMessage[nLangCount] ) * 2 );
					
					file.PutWideString( _T(pl) );
					
					if( nLangCount == LANG_MAX-1 )
						file.PutWideString( "\r\n" );
					else
						file.PutWideString( ",\r\n" );
				}
				
				file.PutWideString( "\t\t);\r\n" );
			}
			/**/

			file.PutWideString( "\t}\r\n" );
		}
		file.PutWideString( "}\r\n" );
		file.PutWideString( "\r\n" );
	}
	
	return TRUE;
}


void CQuestDlg::OnQuestMstart() 
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
        "PropQuest Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
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

BOOL CQuestDlg::Merge(VEC_PROPQUEST& vecSrc, const VEC_PROPQUEST& vecDest)
{
	int nLang = m_Lang.GetCurSel();
	int nLangKor = m_LangKor.GetCurSel();
	
	for( int i=0; i<vecDest.size(); i++ )
	{	
		CString sss2 = vecDest[i].m_strNumber;

		for( int j=0; j<vecSrc.size(); j++ )
		{
			PROPQUEST pp = vecSrc[j];

			if( vecDest[i].m_strNumber.Find(vecSrc[j].m_strNumber) != -1 && vecDest[i].m_strNumber.GetLength() == vecSrc[j].m_strNumber.GetLength() )
			{
				// 내용이 있으면 복사, 없으면 갱신된 내용임
				if ( m_bSkip )
				{
					if( _wcsicmp( vecSrc[j].m_Title.awszMessage[0], vecDest[i].m_Title.awszMessage[0] ) == 0 )
						wcscpy( vecSrc[j].m_Title.awszMessage[nLangKor], vecDest[i].m_Title.awszMessage[nLang] );
					else
					{
						*vecSrc[j].m_Title.awszMessage[nLangKor] = 0;
						//wcscpy( vecSrc[j].m_Title.awszMessage[nLangKor], 0 );
					}
				}
				else
					wcscpy( vecSrc[j].m_Title.awszMessage[nLangKor], vecDest[i].m_Title.awszMessage[nLang] );

				if( !vecSrc[j].m_NpcName.m_strTitle.IsEmpty() )//&& !vecDest[i].m_NpcName.m_strTitle.IsEmpty() )
				{
					if ( m_bSkip )
					{
						if( _wcsicmp( vecSrc[j].m_NpcName.awszMessage[0], vecDest[i].m_NpcName.awszMessage[0] ) == 0 )
							wcscpy( vecSrc[j].m_NpcName.awszMessage[nLangKor], vecDest[i].m_NpcName.awszMessage[nLang] );
						else
						{
							*vecSrc[j].m_NpcName.awszMessage[nLangKor] = 0;
							//wcscpy( vecSrc[j].m_NpcName.awszMessage[nLangKor], 0 );
						}
					}
					else
						wcscpy( vecSrc[j].m_NpcName.awszMessage[nLangKor], vecDest[i].m_NpcName.awszMessage[nLang] );
				}
				
				if( !vecSrc[j].m_PatrolZoneName.m_strTitle.IsEmpty() )//&& !vecDest[i].m_NpcName.m_strTitle.IsEmpty() )
				{
					if ( m_bSkip )
					{
						if( _wcsicmp( vecSrc[j].m_PatrolZoneName.awszMessage[0], vecDest[i].m_PatrolZoneName.awszMessage[0] ) == 0 )
							wcscpy( vecSrc[j].m_PatrolZoneName.awszMessage[nLangKor], vecDest[i].m_PatrolZoneName.awszMessage[nLang] );
						else
						{
							*vecSrc[j].m_PatrolZoneName.awszMessage[nLangKor] = 0;
							//wcscpy( vecSrc[j].m_NpcName.awszMessage[nLangKor], 0 );
						}
					}
					else
						wcscpy( vecSrc[j].m_PatrolZoneName.awszMessage[nLangKor], vecDest[i].m_PatrolZoneName.awszMessage[nLang] );
				}
				
#ifdef _DEBUG
				int nCount1 = vecDest[i].m_vecSetDialog.size();
				int nCount2 = vecSrc[j].m_vecSetDialog.size();
#endif 
				
				for( int nSetDialog = 0; nSetDialog < vecDest[i].m_vecSetDialog.size(); nSetDialog++ )
				{
					NATION_STR_EX nation_str_ex;
					
					nation_str_ex = vecDest[i].m_vecSetDialog[nSetDialog];

					for( int nSetDialog2 = 0; nSetDialog2 < vecSrc[j].m_vecSetDialog.size(); nSetDialog2++ )
					{
						NATION_STR_EX nation_str_ex2;
						
						nation_str_ex2 = vecSrc[j].m_vecSetDialog[nSetDialog2];

						/*
						int nNum1 = atoi(nation_str_ex.m_strIndex);
						int nNum2 = atoi(nation_str_ex2.m_strIndex);
						
						if( nNum1 == nNum2 )
						*/
						if( nation_str_ex.m_strIndex == nation_str_ex2.m_strIndex )
						{
							// 내용이 있으면 복사, 없으면 갱신된 내용임
							if ( m_bSkip )
							{
								if( _wcsicmp( vecSrc[j].m_vecSetDialog[nSetDialog2].awszMessage[0], vecDest[i].m_vecSetDialog[nSetDialog].awszMessage[0] ) == 0 )
									wcscpy( vecSrc[j].m_vecSetDialog[nSetDialog2].awszMessage[nLangKor], vecDest[i].m_vecSetDialog[nSetDialog].awszMessage[nLang] );
								else
								{
									*vecSrc[j].m_vecSetDialog[nSetDialog2].awszMessage[nLangKor] = 0;
									//wcscpy( vecSrc[j].m_vecSetDialog[nSetDialog2].awszMessage[nLangKor], 0 );
								}
							}
							else
								wcscpy( vecSrc[j].m_vecSetDialog[nSetDialog2].awszMessage[nLangKor], vecDest[i].m_vecSetDialog[nSetDialog].awszMessage[nLang] );
							break;
						}
					}
				}

				for( int nStateCount=0; nStateCount<vecDest[i].m_vecState.size(); nStateCount++ )
				{
					NATION_STATE_EX vecState;
					
					vecState = vecDest[i].m_vecState[nStateCount];
					
					for( int nStateCount2 = 0; nStateCount2<vecSrc[j].m_vecState.size(); nStateCount2++ )
					{
						NATION_STATE_EX vecState2;
						
						vecState2 = vecSrc[j].m_vecState[nStateCount2];
						
						if( vecState.m_strIndex == vecState2.m_strIndex )
						{
							for( int nnn = 0; nnn < vecState.m_vecStateSet.size(); nnn++ )
							{
								NATION_STR nation_str1;
								nation_str1 = vecState.m_vecStateSet[nnn];

								for( int nnn2 = 0; nnn2 < vecState2.m_vecStateSet.size(); nnn2++ )
								{
									NATION_STR nation_str2;
									nation_str2 = vecState2.m_vecStateSet[nnn2];

									// 내용이 있으면 복사, 없으면 갱신된 내용임
									if( nation_str1.m_strTitle == nation_str2.m_strTitle )
									{
										if ( m_bSkip )
										{
											if( _wcsicmp( vecSrc[j].m_vecState[nStateCount2].m_vecStateSet[nnn2].awszMessage[0], vecDest[i].m_vecState[nStateCount].m_vecStateSet[nnn].awszMessage[0] ) == 0 )
												wcscpy( vecSrc[j].m_vecState[nStateCount2].m_vecStateSet[nnn2].awszMessage[nLangKor], vecDest[i].m_vecState[nStateCount].m_vecStateSet[nnn].awszMessage[nLang] );
											else
											{
												*vecSrc[j].m_vecState[nStateCount2].m_vecStateSet[nnn2].awszMessage[nLangKor] = 0;
												//wcscpy( vecSrc[j].m_vecState[nStateCount2].m_vecStateSet[nnn2].awszMessage[nLangKor], 0 );
											}
										}
										else
											wcscpy( vecSrc[j].m_vecState[nStateCount2].m_vecStateSet[nnn2].awszMessage[nLangKor], vecDest[i].m_vecState[nStateCount].m_vecStateSet[nnn].awszMessage[nLang] );
										
										break;
									}
								}
							}

							break;
						}
					}
				}

				/*
				for( int nStateCount=0; nStateCount<vecDest[i].m_vecState.size(); nStateCount++ )
				{
					NATION_STATE_EX vecState;
					
					vecState = vecDest[i].m_vecState[nStateCount];
					
					for( int nStateCount2 = 0; nStateCount2<vecSrc[j].m_vecState.size(); nStateCount2++ )
					{
						NATION_STATE_EX vecState2;
						
						vecState2 = vecSrc[j].m_vecState[nStateCount2];

						if( vecState.m_strIndex == vecState2.m_strIndex )
						{
							// 내용이 있으면 복사, 없으면 갱신된 내용임
							if ( m_bSkip )
							{
								if( _wcsicmp( vecSrc[j].m_vecState[nStateCount2].m_strDesc.awszMessage[0], vecDest[i].m_vecState[nStateCount].m_strDesc.awszMessage[0] ) == 0 )
									wcscpy( vecSrc[j].m_vecState[nStateCount2].m_strDesc.awszMessage[nLangKor], vecDest[i].m_vecState[nStateCount].m_strDesc.awszMessage[nLang] );
								else
									wcscpy( vecSrc[j].m_vecState[nStateCount2].m_strDesc.awszMessage[nLangKor], 0 );
							}
							else
								wcscpy( vecSrc[j].m_vecState[nStateCount2].m_strDesc.awszMessage[nLangKor], vecDest[i].m_vecState[nStateCount].m_strDesc.awszMessage[nLang] );
							
							break;
						}
					}
				}
				*/
						
				break;
			}
		}
	}

	return TRUE;
}

BOOL CQuestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bSkip = TRUE;
	
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuestDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	m_bSkip = !m_bSkip;	
}
