
#include "FLLogPubEtc.h"
#include "FLLogFormatEtc.h"
#include "../common/FLStringFunction.h"
#include "../thread/FLAutoLock.h"

//////////////////////////////////////////////////////////////////////////

FLLogPubDebugString::FLLogPubDebugString()
{
}

FLLogPubDebugString::~FLLogPubDebugString()
{
}

void	FLLogPubDebugString::DoPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		TCHAR szBuffer[FLLOG_MAX_LOG_STRING+1024] = { 0, };

		FLLogSampleFormat cFormater;
		cFormater.Format( pEvent, szBuffer, _countof( szBuffer ) );

		::OutputDebugString( szBuffer );
	}
}

//////////////////////////////////////////////////////////////////////////

#include <cstdio>

FLLogPubStdOut::FLLogPubStdOut()
{
}

FLLogPubStdOut::~FLLogPubStdOut()
{
}

void	FLLogPubStdOut::DoPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		TCHAR szBuffer[FLLOG_MAX_LOG_STRING+1024] = { 0, };

		FLLogSampleFormat cFormater;
		cFormater.Format( pEvent, szBuffer, _countof( szBuffer ) );

		::_tprintf( _T( "%s" ), szBuffer );
	}
}

//////////////////////////////////////////////////////////////////////////

FLLogPubMessageBox::FLLogPubMessageBox()
{
	m_bShow = true;
}

FLLogPubMessageBox::~FLLogPubMessageBox()
{
}

void	FLLogPubMessageBox::DoPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		if( m_bShow == true )
		{
			TCHAR szBuffer[FLLOG_MAX_LOG_STRING+1024] = { 0, };

			FLLogSampleFormat cFormater;
			cFormater.Format( pEvent, szBuffer, _countof( szBuffer ) );

			if( ::MessageBox( NULL, szBuffer, GetLogLevelString( pEvent->nLevel ), MB_OKCANCEL ) == IDCANCEL )
			{
				m_bShow = false;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

FLLogPubComposite::FLLogPubComposite()
{
}

FLLogPubComposite::~FLLogPubComposite()
{
	Clear();
}

void	FLLogPubComposite::Add( FLLogPublisher* pPublisher )
{
	if( pPublisher != NULL )
	{
		CMclAutoLock	Lock( m_csPublishers );
		m_listPublishers.push_back( pPublisher );
	}
}

void	FLLogPubComposite::Delete( FLLogPublisher* pPublisher )
{
	CMclAutoLock	Lock( m_csPublishers );

	std::list< FLLogPublisher* >::iterator pos = m_listPublishers.begin();
	std::list< FLLogPublisher* >::iterator end = m_listPublishers.end();

	for( ; pos != end; ++pos )
	{
		FLLogPublisher* pDestPublisher = (*pos);
		if( pDestPublisher == pPublisher )
		{
			m_listPublishers.erase( pos );
			break;
		}
	}
}

void	FLLogPubComposite::Clear()
{
	CMclAutoLock	Lock( m_csPublishers );
	m_listPublishers.clear();
}

void	FLLogPubComposite::DoPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		CMclAutoLock	Lock( m_csPublishers );

		std::list< FLLogPublisher* >::const_iterator pos = m_listPublishers.begin();
		std::list< FLLogPublisher* >::const_iterator end = m_listPublishers.end();

		for( ; pos != end; ++pos )
		{
			FLLogPublisher* pPublisher = (*pos);
			if( pPublisher != NULL )
			{
				pPublisher->Publish( pEvent );
			}
		}
	}
}

void	FLLogPubComposite::DoDoneTick()
{
	CMclAutoLock	Lock( m_csPublishers );

	std::list< FLLogPublisher* >::const_iterator pos = m_listPublishers.begin();
	std::list< FLLogPublisher* >::const_iterator end = m_listPublishers.end();

	for( ; pos != end; ++pos )
	{
		FLLogPublisher* pPublisher = (*pos);
		if( pPublisher != NULL )
		{
			pPublisher->DoneTick();
		}
	}
}
