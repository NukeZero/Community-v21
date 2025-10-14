#include "stdafx.h"
#include "dbmanager.h"

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


void	IndonesiaAuthTimeout( void* pParam, AUTH_INFO* pAuthInfo )
{
	FLIDNAuthClient* pIDNAuthClient = reinterpret_cast< FLIDNAuthClient* >( pParam );
	if( pIDNAuthClient != NULL )
	{
		//pIDNAuthClient->SendLogoutNTY( pAuthInfo->AccountInfo.szAccount );
		FLERROR_LOG( PROGRAM_NAME, _T( "Indonesia Auth Login Timeout: %s" ), pAuthInfo->AccountInfo.szAccount );
	}
}

u_int __stdcall		IndonesiaAuthWorker( LPVOID pParam )
{
	CQuery query;

	FLIDNAuthClient cIDNAuth( reinterpret_cast< BYTE >( pParam )+1 );
	FLAuthWait cAuthWait;

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
		//////////////////////////////////////////////////////////////////////////
		// 인증 패킷 처리
		cIDNAuth.Process();
		// 로그인 타임아웃 처리
		cAuthWait.TimeoutProcess( IndonesiaAuthTimeout, &cIDNAuth );

		// 
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
			&dwBytesTransferred,
			(LPDWORD)&lpCompletionKey,
			(LPOVERLAPPED*)&pov,
			1 );

		if( fOk == FALSE ) 
		{
			if( ::GetLastError() == WAIT_TIMEOUT )
			{
				continue;
			}

			FLASSERT( 0 );				// fatal error
			continue;
		}

		if( pov == NULL )	// terminate
			return( 0 );

		switch( pov->nQueryMode )
		{
		case CERTIFY:
			g_DbManager.Certify_Indonesia( pov, cIDNAuth, cAuthWait );
			break;
		case CLOSE_EXISTING_CONNECTION:
			g_DbManager.CloseExistingConnection_Indonesia( query, pov );
			break;
		case CERTIFY_ACK:
			g_DbManager.CertifyAck_Indonesia( static_cast< BYTE >( dwBytesTransferred ), query, pov, cIDNAuth, cAuthWait );
			break;
		case ACCOUNT_LOGOUT:
			g_DbManager.AccountLogout_Indonesia( pov, cIDNAuth );
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

	g_dpCertifier.DeleteDPObject();
	return( 0 );
}

void	CDbManager::Certify_Indonesia( LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth, FLAuthWait& cAuthWait )
{
	if( cAuthWait.Register( &pov->AuthInfo ) == true )
	{
		cIDNAuth.SendLoginREQ( pov->AuthInfo.AccountInfo.szAccount, pov->AuthInfo.AccountInfo.szPassword, pov->AuthInfo.dwIP );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "인증 대기 객체에 추가하지 못했습니다. Account: %s" ), pov->AuthInfo.AccountInfo.szAccount );
	}
}

void	CDbManager::CertifyAck_Indonesia( BYTE byResult, CQuery & query, LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth, FLAuthWait& cAuthWait )
{
	if( cAuthWait.Unregister( pov->AuthInfo.AccountInfo.szAccount, &pov->AuthInfo ) == false )
	{
		// 타임아웃되었다.
		if( byResult == IDN_AUTH_ERROR_OK )
		{
			cIDNAuth.SendLogoutNTY( pov->AuthInfo.AccountInfo.szAccount );
		}
	}
	else
	{
		LONG lError = ERROR_BILLING_DATABASE_ERROR;
		switch( byResult )
		{
		case IDN_AUTH_ERROR_OK:
			FLStrcpy( pov->AuthInfo.AccountInfo.szBak, _countof( pov->AuthInfo.AccountInfo.szBak ), pov->AuthInfo.AccountInfo.szAccount );
			if( OnCertifyQueryOK( query, pov ) == false )
			{
				cIDNAuth.SendLogoutNTY( pov->AuthInfo.AccountInfo.szAccount );
				break;
			}

			lError = ERROR_OK;
			break;
		case IDN_AUTH_ERROR_DUPLICATED_LOGIN:	// 다른 게임에 로그인 되어 있습니다. 5분후 다시 접속요망
			lError = ERROR_INDONESIA_AUTH_DUPLICATED_LOGIN;
			break;
		case IDN_AUTH_ERROR_ID_PASSWORD_WRONG:	// ID 혹은 Password가 잘못되었습니다.
			lError = ERROR_INDONESIA_AUTH_ID_PASSWORD_WRONG;
			break;
		}

		if( lError != ERROR_OK )
		{
			g_dpCertifier.SendError( lError, pov->AuthInfo.dpId );
		}
	}
}

void	CDbManager::CloseExistingConnection_Indonesia( CQuery & /*query*/, LPDB_OVERLAPPED_PLUS pov )
{
	// Flyff 자체 중복 로그인으로 인한 접속 종료.
	// 인도네시아 인증 서버에서 온 중복 로그인 접속 종료.
	g_dpAccountClient.SendCloseExistingConnection( pov->AuthInfo.AccountInfo.szAccount );
}

void	CDbManager::AccountLogout_Indonesia( LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth )
{
	cIDNAuth.SendLogoutNTY( pov->AuthInfo.AccountInfo.szAccount );
}
