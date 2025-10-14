#include "stdafx.h"

#include "dbmanager.h"

#include <afxinet.h>

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include <..\Resource\Lang.h>
#include "User.h"
#include <json\json.h>

extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;

extern	CDbManager	g_DbManager;
extern	HANDLE		s_hHandle;

/////////////////////////////////////////////////////////////////////////////////////////////////
// global
char	PHILIPPINES_AUTH_WEB_URL[64]	= "";
char	PHILIPPINES_AUTH_WEB_PAGE[64]	= "";
int		PHILIPPINES_AUTH_PORT			= 0;
int		PHILIPPINES_AUTH_PORT_IPBONUS	= 0;

char	g_szMasterID[21] = "";
/////////////////////////////////////////////////////////////////////////////////////////////////

u_int __stdcall PhilippinesAuthWorker( LPVOID pParam )
{
	CAccountMgr mgr;
	CQuery query;

	if( FALSE == query.Connect( 3, g_DbManager.GetDBConnectString( LOGIN_DSN_NAME )
		, g_DbManager.GetDBConnectString( LOGIN_DB_ADMIN_ID )
		, g_DbManager.GetDBConnectString( LOGIN_PASSWORD ) ) )
	{
		AfxMessageBox( "can't connect to database : login" );
	}

	SetEvent( s_hHandle );

	HANDLE hIOCP	= g_DbManager.GetIOCPHandle( (int)pParam );
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS pov	= NULL;

	while( 1 )
	{
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										&dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pov,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			FLASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		FLINFO_LOG( PROGRAM_NAME, _T( "PhilippinesAuthWorker : [%d]" ), pov->nQueryMode  );
		switch( pov->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify_Philippines( query, pov, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection_Philippines( query, pov );
				break;
			case ACCOUNT_LOGOUT:
				CheckOutIPBonus();
				break;
			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "pData->nQueryMode Invalid : [%d]" ), pov->nQueryMode );
				}
				break;
		}

		if( pov != NULL )
		{
			g_DbManager.m_pDbIOData->Free( pov );
			pov = NULL;
		}
	}
	return( 0 );
}

void CDbManager::Certify_Philippines( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr )
{
	if( pov->AuthInfo.dwIP && pov->AuthInfo.AccountInfo.szAccount[0] != '\0' )
	{
		ACCOUNT_CHECK result	= CHECK_OK;
		result	= mgr.Check( pov->AuthInfo );

		switch( result )
		{
			case CHECK_1TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15SEC_PREVENT, pov->AuthInfo.dpId );
				return;
			case CHECK_3TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15MIN_PREVENT, pov->AuthInfo.dpId );
				return;
		}
	}

	T_PHILIPPINES_AUTH_RESLUT	tAuthResult = { 0 };

	GetPhilippinesAuthResult( pov, tAuthResult );
	FLINFO_LOG( PROGRAM_NAME, _T( "%d, %s, %s, %d" ), tAuthResult.nResult, tAuthResult.szResult, tAuthResult.szMaster_id, tAuthResult.nAkey_flag );

	if( tAuthResult.nResult== 0 )
	{
		if( pov->AuthInfo.dwIP )
			mgr.ResetErrorCount();

		CheckInIPBonus( pov->AuthInfo.dpId, tAuthResult );

		FLStrcpy( pov->AuthInfo.AccountInfo.szBak, _countof( pov->AuthInfo.AccountInfo.szBak ), pov->AuthInfo.AccountInfo.szAccount );
		//FLStrcpy( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), tAuthResult.szGPotatoID );
		//FLSPrintf( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), "%d", tAuthResult.nGPotatoNo );
		//FLSPrintf( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), "%s %d", tAuthResult.szGPotatoID, tAuthResult.nGPotatoNo );
		//CUser* pUser = CUserMng::GetInstance()->GetUser( pov->AuthInfo.dpId );
		//if ( pUser )	pUser->nGPotatoNo = tAuthResult.nGPotatoNo;
		OnCertifyQueryOK( query, pov, "" );

		//SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, tAuthResult.bGM );
		//SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, false );
	}
	/*else if( tAuthResult.nResult == WEBZEN_ACCOUNT_ID_CONVERTING_FAIL ) {
		g_dpCertifier.SendErrorCodeString( ERROR_WEBZEN_ACCOUNT_ID_CONVERTING_FAIL, tAuthResult.szResult, pov->AuthInfo.dpId );
	}
	else if( tAuthResult.nResult == WEBZEN_GDPR_AGREEMENT ) {
		g_dpCertifier.SendErrorCodeString( ERROR_WEBZEN_GDPR_AGREEMENT, tAuthResult.szResult, pov->AuthInfo.dpId );
	}
	else if( tAuthResult.nResult == WEBZEN_TERMS_AGREEMENT ) {
		g_dpCertifier.SendErrorCodeString( ERROR_WEBZEN_TERMS_AGREEMENT, tAuthResult.szResult, pov->AuthInfo.dpId );
	}
	else if( tAuthResult.nResult == WEBZEN_TRY_CONNECT_BLOCKING_AREA ) {
		g_dpCertifier.SendErrorCodeString( ERROR_WEBZEN_TRY_CONNECT_BLOCKING_AREA, tAuthResult.szResult, pov->AuthInfo.dpId );
	}*/
	else
	{
		g_dpCertifier.SendErrorString( tAuthResult.szResult, pov->AuthInfo.dpId );
	}
}

