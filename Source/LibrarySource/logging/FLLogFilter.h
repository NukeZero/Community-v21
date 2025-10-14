
#ifndef	__FLLOGFILTER_H__
#define	__FLLOGFILTER_H__



#include "FLLogCommon.h"
#include "FLLogEvent.h"


class	FLLogFilter
{
public:

	virtual ~FLLogFilter();

	bool	IsPublish( const FLLogEvent* pEvent );

private:

	virtual bool	DoIsPublish( const FLLogEvent* pEvent )	= 0;

};







#endif