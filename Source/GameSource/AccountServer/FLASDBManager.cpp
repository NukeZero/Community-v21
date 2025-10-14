#include "stdafx.h"
#include "FLASDBManager.h"
#include "account.h"
#include <afxdisp.h>
#include <lang.h>


extern	CAccountMng		g_AccountMng;
#include "dpdbsrvr.h"
extern	CDPDBSrvr	g_dpDbSrvr;
///////////////////////////////////////////////////////////////////////////////////////////////////
// FLASDBManager
///////////////////////////////////////////////////////////////////////////////////////////////////

FLASDBManager*	FLASDBManager::GetInstance()
{
	static FLASDBManager xFLASDBManager;

	return	&xFLASDBManager;
}


FLASDBManager::FLASDBManager()
{
	m_pDbIOData	= new FLMemPooler<T_AS_DBOVERLAPPED>(512);

	m_szLoginPWD[0] = '\0';
}

FLASDBManager::~FLASDBManager()
{
	SAFE_DELETE( m_pDbIOData );
}

HANDLE s_hHandle	= (HANDLE)NULL;

void	FLASDBManager::CreateDbWorkers( void )
{
	s_hHandle	= CreateEvent( NULL, FALSE, FALSE, NULL );

	m_hDbCompletionPort		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	FLASSERT( m_hDbCompletionPort );
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		HANDLE hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)this, 0, NULL );
		FLASSERT( hThread );
		m_hDbWorkerThreadTerminate[i]	= hThread;

		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// TIMEOUT\t// ODBC" ) );
	}

	CloseHandle( s_hHandle );

	return;
}

void	FLASDBManager::CloseDbWorkers( void )
{
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
		PostQueuedCompletionStatus( m_hDbCompletionPort, 0, NULL, NULL );

	WaitForMultipleObjects( DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThreadTerminate, TRUE, INFINITE );

	CLOSE_HANDLE( m_hDbCompletionPort );

	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ ) {
		CLOSE_HANDLE( m_hDbWorkerThreadTerminate[i] );
	}

	SAFE_DELETE( m_pDbIOData );

	return;
}

void	FLASDBManager::GetStrTime( CTime *pTime, char *strbuf )
{
	char sYear[5]	= { 0, };
	char sMonth[3]	= { 0, };
	char sDay[3]	= { 0, };
	char sHour[3]	= { 0, };
	char sMin[3]	= { 0, };

	FLStrncpy( sYear, _countof( sYear ), strbuf, 4 );
	FLStrncpy( sMonth, _countof( sMonth ), strbuf + 4, 2 );
	FLStrncpy( sDay, _countof( sDay ), strbuf + 6, 2 );
	FLStrncpy( sHour, _countof( sHour ), strbuf + 8, 2 );
	FLStrncpy( sMin, _countof( sMin ), strbuf + 10, 2 );

	*pTime	= CTime( atoi( sYear ), atoi( sMonth ), atoi( sDay ), atoi( sHour ), atoi( sMin ), 0 );

	return;
}

void	FLASDBManager::DBQryAccount( char* qryAccount, size_t cchAccount, char* Gu, char* szAccount )
{
	FLSPrintf( qryAccount, cchAccount, "LOGINJOIN_STR '%s', '%s'", Gu, szAccount );

	return;
}

