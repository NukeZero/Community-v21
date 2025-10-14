// AccountServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

//#include <vld/vld.h>

#include "resource.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include "account.h"
#include "dpadbill.h"
#include <..\Resource\Lang.h>


#include "FLASDBManager.h"
#include "FLAdbillProtocol.h"

#include "../_Common/FLServerDescription.h"
#include "ConnectionIPCheck.h"

extern	CDPSrvr		g_dpSrvr;
extern	CDPDBSrvr	g_dpDbSrvr;
extern	CAccountMng	g_AccountMng;

extern	char	g_szCertifierBindIP[16];
extern	char	g_szDatabaseBindIP[16];
extern	char	g_szAdbillBindIP[16];
extern	char	g_szBillingIP[16];


#define MAX_LOADSTRING 100

// Global Variables:
FLMyTrace		g_MyTrace;
HINSTANCE		hInst;								// current instance
HWND			hMainWnd;
TCHAR			szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR			szWindowClass[MAX_LOADSTRING];		// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void	ExitInstance( void );
BOOL	Script( LPCTSTR lpszFileName );
BOOL	CALLBACK	MaxConnDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
BOOL	CALLBACK	ToggleDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
BOOL	CALLBACK	SetTimeDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );



// //////////////////////////////////////////////////////////////////////////
// //	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
// //	ENDTEST100111	FFL_DUMP
// //////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iomanip>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
 	FLDumpInstaller::Install( FLCommonDumper::GetInstance() );
#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::InstallFLAssertHooker();
#endif

	if (!g_xFlyffConfig->LoadFlyffConfig("config.ini"))
		return false;
	
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ACCOUNTSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int nRet = 0;
	if (InitInstance(hInstance, nCmdShow))
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
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ACCOUNTSERVER);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_ACCOUNTSERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // Store instance handle in our global variable
	hMainWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return false;

	HMENU__* hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu, IDM_OPT_INTERNAL, MF_CHECKED);
	EnableMenuItem(hMenu, IDM_OPT_INTERNAL, MF_DISABLED | MF_GRAYED);
	g_dpSrvr.m_bCheckAddr = true;

	const int x = 400;
	const int y = 416;
	SetWindowPos(hWnd, nullptr, x, y, 400, 416, SWP_SHOWWINDOW);

	FLLOG_ADD_NETWORK(NETWORK_LOG_NAME);
	if (!InitializeNetLib())
		return false;

	::srand(timeGetTime());
	if (!Script("AccountServer.ini"))
		return false;

	if (!CConnectionIPCheck::GetInstance()->Load("pmttd.ini")) 
		FLWARNING_LOG(PROGRAM_NAME, _T("FAILED LOAD pmttd.ini"));
	g_xASDBManager->CreateDbWorkers();

	if (!g_dpSrvr.LoadIPCut("IPCut.ini"))
		FLTRACE_LOG(PROGRAM_NAME, _T("LoadIPCut FALSE"));

	g_MyTrace.Initialize(hWnd, "SquireD", RGB(0x00, 0x00, 0x00), RGB(0xff, 0xff, 0xff));
	const CTime time = CTime::GetCurrentTime();
	g_MyTrace.Add(0, FALSE, "%s", time.Format("%Y/%m/%d %H:%M:%S"));
	g_MyTrace.AddLine('\0');

	if (!g_dpSrvr.StartServer(g_szCertifierBindIP, PN_ACCOUNTSRVR_0, FLNormalProtocol::GetInstance())
		|| !g_dpDbSrvr.StartServer(g_szDatabaseBindIP, PN_ACCOUNTSRVR_1, FLNormalProtocol::GetInstance())
		|| !CDPAdbill::GetInstance()->StartServer(g_szAdbillBindIP, PN_ADBILL, FLAdbillProtocol::GetInstance())
		)
	{
		AfxMessageBox("Unable to start server");
		return false;
	}

	SetTimer(hWnd, IDT_SENDPLAYERCOUNT, 1000 * 60, nullptr);
	SetTimer(hWnd, IDT_TIME_CHECKADDR, 1000 * 30, nullptr);
	if (g_xFlyffConfig->GetMainLanguage() == LANG_THA)
		SetTimer(hWnd, IDT_PREVENT_EXCESS, 1000 * 60, nullptr);

	SetTimer(hWnd, IDT_BUYING_INFO, 500, nullptr);
	if (!g_xASDBManager->AllOff())
		return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return true;
}

