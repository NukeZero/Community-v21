
#ifndef	__FLLOGLEVELFILTER_H__
#define	__FLLOGLEVELFILTER_H__


#include "FLLogCommon.h"
#include "FLLogFilter.h"



class	FLLogLevelFilter : public FLLogFilter
{
public:

	FLLogLevelFilter();
	~FLLogLevelFilter();

	int		SetLevel( int nLevel );

private:

	bool	DoIsPublish( const FLLogEvent* pEvent );

private:

	int		m_nLevel;

};






#endif