void	FLASDBManager::DBQryLogSMItem( char* szSql, size_t cchSql, T_LOGSTR_DATA & info)
{
	if( g_xFlyffConfig->IsConnectLogDBMode() == false )
	{
		return;
	}

	const char * szBadDBString = "\'";
	const char * szConvertDBString = " ";

	FLStrConvertChars( info.szState, szBadDBString, szConvertDBString );
	FLStrConvertChars( info.szKilledPlayerName, szBadDBString, szConvertDBString );
	FLStrConvertChars( info.szGetPlayerName, szBadDBString, szConvertDBString );
	FLStrConvertChars( info.szItemName, szBadDBString, szConvertDBString );
	FLStrConvertChars( info.szStartTime, szBadDBString, szConvertDBString );
	FLStrConvertChars( info.szRemoteIP, szBadDBString, szConvertDBString );

	FLSPrintf( szSql, cchSql, "LOG_STR '%s',@im_idPlayer='%07d',@iserverindex='%02d',@im_nExp1=%I64d,@im_nLevel=%d,"
		"@im_nJob=%d,@im_nJobLv=%d,@im_nFlightLv=%d,@im_nStr=%d,@im_nDex=%d,"
		"@im_nInt=%d,@im_nSta=%d,@im_nRemainGP=%d,@im_nRemainLP=%d,@iState='%s',"
		"@idwWorldID=%d,@ikilled_m_szName='%s',@im_vPos_x=%f,@im_vPos_y=%f,@im_vPos_z=%f,"
		"@iattackPower=%d,@imax_HP=%d,@im_GetszName='%s',@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@iItem_count=%d,@iItem_AddLv=%d,"
		"@iStart_Time='%s',@iTotalPlayTime=%d,@iremoteIP='%s',@iQuest_Index=%d,@ikilled_m_idPlayer='%02d'",
		info.szType, info.dwPlayerID, info.dwServerIndex, info.n64Exp1, info.nLevel,
		info.nJob, info.nJobLv, info.nFlightLv, info.nStr, info.nDex,
		info.nInt, info.nSta, info.nRemainGP, info.nRemainLP, info.szState,
		info.nWorldID, info.szKilledPlayerName, info.fPosX, info.fPosY, info.fPosZ,
		info.nAttackPower, info.nMaxHP, info.szGetPlayerName, info.dwGold, info.nRemainGold,
		info.nItemUniqueNo, info.szItemName, info.nItemDurability, info.nItemCount, info.nItemAddLv,
		info.szStartTime, info.nTotalPlayerTime, info.szRemoteIP, info.nQuestIndex, info.dwKillPlayerID );

	if( ::strlen(szSql) > 4096 )
	{
		FLASSERT(0);
	}

	return;
}


void	FLASDBManager::UpdateTracking( BOOL bON, LPCTSTR lpszAccount )
{
	if( g_xFlyffConfig->IsConnectAccountDBMode() == false )
	{
		return;
	}

	PT_AS_DBOVERLAPPED lpDbOverlappedPlus		= m_pDbIOData->Alloc();
	FLStrcpy( lpDbOverlappedPlus->szAccount, _countof( lpDbOverlappedPlus->szAccount ), lpszAccount );

	if( bON )
	{
		lpDbOverlappedPlus->nQueryMode	= ACCOUNT_ON;
	}
	else
	{
		lpDbOverlappedPlus->nQueryMode	= ACCOUNT_OFF;
	}

	PostQueuedCompletionStatus( m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );

	return;
}

// 모두 OFF로 해줌
bool	FLASDBManager::AllOff()
{
	if( g_xFlyffConfig->IsConnectAccountDBMode() == false )
	{
		return	true;
	}

	CQuery qry;
#ifdef __DbStuff
	if (!qry.Connect(CQuery::SqlDatabase::ACCOUNT))
#else
	if (FALSE == qry.Connect(3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, m_szLoginPWD))
#endif
	{
		AfxMessageBox( "Error AllOff: DB Connect Loing InitInstance Failed " );
		return	false;
	}

	char szQuery[1024] = {0,};
	DBQryAccount( szQuery, _countof( szQuery ), "A3" );

	if( FALSE == qry.Exec( szQuery ) )
	{
		AfxMessageBox( " DB Qry : ALL_OFF 구문 실패 " );
		return FALSE;
	}

	return	true;
}

void	FLASDBManager::AccountOn( CQuery & qry, PT_AS_DBOVERLAPPED lpDbOverlappedPlus )
{
	DBQryAccount( lpDbOverlappedPlus->szQuery, _countof( lpDbOverlappedPlus->szQuery ), "A1", lpDbOverlappedPlus->szAccount );

	if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), lpDbOverlappedPlus->szQuery );
	}

	return;
}


void	FLASDBManager::AccountOff( CQuery & qry, PT_AS_DBOVERLAPPED lpDbOverlappedPlus )
{
	// 康
	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
	{
		FLSPrintf( lpDbOverlappedPlus->szQuery, _countof( lpDbOverlappedPlus->szQuery ), "dbo.uspExtendSession @paccount = '%s'", lpDbOverlappedPlus->szAccount );
		if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), lpDbOverlappedPlus->szQuery );
	}

	DBQryAccount( lpDbOverlappedPlus->szQuery, _countof( lpDbOverlappedPlus->szQuery ), "A2", lpDbOverlappedPlus->szAccount );

	if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), lpDbOverlappedPlus->szQuery );
	}

	return;
}