void CDbManager::CloseExistingConnection_Philippines( CQuery & query, LPDB_OVERLAPPED_PLUS pov )
{
	T_PHILIPPINES_AUTH_RESLUT	tAuthResult = { 0 };
	GetPhilippinesAuthResult( pov, tAuthResult );

	if( tAuthResult.nResult == 0 )
	{
		//FLStrcpy( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), tAuthResult.szGPotatoID );
		g_dpAccountClient.SendCloseExistingConnection( pov->AuthInfo.AccountInfo.szAccount );

		//SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, tAuthResult.bGM );
		//SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, false );
		CheckInIPBonus( pov->AuthInfo.dpId, tAuthResult );
	}

	return;
}


void	GetPhilippinesAuthResult( LPDB_OVERLAPPED_PLUS pov, T_PHILIPPINES_AUTH_RESLUT & result )
{
	CInternetSession	session( _T( "PhilippinesAuth" ) );

	CHttpConnection*	pServer	= NULL;
	CHttpFile*			pFile	= NULL;

	char	pszSendMsg[700]	= { 0,};
	CString	strHeaders;
	
	int		nReadSize		= 0;

	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->AuthInfo.dpId, sAddr, _countof( sAddr ) );

	char sDomain[3][5] = { "INPP", "INFB", "INGG" };

	FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "action=LOGIN&domain=%s&id=%s&pw=%s&ip=%s&ref=%d"
		, sDomain[pov->AuthInfo.AccountInfo.nSiteValue], pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr, PHILIPPINES_AUTH_PORT );
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";

	FLINFO_LOG( PROGRAM_NAME, _T( "SendMsg : [%s]" ), pszSendMsg );
	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100519
	DWORD	dwStartTick	= 0;
	DWORD	dwEndTick	= 0;

	dwStartTick	= ::GetTickCount();
	//	END100519
	//////////////////////////////////////////////////////////////////////////

	try
	{
		pServer	= NULL;

		TCHAR szWebCertURL[128] = {0, };
		FLStrcpy( szWebCertURL, _countof( szWebCertURL ), PHILIPPINES_AUTH_WEB_URL );

		const INTERNET_PORT nPort	= PHILIPPINES_AUTH_PORT <= 0 ? INTERNET_INVALID_PORT_NUMBER : static_cast<INTERNET_PORT>( PHILIPPINES_AUTH_PORT );
		const LPCTSTR pstrUserName	= NULL;
		const LPCTSTR pstrPassword	= NULL;

		pServer		= session.GetHttpConnection( szWebCertURL, nPort, pstrUserName, pstrPassword );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CONNECTION. URL(%s), PORT(%d) ]" ), szWebCertURL, nPort );
			goto DONE;
		}
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, PHILIPPINES_AUTH_WEB_PAGE );
		if( pFile == NULL  )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenRequest Failed!" ) );
			goto DONE;
		}
		if( !( pFile->SendRequest( strHeaders, (LPVOID)pszSendMsg, lstrlen( pszSendMsg ) ) ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SendRequest Failed! -> Msg : %s" ), pszSendMsg ); 	
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );

		FLERROR_LOG( PROGRAM_NAME, _T( "InternetException : %s" ), szError );

		goto DONE;
	}

	char	szResult[4096]	= {0, };
	nReadSize	= (int)pFile->Read( szResult, sizeof( szResult ) );


