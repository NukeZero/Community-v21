// LoginServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dploginsrvr.h"
#include "dpdatabaseclient.h"
#include "dpcoreclient.h"

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include "user.h"


//#include <vld/vld.h>
#ifdef __gameguard
#include <gameguard/ggsrv30.h>
#ifdef	_DLL
#pragma comment( lib, "ggsrv30lib_x86_MD.lib" )
#else
#pragma comment( lib, "ggsrv30lib_x86_MT.lib" )
#endif
#endif



extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_dpCoreClient;
extern	CUserMng			g_UserMng;


#define MAX_LOADSTRING 100

FLMyTrace	g_MyTrace;
HINSTANCE	hInst;	// current instance
TCHAR		szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR		szWindowClass[MAX_LOADSTRING];	// The title bar text
HWND		hMainWnd;


ATOM	MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance(void);
BOOL	Script(LPCSTR lpszFileName);

//////////
#define NPROTECT_UPDATE_TIMER	1234
#define XTRAP_UPDATE_TIMER		1235


//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	FFL_DUMP
//////////////////////////////////////////////////////////////////////////

#include "../_Common/FLXTrap_Server.h"

int APIENTRY WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _XTRAP	
	FLXTrap_Server::GetInstance();
#endif

	FLDumpInstaller::Install(FLCommonDumper::GetInstance());
#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::InstallFLAssertHooker();
#endif

	if (!g_xFlyffConfig->LoadFlyffConfig("config.ini"))
		return false;


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LOGINSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int nRet = 0;
	if (InitInstance(hInstance, nCmdShow) == TRUE)
	{
		FLUIMessageLoop kMessageLoop;
		nRet = kMessageLoop.Run();
	}

	ExitInstance();

#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::UninstallFLAssertHooker();
#endif

	FLDumpInstaller::Uninstall();
	FLLOG_FLUSH();

	return nRet;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_LOGINSERVER);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_LOGINSERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	if (!Script("LoginServer.ini"))
		return FALSE;

	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	SetTimer(hWnd, 0, 60 * 1000, NULL);

	int x = 880, y = 606;
	SetWindowPos(hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW);

	g_MyTrace.Initialize(hWnd, "SquireD", RGB(0x00, 0x00, 0x00), RGB(0xff, 0xff, 0xff));

	CTime time = CTime::GetCurrentTime();
	g_MyTrace.Add(0, FALSE, "%s", time.Format("%Y/%m/%d %H:%M:%S"));
	g_MyTrace.AddLine('\0');

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	FLLOG_ADD_NETWORK(NETWORK_LOG_NAME);
	if (InitializeNetLib() == FALSE)
		return FALSE;

	if (!g_dpDBClient.ConnectToServer(g_lpDBAddr, PN_DBSRVR_0, FLNormalProtocol::GetInstance()))
	{
		return FALSE;
	}

	if (!g_dpCoreClient.ConnectToServer(g_lpCoreAddr, PN_CORESRVR + 2, FLNormalProtocol::GetInstance()))
	{
		return FALSE;
	}
	g_dpCoreClient.QueryTickCount();	// block,

	if (!g_dpLoginSrvr.StartServer(g_szClientBindIP, PN_LOGINSRVR, FLXORProtocol::GetInstance(), FALSE))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616
#ifdef __gameguard
	if (g_dpLoginSrvr.IsUseGameGuard() == true)
	{
//		DWORD dw = InitGameguardAuth( NULL, 5, true, 0x3 );
//		DWORD dw = InitGameguardAuth( NULL, 1000, true, 0 );
//		DWORD dw = InitGameguardAuth( NULL, 1000, true, 0x03 );
		const UINT32 dw = InitCSAuth3("./");
		if (dw != ERROR_SUCCESS)
		{
			char s[128];
			FLSPrintf(s, _countof(s), "InitGameguardAuth//%d - check ggauth*.dll", dw);
			MessageBox(hWnd, s, "", MB_OK);
			return FALSE;
		}
		SetTimer(hWnd, 1, SEC(1), NULL);
		SetTimer(hWnd, NPROTECT_UPDATE_TIMER, MIN(5), NULL);	//5분 주기 권장
	}
	//	END100616
	//////////////////////////////////////////////////////////////////////////
#endif

