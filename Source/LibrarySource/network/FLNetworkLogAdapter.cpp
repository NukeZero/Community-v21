
#include "FLNetworkLogAdapter.h"


static const TCHAR*	NETWORK_LOG_NAME	= NULL;

void	SetNetworkLogName( const TCHAR* szName )
{
	NETWORK_LOG_NAME = szName;
}

const TCHAR*	GetNetworkLogName()
{
	return NETWORK_LOG_NAME;
}
