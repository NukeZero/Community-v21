// CacheServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

//#include <vld/vld.h>

#include "resource.h"
#include "DPCoreClient.h"
#include "DPClient.h"
#include "DPCacheSrvr.h"

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include "player.h"
#include <..\Resource\Lang.h>



#define MAX_LOADSTRING 100

extern	CDPCoreClient	g_DPCoreClient;
extern	CDPClientArray	g_DPClientArray;
extern	CDPCacheSrvr	g_DPCacheSrvr;


FLMyTrace				g_MyTrace;

HINSTANCE				hInst;
TCHAR					szTitle[MAX_LOADSTRING];
TCHAR					szWindowClass[MAX_LOADSTRING];
char					g_szCoreAddr[16];
char					g_szClientBindIP[16] = { 0, };
HWND					hMainWnd;


ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCSTR lpszFileName );



//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	FFL_DUMP
//////////////////////////////////////////////////////////////////////////



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );


	//////////////////////////////////////////////////////////////////////////
	//	BEGINTEST100111	FFL_DUMP
 	FLDumpInstaller::Install( FLCommonDumper::GetInstance() );

#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::InstallFLAssertHooker();
#endif

// #ifdef __MAINSERVER
// 	FLBugTrapper::Install( PROGRAM_NAME );
// #else
// 	FLBugTrapper::Install( PROGRAM_NAME, true );
// #endif // __MAINSERVER
	//	ENDTEST100111	FFL_DUMP
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// mirchang_100914 load config file

	if( g_xFlyffConfig->LoadFlyffConfig( "config.ini" ) == false )
	{
		return FALSE;
	}

	// mirchang_100914 load config file
	//////////////////////////////////////////////////////////////////////////

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CACHESERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int nRet = 0;
	if( InitInstance( hInstance, nCmdShow ) == TRUE )
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CACHESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CACHESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	SetTimer( hMainWnd, 3, SEC( 60 ), NULL );	// 10(s)

	int x = 480, y = 606;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( !Script( "CacheServer.ini" ) )
		return FALSE;

	if( !g_DPCoreClient.ConnectToServer( g_szCoreAddr, PN_CORESRVR + 1, FLNormalProtocol::GetInstance() ) )
		return FALSE;
	
	if( !g_DPCacheSrvr.StartServer( g_szClientBindIP, g_xFlyffConfig->GetCachePort(), FLXORProtocol::GetInstance(), FALSE ) )
		return FALSE;


	g_MyTrace.AddLine( '-' );
	char lpszIpAddr[16];
	g_DPCoreClient.GetPlayerAddr( DPID_SERVERPLAYER, lpszIpAddr, _countof( lpszIpAddr ) );

	g_MyTrace.Add( FLMyTrace::Key( "main" ), FALSE, "%s", lpszIpAddr );
	g_MyTrace.AddLine( '-' );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExitInstance( void )
{
	g_DPCoreClient.DeleteDPObject();
	g_DPClientArray.Free();
	g_DPCacheSrvr.DeleteDPObject();

	UninitializeNetLib();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;//, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			//wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				switch( wParam )
				{
					case 3:
						CPlayerMng::Instance()->DestroyGarbage();
						break;
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
//			RECT rt;
//			GetClientRect(hWnd, &rt);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, 3 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

BOOL Script( LPCSTR lpszFileName )
{
	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == "CLIENT_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szClientBindIP, _countof( g_szClientBindIP ), s.Token );
			}
			else if( s.Token == "Core" )
			{
				s.GetToken();
				FLStrcpy( g_szCoreAddr, _countof( g_szCoreAddr ), s.Token );
			}
			s.GetToken();
		}
		return TRUE;
	}
	FLERROR_LOG( PROGRAM_NAME, _T( "Can't open file, %s." ), lpszFileName );
	return FALSE;
};