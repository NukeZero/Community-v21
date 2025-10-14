
#include "FLLogLevelFilter.h"


FLLogLevelFilter::FLLogLevelFilter()
{
	m_nLevel = FLLOG_ALL;
}

FLLogLevelFilter::~FLLogLevelFilter()
{
}

int		FLLogLevelFilter::SetLevel( int nLevel )
{
	int nOldLevel = m_nLevel;
	m_nLevel = nLevel;
	return nOldLevel;
}

bool	FLLogLevelFilter::DoIsPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		if( pEvent->nLevel >= m_nLevel )
		{
			return true;
		}
	}

	return false;
}
