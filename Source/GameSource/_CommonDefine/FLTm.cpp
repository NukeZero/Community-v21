#include "StdAfx.h"
#include "FLTm.h"

#include <limits>

#undef max
#include <algorithm>


//
//FLDateTime::FLTm	FLGetCurrentTime()
//{
//	__time64_t t		= ::_time64( NULL );	
//
//	t					*= FLDateTimeSpeedProp::GetInstance().GetMultiple();
//
//	FLDateTime::FLTm ret_tm;
//	if( FLTimeTToDateTime( ret_tm, t ) == FALSE )
//		return INVALID_DATETIME;
//	return ret_tm;
//}

BOOL						FLAddDateTime( FLTm & o_fltm, const long nSecond )		//@@@@@ DateTime뿐이 쓸수 없음
{
	__time64_t t;		//= ::_time64( NULL ) + nSecond;	
	if( FLDateTimeToTimeT( t, o_fltm ) == FALSE )
		return FALSE;

	t						+= nSecond;
	return FLTimeTToDateTime( o_fltm, t );
}

//BOOL					FLGetDistance( __time64_t o_t, const FLTm & left, const FLTm & right )
//{
//	// mktime에서 년월일 각각 0이 될 수 없으므로 1년 1월 1일을 더해줘야 정상 계산이 됨
//	const __time64_t SECOND_1YEAR_1MONTH_1DAY = ONE_YEAR_SECOND + 31 * ONE_DAY_SECOND + ONE_DAY_SECOND;
//	
//
//
//	return 0;
//}

BOOL						FLDateTimeToTimeT( __time64_t & o_timet, const FLTm fltm )
{
	struct tm tm;
	tm.tm_year			= ( int )fltm.year - 1900;
	tm.tm_mon			= ( int )fltm.month - 1;
	tm.tm_mday			= ( int )fltm.day;
	tm.tm_hour			= ( int )fltm.hour;
	tm.tm_min			= ( int )fltm.minute;
	tm.tm_sec			= ( int )fltm.second;
	tm.tm_isdst = -1;
	
	const __time64_t t	= ::_mktime64( &tm );
	if( t == -1 )
		return FALSE;
	else
		o_timet			= t;
	return TRUE;
}

BOOL						FLTimeTToDateTime( FLTm & o_fltm, const __time64_t t )
{
	struct tm conv_tm;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	if( _localtime64_s( &conv_tm, &t ) != 0 )
	{
		return FALSE;
	}
#else
	struct tm* ptm		= ::_localtime64( &t );
	if( ptm == NULL )
		return FALSE;

	conv_tm = *ptm;
#endif

	//const __time64_t tCorrection	= ::_mktime64( tm );
	//tm								= ::_localtime64( &t );
	//if( tm == NULL )
	//	return FALSE;

	o_fltm.year			= conv_tm.tm_year + 1900;
	o_fltm.month		= conv_tm.tm_mon + 1;
	o_fltm.day			= conv_tm.tm_mday;
	o_fltm.hour			= conv_tm.tm_hour;
	o_fltm.minute		= conv_tm.tm_min;
	o_fltm.second		= conv_tm.tm_sec;
	o_fltm.padding		= 0;

	return TRUE;
}


FLTm						FLStringToFLTm( const TCHAR * szDateTime, const FLTm::E_FORMAT::E eStrFormat )
{
	FLTm ret_tm;
	ret_tm.value			= 0;

	switch( eStrFormat )
	{
	case FLTm::E_FORMAT::YYYYMMDD_HHMMSS:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::DATETIME_STRING_MAX - 1 ) ) 
				return INVALID_DATETIME;

			ret_tm.year			= ::atoi( &szDateTime[ 0 ] );
			ret_tm.month		= ::atoi( &szDateTime[ 5 ] );
			ret_tm.day			= ::atoi( &szDateTime[ 8 ] );
			ret_tm.hour			= ::atoi( &szDateTime[ 11 ] );
			ret_tm.minute		= ::atoi( &szDateTime[ 14 ] );
			ret_tm.second		= ::atoi( &szDateTime[ 17 ] );
		}
		break;

	case FLTm::E_FORMAT::YYYYMMDD_HHMM:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::DATEMIN_STRING_MAX - 1 ) ) 
				return INVALID_DATETIME;

			ret_tm.year			= ::atoi( &szDateTime[ 0 ] );
			ret_tm.month		= ::atoi( &szDateTime[ 5 ] );
			ret_tm.day			= ::atoi( &szDateTime[ 8 ] );
			ret_tm.hour			= ::atoi( &szDateTime[ 11 ] );
			ret_tm.minute		= ::atoi( &szDateTime[ 14 ] );
		}
		break;

	case FLTm::E_FORMAT::YYYYMMDD:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::DATE_STRING_MAX - 1 ) ) 
				return INVALID_DATETIME;

			ret_tm.year			= ::atoi( &szDateTime[ 0 ] );
			ret_tm.month		= ::atoi( &szDateTime[ 5 ] );
			ret_tm.day			= ::atoi( &szDateTime[ 8 ] );
		}
		break;

	case FLTm::E_FORMAT::HHMMSS:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::TIME_STRING_MAX - 1 ) ) 
				return INVALID_DATETIME;

			ret_tm.hour			= ::atoi( &szDateTime[ 0 ] );
			ret_tm.minute		= ::atoi( &szDateTime[ 3 ] );
			ret_tm.second		= ::atoi( &szDateTime[ 6 ] );
		}
		break;
	
	default:
		return INVALID_DATETIME;
		break;
	}

	
	if( ret_tm.year > 3000 || ret_tm.month > 12 || ret_tm.day > 31
		|| ret_tm.hour >= 24 || ret_tm.minute >= 60 || ret_tm.second >= 60 )
		return INVALID_DATETIME;

	//오류 검사.
	//__time64_t t;
	//if( FLDateTimeToTimeT( t, ret_tm ) == FALSE )
	//	return INVALID_DATETIME;

	//if( FLTimeTToDateTime( ret_tm, t ) == FALSE )
	//	return INVALID_DATETIME;

	return ret_tm;
}




