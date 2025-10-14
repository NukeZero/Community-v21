#include "stdafx.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"
#include <lang.h>

#include "user.h"
#include "DBManager.h"
#include "../_Common/FLServerDescription.h"

extern	CDPCertifier	g_dpCertifier;
extern	CDbManager		g_DbManager;

CDPAccountClient::CDPAccountClient()
{
	ON_MSG( PACKETTYPE_ADD_ACCOUNT, OnAddAccount );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, OnDestroyPlayer );
	ON_MSG( PACKETTYPE_SERVER_LIST, OnServerList );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, OnPlayerCount );
	ON_MSG( PACKETTYPE_ENABLE_SERVER, OnServerConnectionMode );
	ON_MSG( PACKETTYPE_LOGOUT_ACCOUNT, OnLogoutAccount );
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			break;
	}
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpid2	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );


	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
//	FLASSERT( pfn );
	if( pfn )
		( this->*( pfn ) )( ar, dpid2 );
}

void CDPAccountClient::SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE cbAccountFlag, DPID idFrom, int /*fCheck*/, const char* szCheck, const char* szBak, DWORD dwPCBangClass, const CTime tmBirthDate )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, idFrom );
	ar.WriteString( lpAddr );
	ar.WriteString( lpszAccount );
	ar << cbAccountFlag;

	ar.WriteString( szCheck );
	ar.WriteString( szBak );

	ar << dwPCBangClass;
	
//	if( _GetContentState( CT_SHUTDOWN_RULE  ) == CS_VER1 )
	{
		ar << tmBirthDate;
	}

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAccount( DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_REMOVE_ACCOUNT, idFrom );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ROUTE, idFrom );
	ar << dwIdofServer;
	ar << uIdofMulti;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendPing( DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PING, idFrom );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendCloseExistingConnection( const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPAccountClient::SendPacket( DWORD dwDPID, const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;
	ar << dwDPID;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CDPAccountClient::OnAddAccount( CAr & ar, DPID dpid )
{
	BYTE	cbResult;
	DWORD	dwAuthKey = 0;
	BYTE	cbAccountFlag = 0;
	long	lTimeSpan = 0;
	int		nPermitip;

	ar >> cbResult;
	ar >> dwAuthKey;
	ar >> cbAccountFlag;
	ar >> nPermitip;


	char szGPotatoNo[MAX_ACCOUNT]	= { 0,};
	char szCheck[255]	= { 0,};
	char szBak[MAX_ACCOUNT]	= { 0,};

	ar.ReadString( szGPotatoNo, _countof( szGPotatoNo ) );
	ar.ReadString( szCheck, _countof( szCheck ) );
	ar.ReadString( szBak, _countof( szBak ) );

	switch( cbResult )
	{
		case ACCOUNT_DUPLIACTE:					// 중복 에러
			g_dpCertifier.SendError( ERROR_DUPLICATE_ACCOUNT, dpid );
			break;
		case ACCOUNT_CHECK_OK:					// 성공
			{

					CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
					CUser* pUser	= CUserMng::GetInstance()->GetUser( dpid );
					if( !pUser )
						return;
					pUser->m_dwTick		= 0xffffffff;
					if( lstrcmp( pUser->GetAccount(), szBak ) )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), pUser->GetAccount(), szBak );
						return;
					}


				g_dpCertifier.SendCertifyAck( dpid, dwAuthKey, cbAccountFlag, lTimeSpan, szGPotatoNo, szCheck, szBak, pUser->nGPotatoNo );

				FLPacketServerDescriptionNoti kPacket;
				g_pServerDescription->GetAllServerDescription( &kPacket.vecServerDesc );

				for( ServerDescItr kCItr = kPacket.vecServerDesc.begin(); kCItr != kPacket.vecServerDesc.end();  )
				{	
					if( !nPermitip && kCItr->lCanConnection == 2)
					{
						kCItr = kPacket.vecServerDesc.erase(kCItr);
					}
					else	kCItr++;
				}
						
				g_dpCertifier.SendPacket( dpid, &kPacket );
				break;
			}
		case ACCOUNT_EXTERNAL_ADDR:				// 허용되지 않는 주소 
			g_dpCertifier.SendError( ERROR_EXTERNAL_ADDR, dpid );
			break;
		case ACCOUNT_OVERFLOW:					// 인원초과
			g_dpCertifier.SendError( ERROR_OVERFLOW, dpid );
			break;
		case ACCOUNT_BILLING_INFO_FAILED:		// 빌링 정보 없음
			g_dpCertifier.SendError( ERROR_BILLING_INFO_FAILED, dpid );
			break;
		case ACCOUNT_BILLING_DATABASE_ERROR:	// 빌링 DB 에러 
			g_dpCertifier.SendError( ERROR_BILLING_DATABASE_ERROR, dpid );
			break;
		case ACCOUNT_BILLING_TIME_OVER:			// 빌링 사용시간 만료 
			g_dpCertifier.SendError( ERROR_BILLING_TIME_OVER, dpid );
			break;
		case ACCOUNT_BILLING_OTHER_ERROR:		// 빌링 기타 다른 에러 
			g_dpCertifier.SendError( ERROR_BILLING_OTHER_ERROR, dpid );
			break;
		case ACCOUNT_BILLING_DISCONNECTED:
			g_dpCertifier.SendError( ERROR_BILLING_DISCONNECTED, dpid );
			break;
		case ACCOUNT_IPCUT_ADDR:
			g_dpCertifier.SendError( ERROR_IP_CUT, dpid );
			break;
		default:
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "result:%d " ), cbResult );
				CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
				CUser* pUser	= CUserMng::GetInstance()->GetUser( dpid );
				if( pUser )
					FLERROR_LOG( PROGRAM_NAME, _T( "account: %s" ), pUser->GetAccount() );
				break;
			}
	}

	// AccountServer에서 인증이 실패하면 빌링에도 로그아웃을 알린다.
	if( cbResult != ACCOUNT_CHECK_OK )
	{
		CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
		CUser* pUser	= CUserMng::GetInstance()->GetUser( dpid );
		if( pUser )
		{
			LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
			memset( &pData->AuthInfo, 0, sizeof( pData->AuthInfo ) );
			FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), pUser->GetAccount() );
			pData->nQueryMode		= ACCOUNT_LOGOUT;

			g_DbManager.PostQ( pData );
		}
	}
}

