#include "stdafx.h"
#include "dbmanager.h"

#include <afxinet.h>

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include <..\Resource\Lang.h>

#include "FLIDNAuthClient.h"
#include "FLAuthWait.h"

extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;

extern	CDbManager	g_DbManager;
extern	HANDLE		s_hHandle;


/////////////////////////////////////////////////////////////////////////////////////////////////
// global
std::string		g_strWebCertURL_NotGPotato;	//	일본 웹 인증 관련
std::string		g_strWebCertURL_GPotato;	//	일본 웹 인증 관련
/////////////////////////////////////////////////////////////////////////////////////////////////


u_int __stdcall JapanAuthWorker( LPVOID pParam )
{
	CAccountMgr mgr;
	CQuery query;

	if( g_xFlyffConfig->IsConnectAccountDBMode() == true )
	{
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
				g_DbManager.Certify_Japan( query, pov, mgr );
				break;
			
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection_Japan( query, pov );
				break;

			case ACCOUNT_LOGOUT:
				g_DbManager.AccountLogOut_Japan( pov );
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

void CDbManager::Certify_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr )
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
	
	JAPAN_AUTH_RESULT tAuthResult = GetJapanAuthResult( pov );

	if( tAuthResult.nResult == JAPAN_AUTH_RESULT_AGREE )
	{
		if( pov->AuthInfo.dwIP )
			mgr.ResetErrorCount();

		FLStrcpy( pov->AuthInfo.AccountInfo.szBak, _countof( pov->AuthInfo.AccountInfo.szBak ), pov->AuthInfo.AccountInfo.szAccount );
		OnCertifyQueryOK( query, pov );
	}
	else if( tAuthResult.nResult == JAPAN_AUTH_RESULT_NEED_AGREEMENT )
	{
		g_dpCertifier.SendError( ERROR_JAPAN_AUTH_NEED_AGREEMENT, pov->AuthInfo.dpId );
	}
	else if( tAuthResult.nResult == -1 )
	{
		g_dpCertifier.SendError( ERROR_OK, pov->AuthInfo.dpId );
	}
	else
	{
		g_dpCertifier.SendErrorString( tAuthResult.szResultMsg, pov->AuthInfo.dpId );
		g_dpCertifier.SendError( ERROR_OK, pov->AuthInfo.dpId );
	}
}

void CDbManager::CloseExistingConnection_Japan( CQuery & /*query*/, LPDB_OVERLAPPED_PLUS pov )
{
	JAPAN_AUTH_RESULT r = GetJapanAuthResult( pov );

	if( r.nResult == JAPAN_AUTH_RESULT_AGREE )
	{
		g_dpAccountClient.SendCloseExistingConnection( pov->AuthInfo.AccountInfo.szAccount );
	}

	return;
}

void	CDbManager::AccountLogOut_Japan( LPDB_OVERLAPPED_PLUS pov )
{
	const DWORD dwResult	= NotiAccountLogOut_Japan( pov );
//	if( dwResult != 0 )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ ACCOUNT LOG OUT NOTI RESULT(%u) ]" ), dwResult );
	}
}

JAPAN_AUTH_RESULT GetJapanAuthResult( LPDB_OVERLAPPED_PLUS pov )
{
	JAPAN_AUTH_RESULT result;
	
	CInternetSession	session( _T( "Japan_Auth" ) );
	CHttpConnection*	pServer		= NULL;

	CHttpFile*	pFile	= NULL;
	char pszSendMsg[700]	= { 0,};
	
	
	int nReadSize	= 0;
	
	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->AuthInfo.dpId, sAddr, _countof( sAddr ) );
	

	if(	pov->AuthInfo.AccountInfo.nSiteValue == 10 )		// GPotato
	{
		FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "/Account/Authentication.Aspx?MODE=L&GAMECODE=04&GPOTATOID=%s&PASSWORD=%s&USERIP=%s", pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr );
	}
	else if(pov->AuthInfo.AccountInfo.nSiteValue == 11 )	// Flyff
	{
		FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "/flyff/member/flyffAuth.aspx?UID=%s&PWD=%s&SID=flyff&UIP=%s", pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr );
	}
	else	// hangame
	{
		FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "/flyff/member/flyffAuth.aspx?UID=%s&PWD=%s&SID=hangame&UIP=%s", pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr );
	}


	char szHeaders[256] = {0, };
	FLStrcpy( szHeaders, _countof( szHeaders ), "Content-Type: application/x-www-form-urlencoded" );

	try
	{
		pServer = NULL;
		TCHAR szWebCertURL[128] = {0, };
		if( pov->AuthInfo.AccountInfo.nSiteValue == 10 )		// GPotato
		{
			FLStrcpy( szWebCertURL, _countof( szWebCertURL ), g_strWebCertURL_GPotato.c_str() );
		}
		else										// Not GPotato
		{
			FLStrcpy( szWebCertURL, _countof( szWebCertURL ), g_strWebCertURL_NotGPotato.c_str() );
		}

		pServer = session.GetHttpConnection( szWebCertURL );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "\"%s\" Connection Failed!" ), szWebCertURL );
			goto DONE;
		}
		
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, pszSendMsg );

		if( pFile == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenRequest Failed!" ) );
			goto DONE;
		}

		if( pFile->SendRequest() == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SendRequest Failed! -> Msg : %s" ), pszSendMsg );
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );
		goto DONE;
	}

	char szResult[256] = {0, };
	nReadSize	= (int)pFile->Read( szResult, sizeof( szResult ) );

