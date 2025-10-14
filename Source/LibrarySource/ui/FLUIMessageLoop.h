
#ifndef	__FLUIMESSAGELOOP_H__
#define	__FLUIMESSAGELOOP_H__


#include "FLUICommon.h"


//////////////////////////////////////////////////////////////////////////
class	FLUIIdleHandler
{
public:

	bool	OnIdle();

private:

	virtual bool	DoOnIdle()	= 0;

};

//////////////////////////////////////////////////////////////////////////

class	FLUIMessageFilter
{
public:

	bool	PreTranslateMessage( MSG* pMsg );

private:

	virtual bool	DoPreTranslateMessage( MSG* pMsg )	= 0;

};
//////////////////////////////////////////////////////////////////////////


class	FLUIMessageLoop
{
public:

	FLUIMessageLoop();
	~FLUIMessageLoop();

	void	SetIdleHandler( FLUIIdleHandler* pHandler );
	void	SetMessageFilter( FLUIMessageFilter* pFilter );

	int		Run();

private:

	bool	PreTranslateMessage( MSG* pMsg );
	bool	OnIdle();

private:

	FLUIIdleHandler*	m_pIdleHandler;
	FLUIMessageFilter*	m_pMessageFilter;

};





#endif