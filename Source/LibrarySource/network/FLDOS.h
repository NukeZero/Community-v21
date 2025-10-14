
#ifndef __FLDOS_H__
#define	__FLDOS_H__


#include "FLNetworkCommon.h"

#include <map>
#include "../memory/FLMemPooler.h"

typedef struct	_CONNECTIONREQUEST
{
	u_long	ip;
	DWORD	dwHit;
	int		nHit;
	BOOL	b;

} CONNECTIONREQUEST,	*PCONNECTIONREQUEST;


#define	MAX_ADDR	1024
#undef MAX_HIT
#define	MAX_HIT		50


class	CDos2 : public std::map< u_long, PCONNECTIONREQUEST >
{
private:

	enum
	{
		DOS_TIMEOUT	= (10*60*1000),	// 10Ка
	};

public:

	CDos2();
	~CDos2();

	int		IsOver( u_long addr );

private:

	void				Add( u_long addr );
	PCONNECTIONREQUEST	Get( u_long addr );
	void				Release();

private:

	typedef	FLMemPooler< CONNECTIONREQUEST >	CONNECTION_REQUEST_POOL;

	DWORD						m_dwStartTick;
	DWORD						m_adwAddr[MAX_ADDR];
	int							m_nAddr;
	CONNECTION_REQUEST_POOL*	m_pPool;

};

#endif	// __DOS2_H__
