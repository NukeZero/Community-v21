
#ifndef	__FLLOGEVENT_H__
#define	__FLLOGEVENT_H__


#include "FLLogCommon.h"
#include "../memory/FLMemPooler.h"


class	FLLogEvent
{
public:

	static	FLLogEvent*	Alloc();
	static	void		Free( FLLogEvent* pEvent );

private:

	FLLogEvent();
	~FLLogEvent();

public:

	void	Set( int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine );

	void	Format( const TCHAR* szFormat, ... );
	void	FormatV( const TCHAR* szFormat, va_list args );

	void	Clear();

	MEMPOOLER_DECLARE( FLLogEvent );

public:

	FLLogEvent*		pNext;

	int				nLevel;
	const TCHAR *	szFunction;
	const TCHAR *	szFile;
	int				nLine;
	SYSTEMTIME		tTime;
	TCHAR			szText[FLLOG_MAX_LOG_STRING];

};


void	FLLogEventPoolDestroy();



#endif