void CDPAccountClient::OnDestroyPlayer( CAr & /*ar*/, DPID dpid )
{
	g_dpCertifier.DestroyPlayer( dpid );
}

void CDPAccountClient::OnServerList( CAr & ar, DPID /*dpid*/ )
{
	FLPacketServerDescriptionNoti kPacket;
	kPacket.Deserialize( ar );

	g_pServerDescription->Clear();

	for( ServerDescCItr kCItr = kPacket.vecServerDesc.begin(); kCItr != kPacket.vecServerDesc.end(); ++kCItr )
	{
		g_pServerDescription->Add( &( *kCItr ) );
	}
}

void CDPAccountClient::OnPlayerCount( CAr & ar, DPID /*dpid*/ )
{
	DWORD dwParentID, dwMyID;
	long lConnectionCount;
	ar >> dwParentID >> dwMyID >> lConnectionCount;

	g_pServerDescription->SetConnectionCount( dwParentID, dwMyID, lConnectionCount );
}

void CDPAccountClient::OnServerConnectionMode( CAr & ar, DPID /*dpid*/ )
{
	DWORD dwParentID, dwMyID;
	long lCanConnection;
	ar >> dwParentID >> dwMyID >> lCanConnection;

	g_pServerDescription->SetConnectionMode( dwParentID, dwMyID, lCanConnection );
}

void	CDPAccountClient::OnLogoutAccount( CAr & ar, DPID /*dpid*/ )
{
	char szAccount[MAX_ACCOUNT] = { 0, };
	ar.ReadString( szAccount, _countof( szAccount ) );

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	memset( &pData->AuthInfo, 0, sizeof( pData->AuthInfo ) );
	FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), szAccount );
	pData->nQueryMode		= ACCOUNT_LOGOUT;

	g_DbManager.PostQ( pData );
}

CDPAccountClient	g_dpAccountClient;