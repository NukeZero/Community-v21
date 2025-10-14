
#include "stdafx.h"
#include "FLIDNAuthClient.h"
#include "dpcertifier.h"
#include "DBManager.h"

#include "FLIDNAuthProtocol.h"


char	IDN_AUTH_SERVER_IP[64]	= "";
WORD	IDN_AUTH_SERVER_PORT	= 0;
BYTE	IDN_AUTH_CHANNEL_NUMBER	= 1;


extern	CDPCertifier		g_dpCertifier;
extern	CDbManager			g_DbManager;

FLIDNAuthClient::FLIDNAuthClient( BYTE byWorldSN )
{
	ON_MSG( IDN_AUTH_PACKET_INITIALIZE_ACK,			OnInitializeACK );
	ON_MSG( IDN_AUTH_PACKET_LOGIN_ACK,				OnLoginACK );
	ON_MSG( IDN_AUTH_PACKET_ACCOUNT_TERMINATE_NTY,	OnAccountTerminateNTY );

	m_byWorldSN		= byWorldSN;
	m_byChannelSN	= IDN_AUTH_CHANNEL_NUMBER;
	m_nConnectionState = DISCONNECTED;
	m_dwAliveTick = 0;
}

FLIDNAuthClient::~FLIDNAuthClient()
{
	m_nConnectionState = DISCONNECTED;
}

void	FLIDNAuthClient::Process()
{
	if( m_nConnectionState == DISCONNECTED )
	{
		// connect
		if( ConnectToServer( IDN_AUTH_SERVER_IP, IDN_AUTH_SERVER_PORT, FLIDNAuthProtocol::GetInstance(), TRUE ) == TRUE )
		{
			m_nConnectionState = CONNECTING;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "IDNAuth Server Connect Failed IP: %s, Port: %d" ), IDN_AUTH_SERVER_IP, IDN_AUTH_SERVER_PORT );
		}
	}
	else if( m_nConnectionState == CONNECTED )
	{
		DWORD dwNowTick = ::GetTickCount();
		if( (dwNowTick-m_dwAliveTick) >= SEC( T_IDN_AUTH_PACKET_ALIVE_NTY::SEND_INTERVAL_SEC ) )
		{
			SendAliveNTY();
			m_dwAliveTick = dwNowTick;
		}
	}

	ReceiveMessage();
}

void	FLIDNAuthClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			SendInitiailzeREQ( IDN_AUTH_GAME_SN, m_byWorldSN, m_byChannelSN );
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			UNREFERENCED_PARAMETER( lpDestroyPlayer );

			m_nConnectionState = DISCONNECTED;
			FLERROR_LOG( PROGRAM_NAME, _T( "IDNAuth Disconnect WorldSN: %d" ), m_byWorldSN );
			break;
		}
	}
}

void	FLIDNAuthClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	DWORD dwType = T_IDN_AUTH_MESSAGE_HEADER::GetType( lpMsg, dwMsgSize );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dwType );
	//	FLASSERT( pfn );
	if( pfn )
	{
		CAr ar( lpMsg, dwMsgSize );
		( this->*( pfn ) )( ar, idFrom );
	}
}

void	FLIDNAuthClient::OnInitializeACK( CAr & ar, DPID /*idFrom*/ )
{
	T_IDN_AUTH_PACKET_INITIALIZE_ACK cMsg;
	if( cMsg.Deserialize( ar ) == false )
	{
		DeleteDPObject();
		FLERROR_LOG( PROGRAM_NAME, _T( "Packet Deserialize error WorldSN: %d" ), m_byWorldSN );
		return;
	}

	if( cMsg.byResult == IDN_AUTH_ERROR_OK )
	{
		// 인증서버 접속 완료
		m_nConnectionState = CONNECTED;
		m_dwAliveTick = ::GetTickCount();

		FLINFO_LOG( PROGRAM_NAME, _T( "IDNAuth Initialize OK WorldSN: %d" ), m_byWorldSN );
		return;
	}
	else
	{
		switch( cMsg.byResult )
		{
		case IDN_AUTH_ERROR_WRONG_PACKET_SIZE:	// 패킷 사이즈 오류
			FLERROR_LOG( PROGRAM_NAME, _T( "packet size failed. WorldSN: %d" ), m_byWorldSN );
			break;
		case IDN_AUTH_ERROR_DUPLICATED_INITIALIZE:	// INITIALIZE 패킷(서버 중복 )
			FLERROR_LOG( PROGRAM_NAME, _T( "duplicated initialize. WorldSN: %d" ), m_byWorldSN );
			break;
		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "unknown. Result: %d, WorldSN: %d" ), cMsg.byResult, m_byWorldSN );
			break;
		}

		DeleteDPObject();
	}
}

