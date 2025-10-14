// DatabaseServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

//#include <vld/vld.h>


#include "resource.h"
#include "dploginsrvr.h"
#include "dptrans.h"
#include "dpaccountclient.h"

#include <logging/FLMyTrace.h>
#include <ui/FLUIMessageLoop.h>

#include "..\_Network\Objects\Obj.h"
#include <xutil.h>
#include <guild.h>
extern	CGuildMng	g_GuildMng;

#include <post.h>

#include <mmsystem.h>

#include "dbcontroller.h"

extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CDPAccountClient	g_dpAccountClient;
extern	CProject	prj;

#include "DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;

#include <spevent.h>

#include "tlord.h"

#include <serialnumber.h>
#include "DBSerialNumberRequest.h"

#define MAX_LOADSTRING 100

extern	APP_INFO			g_appInfo;
extern	T_SLOT_ACTION_FLAG	g_tSlotActionFlag;

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

#include "../_Common/FLServerDescription.h"
#include <FLDataBase.h>


FLMyTrace	g_MyTrace;

char	g_szLoginBindIP[16] = { 0, };
char	g_szCoreBindIP[16] = { 0, };
char	g_szWorldBindIP[16] = { 0, };

char	lpConnection[16];

BOOL	m_bBankToItemSendTbl	= FALSE;
BOOL	m_bInventoryToItemSendTbl	= FALSE;
BOOL	s_bItemRemove	= FALSE;
BOOL	s_bRemoveInvalidItem	= FALSE;

#include "DbController.h"

#include <Tax.h>
#include "QuizDBCtrl.h"
#include "GuildHouseDBCtrl.h"
#include "CampusDBCtrl.h"

#include "../_Database/FLConsignmentSale_Query.h"

//////////////////////////////////////////////////////////////////////////
#include <FLDBDataConvert.h>
//////////////////////////////////////////////////////////////////////////

TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text

// Foward declarations of functions included in this code module:
ATOM	MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance( void );
BOOL	Script( LPCSTR lpszFileName );


#define	TID_MEM_RELEASE		0
#define TID_RELOAD_PROJECT  1
#define	TID_POSTPROC	2
#define	TID_QUERY_REMOVE_GUILD_BANK_TBL	3
#define	TIMER_ID_EVENT_GENERIC	4

//#if defined(__INTERNALSERVER)
//	static char g_szINI[] = "databaseserver.ini";
//#else
	static char g_szINI[] = "..\\script\\databaseserver.ini";
//#endif



//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

//#include <bugtrap/FLBugTrapper.h>

#include <FLIncludeAllLibrary.h>
//	ENDTEST100111	FFL_DUMP
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

	LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString( hInstance, IDC_DATABASESERVER, szWindowClass, MAX_LOADSTRING );
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DATABASESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName	= (LPCSTR)IDC_DATABASESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	g_hInstance = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd	= CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	  return FALSE;

	// Item 시리얼 번호 생성 랜덤값 초기화
	xSRand( timeGetTime()  );	// 속도를 요하는 랜덤은 xRandom()을 쓰도록...

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( Script( g_szINI ) == FALSE )
	{
		char szMsg[256] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "FAIL read ini - %s", g_szINI );
		AfxMessageBox( szMsg, MB_OK );
		return FALSE;
	}

	if( CSerialNumber::GetInstance()->Init( CDBSerialNumberRequest::GetInstance() ) == FALSE )
	{
		return FALSE;
	}
	
	if( CQuery::EnableConnectionPooling() )
		FLTRACE_LOG( PROGRAM_NAME, _T( "EnableConnectionPooling" ) );

	g_DbManager.CreateDbWorkers();

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
	const size_t CHARACTER_DB_CONNECTION_COUNT	= 3 + 8;
	const size_t LOG_DB_CONNECTION_COUNT		= 3 + 8;
#else
	const size_t CHARACTER_DB_CONNECTION_COUNT	= 3;
	const size_t LOG_DB_CONNECTION_COUNT		= 3;
#endif

#ifdef __DbStuff
	CHARACTER_DB().Start(CHARACTER_DB_CONNECTION_COUNT, CQuery::SqlDatabase::CHARACTER);
	LOG_DB().Start(LOG_DB_CONNECTION_COUNT, CQuery::SqlDatabase::LOGGING);
#else
	CHARACTER_DB().Start( CHARACTER_DB_CONNECTION_COUNT, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, g_DbManager.DB_ADMIN_PASS_CHARACTER01 );
	LOG_DB().Start( LOG_DB_CONNECTION_COUNT, DSN_NAME_LOG, DB_ADMIN_ID_LOG, g_DbManager.DB_ADMIN_PASS_LOG );
