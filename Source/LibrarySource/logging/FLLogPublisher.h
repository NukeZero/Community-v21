
#ifndef	__FLLOGPUBLISHER_H__
#define	__FLLOGPUBLISHER_H__

#include "FLLogCommon.h"
#include "FLLogEvent.h"


class	FLLogPublisher
{
public:

	virtual ~FLLogPublisher();

	void	Publish( const FLLogEvent* pEvent );
	void	DoneTick();

private:

	virtual void	DoPublish( const FLLogEvent* pEvent )	= 0;
	virtual void	DoDoneTick();

};


#endif