DONE:
	if( pFile != NULL )
	{
		pFile->Close();
		SAFE_DELETE( pFile );
		pFile	= NULL;
	}
	if( pServer != NULL )
	{
		pServer->Close();
		SAFE_DELETE( pServer );
		pServer		= NULL;
	}
	session.Close();

	if( nReadSize > 0 )
	{
		CString strResult = szResult;
		int nFirst	= strResult.Find( "\r\n", 0 );
		if( nFirst < 0 )
		{
			nFirst	= strResult.Find( "\n\r", 0 );
		}
		if( nFirst < 0 )
		{
			result.nResult = -1;
			return result;
		}
		result.nResult	= atoi( (LPCSTR)strResult.Left( nFirst ) );

		nFirst	+= 2;	// \r\n
		int nLast	= strResult.Find( "\n", nFirst );
		if( nLast < 0 )
		{
			nLast	= strResult.Find( "\n\r", nFirst );
		}
		if( nLast < 0 )
		{
			result.nResult = -1;
			return result;
		}
		if( nLast - nFirst > 255 )
		{
			result.nResult = -1;
			return result;
		}

		WCHAR szUnicode[256] = { 0 };
		TCHAR szAnsi[256] = { 0 };

		::MultiByteToWideChar( CP_UTF8, 0, strResult.Mid( nFirst, nLast - nFirst ), -1, szUnicode, _countof( szUnicode ) );
		::WideCharToMultiByte( CP_ACP, 0, szUnicode, -1, szAnsi, sizeof( szAnsi ), NULL, NULL );

		FLStrcpy( result.szResultMsg, _countof( result.szResultMsg ), szAnsi );
	}
	else
	{
		result.nResult	= -1;
		FLStrcpy( result.szResultMsg, _countof( result.szResultMsg ), "Could not establish a connection with the server..." );
	}
	
	return result;
}

DWORD	NotiAccountLogOut_Japan( LPDB_OVERLAPPED_PLUS pov )
{
	
	CInternetSession	session( _T( "Japan_Auth" ) );

	CHttpFile*			pFile		= NULL;
	CHttpConnection*	pServer		= NULL;

	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->AuthInfo.dpId, sAddr, _countof( sAddr ) );

	char pszSendMsg[700]	= { 0, };
	FLSPrintf( pszSendMsg, _countof( pszSendMsg ), "/Account/Authentication.Aspx?MODE=O&GAMECODE=04&GPOTATOID=%s&PASSWORD=%s&USERIP=%s", pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, sAddr );

	char szHeaders[256]		= { 0, };
	FLStrcpy( szHeaders, _countof( szHeaders ), "Content-Type: application/x-www-form-urlencoded" );

	DWORD dwResult			= NULL_ID;
	int nReadSize			= 0;

	try
	{
		TCHAR szWebCertURL[128]			= { 0, };
		FLStrcpy( szWebCertURL, _countof( szWebCertURL ), g_strWebCertURL_GPotato.c_str() );

		pServer							= session.GetHttpConnection( szWebCertURL );

		if( pServer == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "\"%s\" Connection Failed!" ), szWebCertURL );
			goto DONE;
		}

		pFile							= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, pszSendMsg );

		if( pFile == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "OpenRequest Failed!" ) );
			goto DONE;
		}

		if( pFile->SendRequest() == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SendRequest Failed! -> Msg : %s" ), pszSendMsg );
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );
		goto DONE;
	}

	char szResult[256]	= { 0, };
	nReadSize	= (int)pFile->Read( szResult, sizeof( szResult ) );

DONE:
	if( pFile != NULL )
	{
		pFile->Close();
		SAFE_DELETE( pFile );
		pFile	= NULL;
	}
	if( pServer != NULL )
	{
		pServer->Close();
		SAFE_DELETE( pServer );
		pServer		= NULL;
	}
	session.Close();

	if( nReadSize > 0 )
	{
		CString strResult	= szResult;
		int nFirst			= strResult.Find( "\r\n", 0 );
		
		if( nFirst < 0 )
		{
			nFirst			= strResult.Find( "\n\r", 0 );
		}
		
		if( nFirst < 0 )
		{
			return dwResult;
		}

		dwResult		= ::_tstol( (LPCSTR)strResult.Left( nFirst ) );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Could not establish a connection with the server..." ) );
	}

	return dwResult;
}
