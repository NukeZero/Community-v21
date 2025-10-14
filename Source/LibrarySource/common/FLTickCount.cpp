
#include "FLTickCount.h"


FLTickCount::FLTickCount()
{
	m_n64Start	= 0;
	m_dwBase	= 0;
	m_dwOffset	= 0;
}

FLTickCount::~FLTickCount()
{
}

void	FLTickCount::Start( const SYSTEMTIME& tSysTime )
{
	FILETIME tFileTime;
	::SystemTimeToFileTime( &tSysTime, &tFileTime );
	::memcpy( &m_n64Start, &tFileTime, sizeof( m_n64Start ) );
	m_dwBase	= ::GetTickCount();
}

void	FLTickCount::Start( const __int64& n64Start )
{
	m_n64Start	= n64Start;
	m_dwBase	= m_dwOffset	= ::GetTickCount();
}

void	FLTickCount::Start()
{
	FILETIME tFileTime;
	::GetSystemTimeAsFileTime( &tFileTime );
	::memcpy( &m_n64Start, &tFileTime, sizeof( m_n64Start ) );
	m_dwBase	= ::GetTickCount();
}

__int64	FLTickCount::GetTickCount()
{
	m_dwOffset	= ::GetTickCount();
	return( m_n64Start + ( m_dwOffset - m_dwBase ) * 10000 );
}

int		FLTickCount::GetOffset( const __int64& n64Start )	const
{
	LARGE_INTEGER liDiff;
	liDiff.QuadPart		= ::GetTickCount() - n64Start;
	return liDiff.LowPart / 10000;
}
