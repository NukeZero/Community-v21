
#include "FLLogEvent.h"
#include "../common/FLStringFunction.h"

MEMPOOLER_IMPLEMENT( FLLogEvent, 128 );

FLLogEvent*	FLLogEvent::Alloc()
{
	return new FLLogEvent();
}

void	FLLogEvent::Free( FLLogEvent* pEvent )
{
	SAFE_DELETE( pEvent );
}

FLLogEvent::FLLogEvent()
{
	Clear();
}

FLLogEvent::~FLLogEvent()
{
	Clear();
}

void	FLLogEvent::Set( int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine )
{
	this->nLevel = nLevel;
	this->szFunction = szFunction;
	this->szFile = szFile;
	this->nLine = nLine;
	::GetLocalTime( &tTime );
}

void	FLLogEvent::Format( const TCHAR* szFormat, ... )
{
	va_list args;

	va_start( args, szFormat );
	FormatV( szFormat, args );
	va_end( args );
}

void	FLLogEvent::FormatV( const TCHAR* szFormat, va_list args )
{
	FLVSPrintf( szText, _countof( szText ), szFormat, args );
}

void	FLLogEvent::Clear()
{
	pNext = NULL;

	nLevel = FLLOG_NONE;

	szFunction = NULL;
	szFile = NULL;
	nLine = 0;

	::memset( &tTime, 0, sizeof( tTime ) );

	::memset( szText, 0, sizeof( szText ) );
}


void	FLLogEventPoolDestroy()
{
	MEMPOOLER_DESTROY( FLLogEvent );
}