BOOL Script( LPCTSTR lpszFileName )
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
			else if (s.Token == "TEST")
			{
				HMENU hMenu = GetMenu(hMainWnd);
				CheckMenuItem(hMenu, IDM_OPT_EXTERNAL, MF_CHECKED);
				EnableMenuItem(hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED);
				CheckMenuItem(hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED);
				EnableMenuItem(hMenu, IDM_OPT_INTERNAL, MF_ENABLED);
				g_dpSrvr.m_bCheckAddr = false;
				char lpString[MAX_LOADSTRING];
#ifdef _DEBUG
				FLSPrintf(lpString, _countof(lpString), "%s - debug", szTitle);
#else	// _DEBUG
				FLSPrintf(lpString, _countof(lpString), "%s - release", szTitle);
#endif	// _DEBUG
				SetWindowText(hMainWnd, lpString);
			}
			else if (s.Token == "AddTail")
			{
				T_SERVER_DESC kServerDesc;

				s.GetToken();	// (
				kServerDesc.dwParentID = static_cast<DWORD>(s.GetNumber());
				s.GetToken();	// ,
				kServerDesc.dwMyID = static_cast<DWORD>(s.GetNumber());
				s.GetToken();	// ,
				s.GetToken();
				FLStrcpy(kServerDesc.szName, _countof(kServerDesc.szName), s.Token);
				s.GetToken();	// ,
				s.GetToken();
				FLStrcpy(kServerDesc.szAddress, _countof(kServerDesc.szAddress), s.Token);
				s.GetToken();	// ,
				kServerDesc.bAdultOnly = static_cast<bool>(s.GetNumber() != 0);
				s.GetToken();	// ,
				kServerDesc.lCanConnection = static_cast<long>(s.GetNumber());

				s.GetToken();	// ,
				kServerDesc.lMaxConnection = static_cast<long>(s.GetNumber());
				s.GetToken();	// )

				if (s.Token != ")" && s.Token == ",")
				{
					kServerDesc.bTransformCharacter = static_cast<bool>(s.GetNumber() != 0);
					s.GetToken();	// )
				}

				g_pServerDescription->Add(&kServerDesc);
			}

			else if (s.Token == "CERTIFIER_BIND_IP")
			{
				s.GetToken();
				FLStrcpy(g_szCertifierBindIP, _countof(g_szCertifierBindIP), s.Token);
			}
			else if (s.Token == "DATABASE_BIND_IP")
			{
				s.GetToken();
				FLStrcpy(g_szDatabaseBindIP, _countof(g_szDatabaseBindIP), s.Token);
			}
			else if (s.Token == "ADBILL_BIND_IP")
			{
				s.GetToken();
				FLStrcpy(g_szAdbillBindIP, _countof(g_szAdbillBindIP), s.Token);
			}
			else if (s.Token == "MAX")
			{
				g_dpSrvr.m_nMaxConn = s.GetNumber();
			}
			else if (s.Token == "DSN_NAME_LOGIN")
			{
				s.GetToken();
				FLStrcpy(DSN_NAME_LOGIN, _countof(DSN_NAME_LOGIN), s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_LOGIN")
			{
				s.GetToken();
				FLStrcpy(DB_ADMIN_ID_LOGIN, _countof(DB_ADMIN_ID_LOGIN), s.Token);
			}
			else if (s.Token == "DSN_NAME_LOG")
			{
				s.GetToken();
				FLStrcpy(DSN_NAME_LOG, _countof(DSN_NAME_LOG), s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_LOG")
			{
				s.GetToken();
				FLStrcpy(DB_ADMIN_ID_LOG, _countof(DB_ADMIN_ID_LOG), s.Token);
			}
			else if (s.Token == "DSN_NAME_BILLING")
			{
				s.GetToken();
				FLStrcpy(DSN_NAME_BILLING, _countof(DSN_NAME_BILLING), s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_BILLING")
			{
				s.GetToken();
				FLStrcpy(DB_ADMIN_ID_BILLING, _countof(DB_ADMIN_ID_BILLING), s.Token);
			}
			else if (s.Token == "NOT_RELOADPRO")
			{
				g_dpSrvr.m_bReloadPro = FALSE;
			}
			else if (s.Token == "DB_PWD_LOGIN")
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken(s.Token, g_xASDBManager->m_szLoginPWD);
#else
				FLStrcpy(g_xASDBManager->m_szLoginPWD, _countof(g_xASDBManager->m_szLoginPWD), s.Token);
#endif
				FLTRACE_LOG(PROGRAM_NAME, _T("%s"), g_xASDBManager->m_szLoginPWD);
			}
			else if (s.Token == "DB_PWD_LOG")
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken(s.Token, g_xASDBManager->m_szLogPWD);
#else
				FLStrcpy(g_xASDBManager->m_szLogPWD, _countof(g_xASDBManager->m_szLogPWD), s.Token);
#endif
				FLTRACE_LOG(PROGRAM_NAME, _T("%s"), g_xASDBManager->m_szLogPWD);
			}
			else if (s.Token == "AddChannel")
			{
				s.GetToken();
				g_dpDbSrvr.m_vecstrChannelAccount.push_back(s.Token);
			}
			else if (s.Token == "BILLING_IP")
			{
				s.GetToken();
				FLStrcpy(g_szBillingIP, _countof(g_szBillingIP), s.Token);
			}

			s.GetToken();
		}

#ifdef __MAINSERVER
		if (g_szBillingIP[0] == '\0')
		{
			FLERROR_LOG(PROGRAM_NAME, _T("[ NOT EXIST BILLING IP. ]"));
			return false;
		}
#endif
		return true;
	}

	FLERROR_LOG(PROGRAM_NAME, _T("Can't open file %s"), lpszFileName);
	return false;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;//, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_TIMER:
			{
				switch( wParam )
				{
				case IDT_SENDPLAYERCOUNT:
					g_dpDbSrvr.SendPlayerCount();
					break;
				case IDT_PREVENT_EXCESS:
					g_AccountMng.PreventExcess();	
					break;
				case IDT_TIME_CHECKADDR:
					{
						if( g_dpSrvr.m_bCheckAddr && g_AccountMng.IsTimeCheckAddr() )
						{
							HMENU hMenu		= GetMenu( hWnd );
							CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_CHECKED );
							EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED );
							CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED );
							EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_ENABLED );
							g_dpSrvr.m_bCheckAddr	= false;
						}
					}
					break;
				case IDT_BUYING_INFO:
					g_xBuyingInfoManager->Process();
					break;
				}
			}
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			//wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_OPT_INTERNAL:
					{
						HMENU hMenu		= GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_CHECKED );
						EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_DISABLED | MF_GRAYED );
						CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_UNCHECKED );
						EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_ENABLED );
						g_dpSrvr.m_bCheckAddr	= true;
						break;
					}
				case IDM_OPT_EXTERNAL:
					{
						HMENU hMenu		= GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_CHECKED );
						EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED );
						CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED );
						EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_ENABLED );
						g_dpSrvr.m_bCheckAddr	= false;
						break;
					}
				case IDM_OPT_MAX_CONN:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_MAX_CONN ), hWnd, MaxConnDlgProc );
						break;
					}
				case IDM_OPT_TOGGLE:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_TOGGLE ), hWnd, ToggleDlgProc );
						break;
					}
				case IDM_OPT_SETTIME:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_TIME ), hWnd, SetTimeDlgProc );
						break;
					}
				case IDM_OPT_INTERNAL_IP:
					{
//						g_dpSrvr.LoadAddrPmttd( "pmttd.ini" );
						CConnectionIPCheck::GetInstance()->Load( "pmttd.ini" );
						g_dpSrvr.LoadIPCut( "IPCut.ini" );
						break;
					}
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			KillTimer( hMainWnd, IDT_SENDPLAYERCOUNT );
			if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
				KillTimer( hMainWnd, IDT_PREVENT_EXCESS );

			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void ExitInstance( void )
{

	g_dpSrvr.DeleteDPObject();
	g_dpDbSrvr.DeleteDPObject();
	CDPAdbill::GetInstance()->DeleteDPObject();

	g_AccountMng.Clear();
	MEMPOOLER_DESTROY( CAccount );
	
	g_xASDBManager->CloseDbWorkers();

	UninitializeNetLib();
}

