#include "stdafx.h"
#include "dbcontroller.h"
#include <post.h>
#include "dptrans.h"

CDbController::CDbController()
:
m_pQuery( 0 ),
m_hIocp( 0 ),
m_hDbHndlr( 0 ),
m_dwTimer( 0 )
{
}

CDbController::~CDbController()
{
}

BOOL CDbController::CreateDbHandler( DWORD dwMilliseconds )
{
	m_hIocp		= ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if( !m_hIocp )
		return FALSE;
	DWORD dwThreadId;
	m_hDbHndlr	= chBEGINTHREADEX( NULL, 0, _DbHandler, (LPVOID)this, 0, &dwThreadId );
	if( !m_hDbHndlr )
	{
		::CloseHandle( m_hIocp );
		return FALSE;
	}

	SetTimer( dwMilliseconds );

	return TRUE;
}

void CDbController::SetTimer( DWORD dwMilliseconds )
{
	if( dwMilliseconds != INFINITE )
	{
		m_dwTimer	= dwMilliseconds;
		CDbControllerTimer::GetInstance()->Register( this );
	}
}

void CDbController::CloseDbHandler( void )
{
	if( m_hIocp )
	{
		::PostQueuedCompletionStatus( m_hIocp, 0, NULL, NULL );
		::WaitForSingleObject( m_hDbHndlr, INFINITE );
		CLOSE_HANDLE( m_hIocp );
		CLOSE_HANDLE( m_hDbHndlr );
	}
}

BOOL CDbController::PostRequest( int nQuery, BYTE* lpBuf, u_long nBufSize, ULONG_PTR dwCompletionKey )
{
	LPDB_OVERLAPPED_PLUS pov	= new DB_OVERLAPPED_PLUS;
//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	if( lpBuf != NULL )
	{
		if( nBufSize < 0 || nBufSize > (1024*1024) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "nBufSize error. nQuery[%d], nBufSize[%d]" ), nQuery, nBufSize );

			return	FALSE;
		}
		else
		{
			pov->lpBuf	= new BYTE[nBufSize];
			memset( pov->lpBuf, 0, sizeof( BYTE ) * nBufSize );

			::memcpy( pov->lpBuf, lpBuf, nBufSize );
			pov->uBufSize	= nBufSize;
		}
	}
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.

	pov->nQueryMode		= nQuery;
	return ::PostQueuedCompletionStatus( m_hIocp, 1, dwCompletionKey, &pov->Overlapped );
}

u_long	CDbController::_DbHandler( LPVOID* pParam )
{
	CDbController* pController	= reinterpret_cast<CDbController*>( pParam );
	pController->DbHandler();
	return 0;
}

CQuery* CDbController::CreateQuery( void )
{
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	const char* pass = CDbManager::GetInstance().DB_ADMIN_PASS_CHARACTER01;
	if( !pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, pass ) )
#endif
	{
		::AfxMessageBox( "Can't connect db: CDbController.CreateQuery" );
		SAFE_DELETE( pQuery );
	}
	return pQuery;
}

void CDbController::DbHandler( void )
{
	CQuery* pQuery	= CreateQuery();
	if( pQuery == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CreateQuery Failed" ) );
		return;
	}

	SetQueryObject( pQuery );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	ULONG_PTR dwCompletionKey	= 0;
	LPDB_OVERLAPPED_PLUS pov	= NULL;

	while( true )
	{
		bReturnValue
			= ::GetQueuedCompletionStatus( m_hIocp,
										&dwBytesTransferred,
										&dwCompletionKey,
										(LPOVERLAPPED*)&pov,
										INFINITE 
									);
		
		if( FALSE == bReturnValue )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			continue;
		}

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			return;
		}

		if( pov->nQueryMode == eTimer )
		{
			OnTimer();
		}
		
		Handler( pov, dwCompletionKey );

		//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		SAFE_DELETE_ARRAY( pov->lpBuf );
		//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		SAFE_DELETE( pov );
	}
}

////////////////////////////////////////////////////////////////////////////////
CDbControllerTimer::CDbControllerTimer()
:
m_hTickThread( 0 ),
m_hCloseTick( 0 )
{
}

