
#ifndef	__FLLOGMODULE_H__
#define	__FLLOGMODULE_H__


#include "FLLogCommon.h"
#include "FLLogPublisher.h"
#include "FLLogFlush.h"
#include "FLLogFilter.h"

#include "../thread/FLCriticalSection.h"



class	FLLogModule : public FLLogFlush
{
public:

	FLLogModule();
	~FLLogModule();

	bool		Create( FLLogPublisher* pPublisher, FLLogFilter* pFilter );
	void		Destroy();

	void		Push( FLLogEvent* pEvent );

private:

	void		DoFlush();

	FLLogEvent*	Fetch();

private:

	CMclCritSec		m_csEvent;
	FLLogEvent*		m_pHead;
	FLLogEvent*		m_pTail;

	FLLogFilter*	m_pFilter;
	FLLogPublisher*	m_pPublisher;

};



#endif