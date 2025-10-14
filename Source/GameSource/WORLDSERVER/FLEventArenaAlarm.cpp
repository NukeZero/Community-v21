
#include "stdafx.h"
#include "FLEventArenaAlarm.h"


FLEventArenaAlarm::FLEventArenaAlarm()
{
}

FLEventArenaAlarm::~FLEventArenaAlarm()
{
	m_listAlarms.clear();
}

void	FLEventArenaAlarm::Start( DWORD dwInterval, int nUserTag )
{
	m_listAlarms.clear();

	T_ALARM_DATA tData;
	tData.dwInterval = dwInterval;
	tData.nUserTag = nUserTag;
	m_listAlarms.push_back( tData );

	Start();
}

void	FLEventArenaAlarm::Start( const std::vector< T_ALARM_DATA >& vecIntervals )
{
	m_listAlarms.clear();

	std::vector< T_ALARM_DATA >::const_iterator pos = vecIntervals.begin();
	std::vector< T_ALARM_DATA >::const_iterator end = vecIntervals.end();

	for( ; pos != end; ++pos )
	{
		m_listAlarms.push_back( *pos );
	}

	Start();
}

void	FLEventArenaAlarm::Stop()
{
	m_listAlarms.clear();
}

int		FLEventArenaAlarm::TryPop()
{
	if( !m_listAlarms.empty() )
	{
		if( m_kTimer.IsOver() )
		{
			int nUserTag = m_listAlarms.front().nUserTag;
			m_listAlarms.pop_front();

			Start();

			return nUserTag;
		}
	}

	return -1;
}

DWORD	FLEventArenaAlarm::GetReamin()	const
{
	double dRemain = 0;
	if( !IsOver() )
	{
		dRemain = m_kTimer.GetRemain();
		if( dRemain <= 0.0 )
		{
			dRemain = 0;
		}
	}

	return static_cast< DWORD >( dRemain );
}

bool	FLEventArenaAlarm::IsOver()	const
{
	if( m_listAlarms.empty() )
	{
		return true;
	}

	if( m_listAlarms.size() == 1 && m_kTimer.IsOver() )
	{
		return true;
	}

	return false;
}

void	FLEventArenaAlarm::Start()
{
	if( !m_listAlarms.empty() )
	{
		m_kTimer.Set( static_cast< float >( m_listAlarms.front().dwInterval ) );
	}
}
