#ifndef __DPCORECLIENT_H__
#define __DPCORECLIENT_H__

#pragma once

#include <network/FLDPMng.h>


class CDPCoreClient : public CDPMng
{

public:
	CDPCoreClient();
	virtual	~CDPCoreClient();

	// Operations
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendToServer( LPVOID lpMsg, DWORD dwMsgSize )
		{	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );	}

	void	SendPreJoin( DWORD dwAuthKey, const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer );
	void	SendLeave( u_long idPlayer );
	void	QueryTickCount( void );
	void	OnQueryTickCount( CAr & ar );

	USES_PFNENTRIES( CDPCoreClient, ( CAr & ar ) );
	// Handlers
	void	OnPreJoin( CAr & ar );	// result
	void	OnQueryRemovePlayer( CAr & ar );
};

#endif	// __DPCORECLIENT_H__