CDbControllerTimer::~CDbControllerTimer()
{
}

void CDbControllerTimer::Register( CDbController* dbCtrl )
{
	CMclAutoLock Lock( m_csLock );
	m_vDbController.push_back(std::make_pair( dbCtrl, ::GetTickCount() + dbCtrl->GetTimer() ) );
}

u_int CDbControllerTimer::_Tick( LPVOID pParam )
{
	CDbControllerTimer* pTimer	= reinterpret_cast<CDbControllerTimer*>( pParam );
	pTimer->Tick();
	return 0;
}

void CDbControllerTimer::Tick( void )
{
	while( WaitForSingleObject( m_hCloseTick, SEC( 1 ) ) != WAIT_OBJECT_0 )
	{
		DWORD dwTick	= GetTickCount();
		CMclAutoLock Lock( m_csLock );
		for( VPDD::iterator i = m_vDbController.begin(); i != m_vDbController.end(); ++i )
		{
			if( dwTick > ( *i ).second )
			{
				( *i ).first->PostRequest( CDbController::eTimer, NULL, 0, 0 );
				( *i ).second	+= ( *i ).first->GetTimer();
			}
		}
	}
}

void CDbControllerTimer::Create( void )
{
	DWORD dwThreadId	= 0;
	m_hCloseTick	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hTickThread	= chBEGINTHREADEX( NULL, 0, _Tick, (LPVOID)this, 0, &dwThreadId );
	FLASSERT( m_hCloseTick );
	FLASSERT( m_hTickThread );
}

void CDbControllerTimer::Destroy( void )
{
	CLOSE_THREAD( m_hTickThread, m_hCloseTick );
}

