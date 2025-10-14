#include "StdAfx.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "DPCacheSrvr.h"
#include "Player.h"
#include "lang.h"

#include <logging/FLMyTrace.h>
extern	FLMyTrace	g_MyTrace;

extern	CDPClientArray	g_DPClientArray;
extern	CDPCoreClient	g_DPCoreClient;


CDPCacheSrvr::CDPCacheSrvr()
{
	ON_MSG( PACKETTYPE_JOIN, OnAddConnection );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
}

CDPCacheSrvr::~CDPCacheSrvr()
{

}

void CDPCacheSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	static	int	cbPlayer	= 0;

	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				g_MyTrace.Add( FLMyTrace::Key( "so" ), FALSE, "//%04d//", ++cbPlayer );
				if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
					FLERROR_LOG( PROGRAM_NAME, _T( "//%04d//" ), cbPlayer );		// "05"
				CPlayerMng::Instance()->AddPlayer( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				g_MyTrace.Add( FLMyTrace::Key( "so" ), FALSE, "//%04d//", --cbPlayer );
				if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
					FLERROR_LOG( PROGRAM_NAME, _T( "//%04d//" ), cbPlayer );		// "05"
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}


void CDPCacheSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );		// Skip dpid

	if( dwMsgSize < 8 )
		return;

	GETTYPE( ar );

	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );

//	일본 접속 오류로 인해 ReadCount Check후 많은 패킷을 날리는 유저 disconnect
	time_t current_sec;
	time(&current_sec);
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( idFrom );
	if ( !pPlayer )	return;

	if ( ( g_xFlyffConfig->GetMainLanguage() == LANG_JAP ) && dw == PACKETTYPE_DROPITEM ) 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "DropItem (Char:%s) PacketType : %x ReadCount:%d IP:%s, MsgSize:%d" ),  pPlayer->GetPlayer(), dw, pPlayer->m_nReadCount, pPlayer->GetAddr(), dwMsgSize ); 
		DestroyPlayer( idFrom );
		return;
	}

	//static int nCount = 0;
	// 
	//if (pPlayer->m_nReadCount>=30)
	//{
	//	FLERROR_LOG( PROGRAM_NAME, _T( "(Char:%s) PacketType : %x ReadCount:%d IP:%s, MsgSize:%d" ),  pPlayer->GetPlayer(), dw, pPlayer->m_nReadCount, pPlayer->GetAddr(), dwMsgSize ); 
	//}

	//if (pPlayer->m_nLastReadSec != current_sec)
	//{
	//	if (pPlayer->m_nReadCount>=30)
 //  		{
	//		FLERROR_LOG( PROGRAM_NAME, _T( "(Char:%s) PacketType : %x ReadCount:%d IP:%s, MsgSize:%d" ),  pPlayer->GetPlayer(), dw, pPlayer->m_nReadCount, pPlayer->GetAddr(), dwMsgSize ); 
	//		pPlayer->m_nPenalty++;	
	//	}
	//	if ( nCount > 2000 )
	//		FLERROR_LOG( PROGRAM_NAME, _T( "Count (Char:%s) PacketType : %x ReadCount:%d IP:%s, MsgSize:%d" ),  pPlayer->GetPlayer(), dw, pPlayer->m_nReadCount, pPlayer->GetAddr(), dwMsgSize );
	//	pPlayer->m_nLastReadSec  = current_sec; 
	//	pPlayer->m_nReadCount=0;
	//	nCount = 0;
	//}	else
	//{	pPlayer->m_nReadCount++;
	//	nCount++;
	//}
 //  	if	(pPlayer->m_nPenalty>=10)
	//{	pPlayer->m_nPenalty =0;
	////	DestroyPlayer( idFrom );
	//}
