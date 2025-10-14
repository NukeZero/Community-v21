
#include "FLLogContext.h"
#include "../thread/FLAutoLock.h"


namespace
{
	unsigned int	__stdcall	FlushThread( void* pValue )
	{
		FLLogContext* pContext = reinterpret_cast< FLLogContext* >( pValue );
		pContext->Run();
		return 0;
	}
}


FLLogContext::FLLogContext()
{
	m_hThread = NULL;
	m_bIsStop = true;

	m_dwInterval = 0;
}

FLLogContext::~FLLogContext()
{
	Stop();
	Clear();
}

void	FLLogContext::Clear()
{
	CMclAutoLock Lock( m_csFlushs );
	m_listFlushs.clear();
}

bool	FLLogContext::Start( DWORD dwInterval )
{
	m_bIsStop = false;
	m_dwInterval = dwInterval;

	DWORD dwThreadID = 0;
	m_hThread = chBEGINTHREADEX( NULL, 0, FlushThread, this, 0, &dwThreadID );
	if( m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE )
	{
		m_bIsStop = true;
		return false;
	}

	return true;
}

void	FLLogContext::Stop( DWORD dwTimeout )
{
	if( m_hThread != NULL && m_hThread != INVALID_HANDLE_VALUE )
	{
		m_bIsStop = true;

		if( ::WaitForSingleObject( m_hThread, dwTimeout ) != WAIT_OBJECT_0 )
		{
			::TerminateThread( m_hThread, 1 );
		}

		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}
}

void	FLLogContext::Attach( FLLogFlush* pFlush )
{
	if( pFlush != NULL )
	{
		CMclAutoLock Lock( m_csFlushs );
		m_listFlushs.push_back( pFlush );
	}
}

void	FLLogContext::Detach( FLLogFlush* pFlush )
{
	CMclAutoLock Lock( m_csFlushs );

	LOGFLUSH_ITER pos = m_listFlushs.begin();
	LOGFLUSH_ITER end = m_listFlushs.end();

	for( ; pos != end; ++pos )
	{
		FLLogFlush* pTemp = (*pos);
		if( pTemp == pFlush )
		{
			m_listFlushs.erase( pos );
			break;
		}
	}
}

void	FLLogContext::Run()
{
	while( m_bIsStop == false )
	{
		RunFlush();
		::Sleep( m_dwInterval );
	}
	RunFlush();

	::TerminateThread( m_hThread, 1 );
}

void	FLLogContext::Flush()
{
	RunFlush();
}

void	FLLogContext::RunFlush()
{
	CMclAutoLock Lock( m_csFlushs );

	LOGFLUSH_CITER pos = m_listFlushs.begin();
	LOGFLUSH_CITER end = m_listFlushs.end();

	for( ; pos != end; ++pos )
	{
		FLLogFlush* pFlush = (*pos);
		if( pFlush != NULL )
		{
			pFlush->Flush();
		}
	}

	
}
