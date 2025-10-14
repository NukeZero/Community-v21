// Certifiier.cpp : Defines the entry point for the application.
#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

//#include <vld/vld.h>

#include "resource.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include "dbmanager.h"
extern	CDbManager	g_DbManager;
#include "user.h"


FLMyTrace	g_MyTrace;

#define MAX_LOADSTRING 100

////////////////////////////////////////////////////////////////////////////////
class Heartbeat
{
public:
	Heartbeat( const char* name );
	virtual	~Heartbeat();
	void	Init();
	void	Run();
private:
	HANDLE	_heartbeat;
	const	std::string	_name;
};
Heartbeat hb( "10" );
////////////////////////////////////////////////////////////////////////////////

// Global Variables:
HINSTANCE hInst;	// current instance
TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text
HWND	hMainWnd;
char	g_szClientBindIP[16] = { 0, };
char	lpConnection[16];
//  [10/22/2014 ChangSeop.Kim]
std::string	g_strInGameShopURL;
//  [10/22/2014 ChangSeop.Kim]
extern	CDPCertifier	g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;
extern	std::string	g_strWebCertURL_GPotato;
extern	std::string	g_strWebCertURL_NotGPotato;

//////////////////////////////////////////////////////////////////////////
// 인도네시아 인증 서버 정보
extern	char	IDN_AUTH_SERVER_IP[64];
extern	WORD	IDN_AUTH_SERVER_PORT;
extern	BYTE	IDN_AUTH_CHANNEL_NUMBER;

//////////////////////////////////////////////////////////////////////////
//	미국 인증 웹페이지 정보
extern	char	USA_AUTH_WEB_URL[64];
extern	char	USA_AUTH_WEB_PAGE[64];
extern	int		USA_AUTH_PORT;

//////////////////////////////////////////////////////////////////////////
//	필리핀 인증 웹페이지 정보
char	PHILIPPINES_AUTH_WEB_URL[64];
char	PHILIPPINES_AUTH_WEB_PAGE[64];
int		PHILIPPINES_AUTH_PORT;
int		PHILIPPINES_AUTH_PORT_IPBONUS;

ATOM	MyRegisterClass(HINSTANCE hInstance);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance( void );
BOOL	Script( LPCTSTR lpszFileName );
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);