DONE:
	if( pFile )
	{
		pFile->Close();
		SAFE_DELETE( pFile );
		pFile	= NULL;
	}
	if( pServer )
	{
		pServer->Close();
		SAFE_DELETE( pServer );
		pServer		= NULL;
	}
	session.Close();

	if( nReadSize > 0 )
	{
		//CString strResult = szResult;

		std::string strResult = szResult;
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( strResult, root );

		if ( !parsingSuccessful )
		{
			//std::cout  << "Failed to parse configuration\n"
			//	<< reader.getFormatedErrorMessages();
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, szResult );
			return;
		}
		result.nResult = root.get("error_code","").asInt();
		//std::string encoding = root.get("encoding", "" ).asString();
		FLStrcpy( result.szResult, _countof( result.szResult ), root.get("error_desc", "" ).asCString());
		FLStrcpy( result.szMaster_id, _countof( result.szMaster_id ), root.get("master_id", "" ).asCString());
		result.nAkey_flag = root.get("akey_flag","").asInt();
		FLStrcpy( result.szRef, _countof( result.szRef ), root.get("ref", "" ).asCString());
		
		FLINFO_LOG( PROGRAM_NAME, _T( "Result : [%d %s %s %d %s]" ), result.nResult, result.szResult, result.szMaster_id, result.nAkey_flag, result.szRef );
		// 1. nFirst > 0
		// 2. len <= max
		// 3. if nResult	= 0, szResult = SUCCESS
//		int nFirst	= strResult.Find( "\r\n", 0 );
//		if( nFirst < 0 )
//		{
//			nFirst	= strResult.Find( "\n\r", 0 );
//		}
//
////		VALID_CRLF( nFirst, result, strResult )
//		if( ( nFirst ) < 0 )
//		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		result.nResult	= atoi( (LPCSTR)strResult.Left( nFirst ) );
//		nFirst	+= 2;	// \r\n
//
//		int nLast	= strResult.Find( "\r\n", nFirst );
//		if( nLast < 0 )
//		{
//			nLast	= strResult.Find( "\n\r", nFirst );
//		}
//
//		if( nLast < 0 )
//		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		VALID_CRLF( nLast, result, strResult )
//		VALID_STR( nLast - nFirst, 255, result, strResult )
//		FLStrcpy( result.szResult, _countof( result.szResult ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
//
//		if( result.nResult != 0 )
//		{
////			result.nResult	= -1;
////			*result.szResult	= '\0';
////			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		nFirst	= nLast + 2;	// \r\n
//		nLast	= strResult.Find( "\r\n", nFirst );
//		if( nLast < 0 )
//		{
//			nLast	= strResult.Find( "\n\r", nFirst );
//		}
//
//		if( nLast < 0 )
//		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		VALID_CRLF( nLast, result, strResult )
//		VALID_STR( nLast - nFirst, 20, result, strResult )
//		FLStrcpy( result.szMaster_id, _countof( result.szMaster_id ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
//
//
//		nFirst	= nLast + 2;	// \r\n
//		nLast	= strResult.Find( "\r\n", nFirst );
//		if( nLast < 0 )
//		{
//			nLast	= strResult.Find( "\n\r", nFirst );
//		}
//
//		if( nLast < 0 )
//		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		VALID_CRLF( nLast, result, strResult )
//		result.nAkey_flag	= atoi( (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
//
//		//////////////////////////////////////////////////////////////////////////
//		// nickname - 20
//		nFirst	= nLast + 2;	// \r\n
//		nLast	= strResult.Find( "\r\n", nFirst );
//		if( nLast < 0 )
//		{
//			nLast	= strResult.Find( "\n\r", nFirst );
//		}
//
//		if( nLast < 0 )
//		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
//			return;
//		}
//
//		VALID_CRLF( nLast, result, strResult )
//		VALID_STR( nLast - nFirst, 20, result, strResult )
//
//		FLStrcpy( result.szRef, _countof( result.szRef ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
		//////////////////////////////////////////////////////////////////////////

		
	}
	else
	{
		result.nResult	= -1;
		FLStrcpy( result.szResult, _countof( result.szResult ), "Could not establish a connection with the server..." );
	}

	/*switch( result.nResult )
	{
		case 1000:
			{
				FLStrcpy( result.szResult, _countof( result.szResult ), "Unexpected system error occurred." );
			}
			break;
		case 2000:
			{
				FLStrcpy( result.szResult, _countof( result.szResult ), "GPotato is running maintenance from 10 am t0 10 pm." );
			}
			break;
		case 3000:
			{
			FLStrcpy( result.szResult, _countof( result.szResult ), "Your account was blocked by GPOTATO" );
			}
			break;
	}*/


	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100519