//	일본 접속 오류로 인해 ReadCount Check후 많은 패킷을 날리는 유저 disconnect

	if( pfn ) {
		( this->*( pfn ) )( ar, idFrom, (BYTE*)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID) );
	}
	else {
		switch( dw )
		{
//			case PACKETTYPE_ADDPARTYMEMBER:
			case PACKETTYPE_REMOVEPARTYMEMBER:
			case PACKETTYPE_CHANGETROUP:
			case PACKETTYPE_CHANPARTYNAME:
			case PACKETTYPE_PARTYCHANGEITEMMODE:
			case PACKETTYPE_PARTYCHANGEEXPMODE:
			case PACKETTYPE_ADDFRIEND:
			case PACKETTYPE_GETFRIENDSTATE:
			case PACKETTYPE_SETFRIENDSTATE:
			case PACKETTYPE_FRIENDINTERCEPTSTATE:
			case PACKETTYPE_REMOVEFRIEND:
			case PACKETTYPE_PARTYCHANGELEADER:
			case PACKETTYPE_DESTROY_GUILD:
			case PACKETTYPE_ADD_GUILD_MEMBER:
			case PACKETTYPE_REMOVE_GUILD_MEMBER:
			case PACKETTYPE_GUILD_MEMBER_LEVEL:
			case PACKETTYPE_GUILD_AUTHORITY:	
			case PACKETTYPE_GUILD_PENYA:
			case PACKETTYPE_GUILD_SETNAME:
			case PACKETTYPE_GUILD_CLASS:
			case PACKETTYPE_GUILD_NICKNAME:
			case PACKETTYPE_DECL_GUILD_WAR:		// 선전 포고
			case PACKETTYPE_ACPT_GUILD_WAR:		// 개전
			case PACKETTYPE_SURRENDER:	// 항복
			case PACKETTYPE_QUERY_TRUCE:	// 휴전
			case PACKETTYPE_ACPT_TRUCE:		// 휴전

#ifdef __GUILDVOTE
			case PACKETTYPE_NC_ADDVOTE:	   // 투표 입력 
			case PACKETTYPE_NC_REMOVEVOTE: // 투표 취소 
			case PACKETTYPE_NC_CLOSEVOTE:  // 투표 종료 
			case PACKETTYPE_NC_CASTVOTE:   // 투표하기 
#endif  // __GUILDVOTE
			case PACKETTYPE_CHG_MASTER:   // 길드 마스터 변경 
			case PACKETTYPE_SENDTAG:
				g_DPCoreClient.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
			default:
				g_DPClientArray.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
		}
	}
}


void CDPCacheSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
	{
		pPlayer->SetAddr( this );
		pPlayer->Join( ar );
		CRTMessenger rtmessenger;
		int nSize	= rtmessenger.Serialize( ar );
		//FLERROR_LOG( PROGRAM_NAME, _T( "CRTMessenger.Serialize: %s(%d)" ), pPlayer->GetAddr(), nSize ); //"05"
		if( nSize )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CRTMessenger.Serialize: %s(%d)" ), pPlayer->GetAddr(), nSize ); 
			DestroyPlayer( dpid );
		}
		else
			g_DPCoreClient.SendAddPlayer( pPlayer, rtmessenger );
	}
	else
		DestroyPlayer( dpid );
}

void CDPCacheSrvr::OnRemoveConnection( DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
	{
		if( pPlayer->GetClient() )	// JOIN패킷을 보낸 플레이어만 LEAVE패킷을 보낸다.
		{
			BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, DPID_UNKNOWN );
			ar << pPlayer->GetPlayerId();
			LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
			g_DPClientArray.SendToServer( dpid, lpBuf, nBufSize );
		}
		CPlayerMng::Instance()->RemovePlayer( dpid );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GetPlayer return NULL." ) );
	}
}

void CDPCacheSrvr::OnKeepAlive( CAr & /*ar*/, DPID dpid, LPBYTE, u_long )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
		pPlayer->SetAlive( TRUE );
}

CDPCacheSrvr	g_DPCacheSrvr;