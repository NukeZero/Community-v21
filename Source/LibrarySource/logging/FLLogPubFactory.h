
#ifndef	__FLLOGPUBFACTORY_H__
#define	__FLLOGPUBFACTORY_H__


#include "FLLogCommon.h"
#include "FLLogPublisher.h"

class	FLLogPubFactory
{
public:

	virtual ~FLLogPubFactory();

	FLLogPublisher*	Create( const TCHAR* szName );
	void	Destroy( FLLogPublisher* pPublisher );

private:

	virtual FLLogPublisher*	DoCreate( const TCHAR* szName )	= 0;
	virtual void	DoDestroy( FLLogPublisher* pPublisher )	= 0;

};









#endif