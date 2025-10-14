
#include "FLLogFilter.h"



FLLogFilter::~FLLogFilter()
{
}

bool	FLLogFilter::IsPublish( const FLLogEvent* pEvent )
{
	return DoIsPublish( pEvent );
}
