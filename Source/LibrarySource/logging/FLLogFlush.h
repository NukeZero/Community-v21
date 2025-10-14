
#ifndef	__FLLOGFLUSH_H__
#define	__FLLOGFLUSH_H__


#include "FLLogCommon.h"


class	FLLogFlush
{
public:

	void	Flush();

private:

	virtual void	DoFlush()	= 0;

};







#endif