#ifndef __SVRMNG_H__
#define	__SVRMNG_H__

#define	MAX_SVRSYS	16
#define	MAX_SVRTYPE		8
#define	MAX_SVRINFO		16

#include "ar.h"
#include <dplay.h>

class CSvrInfo
{
public:
	CHAR	m_lpAddr[16];
	BYTE	m_fConnect;
	DWORD	m_dwID;
	DPID	m_dpid;
	long	m_nProcessorTime;
	long	m_nCommittedBytesInUse;
	u_long	m_uPlayerCount;
public:
//	Constructions
	CSvrInfo();
	~CSvrInfo();
//	Operations
	void	Serialize( CAr & ar );
};
	
class CSvrMng
{
public:
	CSvrInfo	m_aaSvrInfo[MAX_SVRTYPE][MAX_SVRINFO];
	u_long	m_uTotalPlayer;
	u_long	m_auSize[MAX_SVRTYPE];
	CMclCritSec		m_AddRemoveLock;

public:
//	Constructions
	CSvrMng();
	~CSvrMng();
//	Operations
	void	Cleanup( CHAR cType );
	void	Add( CHAR cType, const CHAR* lpAddr, DWORD dwID, DPID dpid );
	void	Remove( CHAR cType, DPID dpid );
	int		Find( CHAR cType, const CHAR* lpAddr, DWORD dwID );	// a
	int		Find( CHAR cType, DPID dpid );	// r
	void	Serialize( CAr & ar );
};

#endif	// __SVRMNG_H__