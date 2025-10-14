
#include "FLLogPubFactory.h"


FLLogPubFactory::~FLLogPubFactory()
{
}

FLLogPublisher*	FLLogPubFactory::Create( const TCHAR* szName )
{
	return DoCreate( szName );
}

void	FLLogPubFactory::Destroy( FLLogPublisher* pPublisher )
{
	DoDestroy( pPublisher );
}
