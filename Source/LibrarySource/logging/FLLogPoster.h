
#ifndef	__FLLOGPOSTER_H__
#define	__FLLOGPOSTER_H__


#include "FLLogCommon.h"
#include "FLLogEvent.h"


class	FLLogPoster
{
public:

	FLLogPoster( int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine );
	void	operator()( const TCHAR* szName, const TCHAR* szFormat, ... );

private:

	int				m_nLevel;
	const TCHAR*	m_szFunction;
	const TCHAR*	m_szFile;
	int				m_nLine;

};

void  NOOP_FUNC( ... );		//C4100 warning을 없애기 위한 몸부림







#endif