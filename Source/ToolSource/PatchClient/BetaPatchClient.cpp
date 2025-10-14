// BetaPatchClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BetaPatchClient.h"
#include "BetaPatchClientDlg.h"
#include "PatchManager.h"

#include <common/FLFileUtil.h>
#include <FLIncludeAllLibrary.h>

extern CBetaPatchClientDlg *g_pDlg;
extern CPatchManager		g_PatchManager;
/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientApp

BEGIN_MESSAGE_MAP(CBetaPatchClientApp, CWinApp)
	//{{AFX_MSG_MAP(CBetaPatchClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientApp construction

CBetaPatchClientApp::CBetaPatchClientApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBetaPatchClientApp object

CBetaPatchClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientApp initialization





CHECK_TYPE CBetaPatchClientApp::CheckSingleInstance()
{
	//	Neuz 뮤텍스 검사
	HANDLE hNeuzMutex = OpenMutex( MUTEX_ALL_ACCESS, TRUE, "neuz" );
	if( hNeuzMutex != NULL )
	{
		//멀티 띄우기 허용 (5차부터), 뉴즈가 띄워져 있으면 이미 패치받았다고 가정한다.
/*
#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
		//무인자 일 경우 홈페이지로 연결
		CString strCmdLine;
		strCmdLine.Format("%s", AfxGetApp()->m_lpCmdLine);
		if(strCmdLine.GetLength() == 0 || strCmdLine == "1")
		{
			return CHECK_TRUE;
		}
		else
			return CHECK_SKIP;

//		else
//			AfxMessageBox( strCmdLine );
#endif //__LINK_PORTAL
#else // LANG_KOR
*/
		return CHECK_SKIP;
//#endif // LANG_KOR
	}
	
	//	패치 클라이언트 뮤텍스
	HANDLE hMutex	= CreateMutex( NULL, TRUE, "masquerade" );
	DWORD dwError	= GetLastError();
	if( hMutex )
		ReleaseMutex( hMutex );
	if( dwError == ERROR_ALREADY_EXISTS )
	{
		AfxMessageBox( IDS_RUNNING_PATCH );
		return CHECK_FALSE;
	}

	return CHECK_TRUE;
}

// 예전에는 레지스트리의 패스정보를 이용했지만 2006.04부터 이를 이용하지 않게 수정 
BOOL CBetaPatchClientApp::InitPath()
{
	g_PatchManager.SetCurrentLocalDirectory();
	return TRUE;
}


// DirectX 검사 VerSion이 9 미만이면 실행이 안됨
BOOL CBetaPatchClientApp::CheckDirectXVersion()
{
	DWORD dwVersion, dwRevision;
	if (DirectXSetupGetVersion(&dwVersion, &dwRevision)) 
	{
		int HighVersion = HIWORD(dwVersion);
		int LowVersion  = LOWORD(dwVersion);
		if( LowVersion >= 9 ) 
			return TRUE;
	}

	return FALSE;
}

void CBetaPatchClientApp::RunClient()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset( &si, 0, sizeof( STARTUPINFO ) );
	si.cb = sizeof( STARTUPINFO );

	TCHAR szBuffer[2048];
	int nCount = FLSPrintf( szBuffer, _countof( szBuffer ), "Neuz.exe %s %s", KEY, AfxGetApp()->m_lpCmdLine );
	if( nCount <= 0 )
		return;

	if( CreateProcess( NULL, szBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
	{
		if( AfxGetMainWnd() )
			AfxGetMainWnd()->PostMessage( WM_CLOSE );
	}
	else
	{
		char szError[256];
		FLSPrintf( szError, _countof( szError ), "ErrorNumber: %d Neuz.exe Run Error", GetLastError() );
		AfxMessageBox( szError );
	}				
}

void	CBetaPatchClientApp::DeleteResourceFiles( const TCHAR * pszFileName )
{
	TCHAR	szFileName[_MAX_PATH];
	memset( szFileName, 0, sizeof(szFileName) );

	FLStrcpy( szFileName, _countof(szFileName), pszFileName );

	SHFILEOPSTRUCT FileOp = {0};
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;       // 삭제 속성 설정
	FileOp.pFrom = NULL;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI; // 확인메시지가 안뜨도록 설정
	FileOp.fAnyOperationsAborted = false;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = szFileName;

//	m_szCurrentDirectory;
	int	nRetVal = SHFileOperation(&FileOp);       // 삭제 작업

	if( nRetVal != 0 && nRetVal != 1026 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "DeleteResourceFile is Failed. [%s] [%d]" ), szFileName, nRetVal );
	}

	return;
}

