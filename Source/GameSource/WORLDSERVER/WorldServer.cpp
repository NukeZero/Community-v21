#include "StdAfx.h"

//#include <vld/vld.h> //Visual Leak detector

#include <ui/FLUIMessageLoop.h>

#include "Resource.h"
#include "GlobalTime.h"
#include "User.h"
#include <WorldMng.h>
#include "WorldServer.h"
#include "ThreadMng.h"
#include <AIBear.h>
#include <AIBigMuscle.h>
#include <AIClockWorks.h>
#include <AIGuard.h>
#include <AIKrrr.h>
#include <AIPet.h>
#include <AIAggroNormal.h>
#include <guild.h>
#include <guildwar.h>
#include <party.h>
#include <Chatting.h>
#include <spevent.h>
#include "WorldDialog.h"
#include "FLTeleportMapUserPointManager.h"

#include "../_CommonDefine/FLTm.h"

#include "NetSerialNumberRequest.h"

#include <couple.h>
#include <housing.h>

#include "slord.h"

#include <langman.h>

#include "FLEventArenaGlobal.h"


#include "../_CommonDefine/FLLFH.h"



LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				Script( LPCSTR lpszFileName );
void				ExitInstance( void );
void				SetTitle();
extern BOOL			LoadAIScript();

const int			MAX_LOADSTRING = 100;
static TCHAR		g_szTitle[MAX_LOADSTRING];			
static TCHAR		g_szWindowClass[MAX_LOADSTRING];	
static HWND			g_hMainWnd;
static char			g_szBuffer[LOGTYPE_MAX][1024] = {0, };

char				g_szCacheBindIP[16] = { 0, };
char				g_szCoreAddr[16];

//#ifdef __IDC
static char  		g_szINI[] = "..\\script\\WorldServer.ini";
//#else	
//static char  		g_szINI[] = "WorldServer.ini";
//#endif	

extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;
extern	CChattingMng		g_ChattingMng;
extern	CPartyMng			g_PartyMng;
extern	char				g_szDBAddr[16];
extern	char				g_sHeartbeat[32];
extern	CWorldMng			g_WorldMng;
extern CGuildCombat			g_GuildCombatMng;


#ifdef	_DEBUG
#include <logging/FLLogPubLoggingDialog.h>
FLLogPubLoggingDialog	g_cLogPubDialog;
#endif

//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	FFL_DUMPk
//////////////////////////////////////////////////////////////////////////




#include <AIBear.h>


#include <string>



