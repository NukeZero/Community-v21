#include "StdAfx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

//#include <vld/vld.h>

#include "resource.h"
#include "dpcoresrvr.h"
#include "dpcachesrvr.h"
#include "dploginsrvr.h"
#include <msghdr.h>

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include <xutil.h>
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient g_dpDatabaseClient;

#include <lang.h>

#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;

extern	CDPCoreSrvr		g_dpCoreSrvr;
extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CDPLoginSrvr	g_dpLoginSrvr;

#include <party.h>
extern	CPartyMng	g_PartyMng;

#include "player.h"
extern	CPlayerMng	g_PlayerMng;

extern	BOOL	s_bTest;

extern	DWORD	s_dwEvent;
#include <flyffevent.h>

#include "../_Common/SeasonEffect.h"

/////////////////////////////////////////////////////////////
#define			MAX_LOADSTRING	100

FLMyTrace		g_MyTrace;

const WORD		TIMER_ID_DEFAULT = 100;
const WORD		TIMER_ID_WANTED  = 200;

const	WORD	TIMER_ID_EVENT	= 500;

#ifdef __INTERNALSERVER
	const UINT g_uWantedElapse = SEC( 30 );
#else
	const UINT g_uWantedElapse = MIN( 10 );
#endif //__INTERNALSERVER

/////////////////////////////////////////////////////////////
HINSTANCE	hInst;
HWND		hMainWnd;
TCHAR		szTitle[MAX_LOADSTRING];
TCHAR		szWindowClass[MAX_LOADSTRING];
DWORD		g_dwSys;
DWORD		g_dwId;
CHAR		szCertifierAddr[16];

char		g_szLoginBindIP[16] = { 0, };
char		g_szWorldBindIP[16] = { 0, };
char		g_szCacheBindIP[16] = { 0, };
CHAR		szDataBaseAddr[16];


ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCSTR lpszFileName );
void				OnTimer( WORD wTimerID );



//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	FFL_DUMP
//////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
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

	if (!g_xFlyffConfig->LoadFlyffConfig( "config.ini" ))
		return false;

	// mirchang_100914 load config file
	//////////////////////////////////////////////////////////////////////////

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CORESERVER, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CORESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName	= (LPCSTR)IDC_CORESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd	=
		CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

	const int x = 0;
	const int y = 416;
	SetWindowPos( hWnd, nullptr, x, y, 400, 416, SWP_SHOWWINDOW );

	g_GameTimer.Compute();
	SetTimer( hWnd, TIMER_ID_DEFAULT, 1000, nullptr);

#if !defined(__INTERNALSERVER)
	SetTimer( hWnd, TIMER_ID_WANTED, g_uWantedElapse, nullptr);