#endif

	nsConsignmentSale::FLQuery_Load kLoad;
	kLoad.DirectExecute( CHARACTER_DB(), 0 );


#ifdef __INTERNALSERVER
	SetTimer(  hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN( 1 ), NULL );
#else	// __INTERNALSERVER
	SetTimer(  hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN( 10 ), NULL );
#endif	// __INTERNALSERVER

	if( !g_DbManager.LoadPlayerData() )
		return FALSE;

	if( !g_DbManager.GetPartyName() )
	{
		FLASSERT( 0 );
	}

	if( !g_DbManager.LoadPost() )
	{
		FLASSERT( 0 );
	}
	SetTimer( hWnd, TID_POSTPROC, MIN( 1 ), NULL );

	

	if( !CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" ) )
	{
//		FLASSERT( 0 );
		AfxMessageBox( "propevent.inc" );
	}
	SetTimer( hWnd, TIMER_ID_EVENT_GENERIC, MIN( 1 ), NULL );

	if( !g_DbManager.OpenGuildCombat() )
	{
		FLASSERT( 0 );
	}

	// 군주 시스템을 구성하는 협력 객체들을 생성한다
	CTLord::Instance()->CreateColleagues();

	CDbControllerTimer::GetInstance()->Create();

	// load QuizEvent Id
	if( !CQuizDBMng::GetInstance()->InitQuizEventId() )
	{
		FLASSERT( 0 );
	}
	//CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_INIT );

	GuildHouseDBMng->PostRequest( GUILDHOUSE_LOAD );
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_LOAD );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN Guild Bank Convert
	if( g_kFLDBDataConvert->ConvertGuildBank() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Convert Guild Bank Failed." ) );
		return FALSE;
	}
	// END Guild Bank Convert
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load All Guild Data
	g_DbManager.OpenGuild();
	//////////////////////////////////////////////////////////////////////////

	if( !g_dpLoginSrvr.StartServer( g_szLoginBindIP, PN_DBSRVR_0, FLNormalProtocol::GetInstance() ) )
	{
		return FALSE;
	}
	
	if( !CDPTrans::GetInstance()->StartServer( g_szWorldBindIP, PN_DBSRVR_1, FLNormalProtocol::GetInstance() ) )
	{
		return FALSE;
	}

	if( !g_dpCoreSrvr.StartServer( g_szCoreBindIP, PN_DBSRVR_2, FLNormalProtocol::GetInstance() ) )
	{
		return FALSE;
	}

	if( FALSE == g_dpAccountClient.ConnectToServer( lpConnection, PN_ACCOUNTSRVR_1, FLNormalProtocol::GetInstance() ) )
		return FALSE;
	g_dpAccountClient.SendAddIdofServer( g_appInfo.dwSys );

	//NotifyProcessStart( g_appInfo.dwId );	

	int x = 0, y = 0;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0xff, 0xff, 0xff ), RGB( 0x00, 0x00, 0x00 ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void ExitInstance( void )
{
	g_dpLoginSrvr.DeleteDPObject();
	CDPTrans::GetInstance()->DeleteDPObject();
	g_dpAccountClient.DeleteDPObject();
	g_dpCoreSrvr.DeleteDPObject();
	g_DbManager.Clear();
	g_GuildMng.Clear();

	MEMPOOLER_DESTROY( CGuildMember );
	MEMPOOLER_DESTROY( CGuild );
	MEMPOOLER_DESTROY( CMover );

	CDbControllerTimer::GetInstance()->Destroy();
	// 군주 시스템 협력 객체들을 제거한다
	CTLord::Instance()->DestroyColleagues();

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
				case IDM_ABOUT:
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_TEST:
					{
						CAr ar;
						int nNum = 100;
						ar << nNum;
						u_long nBufSize;
						LPBYTE lpBuf	= ar.GetBuffer( &nBufSize ); 
						CDPTrans::GetInstance()->OnSaveConcurrentUserNumber( ar, DPID_UNKNOWN, DPID_UNKNOWN, DPID_UNKNOWN, lpBuf, nBufSize );
						break;
					}
				case IDM_EVENTLUA_APPLYNOW:
					{
						prj.m_EventLua.m_Access.Enter();
						prj.m_EventLua.LoadScript();
						CDPTrans::GetInstance()->SendEventLuaChanged();
						if( prj.m_EventLua.CheckEventState() )
							CDPTrans::GetInstance()->SendEventLuaState( prj.m_EventLua.m_mapState, FALSE );
						prj.m_EventLua.m_Access.Leave();
					}
					break;
				case IDM_PCBANG_ON:
					{
						HMENU hMenu = GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_PCBANG_ON, MF_CHECKED );
						CheckMenuItem( hMenu, IDM_PCBANG_OFF, MF_UNCHECKED );
						CDPTrans::GetInstance()->m_bPCBangApply = TRUE;
						CDPTrans::GetInstance()->SendPCBangSetApply( DPID_ALLPLAYERS );
					}
					break;
				case IDM_PCBANG_OFF:
					{
						HMENU hMenu = GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_PCBANG_OFF, MF_CHECKED );
						CheckMenuItem( hMenu, IDM_PCBANG_ON, MF_UNCHECKED );
						CDPTrans::GetInstance()->m_bPCBangApply = FALSE;
						CDPTrans::GetInstance()->SendPCBangSetApply( DPID_ALLPLAYERS );
					}
					break;
				case IDM_QUIZEVENT_APPLYNOW:
					{
						if( !CQuiz::GetInstance()->IsRun() )
						{
							CQuiz::GetInstance()->LoadScript();
							CDPTrans::GetInstance()->SendQuizEventChanged();
						}
					}
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
					case TID_POSTPROC:
						CPost::GetInstance()->Process();
						break;
					case TID_QUERY_REMOVE_GUILD_BANK_TBL:
						g_DbManager.QueryRemoveGuildBankTbl();
						break;
					case TIMER_ID_EVENT_GENERIC:
						{
							if( CEventGeneric::GetInstance()->Run() )
							CDPTrans::GetInstance()->SendEventFlag( CEventGeneric::GetInstance()->GetFlag() ); 
							prj.m_EventLua.m_Access.Enter();

							if( prj.m_EventLua.IsNoticeTime() )
								g_dpCoreSrvr.SendEventLuaNotice();

							if( prj.m_EventLua.CheckEventState() )
								CDPTrans::GetInstance()->SendEventLuaState( prj.m_EventLua.m_mapState, TRUE );
							prj.m_EventLua.m_Access.Leave();

							if( prj.m_GuildCombat1to1.CheckOpenTIme() == 1
								&& prj.m_GuildCombat1to1.m_nState == CGuildCombat1to1Mng::GC1TO1_CLOSE )
								CDPTrans::GetInstance()->SendGC1to1Open();

							LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
							lpDbOverlappedPlus->nQueryMode	= QM_GETMAIL_REALTIME;
							PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
							//g_DbManager.QueryGetMailRealTime();
						}
						break;
						
					default:
						break;
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hWnd, TID_POSTPROC );
			KillTimer( hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL );
			KillTimer( hWnd, TIMER_ID_EVENT_GENERIC );
			PostQuitMessage(0);
			break;
		case WM_RBUTTONDOWN:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


