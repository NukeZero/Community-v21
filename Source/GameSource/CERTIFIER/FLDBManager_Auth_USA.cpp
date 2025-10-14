#include "stdafx.h"
#include "dbmanager.h"

#include <afxinet.h>

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include <..\Resource\Lang.h>
#include "User.h"

extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;

extern	CDbManager	g_DbManager;
extern	HANDLE		s_hHandle;

/////////////////////////////////////////////////////////////////////////////////////////////////
// global
char	USA_AUTH_WEB_URL[64]	= "";
char	USA_AUTH_WEB_PAGE[64]	= "";
int		USA_AUTH_PORT			= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////

u_int __stdcall USAAuthWorker( LPVOID pParam )
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

		switch( pov->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify_USA( query, pov, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection_USA( query, pov );
				break;
			case ACCOUNT_LOGOUT:
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

void CDbManager::Certify_USA( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr )
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

	T_USA_AUTH_RESLUT	tAuthResult = { 0 };

	GetUSAAuthResult( pov, tAuthResult );
	FLINFO_LOG( PROGRAM_NAME, _T( "%d, %s, %s, %d" ), tAuthResult.nResult, tAuthResult.szResult, tAuthResult.szGPotatoID, tAuthResult.nGPotatoNo );

	if( tAuthResult.nResult== 0 )
	{
		if( pov->AuthInfo.dwIP )
			mgr.ResetErrorCount();

		FLStrcpy( pov->AuthInfo.AccountInfo.szBak, _countof( pov->AuthInfo.AccountInfo.szBak ), pov->AuthInfo.AccountInfo.szAccount );
		FLStrcpy( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), tAuthResult.szGPotatoID );
		//FLSPrintf( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), "%d", tAuthResult.nGPotatoNo );
		//FLSPrintf( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), "%s %d", tAuthResult.szGPotatoID, tAuthResult.nGPotatoNo );
		CUser* pUser = CUserMng::GetInstance()->GetUser( pov->AuthInfo.dpId );
		if ( pUser )	pUser->nGPotatoNo = tAuthResult.nGPotatoNo;
		OnCertifyQueryOK( query, pov, tAuthResult.szCheck );

		SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, tAuthResult.bGM );
	}
	else if( tAuthResult.nResult == WEBZEN_ACCOUNT_ID_CONVERTING_FAIL ) {
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
	}
	else
	{
		g_dpCertifier.SendErrorString( tAuthResult.szResult, pov->AuthInfo.dpId );
	}
}

void CDbManager::CloseExistingConnection_USA( CQuery & query, LPDB_OVERLAPPED_PLUS pov )
{
	T_USA_AUTH_RESLUT	tAuthResult = { 0 };
	GetUSAAuthResult( pov, tAuthResult );

	if( tAuthResult.nResult == 0 )
	{
		FLStrcpy( pov->AuthInfo.AccountInfo.szAccount, _countof( pov->AuthInfo.AccountInfo.szAccount ), tAuthResult.szGPotatoID );
		g_dpAccountClient.SendCloseExistingConnection( pov->AuthInfo.AccountInfo.szAccount );

		SQLAddAccount( query, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, tAuthResult.bGM );
	}

	return;
}


void	GetUSAAuthResult( LPDB_OVERLAPPED_PLUS pov, T_USA_AUTH_RESLUT & result )
{
	CInternetSession	session( _T( "USAAuth" ) );

	CHttpConnection*	pServer	= NULL;
	CHttpFile*			pFile	= NULL;

	char	pszSendMsg[700]	= { 0,};
	CString	strHeaders;
	
	int		nReadSize		= 0;

	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->AuthInfo.dpId, sAddr, _countof( sAddr ) );


	const int GAMECD = 4;

	FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "MODE=C&LANGCD=EN&GAMECD=%d&GPOTATOID=%s&PASSWORD=%s&USERIP=%s"
		, GAMECD, pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr );
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";

	//FLINFO_LOG( PROGRAM_NAME, _T( "SendMsg : [%s]" ), pszSendMsg );


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
		FLStrcpy( szWebCertURL, _countof( szWebCertURL ), USA_AUTH_WEB_URL );

		const INTERNET_PORT nPort	= USA_AUTH_PORT <= 0 ? INTERNET_INVALID_PORT_NUMBER : static_cast<INTERNET_PORT>( USA_AUTH_PORT );
		const LPCTSTR pstrUserName	= NULL;
		const LPCTSTR pstrPassword	= NULL;

		pServer		= session.GetHttpConnection( szWebCertURL, nPort, pstrUserName, pstrPassword );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CONNECTION. URL(%s), PORT(%d) ]" ), szWebCertURL, nPort );
			goto DONE;
		}
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, USA_AUTH_WEB_PAGE );
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
		CString strResult = szResult;
		// 1. nFirst > 0
		// 2. len <= max
		// 3. if nResult	= 0, szResult = SUCCESS
		int nFirst	= strResult.Find( "\r\n", 0 );
		if( nFirst < 0 )
		{
			nFirst	= strResult.Find( "\n\r", 0 );
		}

//		VALID_CRLF( nFirst, result, strResult )
		if( ( nFirst ) < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		result.nResult	= atoi( (LPCSTR)strResult.Left( nFirst ) );
		nFirst	+= 2;	// \r\n

		int nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}

		if( nLast < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 255, result, strResult )
		FLStrcpy( result.szResult, _countof( result.szResult ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );

		if( result.nResult != 0 )
		{
//			result.nResult	= -1;
//			*result.szResult	= '\0';
//			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}

		if( nLast < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 20, result, strResult )
		FLStrcpy( result.szGPotatoID, _countof( result.szGPotatoID ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );


		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}

		if( nLast < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		VALID_CRLF( nLast, result, strResult )
		result.nGPotatoNo	= atoi( (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );

		//////////////////////////////////////////////////////////////////////////
		// nickname - 20
		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}

		if( nLast < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 20, result, strResult )

		FLStrcpy( result.szNickName, _countof( result.szNickName ), (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// GM - 1
		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}

		if( nLast < 0 )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			FLERROR_LOG( PROGRAM_NAME, (LPCTSTR)strResult );
			return;
		}

		VALID_CRLF( nLast, result, strResult )

		result.bGM	= ( strResult.Mid( nFirst, nLast - nFirst ) == "Y" ) ? TRUE : FALSE;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// hash data - 50
		nFirst	= nLast + 2;	// \r\n

		VALID_STR( strResult.GetLength() - nFirst, 254, result, strResult )

		FLStrcpy( result.szCheck, _countof( result.szCheck ), (LPCSTR)strResult.Right( strResult.GetLength() - nFirst ) );
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		result.nResult	= -1;
		FLStrcpy( result.szResult, _countof( result.szResult ), "Could not establish a connection with the server..." );
	}

	switch( result.nResult )
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
	}


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
