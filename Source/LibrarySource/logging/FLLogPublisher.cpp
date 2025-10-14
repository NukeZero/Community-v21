
#include "FLLogPublisher.h"


FLLogPublisher::~FLLogPublisher()
{
}

void	FLLogPublisher::Publish( const FLLogEvent* pEvent )
{
	DoPublish( pEvent );
}

void	FLLogPublisher::DoneTick()
{
	DoDoneTick();
}

void	FLLogPublisher::DoDoneTick()
{
}