BOOL Script( LPCSTR lpszFileName )
{
	CDbManager& db = CDbManager::GetInstance();

//	BOOL	bToolRemoveQuest	= FALSE;
//	BOOL	bToolItemId		= FALSE;
//	BOOL	bToolRestorePet	= FALSE;
	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == "ResourceFolder" )
			{
				s.GetToken();
				if( !SetCurrentDirectory( s.Token ) )
				{
					g_MyTrace.Add( FLMyTrace::Key( "status" ), FALSE, "Folder not found, %s", s.Token );
					return FALSE;
				}
			}
			else if( s.Token == "Sys" )
			{
				g_appInfo.dwSys		= s.GetNumber();
				g_pServerDescription->SetServerIndex( g_appInfo.dwSys );
			}
			else if( s.Token ==  "dwId" )
			{
				g_appInfo.dwId		= s.GetNumber();
			}
			else if( s.Token == "SERIAL_NUMBER_STATUS" )
			{
				g_appInfo.nSerialNumberStatus = s.GetNumber();
			}
			else if( s.Token == "SERIAL_NUMBER_REQUEST" )
			{
				int nCount = s.GetNumber();
				if( nCount > 0 )
				{
					CDBSerialNumberRequest::GetInstance()->SetRequestCount( nCount );
				}
			}
			else if( s.Token == "LOGIN_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szLoginBindIP, _countof( g_szLoginBindIP ), s.Token );
			}
			else if( s.Token == "CORE_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szCoreBindIP, _countof( g_szCoreBindIP ), s.Token );
			}
			else if( s.Token == "WORLD_BIND_IP" )
			{
				s.GetToken();
				FLStrcpy( g_szWorldBindIP, _countof( g_szWorldBindIP ), s.Token );
			}
			else if( s.Token == "Account" )
			{
				s.GetToken();
				FLStrcpy( lpConnection, _countof( lpConnection ), s.Token );
			}
			else if( s.Token == "DSN_NAME_CHARACTER" )
			{
				s.GetToken();
				FLStrcpy( DSN_NAME_CHARACTER01, _countof( DSN_NAME_CHARACTER01 ), s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_CHARACTER" )
			{
				s.GetToken();
				FLStrcpy( DB_ADMIN_ID_CHARACTER01, _countof( DB_ADMIN_ID_CHARACTER01 ), s.Token );
			}
			else if( s.Token == "DSN_NAME_LOG" )
			{
				s.GetToken();
				FLStrcpy( DSN_NAME_LOG, _countof( DSN_NAME_LOG ), s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_LOG" )
			{
				s.GetToken();
				FLStrcpy( DB_ADMIN_ID_LOG, _countof( DB_ADMIN_ID_LOG ), s.Token );
			}
#ifdef __S1108_BACK_END_SYSTEM
			else if( s.Token == "DSN_NAME_BACKSYSTEM" )
			{
				s.GetToken();
				FLStrcpy( DSN_NAME_BACKSYSTEM, _countof( DSN_NAME_BACKSYSTEM ), s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_BACKSYSTEM" )
			{
				s.GetToken();
				FLStrcpy( DB_ADMIN_ID_BACKSYSTEM, _countof( DB_ADMIN_ID_BACKSYSTEM ), s.Token );
			}
			else if( s.Token == "BACKENDSYSTEM" )
			{
				prj.m_bBackEndSystem = TRUE;
			}
#endif // __S1108_BACK_END_SYSTEM
			else if( s.Token == "DSN_NAME_ITEMUPDATE" )
			{
				s.GetToken();
				FLStrcpy( DSN_NAME_ITEMUPDATE, _countof( DSN_NAME_ITEMUPDATE ), s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_ITEMUPDATE" )
			{
				s.GetToken();
				FLStrcpy( DB_ADMIN_ID_ITEMUPDATE, _countof( DB_ADMIN_ID_ITEMUPDATE ), s.Token );
			}
			else if( s.Token == "ITEMUPDATE" )
			{
				prj.m_bItemUpdate = TRUE;
			}
			else if( s.Token == "DB_PWD_LOG" )
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken(s.Token, db.DB_ADMIN_PASS_LOG);
#else
				FLStrcpy(db.DB_ADMIN_PASS_LOG, _countof(db.DB_ADMIN_PASS_LOG), s.Token);
#endif
				FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), db.DB_ADMIN_PASS_LOG ); 
			}
			else if( s.Token == "DB_PWD_CHARACTER" )
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_CHARACTER01 ); 
#else
				FLStrcpy(db.DB_ADMIN_PASS_CHARACTER01, _countof(db.DB_ADMIN_PASS_CHARACTER01), s.Token);
#endif
				FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), db.DB_ADMIN_PASS_CHARACTER01 ); 
			}
			else if( s.Token == "DB_PWD_BACKEND" )
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_BACKSYSTEM ); 
#else
				FLStrcpy(db.DB_ADMIN_PASS_BACKSYSTEM, _countof(db.DB_ADMIN_PASS_BACKSYSTEM), s.Token);
