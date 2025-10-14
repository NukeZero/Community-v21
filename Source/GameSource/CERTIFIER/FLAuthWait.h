
#ifndef	__FLAUTHWAIT_H__
#define	__FLAUTHWAIT_H__


#include "FLDBOverlappedPlus.h"



typedef	void	(*TIMEOUT_HANDLER)( void* pParam, AUTH_INFO* pAuthInfo );

class	FLAuthWait
{
private:

	struct	T_AUTH_WAIT
	{
		DWORD		dwRegisterTick;
		AUTH_INFO	AuthInfo;
	};

	enum
	{
		TIMEOUT_SEC	= 60,
	};

public:

	FLAuthWait();
	~FLAuthWait();

	bool	Register( AUTH_INFO* pAuthInfo );
	bool	Unregister( const char* szAccount, AUTH_INFO* pAuthInfo );

	void	TimeoutProcess( TIMEOUT_HANDLER pFunction, void* pParam );

private:

	bool	IsValid( AUTH_INFO* pAuthInfo );

private:

	typedef	std::list< T_AUTH_WAIT >	AUTH_WAIT_LIST;

	AUTH_WAIT_LIST	m_listWait;

};



#endif