BOOL CALLBACK ToggleDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM /*lParam*/ )
{
	switch( iMessage )
	{
		case WM_INITDIALOG:
			{
				SetDlgItemText( hDlg, IDC_EDIT1, "Parent" );
				SetDlgItemText( hDlg, IDC_EDIT2, "ID" );
				break;
			}
		case WM_COMMAND:
			switch( wParam )
			{
				case IDOK:
					{
						char lpString[MAX_PATH]	= { 0, };
						DWORD dwParent	= 0;
						DWORD dwID	= 0;
						GetDlgItemText( hDlg, IDC_EDIT3, lpString, MAX_PATH );
						dwParent	= atoi( lpString );
						GetDlgItemText( hDlg, IDC_EDIT4, lpString, MAX_PATH );
						dwID	= atoi( lpString );
						
						g_dpSrvr.RemoteServerConnectionMode( dwParent, dwID, true );

						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDCANCEL:
					{
						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDC_BUTTON_DISABLE:
					{
						char lpString[MAX_PATH]	= { 0, };
						DWORD dwParent	= 0;
						DWORD dwID	= 0;
						GetDlgItemText( hDlg, IDC_EDIT3, lpString, MAX_PATH );
						dwParent	= atoi( lpString );
						GetDlgItemText( hDlg, IDC_EDIT4, lpString, MAX_PATH );
						dwID	= atoi( lpString );

						g_dpSrvr.RemoteServerConnectionMode( dwParent, dwID, false );
						
						EndDialog( hDlg, 0 );
						return TRUE;
					}
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK MaxConnDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM /*lParam*/ )
{
	switch( iMessage )
	{
		case WM_INITDIALOG:
			{
				char lpString[16];
				FLIntToString( (int)g_dpSrvr.m_nMaxConn, lpString, _countof( lpString ), 10 );
				SetDlgItemText( hDlg, IDC_EDIT_MAX_CONN, lpString );
				break;
			}
		case WM_COMMAND:
			switch( wParam )
			{
				case IDOK:
					{
						char lpString[16];
						GetDlgItemText( hDlg, IDC_EDIT_MAX_CONN, lpString, 16 );
						int nMaxConn	= atoi( lpString );
						if( nMaxConn > -1 )
							g_dpSrvr.m_nMaxConn		= nMaxConn;
						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDCANCEL:
					{
						EndDialog( hDlg, 0 );
						return TRUE;
					}
			}
			break;
	}
	return FALSE;
}

void PrintTimeDlg( HWND hDlg )
{
	char lpString[16];
	FLSPrintf( lpString, _countof( lpString ), "%d", g_AccountMng.m_nYear );
	SetDlgItemText( hDlg, IDC_EDIT_YEAR, lpString );
	FLSPrintf( lpString, _countof( lpString ), "%d", g_AccountMng.m_nMonth );
	SetDlgItemText( hDlg, IDC_EDIT_MONTH, lpString );
	FLSPrintf( lpString, _countof( lpString ), "%d", g_AccountMng.m_nDay );
	SetDlgItemText( hDlg, IDC_EDIT_DAY, lpString );
	FLSPrintf( lpString, _countof( lpString ), "%d", g_AccountMng.m_nHour );
	SetDlgItemText( hDlg, IDC_EDIT_HOUR, lpString );
	FLSPrintf( lpString, _countof( lpString ), "%d", g_AccountMng.m_nMinute );
	SetDlgItemText( hDlg, IDC_EDIT_MINUTE, lpString );
}

BOOL SetTimeDlg( HWND hDlg )
{
	int nYear, nMonth, nDay, nHour, nMinute;
	char lpString[16];
	GetDlgItemText( hDlg, IDC_EDIT_YEAR, lpString, 16 );
	nYear = atoi( lpString );
	GetDlgItemText( hDlg, IDC_EDIT_MONTH, lpString, 16 );
	nMonth = atoi( lpString );
	if( nMonth <= 0 || 12 < nMonth )
	{
		AfxMessageBox( "Error Retry \"Month : 1 ~ 12\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_DAY, lpString, 16 );
	nDay = atoi( lpString );
	if( nDay <= 0 || 31 < nDay )
	{
		AfxMessageBox( "Error Retry \"Day : 1 ~ 31\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_HOUR, lpString, 16 );
	nHour = atoi( lpString );
	if( nHour < 0 || 23 < nHour )
	{
		AfxMessageBox( "Error Retry \"Hour : 0 ~ 23\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_MINUTE, lpString, 16 );
	nMinute = atoi( lpString );
	if( nMinute < 0 || 59 < nMinute )
	{
		AfxMessageBox( "Error Retry \"Minute : 0 ~ 59\"" );
		return FALSE;
	}
	g_AccountMng.m_nYear	= nYear;
	g_AccountMng.m_nMonth	= nMonth;
	g_AccountMng.m_nDay		= nDay;
	g_AccountMng.m_nHour	= nHour;
	g_AccountMng.m_nMinute	= nMinute;
	return TRUE;	
}

BOOL CALLBACK SetTimeDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );

	switch( iMessage )
	{
	case WM_INITDIALOG:
		{
			PrintTimeDlg( hDlg );
			break;
		}
	case WM_COMMAND:
		switch( wParam )
		{
		case IDOK:
			{
				if( SetTimeDlg( hDlg ) == FALSE )
				{
					return TRUE;
				}
				EndDialog( hDlg, 0 );
				return TRUE;
			}
		case IDCANCEL:
			{
				EndDialog( hDlg, 0 );
				return TRUE;
			}
		case IDCLEAR:
			{
				CTime tTime = CTime::GetCurrentTime();
				g_AccountMng.m_nYear = tTime.GetYear() - 1;
				g_AccountMng.m_nMonth = tTime.GetMonth();
				g_AccountMng.m_nDay = tTime.GetDay();
				g_AccountMng.m_nHour = tTime.GetHour();
				g_AccountMng.m_nMinute = tTime.GetMinute();
				PrintTimeDlg( hDlg );
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}
