#include "stdafx.h"
#include "dbmanager.h"

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include <..\Resource\Lang.h>

//#include "../_CommonDefine/Spec/FLShutdownRule_Spec.h"
#include "../_Common/FLShutdownRule.h"
#include "../_Common/FLContentSwitch.h"

#include "user.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// extern
extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;
extern	UINT				_HashKey( const char* key );

/////////////////////////////////////////////////////////////////////////////////////////////////
// global
CDbManager	g_DbManager;
HANDLE		s_hHandle	= (HANDLE)NULL;
extern std::string		g_strWebCertURL_NotGPotato;	//	일본 웹 인증 관련
extern std::string		g_strWebCertURL_GPotato;	//	일본 웹 인증 관련

extern	char	USA_AUTH_WEB_URL[64];
extern	char	USA_AUTH_WEB_PAGE[64];
extern	int		USA_AUTH_PORT;

extern	char	PHILIPPINES_AUTH_WEB_URL[64];
extern	char	PHILIPPINES_AUTH_WEB_PAGE[64];
extern	int		PHILIPPINES_AUTH_PORT;
extern	int		PHILIPPINES_AUTH_PORT_IPBONUS;
/////////////////////////////////////////////////////////////////////////////////////////////////
CDbManager::CDbManager()
{
	m_pDbIOData		= new FLMemPooler<DB_OVERLAPPED_PLUS>(512);
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= NULL;
		m_hDbWorkerThread[i] = NULL;
	}

	memset( m_szLogin_DSN_Name, 0, sizeof( m_szLogin_DSN_Name ) );
	memset( m_szLogin_DB_AdminId, 0, sizeof( m_szLogin_DB_AdminId ) );
	memset( m_szLogin_PWD, 0, sizeof( m_szLogin_PWD ) );
}

CDbManager::~CDbManager()
{
	SAFE_DELETE( m_pDbIOData );
}

TCHAR*	CDbManager::GetDBConnectString( DB_CONNECT_TYPE nType )
{
	switch( nType )
	{
	case LOGIN_DSN_NAME:
		return m_szLogin_DSN_Name;

	case LOGIN_DB_ADMIN_ID:
		return m_szLogin_DB_AdminId;

	case LOGIN_PASSWORD:
		return m_szLogin_PWD;

	default:
		break;
	}

	return _T( "\0" );
}

void	CDbManager::SetDBConnectString( DB_CONNECT_TYPE nType, const char* pString )
{
	switch( nType )
	{
	case LOGIN_DSN_NAME:
		FLStrcpy( m_szLogin_DSN_Name, _countof( m_szLogin_DSN_Name ), pString );
		break;

	case LOGIN_DB_ADMIN_ID:
		FLStrcpy( m_szLogin_DB_AdminId, _countof( m_szLogin_DB_AdminId ), pString );
		break;

	case LOGIN_PASSWORD:
		FLStrcpy( m_szLogin_PWD, _countof( m_szLogin_PWD ), pString );
		break;

	default:
		break;
	}
}

BOOL CDbManager::CreateDbWorkers()
{
	s_hHandle = CreateEvent( NULL, FALSE, FALSE, NULL );

	bool	bUSAAuthSetting = false;

	if( FLStrlen( USA_AUTH_WEB_URL, sizeof(USA_AUTH_WEB_URL) ) > 0 && FLStrlen( USA_AUTH_WEB_PAGE, sizeof(USA_AUTH_WEB_PAGE) ) > 0 )
	{
		bUSAAuthSetting	= true;
	}


	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		FLASSERT( m_hIOCP[i] );
		HANDLE hThread;
		if( g_xFlyffConfig->GetMainLanguage() == LANG_GER || g_xFlyffConfig->GetMainLanguage() == LANG_FRE )
		{
			hThread	= chBEGINTHREADEX( NULL, 0, GPotatoAuthWorker, (LPVOID)i, 0, NULL );
		}
		else if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP && g_strWebCertURL_GPotato.empty() == false && g_strWebCertURL_NotGPotato.empty() == false )
		{
			hThread	= chBEGINTHREADEX( NULL, 0, JapanAuthWorker, (LPVOID)i, 0, NULL );
		}
		else if( g_xFlyffConfig->GetMainLanguage() == LANG_ID )
		{
			hThread = chBEGINTHREADEX( NULL, 0, IndonesiaAuthWorker, (LPVOID)i, 0, NULL );
		}
		else if( g_xFlyffConfig->GetMainLanguage() == LANG_USA && bUSAAuthSetting == true )
		{
			hThread	= chBEGINTHREADEX( NULL, 0, USAAuthWorker, (LPVOID)i, 0, NULL );
		}
		else
		{
			hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)i, 0, NULL );
		}

		FLASSERT( hThread );
		m_hDbWorkerThread[i]	= hThread;
		if( WaitForSingleObject( s_hHandle, SEC( 10 ) ) == WAIT_TIMEOUT )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "CERTIFIER.EXE\t// TIMEOUT\t// ODBC" ) );
			return FALSE;
		}
	}

	CloseHandle( s_hHandle );
	return TRUE;
}