BOOL CBetaPatchClientApp::InitInstance()
{
	AfxEnableControlContainer();

	BOOL bSkipPatch = FALSE;

	CHECK_TYPE type = CheckSingleInstance();
	switch( type )
	{
	case CHECK_FALSE:
		return FALSE;
	case CHECK_SKIP:
		RunClient();
		return FALSE;
	}
	
	::DeleteFile( "NewFlyff.exe" );
	::DeleteFile( _T( "config.ini" ) );
	
	if( InitPath() == FALSE )				// 레지스트리에서 실행 Path를 얻어 지정한다.
		return FALSE;

	if( CheckDirectXVersion() == FALSE )	
	{
		AfxMessageBox( IDS_DIRECTX_INSTALL );	// DirectX 9가 설치되어 있지 않습니다.
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN110418	Resrouce.txt 파일에 맞게 Merge 폴더 내 파일 삭제


	DeleteResourceFiles( "Client\\faq.inc" );
	DeleteResourceFiles( "Client\\help.inc" );
	DeleteResourceFiles( "Client\\instantHelp.inc" );
	DeleteResourceFiles( "Client\\sound.inc" );
	DeleteResourceFiles( "Client\\tip.inc" );
	DeleteResourceFiles( "Client\\treeHelp.inc" );
	DeleteResourceFiles( "Client\\PatchClient.inc" );
	DeleteResourceFiles( "Client\\Guide.inc" );



	DeleteResourceFiles( "Char\\*.tga" );
	DeleteResourceFiles( "Char\\*.scc" );
	DeleteResourceFiles( "Char\\*.db" );
	DeleteResourceFiles( "Char\\Char.res" );
	//	Char\Char_a.res { wildcard "Char\Char_a*.*" } ~ Char_z.res { wildcard "Char\Char_z*.*" }

	DeleteResourceFiles( "Icon\\*.bak" );
	DeleteResourceFiles( "Icon\\*.bmp" );
	DeleteResourceFiles( "Icon\\*.db" );
	DeleteResourceFiles( "Icon\\*.dds" );
	DeleteResourceFiles( "Icon\\*.inc" );
	DeleteResourceFiles( "Icon\\*.jpg" );
	DeleteResourceFiles( "Icon\\*.scc" );
	DeleteResourceFiles( "Icon\\*.tga" );
	DeleteResourceFiles( "Icon\\Icon.res" );
	//	Icon\Icon_a.res { wildcard "Icon\a*.*" } ~ Icon_z.res { wildcard "Icon\z*.*" }

	DeleteResourceFiles( "Item\\*.db" );
	DeleteResourceFiles( "Item\\*.dds" );
	DeleteResourceFiles( "Item\\*.scc" );
	DeleteResourceFiles( "Item\\*.tga" );
	DeleteResourceFiles( "Item\\Item.res" );
	//	Item\item_itm_a.res { wildcard "Item\itm_a*.*" } ~ item_itm_z.res { wildcard "Item\itm_z*.*" }
	//	Item\item_item.res { wildcard "Item\item*.*" }
	//	Item\item_map.res { wildcard "Item\map*.*" }
	//	Item\item_skill.res { wildcard "Item\skill*.*" }

	DeleteResourceFiles( "Music\\*.db" );
	DeleteResourceFiles( "Music\\*.gz" );
	DeleteResourceFiles( "Music\\*.mp3" );
	DeleteResourceFiles( "Music\\*.scc" );
//	DeleteResourceFiles( "Music\\*.bgm" );
//	DeleteResourceFiles( "Music\\*.ogg" );

	DeleteResourceFiles( "SFX\\*.dds" );
	DeleteResourceFiles( "SFX\\*.inc" );
	DeleteResourceFiles( "SFX\\*.sfx" );
	DeleteResourceFiles( "SFX\\*.tga" );
	DeleteResourceFiles( "SFX\\*.scc" );
	DeleteResourceFiles( "SFX\\*.db" );
	DeleteResourceFiles( "SFX\\sfx.res" );
	//	SFX\sfx_a.res { wildcard "sfx\a*.*"	} ~ sfx_z.res { wildcard "sfx\z*.*"	}

	DeleteResourceFiles( "SFX\\scripts\\*.lua" );
	DeleteResourceFiles( "SFX\\scripts\\*.scc" );
	DeleteResourceFiles( "SFX\\scripts\\*.db" );
	DeleteResourceFiles( "SFX\\scripts\\sfxscripts.res" );
	//	SFX\script\sfxscripts.res" { wildcard "sfx\scripts\*.*" }

	DeleteResourceFiles( "SFX\\Texture\\*.dds" );
	DeleteResourceFiles( "SFX\\Texture\\*.sfx" );
	DeleteResourceFiles( "SFX\\Texture\\*.psd" );
	DeleteResourceFiles( "SFX\\Texture\\*.scc" );
	DeleteResourceFiles( "SFX\\Texture\\*.db" );
	DeleteResourceFiles( "SFX\\Texture\\sfxtex.res" );
	//	SFX\Texture\sfxtex_a.res - SFX\Texture\sfxtex_z.res

	DeleteResourceFiles( "World\\TextureMid\\*.dds" );
	DeleteResourceFiles( "World\\TextureMid\\*.scc" );
	DeleteResourceFiles( "World\\TextureMid\\*.db" );
	DeleteResourceFiles( "World\\TextureMid\\wtexm.res" );
	//	World\TextureMid\wtexm_a.res { wildcard "World\textureMid\a*.*" } ~ World\TextureMid\wtexm_a.res { wildcard "World\textureMid\a*.*" }

	DeleteResourceFiles( "World\\TextureLow\\*.dds" );
	DeleteResourceFiles( "World\\TextureLow\\*.scc" );
	DeleteResourceFiles( "World\\TextureLow\\*.db" );
	DeleteResourceFiles( "World\\TextureLow\\wtexl.res" );
	//	World\TextureLow\wtexl_a.res { wildcard "World\textureLow\a*.*" } ~ World\TextureLow\wtexl_z.res { wildcard "World\textureLow\z*.*" }

	DeleteResourceFiles( "World\\Texture\\*.dds" );
	DeleteResourceFiles( "World\\Texture\\*.scc" );
	DeleteResourceFiles( "World\\Texture\\*.db" );
	DeleteResourceFiles( "World\\Texture\\wtex.res" );
	//	World\Texture\wtex_a.res { wildcard "World\texture\a*.*" } ~ World\Texture\wtex_z.res { wildcard "World\texture\z*.*" }

	//////////////////////////////////////////////////////////////////////////
	DeleteResourceFiles( "Weather\\*.bmp" );
	DeleteResourceFiles( "Weather\\*.dds" );
	DeleteResourceFiles( "Weather\\*.tga" );
	DeleteResourceFiles( "Weather\\*.scc" );
	DeleteResourceFiles( "Weather\\*.db" );
	DeleteResourceFiles( "Weather\\Weather.res" );

	DeleteResourceFiles( "world\\wdMadrigal\\*.cnt" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.csv" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.db" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.dds" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.dyo" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.rgn*" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.scc" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.txt" );
	DeleteResourceFiles( "world\\wdMadrigal\\*.wld" );
	DeleteResourceFiles( "world\\wdMadrigal\\MiniMap" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\wdMadrigal\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdEvent01\\*.cnt" );
	DeleteResourceFiles( "world\\WdEvent01\\*.csv" );
	DeleteResourceFiles( "world\\WdEvent01\\*.db" );
	DeleteResourceFiles( "world\\WdEvent01\\*.dds" );
	DeleteResourceFiles( "world\\WdEvent01\\*.dyo" );
	DeleteResourceFiles( "world\\WdEvent01\\*.rgn" );
	DeleteResourceFiles( "world\\WdEvent01\\*.scc" );
	DeleteResourceFiles( "world\\WdEvent01\\*.txt" );
	DeleteResourceFiles( "world\\WdEvent01\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdEvent01\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuFlMas\\*.cnt" );
	DeleteResourceFiles( "world\\DuFlMas\\*.csv" );
	DeleteResourceFiles( "world\\DuFlMas\\*.db" );
	DeleteResourceFiles( "world\\DuFlMas\\*.dds" );
	DeleteResourceFiles( "world\\DuFlMas\\*.dyo" );
	DeleteResourceFiles( "world\\DuFlMas\\*.rgn" );
	DeleteResourceFiles( "world\\DuFlMas\\*.scc" );
	DeleteResourceFiles( "world\\DuFlMas\\*.txt" );
	DeleteResourceFiles( "world\\DuFlMas\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuFlMas\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuDaDk\\*.cnt" );
	DeleteResourceFiles( "world\\DuDaDk\\*.csv" );
	DeleteResourceFiles( "world\\DuDaDk\\*.db" );
	DeleteResourceFiles( "world\\DuDaDk\\*.dds" );
	DeleteResourceFiles( "world\\DuDaDk\\*.dyo" );
	DeleteResourceFiles( "world\\DuDaDk\\*.rgn" );
	DeleteResourceFiles( "world\\DuDaDk\\*.scc" );
	DeleteResourceFiles( "world\\DuDaDk\\*.txt" );
	DeleteResourceFiles( "world\\DuDaDk\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuDaDk\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdKebaras\\*.cnt" );
	DeleteResourceFiles( "world\\WdKebaras\\*.csv" );
	DeleteResourceFiles( "world\\WdKebaras\\*.db" );
	DeleteResourceFiles( "world\\WdKebaras\\*.dds" );
	DeleteResourceFiles( "world\\WdKebaras\\*.dyo" );
	DeleteResourceFiles( "world\\WdKebaras\\*.rgn" );
	DeleteResourceFiles( "world\\WdKebaras\\*.scc" );
	DeleteResourceFiles( "world\\WdKebaras\\*.txt" );
	DeleteResourceFiles( "world\\WdKebaras\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdKebaras\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuBear\\*.cnt" );
	DeleteResourceFiles( "world\\DuBear\\*.csv" );
	DeleteResourceFiles( "world\\DuBear\\*.db" );
	DeleteResourceFiles( "world\\DuBear\\*.dds" );
	DeleteResourceFiles( "world\\DuBear\\*.dyo" );
	DeleteResourceFiles( "world\\DuBear\\*.rgn" );
	DeleteResourceFiles( "world\\DuBear\\*.scc" );
	DeleteResourceFiles( "world\\DuBear\\*.txt" );
	DeleteResourceFiles( "world\\DuBear\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuBear\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuKrr\\*.cnt" );
	DeleteResourceFiles( "world\\DuKrr\\*.csv" );
	DeleteResourceFiles( "world\\DuKrr\\*.db" );
	DeleteResourceFiles( "world\\DuKrr\\*.dds" );
	DeleteResourceFiles( "world\\DuKrr\\*.dyo" );
	DeleteResourceFiles( "world\\DuKrr\\*.rgn" );
	DeleteResourceFiles( "world\\DuKrr\\*.scc" );
	DeleteResourceFiles( "world\\DuKrr\\*.txt" );
	DeleteResourceFiles( "world\\DuKrr\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuKrr\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuMuscle\\*.cnt" );
	DeleteResourceFiles( "world\\DuMuscle\\*.csv" );
	DeleteResourceFiles( "world\\DuMuscle\\*.db" );
	DeleteResourceFiles( "world\\DuMuscle\\*.dds" );
	DeleteResourceFiles( "world\\DuMuscle\\*.dyo" );
	DeleteResourceFiles( "world\\DuMuscle\\*.rgn" );
	DeleteResourceFiles( "world\\DuMuscle\\*.scc" );
	DeleteResourceFiles( "world\\DuMuscle\\*.txt" );
	DeleteResourceFiles( "world\\DuMuscle\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuMuscle\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.cnt" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.csv" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.db" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.dds" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.dyo" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.rgn" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.scc" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.txt" );
	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdGuildWar1To1\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdGuildWar\\*.cnt" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.csv" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.db" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.dds" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.dyo" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.rgn" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.scc" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.txt" );
	DeleteResourceFiles( "world\\WdGuildWar\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdGuildWar\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuSaTemple\\*.cnt" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.csv" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.db" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.dds" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.dyo" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.rgn" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.scc" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.txt" );
	DeleteResourceFiles( "world\\DuSaTemple\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuSaTemple\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.cnt" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.csv" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.db" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.dds" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.dyo" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.rgn" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.scc" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.txt" );
	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuSaTempleBoss\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdVolcane\\*.cnt" );
	DeleteResourceFiles( "world\\WdVolcane\\*.csv" );
	DeleteResourceFiles( "world\\WdVolcane\\*.db" );
	DeleteResourceFiles( "world\\WdVolcane\\*.dds" );
	DeleteResourceFiles( "world\\WdVolcane\\*.dyo" );
	DeleteResourceFiles( "world\\WdVolcane\\*.rgn" );
	DeleteResourceFiles( "world\\WdVolcane\\*.scc" );
	DeleteResourceFiles( "world\\WdVolcane\\*.txt" );
	DeleteResourceFiles( "world\\WdVolcane\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdVolcane\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdVolcaneRed\\*.cnt" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.csv" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.db" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.dds" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.dyo" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.rgn" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.scc" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.txt" );
	DeleteResourceFiles( "world\\WdVolcaneRed\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdVolcaneRed\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.cnt" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.csv" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.db" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.dds" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.dyo" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.rgn" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.scc" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.txt" );
	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdVolcaneYellow\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdArena\\*.cnt" );
	DeleteResourceFiles( "world\\WdArena\\*.csv" );
	DeleteResourceFiles( "world\\WdArena\\*.db" );
	DeleteResourceFiles( "world\\WdArena\\*.dds" );
	DeleteResourceFiles( "world\\WdArena\\*.dyo" );
	DeleteResourceFiles( "world\\WdArena\\*.rgn" );
	DeleteResourceFiles( "world\\WdArena\\*.scc" );
	DeleteResourceFiles( "world\\WdArena\\*.txt" );
	DeleteResourceFiles( "world\\WdArena\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdArena\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdArena_1\\*.cnt" );
	DeleteResourceFiles( "world\\WdArena_1\\*.csv" );
	DeleteResourceFiles( "world\\WdArena_1\\*.db" );
	DeleteResourceFiles( "world\\WdArena_1\\*.dds" );
	DeleteResourceFiles( "world\\WdArena_1\\*.dyo" );
	DeleteResourceFiles( "world\\WdArena_1\\*.rgn" );
	DeleteResourceFiles( "world\\WdArena_1\\*.scc" );
	DeleteResourceFiles( "world\\WdArena_1\\*.txt" );
	DeleteResourceFiles( "world\\WdArena_1\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdArena_1\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven01\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven01\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven01\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven02\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven02\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven02\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven03\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven03\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven03\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven04\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven04\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven04\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven05\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven05\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven05\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven06\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven06\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven06\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdHeaven06_1\\*.cnt" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.csv" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.db" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.dds" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.dyo" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.rgn" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.scc" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.txt" );
	DeleteResourceFiles( "world\\WdHeaven06_1\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdHeaven06_1\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdCisland\\*.cnt" );
	DeleteResourceFiles( "world\\WdCisland\\*.csv" );
	DeleteResourceFiles( "world\\WdCisland\\*.db" );
	DeleteResourceFiles( "world\\WdCisland\\*.dds" );
	DeleteResourceFiles( "world\\WdCisland\\*.dyo" );
	DeleteResourceFiles( "world\\WdCisland\\*.rgn" );
	DeleteResourceFiles( "world\\WdCisland\\*.scc" );
	DeleteResourceFiles( "world\\WdCisland\\*.txt" );
	DeleteResourceFiles( "world\\WdCisland\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdCisland\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdMiniroom\\*.cnt" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.csv" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.db" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.dds" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.dyo" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.rgn" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.scc" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.txt" );
	DeleteResourceFiles( "world\\WdMiniroom\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdMiniroom\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuOminous\\*.cnt" );
	DeleteResourceFiles( "world\\DuOminous\\*.csv" );
	DeleteResourceFiles( "world\\DuOminous\\*.db" );
	DeleteResourceFiles( "world\\DuOminous\\*.dds" );
	DeleteResourceFiles( "world\\DuOminous\\*.dyo" );
	DeleteResourceFiles( "world\\DuOminous\\*.rgn" );
	DeleteResourceFiles( "world\\DuOminous\\*.scc" );
	DeleteResourceFiles( "world\\DuOminous\\*.txt" );
	DeleteResourceFiles( "world\\DuOminous\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuOminous\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuOminous_1\\*.cnt" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.csv" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.db" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.dds" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.dyo" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.rgn" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.scc" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.txt" );
	DeleteResourceFiles( "world\\DuOminous_1\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuOminous_1\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdQuiz\\*.cnt" );
	DeleteResourceFiles( "world\\WdQuiz\\*.csv" );
	DeleteResourceFiles( "world\\WdQuiz\\*.db" );
	DeleteResourceFiles( "world\\WdQuiz\\*.dds" );
	DeleteResourceFiles( "world\\WdQuiz\\*.dyo" );
	DeleteResourceFiles( "world\\WdQuiz\\*.rgn" );
	DeleteResourceFiles( "world\\WdQuiz\\*.scc" );
	DeleteResourceFiles( "world\\WdQuiz\\*.txt" );
	DeleteResourceFiles( "world\\WdQuiz\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdQuiz\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.cnt" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.csv" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.db" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.dds" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.dyo" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.rgn" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.scc" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.txt" );
	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdGuildhousesmall\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.cnt" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.csv" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.db" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.dds" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.dyo" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.rgn" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.scc" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.txt" );
	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\Wdguildhousemiddle\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.cnt" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.csv" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.db" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.dds" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.dyo" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.rgn" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.scc" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.txt" );
	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\Wdguildhouselarge\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DudreadfulCave\\*.cnt" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.csv" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.db" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.dds" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.dyo" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.rgn" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.scc" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.txt" );
	DeleteResourceFiles( "world\\DudreadfulCave\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DudreadfulCave\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuRustia\\*.cnt" );
	DeleteResourceFiles( "world\\DuRustia\\*.csv" );
	DeleteResourceFiles( "world\\DuRustia\\*.db" );
	DeleteResourceFiles( "world\\DuRustia\\*.dds" );
	DeleteResourceFiles( "world\\DuRustia\\*.dyo" );
	DeleteResourceFiles( "world\\DuRustia\\*.rgn" );
	DeleteResourceFiles( "world\\DuRustia\\*.scc" );
	DeleteResourceFiles( "world\\DuRustia\\*.txt" );
	DeleteResourceFiles( "world\\DuRustia\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuRustia\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuRustia_1\\*.cnt" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.csv" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.db" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.dds" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.dyo" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.rgn" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.scc" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.txt" );
	DeleteResourceFiles( "world\\DuRustia_1\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuRustia_1\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuBehamah\\*.cnt" );
	DeleteResourceFiles( "world\\DuBehamah\\*.csv" );
	DeleteResourceFiles( "world\\DuBehamah\\*.db" );
	DeleteResourceFiles( "world\\DuBehamah\\*.dds" );
	DeleteResourceFiles( "world\\DuBehamah\\*.dyo" );
	DeleteResourceFiles( "world\\DuBehamah\\*.rgn" );
	DeleteResourceFiles( "world\\DuBehamah\\*.scc" );
	DeleteResourceFiles( "world\\DuBehamah\\*.txt" );
	DeleteResourceFiles( "world\\DuBehamah\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuBehamah\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdRartesia\\*.cnt" );
	DeleteResourceFiles( "world\\WdRartesia\\*.csv" );
	DeleteResourceFiles( "world\\WdRartesia\\*.db" );
	DeleteResourceFiles( "world\\WdRartesia\\*.dds" );
	DeleteResourceFiles( "world\\WdRartesia\\*.dyo" );
	DeleteResourceFiles( "world\\WdRartesia\\*.rgn" );
	DeleteResourceFiles( "world\\WdRartesia\\*.scc" );
	DeleteResourceFiles( "world\\WdRartesia\\*.txt" );
	DeleteResourceFiles( "world\\WdRartesia\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdRartesia\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\DuKalgas\\*.cnt" );
	DeleteResourceFiles( "world\\DuKalgas\\*.csv" );
	DeleteResourceFiles( "world\\DuKalgas\\*.db" );
	DeleteResourceFiles( "world\\DuKalgas\\*.dds" );
	DeleteResourceFiles( "world\\DuKalgas\\*.dyo" );
	DeleteResourceFiles( "world\\DuKalgas\\*.rgn" );
	DeleteResourceFiles( "world\\DuKalgas\\*.scc" );
	DeleteResourceFiles( "world\\DuKalgas\\*.txt" );
	DeleteResourceFiles( "world\\DuKalgas\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuKalgas\\*.lnd" );
//#endif

	DeleteResourceFiles( "world\\WdColosseum\\*.cnt" );
	DeleteResourceFiles( "world\\WdColosseum\\*.csv" );
	DeleteResourceFiles( "world\\WdColosseum\\*.db" );
	DeleteResourceFiles( "world\\WdColosseum\\*.dds" );
	DeleteResourceFiles( "world\\WdColosseum\\*.dyo" );
	DeleteResourceFiles( "world\\WdColosseum\\*.rgn" );
	DeleteResourceFiles( "world\\WdColosseum\\*.scc" );
	DeleteResourceFiles( "world\\WdColosseum\\*.txt" );
	DeleteResourceFiles( "world\\WdColosseum\\*.wld" );
//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\WdColosseum\\*.lnd" );
//#endif

//#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
//	DeleteResourceFiles( "world\\DuUpresia\\*.lnd" );
//#endif

#if	  defined(__MAINSERVER) || defined(__TESTSERVER_EXTERNAL) || defined(__TESTSERVER_KOREA_16) || defined(__TESTSERVER_INTERNAL_CURR_VER)
	FLDeleteFile( "./world/", "*.lnd" );
#endif

	DeleteResourceFiles( "world\\Wdconvert" );

	DeleteResourceFiles( "world\\WdLux" );
	  

	//	END110418	Resrouce.txt 파일에 맞게 Merge 폴더 내 파일 삭제
	//////////////////////////////////////////////////////////////////////////


	CBetaPatchClientDlg dlg;
	m_pMainWnd = &dlg;
	g_pDlg = &dlg;

	dlg.DoModal();

	FLLOG_FLUSH();
	return FALSE;
}
