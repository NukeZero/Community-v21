
#ifndef	__FLLOGFORMAT_H__
#define	__FLLOGFORMAT_H__


#include "FLLogCommon.h"
#include "FLLogEvent.h"


class	FLLogFormat
{
public:

	bool	Format( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount );

private:

	virtual bool	DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount )	= 0;

};











#endif