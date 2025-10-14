
#ifndef	__FLNETWORKLOGADAPTER_H__
#define	__FLNETWORKLOGADAPTER_H__


#ifndef	_DEBUG
#define	__FLLOG_EXCLUDE_DEBUG_TRACE
#endif


#include "../logging/FLLogFacade.h"


void			SetNetworkLogName( const TCHAR* szName );
const TCHAR*	GetNetworkLogName();





#endif