#endif
				FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), db.DB_ADMIN_PASS_BACKSYSTEM ); 
			}
			else if( s.Token == "DB_PWD_ITEMUPDATE" )
			{
				s.GetToken();
#ifdef __PWD_CRYPT_DB
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_ITEMUPDATE ); 
#else
				FLStrcpy(db.DB_ADMIN_PASS_ITEMUPDATE, _countof(db.DB_ADMIN_PASS_ITEMUPDATE), s.Token);
#endif
				FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), db.DB_ADMIN_PASS_ITEMUPDATE ); 
			}
			else if( s.Token == "SLOT_DONOT_CREATE" )
			{
				g_tSlotActionFlag.bNotCreate = true;
			}
			else if( s.Token == "SLOT_DONOT_DELETE" )
			{
				g_tSlotActionFlag.bNotDelete = true;
			}

			s.GetToken();
		}

		//////////////////////////////////////////////////////////////////////////
		const TCHAR szFileName[]	= _T( "MoveScript.inc" );
		if( g_pScriptFileManager->LoadMoveFileList( szFileName ) == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED MOVE SCRIPT FILE. PLEASE CHECK:(%s) ]" ), szFileName );
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////

		if( !prj.OpenProject( "masquerade.prj" ) )
		{
			g_MyTrace.Add( FLMyTrace::Key( "status" ), FALSE, "Fail to open project" );
			return FALSE;
		}
		else
		{
			g_MyTrace.Add( FLMyTrace::Key( "status" ), FALSE, "Ready" );
			return TRUE;
		}
	}

	g_MyTrace.Add( FLMyTrace::Key( "status" ), FALSE, "Fail to open file, %s", lpszFileName );
	return FALSE;
};