//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	DUMP
//////////////////////////////////////////////////////////////////////////



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     /*lpCmdLine*/,
                     int       nCmdShow)
{
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
	LoadString(hInstance, IDC_CERTIFIIER, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CERTIFIIER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CERTIFIERSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst	= hInstance;	// Store instance handle in our global variable

	hMainWnd	= hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	/*
	HMENU hMenu	= GetMenu( hWnd );
	CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_CHECKED );
	EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_DISABLED | MF_GRAYED );
	*/
	g_fInternal	= TRUE;

	int x = 800, y = 416;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( Script( "Certifier.ini" ) == FALSE )
		return FALSE;

//	if( CQuery::EnableConnectionPooling() )
//		FLTRACE_LOG( PROGRAM_NAME, _T( "EnableConnectionPooling" ) );

	if( g_DbManager.CreateDbWorkers() == FALSE )
	{
		AfxMessageBox( "g_DbManager.CreateDbWorkers odbc connect timeout." ); 
		return FALSE;
	}

	g_DbManager.LoadEveSchoolAccount();

	if( g_dpCertifier.StartServer( g_szClientBindIP, PN_CERTIFIER, FLXORProtocol::GetInstance() ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Certifier Start Fail. BindIP[%s]" ), g_szClientBindIP );

		return FALSE;
	}

	if( FALSE == g_dpAccountClient.ConnectToServer( lpConnection, PN_ACCOUNTSRVR_0, FLNormalProtocol::GetInstance() ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "AccountClient Connect Fail. AccountIP[%s]" ), lpConnection );

		return FALSE;
	}

	SetTimer( hWnd, 0, SEC( 10 ), NULL );

	return TRUE;
}

void ExitInstance( void )
{
	g_dpCertifier.DeleteDPObject();
	g_dpAccountClient.DeleteDPObject();
	g_DbManager.DestroyDbWorkers();
	
	UninitializeNetLib();
}

BOOL Script( LPCTSTR lpszFileName )
{
	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == ";" )
			{
				s.GetToken();
				continue;
			}
			else if( s.Token == "CLIENT_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szClientBindIP, _countof( g_szClientBindIP ), s.Token );
			}
			else if( s.Token == "Account" )
			{
				s.GetToken();
				FLStrcpy( lpConnection, _countof( lpConnection ), s.Token );
			}
			else if( s.Token == "LIMIT" )
			{
				g_dpCertifier.SetLimitation( s.GetNumber() );
			}
			else if( s.Token == "DSN_NAME_LOGIN" )
			{
				s.GetToken();
				g_DbManager.SetDBConnectString( LOGIN_DSN_NAME, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_LOGIN" )
			{
				s.GetToken();
				g_DbManager.SetDBConnectString( LOGIN_DB_ADMIN_ID, s.Token );
			}
			else if( s.Token == "DB_PWD_LOGIN" )
			{
				s.GetToken();
				TCHAR szPWD[256] = { 0, };
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken( s.Token, szPWD );
				g_DbManager.SetDBConnectString(LOGIN_PASSWORD, szPWD);
#else
				g_DbManager.SetDBConnectString(LOGIN_PASSWORD, s.Token);
#endif

			}
			else if( s.Token == "HEARTBEAT" )
			{
				hb.Init();
			}
			else if( s.Token == "WebCertURL_GPotato" )
			{
				s.GetToken();
				g_strWebCertURL_GPotato = s.Token;
			}
			else if( s.Token == "WebCertURL_NotGPotato" )
			{
				s.GetToken();
				g_strWebCertURL_NotGPotato = s.Token;
			}
			else if( s.Token == "USA_AUTH_WEB_URL" )
			{
				s.GetToken();
				FLStrcpy( USA_AUTH_WEB_URL, _countof( USA_AUTH_WEB_URL ), s.Token );
			}
			else if( s.Token == "USA_AUTH_WEB_PAGE" )
			{
				s.GetToken();
				FLStrcpy( USA_AUTH_WEB_PAGE, _countof( USA_AUTH_WEB_PAGE ), s.Token );
			}
			else if( s.Token == "USA_AUTH_PORT" )
			{
				USA_AUTH_PORT	= s.GetNumber();
			}
			else if( s.Token == "PHILIPPINES_AUTH_WEB_URL" )
			{
				s.GetToken();
				FLStrcpy( PHILIPPINES_AUTH_WEB_URL, _countof( PHILIPPINES_AUTH_WEB_URL ), s.Token );
			}
			else if( s.Token == "PHILIPPINES_AUTH_WEB_PAGE" )
			{
				s.GetToken();
				FLStrcpy( PHILIPPINES_AUTH_WEB_PAGE, _countof( PHILIPPINES_AUTH_WEB_PAGE ), s.Token );
			}
			else if( s.Token == "PHILIPPINES_AUTH_PORT" )
			{
				PHILIPPINES_AUTH_PORT	= s.GetNumber();
			}

			else if( s.Token == "PHILIPPINES_AUTH_PORT_IPBONUS" )
			{
				PHILIPPINES_AUTH_PORT_IPBONUS	= s.GetNumber();
			}

			else if( s.Token == "IDN_AUTH_SERVER_IP" )
			{
				s.GetToken();
				FLStrcpy( IDN_AUTH_SERVER_IP, _countof( IDN_AUTH_SERVER_IP ), s.Token );
			}
			else if( s.Token == "IDN_AUTH_SERVER_PORT" )
			{
				IDN_AUTH_SERVER_PORT = static_cast< WORD >( s.GetNumber() );
			}
			else if( s.Token == "IDN_AUTH_CHANNEL_NUMBER" )
			{
				IDN_AUTH_CHANNEL_NUMBER = (BYTE)( s.GetNumber() );
			}
			//  [10/22/2014 ChangSeop.Kim]
			else if( s.Token == "INGAME_SHOP_URL" ) {
				s.GetToken();
				g_strInGameShopURL = s.Token;
			}
			//  [10/22/2014 ChangSeop.Kim]

			s.GetToken();
		}
		return TRUE;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "Can't open file, %s." ), lpszFileName );
	return FALSE;
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
				WPARAM wTimerID	= wParam;
				switch( wTimerID )
				{
					case 0:	// 
						{
							CUserMng::GetInstance()->ClearDum( (CDPMng*)&g_dpCertifier );
							hb.Run();
							break;
						}
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, 0 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
Heartbeat::Heartbeat( const char* name )
:
_heartbeat( 0 ),
_name( name )
{
}

Heartbeat::~Heartbeat()
{
	CLOSE_HANDLE( _heartbeat );
}

void Heartbeat::Init()
{
	_heartbeat				= ::CreateEvent( NULL, FALSE, FALSE, _name.c_str() );
	if( !( _heartbeat ) )
		_heartbeat		= ::OpenEvent( EVENT_MODIFY_STATE, FALSE, _name.c_str() );
	if( !_heartbeat )
		FLERROR_LOG( PROGRAM_NAME, _T( "Heartbeat.Init" ) );
}

void Heartbeat::Run()
{
	if( _heartbeat )
		::SetEvent( _heartbeat );
}

////////////////////////////////////////////////////////////////////////////////