//	FLINFO_LOG( PROGRAM_NAME, _T( "Login Fail : %s" ),result.szResult );

	dwEndTick	= ::GetTickCount();

	if( dwEndTick >= dwStartTick + SEC(10) )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "GetGPAuthResult Time Over 10 Sec, Tick[%d], Account[%s], Password[%s]" ), (int)( dwEndTick - dwStartTick ), sAccount, sPassword );
	}
	//	END100519
	//////////////////////////////////////////////////////////////////////////


	return;
}

void	CheckInIPBonus( DWORD dpId, T_PHILIPPINES_AUTH_RESLUT result )
{
	CInternetSession	session( _T( "PhilippinesAuth" ) );

	CHttpConnection*	pServer	= NULL;
	CHttpFile*			pFile	= NULL;

	char	pszSendMsg[700]	= { 0,};
	CString	strHeaders;
	
	int		nReadSize		= 0;

	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( dpId, sAddr, _countof( sAddr ) );

	FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "action=CHECKIN&master_id=%s&ip=%s&ref=%d", result.szMaster_id, sAddr, PHILIPPINES_AUTH_PORT_IPBONUS );

	FLStrcpy( g_szMasterID, _countof( g_szMasterID ), result.szMaster_id );
	
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";

	FLINFO_LOG( PROGRAM_NAME, _T( "CheckInIPBonus : [%s]" ), pszSendMsg );




	try
	{
		pServer	= NULL;

		TCHAR szWebCertURL[128] = {0, };
		FLStrcpy( szWebCertURL, _countof( szWebCertURL ), PHILIPPINES_AUTH_WEB_URL );

		const INTERNET_PORT nPort	= PHILIPPINES_AUTH_PORT_IPBONUS <= 0 ? INTERNET_INVALID_PORT_NUMBER : static_cast<INTERNET_PORT>( PHILIPPINES_AUTH_PORT_IPBONUS );
		const LPCTSTR pstrUserName	= NULL;
		const LPCTSTR pstrPassword	= NULL;

		pServer		= session.GetHttpConnection( szWebCertURL, nPort, pstrUserName, pstrPassword );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CONNECTION. URL(%s), PORT(%d) ]" ), szWebCertURL, nPort );
			goto DONE;
		}
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, PHILIPPINES_AUTH_WEB_PAGE );
		if( pFile == NULL  )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenRequest Failed!" ) );
			goto DONE;
		}
		if( !( pFile->SendRequest( strHeaders, (LPVOID)pszSendMsg, lstrlen( pszSendMsg ) ) ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SendRequest Failed! -> Msg : %s" ), pszSendMsg ); 	
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );

		FLERROR_LOG( PROGRAM_NAME, _T( "InternetException : %s" ), szError );

		goto DONE;
	}

	char	szResult[4096]	= {0, };
	nReadSize	= (int)pFile->Read( szResult, sizeof( szResult ) );


DONE:
	if( pFile )
	{
		pFile->Close();
		SAFE_DELETE( pFile );
		pFile	= NULL;
	}
	if( pServer )
	{
		pServer->Close();
		SAFE_DELETE( pServer );
		pServer		= NULL;
	}
	session.Close();

	T_PHILIPPINES_AUTH_IPBONUS_CHECKIN_RESULT ipbonusresult;

	if( nReadSize > 0 )
	{
		std::string strResult = szResult;
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( strResult, root );

		if ( !parsingSuccessful )
		{
			//std::cout  << "Failed to parse configuration\n"
			//	<< reader.getFormatedErrorMessages();
			FLERROR_LOG( PROGRAM_NAME, szResult );
			return;
		}
		ipbonusresult.nBonus_Level = root.get("bonus_level","").asInt();

		FLStrcpy( ipbonusresult.szMaster_id, _countof( ipbonusresult.szMaster_id ), root.get("master_id", "" ).asCString());
		FLStrcpy( ipbonusresult.szRef, _countof( ipbonusresult.szRef ), root.get("ref", "" ).asCString());
		
		FLINFO_LOG( PROGRAM_NAME, _T( "Result : [%d %s %s]" ), ipbonusresult.nBonus_Level, ipbonusresult.szMaster_id, ipbonusresult.szRef );

		
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Could not establish a connection with the server..." ) );
	}


	return;
}

