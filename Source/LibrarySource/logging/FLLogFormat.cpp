
#include "FLLogFormat.h"


bool	FLLogFormat::Format( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount )
{
	return DoFormat( pEvent, szBuffer, cchCount );
}
