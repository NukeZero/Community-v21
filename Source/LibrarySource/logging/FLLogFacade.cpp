
#include "FLLogFacade.h"
#include "FLLogFormatEtc.h"
#include "../common/FLStringFunction.h"
#include "../thread/FLAutoLock.h"

#include "../network/FLNetworkLogAdapter.h"


//////////////////////////////////////////////////////////////////////////
FLLogFacade::FLLog::FLLog()
{
	m_pFactory = NULL;
	m_pUserPublisher = NULL;
	::memset( m_szName, 0, sizeof( m_szName ) );
}

FLLogFacade::FLLog::~FLLog()
{
	Destroy();
}

bool	FLLogFacade::FLLog::Create( FLLogContext* pContext, const TCHAR* szName, FLLogPubFactory* pFactory )
{
	if( pContext == NULL || szName == NULL || pFactory == NULL )
	{
		return false;
	}

	FLStrcpy( m_szName, _countof( m_szName ), szName );

	m_pFactory = pFactory;
	m_pUserPublisher = m_pFactory->Create( szName );
	if( m_pUserPublisher == NULL )
	{
		return false;
	}

	m_cPubComposite.Add( m_pUserPublisher );

#ifndef	_DEBUG
	m_cLevelFilter.SetLevel( FLLOG_INFO );
#endif

	if( m_cModule.Create( &m_cPubComposite, &m_cLevelFilter ) == false )
	{
		return false;
	}

	pContext->Attach( &m_cModule );
	return true;
}

void	FLLogFacade::FLLog::Destroy()
{
	::memset( m_szName, 0, sizeof( m_szName ) );
	m_cModule.Destroy();

	m_cPubComposite.Clear();

	if( m_pFactory != NULL && m_pUserPublisher != NULL )
	{
		m_pFactory->Destroy( m_pUserPublisher );
	}

	m_pFactory = NULL;
	m_pUserPublisher = NULL;
}

void	FLLogFacade::FLLog::AddPublisher( FLLogPublisher* pPublisher )
{
	m_cPubComposite.Add( pPublisher );
}

void	FLLogFacade::FLLog::DeletePublisher( FLLogPublisher* pPublisher )
{
	m_cPubComposite.Delete( pPublisher );
}

void	FLLogFacade::FLLog::Push( FLLogEvent* pEvent )
{
	m_cModule.Push( pEvent );
}

bool	FLLogFacade::FLLog::IsEqual( const TCHAR* szName )	const
{
	if( m_szName == szName || ::_tcsicmp( m_szName, szName ) == 0 )
	{
		return true;
	}

	return false;
}

int		FLLogFacade::FLLog::SetLevel( int nLevel )
{
	return m_cLevelFilter.SetLevel( nLevel );
}

const TCHAR*	FLLogFacade::FLLog::GetName()	const
{
	return m_szName;
}
//////////////////////////////////////////////////////////////////////////


FLLogFacade*	FLLogFacade::GetInstance()
{
	static FLLogFacade cLogFacade;
	return &cLogFacade;
}

FLLogFacade::FLLogFacade()
{
	m_pLastHit = NULL;
	Start();
}

FLLogFacade::~FLLogFacade()
{
	SetNetworkLogName( NULL );
	Stop();

	{
		m_pLastHit = NULL;

		CMclAutoLock Lock( m_csLogs );

		LOG_ITER pos = m_listLogs.begin();
		LOG_ITER end = m_listLogs.end();

		for( ; pos != end; ++pos )
		{
			FLLog* pTemp = (*pos);
			SAFE_DELETE( pTemp );
		}

		m_listLogs.clear();
	}

	FLLogEventPoolDestroy();
}

void	FLLogFacade::Start()
{
	FLVERIFY( m_cContext.Start() );
}

void	FLLogFacade::Stop()
{
	m_cContext.Stop();
}

bool	FLLogFacade::AddNetworkLog( const TCHAR* szName )
{
	if( Add( szName ) == false )
	{
		return false;
	}

	FLLog* pLog = Search( szName );
	if( pLog == NULL )
	{
		return false;
	}

	SetNetworkLogName( pLog->GetName() );
	return true;
}

bool	FLLogFacade::Add( const TCHAR* szName )
{
	FLLog* pLog = Search( szName );
	if( pLog == NULL )
	{
		pLog = new FLLog();
		if( pLog->Create( &m_cContext, szName, &m_cPubFactory ) == false )
		{
			pLog->Destroy();
			SAFE_DELETE( pLog );
			return false;
		}

		CMclAutoLock Lock( m_csLogs );
		m_listLogs.push_back( pLog );
	}

	return true;
}

void	FLLogFacade::AddPublisher( const TCHAR* szName, FLLogPublisher* pPublisher )
{
	FLLog* pLog = SearchAndCreate( szName );
	if( pLog != NULL )
	{
		pLog->AddPublisher( pPublisher );
	}
}

void	FLLogFacade::DeletePublisher( const TCHAR* szName, FLLogPublisher* pPublisher )
{
	FLLog* pLog = SearchAndCreate( szName );
	if( pLog != NULL )
	{
		pLog->DeletePublisher( pPublisher );
	}
}

int		FLLogFacade::ChangeLevel( const TCHAR* szName, int nLevel )
{
	FLLog* pLog = SearchAndCreate( szName );
	if( pLog != NULL )
	{
		return pLog->SetLevel( nLevel );
	}

	return FLLOG_ALL;
}

void	FLLogFacade::Push( const TCHAR* szName, int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine, const TCHAR* szFormat, ... )
{
	if( szName == NULL )
	{
		return;
	}

	va_list args;
	va_start( args, szFormat );

	FLLogEvent* pEvent = FLLogEvent::Alloc();
	pEvent->Set( nLevel, szFunction, szFile, nLine );
	pEvent->FormatV( szFormat, args );

	va_end( args );

	if( Push( szName, pEvent ) == false )
	{
		FLLogEvent::Free( pEvent );
	}
}

bool	FLLogFacade::Push( const TCHAR* szName, FLLogEvent* pEvent )
{
	if( szName == NULL )
	{
		return false;
	}

	FLLog* pLog = SearchAndCreate( szName );
	if( pLog != NULL )
	{
		pLog->Push( pEvent );
		return true;
	}

	return false;
}

void	FLLogFacade::Flush()
{
	m_cContext.Flush();
}

FLLogFacade::FLLog*	FLLogFacade::SearchAndCreate( const TCHAR* szName )
{
	FLLog* pLog = Search( szName );
	if( pLog == NULL )
	{
		FLVERIFY( Add( szName ) );
		pLog = Search( szName );
	}

	return pLog;
}

FLLogFacade::FLLog*	FLLogFacade::Search( const TCHAR* szName )
{
	FLLog* pLog = NULL;
	if( m_pLastHit != NULL && m_pLastHit->IsEqual( szName ) == true )
	{
		pLog = m_pLastHit;
	}
	else
	{
		CMclAutoLock Lock( m_csLogs );

		LOG_ITER pos = m_listLogs.begin();
		LOG_ITER end = m_listLogs.end();

		for( ; pos != end; ++pos )
		{
			FLLog* pTemp = (*pos);
			if( pTemp != NULL )
			{
				if( pTemp->IsEqual( szName ) == true )
				{
					pLog = pTemp;
					m_pLastHit = pLog;
					break;
				}
			}
		}
	}

	return pLog;
}
