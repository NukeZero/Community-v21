#include "stdafx.h"
#include "ThreadMng.h"
#include <WorldMng.h>
#include "User.h"
#include "WorldServer.h"
#include "GlobalTime.h"
#include <party.h>
#include <guildquest.h>

#include <eveschool.h>
extern	CGuildCombat	g_GuildCombatMng;

#include <SecretRoom.h>
#include <CreateMonster.h>
#include "rangda.h"
#include <RainbowRace.h>
#include <PCBang.h>
#include <InstanceDungeonParty.h>

#include <Quiz.h>

#include <spevent.h>
#include <guild.h>
#include <guildwar.h>

#include <serialnumber.h>
#include "NetSerialNumberRequest.h"

#include "FLRandomRespawnNPC.h"
#include "FLCooperativeContributions.h"

#include "FLEventArenaGlobal.h"

#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

#include "FLShutdownRule_Mng.h"

#include "FLFlyffPieceDailyPayment.h"

extern	char				g_szCacheBindIP[16];
extern	char				g_szDBAddr[16];
extern	char				g_szCacheAddr[16];
extern	char				g_szCoreAddr[16];
extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;

extern	CWorldMng			g_WorldMng;
extern  DWORD				g_tmCurrent;
//extern	time_t	g_tCurrent;
extern	char				g_sHeartbeat[32];
extern	CPartyMng			g_PartyMng;

extern  BOOL LoadAIScript();		// aimonster2.cpp

void LogPerformance( DWORD dwCurTick );

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTimeout
////////////////////////////////////////////////////////////////////////////////////////////////////////

CTimeout::CTimeout( DWORD dwInterval, DWORD dwFirstTimeout )
{
	m_dwInterval		= dwInterval;
	m_endTime	= timeGetTime() + dwFirstTimeout;
}

DWORD CTimeout::Over( DWORD dwCurr )
{
	if( dwCurr > m_endTime )
		return ( dwCurr - m_endTime );
	return 0;
}
void CTimeout::Reset( DWORD dwCurr, DWORD dwTimeout )
{
	if( dwTimeout > m_dwInterval )
		m_endTime	= dwCurr;
	else
		m_endTime	= dwCurr + ( m_dwInterval - dwTimeout );
}
BOOL CTimeout::TimeoutReset( DWORD dwCurr )
{
	DWORD dwTimeout	= Over( dwCurr );
	if( dwTimeout > 0 )
	{
		Reset( dwCurr, dwTimeout );
		return TRUE; 
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogPerformance( DWORD dwCurTick )
{
	static int nFrame = 0;			
	static DWORD dwPrev = 0;
	static DWORD dwElapsed = 0;

	++nFrame;
	DWORD dwTick = dwCurTick - dwPrev; 
	dwPrev = dwCurTick;
	dwElapsed += dwTick;

	if( dwTick > 1000 )
	{
		SetLogInfo( LOGTYPE_WARN1, "warning - tick:%d", dwTick );
		FLWARNING_LOG( PROGRAM_NAME, _T( "warning - tick:%u" ), dwTick );
	}

	if( dwElapsed > 1000 )
	{
//#ifdef _DEBUG
		//static int nHour = 0;
		//if( nHour != g_GameTimer.m_nHour )
		//{

		//	//OutputDebugStr( str );
		//	
		//}
		//TCHAR str[ 1024 ];
		////nHour	= g_GameTimer.m_nHour;
		//FLSPrintf( str, _countof( str ), _T("gametime : %04d %02d:%02d:%02d	\r\n"), g_GameTimer.m_nDay, g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
		//SetLogInfo( LOGTYPE_CCU, str );
//#else
		SetLogInfo( LOGTYPE_CCU, "CCU:%d", g_xWSUserManager->GetCount() );
//#endif //_DEBUG

		SetLogInfo( LOGTYPE_PERFOMANCE, "frame:%d tick:%d", nFrame, dwTick );
		SetLogInfo( LOGTYPE_REDRAW, "" );
		SetLogInfo( LOGTYPE_DBDISCONNECT, g_dpDBClient.GetAlive() ? "DbConnect" : "DbDisconnect" );
		SetLogInfo( LOGTYPE_COREDISCONNECT, g_DPCoreClient.GetAlive() ? "CoreConnect" : "CoreDisconnect" );

		dwElapsed = 0; // -= 1000
		nFrame = 0;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRunObject
////////////////////////////////////////////////////////////////////////////////////////////////////////
CRunObject::CRunObject()
{
	m_hRunObject	= (HANDLE)0;
	m_hClose	= (HANDLE)0;
}

CRunObject::~CRunObject()
{
	Close();
}

BOOL CRunObject::Init( void )
{
	if( LoadAIScript() == FALSE )
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// 시리얼 넘버 초기화
	if( CSerialNumber::GetInstance()->Init( CNetSerialNumberRequest::GetInstance() ) == false )
	{
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	if( !g_dpDBClient.Run( g_szDBAddr, PN_DBSRVR_1, TRUE ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't connect to database server." ) );
		return FALSE;
	}

// 
	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	DWORD dwThreadId;
	m_hRunObject	= chBEGINTHREADEX( NULL, 0, _Run, (LPVOID)this, 0, &dwThreadId ); 
//


	if( !g_DPSrvr.StartServer( g_szCacheBindIP, (u_short)( g_uKey + PN_WORLDSRVR ), FLNormalProtocol::GetInstance(), TRUE ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't start server." ) );
		return FALSE;
	}
	if( !g_DPCoreClient.Run( g_szCoreAddr, PN_CORESRVR + 0, g_uKey ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't connect to core server." ) );
		return FALSE;
	}

// 	if ( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
// 		g_dpDBClient.SendQueryGuildBank( g_uKey );
	if( g_eLocal.GetState( EVE_WORMON ) )
		g_dpDBClient.SendQueryGuildQuest();

	if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
		g_dpDBClient.SendRainbowRaceReqLoad();

	g_dpDBClient.CalluspXXXMultiServer( g_uIdofMulti, NULL );
	if( !CXMasEvent::GetInstance()->LoadScript( "spevent.txt" ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "can't read spevent.txt" ) );
		return FALSE;
	}
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
#ifdef __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "..\\script\\school.txt" ) )	//
#else	// __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "school.txt" ) )
#endif	// __IDC
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "school.txt not found" ) );
			return FALSE;
		}
	}
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) && !g_GuildCombatMng.LoadScript( "GuildCombat.txt" ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "GuildCombat.txt not found" ) );
		return FALSE;
	}

	return TRUE;
}

