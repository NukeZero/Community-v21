#ifndef __DPCACHESRVR_H__
#define __DPCACHESRVR_H__

#include <network/FLDPMng.h>
#include <MsgHdr.h>


class CDPCacheSrvr : public CDPMng
{
private:

public:
	// Constructions
	CDPCacheSrvr();
	virtual	~CDPCacheSrvr();
	
	// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	USES_PFNENTRIES( CDPCacheSrvr, ( CAr & ar, DPID, LPBYTE, u_long ) );

	// Handlers
	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveConnection( DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE, u_long );
};


#endif