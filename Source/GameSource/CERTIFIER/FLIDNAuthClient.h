
#ifndef	__FLIDNAUTHCLIENT_H__
#define	__FLIDNAUTHCLIENT_H__


#include <network/FLDPMng.h>
#include "../_Network/msghdr.h"

#include "FLIDNAuthDefine.h"
#include "FLDBOverlappedPlus.h"


class	FLIDNAuthClient : public CDPMng
{
public:

	enum
	{
		DISCONNECTED,	// 접속 종료 상태
		CONNECTING,		// 접속 중
		CONNECTED,		// 접속 완료
	};

	explicit FLIDNAuthClient( BYTE byWorldSN );
	~FLIDNAuthClient();

	void	Process();

	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	USES_PFNENTRIES( FLIDNAuthClient, ( CAr & ar, DPID idFrom ) );

	void	OnInitializeACK( CAr & ar, DPID idFrom );
	void	OnLoginACK( CAr & ar, DPID idFrom );
	void	OnAccountTerminateNTY( CAr & ar, DPID idFrom );

public:

	void	SendInitiailzeREQ( BYTE byGameSN, BYTE byWorldSN, BYTE byChannelSN );
	void	SendLoginREQ( const char* szAccount, const char* szPassword, DWORD dwIP );
	void	SendLogoutNTY( const char* szAccount );
	void	SendAliveNTY();

private:

	void	SendIDNAuthPacket( FLSerializer* pMsg );

private:

	BYTE			m_byWorldSN;
	BYTE			m_byChannelSN;

	int				m_nConnectionState;
	DWORD			m_dwAliveTick;

};



#endif