#ifdef _xtrap
	if (FLXTrap_Server::GetInstance().IsActive() == TRUE)
	{
		SetTimer(hWnd, XTRAP_UPDATE_TIMER, FLXTrap_Server::XTRAP_UPDATE_INTERVAL, NULL);
	}
#endif



	return TRUE;
}

void ExitInstance(void)
{
	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616
#ifdef __gameguard
	if (g_dpLoginSrvr.IsUseGameGuard() == true)
	{
		CloseCSAuth3();
		//CleanupGameguardAuth();
	}
#endif
	//	END100616
	//////////////////////////////////////////////////////////////////////////

	g_dpLoginSrvr.DeleteDPObject();
	g_dpCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();
	g_UserMng.Free();
	MEMPOOLER_DESTROY(CUser);

	UninitializeNetLib();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;//, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;


	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616
	switch (message)
	{
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			//wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				case IDM_DO_UPDATE_GAMEGUARD_AUTH_DLL:
				{
					//AddAuthProtocol( "./ggauth01.dll" );
					break;
				}
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
		{
			const WPARAM wTimerID = wParam;
			switch (wTimerID)
			{
				case	0:
				{
					g_UserMng.DestroyAbnormalPlayer();
				}
				break;
#ifdef __gameugard
				case	1:
				{
					if (g_dpLoginSrvr.IsUseGameGuard() == true)
					{
						g_UserMng.CheckAll();
					}
				}
				break;
				case	NPROTECT_UPDATE_TIMER:
				{
					//GGAuthUpdateTimer();
				}
				break;
#endif
#ifdef _xtrap
				case	XTRAP_UPDATE_TIMER:
				{
					const DWORD	dwCurrTick = ::GetTickCount();
					if (FLXTrap_Server::GetInstance().IsActive() == TRUE && FLXTrap_Server::GetInstance().IsUpdateTime(dwCurrTick) == TRUE)
						g_UserMng.XTrapUpdate();
				}
				break;
#endif

			}
			break;
		}
		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint(hdc);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
		{
			KillTimer(hMainWnd, 0);

			if (g_dpLoginSrvr.IsUseGameGuard() == true)
			{
				KillTimer(hMainWnd, 1);
			}

			PostQuitMessage(0);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	//	END100616
	//////////////////////////////////////////////////////////////////////////

	return 0;
}

BOOL Script(LPCSTR lpszFileName)
{
	CScanner s;

	if (s.Load(lpszFileName))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token == ";")
			{
				s.GetToken();
				continue;
			}

			if (s.Token == "CLIENT_BIND_IP")
			{
				s.GetToken();
				FLStrcpy(g_szClientBindIP, _countof(g_szClientBindIP), s.Token);
			}
			else if (s.Token == "DB")
			{
				s.GetToken();
				FLStrcpy(g_lpDBAddr, _countof(g_lpDBAddr), s.Token);
			}
			else if (s.Token == "Core")
			{
				s.GetToken();
				FLStrcpy(g_lpCoreAddr, _countof(g_lpCoreAddr), s.Token);
			}
			else if (s.Token == "AddCache")
			{
				s.GetToken();	// (
				s.GetToken();	// ip
				g_dpLoginSrvr.AddCahce(s.Token);
				s.GetToken();	// )
			}
#ifdef __gameguard
			else if (s.Token == "NPROTECT")
			{
				if (FLXTrap_Server::GetInstance().IsActive() == TRUE)
				{
					FLERROR_LOG(PROGRAM_NAME, _T("[ NPROTECT : XTRAP LOADED ]"));
					return false;
				}
				g_dpLoginSrvr.SetUseGameGuard();
			}
#endif
#ifdef __xtrap
			else if (s.Token == "XTRAP")
			{
				if (g_dpLoginSrvr.IsUseGameGuard() == TRUE)
				{
					FLERROR_LOG(PROGRAM_NAME, _T("[ XTRAP : NPROTECT LOADED ]"));
					return FALSE;
				}
#ifndef _DEBUG
				FLXTrap_Server::GetInstance().SetActive(TRUE);
				const BOOL bRet = FLXTrap_Server::GetInstance().Start();
				if (bRet == FALSE)
				{
					FLERROR_LOG(PROGRAM_NAME, _T("[ FAIL - XTRAP START ]"));
					return FALSE;
				}
#endif //_DEBUG
			}
#endif
			s.GetToken();
		}
		return true;
	}

	FLERROR_LOG(PROGRAM_NAME, _T("Can't open file, %s."), lpszFileName);
	return false;
};