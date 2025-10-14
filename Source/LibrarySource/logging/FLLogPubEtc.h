
#ifndef	__FLLOGPUBETC_H__
#define	__FLLOGPUBETC_H__


#include "FLLogCommon.h"
#include "FLLogPublisher.h"

#include "../thread/FLCriticalSection.h"

//////////////////////////////////////////////////////////////////////////

class	FLLogPubDebugString : public FLLogPublisher
{
public:

	FLLogPubDebugString();
	~FLLogPubDebugString();

private:

	void	DoPublish( const FLLogEvent* pEvent );

};

//////////////////////////////////////////////////////////////////////////

class	FLLogPubStdOut : public FLLogPublisher
{
public:

	FLLogPubStdOut();
	~FLLogPubStdOut();

private:

	void	DoPublish( const FLLogEvent* pEvent );

};

//////////////////////////////////////////////////////////////////////////

class	FLLogPubMessageBox : public FLLogPublisher
{
public:

	FLLogPubMessageBox();
	~FLLogPubMessageBox();

private:

	void	DoPublish( const FLLogEvent* pEvent );

private:

	bool	m_bShow;

};

//////////////////////////////////////////////////////////////////////////

#include <list>

// thread-safe
class	FLLogPubComposite : public FLLogPublisher
{
public:

	FLLogPubComposite();
	~FLLogPubComposite();

	void	Add( FLLogPublisher* pPublisher );
	void	Delete( FLLogPublisher* pPublisher );

	void	Clear();

private:

	void	DoPublish( const FLLogEvent* pEvent );
	void	DoDoneTick();

private:

	CMclCritSec						m_csPublishers;
	std::list< FLLogPublisher* >	m_listPublishers;

};

//////////////////////////////////////////////////////////////////////////


#endif