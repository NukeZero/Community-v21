#ifndef __DPLOGINSRVR_H__
#define __DPLOGINSRVR_H__

#pragma once

#include <network/FLDPMng.h>

class CDPLoginSrvr : public CDPMng
{
public:
//	Constructions
	CDPLoginSrvr();
	virtual	~CDPLoginSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations

	void	SendQueryRemovePlayer( const CHAR* lpszAccount );
//	Handlers
	USES_PFNENTRIES( CDPLoginSrvr, ( CAr & ar ) );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	OnQueryTickCount( CAr & ar );
	void	OnPreJoin( CAr & ar );
	void	OnLeave( CAr & ar );

private:

	DPID	m_dpid;
};

#endif	// __DPLOGINSRVR_H__