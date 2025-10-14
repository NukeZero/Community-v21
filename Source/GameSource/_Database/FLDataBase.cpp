#include "stdafx.h"

#include "FLDataBase.h"

#include "FLQueryCommand.h"
#include "Query.h"


FLDataBase::FLDataBase() 
: m_parDB( NULL ), m_uDBThreadCount( 0 )
{

}

FLDataBase::~FLDataBase()
{
	Stop();
}

#ifdef __DbStuff
bool FLDataBase::Start(const size_t uDBThreadCount, CQuery::SqlDatabase dbType)
{
	if (m_parDB)
	{
		FLERROR_LOG(PROGRAM_NAME, _T("[ ALREADY START ]"));
		return false;
	}

	m_uDBThreadCount = uDBThreadCount;
	m_parDB = new FLDBChild[m_uDBThreadCount];
	for (size_t i = 0; i < m_uDBThreadCount; ++i)
	{
		if (!m_parDB[i].Start(dbType))
		{
			FLERROR_LOG(PROGRAM_NAME, _T("[ DB connect FAIL ]"));
			SAFE_DELETE_ARRAY(m_parDB);
			return false;
		}
	}
	return true;
}

#else
BOOL FLDataBase::Start( const size_t uDBThreadCount, const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW )
{
	if( m_parDB != NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ALREADY START ]" ) );
		return FALSE;
	}

	m_uDBThreadCount	= uDBThreadCount;
	m_parDB				= new FLDBChild[ m_uDBThreadCount ];
	for( size_t i = 0; i < m_uDBThreadCount; ++i )
	{
		if( m_parDB[ i ].Start( szDSNName, szID, szPW ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DB connect FAIL ]" ) );
			SAFE_DELETE_ARRAY( m_parDB );
			return FALSE;
		}	
	}

	return TRUE;
}
#endif

void FLDataBase::Stop()
{
	SAFE_DELETE_ARRAY( m_parDB );
}

void FLDataBase::PostQueryCommand( const unsigned long uKey, FLQueryCommand * pCommand )
{
	if( pCommand == NULL )
		return;

	if( m_parDB == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ DB_MANAGER NOT START ]" ) );
		SAFE_DELETE( pCommand );
		return;
	}

	m_parDB[ uKey % m_uDBThreadCount ].PostQueryCommand( pCommand );
}

CQuery * FLDataBase::GetQuery( const unsigned long uKey )
{
	return ( m_parDB == NULL ) ? NULL : &m_parDB[ uKey % m_uDBThreadCount ].m_Query;
}



//-----------------------------------------------------------------------------------------------------------------------------//

FLDBChild::FLDBChild()
: m_hThread( INVALID_HANDLE_VALUE )
, m_hIOCP( INVALID_HANDLE_VALUE )
{
}

FLDBChild::~FLDBChild()
{
	Stop();
}

#ifdef __DbStuff
bool FLDBChild::Start(CQuery::SqlDatabase dbType)
{
	if (m_hIOCP != INVALID_HANDLE_VALUE)
		return false;

	if (!m_Query.Connect(dbType))
	{
		FLERROR_LOG(PROGRAM_NAME, _T("[ FAIL - DB CONNECT : dbType %d) ]"), dbType);
		return false;
	}
	
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		FLERROR_LOG(PROGRAM_NAME, "[ FAIL - CreateIoCompletionPort : GetLastError(%d) ]", ::GetLastError());
		return false;
	}

	UINT uThreadId = 0;
	m_hThread = (HANDLE)::_beginthreadex(nullptr, 0, ThreadProc, (LPVOID)this, 0, &uThreadId);
	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		FLERROR_LOG(PROGRAM_NAME, "[ FAIL - Create Thread : GetLastError(%d) ]", ::GetLastError());
		return false;
	}
	return true;
}

#else
BOOL FLDBChild::Start( const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW )
{
	if( m_hIOCP != INVALID_HANDLE_VALUE )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ FAIL - duplication (%s) ]", szDSNName );
		return FALSE;
	}

	if( this->connect( szDSNName, szID, szPW ) == FALSE )
		return FALSE;

	m_hIOCP					= ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if( m_hIOCP == INVALID_HANDLE_VALUE )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ FAIL - CreateIoCompletionPort : GetLastError(%d) ]", ::GetLastError() );
		return FALSE;
	}

	UINT uThreadId			= 0;
	m_hThread				= ( HANDLE )::_beginthreadex( NULL, 0, ThreadProc, (LPVOID)this, 0, &uThreadId );
	if( m_hThread == INVALID_HANDLE_VALUE )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ FAIL - Create Thread : GetLastError(%d) ]", ::GetLastError() );
		return FALSE;
	}

	return TRUE;
}
#endif

void FLDBChild::Stop()
{
	if( m_hIOCP != INVALID_HANDLE_VALUE )
	{
		::PostQueuedCompletionStatus( m_hIOCP, 0, NULL, NULL );
		::WaitForSingleObject( m_hThread, INFINITE );

		::CloseHandle( m_hIOCP );
		::CloseHandle( m_hThread );

		m_hIOCP				= INVALID_HANDLE_VALUE;
		m_hThread			= INVALID_HANDLE_VALUE;
	}

	return;
}


void FLDBChild::PostQueryCommand( FLQueryCommand * pCommand )
{
	if( pCommand == NULL )
		return;

	if( ::PostQueuedCompletionStatus( m_hIOCP, DB_QUERY, NULL, reinterpret_cast< LPOVERLAPPED >( pCommand ) ) == FALSE )		
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ DATABASE INSTANCE NOT START OR SYSTEM ERROR : GetLastError(%d) ] " ), ::GetLastError() );
		delete pCommand;
		Stop();			// critical한 상황이므로 쓰레드를 죽인다.
	}
}

#ifndef __DbStuff
BOOL FLDBChild::connect( const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW )
{
	FLStrcpy( m_szDSNName, _countof( m_szDSNName ), szDSNName );
	FLStrcpy( m_szID, _countof( m_szID ), szID );
	FLStrcpy( m_szPW, _countof( m_szPW ), szPW );

	if( m_Query.Connect( 3, m_szDSNName, m_szID, m_szPW ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - DB CONNECT : DNS(%s) ]" ), szDSNName );
		return FALSE;
	}

	return TRUE;
}
#endif


unsigned WINAPI FLDBChild::ThreadProc( void * pVoid )
{
	FLDBChild * pDB			= reinterpret_cast< FLDBChild * >( pVoid );
	if( pDB == NULL )
		return 0;

	for( ;; )
	{
		DWORD dwBytesTransferred	= 0;
		ULONG_PTR pCompletionKey	= 0;
		FLQueryCommand * pQueryWrapper = NULL;

		const BOOL bRet	= ::GetQueuedCompletionStatus( pDB->m_hIOCP, &dwBytesTransferred, &pCompletionKey, reinterpret_cast< OVERLAPPED ** >( &pQueryWrapper ), INFINITE );
		if( bRet == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			continue;
		}

		if( dwBytesTransferred == DB_EXIT ) //종료
			break;
			//return 0;

		if( pQueryWrapper == NULL )
			continue;

		pQueryWrapper->Execute( pDB->m_Query );	//connect fail일 경우. reconnect 시도

		delete pQueryWrapper;
	}

	return 0;
}