tstring				FLTmToString( const FLTm & tm, const FLTm::E_FORMAT::E eFormat ) /*"2000-09-10 16:00:00" 포맷만 지원 */
{
	//tstring str;
	switch( eFormat )
	{
	case FLTm::E_FORMAT::YYYYMMDD_HHMMSS:
		{
			TCHAR out[ FLTm::DATETIME_STRING_MAX ];
			FLSPrintf( out, _countof( out ), _T( "%04I64u-%02I64u-%02I64u %02I64u:%02I64u:%02I64u" ), tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second );
			return tstring( out );
		}
		break;
	case FLTm::E_FORMAT::YYYYMMDD:
		{
			TCHAR out[ FLTm::DATE_STRING_MAX ];
			FLSPrintf( out, _countof( out ), _T( "%04I64u-%02I64u-%02I64u" ), tm.year, tm.month, tm.day );
			return tstring( out );
		}
		break;

	case FLTm::E_FORMAT::HHMMSS:
		{
			TCHAR out[ FLTm::TIME_STRING_MAX ];
			FLSPrintf( out, _countof( out ), _T( "%02I64u:%02I64u:%02I64u" ), tm.hour, tm.minute, tm.second );
			return tstring( out );
		}
		break;
	}
	
	return tstring( _T( "" ) );
}

long						FLTimeToSecond( const long hour, const long minute, const long second )
{
	return hour * FLTm::ONE_HOUR_SECOND + minute * FLTm::ONE_MINUTE_SECOND + second;
}

//
//UINT64						FLTimeToSecond( const long year, const long hour, const long minute, const long second )
//{
//	return ( UINT64 )( year * FLTm::ONE_YEAR_SECOND ) + ( UINT64 )( hour * FLTm::ONE_HOUR_SECOND ) + ( UINT64 )( minute * FLTm::ONE_MINUTE_SECOND ) + ( UINT64 )( second );
//}


FLTm						FLExtractDate( const FLTm & fltm )
{
	FLTm ret_tm;
	ret_tm.value			= ( fltm.value >> 41 ) << 41;
	//ret_tm.value			= ( fltm.value >> 17 ) << 17;
	//ret_tm.hour				= 0;
	//ret_tm.minute			= 0;
	//ret_tm.second			= 0;
	//ret_tm.padding			= 0;
	return ret_tm;
}



FLTm						FLExtractTime( const FLTm & fltm )
{
	FLTm ret_tm;
	ret_tm.value			= ( fltm.value << 23 ) >> 23;
	//ret_tm.value			= ( fltm.value << 47 ) >> 47;
	//ret_tm.year				= 0;
	//ret_tm.month			= 0;
	//ret_tm.day				= 0;
	//ret_tm.padding			= 0;
	return ret_tm;
}


FLTm						FLMergeTm( const FLTm & tmDateArea, const FLTm & tmTimeArea )
{
	FLTm ret_tm;
	ret_tm.value			= FLExtractDate( tmDateArea ).value | FLExtractTime( tmTimeArea ).value;
	return ret_tm;
}


//BOOL				FLLessThanTimeOnly( const FLDateTime::FLTm & src, const FLDateTime::FLTm & dst )
//{
//	return ( src.value << 47 ) < ( dst.value << 47 );
//}

