
#ifndef	__FLLOGCONTEXT_H__
#define	__FLLOGCONTEXT_H__


#include "FLLogCommon.h"
#include "FLLogFlush.h"

#include "../thread/FLCriticalSection.h"

#include <list>


class	FLLogContext
{
public:

	FLLogContext();
	~FLLogContext();

	bool	Start( DWORD dwInterval = 10 );
	void	Stop( DWORD dwTimeout = INFINITE );

	void	Attach( FLLogFlush* pFlush );
	void	Detach( FLLogFlush* pFlush );

	void	Run();
	void	Flush();

private:

	void	RunFlush();
	void	Clear();

private:

	typedef	std::list< FLLogFlush* >		LOGFLUSH_LIST;
	typedef	LOGFLUSH_LIST::iterator			LOGFLUSH_ITER;
	typedef	LOGFLUSH_LIST::const_iterator	LOGFLUSH_CITER;

	HANDLE			m_hThread;
	volatile bool	m_bIsStop;

	DWORD			m_dwInterval;

	CMclCritSec		m_csFlushs;
	LOGFLUSH_LIST	m_listFlushs;

};


#endif