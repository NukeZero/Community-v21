
#include "FLLogCommon.h"



static const TCHAR	LOGGING_LEVEL_STRING[][10] =
{
	_T( "UNKNOWN" ),
	_T( "DEBUG" ),
	_T( "TRACE" ),
	_T( "INFO" ),
	_T( "WARNING" ),
	_T( "ERROR" ),
	_T( "FAULT" ),
};


const TCHAR*	GetLogLevelString( int nLevel )
{
	switch( nLevel )
	{
	case FLLOG_DEBUG:
	case FLLOG_TRACE:
	case FLLOG_INFO:
	case FLLOG_WARNING:
	case FLLOG_ERROR:
	case FLLOG_FAULT:
		return LOGGING_LEVEL_STRING[nLevel];
	}

	return LOGGING_LEVEL_STRING[0];
}