void	CDbManager::DestroyDbWorkers()
{
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		PostQueuedCompletionStatus( m_hIOCP[i], 0, NULL, NULL );
	}

	WaitForMultipleObjects( DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThread, TRUE, INFINITE );

	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		CLOSE_HANDLE( m_hIOCP[i] );
		CLOSE_HANDLE( m_hDbWorkerThread[i] );
	}
}


void CDbManager::SQLAddAccount( CQuery & query, char* szAccount, char* /*szPassword*/, BOOL bGM )
{
	query.Clear();
	query.ClearParam();
	char szSQL[100]	= { 0,};

	char chAuth	= ( bGM? 'P': 'F' );

	TCHAR szPW[5]	= { 0, };
	FLStrcpy( szPW, _countof( szPW ), "0000" );		// 통계자료에서만 쓰인다.. 패스워드 정보는 굳이 남길 필요 없어서..

//	FLSPrintf( szSQL, _countof( szSQL ), "uspAddAccount '%s', '%s', '%c'", szAccount, szPassword, chAuth );
	FLSPrintf( szSQL, _countof( szSQL ), "uspAddAccount '%s', '%s', '%c'", szAccount, szPW, chAuth );

	if( query.Exec( szSQL ) == FALSE )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "uspAddAccount is Error. Account[%s], szPassword[%s], chAuth[%c]" ), szAccount, szPassword, chAuth );
		FLERROR_LOG( PROGRAM_NAME, _T( "uspAddAccount is Error. Account[%s], szPassword[%s], chAuth[%c]" ), szAccount, szPW, chAuth );
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

bool CDbManager::DBQryAccount( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pData )
{
#if __VER > 20
#pragma message( "함수 지워라 ")
#endif


	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "{call LOGIN_STR(" );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "%s?,?,?,?)}", szQuery );
	}
	else
	{
		FLSPrintf( szQuery, _countof( szQuery ), "%s?,?,?)}", szQuery );
	}

	TCHAR szAddr[ 16 ] = { 0, };
	g_dpCertifier.GetPlayerAddr( pData->AuthInfo.dpId, szAddr, _countof( szAddr ) );

	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 32, pData->AuthInfo.AccountInfo.szAccount );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 32, pData->AuthInfo.AccountInfo.szPassword );
	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
	{
		pQuery->BindParameterVarChar( SQL_PARAM_INPUT,	50,	pData->AuthInfo.AccountInfo.szSessionPwd );
	}
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,		15,	szAddr );

	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %s, %s" )
			, szQuery, pData->AuthInfo.AccountInfo.szAccount, pData->AuthInfo.AccountInfo.szPassword
			, pData->AuthInfo.AccountInfo.szSessionPwd, szAddr );
		return false;
	}

	return true;
}



