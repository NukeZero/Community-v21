#include "StdAfx.h"
#include "FLCommonCheck.h"

FLCommonCheck::FLCommonCheck()
{

}

FLCommonCheck::~FLCommonCheck()
{

}

FLCommonCheck*	FLCommonCheck::GetInstance()
{
	static FLCommonCheck	kCommonCheck;
	
	return & kCommonCheck;
}

bool	FLCommonCheck::CheckTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST )
{
	if( ( nYear < 1900 )
		|| ( nMonth < 1 || nMonth > 12 )
		|| ( nDay < 1 || nDay > 31 )
		)
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CTime Is Invalid. Y:[%d], M:[%d], D:[%d], H:[%d], M:[%d], S:[%d]" )
			, nYear, nMonth, nDay, nHour, nMin, nSec );

		return false;
	}

	struct tm atm;
	atm.tm_sec				= nSec;
	atm.tm_min				= nMin;
	atm.tm_hour				= nHour;
	atm.tm_mday				= nDay;
	atm.tm_mon				= nMonth - 1;
	atm.tm_year				= nYear - 1900;
	atm.tm_isdst			= nDST;
	__time64_t kTimeTmp		= _mktime64( &atm );

	if( kTimeTmp == -1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CTime Is Invalid. Y:[%d], M:[%d], D:[%d], H:[%d], M:[%d], S:[%d]" )
			, nYear, nMonth, nDay, nHour, nMin, nSec );

		return false;
	}

	return true;
}

bool	FLCommonCheck::CheckTime( const SYSTEMTIME & kTime, int nDST )
{
	if( CheckTime( static_cast<int>( kTime.wYear ), static_cast<int>( kTime.wMonth ), static_cast<int>( kTime.wDay )
		, static_cast<int>( kTime.wHour ), static_cast<int>( kTime.wMinute ), static_cast<int>( kTime.wSecond ), nDST ) == false )
	{
		return false;
	}

	return true;
}

bool	FLCommonCheck::CheckTime( const FILETIME & kTime, int nDST )
{
	FILETIME localTime;
	if( FileTimeToLocalFileTime( &kTime, &localTime ) == FALSE )
	{
		return false;
	}

	SYSTEMTIME sysTime;
	if( FileTimeToSystemTime( &localTime, &sysTime ) == FALSE )
	{
		return false;
	}

	if( CheckTime( sysTime, nDST ) == false )
	{
		return false;
	}

	return true;
}