#include "../_CommonDefine/Util/FLCSVParser.h"


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     /*lpCmdLine*/,
                     int       nCmdShow)
{
	FLLFH::GetInstance().Install();



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


	//_CrtSetBreakAlloc( 628278 );	// memory leak 탐지
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WORLDSERVER, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//neodeath random NPC
	//DEBUG_EXCUTE( DATE_TIMER().SetTimeSpeed_DebugMode( 330 ) );		//테스트를 위한 배속 세팅  

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100914 load config file

	if( g_xFlyffConfig->LoadFlyffConfig( "config.ini" ) == false )
	{
		return FALSE;
	}

	// mirchang_100914 load config file
	//////////////////////////////////////////////////////////////////////////

	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
		g_eLocal.SetState( EVE_18, 1 );

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WORLDSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_WORLDSERVER;
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

void SetLogInfo( LOGTYPE type, LPCTSTR lpszFormat, ... )
{
	if( type == LOGTYPE_REDRAW )
	{
		InvalidateRect( g_hMainWnd, NULL, TRUE );
		return;
	}

	va_list args;
	va_start(args, lpszFormat);

	FLVSPrintf( g_szBuffer[ type ], _countof( g_szBuffer[ type ] ), lpszFormat, args );

	va_end(args);
}


void OnPaint( HDC& hDC )
{
	int x, y;
	x = 0;
	y = 0;
	
	for( int i=0; i<LOGTYPE_MAX; ++i )
	{
		TextOut( hDC, x, y, g_szBuffer[i], strlen( g_szBuffer[i] ) );
		y += 20;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;// = 0, wmEvent = 0;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_TIMER:
			{
				WPARAM wTimerID	= wParam;
				switch( wTimerID )
				{
					case 0:
						g_dpDBClient.SendPing();
						g_DPCoreClient.SendPing();
						break;
					case 1:
						// g_xWSUserManager->OutputStatistics();
						break;
				}
				break;
			}
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			//wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			OnPaint( hdc );
			EndPaint( hWnd, &ps );
			break;
		case WM_LBUTTONDOWN:
#ifdef	_DEBUG
			g_cLogPubDialog.Show();
#endif
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_DESTROY:
			KillTimer( g_hMainWnd, 0 );
			KillTimer( g_hMainWnd, 1 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// float point 에러가 exception을 내게 한다.
void EnableFloatException()
{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	unsigned int dwCurrent = 0;
	::_controlfp_s( &dwCurrent, 0, 0 );
	unsigned int dwNew = dwCurrent & ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	::_controlfp_s( NULL, dwNew, MCW_EM );
#else
	int cw = _controlfp( 0, 0 );
	cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	_controlfp( cw, MCW_EM );
#endif
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // Set up the highest resolution timer we can manage 
//	TIMECAPS tc; 
//	UINT nPeriod;
//	if( TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc)) ) 
//		nPeriod = tc.wPeriodMin;
//	else
//		nPeriod = 1;  
//	timeBeginPeriod( nPeriod ); 

	HWND hWnd;
	g_hMainWnd	= hWnd	= CreateWindow(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	int x = 480, y = 0;
	SetWindowPos( hWnd, NULL, x, y, 800, 416, SWP_SHOWWINDOW );

	if (!hWnd)
		return FALSE;

	SetTimer( g_hMainWnd, 0, SEC( 60 ), NULL );	// 60
	SetTimer( g_hMainWnd, 1, MIN( 60 ), NULL );	// 3600

	CTime time = CTime::GetCurrentTime();
	SetLogInfo( LOGTYPE_TIME, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	EnableFloatException();

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
		return FALSE;

	xSRand( timeGetTime()  );	// 속도를 요하는 랜덤은 xRandom()을 쓰도록...

	if( Script( g_szINI ) == TRUE )
	{
#ifdef	_DEBUG
		TCHAR szResDllPath[MAX_PATH] = { 0, };
		::GetCurrentDirectory( _countof( szResDllPath ), szResDllPath );
		FLStrcat( szResDllPath, _countof( szResDllPath ), _T( "\\FLResCommon.dll" ) );
		if( g_cLogPubDialog.Create( PROGRAM_NAME, g_hMainWnd, szResDllPath ) == true )
		{
			FLLOG_ADD_PUBLISHER( PROGRAM_NAME, &g_cLogPubDialog );
			g_cLogPubDialog.Show();
		}
#endif
		//////////////////////////////////////////////////////////////////////////
		const TCHAR szFileName[]	= _T( "MoveScript.inc" );
		if( g_pScriptFileManager->LoadMoveFileList( szFileName ) == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED MOVE SCRIPT FILE. PLEASE CHECK:(%s) ]" ), szFileName );
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////

		if( !prj.OpenProject( "Masquerade.prj" ) ) 
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenProject ERROR check Masquerade.prj" ) );
			TCHAR szMsg[256] = { 0, };
			FLSPrintf( szMsg, _countof( szMsg ), "Read Failed. - %s", "Masquerade.prj" );
			AfxMessageBox( szMsg, MB_OK );
			return FALSE;
		}

		if( g_pEventArenaGlobal->Build() == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Event Arena Channel build failed" ) );
			TCHAR szMsg[256] = { 0, };
			FLSPrintf( szMsg, _countof( szMsg ), "Event Arena Channel build failed" );
			AfxMessageBox( szMsg, MB_OK );

			return FALSE;
		}

		SetLogInfo( LOGTYPE_REDRAW, "" );
		SetTitle();

	#if defined(__REMOVE_SCIRPT_060712)
		if( CWorldDialog::GetInstance().Init() == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "WorldDialog.DLL load failed" ) );
			return FALSE;
		}
		else
		{
			FLINFO_LOG( PROGRAM_NAME, _T( "WorldDialog.DLL load success" ) );
		}
	#endif
		return CRunObject::GetInstance()->Init();
	}
	else
	{
		char szBuffer[300] = {0,};
		char szDir[255] = {0,};
		GetCurrentDirectory( 255, szDir );
		FLSPrintf( szBuffer, _countof( szBuffer ), "%s\\WorldServer.ini File not found", szDir );
		AfxMessageBox( szBuffer );
		return FALSE;
	}
}

void ExitInstance( void )
{
	CRunObject::GetInstance()->Close();

	g_pEventArenaGlobal->Clear();
	g_GuildCombatMng.GuildCombatClear( 99 );

	g_DPSrvr.DeleteDPObject();
	g_DPCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();

	g_WorldMng.Free();

	MEMPOOLER_DESTROY( FLWSUser );
	MEMPOOLER_DESTROY( CMover );
	MEMPOOLER_DESTROY( CActionMover );
	MEMPOOLER_DESTROY( CAIMonster );
	MEMPOOLER_DESTROY( CItem );

	g_PartyMng.Clear();

	MEMPOOLER_DESTROY( CParty );

	g_ChattingMng.Clear();

	MEMPOOLER_DESTROY( CChatting );

	g_GuildWarMng.Clear();

	g_GuildMng.Clear();

	CHousingMng::GetInstance()->Clear();

	MEMPOOLER_DESTROY( CGuildMember );
	MEMPOOLER_DESTROY( CGuild );
	MEMPOOLER_DESTROY( CAIPet );
	MEMPOOLER_DESTROY( CAIKrrr );
	MEMPOOLER_DESTROY( CAIGuard );
	MEMPOOLER_DESTROY( CAIClockWorks );
	MEMPOOLER_DESTROY( CAIBigMuscle );
	MEMPOOLER_DESTROY( CAIBear );

	UninitializeNetLib();

	MEMPOOLER_DESTROY( CAIPet );
	MEMPOOLER_DESTROY( CAIAggroNormal );
	g_pTeleportMapUserPointMng->Shutdown();

	g_Object3DMng.Destroy();
	prj.m_modelMng.DeleteDeviceObjects();

	CSLord::Instance()->DestroyColleagues();

#ifdef	_DEBUG
	FLLOG_DEL_PUBLISHER( PROGRAM_NAME, &g_cLogPubDialog );
	g_cLogPubDialog.Destroy();
#endif
}


void SetTitle()
{
	char szLang[MAX_PATH]	= { 0, };
	FLSPrintf( szLang, _countof( szLang ), CLangMan::GetInstance()->GetLangData( g_xFlyffConfig->GetMainLanguage() )->szTitle );

	char szTitle[256] = { 0, };
	FLSPrintf( szTitle, _countof( szTitle ), "%s(%d)-%s", g_szTitle, g_uKey, szLang );
	SetWindowText( g_hMainWnd, szTitle );
}

BOOL Script( LPCSTR lpszFileName )
{
	g_eLocal.SetState( EVE_RESPAWN, 1 );

	CScanner s;
	if( s.Load( lpszFileName ) == FALSE)
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Fail to open file - %s" ), lpszFileName );
		return FALSE;
	}

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == "ResourceFolder" )
		{
			s.GetToken();
			if( !SetCurrentDirectory( s.Token ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Folder not found, %s" ), s.Token );
				return FALSE;
			}
		}
		else if( s.Token == "Key" )
		{
			g_uKey		 = (DWORD)s.GetNumber();
			g_uIdofMulti = g_uKey / 100;
		}
		else if( s.Token == "CACHE_BIND_IP" )
		{
			s.GetToken();
			FLStrcpy( g_szCacheBindIP, _countof( g_szCacheBindIP ), s.Token );
		}
		else if( s.Token == "Core" )
		{
			s.GetToken();
			FLStrcpy( g_szCoreAddr, _countof( g_szCoreAddr ), s.Token );
		}
		else if( s.Token == "DB" )
		{
			s.GetToken();
			FLStrcpy( g_szDBAddr, _countof( g_szDBAddr ), s.Token );
		}
		else if( s.Token == "Account" )
		{
			s.GetToken();
		}
		else if( s.Token == "Heartbeat" )
		{
			s.GetToken();
			FLStrcpy( g_sHeartbeat, _countof( g_sHeartbeat ), s.Token );
		}
		else if( s.Token == "18" )
		{
			g_eLocal.SetState( EVE_18, 1 );		// 18세 서버 
		}
		else if( s.Token == "PK" )
		{
			g_eLocal.SetState( EVE_PK, 1 );
		}
		else if( s.Token == "PKCOST" )
		{
			g_eLocal.SetState( EVE_PKCOST, 1 );
		}
		else if( s.Token == "STEAL" )
		{
			g_eLocal.SetState( EVE_STEAL, 1 );				
		}
		else if (s.Token == "GUILDBANK")
		{
			g_eLocal.SetState( ENABLE_GUILD_INVENTORY, 1 );
		}
		else if( s.Token == "GUILDWAR" )
		{
			g_eLocal.SetState( EVE_GUILDWAR, 1 );
		}
		else if( s.Token == "DROPITEM_REMOVE" )
		{
			g_eLocal.SetState( EVE_DROPITEMREMOVE, 1 );
		}			
		else if( s.Token == "SCHOOL" )
		{
			//g_eLocal.SetState( EVE_SCHOOL, 1 );
		}
		else if( s.Token == "GUILDCOMBAT")
		{
			g_eLocal.SetState( EVE_GUILDCOMBAT, 1 );
		}
		else if( s.Token == "WORMON" )
		{
			g_eLocal.SetState( EVE_WORMON, 1 );
		}
		else if( s.Token == "respawn" )
		{
			g_eLocal.SetState( EVE_RESPAWN, (BYTE)s.GetNumber() );
		}
		else if( s.Token == "RECOMMEND" )
		{
			g_eLocal.SetState( EVE_RECOMMEND, 1 );
		}
		else if( s.Token == "GUILDCOMBAT1TO1" )
		{
			g_eLocal.SetState( EVE_GUILDCOMBAT1TO1, 1 );
		}
		else if( s.Token == "ARENA" )
		{
			g_eLocal.SetState( EVE_ARENA, 1 );
		}
		else if( s.Token == "SECRETROOM" )
		{
			g_eLocal.SetState( EVE_SECRETROOM, 1 );
		}
		else if( s.Token == "RAINBOWRACE" )
		{
			g_eLocal.SetState( EVE_RAINBOWRACE, 1 );
		}
		else if( s.Token == "SERIAL_NUMBER_REQUEST" )
		{
			int nCount = s.GetNumber();
			if( nCount > 0 )
			{
				CNetSerialNumberRequest::GetInstance()->SetRequestCount( nCount );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		else if( s.Token == "EVENT_ARENA" )
		{
			g_pEventArenaGlobal->SetArenaChannel();
		}
		else if( s.Token == "EVENT_ARENA_CONFIG_FILE" )
		{
			s.GetToken();
			g_pEventArenaGlobal->SetConfigFile( s.Token );
		}
		//////////////////////////////////////////////////////////////////////////

		s.GetToken();
	}
	return TRUE;
}