bool CDbManager::Query_Account( CQuery & query, LPDB_OVERLAPPED_PLUS pData )
{
	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };
	FLSPrintf( szQuery, _countof( szQuery ), "{call test_LOGIN_STR(" );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
		FLSPrintf( szQuery, _countof( szQuery ), "%s?,?,?,?,?,?,?)}", szQuery );
	else
		FLSPrintf( szQuery, _countof( szQuery ), "%s?,?,?,?,?,?)}", szQuery );

	TCHAR szAddr[ 16 ] = { 0, };
	g_dpCertifier.GetPlayerAddr( pData->AuthInfo.dpId, szAddr, _countof( szAddr ) );

	//in
	query.BindParameterVarChar( SQL_PARAM_INPUT,		32,	pData->AuthInfo.AccountInfo.szAccount );
	query.BindParameterVarChar( SQL_PARAM_INPUT,		32,	pData->AuthInfo.AccountInfo.szPassword );
	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
	{
		query.BindParameterVarChar( SQL_PARAM_INPUT,	50,	pData->AuthInfo.AccountInfo.szSessionPwd );
	}
	query.BindParameterVarChar( SQL_PARAM_INPUT,		15,	szAddr );

	int nShutdownAge		= FLShutdownRule::SHUTDOWN_AGE;
	query.BindParameterInt( SQL_PARAM_INPUT,			&nShutdownAge );
	int nShutdownStartHour	= FLShutdownRule::SHUTDOWN_START_HOUR;
	query.BindParameterInt( SQL_PARAM_INPUT,			&nShutdownStartHour );
	int nShutdownEndHour	= FLShutdownRule::SHUTDOWN_END_HOUR;
	query.BindParameterInt( SQL_PARAM_INPUT,			&nShutdownEndHour );

	//exec
	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %s, %s" )
			, szQuery, pData->AuthInfo.AccountInfo.szAccount, pData->AuthInfo.AccountInfo.szPassword
			, pData->AuthInfo.AccountInfo.szSessionPwd, szAddr );
		return false;
	}

	return true;
}
// END110119
//////////////////////////////////////////////////////////////////////////

// AccountFlag를 얻는다.
// f18 - 디비에서 얻는 데이타 
//       태국: 2 - 미성년자, 4 - 미등록 성인, 8 - 등록된 성인 
//       그외: 0 - 미성년자, 1 - 성인 

