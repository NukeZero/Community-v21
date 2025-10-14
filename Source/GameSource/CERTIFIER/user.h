#ifndef __USER_H__
#define	__USER_H__

#include <map>
#include <string>
#include <network/FLDPMng.h>


//////////////////////////////////////////////////////////////////////////
class CUser
{
public:

	DPID	m_dpid;
	DWORD	m_dwTick;
	BOOL	m_bValid;
	int		nGPotatoNo;

private:

	char	m_pszAccount[MAX_ACCOUNT];

public:

	CUser( DPID dpid );
	~CUser();

	void SetAccount( const char* szAccount );
	const char*	GetAccount();

};
//////////////////////////////////////////////////////////////////////////

class CUserMng : private std::map<DPID, CUser*>
{
public:

	CMclCritSec		m_AddRemoveLock;

private:

	CUserMng();
	~CUserMng();

public:

	BOOL	AddUser( DPID dpid );
	BOOL	RemoveUser( DPID dpid );
	CUser*	GetUser( DPID dpid );
	void	ClearDum( CDPMng* pdp );

	static	CUserMng*	GetInstance( void );
};


#endif	// __USER_H__