CDbControllerTimer* CDbControllerTimer::GetInstance( void )
{
	static CDbControllerTimer sDbControllerTimer;
	return &sDbControllerTimer;
}
////////////////////////////////////////////////////////////////////////////////
// namespace	post	{
// 	// implementation
// 	void PostItem( CQuery* pQuery, const u_long idReceiver, const DWORD dwItemId, const int nItemNum, const char * szTitle, const char * szText )
// 	{
// 		CMail* pMail			= new CMail;
// 		pMail->m_tmCreate		= ::time_null();
// 		pMail->m_idSender		= 0;
// 		pMail->m_szTitle[ 0 ]	= '\0';
// 		pMail->m_szText[ 0 ]	= '\0';
// 
// 		if( szTitle != NULL )
// 			FLSPrintf( pMail->m_szTitle, _countof( pMail->m_szTitle ), szTitle );
// 		if( szText != NULL )
// 			FLSPrintf( pMail->m_szText, _countof( pMail->m_szText ), szText );
// 
// 		pMail->m_pItemElem	= NULL;
// 		if( dwItemId != NULL_ID && nItemNum > 0 )
// 		{
// 			pMail->m_pItemElem	= new FLItemElem;
// 			pMail->m_pItemElem->m_dwItemId	= dwItemId;
// 			pMail->m_pItemElem->m_nItemNum	= nItemNum;
// 			pMail->m_pItemElem->m_dwKeepTime=	0;
// 			pMail->m_pItemElem->SetSerialNumber();
// 		}
// 
// 		Post( idReceiver, pMail, pQuery );
// 	}
// 
// 
// 
// 	void Post( u_long idReceiver, CMail* pMail, CQuery* pQuery )
// 	{
// 		CPost* pPost	= CPost::GetInstance();
// 		pPost->m_csPost.Enter();
// 		BOOL bResult	= pPost->AddMail( idReceiver, pMail ) > 0;
// 		pPost->m_csPost.Leave();
// 		if( bResult )
// 		{
// 			//////////////////////////////////////////////////////////////////////////
// 			// BEGIN110119
// 
// 			CDbManager::T_MAIL_STR_INFO tMail;
// 			mem_set( &tMail, 0, sizeof( tMail ) );
// 
// 			FLStrcpy( tMail.szGu, _countof( tMail.szGu ), "A1" );
// 			FLStrcpy( tMail.szMailTitle, _countof( tMail.szMailTitle ), pMail->m_szTitle );
// 			FLStrcpy( tMail.szMailText, _countof( tMail.szMailText ), pMail->m_szText );
// 			tMail.dwReceivePlayerId = idReceiver;
// 			tMail = pMail;
// 
// 			if( CDbManager::GetInstance().DbQryMail( pQuery, &tMail ) == true )
// 			{
// 				CDPTrans::GetInstance()->SendPostMail( TRUE, idReceiver, pMail );
// 			}
// 			else
// 			{
// 				pPost->m_csPost.Enter();
// 				CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
// 				FLASSERT( pMailBox );
// 				if( pMailBox )
// 				{
// 					pMailBox->RemoveMail( pMail->m_nMail );
// 				}
// 				pPost->m_csPost.Leave();
// 			}
// 
// 			// END110119
// 			//////////////////////////////////////////////////////////////////////////
// 		}
// 	}
// 
// 	void QueryCooperativeContributions_RewarItemByMail( const u_long idReceiver, const DWORD dwContributionID, const FLTm tExpire, CMail* pMail, CQuery* pQuery )
// 	{
// 		char szQuery[ MAX_QUERY_SIZE ]	= { 0 };
// 		CDbManager::MakeQuery_CooperativeContributionsRewardItemMail( idReceiver, dwContributionID, tExpire, szQuery, _countof( szQuery ), pMail, idReceiver );
// 		SQLINTEGER cbLen	= SQL_NTS;
// 		BOOL bOk	= pQuery->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 128, 0, (void*)pMail->m_szTitle, 0, &cbLen )
// 			&& pQuery->BindParameter( 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1024, 0, (void*)pMail->m_szText, 0, &cbLen );
// 		if( bOk )
// 			bOk		= pQuery->Exec( szQuery );
// 		if( bOk )
// 		{
// 			if( pQuery->Fetch() == TRUE && pQuery->GetInt( "fError" ) == 0 )		//성공
// 			{
// 				CPost* pPost		= CPost::GetInstance();
// 				pPost->m_csPost.Enter();
// 				const BOOL bResult	= pPost->AddMail( idReceiver, pMail ) > 0;
// 				pPost->m_csPost.Leave();
// 				if( bResult == TRUE )
// 					CDPTrans::GetInstance()->SendPostMail( TRUE, idReceiver, pMail );
// 			}
// 		}
// 		else
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "BindParameter() or Exec(): PostDeposit() [%s]" ), szQuery );
// 		}
// 	}
// 
// 	void	ExecCooperativeContributions_RewarItemByMail( CQuery & Query, const u_long idReceiver, const DWORD dwContributionID, const FLTm tExpire
// 		, const DWORD dwItemId, const int nItemNum, const char * szTitle, const char * szText )
// 	{
// 		if( dwItemId == NULL_ID || nItemNum <= 0 )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ARGUMENT : ExecCooperativeContributions_RewarItemByMail ]" ) );
// 			return;
// 		}
// 
// 		CMail* pMail		= NULL;
// 		try
// 		{
// 			pMail		= new CMail;
// 			pMail->m_tmCreate	= ::time_null();
// 			pMail->m_idSender	= 0;
// 			FLSPrintf( pMail->m_szTitle, _countof( pMail->m_szTitle ), szTitle );
// 			FLSPrintf( pMail->m_szText, _countof( pMail->m_szText ), szText );
// 
// 			pMail->m_pItemElem	= new FLItemElem;
// 			pMail->m_pItemElem->m_dwItemId	= dwItemId;
// 			pMail->m_pItemElem->m_nItemNum	= nItemNum;
// 			pMail->m_pItemElem->m_dwKeepTime=	0;
// 			pMail->m_pItemElem->SetSerialNumber();
// 
// 			post::QueryCooperativeContributions_RewarItemByMail( idReceiver, dwContributionID, tExpire, pMail, &Query );
// 		}
// 		catch( ... )
// 		{
// 			SAFE_DELETE( pMail );
// 		}
// 	}
// };