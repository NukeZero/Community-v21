#include "StdAfx.h"
#include "Webbox.h"
#include <network/FLDPMng.h>
#include "xUtil.h"

HANDLE g_hMutex = nullptr;
static HHOOK g_hHook;

extern LPTSTR WEB_ADDRESS_DEFAULT;
extern LPTSTR WEB_POSTDATA;

#ifdef	_DEBUG
#include <logging/FLLogPubLoggingDialog.h>
FLLogPubLoggingDialog g_cLogPubDialog;
#endif
#include <FLIncludeAllLibrary.h>
#ifdef _USE_XTRAP
#include "XTrapLib.h"
#endif


bool ParseCmdLine(const char* lpCmdLine)
{
	TCHAR szArg1[64] = {0, }; 
	TCHAR szArg2[64] = {0, }; 
	TCHAR szArg3[64] = {0, }; 
	TCHAR szArg4[MAX_ENCRYPT_PWD] = {0, }; 


	TCHAR szArg5[64] = {0, };
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	_stscanf_s( lpCmdLine, "%s %s %s %s %s",
		szArg1, _countof( szArg1 ),
		szArg2, _countof( szArg2 ),
		szArg3, _countof( szArg3 ),
		szArg4, _countof( szArg4 ),
		szArg5, _countof( szArg5 ) );
#else
	_stscanf( lpCmdLine, "%s %s %s %s %s", szArg1, szArg2, szArg3, szArg4, szArg5 );
#endif

#ifdef __MAINSERVER
	if( _strcmpi( szArg1, Settings::NEUZ_BHASH) )
		return false;
#endif

	const char szPortal[LANG_MAX][3][100]	=
	{
		"localhost",	"",	"",					// LANG_KOR
		"",	"",	"",								// LANG_USA
		"hangame.co.jp",	"",	"",				// LANG_JAP
		"",	"",	"",								// LANG_CHI
		"flyffonline.ini3.co.th",	"",	"",		// LANG_THA
		"www.omg.com.tw/fff/", "", "",			// LANG_TWN
		"",	"",	"",								// LANG_GER
		"http://www.zoomby.net/",	"",	"",		// LANG_SPA
		"",	"",	"",								// LANG_FRE
		"",	"",	"",								// LANG_HK
		"",	"",	"",								// LANG_POR
		"",	"",	"",								// LANG_VTN
		"", "", "",								// LANG_RUS
		"", "", "",								// LANG_ID
		"", "", "",								// LANG_PHP
	};
	//	BEGINTODO::::::
	if (lstrlen(szPortal[g_xFlyffConfig->GetMainLanguage()][0]) > 0 && lstrcmpi(szArg2, szPortal[g_xFlyffConfig->GetMainLanguage()][0]) == 0)
	{
		g_Neuz.m_bThroughPortal = true;
		g_xFlyffConfig->DeactivatePasswordEncryption(szArg2);
		g_Neuz.SetAccountInfo(szArg3, szArg4, szArg5);
	}

	return true;
}

void EnableFloatException()
{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	unsigned int dwCurrent = 0;
	_controlfp_s( &dwCurrent, 0, 0 );
	const unsigned int dwNew = dwCurrent & ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	_controlfp_s(nullptr, dwNew, MCW_EM );
#else
	int cw = _controlfp( 0, 0 );
	cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	_controlfp( cw, MCW_EM );
#endif
}

#include "VersionHelpers.h"
BOOL InitApp()
{
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DADAPTER_IDENTIFIER9 Identifier;
	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &Identifier);
	if (!IsWindowsVistaOrGreater() && Identifier.VendorId != 4098)
		EnableFloatException();
	SAFE_RELEASE(pD3D);


	TIMECAPS tc;
	const UINT uPeriod = (TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc))) ? tc.wPeriodMin : 1;  
	timeBeginPeriod( uPeriod ); 

	xSRand(timeGetTime());
	srand(timeGetTime());
	InitUPS();

	FLLOG_ADD_NETWORK(NETWORK_LOG_NAME);
	if (!InitializeNetLib())
		return false;

#ifndef _DEBUG
	InitEH();
	const char* szCPU = GetCPUInfo();
	FLERROR_LOG(PROGRAM_NAME, szCPU);
#endif

	return true;
}

void SkipPatch()
{
	if (FindWindow(_T("saida"), nullptr))
	{
		if (g_xFlyffConfig->IsAbleMultiExecution() == false)
		{
			AfxMessageBox("Already running Flyff client!!");
			::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0);
		}
	}

	g_hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, "neuz");
	if (!g_hMutex)
	{
		g_hMutex = CreateMutex(nullptr, TRUE, "neuz");
		if (g_hMutex)
			ReleaseMutex(g_hMutex);
	}
	else
	{
		if (g_xFlyffConfig->IsAbleMultiExecution() == false)
		{
			FLERROR_LOG(PROGRAM_NAME, _T("Double Execution Error!"));
			::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0);
		}
	}
}

int APIENTRY WinMain(HINSTANCE__* hInstance, [[maybe_unused]] HINSTANCE__* hPrevInstance, char* lpCmdLine, [[maybe_unused]] int nShowCmd)
{
	if (!g_xFlyffConfig->LoadFlyffConfig("config.ini"))
		return false;

#ifdef _DEBUG
	afxTraceEnabled = TRUE;
#endif

	g_Option.Init();
	g_Neuz.LoadOption();
	SkipPatch();

	g_Neuz.m_bGPotatoAuth = (g_xFlyffConfig->GetMainLanguage() == LANG_GER || g_xFlyffConfig->GetMainLanguage() == LANG_FRE || g_xFlyffConfig->GetMainLanguage() == LANG_USA);
	InitWebGlobalVar();

	if (!ParseCmdLine(lpCmdLine))
		return 0;

	InitApp();
	if (FAILED(g_Neuz.Create(hInstance)))
		return 0;

#ifdef _USE_XTRAP
	XTrap_C_KeepAlive( );
#endif

#ifdef	_DEBUG
	if (g_cLogPubDialog.Create(PROGRAM_NAME, g_Neuz.GetSafeHwnd(), _T("FLResCommon.dll")) == true)
		FLLOG_ADD_PUBLISHER(PROGRAM_NAME, &g_cLogPubDialog);
#endif

	const INT nResult = g_Neuz.Run();

#ifdef	_DEBUG
	g_cLogPubDialog.Destroy();
	FLLOG_DEL_PUBLISHER( PROGRAM_NAME, &g_cLogPubDialog );
#endif

	FLLOG_FLUSH();

	return nResult;
}