//BOOL /*FLDateTime::*/FLEqualTimeOnly( const FLDateTime::FLTm & src, const FLDateTime::FLTm & dst )
//{
//	return ( src.value << 47 ) == ( dst.value << 47 );
//}

FLTm	FLInvalidDateTime()
{
//	FLTm tm	= { std::numeric_limits< unsigned __int64 >::max() };
	FLTm tm;
	tm.value		= std::numeric_limits< unsigned __int64 >::max();
	return tm;
}



//---------------------------------------------------------------------------------------------------------------------//

FLDateTimer::FLDateTimer()
: m_wMultiple( 1 )
{
//#ifdef _DEBUG
	m_tStart				= ::_time64( NULL );
//#endif //_DEBUG
}

FLDateTimer::~FLDateTimer()
{
}


WORD FLDateTimer::GetMultiple() const
{
	return m_wMultiple;
}

FLTm			FLDateTimer::GetCurrentTime()	
{
	
//#ifdef _DEBUG
	__time64_t tCurr		= ::_time64( NULL );	
	const __time64_t tDiff	= tCurr - m_tStart;

	m_wMultiple				= std::max( ( WORD )1, m_wMultiple );
	tCurr					= m_tStart + ( tDiff * m_wMultiple );
//#else
//	const __time64_t tCurr	= ::_time64( NULL );	
//#endif //_DEBUG

	struct tm conv_tm;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	if( _localtime64_s( &conv_tm, &tCurr ) != 0 )
	{
		return INVALID_DATETIME;
	}
#else
	struct tm* ptm		= ::_localtime64( &tCurr );
	if( ptm == NULL )
		return INVALID_DATETIME;

	conv_tm = *ptm;
#endif

	if( ::_mktime64( &conv_tm ) == -1 )
		return INVALID_DATETIME;

	FLTm ret_tm;
	ret_tm.year				= conv_tm.tm_year + 1900;
	ret_tm.month			= conv_tm.tm_mon + 1;
	ret_tm.day				= conv_tm.tm_mday;
	ret_tm.hour				= conv_tm.tm_hour;
	ret_tm.minute			= conv_tm.tm_min;
	ret_tm.second			= conv_tm.tm_sec;
	ret_tm.padding			= 0;
	return ret_tm;
}



//#ifdef _DEBUG
WORD FLDateTimer::GetTimeSpeed_DebugMode() const
{
	return m_wMultiple;
}

void FLDateTimer::SetTimeSpeed_DebugMode( const WORD wMultiple )			//갑자기 시간이 뻥튀기된다.
{
	//시작 시간 갱신.
	m_wMultiple						= ( wMultiple < 1 ) ? 1 : wMultiple;
}

//#endif //_DEBUG;



CTime FLStringToCTime( const char * szDateTime, const FLTm::E_FORMAT::E eFormat )
{
	int nYear				= 0;
	int nMonth				= 0;
	int nDay				= 0;
	int nHour				= 0;
	int nMin				= 0;
	int nSec				= 0;

	switch( eFormat )
	{
	case FLTm::E_FORMAT::YYYYMMDD_HHMMSS:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::DATETIME_STRING_MAX - 1 ) ) 
				return CTime();

			nYear			= ::atoi( &szDateTime[ 0 ] );
			nMonth			= ::atoi( &szDateTime[ 5 ] );
			nDay			= ::atoi( &szDateTime[ 8 ] );
			nHour			= ::atoi( &szDateTime[ 11 ] );
			nMin			= ::atoi( &szDateTime[ 14 ] );
			nSec			= ::atoi( &szDateTime[ 17 ] );
		}
		break;

	case FLTm::E_FORMAT::YYYYMMDD:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::DATE_STRING_MAX - 1 ) ) 
				return CTime();

			nYear			= ::atoi( &szDateTime[ 0 ] );
			nMonth			= ::atoi( &szDateTime[ 5 ] );
			nDay			= ::atoi( &szDateTime[ 8 ] );
		}
		break;

	case FLTm::E_FORMAT::HHMMSS:
		{
			if( ::_tcslen( szDateTime ) < ( FLTm::TIME_STRING_MAX - 1 ) ) 
				return CTime();

			nHour			= ::atoi( &szDateTime[ 0 ] );
			nMin			= ::atoi( &szDateTime[ 3 ] );
			nSec			= ::atoi( &szDateTime[ 6 ] );
		}
		break;

	default:
		return CTime();
		break;
	}

	return CTime( nYear, nMonth, nDay, nHour, nMin, nSec );
}

int FLDateStringToInt( const char * szDateTime )
{
	const int nYear	= ::atoi( &szDateTime[ 0 ] );
	const int nMonth= ::atoi( &szDateTime[ 5 ] );
	const int nDay	= ::atoi( &szDateTime[ 8 ] );

	return nYear * 10000 + nMonth * 100 + nDay;
}
