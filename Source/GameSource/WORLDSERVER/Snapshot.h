#ifndef __SNAPSHOT_H__
#define	__SNAPSHOT_H__

#pragma once

#include <network/FLAr.h>
//#include "BLQueue.h"
#include <memory/FLMemPooler.h>

class CSnapshot
{
public:
	DPID	dpidCache;
	DPID	dpidUser;
	CAr		ar;
	short	cb;
public:
	// Constructions
	CSnapshot();
	CSnapshot( DPID idCache, DPID idUser, OBJID objid, DWORD dwHdr );
	virtual	~CSnapshot();

	// Operations
	void	SetSnapshot( OBJID objid, DWORD dwHdr );
	void	Flush( void )
		{
			dpidCache	= dpidUser	= DPID_UNKNOWN;
			ar.Flush();
			cb	= 0;
		}
};
#endif	// __SNAPSHOT_H__