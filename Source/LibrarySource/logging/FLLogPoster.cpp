
#include "FLLogPoster.h"
#include "FLLogFacade.h"


FLLogPoster::FLLogPoster( int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine )
	: m_nLevel( nLevel )
	, m_szFunction( szFunction )
	, m_szFile( szFile )
	, m_nLine( nLine )
{
}

void	FLLogPoster::operator()( const TCHAR* szName, const TCHAR* szFormat, ... )
{
	if( szName == NULL )
	{
		return;
	}

	va_list args;
	va_start( args, szFormat );

	FLLogEvent* pEvent = FLLogEvent::Alloc();
	pEvent->Set( m_nLevel, m_szFunction, m_szFile, m_nLine );
	pEvent->FormatV( szFormat, args );

	va_end( args );

	if( g_cLogFacade->Push( szName, pEvent ) == false )
	{
		FLLogEvent::Free( pEvent );
	}
}

void NOOP_FUNC( ... )
{

}