void	FLASDBManager::LogSMItem( CQuery & qryLog, PT_AS_DBOVERLAPPED lpDbOverlappedPlus )
{
	if( g_xFlyffConfig->IsConnectLogDBMode() == false )
	{
		return;
	}

	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->dwBufSize );

	/*
	bi2.szBxaid		// 구매 번호
	bi2.dwItemId	// 아이템 인덱스
	bi2.dwItemNum	// 아이템 개수
	bi2.dwServerIndex	// 서버 인덱스
	bi2.dwPlayerId		// 플레이어 식별자
	bi2.dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
	sn		// 시리얼 번호
	*/

	BUYING_INFO2	bi2;
	SERIALNUMBER	iSerialNumber = 0;

	arRead.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	arRead >> iSerialNumber;

	// 구매번호는 : pszGetidPlayer
	// 아이템인덱스 : dwGold // ItemName은 형식에 안맞음

	T_LOGSTR_DATA tLog( "L3" );

	if( bi2.dwRetVal )
	{
		FLStrcpy( tLog.szState, _countof( tLog.szState ), "3" );		// 성공
	}
	else
	{
		FLStrcpy( tLog.szState, _countof( tLog.szState ), "4" );		// 실패
	}

	tLog.dwServerIndex = bi2.dwServerIndex;

	FLSPrintf( tLog.szKilledPlayerName, _countof( tLog.szKilledPlayerName ), "%07d", bi2.dwPlayerId );
	FLStrcpy( tLog.szGetPlayerName, _countof(tLog.szGetPlayerName), bi2.szBxaid );
	tLog.nItemUniqueNo		= iSerialNumber;
	tLog.nItemCount			= bi2.dwItemNum;
	tLog.dwGold				= bi2.dwItemId;

	DBQryLogSMItem( lpDbOverlappedPlus->szQuery, _countof( lpDbOverlappedPlus->szQuery ), tLog );

	if( FALSE == qryLog.Exec( lpDbOverlappedPlus->szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), lpDbOverlappedPlus->szQuery );
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DbWorkerThread
///////////////////////////////////////////////////////////////////////////////////////////////////

u_int __stdcall	DbWorkerThread( LPVOID lpvDbManager )
{
	FLASDBManager* pDbManager	= (FLASDBManager*)lpvDbManager;

	CQuery qryLogin;
	CQuery qryLog;

	if( g_xFlyffConfig->IsConnectAccountDBMode() == true )
	{
#ifdef __DbStuff
		if (!qryLogin.Connect(CQuery::SqlDatabase::ACCOUNT))
#else
		if( FALSE == qryLogin.Connect( 3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, pDbManager->m_szLoginPWD ) )
#endif
		{
			AfxMessageBox( " DB Login Connect Failed " );
			return 0;
		}
	}


	if( g_xFlyffConfig->IsConnectLogDBMode() == true )
	{
#ifdef __DbStuff
		if (!qryLog.Connect(CQuery::SqlDatabase::LOGGING))
#else
		if( FALSE == qryLog.Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pDbManager->m_szLogPWD ) )
#endif
		{
			AfxMessageBox( " DB Log Connect Failed " );
			return 0;
		}
	}

	SetEvent( s_hHandle );

	BOOL fOk;
	HANDLE hCompletionPort	= pDbManager->m_hDbCompletionPort;
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	PT_AS_DBOVERLAPPED lpDbOverlappedPlus		= NULL;

	while( 1 )
	{
		fOk	= GetQueuedCompletionStatus( hCompletionPort,
			&dwBytesTransferred,
			(LPDWORD)&lpCompletionKey,
			(LPOVERLAPPED*)&lpDbOverlappedPlus,
			INFINITE );

		if( fOk == FALSE ) 
		{	
			FLASSERT( 0 );
			if( lpDbOverlappedPlus != NULL )
			{
				g_xASDBManager->m_pDbIOData->Free( lpDbOverlappedPlus );
				lpDbOverlappedPlus = NULL;
			}
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
		{
			if( lpDbOverlappedPlus != NULL )
			{
				g_xASDBManager->m_pDbIOData->Free( lpDbOverlappedPlus );
				lpDbOverlappedPlus = NULL;
			}

			return( 0 );
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case	ACCOUNT_ON:
				{
					pDbManager->AccountOn( qryLogin, lpDbOverlappedPlus );
				}
				break;
			case	ACCOUNT_OFF:
				{
					pDbManager->AccountOff( qryLogin, lpDbOverlappedPlus );
				}
				break;
			case	LOG_SM_ITEM:
				{
					pDbManager->LogSMItem( qryLog, lpDbOverlappedPlus );
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "lpDbOverlappedPlus->nQueryMode Invalid : [%d]" ), lpDbOverlappedPlus->nQueryMode );
				}
				break;
		}

		if( lpDbOverlappedPlus != NULL )
		{
			g_xASDBManager->m_pDbIOData->Free( lpDbOverlappedPlus );
			lpDbOverlappedPlus = NULL;
		}
	}

	return( 0 );
}