void	FLIDNAuthClient::OnLoginACK( CAr & ar, DPID /*idFrom*/ )
{
	T_IDN_AUTH_PACKET_LOGIN_ACK cMsg;
	if( cMsg.Deserialize( ar ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Packet Deserialize error" ) );
		return;
	}

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	memset( pData, 0, sizeof( *pData ) );

	pData->nQueryMode = CERTIFY_ACK;
	FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), cMsg.szAccount );
	g_DbManager.PostQ( pData, cMsg.byResult );

	FLINFO_LOG( PROGRAM_NAME, _T( "LoginAck Account: %s, Result: %d, WorldSN: %d" ), cMsg.szAccount, cMsg.byResult, m_byWorldSN );
}

void	FLIDNAuthClient::OnAccountTerminateNTY( CAr & ar, DPID /*idFrom*/ )
{
	T_IDN_AUTH_PACKET_TERMINATE_NTY cMsg;
	if( cMsg.Deserialize( ar ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Packet Deserialize error WorldSN: %d" ), m_byWorldSN );
		return;
	}

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), cMsg.szAccount );
	FLStrLowercase( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ) );
	pData->AuthInfo.dwIP	= 0;		// don't use
	pData->nQueryMode		= CLOSE_EXISTING_CONNECTION;
	g_DbManager.PostQ( pData );

	FLINFO_LOG( PROGRAM_NAME, _T( "Account Terminate Account: %s, WorldSN: %d" ), cMsg.szAccount, m_byWorldSN );
}

void	FLIDNAuthClient::SendInitiailzeREQ( BYTE byGameSN, BYTE byWorldSN, BYTE byChannelSN )
{
	T_IDN_AUTH_PACKET_INITIALIZE_REQ cMsg;
	cMsg.byGameSN = byGameSN;
	cMsg.byWorldSN = byWorldSN;
	cMsg.byChannelSN = byChannelSN;

	SendIDNAuthPacket( &cMsg );
}

void	FLIDNAuthClient::SendLoginREQ( const char* szAccount, const char* szPassword, DWORD dwIP )
{
	T_IDN_AUTH_PACKET_LOGIN_REQ cMsg;
	FLStrcpy( cMsg.szAccount, _countof( cMsg.szAccount ), szAccount );
	FLStrcpy( cMsg.szPassword, _countof( cMsg.szPassword ), szPassword );
	cMsg.dwIPAddress = dwIP;

	SendIDNAuthPacket( &cMsg );
}

void	FLIDNAuthClient::SendLogoutNTY( const char* szAccount )
{
	T_IDN_AUTH_PACKET_LOGOUT_NTY cMsg;
	FLStrcpy( cMsg.szAccount, _countof( cMsg.szAccount ), szAccount );

	SendIDNAuthPacket( &cMsg );
}

void	FLIDNAuthClient::SendAliveNTY()
{
	T_IDN_AUTH_PACKET_ALIVE_NTY cMsg;

	SendIDNAuthPacket( &cMsg );
}

void	FLIDNAuthClient::SendIDNAuthPacket( FLSerializer* pMsg )
{
	CAr ar;
	if( pMsg->Serialize( ar ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Serialize Error WorldSN: %d" ), m_byWorldSN );
		return;
	}

	u_long nSize = 0;
	BYTE* pBuff = ar.GetBuffer( &nSize );
	if( Send( pBuff, nSize, DPID_SERVERPLAYER ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "SEND Error WorldSN: %d" ), m_byWorldSN );
		return;
	}
}