void CRunObject::Close( void )
{
	CLOSE_THREAD( m_hRunObject, m_hClose );
}

u_int CRunObject::_Run( LPVOID pParam )
{
	CRunObject* pRunObject	= (CRunObject*)pParam;
	pRunObject->Run();
	return 0;
}

void CRunObject::Run( void )
{
	FLDumpInstaller::InstallThreadExceptionHandler();

	HANDLE hHeartbeat = CreateEvent( NULL, FALSE, FALSE, g_sHeartbeat );
	if( !( hHeartbeat ) )
		hHeartbeat	= OpenEvent( EVENT_MODIFY_STATE, FALSE, g_sHeartbeat );
	if( NULL == hHeartbeat )
		FLERROR_LOG( PROGRAM_NAME, _T( "MAPI_E_NOT_ENOUGH_RESOURCES" ) );
	DWORD dwTickHearbeat = timeGetTime() + SEC( 2 );

	CTimeout	timeoutObject( 67, 0 );
	CTimeout	timeoutReadAgent( 67, 20 );
	CTimeout	timeoutReadCore( 67, 40 );
	CTimeout	timeoutReadTrans( 67, 50 );
	CTimeout	timeoutReadAccount( 67, 60 );
	CTimeout	timeoutRespawn( 1000, 200 );
	CTimeout	timeout( 1000, 600 );
	CTimeout	timeoutCallTheRoll( MIN( 1 ), 70 );

	while( WaitForSingleObject( m_hClose, 1 ) != WAIT_OBJECT_0 )
	{
		{
			g_tmCurrent		= timeGetTime();
			g_tCurrent	= time( NULL );

			if( g_tmCurrent > dwTickHearbeat )
			{
				if( SetEvent( hHeartbeat ) == FALSE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Heartbeat SetEvent failed, GetLastError: %d" ), ::GetLastError() );
				}

				dwTickHearbeat = g_tmCurrent + SEC( 2 );
			}

			if( timeoutObject.TimeoutReset( g_tmCurrent ) )
			{
				g_WorldMng.Process();
			}

			if( timeoutReadAgent.TimeoutReset( g_tmCurrent ) )
			{
				g_DPSrvr.ReceiveMessage();
			}

			if( timeoutReadCore.TimeoutReset( g_tmCurrent ) )
			{
				g_DPCoreClient.ReceiveMessage();
			}

			if( timeoutReadTrans.TimeoutReset( g_tmCurrent ) )
			{
				g_dpDBClient.ReceiveMessage();
			}
		
		#ifdef __EVENTLUA_KEEPCONNECT
			if( timeoutReadAccount.TimeoutReset( g_tmCurrent ) )
				prj.m_EventLua.KeepConnectEventProcess();
		#endif // __EVENTLUA_KEEPCONNECT

			if( timeoutRespawn.TimeoutReset( g_tmCurrent ) )
			{
				u_long uRespawned = g_WorldMng.Respawn();
//				if( uRespawned > 0 )
					SetLogInfo( LOGTYPE_RESPAWN, "Respawn:%d Object:%d", uRespawned, g_WorldMng.GetObjCount() );
			}

			if( timeout.TimeoutReset( g_tmCurrent ) )
			{
				g_GameTimer.Compute();
				CGuildQuestProcessor::GetInstance()->Process();
				CPartyQuestProcessor::GetInstance()->Process();

				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					g_GuildCombatMng.Process();

				if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
					g_GuildCombat1to1Mng.Process();

				if( g_eLocal.GetState( EVE_SECRETROOM ) )
				{
					CSecretRoomMng::GetInstance()->Process();
				}

				if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
				{
					CRainbowRaceMng::GetInstance()->Process();
				}

				{
					CPCBang::GetInstance()->ProcessPCBang();
				}

				{
					CInstanceDungeonParty::GetInstance()->Process();
				}

				{
					if( CQuiz::GetInstance()->IsAutoMode() && CQuiz::GetInstance()->IsRun() )
						CQuiz::GetInstance()->Process();
				}

				if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					g_GuildMng.Process();

				if( g_eLocal.GetState( EVE_GUILDWAR ) )
					g_GuildWarMng.Process();

//				g_PartyMng.PartyMapInfo();

				// 랜덤 이벤트 몬스터 초당 1회 틱
				CRangdaController::Instance()->OnTimer();

	#ifdef __EVENTLUA_SPAWN
				prj.m_EventLua.EventSpawnProcess();
	#endif // __EVENTLUA_SPAWN
			}

			if( timeoutCallTheRoll.TimeoutReset( g_tmCurrent ) )	// 1분당 
			{
				CEventGeneric::GetInstance()->CallTheRoll();
				CCreateMonster::GetInstance()->ProcessRemoveMonster();

				using namespace nsFlyffPieceDailyPayment;
				g_pFlyffPieceDailyPayment->Process( g_tCurrent );
			}

#ifdef __EVENT_0117
			CEventGeneric::GetInstance()->Spawn();
#endif	// __EVENT_0117

			//neodeath 알람
			//DATETIME_ALARMCLOCK().Update( DATE_TIMER().GetCurrentTime() );
			PERIOD_REPETITION_ALARM_MNG().Update();

//			//협동기부
			if( _GetContentState( CT_DONATION ) == CS_VER1 )
				COOPERATIVE_CONTRIBUTIONS().TryClean_ExpiredGlobalReward();				//만료 체크

			if( g_pEventArenaGlobal->IsArenaChannel() )
			{
				g_pEventArena->Run();
			}


			g_PartyMng.PartyMapInfo();


			//if( _GetContentState( CT_SHUTDOWN_RULE ) == CS_VER1 )
			if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
				FLShutdownRule_Mng::GetInstance().TryShutdown();
		}
		
		LogPerformance( timeGetTime() );
	
	}
	CLOSE_HANDLE( m_hClose );
	CLOSE_HANDLE( hHeartbeat );
}

CRunObject*	CRunObject::GetInstance( void )
{
	static CRunObject	sRunObject;
	return &sRunObject;
}