void				CheckOutIPBonus()
{
	CInternetSession	session( _T( "PhilippinesAuth" ) );

	CHttpConnection*	pServer	= NULL;
	CHttpFile*			pFile	= NULL;

	char	pszSendMsg[700]	= { 0,};
	CString	strHeaders;
	
	int		nReadSize		= 0;
	if ( strcmp( g_szMasterID , "" ) == 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Could not establish a connection with the server..." ) );
		return;
	}

	FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "action=CHECKOUT&master_id=%s&ref=%d", g_szMasterID, PHILIPPINES_AUTH_PORT_IPBONUS );
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";

	FLINFO_LOG( PROGRAM_NAME, _T( "CheckInIPBonus : [%s]" ), pszSendMsg );




	try
	{
		pServer	= NULL;

		TCHAR szWebCertURL[128] = {0, };
		FLStrcpy( szWebCertURL, _countof( szWebCertURL ), PHILIPPINES_AUTH_WEB_URL );

		const INTERNET_PORT nPort	= PHILIPPINES_AUTH_PORT_IPBONUS <= 0 ? INTERNET_INVALID_PORT_NUMBER : static_cast<INTERNET_PORT>( PHILIPPINES_AUTH_PORT_IPBONUS );
		const LPCTSTR pstrUserName	= NULL;
		const LPCTSTR pstrPassword	= NULL;

		pServer		= session.GetHttpConnection( szWebCertURL, nPort, pstrUserName, pstrPassword );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CONNECTION. URL(%s), PORT(%d) ]" ), szWebCertURL, nPort );
			goto DONE;
		}
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, PHILIPPINES_AUTH_WEB_PAGE );
		if( pFile == NULL  )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenRequest Failed!" ) );
			goto DONE;
		}
		if( !( pFile->SendRequest( strHeaders, (LPVOID)pszSendMsg, lstrlen( pszSendMsg ) ) ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SendRequest Failed! -> Msg : %s" ), pszSendMsg ); 	
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );

		FLERROR_LOG( PROGRAM_NAME, _T( "InternetException : %s" ), szError );

		goto DONE;
	}

	char	szResult[4096]	= {0, };
	nReadSize	= (int)pFile->Read( szResult, sizeof( szResult ) );


DONE:
	if( pFile )
	{
		pFile->Close();
		SAFE_DELETE( pFile );
		pFile	= NULL;
	}
	if( pServer )
	{
		pServer->Close();
		SAFE_DELETE( pServer );
		pServer		= NULL;
	}
	session.Close();

	T_PHILIPPINES_AUTH_IPBONUS_CHECKOUT_RESULT ipbonusresult;

	if( nReadSize > 0 )
	{
		std::string strResult = szResult;
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( strResult, root );

		if ( !parsingSuccessful )
		{
			//std::cout  << "Failed to parse configuration\n"
			//	<< reader.getFormatedErrorMessages();
			FLERROR_LOG( PROGRAM_NAME, szResult );
			return;
		}
		ipbonusresult.nError_Code = root.get("error_code","").asInt();

		FLStrcpy( ipbonusresult.szMaster_id, _countof( ipbonusresult.szMaster_id ), root.get("master_id", "" ).asCString());
		FLStrcpy( ipbonusresult.szRef, _countof( ipbonusresult.szRef ), root.get("ref", "" ).asCString());
		
		FLINFO_LOG( PROGRAM_NAME, _T( "Result : [%d %s %s]" ), ipbonusresult.nError_Code, ipbonusresult.szMaster_id, ipbonusresult.szRef );

		
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Could not establish a connection with the server..." ) );
	}


	return;
}