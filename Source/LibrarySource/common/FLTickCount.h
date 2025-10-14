#ifndef __FLTICKCOUNT_H__
#define	__FLTICKCOUNT_H__

#include "FLCommon.h"


class	FLTickCount
{
public:

	FLTickCount();
	~FLTickCount();

	void	Start( const SYSTEMTIME& tSysTime );
	void	Start( const __int64& n64Start );
	void	Start();

	__int64	GetTickCount();
	int		GetOffset( const __int64 & n64Start )	const;

private:

	__int64	m_n64Start;
	DWORD	m_dwBase;
	DWORD	m_dwOffset;

};


#endif