BYTE CDbManager::GetAccountFlag( int f18, LPCTSTR szAccount )
{
	BYTE cb18 = (BYTE)f18;
	BYTE cbAccountFlag = 0x00;

	if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
	{
		if( cb18 & 0x04 )
			cbAccountFlag |= ACCOUNT_FLAG_UNREGISTER18;		// 미등록 성인
		else if( cb18 & 0x08 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}
	else
	{
		if( cb18 & 0x01 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}

	if( IsEveSchoolAccount( szAccount ) )
		cbAccountFlag |= ACCOUNT_FLAG_SCHOOLEVENT;

//	FLTRACE_LOG( PROGRAM_NAME, _T( "%s - f18:%02x cbAccountFlag:%02x" ), szAccount, cb18, cbAccountFlag );

	return cbAccountFlag;
}

bool	CDbManager::OnCertifyQueryOK( CQuery & query, LPDB_OVERLAPPED_PLUS pData, const char* szCheck )
{
	CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
	CUser* pUser = CUserMng::GetInstance()->GetUser( pData->AuthInfo.dpId );
	if( pUser == NULL )
	{
		// 접속이 끊어졌다.
		return false;
	}

	const int nLang = g_xFlyffConfig->GetMainLanguage();

	const BOOL bGPotatoAuth	= nLang == LANG_GER || nLang == LANG_FRE || nLang == LANG_JAP || nLang == LANG_ID || nLang == LANG_USA || nLang == LANG_PHP;		// web login?

	CTime tmBirthDate;
	int n18	= 1;
	if( bGPotatoAuth == FALSE )
	{
//		if( _GetContentState( CT_SHUTDOWN_RULE  ) == CS_VER1 )
		{
			char szBirthDate[ FLTm::DATE_STRING_MAX ] = { 0 };

			if( query.IsValidColumn( "fBirth" ) == false )
			{
				tmBirthDate	= FLStringToCTime( CTIME_DATE_MIN_STR, FLTm::E_FORMAT::YYYYMMDD );

//				FLERROR_LOG( PROGRAM_NAME, _T( "OnCertifyQueryOK IsValidColumn( fBirth ) is false" ) );
			}
			else
			{
				query.GetStr( "fBirth", szBirthDate, _countof( szBirthDate ) );

				const int nBirthDate = FLDateStringToInt( szBirthDate );

				if( nBirthDate < CTIME_DATE_MIN )
				{
					tmBirthDate	= FLStringToCTime( CTIME_DATE_MIN_STR, FLTm::E_FORMAT::YYYYMMDD );

					FLERROR_LOG( PROGRAM_NAME, _T( "OnCertifyQueryOK fBirth is invalid data. [%s]" ), szBirthDate );
				}
				else
				{
					tmBirthDate = FLStringToCTime( szBirthDate, FLTm::E_FORMAT::YYYYMMDD );
				}
			}
		}

		if( query.IsValidColumn( "f18" ) == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OnCertifyQueryOK IsValidColumn( f18 ) is false" ) );
		}
		else
		{
			n18	= query.GetInt( "f18" );
		}
	}

	BYTE cbAccountFlag	= GetAccountFlag( n18, pData->AuthInfo.AccountInfo.szAccount );
	if( nLang == LANG_THA )
	{
		if( (cbAccountFlag & ACCOUNT_FLAG_18) == 0x00 )
		{
			CTime cur = CTime::GetCurrentTime();
			if( cur.GetHour() >= 22 || cur.GetHour() < 6 ) 
			{
				g_dpCertifier.SendError( ERROR_TOO_LATE_PLAY, pData->AuthInfo.dpId );
				return false;
			}
		}
	}


	int fCheck = 0;

	char lpAddr[16]	= { 0, };
	g_dpCertifier.GetPlayerAddr( pData->AuthInfo.dpId, lpAddr, _countof( lpAddr ) );

	//g_dpAccountClient.SendAddAccount( lpAddr, pData->AuthInfo.AccountInfo.szAccount, cbAccountFlag, pData->AuthInfo.dpId, fCheck, szCheck, pData->AuthInfo.AccountInfo.szBak, pData->AuthInfo.AccountInfo.dwPCBangClass );
	g_dpAccountClient.SendAddAccount( lpAddr, pData->AuthInfo.AccountInfo.szAccount, cbAccountFlag, pData->AuthInfo.dpId, fCheck, szCheck, pData->AuthInfo.AccountInfo.szBak, pData->AuthInfo.AccountInfo.dwPCBangClass, tmBirthDate );
	return true;
}

void CDbManager::Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr )
{
		
	if( pData->AuthInfo.dwIP && pData->AuthInfo.AccountInfo.szAccount[0] != '\0' )
	{
		ACCOUNT_CHECK result = CHECK_OK;
		result = accountMgr.Check( pData->AuthInfo );
		switch( result )
		{
		case CHECK_1TIMES_ERROR:
			g_dpCertifier.SendError( ERROR_15SEC_PREVENT, pData->AuthInfo.dpId );
			return;
		case CHECK_3TIMES_ERROR:
			g_dpCertifier.SendError( ERROR_15MIN_PREVENT, pData->AuthInfo.dpId );
			return;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	query.Clear();
	query.ClearParam();

	int nCode = ERROR_CERT_GENERAL;

	if(  g_xFlyffConfig->GetMainLanguage() == LANG_KOR		//한국만 셧다운제 쿼리
		? ( Query_Account( query, pData ) == true ) 
		: ( DBQryAccount( &query, pData ) == true ) )
//	if( DBQryAccount( &query, pData ) == true )
//	if( Query_Account( query, pData ) == true )
	{
		if( query.Fetch() )
		{
			int nError	= query.GetInt( "fError" );
			switch( nError )
			{
			case 0:
				pData->AuthInfo.AccountInfo.dwPCBangClass = query.GetInt( "fPCZone" );
				if( pData->AuthInfo.dwIP )
					accountMgr.ResetErrorCount();

				FLStrcpy( pData->AuthInfo.AccountInfo.szBak, _countof( pData->AuthInfo.AccountInfo.szBak ), pData->AuthInfo.AccountInfo.szAccount );

				OnCertifyQueryOK( query, pData );
				return;
			case 1:	// 암호틀림
				if( pData->AuthInfo.dwIP )
					accountMgr.IncErrorCount();

				nCode = ERROR_FLYFF_PASSWORD;
				break;
			case 3:	// 계정블럭이거나 유료화 초과
				nCode = ERROR_BLOCKGOLD_ACCOUNT;				
				break;
			case 4:	// 실명인증후 게임접속이 가능합니다 www.flyff.com으로 접속해주십시오
				nCode = ERROR_FLYFF_AUTH;
				break;
			case 5: // 프리프는 12세 이상 이용가 이므로 게임접속을 할수 없습니다.
				nCode = ERROR_FLYFF_PERMIT;
				break;
			case 6: // 14세 미만 가입자 분들은 부모님 동의서를 보내주셔야 게임 접속이 가능합니다. www.flyff.com 으로 접속하셔서 확인해 주세요.
				nCode = ERROR_FLYFF_NEED_AGREEMENT;
				break;
			case 7:	// Web탈퇴자 회원
				nCode = ERROR_FLYFF_NO_MEMBERSHIP;
				break;
			case 9:	// 실시간 데이터 작업 유저
				nCode = ERROR_FLYFF_DB_JOB_ING;
				break;
			case 11:
				nCode = ERROR_SHUTDOWN_RULE;
				break;
			case 91:
				nCode	= ERROR_FLYFF_EXPIRED_SESSION_PASSWORD;
				break;
			default: 
				nCode = ERROR_FLYFF_ACCOUNT;				
				break;
			}
		}
	}
	g_dpCertifier.SendError( nCode, pData->AuthInfo.dpId );

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

void CDbManager::CloseExistingConnection( CQuery & query, LPDB_OVERLAPPED_PLUS pData )
{
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	query.Clear();
	query.ClearParam();

	if(  g_xFlyffConfig->GetMainLanguage() == LANG_KOR		//한국만 셧다운제 쿼리
		? ( Query_Account( query, pData ) == true ) 
		: ( DBQryAccount( &query, pData ) == true ) )
//	if( DBQryAccount( &query, pData ) == true )
	{
		if( query.Fetch() )
		{
			if( 0 == query.GetInt( "fError" ) )
				g_dpAccountClient.SendCloseExistingConnection( pData->AuthInfo.AccountInfo.szAccount );
		}
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

u_int __stdcall DbWorkerThread( LPVOID nIndex )
{
	CAccountMgr mgr;

	CQuery query;
#ifdef __DbStuff
	if (!query.Connect(CQuery::SqlDatabase::ACCOUNT))
#else
	if( FALSE == query.Connect( 3, g_DbManager.GetDBConnectString( LOGIN_DSN_NAME )
		, g_DbManager.GetDBConnectString( LOGIN_DB_ADMIN_ID )
		, g_DbManager.GetDBConnectString( LOGIN_PASSWORD ) ) )
#endif
	{
		AfxMessageBox( "Error : Not Connect Login DB" );
	}

	SetEvent( s_hHandle );

	HANDLE hIOCP				= g_DbManager.GetIOCPHandle( (int)nIndex );
	DWORD dwBytesTransferred	= 0;

#if _WIN64
	ULONG_PTR lpCompletionKey = NULL;
#else
	LPDWORD lpCompletionKey = NULL;
#endif
	LPDB_OVERLAPPED_PLUS pData	= NULL;

	while( 1 )
	{
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										 &dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pData,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			FLASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		
		switch( pData->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify( query, pData, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection( query, pData );
				break;
			case ACCOUNT_LOGOUT:
				break;
			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "pData->nQueryMode Invalid : [%d]" ), pData->nQueryMode );
				}
				break;
		}

		if( pData != NULL )
		{
			g_DbManager.m_pDbIOData->Free( pData );
			pData = NULL;
		}
	}
	return( 0 );
}

BOOL CDbManager::LoadEveSchoolAccount( void )
{
	CScanner s;
	if( s.Load( "EveSchoolAccount.txt" ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			m_eveSchoolAccount.insert( (const char*)s.Token );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDbManager::IsEveSchoolAccount( const char* pszAccount )
{
	std::set<std::string>::iterator i	= m_eveSchoolAccount.find( pszAccount );
	return( i != m_eveSchoolAccount.end() );
}

void CDbManager::PostQ( LPDB_OVERLAPPED_PLUS pData, DWORD dwNumberOfBytesTransferred )
{
	UINT nKey = ::_HashKey( pData->AuthInfo.AccountInfo.szAccount );
	int nIOCP = nKey % DEFAULT_DB_WORKER_THREAD_NUM;

	::PostQueuedCompletionStatus( GetIOCPHandle( nIOCP ), dwNumberOfBytesTransferred, NULL, (LPOVERLAPPED)pData );
}

HANDLE CDbManager::GetIOCPHandle( int n )
{
	return m_hIOCP[n];
}

