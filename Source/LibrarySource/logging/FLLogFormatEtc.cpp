
#include "FLLogFormatEtc.h"
#include "../common/FLStringFunction.h"

#include <iomanip>


//////////////////////////////////////////////////////////////////////////
namespace
{
	const TCHAR* szUnknownFunction = _T( "Unknown Function" );
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

bool	FLLogSampleFormat::DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount )
{
	const TCHAR* szFunction = pEvent->szFunction;
	if( szFunction == NULL )
	{
		szFunction = szUnknownFunction;
	}

	if( FLSPrintf( szBuffer, cchCount, _T( "[%s] %s(%d): %s\r\n" ),
		GetLogLevelString( pEvent->nLevel ),
		szFunction,
		pEvent->nLine,
		pEvent->szText ) < 0 )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool	FLLogGenericFormat::DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount )
{
	const TCHAR* szFunction = pEvent->szFunction;
	if( szFunction == NULL )
	{
		szFunction = szUnknownFunction;
	}

	if( FLSPrintf( szBuffer, cchCount,
		_T( "[%04d-%02d-%02d %02d:%02d:%02d] (%s) %s(%d): %s\r\n" ),
		pEvent->tTime.wYear, pEvent->tTime.wMonth, pEvent->tTime.wDay,
		pEvent->tTime.wHour, pEvent->tTime.wMinute, pEvent->tTime.wSecond,
		GetLogLevelString( pEvent->nLevel ),
		szFunction,
		pEvent->nLine,
		pEvent->szText ) < 0 )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool	FLLogCSVFormat::DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount )
{
	//////////////////////////////////////////////////////////////////////////
	// TODO 변환자 변경
	TCHAR szTempText[FLLOG_MAX_LOG_STRING] = { 0, };
	FLStrcpy( szTempText, _countof( szTempText ), pEvent->szText );
	FLStrConvertChars( szTempText, _T( "\"" ), _T( "`" ) );
	//////////////////////////////////////////////////////////////////////////

	const TCHAR* szFunction = pEvent->szFunction;
	if( szFunction == NULL )
	{
		szFunction = szUnknownFunction;
	}

	if( FLSPrintf( szBuffer, cchCount,
		_T( "\"%04d-%02d-%02d %02d:%02d:%02d\",\"%s\",\"%s\",\"%d\",\"%s\"\r\n" ),
		pEvent->tTime.wYear, pEvent->tTime.wMonth, pEvent->tTime.wDay,
		pEvent->tTime.wHour, pEvent->tTime.wMinute, pEvent->tTime.wSecond,
		GetLogLevelString( pEvent->nLevel ),
		szFunction,
		pEvent->nLine,
		szTempText ) < 0 )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
