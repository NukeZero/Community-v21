#include "StdAfx.h"

#include "FLShutdownRule.h"


bool FLShutdownRule::IsShutDownTime()
{
	const CTime & tmCurr		= CTime::GetCurrentTime();
	if( FLShutdownRule::SHUTDOWN_START_HOUR > FLShutdownRule::SHUTDOWN_END_HOUR )
	{
		if( FLShutdownRule::SHUTDOWN_START_HOUR > tmCurr.GetHour() && tmCurr.GetHour() >= FLShutdownRule::SHUTDOWN_END_HOUR )
			return false;
	}
	else
	{
		if( FLShutdownRule::SHUTDOWN_START_HOUR > tmCurr.GetHour() || tmCurr.GetHour() >= FLShutdownRule::SHUTDOWN_END_HOUR )
			return false;
	}
	return true;
}

bool FLShutdownRule::IsShutDownAge( const CTime & tmBirthDate )
{
	//const CTime tmExpired		= tmBirthDate + CTimeSpan( FLShutdownRule::SHUTDOWN_AGE * 365, 0, 0, 0 );
	//return ( tmExpired <= CTime::GetCurrentTime() ) ? false : true;

	//const CTime tmMinBirth		=CTime::GetCurrentTime() - CTimeSpan( FLShutdownRule::SHUTDOWN_AGE * 365, 0, 0, 0 );
	//return ( tmBirthDate >= tmMinBirth );
	
	struct tm tCurr;
	if( tmBirthDate.GetLocalTm( &tCurr ) == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ INVALID BIRTH : %d-%d-%d %d:%d:%d ]", tmBirthDate.GetYear(), tmBirthDate.GetMonth(), tmBirthDate.GetDay(), tmBirthDate.GetHour(), tmBirthDate.GetMinute(), tmBirthDate.GetSecond() );
		return false;
	}

	tCurr.tm_year			+=  FLShutdownRule::SHUTDOWN_AGE;
	tCurr.tm_isdst			= -1;

	const __time64_t t		= ::_mktime64( &tCurr );
	const CTime tmExpired( t );
	
	return ( tmExpired <= CTime::GetCurrentTime() ) ? false : true;
}


bool FLShutdownRule::InShutDownRule( const CTime & tmBirthDate )
{
	//16살 이상이면 pass
	if( IsShutDownAge( tmBirthDate ) == false )
		return false;

	//셧다운 시간이면
	return IsShutDownTime();
}