#endif

	SetTimer( hWnd, TIMER_ID_EVENT, 1000 * 60, nullptr);

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0xff, 0xff, 0xff ), RGB( 0x00, 0x00, 0x00 ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	g_TickCount.Start();

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( !Script( "CoreServer.ini" ) )
	{
		return FALSE;
	}

	if( !g_dpCoreSrvr.StartServer( g_szWorldBindIP, PN_CORESRVR + 0, FLNormalProtocol::GetInstance() ) )
		return FALSE;

	if( !g_DPCacheSrvr.StartServer( g_szCacheBindIP, PN_CORESRVR + 1, FLNormalProtocol::GetInstance() ) )
		return FALSE;

	if( !g_dpLoginSrvr.StartServer( g_szLoginBindIP, PN_CORESRVR + 2, FLNormalProtocol::GetInstance() ) )
		return FALSE;

	if( !g_dpDatabaseClient.ConnectToServer( szDataBaseAddr, PN_DBSRVR_2, FLNormalProtocol::GetInstance(), FALSE ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not TransServer Connect" ) );
		return FALSE;
	}

	if( !g_PartyMng.CreateWorkers() )
	{
		FLASSERT( 0 );
	}

	if( g_pSeasonEffect->CreateWorkers() == false )
	{
		FLASSERT( 0 );
	}

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExitInstance( void )
{
	g_dpCoreSrvr.DeleteDPObject();
	g_DPCacheSrvr.DeleteDPObject();
	g_dpLoginSrvr.DeleteDPObject();
	g_dpDatabaseClient.DeleteDPObject();

	g_PartyMng.Clear();

	g_PlayerMng.Free();

	MEMPOOLER_DESTROY( CParty );
	MEMPOOLER_DESTROY( CPlayer );
	g_GuildMng.m_AddRemoveLock.Enter();
	g_GuildWarMng.Clear();
	g_GuildMng.m_AddRemoveLock.Leave();

	g_GuildMng.Clear();
	MEMPOOLER_DESTROY( CGuildMember );
	MEMPOOLER_DESTROY( CGuild );

	UninitializeNetLib();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId; //, wmEvent;
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
				case IDM_ABOUT:
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				OnTimer( static_cast< WORD >( wParam ) ); // wTimerID = wParam 
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
			KillTimer( hMainWnd, TIMER_ID_DEFAULT );
#if !defined(__INTERNALSERVER)
			KillTimer( hMainWnd, TIMER_ID_WANTED );
#endif
			KillTimer( hMainWnd, TIMER_ID_EVENT );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void OnTimer( WORD wTimerID )
{
	if( wTimerID == TIMER_ID_DEFAULT )
	{
		g_GameTimer.Compute();

		static BOOL s_bCountdown	= FALSE;
		CTime tmCur		= CTime::GetCurrentTime();

		//////////////////////////////////////////////////////////////////////////
		// 새해 카운트다운 효과..
		int nMonth		= tmCur.GetMonth();
		int nDay		= tmCur.GetDay();
		if( nMonth == 12 && nDay == 31 )	// 12월 31일
		{
			int nHour	= tmCur.GetHour();
			int nMin	= tmCur.GetMinute();

			if( nHour == 23 && nMin == 59 && s_bCountdown == FALSE )
			{
				s_bCountdown	= TRUE;
				g_DPCacheSrvr.SendHdr( PACKETTYPE_NEWYEAR, DPID_ALLPLAYERS, DPID_ALLPLAYERS );
			}
		}
		else if( s_bCountdown == TRUE )
		{
			s_bCountdown	= FALSE;
		}
		return;
	}

#if !defined(__INTERNALSERVER)
	if( wTimerID == TIMER_ID_WANTED )	// 현상금 처리 
	{
		g_dpCoreSrvr.SendCWWantedList();	// 현상금 리스트를 보낸다.
		return;
	}
#endif // __INTERNALSERVER

	if( wTimerID == TIMER_ID_EVENT )
	{
		BOOL	f	= FALSE;
		CTime	tmCurr	= CTime::GetCurrentTime();

		BOOL bXmas;
		if( s_bTest )
		{
			bXmas	= tmCurr < CTime( 2005, 12, 26, 10, 0, 0 );
		}
		else
		{
			bXmas	= ( tmCurr >= CTime( 2005, 12, 23, 0, 0, 0 ) && tmCurr < CTime( 2005, 12, 27, 9, 0, 0 ) );
		}

		BOOL bNewYear;
		if( s_bTest )
		{
			bNewYear	= tmCurr < CTime( 2006, 1, 31, 10, 0, 0 );
		}
		else
		{
			bNewYear	= ( tmCurr >= CTime( 2006, 1, 26, 0, 0, 0 ) && tmCurr < CTime( 2006, 1, 31, 10, 0, 0 ) );
		}

		BOOL bValentinesDay;
		if( s_bTest )
		{
			bValentinesDay	= tmCurr < CTime( 2006, 2, 15, 13, 0, 0 );
		}
		else
		{
			bValentinesDay	= ( tmCurr >= CTime( 2006, 2, 14, 0, 0, 0 ) && tmCurr < CTime( 2006, 2, 15, 13, 0, 0 ) );
		}
			
		if( bXmas && !( s_dwEvent & EVE_FLAG1219 ) )
		{
			s_dwEvent	|= EVE_FLAG1219;
			f	= TRUE;
		}
		else if( !bXmas && ( s_dwEvent & EVE_FLAG1219 ) )
		{
			s_dwEvent	&= ~EVE_FLAG1219;
			f	= TRUE;
		}

		if( bNewYear && !( s_dwEvent & EVE_FLAG0127 ) )
		{
			s_dwEvent	|= EVE_FLAG0127;
			f	= TRUE;
		}
		else if( !bNewYear && ( s_dwEvent & EVE_FLAG0127 ) )
		{
			s_dwEvent	&= ~EVE_FLAG0127;
			f	= TRUE;
		}
		
		if( bValentinesDay && !( s_dwEvent & EVE_FLAG0214 ) )
		{
			s_dwEvent	|= EVE_FLAG0214;
			f	= TRUE;
		}
		else if( !bValentinesDay && ( s_dwEvent & EVE_FLAG0214 ) )
		{
			s_dwEvent	&= ~EVE_FLAG0214;
			f	= TRUE;
		}

		if( f )
		{
			g_dpCoreSrvr.SendEvent( s_dwEvent );
		}
	}
}

BOOL Script( LPCSTR lpszFileName )
{
	CScanner s;
	POINT topLeft;
	SIZE	size;

	if( s.Load( lpszFileName ) )
	{
		g_MyTrace.AddLine( '-' );

		s.GetToken();

		while( s.tok != FINISHED )
		{
			if( s.Token == "Sys" )
			{
				g_dwSys	= s.GetNumber();
			}
			else if( s.Token ==  "dwId" )
			{
				g_dwId = s.GetNumber();
			}
			else if( s.Token == "LOGIN_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szLoginBindIP, _countof( g_szLoginBindIP ), s.Token );
			}
			else if( s.Token == "WORLD_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szWorldBindIP, _countof( g_szWorldBindIP ), s.Token );
			}
			else if( s.Token == "CACHE_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szCacheBindIP, _countof( g_szCacheBindIP ), s.Token );
			}
			else if( s.Token == "Database" )
			{
				s.GetToken();
				FLStrcpy( szDataBaseAddr, _countof( szDataBaseAddr ), s.Token );
			}
			else if( s.Token == "Certifier" )
			{
				s.GetToken();
				FLStrcpy( szCertifierAddr, _countof( szCertifierAddr ), s.Token );
			}
			else if( s.Token == "TEST" )
			{
				s_bTest	= TRUE;			
			}
			else if( s.Token == "PartyExpRate" )
			{
				s_fPartyExpRate		= s.GetFloat();
			}
			else
			{
				CServerDesc* pServer = NULL;

				try
				{
					pServer		= new CServerDesc;
					u_long uKey	= (u_long)_ttoi( s.Token );
					pServer->SetKey( uKey );

					if( s.GetToken() == DELIMITER )
					{
						while( s.GetToken() != DELIMITER )
						{
							CJurisdiction* pJurisdiction	= new CJurisdiction;
							pJurisdiction->m_dwWorldID	= (DWORD)_ttoi( s.Token );
							topLeft.x	= s.GetNumber();	topLeft.y	= s.GetNumber();
							size.cx		= s.GetNumber();	size.cy		= s.GetNumber();
							pJurisdiction->m_rect.SetRect( topLeft.x, topLeft.y, topLeft.x + size.cx, topLeft.y + size.cy );
							pJurisdiction->m_wLeft	= static_cast< WORD >( s.GetNumber() );	
							pJurisdiction->m_wRight		= static_cast< WORD >( s.GetNumber() );
							pServer->m_lspJurisdiction.push_back( pJurisdiction );
						}
					}
					bool bResult	= g_dpCoreSrvr.m_apSleepServer.insert( CServerDescArray::value_type( pServer->GetKey(), pServer ) ).second;
					FLASSERT( bResult );
					g_MyTrace.Add( pServer->GetKey(), TRUE, "%04d", pServer->GetKey() );
				}
				catch( ... )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ CServerDesc Alloc Failed. ]" ) );
					SAFE_DELETE( pServer );
				}
			}
			s.GetToken();
		}
		g_MyTrace.AddLine( '-' );

		return TRUE;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "Can't open file, %s." ), lpszFileName );
	return FALSE;
}